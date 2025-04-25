#ifndef __H_STEPPINGSTONE
#define __H_STEPPINGSTONE

enum
{
	STEPPINGSTONE_STATEREST,
	STEPPINGSTONE_STATESINK,
	STEPPINGSTONE_STATEBOBBLE,
	STEPPINGSTONE_STATERISE,
	STEPPINGSTONE_STATENEGSINK,
	STEPPINGSTONE_STATENEGRISE,
	STEPPINGSTONE_STATEHOLD,
};

enum EStepStoneColl
{
	STONECOLL_FALSE,
	STONECOLL_TRUE,
	STONECOLL_BOBBLE,
	STONECOLL_NOMOVETAZ,
};

typedef struct TAG_STEPPINGSTONEOBJECTINFO
{
	TBVector		initialPosition;
	TBVector		currentPosition;

	ACTORINSTANCE	*collider;
	int				type;
	float			heightOfSink;
	EStepStoneColl	collision;

	float			elapsedTime;			//time since start of bobble, if -1 start sinking
	float			collisionTime;
	float			idleBobbleTime;
	uint32			state;

	float			idleBobbleInc;
	float			bobbleTime;
	float			sinkSpeed;
	float			bobbleHeight;
} STEPPINGSTONEOBJECTINFO BALIGN16;


/* --------------------------------------------------------------------------------
   Function : CreateSteppingStoneObject
   Purpose : Initialises actorInstances that act as stepping stones
   Parameters : ACTORINSTANCE *, height of sink
   Returns : 
   Info : 
*/

void CreateSteppingStoneObject(ACTORINSTANCE *steppingStone, float heightOfSink, TBVector initPosition, float bobbleTime, float sinkSpeed, float bobbleHeight);

/* --------------------------------------------------------------------------------
   Function : CreateSteppingStoneActorInstanceInfo
   Purpose : Creates and initialises springy info part of object
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

int CreateSteppingStoneActorInstanceInfo( ACTORINSTANCE *actorInstance );

/* --------------------------------------------------------------------------------
   Function : RespondToSteppingStoneCollision
   Purpose : collision callback
   Parameters : ACTORINSTANCE * x2
   Returns : 
   Info : 
*/

int RespondToSteppingStoneCollision( ACTORINSTANCE *collider, ACTORINSTANCE *stone );

/* --------------------------------------------------------------------------------
   Function : UpdateSteppingStone
   Purpose : updates position and velocity of stepping stones in instance list
   Parameters : ACTORINSTANCELIST *
   Returns : 
   Info : 
*/

void UpdateSteppingStone(ACTORINSTANCE *ptr);

/* --------------------------------------------------------------------------------
   Function : SteppingStonePart
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SteppingStonePart( ACTORINSTANCE *stone );

/* --------------------------------------------------------------------------------
   Function : NegativeSteppingStonePart
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void NegativeSteppingStonePart(ACTORINSTANCE *stone);

/* --------------------------------------------------------------------------------
   Function : ApplyBobble
   Purpose : calculates bobble
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void ApplyBobble( ACTORINSTANCE *stone, float maxBobble, float attenuation, float elapsedTime );

/* --------------------------------------------------------------------------------
   Function : SinkSteppingStone
   Purpose : calculates sinking of stepping stone
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void SinkSteppingStone( ACTORINSTANCE *stone );

/* --------------------------------------------------------------------------------
   Function : RaiseTheSteppingStone
   Purpose : calculates raising (at a constnt rate) of stepping stone from murky depths of 'the pond'
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void RaiseTheSteppingStone( ACTORINSTANCE *stone );


#endif