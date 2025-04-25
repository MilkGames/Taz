// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : maths.cpp
//   Purpose : maths functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "physics.h"
#include "status.h"
#include "scene.h"
#include "paths.h"
#include "animation.h"
#include "maths.h"
#include "phonebox.h"
#include "playerstats.h"
#include "main.h"
#include "timer.h"
#include "sfx.h"
#include "paths.h"
#include "characters.h"
#include "destruct.h"
#include "lights.h"
#include "musicandsavagebeasts.h"
#include "shadow.h"	// PP: test
#include "water.h"
#include "prompt.h"

char *CharState[] = 
{
	"STATE_MOVE",
	"STATE_SKIDSTOP",
	"STATE_TIPTOE",
	"STATE_SLIDE",
	"STATE_GETUPFROMSLIDE",
	"STATE_GETUPFROMWATER",
	"STATE_BIGFALL",
	"STATE_SPLAT",
	"STATE_JUMP",
	"STATE_FALL",
	"STATE_IDLE",
	"STATE_BITE",
	"STATE_SPINUP",
	"STATE_SPIN",
	"STATE_SPINDOWN",
	"STATE_SPINDOWNONWATER",
	"STATE_RECOVER",
	"STATE_COLLECTPOSTCARD",
	"STATE_HOLDINGPOSTCARD",
	"STATE_DESTROYPOSTCARD",
	"STATE_KOIKFROMWATER",
	"STATE_PROJECTILE",
	"STATE_PROJECTILESLIDE",
	"STATE_SWINGING",
	"STATE_SPRUNG",
	"STATE_VEHICLE",
	"STATE_TRAPPED",
	"STATE_DEAD",
	"STATE_DONOTHING",
	"STATE_ELECTROCUTED",
	"STATE_GROUNDELECTROCUTED",
	"STATE_ONICE",
	"STATE_SPINONICE",
	"STATE_WATERSLIDE",
	"STATE_PLAYANIMATION",
	"STATE_SCARE",
	"STATE_ENTERINGPHONEBOX",
	"STATE_ONFOUNTAIN",
	"STATE_FRONTENDUSE",
	"STATE_GOTOSLEEP",
	"STATE_SLEEP",
	"STATE_DANCE",
	"STATE_DEBUGMOVE",
	"STATE_SQUASHED",
	"STATE_CATATONIC",
	"STATE_CATATONICPHYS",
	"STATE_MOVESQUASHED",
	"STATE_SHOCKED",
	"STATE_CATATONICDELAY",
	"STATE_INTRANSPORT",
	"STATE_ATLASSPINUP",
	"STATE_ATLASSPIN",
	"STATE_ATLASSPINDOWN",
	"STATE_ATLASSPHERES",
	"STATE_LOOKAROUND",
	"STATE_ENTERLOOKAROUND",
	"STATE_EAT",
	"STATE_SPIT",
	"STATE_BUBBLEGUM",
	"STATE_CHILLIPEPPER",
	"STATE_FRONTEND",
	"STATE_RUNON",
	"STATE_INIT",
	"STATE_NINJAKICK",
	"STATE_BURNT",
	"STATE_SKATECHARGE",
	"STATE_SPLATTED",
	"STATE_SPLATRECOVER",
	"STATE_SNOWBOARDATTACK",
	"STATE_SURFBOARDATTACK",
	"STATE_RAPPERATTACK",
	"STATE_WEREWOLFATTACK",
	"STATE_COWBOYSHOOT",
	"STATE_TAZANYELL",
	"STATE_INDYROLL",
	"STATE_CHEESYATTACK",
	"STATE_MESMERISED",
	"STATE_ONMINECART",
	"STATE_INPORTAL",
	"STATE_BADFOOD",
	"STATE_ENTERINGXDOOR",
	"STATE_MOUSE",
	"STATE_BALL",
	"STATE_CRATE",
	"STATE_CAGED",
	"STATE_CAGEDMOVE",
	"STATE_SMASH",
	"STATE_VEHICLEWATERKOIK",
	"STATE_KOIKFROMDEATHPLANE",
	"STATE_NET",
	"STATE_PLAYANIMANDFREEZE",
	"STATE_LOSECOSTUME",
	"STATE_WAITFORTEXT",
	"STATE_ZAPPEDINTOMOUSE",
	"STATE_ZAPPEDINTOBALL",
	"STATE_ZAPPEDINTOTAZ",
};

