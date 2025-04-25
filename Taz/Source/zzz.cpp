// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : camera.cpp
//   Purpose : camera functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "status.h"
#include "maths.h"
#include "zzz.h"
#include "cheesegun.h"
#include "display.h"			// PP: display stuff

TBExtraSystem	*zzzExtraSystem;

/* --------------------------------------------------------------------------------
   Function : ActivateSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int ActivateSleepyZedsForCharacter(ACTORINSTANCE *actorInstance, char *bone, TBTexture *texture, TBTexture *altTexture, float size)
{
	if(!actorInstance->actorStatus) return FALSE;

	// TP: already have zzz's
	if (actorInstance->actorStatus->zzzInfo) return FALSE;

	if(!(actorInstance->actorStatus->zzzInfo = (ZZZINFO*)MALLOC(sizeof(ZZZINFO)))) return FALSE;

	if(bone)
	{
		actorInstance->actorStatus->zzzInfo->originNode = baFindNode(actorInstance->actorInstance.rootNodeInstance, bone);
	}
	else
	{
		actorInstance->actorStatus->zzzInfo->originNode = NULL;
	}
	actorInstance->actorStatus->zzzInfo->texture = texture;
	actorInstance->actorStatus->zzzInfo->altTexture = altTexture;
	actorInstance->actorStatus->zzzInfo->clock = 0.0f;
	actorInstance->actorStatus->zzzInfo->size = size;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TurnOffSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void TurnOffSleepyZedsForCharacter(ACTORINSTANCE *actorInstance)
{
	if(actorInstance->actorStatus->zzzInfo)
	{
		FREE(actorInstance->actorStatus->zzzInfo);
		actorInstance->actorStatus->zzzInfo = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateSleepyZedsForCharacter
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateSleepyZedsForCharacter(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;
	TBVector		tempPos;
	TBTexture		*tempTexture;
	float			four;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if((ptr->actorStatus)&&(ptr->actorStatus->zzzInfo)&&(ptr->actorStatus->zzzInfo->texture))
		{
			if((ptr->actorStatus->zzzInfo->clock += fTime) > ZZZ_PERIOD)
			{
				ptr->actorStatus->zzzInfo->clock = 0.0f;
				if(ptr->actorStatus->zzzInfo->originNode)
				{
					baGetNodesWorldPosition(&ptr->actorInstance, ptr->actorStatus->zzzInfo->originNode, tempPos);
				}
				else
				{
					bmVectorCopy(tempPos, ptr->actorInstance.position);
				}
				//order is: position, scale, texture*, life
				tempTexture = ptr->actorStatus->zzzInfo->texture;
				if(ptr->actorStatus->zzzInfo->altTexture)
				{
					if(bmRand()%2)
					{
						tempTexture = ptr->actorStatus->zzzInfo->altTexture;
					}
				}
				four = 4.0f;
				baCreateExtra(zzzExtraSystem, tempPos, &ptr->actorStatus->zzzInfo->size, tempTexture, &four);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : InitZzzExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitZzzExtraSystem(void)
{
	zzzExtraSystem = baCreateExtraSystem("zzz",ZzzExtraCreateCallback,
		ZzzExtraUpdateCallback,ZzzExtraDeleteCallback,ZzzExtraRenderCallback,
		ZzzExtraSystemInitCallback,sizeof(ZZZEXTRAINFO),MAX_ZZZ,0,0);

#ifndef CONSUMERDEMO
	InitCheeseExtraSystem();
#endif
}

/* --------------------------------------------------------------------------------
   Function : ZzzExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    ZZZEXTRAINFO		*data = (ZZZEXTRAINFO*)vData;
	float			*position;
	float			scale;
	TBTexture		*texture;
	float			life;

    if(parms)
	{
		position = va_arg(parms, float*);
		scale = *va_arg(parms, float*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		life = *va_arg(parms, float*);

		bmVectorCopy(data->position, position);
		data->position[1] += METERTOUNIT(0.3f);
		data->scale = scale;
		data->texture = texture;
		data->life = life;
		data->angle = RAD((float)(bmRand()%360));
		data->radius = 0.0f;
		data->height = 0.0f;
		data->alpha = 128.0f;
	}
}

/* --------------------------------------------------------------------------------
   Function : ZzzExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ZzzExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	ZZZEXTRAINFO *data = (ZZZEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ZzzExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	ZZZEXTRAINFO		*data;
	int				i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (ZZZEXTRAINFO*)dataPtrs[i];

//		if((data->life -= fTime) > 0.0f)
		if((data->alpha -= (128.0f/data->life)*fTime) > 0.0f)
		{
			//update radius
			data->radius += ZZZ_RADIUSRATE*fTime;
			//update height
			data->height += ZZZ_RADIUSRATE*2.0f*fTime;
			//update angle
			data->angle += ZZZ_ANGULARVEL*fTime;
			//update scale
			data->scale += ZZZ_SCALERATE*fTime;
			//update alpha
//			if((data->alpha -= fTime) < 0.0f)
//			{
//				data->alpha = 0.0f;
//			}

		}
		else
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ZzzExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	ZZZEXTRAINFO	*data;
	TBVector		tempPos;
	int		i;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	for (i=0;i<noofInstances;i++)
	{
		data = (ZZZEXTRAINFO*)dataPtrs[i];
	
		//calculate current position
		tempPos[0] = data->position[0] + data->radius*bmSin(data->angle);
		tempPos[1] = data->position[1] + data->height;
		tempPos[2] = data->position[2] + data->radius*bmCos(data->angle);
		tempPos[3] = 1.0f;
		
		bdDrawFlatSprite(tempPos,data->scale, data->scale,0.0f,data->texture,0,0,255,255,255,bmFloatToInt(data->alpha)<<1,NULL,0.0f);
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : ZzzExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void ZzzExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : CreateQuestionMarkAboveActor
   Purpose : Creates question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

int CreateQuestionMarkAboveActor(ACTORINSTANCE *actorInstance, char *bone, char *texture, float size, TBVector offset)
{
	TBActorNodeInstance		*node;

	node = baFindNode(actorInstance->actorInstance.rootNodeInstance, bone);

	if (!node) 
	{
		bkPrintf("*** WARNING *** Bone for question mark not found.\n");
		return false;
	}

	TurnOffSleepyZedsForCharacter(actorInstance);

	if(!actorInstance->actorStatus) return FALSE;
	
	if(actorInstance->actorStatus->questionMarkInfo)
	{
		FREE(actorInstance->actorStatus->questionMarkInfo);
	}
	actorInstance->actorStatus->questionMarkInfo = (QUESTIONMARKINFO*)MALLOC(sizeof(QUESTIONMARKINFO));
	if(!actorInstance->actorStatus->questionMarkInfo)
	{
		bkPrintf("*** WARNING *** questionMarkInfo MALLOC failed!\n");
		return FALSE;
	}

	actorInstance->actorStatus->questionMarkInfo->bone = node;
	baGetNodesWorldPosition(&actorInstance->actorInstance, actorInstance->actorStatus->questionMarkInfo->bone,actorInstance->actorStatus->questionMarkInfo->position);
	bmVectorAdd(actorInstance->actorStatus->questionMarkInfo->position,actorInstance->actorStatus->questionMarkInfo->position,offset);
	actorInstance->actorStatus->questionMarkInfo->texture = bkLoadTexture(NULL,texture,0);
	bmVectorCopy(actorInstance->actorStatus->questionMarkInfo->offset, offset);
	actorInstance->actorStatus->questionMarkInfo->falpha = 0.0f;
	actorInstance->actorStatus->questionMarkInfo->timer = 0.0f;
	actorInstance->actorStatus->questionMarkInfo->size = size;
	actorInstance->actorStatus->questionMarkInfo->pulseInc = 0.0f;
	actorInstance->actorStatus->questionMarkInfo->state = QUESTIONMARKSTATE_FADEUP;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : DestroyQuestionMarkAboveActor
   Purpose : Destroys the question mark above an actors head
   Parameters : 
   Returns : true if there was a question mark
   Info : 
*/

