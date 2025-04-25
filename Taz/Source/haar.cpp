// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : haar.cpp
//   Purpose : functions to create a mist effect which is pleasing to the eye
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "main.h"
#include "status.h"
#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "textures.h"
#include "haar.h"

TBExtraSystem	*haarExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateHaarObject
   Purpose : create an object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

int CreateHaarObject(ACTORINSTANCE *actorInstance, float density, float r, float g, float b)
{
	TBMatrix		tempMatrix;
	COLLISIONLISTENTRY	*collPtr;
	float				xLength, zLength;
	int					i;

	actorInstance->haarInfo = (HAARINFO*)ZALLOC(sizeof(HAARINFO));
	if(actorInstance->haarInfo)
	{
		SETVECTOR(actorInstance->haarInfo->direction, 0.0f,0.0f, -1.0f, 1.0f);
		bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
		bmMatMultiplyVector(tempMatrix, actorInstance->haarInfo->direction);
		bmVectorScaleToLength(actorInstance->haarInfo->direction, actorInstance->haarInfo->direction, HAAR_SPEED);

		collPtr = AddActorToCollisionCache(&collisionCache, actorInstance, 0);
		MakeEntryResident(&collisionCache, collPtr);
		if(collPtr->numPolys != 2)
		{
			bkPrintf("*** WARNING *** collision list entry %s has incomplete entry\n", collPtr->actorInstance->actorName);
			SAFE_FREE(actorInstance->haarInfo);
			return FALSE;
		}
		//grab first six verts, there should be no more than two polys anyway
		if(collPtr->numPolys != 2) return FALSE;				//Auburn has some explaining to do

		short	*index = &collPtr->polys[0].i1;
		for(i=0;i<3;i++)
		{
			BDVERTEX_SETXYZW(&actorInstance->haarInfo->verts[i], collPtr->vertices[index[i]].x, collPtr->vertices[index[i]].y, collPtr->vertices[index[i]].z, 1.0f);
			BDVERTEX_SETNORMAL(&actorInstance->haarInfo->verts[i], collPtr->polys[0].nx, collPtr->polys[0].ny, collPtr->polys[0].nz);
			BDVERTEX_SETRGBA(&actorInstance->haarInfo->verts[i], r, g, b, HAAR_ALPHA);
		}
		index = &collPtr->polys[1].i1;
		for(i=3;i<6;i++)
		{
			BDVERTEX_SETXYZW(&actorInstance->haarInfo->verts[i], collPtr->vertices[index[i-3]].x, collPtr->vertices[index[i-3]].y, collPtr->vertices[index[i-3]].z, 1.0f);
			BDVERTEX_SETNORMAL(&actorInstance->haarInfo->verts[i], collPtr->polys[1].nx, collPtr->polys[1].ny, collPtr->polys[1].nz);
			BDVERTEX_SETRGBA(&actorInstance->haarInfo->verts[i], r, g, b, HAAR_ALPHA);
		}
		xLength = UNITTOMETER(bmFAbs(actorInstance->actorInstance.actor->xMax - actorInstance->actorInstance.actor->xMin))*0.25f;
		zLength = UNITTOMETER(bmFAbs(actorInstance->actorInstance.actor->zMax - actorInstance->actorInstance.actor->zMin))*0.25f;

		density *= 0.5f;

		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[0], 0.0f*density*xLength, 1.0f*density*zLength);
		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[1], 0.0f*density*xLength, 0.0f*density*zLength);
		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[2], 1.0f*density*xLength, 0.0f*density*zLength);

		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[3], 0.0f*density*xLength, 1.0f*density*zLength);
		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[4], 1.0f*density*xLength, 0.0f*density*zLength);
		BDVERTEX_SETUV(&actorInstance->haarInfo->verts[5], 1.0f*density*xLength, 1.0f*density*zLength);

		RemoveActorFromCollisionCache(&collisionCache, actorInstance);
		baCreateExtra(haarExtraSystem, actorInstance->actorInstance.position, actorInstance->haarInfo->direction, actorInstance->haarInfo->verts, &density);
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeHaarObject
   Purpose : updates the object which controls a bank of mist
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHaarObject(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->haarInfo)
	{
		SAFE_FREE(actorInstance->haarInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : ChooseRandomPositionWithinBoundingBox
   Purpose : chooses a random position within the bounding box of an actor
   Parameters : 
   Returns : 
   Info : 
*/

void ChooseRandomPositionWithinBoundingBox(TBVector dest, TBActorInstance *actorInstance)
{
	int			size[3];
	TBVector	local;

	size[0] = (int)((actorInstance->actor->xMax - actorInstance->actor->xMin)*actorInstance->scale[0]);
	size[1] = (int)(actorInstance->actor->yMin*actorInstance->scale[1]);
	size[2] = (int)((actorInstance->actor->zMax - actorInstance->actor->zMin)*actorInstance->scale[2]);

	local[0] = (float)(bmRand()%size[0]);
	local[1] = (float)(size[1]);
	local[2] = (float)(bmRand()%size[2]);
	local[3] = 1.0f;

	bmVectorAdd(dest, local, actorInstance->position);
}

/* --------------------------------------------------------------------------------
   Function : InitHaarExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitHaarExtraSystem(void)
{
	haarExtraSystem = baCreateExtraSystem("haar",HaarExtraCreateCallback,
		HaarExtraUpdateCallback,HaarExtraDeleteCallback,HaarExtraRenderCallback,
		HaarExtraSystemInitCallback,sizeof(HAAREXTRAINFO),MAX_HAAR,0,0);
}

/* --------------------------------------------------------------------------------
   Function : HaarExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    HAAREXTRAINFO		*data = (HAAREXTRAINFO*)vData;
	float				*position;
	float				*direction;
	TBPrimVertex		*verts;
	float				x, y, z, w, xn, yn, zn;
#if BPLATFORM == PS2
	float				r, g, b, a;
#else
	int					r, g, b, a;
#endif


    if(parms)
	{
		position = va_arg(parms, float*);
		direction = va_arg(parms, float*);
		verts = va_arg(parms, TBPrimVertex*);
		data->density = *va_arg(parms, float*);

		bmVectorCopy(data->position, position);
		bmVectorCopy(data->velocity, direction);
		for(int i = 0;i < 6;i++)
		{
			BDVERTEX_GETXYZW(&verts[i], x, y, z, w);
			BDVERTEX_GETNORMAL(&verts[i], xn, yn, zn);
			BDVERTEX_GETRGBA(&verts[i], r, g, b, a);
			BDVERTEX_GETUV(&verts[i], data->u[i], data->v[i]);

			BDVERTEX_SETXYZW(&data->verts[i], x, y, z, w);
			BDVERTEX_SETNORMAL(&data->verts[i], xn, yn, zn);
			BDVERTEX_SETRGBA(&data->verts[i], r, g, b, a);
			BDVERTEX_SETUV(&data->verts[i], data->u[i], data->v[i]);
		}
		data->life = 10.0f;
#if BPLATFORM == PS2
		data->a = 0.0f;
		data->r = data->g = data->b = 128.0f;
#else
		data->a = 0;
		data->r = data->g = data->b = 128;
#endif

		data->scale = HAAR_SCALE;
		data->alphaState = 0;
	}
}

/* --------------------------------------------------------------------------------
   Function : HaarExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int HaarExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	HAAREXTRAINFO *data = (HAAREXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : HaarExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	HAAREXTRAINFO		*data;
	float				u, v;
	
	for (int32 i=0;i<noofInstances;i++)
	{
		data = (HAAREXTRAINFO*)dataPtrs[i];

		//scroll texture coords in direction of velocity
		for(int i = 5;i >= 0; --i)
		{
			BDVERTEX_GETUV(&data->verts[i], u, v);

//			if((u += 0.1f*fTime) > (data->u[i]+1.0f)) u -= 1.0f;

			u += 0.1f*fTime;
			while(u > (data->u[i]+1.0f))
			{
				u -= 1.0f;
			}

			BDVERTEX_SETUV(&data->verts[i], u, v);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void HaarExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	HAAREXTRAINFO	*data;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	for (int i=0;i<noofInstances;i++)
	{
		data = (HAAREXTRAINFO*)dataPtrs[i];

		bdSetIdentityObjectMatrix();
		bdSetTexture(0, textureHaar);
//		bdSetTexture(0, textureSwirl);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST, &data->verts, 6, BVERTTYPE_SINGLE);
	}

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU, 1);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPV, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPV, 1);
}

/* --------------------------------------------------------------------------------
   Function : FireworksExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void HaarExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}
