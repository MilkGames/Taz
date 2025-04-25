#ifndef __H_MUSICANDSAVAGEBEASTS
#define __H_MUSICANDSAVAGEBEASTS

#include "collision.h"

#define MUSICBOX_RANGE		(METERTOUNIT(5.0f))
#define MUSICBOX_NOTEFREQ	(0.75f)
#define MUSICBOX_NUMSTAVES	(25)
#define MUSICBOX_STAVENUMPOINTS	(10)

#define MUSICBOX_STAVEAMPCONST	(0.25f)
#define MUSICBOX_STAVEFREQCONST	(360.0f/(float)MUSICBOX_STAVENUMPOINTS)
#define MUSICBOX_STAVELENGTH	(METERTOUNIT(5.0f))
#define MUSICBOX_STAVEMAXALPHA	(127)
#define MUSICBOX_STAVESIZECONST	(0.004f)
#define MUSICBOX_STAVETIMECONST	(10.0f)

//equation describing points of stave
// amplitude = const*t^2*cos(const*t)
//gives sinusoidal vibration with gradual increase in amplitude

enum
{
	MUSICBOXEXTRA_STAVE,			// CMD: streak forming the musical stave
	MUSICBOXEXTRA_NOTE,				// CMD: the notes dancing along the stave
	MUSICBOXEXTRA_HEAD,				// CMD: the notes dancing around Taz's head
	MUSICBOXEXTRA_FADE,				// CMD: any note that is required to fade immediately
};

typedef struct TAG_MUSICBOXINFO
{
	float	radius;
	uint32	boundaryCRC;
	float	clock;
	TBTexture	*crotchet;

	TBTexture	*quaver;
	TBTexture	*semiquaver;
	int			numPolys;
	COLLVERTEX	*verts;
	COLLPOLY	*polys;

	uint32		state;
	int32		iPad[3];
} MUSICBOXINFO, _MUSICBOXINFO BALIGN16;

typedef struct TAG_MUSICBOXEXTRAINFO
{
	TBMatrix		orientation;
	TBVector		position;

//	TBVector		points[MUSICBOX_STAVENUMPOINTS];
	TBVector		stave[MUSICBOX_STAVENUMPOINTS];
	float			size[MUSICBOX_STAVENUMPOINTS];
	int				colour[MUSICBOX_STAVENUMPOINTS][4];

	float			clock;
	TBTexture		*texture;
	float			texCoorOffset;
	float			alpha;
	
	uint32			type;
	uint32			flip;
	ACTORINSTANCE	*musicBox;								// CMD: music box actorInstance, with which the extra is associated
	TBActorNodeInstance	*node;
}MUSICBOXEXTRAINFO, _MUSICBOXEXTRAINFO BALIGN16;

extern TBExtraSystem	*staveExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateMusicBoxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateMusicBoxInstance(struct TAG_ACTORINSTANCE *actorInstance, float radius, uint32 boundaryCRC, uint32 state);

/* --------------------------------------------------------------------------------
   Function : CreateMusicBoxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMusicBoxInstances(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CheckIfVectorPositionLiesWithinInstance
   Purpose : checks to see if a point lies within an instance
   Parameters : 
   Returns : TRUE/ FALSE
   Info : 
*/

void PreCacheMusicBoxBoundaryInfo(ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : CheckIfVectorPositionLiesWithinInstance
   Purpose : checks to see if a point lies within an instance
   Parameters : 
   Returns : TRUE/ FALSE
   Info : 
*/

int CheckIfVectorPositionLiesWithinInstance(COLLPOLY *polys, COLLVERTEX *verts, uint32 numVerts, TBVector position);

/* --------------------------------------------------------------------------------
   Function : MusicBoxStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void MusicBoxStateChangeCallback(struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : InitStaveExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitStaveExtraSystem(void);

/* --------------------------------------------------------------------------------
   Function : StaveExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void StaveExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms);

/* --------------------------------------------------------------------------------
   Function : StaveExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int StaveExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata);

/* --------------------------------------------------------------------------------
   Function : StaveExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void StaveExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);

/* --------------------------------------------------------------------------------
   Function : StaveExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void StaveExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);

/* --------------------------------------------------------------------------------
   Function : StaveExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void StaveExtraSystemInitCallback(TBExtraSystem *eSystem, int reason);

#endif