int DestroyQuestionMarkAboveActor(ACTORINSTANCE *actorInstance)
{
	QUESTIONMARKINFO *info = actorInstance->actorStatus->questionMarkInfo;

	if(info)
	{
		info->state = QUESTIONMARKSTATE_FADEDOWN;
		return TRUE;
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateQuestionMarkAboveActor
   Purpose : Updates the question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateQuestionMarkAboveActor(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE		*ptr;
	QUESTIONMARKINFO	*info;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->actorStatus == NULL)
			continue;

		if(ptr->actorStatus->questionMarkInfo)
		{
			info = ptr->actorStatus->questionMarkInfo;
			
			baGetNodesWorldPosition(&ptr->actorInstance, info->bone,info->position);
			bmVectorAdd(info->position,info->position,info->offset);

			switch(info->state)
			{
			case QUESTIONMARKSTATE_FADEUP:
				if((info->falpha += (QUESTIONMARK_FADERATE*fTime)) > 255.0f)
				{
					info->falpha = 255.0;
					info->state = QUESTIONMARKSTATE_STEADY;
				}
				info->alpha = (int)info->falpha;
				break;
			case QUESTIONMARKSTATE_STEADY:
				info->timer += fTime;
				info->pulseInc = (info->size*0.25f)*bmSin(info->timer*RAD(180.0f));
				break;
			case QUESTIONMARKSTATE_FADEDOWN:
				if((info->falpha -= (QUESTIONMARK_FADERATE*fTime)) < 0.0f)
				{
					FREE(ptr->actorStatus->questionMarkInfo);
					ptr->actorStatus->questionMarkInfo = NULL;
				}
				info->alpha = (int)info->falpha;
				break;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawQuestionMarkAboveActor
   Purpose : Draws the question mark above an actors head
   Parameters : 
   Returns : 
   Info : 
*/

void DrawQuestionMarkAboveActor(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE		*ptr;
	QUESTIONMARKINFO	*info;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->actorStatus == NULL)
			continue;

		if(ptr->actorStatus->questionMarkInfo)
		{
			info = ptr->actorStatus->questionMarkInfo;

			info->position[3] = 1.0f;

			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
			
			bdDrawFlatSprite(info->position,(info->size+info->pulseInc),(info->size+info->pulseInc),0.0f,info->texture,0,0,128,128,128,info->alpha<<1,NULL,0.0f);
	
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		}
	}
}