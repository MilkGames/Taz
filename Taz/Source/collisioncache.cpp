// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : collisioncache.cpp
//   Purpose : functions for handling the creation and storage of collision meshes
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "collisioncache.h"
#include "status.h"
#include "maths.h"
#include "timer.h"
#include "main.h"
#include "mapfile.h"
#include "scene.h"
#include "Util.h"					// PP: Utility functions
#include "paths.h"
#include "shadow.h"					// PP: functions to create and draw shadows on the landscape

BOUNDINGBOX	worldBoundingBox;

/* --------------------------------------------------------------------------------
   Function : ProcessCollisionForActors
   Purpose : Process collision and physics for an actor list using the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCELIST
   Returns : 
   Info : 
*/
void ProcessCollisionForActors(COLLISIONCACHE *collisionCache, ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->actorBehaviour)
		{
			ProcessCollisionForActor(collisionCache,ptr,0);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ProcessCollisionForActor
   Purpose : Process collision and physics for an actor using the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCE,flags, pointer to a pointer to return collinfo if needed
   Returns : 3-cylinder coll, 2-sphere coll, 1-mesh collision, 0-no collision
   Info : 
*/
int ProcessCollisionForActor(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance, uint32 flags, COLL_INFO **retCollInfo)
{
	COLLISIONLISTENTRY		*meshLists[MAX_COLL_MESHES],*mesh;
	bool			instances[MAX_COLL_MESHES];
	ACTORINSTANCE	*cylInstances[MAX_COLL_CYLINDERS];
	ACTORINSTANCE	*sphereInstances[MAX_COLL_SPHERES];
	int				i=0,j;
	int				numberOfVerts[MAX_COLL_MESHES],numberOfMeshes;
	TBVector			testPos,tempVector;
	BOUNDINGBOX		actorBox;
	COLLISIONLISTENTRY *ptr;
	static COLL_INFO		collInfo;
	COLL_CYLINDER	collCylinders[MAX_COLL_CYLINDERS];
	COLL_SPHERE		collSpheres[MAX_COLL_SPHERES];
	int				numCylinders = 0;
	int				numSpheres = 0;
	ACTOR_SPHERE	*sphere = &actorInstance->actorBehaviour->collSphere;
	ACTOR_CYLINDER	*cylinder = &actorInstance->actorBehaviour->collCylinder[0];
	int				coll = FALSE;
	float			speed;

	if((actorInstance->flags&ACTORFLAG_NOCOLLISION)&&(actorInstance->flags&ACTORFLAG_NOVELOCITY)) return 0;
	// calculate path of actor

	float	oldfTime = fTime;

	if(actorInstance->actorStatus)
	{
		if (actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
		{
			fTime = 1.125f/fps;
			SetWorldGravity(NORMAL_WORLD_GRAVITY / 6.0f);
		}
	}

	bmVectorCopy(tempVector,actorInstance->actorBehaviour->physics.velocity);
	bmVectorCMul(tempVector,tempVector,fTime);
	speed = bmVectorLen(tempVector);

	if (actorInstance->flags&ACTORFLAG_NOCOLLISION)
	{
		// PP: I think we still want velocity applied even if we don't want collision, at least I hope you do because I do.
		// PP: "...and if you don't let me do what I want to I'll scream and scream until I'm sick," continued Violet-Elizabeth.
		// PP: Ginger and Herbert turned expectantly towards William, who they well knew held little respect for girls.
		// PP: He found them agressive and domineering, willing to enlist the aid of authority at the slightest provocation.
		// PP: "Well!"  ejaculated William, "You can jolly well *make* yourself sick for all I care!"
		// PP: "Come on, Jumble, we shall all have a jolly old time without her at the hideout, what with all that lickerish-water wot Douglas made," he teased.		// PP: MUST...SLEEP...

		bmVectorAdd(actorInstance->actorInstance.position, actorInstance->actorInstance.position, tempVector);

		return 0;
	}

	bmVectorCopy(testPos, actorInstance->actorInstance.position);

	actorBox.xmin = testPos[X] - sphere->radius + sphere->offset[X];
	actorBox.xmax = testPos[X] + sphere->radius + sphere->offset[X];
	actorBox.ymin = testPos[Y] - sphere->radius + sphere->offset[Y];
	actorBox.ymax = testPos[Y] + sphere->radius + sphere->offset[Y];
	actorBox.zmin = testPos[Z] - sphere->radius + sphere->offset[Z];
	actorBox.zmax = testPos[Z] + sphere->radius + sphere->offset[Z];

	// extend box for test
	if (tempVector[X] > 0.0f)
		actorBox.xmax += tempVector[X];
	else
		actorBox.xmin += tempVector[X];
	if (tempVector[Y] > 0.0f)
		actorBox.ymax += tempVector[Y];
	else
		actorBox.ymin += tempVector[Y];
	if (tempVector[Z] > 0.0f)
		actorBox.zmax += tempVector[Z];
	else
		actorBox.zmin += tempVector[Z];
	// add margin for error of 1/2 meter
	actorBox.xmax += METERTOUNIT(0.5f);
	actorBox.xmin -= METERTOUNIT(0.5f);
	actorBox.ymax += METERTOUNIT(0.5f);
	actorBox.ymin -= METERTOUNIT(0.5f);
	actorBox.zmax += METERTOUNIT(0.5f);
	actorBox.zmin -= METERTOUNIT(0.5f);

	meshLists[i] = NULL;
	StartProfileTimer(&tonyProfile,2,"findcoll");
	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		// test if in or going to enter bounding box
		if ((actorInstance->actorBehaviour)&&(actorInstance->actorBehaviour->stopCollidingWith)&&(actorInstance->actorBehaviour->stopCollidingWith == ptr->actorInstance))
		{
			actorInstance->actorBehaviour->stopCollidingWithClock += 1.0f/fps;
			if((actorInstance->actorBehaviour->stopCollidingWithTime)&&(actorInstance->actorBehaviour->stopCollidingWithTime < actorInstance->actorBehaviour->stopCollidingWithClock))
			{
				actorInstance->actorBehaviour->stopCollidingWith = NULL;
				actorInstance->actorBehaviour->stopCollidingWithClock = 0.0f;
				actorInstance->actorBehaviour->stopCollidingWithTime = 0.0f;
			}
			// stop colliding with
			if (!TestBoxCollision(&actorBox,&ptr->boundingBox))
			{
				actorInstance->actorBehaviour->stopCollidingWith = NULL;
				actorInstance->actorBehaviour->stopCollidingWithClock = 0.0f;
				actorInstance->actorBehaviour->stopCollidingWithTime = 0.0f;
				//check for callback
				if(ptr->actorInstance->outsideBoundingBox)
				{
					ptr->actorInstance->outsideBoundingBox(actorInstance, ptr->actorInstance);
				}
			}
			if((!actorInstance->actorBehaviour->stopCollidingWithTime)||(actorInstance->actorBehaviour->stopCollidingWithTime >= actorInstance->actorBehaviour->stopCollidingWithClock))
			{
				continue;
			}
		}
		if (TestBoxCollision(&actorBox,&ptr->boundingBox)&&(i<MAX_COLL_MESHES))
		{
			if (MakeEntryResident(collisionCache,ptr)==TRUE)
			{
				// resident so add to collision process
				instances[i] = true;
				meshLists[i++] = ptr;
			}
			else
			{
#if BPLATFORM==GAMECUBE
			static int onceOnly = 1;
			if (onceOnly)
			{
				bkPrintf("*** WARNING *** Could not make entry resident\n");
				onceOnly = 0;
			}
#else
			bkPrintf("*** WARNING *** Could not make entry resident\n");
#endif
			}
		}
	}
	EndProfileTimer(&tonyProfile,2);

	numberOfMeshes = i;

	// check for cylinders to test against
	if (actorInstance->actorBehaviour->collCylinder[0].radius != 0.0f)
	{
		for (ptr = collisionCache->cylinderList.head.next;ptr != &collisionCache->cylinderList.head;ptr = ptr->next)
		{
			if (actorInstance != ptr->actorInstance)
			{
				if ((actorInstance->actorBehaviour)&&(actorInstance->actorBehaviour->stopCollidingWith)&&(actorInstance->actorBehaviour->stopCollidingWith == ptr->actorInstance))
				{
					actorInstance->actorBehaviour->stopCollidingWithClock += 1.0f/fps;
					if((actorInstance->actorBehaviour->stopCollidingWithTime)&&(actorInstance->actorBehaviour->stopCollidingWithTime < actorInstance->actorBehaviour->stopCollidingWithClock))
					{
						actorInstance->actorBehaviour->stopCollidingWith = NULL;
						actorInstance->actorBehaviour->stopCollidingWithClock = 0.0f;
						actorInstance->actorBehaviour->stopCollidingWithTime = 0.0f;
					}
				}
				else
				{
					// check not current instance
					for (i=0;i<MAX_ACTORCYLINDERS;i++)
					{
						if (ptr->actorInstance->actorBehaviour->collCylinder[i].radius!=0.0f)
						{
							if (numCylinders<MAX_COLL_CYLINDERS)
							{
								TBVector	vec1,vec2;

								bmVectorCopy(vec1, ptr->actorInstance->actorInstance.position);
								bmVectorCopy(vec2, actorInstance->actorInstance.position);
								vec1[Y] = 0.0f;
								vec2[Y] = 0.0f;
								
								if (bmVectorDistance(vec1, vec2) < (ptr->actorInstance->actorBehaviour->collCylinder[i].radius+actorInstance->actorBehaviour->collCylinder[0].radius+speed))
								{
									if (ptr->actorInstance->actorBehaviour->collCylinder[i].node)
									{
										// calculate offset using node
										baGetNodesWorldPosition(&ptr->actorInstance->actorInstance,ptr->actorInstance->actorBehaviour->collCylinder[i].node,collCylinders[numCylinders].pos);
										bmVectorAdd(collCylinders[numCylinders].pos,collCylinders[numCylinders].pos,ptr->actorInstance->actorBehaviour->collCylinder[i].offset);
									}
									else
									{
										bmVectorAdd(collCylinders[numCylinders].pos,ptr->actorInstance->actorInstance.position,ptr->actorInstance->actorBehaviour->collCylinder[i].offset);
									}
									collCylinders[numCylinders].height = ptr->actorInstance->actorBehaviour->collCylinder[i].height;
									collCylinders[numCylinders].radius = ptr->actorInstance->actorBehaviour->collCylinder[i].radius;
									collCylinders[numCylinders].type = ptr->actorInstance->actorBehaviour->collCylinder[i].type;
									cylInstances[numCylinders++] = ptr->actorInstance;
								}
							}
						}
					}
				}
			}
		}
	}

	//check for spheres to test
	for(ptr = collisionCache->sphereList.head.next; ptr != &collisionCache->sphereList.head; ptr = ptr->next)
	{
		if (numSpheres<MAX_COLL_SPHERES)
		{
			if (ptr->actorInstance->actorBehaviour->collSphere.radius!=0.0f)
			{
				if (bmVectorDistance(ptr->actorInstance->actorInstance.position, actorInstance->actorInstance.position) < (ptr->actorInstance->actorBehaviour->collSphere.radius+actorInstance->actorBehaviour->collSphere.radius+speed))
				{
					if ((actorInstance->actorBehaviour)&&(actorInstance->actorBehaviour->stopCollidingWith)&&(actorInstance->actorBehaviour->stopCollidingWith == ptr->actorInstance))
					{
						if((actorInstance->actorBehaviour->stopCollidingWithTime == 0.0f)||(actorInstance->actorBehaviour->stopCollidingWithTime >= actorInstance->actorBehaviour->stopCollidingWithClock))
						{
							continue;
						}
					}

					if(actorInstance != ptr->actorInstance)
					{
						bmVectorAdd(collSpheres[numSpheres].pos, ptr->actorInstance->actorInstance.position, ptr->actorInstance->actorBehaviour->collSphere.offset);
						collSpheres[numSpheres].radius = ptr->actorInstance->actorBehaviour->collSphere.radius;
						collSpheres[numSpheres].flags = ptr->actorInstance->actorBehaviour->physics.flags;
						sphereInstances[numSpheres++] = ptr->actorInstance;
					}
				}
			}
		}
	}

	if (ProcessActorBehaviour(actorInstance, meshLists, numberOfMeshes, cylInstances, collCylinders,numCylinders, sphereInstances, collSpheres, numSpheres, flags, &collInfo))
	{
		// process cylinders first
		for (i=0;i<MAX_COLL_CYLINDERS;i++)
		{
			if (collInfo.cylinder[i].Index!=-1)
			{
				collInfo.cylinder[i].actorInstance = cylInstances[collInfo.cylinder[i].Index];

				if (cylInstances[collInfo.cylinder[i].Index]->RespondToCollisionRoutine)
				{
					if (!cylInstances[collInfo.cylinder[i].Index]->RespondToCollisionRoutine(actorInstance,cylInstances[collInfo.cylinder[i].Index]))
					{
						if (actorInstance->RespondToCollisionRoutine)
						{
							actorInstance->RespondToCollisionRoutine(cylInstances[collInfo.cylinder[i].Index],actorInstance);
						}
					}
				}
				else
				{
					if (actorInstance->RespondToCollisionRoutine)
					{
						actorInstance->RespondToCollisionRoutine(cylInstances[collInfo.cylinder[i].Index],actorInstance);
					}
				}
				coll = 3;
			}
			else
			{
				break;
			}
		}

		//precess spheres
		for(i = 0; i < MAX_COLL_SPHERES; i++)
		{
			if(collInfo.sphere[i].Index != -1)
			{
				collInfo.sphere[i].actorInstance = sphereInstances[collInfo.sphere[i].Index];

				if(sphereInstances[collInfo.sphere[i].Index]->RespondToCollisionRoutine)
				{
					if (! sphereInstances[collInfo.sphere[i].Index]->RespondToCollisionRoutine(actorInstance, sphereInstances[collInfo.sphere[i].Index]))
					{
						// TP: if the respond to collision routine does not destroy the sphereinstance
						if(actorInstance->RespondToCollisionRoutine)
						{
							actorInstance->RespondToCollisionRoutine(sphereInstances[collInfo.sphere[i].Index],actorInstance);
						}
					}
				}
				else
				{
					if(actorInstance->RespondToCollisionRoutine)
					{
						actorInstance->RespondToCollisionRoutine(sphereInstances[collInfo.sphere[i].Index],actorInstance);
					}
				}
				coll = 2;
			}
			else
			{
				break;
			}
		}

		// process meshes next
		for (i=0;i<MAX_COLL_MESHES;i++)
		{
			if (collInfo.mesh[i].Index!=-1)
			{
				collInfo.mesh[i].collEntry = meshLists[collInfo.mesh[i].Index];

				if (!(flags&PROCESSCOLLFLAG_DONTAFFECTOBJECTS))
				{
					coll = 1;

					if ((meshLists[collInfo.mesh[i].Index]->actorInstance)&&(instances[collInfo.mesh[i].Index]==true))
					{
						if (meshLists[collInfo.mesh[i].Index]->actorInstance->pathInfo)
						{
							if (actorInstance->actorStatus)
							{
								// CMD: check for collision with platform other than previous
								if(actorInstance->actorStatus->pathInfoCollidingWith)
								{
									if(actorInstance->actorStatus->pathInfoCollidingWith != meshLists[collInfo.mesh[i].Index]->actorInstance)
									{
										if (actorInstance->actorStatus->pathInfoCollidingWith->pathInfo->type==PATHTYPE_SPIN)
										{
											ChangePathState(actorInstance->actorStatus->pathInfoCollidingWith->pathInfo,PATHSTATE_ANTICLOCKWISE);
										}
									}
								}
								actorInstance->actorStatus->pathInfoCollidingWith = meshLists[collInfo.mesh[i].Index]->actorInstance;
								actorInstance->actorStatus->timeSinceCollidingWithPath = 0.0f;
							}
						}
						if (meshLists[collInfo.mesh[i].Index]->actorInstance->RespondToCollisionRoutine)
						{
							// call collision responce
							if (meshLists[collInfo.mesh[i].Index]->actorInstance->RespondToCollisionRoutine(actorInstance,meshLists[collInfo.mesh[i].Index]->actorInstance))
							{
								// TP: if returns true then assume the object has been destroyed and don't allow the collentry to be used
								collInfo.mesh[i].collEntry = NULL;
							}
						}
						// TP: dont respond to this collision again
						instances[collInfo.mesh[i].Index] = false;
					}
				}
			}
			else
			{
				break;
			}
		}
		if (retCollInfo) *retCollInfo = &collInfo;
	}

	fTime = oldfTime;	
	SetWorldGravity(NORMAL_WORLD_GRAVITY);
	
	return coll;
}


/* --------------------------------------------------------------------------------
   Function : RayTest
   Purpose : Test if the ray hits any polys in the collision cache
   Parameters : COLLISIONCACHE, TBVector start, TBVector end
   Returns : TRUE - found point, FALSE - no point found
   Info : 
*/

bool RayTest(COLLISIONCACHE *collisionCache, TBVector start, TBVector end)
{
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	int					numberOfMeshes=0;
	int					i;
	TBVector			testPos,tempVector;
	BOUNDINGBOX			actorBox;
	COLLISIONLISTENTRY	*ptr;
	BOOL				value;
	TBVector			intersect;								// PP: point of intersection with the meshes, below test point
	bool				retVal=false;

	actorBox.xmin = start[X] < end[X]?start[X]:end[X];
	actorBox.xmax = start[X] > end[X]?start[X]:end[X];
	actorBox.ymin = start[Y] < end[Y]?start[Y]:end[Y];
	actorBox.ymax = start[Y] > end[Y]?start[Y]:end[Y];
	actorBox.zmin = start[Z] < end[Z]?start[Z]:end[Z];
	actorBox.zmax = start[Z] > end[Z]?start[Z]:end[Z];

	meshLists[numberOfMeshes] = NULL;
	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		// test if in or going to enter bounding box
		if ((!ptr->actorInstance)||(ptr->actorInstance->instanceList)&&(ptr->actorInstance->instanceList!=&map.triggerInstances)&&(ptr->actorInstance->instanceList!=&map.invisibleInstances))
		{
			if ((numberOfMeshes<MAX_COLL_MESHES)&&TestBoxCollision(&actorBox,&ptr->boundingBox))
			{
				if (MakeEntryResident(collisionCache,ptr)==TRUE)
				{
					// resident so add to collision process
					meshLists[numberOfMeshes] = ptr;
					numberOfMeshes++;
				}
				else
				{
#if BPLATFORM==GAMECUBE
					static int onceOnly = 1;
					if (onceOnly)
					{
						bkPrintf("*** WARNING *** Could not make entry resident\n");
						onceOnly = 0;
					}
#else
					bkPrintf("*** WARNING *** Could not make entry resident\n");
#endif
				}
			}
		}
	}

	for (i=0;i<numberOfMeshes;i++)
	{
		if(RayTriangleCollision(meshLists[i], start, end, intersect, &actorBox))
		{
			retVal = true;
			break;
		}
	}

	return retVal;
}


