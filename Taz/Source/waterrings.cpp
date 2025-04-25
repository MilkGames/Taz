// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : waterrings.cpp
//   Purpose : functions to create rings around actors while in water
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "waterrings.h"
#include "maths.h"
#include "collisioncache.h"
#include "scene.h"
#include "physics.h"
#include "textures.h"
#include "water.h"
#include "main.h"
#include "util.h"

WATERRINGLIST		waterRingList;

/* --------------------------------------------------------------------------------
   Function : InitialiseWaterRingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseWaterRingList(void)
{
	waterRingList.numEntries = 0;
	waterRingList.head.next = waterRingList.head.prev = &waterRingList.head;
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterRing
   Purpose : creates a water ring, which consists of a texture, mapped onto a couple of poly's
   Parameters : 
   Returns : 
   Info : 
*/

WATERRING *CreateWaterRing(TBVector position, TBTexture *texture, float maxScale, float minScale, float rotation, uint32 flags)
{
	WATERRING		*ring;
	float			yValue;
	TBVector		tempPosition;

	if(minScale >= maxScale) return FALSE;

	if((ring = AddEntryToWaterRingList()) == NULL) return FALSE;

	if(flags & WATERRING_NOHEIGHTOFPOINT)
	{
		bmVectorCopy(ring->initialPosition, position);
	}
	else
	{
		//calculate height of water plane
		bmVectorCopy(tempPosition, position);
		tempPosition[1] += METERTOUNIT(1.0f);
		ProcessHeightOfPoint(&collisionCache, tempPosition, &yValue);
		tempPosition[1] = yValue + 5.0f;
		//fill in attributes
		bmVectorCopy(ring->initialPosition, tempPosition);
	}

	ring->maxSize = METERTOUNIT(maxScale);
	ring->currentSize = METERTOUNIT(minScale);
	ring->alphaRate = (256.0f/(ring->maxSize-ring->currentSize))*WATER_RING_SPEED;
	if(texture)		ring->texture = texture;
	else			ring->texture = textureWaterRing;
//	if(flags & WATERRING_NOHEIGHTOFPOINT)
//	{
//		ring->alpha = 50.0f + UtilGenerateRandomNumber(70.0f, 0.0f);
//	}
	ring->alpha = 127.0f;
	ring->drawRing = FALSE;
	ring->rotation = rotation;
	ring->type = WATEREFFECT_RING;
	ring->mapInstance = NULL;

	// CMD: set square of side minScale*0.5f, centred on origin
	BDVERTEX_SETXYZW(&ring->vertices[0], -ring->currentSize, 0.0f, -ring->currentSize, 1.0f);
	BDVERTEX_SETUV(&ring->vertices[0], 0.0f, 0.0f);
	BDVERTEX_SETXYZW(&ring->vertices[1], ring->currentSize, 0.0f, -ring->currentSize, 1.0f);
	BDVERTEX_SETUV(&ring->vertices[1], 0.0f, 1.0f);
	BDVERTEX_SETXYZW(&ring->vertices[2], -ring->currentSize, 0.0f, ring->currentSize, 1.0f);
	BDVERTEX_SETUV(&ring->vertices[2], 1.0f, 0.0f);
	BDVERTEX_SETXYZW(&ring->vertices[3], ring->currentSize, 0.0f, ring->currentSize, 1.0f);
	BDVERTEX_SETUV(&ring->vertices[3], 1.0f, 1.0f);

	//set normals, and colour
	for(int counter = 3; counter >= 0; --counter)
	{
		BDVERTEX_SETNORMAL(&ring->vertices[counter], 0.0f, 1.0f, 0.0f);
		BDVERTEX_SETRGBA(&ring->vertices[counter], 127.0f, 127.0f, 127.0f, 127.0f);
	}

	return ring;
}

