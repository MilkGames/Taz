// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mtweetymagnet.cpp
//   Purpose : functions associated with the magnet appearing in the mecha tweety boss game 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "textures.h"
#include "util.h"
#include "mechatweety.h"
#include "electricityattack.h"
#include "mtweetymagnet.h"

TBExtraSystem	*mangaMagnetExtraSystem;
TBPrimVertex2D	*mangaMagnetDrawBuffer;
TBPrimVertex2D	*badMangaMagnetDrawBuffer;

/* --------------------------------------------------------------------------------
   Function : InitMangaMagnetExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/ 
void InitMangaMagnetExtraSystem(void)
{
	mangaMagnetExtraSystem = baCreateExtraSystem("mangaMagnet",MangaMagnetExtraCreateCallback,
		MangaMagnetExtraUpdateCallback,MangaMagnetExtraDeleteCallback,MangaMagnetExtraRenderCallback,
		MangaMagnetExtraSystemInitCallback,sizeof(MANGAMAGNETINFO),MAX_MANGABLOBS,0,0);
}

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    MANGAMAGNETINFO		*data = (MANGAMAGNETINFO*)vData;
	float				*position;
	float				*velocity;

    if(parms)
	{
		position = va_arg(parms, float*);
		velocity = va_arg(parms, float*);
		data->distanceRemaining = *va_arg(parms, float*);
		data->flags = (uint32)va_arg(parms, int);

		bmVectorCopy(data->position, position);
		bmVectorCopy(data->velocity, velocity);
		if(data->flags & MAGNETFLAG_SOURCE)
		{
			data->size = METERTOUNIT(3.0f);
		}
		else
		if(data->flags & MAGNETFLAG_TWEETY)
		{
			data->size = METERTOUNIT(3.0f);
		}
		else
		{
			data->size = UtilGenerateRandomNumber(METERTOUNIT(0.2f), METERTOUNIT(0.05f));
		}
		data->colour[0] = 128;
		data->colour[1] = 128;
		data->colour[2] = 128;
		data->colour[3] = 0;				// CMD: must be set to zero in case magnet is not 'active'
		data->upperAlpha = 0.0f;			// CMD: likewise
		data->clock = 0.2f;
		data->heightFlicker = 0.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int MangaMagnetExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	MANGAMAGNETINFO		*data = (MANGAMAGNETINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	MANGAMAGNETINFO		*data;
	TBVector			posInc;
	TBVector			right, left;
	
	baGetNodesWorldPosition(&globalMechaTweety->actorInstance, baFindNode(globalMechaTweety->actorInstance.rootNodeInstance, "arml_lower"), left);
	baGetNodesWorldPosition(&globalMechaTweety->actorInstance, baFindNode(globalMechaTweety->actorInstance.rootNodeInstance, "armr_lower"), right);

	for (int32 i=0;i<noofInstances;i++)
	{
		data = (MANGAMAGNETINFO*)dataPtrs[i];

		if(data->flags & MAGNETFLAG_SOURCE)
		{
			if(mTweetyGenericInfo->flags & MTWEETY_DRAWMAGNETFX)
			{
				data->colour[3] = (int32)UtilGenerateRandomNumber(200.0f, 0.0f);
			}
			else
			{
				data->colour[3] = 0;
			}
			// CMD: check on state of magnet
//			if(data->flags & MAGNETFLAG_LEFT)
//			{
//				if(mTweetyGenericInfo->magnetsDestroyed[0]&&mTweetyGenericInfo->magnetsDestroyed[1])
//				{
//					data->flags &= ~MAGNETFLAG_GOOD;
//					data->flags |= MAGNETFLAG_BAD;
//				}
//			}
//			if(data->flags & MAGNETFLAG_RIGHT)
//			{
//				if(mTweetyGenericInfo->magnetsDestroyed[2]&&mTweetyGenericInfo->magnetsDestroyed[3])
//				{
//					data->flags &= ~MAGNETFLAG_GOOD;
//					data->flags |= MAGNETFLAG_BAD;
//				}
//			}
		}
		else
		if(data->flags & MAGNETFLAG_TWEETY)
		{
			if(globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_MAGNETCHARGE)
			{
				//fix blobs to tweety's claws, but no need if they aren't drawn
				if(data->flags & MAGNETFLAG_LEFT)
				{
//					if(mTweetyGenericInfo->magnetsDestroyed[0]&&mTweetyGenericInfo->magnetsDestroyed[1])
//					{
//						data->flags &= ~MAGNETFLAG_GOOD;
//						data->flags |= MAGNETFLAG_BAD;
//					}
					bmVectorCopy(data->position, left);
				}
				if(data->flags & MAGNETFLAG_RIGHT)
				{
//					if(mTweetyGenericInfo->magnetsDestroyed[2]&&mTweetyGenericInfo->magnetsDestroyed[3])
//					{
//						data->flags &= ~MAGNETFLAG_GOOD;
//						data->flags |= MAGNETFLAG_BAD;
//					}
					bmVectorCopy(data->position, right);
				}
				data->colour[3] = (int32)UtilGenerateRandomNumber(200.0f, 0.0f);
			}
		}
		else
		{
			//update pollen
			if(data->distanceRemaining > 0.0f)
			{
				//update alpha
				if(mTweetyGenericInfo->flags & MTWEETY_DRAWMAGNETFX)
				{
					if((data->upperAlpha += 512.0f*fTime) > 200.0f) data->upperAlpha = 200.0f;
					if((data->clock -= fTime) < 0.0f)
					{
						data->clock = 0.1f;
						data->colour[3] = (int32)UtilGenerateRandomNumber(data->upperAlpha, 0.0f);
					}
				}
				else
				{
					if((data->upperAlpha -= 512.0f*fTime) < 0.0f) 
					{
						data->upperAlpha = 0.0f;
					}
					if((data->clock -= fTime) < 0.0f)
					{
						data->clock = 0.1f;
						data->colour[3] = (int32)UtilGenerateRandomNumber(data->upperAlpha, 0.0f);
					}
				}
				//increment position
				bmVectorCMul(posInc, data->velocity, fTime);
				bmVectorAdd(data->position, data->position, posInc);
				//'flicker' height
				data->heightFlicker = UtilGenerateRandomNumber(MAGNET_MAX_FLICKER, -MAGNET_MAX_FLICKER);
				//decrement distance remaining
				data->distanceRemaining -= bmFAbs(posInc[0]);
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
   Function : MangaMagnetExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void MangaMagnetExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	MANGAMAGNETINFO		*data;
	int32				i, bufIndex, badBufIndex;
	TBVector			position;
	

	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	badBufIndex = 0;

	for (i=0;i<noofInstances;i++)
	{
		data = (MANGAMAGNETINFO*)dataPtrs[i];
		if(data->flags & MAGNETFLAG_TWEETY)
		{
			if(~globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_MAGNETCHARGE)
			{
				continue;
			}
		}

		bmVectorCopy(position, data->position);
		position[1] += data->heightFlicker;
		position[3] = 1.0f;

		bdSetIdentityObjectMatrix();

		if(data->flags & MAGNETFLAG_GOOD)
		{
			if(bdDrawFlatSprite(position, data->size, data->size, 0.0f, texturePowerUpGlow, 0, 0,
								data->colour[0], data->colour[1], data->colour[2], data->colour[3], &mangaMagnetDrawBuffer[bufIndex], 0.0f))
			{
				bufIndex += 4;
			}
		}
		else
		if(data->flags & MAGNETFLAG_BAD)
		{
			if(bdDrawFlatSprite(position, data->size, data->size, 0.0f, texturePowerUpGlow, 0, 0,
								data->colour[0], data->colour[1], data->colour[2], data->colour[3], &badMangaMagnetDrawBuffer[badBufIndex], 0.0f))
			{
				badBufIndex += 4;
			}
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();

	bdSetTexture(0, textureMechaElec);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, mangaMagnetDrawBuffer, bufIndex,BVERTTYPE_SINGLE2D);

	bdSetTexture(0, textureBadMechaElec);
	if(badBufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, badMangaMagnetDrawBuffer, badBufIndex, BVERTTYPE_SINGLE2D);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : MangaMagnetExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void MangaMagnetExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : InitialiseMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int InitialiseMagnetStreaks(MECHATWEETYINFO *info)
{
	MAGNETSTREAK	*ptr;
	int			i, j, left;

	info->magnetStreak = (MAGNETSTREAK*)ZALLOC(sizeof(MAGNETSTREAK)*NUM_MAGNET_STREAKS);
	ASSERTM(info->magnetStreak, "out of memory");

	left = TRUE;
	for(i = 0;i < NUM_MAGNET_STREAKS;++i)
	{
		ptr = &info->magnetStreak[i];
		if(left)
		{
			bmVectorCopy(ptr->start, mTweetyGenericInfo->magnetNodes[1]);
			bmVectorCMul(ptr->control2, mTweetyGenericInfo->magnetNodes[2], 0.5f);
			bmVectorAdd(ptr->control2, ptr->start, ptr->control2);
			ptr->control2[0] -= METERTOUNIT(5.0f);
			ptr->control2[2] -= METERTOUNIT(1.0f);
			bmVectorCopy(ptr->control1, ptr->control2);
			ptr->control1[2] += METERTOUNIT(0.5f);

			bmVectorCopy(ptr->initC1, ptr->control1);
			bmVectorCopy(ptr->initC2, ptr->control2);

			ptr->node = baFindNode(globalMechaTweety->actorInstance.rootNodeInstance, "arml_lower");
		}
		else
		{
			bmVectorCopy(ptr->start, mTweetyGenericInfo->magnetNodes[0]);
			bmVectorCMul(ptr->control2, mTweetyGenericInfo->magnetNodes[2], -0.5f);
			bmVectorAdd(ptr->control2, ptr->start, ptr->control2);
			ptr->control2[0] += METERTOUNIT(5.0f);
			ptr->control2[2] -= METERTOUNIT(1.0f);
			bmVectorCopy(ptr->control1, ptr->control2);
			ptr->control1[2] += METERTOUNIT(0.5f);

			bmVectorCopy(ptr->initC1, ptr->control1);
			bmVectorCopy(ptr->initC2, ptr->control2);

			ptr->node = baFindNode(globalMechaTweety->actorInstance.rootNodeInstance, "armr_lower");
		}
		SETVECTOR(ptr->jitter, 0.0f, 0.0f, 0.0f, 1.0f);
		ptr->clock = 0.0f;
		ptr->jitterClock = UtilGenerateRandomNumber(1.0f, 0.0f);
		for(j = 0;j < NUM_MAGNET_POINTS;++j)
		{
			ptr->colour[j][0] = ptr->colour[j][1] = ptr->colour[j][2] = 128;
			ptr->colour[j][3] = 200;
			ptr->size[j] = UtilGenerateRandomNumber(METERTOUNIT(0.1f), METERTOUNIT(0.05f));
		}
		ptr->texture = textureMechaElec;
		
		if(left) left = FALSE;
		else left = TRUE;
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMagnetStreaks(MECHATWEETYINFO *info)
{
	MAGNETSTREAK	*ptr;
	TBVector		position, tempVec;

	if(info->flags & MTWEETY_MAGNETCHARGE)
	{
		for(int i = 0;i < NUM_MAGNET_STREAKS;++i)
		{
			ptr = &info->magnetStreak[i];
	
			//update control points
			ptr->clock += fTime;
			ptr->control1[1] = ptr->initC1[1] + METERTOUNIT(1.0f)*bmSin(RAD(30.0f*i)+RAD(180.0f*ptr->clock));
			ptr->control2[1] = ptr->initC2[1] + METERTOUNIT(1.0f)*bmSin(RAD(90.0f)+RAD(30.0f*i)+RAD(180.0f*ptr->clock));
			//calculate points in streak
			baGetNodesWorldPosition(&info->actorInstance->actorInstance, ptr->node, position);
			if((ptr->jitterClock -= fTime) < 0.0f)
			{
				ptr->jitterClock = 1.0f;
				ptr->jitter[1] = UtilGenerateRandomNumber(METERTOUNIT(2.5f), -METERTOUNIT(2.5f));
				ptr->jitter[2] = UtilGenerateRandomNumber(METERTOUNIT(3.0f), -METERTOUNIT(3.0f));
			}
			else
			{
				//scale jitter slowly to zero
				bmVectorCMul(ptr->jitter, ptr->jitter, 0.9f);
			}
			bmVectorAdd(tempVec, ptr->start, ptr->jitter);
			CalcCurve(tempVec, ptr->control1, ptr->control2, position, NUM_MAGNET_POINTS, ptr->points);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawMagnetStreaks(MECHATWEETYINFO *info)
{
	MAGNETSTREAK	*ptr;
	int32			noofVerts;
	int				i, j;
	float			screenSize[NUM_MAGNET_POINTS];

	if(info->flags & MTWEETY_MAGNETCHARGE)
	{
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	
		for(i=0;i<NUM_MAGNET_STREAKS;i++)
		{
			ptr = &info->magnetStreak[i];
	
			for(j = 0;j < NUM_MAGNET_POINTS;j++)
			{
				ptr->points[j][3] = 1.0f;
			}
	
			bdSetIdentityObjectMatrix();
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			noofVerts = bdDrawMultiStreakClipped(NUM_MAGNET_POINTS, &ptr->points[0][0], ptr->size, screenSize,
													&ptr->colour[0][0], ptr->buffer, 0.0f);
	
			if(noofVerts)
			{
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				if(globalMechaTweety->mechaTweetyInfo->flags & MTWEETY_BADELECTRICITY)
				{
					bdSetTexture(0, textureBadMechaElec);
				}
				else
				{
					bdSetTexture(0, ptr->texture);
				}
				bdSetIdentityObjectMatrix();
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, ptr->buffer, noofVerts, BVERTTYPE_SINGLE2D);
			}
		}

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeMagnetStreaks
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeMagnetStreaks(MECHATWEETYINFO *info)
{
	SAFE_FREE(info->magnetStreak);
}