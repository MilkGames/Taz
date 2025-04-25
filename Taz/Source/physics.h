#ifndef __H_PHYSICS
#define __H_PHYSICS

#include "collision.h"
#include "maths.h"

//  -----------------------------------------------------
//  File: physics.h
//  -----------------------------------------------------
//  by David Serafim (03/00-05/00)
//  
//  (C) ISL 2000
//  -----------------------------------------------------


extern float	worldGravity;
extern float	worldMaxGravSpeed;

#define		NORMAL_WORLD_GRAVITY		(METERTOUNIT(-20.0f))
#define		ICE_MAX_SPEED				(METERTOUNIT(500.0f))

// force multiple collision tests for objects that move at hi-speed
#define		PHYSFLAG_FORCEMULTCOLL		(1L<<0)
// clear y velocity (if positive) to avoid going up a wall
#define		PHYSFLAG_WALLVELCORRECTION	(1L<<1)
#define		PHYSFLAG_WALLGROUNDCOLLFIX  (1L<<2)
#define		PHYSFLAG_NOGRAVITY			(1L<<3)
#define		PHYSFLAG_NOMOVE				(1L<<4)
#define		PHYSFLAG_DIRGRAVITY			(1L<<5)
// flags set by ProcessActorBehaviour
#define		PHYSFLAG_GROUNDCOLL			(1L<<16)	// we hit ground last frame
#define		PHYSFLAG_WALLCOLL			(1L<<17)	// we hit a wall last frame
#define		PHYSFLAG_CYLINDERCOLL		(1L<<18)	// we hit a cylinder last frame
#define		PHYSFLAG_HEAVY				(1<<19)
//terrain flags
#define		TERRAIN_GROUND				(1<<0)
#define		TERRAIN_ICE					(1<<1)
#define		MAX_ICE_SPEED				(METERTOUNIT(0.15f))

#define		MAX_ACTORCYLINDERS		(3)
// THIS STRUCTURES ARE FOR INTERNAL USE OF PHYSICS.CPP
// AVOID CHANGING THEM DIRECTLY
typedef struct TAG_ACTOR_PHYSICS
{
	TBVector	velocity;				// speed vector
	TBVector	velImpactFromLastColl;	// TP: affect on velocity of last collision

	float	mass;			// for object against object collision
	float	minVel;			// minimum allowed speed per axis
	float	speed;			// speed vector length
	float	maxSpeed;		// maximum allowed speed vector length
	float	speedIn;

							// all this values range from 0.0 to 1.0
	float	helium;			// 0 = none, 1 = levitate, 2 = balloon
	float	inertia;		// 0 = rolls on forever, 1 stops immediately
	float	bounce;			// 0 = no bounce, 1 = bouncy ball, 1.1 will bounce forever
	float	friction;		// 0 = slippery, 1 = sticky

	TBVector	lastAverageNormal;	// last collision polys average normal
	float	lastInvFriction;	// friction from last poly collision

	float	minFricAngle;	// cosine of the minimum angle value to apply friction. 0.7 = no friction below 45deg
	int32	flags;			// 
	char	coll;			// was there a collision last frame
	char	currentTerrain;	// current terrain that actor is standing on
} ACTOR_PHYSICS,_ACTOR_PHYSICS BALIGN16;

typedef struct TAG_ACTOR_SPHERE
{
	TBVector			offset;
	float				radius;
} ACTOR_SPHERE,	_ACTOR_SPHERE BALIGN16;

typedef struct TAG_ACTOR_CYLINDER
{
	TBVector			offset;

	float				height;
	float				radius;
	int					type;
	TBActorNodeInstance *node;
} ACTOR_CYLINDER, _ACTOR_CYLINDER BALIGN16;

typedef struct TAG_ACTOR_BEHAVIOUR
{
	ACTOR_SPHERE				collSphere;
	ACTOR_CYLINDER				collCylinder[MAX_ACTORCYLINDERS];
	TBVector					remainVel;								// remaining velocity to add to main velocity vector
	TBQuaternion 				rotAim;									// direction actor should face
	ACTOR_PHYSICS				physics;

	float						rotSpeed;								// 0.0 to 1.0 (unit)
	struct TAG_ACTORINSTANCE	*stopCollidingWith;			// temporarly stop collision with actor until I have left its bounding box
	float						stopCollidingWithClock;		// timer used in conjunction with stopCollidingWith
	float						stopCollidingWithTime;		// time within which, stopCollidingWith is active

	TBActor						*actor;
} ACTOR_BEHAVIOUR,_ACTOR_BEHAVIOUR BALIGN16;

