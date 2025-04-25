// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : polyexplode.cpp
//   Purpose : functions for exploding objects at poly level
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "scene.h"
#include "util.h"
#include "maths.h"
#include "physics.h"
#include "status.h"
#include "lights.h"
#include "polyeffects.h"
#include "textures.h"

POLYEFFECTSLIST		polyEffectsList;
TBLightsource		polyEffectsLight;


/* --------------------------------------------------------------------------------
   Function : CreatePolyLevelCopyOfActorInstance
   Purpose : Create a copy of actor and place in primlist
   Parameters : 
   Returns : success/fail
   Info : 
*/

ACTORINSTANCE *CreatePolyLevelCopyOfActorInstance(COLLISIONCACHE *collisionCache, POLYEFFECTSLIST *polyEffectsList, ACTORINSTANCE *actorInstance, TBVector offset, int destroyOriginal, TBTexture *texture, uint32 flags)
{
	POLYEFFECTSINFO		*newEntry, *tempPtr;
	COLLISIONLISTENTRY	*ptr;
	COLLISIONLIST		*collisionList = &collisionCache->collisionList;
	TEXCOORDS			*textureCoords;
	int					polyNum, i, red, green, blue, alpha, numTexPolys;
	TBVector			worldPos, tempVector, vertPos;
	float				height, length;

	for (ptr = collisionList->head.next;ptr != &collisionList->head;ptr = ptr->next)
	{
		if ((void*)ptr->actorInstance == (void*)actorInstance)
		{
			if(ptr->numPolys==0)
			{
				bkPrintf("*** WARNING *** actorInstance %s has incomplete collision list entry.\n",ptr->actorInstance->actorName);
				return actorInstance;
			}
			//check that vertices are resident
			//get height of actor
			RemoveActorFromCollisionCache(collisionCache, actorInstance);
			bmVectorCopy(tempVector, actorInstance->actorInstance.position);
			tempVector[1] += METERTOUNIT(1.0f);
			height = GetHeightOfPoint(tempVector,PHOP_EXCLUDEOBJECTS);
			height -= METERTOUNIT(1.0f);

			ptr = AddActorToCollisionCache(collisionCache, actorInstance, 0);
			MakeEntryResident(collisionCache, ptr);

			numTexPolys = 0;
			// CMD: only do this if a texture is specified
			if(texture)
			{
				// CMD: grab texture coords
				// CMD: should be the same no as the polys but just in case
#if BPLATFORM==GAMECUBE
				GetHeaderFromARAM(actorInstance, NULL, &numTexPolys);
#else
				baGetActorGeometry(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,2,NULL,&numTexPolys,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);
#endif
				if(numTexPolys != ptr->numPolys) ASSERTM(NULL, "number of polys in shatter objects does not match!");
				if(numTexPolys)
				{
					textureCoords = (TEXCOORDS*)ZALLOC(sizeof(TEXCOORDS)*numTexPolys);
					ASSERTM(textureCoords, "no memory for poly effects texture coords stream");
#if BPLATFORM==GAMECUBE
					textureCoords = (TEXCOORDS*)GetActorGeometryFromARAM(actorInstance, 2, NULL, textureCoords, 0, 0);
#else
					textureCoords = (TEXCOORDS*)baGetActorGeometry(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,2,NULL,NULL,NULL,textureCoords,0);
#endif
				}
			}


			//loop through vertex list
			for(polyNum = 0;polyNum < ptr->numPolys;polyNum ++)
			{
				//add entry to effects list
				tempPtr = polyEffectsList->head.next;

				//create new structure for incoming information
				if(newEntry = (POLYEFFECTSINFO *)ZALLOC(sizeof(POLYEFFECTSINFO)))
				{
					//fill in attributes
					newEntry->flags = flags;
					newEntry->texture = texture;

					if(~flags & POLYFX_RIPPLE)
					{
						worldPos[0] = ptr->vertices[ptr->polys[polyNum].i1].x + ptr->vertices[ptr->polys[polyNum].i2].x + ptr->vertices[ptr->polys[polyNum].i3].x;
						worldPos[1] = ptr->vertices[ptr->polys[polyNum].i1].y + ptr->vertices[ptr->polys[polyNum].i2].y + ptr->vertices[ptr->polys[polyNum].i3].y;
						worldPos[2] = ptr->vertices[ptr->polys[polyNum].i1].z + ptr->vertices[ptr->polys[polyNum].i2].z + ptr->vertices[ptr->polys[polyNum].i3].z;
						worldPos[3] = 1.0f;
						bmVectorCDiv(worldPos, worldPos, 3.0f);
					}
					short	*index = &ptr->polys[polyNum].i1;
					for(i=0;i<3;++i)
					{
						if(~flags & POLYFX_RIPPLE)
						{
							BDVERTEX_SETXYZW(&newEntry->vertices[i],(ptr->vertices[index[i]].x - worldPos[0]),
								(ptr->vertices[index[i]].y - worldPos[1]),(ptr->vertices[index[i]].z - worldPos[2]),1.0f);
							BDVERTEX_SETNORMAL(&newEntry->vertices[i],ptr->polys[polyNum].nx,
								ptr->polys[polyNum].ny,ptr->polys[polyNum].nz);
							// CMD: temp until Whigham works his magic
							BDVERTEX_SETRGBA(&newEntry->vertices[i],127,127,127,80);
//							BDVERTEX_SETUV(&newEntry->vertices[i], (float)i*0.5f, 0.0f);
							// CMD: endtemp
						}
						else
						{
							BDVERTEX_SETXYZW(&newEntry->vertices[i],ptr->vertices[index[i]].x,ptr->vertices[index[i]].y,
											ptr->vertices[index[i]].z,1.0f);
							BDVERTEX_SETNORMAL(&newEntry->vertices[i],ptr->polys[polyNum].nx,
								ptr->polys[polyNum].ny,ptr->polys[polyNum].nz);
							// CMD: temp until Whigham works his magic
							BDVERTEX_SETRGBA(&newEntry->vertices[i],50,50,50,255);
						}
					}
					if(numTexPolys)
					{
						// CMD: fill in tex coords
						BDVERTEX_SETUV(&newEntry->vertices[0], textureCoords[polyNum].u0, textureCoords[polyNum].v0);
						BDVERTEX_SETUV(&newEntry->vertices[1], textureCoords[polyNum].u1, textureCoords[polyNum].v1);
						BDVERTEX_SETUV(&newEntry->vertices[2], textureCoords[polyNum].u2, textureCoords[polyNum].v2);
					}

					if(~flags & POLYFX_RIPPLE)
					{
						bmVectorSub(newEntry->localPosition, worldPos, actorInstance->actorInstance.position);
						if(offset != NULL)
						{
							bmVectorAdd(newEntry->worldPosition, actorInstance->actorInstance.position, offset);
							newEntry->height = height + newEntry->localPosition[1] + offset[1];
						}
						else
						{
							bmVectorCopy(newEntry->worldPosition, actorInstance->actorInstance.position);
							newEntry->height = height + newEntry->localPosition[1];
						}
					}

//					SETVECTOR(newEntry->orientation, 0.0f, 0.0f, 0.0f, 1.0f);
					bmQuatCopy(newEntry->orientation, actorInstance->actorInstance.orientation);
					newEntry->scale = actorInstance->actorInstance.scale[0];

					//set velocity
					if(flags & POLYFX_EXPLODE)
					{
						bmVectorCopy(tempVector, newEntry->localPosition);
						bmVectorScaleToLength(newEntry->velocity, tempVector, METERTOUNIT(4.8f));
						SETVECTOR(newEntry->rotation, RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),1.0f);
					}
					else
					if(flags & POLYFX_RIPPLE)
					{
						// CMD: set up parameters slightly differently for ripple
						// CMD: assume that object is a vertical plane, and use 'height' as centre of ripple
						bmVectorCopy(newEntry->rotation, gameStatus.player[0].actorInstance->actorInstance.position);
						newEntry->rotation[1] += METERTOUNIT(0.5f);
						
						newEntry->clock = 0.0f;
						for(i = 2;i >=0;--i)
						{
							BDVERTEX_GETXYZW(&newEntry->vertices[i], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
							bmVectorSub(tempVector, vertPos, newEntry->rotation);
							if((length = bmVectorLen(tempVector)) < METERTOUNIT(2.0f))
							{
								newEntry->amp[i] = MARIORIPPLE_MAXAMP - (length/METERTOUNIT(2.0f));
							}
							else
							{
								newEntry->amp[i] = 0.0f;
							}
						}
					}
					else
					{
						SETVECTOR(newEntry->velocity, 0.0f, 0.0f, 0.0f, 1.0f);
						SETVECTOR(newEntry->rotation, 0.0f, 0.0f, 0.0f, 1.0f); 
					}
					newEntry->next = tempPtr;
					newEntry->prev = tempPtr->prev;
					tempPtr->prev->next = newEntry;
					tempPtr->prev = newEntry;
					polyEffectsList->numEntries++;
				}

				ASSERT(newEntry);
			}
			if(numTexPolys)
			{
				FREE(textureCoords);
			}
			//destroy original?
			if(destroyOriginal)
			{
				FreeActorInstance(actorInstance);
				actorInstance = NULL;
			}
			return actorInstance;
		}
	}
	return actorInstance;
}

