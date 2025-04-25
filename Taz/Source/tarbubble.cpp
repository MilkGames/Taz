// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tarbubble.cpp
//   Purpose : functions to bubbles for cement/lava...
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "util.h"
#include "tarbubble.h"

TBExtraSystem		*tarBubbleExtraSystem;

/* --------------------------------------------------------------------------------
   Function : CreateTarBubbleEmmiter
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

int CreateTarBubbleEmmiter(ACTORINSTANCE *actorInstance, char *textureName, float height, float size, uint32 active)
{
	actorInstance->tarBubbleInfo = (TARBUBBLEINFO*)ZALLOC(sizeof(TARBUBBLEINFO));
	ASSERTM(actorInstance->tarBubbleInfo, "out of memory");

	actorInstance->special = SPECIALTYPE_TARBUBBLE;

	actorInstance->tarBubbleInfo->textureCRC = bkCRC32((uchar*)textureName, strlen(textureName), 0);
	actorInstance->tarBubbleInfo->period = actorInstance->tarBubbleInfo->clock = 0.7f;
	actorInstance->tarBubbleInfo->height = height;
	actorInstance->tarBubbleInfo->width = size;
	actorInstance->tarBubbleInfo->active = active;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: FindTexturesFromMapUsedByTarBubbleObjects
	Purpose		: Once all of the map has been loaded we can look for the textures used by sparking objects
	Parameters	: none
	Returns		: nothing
	Info		: check out the long function name
*/

void FindTexturesFromMapUsedByTarBubbleObjects(void)
{
	ACTORINSTANCE	*ptr;

	//should only need to search the level instances
	for(ptr = map.invisibleInstances.head.next;ptr != &map.invisibleInstances.head;ptr = ptr->next)
	{
		if((ptr->special == SPECIALTYPE_TARBUBBLE)&&(ptr->tarBubbleInfo))
		{
			if(ptr->tarBubbleInfo->textureCRC)
			{
				ptr->tarBubbleInfo->texture = bkLoadTextureByCRC(NULL, ptr->tarBubbleInfo->textureCRC, 0);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateTarBubbleEmmiter
   Purpose : 
   Parameters :
   Returns : 
   Info : 
*/

void UpdateTarBubbleEmmiter(ACTORINSTANCE *actorInstance)
{
	TARBUBBLEINFO	*info = actorInstance->tarBubbleInfo;

	if((info->active)&&((info->clock -= fTime) < 0.0f))
	{
		info->clock = info->period;

		baCreateExtra(tarBubbleExtraSystem, actorInstance->actorInstance.position, info->texture, &info->height, &info->width);
	}
}

/* --------------------------------------------------------------------------------
   Function : InitTarBubbleExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitTarBubbleExtraSystem(void)
{
	tarBubbleExtraSystem = baCreateExtraSystem("tarBubble",TarBubbleExtraCreateCallback,
		TarBubbleExtraUpdateCallback,TarBubbleExtraDeleteCallback,TarBubbleExtraRenderCallback,
		TarBubbleExtraSystemInitCallback,sizeof(TARBUBBLE),MAX_TARBUBBLES,0,0);
}

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    TARBUBBLE		*data = (TARBUBBLE*)vData;
	float			*position;

    if(parms)
	{
		position = va_arg(parms, float*);
		bmVectorCopy(data->position, position);
		bmVectorCopy(data->drawPos, data->position);
		data->texture = (TBTexture*)va_arg(parms, TBTexture*);
		data->height = *va_arg(parms, float*);
		data->width = *va_arg(parms, float*);
		data->clock = 0.0f;
		data->colour.r = data->colour.g = data->colour.b = 128;
		data->colour.a = 0.0f;

		data->rotation = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
	}
}

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/

int TarBubbleExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	TARBUBBLE *data = (TARBUBBLE*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	TARBUBBLE		*data;
	int				i;
	float			tempFloat;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (TARBUBBLE*)dataPtrs[i];

		if((data->clock += fTime) < ITS_BUBBLE_TIME)
		{
			// CMD: calculate height
			tempFloat = data->height * data->clock / ITS_BUBBLE_TIME;
			data->drawPos[1] = data->position[1] + tempFloat;
			// CMD: calculate size
			data->drawSize = data->width*bmSin(RAD(data->clock*180.0f/ITS_BUBBLE_TIME)) + data->width*0.2f*bmSin(RAD(data->clock*1040.0f/ITS_BUBBLE_TIME));
			// CMD: update alpha
			data->colour.drawAlpha = 128.0f;		//temp
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
   Function : TarBubbleExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	TARBUBBLE *data;
	int			i;

	if((int32)context != EXTRA_ALPHA) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	for (i=0;i<noofInstances;i++)
	{
		data = (TARBUBBLE*)dataPtrs[i];
	
		data->drawPos[3] = 1.0f;
		
		bdDrawFlatSprite(data->drawPos,data->drawSize,data->drawSize,data->rotation,data->texture,0,0,
							data->colour.r,data->colour.g,data->colour.b,bmFloatToInt(data->colour.drawAlpha),NULL,0.0f);
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : TarBubbleExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void TarBubbleExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}
