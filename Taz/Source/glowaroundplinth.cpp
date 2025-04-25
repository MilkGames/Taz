// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : glowaroundplinth.cpp
//   Purpose : create and update a funky glow around the tutorial books in the zoo hub
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "textures.h"
#include "maths.h"
#include "collectibles.h"
#include "util.h"
#include "glowaroundplinth.h"

// PP: TEMP TEST!!!! remove this
#include "TextFX.h"

TBExtraSystem	*plinthGlowExtraSystem;
PLINTHGLOWINFO	*zooHubPlinthGlow;
PLINTHGLOWINFO	*xDoorsGlow;
POLICEGLOWINFO	hubPoliceBoxGlow;
PLINTHGLOWINFO	secretItemGlow;

// PP: GRRRRRR ZBIAS
#if(BPLATFORM == PS2)
#define PLINTH_GLOW_ZBIAS		METERTOUNIT(0.5f)
#else// PP: not ps2
#define PLINTH_GLOW_ZBIAS		0.005f
#endif// PP: not ps2


/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseGlowAroundPlinth(void)
{
	secretItemGlow.plinth = NULL;
	
	hubPoliceBoxGlow.plinth = NULL;
	
	zooHubPlinthGlow = NULL;
}

void StartPoliceBoxGlowEffect(ACTORINSTANCE *actorInstance)
{
	if (!actorInstance)
		return;

	int	j;

	bmVectorCopy(hubPoliceBoxGlow.position, actorInstance->actorInstance.position);
	hubPoliceBoxGlow.position[1] += METERTOUNIT(1.5f);
	hubPoliceBoxGlow.position[3] = 1.0f;
	// CMD: fill in bounding box values
	hubPoliceBoxGlow.bb.xmin = hubPoliceBoxGlow.position[0] + METERTOUNIT(1.6f);
	hubPoliceBoxGlow.bb.xmax = hubPoliceBoxGlow.position[0] - METERTOUNIT(1.6f);
	hubPoliceBoxGlow.bb.ymin = hubPoliceBoxGlow.position[1] + METERTOUNIT(1.6f);
	hubPoliceBoxGlow.bb.ymax = hubPoliceBoxGlow.position[1] - METERTOUNIT(1.6f);
	hubPoliceBoxGlow.bb.zmin = hubPoliceBoxGlow.position[2] + METERTOUNIT(1.6f);
	hubPoliceBoxGlow.bb.zmax = hubPoliceBoxGlow.position[2] - METERTOUNIT(1.6f);

	hubPoliceBoxGlow.plinth = (void*)actorInstance;

	hubPoliceBoxGlow.clock = 0.0f;
	hubPoliceBoxGlow.alpha = 50;
	
	for(j = (POLICEGLOW_NUMWIGGLES-1);j >= 0;--j)
	{
		hubPoliceBoxGlow.wiggles[j].clock = UtilGenerateRandomNumber(10.0f, 0.0f);
		hubPoliceBoxGlow.wiggles[j].phi = 0.0f;
		hubPoliceBoxGlow.wiggles[j].phiVel = RAD(UtilGenerateRandomNumber(25.0f, 10.0f));
		hubPoliceBoxGlow.wiggles[j].radius = METERTOUNIT(1.5f);
		hubPoliceBoxGlow.wiggles[j].radiusDelta = 0.0f;
		hubPoliceBoxGlow.wiggles[j].theta = 0.0f;
		hubPoliceBoxGlow.wiggles[j].thetaVel = RAD(UtilGenerateRandomNumber(50.0f, 25.0f));
	}

	return;
}

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundPlinthInZooHub(void)
{
	ACTORINSTANCE	*tempInstance;
	int				i,j;

	if(zooHubPlinthGlow = (PLINTHGLOWINFO*)ZALLOC(sizeof(PLINTHGLOWINFO)*4))
	{
		for(i = 3;i >= 0;--i)
		{
			switch(i)
			{
			case 0:
				tempInstance = FindActorInstanceInMap("tweety trigger", 0, &map);
				break;
			case 1:
				tempInstance = FindActorInstanceInMap("tweety trigger01", 0, &map);
				break;
			case 2:
				tempInstance = FindActorInstanceInMap("tweety trigger02", 0, &map);
				break;
			case 3:
				tempInstance = FindActorInstanceInMap("tweety trigger03", 0, &map);
				break;
			}
			if(!tempInstance)
			{
				SAFE_FREE(zooHubPlinthGlow);
				return zooHubPlinthGlow;
			}

			bmVectorCopy(zooHubPlinthGlow[i].position, tempInstance->actorInstance.position);
			zooHubPlinthGlow[i].position[1] += METERTOUNIT(2.0f);
			zooHubPlinthGlow[i].position[3] = 1.0f;
			// CMD: fill in bounding box values
			zooHubPlinthGlow[i].bb.xmin = zooHubPlinthGlow[i].position[0] + METERTOUNIT(1.0f);
			zooHubPlinthGlow[i].bb.xmax = zooHubPlinthGlow[i].position[0] - METERTOUNIT(1.0f);
			zooHubPlinthGlow[i].bb.ymin = zooHubPlinthGlow[i].position[1] + METERTOUNIT(1.0f);
			zooHubPlinthGlow[i].bb.ymax = zooHubPlinthGlow[i].position[1] - METERTOUNIT(1.0f);
			zooHubPlinthGlow[i].bb.zmin = zooHubPlinthGlow[i].position[2] + METERTOUNIT(1.0f);
			zooHubPlinthGlow[i].bb.zmax = zooHubPlinthGlow[i].position[2] - METERTOUNIT(1.0f);

			zooHubPlinthGlow[i].plinth = (void*)tempInstance;

			zooHubPlinthGlow[i].clock = 0.0f;
			zooHubPlinthGlow[i].alpha = 50;

			for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
			{
				zooHubPlinthGlow[i].wiggles[j].clock = UtilGenerateRandomNumber(10.0f, 0.0f);
				zooHubPlinthGlow[i].wiggles[j].phi = 0.0f;
				zooHubPlinthGlow[i].wiggles[j].phiVel = RAD(UtilGenerateRandomNumber(75.0f, 50.0f));
				zooHubPlinthGlow[i].wiggles[j].radius = METERTOUNIT(0.75f);
				zooHubPlinthGlow[i].wiggles[j].radiusDelta = 0.0f;
				zooHubPlinthGlow[i].wiggles[j].theta = 0.0f;
				zooHubPlinthGlow[i].wiggles[j].thetaVel = RAD(UtilGenerateRandomNumber(120.0f, 80.0f));
			}
		}
	}
	return zooHubPlinthGlow;
}

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundSecretItem(void	*context)
{
	COLLECTIBLE		*data = (COLLECTIBLE*)context;
	int				j;

	secretItemGlow.plinth = context;

	bmVectorCopy(secretItemGlow.position, data->position);
	secretItemGlow.position[1] += METERTOUNIT(0.0f);
	secretItemGlow.position[3] = 1.0f;
	// CMD: fill in bounding box values
	secretItemGlow.bb.xmin = secretItemGlow.position[0] + METERTOUNIT(1.0f);
	secretItemGlow.bb.xmax = secretItemGlow.position[0] - METERTOUNIT(1.0f);
	secretItemGlow.bb.ymin = secretItemGlow.position[1] + METERTOUNIT(1.0f);
	secretItemGlow.bb.ymax = secretItemGlow.position[1] - METERTOUNIT(1.0f);
	secretItemGlow.bb.zmin = secretItemGlow.position[2] + METERTOUNIT(1.0f);
	secretItemGlow.bb.zmax = secretItemGlow.position[2] - METERTOUNIT(1.0f);

	secretItemGlow.clock = 0.0f;
	secretItemGlow.alpha = 50;

	for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
	{
		secretItemGlow.wiggles[j].clock = UtilGenerateRandomNumber(10.0f, 0.0f);
		secretItemGlow.wiggles[j].phi = 0.0f;
		secretItemGlow.wiggles[j].phiVel = RAD(UtilGenerateRandomNumber(75.0f, 50.0f));
		secretItemGlow.wiggles[j].radius = METERTOUNIT(0.75f);
		secretItemGlow.wiggles[j].radiusDelta = 0.0f;
		secretItemGlow.wiggles[j].theta = 0.0f;
		secretItemGlow.wiggles[j].thetaVel = RAD(UtilGenerateRandomNumber(120.0f, 80.0f));
	}

	return &secretItemGlow;
}

