// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : voodoocloud.cpp
//   Purpose : functions to control voodoo cloud attack
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "main.h"
#include "characters.h"
#include "maths.h"
#include "scene.h"
#include "attach.h"
#include "animation.h"
#include "lights.h"
#include "fallingobjects.h"
#include "voodoocloud.h"
#include "status.h"
#include "sfx.h"

TBExtraSystem		*voodooCloudExtraSystem;

char				*voodooCloudClangers[] = 
{
	{"extras\\anvil.obe"},
	{"extras\\littlepiano.obe"},
};

/* --------------------------------------------------------------------------------
   Function : CreateVoodooCloud
   Purpose : creates cloud for voodoocloud attack
   Parameters : 
   Returns : 
   Info : 
*/

int CreateVoodooCloud(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo->voodooCloudInfo) return false;	// TP: already active

	if((actorInstance->characterInfo->voodooCloudInfo = CreateVoodooCloudInfo()) == NULL) return FALSE;

	actorInstance->characterInfo->voodooCloudInfo->state = VOODOOCLOUDSTATE_APPEAR;
	actorInstance->characterInfo->voodooCloudInfo->clock = 0.0f;
	actorInstance->characterInfo->voodooCloudInfo->rainClock = 0.0f;

	baInitActorInstance(&actorInstance->characterInfo->voodooCloudInfo->actorInstance, FindActorInActorList("voodoocloud.obe"),2);
	baPrepActorInstance(&actorInstance->characterInfo->voodooCloudInfo->actorInstance,TRUE);
	baSetNodeFrame(&actorInstance->characterInfo->voodooCloudInfo->actorInstance, actorInstance->characterInfo->voodooCloudInfo->actorInstance.rootNodeInstance, 0, TRUE);
	baFlushNodeQueue(&actorInstance->characterInfo->voodooCloudInfo->actorInstance, actorInstance->characterInfo->voodooCloudInfo->actorInstance.rootNodeInstance,TRUE);
	EnableActorCartoonRenderMode(&actorInstance->characterInfo->voodooCloudInfo->actorInstance);

	bmVectorCopy(actorInstance->characterInfo->voodooCloudInfo->actorInstance.position, actorInstance->actorInstance.position);
	actorInstance->characterInfo->voodooCloudInfo->offset = METERTOUNIT(1.7f);
	actorInstance->characterInfo->voodooCloudInfo->actorInstance.position[1] += actorInstance->characterInfo->voodooCloudInfo->offset;
	SETVECTOR(actorInstance->characterInfo->voodooCloudInfo->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
	bmQuatCopy(actorInstance->characterInfo->voodooCloudInfo->actorInstance.orientation, actorInstance->actorInstance.orientation);

	FlushAnimationQueue(&actorInstance->characterInfo->voodooCloudInfo->actorInstance);
	PlayAnimationByName(&actorInstance->characterInfo->voodooCloudInfo->actorInstance,"appear",1.0f,0,0,0.1f,NONE);

	//set up lightning info
	actorInstance->characterInfo->voodooCloudInfo->lightningInfo.calcStartPoint = FALSE;
	actorInstance->characterInfo->voodooCloudInfo->lightningInfo.state = FALSE;
	actorInstance->characterInfo->voodooCloudInfo->lightningInfo.size[0] = METERTOUNIT(0.1f);
	actorInstance->characterInfo->voodooCloudInfo->lightningInfo.texture = bkLoadTexture(NULL,"lightning.tga",0);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CreateVoodooCloudInfo
   Purpose : creates cloud for voodoocloud attack
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOCLOUDINFO *CreateVoodooCloudInfo(void)
{
	VOODOOCLOUDINFO *info;

	info = (VOODOOCLOUDINFO*)ZALLOC(sizeof(VOODOOCLOUDINFO));
	if(info)
	{
		return info;
	}
	else
	{
		bkPrintf("*** WARNING ***, could not complete voodooCloudInfo MALLOC.\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void PrepVoodooCloud(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->characterInfo->voodooCloudInfo)
	{
		baPrepActorInstance(&actorInstance->characterInfo->voodooCloudInfo->actorInstance, TRUE);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: DeleteVoodooCloud
	Purpose		: Called to remove the cloud from above taz's head
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DeleteVoodooCloud(ACTORINSTANCE *actorInstance)
{
	TBActorInstance		*cloudInstance;

	if (!actorInstance->characterInfo->voodooCloudInfo) return;

	cloudInstance = &actorInstance->characterInfo->voodooCloudInfo->actorInstance;
	baFreeActorInstance(cloudInstance);
	SAFE_FREE(actorInstance->characterInfo->voodooCloudInfo);
}


/* --------------------------------------------------------------------------------
   Function : UpdateVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateVoodooCloud(ACTORINSTANCE *actorInstance)
{
	VOODOOCLOUDINFO		*voodooInfo;
	ACTORINSTANCE		*droppedObject;
	TBActorInstance		*cloudInstance;
	TBVector			tempVector, rainPos;
	int					i,random;
	float				tempFloat[4];

	//update voodoo cloud lightning
	if((actorInstance->characterInfo->voodooCloudInfo)&&(actorInstance->characterInfo->voodooCloudInfo->state==VOODOOCLOUDSTATE_LIGHTNING))
	{
		if((actorInstance->characterInfo->voodooCloudInfo->lightningInfo.clock -= fTime) > 0.0f)
		{
			if(!actorInstance->characterInfo->voodooCloudInfo->lightningInfo.state)
			{
				actorInstance->characterInfo->voodooCloudInfo->lightningInfo.state = TRUE;
				if(characterSounds.sam == -1)
				{
					random = bmRand()%2;
					switch(random)
					{
					case 0:
						CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_10b.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
						break;
					case 1:
						CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_11a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
						break;
					}
				}
				ReduceCharactersHealth(actorInstance, 1.0f);
				if (EnterState(actorInstance, STATE_ELECTROCUTED))
				{
					FlushAnimationQueue(actorInstance);
					PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
					PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
					PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
					PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
				}
			}
			actorInstance->characterInfo->voodooCloudInfo->lightningInfo.calcStartPoint = FALSE;
			if(CreateLightningNodePoints(&actorInstance->characterInfo->voodooCloudInfo->lightningInfo, actorInstance))
			{
				actorInstance->characterInfo->voodooCloudInfo->lightningInfo.state = TRUE;
			}
		}
		else
		{
			actorInstance->characterInfo->voodooCloudInfo->lightningInfo.clock = 0.0f;
			actorInstance->characterInfo->voodooCloudInfo->lightningInfo.state = FALSE;
			actorInstance->characterInfo->voodooCloudInfo->lightningInfo.calcStartPoint = TRUE;
		}
	}

	if((voodooInfo = actorInstance->characterInfo->voodooCloudInfo) == NULL) return;

	cloudInstance = &actorInstance->characterInfo->voodooCloudInfo->actorInstance;

	//update animation
	baUpdateNodes(cloudInstance, cloudInstance->rootNodeInstance, speedRatio, NULL);
	//update position
	bmVectorCopy(cloudInstance->position, actorInstance->actorInstance.position);
	//update orientation
	bmQuatCopy(cloudInstance->orientation, actorInstance->actorInstance.orientation);
	cloudInstance->position[1] += voodooInfo->offset;

	switch(voodooInfo->state)
	{
	case VOODOOCLOUDSTATE_APPEAR:
		if(!CheckIfPlayingAnimation(cloudInstance, "appear"))
		{
			PlayAnimationByName(cloudInstance,"idle",1.0f,1,0,0.1f,IDLE);
			voodooInfo->clock = VOODOOCLOUD_IDLETIME;
			voodooInfo->state = VOODOOCLOUDSTATE_PASSIVE;
		}
		break;
	case VOODOOCLOUDSTATE_DISAPPEAR:
		if(!CheckIfPlayingAnimation(cloudInstance, "disappear"))
		{
			baFreeActorInstance(cloudInstance);
			FREE(actorInstance->characterInfo->voodooCloudInfo);
			actorInstance->characterInfo->voodooCloudInfo = NULL;
		}
		return;
	case VOODOOCLOUDSTATE_PASSIVE:
		if((voodooInfo->clock -= fTime) < 0.0f)
		{
			i = bmRand()%7;
			switch(i)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				voodooInfo->state = VOODOOCLOUDSTATE_LIGHTNING;
				voodooInfo->clock = voodooInfo->lightningInfo.clock = 1.0f;
				bmVectorCopy(voodooInfo->lightningInfo.currentStartPos, cloudInstance->position);
				break;
			case 5:
				voodooInfo->state = VOODOOCLOUDSTATE_CLANGER;
				break;
			case 6:
				break;
			}
		}
		break;
	case VOODOOCLOUDSTATE_LIGHTNING:
		if((voodooInfo->clock -= fTime) < 0.0f)
		{
			FlushAnimationQueue(&voodooInfo->actorInstance);
			PlayAnimationByName(&voodooInfo->actorInstance,"disappear",1.0f,0,0,0.1f,DEATH);
			voodooInfo->state = VOODOOCLOUDSTATE_DISAPPEAR;
			voodooInfo->clock = VOODOOCLOUD_IDLETIME;
		}
		break;
	case VOODOOCLOUDSTATE_CLANGER:
		bmVectorSub(tempVector, actorInstance->actorInstance.position, voodooInfo->actorInstance.position);
		droppedObject = DropObject(&map.levelInstances, 0, 0, voodooCloudClangers[bmRand()%2], voodooInfo->actorInstance.position, FALLINGTYPE_ONCE,FALLINGTYPE_DESTROY, NULL, NULL, bmVectorLen(tempVector),1);
		if (droppedObject)
		{
			EnableActorLineRenderMode(&droppedObject->actorInstance);
			FlushAnimationQueue(&voodooInfo->actorInstance);
			PlayAnimationByName(&voodooInfo->actorInstance,"disappear",1.0f,0,0,0.1f,DEATH);
		}
		voodooInfo->clock = VOODOOCLOUD_IDLETIME + 10.0f;
		voodooInfo->state = VOODOOCLOUDSTATE_DISAPPEAR;
		break;
	case VOODOOCLOUDSTATE_HAT:
		break;
	}

	if((voodooInfo->state != VOODOOCLOUDSTATE_APPEAR)&&(voodooInfo->state != VOODOOCLOUDSTATE_DISAPPEAR))
	{
		if((voodooInfo->rainClock -= fTime) <0.0f)
		{
//			bmVectorCopy(rainPos, voodooInfo->actorInstance.position);
			rainPos[0] = (float)((bmRand()%100)-50);
			rainPos[2] = (float)((bmRand()%100)-50);
			tempFloat[0] = METERTOUNIT(-4.0f);
			tempFloat[1] = METERTOUNIT(0.01f);
			tempFloat[2] = METERTOUNIT(0.1f);
			tempFloat[3] = 0.5f;
			baCreateExtra(voodooCloudExtraSystem,&rainPos[0],&rainPos[2],&tempFloat[0], &tempFloat[1], &tempFloat[2], &tempFloat[3]);
	
			voodooInfo->rainClock = 0.02f;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawVoodooCloud
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawVoodooCloud(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->characterInfo->voodooCloudInfo)
	{
		baSetGlobalAlpha(256);

		lineParms.r = 0;
		lineParms.g = 0;
		lineParms.b = 0;
		lineParms.a = 255;
		lineParms.thickness = 3.5f;

		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		baDrawActorInstance(&actorInstance->characterInfo->voodooCloudInfo->actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
	}
}


////////////////////////// EXTRAS //////////////////////

/* --------------------------------------------------------------------------------
   Function : InitVoodooCloudExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/

void InitVoodooCloudExtraSystem(void)
{
	voodooCloudExtraSystem = baCreateExtraSystem("voodoocloud",VoodooCloudExtraCreateCallback,
		VoodooCloudExtraUpdateCallback,VoodooCloudExtraDeleteCallback,VoodooCloudExtraRenderCallback,
		VoodooCloudExtraSystemInitCallback,sizeof(VOODOOCLOUDRAININFO),MAX_VOODOORAIN,0,0);
}

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void VoodooCloudExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    VOODOOCLOUDRAININFO		*data = (VOODOOCLOUDRAININFO*)vData;
	float			xOffset;
	float			zOffset;
	float			velocity;
	float			xDim;
	float			yDim;
	float			life;

    if(parms)
	{
		xOffset = *va_arg(parms, float*);
		zOffset = *va_arg(parms, float*);
		velocity = *va_arg(parms, float*);
		xDim = *va_arg(parms,float*);
		yDim = *va_arg(parms,float*);
		life = *va_arg(parms,float*);

		bmVectorCopy(data->position, gameStatus.player1->characterInfo->voodooCloudInfo->actorInstance.position);
		data->xOffset = xOffset;
		data->zOffset = zOffset;
		data->velocity = velocity;
		data->xDim = xDim;
		data->yDim = yDim;
		data->life = life;
		data->texture = bkLoadTexture(NULL, "voodoorain.tga", 0);
		data->red = data->green = data->blue = 255;
		data->alpha = 80;
	}
}

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int VoodooCloudExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	VOODOOCLOUDRAININFO *data = (VOODOOCLOUDRAININFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	VOODOOCLOUDRAININFO		*data;
	int						i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (VOODOOCLOUDRAININFO*)dataPtrs[i];

		if(data->life)
		{
			data->life -= t;
			if(data->life > 0.0f)
			{
				data->position[0] = gameStatus.player1->actorInstance.position[0] + data->xOffset;
				data->position[1] += (data->velocity*fTime);
				data->position[2] = gameStatus.player1->actorInstance.position[2] + data->zOffset;
			}
			else
			{
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : VoodooCloudExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	VOODOOCLOUDRAININFO	*data;
	int			i, bufIndex;

	if((int32)context != EXTRA_SOLID) return;
	
	bufIndex = 0;

	for (i=0;i<noofInstances;i++)
	{
		if(i >= MAX_VOODOORAIN) break;

		data = (VOODOOCLOUDRAININFO*)dataPtrs[i];
		
		data->position[3] = 1.0f;

		if(bdDrawFlatSprite(data->position,data->xDim,data->yDim,0.0f,data->texture,0,0,data->red,data->green,data->blue,data->alpha<<1,&global2DVertexBuffer[bufIndex],0.0f))
		{
			bufIndex += 4;
		}
	}
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	bdSetIdentityObjectMatrix();
	bdSetTexture(0, data->texture);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : SplashExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void VoodooCloudExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}