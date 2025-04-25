// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : superburpeffect.cpp
//   Purpose : functions to update and draw superburp effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "maths.h"
#include "characters.h"
#include "debris.h"
#include "holysmoke.h"
#include "precipitation.h"
#include "textures.h"
#include "superburp.h"
#include "superburpeffect.h"


/* --------------------------------------------------------------------------------
   Function : CreateSuperBurpBlastRadius
   Purpose : Creates structure for blast radius effect
   Parameters : actorInstance that has superburped
   Returns : true/false depending on success
   Info : the capital of Surinam is Paramarabou
*/

int CreateSuperBurpBlastRadius(ACTORINSTANCE *actorInstance)
{
	BLASTRADIUSINFO		*ptr;
	TBActor				*actor;

	ptr = (BLASTRADIUSINFO*)ZALLOC(sizeof(BLASTRADIUSINFO));
	if(ptr == NULL) return FALSE;

	ptr->alpha = 255.0f;
	ptr->radius = 0.0f;
	//find and create object to be used as blast radius
	actor = FindActorInActorList("blastradius.obe");
	if(actor)
	{
		baInitActorInstance(&ptr->blastActorInstance, actor,0);
		baPrepActorInstance(&ptr->blastActorInstance,TRUE);
		baSetActorRenderMode(&ptr->blastActorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		baSetActorRenderMode(&ptr->blastActorInstance, 1, BACTRENDERMODE_NONE, NULL);
		
		baGetNodesWorldPosition(&actorInstance->actorInstance,baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem"),ptr->blastActorInstance.position);
		SETVECTOR(ptr->blastActorInstance.scale,ptr->radius,ptr->radius,ptr->radius,1.0f);
		bmQuatCopy(ptr->blastActorInstance.orientation,bIdentityQuaternion);
		actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo = ptr;
		return TRUE;
	}
	else
	{
		bkPrintf("*** WARNING *** could not find blastradius.obe, super burp will not function!/n");
		FREE(ptr);
		ptr = NULL;
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepSuperBurpBlastRadius
   Purpose : preps and frees blast radius effect
   Parameters : actorInstance that has superburped
   Returns : TRUE if exists
   Info : I have been reading about C++ recently, and it is actually a bit poo
*/

int PrepSuperBurpBlastRadius(ACTORINSTANCE *actorInstance)
{
	BLASTRADIUSINFO	*ptr = actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo;

	if(ptr)
	{
		if(ptr->alpha > 0.0f)
		{
			baPrepActorInstance(&ptr->blastActorInstance,TRUE);
			return TRUE;
		}
		else
		{
			baFreeActorInstance(&ptr->blastActorInstance);
			FREE(actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo);
			actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo = NULL;
			return FALSE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateSuperBurpBlastRadius
   Purpose : updates blast radius effect
   Parameters : actorInstance that has superburped
   Returns : 
   Info : women live longer than men, except those shot by their husbands
*/

void UpdateSuperBurpBlastRadius(ACTORINSTANCE *actorInstance)
{
	BLASTRADIUSINFO	*ptr = actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo;

	if(ptr)
	{
		ptr->radius += BLASTRADIUS_VELOCITY*fTime;
		SETVECTOR(ptr->blastActorInstance.scale,ptr->radius,ptr->radius,ptr->radius,1.0f);
		if((ptr->alpha -= 128.0f*fTime) < 0.0f)
		{
			ptr->alpha = 0.0f;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawSuperBurpBlastRadius
   Purpose : Draw blast radius effect
   Parameters : actorInstance that has superburped
   Returns : 
   Info : Louise Redknapp is in fact less than 2Inches tall, which must mean that Jamie is also very small.
*/

void DrawSuperBurpBlastRadius(ACTORINSTANCE *actorInstance)
{
	BLASTRADIUSINFO	*ptr = actorInstance->characterInfo->superBurpEffectInfo.blastRadiusInfo;

	if(ptr)
	{
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		
		baSetGlobalAlpha((int)ptr->alpha);
		bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,0);

		bdSetTexture(0, NULL);
		baDrawActorInstance(&ptr->blastActorInstance,BSETPRIMALPHATYPE_ALPHA);

		bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
		bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
		
		baSetGlobalAlpha(256);
		bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);
	}
}

//////////////////////////////////////// BUBBLES //////////////////////////////////////

/*	--------------------------------------------------------------------------------
	Function 	: CreateBubbles
	Purpose 	: Creates bubbles from taz's mouth when in superburp mode
	Parameters 	: 
	Returns 	: 
	Info 		: Mint Aero's are very nice, however thay are not recomended for after a largeish lunch.
*/

void CreateBubbles(ACTORINSTANCE *actorInstance,float maxScale, float life, EBubbleColour colour)
{
	TBTexture		*texture;
	TBMatrix		matrix,trans,result;
	TBVector		velocity,position;
	TBVector		tempPosition, tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter, numSprites;
	float			angle, zero, alpha;
	TBActor			*actor = NULL;
	PRECIPITATION	*precipitation = NULL;
	TBActorNodeInstance		*node;
	uint			flag;

	switch(colour)
	{
	case GREEN_BUBBLE:
		texture = textureGreenBubble;
		flag = DEBRISFLAG_GREENBUBBLE|DEBRISFLAG_BUBBLES;
		break;
	case WHITE_BUBBLE:
		texture = textureWhiteBubble;
		flag = DEBRISFLAG_WHITEBUBBLE|DEBRISFLAG_BUBBLES;
		break;
	}

	if(texture)
	{
		if(fps >= 58.0f) numSprites = MAX_BUBBLE_SPRITES;
		else
			if(fps >= 28.0f) numSprites = 2*MAX_BUBBLE_SPRITES;
			else numSprites = 3*MAX_BUBBLE_SPRITES;

		for(counter = 0; counter < numSprites; counter++)
		{
			angle = RAD((float)(bmRand()%360));
			//set velocity to be a general radial drift
			velocity[0] = SLOW_SMOKE_SPEED * bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = SLOW_SMOKE_SPEED * bmSin(angle);

			//find taz's mouth position
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance,"snoutstem");
			if(node) baGetNodesWorldPosition(&actorInstance->actorInstance,node,position);
			else
			{
				SETVECTOR(position,0.0f,METERTOUNIT(0.8f),0.0f,1.0f);
				bmQuatToMatrix(matrix,actorInstance->actorInstance.orientation);
				bmMatTranslate(trans,actorInstance->actorInstance.position[0],actorInstance->actorInstance.position[1],actorInstance->actorInstance.position[2]);
				bmMatMultiply(result,trans,matrix);
				bmMatMultiplyVector(result,position);
			}
			//alter sprite position a little
			bmVectorCopy(tempPosition, position);
			bmVectorCDiv(tempVelocity, velocity, 10.0f);
			bmVectorAdd(tempPosition, velocity, tempPosition);

			//add y componant of velocity
			velocity[1] = METERTOUNIT(0.6f);

			zero = 0.0f;
			alpha = 50.0f;
			baCreateExtra(bubbleSystem,tempPosition,velocity,texture,&life,&maxScale,&zero,128,128,128,&alpha,flag,&zero);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateBubbles
	Purpose 	: updates bubbles
	Parameters 	: debris data structure
	Returns 	: 
	Info 		: thinking up usefull info at short notice can be quite difficult
*/

int32 UpdateBubbles(BUBBLESEXTRA *data)
{
	TBVector		posInc;
	float			number;

	number = BURPBUBBLE_LIFE - (((float)(bmRand()%10)/10.0f)+0.25f);
	if(data->life < number)
	{
		//update scale
		if((data->size -= METERTOUNIT(0.6f)*fTime) < 0.0f) return TRUE;
	}
	// CMD: update position
	bmVectorCMul(posInc, data->velocity, fTime);
	bmVectorAdd(data->position, data->position, posInc);
	return FALSE;
}