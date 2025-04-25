// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : voodoospelleffect.cpp
//   Purpose : create and update a funky effect for when the witch doctor gets mad
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "voodoospelleffect.h"

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *CreateVoodooSpellEffect(ACTORINSTANCE *actorInstance, char *node, float duration, float size)
{
	VOODOOSPELLFXINFO	*info;

	info = (VOODOOSPELLFXINFO*)ZALLOC(sizeof(VOODOOSPELLFXINFO));
	ASSERTM(info, "bad voodoo");

	info->actorInstance = actorInstance;
	info->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, node);
	info->duration = duration;
	info->size = size;
	info->alpha = 0.0f;
	info->rearTexture = bkLoadTexture(NULL, "voodoospellflare.tga", 0);
	info->texture = bkLoadTexture(NULL, "voodoospell.tga", 0);
	info->rotAngle[0] = 0.0f;
	info->rotAngle[1] = 0.0f;
	info->rotAngSpeed = RAD(60.0f);


	return info;
}

/* --------------------------------------------------------------------------------
   Function : UpdateVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *UpdateVoodooSpellEffect(VOODOOSPELLFXINFO *info)
{
	float			rotInc;

	if(!info) return NULL;

	if((info->duration -= fTime) < 0.0f) return FreeVoodooSpellEffect(info);

	// CMD: update rotation
	rotInc = info->rotAngSpeed*fTime;

	if((info->rotAngle[0] += rotInc) > RAD(360.0f)) info->rotAngle[0] -= RAD(360.0f);
	if((info->rotAngle[1] -= rotInc) < RAD(-360.0f)) info->rotAngle[1] += RAD(360.0f);

	// CMD: update size
//	if((info->size += METERTOUNIT(1.0f)*fTime) > METERTOUNIT(0.5f)) info->size = METERTOUNIT(0.5f);

	// CMD: update alpha
	if((info->alpha += 100.0f*fTime) > 50.0f) info->alpha = 50.0f;

	return info;
}

/* --------------------------------------------------------------------------------
   Function : FreeVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

VOODOOSPELLFXINFO *FreeVoodooSpellEffect(VOODOOSPELLFXINFO *info)
{
	if(info)
	{
		SAFE_FREE(info);
		info = NULL;
	}
	return info;
}

/* --------------------------------------------------------------------------------
   Function : FreeVoodooSpellEffect
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawVoodooSpellEffect(VOODOOSPELLFXINFO *info)
{
	TBVector		position;

	if(!info) return;

	baGetNodesWorldPosition(&info->actorInstance->actorInstance, info->node, position);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);

	bdDrawFlatSprite(position, info->size, info->size*3.0f, info->rotAngle[0], info->rearTexture, 0, 0, 127, 127, 127, (int)info->alpha, NULL, METERTOUNIT(-0.5f));
	bdDrawFlatSprite(position, info->size, info->size*3.0f, info->rotAngle[1], info->rearTexture, 0, 0, 127, 127, 127, (int)info->alpha, NULL, METERTOUNIT(-0.5f));

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, FALSE);

	bdDrawFlatSprite(position, info->size, info->size, info->rotAngle[0], info->texture, 0, 0, 127, 127, 127, (int)info->alpha, NULL, METERTOUNIT(-0.5f));
	bdDrawFlatSprite(position, info->size, info->size, info->rotAngle[1], info->texture, 0, 0, 127, 127, 127, (int)info->alpha, NULL, METERTOUNIT(-0.5f));

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}