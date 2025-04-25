 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : attach.cpp
//   Purpose : provides functions to allow objects to be attached to characters
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "characters.h"
#include "maths.h"
#include "MonitorFX.h"					// PP: for the watch display
#include "Cheats.h"						// PP: Cheats


/* --------------------------------------------------------------------------------
   Function : InitAttachedInfo
   Purpose : Initialise the attached info structure
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void InitAttachedInfo(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			attachInfo->objects[i] = NULL;
			attachInfo->flags[i] = 0;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DestroyAttachedInfo
   Purpose : Remove all attached objects and resets the structure
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void DestroyAttachedInfo(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			RemoveObjectFromCharacter(actorInstance,i);
		}
	}

	// PP: if we're wearing the watch, that'll have to go - don't bother checking, just switch it off
	MonitorFX_watch.shutdown();
}

/* --------------------------------------------------------------------------------
   Function : AttachObjectToCharacter
   Purpose : Create an actor instance and attach to a characters bone with offsets
   Parameters : actor, object to attach, bone to attach to, pos offset, scale, rot, flags
   Returns : handle to attached object, -1 for fail
   Info : 
*/
int AttachObjectToCharacter(ACTORINSTANCE *actorInstance, char *object, char *bone, TBVector pos, TBVector scale, TBQuaternion quat, int flags)
{
	TBActor				*objectToAttach;
	TBActorNodeInstance *node;
	ATTACH				*attach;
	int					i;

	if (!actorInstance->characterInfo) 
	{
		bkPrintf("*** WARNING *** Attempt to attack object to actor without characterInfo\n");
		return (-1);
	}

	attach = &actorInstance->characterInfo->attachedObjects;
	
	// find object
	objectToAttach = FindActorInActorList(object);
	if (!objectToAttach) return -1;

	// find bone in character
	node = baFindNode(actorInstance->actorInstance.rootNodeInstance,bone);
	if (!node) return -1;

	// create object
	for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
	{
		if (attach->objects[i]==NULL)
		{
			attach->objects[i] = (TBActorInstance*)ZALLOC(sizeof(TBActorInstance));
			if (attach->objects[i] == NULL) return (-1);

			baInitActorInstance(attach->objects[i], objectToAttach, 1);
			if (pos)
			{
				bmVectorCopy(attach->objects[i]->position, pos);
			}
			else
			{
				SETVECTOR(attach->objects[i]->position,0.0f,0.0f,0.0f,1.0f);
			}
			if (scale)
			{
				bmVectorCopy(attach->objects[i]->scale, scale);
			}
			else
			{
				SETVECTOR(attach->objects[i]->scale,1.0f,1.0f,1.0f,1.0f);
			}
			if (quat)
			{
				bmQuatCopy(attach->objects[i]->orientation, quat);
			}
			else
			{
				bmQuatCopy(attach->objects[i]->orientation,bIdentityQuaternion);
			}

			attach->node[i] = node;
			
			baPrepActorInstance(attach->objects[i],TRUE);
			if (!(flags & ATTACHFLAG_NOBLACKOUTLINE))
			{
				EnableActorCartoonRenderMode(attach->objects[i]);
			}
			attach->alpha[i] = 256;
			attach->flags[i] = flags;
			return i;
		}
	}

	return (-1);
}

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	SetAttachedObjectAlpha(ACTORINSTANCE *actorInstance, int handle, float alpha)
{
	ATTACH	*attachInfo;

	if(actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		if(attachInfo->objects[handle])
		{
			attachInfo->alpha[handle] = alpha;
			if(alpha == 256)
			{
				attachInfo->flags[handle] &= ~ATTACHFLAG_PRIMTYPEALPHA;
			}
			else
			{
				attachInfo->flags[handle] |= ATTACHFLAG_PRIMTYPEALPHA;
			}
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	AddAttachedObjectFlags(ACTORINSTANCE *actorInstance, int handle, uint32	flag)
{
	ATTACH	*attachInfo;

	if(actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		if(attachInfo->objects[handle])
		{
			attachInfo->flags[handle] |= flag;
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : SetAttachedObjectAlpha
   Purpose : sets the alpha on an attached object
   Parameters : 
   Returns : 
   Info : 
*/

int	RemoveAttachedObjectFlags(ACTORINSTANCE *actorInstance, int handle, uint32	flag)
{
	ATTACH	*attachInfo;

	if(actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		if(attachInfo->objects[handle])
		{
			attachInfo->flags[handle] &= ~flag;
			return TRUE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromCharacter
   Purpose : Remove an object from the character using the handle returned from attach
   Parameters : actor, handle
   Returns : 
   Info : 
*/
void RemoveObjectFromCharacter(ACTORINSTANCE *actorInstance, int handle)
{
	ATTACH *attachInfo;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		if (attachInfo->objects[handle])
		{
			baFreeActorInstance(attachInfo->objects[handle]);
			FREE(attachInfo->objects[handle]);
			attachInfo->objects[handle] = NULL;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepAttachedObjects
   Purpose : Prepare the attached objects for drawing
   Parameters : actor
   Returns : 
   Info : 
*/
void PrepAttachedObjects(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;
	TBMatrix matrix,matrix2,matrix3;


	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i])
			{
				if (attachInfo->flags[i]&ATTACHFLAG_INHERITSCALE)
				{
					baPrepActorInstance(attachInfo->objects[i],TRUE);

					bmMatMultiply(matrix,attachInfo->node[i]->nodeToLocalWorld,attachInfo->objects[i]->objectToWorld);
					bmMatMultiply(attachInfo->objects[i]->objectToWorld,
						actorInstance->actorInstance.objectToWorld,matrix);
					baGetNodesWorldPosition(attachInfo->objects[i], attachInfo->objects[i]->rootNodeInstance, attachInfo->actualWorldPosition[i]);
				}
				else
				{
					baPrepActorInstance(attachInfo->objects[i],TRUE);

					bmMatScale(matrix2,1.0f/attachInfo->node[i]->animState.scale[X],1.0f/attachInfo->node[i]->animState.scale[Y],1.0f/attachInfo->node[i]->animState.scale[Z]);

					// node to local world, inheritedXForm
					bmMatMultiply(matrix3,attachInfo->node[i]->nodeToLocalWorld,matrix2);

					bmMatMultiply(matrix,matrix3,attachInfo->objects[i]->objectToWorld);
					bmMatMultiply(attachInfo->objects[i]->objectToWorld,
						actorInstance->actorInstance.objectToWorld,matrix);
					baGetNodesWorldPosition(attachInfo->objects[i], attachInfo->objects[i]->rootNodeInstance, attachInfo->actualWorldPosition[i]);
				}
			}
		}
	}
}


/* --------------------------------------------------------------------------------
   Function		: DrawGothMask
   Purpose		: Draws a goth mask on an ACTORINSTANCE
   Parameters	: ptr to ACTORINSTANCE
   Returns		: 
   Info			: // PP: actorInstance must have a "snoutstem" bone
					This is only designed for Taz/She-Devil
*/

#define GOTHMASK_Z_OFFSET		(-40.0f)	// PP: z offset of mask
#define GOTHMASK_THICKNESS		(-0.2f)		// PP: need this on PS2 for some reason - half the distance between front & back layers
#define GOTHMASK_WIDTH			130.0f		// PP: width of mask
#define GOTHMASK_TILT			0.7f		// PP: tilt of mask

static void DrawGothMask(ACTORINSTANCE* const actorInstance)
{
	static TBPrimVertex		vtx[4];
	VEC						headPos;
	TBActorNodeInstance*	headBone;
	TBMatrix				rotMat;
	TBMatrix				tempMat;
	TBMatrix				finalMat;
	int						i;

	headBone=baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem");

	if(headBone != NULL)
	{
		baGetNodesWorldPosition(&(actorInstance->actorInstance), headBone, headPos.v);

		bmMatXRotation(rotMat, GOTHMASK_TILT);

		bmMatMultiply(tempMat, headBone->nodeToLocalWorld, rotMat);

		bmMatMultiply(finalMat, actorInstance->actorInstance.objectToWorld, tempMat);
		
		bdSetObjectMatrix(finalMat);

		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, BDCULLMODE_CCW, 0);// PP: seems to have no effect on PS2

		// PP: draw back of goth mask...

		bdSetTexture(0, QwilsonBackTexture);

		for(i=0; i<4; i++)
		{
			BDVERTEX_SETUV(&vtx[i], (i&1)?1.0f:0.0f, (i/2)?1.0f:0.0f);
			BDVERTEX_SETXYZW(&vtx[i], (i&1)?-GOTHMASK_WIDTH:GOTHMASK_WIDTH, (i/2)?(-GOTHMASK_WIDTH*0.5f):(GOTHMASK_WIDTH*0.5f), GOTHMASK_Z_OFFSET-GOTHMASK_THICKNESS, 1.0f);
			BDVERTEX_SETRGBA(&vtx[i], 128, 128, 128, 128);
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vtx,4,BVERTTYPE_SINGLE);

		// PP: draw front of goth mask...
		
		bdSetTexture(0, QwilsonTexture);

		for(i=0; i<4; i++)
		{
			BDVERTEX_SETUV(&vtx[i], (i&1)?0.0f:1.0f, (i/2)?1.0f:0.0f);
			BDVERTEX_SETXYZW(&vtx[i], (i&1)?GOTHMASK_WIDTH:-GOTHMASK_WIDTH, (i/2)?(-GOTHMASK_WIDTH*0.5f):(GOTHMASK_WIDTH*0.5f), GOTHMASK_Z_OFFSET+GOTHMASK_THICKNESS, 1.0f);
			BDVERTEX_SETRGBA(&vtx[i], 255, 255, 255, 128);
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vtx,4,BVERTTYPE_SINGLE);

		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, BDCULLMODE_OFF, 0);
	}
}


/* --------------------------------------------------------------------------------
   Function : DrawAttachedObjects
   Purpose : Draw all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void DrawAttachedObjects(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;
	TBSavedRenderStates  saveState;

	if (actorInstance->flags & ACTORFLAG_DONTDRAW)
	{
		// TP: Dont draw objects attached to instance with dontdraw flag on
		return;
	}

	// PP: if CHEAT_QWILSON is active, surely Taz should be wearing a Goth mask of some description...?
	if(CheatActive(CHEAT_QWILSON))
	{
		if((actorInstance == gameStatus.player1)||(actorInstance == gameStatus.player2))
		{
			DrawGothMask(actorInstance);
		}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	baSetGlobalAlpha(256);


	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i])
			{
				if(attachInfo->flags[i] & ATTACHFLAG_PRIMTYPEALPHA) continue;
				baDrawActorInstance(attachInfo->objects[i],BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
			}
		}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);


}

/* --------------------------------------------------------------------------------
   Function : DrawAttachedObjects
   Purpose : Draw all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void DrawAttachedObjects2(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;

	if (actorInstance->flags & ACTORFLAG_DONTDRAW)
	{
		// TP: Dont draw objects attached to instance with dontdraw flag on
		return;
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,0);

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i])
			{
				if(attachInfo->flags[i] & ATTACHFLAG_PRIMTYPEALPHA)
				{
					baSetGlobalAlpha((int32)attachInfo->alpha[i]);
					baDrawActorInstance(attachInfo->objects[i],BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
				}
			}
		}
	}

	baSetGlobalAlpha(256);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : UpdateAttachedObjects
   Purpose : Update all objects attached to an actor
   Parameters : actor
   Returns : 
   Info : 
*/
void UpdateAttachedObjects(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int i;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i])
			{
				baUpdateNodes(attachInfo->objects[i], attachInfo->objects[i]->rootNodeInstance, speedRatio, NULL);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ReturnAttachedObjectInstance
   Purpose : Returns a pointer to the attached objects actorInstance;
   Parameters : ACTORINSTANCE, handle
   Returns : TBActorInstance *
   Info : 
*/
TBActorInstance *ReturnAttachedObjectInstance(ACTORINSTANCE *actorInstance, int handle)
{
	ATTACH *attachInfo;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		return attachInfo->objects[handle];
	}

	return NULL;
}


/* --------------------------------------------------------------------------------
   Function		: FindAttachedObjectInstance
   Purpose		: Finds out if a particular TBActorInstnance is attached to a particular ACTORINSTANCE
   Parameters	: ACTORINSTANCE of owner, TBActorInstance of object
   Returns		: true if the object is attached
   Info			: // PP: 
*/
bool FindAttachedObjectInstance(const ACTORINSTANCE* const owner, const TBActorInstance* const object)
{
	ATTACH *attachInfo;
	int i;

	if (owner->characterInfo)
	{
		attachInfo = &owner->characterInfo->attachedObjects;

		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i] == object)
			{
				// PP: We found 'im Sarge
				return true;
			}
		}
	}

	// PP: He got away Sarge
	return false;
}


/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsWorldPosition
   Purpose : Returns the actual world position of an attached object
   Parameters : ACTORINSTANCE, handle, pos
   Returns : 
   Info : 
*/
void GetAttachedObjectsWorldPosition(ACTORINSTANCE *actorInstance, int handle, TBVector pos)
{
	ATTACH *attachInfo;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;

		if (attachInfo->objects[handle])
		{
			bmVectorCopy(pos,attachInfo->actualWorldPosition[handle]);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsWorldPosition
   Purpose : Returns the actual world position of an attached object
   Parameters : ACTORINSTANCE, name, pos
   Returns : 
   Info : 
*/
void GetAttachedObjectsWorldPosition(ACTORINSTANCE *actorInstance, char *object, TBVector pos)
{
	ATTACH	*attachInfo;
	TBActor	*actor;
	int		i;

	actor = FindActorInActorList(object);
	if (!actor) return;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;
	
		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i]->actor==actor)
			{
				bmVectorCopy(pos,attachInfo->actualWorldPosition[i]);
				return;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : GetAttachedObjectsHandle
   Purpose : Returns the handle for the first attached object of the type object
   Parameters : ACTORINSTANCE, name
   Returns : handle number
   Info : 
*/
int	GetAttachedObjectsHandle(ACTORINSTANCE *actorInstance, char *object)
{
	ATTACH	*attachInfo;
	TBActor	*actor;
	int		i;

	actor = FindActorInActorList(object);
	if (!actor) return -1;

	if (actorInstance->characterInfo)
	{
		attachInfo = &actorInstance->characterInfo->attachedObjects;
	
		for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
		{
			if (attachInfo->objects[i]->actor==actor)
			{
				return i;
			}
		}
	}

	// PP: (PC warning C4715, not all control paths return a value): return -1, hopefully causing a crash
	return -1;// PP: object not attached
}

/* --------------------------------------------------------------------------------
   Function : AnimateAttachedObjects
   Purpose : Animate any objects attached
   Parameters : 
   Returns : 
   Info : 
*/
void AnimateAttachedObjects(ACTORINSTANCE *actorInstance,TBActorAnimSegment *animation,int speed,float blend,char loop,void *priority)
{
	ATTACH *attachInfo;
	int	i;

	if (!actorInstance->characterInfo) return;

	attachInfo = &actorInstance->characterInfo->attachedObjects;

	for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
	{
		if ((attachInfo->objects[i])&&(attachInfo->flags[i] & ATTACHFLAG_ANIMATEWITHCHAR))
		{
			// duplicate animation on attached object
			if (blend>0.0f)
			{
				baQueueNodeAnimBlendState(attachInfo->objects[i], attachInfo->objects[i]->rootNodeInstance,
					animation, speed, -1, blend, loop>0?(BANIMQUEUEFLAG_LOOPING|BANIMQUEUEFLAG_LOOPPENDING):0,
					NULL,priority);
			}
			else
			{
				baQueueNodeAnimNormal(attachInfo->objects[i], attachInfo->objects[i]->rootNodeInstance, 
					animation, speed, -1, loop>0?(BANIMQUEUEFLAG_LOOPING):0, NULL,priority);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FlushAttachedObjects
   Purpose : Flush the animation for all attached objects
   Parameters : 
   Returns : 
   Info : 
*/
void FlushAttachedObjects(ACTORINSTANCE *actorInstance)
{
	ATTACH *attachInfo;
	int	i;

	if (!actorInstance->characterInfo) return;

	attachInfo = &actorInstance->characterInfo->attachedObjects;

	for (i=0;i<MAX_ATTACHED_OBJECTS;i++)
	{
		if ((attachInfo->objects[i])&&(attachInfo->flags[i] & ATTACHFLAG_ANIMATEWITHCHAR))
		{
			baFlushNodeQueue(attachInfo->objects[i],attachInfo->objects[i]->rootNodeInstance,TRUE);
		}
	}
}