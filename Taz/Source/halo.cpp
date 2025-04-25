// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : halo.cpp
//   Purpose : functions to create a mist effect which is pleasing to the eye
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "main.h"
#include "status.h"
#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "textures.h"
#include "util.h"
#include "halo.h"


/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

int CreateHaloObject(ACTORINSTANCE *actorInstance, float size, uint32 textureCRC, uint32 nodeCRC, int red, int green, int blue)
{
	int			i;

	actorInstance->special = SPECIALTYPE_HALO;
	actorInstance->flags |= ACTORFLAG_DONTDRAW;
	actorInstance->haloInfo = (HALOINFO*)ZALLOC(sizeof(HALOINFO));
	ASSERTM(actorInstance->haloInfo, "out of memory");

	actorInstance->haloInfo->size = size;
	actorInstance->haloInfo->texture = bkLoadTextureByCRC(NULL, textureCRC, 0);
	actorInstance->haloInfo->followNode = baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance, nodeCRC);

	actorInstance->haloInfo->numFireflies = bmRand()%HALO_MAXFLIES;
	actorInstance->haloInfo->flyPositions = (TBVector*)ZALLOC(sizeof(TBVector)*actorInstance->haloInfo->numFireflies);
	ASSERTM(actorInstance->haloInfo->flyPositions, "out of memory");
	actorInstance->haloInfo->flyOrientations = (TBQuaternion*)ZALLOC(sizeof(TBQuaternion)*actorInstance->haloInfo->numFireflies);
	ASSERTM(actorInstance->haloInfo->flyOrientations, "out of memory");
	for(i = (actorInstance->haloInfo->numFireflies-1);i >= 0;--i)
	{
		bmQuatCopy(actorInstance->haloInfo->flyOrientations[i], bIdentityQuaternion);
	}
	actorInstance->haloInfo->flyTexture = bkLoadTexture(NULL, "shad01.tga", 0);
	actorInstance->haloInfo->flySize = METERTOUNIT(0.04f);
	actorInstance->haloInfo->r = red;
	actorInstance->haloInfo->g = green;
	actorInstance->haloInfo->b = blue;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateHaloObject(ACTORINSTANCE *actorInstance)
{
	HALOINFO	*info = actorInstance->haloInfo;
	TBMatrix	orientation, position, result;
	TBVector	posVec;
	int			i;
	float		value = 0.1f;

	if(info)
	{
		if(info->followNode)
		{
			baGetNodesWorldPosition(&actorInstance->actorInstance, info->followNode, posVec);
			bmMatTranslate(position, posVec[0], posVec[1], posVec[2]);
		}
		else
		{
			bmMatTranslate(position, actorInstance->actorInstance.position[0], actorInstance->actorInstance.position[1], actorInstance->actorInstance.position[2]);
		}
		for(i = (info->numFireflies-1);i >= 0;--i)
		{
			// CMD: update orientation
			info->flyOrientations[i][0] += UtilGenerateRandomNumber(value, -value);
			info->flyOrientations[i][1] += UtilGenerateRandomNumber(value, -value);
			info->flyOrientations[i][2] += UtilGenerateRandomNumber(value, -value);
			info->flyOrientations[i][3] += UtilGenerateRandomNumber(value, -value);
			utilNormaliseQuaternion(info->flyOrientations[i]);
			// CMD: calculate position
			bmQuatToMatrix(orientation, info->flyOrientations[i]);
			bmMatMultiply(result, position, orientation);
			SETVECTOR(info->flyPositions[i], 0.0f, 0.0f, METERTOUNIT(1.0f), 1.0f);
			bmMatMultiplyVector(result, info->flyPositions[i]);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void DrawHaloObject(ACTORINSTANCELIST *instanceList)
{
	ACTORINSTANCE	*ptr;
	HALOINFO		*info;
	TBVector		displacement, position;
	float			alpha;
	int				i;

//	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, TRUE);

	for(ptr = instanceList->head.next;ptr != &instanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_HALO)
		{
			if(info = ptr->haloInfo)
			{
				if(info->followNode)
				{
					baGetNodesWorldPosition(&ptr->actorInstance, info->followNode, position);
				}
				else
				{
					bmVectorCopy(position, ptr->actorInstance.position);
				}
				// CMD: draw halo
				bmVectorSub(displacement, position, gameStatus.player[0].camera->pos);
				if((alpha = (50.0f - (50.0f*bmVectorLen(displacement)/METERTOUNIT(40.0f))) ) < 0.0f) alpha = 0.0f;

				// PP: Must set W component of sprite position to 1; leaving it at 0 will cause a divide by zero inside sprite draw (on Xbox at least)
				// PP: Of course, if you'd all just use my handy VEC wrapper (see Vec.h), this problem wouldn't arise... ;o)
				position[W]=1.0f;

				bdDrawFlatSprite(position,info->size,info->size,0.0f,textureGenericGlow,0,0,info->r,info->g,info->b,bmFloatToInt(alpha)<<1,NULL,250.0f);
				// CMD: draw flies
				if(alpha)
				{
					for(i = (info->numFireflies-1);i >= 0;--i)
					{
						bdDrawFlatSprite(info->flyPositions[i],info->flySize,info->flySize,0.0f,info->flyTexture,0,0,0,0,0,255,NULL,0.0f);
					}
				}
			}
		}
	}
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,FALSE);

}

/* --------------------------------------------------------------------------------
   Function : CreateHaloObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHaloObjectMemory(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->special == SPECIALTYPE_HALO)
	{
		if(actorInstance->haloInfo)
		{
			if(actorInstance->haloInfo->flyOrientations)
			{
				SAFE_FREE(actorInstance->haloInfo->flyOrientations);
			}
			if(actorInstance->haloInfo->flyPositions)
			{
				SAFE_FREE(actorInstance->haloInfo->flyPositions);
			}
			actorInstance->haloInfo->numFireflies = 0;
		}
	}
}