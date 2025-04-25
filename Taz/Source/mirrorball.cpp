// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : mirrorball.cpp
//   Purpose : functions for simulating a mirrorball
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "actors.h"
#include "animation.h"
#include "lights.h"
#include "mirrorball.h"

MIRRORBALLINFO	*tazboss1Mirrorball;
int	lightColours[][3] = {{255, 19, 19},{19, 255, 19},{19, 19, 255},{255, 255, 19}};

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

MIRRORBALLINFO *CreateMirrorballLights(void)
{
	MIRRORBALLINFO	*info;
	TBVector		position;	

	if(info = (MIRRORBALLINFO*)MALLOC(sizeof(MIRRORBALLINFO)))
	{
		if(baCreateLight(&info->red, BLIGHTFLAG_SPECULAR))
		{
			bmVectorSet(position, METERTOUNIT(10.0f), METERTOUNIT(0.0f), METERTOUNIT(10.0f), 1.0f);
			baSetDirectionalLight(&info->red, position);
			baSetLightColour(&info->red, lightColours[0][0], lightColours[0][1], lightColours[0][2]);
			baSetSpecularIntensity(&info->red, 0.8f);
		}
		if(baCreateLight(&info->blue, BLIGHTFLAG_SPECULAR))
		{
			bmVectorSet(position, METERTOUNIT(10.0f), METERTOUNIT(0.0f), METERTOUNIT(-10.0f), 1.0f);
			baSetDirectionalLight(&info->blue, position);
			baSetLightColour(&info->blue, lightColours[1][0], lightColours[1][1], lightColours[1][2]);
			baSetSpecularIntensity(&info->blue, 0.8f);
		}
		if(baCreateLight(&info->green, BLIGHTFLAG_SPECULAR))
		{
			bmVectorSet(position, METERTOUNIT(-10.0f), METERTOUNIT(0.0f), METERTOUNIT(10.0f), 1.0f);
			baSetDirectionalLight(&info->green, position);
			baSetLightColour(&info->green, lightColours[2][0], lightColours[2][1], lightColours[2][2]);
			baSetSpecularIntensity(&info->green, 0.8f);
		}
		if(baCreateLight(&info->yellow, BLIGHTFLAG_SPECULAR))
		{
			bmVectorSet(position, METERTOUNIT(-10.0f), METERTOUNIT(0.0f), METERTOUNIT(-10.0f), 1.0f);
			baSetDirectionalLight(&info->yellow, position);
			baSetLightColour(&info->yellow, lightColours[3][0], lightColours[3][1], lightColours[3][2]);
			baSetSpecularIntensity(&info->yellow, 0.8f);
		}
		if(baCreateLight(&info->bottom, BLIGHTFLAG_SPECULAR))
		{
			bmVectorSet(position, METERTOUNIT(0.0f), METERTOUNIT(1.0f), METERTOUNIT(0.0f), 1.0f);
			baSetDirectionalLight(&info->bottom, position);
			baSetLightColour(&info->bottom, 255, 19, 19);
			baSetSpecularIntensity(&info->bottom, 0.8f);
		}
		float		clock = 0.1f;
	}
	return info;
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateMirrorball(ACTORINSTANCE *actorInstance)
{
	int				i, num;
	int				colour[] = {0, 1, 2, 3};
	int				newCols[4];

	// IH: TEMPORARY: This is stopping my disk working - get it out.
#if BPLATFORM==GAMECUBE
	return;
#endif

	if((tazboss1Mirrorball->clock -= fTime) < 0.0f)
	{
		tazboss1Mirrorball->clock = 0.1f;
		for(i = 3;i >= 0;--i)
		{
			// CMD: choose random member
			newCols[i] = colour[num = bmRand()%(i+1)];
			// CMD: now shuffle the array
			colour[num] = colour[i];
		}
		baSetLightColour(&tazboss1Mirrorball->red, lightColours[newCols[0]][0], lightColours[newCols[0]][1], lightColours[newCols[0]][2]);
		baSetLightColour(&tazboss1Mirrorball->green, lightColours[newCols[1]][0], lightColours[newCols[1]][1], lightColours[newCols[1]][2]);
		baSetLightColour(&tazboss1Mirrorball->blue, lightColours[newCols[2]][0], lightColours[newCols[2]][1], lightColours[newCols[2]][2]);
		baSetLightColour(&tazboss1Mirrorball->yellow, lightColours[newCols[3]][0], lightColours[newCols[3]][1], lightColours[newCols[3]][2]);
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int DrawMirrorball(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->special == SPECIALTYPE_MIRRORBALL)
		{
			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);	
			bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 0);
			bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 1);

			baSetGlobalAlpha(ptr->currentAlpha);

#if BPLATFORM == PC
			bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif
#if BPLATFORM != GAMECUBE
			baEnableLight(&cartoonLight, FALSE);
			baEnableLight(&tazboss1Mirrorball->red, TRUE);
			baEnableLight(&tazboss1Mirrorball->green, TRUE);
			baEnableLight(&tazboss1Mirrorball->blue, TRUE);
			baEnableLight(&tazboss1Mirrorball->yellow, TRUE);
			baEnableLight(&tazboss1Mirrorball->bottom, TRUE);
#endif
			ptr->onScreen = baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_SOLID);

			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 0);
			bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 1);
#if BPLATFORM != GAMECUBE
			baEnableLight(&tazboss1Mirrorball->red, FALSE);
			baEnableLight(&tazboss1Mirrorball->green, FALSE);
			baEnableLight(&tazboss1Mirrorball->blue, FALSE);
			baEnableLight(&tazboss1Mirrorball->yellow, FALSE);
			baEnableLight(&tazboss1Mirrorball->bottom, FALSE);
			baEnableLight(&cartoonLight, TRUE);
#endif
			if (map.fogState==true)
			{
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
			}
		}
	}
	baSetGlobalAlpha(256);

	return ptr->onScreen;
}