#define		MAX_COLL_MESHES		44	// maximum number of meshes for COLL_MESHINFO
#define		MAX_COLL_POLYS		47	// maximum number of faces that COLL_MESHINFO can store per mesh
#define		MAX_COLL_CYLINDERS	10	// maximum number of cylinders for COLL_MESHINFO
#define		MAX_COLL_SPHERES	50	// maximum number of spheres for COLL_MESHINFO

#define		RETFLAG_NORESPONCE	(1<<0)

#define		COLLTYPE_CYLINDER_TOP		1
#define		COLLTYPE_CYLINDER_BOTTOM	2
#define		COLLTYPE_CYLINDER_SIDE		3

// this is the structure that ProcessActorBehaviour returns a pointer to
typedef struct TAG_COLL_MESHINFO
{
	TBVector wallNormal;		// average wall normal	

	TBVector groundNormal;	// average ground normal

	struct {
		int32				Index;		// this mesh index
		int32				polyIndexList[MAX_COLL_POLYS];	// mesh polys indexes
		int32				flags;
		struct TAG_COLLISIONLISTENTRY	*collEntry;
	} mesh[MAX_COLL_MESHES+1];

	struct {
		int32		Index;		// this cylinder index
		int32		collType;	// top, bottom or side collision
		struct TAG_ACTORINSTANCE *actorInstance;
	} cylinder[MAX_COLL_CYLINDERS+1];

	struct {
		int32		Index;
		int32		collType;
		struct TAG_ACTORINSTANCE *actorInstance;
	} sphere[MAX_COLL_SPHERES+1];

} COLL_INFO, _COLL_INFO BALIGN16;

enum EPreCollType
{
	PRECOLLTYPE_MESH,
	PRECOLLTYPE_SPHERE,
	PRECOLLTYPE_CYLINDER,
};


typedef struct TAG_PRECOLLINFO
{
	int		type;		// TP: type of collision, cylinder/sphere/mesh
	int		specific;	// TP: info specific to type of collision
} PRECOLLINFO;


// function declarations

/*	--------------------------------------------------------------------------------
	Function 	: ApplyGravity
	Purpose 	: updates position and velocity as a result of gravity
	Parameters 	: pointer to component of stepvec, pointer to component of velocity, gravity
					NOTE: remember that gravity is -ve, ie in the real world = -9.81 m/s^2
	Returns 	: int
	Info 		:
*/
int ApplyGravity(TBVector positionInc, TBVector currentVelocity, float gravity, float t=fTime);

/*	--------------------------------------------------------------------------------
	Function 	: ApplyGravityInDirection
	Purpose 	: applies gravity in a given direction
	Parameters 	: 
					NOTE: remember that gravity is -ve, ie in the real world = -9.81 m/s^2
	Returns 	: int
	Info 		:
*/
int ApplyGravityInDirection(TBVector positionInc, TBVector currentVelocity, TBVector direction, float gravity);

/*	--------------------------------------------------------------------------------
	Function 	: ApplyFriction
	Purpose 	: updates velocity as a result of friction
	Parameters 	: float *, float *
	Returns 	: int
	Info 		:
*/

/*	--------------------------------------------------------------------------------
	Function 	: SetWorldGravity
	Purpose 	: 
	Parameters 	: float
	Returns 	: 
	Info 		:
*/

void SetWorldGravity(float newGravity);

int ApplyFriction( ACTOR_PHYSICS *physics );


void applyInertia( ACTOR_PHYSICS *physics );
void checkSpeedLimit( ACTOR_PHYSICS *physics, float worldMaxSpeed );

void InitActorBehaviour( ACTOR_BEHAVIOUR *actorBehaviour, float collRadius );
void SetActorMaxSpeed( ACTOR_BEHAVIOUR *actorBehaviour, float maxSpeed );
void SetActorMinVel( ACTOR_BEHAVIOUR *actorBehaviour, float minVel );
void SetActorHelium( ACTOR_BEHAVIOUR *actorBehaviour, float helium );
void SetActorBounce( ACTOR_BEHAVIOUR *actorBehaviour, float impact );
void SetActorFriction( ACTOR_BEHAVIOUR *actorBehaviour, float friction );
void SetActorMinFricAngle( ACTOR_BEHAVIOUR *actorBehaviour, float minFricAngle );
void SetActorInertia( ACTOR_BEHAVIOUR *actorBehaviour, float inertia );
void SetActorFlags( ACTOR_BEHAVIOUR *actorBehaviour, int flags );
void ActorLookAt( ACTOR_BEHAVIOUR *actorBehaviour, ACTOR_BEHAVIOUR *destActorBehaviour,
				  float rotAngle, float rotSpeed );
