// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : effects.cpp
//   Purpose : 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"
#include "effects.h"
#include "maths.h"
#include "holysmoke.h"
#include "main.h"
#include "characters.h"
#include "attach.h"
#include "lights.h"


EFFECTSLIST		effectsList;			//list of all effects in level

LINKEDLIST<MIRROR>	MIRROR::mirrorList;


/* --------------------------------------------------------------------------------
   Function : InitEffectsList
   Purpose : guess what it does!
   Parameters : 
   Returns : 
   Info : 
*/

void InitEffectsList(void)
{
	effectsList.numEntries = 0;
	effectsList.head.next = effectsList.head.prev = &effectsList.head;
}

/* --------------------------------------------------------------------------------
   Function : AddEntryToEffectsList
   Purpose : Function to store information about effects
   Parameters : 
   Returns : 
   Info : 
*/

void AddEntryToEffectsList(TBVector startPos, TBVector velocity, float period, uint32 type)
{
	EFFECTSLISTENTRY		*newEntry;
	EFFECTSLISTENTRY		*tempPtr;

	tempPtr = effectsList.head.next;

	//create new structure for incoming information
	if(newEntry = (EFFECTSLISTENTRY *)ZALLOC(sizeof(EFFECTSLISTENTRY)))
	{
		bmVectorCopy(newEntry->startPos, startPos);
		newEntry->period = period;
		newEntry->type = type;
		newEntry->clock = 0.0f;
		bmVectorCopy(newEntry->velocity, velocity);
	}

	ASSERT(newEntry);

	newEntry->next = tempPtr;
	newEntry->prev = tempPtr->prev;
	tempPtr->prev->next = newEntry;
	tempPtr->prev = newEntry;
	effectsList.numEntries++;
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromEffectsList(EFFECTSLISTENTRY *remPtr)
{
	EFFECTSLISTENTRY  *ptr,*temp;

	if(remPtr == NULL)
	{
		bkPrintf("**** ERROR **** pointer to be removed from falling list is NULL");
		return FALSE;
	}

	for (ptr = effectsList.head.next;ptr != &effectsList.head;)
	{
		temp = ptr->next;
		if (ptr == remPtr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			effectsList.numEntries--;
			FREE(ptr);
		}
		ptr = temp;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateEffectsList
   Purpose : updates efects list
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateEffectsList(EFFECTSLIST *list)
{
	EFFECTSLISTENTRY	*ptr;
	TBVector	zeroVec = {0.0f, METERTOUNIT(0.75f), 0.0f, 1.0f};
	TBVector	tempVector;

	
	if(effectsList.head.next != NULL)
	{
		for (ptr = effectsList.head.next;ptr != &effectsList.head;ptr = ptr->next)
		{
			bmVectorSub(tempVector, gameStatus.viewPoint, ptr->startPos);
			if(bmVectorLen(tempVector) < METERTOUNIT(20.0f)) continue;
			switch(ptr->type)
			{
			case EFFECT_CHIMNEYPUFF:
				if((ptr->clock += 1.0f/fps) >= ptr->period)
				{
					CreateImpactClouds(ptr->startPos, gameStatus.windVelocity, ptr->velocity, METERTOUNIT(0.2f), 6.0f, METERTOUNIT(0.5f), SMOKE_BIGFALL);
					ptr->clock = 0.0f;
				}
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeAllEffectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllEffectsFromList(void)
{
	EFFECTSLISTENTRY		  *ptr,*temp;

	bkPrintf("FreeAllEffectsFromList\n");

	for (ptr = effectsList.head.next;ptr != &effectsList.head;)
	{
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		effectsList.numEntries--;

		temp = ptr->next;
		FREE(ptr);
		ptr = temp;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FreeAllMirrors
	Purpose		: Free all the mirrors on a level
	Parameters	: 
	Returns		: 
	Info		: should be called before freeing the actorInstance lists!
*/
void FreeAllMirrors(void)
{
	MIRROR		*ptr;
	int			i;

	for (i=MIRROR::mirrorList.getSize()-1;i>=0;i--)
	{
		ptr = MIRROR::mirrorList.get(i);
		delete ptr;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DrawAllMirrors
	Purpose		: Draw all mirrors affected by actorInstance
	Parameters	: actorInstance
	Returns		: 
	Info		: should be called before freeing the actorInstance lists!
*/
void DrawAllMirrors(ACTORINSTANCE *actorInstance)
{
	MIRROR		*ptr;
	int			i;

	for (i=MIRROR::mirrorList.getSize()-1;i>=0;i--)
	{
		ptr = MIRROR::mirrorList.get(i);

		// TP: check if actorInstance is inside the mirror box
		if (IsPointWithinBox(ptr->actorInstance, actorInstance->actorInstance.position))
		{
			// TP: if it is then call draw instance
			ptr->DrawInstance(actorInstance,0);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FreeMirror
	Purpose		: Free a mirror
	Parameters	: actorInstance driving the mirror
	Returns		: 
	Info		: 
*/
void FreeMirror(ACTORINSTANCE *actorInstance)
{
	MIRROR		*ptr;
	int			i;

	for (i=MIRROR::mirrorList.getSize()-1;i>=0;i--)
	{
		ptr = MIRROR::mirrorList.get(i);

		if (ptr->actorInstance == actorInstance)
		{
			delete ptr;
			return;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: MIRROR::DrawInstance
	Purpose		: Draws the mirror version of an object
	Parameters	: ActorInstance, 2d line to mirror in, flags
	Returns		: 
	Info		: 
*/
void MIRROR::DrawInstance(struct TAG_ACTORINSTANCE *actorInstance, uint32 flags)
{
	float			tempScale,dist;
	TBVector		tempPos,oldPos,mirrorPlaneVec,tempVector;
	TBVector		pos2d,temp2d;
	TBQuaternion	oldQuat,tempQuat,copyQuat;
	float			mirrorPlaneAngle,newAng,charAng;
	int				i;

	DisableActorCartoonRenderMode(&actorInstance->actorInstance);

	// TP: create mirror effect
	tempScale = actorInstance->actorInstance.scale[X];
	actorInstance->actorInstance.scale[X] = -tempScale;

	// TP: set position
	pos2d[X] = actorInstance->actorInstance.position[X];
	pos2d[Y] = 0.0f;
	pos2d[Z] = actorInstance->actorInstance.position[Z];

	bmVectorCopy(oldPos,actorInstance->actorInstance.position);

	dist = bmGeomDistanceFromInfiniteLine(&mirrorPlane, pos2d, temp2d);

	// TP: BE NORMAL!
	tempPos[X] = mirrorPlane.end[Z] - mirrorPlane.start[Z];
	tempPos[Y] = 0.0f;
	tempPos[Z] = mirrorPlane.end[X] - mirrorPlane.start[X];
	bmVectorScaleToLength(tempPos,tempPos,-dist*2);
	bmVectorAdd(actorInstance->actorInstance.position, actorInstance->actorInstance.position,tempPos);

	// TP: set orientation
	bmQuatCopy(oldQuat,actorInstance->actorInstance.orientation);
	mirrorPlaneVec[X] = mirrorPlane.end[X] - mirrorPlane.start[X];
	mirrorPlaneVec[Y] = 0.0f;
	mirrorPlaneVec[Z] = mirrorPlane.end[Z] - mirrorPlane.start[Z];
	mirrorPlaneVec[W] = 1.0f;
	mirrorPlaneAngle = Aabs(GetAngleAroundYFromVector(mirrorPlaneVec)-PI);

	charAng = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
	newAng = FindShortestAngleSigned(charAng, mirrorPlaneAngle);
	newAng = -2*newAng;

	SETVECTOR(tempVector,0.0f,1.0f,0.0f,newAng);
	bmRotationToQuat(tempQuat,tempVector);
	bmQuatCopy(copyQuat,actorInstance->actorInstance.orientation);
	bmQuatMultiply(actorInstance->actorInstance.orientation, copyQuat, tempQuat);
	
	// TP: prep
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);

	// TP: redraw actor
	if ((!(actorInstance->flags & (ACTORFLAG_DONTDRAW|ACTORFLAG_CUSTOMDRAW)))&&(actorInstance->currentAlpha))
	{
		lineParms.thickness = actorInstance->lineThickness;
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		baSetGlobalAlpha(actorInstance->currentAlpha);

#if BPLATFORM == PC
		bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
		if (flags&DRAWACTORFLAG_DONTCLEARONSCREEN)
		{
			// TP: Only set onScreen don't remove
			actorInstance->onScreen |= baDrawActorInstance(&actorInstance->actorInstance,BSETPRIMALPHATYPE_SOLID);
		}
		else
		{
			actorInstance->onScreen = baDrawActorInstance(&actorInstance->actorInstance,BSETPRIMALPHATYPE_SOLID);
		}
		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
	}
	
	PrepAttachedObjects(actorInstance);
	PrepCharactersHealth(actorInstance);

	for(i=0;actorInstance->characterInfo->attachedObjects.objects[i]!=NULL;i++)
	{
		DisableActorCartoonRenderMode(actorInstance->characterInfo->attachedObjects.objects[i]);
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	baSetGlobalAlpha(256);
	bdSetIdentityObjectMatrix();

	UpdateSpinEffectForActor(actorInstance, 0.0f);
	DrawSpinEffectForActor(actorInstance);
	DrawCharacterInfo(actorInstance);

	bmVectorCopy(actorInstance->actorInstance.position, oldPos);
	actorInstance->actorInstance.scale[X] = tempScale;
	bmQuatCopy(actorInstance->actorInstance.orientation, oldQuat);
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);

	PrepAttachedObjects(actorInstance);
	PrepCharactersHealth(actorInstance);

	baSetGlobalAlpha(256);
	lineParms.thickness = 3.5f;	

	for(i=0;actorInstance->characterInfo->attachedObjects.objects[i]!=NULL;i++)
	{
		if (!(actorInstance->characterInfo->attachedObjects.flags[i]&ATTACHFLAG_NOBLACKOUTLINE))
		{
			EnableActorCartoonRenderMode(actorInstance->characterInfo->attachedObjects.objects[i]);
		}
	}

	UpdateSpinEffectForActor(actorInstance, 0.0f);
	EnableActorCartoonRenderMode(&actorInstance->actorInstance);
}

/*  --------------------------------------------------------------------------------
	Function	: MIRROR::TAG_MIRROR
	Purpose		: Default constructor
	Parameters	: actorInstance of mirror
	Returns		: 
	Info		: 
*/
MIRROR::TAG_MIRROR(struct TAG_ACTORINSTANCE *actorInstance)
{
	mirrorList.append(this);
	this->actorInstance = actorInstance;

	// TP: calculate mirror plane
	mirrorPlane.start[X] = actorInstance->actorInstance.actor->xMin;
	mirrorPlane.start[Y] = 0.0f;
	mirrorPlane.start[Z] = actorInstance->actorInstance.actor->zMax;
	mirrorPlane.start[W] = 1.0f;

	mirrorPlane.end[X] = actorInstance->actorInstance.actor->xMax;
	mirrorPlane.end[Y] = 0.0f;
	mirrorPlane.end[Z] = actorInstance->actorInstance.actor->zMax;
	mirrorPlane.end[W] = 1.0f;

	// TP: rotate start and end points
	bmMatMultiplyVector(actorInstance->actorInstance.objectToWorld,mirrorPlane.start);
	mirrorPlane.start[Y] = 0.0f;
	bmMatMultiplyVector(actorInstance->actorInstance.objectToWorld,mirrorPlane.end);
	mirrorPlane.end[Y] = 0.0f;

	actorInstance->special = SPECIALTYPE_MIRROR;
}
