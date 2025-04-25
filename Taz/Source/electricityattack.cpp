// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : electricityattack.cpp
//   Purpose : attack move for mecha tweety boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "animation.h"
#include "physics.h"
#include "health.h"
#include "status.h"
#include "sfx.h"
#include "mechatweety.h"
#include "textures.h"
#include "characters.h"
#include "lights.h"
#include "electricityattack.h"


/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ELECTRICITYATTACKINFO *CreateElectricityAttack(int32 numPointsPerStreak, int32 numStreaks)
{
	ELECTRICITYATTACKINFO	*info = NULL;

	info = (ELECTRICITYATTACKINFO*)ZALLOC(sizeof(ELECTRICITYATTACKINFO));
	ASSERTM(info, "could not create electricity attack info");

	info->waveData = (MTWEETYWAVEDATA*)ZALLOC(sizeof(MTWEETYWAVEDATA)*numStreaks);
	ASSERTM(info->waveData, "out of memory");

	for(int i = 0;i < numStreaks; i++)
	{
		info->waveData[i].streak.points = (TBVector*)ZALLOC(sizeof(TBVector)*numPointsPerStreak);
		ASSERTM(info->waveData[i].streak.points, "out of memory");
		info->waveData[i].streak.tempPoints = (TBVector*)ZALLOC(sizeof(TBVector)*numPointsPerStreak);
		ASSERTM(info->waveData[i].streak.tempPoints, "out of memory");
		info->waveData[i].streak.size = (float*)ZALLOC(sizeof(float)*numPointsPerStreak);
		ASSERTM(info->waveData[i].streak.size, "out of memory");
		info->waveData[i].streak.screenSize = (float*)ZALLOC(sizeof(float)*numPointsPerStreak);
		ASSERTM(info->waveData[i].streak.screenSize, "out of memory");
		info->waveData[i].streak.colour = (int*)ZALLOC(sizeof(int)*4*numPointsPerStreak);
		ASSERTM(info->waveData[i].streak.colour, "out of memory");
		info->waveData[i].streak.buffer = (TBPrimVertex2D*)ZALLOC(sizeof(TBPrimVertex2D)*((numPointsPerStreak*4)+4));
		ASSERTM(info->waveData[i].streak.buffer, "out of memory");
	}

	info->numPoints = numPointsPerStreak;
	info->numStreaks = numStreaks;
	//if i've got this far then everything is ok
	return info;
}

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateElectricityAttack(ACTORINSTANCE *attacker, ELECTRICITYATTACKINFO *info)
{
	MTWEETYWAVEDATA		*wave;
	TBMatrix	translation, rotation, twist, result;
	TBQuaternion	rotQuat;
	TBVector	nodePos;
	float		proportion;
	int			i, j;

	baGetNodesWorldPosition(&attacker->actorInstance, info->node, nodePos);
	bmMatTranslate(translation, nodePos[0], nodePos[1], nodePos[2]);
//	bmVectorSub(relative, info->targetPos, nodePos);

	// CMD: update waveform
	for(j = 0;j < info->numStreaks;j++)
	{
		wave = &info->waveData[j];

		bmDirectionToQuat(rotQuat, info->offset, 0.0f);
		bmQuatToMatrix(rotation, rotQuat);
		bmMatZRotation(twist, wave->tilt);

		bmMatMultiply(result, rotation, twist);
		bmMatMultiply(twist, translation, result);
		for(i = 0;i < info->numPoints;i++)
		{
			proportion = (float)i/info->numPoints;
			wave->clock += fTime;

			wave->streak.points[i][0] = 0.0f;
			wave->streak.points[i][1] = wave->amplitudeConst*((float)(i*i)/(info->numPoints*info->numPoints*2.0f))*bmSin((wave->frequency*proportion)-(wave->streak.timeConst*wave->clock));
			wave->streak.points[i][3] = 1.0f;
			
			bmVectorCopy(wave->streak.tempPoints[i], wave->streak.points[i]);
			bmMatMultiplyVector(twist, wave->streak.tempPoints[i]);
		}

		// CMD: update tilt
		if((bmFAbs(wave->tiltSpeed += wave->tiltDirection*RAD(1024.0f)*fTime)) > RAD(1024.0f))
		{
			wave->tiltSpeed = wave->tiltDirection*RAD(360.0f);
		}
		wave->tilt += wave->tiltSpeed*fTime;

		// CMD: update time constant
	}
}

