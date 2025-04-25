#ifndef __H_TRANSPORTEREFFECT
#define __H_TRANSPORTEREFFECT

#include "actors.h"

#define TRANSFX_NUMSPARKLES		(40)
#define MAX_TRANSFXEXTRAS		(400)

enum EtransFXState
{
	TRANSXFSTATE_FADEDOWN,
	TRANSXFSTATE_FADEUP,
};

typedef struct TAG_TRANSFXINFO
{
	// CMD: transporter info
	TBVector		targetPosition;
	TBQuaternion	targetOrientation;

	// CMD: actor parms
	float			actorAlpha;

	// CMD: extra parms
	float			maxHeight;
	float			radius;
	float			delay;
	float			xSize;
	float			ySize;

	int				fadeUpOnCompletion;
	EtransFXState	state;
	struct TAG_TRIGGERINFO		*onCompletion;
	struct TAG_TRIGGERINFO		*onReachingDestination;

	float			angle[TRANSFX_NUMSPARKLES];
	float			angVel[TRANSFX_NUMSPARKLES];
	float			height[TRANSFX_NUMSPARKLES];
	float			clock[TRANSFX_NUMSPARKLES];
	uint32			draw[TRANSFX_NUMSPARKLES];
}TRANSFXINFO, _TRANSFXINFO BALIGN16;

typedef struct TAG_TRANSFXEXTRA
{
	TBVector	position;

	float		alpha;
	float		alphaFadeRate;
	int32		iPad[2];
}TRANSFXEXTRA, _TRANSFXEXTRA BALIGN16;

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int EnableCharacterTransporterMode(ACTORINSTANCE *actorInstance, float height, float radius, int fadeUp);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void DisableCharacterTransporterMode(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void SetCharacterTransportToPosition(ACTORINSTANCE *actorInstance, TBVector position, TBQuaternion orientation);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int UpdateCharacterInTransporter(ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int DrawCharacterInTransporter(ACTORINSTANCE *actorInstance);

#endif