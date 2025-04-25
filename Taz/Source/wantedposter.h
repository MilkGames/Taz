#ifndef __H_WANTEDPOSTER
#define __H_WANTEDPOSTER

enum
{
	POSTERSPECIALCASE_CLOCK,
};

enum
{
	POSTERANIMSTATE_IDLE,
	POSTERANIMSTATE_EVENT,
	POSTERANIMSTATE_SPENT,
	POSTERANIMSTATE_DEAD,
};

typedef struct TAG_WANTEDPOSTERINFO
{
	int32			posterNumber;
	int32			posterSpecialCase;
	struct TAG_TRIGGERINFO		*onDestruction;
	uint32			animationState;
} WANTEDPOSTERINFO, _WANTEDPOSTERINFO BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateWantedPosterInstance
   Purpose : Create a destructibleObjectInstance
   Parameters : ACTORINSTANCE, type (DESTRUCTIBLETYPE_...)
   Returns : 
   Info : 
*/
int CreateWantedPosterInstance(ACTORINSTANCE *actorInstance, int32 posterNumber, int32 posterSpecialCase);

/* --------------------------------------------------------------------------------
   Function : PreResponToDestructibleCollision
   Purpose : Function to be called when a collision with a destructible has occured
   Parameters : ACTORINSTANCE object hitting rock, ACTORINSTANCE *destructible
   Returns : 
   Info : 
*/
int PreRespondToWaterPosterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *destructible, struct TAG_PRECOLLINFO *info);

/* --------------------------------------------------------------------------------
   Function : RespondToWantedPosterCollision
   Purpose : Function to be called when a collision with a wanted poster has occured
   Parameters : ACTORINSTANCE *collider, ACTORINSTANCE *poster
   Returns : 
   Info : 
*/
int RespondToWantedPosterCollision(ACTORINSTANCE *collider, ACTORINSTANCE *poster);

/* --------------------------------------------------------------------------------
   Function : UpdateSpecialCaseWantedPoster
   Purpose : update function for wanted posters that require individual attention, hence special case
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/
int UpdateSpecialCaseWantedPoster(ACTORINSTANCE *ptr);

#endif