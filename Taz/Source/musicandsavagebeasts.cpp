// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : musicandsavagebeasts.cpp
//   Purpose : functions to sooth the savage beast Taz
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "status.h"
#include "animation.h"
#include "collision.h"
#include "collisioncache.h"
#include "actors.h"
#include "musicandsavagebeasts.h"

TBExtraSystem	*staveExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateMusicBoxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateMusicBoxInstance(ACTORINSTANCE *actorInstance, float radius, uint32 boundaryCRC, uint32 state)
{
	float		zero = 0.0f;

	actorInstance->special = SPECIALTYPE_MUSICBOX;
	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	actorInstance->musicBoxInfo = (MUSICBOXINFO*)ZALLOC(sizeof(MUSICBOXINFO));
	ASSERTM(actorInstance->musicBoxInfo, "musicBoxInfo could not be malloced");

	actorInstance->musicBoxInfo->radius = radius;
	actorInstance->musicBoxInfo->boundaryCRC = boundaryCRC;
	actorInstance->musicBoxInfo->clock = MUSICBOX_NOTEFREQ;
	actorInstance->musicBoxInfo->crotchet = bkLoadTexture(NULL, "crotchet.tga", 0);
	actorInstance->musicBoxInfo->quaver = bkLoadTexture(NULL, "quaver.tga", 0);
	actorInstance->musicBoxInfo->semiquaver = bkLoadTexture(NULL, "semiquaver.tga", 0);
	actorInstance->musicBoxInfo->numPolys = 0;
	actorInstance->musicBoxInfo->verts = NULL;
	actorInstance->musicBoxInfo->polys = NULL;
	actorInstance->musicBoxInfo->state = state;

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "lowvolume", 1.0f, 1, 0, 0.1f, IDLE);

	//create one extra per music box
	baCreateExtra(staveExtraSystem, actorInstance->actorInstance.position, actorInstance->actorInstance.orientation, MUSICBOXEXTRA_STAVE, NULL, actorInstance, &zero);
}

/* --------------------------------------------------------------------------------
   Function : CreateMusicBoxInstance
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMusicBoxInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	ACTORINSTANCE	*listener;
	TBVector	displacement;
	TBTexture	*texture;
	int32		withinRadius;
	float		zero = 0.0f;

	for(int i = (gameStatus.multiplayer.numPlayers-1);i >= 0;--i)
	{
		if(!(listener = gameStatus.player[i].actorInstance)) continue;
		
		withinRadius = FALSE;
		for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
		{
			if(ptr->special != SPECIALTYPE_MUSICBOX) continue;
			if(!ptr->musicBoxInfo->state) continue;

			//update clock
			if((ptr->musicBoxInfo->clock -= fTime) < 0.0f)
			{
				ptr->musicBoxInfo->clock = MUSICBOX_NOTEFREQ;
				switch(bmRand()%3)
				{
				case 0:
					texture = ptr->musicBoxInfo->crotchet;
					break;
				case 1:
					texture = ptr->musicBoxInfo->quaver;
					break;
				case 2:
					texture = ptr->musicBoxInfo->semiquaver;
					break;
				}
				baCreateExtra(staveExtraSystem, ptr->actorInstance.position, ptr->actorInstance.orientation, MUSICBOXEXTRA_NOTE, texture, NULL, &zero);
			}

			//check distance between player and music box
			if(ptr->musicBoxInfo->radius)
			{
				bmVectorSub(displacement, ptr->actorInstance.position, listener->actorInstance.position);

				if(bmVectorLen(displacement) < ptr->musicBoxInfo->radius)
				{
					withinRadius = TRUE;
					break;
				}
			}
			if(ptr->musicBoxInfo->boundaryCRC)
			{
				if(CheckIfVectorPositionLiesWithinInstance(ptr->musicBoxInfo->polys, ptr->musicBoxInfo->verts, ptr->musicBoxInfo->numPolys, listener->actorInstance.position))
				{
					withinRadius = TRUE;
					break;
				}
			}
		}

		if(withinRadius)
		{
			switch(listener->actorStatus->currentState)
			{
			case STATE_MOVE:
			case STATE_SPIN:
			case STATE_SKIDSTOP:
			case STATE_TIPTOE:
			case STATE_CHILLIPEPPER:
				RequestStateChangePlus(listener, STATE_MESMERISED, MusicBoxStateChangeCallback);
			}
		}
		else
		{
			if(listener->actorStatus->currentState == STATE_MESMERISED)
			{
				RequestStateChange(listener, STATE_MOVE);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CheckIfVectorPositionLiesWithinInstance
   Purpose : checks to see if a point lies within an instance
   Parameters : 
   Returns : TRUE/ FALSE
   Info : 
*/

