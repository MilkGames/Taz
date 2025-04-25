// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : scenicobjects.cpp
//   Purpose : functions to do with scenic objects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "actors.h"
#include "animation.h"
#include "scene.h"
#include "holysmoke.h"
#include "scenicobjects.h"
#include "status.h"
#include "main.h"
#include "textures.h"
#include "sfx.h"

TBExtraSystem	*scenicFireworksSystem;

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	if ((bmRand()%10)>5)
	{
		PlayAnimationByName(actorInstance,"bounceup",1.0f,1,0,0.1f,JUMP);	
	}
	else
	{
		PlayAnimationByName(actorInstance,"bounceup2",1.0f,0,0,0.1f,JUMP);	
	}
	if (!(bmRand() % 4))
		PlaySample("spring2.wav",255,0,0,actorInstance->actorInstance.position,0);
	else
		PlaySample("spring.wav",255,0,0,actorInstance->actorInstance.position,0);
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: myCollisionCallback
	Purpose		: callback for springy scenics
	Parameters	: actorInstance of player
	Returns		: 
	Info		: 
*/

static int myCollisionCallback(ACTORINSTANCE *actorInstance, ACTORINSTANCE *object)
{
	TBVector		yDirection = { 0.0f, 1.0f, 0.0f, 1.0f };

	// TP: bail out if this is not the player
	if (actorInstance->playerNo!=0) return FALSE;

	RequestStateChangePlus(actorInstance,STATE_SPRUNG,myStateChange);

	// TP: give taz a 7 mps bounce
	actorInstance->actorBehaviour->physics.velocity[Y] = 0.0f;
	AddActorSpeed(actorInstance->actorBehaviour, yDirection, METERTOUNIT(7.0f) );

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CreateScenic
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateScenic(ACTORINSTANCE *actorInstance, char *animation, int collision, uint32 smokeNodeCRC, float maxScale, EScenic type, int liveupdate, bool spring)
{
	actorInstance->special = SPECIALTYPE_SCENIC;
	if(!(actorInstance->scenicInfo = CreateScenicInfo()))
	{
		return FALSE;
	}

	actorInstance->scenicInfo->defaultAnimationCRC = bkCRC32((uchar*)animation, strlen(animation), 0);
	actorInstance->scenicInfo->smokeNodeCRC = smokeNodeCRC;
	actorInstance->scenicInfo->maxScale = maxScale;
	actorInstance->scenicInfo->type = type;

	if(collision) AddActorToCollisionCache(&collisionCache, actorInstance, liveupdate);

	if (spring)	SetCollisionCallback(actorInstance, myCollisionCallback);

	if (animation[0]!=0)
	{
		FlushAnimationQueue(actorInstance);
		PlayAnimationByName(actorInstance, animation, 1.0f, 1, 0, 0.0f, NONE);
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateScenicInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

SCENICINFO *CreateScenicInfo(void)
{
	SCENICINFO	*tempStruct;

	tempStruct = (SCENICINFO*)ZALLOC(sizeof(SCENICINFO));
	if(tempStruct)
	{
		return tempStruct;
	}
	else
	{
		bkPrintf("*** WARNING ***, scenic info structure could not be created.\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateScenicInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateScenicObject(ACTORINSTANCE *actorInstance)
{
	ASSERTM(actorInstance->scenicInfo, "specialtype_scenic instance does not have an info structure!!");
	TBVector			position;
	float				size;
	TBActorNodeInstance	*node;

	if(actorInstance->scenicInfo->smokeNodeCRC)
	{
		DrawSmokeAtNodeInstance(actorInstance, NULL, actorInstance->scenicInfo->smokeNodeCRC, SMOKE_SKIDSTOP, actorInstance->scenicInfo->maxScale);
	}
/*	switch(actorInstance->scenicInfo->type)
	{
	case SCENICTYPE_FIREWORKS:
		if(CheckIfPlayingAnimation(actorInstance, "explode"))
		{
			size = METERTOUNIT(UtilGenerateRandomNumber(0.1f, 0.05f));
			if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "bank - tnt rocketa"))
			{
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);
				baCreateExtra(scenicFireworksSystem, position, &size);
			}
			size = METERTOUNIT(UtilGenerateRandomNumber(0.1f, 0.05f));
			if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "bank - tnt rocketb"))
			{
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);
				baCreateExtra(scenicFireworksSystem, position, &size);
			}
		}
		break;
	}*/
}

/* --------------------------------------------------------------------------------
   Function : InitScenicFireworksExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitScenicFireworksExtraSystem(void)
{
	scenicFireworksSystem = baCreateExtraSystem("scenicFireworks",ScenicFireworksExtraCreateCallback,
		ScenicFireworksExtraUpdateCallback,ScenicFireworksExtraDeleteCallback,ScenicFireworksExtraRenderCallback,
		ScenicFireworksExtraSystemInitCallback,sizeof(SCENICFIREWORKS),NUM_SCENIC_FIREWORKS,0,0);
}

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void ScenicFireworksExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    SCENICFIREWORKS		*data = (SCENICFIREWORKS*)vData;
	float				*position;
	float				*velocity;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		data->size = *va_arg(parms, float*);
		data->alpha = UtilGenerateRandomNumber(127.0f, 100.0f);
	}
}

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ScenicFireworksExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	SCENICFIREWORKS		*data;
	TBVector			posInc;	
	int					i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (SCENICFIREWORKS*)dataPtrs[i];

		if((data->alpha -= 127.0f*fTime) < 0.0f)
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	SCENICFIREWORKS		*data;
	int					i, bufIndex;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	
	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (SCENICFIREWORKS*)dataPtrs[i];
		
		data->position[3] = 1.0f;

		bdSetIdentityObjectMatrix();
		if(bdDrawFlatSprite(data->position,data->size,data->size,0.0f,NULL,0,0,
							120, 85, 33,bmFloatToInt(data->alpha)<<1,&global2DVertexBuffer[bufIndex],0.0f))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	bdSetIdentityObjectMatrix();
	bdSetTexture(0, texturePowerUpGlow);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : ScenicFireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ScenicFireworksExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}