/*	--------------------------------------------------------------------------------
	Function 	: InitActorStatus
	Purpose 	: Initialise an actor status structure
	Parameters 	: ACTOR_STATUS
	Returns 	: 
	Info 		:
*/
void InitActorStatus(ACTOR_STATUS *actorStatus)
{
	memset(actorStatus, 0, sizeof(ACTOR_STATUS));

	actorStatus->timeWithoutGroundCollision = 0.0f;
	actorStatus->timeWithoutWallCollision = 0.0f;
	actorStatus->positionBeforeJump[Y] = METERTOUNIT(1000.0f);
	actorStatus->speedBeforeJump = 0.0f;
	actorStatus->timeInRunAnimation = 0.0f;
	actorStatus->lastState = STATE_INIT;
	actorStatus->currentState = STATE_INIT;
	actorStatus->frontEndSubState = STATE_IDLE;
	actorStatus->heightAboveGround = 0.0f;
	actorStatus->startPosition[X] = 0.0f;
	actorStatus->startPosition[Y] = 0.0f;
	actorStatus->startPosition[Z] = 0.0f;
	actorStatus->ageOfHeightTest = 0.0f;
	actorStatus->pathInfoCollidingWith = NULL;
	actorStatus->timeSinceCollidingWithPath = 0.0f;
	actorStatus->stateRequest = (ECharState)-1;
	actorStatus->soundHandle[0] = -1;
	actorStatus->soundHandle[1] = -1;
	actorStatus->escalatorInfoCollWith = NULL;
	actorStatus->timeSinceCollidingWithEscalator = 0.0f;
	actorStatus->vehicleInfo = NULL;
	actorStatus->timeSinceLastTazTopCollected = 0.0f;
	actorStatus->timeSinceLastTiltingCollision = 10.0f;	// TP: some largish value
	actorStatus->zzzInfo = NULL;
	actorStatus->snoreHandle = 0;
	actorStatus->stateRequestCallback = NULL;
	actorStatus->timeWithoutVehicleCollision = 0.0f;
	actorStatus->timeInSquashedState = 0.0f;
	actorStatus->targetSpinSpeed = 0.0f;
	actorStatus->catatonicDelay = 0.0f;
	actorStatus->postCatatonicTrigger = NULL;
	actorStatus->questionMarkInfo = NULL;
	actorStatus->distanceToClosestMusicbox = -1.0f;
	actorStatus->timeWithoutIceCollision = 3.0f;		// TP: set to more that 2 seconds to avoid drawing footsteps at start of game
	actorStatus->slowDownTime = 0.0f;
	actorStatus->xDoorInUse = NULL;
	actorStatus->currentMoveState = STATE_MOVE;
	actorStatus->lastWaterType = WATERTYPE_WATER;

	actorStatus->multiplayer.lastStar = -1;
	actorStatus->multiplayer.effect = PLAYEREFFECTNORMAL;
	actorStatus->timeGoingUpWaterSlide = 0.0f;
	actorStatus->costumeChangeInfo = NULL;
}


