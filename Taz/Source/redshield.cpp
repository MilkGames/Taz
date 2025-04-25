// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : redshield.cpp
//   Purpose : attack move for mecha tweety boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "attach.h"
#include "util.h"
#include "physics.h"
#include "characters.h"
#include "mechatweety.h"
#include "textures.h"
#include "redshield.h"
#include "sfx.h"
#include "Cheats.h"						// PP: Cheats

/* --------------------------------------------------------------------------------
   Function : CreateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

REDSHIELDINFO *CreateRedShieldArroundActor(ACTORINSTANCE *actorInstance, char *nodeName, float size)
{
	REDSHIELDINFO	*ptr;
	SHIELDPASTAINFO	*info;
	SHIELDRINGSINFO	*ring;
	TBVector		position, scale, nodePos;
	TBMatrix		matrix;
	float			tempFloat;
	int				i, j;
	
	ptr = (REDSHIELDINFO*)ZALLOC(sizeof(REDSHIELDINFO));
	ASSERTM(ptr, "out of memory");

	SETVECTOR(position, 0.0f, 0.0f, 0.0f, 1.0f);
	tempFloat = UNITTOMETER(size);
	SETVECTOR(scale, tempFloat, tempFloat, tempFloat, 1.0f);

	ptr->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, nodeName);
	if(ptr->node) baGetNodesWorldPosition(&actorInstance->actorInstance, ptr->node, nodePos);

	ptr->handle = AttachObjectToCharacter(actorInstance, "extras\\tweetyshield.obe", nodeName, position, scale, bIdentityQuaternion, 0);
	ptr->actorInstance = ReturnAttachedObjectInstance(actorInstance, ptr->handle);
	DisableActorCartoonRenderMode(ptr->actorInstance);
	SetAttachedObjectAlpha(actorInstance, ptr->handle, 50.0f);

	ptr->state = REDSHIELD_ACTIVE;
	ptr->clock = 0.0f;
	ptr->scale = ptr->initialScale = tempFloat;
	ptr->drawShield = TRUE;
	ptr->flags = 0;
	ptr->soundHandle = PlayTrackingSample("loop\\tweetyshieldloop.wav", 255, actorInstance->actorInstance.position);
	ptr->shieldOwner = actorInstance;

	// CMD: create pasta based effect
	// CMD: spaghetti (streak) emminating from the centre to edge of sphere
	// CMD: control points are at radius/2, radius, so that the streak pokes out from inside the shield
	info = ptr->spaghetti;
	for(i = NUM_SHIELD_SPAGHETTI;i > 0;--i)
	{
		info->clock = 0.0f;
		info->ctrlRadius[0] = info->ctrlInitRad[0] = size;
		info->ctrlRadius[1] = info->ctrlInitRad[1] = size*2.5f;
		info->ctrlRadius[2] = info->ctrlInitRad[2] = size*2.0f;
		for(j=2;j>=0;--j)
		{
			bmQuatCopy(info->ctrlQuats[j], bIdentityQuaternion);
			bmQuatCopy(info->ctrlTargetQuats[j], bIdentityQuaternion);
			info->ctrlRotSpeed[j] = UtilGenerateRandomNumber(MAX_SPAGHETTI_SPEED, MIN_SPAGHETTI_SPEED);

			bmQuatToMatrix(matrix, info->ctrlQuats[j]);
			bmVectorSet(scale, 0.0f, info->ctrlRadius[j], 0.0f, 1.0f);
			bmMatMultiplyVector(matrix, scale);
			bmVectorAdd(info->ctrlPts[j], scale, nodePos);
		}
		for(j = (NUM_SHIELD_POINTS-1);j >=0;--j)
		{
			info->colour[j][0] = 100;
			info->colour[j][1] = 22;
			info->colour[j][2] = 119;
			info->colour[j][3] = 200;

			info->size[j] = METERTOUNIT(0.1f);
		}
		info++;
	}
	ring = ptr->shieldRings;
	for(i = 3;i > 0;i--)
	{
		ring->clock = 0.0f;
		ring->rotation = 0.0f;
		ring->rotSpeed = RAD(UtilGenerateRandomNumber(180.0f, 100.0f));

		tempFloat = 0.0f;
		for(j = (NUM_RING_POINTS-1);j >= 0;--j)
		{
			ring->size[j] = SHIELD_RING_WIDTH;
			ring->colour[j][0] = 100;
			ring->colour[j][1] = 100;
			ring->colour[j][2] = 127;
			ring->colour[j][3] = 100;
			switch(i)
			{
			case 1:
				bmVectorSet(ring->initPoints[j], 2.0f*size*bmSin(tempFloat), 2.0f*size*bmCos(tempFloat), 0.0f, 1.0f);
				break;
			case 2:
				bmVectorSet(ring->initPoints[j], 2.0f*size*bmSin(tempFloat), 0.0f, 2.0f*size*bmCos(tempFloat), 1.0f);
				break;
			case 3:
				bmVectorSet(ring->initPoints[j], 0.0f, 2.0f*size*bmSin(tempFloat), 2.0f*size*bmCos(tempFloat), 1.0f);
				break;
			}
			tempFloat += RAD(360.0f/(NUM_RING_POINTS-1));

			ring->blobClock = UtilGenerateRandomNumber(5.0f, 2.0f);
			ring->blobPoint = bmRand()%NUM_RING_POINTS;
			ring->initialBlobSize = ring->size[ring->blobPoint];
			ring->blobSize = UtilGenerateRandomNumber(METERTOUNIT(0.2f), METERTOUNIT(0.1f));
		}
		ring++;
	}

	return ptr;
}

/* --------------------------------------------------------------------------------
   Function : UpdateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateRedShieldArroundActor(ACTORINSTANCE *actorInstance, REDSHIELDINFO *info)
{
	SHIELDPASTAINFO		*spag;
	SHIELDRINGSINFO		*ringInfo;
	TBMatrix			matrix, rot, result;
	TBVector			tempVector, nodePos, length;
	float				tempFloat;
	int					i, j;

	if(info)
	{
		// CMD: update pasta based effects
		baGetNodesWorldPosition(&actorInstance->actorInstance, info->node, nodePos);
		spag = info->spaghetti;
		for(i = NUM_SHIELD_SPAGHETTI;i > 0;--i)
		{
			switch(info->state)
			{
			case REDSHIELD_ACTIVE:
				for(j = 2;j >= 0;j--)
				{
					if((spag->ctrlRadius[j] += METERTOUNIT(8.0f)*fTime) > spag->ctrlInitRad[j])
					{
						spag->ctrlRadius[j] = spag->ctrlInitRad[j];
						info->drawShield = TRUE;
					}
				}
				break;
			case REDSHIELD_INACTIVE:
				for(j = 2;j >= 0;j--)
				{
					info->drawShield = FALSE;
					if((spag->ctrlRadius[j] -= METERTOUNIT(8.0f)*fTime) < 0.0f)
					{
						spag->ctrlRadius[j] = 0.0f;
					}
				}
				break;
			}

			// CMD: increment clock
			spag->clock += fTime;
			for(j = (NUM_SHIELD_POINTS-1);j >= 0;--j)
			{
				// CMD: update size so that energy seem to be pulsing along streak
//				tempFloat = bmSin(RAD(30.0f*(5.2f*(float)j + spag->clock)));
//				spag->size[j] = METERTOUNIT(0.1f); // + METERTOUNIT(0.025f)*tempFloat*tempFloat;
				bmVectorSub(length, spag->points[j], nodePos);
				spag->colour[j][3] = (int)(UNITTOMETER(bmVectorLen(length))/spag->ctrlInitRad[3]*100.0f);
				if(spag->colour[j][3] < 0) spag->colour[j][3] = 0;
				if(spag->colour[j][3] >200) spag->colour[j][3] = 200;
			}
			for(j = 2;j >= 0;--j)
			{
				// CMD: slerp quaternions
				TBQuaternion tempQuat;
				bmQuatCopy(tempQuat, spag->ctrlQuats[j]);
				if(QuatSlerpAtFixedSpeed(spag->ctrlQuats[j], tempQuat, spag->ctrlTargetQuats[j], spag->ctrlRotSpeed[j]*fTime))
				{
					// CMD: choose new random orientation
					do
					{
						spag->ctrlTargetQuats[j][0] = UtilGenerateRandomNumber(1.0f, -1.0f);
						spag->ctrlTargetQuats[j][1] = UtilGenerateRandomNumber(1.0f, -1.0f);
						spag->ctrlTargetQuats[j][2] = UtilGenerateRandomNumber(1.0f, -1.0f);
						spag->ctrlTargetQuats[j][3] = UtilGenerateRandomNumber(1.0f, -1.0f);
					} while(!(spag->ctrlTargetQuats[j][0]||spag->ctrlTargetQuats[j][1]||spag->ctrlTargetQuats[j][2]||spag->ctrlTargetQuats[j][3]));
					utilNormaliseQuaternion(spag->ctrlTargetQuats[j]);
					spag->ctrlRotSpeed[j] = UtilGenerateRandomNumber(MAX_SPAGHETTI_SPEED, MIN_SPAGHETTI_SPEED);
				}
				// CMD: calculate control points
				bmQuatToMatrix(matrix, spag->ctrlQuats[j]);
				bmVectorSet(tempVector, 0.0f, spag->ctrlRadius[j], 0.0f, 1.0f);
				bmMatMultiplyVector(matrix, tempVector);
				bmVectorAdd(spag->ctrlPts[j], tempVector, nodePos);
				if(!j)
				{
					// CMD: calculate start pos
					bmVectorSet(tempVector, 0.0f, spag->ctrlRadius[j]*0.5f, 0.0f, 1.0f);
					bmMatMultiplyVector(matrix, tempVector);
					bmVectorAdd(spag->startPoint, tempVector, nodePos);
				}
			}
			CalcCurve(spag->startPoint, spag->ctrlPts[0], spag->ctrlPts[1], spag->ctrlPts[2], NUM_SHIELD_POINTS, spag->points);
			spag++;
		}
		ringInfo = info->shieldRings;
		for(i = 3;i > 0;i--)
		{
			for(j = (NUM_RING_POINTS-1);j >= 0;--j)
			{
				switch(i)
				{
				case 1:
					bmMatXRotation(matrix, ringInfo->rotation*0.5f);
					bmMatZRotation(rot, ringInfo->rotation);
					bmMatMultiply(result, matrix, rot);
					break;
				case 2:
					bmMatZRotation(matrix, ringInfo->rotation*0.5f);
					bmMatYRotation(rot, ringInfo->rotation);
					bmMatMultiply(result, matrix, rot);
					break;
				case 3:
					bmMatYRotation(matrix, ringInfo->rotation*0.5f);
					bmMatXRotation(rot, ringInfo->rotation);
					bmMatMultiply(result, matrix, rot);
					break;
				}
				bmVectorCopy(ringInfo->points[j], ringInfo->initPoints[j]);
				bmMatMultiplyVector(result, ringInfo->points[j]);
				bmVectorAdd(ringInfo->points[j], ringInfo->points[j], nodePos);
			}
			// CMD: update rotation
			if((ringInfo->rotation += ringInfo->rotSpeed*fTime) > RAD(360.0f)) ringInfo->rotation -= RAD(360.0f);

			// CMD: update blobs
			if((ringInfo->blobSize -= METERTOUNIT(0.3f)*fTime) < ringInfo->initialBlobSize)
			{
				ringInfo->blobSize = ringInfo->initialBlobSize;

				if((ringInfo->blobClock -= fTime) < 0.0f)
				{
					ringInfo->blobClock = UtilGenerateRandomNumber(5.0f, 2.0f);
					// CMD: chose a point to blob
					ringInfo->blobPoint = bmRand()%NUM_RING_POINTS;
					ringInfo->initialBlobSize = ringInfo->size[ringInfo->blobPoint];
					ringInfo->blobSize = UtilGenerateRandomNumber(METERTOUNIT(0.5f), METERTOUNIT(0.25f));
				}
			}
			// CMD: overwrite blob size to correct point
			ringInfo->size[ringInfo->blobPoint] = ringInfo->blobSize;

			ringInfo++;
		}
		// CMD: update shield actor alpha
		switch(info->state)
		{
		case REDSHIELD_ACTIVE:
			if(info->flags & REDSHIELD_FLICKER) SetAttachedObjectAlpha(actorInstance, info->handle, UtilGenerateRandomNumber(50.0f, 0.0f));
			else SetAttachedObjectAlpha(actorInstance, info->handle, 50.0f);
			break;
		case REDSHIELD_INACTIVE:
			if(info->flags & REDSHIELD_FLICKER) info->flags &= ~REDSHIELD_FLICKER;
			SetAttachedObjectAlpha(actorInstance, info->handle, 0.0f);
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawRedShieldSpaghetti
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void DrawRedShieldSpaghetti(ACTORINSTANCE *actorInstance, REDSHIELDINFO *info)
{
	SHIELDPASTAINFO		*spagInfo;
	SHIELDRINGSINFO		*ringInfo;
	int32				noofVerts;
	int					i, j, k;
	float				screenSize[NUM_RING_POINTS];

	// CMD: check for no draw during cutscenes
	if(actorInstance->flags & ACTORFLAG_DONTDRAW) return;

#ifndef INFOGRAMES
#ifdef _DEBUG
	if(showDebugInfo)
	{
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

		switch(info->state)
		{
		case REDSHIELD_ACTIVE:
			strcpy(stringBuf, "Shield Active");
			break;
		case REDSHIELD_INACTIVE:
			strcpy(stringBuf, "Shield Inactive");
			break;
		}

		bkString8to16(string16Buf, stringBuf);
		bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), 400.0f, 250.0f, 127,127,127, 127, NULL, NULL);

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	}
#endif
#endif

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	spagInfo = info->spaghetti;
	for(j=NUM_SHIELD_SPAGHETTI;j>0;--j)
	{
		for(k = 0;k < NUM_SHIELD_POINTS;k++)
		{
			spagInfo->points[k][3] = 1.0f;
		}

		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		noofVerts = bdDrawMultiStreakClipped(NUM_SHIELD_POINTS, &spagInfo->points[0][0], spagInfo->size, screenSize,
												&spagInfo->colour[0][0], global2DVertexBuffer, 0.0f);

		if(noofVerts)
		{
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
			bdSetTexture(0, textureMechaElec);
			bdSetIdentityObjectMatrix();
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofVerts, BVERTTYPE_SINGLE2D);
		}
		spagInfo++;
	}

	if(info->drawShield)
	{
		ringInfo = info->shieldRings;
		for(j = 3;j > 0;--j)
		{
			for(k = 0;k < NUM_RING_POINTS;k++)
			{
				ringInfo->points[k][3] = 1.0f;
			}
			bdSetIdentityObjectMatrix();
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			noofVerts = bdDrawMultiStreakClipped(NUM_RING_POINTS, &ringInfo->points[0][0], ringInfo->size, screenSize,
													&ringInfo->colour[0][0], global2DVertexBuffer, 0.0f);

			if(noofVerts)
			{
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdSetTexture(0, textureMechaElec);
				bdSetIdentityObjectMatrix();
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, global2DVertexBuffer, noofVerts, BVERTTYPE_SINGLE2D);
			}
			ringInfo++;
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : RequestRedShieldStateChange
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void RequestRedShieldStateChange(REDSHIELDINFO *info, EShieldState newState)
{
	info->clock = 0.0f;
	info->state = newState;
	switch(newState)
	{
	case REDSHIELD_ACTIVE:
		info->soundHandle = PlayTrackingSample("loop\\tweetyshieldloop.wav", 255, info->shieldOwner->actorInstance.position);
		break;
	case REDSHIELD_INACTIVE:
		StopChannel(&info->soundHandle);
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateRedShieldArroundActor
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void DestroyRedShieldArroundActor(REDSHIELDINFO *info)
{
	if(info)
	{
		RemoveObjectFromCharacter(globalMechaTweety, info->handle);
		SAFE_FREE(info);
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void CreateTazShield(ACTORINSTANCE *attachee, int32 initialHealth)
{
	TAZSHIELDINFO	*info;
	TBActorInstance	*instance;
	TBVector		scale, offset;

	if(!gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo)
	{
		gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo = (TAZSHIELDINFO*)ZALLOC(sizeof(TAZSHIELDINFO));
		ASSERTM(gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo, "out of memory for taz shield");

		info = gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo;
		info->currentScale = info->initialScale = 1.5f;
		SETVECTOR(scale, info->initialScale, info->initialScale, info->initialScale, 1.0f);
		SETVECTOR(offset, 0.0f, 0.0f, 0.0f, 1.0f);
	
		gameStatus.player[0].actorInstance->characterInfo->tazShieldInfo->tazShieldHandle = AttachObjectToCharacter(attachee, "crashhelmet.obe", "spinetop", offset, scale, bIdentityQuaternion, ATTACHFLAG_INHERITSCALE);// PP: REMOUT: "extras\\..." (now using 'crashhelmet' package)
	
		instance = ReturnAttachedObjectInstance(attachee, info->tazShieldHandle);
		DisableActorCartoonRenderMode(instance);
	
		if((info->health = initialHealth) > 9) info->health = 9;
		info->shieldGif = bkLoadTexture(NULL, "crashhelmet_numbers01.gif", 0);// PP: REMOUT: "extras\\..." (now using 'crashhelmet' package)
		bdSetTextureFrame(info->shieldGif, info->shieldGifFrame = (info->health-1));
	}
}

/* --------------------------------------------------------------------------------
   Function : ReattachTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

void ReattachTazShield(ACTORINSTANCE *attachee, TAZSHIELDINFO *shieldInfo)
{
	TBVector			scale, offset;
	TBActorInstance		*instance;

	if(shieldInfo->health)
	{
		SETVECTOR(scale, shieldInfo->currentScale, shieldInfo->currentScale, shieldInfo->currentScale, 1.0f);
		SETVECTOR(offset, 0.0f, 0.0f, 0.0f, 1.0f);
	
		if((shieldInfo->tazShieldHandle = AttachObjectToCharacter(attachee, "crashhelmet.obe", "spinetop", offset, scale, bIdentityQuaternion, ATTACHFLAG_INHERITSCALE)) != -1)
		{
			instance = ReturnAttachedObjectInstance(attachee, shieldInfo->tazShieldHandle);
			DisableActorCartoonRenderMode(instance);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ReInitialiseShieldHealth
   Purpose : puts helmet back to its initial state
   Parameters : 
   Returns :
   Info : 
*/

