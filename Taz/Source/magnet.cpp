// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fountainextrasystem.cpp
//   Purpose : 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "util.h"
#include "sfx.h"
#include "scene.h"
#include "status.h"
#include "physics.h"
#include "textures.h"
#include "magnet.h"

/* --------------------------------------------------------------------------------
   Function : CreateMagnetInstance
   Purpose : makes instances act like magnets
   Parameters : fairly obvious stuff!
   Returns : true/false dependant on success
   Info : If the traditionally shaped magnet were to be used as a horseshoe, horses would be a great deal more versatile
*/

MAGNETINFO *CreateMagnetInstance(ACTORINSTANCE *actorInstance, float minRadius, float maxRadius)
{
	MAGNETINFO	*ptr;
	TBMatrix	matrix;
	int			i, j, k;

	if((ptr = (MAGNETINFO*)ZALLOC(sizeof(MAGNETINFO))) == NULL)
	{
		bkPrintf("*** WARNING ***, magnet info structure could not be allocated./n");
		return NULL;
	}

	for(i = 0;i < MAGNET_NUM_PLAYERS;i++)
	{
		ptr->character[i].minRadius = minRadius;
		ptr->character[i].maxRadius = maxRadius;
		ptr->character[i].state = MAGNETSTATE_NEUTRAL;
		ptr->character[i].magSoundChannel = -1;
	
		// CMD: calculate control points to be at minRad and midway betweety minRad and maxRad
		// CMD: along the z axis of the instance
		bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);
	
		bmVectorSet(ptr->character[i].effectInfo.initCtrlPts[0], 0.0f, 0.0f, (minRadius*0.5f), 1.0f);
		bmMatMultiplyVector(matrix, ptr->character[i].effectInfo.initCtrlPts[0]);
		bmVectorAdd(ptr->character[i].effectInfo.initCtrlPts[0], ptr->character[i].effectInfo.initCtrlPts[0], actorInstance->actorInstance.position);
		bmVectorSet(ptr->character[i].effectInfo.initCtrlPts[1], 0.0f, 0.0f, minRadius, 1.0f);
		bmMatMultiplyVector(matrix, ptr->character[i].effectInfo.initCtrlPts[1]);
		bmVectorAdd(ptr->character[i].effectInfo.initCtrlPts[1], ptr->character[i].effectInfo.initCtrlPts[1], actorInstance->actorInstance.position);

		for(j = 0;j < MAGNET_NUM_STREAKS;j++)
		{
			bmVectorCopy(ptr->character[i].effectInfo.streakInfo[j].startPoint, actorInstance->actorInstance.position);
			bmVectorCopy(ptr->character[i].effectInfo.streakInfo[j].ctrlPts[0], ptr->character[i].effectInfo.initCtrlPts[0]);
			bmVectorCopy(ptr->character[i].effectInfo.streakInfo[j].ctrlPts[1], ptr->character[i].effectInfo.initCtrlPts[1]);
			ptr->character[i].effectInfo.streakInfo[j].ctrlClock = UtilGenerateRandomNumber(10.0f, 0.0f);
			ptr->character[i].effectInfo.streakInfo[j].jitterClock = UtilGenerateRandomNumber(MAGNET_JITTER_PERIOD, 0.0f);
			ptr->character[i].effectInfo.streakInfo[j].startClock = UtilGenerateRandomNumber(MAGNET_START_PERIOD, 0.0f);
			for(k = 0;k < MAGNET_NUM_POINTS; k++)
			{
				ptr->character[i].effectInfo.streakInfo[j].size[k] = UtilGenerateRandomNumber(METERTOUNIT(0.1f), METERTOUNIT(0.05f));
				ptr->character[i].effectInfo.streakInfo[j].colour[k][0] = 127;
				ptr->character[i].effectInfo.streakInfo[j].colour[k][1] = 127;
				ptr->character[i].effectInfo.streakInfo[j].colour[k][2] = 127;
				ptr->character[i].effectInfo.streakInfo[j].colour[k][3] = 200;
			}
		}
	}
	
	actorInstance->magnetInfo = ptr;
	actorInstance->special = SPECIALTYPE_MAGNET;
	return ptr;
}