void ActorRotate( struct TAG_ACTORINSTANCE *actorInstance, TBQuaternion destdir, float rotSpeed );
void SetActorVelocity( ACTOR_BEHAVIOUR *actorBehaviour, TBVector newVel );
void SetActorSpeed( ACTOR_BEHAVIOUR *actorBehaviour, TBVector velDir, float newSpeed );
void AddActorVelocity( ACTOR_BEHAVIOUR *actorBehaviour, TBVector vel );
void AddActorSpeed( ACTOR_BEHAVIOUR *actorBehaviour, TBVector velDir, float spd );
void SetActorCollSphere(ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius);


COLL_INFO *ProcessActorBehaviour( struct TAG_ACTORINSTANCE *actorInstance, struct TAG_COLLISIONLISTENTRY **meshLists, int noofMeshLists,
								 struct TAG_ACTORINSTANCE **cylInstances, COLL_CYLINDER *cylList, int noofCylLists, 
								 struct TAG_ACTORINSTANCE **sphereInstances, COLL_SPHERE *sphereList, int noofSpheres, uint32 flags, COLL_INFO *retcollinfo);

/*	--------------------------------------------------------------------------------
	Function 	: ProcessCylinderCollision
	Purpose 	: process cylinder to cylinder collision
	Parameters 	: ?
	Returns 	: number of cylinder collisions occured
	Info 		: 
*/
int ProcessCylinderCollision(TBVector pos,struct TAG_ACTOR_BEHAVIOUR *actorBehaviour, struct TAG_ACTORINSTANCE **cylInstances, COLL_CYLINDER *cylList,int noofCylLists, 
									 struct TAG_ACTORINSTANCE *actorInstance, TBVector stepvec, COLL_INFO *collinfo, COLL_INFO *retcollinfo, int *rcidx);

/*	--------------------------------------------------------------------------------
	Function 	: ProcessSphereCollision
	Purpose 	: process sphere to sphere collision
	Parameters 	: ?
	Returns 	: number of sphere collisions occured
	Info 		: 
*/
int ProcessSphereCollision(TBVector pos, struct TAG_ACTOR_BEHAVIOUR *actorBehaviour, struct TAG_ACTORINSTANCE **sphereInstances, COLL_SPHERE *sphereList,int noofSphereLists, 
									 struct TAG_ACTORINSTANCE *actorInstance, TBVector stepvec, COLL_INFO *collinfo, COLL_INFO *retcollinfo, int *rsidx);

/*	--------------------------------------------------------------------------------
	Function 	: PerformCollisionFaceResponse
	Purpose 	: 
	Parameters 	: ?
	Returns 	: invPolyFriction
	Info 		: 
*/
float PerformCollisionFaceResponse(struct TAG_ACTORINSTANCE *actorInstance,int numfaces,COLL_FACEINFO *faceinfo,TBVector pos, float invPolyFric,COLL_INFO *collinfo,
								   TBVector stepvec,float speed,TBVector corVel);

/*	--------------------------------------------------------------------------------
	Function 	: PerformSphereResponse
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PerformSphereResponse(struct TAG_ACTORINSTANCE *actorInstance,int numSpheres,COLL_SPHERE *sphereList,TBVector pos, COLL_INFO *collinfo,TBVector stepvec,float speed,TBVector corVel);

/* --------------------------------------------------------------------------------
   Function : SetActorCylinder
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void SetActorCylinder(int cylindeNumber,ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius,float height,int type = CYLINDER_FLATTOP);

/* --------------------------------------------------------------------------------
   Function : SetActorCylinder
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void SetActorCylinder(int cylinderNumber,TBActorNodeInstance *node,ACTOR_BEHAVIOUR *actorBehaviour, TBVector offset, float radius,float height,int type = CYLINDER_FLATTOP);

/*	--------------------------------------------------------------------------------
	Function 	: PerformCylinderResponse
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PerformCylinderResponse(struct TAG_ACTORINSTANCE *actorInstance,int numCyl,COLL_CYLINDER *cylList,TBVector pos, COLL_INFO *collinfo,TBVector stepvec,float speed,TBVector corVel);


#endif