/* --------------------------------------------------------------------------------
   Function : ProcessHeightOfPoint
   Purpose : Run through collision cache and return point on ground below given coord
   Parameters : COLLISIONCACHE, TBVector point, TBVector pointOnGround
   Returns : TRUE - found point, FALSE - no point found
   Info : 
*/
BOOL ProcessHeightOfPoint(COLLISIONCACHE *collisionCache, TBVector point, float *pointOnGround, int flags, ESpecialType specialType)
{
	COLLISIONLISTENTRY	*meshLists[MAX_COLL_MESHES];
	ACTORINSTANCE		*instances[MAX_COLL_MESHES];
	COLLISIONLISTENTRY	*collisionEntries[MAX_COLL_MESHES];
	int					numberOfMeshes=0;
	int					i;
	int					numberOfVerts[MAX_COLL_MESHES];
	TBVector				testPos,tempVector;
	BOUNDINGBOX			actorBox;
	COLLISIONLISTENTRY	*ptr;
	BOOL				value;

	// PP: 	
	TBVector			intersect;								// PP: point of intersection with the meshes, below test point

	bmVectorCopy(testPos,point);

	actorBox.xmin = testPos[X];
	actorBox.xmax = testPos[X];
	actorBox.ymin = testPos[Y];
	actorBox.ymax = testPos[Y];
	actorBox.zmin = testPos[Z];
	actorBox.zmax = testPos[Z];

	meshLists[numberOfMeshes] = NULL;
	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		// test if in or going to enter bounding box
		if ((flags&PHOP_EXCLUDEOBJECTS)&&(ptr->actorInstance))
		{
			// TP: if collision entry has an actorInstance then it is not part of the set
			continue;
		}
		if(((uint32)specialType != -1)&&(ptr->actorInstance))
		{
			// CMD: if collision entry not part of set and specified specialType then don't test
			if(specialType == ptr->actorInstance->special) continue;
		}
		if ((!ptr->actorInstance)||(ptr->actorInstance->instanceList)&&(ptr->actorInstance->instanceList!=&map.triggerInstances)&&(ptr->actorInstance->instanceList!=&map.invisibleInstances))
		{
			if (TestBoxCollisionXZ(&actorBox,&ptr->boundingBox)&&(numberOfMeshes<MAX_COLL_MESHES))
			{
				if (MakeEntryResident(collisionCache,ptr)==TRUE)
				{
					// resident so add to collision process
					meshLists[numberOfMeshes] = ptr;
					numberOfMeshes++;
				}
				else
				{
#if BPLATFORM==GAMECUBE
					static int onceOnly = 1;
					if (onceOnly)
					{
						bkPrintf("*** WARNING *** Could not make entry resident\n");
						onceOnly = 0;
					}
#else
					bkPrintf("*** WARNING *** Could not make entry resident\n");
#endif
				}
			}
		}
	}

	tempVector[X] = testPos[X];
	tempVector[Y] = -1000000.0f;	// any large negative number
	tempVector[Z] = testPos[Z];
	tempVector[W] = 1.0f;
	value = 0;
	for (i=0;i<numberOfMeshes;i++)
	{
		if (meshLists[i]->boundingBox.ymax > tempVector[Y]) // only do test if box is higher that current point
		{

			// PP: ***************************************************************************************
			// PP: NOW USING BABEL LINE-TRIANGLE COLLISION FOR THE HEIGHT-OF-POINT TEST - AND IT NOW WORKS
			// PP: ***************************************************************************************

			// PP: if (CircleFaceCollision(vertLists[i], numberOfVerts[i], point, 0 ,TRUE ,pointOnGround))
			if(RayTriangleCollision(meshLists[i], testPos, tempVector, intersect))
			{
				*pointOnGround=intersect[Y];

				value = 1;
				if ((*pointOnGround) > tempVector[Y])
				{
					tempVector[Y] = (*pointOnGround);
				}
			}
		}
	}

	(*pointOnGround) = tempVector[Y];
	return value;
}

