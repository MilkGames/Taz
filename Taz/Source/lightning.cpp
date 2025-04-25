// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : lightning.cpp
//   Purpose : uses streaks to produces a lightning effect
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "actors.h"
#include "maths.h"
#include "scene.h"
#include "sparks.h"
#include "animation.h"
#include "sfx.h"
#include "status.h"
#include "characters.h"
#include "lightning.h"
#include "voodoocloud.h"
#include "player.h"
#include "textures.h"


/* --------------------------------------------------------------------------------
   Function : CreateLightning
   Purpose : creates lightning, for it is the function of the gods
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLightning(char *objectName, char *name, CPMNode *node, CPMNode *rootNode)
{
	ACTORINSTANCE	*actorInstance;

	actorInstance = CreateActorInstance(&map.invisibleInstances, objectName, name);
	actorInstance->lightningInfo = CreateLightningInfo();
	if(!actorInstance->lightningInfo) return FALSE;

	actorInstance->special = SPECIALTYPE_LIGHTNING;

	if(!(actorInstance->lightningInfo->startPoints = GetElectricityPathNodesFromMap("startPoint", node)))
		actorInstance->lightningInfo->startPoints = GetElectricityPathNodesFromMap("startPoint", rootNode);
	if(actorInstance->lightningInfo->startPoints)
	{
		actorInstance->lightningInfo->texture = textureLightning;
		actorInstance->lightningInfo->size[0] = METERTOUNIT(0.3f);
		actorInstance->lightningInfo->clock = 0.0f;
		actorInstance->lightningInfo->state = FALSE;
		actorInstance->lightningInfo->calcStartPoint = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CreateLightningInfo
   Purpose : Creates lightning info structure
   Parameters : 
   Returns : 
   Info : 
*/

LIGHTNINGINFO *CreateLightningInfo(void)
{
	LIGHTNINGINFO	*tempInfo;

	tempInfo = (LIGHTNINGINFO*)ZALLOC(sizeof(LIGHTNINGINFO));
	if(tempInfo)
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("*** WARNING *** lightningInfo MALLOC failed\n");
		return NULL;
	}
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
	PlayAnimationByName(actorInstance, "electrocute", 1.0f, 0, 1, 0.1f, NONE);

	PlaySample("electricspark.wav",255,actorInstance->actorInstance.position);
	if(characterSounds.sam == -1)
	{
		int random = bmRand()%2;
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
}

