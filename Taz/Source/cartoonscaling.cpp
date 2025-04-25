// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : cartoonscaling.cpp
//   Purpose : 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "cartoonscaling.h"
#include "fallingobjects.h"

/* --------------------------------------------------------------------------------
   Function : updateCartoonScaling
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

void UpdateCartoonScaling(ACTORINSTANCE *actor, CARTOONSCALEINFO *cartoonScaleInfo)
{
	float			scaleFactor, tempFloat;

	if (cartoonScaleInfo->type == SCALETYPE_DOWN)
	{
		cartoonScaleInfo->time -= fTime;
	}
	else
	{
		cartoonScaleInfo->time += fTime;
	}

	switch(cartoonScaleInfo->stage)
	{
	case CARTOONSCALE_STAGEONE:
		scaleFactor = cartoonScaleInfo->maxScale*4.0f*cartoonScaleInfo->time;
		actor->actorInstance.scale[0] = scaleFactor;
		actor->actorInstance.scale[1] = scaleFactor;
		actor->actorInstance.scale[2] = scaleFactor;
	
		if(actor->actorInstance.scale[0] >= cartoonScaleInfo->maxScale)
		{
			actor->actorInstance.scale[0] = cartoonScaleInfo->maxScale;
			actor->actorInstance.scale[1] = cartoonScaleInfo->maxScale;
			actor->actorInstance.scale[2] = cartoonScaleInfo->maxScale;
			cartoonScaleInfo->stage = CARTOONSCALE_STAGETWO;
			cartoonScaleInfo->time = 1.0f;
		}
		break;
	case CARTOONSCALE_STAGETWO:
		tempFloat = bmCos(cartoonScaleInfo->time*20.0f);
		scaleFactor = 1.0f + ((cartoonScaleInfo->maxScale-1.0f)/(cartoonScaleInfo->time*5.0f))*(tempFloat*tempFloat);
		actor->actorInstance.scale[0] = scaleFactor;
		actor->actorInstance.scale[1] = scaleFactor;
		actor->actorInstance.scale[2] = scaleFactor;
		if(cartoonScaleInfo->time >= 1.5f)
		{
			actor->actorInstance.scale[0] = 1.0f;
			actor->actorInstance.scale[1] = 1.0f;
			actor->actorInstance.scale[2] = 1.0f;
			cartoonScaleInfo->stage = CARTOONSCALE_STAGETHREE;
			cartoonScaleInfo->time = 0.0f;
		}
		break;
	case CARTOONSCALE_STAGETHREE:
		if(actor->special == SPECIALTYPE_FALLINGOBJECT)
		{
			UpdateFallingObjectInfo(actor);
		}
		break;
	case CARTOONSCALEDOWN_STAGEONE:
		tempFloat = bmCos(cartoonScaleInfo->time*20.0f);
		scaleFactor = 1.0f + ((cartoonScaleInfo->maxScale-1.0f)/(cartoonScaleInfo->time*5.0f))*(tempFloat*tempFloat);
		actor->actorInstance.scale[0] = scaleFactor;
		actor->actorInstance.scale[1] = scaleFactor;
		actor->actorInstance.scale[2] = scaleFactor;
		if(cartoonScaleInfo->time <= 1.0f)
		{
			actor->actorInstance.scale[0] = cartoonScaleInfo->maxScale;
			actor->actorInstance.scale[1] = cartoonScaleInfo->maxScale;
			actor->actorInstance.scale[2] = cartoonScaleInfo->maxScale;
			cartoonScaleInfo->stage = CARTOONSCALEDOWN_STAGETWO;
			cartoonScaleInfo->time = cartoonScaleInfo->maxScale / (cartoonScaleInfo->maxScale *4.0f);
		}
		break;
	case CARTOONSCALEDOWN_STAGETWO:
		scaleFactor = cartoonScaleInfo->maxScale*4.0f*cartoonScaleInfo->time;
		actor->actorInstance.scale[0] = scaleFactor;
		actor->actorInstance.scale[1] = scaleFactor;
		actor->actorInstance.scale[2] = scaleFactor;
	
		if(actor->actorInstance.scale[0] <= 0.0f)
		{
			actor->actorInstance.scale[0] = 0.0f;
			actor->actorInstance.scale[1] = 0.0f;
			actor->actorInstance.scale[2] = 0.0f;
			cartoonScaleInfo->stage = CARTOONSCALEDOWN_STAGETHREE;
			cartoonScaleInfo->time = 0.0f;
		}
		break;
	case CARTOONSCALEDOWN_STAGETHREE:
		break;

		baPrepActorInstance(&actor->actorInstance, TRUE);
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateCartoonScalingInfo
   Purpose : 
   Parameters : ACTORINSTANCE *
   Returns : 
   Info : 
*/

CARTOONSCALEINFO *CreateCartoonScalingInfo(float maxScale)
{
	CARTOONSCALEINFO		*cartoonScaleInfo;

	cartoonScaleInfo = (CARTOONSCALEINFO*)ZALLOC(sizeof(CARTOONSCALEINFO));
	if(cartoonScaleInfo)
	{
		cartoonScaleInfo->maxScale = maxScale;
		return cartoonScaleInfo;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create cartoon scale info structure\n");
		return cartoonScaleInfo;
	}
}