/* --------------------------------------------------------------------------------
   Function : UpdateMagnetInstance
   Purpose : updates magnet instances
   Parameters : magnet instance actorInstance
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void UpdateMagnetInstance(ACTORINSTANCE *actorInstance)
{
	MAGNETINFO			*info = actorInstance->magnetInfo;
	MAGNETPLAYERINFO	*magCharInfo;
	MAGNETSTREAKINFO	*magStreakInfo;
	TBVector			velocity, disp, tempVec, startVec;
	float				speed,length;
	int					i,j, k;

	for(i = (MAGNET_NUM_PLAYERS-1);i >= 0;--i)
	{

		magCharInfo = &info->character[i];

		// CMD: just for now check for atlas spheres
		switch(magCharInfo->actorEffected->actorStatus->currentState)
		{
		case STATE_ATLASSPINUP:
		case STATE_ATLASSPIN:
		case STATE_ATLASSPINDOWN:
		case STATE_ATLASSPHERES:
			break;
		default:
			continue;
		}

		bmVectorSub(disp, actorInstance->actorInstance.position, magCharInfo->actorEffected->actorInstance.position);
		length = bmVectorLen(disp);

		// CMD: update physical magnet effect
		switch(magCharInfo->state)
		{
		case MAGNETSTATE_NEUTRAL:
			if(length < magCharInfo->maxRadius)
			{
				// CMD: which side of halfway?
				if(length < (magCharInfo->minRadius + (magCharInfo->maxRadius-magCharInfo->minRadius)*0.5f))
				{
					if (magCharInfo->magSoundChannel == -1)
						magCharInfo->magSoundChannel = PlayTrackingSample("electricloop.wav", 255, actorInstance->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));

					magCharInfo->state = MAGNETSTATE_REPULSIVE;

					for(j=0;j < MAGNET_NUM_STREAKS;j++)
					{
						for(k=0;k < MAGNET_NUM_POINTS;k++)
						{
							magCharInfo->effectInfo.streakInfo[j].colour[k][0] = 127;
							magCharInfo->effectInfo.streakInfo[j].colour[k][1] = 0;
							magCharInfo->effectInfo.streakInfo[j].colour[k][2] = 0;
						}
					}
				}
				else
				{
					magCharInfo->state = MAGNETSTATE_ATTRACTIVE;

					if (magCharInfo->magSoundChannel == -1)
						magCharInfo->magSoundChannel = PlayTrackingSample("electricloop.wav", 255, actorInstance->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));

					for(j=0;j < MAGNET_NUM_STREAKS;j++)
					{
						for(k=0;k < MAGNET_NUM_POINTS;k++)
						{
							magCharInfo->effectInfo.streakInfo[j].colour[k][0] = 127;
							magCharInfo->effectInfo.streakInfo[j].colour[k][1] = 127;
							magCharInfo->effectInfo.streakInfo[j].colour[k][2] = 0;
						}
					}
				}
			}
			break;
		case MAGNETSTATE_ATTRACTIVE:
			if(length < magCharInfo->maxRadius)
			{
				if(length < (magCharInfo->minRadius + (magCharInfo->maxRadius-magCharInfo->minRadius)*0.5f))
				{
					if (magCharInfo->magSoundChannel == -1)
						magCharInfo->magSoundChannel = PlayTrackingSample("electricloop.wav", 255, actorInstance->actorInstance.position, 0, 0, METERTOUNIT(5.0f), METERTOUNIT(12.5f));

					magCharInfo->state = MAGNETSTATE_REPULSIVE;

					for(j=0;j < MAGNET_NUM_STREAKS;j++)
					{
						for(k=0;k < MAGNET_NUM_POINTS;k++)
						{
							magCharInfo->effectInfo.streakInfo[j].colour[k][0] = 127;
							magCharInfo->effectInfo.streakInfo[j].colour[k][1] = 0;
							magCharInfo->effectInfo.streakInfo[j].colour[k][2] = 0;
						}
					}
				}
			}
			else
			{
				StopChannel(&magCharInfo->magSoundChannel);

				magCharInfo->state = MAGNETSTATE_NEUTRAL;
				break;
			}
			// CMD: apply velocity
			bmVectorSub(velocity, actorInstance->actorInstance.position, info->character[i].actorEffected->actorInstance.position);
			bmVectorScaleToLength(velocity, velocity, (MAGNET_MAX_VEL - (MAGNET_MAX_VEL*bmVectorLen(velocity)/info->character[i].maxRadius)));
			AddActorVelocity(info->character[i].actorEffected->actorBehaviour, velocity);
			break;
		case MAGNETSTATE_REPULSIVE:
			if(length < magCharInfo->maxRadius)
			{
				if(length > (magCharInfo->minRadius + (magCharInfo->maxRadius-magCharInfo->minRadius)*0.5f))
				{
					// CMD: don't go into attractive state, so that character does not get stuck midway
					break;
				}
			}
			else
			{
				StopChannel(&magCharInfo->magSoundChannel);

				magCharInfo->state = MAGNETSTATE_NEUTRAL;
				break;
			}
			// CMD: apply velocity
			bmVectorSub(velocity, info->character[i].actorEffected->actorInstance.position, actorInstance->actorInstance.position);
			bmVectorScaleToLength(velocity, velocity, (MAGNET_MAX_VEL - (MAGNET_MAX_VEL*bmVectorLen(velocity)/info->character[i].maxRadius)));
			AddActorVelocity(info->character[i].actorEffected->actorBehaviour, velocity);
			break;
		}
		

		// CMD: update streak effect, and all knobs and twiddles
		for(j = 0;j < MAGNET_NUM_STREAKS;++j)
		{
			magStreakInfo = &magCharInfo->effectInfo.streakInfo[j];
	
			// CMD: update control points
			magStreakInfo->ctrlClock += fTime;
			magStreakInfo->ctrlPts[0][1] = magCharInfo->effectInfo.initCtrlPts[0][1] + METERTOUNIT(1.0f)*bmSin(RAD(30.0f*i)+RAD(360.0f*magStreakInfo->ctrlClock));
			magStreakInfo->ctrlPts[1][1] = magCharInfo->effectInfo.initCtrlPts[1][1] + METERTOUNIT(1.0f)*bmSin(RAD(30.0f*i)+RAD(360.0f*magStreakInfo->ctrlClock));
			// CMD: update start point
			if((magStreakInfo->startClock -= fTime) < 0.0f)
			{
				magStreakInfo->startClock = MAGNET_START_PERIOD;
				magStreakInfo->startPoint[0] = 0.0f;
				magStreakInfo->startPoint[1] = UtilGenerateRandomNumber(METERTOUNIT(2.0f), METERTOUNIT(-2.0f));
				magStreakInfo->startPoint[2] = 0.0f;
				magStreakInfo->startPoint[3] = 1.0f;
			}
			else
			{
				//scale jitter slowly to zero
				bmVectorCMul(magStreakInfo->startPoint, magStreakInfo->startPoint, 0.9f);
			}
			bmVectorAdd(startVec , actorInstance->actorInstance.position, magStreakInfo->startPoint);

			// CMD: calculate points in streak
			if((magStreakInfo->jitterClock -= fTime) < 0.0f)
			{
				magStreakInfo->jitterClock = MAGNET_JITTER_PERIOD;
				magStreakInfo->endJitter[0] = UtilGenerateRandomNumber(METERTOUNIT(3.0f), -METERTOUNIT(3.0f));
				magStreakInfo->endJitter[1] = UtilGenerateRandomNumber(METERTOUNIT(2.5f), 0.0f);
				magStreakInfo->endJitter[2] = UtilGenerateRandomNumber(METERTOUNIT(3.0f), -METERTOUNIT(3.0f));
				magStreakInfo->endJitter[3] = 1.0f;
			}
			else
			{
				//scale jitter slowly to zero
				bmVectorCMul(magStreakInfo->endJitter, magStreakInfo->endJitter, 0.9f);
			}
			bmVectorAdd(tempVec, magCharInfo->actorEffected->actorInstance.position, magStreakInfo->endJitter);
			CalcCurve(startVec, magStreakInfo->ctrlPts[0], magStreakInfo->ctrlPts[1], 
																			tempVec, MAGNET_NUM_POINTS, magStreakInfo->points);
		}
	}
}
	
/* --------------------------------------------------------------------------------
   Function : DrawMagnetStuff
   Purpose : updates magnet instances
   Parameters : magnet instance actorInstance
   Returns : 
   Info : Willy Weasil is, contrary to general opinion, a fantastic game character
*/