/* --------------------------------------------------------------------------------
   Function : CreateCollisionCache
   Purpose : Creates and intialises a collision cache
   Parameters : COLLISIONCACHE, maxSize in bytes
   Returns : TRUE/FALSE
   Info : 
*/
BOOL CreateCollisionCache(COLLISIONCACHE *collisionCache, int maxSize)
{
	collisionCache->collisionList.numEntries = 0;
	collisionCache->collisionList.head.next = collisionCache->collisionList.head.prev = &collisionCache->collisionList.head;
	collisionCache->maxMemoryAvailable = maxSize;
	collisionCache->memoryUsed = collisionCache->peakMemoryUsed = 0;
	collisionCache->cylinderList.numEntries = 0;
	collisionCache->cylinderList.head.next = collisionCache->cylinderList.head.prev = &collisionCache->cylinderList.head;
	collisionCache->sphereList.numEntries = 0;
	collisionCache->sphereList.head.next = collisionCache->sphereList.head.prev = &collisionCache->sphereList.head;
	collisionCache->hitsPerFrame = 0;
	collisionCache->missesPerFrame = 0;
	collisionCache->hitsPerScene = 0;
	collisionCache->missesPerScene = 0;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateCollisionCache
   Purpose : updates the collision cache freeing and creating collision meshes as needed
   Parameters : COLLISIONCACHE
   Returns : TRUE/FALSE
   Info : should be called once per frame
*/
void UpdateCollisionCache(COLLISIONCACHE *collisionCache)
{
	COLLISIONLISTENTRY *ptr,*temp;

	collisionCache->hitsPerScene += collisionCache->hitsPerFrame;
	collisionCache->missesPerScene += collisionCache->missesPerFrame;
	collisionCache->hitsPerFrame = 0;
	collisionCache->missesPerFrame = 0;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;)
	{
		temp = ptr->next;
		ptr->timeSinceLastUsed += fTime;
		if (ptr->flags&CACHEFLAGS_LIVEUPDATE)
		{
			if (ptr->actorInstance->flags&ACTORFLAG_HASANIMATED)
			{
				ptr->flags |= CACHEFLAGS_NEEDSUPDATE;
			}
			else
			{
				// TP: has object moved,scaled or rotated.
				if (ptr->liveUpdateInfo)
				{
					if (
						(!bmVectorEqual(ptr->actorInstance->actorInstance.position,ptr->liveUpdateInfo->position))||
						(!bmVectorEqual(ptr->actorInstance->actorInstance.scale,ptr->liveUpdateInfo->scale))||
						(!bmVectorEqual(ptr->actorInstance->actorInstance.orientation,ptr->liveUpdateInfo->orientation)))
					{
						ptr->flags |= CACHEFLAGS_NEEDSUPDATE;
					}
				}
				else
				{
					ptr->flags |= CACHEFLAGS_NEEDSUPDATE;
				}
			}
			UpdateCollisionBoundingBox(ptr);
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : MakeEntireCollisionCacheDormant
   Purpose : Make every entry in the collision cache dormant and frees the memory
   Parameters : COLLISIONCACHE
   Returns : 
   Info : 
*/
void MakeEntireCollisionCacheDormant(COLLISIONCACHE *collisionCache)
{
	COLLISIONLISTENTRY *ptr;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		if ((ptr->cacheStatus == CACHESTATUS_RESIDENT)&&(!(ptr->flags&CACHEFLAGS_STAYRESIDENT)))
		{
			MakeEntryDormant(collisionCache,ptr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeCollisionCache
   Purpose : removes the collision cache and all collision entries in its list
   Parameters : COLLISIONCACHE
   Returns : 
   Info : 
*/
void FreeCollisionCache(COLLISIONCACHE *collisionCache)
{
	COLLISIONLISTENTRY *ptr,*temp;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;)
	{
		temp = ptr->next;

		if (ptr->cacheStatus==CACHESTATUS_RESIDENT)
		{
			MakeEntryDormant(collisionCache,ptr);
		}
		if (ptr->liveUpdateInfo) SAFE_FREE(ptr->liveUpdateInfo);

		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		collisionCache->collisionList.numEntries--;
		SAFE_FREE(ptr);
		ptr = temp;
	}
	for (ptr = collisionCache->cylinderList.head.next;ptr != &collisionCache->cylinderList.head;)
	{
		temp = ptr->next;

		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		collisionCache->cylinderList.numEntries--;
		SAFE_FREE(ptr);
		ptr = temp;
	}
	for(ptr = collisionCache->sphereList.head.next; ptr != &collisionCache->sphereList.head;)
	{
		temp = ptr->next;

		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		collisionCache->sphereList.numEntries--;
		SAFE_FREE(ptr);
		ptr = temp;
	}

	bkPrintf("Collision Cache Free'd, peak usage %d\n",collisionCache->peakMemoryUsed);
}

/* --------------------------------------------------------------------------------
   Function : SubDivideCollisionEntry
   Purpose : add an actorInstance to the collision cache system and sub divide
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags, subdividex, y ,z 
   Returns : TRUE/FALSE
   Info : 
*/
BOOL SubDivideCollisionEntry(COLLISIONCACHE *collisionCache,COLLISIONLISTENTRY *collisionListEntry, uint32 flags,int subDivideX,int subDivideY,int subDivideZ)
{
	COLLISIONLISTENTRY	*tempEntry;
	COLLPOLY	**polyLists;
	BOUNDINGBOX	**boundingBoxes;
	BOUNDINGBOX	*boundingBox;
	int			subDivideYX = subDivideY*subDivideX;
	int32		*count,i,xInc,yInc,zInc,x,y,z,j;
	int32		numBoxes = subDivideX*subDivideY*subDivideZ;
	int			currBox;
	float		value,fx,fy,fz,fw;

	polyLists = (COLLPOLY**)ZALLOC(sizeof(COLLPOLY*)*numBoxes);
	ASSERT(polyLists);

	count = (int32*)ZALLOC(sizeof(int32*)*numBoxes);
	ASSERT(count);

	boundingBoxes = (BOUNDINGBOX**)ZALLOC(sizeof(BOUNDINGBOX*)*numBoxes);
	ASSERT(boundingBoxes);

	xInc = (int32)((collisionListEntry->boundingBox.xmax - collisionListEntry->boundingBox.xmin) / subDivideX) + 1;
	yInc = (int32)((collisionListEntry->boundingBox.ymax - collisionListEntry->boundingBox.ymin) / subDivideY) + 1;
	zInc = (int32)((collisionListEntry->boundingBox.zmax - collisionListEntry->boundingBox.zmin) / subDivideZ) + 1;

	if ((!polyLists)||(!count)||(!boundingBoxes))
	{
		bkPrintf("*** WARNING *** Could not allocate memory for sub divide.\n");
	}

	for (i=0;i<numBoxes;i++) 
	{
		count[i] = 0;
		polyLists[i] = NULL;
	}

	for (i=0;i<collisionListEntry->numPolys;i++)
	{
		COLLPOLY_GETV1(collisionListEntry->polys[i], collisionListEntry->vertices, fx, fy, fz, fw);
		// TP: For each group of 3 verts
		x = (int32)((fx - collisionListEntry->boundingBox.xmin) / xInc);
		y = (int32)((fy - collisionListEntry->boundingBox.ymin) / yInc);
		z = (int32)((fz - collisionListEntry->boundingBox.zmin) / zInc);

		// TP: Count number of polys in each sub box
		currBox = x+(y*subDivideX)+(z*subDivideYX);
		if (currBox >= numBoxes)
		{
			bkPrintf("*** ERROR *** problem in subdivide coll mesh.\n");
		}
		count[currBox]++;
	}
	/*
	for (i=0;i<collisionListEntry->numVertices;i+=3)
	{
		// TP: For each group of 3 verts
		x = (int32)((collisionListEntry->vertices[i].x - collisionListEntry->boundingBox.xmin) / xInc);
		y = (int32)((collisionListEntry->vertices[i].y - collisionListEntry->boundingBox.ymin) / yInc);
		z = (int32)((collisionListEntry->vertices[i].z - collisionListEntry->boundingBox.zmin) / zInc);

		// TP: Count number of polys in each sub box
		currBox = x+(y*subDivideX)+(z*subDivideYX);
		if (currBox >= numBoxes)
		{
			bkPrintf("*** ERROR *** problem in subdivide coll mesh.\n");
		}
		count[currBox]++;
	}*/

	// TP: Create collision cache entries for all used sub boxes
	for (i=0;i<numBoxes;i++)
	{
		if (count[i])
		{
			// TP: if box contains polys then create coll entry
			tempEntry = CreateAndAddCollisionListEntry(&collisionCache->collisionList,NULL);
			if (tempEntry==NULL)
			{
				bkPrintf("*** WARNING *** could not subdivide object\n");
				return FALSE;
			}
			tempEntry->numPolys = count[i];
			tempEntry->numVertices = collisionListEntry->numVertices;

			tempEntry->memoryNeeded = count[i]*sizeof(COLLPOLY);
			collisionCache->memoryUsed += tempEntry->memoryNeeded;

			tempEntry->polys = (COLLPOLY*)ZALLOC(tempEntry->memoryNeeded);

			tempEntry->vertices = collisionListEntry->vertices;
			if (tempEntry->vertices==NULL)
			{
				tempEntry->numVertices = 0;
				tempEntry->memoryNeeded = 0;
				bkPrintf("*** WARNING *** could not subdivide object\n");
				return FALSE;
			}
			tempEntry->flags = flags|CACHEFLAGS_DONTFREEVERTS;
			tempEntry->cacheStatus = CACHESTATUS_RESIDENT;
			tempEntry->boundingBox.xmax = -1000000.0f;
			tempEntry->boundingBox.xmin = 1000000.0f;
			tempEntry->boundingBox.ymax = -1000000.0f;
			tempEntry->boundingBox.ymin = 1000000.0f;
			tempEntry->boundingBox.zmax = -1000000.0f;
			tempEntry->boundingBox.zmin = 1000000.0f;

			polyLists[i] = tempEntry->polys;
			boundingBoxes[i] = &tempEntry->boundingBox;
		}
	}

	// TP: Move polys into thier groups
	for (i=0;i<collisionListEntry->numPolys;i++)
	{
		COLLPOLY_GETV1(collisionListEntry->polys[i], collisionListEntry->vertices, fx, fy, fz, fw);
		// TP: For each group of 3 verts
		x = (int32)((fx - collisionListEntry->boundingBox.xmin) / xInc);
		y = (int32)((fy - collisionListEntry->boundingBox.ymin) / yInc);
		z = (int32)((fz - collisionListEntry->boundingBox.zmin) / zInc);

		// TP: Count number of polys in each sub box
		currBox = x+(y*subDivideX)+(z*subDivideYX);
		boundingBox = boundingBoxes[currBox];

		COLLPOLY_GETV1(collisionListEntry->polys[i], collisionListEntry->vertices, fx, fy, fz, fw);
		if (fx > boundingBox->xmax) boundingBox->xmax = fx;
		if (fx < boundingBox->xmin) boundingBox->xmin = fx;
		if (fy > boundingBox->ymax) boundingBox->ymax = fy;
		if (fy < boundingBox->ymin) boundingBox->ymin = fy;
		if (fz > boundingBox->zmax) boundingBox->zmax = fz;
		if (fz < boundingBox->zmin) boundingBox->zmin = fz;
		COLLPOLY_GETV2(collisionListEntry->polys[i], collisionListEntry->vertices, fx, fy, fz, fw);
		if (fx > boundingBox->xmax) boundingBox->xmax = fx;
		if (fx < boundingBox->xmin) boundingBox->xmin = fx;
		if (fy > boundingBox->ymax) boundingBox->ymax = fy;
		if (fy < boundingBox->ymin) boundingBox->ymin = fy;
		if (fz > boundingBox->zmax) boundingBox->zmax = fz;
		if (fz < boundingBox->zmin) boundingBox->zmin = fz;
		COLLPOLY_GETV3(collisionListEntry->polys[i], collisionListEntry->vertices, fx, fy, fz, fw);
		if (fx > boundingBox->xmax) boundingBox->xmax = fx;
		if (fx < boundingBox->xmin) boundingBox->xmin = fx;
		if (fy > boundingBox->ymax) boundingBox->ymax = fy;
		if (fy < boundingBox->ymin) boundingBox->ymin = fy;
		if (fz > boundingBox->zmax) boundingBox->zmax = fz;
		if (fz < boundingBox->zmin) boundingBox->zmin = fz;

		fmemcpy(polyLists[currBox],&collisionListEntry->polys[i],sizeof(COLLPOLY));
		polyLists[currBox] ++;
	}

	SAFE_FREE(polyLists);
	SAFE_FREE(count);
	SAFE_FREE(boundingBoxes);

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCylinderCache
   Purpose : add an actorInstance to the collision cylinder cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : TRUE/FALSE
   Info : 
*/
BOOL AddActorToCollisionCylinderCache(COLLISIONCACHE *collisionCache,ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *cylinderListEntry;

	if (actorInstance->actorBehaviour)
	{
		cylinderListEntry = CreateAndAddCollisionListEntry(&collisionCache->cylinderList,actorInstance);
		if (cylinderListEntry)
		{
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionSphereCache
   Purpose : add an actorInstance to the collision cylinder cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : TRUE/FALSE
   Info : 
*/

BOOL AddActorToCollisionSphereCache(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY		*sphereListEntry;

	if(actorInstance->actorBehaviour)
	{
		sphereListEntry = CreateAndAddCollisionListEntry(&collisionCache->sphereList, actorInstance);
		if(!sphereListEntry)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ActivateLiveUpdate
   Purpose : turn on live update for an collision entry
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void ActivateLiveUpdate(COLLISIONLISTENTRY *collisionEntry)
{
	if (!collisionEntry) return;
	if (collisionEntry->flags&CACHEFLAGS_LIVEUPDATE) return;

	collisionEntry->liveUpdateInfo = (LIVEUPDATEINFO*)ZALLOC(sizeof(LIVEUPDATEINFO));
	if (!collisionEntry->liveUpdateInfo) return;

	collisionEntry->flags |= CACHEFLAGS_NEEDSUPDATE|CACHEFLAGS_LIVEUPDATE;
}

/* --------------------------------------------------------------------------------
   Function : DeactiveLiveUpdate
   Purpose : stop the live update for the collisionentry
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void DeactivateLiveUpdate(COLLISIONLISTENTRY *collisionEntry)
{
	if (!collisionEntry) return;
	if (!(collisionEntry->flags&CACHEFLAGS_LIVEUPDATE)) return;

	if (collisionEntry->liveUpdateInfo) SAFE_FREE(collisionEntry->liveUpdateInfo);
	collisionEntry->flags &= ~(CACHEFLAGS_NEEDSUPDATE|CACHEFLAGS_LIVEUPDATE);
}

/*  --------------------------------------------------------------------------------
	Function	: CollDataType
	Purpose		: Determines if the collision data for the object is the new style or old
	Parameters	: set
	Returns		: 0 - old, 1 - new
	Info		: 
*/

int	CollDataType(TBSet *set)
{
	return 1;
}

/*  --------------------------------------------------------------------------------
	Function	: CollDataType
	Purpose		: Determines if the collision data for the object is the new style or old
	Parameters	: set
	Returns		: 0 - old, 1 - new
	Info		: 
*/

int	CollDataType(ACTORINSTANCE *actorInstance)
{
	int	bufSize;

	baGetActorGeometry(&actorInstance->actorInstance, NULL, 1, NULL, NULL, &bufSize, NULL, BGETACTGEOMETRY_COUNTONLY);
	
	if (bufSize) 
	{
		return 1;
	}
	else 
	{
		baGetActorGeometry(&actorInstance->actorInstance, NULL, 1, NULL, NULL, &bufSize, NULL, BGETACTGEOMETRY_COUNTONLY);
		if (bufSize)
		{
			return 0;
		}
		else
		{
			// TP: no data at all
			return 1;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCache
   Purpose : add an actorInstance to the collision cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : collisionListEntry/FALSE
   Info : SET VERSION
*/
COLLISIONLISTENTRY *AddActorToCollisionCache(COLLISIONCACHE *collisionCache, TBSet *set, uint32 flags)
{
	COLLISIONLISTENTRY *collisionListEntry;

	if (set==NULL) return NULL;

	// TP: identify style of collision data
	if (CollDataType(set)==0)
	{
		ASSERTM(0, "*** ERROR *** Collision data is old style, package must be re-built!!!\n");
	}

	collisionListEntry = CreateAndAddCollisionListEntry(&collisionCache->collisionList, NULL);
		
	if (collisionListEntry)
	{
		collisionListEntry->numVertices = NumberOfVertsInSet(set);
		collisionListEntry->numPolys = NumberOfTrisInSet(set);
		collisionListEntry->memoryNeeded = collisionListEntry->numVertices*sizeof(COLLVERTEX)+collisionListEntry->numPolys*sizeof(COLLPOLY);

		if (collisionListEntry->memoryNeeded==0)
		{
			return NULL;
		}

		collisionListEntry->flags = flags;

		collisionListEntry->vertices = (COLLVERTEX*)baGetSetGeometry(set,0,0,NULL);
		collisionListEntry->polys = (COLLPOLY*)baGetSetGeometry(set,1,0,NULL);
		collisionListEntry->cacheStatus = CACHESTATUS_RESIDENT;
		FillInMinMaxAndNormal(collisionListEntry->vertices,collisionListEntry->polys, collisionListEntry->numPolys,false);

		// if resident then create bounding box from stored verts
		CalculateBoundingBox(&collisionListEntry->boundingBox,&collisionListEntry->localBoundingBox,NULL,collisionListEntry->vertices,collisionListEntry->numVertices);

		if (flags&CACHEFLAGS_SUB)
		{
			fmemcpy(&worldBoundingBox,&collisionListEntry->boundingBox,sizeof(BOUNDINGBOX));
			worldBoundingBox.xmax += METERTOUNIT(2.0f);
			worldBoundingBox.xmin -= METERTOUNIT(2.0f);
			worldBoundingBox.ymax += METERTOUNIT(2.0f);
			worldBoundingBox.ymin -= METERTOUNIT(2.0f);
			worldBoundingBox.zmax += METERTOUNIT(2.0f);
			worldBoundingBox.zmin -= METERTOUNIT(2.0f);
			SubDivideCollisionEntry(collisionCache,collisionListEntry,flags,10,4,10);

			collisionListEntry->prev->next = collisionListEntry->next;
			collisionListEntry->next->prev = collisionListEntry->prev;
			collisionCache->collisionList.numEntries--;
			SAFE_FREE(collisionListEntry);

		}
		return collisionListEntry;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : AddActorToCollisionCache
   Purpose : add an actorInstance to the collision cache system 
   Parameters : COLLISIONCACHE, ACTORINSTANCE, flags
   Returns : collisionListEntry/FALSE
   Info : 
*/
COLLISIONLISTENTRY *AddActorToCollisionCache(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance, uint32 flags)
{
	COLLISIONLISTENTRY *collisionListEntry;

	if (actorInstance==NULL) return NULL;

	// TP: identify style of collision data
	if (CollDataType(actorInstance)==0)
	{
		bkPrintf("*** ERROR *** Collision data is old style on %s, package must be re-built!!!\n", actorInstance->actorName);
		ASSERT(0);
	}

	collisionListEntry = CreateAndAddCollisionListEntry(&collisionCache->collisionList, actorInstance);
		
	if (collisionListEntry)
	{
		collisionListEntry->numVertices = NumberOfVertsInActor(&actorInstance->actorInstance);
		collisionListEntry->numPolys = NumberOfTrisInActor(&actorInstance->actorInstance);
		collisionListEntry->memoryNeeded = collisionListEntry->numVertices*sizeof(COLLVERTEX)+collisionListEntry->numPolys*sizeof(COLLPOLY);

		if (collisionListEntry->memoryNeeded==0)
		{
			RemoveAndFreeCollisionListEntry(collisionCache,actorInstance);
			return NULL;
		}

		if (flags&CACHEFLAGS_DOUBLESIDED)
		{
			collisionListEntry->memoryNeeded += collisionListEntry->numPolys*sizeof(COLLPOLY);
		}

		collisionListEntry->flags = flags;

		if (flags&CACHEFLAGS_LIVEUPDATE)
		{
			collisionListEntry->liveUpdateInfo = (LIVEUPDATEINFO*)ZALLOC(sizeof(LIVEUPDATEINFO));
			collisionListEntry->flags |= CACHEFLAGS_NEEDSUPDATE;
		}

		if (collisionListEntry->flags&CACHEFLAGS_STAYRESIDENT)
		{
			// if STAYRESIDENT they make resident once only
			MakeEntryResident(collisionCache,collisionListEntry);
		}
		if (collisionListEntry->cacheStatus==CACHESTATUS_RESIDENT)
		{
			// if resident then create bounding box from stored verts
			CalculateBoundingBox(&collisionListEntry->boundingBox,&collisionListEntry->localBoundingBox,actorInstance,collisionListEntry->vertices,collisionListEntry->numVertices);
		}
		else
		{
			// if not resident then load mesh, create bounding box and then free mesh
			CalculateBoundingBox(&collisionListEntry->boundingBox,&collisionListEntry->localBoundingBox,actorInstance,NULL,collisionListEntry->numVertices);
		}

		if (flags&CACHEFLAGS_SUB)
		{
			fmemcpy(&worldBoundingBox,&collisionListEntry->boundingBox,sizeof(BOUNDINGBOX));
			worldBoundingBox.xmax += METERTOUNIT(2.0f);
			worldBoundingBox.xmin -= METERTOUNIT(2.0f);
			worldBoundingBox.ymax += METERTOUNIT(2.0f);
			worldBoundingBox.ymin -= METERTOUNIT(2.0f);
			worldBoundingBox.zmax += METERTOUNIT(2.0f);
			worldBoundingBox.zmin -= METERTOUNIT(2.0f);
			//SubDivideCollisionEntry(collisionCache,collisionListEntry,flags,10,4,10);
			//RemoveActorFromCollisionCache(collisionCache,actorInstance);
		}

		if (flags&CACHEFLAGS_LIVEUPDATE)
		{
			// if liveupdate then must perform update bounding box
			UpdateCollisionBoundingBox(collisionListEntry);
		}

		return collisionListEntry;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : MakeCollisionEntryDoubleSided
   Purpose : Create a second side to existing verts
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void MakeCollisionEntryDoubleSided(COLLISIONLISTENTRY *collisionListEntry)
{
	COLLPOLY	*source,*dest;
	int i;

	dest = &collisionListEntry->polys[collisionListEntry->numPolys];

	for (i=0;i<collisionListEntry->numPolys;i++)
	{
		fmemcpy(&dest[i], &collisionListEntry->polys[i], sizeof(COLLPOLY));
		dest[i].nx = -dest[i].nx;
		dest[i].ny = -dest[i].ny;
		dest[i].nz = -dest[i].nz;
	}
}


/* --------------------------------------------------------------------------------
   Function : DrawCollisionListEntry
   Purpose : Draw the collision list entry for debug purposes
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void DrawCollisionEntry(COLLISIONLISTENTRY *collisionListEntry)
{
	TBPrimVertex	*verts;
	TBPrimVertex	point[2];
	COLLVERTEXOLD		*collVerts;
	TBVector		tempVector,norm,tempVector2;

	if (!collisionListEntry) return;

	verts = (TBPrimVertex*)ZALLOC(collisionListEntry->numPolys*2*sizeof(TBPrimVertex));
	if (!verts) return;

	for (int i=0;i<collisionListEntry->numPolys;i++)
	{
		COLLPOLY_GETV1(collisionListEntry->polys[i], collisionListEntry->vertices, tempVector[X], tempVector[Y], tempVector[Z], tempVector[W]);
		bmVectorCopy(tempVector2, tempVector);
		COLLPOLY_GETV2(collisionListEntry->polys[i], collisionListEntry->vertices, tempVector[X], tempVector[Y], tempVector[Z], tempVector[W]);
		bmVectorAdd(tempVector2, tempVector2, tempVector);
		COLLPOLY_GETV3(collisionListEntry->polys[i], collisionListEntry->vertices, tempVector[X], tempVector[Y], tempVector[Z], tempVector[W]);
		bmVectorAdd(tempVector2, tempVector2, tempVector);

		bmVectorCDiv(tempVector2, tempVector2, 3);

		BDVERTEX_SETXYZW(&point[0],tempVector2[X],tempVector2[Y],tempVector2[Z],1.0f);
		BDVERTEX_SETRGBA(&point[0],255,255,255,255);

		bmVectorSet(tempVector, collisionListEntry->polys[i].nx, collisionListEntry->polys[i].ny, collisionListEntry->polys[i].nz, 1.0f);
		bmVectorCMul(tempVector, tempVector, METERTOUNIT(1.0f));
		bmVectorAdd(tempVector2, tempVector2, tempVector);

		BDVERTEX_SETXYZW(&point[1],tempVector2[X],tempVector2[Y],tempVector2[Z],1.0f);
		BDVERTEX_SETRGBA(&point[1],255,255,255,255);
		
		bdDrawPrimitive(BDPRIMTYPE_LINELIST,point,2,BVERTTYPE_SINGLE);
	}
/*
	for (int i=0;i<collisionListEntry->numVertices;i++)
	{
		COLLVERTEXOLD_GETXYZW(&collVerts[i],tempVector[X],tempVector[Y],tempVector[Z],tempVector[W]);
		BDVERTEX_SETXYZW(&verts[i],tempVector[X],tempVector[Y],tempVector[Z],tempVector[W]);
		BDVERTEX_SETRGBA(&verts[i],128,0,0,60);
		if ((i%3)==0)	
		{
			fmemcpy(point,&verts[i],sizeof(TBPrimVertex));
			COLLVERTEXOLD_GETNORMAL(&collVerts[i],norm[X],norm[Y],norm[Z]);
			bmVectorScaleToLength(tempVector2,norm,METERTOUNIT(1.0f));
			bmVectorAdd(tempVector,tempVector,tempVector2);
			BDVERTEX_SETXYZW(&point[1],tempVector[X],tempVector[Y],tempVector[Z],1.0f);
			BDVERTEX_SETRGBA(&point[1],255,255,255,255);
			bdDrawPrimitive(BDPRIMTYPE_LINELIST,point,2,BVERTTYPE_SINGLE);
		}
		BDVERTEX_SETNORMAL(&verts[i],norm[X],norm[Y],norm[Z]);
	}
*/
//	ASSERT(collisionListEntry->numVertices);
//	bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST,verts,collisionListEntry->numVertices,BVERTTYPE_SINGLE);

	SAFE_FREE(verts);
}


/* --------------------------------------------------------------------------------
   Function : DrawResidentCollisionEntries
   Purpose : Draw all collision entrys that are resident except the landscape
   Parameters : COLLISIONLIST
   Returns : 
   Info : 
*/
void DrawResidentCollisionEntries(COLLISIONCACHE *collisionCache)
{
	COLLISIONLISTENTRY	*ptr;
	COLLISIONLIST		*collisionList = &collisionCache->collisionList;

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,TRUE,FALSE);
	bdSetTexture(0, NULL);

	bdSetZBias(-100000.0f);

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		if ((ptr->cacheStatus == CACHESTATUS_RESIDENT)&&(!(ptr->flags&CACHEFLAGS_STAYRESIDENT)))
		{
			DrawCollisionEntry(ptr);
		}
	}
	bdSetZBias(0.0f);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : UpdateCollisionBoundingBox
   Purpose : updates the world bounding box based on the position of the actor and the local bounding box
   Parameters : COLLISIONLISTENTRY
   Returns : 
   Info : For speed this creates a bounding box using the maxRadius parameter to set the size
*/
void UpdateCollisionBoundingBox(COLLISIONLISTENTRY *collisionListEntry)
{
	float		maxScale;

	if (!collisionListEntry->actorInstance) return;

	maxScale = collisionListEntry->actorInstance->actorInstance.scale[X];
	if (collisionListEntry->actorInstance->actorInstance.scale[Y] > maxScale) maxScale = collisionListEntry->actorInstance->actorInstance.scale[Y];
	if (collisionListEntry->actorInstance->actorInstance.scale[Z] > maxScale) maxScale = collisionListEntry->actorInstance->actorInstance.scale[Z];

	collisionListEntry->boundingBox.xmax = collisionListEntry->actorInstance->actorInstance.position[X] + collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
	collisionListEntry->boundingBox.xmin = collisionListEntry->actorInstance->actorInstance.position[X] - collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
	collisionListEntry->boundingBox.ymax = collisionListEntry->actorInstance->actorInstance.position[Y] + collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
	collisionListEntry->boundingBox.ymin = collisionListEntry->actorInstance->actorInstance.position[Y] - collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
	collisionListEntry->boundingBox.zmax = collisionListEntry->actorInstance->actorInstance.position[Z] + collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
	collisionListEntry->boundingBox.zmin = collisionListEntry->actorInstance->actorInstance.position[Z] - collisionListEntry->actorInstance->actorInstance.actor->maxRadius*maxScale;	
}


/* --------------------------------------------------------------------------------
   Function : RemoveActorFromCollisionCache
   Purpose : remove an actor from the collision cache
   Parameters : COLLISIONCACHE, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveActorFromCollisionCache(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	RemoveAndFreeCollisionSphereListEntry(collisionCache,actorInstance);
	RemoveAndFreeCollisionCylinderListEntry(collisionCache,actorInstance);
	return RemoveAndFreeCollisionListEntry(collisionCache, actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : CreateAndAddCollisionListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
COLLISIONLISTENTRY* CreateAndAddCollisionListEntry(COLLISIONLIST *collisionList, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLISTENTRY *collisionListEntry;

	collisionListEntry = (COLLISIONLISTENTRY*)ZALLOC(sizeof(COLLISIONLISTENTRY));
	if (collisionListEntry)
	{
		collisionListEntry->actorInstance = actorInstance;
		collisionListEntry->timeSinceLastUsed = 0;
		collisionListEntry->cacheStatus = CACHESTATUS_DORMANT;
		collisionListEntry->vertices = NULL;
		collisionListEntry->polys = NULL;
		collisionListEntry->numVertices = 0;
		collisionListEntry->memoryNeeded = 0;
		collisionListEntry->liveUpdateInfo = NULL;

		ptr = collisionList->head.next;
		collisionListEntry->next = ptr;
		collisionListEntry->prev = ptr->prev;
		ptr->prev->next = collisionListEntry;
		ptr->prev = collisionListEntry;
		collisionList->numEntries++;

		return collisionListEntry;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create and add collision list entry\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionListEntry(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLIST *collisionList = &collisionCache->collisionList;

	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			if (ptr->cacheStatus==CACHESTATUS_RESIDENT)
			{
				MakeEntryDormant(collisionCache,ptr);
			}
			if (ptr->liveUpdateInfo) SAFE_FREE(ptr->liveUpdateInfo);
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			collisionList->numEntries--;
			SAFE_FREE(ptr);
			return TRUE;
		}
	}
	return FALSE;
}


/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionCylinderListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionCylinderListEntry(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLIST *collisionList = &collisionCache->cylinderList;

	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			collisionList->numEntries--;
			SAFE_FREE(ptr);
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RemoveAndFreeCollisionSphereListEntry
   Purpose : Creates a collision entry for the specified actor instance
   Parameters : COLLISIONLIST, ACTORINSTANCE
   Returns : TRUE/FALSE
   Info : 
*/
BOOL RemoveAndFreeCollisionSphereListEntry(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLIST *collisionList = &collisionCache->sphereList;

	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			collisionList->numEntries--;
			SAFE_FREE(ptr);
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CalculateBoundingBox
   Purpose : calculates a bounding box for a mesh
   Parameters : current world box, local box, actorinstance,verts/NULL,numverts
   Returns : 
   Info : 
*/
void CalculateBoundingBox(BOUNDINGBOX *boundingBox,BOUNDINGBOX *localBoundingBox,ACTORINSTANCE *actorInstance, COLLVERTEX *svertices,int numberVerts)
{
	int			numberOfVerts,i;
	COLLVERTEX	*vertices;
	float		value;
	TBMatrix		matrix,tempMatrix,matrix2;

	TBVector	vert[8];

	boundingBox->xmax = -1000000.0f;
	boundingBox->xmin = 1000000.0f;
	boundingBox->ymax = -1000000.0f;
	boundingBox->ymin = 1000000.0f;
	boundingBox->zmax = -1000000.0f;
	boundingBox->zmin = 1000000.0f;

	if (actorInstance)
	{
		if (localBoundingBox)
		{
			localBoundingBox->xmin = actorInstance->actorInstance.actor->xMin;
			localBoundingBox->xmax = actorInstance->actorInstance.actor->xMax;
			localBoundingBox->ymin = actorInstance->actorInstance.actor->yMin;
			localBoundingBox->ymax = actorInstance->actorInstance.actor->yMax;
			localBoundingBox->zmin = actorInstance->actorInstance.actor->zMin;
			localBoundingBox->zmax = actorInstance->actorInstance.actor->zMax;
		}

		for (i=0;i<8;i++) vert[i][W] = 1.0f;

		vert[0][X] = actorInstance->actorInstance.actor->xMin;
		vert[0][Y] = actorInstance->actorInstance.actor->yMin;
		vert[0][Z] = actorInstance->actorInstance.actor->zMin;
			
		vert[1][X] = actorInstance->actorInstance.actor->xMax;
		vert[1][Y] = actorInstance->actorInstance.actor->yMin;
		vert[1][Z] = actorInstance->actorInstance.actor->zMin;

		vert[2][X] = actorInstance->actorInstance.actor->xMax;
		vert[2][Y] = actorInstance->actorInstance.actor->yMin;
		vert[2][Z] = actorInstance->actorInstance.actor->zMax;

		vert[3][X] = actorInstance->actorInstance.actor->xMin;
		vert[3][Y] = actorInstance->actorInstance.actor->yMin;
		vert[3][Z] = actorInstance->actorInstance.actor->zMax;

		vert[4][X] = actorInstance->actorInstance.actor->xMin;
		vert[4][Y] = actorInstance->actorInstance.actor->yMax;
		vert[4][Z] = actorInstance->actorInstance.actor->zMin;
		
		vert[5][X] = actorInstance->actorInstance.actor->xMax;
		vert[5][Y] = actorInstance->actorInstance.actor->yMax;
		vert[5][Z] = actorInstance->actorInstance.actor->zMin;

		vert[6][X] = actorInstance->actorInstance.actor->xMax;
		vert[6][Y] = actorInstance->actorInstance.actor->yMax;
		vert[6][Z] = actorInstance->actorInstance.actor->zMax;

		vert[7][X] = actorInstance->actorInstance.actor->xMin;
		vert[7][Y] = actorInstance->actorInstance.actor->yMax;
		vert[7][Z] = actorInstance->actorInstance.actor->zMax;

		bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);

		bmMatScale(tempMatrix,actorInstance->actorInstance.scale[X],
			actorInstance->actorInstance.scale[Y],
			actorInstance->actorInstance.scale[Z]);
		bmMatMultiply(matrix2,tempMatrix,matrix);
		bmMatCopy(matrix,matrix2);

		bmMatTranslate(tempMatrix,actorInstance->actorInstance.position[X],
			actorInstance->actorInstance.position[Y],
			actorInstance->actorInstance.position[Z]);
		bmMatMultiply(matrix2,tempMatrix,matrix);

		bmMatMultiplyVector(matrix2,vert[0]);
		bmMatMultiplyVector(matrix2,vert[1]);
		bmMatMultiplyVector(matrix2,vert[2]);
		bmMatMultiplyVector(matrix2,vert[3]);
		bmMatMultiplyVector(matrix2,vert[4]);
		bmMatMultiplyVector(matrix2,vert[5]);
		bmMatMultiplyVector(matrix2,vert[6]);
		bmMatMultiplyVector(matrix2,vert[7]);

		for (i=0;i<8;i++)
		{
			if (vert[i][X] > boundingBox->xmax) boundingBox->xmax = vert[i][X];
			if (vert[i][Y] > boundingBox->ymax) boundingBox->ymax = vert[i][Y];
			if (vert[i][Z] > boundingBox->zmax) boundingBox->zmax = vert[i][Z];

			if (vert[i][X] < boundingBox->xmin) boundingBox->xmin = vert[i][X];
			if (vert[i][Y] < boundingBox->ymin) boundingBox->ymin = vert[i][Y];
			if (vert[i][Z] < boundingBox->zmin) boundingBox->zmin = vert[i][Z];
		}

		return;
	}
	else
	{

		if (svertices) 
		{
			vertices = svertices;
			numberOfVerts = numberVerts;
		}
		else
		{
			vertices = NULL;
			numberOfVerts = numberVerts;
		}

		if (svertices==NULL)
		{
			GetCollisionMesh(&actorInstance->actorInstance,&vertices,NULL,matrix);
		}

		if (numberOfVerts==0) return;

		for (i=0;i<numberOfVerts;i++)
		{
			// update world box
			COLLVERTEX_GETX(&vertices[i],value);
			if (value > boundingBox->xmax) boundingBox->xmax = value;
			if (value < boundingBox->xmin) boundingBox->xmin = value;
			COLLVERTEX_GETY(&vertices[i],value);
			if (value > boundingBox->ymax) boundingBox->ymax = value;
			if (value < boundingBox->ymin) boundingBox->ymin = value;
			COLLVERTEX_GETZ(&vertices[i],value);
			if (value > boundingBox->zmax) boundingBox->zmax = value;
			if (value < boundingBox->zmin) boundingBox->zmin = value;

		}
		if (actorInstance)
		{
			localBoundingBox->xmax = boundingBox->xmax - actorInstance->actorInstance.position[X];
			localBoundingBox->xmin = boundingBox->xmin - actorInstance->actorInstance.position[X];
			localBoundingBox->ymax = boundingBox->ymax - actorInstance->actorInstance.position[Y];
			localBoundingBox->ymin = boundingBox->ymin - actorInstance->actorInstance.position[Y];
			localBoundingBox->zmax = boundingBox->zmax - actorInstance->actorInstance.position[Z];
			localBoundingBox->zmin = boundingBox->zmin - actorInstance->actorInstance.position[Z];
		}

		if (svertices==NULL) SAFE_FREE(vertices);
	}
}

/* --------------------------------------------------------------------------------
   Function : MakeEntryResident
   Purpose : If the entry is not already resident in memory this routine will load it
   Parameters : COLLISIONCACHE, COLLISIONLISTENTRY
   Returns : TRUE - loaded / FALSE - could not make resident
   Info : 
*/
BOOL MakeEntryResident(COLLISIONCACHE *collisionCache,COLLISIONLISTENTRY *collisionListEntry)
{
	TBMatrix				matrix;
	COLLISIONLISTENTRY	*oldestEntry;
	void				*temp;

	if (!collisionListEntry) return false;

	if (collisionListEntry->cacheStatus!=CACHESTATUS_DORMANT) 
	{
		collisionCache->hitsPerFrame++;

		if (((collisionListEntry->flags&CACHEFLAGS_LIVEUPDATE)&&(collisionListEntry->flags&CACHEFLAGS_NEEDSUPDATE))&&(collisionListEntry->timeSinceLastUsed!=0.0f))
		{
			collisionListEntry->flags&=~CACHEFLAGS_NEEDSUPDATE;

			// must allways update a live update object, don't free memory though
			MakeEntryDormant(collisionCache,collisionListEntry,false);
		}
		else
		{
			collisionListEntry->timeSinceLastUsed = 0.0f;
			return TRUE;
		}
	}
	else
	{
		collisionCache->missesPerFrame++;
	}

//	bkPrintf("Make entry resident %s\n",collisionListEntry->actorInstance->instanceName);

	collisionListEntry->timeSinceLastUsed = 0.0f;

	while(collisionListEntry->memoryNeeded>(collisionCache->maxMemoryAvailable-collisionCache->memoryUsed))
	{
		// not enough memory free
		oldestEntry = ReturnOldestCollisionEntry(collisionCache);
		if (oldestEntry)
		{
			if (oldestEntry->timeSinceLastUsed==0.0f)
			{
				// bad news, can only free an entry of equal importance, increase collision cache
				bkPrintf("*** WARNING *** too many collision entries in single collision incident\n");
			}
			MakeEntryDormant(collisionCache,oldestEntry);
		}
		else
		{
			// cannot free any
#if BPLATFORM==GAMECUBE
			static int onceOnly = 1;
			if (onceOnly)
			{
				bkPrintf("*** ERROR *** Could not allocate memory for collision mesh\n");
				onceOnly = 0;
			}
#else
			bkPrintf("*** ERROR *** Could not allocate memory for collision mesh\n");
#endif
			return FALSE;
		}
	}

	// get mesh
	if (collisionListEntry->flags&CACHEFLAGS_LIVEUPDATE)
	{
		StartProfileTimer(&profile,4,"Live coll");
	}

	GetCollisionMesh(&collisionListEntry->actorInstance->actorInstance,&collisionListEntry->vertices,&collisionListEntry->polys,matrix,collisionListEntry->flags&CACHEFLAGS_LIVESCALEUPDATE?true:false);

	if (collisionListEntry->vertices==NULL)
	{
		// TP: could not find or allocate memory
		MakeEntryDormant(collisionCache,collisionListEntry);
		return FALSE;
	}

	if (collisionListEntry->flags&CACHEFLAGS_LIVEUPDATE)
	{
		EndProfileTimer(&profile,4);
	}

	if (collisionListEntry->flags&CACHEFLAGS_DOUBLESIDED)
	{
		// TP: Temp work around
		temp = ZALLOC(collisionListEntry->memoryNeeded);
		ASSERT(temp);
		fmemcpy(temp,collisionListEntry->polys,collisionListEntry->numPolys*sizeof(COLLPOLY));
		SAFE_FREE(collisionListEntry->polys);
		collisionListEntry->polys = (COLLPOLY*)temp;

		MakeCollisionEntryDoubleSided(collisionListEntry);
		collisionListEntry->numPolys *= 2;
	}

	collisionListEntry->cacheStatus = CACHESTATUS_RESIDENT;
	collisionCache->memoryUsed += collisionListEntry->memoryNeeded;
	if (collisionCache->memoryUsed > collisionCache->peakMemoryUsed) collisionCache->peakMemoryUsed = collisionCache->memoryUsed;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : GetCollisionMesh
   Purpose : Spin though an actor extracting the mesh (including children)
   Parameters : root node, vertices, matrix
   Returns : new vertex pointer
   Info : 
*/
bool GetCollisionMesh(TBActorInstance *actorInstance,COLLVERTEX **vertices, COLLPOLY **polys, TBMatrix matrix, bool scale)
{
	int					noofTriangles;
	int					bufSize;

	if ((actorInstance->scale[0]>1.1f)||(actorInstance->scale[1]>1.1f)||(actorInstance->scale[2]>1.1f))
	{
		scale = TRUE;
	}
	if ((actorInstance->scale[0]<0.9f)||(actorInstance->scale[1]<0.9f)||(actorInstance->scale[2]<0.9f))
	{
		scale = TRUE;
	}

	baPrepActorInstance(actorInstance,TRUE);

	*vertices = (COLLVERTEX*)baGetActorGeometry(actorInstance,NULL,0,NULL,&noofTriangles,&bufSize,*vertices,BGETACTGEOMETRY_WORLDSPACE);
	if (polys)
	{
		*polys = (COLLPOLY*)baGetActorGeometry(actorInstance,NULL,1,NULL,&noofTriangles,&bufSize,*polys,BGETACTGEOMETRY_WORLDSPACE);
		if ((*vertices == NULL)||(*polys == NULL)||(noofTriangles == 0))
		{
			bkPrintf("*** ERROR *** No collision information found...\n");
			return false;
		}
	}
	else
	{
		if ((*vertices == NULL)||(noofTriangles == 0))
		{
			bkPrintf("*** ERROR *** No collision information found...\n");
			return false;
		}
	}

	if (*polys)	FillInMinMaxAndNormal(*vertices,*polys,noofTriangles,scale);

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FillInMinMaxAndNormal
	Purpose		: Calcs the min max values and normal if needed and stores them in the COLLVERTEXOLD structure
	Parameters	: vertices, noofTriangles, true/false to recalulate normals
	Returns		: 
	Info		: 
*/
void FillInMinMaxAndNormal(COLLVERTEX *vertices, COLLPOLY *polys, int noofTriangles, bool recalcNorms)
{
	int			i;
#ifdef USE_SHORTS_FOR_COLLISION
	TBVector	point1,point2,point3,tempVector;
	short		min[3],max[3];
#else
	TBVector	point1,point2,point3,min,max,tempVector;
#endif


	for (i=0;i<noofTriangles;i++)
	{
		COLLPOLY_GETV1(polys[i],vertices,point1[X],point1[Y],point1[Z],point1[W]);
		COLLPOLY_GETV2(polys[i],vertices,point2[X],point2[Y],point2[Z],point2[W]);
		COLLPOLY_GETV3(polys[i],vertices,point3[X],point3[Y],point3[Z],point3[W]);
		FindMinMaxFromPoly(min,max,point1,point2,point3);
		// IH: Make the boxes a little larger than they need to be
#ifdef USE_SHORTS_FOR_COLLISION
		COLLPOLY_SETMIN(polys[i],min[X]-1,min[Y]-1,min[Z]-1);
		COLLPOLY_SETMAX(polys[i],max[X]+1,max[Y]+1,max[Z]+1);
#else
		COLLPOLY_SETMIN(polys[i],min[X],min[Y],min[Z]);
		COLLPOLY_SETMAX(polys[i],max[X],max[Y],max[Z]);
#endif
		if (recalcNorms==TRUE)
		{
			bmVectorCalcNormal(tempVector,point1,point2,point3);
			COLLPOLY_SETNORMAL(polys[i],tempVector[X],tempVector[Y],tempVector[Z]);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ConvertCollisionVertsByMatrix
   Purpose : Converts and set of vertices by a matrix and creates extra coll info
   Parameters : vertices, matrix, number of vertices
   Returns : 
   Info : 
*/
/*
void ConvertCollisionVertsByMatrix(COLLVERTEXOLD *vertices,TBMatrix matrix,int number)
{
	int			i;
	TBVector	tempVector;
	COLLVERTEXOLD	*vertPtr;
	TBVector	min,max;
	TBVector	point1,point2,point3;

	// calculate normals
	vertPtr = vertices;

	for (i=0;i<number;i+=3)
	{
		// dont like this bit
		COLLVERTEXOLD_GETXYZW(&vertices[i],point1[X],point1[Y],point1[Z],point1[W]);
		bmMatMultiplyVector(matrix,point1);
		COLLVERTEXOLD_SETXYZW(&vertices[i],point1[X],point1[Y],point1[Z],point1[W]);

		COLLVERTEXOLD_GETXYZW(&vertices[i+1],point2[X],point2[Y],point2[Z],point2[W]);
		bmMatMultiplyVector(matrix,point2);
		COLLVERTEXOLD_SETXYZW(&vertices[i+1],point2[X],point2[Y],point2[Z],point2[W]);

		COLLVERTEXOLD_GETXYZW(&vertices[i+2],point3[X],point3[Y],point3[Z],point3[W]);
		bmMatMultiplyVector(matrix,point3);
		COLLVERTEXOLD_SETXYZW(&vertices[i+2],point3[X],point3[Y],point3[Z],point3[W]);

		bmVectorCalcNormal(tempVector,point1,point2,point3);
		COLLVERTEXOLD_SETNORMAL(vertPtr,tempVector[X],tempVector[Y],tempVector[Z]);

		// store min and max values in the second and third vertex normals
		FindMinMaxFromPoly(min,max,point1,point2,point3);
		COLLVERTEXOLD_SETNORMAL(&vertPtr[1],min[X],min[Y],min[Z]);
		COLLVERTEXOLD_SETNORMAL(&vertPtr[2],max[X],max[Y],max[Z]);
		vertPtr+=3;
	}
}
*/

/* --------------------------------------------------------------------------------
   Function : FindMinMaxFromPoly
   Purpose : Find the min and max coords from a polygon
   Parameters : min, max, point1, point1, point1
   Returns : 
   Info : This is for when shorts are being used for collision
*/
#ifdef USE_SHORTS_FOR_COLLISION
void FindMinMaxFromPoly(short min[3], short max[3], TBVector point1, TBVector point2, TBVector point3)
{
	short	sPoint1[3], sPoint2[3], sPoint3[3];

	// IH: Half all of the values and add one (or take one away if they are -ve)
	// IH: Shrink them now - it is easier

	// IH: TEMPORARY: TEMPORARY: TEMPORARY!
	for( int i = 0; i < 3; i++ )
	{
		if( point1[i] > 240000.0f || point1[i] < -240000.0f ||
			point2[i] > 240000.0f || point2[i] < -240000.0f ||
			point3[i] > 240000.0f || point3[i] < -240000.0f )
		{
			bkPrintf( "*********** POSSIBLE COLLSION OVERFLOW ***************\n" );
			bkPrintf( "p1x: %f, p1y: %f, p1z: %f\n", point1[X], point1[Y], point1[Z] );
			bkPrintf( "p2x: %f, p2y: %f, p2z: %f\n", point2[X], point2[Y], point2[Z] );
			bkPrintf( "p3x: %f, p3y: %f, p3z: %f\n", point3[X], point3[Y], point3[Z] );
		}
	}

	// IH: Point1
	sPoint1[0] = ((short)point1[0])>>3;
	sPoint1[1] = ((short)point1[1])>>3;
	sPoint1[2] = ((short)point1[2])>>3;

	// IH: Point2
	sPoint2[0] = ((short)point2[0])>>3;
	sPoint2[1] = ((short)point2[1])>>3;
	sPoint2[2] = ((short)point2[2])>>3;
	
	// IH: Point3
	sPoint3[0] = ((short)point3[0])>>3;
	sPoint3[1] = ((short)point3[1])>>3;
	sPoint3[2] = ((short)point3[2])>>3;

	min[0] = sPoint1[0];
	max[0] = sPoint1[0];
	min[1] = sPoint1[1];
	max[1] = sPoint1[1];
	min[2] = sPoint1[2];
	max[2] = sPoint1[2];

	if ( max[0] < sPoint2[0] )
		max[0] = sPoint2[0];
	if ( min[0] > sPoint2[0] )
		min[0] = sPoint2[0];
	if ( max[1] < sPoint2[1] )
		max[1] = sPoint2[1];
	if ( min[1] > sPoint2[1] )
		min[1] = sPoint2[1];
	if ( max[2] < sPoint2[2] )
		max[2] = sPoint2[2];
	if ( min[2] > sPoint2[2] )
		min[2] = sPoint2[2];

	if ( max[0] < sPoint3[0] )
		max[0] = sPoint3[0];
	if ( min[0] > sPoint3[0] )
		min[0] = sPoint3[0];
	if ( max[1] < sPoint3[1] )
		max[1] = sPoint3[1];
	if ( min[1] > sPoint3[1] )
		min[1] = sPoint3[1];
	if ( max[2] < sPoint3[2] )
		max[2] = sPoint3[2];
	if ( min[2] > sPoint3[2] )
		min[2] = sPoint3[2];
}
#endif


/* --------------------------------------------------------------------------------
   Function : FindMinMaxFromPoly
   Purpose : Find the min and max coords from a polygon
   Parameters : min, max, point1, point1, point1
   Returns : 
   Info : 
*/
#ifndef USE_SHORTS_FOR_COLLISION
void FindMinMaxFromPoly(TBVector min, TBVector max, TBVector point1, TBVector point2, TBVector point3)
{
	min[0] = point1[0];
	max[0] = point1[0];
	min[1] = point1[1];
	max[1] = point1[1];
	min[2] = point1[2];
	max[2] = point1[2];

	if ( max[0] < point2[0] )
		max[0] = point2[0];
	if ( min[0] > point2[0] )
		min[0] = point2[0];
	if ( max[1] < point2[1] )
		max[1] = point2[1];
	if ( min[1] > point2[1] )
		min[1] = point2[1];
	if ( max[2] < point2[2] )
		max[2] = point2[2];
	if ( min[2] > point2[2] )
		min[2] = point2[2];

	if ( max[0] < point3[0] )
		max[0] = point3[0];
	if ( min[0] > point3[0] )
		min[0] = point3[0];
	if ( max[1] < point3[1] )
		max[1] = point3[1];
	if ( min[1] > point3[1] )
		min[1] = point3[1];
	if ( max[2] < point3[2] )
		max[2] = point3[2];
	if ( min[2] > point3[2] )
		min[2] = point3[2];
}
#endif

/* --------------------------------------------------------------------------------
   Function : MakeEntryDormant
   Purpose : Remove a collision entry from memory
   Parameters : COLLISIONCACHE, COLLISIONLISTENTRY
   Returns : 
   Info : 
*/
void MakeEntryDormant(COLLISIONCACHE *collisionCache,COLLISIONLISTENTRY *collisionListEntry,bool freeMem)
{
	if (!collisionListEntry) return;

	if (collisionListEntry->cacheStatus == CACHESTATUS_DORMANT) return;

	if (collisionListEntry->flags&CACHEFLAGS_DOUBLESIDED) collisionListEntry->numPolys /=2;
	collisionCache->memoryUsed -= collisionListEntry->memoryNeeded;
	collisionListEntry->cacheStatus = CACHESTATUS_DORMANT;
	if (freeMem) 
	{
		if (~collisionListEntry->flags&CACHEFLAGS_DONTFREEVERTS)
		{
			SAFE_FREE(collisionListEntry->vertices);
		}
		SAFE_FREE(collisionListEntry->polys);
	}
}

/* --------------------------------------------------------------------------------
   Function : ReturnOldestCollisionEntry
   Purpose : pick out the least used collision entry
   Parameters : COLLISIONCACHE
   Returns : COLLISIONLISTENTRY
   Info : 
*/
COLLISIONLISTENTRY *ReturnOldestCollisionEntry(COLLISIONCACHE *collisionCache)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLISTENTRY *oldestEntry=NULL;
	float				oldest=-1;

	for (ptr = collisionCache->collisionList.head.next;ptr != &collisionCache->collisionList.head;ptr = ptr->next)
	{
		if (ptr->cacheStatus!=CACHESTATUS_RESIDENT) continue;
		// if stay resident then must not be free'd
		if (ptr->flags&CACHEFLAGS_STAYRESIDENT) continue;
		if (ptr->timeSinceLastUsed > oldest)
		{
			oldest = ptr->timeSinceLastUsed;
			oldestEntry = ptr;
		}
	}
	return oldestEntry;
}

/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInMesh
   Purpose : Return the number of verts in a mesh
   Parameters : TBSegmentedMesh
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInMesh(TBActorInstance *actorInstance)
{
	int					numberOfTris;

	baGetActorGeometry(actorInstance,actorInstance->rootNodeInstance,0,NULL,&numberOfTris,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);
	
	return numberOfTris*3;
}

/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInActor
   Purpose : Return the number of verts in an actor
   Parameters : TBActorInstance
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInActor(TBActorInstance *actorInstance)
{
	int					numberOfVerts;

	baGetActorGeometry(actorInstance,actorInstance->rootNodeInstance,0,&numberOfVerts,NULL,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);

	return numberOfVerts;
}

/* --------------------------------------------------------------------------------
   Function : NumberOfTrisInActor
   Purpose : Return the number of verts in an actor
   Parameters : TBActorInstance
   Returns : number of verts
   Info : 
*/
int NumberOfTrisInActor(TBActorInstance *actorInstance)
{
	int					numberOfTris;

	baGetActorGeometry(actorInstance,actorInstance->rootNodeInstance,0,NULL,&numberOfTris,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);

	return numberOfTris;
}

/* --------------------------------------------------------------------------------
   Function : NumberOfVertsInSet
   Purpose : Return the number of verts in an actor
   Parameters : TBSet
   Returns : number of verts
   Info : 
*/
int NumberOfVertsInSet(TBSet *set)
{
	int					numberOfVerts;

	baGetSetGeometry(set,0,&numberOfVerts,NULL);

	return numberOfVerts;
}

/* --------------------------------------------------------------------------------
   Function : NumberOfTrisInSet
   Purpose : Return the number of tris in an actor
   Parameters : TBSet
   Returns : number of verts
   Info : 
*/
int NumberOfTrisInSet(TBSet *set)
{
	int					numberOfTris;

	baGetSetGeometry(set,0,NULL,&numberOfTris);

	return numberOfTris;
}

/* --------------------------------------------------------------------------------
   Function : TestBoxCollision
   Purpose : test for collision between two boxes
   Parameters : BOUNDINGBOX, BOUNDINGBOX
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestBoxCollision(BOUNDINGBOX *box1,BOUNDINGBOX *box2)
{
	if (box1->xmax < box2->xmin) return FALSE;
	if (box1->xmin > box2->xmax) return FALSE;
	if (box1->zmax < box2->zmin) return FALSE;
	if (box1->zmin > box2->zmax) return FALSE;
	if (box1->ymax < box2->ymin) return FALSE;
	if (box1->ymin > box2->ymax) return FALSE;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TestBoxPointCollision
   Purpose : test for collision between two boxes
   Parameters : BOUNDINGBOX, TBVector
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestPointBoxCollision(BOUNDINGBOX *box1,TBVector point)
{
	if (box1->xmax < point[X]) return FALSE;
	if (box1->xmin > point[X]) return FALSE;
	if (box1->zmax < point[Z]) return FALSE;
	if (box1->zmin > point[Z]) return FALSE;
	if (box1->ymax < point[Y]) return FALSE;
	if (box1->ymin > point[Y]) return FALSE;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TestBoxCollisionXZ
   Purpose : test for collision between two boxes in X and Z directions only
   Parameters : BOUNDINGBOX, BOUNDINGBOX
   Returns : TRUE/FALSE
   Info : 
*/
BOOL TestBoxCollisionXZ(BOUNDINGBOX *box1,BOUNDINGBOX *box2)
{
	if (box1->xmax < box2->xmin) return FALSE;
	if (box1->xmin > box2->xmax) return FALSE;
	if (box1->zmax < box2->zmin) return FALSE;
	if (box1->zmin > box2->zmax) return FALSE;
	return TRUE;
}
/* --------------------------------------------------------------------------------
   Function : IsPointWithinBox
   Purpose : Is point within box
   Parameters : ACTORINSTANCE, point
   Returns : TRUE/FALSE
   Info : 
*/
int	IsPointWithinBox(ACTORINSTANCE *actorInstance, TBVector point)
{
	TBMatrix	matrix;
	TBVector	tempVector;
	BOUNDINGBOX	box;

	if (GetBoxBoundsAndMatrix(actorInstance,&box,matrix) == FALSE) return FALSE;

	bmVectorCopy(tempVector,point);
	tempVector[W] = 1.0f;
	bmMatMultiplyVector(matrix,tempVector);

	if (tempVector[X] > box.xmax) return FALSE;
	if (tempVector[X] < box.xmin) return FALSE;
	if (tempVector[Z] > box.zmax) return FALSE;
	if (tempVector[Z] < box.zmin) return FALSE;
	if (tempVector[Y] > box.ymax) return FALSE;
	if (tempVector[Y] < box.ymin) return FALSE;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : IsPointWithinBox
   Purpose : Is point within box
   Parameters : ACTORINSTANCE, point
   Returns : TRUE/FALSE
   Info : 
*/
int	IsPointWithinBox(const BOUNDINGBOX *box, const TBMatrix matrix, const TBVector point,int flags)
{
	TBVector	tempVector;

//	if (GetBoxBoundsAndMatrix(actorInstance,&box,matrix) == FALSE) return FALSE;

	bmVectorCopy(tempVector,point);
	tempVector[W] = 1.0f;
	bmMatMultiplyVector(matrix,tempVector);

	if (flags&BOXFLAG_XTEST)
	{
		if (tempVector[X] > box->xmax) return FALSE;
		if (tempVector[X] < box->xmin) return FALSE;
	}
	if (flags&BOXFLAG_ZTEST)
	{
		if (tempVector[Z] > box->zmax) return FALSE;
		if (tempVector[Z] < box->zmin) return FALSE;
	}
	if (flags&BOXFLAG_YTEST)
	{
		if (tempVector[Y] > box->ymax) return FALSE;
		if (tempVector[Y] < box->ymin) return FALSE;
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : GetBoxBoundsAndMatrix
   Purpose : Return the axis aligned bounding box and inverted matrix for test
   Parameters : ACTORINSTANCE, BOUNDINGBOX, matrix
   Returns : TRUE/FALSE
   Info : Returns a axis aligned bounding box with matrix for testing point in box
*/
int GetBoxBoundsAndMatrix(ACTORINSTANCE *actorInstance, BOUNDINGBOX *box, TBMatrix matrix)
{
	int			numberOfVerts,numberOfTris;
	int			res;

	actorInstance->actorInstance.scale[W] = 1.0f;
	actorInstance->actorInstance.position[W] = 1.0f;

	baPrepActorInstance(&actorInstance->actorInstance,TRUE);

	/*
	baGetActorGeometry(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,0,&numberOfVerts,&numberOfTris,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);
	
	if (numberOfVerts != 4) 
	{
		return FALSE;
	}*/
	if (matrix)
	{
		res = bmMatInverse(matrix,actorInstance->actorInstance.objectToWorld);
		box->xmax = actorInstance->actorInstance.actor->xMax;
		box->xmin = actorInstance->actorInstance.actor->xMin;
		box->ymax = actorInstance->actorInstance.actor->yMax;
		box->ymin = actorInstance->actorInstance.actor->yMin;
		box->zmax = actorInstance->actorInstance.actor->zMax;
		box->zmin = actorInstance->actorInstance.actor->zMin;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : GetMinMaxFromVerts
   Purpose : Return a bounding box from verts
   Parameters : BOUNDINGBOX, COLLVERTEXOLD, numberOfVerts
   Returns : 
   Info : 
*/
void GetMinMaxFromVerts(BOUNDINGBOX *box, COLLVERTEX *verts, int numberOfVerts)
{
	int i;

	box->xmax = box->xmin = verts[0].x;
	box->ymax = box->ymin = verts[0].y;
	box->zmax = box->zmin = verts[0].z;

	for (i=0;i<numberOfVerts;i++)
	{
		if (verts[i].x > box->xmax) box->xmax = verts[i].x;
		if (verts[i].x < box->xmin) box->xmin = verts[i].x;
		if (verts[i].y > box->ymax) box->ymax = verts[i].y;
		if (verts[i].y < box->ymin) box->ymin = verts[i].y;
		if (verts[i].z > box->zmax) box->zmax = verts[i].z;
		if (verts[i].z < box->zmin) box->zmin = verts[i].z;
	}
}

/* --------------------------------------------------------------------------------
   Function : RefreshCollisionEntry
   Purpose : Refresh current collision for actor to take account of animation changes
   Parameters : ACTORINSTANCE *actorInstance,
   Returns : 
   Info : 
*/
void RefreshCollisionEntry(COLLISIONCACHE *collisionCache,ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLIST *collisionList = &collisionCache->collisionList;

	baPrepActorInstance(&actorInstance->actorInstance,TRUE);

	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			if (ptr->cacheStatus==CACHESTATUS_RESIDENT)
			{
				MakeEntryDormant(collisionCache,ptr);
				break;
			}
		}
	}

	UpdateCollisionBoundingBox(ptr);
}

/*  --------------------------------------------------------------------------------
	Function	: GetCollisionEntry
	Purpose		: Get a pointer to the collision list entry for an actor
	Parameters	: COLLISIONCACHE, ACTORINSTANCE to find collision for
	Returns		: COLLISIONLISTENTRY*
	Info		: 
*/
COLLISIONLISTENTRY *GetCollisionEntry(COLLISIONCACHE *collisionCache, ACTORINSTANCE *actorInstance)
{
	COLLISIONLISTENTRY *ptr;
	COLLISIONLIST *collisionList = &collisionCache->collisionList;
	
	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			return ptr;
		}
	}
	return NULL;
}

/*  --------------------------------------------------------------------------------
	Function	: GetCollSphereDetails
	Purpose		: Find out the current bounding radius and centre pos of an ACTORINSTANCE
	Parameters	: ACTORINSTANCE, ptr to float to receive radius,
					(opt/NULL=don't care) TBVector to receive centre pos
	Returns		: true if the actor was found in the collision cache, false if not
	Info		: // PP: ***** UNTESTED *****
*/
bool GetCollSphereDetails(struct TAG_ACTORINSTANCE* actorInstance, float * const radiusOut, TBVector centrePos)
{
	COLLISIONLISTENTRY		*collEntry;

	// PP: find the actor instance in the collision cache
	collEntry=GetCollisionEntry(&collisionCache, actorInstance);

	if(collEntry==NULL)// PP: if we didn't find it...
	{
		return false;
	}
	else// PP: if we did find it...
	{
		TBVector	min, max;
		float		radius;

		// PP: fill in radius parameter...

		SETVECTOR(min, collEntry->boundingBox.xmin, collEntry->boundingBox.ymin, collEntry->boundingBox.zmin, 0.0f);
		SETVECTOR(max, collEntry->boundingBox.xmax, collEntry->boundingBox.ymax, collEntry->boundingBox.zmax, 0.0f);

		radius=bmFAbs(bmVectorDistance(min, max))/2.0f;

		*radiusOut=radius;

		// PP: fill in centre pos parameter if it's not NULL...

		if(centrePos != NULL)
		{
			SETVECTOR(centrePos,
						min[X]+((max[X]-min[X])/2.0f),
						min[Y]+((max[Y]-min[Y])/2.0f),
						min[Z]+((max[Z]-min[Z])/2.0f),
						0.f);
		}

		return true;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													BOUNDINGBOX methods

											Cuboid bounding boxes, very handy.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::TAG_BOUNDINGBOX
	Purpose 	: default constructor: calls init
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: 
*/
BOUNDINGBOX::TAG_BOUNDINGBOX(void)
{
	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::init
	Purpose 	: initialise the bounding box
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: sets extents to values such that the box will need to reshape to contain any point you give it
*/
void BOUNDINGBOX::init(void)
{
#define BB_INIT(_axis) \
	this->_axis##min=FLOATMAX; \
	this->_axis##max=/*FLOATMIN*/-FLOATMAX;		// PP: oops: FLOATMIN is the minimum POSITIVE float!  D'oh!

	BB_INIT(x);
	BB_INIT(y);
	BB_INIT(z);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::minDist
	Purpose 	: get the minimum distance between the bounding box and a point
	Parameters 	: 
	Returns 	: the minimum distance between the bounding box and the point!
	Info 		: // PP: 
*/
float BOUNDINGBOX::minDist(const VEC& point) const
{
	VEC		distVec;

#define BB_MINDIST(_AXIS, _axis) \
		distVec[_AXIS]=(this->_axis##min-point[_AXIS]); \
		if(distVec[_AXIS]<0.0f) \
		{ \
			/* PP: point[X] > this->xmin */ \
			distVec[_AXIS]=(point[_AXIS]-this->_axis##max); \
			\
			if(distVec[_AXIS]<0.0f) \
			{ \
				/* PP: point[X] < this->xmax, so it's within box bounds */ \
				distVec[_AXIS]=0.0f; \
			} \
		}

	BB_MINDIST(X,x);
	BB_MINDIST(Y,y);
	BB_MINDIST(Z,z);

	return distVec.magnitude();
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::expandToContain
	Purpose 	: reshape as neccessary to contain a specified position
	Parameters 	: TBVector position to contain
	Returns 	: true if the bounding box changed, else false
	Info 		: // PP: ---- LH Removed inline for gamecube (put in header if you want this back in)
*/
bool BOUNDINGBOX::expandToContain(const TBVector pos)
{
	bool	changed=false;

#define BB_EXPANDTOCONTAIN(_AXIS, _axis) \
	if(pos[_AXIS]>this->_axis##max) \
	{ \
		this->_axis##max=pos[_AXIS]; \
		changed=true; \
	} \
	\
	if(pos[_AXIS]<this->_axis##min) \
	{ \
		this->_axis##min=pos[_AXIS]; \
		changed=true; \
	}

	BB_EXPANDTOCONTAIN(X, x);
	BB_EXPANDTOCONTAIN(Y, y);
	BB_EXPANDTOCONTAIN(Z, z);

	return changed;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::expandToContain
	Purpose 	: reshape as neccessary to contain a specified position
	Parameters 	: VEC position to contain
	Returns 	: true if the bounding box changed, else false
	Info 		: // PP: 
*/
bool BOUNDINGBOX::expandToContain(const VEC& pos)
{
	return this->expandToContain(pos.v);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::draw
	Purpose 	: draw the bounding box
	Parameters 	: colour in which to draw the bounding box
	Returns 	: 
	Info 		: // PP: 
*/
void BOUNDINGBOX::draw(const COLOUR& col) const
{
	utilDrawBoundingBox(*this, col);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOUNDINGBOX::operator =
	Purpose 	: convert a TBCollisionAABB into the BOUNDINGBOX
	Parameters 	: ref to TBCollisionAABB to convert
	Returns 	: BOUNDINGBOX representing the TBCollisionAABB
	Info 		: 
*/
struct TAG_BOUNDINGBOX& BOUNDINGBOX::operator = (const TBCollisionAABB& box)
{
	this->xmin=box.minBox[X];
	this->ymin=box.minBox[Y];
	this->zmin=box.minBox[Z];

	this->xmax=box.maxBox[X];
	this->ymax=box.maxBox[Y];
	this->zmax=box.maxBox[Z];

	return *this;
}