/* --------------------------------------------------------------------------------
   Function : InitPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitPolyEffectsList(POLYEFFECTSLIST *polyEffectsList)
{
	polyEffectsList->numEntries = 0;
	polyEffectsList->head.next = polyEffectsList->head.prev = &polyEffectsList->head;
}

/* --------------------------------------------------------------------------------
   Function : PrepPolyEffects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void PrepPolyEffects(POLYEFFECTSLIST *polyEffectsList)
{
}

/* --------------------------------------------------------------------------------
   Function : UpdatePolyEffects
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePolyEffects(POLYEFFECTSLIST *polyEffectsList)
{
	POLYEFFECTSINFO		*ptr,*temp;
	TBVector			tempVector, tempRot, vertPos, plane, perp, resultVec;
	TBVector			lightDir = {0.0f, -1.0f, 0.0, 1.0f};
	TBMatrix			matrix;
	float				length, amp, normConst, dot, r, g, b, a;
	int					i;

	for (ptr = polyEffectsList->head.next;ptr != &polyEffectsList->head;)
	{
		temp = ptr->next;
		if(ptr->flags & POLYFX_DROP)
		{
			if((ptr->height >= 0.0f)||(ptr->velocity[1] > 0.0f))
			{
				ApplyGravity(tempVector, ptr->velocity, worldGravity);
				bmVectorAdd(ptr->localPosition, ptr->localPosition, tempVector);
				ptr->height += tempVector[1];
				//update orientation
				bmVectorCMul(tempRot, ptr->rotation, fTime);
				bmVectorAdd(ptr->orientation, ptr->orientation, tempRot);
			}
			else
			{
				if((ptr->velocity[1] > METERTOUNIT(-1.0f))&&(ptr->velocity[1] <= 0.0f))
				{
					RemoveObjectFromPolyEffectsList(polyEffectsList, ptr);
					// TP: you might need this Claude
					ptr = temp;
					continue;
				}
				else
				{
					ptr->velocity[1] *= -0.7f;
					if(ptr->velocity[0] >= 0.0f)
					{
						ptr->velocity[0] = ptr->localPosition[0]/bmFAbs(ptr->localPosition[0])*UtilGenerateRandomNumber(METERTOUNIT(1.0f), 0.0f);
						ptr->velocity[2] = ptr->localPosition[2]/bmFAbs(ptr->localPosition[2])*UtilGenerateRandomNumber(METERTOUNIT(1.0f), 0.0f);
					}
					SETVECTOR(ptr->rotation, RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),RAD(UtilGenerateRandomNumber(360.0f, -360.0f)),1.0f);
				}
			}
		}
		if(ptr->flags & POLYFX_EXPLODE)
		{
			if(ptr->height >= 0.0f)
			{
				ApplyGravity(tempVector, ptr->velocity, worldGravity);
				bmVectorAdd(ptr->localPosition, ptr->localPosition, tempVector);
				ptr->height += tempVector[1];
				//update orientation
				bmVectorCMul(tempRot, ptr->rotation, fTime);
				bmVectorAdd(ptr->orientation, ptr->orientation, tempRot);
			}
			else
			{
				if(bmVectorLen(ptr->velocity) < METERTOUNIT(5.0f))
				{
					RemoveObjectFromPolyEffectsList(polyEffectsList, ptr);
					// TP: you might need this Claude
					ptr = temp;
					continue;
				}
				else
				{
					//shift poly back to surface
					ptr->localPosition[1] -= ptr->height;
					ptr->height = 0.0f;
					ptr->velocity[1] *= -0.7f;
				}
			}
		}
		if(ptr->flags & POLYFX_POPBALL)
		{
			if((ptr->height >= 0.0f)||(ptr->velocity[1] > 0.0f))
			{
				ApplyGravity(tempVector, ptr->velocity, worldGravity);
//				bmVectorAdd(tempVector, ptr->localPosition, tempVector);
				for(i = 0;i < 3;i++)
				{
					BDVERTEX_GETXYZW(&ptr->vertices[i], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
					bmVectorAdd(vertPos, vertPos, tempVector);
					BDVERTEX_SETXYZW(&ptr->vertices[i], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
				}
				ptr->height += tempVector[1];
				//update orientation
//				bmVectorCMul(tempRot, ptr->rotation, fTime);
//				bmVectorAdd(ptr->orientation, ptr->orientation, tempRot);
			}
			else
			{
				if((ptr->velocity[1] > METERTOUNIT(-1.0f))&&(ptr->velocity[1] <= 0.0f))
				{
					RemoveObjectFromPolyEffectsList(polyEffectsList, ptr);
					// TP: you might need this Claude
					ptr = temp;
					continue;
				}
				else
				{
					ptr->velocity[1] *= -0.7f;
//					if(ptr->velocity[0] >= 0.0f)
//					{
//						ptr->velocity[0] = ptr->localPosition[0]/bmFAbs(ptr->localPosition[0])*UtilGenerateRandomNumber(METERTOUNIT(0.5f), 0.0f);
//						ptr->velocity[2] = ptr->localPosition[2]/bmFAbs(ptr->localPosition[2])*UtilGenerateRandomNumber(METERTOUNIT(0.5f), 0.0f);
//					}
//					SETVECTOR(ptr->rotation, RAD((float)(bmRand()%720)-360.0f),RAD((float)(bmRand()%720)-360.0f),RAD((float)(bmRand()%720)-360.0f),1.0f);
				}
			}
		}
		if(ptr->flags & POLYFX_RIPPLE)
		{
			ptr->clock += fTime;

			// CMD: calculate orientation matrix for poly
			bmQuatToMatrix(matrix, ptr->orientation);

			// CMD: do amplitude calculation for each vert
			for(i = 2;i >= 0;--i)
			{
				BDVERTEX_GETXYZW(&ptr->vertices[i], vertPos[0], vertPos[1], vertPos[2], vertPos[3]);
				// CMD: calculate distance from centre
				bmVectorSub(tempVector, vertPos, ptr->rotation);
				length = bmVectorLen(tempVector);
				normConst = bmCos(RAD(180.0f)*UNITTOMETER(length) + RAD(360.0f)*ptr->clock);
				if((ptr->amp[i]-=MARIORIPPLE_AMPDEC*fTime) < 0.0f) ptr->amp[i] = 0.0f;
				amp = ptr->amp[i]*normConst;

				bmVectorSet(ptr->offset[i], 0.0f, 0.0f, amp, 1.0f);
				bmMatMultiplyVector(matrix, ptr->offset[i]);

				// CMD: normals
				bmVectorScaleToLength(perp, ptr->offset[i], 1.0f+normConst*normConst);

				normConst = bmSin(RAD(180.0f)*UNITTOMETER(length) + RAD(360.0f)*ptr->clock);
				bmVectorSub(plane, vertPos, ptr->rotation);
				bmVectorScaleToLength(resultVec, plane, (ptr->amp[i]/MARIORIPPLE_MAXAMP)*normConst);
				bmVectorAdd(plane, resultVec, perp);
				bmVectorNorm(ptr->normals[i], plane);

				// CMD: manual lighting
				dot = bmVectorDot(ptr->normals[i], lightDir);
#define POLYFX_LIGHT_MAGNITUDE	(77.0f)
#define POLYFX_DEFAULT_LEVEL	(60.0f)
				dot = POLYFX_LIGHT_MAGNITUDE - (POLYFX_LIGHT_MAGNITUDE*dot);
				BDVERTEX_SETRGBA(&ptr->vertices[i], bmFloatToInt(POLYFX_DEFAULT_LEVEL+dot), bmFloatToInt(POLYFX_DEFAULT_LEVEL+dot), bmFloatToInt(POLYFX_DEFAULT_LEVEL+dot), 127.0f);
			}
		}
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RemoveObjectFromPolyEffectsList(POLYEFFECTSLIST *polyEffectsList, POLYEFFECTSINFO *remPtr)
{
	POLYEFFECTSINFO  *ptr, *temp;

	if(remPtr == NULL)
	{
		return FALSE;
	}

	for (ptr = polyEffectsList->head.next;ptr != &polyEffectsList->head;)
	{
		if (ptr == remPtr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			polyEffectsList->numEntries--;
		
			temp = ptr->next;
			FREE(ptr);
			ptr = temp;
			return TRUE;
		}
		else
		{
			ptr = ptr->next;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeAllPolyEffectsFromList
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPolyEffectsFromList(POLYEFFECTSLIST *polyEffectsList, uint32 flags)
{
	POLYEFFECTSINFO		  *ptr,*temp;

	bkPrintf("FreeAllPolyFXFromList\n");

	for (ptr = polyEffectsList->head.next;ptr != &polyEffectsList->head;)
	{
		if(ptr->flags & flags)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			polyEffectsList->numEntries--;

			temp = ptr->next;
			FREE(ptr);
			ptr = temp;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RemoveObjectFromPolyEffectsList
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawPolyEffects(POLYEFFECTSLIST *polyEffectsList)
{
	POLYEFFECTSINFO		*ptr;
	TBPrimVertex		verts[3]; //, normalVerts[2];
	TBVector			normal, plane, perp, resultVec;
	TBMatrix			translation,rotation,scale;
	TBMatrix			midway, result;
	TBVector			position;
	float				normConst;
	int					i;

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 1);

	for (ptr = polyEffectsList->head.next;ptr != &polyEffectsList->head;ptr=ptr->next)
	{
		if(ptr->flags & POLYFX_RIPPLE)
		{
			// CMD: add offset
			for(i = 2;i >= 0;--i)
			{
				BDVERTEX_GETXYZW(&ptr->vertices[i], position[0], position[1], position[2], position[3]);
				BDVERTEX_SETXYZW(&verts[i], (position[0]+ptr->offset[i][0]), (position[1]+ptr->offset[i][1]), (position[2]+ptr->offset[i][2]), 1.0f);

				BDVERTEX_SETNORMAL(&verts[i], ptr->normals[i][0], ptr->normals[i][1], ptr->normals[i][2]);

				BDVERTEX_GETRGBA(&ptr->vertices[i], position[0], position[1], position[2], position[3]);
				BDVERTEX_SETRGBA(&verts[i], position[0], position[1], position[2], position[3]);

				BDVERTEX_GETUV(&ptr->vertices[i], position[0], position[1]);
				BDVERTEX_SETUV(&verts[i], position[0], position[1]);

//				BDVERTEX_GETXYZW(&ptr->vertices[i], position[0], position[1], position[2], position[3]);
//				BDVERTEX_SETXYZW(&normalVerts[0], position[0], position[1], position[2], position[3]);
//				BDVERTEX_SETXYZW(&normalVerts[1], (position[0]+(ptr->normals[i][0]*100.0f)), 
//												(position[1]+(ptr->normals[i][1]*100.0f)), 
//												(position[2]+(ptr->normals[i][2]*100.0f)), 1.0f);
//				BDVERTEX_SETRGBA(&normalVerts[0], 127.0f, 127.0f, 127.0f, 127.0f);
//				BDVERTEX_SETRGBA(&normalVerts[1], 127.0f, 127.0f, 127.0f, 127.0f);
//				bdSetTexture(0, NULL);
//				bdDrawPrimitive(BDPRIMTYPE_LINELIST, normalVerts, 2, BVERTTYPE_SINGLE);
			}

			bdSetIdentityObjectMatrix();
			bdSetTexture(0, ptr->texture);
//			baEnableLight(&cartoonLight, FALSE);
//			baEnableLight(&polyEffectsLight, TRUE);
//			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 0);
//			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 1);
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST,&verts,3,BVERTTYPE_SINGLE);
//			baEnableLight(&polyEffectsLight, FALSE);
//			baEnableLight(&cartoonLight, TRUE);
//			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 0);
//			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 1);
	
//			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
//			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 1);
//			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 0);
//			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 1);
//			bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU, 0);
//			bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU, 1);
//			bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPV, 0);
//			bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPV, 1);
		}
		else
		{	
			bmVectorAdd(position, ptr->localPosition, ptr->worldPosition);
			bmMatTranslate(translation, position[0], position[1], position[2]);
			bmMatXYZRotation(rotation, ptr->orientation[0],ptr->orientation[1],ptr->orientation[2]);
			bmMatScale(scale, ptr->scale, ptr->scale, ptr->scale);
			bmMatMultiply(midway, rotation, scale);
			bmMatMultiply(result, translation, midway);
			bdSetObjectMatrix(result);

			bdSetTexture(0, ptr->texture);
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST,&ptr->vertices,3,BVERTTYPE_SINGLE);
		}
	}
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 0);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, 1);
	bdSetIdentityObjectMatrix();
}