void DrawMagnetStuff(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*actorInstance;
	MAGNETSTREAKINFO	*info;
	int32				noofVerts;
	int					i, j, k;
	float				screenSize[MAGNET_NUM_POINTS];

#ifndef INFOGRAMES
#ifdef _TAZDEBUG
	char			stringBuf[64];
	ushort			string16Buf[64];
	TBMatrix		matrix;

	bmMatScale(matrix, 0.5f, 0.5f, 0.5f);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetObjectMatrix(matrix);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for(actorInstance = actorInstanceList->head.next;actorInstance != &actorInstanceList->head;actorInstance = actorInstance->next)
	{
		if(actorInstance->special != SPECIALTYPE_MAGNET) continue;

		for(i = (MAGNET_NUM_PLAYERS-1);i >= 0;i--)
		{
			switch(actorInstance->magnetInfo->character[i].state)
			{
			case MAGNETSTATE_NEUTRAL:
				strcpy(stringBuf, "");
				break;
			case MAGNETSTATE_ATTRACTIVE:
				strcpy(stringBuf, "Attractive");
				break;
			case MAGNETSTATE_REPULSIVE:
				strcpy(stringBuf, "Repulsive");
				break;
			}

			bkString8to16(string16Buf, stringBuf);
			if(i)
			{
				// CMD: daffy
				bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), 400.0f, 250.0f, 127,127,127, 127, NULL, NULL);
			}
			else
			{
				// CMD: taz
				bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -425.0f, 250.0f, 127,127,127, 127, NULL, NULL);
			}
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
#endif
#endif

	for(actorInstance = actorInstanceList->head.next;actorInstance != &actorInstanceList->head;actorInstance = actorInstance->next)
	{
		if(actorInstance->special == SPECIALTYPE_MAGNET)
		{
			for(i = (MAGNET_NUM_PLAYERS-1);i >= 0;i--)
			{
				switch(actorInstance->magnetInfo->character[i].state)
				{
				case MAGNETSTATE_ATTRACTIVE:
				case MAGNETSTATE_REPULSIVE:
					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
					bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
				
					for(j=0;j<MAGNET_NUM_STREAKS;j++)
					{
						info = &actorInstance->magnetInfo->character[i].effectInfo.streakInfo[j];
				
						for(k = 0;k < MAGNET_NUM_POINTS;k++)
						{
							info->points[k][3] = 1.0f;
						}
				
						bdSetIdentityObjectMatrix();
						bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
						noofVerts = bdDrawMultiStreakClipped(MAGNET_NUM_POINTS, &info->points[0][0], info->size, screenSize,
																&info->colour[0][0], global2DVertexBuffer, 0.0f);
			
						if(noofVerts)
						{
							bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
							bdSetTexture(0, textureMechaElec);
							bdSetIdentityObjectMatrix();
							bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofVerts, BVERTTYPE_SINGLE2D);
						}
					}

					bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
					bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
					bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
					continue;
				default:
					continue;
				}
			}
		}
	}
}