/* --------------------------------------------------------------------------------
   Function : UpdateLightning
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateLightning(ACTORINSTANCE *ptr, ACTORINSTANCE *collider)
{
	int				random;

	if(ptr->lightningInfo)
	{
		//don't strike if tiptoeing
		if(collider->actorStatus->currentState == STATE_TIPTOE)
		{
			ptr->lightningInfo->clock = 0.0f;
		}
		if((ptr->lightningInfo->clock -= fTime) > 0.0f)
		{
			if(!ptr->lightningInfo->state)
			{
				ptr->lightningInfo->state = TRUE;
/*				FlushAnimationQueue(collider);
				PlayAnimationByName(collider, "electrocute", 1.0f, 0, 0, 0.1f, NONE);
				PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
				PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);
				PlayAnimationByName(collider, "electrocute", 1.0f, 0, 1, 0.1f, NONE);

				PlaySample("electricspark.wav",255,collider->actorInstance.position);
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
				}*/
				RequestStateChangePlus(collider, STATE_ELECTROCUTED,myStateChange);
			}
			if(!CreateLightningNodePoints(ptr->lightningInfo, collider))
			{
				ptr->lightningInfo->state = FALSE;
			}
		}
		else
		{
			ptr->lightningInfo->clock = 0.0f;
			ptr->lightningInfo->state = FALSE;
			ptr->lightningInfo->calcStartPoint = TRUE;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateLightningNodePoints
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateLightningNodePoints(LIGHTNINGINFO *lightningInfo, ACTORINSTANCE *collider)
{
	ACTORINSTANCE	*tempInstance;
	TBActorNodeInstance	*node;
	TBVector		offset, radiusVec, temp, tempAlso, bone;
	float			nodeRadius, length;
	float			decayFactor, initDecay;

	//choose start point
	if(lightningInfo->calcStartPoint)
	{
		if(!lightningInfo->startPoints) return FALSE;

		tempInstance = FindActorInstanceInInstanceList(0,lightningInfo->startPoints->objectNames[bmRand()%lightningInfo->startPoints->numObjects],&map.levelInstances);
		if(!tempInstance) tempInstance = FindActorInstanceInInstanceList(0,lightningInfo->startPoints->objectNames[bmRand()%lightningInfo->startPoints->numObjects],&map.invisibleInstances);
		if(tempInstance == NULL) return FALSE;
		bmVectorCopy(lightningInfo->currentStartPos, tempInstance->actorInstance.position);
		lightningInfo->calcStartPoint = FALSE;
	}
	//find node in collider
	node = baFindNode(collider->actorInstance.rootNodeInstance, "spinetop");
	if(node)
	{
		baGetNodesWorldPosition(&collider->actorInstance, node, bone);
		bmVectorSub(offset, bone, lightningInfo->currentStartPos);
	}
	else
	{
		bmVectorSub(offset, collider->actorInstance.position, lightningInfo->currentStartPos);
		bmVectorCopy(bone, collider->actorInstance.position);
	}
	if((length = bmVectorLen(offset)) ==0.0f) return FALSE;

	nodeRadius = LIGHTNING_RADIUSRATIO*length;

	//find vector perp. to length
	// TP: PROBLEM
	bmVectorSub(temp, gameStatus.player[collider->playerNo].camera->pos, gameStatus.player[collider->playerNo].camera->lookAt);
	bmVectorCross(radiusVec, offset, temp);
	bmVectorNorm(radiusVec, radiusVec);

	//set decay factor
	decayFactor = UtilGenerateRandomNumber(1.0f, 0.5f);
	while((initDecay = UtilGenerateRandomNumber(1.0f, -1.0f)) == 0.0f);

	bmVectorCopy(lightningInfo->nodes[0], lightningInfo->currentStartPos);

	bmVectorScaleToLength(temp, offset, (LIGHTNING_NODE1LENGTH*length));
	bmVectorScaleToLength(radiusVec, radiusVec, (nodeRadius*initDecay));
	bmVectorAdd(tempAlso, radiusVec,temp);
	bmVectorAdd(lightningInfo->nodes[1],tempAlso,lightningInfo->currentStartPos);
	lightningInfo->size[1] = lightningInfo->size[0] - lightningInfo->size[0]*LIGHTNING_NODE1LENGTH; 

	bmVectorScaleToLength(temp, offset, (LIGHTNING_NODE2LENGTH*length));
	bmVectorCMul(radiusVec, radiusVec, -decayFactor);
	bmVectorAdd(tempAlso, radiusVec,temp);
	bmVectorAdd(lightningInfo->nodes[2],tempAlso,lightningInfo->currentStartPos);
	lightningInfo->size[2] = lightningInfo->size[0]-lightningInfo->size[0]*LIGHTNING_NODE2LENGTH;

	bmVectorScaleToLength(temp, offset, (LIGHTNING_NODE3LENGTH*length));
	bmVectorCMul(radiusVec, radiusVec, -decayFactor);
	bmVectorAdd(tempAlso, radiusVec,temp);
	bmVectorAdd(lightningInfo->nodes[3],tempAlso,lightningInfo->currentStartPos);
	lightningInfo->size[3] = lightningInfo->size[0]-lightningInfo->size[0]*LIGHTNING_NODE3LENGTH;

	bmVectorScaleToLength(temp, offset, (LIGHTNING_NODE4LENGTH*length));
	bmVectorCMul(radiusVec, radiusVec, -decayFactor);
	bmVectorAdd(tempAlso, radiusVec,temp);
	bmVectorAdd(lightningInfo->nodes[4],tempAlso,lightningInfo->currentStartPos);
	lightningInfo->size[4] = lightningInfo->size[0]-lightningInfo->size[0]*LIGHTNING_NODE4LENGTH;

	bmVectorCopy(lightningInfo->nodes[5], bone);
	lightningInfo->size[5] = 0.0f;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DrawLightning
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawLightning(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	LIGHTNINGINFO	*info;
	float			screenSize[LIGHTNING_NUMNODES];
	int				colour[6][4];
	int				i, noofverts;


	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);

	for(i=0;i<6;i++)
	{
		colour[i][0] = 127;
		colour[i][1] = 127;
		colour[i][2] = 127;
		colour[i][3] = 100 + bmRand()%144;
	}
	for(ptr=actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr=ptr->next)
	{
		if((ptr->special == SPECIALTYPE_LIGHTNING)&&(ptr->lightningInfo))
		{
			if(!ptr->lightningInfo->state) continue;
			info = ptr->lightningInfo;
			for(i=0;i<6;i++)
			{
				info->nodes[i][3] = 1.0f;
			}

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			noofverts = bdDrawMultiStreakClipped(LIGHTNING_NUMNODES,info->nodes[0],info->size,screenSize,&colour[0][0],global2DVertexBuffer,0.0f);

			bdSetTexture(0, info->texture);
			if(noofverts)
			{
				bdSetIdentityObjectMatrix();
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofverts, BVERTTYPE_SINGLE2D);
			}
		}
		else
		if((ptr->special == SPECIALTYPE_ZAPPER)&&(ptr->zapperInfo))
		{
			if(!ptr->zapperInfo->lightningInfo.state) continue;
			info = &ptr->zapperInfo->lightningInfo;
			for(i=0;i<6;i++)
			{
				info->nodes[i][3] = 1.0f;
				colour[i][3] = 127;
			}

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			noofverts = bdDrawMultiStreakClipped(LIGHTNING_NUMNODES,info->nodes[0],info->size,screenSize,&colour[0][0],global2DVertexBuffer,0.0f);

			bdSetTexture(0, info->texture);
			if(noofverts)
			{
				bdSetIdentityObjectMatrix();
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofverts, BVERTTYPE_SINGLE2D);
			}
		}
	}
	//draw voodoo cloud lightning
	// CMD: quick bodge to only draw once
	if(actorInstanceList = &map.invisibleInstances)
	{
		if((gameStatus.player1->characterInfo->voodooCloudInfo)&&(gameStatus.player1->characterInfo->voodooCloudInfo->state==VOODOOCLOUDSTATE_LIGHTNING))
		{
			if(!gameStatus.player1->characterInfo->voodooCloudInfo->lightningInfo.state) return;
			info = &gameStatus.player1->characterInfo->voodooCloudInfo->lightningInfo;
			for(i=0;i<6;i++)
			{
				info->nodes[i][3] = 1.0f;
			}

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			noofverts = bdDrawMultiStreakClipped(LIGHTNING_NUMNODES,info->nodes[0],info->size,screenSize,&colour[0][0],global2DVertexBuffer,0.0f);
			bdSetTexture(0, info->texture);
			if(noofverts)
			{
				bdSetIdentityObjectMatrix();
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofverts, BVERTTYPE_SINGLE2D);
			}
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}