/* --------------------------------------------------------------------------------
   Function : AddEntryToWaterRingList
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

WATERRING *AddEntryToWaterRingList(void)
{
	WATERRING		*newEntry;
	WATERRING		*tempPtr;

	tempPtr = waterRingList.head.next;

	//create new structure for incoming information
	if((newEntry = (WATERRING *)ZALLOC(sizeof(WATERRING))) == NULL)
	{
		bkPrintf("**** ERROR **** water ring MALLOC failed\n");
		return NULL;
	}

	ASSERT(newEntry);

	newEntry->next = tempPtr;
	newEntry->prev = tempPtr->prev;
	tempPtr->prev->next = newEntry;
	tempPtr->prev = newEntry;
	waterRingList.numEntries++;
	return newEntry;
}

/* --------------------------------------------------------------------------------
   Function : UpdateWaterRing
   Purpose : scales ring until big enough for taz
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterRing(void)
{
	WATERRING		*ptr,*temp;
	float			inc;
	TBVector		tempVec;
	ACTORINSTANCE	*mapInstance, *tempInstance;
	
	if(waterRingList.head.next != NULL)
	{
		for (ptr = waterRingList.head.next;ptr != &waterRingList.head;)
		{
			temp = ptr->next;
			switch(ptr->type)
			{
			case WATEREFFECT_RING:
				// CMD: update size
				ptr->currentSize += WATER_RING_SPEED*fTime;

				BDVERTEX_SETXYZW(&ptr->vertices[0], -ptr->currentSize, 0.0f, -ptr->currentSize, 1.0f);
				BDVERTEX_SETXYZW(&ptr->vertices[1], ptr->currentSize, 0.0f, -ptr->currentSize, 1.0f);
				BDVERTEX_SETXYZW(&ptr->vertices[2], -ptr->currentSize, 0.0f, ptr->currentSize, 1.0f);
				BDVERTEX_SETXYZW(&ptr->vertices[3], ptr->currentSize, 0.0f, ptr->currentSize, 1.0f);

				// CMD: update alpha
				if((ptr->alpha -= ptr->alphaRate*fTime) < 0.0f)
				{
					if(ptr->mapInstance)
					{
						for(tempInstance = map.invisibleInstances.head.next;tempInstance != &map.invisibleInstances.head;tempInstance = tempInstance->next)
						{
							if(tempInstance == ptr->mapInstance)
							{
								if(tempInstance->waterRingInfo)
								{
									tempInstance->waterRingInfo->clock = 0.25f;
								}
							}
						}
					}
					DeleteWaterRingFromList(ptr);
					ptr = temp;
					continue;
				}

				BDVERTEX_SETA(&ptr->vertices[0], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[1], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[2], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[3], ptr->alpha);

				ptr->drawRing = TRUE;
				break;
			case WATEREFFECT_RIPPLE:
				if((ptr->life -= fTime) < 0.0f)
				{
					DeleteWaterRingFromList(ptr);
					ptr = temp;
					continue;
				}
				if(ptr->life < 1.0f)
				{
					if((ptr->alpha -= (int)(70.0f*fTime)) < 0)
					{
						ptr->alpha = 0;
						DeleteWaterRingFromList(ptr);
						ptr = temp;
						continue;
					}
				}
				else
				{
					if((ptr->alpha += (int)(70.0f*fTime)) > 70.0f) ptr->alpha = 70;
				}
				BDVERTEX_SETA(&ptr->vertices[0], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[1], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[2], ptr->alpha);
				BDVERTEX_SETA(&ptr->vertices[3], ptr->alpha);
				// CMD: update scale
				bmVectorCAdd(ptr->currentScale, ptr->currentScale, (WATERWIBBLE_SCALERATE*fTime));
				// CMD: update position
				bmVectorCMul(tempVec, ptr->rippleVelocity, fTime);
				bmVectorAdd(ptr->initialPosition, ptr->initialPosition, tempVec);
				break;
			}
			ptr = temp;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawWaterRing
   Purpose : scales ring until big enough for taz
   Parameters : 
   Returns : 
   Info : 
*/

void DrawWaterRing(void)
{
	WATERRING		*ring;
	TBMatrix	scaleMatrix, transMatrix, concMatrix, rotMatrix;

	// PP: enable blending - need this
//	ENABLEBLEND(TRUE);

	if(waterRingList.head.next != NULL)
	{
		// CMD: no ned to replace one expression with a macro
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE,FALSE);

		for (ring = waterRingList.head.next;ring != &waterRingList.head;ring = ring->next)
		{
			if(ring->drawRing == FALSE) continue;

			switch(ring->type)
			{
			case WATEREFFECT_RIPPLE:
				//matrix stuff
				//scale and, perhaps rotate, at origin, and then translate
				bmMatScale(scaleMatrix, ring->currentScale[0], ring->currentScale[1], ring->currentScale[2]);
				bmMatYRotation(rotMatrix, ring->rippleRot); 
				bmMatTranslate(transMatrix, ring->initialPosition[0], ring->initialPosition[1], ring->initialPosition[2]);
				bmMatMultiply(concMatrix, scaleMatrix, rotMatrix);
				bmMatMultiply(rotMatrix, transMatrix, concMatrix);

				bdSetObjectMatrix(rotMatrix);

				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE,BDALPHABLENDMODE_ADDITIVE,0);
				bdSetTexture(0,ring->texture);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, ring->vertices, 4, BVERTTYPE_SINGLE);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE,BDALPHABLENDMODE_BLEND,0);
				break;
			default:
//				bmMatTranslate(transMatrix,gameStatus.player[0].actorInstance->actorInstance.position[0],ring->initialPosition[1],gameStatus.player[0].actorInstance->actorInstance.position[2]);
				bmMatYRotation(scaleMatrix, ring->rotation);
				bmMatTranslate(transMatrix,ring->initialPosition[0],ring->initialPosition[1],ring->initialPosition[2]);
				bmMatMultiply(concMatrix, transMatrix, scaleMatrix);

				bdSetObjectMatrix(concMatrix);

				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
				bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

				bdSetTexture(0,ring->texture);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, ring->vertices, 4, BVERTTYPE_SINGLE);
				break;
			}

			bdSetIdentityObjectMatrix();
		}
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	}
}

/* --------------------------------------------------------------------------------
   Function : DeleteWaterRingFromList
   Purpose : deletes a ring from the list
   Parameters : 
   Returns : 
   Info : 
*/

int DeleteWaterRingFromList(WATERRING *ring)
{
	WATERRING  *ptr;

	if(ring == NULL)
	{
		bkPrintf("**** ERROR **** pointer to be removed from water ring list is NULL");
		return FALSE;
	}

	if(waterRingList.head.next != NULL)
	{
		for (ptr = waterRingList.head.next;ptr != &waterRingList.head;ptr = ptr->next)
		{
			if (ptr == ring)
			{
				ring->texture = NULL;

				ring->prev->next = ptr->next;
				ring->next->prev = ptr->prev;
				waterRingList.numEntries--;
				SAFE_FREE(ring);
				return TRUE;
			}
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : FreeWaterRingList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeWaterRingList(uint32 type)
{
	WATERRING		  *ptr,*temp;

	bkPrintf("FreeWaterRingList\n");

	for (ptr = waterRingList.head.next;ptr != &waterRingList.head;)
	{
		temp = ptr->next;
		if((ptr->type == type)||(type == WATEREFFECT_ALL))
		{
			ptr->texture = NULL;

			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			waterRingList.numEntries--;

			SAFE_FREE(ptr);
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : creates and object (from the map) that emits water rings
   Parameters : 
   Returns : 
   Info : 
*/

WATERRINGOBJECTINFO *CreateWaterRingObject(ACTORINSTANCE *actorInstance, float maxScale, float minScale, uint32 textureNameCRC, float rotation)
{
	actorInstance->special = SPECIALTYPE_WATERRIPPLE;

	actorInstance->waterRingInfo = (WATERRINGOBJECTINFO*)ZALLOC(sizeof(WATERRINGOBJECTINFO));
	ASSERTM(actorInstance->waterRingInfo, "out of memory");

	actorInstance->waterRingInfo->clock = TIME_BETWEEN_MAP_WATER_RINGS;
	actorInstance->waterRingInfo->maxScale = maxScale;
	actorInstance->waterRingInfo->minScale = minScale;
	actorInstance->waterRingInfo->rotation = rotation;
	actorInstance->waterRingInfo->texture = (TBTexture*)textureNameCRC;

	return actorInstance->waterRingInfo;
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateWaterRingObject(ACTORINSTANCE *actorInstance)
{
	WATERRING		*ringInfo;
	TBVector		sep;
	int				i;

	if(actorInstance->waterRingInfo)
	{
		// CMD: only update if taz or she-devil is in the vicinity
		for(i = gameStatus.multiplayer.numPlayers;i > 0;--i)
		{
			bmVectorSub(sep, gameStatus.player[i-1].actorInstance->actorInstance.position, actorInstance->actorInstance.position);
			if(bmVectorLen(sep) < METERTOUNIT(20.0f))
			{
				// CMD: someone is near
				if((actorInstance->waterRingInfo->clock -= fTime) < 0.0f)
				{
					actorInstance->waterRingInfo->clock = TIME_BETWEEN_MAP_WATER_RINGS;
					if(ringInfo = CreateWaterRing(actorInstance->actorInstance.position, actorInstance->waterRingInfo->texture, actorInstance->waterRingInfo->maxScale, 
									actorInstance->waterRingInfo->minScale, actorInstance->waterRingInfo->rotation, WATERRING_NOHEIGHTOFPOINT))
					{
						ringInfo->mapInstance = actorInstance;
					}
				}
				return;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FixUpWaterRingTextures(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_WATERRIPPLE)
		{
			if(ptr->waterRingInfo->texture)
			{
				ptr->waterRingInfo->texture = bkLoadTextureByCRC(NULL, (uint32)ptr->waterRingInfo->texture, 0);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateWaterRipple
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateWaterRipple(ACTORINSTANCE *actorInstance, TBTexture *texture, float maxScale)
{
	WATERRING		*ring;
	TBVector		position, separation, temp;
	TBVector		up = {0.0f, 1.0f,0.0f, 1.0f};
	float			tempFloat;
	int				tempInt;

	position[1] = 0.0f;

	tempInt = (int)(actorInstance->wateryObjectInfo->boundingBox.xmax - actorInstance->wateryObjectInfo->boundingBox.xmin);
	if((tempInt/250.0f) < 7.0f) return FALSE;		//water too small
	position[0] = (float)(bmRand()%tempInt) + actorInstance->wateryObjectInfo->boundingBox.xmin;
	tempInt = (int)(actorInstance->wateryObjectInfo->boundingBox.zmax - actorInstance->wateryObjectInfo->boundingBox.zmin);
	if((tempInt/250.0f) < 7.0f) return FALSE;		//water too small
	position[2] = (float)(bmRand()%tempInt) + actorInstance->wateryObjectInfo->boundingBox.zmin;

	position[3] = 1.0f;

	bmVectorSub(separation, position, gameStatus.player[0].actorInstance->actorInstance.position);
	separation[1] = 0.0f;
	tempFloat = bmVectorLen(separation);
	
	while(tempFloat < METERTOUNIT(7.0f))
	{
		tempFloat += METERTOUNIT(10.0f);
	}
	while(tempFloat > METERTOUNIT(20.0f))
	{
		tempFloat -= METERTOUNIT(10.0f);
	}
	bmVectorScaleToLength(separation, separation, tempFloat);
	bmVectorAdd(position, gameStatus.player[0].actorInstance->actorInstance.position, separation);

	if((ring = AddEntryToWaterRingList()) == NULL) return FALSE;

	// CMD: set square of side minScale*0.5f, centred on origin
	BDVERTEX_SETXYZW(&ring->vertices[0], -METERTOUNIT(0.5f), 0.0f, -METERTOUNIT(0.5f), 1.0f);
	BDVERTEX_SETUV(&ring->vertices[0], 0.0f, 0.0f);
	BDVERTEX_SETXYZW(&ring->vertices[1], METERTOUNIT(0.5f), 0.0f, -METERTOUNIT(0.5f), 1.0f);
	BDVERTEX_SETUV(&ring->vertices[1], 0.0f, 1.0f);
	BDVERTEX_SETXYZW(&ring->vertices[2], -METERTOUNIT(0.5f), 0.0f, METERTOUNIT(0.5f), 1.0f);
	BDVERTEX_SETUV(&ring->vertices[2], 1.0f, 0.0f);
	BDVERTEX_SETXYZW(&ring->vertices[3], METERTOUNIT(0.5f), 0.0f, METERTOUNIT(0.5f), 1.0f);
	BDVERTEX_SETUV(&ring->vertices[3], 1.0f, 1.0f);

	//set normals, and colour
	for(int counter = 3; counter >= 0; --counter)
	{
		BDVERTEX_SETNORMAL(&ring->vertices[counter], 0.0f, 1.0f, 0.0f);
		BDVERTEX_SETRGBA(&ring->vertices[counter], 127.0f, 127.0f, 127.0f, 0.0f);
	}

	position[1] = actorInstance->wateryObjectInfo->initialCollHeight + 3.0f;

	bmVectorCopy(ring->initialPosition, position);
	//bodge position slightly
	bmVectorCross(temp, ring->initialPosition, up);
	bmVectorScaleToLength(temp, temp, (float)((bmRand()%4000)-2000));
	temp[1] = 0.0f;
	bmVectorAdd(ring->initialPosition, ring->initialPosition, temp);

	bmVectorScaleToLength(ring->rippleVelocity, ring->initialPosition, -WATERWIBBLE_SPEED);
	ring->rippleVelocity[1] = 0.0f;
	//find angle to rotate by
	ring->rippleRot = RAD(90.0f) + bmATan((ring->rippleVelocity[0]/ring->rippleVelocity[2]));

	ring->type = WATEREFFECT_RIPPLE;
	ring->alpha = 0;
	ring->drawRing = TRUE;
	ring->life = 3.0f;
	SETVECTOR(ring->currentScale, 0.0f, 0.0f, 0.0f, 1.0f);
	ring->texture = texture;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CheckWaterWibblePositionAgainstOthers
   Purpose : makes sure that positioin isn't going to interfere with any other wibbles
   Parameters : 
   Returns : TRUE if position is ok
   Info : This assumes there is only one water rin list, can't think why another would be needed
*/

int CheckWaterWibblePositionAgainstOthers(TBVector position)
{
	WATERRING		*ptr;
	TBVector		temp;

	for (ptr = waterRingList.head.next;ptr != &waterRingList.head;)
	{
		bmVectorSub(temp, position, ptr->initialPosition);
		if(bmVectorLen(temp) > METERTOUNIT(2.0f)) continue;
		else return FALSE;
	}
	return TRUE;
}