void ReInitialiseShieldHealth(ACTORINSTANCE *attachee, int32 initialHealth)
{
	TAZSHIELDINFO	*info = attachee->characterInfo->tazShieldInfo;
	TBActorInstance	*instance;
	TBVector		scale;

	if(info)
	{
		info->currentScale = info->initialScale;
		if((info->health = initialHealth) > 9) info->health = 9;
		bdSetTextureFrame(info->shieldGif, info->shieldGifFrame = (info->health-1));

		SETVECTOR(scale, info->initialScale, info->initialScale, info->initialScale, 1.0f);
		if(instance = ReturnAttachedObjectInstance(attachee, info->tazShieldHandle))
		{
			bmVectorCopy(instance->scale, scale);
		}
		else ReattachTazShield(attachee, info);
	}
}

/* --------------------------------------------------------------------------------
   Function : ReduceShieldHealthByOne
   Purpose : creates
   Parameters : 
   Returns : return value is health of Taz, which is one greater than the health of his helmet, convoluted? ... well yes
				but it makes the test more straightforward.
   Info : 
*/

int32 ReduceShieldHealthByOne(ACTORINSTANCE *attachee)
{
	TAZSHIELDINFO		*shieldInfo;
	TBActorInstance		*instance;
	float				scale;

	// PP: enforce invincibility cheat
	if(CheatActive(CHEAT_INVINCIBLE))
	{
		return TRUE;
	}

	// CMD: scale helmet
	if(!(shieldInfo = attachee->characterInfo->tazShieldInfo)) return FALSE;		// CMD: must be dead, no shield!

	shieldInfo->currentScale -= (shieldInfo->initialScale*0.5f)/MTWEETY_NUMHITS;
	// CMD: if taz has shield attached we can apply the new scale now
	// CMD: otherwise (if he is spinning) we need to wait
	instance = ReturnAttachedObjectInstance(attachee, shieldInfo->tazShieldHandle);
	if(instance)
	{
		bmVectorSet(instance->scale, shieldInfo->currentScale, shieldInfo->currentScale, shieldInfo->currentScale, 1.0f);
	}

	// CMD: update gif
	bdSetTextureFrame(shieldInfo->shieldGif, --shieldInfo->shieldGifFrame);
	
	// CMD: update health, and check for death
	if(shieldInfo->health--)
	{
		if(shieldInfo->health == 0)
		{
			RemoveObjectFromCharacter(attachee, shieldInfo->tazShieldHandle);
		}
		return shieldInfo->health+1;				// CMD: not dead yet
	}
	else
	{
		attachee->characterInfo->tazShieldInfo = FreeTazShield(shieldInfo);
		return 0;								// CMD: dead
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeTazShield
   Purpose : creates
   Parameters : 
   Returns : 
   Info : 
*/

TAZSHIELDINFO *FreeTazShield(TAZSHIELDINFO *tazShieldInfo)
{
	if(tazShieldInfo) SAFE_FREE(tazShieldInfo);

	return tazShieldInfo;
}