/*  --------------------------------------------------------------------------------
	Function	: ClearStateChangeRequest
	Purpose		: Removes any state request that has been queued
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/

void ClearStateChangeRequest(ACTORINSTANCE *actorInstance)
{
	actorInstance->actorStatus->stateRequest = (ECharState)-1;
	actorInstance->actorStatus->stateRequestCallback = NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: RequestStateChange
	Purpose 	: Request a character to change state
	Parameters 	: ACTORINSTANCE, newstate
	Returns 	: 
	Info 		:
*/
int RequestStateChange(ACTORINSTANCE *actorInstance,ECharState newState)
{
	int			numNotes = 5, i;
	float		angle = 0.0f;
	TBTexture	*texture;

	switch(actorInstance->actorStatus->currentState)
	{
	case STATE_NET:
		if ((newState!=STATE_CAGED)&&(newState!=STATE_MOVE)) return FALSE;
		break;

	case STATE_ENTERINGPHONEBOX:
		if (newState!=STATE_MOVE) return FALSE;
		break;
	case STATE_VEHICLE:
		switch(newState)
		{
		case STATE_CATATONIC:
		case STATE_CATATONICPHYS:
		case STATE_JUMP:
			break;
		default:
			return FALSE;
		}
		break;
	case STATE_ELECTROCUTED:
		EnableActorCartoonRenderMode(&actorInstance->actorInstance);
		break;
	case STATE_SPIN:
		if((newState == STATE_JUMP)&&(actorInstance->characterInfo->powerUpInfo.hiccup))
		{
			if(actorInstance->actorStatus)
			{
				actorInstance->actorStatus->lastState = actorInstance->actorStatus->currentState;
				actorInstance->actorStatus->currentState = newState;
				actorInstance->actorStatus->stateRequest = (ECharState)-1;
				return TRUE;
			}
		}
		break;
	case STATE_BUBBLEGUM:
		switch(newState)
		{
		case STATE_SLIDE:
		case STATE_SPLAT:
		case STATE_PROJECTILE:
		case STATE_PROJECTILESLIDE:
		case STATE_SWINGING:
		case STATE_SPRUNG:
		case STATE_ONICE:
		case STATE_WATERSLIDE:
		case STATE_ONFOUNTAIN:
		case STATE_MESMERISED:
		case STATE_ONMINECART:
		case STATE_BADFOOD:
		case STATE_ENTERINGXDOOR:
		case STATE_MOUSE:
		case STATE_BALL:
			return FALSE;
		}
		break;
	case STATE_SLIDE:
		switch(newState)
		{
		case STATE_PLAYANIMATION:
			return FALSE;
		default:
			break;
		}
		break;
	}

	switch(newState)
	{
	case STATE_BUBBLEGUM:
		// CMD: states from which you cannot go to bubble gum
		switch(actorInstance->actorStatus->currentState)
		{
			case STATE_SLIDE:
			case STATE_BIGFALL:
			case STATE_PROJECTILE:
			case STATE_PROJECTILESLIDE:
			case STATE_SWINGING:
			case STATE_SPRUNG:
			case STATE_VEHICLE:
			case STATE_ELECTROCUTED:
			case STATE_GROUNDELECTROCUTED:
			case STATE_ONICE:
			case STATE_SPINONICE:
			case STATE_WATERSLIDE:
			case STATE_ONFOUNTAIN:
			case STATE_GOTOSLEEP:
			case STATE_SLEEP:
			case STATE_SQUASHED:
			case STATE_MOVESQUASHED:
			case STATE_SHOCKED:
			case STATE_EAT:
			case STATE_BUBBLEGUM:
			case STATE_CHILLIPEPPER:
			case STATE_BURNT:
			case STATE_SKATECHARGE:
			case STATE_SPLATTED:
			case STATE_SPLATRECOVER:
			case STATE_SNOWBOARDATTACK:
			case STATE_SURFBOARDATTACK:
			case STATE_RAPPERATTACK:
			case STATE_WEREWOLFATTACK:
			case STATE_COWBOYSHOOT:
			case STATE_TAZANYELL:
			case STATE_INDYROLL:
			case STATE_CHEESYATTACK:
			case STATE_MESMERISED:
			case STATE_MOUSE:
			case STATE_BALL:
				return FALSE;
		}
		break;
	case STATE_SQUASHED:
		KillAllPowerUps(actorInstance);
		break;
	case STATE_MESMERISED:
		for(i = numNotes; i > 0;--i)
		{
			switch(bmRand()%3)
			{
			case 0:
				texture = bkLoadTexture(NULL, "crotchet.tga", 0);
				break;
			case 1:
				texture = bkLoadTexture(NULL, "quaver.tga", 0);
			case 2:
				texture = bkLoadTexture(NULL, "semiquaver.tga", 0);
				break;
			}
			baCreateExtra(staveExtraSystem, actorInstance->actorInstance.position, actorInstance->actorInstance.orientation, MUSICBOXEXTRA_HEAD, 
								texture, NULL, &angle);
			angle += RAD(360.0f)/numNotes;
		}
		break;
	case STATE_WATERSLIDE:
		gameStatus.player[actorInstance->playerNo].camera->flags &= ~CAMERA_PADCONTROL;
		break;
	}

	if (actorInstance->actorStatus)
	{
		actorInstance->actorStatus->stateRequest = newState;
		// CMD: set callback to be NULL for occasion when RequestPlus is follwed immediately by request.
		actorInstance->actorStatus->stateRequestCallback = NULL;
	}
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: RequestStateChangePlus
	Purpose 	: Request a character to change state and then call function supplied
	Parameters 	: ACTORINSTANCE, newstate, change state function
	Returns 	: 
	Info 		:
*/
int RequestStateChangePlus(ACTORINSTANCE *actorInstance,ECharState newState,StateChangeCallback stateRequestCallback)
{
//	switch(actorInstance->actorStatus->currentState)
//	{
//	case STATE_VEHICLE:
//		if(newState != STATE_JUMP) return FALSE;
//		return FALSE;
//		break;
//	}

	if (actorInstance->actorStatus) 
	{
		if(RequestStateChange(actorInstance, newState))
		{
//			actorInstance->actorStatus->stateRequest = newState;
			actorInstance->actorStatus->stateRequestCallback = stateRequestCallback;
		}
	}
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateActorStatusInstances
	Purpose 	: Updates the actor status structure
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		:
*/
void UpdateActorStatusInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->actorStatus)
			UpdateActorStatus(ptr);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: EnterState
	Purpose		: Try and get Taz to enter state
	Parameters	: taz instance, state to enter
	Returns		: true if ok, false if not
	Info		: 
*/

bool EnterState(ACTORINSTANCE *actorInstance, ECharState state)
{
	if (RequestStateChange(actorInstance, state))
	{
		if (UpdateCharacterState(&gameStatus.player[actorInstance->playerNo], true))
		{
			return true;
		}
		else
		{
			ClearStateChangeRequest(actorInstance);
		}
	}
	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateActorStatus
	Purpose 	: Updates the actor status
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void UpdateActorStatus(ACTORINSTANCE *actorInstance)
{
	TBVector		tempVector;
	PATHINFO		*pathInfo;

	if (actorInstance->actorBehaviour)
	{
		actorInstance->actorStatus->timeWithoutGroundCollisionLastFrame = actorInstance->actorStatus->timeWithoutGroundCollision;
		actorInstance->actorStatus->timeWithoutWallCollisionLastFrame = actorInstance->actorStatus->timeWithoutWallCollision;
		if (actorInstance->actorBehaviour->physics.coll) // if a collision occured last frame
		{
			if (actorInstance->actorBehaviour->physics.flags&PHYSFLAG_GROUNDCOLL)
			{
				actorInstance->actorStatus->timeWithoutGroundCollision = 0.0f;
			}
			else
			{
				if (fps)
				{
					// update time without collision
					actorInstance->actorStatus->timeWithoutGroundCollision += fTime;
				}
			}
			if (actorInstance->actorBehaviour->physics.flags&PHYSFLAG_WALLCOLL)
			{
				actorInstance->actorStatus->timeWithoutWallCollision = 0.0f;
			}
			else
			{
				if (fps)
				{
					// update time without collision
					actorInstance->actorStatus->timeWithoutWallCollision += fTime;
				}
			}
		}
		else
		{
			if (fps)
			{
				// update time without collision
				actorInstance->actorStatus->timeWithoutGroundCollision += fTime;
				actorInstance->actorStatus->timeWithoutWallCollision += fTime;
			}
		}
	}

	if (actorInstance->actorStatus)
	{
		if (actorInstance->actorStatus->slowDownTime)
		{
			if ((actorInstance->actorStatus->slowDownTime -= fTime) <= 0.0f)
			{
				actorInstance->actorStatus->slowDownTime = 0.0f;
				actorInstance->actorStatus->multiplayer.effect = PLAYEREFFECTNORMAL;
				
				if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES)))
					baSetActorRenderMode(&gameStatus.player1->actorInstance, 1, BACTRENDERMODE_OUTLINED, &lineParms);
			}
		}
		// TP: Update time since last taztop collected
		actorInstance->actorStatus->timeSinceLastTazTopCollected += fTime;
		//update time since last ice collision
		actorInstance->actorStatus->timeWithoutIceCollision += fTime;
		//update time since last water slide collision
		actorInstance->actorStatus->timeWithoutWaterSlideCollision += fTime;
		//update time since last vehicle collision
		actorInstance->actorStatus->timeWithoutVehicleCollision += fTime;
		actorInstance->actorStatus->timeSinceLastTiltingCollision += fTime;
		// update the age of the last height test
		if (actorInstance->actorStatus->ageOfHeightTest < (60.0f)) // dont go past 60 seconds
		{
			actorInstance->actorStatus->ageOfHeightTest += fTime;
		}
		if (fps)
		{
			if (actorInstance->actorStatus->pathInfoCollidingWith)
			{
				pathInfo = actorInstance->actorStatus->pathInfoCollidingWith->pathInfo;

				if (actorInstance->actorStatus->timeSinceCollidingWithPath < 0.2f)
				{
					// TP: look for spin platform
					if (pathInfo->type==PATHTYPE_SPIN)
					{
						if (actorInstance->actorStatus->currentState == STATE_SPIN)
						{
							ChangePathState(pathInfo,PATHSTATE_CLOCKWISE);
						}
						else
						{
							ChangePathState(pathInfo,PATHSTATE_ANTICLOCKWISE);
						}
					}

					// TP: Test for squish
					if (pathInfo->type == PATHTYPE_SQUASH)
					{
						TBVector tempVector2;

						bmVectorSub(tempVector,actorInstance->actorInstance.position,pathInfo->oldPosition);
						bmVectorSub(tempVector2,actorInstance->actorInstance.position,pathInfo->position);

						// TP: Test to see if the platform is moving towards Taz
						if ((bmVectorSquaredLen(tempVector) > bmVectorSquaredLen(tempVector2))&&(actorInstance->actorStatus->timeSinceCollidingWithPath==0.0f))
						{
							StopCollidingWith(gameStatus.player1,actorInstance->actorStatus->pathInfoCollidingWith);
							if (EnterState(gameStatus.player[0].actorInstance, STATE_MOVESQUASHED))
							{
								PlaySample("tazsquished.wav", 255, 0, 0, gameStatus.player1->actorInstance.position);
								PlayAnimationByName(gameStatus.player[0].actorInstance, "recover21a",1.0f, 1, 0, 0.1f, REACT);
								if (gameStatus.player[0].actorInstance)
								{
									gameStatus.player[0].actorInstance->flags |= ACTORFLAG_ALLOWUNSQUASHMOVE;
								}
							}
						}
					}
					else
					{
						// update objects position if on platform
						UpdateActorOnPlatform(actorInstance);
					}
				}
				else
				{
					if (pathInfo->type==PATHTYPE_SPIN)
					{
						ChangePathState(pathInfo,PATHSTATE_ANTICLOCKWISE);
					}
					pathInfo = NULL;
					actorInstance->actorStatus->pathInfoCollidingWith = NULL;
				}
				actorInstance->actorStatus->timeSinceCollidingWithPath += fTime;
			}
		}
	}
	if (actorInstance->actorAnimation)
	{
		if (CheckIfPlayingAnimation(actorInstance,"run"))
		{
			// update time doing run animation
			actorInstance->actorStatus->timeInRunAnimation += fTime;
		}
		else
		{
			actorInstance->actorStatus->timeInRunAnimation = 0.0f;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: GetHeightOfActor
	Purpose 	: calculates or just returns the current Height of point
	Parameters 	: ACTORINSTANCE, maxAge(how forgiving in seconds over the height information,0 to force new check )
	Returns 	: 
	Info 		: 
*/
float GetHeightOfActor(ACTORINSTANCE *actorInstance, float maxAge)
{
	TBVector tempPos,heightOfPoint;
	BOOL	value;

	if (actorInstance->actorStatus)
	{
		if (maxAge > actorInstance->actorStatus->ageOfHeightTest)
		{
			return actorInstance->actorStatus->heightAboveGround;
		}
		else
		{
			// only do height test if no collision or collision with wall only
			if ((!actorInstance->actorBehaviour->physics.coll)||(!(actorInstance->actorBehaviour->physics.flags&PHYSFLAG_GROUNDCOLL)))
			{
				bmVectorAdd(tempPos,actorInstance->actorInstance.position,actorInstance->actorBehaviour->collSphere.offset);
				value = ProcessHeightOfPoint(&collisionCache,tempPos,&heightOfPoint[Y]);

				if (value)
				{
					// contains 3D coordinate on ground below point
					bmVectorCopy(actorInstance->actorStatus->groundBelow,heightOfPoint);
					// calculate height
					heightOfPoint[Y] = actorInstance->actorInstance.position[Y] - heightOfPoint[Y];
					heightOfPoint[Y] += actorInstance->actorBehaviour->collSphere.offset[Y];
					heightOfPoint[Y] -= actorInstance->actorBehaviour->collSphere.radius;
					if (heightOfPoint[Y] < 0.0f) heightOfPoint[Y] = 0.0f;
					actorInstance->actorStatus->heightAboveGround = heightOfPoint[Y];

					actorInstance->actorStatus->ageOfHeightTest = 0.0f;
					return heightOfPoint[Y];
				}
			}
		}
	}
	return 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: GetHeightOfPoint
	Purpose 	: calculates or just returns the current Height of point
	Parameters 	: TBVector, flags (PHOP_...)
	Returns 	: 
	Info 		: 
*/
float GetHeightOfPoint(TBVector point,int flags, ESpecialType specialType)
{
	TBVector tempPos,heightOfPoint;
	BOOL	value;

	bmVectorCopy(tempPos,point);
	value = ProcessHeightOfPoint(&collisionCache,tempPos,&heightOfPoint[Y],flags, specialType);

	if (value)
	{
		// calculate height
		heightOfPoint[Y] = tempPos[Y] - heightOfPoint[Y];
		if (heightOfPoint[Y] < 0.0f) heightOfPoint[Y] = 0.0f;

		return heightOfPoint[Y];
	}
	return 0;
}


/* --------------------------------------------------------------------------------
   Function		: QuickHeightOfPoint
   Purpose		: get the height the ground beneath a point, using a SHADOW struct for the geometry
   Parameters	: point at which to test ground height, shadow whose geometry to use,
					ptr to a float to receive the height of the ground beneath the point
   Returns		: true if some ground was found beneath the point, else false
   Info			: // PP: 
*/
bool QuickHeightOfPoint(const TBVector point, SHADOW* const shadow, float* const groundHeight)
{
	// PP: length of ray used for hop test
#define QUICKHOP_RAY_LENGTH		2500.0f

	VEC							endPoint;
	VEC							intPoint;
	bool						hit=false;				// PP: did we hit any of the triangles?
	float						minTime;				// PP: the min time (distance) to a collision
	TBCollisionRay				ray;					// PP: collision ray between start & end points
	TBCollisionTriangle			tri;					// PP: collision triangle
	TBCollisionInfo				info;					// PP: collision info
	COLLPOLY					*poly;

	endPoint.set(point[X], point[Y]-QUICKHOP_RAY_LENGTH, point[Z]);

	// PP: SET UP COLLISION RAY
	bmVectorCopy(ray.start, point);
	ray.start[W]=1.0f;
	bmVectorSub(ray.unitVector, endPoint, ray.start);// PP: doesn't need to be unit
	ray.unitVector[W]=1.0f;
	ray.length=0.0f;// PP: ignored

	info.flags=(BMCOLLINFO_RETURNPOSITION|BMCOLLINFO_RETURNTIME);

	for(int v=(shadow->numPolys*3)-1; v>=0;)
	{
		// PP: SET UP COLLISION TRIANGLE

		// PP: REMEMBER: you don't wanna be decrementing v inside the BDVERTEX_GETPOS macro!!!!
		BDVERTEX_GETPOS(&shadow->vertices[v], tri.verts[0]);
		v--;
		BDVERTEX_GETPOS(&shadow->vertices[v], tri.verts[1]);
		v--;
		BDVERTEX_GETPOS(&shadow->vertices[v], tri.verts[2]);
		v--;

		// PP: PERFORM COLLISION TEST
		if(bmCollideRayTriangle(&ray, &tri, &info))
		{
			if(!hit)
			{
				// PP: first time we've hit something - this is therefore the closest hit so far
				minTime=info.time;
				hit=true;

				// PP: fill in intersect point
				intPoint=info.position;
			}
			else
			{
				if(info.time < minTime)
				{
					// PP: this is the closest hit so far
					minTime=info.time;

					// PP: fill in intersect point
					intPoint=info.position;
				}
			}
		}
	}

	(*groundHeight) = intPoint[Y];

	return hit;
}


/*	--------------------------------------------------------------------------------
	Function 	: CalculateSpeedBeforeJump
	Purpose 	: calculates to 2D speed of the actor prior to jump and stores in ACTOR_STATUS
	Parameters 	: ACTORINSTANCE
	Returns 	: 
	Info 		:
*/
void CalculateSpeedBeforeJump(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;

	bmVectorCopy(tempVector,actorInstance->actorBehaviour->physics.velocity);
	bmVectorCopy(actorInstance->actorStatus->velBeforeJump,actorInstance->actorBehaviour->physics.velocity);
	tempVector[Y] = 0.0f;
	if (actorInstance->actorStatus)
	{
		actorInstance->actorStatus->speedBeforeJump = bmVectorLen(tempVector);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StopCollidingWith
	Purpose 	: stop collision between two actors until they are parted
	Parameters 	: ACTORINSTANCE actor, ACTORINSTANCE object not to collide with
	Returns 	: 
	Info 		:
*/
void StopCollidingWith(ACTORINSTANCE *actorInstance, ACTORINSTANCE *noCollide)
{
	if (actorInstance->actorBehaviour)
	{
		actorInstance->actorBehaviour->stopCollidingWith = noCollide;
		actorInstance->actorBehaviour->stopCollidingWithTime = 0.0f;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StopCollidingWith
	Purpose 	: sets stop colliding with time
	Parameters 	: ACTORINSTANCE actor, ACTORINSTANCE object not to collide with
	Returns 	: 
	Info 		:
*/
void StopCollidingWith(ACTORINSTANCE *actorInstance, ACTORINSTANCE *noCollide, float time)
{
	if(actorInstance->actorBehaviour)
	{
		actorInstance->actorBehaviour->stopCollidingWith = noCollide;
		actorInstance->actorBehaviour->stopCollidingWithTime = time;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: SetStartPosition
	Purpose 	: sets the start position for the character
	Parameters 	: ACTORINSTANCE, TBVector position
	Returns 	: 
	Info 		:
*/
void SetStartPosition(ACTORINSTANCE *actorInstance, TBVector startPos, TBQuaternion quat)
{
	float height;

	height = GetHeightOfPoint(startPos);
	if (actorInstance->actorStatus)
	{
		bmVectorCopy(actorInstance->actorStatus->startPosition,startPos);
		//actorInstance->actorStatus->startPosition[Y] -= height;
		bmVectorCopy(actorInstance->actorInstance.position,actorInstance->actorStatus->startPosition);

		if (quat)
		{
			TBVector	tempVector,tempQuat;
			SETVECTOR(tempVector,0.0f,1.0f,0.0f,RAD(180.0f));
			bmRotationToQuat(tempQuat,tempVector);

			bmQuatMultiply(actorInstance->actorInstance.orientation,quat,tempQuat);
			//bmQuatCopy(actorInstance->actorInstance.orientation,quat);
			bmQuatCopy(actorInstance->actorStatus->startQuaternion,actorInstance->actorInstance.orientation);
		}
		else
		{
			bmQuatCopy(actorInstance->actorInstance.orientation,bIdentityQuaternion);
			bmQuatCopy(actorInstance->actorStatus->startQuaternion,bIdentityQuaternion);
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ChangeState
	Purpose		: Change Taz's state
	Parameters	: actorInstance, newState
	Returns		: 
	Info		: 
*/

void ChangeState(ACTORINSTANCE *actorInstance, ECharState newState)
{
	ACTOR_STATUS			*actorStatus = actorInstance->actorStatus;
	CHARACTERINFO			*charInfo = actorInstance->characterInfo;
	PADCONTROLSTATUS		*controller = gameStatus.player[actorInstance->playerNo].controller;

#ifdef PHIL
#define STATE_DEBUG
#endif// PP: def PHIL

#ifdef STATE_DEBUG
	bkPrintf("Statechange %s -> %s\n",CharState[actorStatus->currentState],CharState[newState]);
#endif

	if ((newState == STATE_MOVE)&&(actorStatus->currentState==STATE_KOIKFROMWATER))
	{
		bkPrintf("bugger\n");
	}

	// CMD: seems like the best place to put this
	switch(newState)
	{
	case STATE_SCARE:
		// CMD: sustain only needs to be longer than the anim, will flush this when exiting state
		charInfo->rumbleHandle = RequestRumbleEffect(controller, 255, 0.1f, 2.0f, 2.2f, 0.0f, WAVEFORM_RANDOM, 0.0f, 0.0f, 0);
		break;
	case STATE_SQUASHED:
		KillAllPowerUps(actorInstance);
		break;
	case STATE_IDLE:
		if(actorInstance->actorStatus->timeSinceLastTiltingCollision < 1.0f) return;
		break;
	case STATE_ENTERLOOKAROUND:
		if((americanEvents)&&(americanEvents->book))
		{
			americanEvents->book->close();
			americanEvents->localFlags &= ~MORRON_REQUESTMADE;
		}
		KillPromptBook();
		break;
	}

	actorStatus->lastState = actorStatus->currentState;
	actorStatus->currentState = newState;
	actorStatus->timeInCurrentState = 0.0f;
	actorStatus->firstTimeInState = true;
}