void PreCacheMusicBoxBoundaryInfo(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr, *box;
	TBVector		normal;
	int				i;

	for(box = actorInstanceList->head.next;box != &actorInstanceList->head;box = box->next)
	{
		if((box->special == SPECIALTYPE_MUSICBOX)&&(box->musicBoxInfo))
		{
			if(box->musicBoxInfo->boundaryCRC)
			{
				// CMD: find instance in invisible instances list
				ptr = FindActorInstanceInInstanceList(NULL, box->musicBoxInfo->boundaryCRC, &map.invisibleInstances);
	
				if(ptr)
				{
#if BPLATFORM!=GAMECUBE
					box->musicBoxInfo->verts = (COLLVERTEX*)baGetActorGeometry(&ptr->actorInstance,NULL,0,NULL,&box->musicBoxInfo->numPolys,NULL,NULL,BGETACTGEOMETRY_WORLDSPACE);
					box->musicBoxInfo->polys = (COLLPOLY*)baGetActorGeometry(&ptr->actorInstance,NULL,1,NULL,&box->musicBoxInfo->numPolys,NULL,NULL,BGETACTGEOMETRY_WORLDSPACE);
#else
					box->musicBoxInfo->verts = (COLLVERTEX*)GetActorGeometryFromARAM(ptr, 0, &box->musicBoxInfo->numPolys, NULL, BGETACTGEOMETRY_WORLDSPACE, 0);
					box->musicBoxInfo->polys = (COLLPOLY*)GetActorGeometryFromARAM(ptr, 1, &box->musicBoxInfo->numPolys, NULL, BGETACTGEOMETRY_WORLDSPACE, 0);
#endif
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CheckIfVectorPositionLiesWithinInstance
   Purpose : checks to see if a point lies within an instance
   Parameters : 
   Returns : TRUE/ FALSE
   Info : 
*/

int CheckIfVectorPositionLiesWithinInstance(COLLPOLY *polys, COLLVERTEX *verts, uint32 numPolys, TBVector position)
{
	uint32				polyNum;
	TBVector			tempVector, normal;
	float				dot;

	for (polyNum=0;polyNum<numPolys;polyNum++)
	{
		SETVECTOR(normal, verts[polys[polyNum].i1].x, verts[polys[polyNum].i1].y, verts[polys[polyNum].i1].z, 1.0f);
		bmVectorSub(tempVector, normal, position);
		SETVECTOR(normal, polys[polyNum].nx, polys[polyNum].ny, polys[polyNum].nz, 1.0f);

		bmVectorNorm(tempVector, tempVector);
		bmVectorNorm(normal, normal);
		normal[3] = 1.0f; tempVector[3] = 1.0f;
		if(dot = bmVectorDot(tempVector, normal) < 0.0f) return FALSE;
	}

	// CMD: loop through vertex list
	/*
	for(vertNum = (numVerts-1);vertNum >= 0;vertNum -= 3)
	{
		// CMD: check direction of normal against position/vert vector
		SETVECTOR(normal, verts[vertNum].x, verts[vertNum].y, verts[vertNum].z, 1.0f);
		bmVectorSub(tempVector, normal, position);
		SETVECTOR(normal, polys[0].nx, polys[0].ny, polys[0].nz, 1.0f);

		bmVectorNorm(tempVector, tempVector);
		bmVectorNorm(normal, normal);
		normal[3] = 1.0f; tempVector[3] = 1.0f;
		if(dot = bmVectorDot(tempVector, normal) < 0.0f) return FALSE;
	}*/
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : MusicBoxStateChangeCallback
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void MusicBoxStateChangeCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "walk2start", 1.0f,0,0,0.1f,MOVE);
	PlayAnimationByName(actorInstance, "walk2", 1.0f,1,1,0.1f,MOVE);
}

/* --------------------------------------------------------------------------------
   Function : InitStaveExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitStaveExtraSystem(void)
{
	staveExtraSystem = baCreateExtraSystem("stave",StaveExtraCreateCallback,
		StaveExtraUpdateCallback,StaveExtraDeleteCallback,StaveExtraRenderCallback,
		StaveExtraSystemInitCallback,sizeof(MUSICBOXEXTRAINFO),MUSICBOX_NUMSTAVES,0,0);
}

/* --------------------------------------------------------------------------------
   Function : StaveExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void StaveExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    MUSICBOXEXTRAINFO		*data = (MUSICBOXEXTRAINFO*)vData;
	float		*position;
	float		*orientation;
	int			i;
	uint32		type;
	TBTexture	*texture;

    if(parms)
	{
		position = va_arg(parms, float*);
		orientation = va_arg(parms, float*);
		data->type = type = va_arg(parms, uint32);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		data->musicBox = (ACTORINSTANCE *)va_arg(parms, ACTORINSTANCE*);
		data->texCoorOffset = *va_arg(parms, float*);

		bmVectorCopy(data->position, position);
		bmQuatToMatrix(data->orientation, orientation);
		
		switch(type)
		{
		case MUSICBOXEXTRA_STAVE:
			//clock
			data->clock = 0.0f;
			for(i = 0;i < MUSICBOX_STAVENUMPOINTS;i++)
			{
				//calculate sizes
				data->size[i] = METERTOUNIT(0.1f) + METERTOUNIT(MUSICBOX_STAVESIZECONST*(float)(i*i));

				//colours
				data->colour[i][0] = data->colour[i][1] = data->colour[i][2] = 0;
				data->colour[i][3] = (int32)(200.0f*bmSin(RAD(180.0f/(float)MUSICBOX_STAVENUMPOINTS*(float)i)));

				//texture coords
				data->texCoorOffset = 0.0f;
			}

			data->texture = bkLoadTexture(NULL, "stave.tga", 0);
			break;
		case MUSICBOXEXTRA_NOTE:
			data->clock = 0.0f;
			data->size[0] = METERTOUNIT(0.15f);
			data->colour[0][0] = data->colour[0][1] = data->colour[0][2] = 0;
			data->alpha = 100.0f;
			data->texture = texture;
			data->flip = bmRand()%2;

			data->stave[0][0] = METERTOUNIT(0.5f)*bmSin(RAD(90.0f*(data->clock += fTime)));
			data->stave[0][1] = METERTOUNIT(1.4f) + METERTOUNIT(0.5f)*bmSin(RAD(180.0f*data->clock));
			data->stave[0][1] += METERTOUNIT(0.2f)*data->clock;
			data->stave[0][2] = METERTOUNIT(1.0f)*data->clock;

			bmMatMultiplyVector(data->orientation, data->stave[0]);
			bmVectorAdd(data->stave[0], data->stave[0], data->position);
			break;
		case MUSICBOXEXTRA_HEAD:
			data->clock = 0.0f;
			data->size[0] = METERTOUNIT(0.05f);
			data->colour[0][0] = data->colour[0][1] = data->colour[0][2] = 0;
			data->alpha = 0.0f;
			data->texture = texture;
			data->flip = bmRand()%2;

			if(data->node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance, "spinetop"))
			{
				// CMD: will not find node if taz spins into area
				baGetNodesWorldPosition(&gameStatus.player[0].actorInstance->actorInstance, data->node, data->position);
				// CMD: use texCoorOffset as angle
			}
			else
			{
				bmVectorCopy(data->position, gameStatus.player[0].actorInstance->actorInstance.position);
				data->position[1] += METERTOUNIT(0.8f);
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : StaveExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int StaveExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	MUSICBOXEXTRAINFO *data = (MUSICBOXEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : StaveExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void StaveExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	MUSICBOXEXTRAINFO		*data;
	TBVector				tempPos, offset;
	int						j;
	
	for (int32 i=0;i<noofInstances;i++)
	{
		data = (MUSICBOXEXTRAINFO*)dataPtrs[i];

		switch(data->type)
		{
		case MUSICBOXEXTRA_STAVE:
			for(j = 0; j < MUSICBOX_STAVENUMPOINTS;j++)
			{
				//create points
//				data->points[j][0] = 0.0f;
				//vary the y coord along a sin wave with quadratic amplitude growth
//				data->points[j][1] = METERTOUNIT(1.4f) + ((MUSICBOX_STAVEAMPCONST*(float)(j*j)+METERTOUNIT(0.2f))
//					*bmCos(RAD(MUSICBOX_STAVEFREQCONST*(float)j)+RAD(MUSICBOX_STAVETIMECONST*(data->clock += fTime))));
				//add gradient to y coord
//				data->points[j][1] += (float)j/(float)MUSICBOX_STAVENUMPOINTS*METERTOUNIT(1.0f); 
//				data->points[j][2] = (float)j/(float)MUSICBOX_STAVENUMPOINTS*MUSICBOX_STAVELENGTH;

				//create points
				data->stave[j][0] = 0.0f;
				//vary the y coord along a sin wave with quadratic amplitude growth
				data->stave[j][1] = METERTOUNIT(1.4f) + ((MUSICBOX_STAVEAMPCONST*(float)(j*j)+METERTOUNIT(0.2f))
					*bmSin(RAD(MUSICBOX_STAVEFREQCONST*(float)j)+RAD(MUSICBOX_STAVETIMECONST*(data->clock += fTime))));
				//add gradient to y coord
				data->stave[j][1] += (float)j/(float)MUSICBOX_STAVENUMPOINTS*METERTOUNIT(1.0f); 
				data->stave[j][2] = (float)j/(float)MUSICBOX_STAVENUMPOINTS*MUSICBOX_STAVELENGTH;

//				bmMatMultiplyVector(data->orientation, data->points[j]);
//				bmVectorAdd(data->points[j], data->points[j], data->position);

				bmMatMultiplyVector(data->orientation, data->stave[j]);
				bmVectorAdd(data->stave[j], data->stave[j], data->position);
			}

			//texture coords
			if((data->texCoorOffset -= 0.3f*fTime) < -1.0f)
			{
				data->texCoorOffset += 1.0f;
			}
			break;
		case MUSICBOXEXTRA_NOTE:
			if((data->alpha -= 15.0f*fTime) > 0.0f)
			{
				data->stave[0][0] = METERTOUNIT(0.5f)*bmSin(RAD(90.0f*(data->clock += fTime)));
				data->stave[0][1] = METERTOUNIT(1.4f) + METERTOUNIT(0.5f)*bmSin(RAD(180.0f*data->clock));
				data->stave[0][1] += METERTOUNIT(0.2f)*data->clock;
				data->stave[0][2] = METERTOUNIT(1.0f)*data->clock;
	
				bmMatMultiplyVector(data->orientation, data->stave[0]);
				bmVectorAdd(data->stave[0], data->stave[0], data->position);
				break;
			}
			else
			{
				data->alpha = 0.0f;
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
			break;
		case MUSICBOXEXTRA_HEAD:
			// CMD: update alpha for fade in
			if(data->alpha < 100.0f)
			{
				if((data->alpha += 50.0f*fTime) > 100.0f) data->alpha = 100.0f;
			}
			// CMD: update rotation
			if((data->texCoorOffset += RAD(120.0f)*fTime) > RAD(360.0f)) data->texCoorOffset -= RAD(360.0f);

			// CMD: calculate position
			if(!data->node)
			{
				data->node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance, "spinetop");
			}
			if(data->node)
			{
				baGetNodesWorldPosition(&gameStatus.player[0].actorInstance->actorInstance, data->node, tempPos);
				offset[0] = METERTOUNIT(0.4f)*bmCos(data->texCoorOffset);
				offset[2] = METERTOUNIT(0.4f)*bmSin(data->texCoorOffset);
				offset[1] = METERTOUNIT(0.3f) + METERTOUNIT(0.05f)*bmSin(RAD(data->clock += fTime));
				bmVectorAdd(data->position, tempPos, offset);
			}
			else
			{
				bmVectorCopy(data->position, gameStatus.player[0].actorInstance->actorInstance.position);
				data->position[1] += METERTOUNIT(0.8f);
			}
			break;
		case MUSICBOXEXTRA_FADE:
			// CMD: fade to zero alpha and then kill
			if((data->alpha -= 50.0f*fTime) < 0.0f)
			{
				data->alpha = 0.0f;
				baDeleteExtraInstance(eSystem, i);
				noofInstances--;
				continue;
			}
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : StaveExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void StaveExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	MUSICBOXEXTRAINFO *data;
	int32			noofVerts, j;
	float			screenSize[MUSICBOX_STAVENUMPOINTS];
	TBPrimVertex2D	destVerts[((MUSICBOX_STAVENUMPOINTS*4)+4)];
	float			t;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for (int i=0;i<noofInstances;i++)
	{
		data = (MUSICBOXEXTRAINFO*)dataPtrs[i];

		switch(data->type)
		{
		case MUSICBOXEXTRA_STAVE:
			if(!data->musicBox->musicBoxInfo->state) continue;

			for(j=0;j<MUSICBOX_STAVENUMPOINTS;j++)
			{
//				data->points[j][3] = 1.0f;
				data->stave[j][3] = 1.0f;
			}

			//draw stave first
			bdSetIdentityObjectMatrix();
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			noofVerts = bdDrawMultiStreakClipped(MUSICBOX_STAVENUMPOINTS,data->stave[0],data->size,screenSize,&data->colour[0][0],destVerts, 0.0f);

			if(noofVerts)
			{
				//fill in texture coords
				t = data->texCoorOffset;
				for(j = 0;j < noofVerts;j+=2)
				{
					BDVERTEX_SETUV(&destVerts[j], 0.0f, t);
					BDVERTEX_SETUV(&destVerts[j+1], 1.0f, t);
					t += 0.25f;
					//set end alphas
					if((j==0)||(j==(noofVerts-2)))
					{
						BDVERTEX_SETA(&destVerts[j], 0.0f);
						BDVERTEX_SETA(&destVerts[j+1], 0.0f);
					}
				}

				bdSetIdentityObjectMatrix();
				bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
				bdSetTexture(0, data->texture);
				bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, 0);
				bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, destVerts, noofVerts, BVERTTYPE_SINGLE2D);
			}
			break;
		case MUSICBOXEXTRA_NOTE:
			data->stave[0][3] = 1.0f;

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
			
			bdDrawFlatSprite(data->stave[0],data->size[0],data->size[0],0.0f,data->texture,data->flip,data->flip,
				data->colour[0][0],data->colour[0][1],data->colour[0][2],bmFloatToInt(data->alpha)<<1,NULL,0.0f);
	
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			break;
		case MUSICBOXEXTRA_FADE:
		case MUSICBOXEXTRA_HEAD:
			data->position [3] = 1.0f;

			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
			
			bdDrawFlatSprite(data->position,data->size[0],data->size[0],0.0f,data->texture,data->flip,data->flip,
				data->colour[0][0],data->colour[0][1],data->colour[0][2],bmFloatToInt(data->alpha)<<1,NULL,0.0f);
	
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			break;
		}
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : StaveExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void StaveExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}