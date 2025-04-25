#ifndef	__H_LASER
#define	__H_LASER

typedef struct TAG_LASERINFO
{
	struct TAG_ELECTRICITYPATHINFO	*pathNodes;
	TBTexture			*texture;
	float				size;
	char				state;
	char				pad[3];
} LASERINFO, _LASERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateLaser
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLaser(char state, char *objectName, char *name, char *texture, float size, CPMNode *node, CPMNode *rootNode);

/* --------------------------------------------------------------------------------
   Function : UpdateLaser
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawLaser(ACTORINSTANCELIST *actorInstanceList);

typedef struct TAG_LASERBEAM
{
	TBVector	start;
	TBVector	end;

	float		life;
	int			pad[3];
} LASERBEAM,_LASERBEAM BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateLaserBeam
   Purpose : Create a laser beam from start to end
   Parameters : start pos, end pos, life in seconds
   Returns : LASERBEAM*
   Info : 
*/
LASERBEAM *CreateLaserBeam(TBVector start,TBVector end,float life);

/* --------------------------------------------------------------------------------
   Function : UpdateLaserBeams
   Purpose : Updates and deletes laser beams as necessary
   Parameters : 
   Returns : 
   Info : 
*/
void UpdateLaserBeams(void);

/* --------------------------------------------------------------------------------
   Function : DrawLaserBeams
   Purpose : Draws laser beams
   Parameters : 
   Returns : 
   Info : 
*/
void DrawLaserBeams(void);

/* --------------------------------------------------------------------------------
   Function : DeleteLaserBeams
   Purpose : Delete laser beams
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteLaserBeams(void);


#endif