/* --------------------------------------------------------------------------------
   Function : ResetElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void ResetElectricityAttack(ELECTRICITYATTACKINFO *info)
{
	for(int i = 0;i<info->numStreaks;i++)
	{
		info->waveData[i].clock = 0.0f;
		info->waveData[i].tiltSpeed = 0.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

ELECTRICITYATTACKINFO *FreeElectricityAttackInfo(ELECTRICITYATTACKINFO *info)
{
	if(info)
	{
		for(int i = 0;i < info->numStreaks;i++)
		{
			FREE(info->waveData[i].streak.points);
			FREE(info->waveData[i].streak.tempPoints);
			FREE(info->waveData[i].streak.size);
			FREE(info->waveData[i].streak.screenSize);
			FREE(info->waveData[i].streak.colour);
			FREE(info->waveData[i].streak.buffer);
		}

		FREE(info->waveData);
		SAFE_FREE(info);
	}
	return info;
}

/* --------------------------------------------------------------------------------
   Function : CreateElectricityAttack
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawElectricityAttackInfo(ACTORINSTANCE *attacker, ELECTRICITYATTACKINFO *info)
{
	MTWEETYWAVEDATA		*wave;
	int32		noofVerts;
	int			i, j;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for(i=0;i<info->numStreaks;i++)
	{
		wave = &info->waveData[i];

		for(j = 0;j < info->numPoints;j++)
		{
			wave->streak.tempPoints[j][3] = 1.0f;
		}

		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		noofVerts = bdDrawMultiStreakClipped(info->numPoints, &wave->streak.tempPoints[0][0], wave->streak.size, wave->streak.screenSize,
												wave->streak.colour, wave->streak.buffer, 0.0f);

		if(noofVerts)
		{
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
			bdSetTexture(0, wave->streak.texture);
			bdSetIdentityObjectMatrix();
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, wave->streak.buffer, noofVerts, BVERTTYPE_SINGLE2D);
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : FillInAttributesForMTweetyGame
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FillInAttributesForMTweetyGame(ELECTRICITYATTACKINFO *info)
{
	MTWEETYWAVEDATA		*wave;
	float	length;
	int		i, j, k;


	
	info->streakLength = MTWEETY_ATTACKDIST;
	for(i = 0;i < info->numStreaks;i++)
	{
		wave = &info->waveData[i];

		wave->amplitudeConst = METERTOUNIT(4.0f);
		wave->clock = 0.0f;
		wave->frequency = RAD(720.0f);
		wave->tilt = RAD((float)(bmRand()%360));
		wave->tiltSpeed = 0.0f;
		if(bmRand()%2) wave->tiltDirection = 1.0f;
		else wave->tiltDirection = -1.0f;

		length = -info->streakLength/info->numPoints;
		for(k = 0;k< info->numPoints;k++)
		{
			j = k*4;

			wave->streak.points[k][0] = 0.0f;
			wave->streak.points[k][1] = 0.0f;
			wave->streak.points[k][2] = k*length;
			wave->streak.points[k][3] = 1.0f; 
			bmVectorCopy(wave->streak.tempPoints[k], wave->streak.points[k]);

			wave->streak.colour[j] = 128;
			wave->streak.colour[j+1] = 128;
			wave->streak.colour[j+2] = 128;
			wave->streak.colour[j+3] = 140;

			wave->streak.size[k] = METERTOUNIT(0.1f);

			wave->streak.timeConst = RAD(30.0f+((float)(bmRand()%30)));
			wave->streak.texture = textureMechaElec;
		}
	}

	info->state = FALSE;
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, NONE);
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
	actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
	AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(10.0f));

	if (characterSounds.taz == -1)
		CallbackWhenSampleFinished(characterSounds.taz = PlaySample("tazburnt.wav", 255), CharacterSoundsFinishedCallback, &characterSounds.taz);

	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
	ReduceCharactersHealth(actorInstance, 0.5f);
	RequestRumbleEffect(&controller1, 255, 0.0f, 0.1f, 0.6f, 0.0f, WAVEFORM_NORMAL);
	RequestBuzzerEffect(&controller1, 0.5f, 0.0f, WAVEFORM_RANDOM);

	if(actorInstance->characterInfo->powerUpInfo.invisible)
	{
		EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);
		// NH: baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
		// NH: baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
	}
	else
	{
		EnableActorCartoonRenderMode(&actorInstance->actorInstance);
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateElectricityAttackCollisionWithActor
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int UpdateElectricityAttackCollisionWithActor(ELECTRICITYATTACKINFO *info, ACTORINSTANCE *collider)
{
	MTWEETYWAVEDATA		*wave;
	TBVector			tazPos, separation;
	int		i, j;

	// CMD: check that an attack is atually occuring
	if(info->state)
	{
		bmVectorAdd(tazPos, collider->actorInstance.position, collider->actorBehaviour->collSphere.offset);

		for(j = (info->numStreaks-1);j >= 0;--j)
		{
			wave = &info->waveData[j];
			
			for(i = (info->numPoints-1);i >= 0;--i)
			{
				bmVectorSub(separation, tazPos, wave->streak.tempPoints[i]);
				if(bmVectorLen(separation) < (collider->actorBehaviour->collSphere.radius+METERTOUNIT(0.2f)))
				{
					bkPrintf("hit by electricity 0x%x, at time %f\n", info, gameStatus.gameTime);
					RequestStateChangePlus(collider,STATE_BURNT,myStateChange);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}