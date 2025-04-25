// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : tweetytransporter.cpp
//   Purpose : functions to transport tweety from place to place in a spectacular manner
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "main.h"
#include "mechatweety.h"
#include "tweetytransporter.h"


/* --------------------------------------------------------------------------------
   Function : UpdateTransientTransporterEffectForTweety
   Purpose : fades effect up/down
   Parameters : target alpha (will most likely be 256 or 0), current alpha
   Returns : updated alpha
   Info : 
*/

short UpdateTransientTransporterEffectForTweety(MECHATWEETYINFO *info, short targetAlpha, short currentAlpha)
{
	float		target, current;

	target = (float)targetAlpha;
	current = (float)currentAlpha;

	//update alpha
	if(targetAlpha < currentAlpha)
	{
		//update scale
		if((info->actorInstance->actorInstance.scale[0] -= TWEETYTRANSPORTER_ALPHARATE*info->scaleInfo.scale*fTime) < 0.0f)
		{
			info->actorInstance->actorInstance.scale[0] = 0.0f;
		}
		info->actorInstance->actorInstance.scale[2] = info->actorInstance->actorInstance.scale[0];

		// increment must be a decrement
		if((current -= TWEETYTRANSPORTER_ALPHARATE*256.0f*fTime) < target)
		{
			current = target;
		}
		return (short)current;
	}
	else
	if(targetAlpha > currentAlpha)
	{
		//update scale
		if((info->actorInstance->actorInstance.scale[0] += TWEETYTRANSPORTER_ALPHARATE*info->scaleInfo.scale*fTime) > info->scaleInfo.scale)
		{
			info->actorInstance->actorInstance.scale[0] = info->scaleInfo.scale;
		}
		info->actorInstance->actorInstance.scale[2] = info->actorInstance->actorInstance.scale[0];

		if((current += TWEETYTRANSPORTER_ALPHARATE*256.0f*fTime) > target)
		{
			current = target;
		}
		return (short)current;
	}
	else
	{
		return targetAlpha;
	}
}