/* --------------------------------------------------------------------------------
   Function : CreateGlowAroundPlinthInZooHub
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

PLINTHGLOWINFO *CreateGlowAroundInstance(ACTORINSTANCE	*actorInstance, float yOffset)
{
	PLINTHGLOWINFO	*info;
	int				j;

	if(info = (PLINTHGLOWINFO*)MALLOC(sizeof(PLINTHGLOWINFO)))
	{
		info->plinth = actorInstance;

		bmVectorCopy(info->position, actorInstance->actorInstance.position);
		info->position[1] += yOffset;
		info->position[3] = 1.0f;
		// CMD: fill in bounding box values
		info->bb.xmin = info->position[0] + METERTOUNIT(1.0f);
		info->bb.xmax = info->position[0] - METERTOUNIT(1.0f);
		info->bb.ymin = info->position[1] + METERTOUNIT(1.0f);
		info->bb.ymax = info->position[1] - METERTOUNIT(1.0f);
		info->bb.zmin = info->position[2] + METERTOUNIT(1.0f);
		info->bb.zmax = info->position[2] - METERTOUNIT(1.0f);
	
		info->clock = 0.0f;
		info->alpha = 50;
	
		for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
		{
			info->wiggles[j].clock = UtilGenerateRandomNumber(10.0f, 0.0f);
			info->wiggles[j].phi = 0.0f;
			info->wiggles[j].phiVel = RAD(UtilGenerateRandomNumber(75.0f, 50.0f));
			info->wiggles[j].radius = METERTOUNIT(0.75f);
			info->wiggles[j].radiusDelta = 0.0f;
			info->wiggles[j].theta = 0.0f;
			info->wiggles[j].thetaVel = RAD(UtilGenerateRandomNumber(120.0f, 80.0f));
		}
	}

	return info;
}

/* --------------------------------------------------------------------------------
   Function : UpdateGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateGlowAroundPlinth(PLINTHGLOWINFO *plinthArray)
{
	ACTORINSTANCE	*tempInstance;
	COLLECTIBLE		*collectible;
	TBMatrix		rotOne, rotTwo, result;
	TBVector		position;
	int				i, j;

	// CMD: update zoo hub plinths
	if(plinthArray)
	{
		for(i = 3;i >= 0;--i)
		{
			// CMD: visibility test
			if(bdTestBBVisibility(bIdentityMatrix, plinthArray[i].bb.xmin, plinthArray[i].bb.xmax, plinthArray[i].bb.ymin, 
									plinthArray[i].bb.ymax, plinthArray[i].bb.zmin, plinthArray[i].bb.zmax))
			{
				tempInstance = (ACTORINSTANCE*)plinthArray[i].plinth;

				for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
				{
					// CMD: update radius delta
					plinthArray[i].wiggles[j].radiusDelta = METERTOUNIT(0.1f)*bmSin(RAD(360.0f)*(plinthArray[i].wiggles[j].clock += fTime));

					// CMD: update angles
					if((plinthArray[i].wiggles[j].theta += plinthArray[i].wiggles[j].thetaVel*fTime) > RAD(360.0f)) plinthArray[i].wiggles[j].theta -= RAD(360.0f);
					if((plinthArray[i].wiggles[j].phi += plinthArray[i].wiggles[j].phiVel*fTime) > RAD(360.0f)) plinthArray[i].wiggles[j].phi -= RAD(360.0f);

					// CMD: create extra at position specified by angles and radius
					bmVectorSet(position, 0.0f, 0.0f, (plinthArray[i].wiggles[j].radius+plinthArray[i].wiggles[j].radiusDelta), 1.0f);

					bmMatXRotation(rotOne, plinthArray[i].wiggles[j].theta);
					bmMatYRotation(rotTwo, plinthArray[i].wiggles[j].phi);
					bmMatMultiply(result, rotTwo, rotOne);

					bmMatMultiplyVector(result, position);
					bmVectorAdd(position, position, plinthArray[i].position);

					baCreateExtra(plinthGlowExtraSystem, position, (100*tempInstance->currentAlpha), 0, NULL);
				}
			}
		}
	}
	// CMD: update secret item glow
	if(secretItemGlow.plinth)
	{
		collectible = (COLLECTIBLE*)secretItemGlow.plinth;
		if(!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected)
		{
			if(bdTestBBVisibility(bIdentityMatrix, secretItemGlow.bb.xmin, secretItemGlow.bb.xmax, secretItemGlow.bb.ymin,
									secretItemGlow.bb.ymax, secretItemGlow.bb.zmin, secretItemGlow.bb.zmax))
			{
				for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
				{
					// CMD: update radius delta
					secretItemGlow.wiggles[j].radiusDelta = METERTOUNIT(0.1f)*bmSin(RAD(360.0f)*(secretItemGlow.wiggles[j].clock += fTime));
		
					// CMD: update angles
					if((secretItemGlow.wiggles[j].theta += secretItemGlow.wiggles[j].thetaVel*fTime) > RAD(360.0f)) secretItemGlow.wiggles[j].theta -= RAD(360.0f);
					if((secretItemGlow.wiggles[j].phi += secretItemGlow.wiggles[j].phiVel*fTime) > RAD(360.0f)) secretItemGlow.wiggles[j].phi -= RAD(360.0f);
	
					// CMD: create extra at position specified by angles and radius
					bmVectorSet(position, 0.0f, 0.0f, (secretItemGlow.wiggles[j].radius+secretItemGlow.wiggles[j].radiusDelta), 1.0f);

					bmMatXRotation(rotOne, secretItemGlow.wiggles[j].theta);
					bmMatYRotation(rotTwo, secretItemGlow.wiggles[j].phi);
					bmMatMultiply(result, rotTwo, rotOne);
	
					bmMatMultiplyVector(result, position);
					bmVectorAdd(position, position, secretItemGlow.position);
	
					baCreateExtra(plinthGlowExtraSystem, position, 100<<8, 0, NULL);
				}
			}
		}
	}

	if (hubPoliceBoxGlow.plinth)
	{	
		if(bdTestBBVisibility(bIdentityMatrix, hubPoliceBoxGlow.bb.xmin, hubPoliceBoxGlow.bb.xmax, hubPoliceBoxGlow.bb.ymin,
									hubPoliceBoxGlow.bb.ymax, hubPoliceBoxGlow.bb.zmin, hubPoliceBoxGlow.bb.zmax))
		{
			for(j = (POLICEGLOW_NUMWIGGLES-1);j >= 0;--j)
			{
				// CMD: update radius delta
				hubPoliceBoxGlow.wiggles[j].radiusDelta = METERTOUNIT(0.1f)*bmSin(RAD(360.0f)*(hubPoliceBoxGlow.wiggles[j].clock += fTime));
			
				// CMD: update angles
				if((hubPoliceBoxGlow.wiggles[j].theta += hubPoliceBoxGlow.wiggles[j].thetaVel*fTime) > RAD(360.0f)) hubPoliceBoxGlow.wiggles[j].theta -= RAD(360.0f);
				if((hubPoliceBoxGlow.wiggles[j].phi += hubPoliceBoxGlow.wiggles[j].phiVel*fTime) > RAD(360.0f)) hubPoliceBoxGlow.wiggles[j].phi -= RAD(360.0f);
				
				// CMD: create extra at position specified by angles and radius
				bmVectorSet(position, 0.0f, 0.0f, (hubPoliceBoxGlow.wiggles[j].radius+hubPoliceBoxGlow.wiggles[j].radiusDelta), 1.0f);
			
				bmMatXRotation(rotOne, hubPoliceBoxGlow.wiggles[j].theta);
				bmMatYRotation(rotTwo, hubPoliceBoxGlow.wiggles[j].phi);
				bmMatMultiply(result, rotTwo, rotOne);
				
				bmMatMultiplyVector(result, position);
				bmVectorAdd(position, position, hubPoliceBoxGlow.position);
				
				baCreateExtra(plinthGlowExtraSystem, position, 100<<8, 0, NULL);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePlinthGlowAroundActorInstance(PLINTHGLOWINFO *info)
{	
	TBMatrix		rotOne, rotTwo, result;
	TBVector		position;
	int				i, j;

	// CMD: update secret item glow
	if(info)
	{
		if(bdTestBBVisibility(bIdentityMatrix, info->bb.xmin, info->bb.xmax, info->bb.ymin, info->bb.ymax, info->bb.zmin, info->bb.zmax))
		{
			for(j = (PLINTHGLOW_NUMWIGGLES-1);j >= 0;--j)
			{
				// CMD: update radius delta
				info->wiggles[j].radiusDelta = METERTOUNIT(0.1f)*bmSin(RAD(360.0f)*(info->wiggles[j].clock += fTime));
				
				// CMD: update angles
				if((info->wiggles[j].theta += info->wiggles[j].thetaVel*fTime) > RAD(360.0f)) info->wiggles[j].theta -= RAD(360.0f);
				if((info->wiggles[j].phi += info->wiggles[j].phiVel*fTime) > RAD(360.0f)) info->wiggles[j].phi -= RAD(360.0f);
	
				// CMD: create extra at position specified by angles and radius
				bmVectorSet(position, 0.0f, 0.0f, (info->wiggles[j].radius+info->wiggles[j].radiusDelta), 1.0f);

				bmMatXRotation(rotOne, info->wiggles[j].theta);
				bmMatYRotation(rotTwo, info->wiggles[j].phi);
				bmMatMultiply(result, rotTwo, rotOne);
	
				bmMatMultiplyVector(result, position);
				bmVectorAdd(position, position, info->position);
	
				baCreateExtra(plinthGlowExtraSystem, position, 100<<8, 0, NULL);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawGlowAroundPlinth(PLINTHGLOWINFO *plinthArray)
{
	ACTORINSTANCE	*tempInstance;
	COLLECTIBLE		*collectible;
	int				bufIndex, i, alpha;

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
	// CMD: draw glow around plinths in zoohub
	if(plinthArray)
	{
		bdSetIdentityObjectMatrix();// PP: shifted this up here so's sprites are projected through an identity object matrix
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

		bufIndex = 0;
		for(i = 3;i >= 0;--i)
		{
			tempInstance = (ACTORINSTANCE*)plinthArray[i].plinth;
			alpha = (plinthArray[i].alpha*tempInstance->currentAlpha)>>8;
			if(bdDrawFlatSprite(plinthArray[i].position,METERTOUNIT(2.0f),METERTOUNIT(2.0f),0.0f,NULL,0,0,127,120,15,alpha<<1,&global2DVertexBuffer[bufIndex],PLINTH_GLOW_ZBIAS))
			{
				bufIndex += 4;
			}
		}

		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
		bdSetTexture(0, texturePowerUpGlow);

		if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);

		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}

	// CMD: draw glow around secret item
	if(secretItemGlow.plinth)
	{
		collectible = (COLLECTIBLE*)secretItemGlow.plinth;
		if(!gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].secretItemCollected)
		{
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetIdentityObjectMatrix();

			bdDrawFlatSprite(secretItemGlow.position,METERTOUNIT(2.0f),METERTOUNIT(2.0f),0.0f,texturePowerUpGlow,0,0,127,120,15,secretItemGlow.alpha<<1,NULL,PLINTH_GLOW_ZBIAS);

			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}
	}

	// CMD: draw glow around secret item
	if(hubPoliceBoxGlow.plinth)
	{	
		if (hubPoliceBoxGlow.plinth == (void *) -1)
			hubPoliceBoxGlow.alpha--;
		
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetIdentityObjectMatrix();
		
		bdDrawFlatSprite(hubPoliceBoxGlow.position,METERTOUNIT(3.5f),METERTOUNIT(5.0f),0.0f,texturePowerUpGlow,0,0,127,120,15,hubPoliceBoxGlow.alpha<<1,NULL,PLINTH_GLOW_ZBIAS);
		
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

		if (!hubPoliceBoxGlow.alpha)
			hubPoliceBoxGlow.plinth = 0;
	}
}

void EndPoliceBoxGlowEffect()
{
	if (hubPoliceBoxGlow.plinth)
	{	
		hubPoliceBoxGlow.plinth = (void *) -1;
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawGlowAroundPlinth
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeGlowAroundPlinth(PLINTHGLOWINFO *data)
{
	if(data)
	{
		SAFE_FREE(data);
	}
}

/* --------------------------------------------------------------------------------
   Function : InitPlinthGlowSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitPlinthGlowExtraSystem(void)
{
	plinthGlowExtraSystem = baCreateExtraSystem("plinthGlow",PlinthGlowExtraCreateCallback,
		PlinthGlowExtraUpdateCallback,PlinthGlowExtraDeleteCallback,PlinthGlowExtraRenderCallback,
		PlinthGlowExtraSystemInitCallback,sizeof(PLINTHEXTRAINFO),MAX_PLINTHBUBBLES,0,0);
}

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    PLINTHEXTRAINFO		*data = (PLINTHEXTRAINFO*)vData;
	float				*position;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		data->alpha = (int)va_arg(parms, int);
		data->flags = (uint32)va_arg(parms, int);
		position = va_arg(parms, float*);
		if(position) bmVectorCopy(data->velocity, position);

		data->alphaFadeRate = 127.0f/PLINTHGLOW_FADETIME;
		data->clock = 0.0f;
		data->size = METERTOUNIT(0.1f);
	}
}

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int PlinthGlowExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	PLINTHEXTRAINFO *data = (PLINTHEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	PLINTHEXTRAINFO		*data;
	TBVector		posInc;
	
	for (int i=0;i<noofInstances;i++)
	{
		data = (PLINTHEXTRAINFO*)dataPtrs[i];

		if(data->flags & PLINTHFLAGS_VELOCITY)
		{
			ApplyGravity(posInc, data->velocity, worldGravity);
			bmVectorAdd(data->position, data->position, posInc);
		}
		// CMD: update alpha
		if((data->alpha -= (int)(data->alphaFadeRate*fTime)<<8) < 0.0f)
		{
			baDeleteExtraInstance(eSystem, i);
			--noofInstances;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void PlinthGlowExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	PLINTHEXTRAINFO		*data;
	int32			i, bufIndex;


	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();// PP: shifted this up here so's sprites are projected through an identity object matrix

	for (i=0;i<noofInstances;i++)
	{
		data = (PLINTHEXTRAINFO*)dataPtrs[i];
		data->position[3] = 1.0f;

		if(bdDrawFlatSprite(data->position,data->size,data->size,0.0f,NULL,0,0,127,60,30,data->alpha>>7,&global2DVertexBuffer[bufIndex],0))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	bdSetTexture(0, texturePowerUpGlow);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex, BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : PlinthGlowExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void PlinthGlowExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}