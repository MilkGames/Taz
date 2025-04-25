// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : frontendanimation.cpp
//   Purpose : functions to control the frontend
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "main.h"
#include "actors.h"
#include "status.h"
#include "maths.h"
#include "characters.h"
#include "shadow.h"
#include "animation.h"
#include "frontendmain.h"
#include "optionsmenu.h"
#include "TextFX.h"
#include "volume.h"

FRONTENDANIMINFO	frontEndAnimInfo;

static bool	onTnt		= false;
static float smoothSfxTransitionVolume = 1.0f;
static bool grabbingTntHandle = true;

char	*idleAnims[] = 
{
	{"dancepose"},
	{"idle1"},
	{"idle9"},
	{"boogie1"},
	{"boogie2"},
	{"boogie3"},
	{"boogie4"},
	{"boogie5"},
	{"boogie6"},
	{"boogie7"},
	{"boogie8"},
};

#define NUM_IDLE_ANIMS	(11)

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseFrontEndAnimationPaths
	Purpose 	: initialises paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitialiseFrontEndAnimationPaths(void)
{
	ACTORINSTANCE	*tempInstance;
	int				i;
	int				flags = ACTORFLAG_CUSTOMDRAW;

	for(i=0;i<MENU_NUMSUBMENUS;i++)
	{
		switch(i)
		{
		case MENU_MAIN:
			continue;
		case MENU_STARTGAME:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_save01.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_save02.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_OPTIONS:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_opt01.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_opt02.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_AUDIO:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_sound01.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_sound02.obe","path");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_TWOPLAYER:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_multiplayer01.obe","twoplayer");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_multiplayer02.obe","twoplayer");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_RECORDS:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_records01.obe","records");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_records02.obe","records");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_EXTRAS:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_extras01.obe","extras");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_extras02.obe","extras");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_SLOTONE:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savea01.obe","slotOne");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savea02.obe","slotOne");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		case MENU_SLOTTWO:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_saveb01.obe","slotTwo");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_saveb02.obe","slotTwo");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;		
			break;
		case MENU_SLOTTHREE:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savec01.obe","slotThree");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_THERE] = tempInstance;
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savec02.obe","slotThree");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.paths[i][FEPATH_BACK] = tempInstance;
			break;
		}
	}
	//initialise specific save slot paths
	for(i=0;i<NUM_SAVE_SLOT_PATHS;i++)
	{
		switch(i)
		{
		case SLOT_ONE_TO_TWO:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_saveatob.obe","oneToTwo");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_TWO_TO_THREE:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savebtoc.obe","twoToThree");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_THREE_TO_TWO:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savectob.obe","threeToTwo");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_TWO_TO_ONE:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savebtoa.obe","twoToOne");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_ONE_TO_GAME:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savea201.obe","oneToGame");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_TWO_TO_GAME:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_saveb201.obe","twoToGame");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		case SLOT_THREE_TO_GAME:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tazpath_savec201.obe","threeToGame");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			ResetFrontEndAnimPath(tempInstance,"movetaz");
			frontEndAnimInfo.savePaths[i] = tempInstance;
			break;
		}
	}
	//initialise incedental objects
	for(i=0;i<NUM_FE_INCEDENTALS;i++)
	{
		switch(i)
		{
		case FE_GRAMOPHONE:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\gramophone.obe","gramophone");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			PlayAnimationByName(tempInstance,"static",1.0f,1,0,0.1f,NONE);
			frontEndAnimInfo.incedentals[FE_GRAMOPHONE] = tempInstance;
			break;
		case FE_ALARMCLOCK:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\alarmclock01.obe","clock");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			PlayAnimationByName(tempInstance,"static",1.0f,1,0,0.1f,NONE);
			frontEndAnimInfo.incedentals[FE_ALARMCLOCK] = tempInstance;
			break;
		case FE_TNTPLUNGER:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\tntplunger01.obe","plunger");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			
			frontEndAnimInfo.animInfo.progress = 0.0f;
			frontEndAnimInfo.animInfo.currentBlend = 0.0f;
			frontEndAnimInfo.animInfo.smooth = 0.06f;
		// NH: Keep the TNT anim at it's correct position
			FlushAnimationQueue(&tempInstance->actorInstance,0);
			PlayFixedBlendOfTwoAnimations(&tempInstance->actorInstance,"lowvolume","highvolume",&frontEndAnimInfo.animInfo,
										((float)gameStatus.globalSettings.sfxVolume/255.0f),0);

		//	PlayFixedBlendOfTwoAnimations(&tempInstance->actorInstance,"lowvolume","highvolume",&frontEndAnimInfo.animProgress,
		//								(float)gameStatus.globalSettings.sfxVolume/255.0f,0);
					
			//PlayAnimationByName(tempInstance,"static",1.0f,0,0,0.1f,NONE);
			frontEndAnimInfo.incedentals[FE_TNTPLUNGER] = tempInstance;
			break;
		case FE_TELEVISION:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\television01.obe","tv");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			PlayAnimationByName(tempInstance,"static",1.0f,0,0,0.1f,NONE);
			frontEndAnimInfo.incedentals[FE_TELEVISION] = tempInstance;
			break;
		case FE_PIZZAS:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\pizzaboxes.obe","pizza");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			switch(gameStatus.globalSettings.difficulty)
			{
			case DIFF_EASY:
				PlayAnimationByName(tempInstance,"easy",1.0f,0,0,0.1f,NONE);
				break;
			case DIFF_MEDIUM:
				PlayAnimationByName(tempInstance,"medium",1.0f,0,0,0.1f,NONE);
				break;
			case DIFF_HARD:
				PlayAnimationByName(tempInstance,"hard",1.0f,0,0,0.1f,NONE);
				break;
			}
			frontEndAnimInfo.incedentals[FE_PIZZAS] = tempInstance;
			break;
		case FE_DECKCHAIR:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\deckchair01.obe","deckchair");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			frontEndAnimInfo.incedentals[FE_DECKCHAIR] = tempInstance;
			break;
		case FE_INITCAMERA:
			tempInstance = CreateActorInstance(&map.animatedInstances,"extras\\camerastartpos.obe","deckchair");
			SETVECTOR(tempInstance->actorInstance.position, 0.0f, 0.0f, 0.0f, 1.0f);
			SETVECTOR(tempInstance->actorInstance.scale, 1.0f, 1.0f, 1.0f, 1.0f);
			bmQuatCopy(tempInstance->actorInstance.orientation, bIdentityQuaternion);
			tempInstance->flags |= flags;
			frontEndAnimInfo.incedentals[FE_INITCAMERA] = tempInstance;
			break;
		case FE_CHEST:
			tempInstance = FindActorInstanceInMap("chest_node01", 0, &map);
			frontEndAnimInfo.incedentals[FE_CHEST] = tempInstance;
			RemoveInstanceFromInstanceList(tempInstance,tempInstance->instanceList);
			AddInstanceToInstanceList(tempInstance,&map.animatedInstances);
			break;
		}
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: ChooseFrontEndAnimationPath
	Purpose 	: chooses paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int ChooseFrontEndAnimationPath(EMenuPage target)
{
	ACTORINSTANCE		*taz		= gameStatus.player[0].actorInstance;

	if(frontEndAnimInfo.targetMenu != frontEndAnimInfo.lastMenu)
	{
		bkPrintf("targetMenu %d != lastMenu %d \n", frontEndAnimInfo.targetMenu, frontEndAnimInfo.lastMenu);
		return FALSE;
	}

	if (target == MENU_GALLERY || target == MENU_SECRETS ||target == MENU_CREDITS)
	{
		bkPrintf("target == MENU_GALLERY || target == MENU_SECRETS ||target == MENU_CREDITS\n");
		return FALSE;
	}

	if (target == frontEndAnimInfo.lastMenu)
	{
		bkPrintf("target == lastMenu\n");
		return FALSE;
	}
	switch(frontEndAnimInfo.lastMenu)
	{
		case MENU_MAIN:
			if(target == MENU_TWOPLAYER)
			{
				if(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_THERE])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[target][FEPATH_THERE],0);
					PlayAnimationByName(frontEndAnimInfo.paths[target][FEPATH_THERE],"movetaz",5.0f,0,0,0.0f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			}
			if(target == MENU_EXTRAS)
			{
				if(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_THERE])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_THERE],0);
					PlayAnimationByName(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_THERE],"movetaz",1.5f,0,0,0.0f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			}
			if(frontEndAnimInfo.paths[target][FEPATH_THERE])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[target][FEPATH_THERE],0);
				PlayAnimationByName(frontEndAnimInfo.paths[target][FEPATH_THERE],"movetaz",3.0f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_STARTGAME:
			if((target == MENU_SLOTONE) || (target == MENU_SLOTTWO) || (target == MENU_SLOTTHREE))
			{
				if(frontEndAnimInfo.paths[target][FEPATH_THERE])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[target][FEPATH_THERE],0);
					PlayAnimationByName(frontEndAnimInfo.paths[target][FEPATH_THERE],"movetaz",3.0f,0,0,0.0f,NONE);
				}				
				break;
			}	
			if(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_BACK])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_BACK],"movetaz",3.0f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_OPTIONS:
			if(frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_BACK])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_BACK],"movetaz",3.0f,0,0,0.1f,NONE);
				FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TELEVISION],0);
				PlayAnimationByName(frontEndAnimInfo.incedentals[FE_TELEVISION],"static",1.0f,0,0,0.1f,NONE);
				FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
				PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"static",1.0f,1,0,0.1f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
//				frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
			}
			break;
		case MENU_AUDIO:
			if(frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_BACK])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_BACK],"movetaz",3.0f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_TWOPLAYER:
			if(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK],"movetaz",4.0f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_RECORDS:
			if(frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_BACK])
			{
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_BACK],"movetaz",3.0f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_EXTRAS:
			if(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_BACK])
			{
				//FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_CHEST],0);
				//PlayAnimationByName(frontEndAnimInfo.incedentals[FE_CHEST],"chestclosed",1.0f,0,0,0.1f,NONE);
				FlushAnimationQueue(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_BACK],0);
				PlayAnimationByName(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_BACK],"movetaz",1.5f,0,0,0.0f,NONE);
				RequestFrontEndStateChange(STATE_SPINUP);
			}
			break;
		case MENU_SLOTONE:
			switch(target)
			{
			case MENU_SLOTTWO:
				if(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO])
				{
					frontEndAnimInfo.currentSavePath = SLOT_ONE_TO_TWO;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO],"movetaz",1.5f,0,0,0.0f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
					frontEndAnimInfo.requestState = SG_IDLE;
				}
				break;
			case MENU_GAME:
				if(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME])
				{
					frontEndAnimInfo.currentSavePath = SLOT_ONE_TO_GAME;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME],"movetaz",1.5f,0,0,0.0f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			default:
				if(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_BACK])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_BACK],0);
					PlayAnimationByName(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_BACK],"movetaz",3.0f,0,0,0.0f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			}
			break;
		case MENU_SLOTTWO:
			switch(target)
			{
			case MENU_SLOTONE:
				if(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE])
				{
					frontEndAnimInfo.currentSavePath = SLOT_TWO_TO_ONE;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE],"movetaz",1.5f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
					frontEndAnimInfo.requestState = SG_IDLE;
				}
				break;
			case MENU_SLOTTHREE:
				if(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE])
				{
					frontEndAnimInfo.currentSavePath = SLOT_TWO_TO_THREE;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE],"movetaz",1.5f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
					frontEndAnimInfo.requestState = SG_IDLE;
				}
				break;
			case MENU_GAME:
				if(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME])
				{
					frontEndAnimInfo.currentSavePath = SLOT_TWO_TO_GAME;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME],"movetaz",1.5f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			default:
				if(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_BACK])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_BACK],0);
					PlayAnimationByName(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_BACK],"movetaz",3.0f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			}
			break;
		case MENU_SLOTTHREE:
			switch(target)
			{
			case MENU_SLOTTWO:
				if(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO])
				{
					frontEndAnimInfo.currentSavePath = SLOT_THREE_TO_TWO;
					FlushAnimationQueue(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],0);
					PlayAnimationByName(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO],"movetaz",1.5f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
					frontEndAnimInfo.requestState = SG_IDLE;
				}
				break;
			default:
				if(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_BACK])
				{
					FlushAnimationQueue(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_BACK],0);
					PlayAnimationByName(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_BACK],"movetaz",3.0f,0,0,0.1f,NONE);
					RequestFrontEndStateChange(STATE_SPINUP);
				}
				break;
			}
			break;
	}

	// NH: Temp
	if (target != MENU_GALLERY && target != MENU_SECRETS &&target != MENU_CREDITS)
		frontEndAnimInfo.targetMenu = target;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFrontEndAnimPaths
	Purpose 	: updates paths for taz between menu points
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateFrontEndAnimPaths(void)
{
	TBActorNodeInstance	*node;
	ACTORINSTANCE	*taz = gameStatus.player[0].actorInstance;

//	FEPATH_THERE	FEPATHBACK
//	baUpdateNodes(frontEndAnimInfo.paths[][],frontEndAnimInfo.paths[][],fTime,motion);

	switch(frontEndAnimInfo.targetMenu)
	{
	case MENU_MAIN:
		switch(frontEndAnimInfo.lastMenu)
		{
			case MENU_STARTGAME:
				if(!frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_save02");
				break;
			case MENU_OPTIONS:
				if(!frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_opt02");			
				break;
			case MENU_AUDIO:
				if(!frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_sound02");
				break;
			case MENU_TWOPLAYER:
				if(!frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_multiplayer02");
				break;
			case MENU_RECORDS:
				if(!frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_records02");
				break;
			case MENU_EXTRAS:
				if(!frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_extras02");
				break;
			case MENU_SLOTONE:
				if(!frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_BACK]) return;
				node  = baFindNode(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_savea02");
				break;
			case MENU_SLOTTWO:
				if(!frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_saveb02");
				break;
			case MENU_SLOTTHREE:
				if(!frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_BACK]) return;
				node = baFindNode(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_BACK]->actorInstance.rootNodeInstance,"taz_path_savec02");
				break;
		}
		if(node)
		{
			baPrepActorInstance(&frontEndAnimInfo.paths[frontEndAnimInfo.lastMenu][FEPATH_BACK]->actorInstance, TRUE);
			baGetNodesWorldPosition(&frontEndAnimInfo.paths[frontEndAnimInfo.lastMenu][FEPATH_BACK]->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.position);
		}
		
		if(!baIsActorAnimPlaying(&frontEndAnimInfo.paths[frontEndAnimInfo.lastMenu][FEPATH_BACK]->actorInstance,NULL,TRUE))
		{
			switch(frontEndAnimInfo.lastMenu)
			{
			case MENU_SLOTONE:
			case MENU_SLOTTWO:
			case MENU_SLOTTHREE:
				ResetFrontEndAnimPath(frontEndAnimInfo.paths[frontEndAnimInfo.lastMenu][FEPATH_BACK],"movetaz");
				frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu = MENU_STARTGAME;
				bkPrintf("Last And Targ = START GAME\n");
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			default:
				RequestFrontEndStateChange(STATE_SPINDOWN);
				bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,node->animState.orientation);
				ResetFrontEndAnimPath(frontEndAnimInfo.paths[frontEndAnimInfo.lastMenu][FEPATH_BACK],"movetaz");
				frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;
				RequestTazFrontEndAnimation("idle");
				break;
			}
		}
		return;
	case MENU_STARTGAME:
		if(!frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_STARTGAME][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_save01");
		break;
	case MENU_OPTIONS:
		if(!frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_OPTIONS][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_opt01");
		break;
	case MENU_AUDIO:
		if(!frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_AUDIO][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_sound01");
		break;
	case MENU_TWOPLAYER:
		if(!frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_TWOPLAYER][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_multiplayer01");
		break;
	case MENU_RECORDS:
		if(!frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_RECORDS][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_records01");
		break;
	case MENU_EXTRAS:
		if(!frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_THERE]) return;
		node = baFindNode(frontEndAnimInfo.paths[MENU_EXTRAS][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_extras01");
		break;
	case MENU_SLOTONE:
		switch(frontEndAnimInfo.lastMenu)
		{
		case MENU_SLOTTWO:
			if(!frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_TWO_TO_ONE]->actorInstance.rootNodeInstance,"taz_path_savebtoa");
			break;
		default:
			if(!frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE]) return;
			node = baFindNode(frontEndAnimInfo.paths[MENU_SLOTONE][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_savea01");
			break;
		}
		break;
	case MENU_SLOTTWO:
		switch(frontEndAnimInfo.lastMenu)
		{
		case MENU_SLOTONE:
			if(!frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_ONE_TO_TWO]->actorInstance.rootNodeInstance,"taz_path_saveatob");
			break;
		case MENU_SLOTTHREE:
			if(!frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_THREE_TO_TWO]->actorInstance.rootNodeInstance,"taz_path_savectob");
			break;
		default:
			if(!frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE]) return;
			node = baFindNode(frontEndAnimInfo.paths[MENU_SLOTTWO][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_saveb01");
			break;
		}
		break;
	case MENU_SLOTTHREE:
		switch(frontEndAnimInfo.lastMenu)
		{
		case MENU_SLOTTWO:
			if(!frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_TWO_TO_THREE]->actorInstance.rootNodeInstance,"taz_path_savebtoc");
			break;
		default:
			if(!frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE]) return;
			node = baFindNode(frontEndAnimInfo.paths[MENU_SLOTTHREE][FEPATH_THERE]->actorInstance.rootNodeInstance,"taz_path_savec01");
			break;
		}
		break;
	case MENU_GAME:
		switch(frontEndAnimInfo.lastMenu)
		{
		case MENU_SLOTONE:
			if(!frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_ONE_TO_GAME]->actorInstance.rootNodeInstance,"taz_path_savea201");
			break;
		case MENU_SLOTTWO:
			if(!frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_TWO_TO_GAME]->actorInstance.rootNodeInstance,"taz_path_saveb201");
			break;
		case MENU_SLOTTHREE:
			if(!frontEndAnimInfo.savePaths[SLOT_THREE_TO_GAME]) return;
			node = baFindNode(frontEndAnimInfo.savePaths[SLOT_THREE_TO_GAME]->actorInstance.rootNodeInstance,"taz_path_savec201");
			break;
		}
		break;
	}
	if(node)
	{
		//check to see if on standard path
		//or if on save slot path
		if((frontEndAnimInfo.lastMenu != MENU_STARTGAME)&&(frontEndAnimInfo.targetMenu>=MENU_SLOTONE))
		{
			baPrepActorInstance(&frontEndAnimInfo.savePaths[frontEndAnimInfo.currentSavePath]->actorInstance, TRUE);
			baGetNodesWorldPosition(&frontEndAnimInfo.savePaths[frontEndAnimInfo.currentSavePath]->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.position);
			if(!CheckIfPlayingAnimation(frontEndAnimInfo.savePaths[frontEndAnimInfo.currentSavePath],NULL))
			{
				ResetFrontEndAnimPath(frontEndAnimInfo.savePaths[frontEndAnimInfo.currentSavePath],"movetaz");
				
				bkPrintf("Just Finished Moving %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
				
				frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;
				
				bkPrintf("Set Menus to the same %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);

				RequestFrontEndStateChange(STATE_SPINDOWN);
				//reset taz
				bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,node->animState.orientation);
				RequestTazFrontEndAnimation("idle");
			}
		}
		else
		{
			baPrepActorInstance(&frontEndAnimInfo.paths[frontEndAnimInfo.targetMenu][FEPATH_THERE]->actorInstance, TRUE);
			baGetNodesWorldPosition(&frontEndAnimInfo.paths[frontEndAnimInfo.targetMenu][FEPATH_THERE]->actorInstance,node,gameStatus.player[0].actorInstance->actorInstance.position);
			if(!baIsActorAnimPlaying(&frontEndAnimInfo.paths[frontEndAnimInfo.targetMenu][FEPATH_THERE]->actorInstance,NULL,TRUE))
			{
				if(frontEndAnimInfo.targetMenu == MENU_STARTGAME)
				{
					STARTGAMEPAGE	*ptr = frontEndMenu.startGamePage;

					bkPrintf("Picking Route %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
					ResetFrontEndAnimPath(frontEndAnimInfo.paths[frontEndAnimInfo.targetMenu][FEPATH_THERE],"movetaz");
					
					// NH: Coming from main and got to start, so pick route to slot
					if(frontEndAnimInfo.lastMenu == MENU_MAIN)
					{
						frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;

						// NH: Move to which ever slot is selected (or default to slot one)
						if (ptr->page->selectedItem == ptr->slotTwoTb)
							ChooseFrontEndAnimationPath(MENU_SLOTTWO);
						else if (ptr->page->selectedItem == ptr->slotThreeTb)
							ChooseFrontEndAnimationPath(MENU_SLOTTHREE);
						else
							ChooseFrontEndAnimationPath(MENU_SLOTONE);

						bkPrintf("Picked Route %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
					}
					else
					{
						frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;
						ChooseFrontEndAnimationPath(MENU_MAIN);
						bkPrintf("4 %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
					}
				}
				else
				{
					// NH: Not going to start
					bkPrintf("Ready For Idle %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
					frontEndAnimInfo.lastMenu = frontEndAnimInfo.targetMenu;
					RequestFrontEndStateChange(STATE_SPINDOWN);
					//reset taz
					bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,node->animState.orientation);
					ResetFrontEndAnimPath(frontEndAnimInfo.paths[frontEndAnimInfo.targetMenu][FEPATH_THERE],"movetaz");
					bkPrintf("Go to idle %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
					if(frontEndAnimInfo.targetMenu == MENU_SLOTONE || frontEndAnimInfo.targetMenu == MENU_SLOTTWO || frontEndAnimInfo.targetMenu == MENU_SLOTTHREE)
					{
						//FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
						RequestTazFrontEndAnimation("idle");
					}
					bkPrintf("In Idle %d, %d\n", frontEndAnimInfo.lastMenu, frontEndAnimInfo.targetMenu);
				}
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: RequestFrontEndStateChange
	Purpose 	: changes taz's front end state
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int RequestFrontEndStateChange(ECharState newState)
{
	TBQuaternion	tempQuat;
	
	switch(newState)
	{
	case STATE_SPINUP:
		FlushAnimationQueue(gameStatus.player[0].actorInstance,0);
		PlayAnimationByName(gameStatus.player[0].actorInstance, "spinup", 2.0f, 0, 0, 0.1f, NONE);
		gameStatus.player[0].actorInstance->actorStatus->frontEndSubState = newState;
		return TRUE;
	case STATE_SPINDOWN:
		// reset behaviour
		if ((frontEndAnimInfo.targetMenu == frontEndMenu.currentPage) || (frontEndAnimInfo.targetMenu == MENU_EXTRAS && (frontEndMenu.currentPage == MENU_GALLERY || frontEndMenu.currentPage == MENU_SECRETS || frontEndMenu.currentPage == MENU_CREDITS)) || ((frontEndMenu.currentPage == MENU_STARTGAME) && (frontEndAnimInfo.targetMenu == frontEndAnimInfo.lastMenu)))
		{
			if(gameStatus.player[0].actorInstance->characterInfo->spinInfo)
			{
				gameStatus.player[0].actorInstance->characterInfo->spinInfo->spinUpdated = FALSE;
			}
		

			FlushAnimationQueue(gameStatus.player[0].actorInstance,0);
			bmQuatCopy(tempQuat,gameStatus.player[0].actorInstance->actorInstance.orientation);

			ChangeActorInstance(gameStatus.player[0].actorInstance,FindActorInActorList("extras\\tazdevil_frontend.obe"),15,NULL);
			SetShadowNode(gameStatus.player[0].actorInstance,NULL,"root");
			EnableActorCartoonRenderMode(&gameStatus.player[0].actorInstance->actorInstance);
			SetShadowNode(gameStatus.player[0].actorInstance,NULL,"snoutstem");
			bmQuatCopy(gameStatus.player[0].actorInstance->actorInstance.orientation,tempQuat);
			FlushAnimationQueue(gameStatus.player[0].actorInstance,0);
			
			FlushAnimationQueue(gameStatus.player[0].actorInstance,0);
			PlayAnimationByName(gameStatus.player[0].actorInstance, "spindown", 1.0f, 0, 0, 0.0f, NONE);
			SetToStartFrame(gameStatus.player[0].actorInstance);

			gameStatus.player[0].actorInstance->actorStatus->frontEndSubState = newState;
		}
		return TRUE;
	default:
		return FALSE;
	}

}

/*	--------------------------------------------------------------------------------
	Function 	: ResetFrontEndAnimPath
	Purpose 	: changes taz's front end state
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ResetFrontEndAnimPath(ACTORINSTANCE *actorInstance, char *animName)
{
	TBActorAnimSegment	*segment;

	//reset path
	FlushAnimationQueue(actorInstance,0);
	segment = baFindAnimSegment(&actorInstance->actorInstance,animName);
	baSetNodeFrame(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,segment->startFrame,TRUE);
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);
}

/*	--------------------------------------------------------------------------------
	Function 	: RequestTazFrontEndAnimation
	Purpose 	: puts in request for animation
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RequestTazFrontEndAnimation(char *animName)
{
	strcpy(frontEndAnimInfo.animRequest, animName);
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateTazFrontEndAnimation
	Purpose 	: check to see if an anim can play, and what action need to be taken
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateTazFrontEndAnimation(void)
{
	char				*animName	= frontEndAnimInfo.animRequest;
	ACTORINSTANCE		*taz		= gameStatus.player[0].actorInstance;
	ACTORINSTANCE		*pChest		= FindActorInstanceInMap("chest_node01", 0, &map);
	TBActorAnimSegment	*animSegment;
	static EDifficulty	diffShownAt = gameStatus.globalSettings.difficulty;
	static bool			vibShownAt	= gameStatus.globalSettings.vibration;
	static bool			subtitlesShownAt = gameStatus.globalSettings.subtitles;
	static bool			switchBetweenVibStates = false;
	static bool			goingToTnt	= false;
	static bool			diffChange	= false;
	static bool			vibChange	= false;
	static bool			subChange	= false;
	static bool			goingToClockIdle = false;
	
	if(animName == NULL) return FALSE;
	if((frontEndAnimInfo.targetMenu != MENU_STARTGAME) 
		&& ((frontEndAnimInfo.targetMenu != frontEndAnimInfo.lastMenu)
		&& (!(frontEndAnimInfo.requestState == GOING_HOME &&(frontEndAnimInfo.lastMenu == MENU_SLOTONE || frontEndAnimInfo.lastMenu == MENU_SLOTTWO || frontEndAnimInfo.lastMenu == MENU_SLOTTHREE))))) return FALSE;		//in transit

	//check for menu specific animation requests
	switch(frontEndAnimInfo.targetMenu)
	{
	case MENU_MAIN:
	case MENU_INTROMAIN:
		frontEndAnimInfo.currentState = MM_IDLE;

		switch (frontEndMenu.currentPage)	
		{	
		case MENU_RECORDS:
			ChooseFrontEndAnimationPath(MENU_RECORDS);	
			break;
		
		case MENU_TWOPLAYER:
			ChooseFrontEndAnimationPath(MENU_TWOPLAYER);	
			break;

		case MENU_OPTIONS:
			ChooseFrontEndAnimationPath(MENU_OPTIONS);	
			break;

		case MENU_AUDIO:
			ChooseFrontEndAnimationPath(MENU_AUDIO);	
			break;

		case MENU_STARTGAME:
			ChooseFrontEndAnimationPath(MENU_STARTGAME);	
			break;

		case MENU_EXTRAS:
		case MENU_GALLERY:
		case MENU_CREDITS:
			ChooseFrontEndAnimationPath(MENU_EXTRAS);	
			break;

		case MENU_MAIN:
		case MENU_INTROMAIN:
			if(!CheckIfPlayingAnimation(taz,NULL))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				FlushAnimationQueue(taz,0);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				animName[0] = 0;
				return TRUE;
			}
			else if(CheckIfPlayingAnimation(taz,"tazenter"))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				FlushAnimationQueue(taz,0);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,0,0.0f,NULL,NULL);
				animName[0] = 0;
				return TRUE;
			}	
			break;
		}
	
		return FALSE;

	case MENU_STARTGAME:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;

			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}
			break;

		// NH: If taz at idle
		case SG_IDLE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;
			}

			// NH: Randomise idle
			if(!CheckIfPlayingAnimation(taz,NULL))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				return TRUE;
			}
			break;
		
		case GOING_HOME:
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = MM_IDLE;
			}
			break;
		
		case NULL_STATE:
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;
			
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = SG_IDLE;
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);				
			}

			break;
		}
		return FALSE;

	// NH: Check that we are in OPTIONS menu
	case MENU_OPTIONS:
		switch (frontEndAnimInfo.currentState)
		{
			// NH: Check we are NOWHERE (In front of table)
			case MM_IDLE:
			case OM_NOWHERE:
				switch (frontEndAnimInfo.requestState)
				{
					// NH: Check we want to go to PIZZA IDLE
					case OM_PIZZA_IDLE:
						FlushAnimationQueue(taz,1);
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						PlayAnimationByName(taz,"gotopizza",1.0f,0,1,0.1f,NONE);
						PlayAnimationByName(taz,"pizzaidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						break;

					// NH: Check we want to go to CLOCK IDLE
					case OM_CLOCK_IDLE:
						FlushAnimationQueue(taz,1);
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						PlayAnimationByName(taz,"pickupclock",1.0f,0,1,0.1f,NONE);
						if (gameStatus.globalSettings.vibration)
						{
							PlayAnimationByName(taz,"idlevibrationon",0.5f,1,1,0.1f,NONE);
							bkPrintf("idlevibrationon 1\n");
						}
						else
							PlayAnimationByName(taz,"idlevibrationoff",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						bkPrintf("Call Pick Up Anim On Taz\n");
						break;
						
					// NH: Check we want to go to TV IDLE
					case OM_TV_IDLE:
					case OM_TV_TWIDDLE:
					case OM_TV_TWIDDLE2:
					case OM_TV_TWIDDLE3:
					case OM_TV_HIT:
						FlushAnimationQueue(taz,1);
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						PlayAnimationByName(taz,"grabtv",1.0f,0,1,0.1f,NONE);
						PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						break;

					case GOING_HOME:
						frontEndAnimInfo.currentState = GOING_HOME;
						ChooseFrontEndAnimationPath(MENU_MAIN);
						break;

					default:
						ChooseFrontEndAnimationPath(MENU_MAIN);
						break;
				}
				break;
			
			// NH: Check we are at PIZZA
			case OM_PIZZA_IDLE:
				diffChange = true;
				// NH: Depending on what diff we are supposed to be on and what we are showning, change the anim
				switch(gameStatus.globalSettings.difficulty)
				{
					// NH: If we want easy
					case DIFF_EASY:
						// NH: And were on medium
						if (diffShownAt == DIFF_MEDIUM)
							// NH: Do med to easy
							frontEndAnimInfo.requestState = OM_MEDIUM_TO_EASY;
							// NH: Else if were on hard
						else if (diffShownAt == DIFF_HARD)
							// NH: Do hard to med
							frontEndAnimInfo.requestState = OM_HARD_TO_MEDIUM;
						else
							diffChange = false;
						break;
					// NH: And so on...
					case DIFF_MEDIUM:
						if (diffShownAt == DIFF_EASY)
							frontEndAnimInfo.requestState = OM_EASY_TO_MEDIUM;
						else if (diffShownAt == DIFF_HARD)
							frontEndAnimInfo.requestState = OM_HARD_TO_MEDIUM;
						else
							diffChange = false;
						break;
					case DIFF_HARD:						
						if (diffShownAt == DIFF_EASY)
							frontEndAnimInfo.requestState = OM_EASY_TO_MEDIUM;
						else if (diffShownAt == DIFF_MEDIUM)
							frontEndAnimInfo.requestState = OM_MEDIUM_TO_HARD;
						else
							diffChange = false;
						break;
				}
				
				if (diffChange)
				{	
					switch (frontEndAnimInfo.requestState)
					{
						// NH: Check we want to do EASY -> MED 
						case OM_EASY_TO_MEDIUM:
							FlushAnimationQueue(taz,1);
							//frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
							PlayAnimationByName(taz,"easytomedium",1.0f,0,0,0.1f,NONE);
							PlayAnimationByName(taz,"pizzaidle",1.0f,1,1,0.1f,NONE);
							SetToStartFrame(taz);
							diffShownAt = DIFF_MEDIUM;
							frontEndAnimInfo.currentState = NULL_STATE;
					//frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
							break;

						// NH: Check we want to do MED -> HARD 
						case OM_MEDIUM_TO_HARD:
							FlushAnimationQueue(taz,1);
							//frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
							PlayAnimationByName(taz,"mediumtohard",1.0f,0,0,0.1f,NONE);
							PlayAnimationByName(taz,"pizzaidle",1.0f,1,1,0.1f,NONE);
							SetToStartFrame(taz);
							diffShownAt = DIFF_HARD;
							frontEndAnimInfo.currentState = NULL_STATE;
					//frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
							break;

						// NH: Check we want to do HARD -> MED 
						case OM_HARD_TO_MEDIUM:
							FlushAnimationQueue(taz,1);
							//frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
							PlayAnimationByName(taz,"hardtomedium",1.0f,0,0,0.1f,NONE);
							PlayAnimationByName(taz,"pizzaidle",1.0f,1,1,0.1f,NONE);
							SetToStartFrame(taz);
							diffShownAt = DIFF_MEDIUM;
							frontEndAnimInfo.currentState = NULL_STATE;
					//frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
							break;

						// NH: Check we want to do MED -> EASY 
						case OM_MEDIUM_TO_EASY:
							FlushAnimationQueue(taz,1);
							//frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
							PlayAnimationByName(taz,"mediumtoeasy",1.0f,0,0,0.1f,NONE);
							PlayAnimationByName(taz,"pizzaidle",1.0f,1,1,0.1f,NONE);
							SetToStartFrame(taz);
							diffShownAt = DIFF_EASY;
							frontEndAnimInfo.currentState = NULL_STATE;
					//frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
							break;
					}
				}
					
				if (frontEndAnimInfo.requestState != frontEndAnimInfo.currentState
					&& frontEndAnimInfo.requestState != OM_EASY_TO_MEDIUM 
					&& frontEndAnimInfo.requestState != OM_MEDIUM_TO_HARD
					&& frontEndAnimInfo.requestState != OM_HARD_TO_MEDIUM
					&& frontEndAnimInfo.requestState != OM_MEDIUM_TO_EASY)
					{
						// NH: Jump from PIZZA
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"leavepizza",1.0f,0,0,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
					}
					
				break;
				
			// NH: Check we are at CLOCK
			case OM_CLOCK_IDLE:
				vibChange = true;
				switch(gameStatus.globalSettings.vibration)
				{
					// NH: If we want vib on
					case true:
						// NH: And were currently off
						if (vibShownAt == false)
							// NH: Do vib off to on
							frontEndAnimInfo.requestState = OM_VIB_TO_ON;
						else
							vibChange = false;
						break;
					// NH: or off
					case false:						
						if (vibShownAt == true)
							// NH: Do vib on to off
							frontEndAnimInfo.requestState = OM_VIB_TO_OFF;
						else
							vibChange = false;
						break;
				}
				
				if (vibChange)
				{	
					switch (frontEndAnimInfo.requestState)
					{
						// NH: Check we want to do VIB OFF -> ON 
						case OM_VIB_TO_ON:
							// NH: Play switch sample
							PlaySample("alarmclockswitch.wav", 255);
							FlushAnimationQueue(taz,1);
							PlayAnimationByName(taz,"turnvibrationon",1.0f,0,0,0.1f,NONE);
							SetCallbackOnNamedAnimation(&taz->actorInstance, "turnvibrationon", (TBQueuedAnimCallback)FrontEndRumbleCallback, (void*)frontEndMenu.gameOptionsPage);
							PlayAnimationByName(taz,"idlevibrationon",0.5f,1,1,0.1f,NONE);
							bkPrintf("idlevibrationon 2\n");
							SetToStartFrame(taz);
							vibShownAt = true;
							frontEndAnimInfo.currentState = NULL_STATE;
							break;

						// NH: Check we want to do VIB ON -> OFF 
						case OM_VIB_TO_OFF:
							// NH: Play switch sample
							PlaySample("alarmclockswitch.wav", 255);
							FlushAnimationQueue(taz,1);
							PlayAnimationByName(taz,"turnvibrationoff",1.0f,0,0,0.1f,NONE);
							PlayAnimationByName(taz,"idlevibrationoff",1.0f,1,1,0.1f,NONE);
							SetToStartFrame(taz);
							vibShownAt = false;
							frontEndAnimInfo.currentState = NULL_STATE;
							break;
					}
				}
					
				if (frontEndAnimInfo.requestState != frontEndAnimInfo.currentState
					&& frontEndAnimInfo.requestState != OM_VIB_TO_ON 
					&& frontEndAnimInfo.requestState != OM_VIB_TO_OFF)
					{
						// NH: Jump off the CLOCK!
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"putclockback",1.0f,0,0,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;					
					}	
				break;
			
			// NH: Check we are at TV
			case OM_TV_IDLE:
				if (frontEndMenu.frontEndMenuBook->getCurrentPage() != frontEndMenu.gameOptionsPage->adjustScreenPage)
				{
					subChange = true;
					switch(gameStatus.globalSettings.subtitles)
					{
						case true:
							if (subtitlesShownAt == false)
							{
								frontEndAnimInfo.requestState = OM_TV_TWIDDLE;
								subtitlesShownAt = true;
							}
							else
								subChange = false;
							break;
						case false:
							if (subtitlesShownAt == true)
							{
								frontEndAnimInfo.requestState = OM_TV_TWIDDLE;
								subtitlesShownAt = false;
							}
							else
								subChange = false;
							break;
					}

					if (subChange)
					{
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"tvadjust1",1.0f,0,0,0.1f,NONE);
						PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
					}

					if (frontEndAnimInfo.requestState != frontEndAnimInfo.currentState
						&& frontEndAnimInfo.requestState != OM_TV_TWIDDLE
						&& frontEndAnimInfo.requestState != NULL_STATE)
					{
						// NH: Just jump off the damn TV!
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"leavetv",1.0f,0,0,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
					}					
				}
				else
				{
					switch (frontEndAnimInfo.requestState)
					{
					case OM_TV_TWIDDLE:
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"tvadjust1",1.0f,0,0,0.1f,NONE);
						PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						frontEndAnimInfo.requestState = NULL_STATE;
						break;

					case OM_TV_TWIDDLE2:
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"tvadjust2",1.0f,0,0,0.1f,NONE);
						PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						frontEndAnimInfo.requestState = NULL_STATE;
						break;

					case OM_TV_TWIDDLE3:
						frontEndAnimInfo.lastState = frontEndAnimInfo.currentState;
						FlushAnimationQueue(taz,1);
						PlayAnimationByName(taz,"tvadjust3",1.0f,0,0,0.1f,NONE);
						PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
						SetToStartFrame(taz);
						frontEndAnimInfo.currentState = NULL_STATE;
						frontEndAnimInfo.requestState = NULL_STATE;
						break;

					}
				}
				
				switch (frontEndAnimInfo.requestState)
				{
				case OM_TV_HIT:		
					FlushAnimationQueue(taz,1);
					PlayAnimationByName(taz,"tvselect",1.0f,0,0,0.1f,NONE);
					PlayAnimationByName(taz,"tvidle",1.0f,1,1,0.1f,NONE);
					SetToStartFrame(taz);
					frontEndAnimInfo.currentState = NULL_STATE;
					frontEndAnimInfo.requestState = NULL_STATE;
					break;
				}
				break;
			
			// NH: If NULL STATE
			case NULL_STATE:
				switch (frontEndAnimInfo.lastState)
				{
					case MM_IDLE:
					case OM_NOWHERE:
						if (CheckIfPlayingAnimation(taz,"tvidle"))
						{
							frontEndAnimInfo.currentState = OM_TV_IDLE;
						}
						else if (CheckIfPlayingAnimation(taz,"pizzaidle"))
						{
							frontEndAnimInfo.currentState = OM_PIZZA_IDLE;
						}
						else if ((CheckIfPlayingAnimation(taz,"idlevibrationoff")) || (CheckIfPlayingAnimation(taz,"idlevibrationon")))
						{
							frontEndAnimInfo.currentState = OM_CLOCK_IDLE;
						}
						break;

					case OM_PIZZA_IDLE:
						if ((diffChange) && (CheckIfPlayingAnimation(taz,"pizzaidle")))
						{
							frontEndAnimInfo.currentState = OM_PIZZA_IDLE;
						}
						else if (CheckIfPlayingAnimation(taz,"pizzaidle"))
						{
							frontEndAnimInfo.currentState = NULL_STATE;
						}
						else if ((!diffChange) && (!CheckIfPlayingAnimation(taz,"leavepizza")))
						{
							frontEndAnimInfo.currentState = OM_NOWHERE;
						}
						break;
					
					case OM_CLOCK_IDLE:
						if ((vibChange) && (CheckIfPlayingAnimation(taz,"idlevibrationoff") || CheckIfPlayingAnimation(taz,"idlevibrationon")))
						{
							frontEndAnimInfo.currentState = OM_CLOCK_IDLE;
						}
						else if (CheckIfPlayingAnimation(taz,"idlevibrationoff") || CheckIfPlayingAnimation(taz,"idlevibrationon"))
						{
							frontEndAnimInfo.currentState = NULL_STATE;
						}
						else if ((!vibChange) && (!CheckIfPlayingAnimation(taz,"putclockback")))
						{
							frontEndAnimInfo.currentState = OM_NOWHERE;
						}
						break;
					
					case OM_TV_IDLE:
						if (CheckIfPlayingAnimation(taz,"tvidle"))
						{
							frontEndAnimInfo.currentState = OM_TV_IDLE;
						}
						else if (CheckIfPlayingAnimation(taz,"tvadjust1") || CheckIfPlayingAnimation(taz,"tvadjust2") || CheckIfPlayingAnimation(taz,"tvadjust3") || CheckIfPlayingAnimation(taz,"tvselect"))
						{
							frontEndAnimInfo.currentState = NULL_STATE;
						}
						else if (!CheckIfPlayingAnimation(taz,"leavetv"))
						{
							frontEndAnimInfo.currentState = OM_NOWHERE;
						}
						break;
				}
				break;
		}
		return FALSE;

	case MENU_AUDIO:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If taz has been requested to goto gramophone (idle)			
			case AM_GRAMOPHONE:
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"grabgramophone",1.0f,0,1,0.1f,NONE);
				PlayAnimationByName(taz,"windgramophone1",1.0f,1,1,0.1f,NONE);
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = NULL_STATE;
				grabbingTntHandle = false;
				break;

			case AM_PLUNGER:
				smoothSfxTransitionVolume = 1.0f;
				grabbingTntHandle = true;
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"grabplunger",1.0f,0,1,0.1f,NONE);
				PlayAnimationByName(taz,"sfxhighvolume",0.01f,0,1,0.1f,NONE);
				
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;
		
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				ChooseFrontEndAnimationPath(MENU_MAIN);
				grabbingTntHandle = false;
				break;
			
			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}

			break;

		// NH: If taz at gramophone (idle)
		case AM_GRAMOPHONE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				ChooseFrontEndAnimationPath(frontEndMenu.currentPage);
				break;
			
			// NH: Or plunger idle
			case AM_PLUNGER:
				smoothSfxTransitionVolume = 1.0f;
				grabbingTntHandle = true;
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"grabplunger",1.0f,0,0,0.1f,NONE);
				PlayAnimationByName(taz,"sfxhighvolume",0.01f,0,1,0.1f,NONE);
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;
			}
			break;

		case GOING_HOME:
			if (!CheckIfPlayingAnimation(taz,"leaveplunger"))
			{
				frontEndAnimInfo.currentState = MM_IDLE;//ChooseFrontEndAnimationPath(frontEndMenu.currentPage);
			}
			onTnt = false;
			break;
			
		// NH: If taz at plunger (idle)
		case AM_PLUNGER:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: Or gramophone idle
			case AM_GRAMOPHONE:
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"leaveplunger",1.0f,0,0,0.0f,NONE);
				PlayAnimationByName(taz,"grabgramophone",1.0f,0,1,0.1f,NONE);
				PlayAnimationByName(taz,"windgramophone1",1.0f,1,1,0.1f,NONE);
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = NULL_STATE;
				onTnt = false;
				break;

			case AM_PLUNGER:
				smoothSfxTransitionVolume = 1.0f;
				grabbingTntHandle = true;
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"grabplunger",1.0f,0,0,0.1f,NONE);
				PlayAnimationByName(taz,"sfxhighvolume",0.01f,0,1,0.1f,NONE);
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;

				// TP: Nothing for this I take it
			case NULL_STATE:
				break;

				// NH: If requested to return to main menu (idle)
				// TP: Need to take care of the possibility of exiting from plunger straight to records etc. (BUG:87021A
			default:
				FlushAnimationQueue(taz,1);
				PlayAnimationByName(taz,"leaveplunger",1.0f,0,0,0.0f,NONE);
				SetToStartFrame(taz);
				frontEndAnimInfo.currentState = GOING_HOME;
				onTnt = false;
				break;

			}
			break;

		case NULL_STATE:
			if (CheckIfPlayingAnimation(taz,"grabgramophone") || CheckIfPlayingAnimation(taz,"windgramophone1"))
			{
				frontEndAnimInfo.currentState = AM_GRAMOPHONE; 
				onTnt = false;
			}
			else if (CheckIfPlayingAnimation(taz,"sfxlowvolume") || CheckIfPlayingAnimation(taz,"sfxhighvolume"))
			{				
				frontEndAnimInfo.currentState = AM_PLUNGER;
				onTnt = true;
			}
			
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;			

			break;
		}
		return FALSE;

	case MENU_TWOPLAYER:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If taz has been requested to goto idle (idle)
			case TP_IDLE:				
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,101);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;

			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;

			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}
			break;

		// NH: If taz at idle
		case TP_IDLE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;
			}

			// NH: Randomise idle
			if(!CheckIfPlayingAnimation(taz,NULL))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,25);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				return TRUE;
			}

			break;

		case GOING_HOME:
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = MM_IDLE;
			}
			break;

		case NULL_STATE:
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;
			
			if (!CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = TP_IDLE;
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				FlushAnimationQueue(taz,0);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);				
			}

			
			break;
		}
		return FALSE;

	case MENU_RECORDS:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If taz has been requested to goto idle (idle)
			case RM_IDLE:	
				//FlushAnimationQueue(taz,0,101);
				PlayAnimationByName(taz,"recordsidle",1.0f,1,1,0.1f,NONE);
				SetToStartFrame(taz);

				frontEndAnimInfo.currentState = NULL_STATE;
				break;

			case GOING_HOME:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				frontEndAnimInfo.currentState = GOING_HOME;
				break;

			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}
			break;

		// NH: If taz at idle
		case RM_IDLE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;
			}
			break;

		case GOING_HOME:
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = MM_IDLE;
				ChooseFrontEndAnimationPath(frontEndMenu.currentPage);
			}
			break;
		
		case NULL_STATE:
			
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;

			if (!CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = RM_IDLE;
				//FlushAnimationQueue(taz,0,101);
				PlayAnimationByName(taz,"recordsidle",1.0f,1,1,0.1f,NONE);
				SetToStartFrame(taz);
			}
			break;
		}
		return FALSE;

	case MENU_EXTRAS:
	case MENU_CREDITS:
	case MENU_GALLERY:
	case MENU_SECRETS:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If taz has been requested to goto idle (idle)
			case EM_IDLE:	
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,101);
				FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;
			
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				ChooseFrontEndAnimationPath(frontEndMenu.currentPage);
				break;

			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}
			break;

		// NH: If taz at idle
		case EM_IDLE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;
			}

			// NH: Randomise idle
			if(!CheckIfPlayingAnimation(taz,NULL))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,25);
				//FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				return TRUE;
			}
			break;
		
		case GOING_HOME:
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = MM_IDLE;				
			}
			break;
		
		case NULL_STATE:
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;
			
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = EM_IDLE;
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,101);
				//FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);				
			}

			break;
		}
		return FALSE;

	case MENU_SLOTONE:
	case MENU_SLOTTWO:
	case MENU_SLOTTHREE:
	case MENU_GAME:
		switch (frontEndAnimInfo.currentState)
		{
		// NH: If currently in main menus idle state..
		case MM_IDLE:
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If taz has been requested to goto idle (idle)
			case SG_IDLE:	
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,101);
				FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				frontEndAnimInfo.currentState = NULL_STATE;
				break;
			
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;

			default:
				ChooseFrontEndAnimationPath(MENU_MAIN);
				break;
			}
			break;

		// NH: If taz at idle
		case SG_IDLE:	
			switch (frontEndAnimInfo.requestState)
			{
			// NH: If requested to return to main menu (idle)
			case GOING_HOME:
				frontEndAnimInfo.currentState = GOING_HOME;
				break;
			}

			// NH: Randomise idle
			if(!CheckIfPlayingAnimation(taz,NULL))
			{
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,25);
				//FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);
				return TRUE;
			}
			break;
		
		case GOING_HOME:
			if (CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = MM_IDLE;
			}
			break;
		
		case NULL_STATE:
			if (frontEndAnimInfo.requestState == frontEndAnimInfo.currentState)
				frontEndAnimInfo.requestState = NULL_STATE;
			
			if (!CheckIfPlayingAnimation(taz,NULL))
			{
				frontEndAnimInfo.currentState = SG_IDLE;
				animSegment = ChooseIdleAnimationForTazInFrontEnd(taz);
				//flush queue only
				//FlushAnimationQueue(taz,0,101);
				//FrontEndRotateTazCallback(BANIMQUEUEREASON_ENTRYCOMPLETE,&gameStatus.player[0].actorInstance->actorInstance,NULL,NULL,NULL);
				PlayAnimationBySegmentPointerAndSetCallback(&taz->actorInstance,animSegment,1.0f,0,1,0.1f,NULL,NULL);				
			}

			break;
		}
		return FALSE;
		
	default:
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateIncedentalsFrontEndAnimation
	Purpose 	: check to see if an anim can play, and what action need to be taken
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int UpdateIncedentalsFrontEndAnimation(void)
{
	ACTORINSTANCE	*taz = gameStatus.player[0].actorInstance;
	TBActorAnimSegment	*anim;
	int				currSourceFrame;
	ANIMBLENDINFO	tntProgress;
	static bool		e2m = false;
	static bool		m2h = false;
	static bool		h2m = false;
	static bool		m2e = false;
	float			tempVol;
	float			currentVol;
	
	tntProgress.currentBlend = 0.0f;
	tntProgress.progress = 0.0f;
	tntProgress.smooth = 0.06f;
	if (!onTnt)
	{
		// NH: Keep the TNT anim at it's correct position
		FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TNTPLUNGER],0);
		PlayFixedBlendOfTwoAnimations(&frontEndAnimInfo.incedentals[FE_TNTPLUNGER]->actorInstance,"lowvolume","highvolume",
							&frontEndAnimInfo.tntBlendInfo,	((float)gameStatus.globalSettings.sfxVolume/255.0f),0);
	}
			
	// NH: Play idle gramophone
	if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_GRAMOPHONE],"idle"))
	{
		if (!CheckIfPlayingAnimation(taz,"windgramophone1"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_GRAMOPHONE],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_GRAMOPHONE],"idle",1.0f,1,0,0.1f,NONE);

		}
	}

	// NH: Play TV spark idle
	if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_TELEVISION],"tv_spark"))
	{
		FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TELEVISION],0);
		PlayAnimationByName(frontEndAnimInfo.incedentals[FE_TELEVISION],"tv_spark",1.0f,1,0,0.1f,NONE);
	}

	//play anims on objects to match what anim taz is currently playing
	if(CheckIfPlayingAnimation(taz,"pickupclock"))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"pickupclock") && CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"static"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"pickupclock",1.0f,0,0,0.0f,NONE);
			SetToStartFrame(frontEndAnimInfo.incedentals[FE_ALARMCLOCK]);
			
			return TRUE;
		}
		return FALSE;
	}
	if(CheckIfPlayingAnimation(taz,"turnvibrationoff"))
	{
		if((!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"turnvibrationoff")) && (CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationon")))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"turnvibrationoff",1.0f,0,0,0.0f,NONE);
			return TRUE;
		}
		return FALSE;
	}
	if(CheckIfPlayingAnimation(taz,"idlevibrationoff"))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationoff"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationoff",1.0f,1,0,0.0f,NONE);
			return TRUE;
		}
		return FALSE;
	}
	if(CheckIfPlayingAnimation(taz,"turnvibrationon"))
	{
		if((!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"turnvibrationon")) && (CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationoff")))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"turnvibrationon",1.0f,0,0,0.0f,NONE);
			bkPrintf("On\n");
			return TRUE;
		}
		return FALSE;
	}
	if(CheckIfPlayingAnimation(taz,"idlevibrationon"))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationon"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"idlevibrationon",0.5f,1,0,0.0f,NONE);
			bkPrintf("idlevibrationon 3\n");
			return TRUE;
		}
		return FALSE;
	}
	if(CheckIfPlayingAnimation(taz,"putclockback"))
	{
		if((!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"putclockback")) && (!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"static")))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"putclockback",1.0f,0,0,0.1f,NONE);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_ALARMCLOCK],"static",1.0f,1,1,0.1f,NONE);
			SetToStartFrame(frontEndAnimInfo.incedentals[FE_ALARMCLOCK]);
			
			return TRUE;
		}
		return FALSE;
	}
	
	if(CheckIfPlayingAnimation(taz,"tvselect"))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_TELEVISION],"tvselect"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TELEVISION],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_TELEVISION],"tvselect",1.0f,0,0,0.1f,NONE);
			return TRUE;
		}
		return FALSE;
	}
	
	if(CheckIfPlayingAnimation(taz,"windgramophone1"))
	{
			PlayFixedBlendOfTwoAnimations(&frontEndAnimInfo.incedentals[FE_GRAMOPHONE]->actorInstance,"lowvolume","highvolume",
											&frontEndAnimInfo.animInfo,(float)gameStatus.globalSettings.musicVolume/225.0f,
											ANIMFLAG_UPDATE|ANIMFLAG_LOOP);
			anim = baFindAnimSegment(&taz->actorInstance,"windgramophone1");
			currSourceFrame = (int)(frontEndAnimInfo.animInfo.progress * (float)(anim->endFrame - anim->startFrame)) + anim->startFrame;
			baSetNodeFrame(&taz->actorInstance,taz->actorInstance.rootNodeInstance,currSourceFrame,TRUE);
			return TRUE;
	}
	
	if(onTnt)
	{
		frontEndAnimInfo.animInfo.progress = 0.0f;
		FlushAnimationQueue(taz,0);

		currentVol = ((float)gameStatus.globalSettings.sfxVolume/255.0f);
		
		tempVol = smoothSfxTransitionVolume - currentVol;
		
		// NH: Just here to make transition look nice and smooth
		if (grabbingTntHandle)
		{
			// NH: Move Taz so he grabs plunger smoothly
			// NH: Over half the travel distance to move so smooth it out
			smoothSfxTransitionVolume -= (tempVol/0.2f * fTime);

			if (smoothSfxTransitionVolume <= currentVol+0.01f)
			{
				smoothSfxTransitionVolume = currentVol;
				grabbingTntHandle = volumeStuff.ShovePlungerDown = false;
			}
		}
		else if (volumeStuff.ShovePlungerDown)
		{
			// NH: Slam it down!
			if (smoothSfxTransitionVolume > 0.0f)
			{
				smoothSfxTransitionVolume -= (1.0f/0.1f * fTime);				
			}
			else
			{
				//frontEndMenu.audioPage->ShovePlungerDown = false;
				volumeStuff.ShovePlungerDown = false;
			}
		}
		else
		{
			if (tempVol > 0.0f)
			{
				smoothSfxTransitionVolume -= ((tempVol)/0.25f * fTime);
			}
			else if (tempVol < 0.0f)
			{
				smoothSfxTransitionVolume += ((-tempVol)/0.25f * fTime);
			}			
		}
		smoothSfxTransitionVolume = CONSTRAIN(smoothSfxTransitionVolume, 0, 1);
				
		if (!grabbingTntHandle)
		{
			// NH: Keep the TNT anim at it's correct position
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TNTPLUNGER],0);
			PlayFixedBlendOfTwoAnimations(&frontEndAnimInfo.incedentals[FE_TNTPLUNGER]->actorInstance,"lowvolume","highvolume",
								&frontEndAnimInfo.tntBlendInfo,smoothSfxTransitionVolume,0);
		}
		else
		{
			// NH: Keep the TNT anim at it's correct position
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_TNTPLUNGER],0);
			PlayFixedBlendOfTwoAnimations(&frontEndAnimInfo.incedentals[FE_TNTPLUNGER]->actorInstance,"lowvolume","highvolume",
								&frontEndAnimInfo.tntBlendInfo,((float)gameStatus.globalSettings.sfxVolume/255.0f),0);
		}

		PlayFixedBlendOfTwoAnimations(&taz->actorInstance,"sfxlowvolume","sfxhighvolume",&frontEndAnimInfo.tntBlendInfo,
										smoothSfxTransitionVolume,0);
		return TRUE;
	}

	if(frontEndAnimInfo.targetMenu == MENU_SECRETS  || frontEndMenu.currentPage == MENU_SECRETS)
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_CHEST],"opening") &&
			!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_CHEST],"chestopen"))
		{
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_CHEST],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_CHEST],"opening",1.0f,0,0,0.1f,NONE);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_CHEST],"chestopen",1.0f,1,0,0.1f,NONE);
			return TRUE;
		}	
		return FALSE;
	}
	else if (CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_CHEST],"chestopen") ||
			CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_CHEST],"opening"))
	{
		FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_CHEST],0);
		PlayAnimationByName(frontEndAnimInfo.incedentals[FE_CHEST],"closing",1.0f,0,0,0.1f,NONE);
		PlayAnimationByName(frontEndAnimInfo.incedentals[FE_CHEST],"chestclosed",1.0f,1,0,0.1f,NONE);
		return TRUE;
	}

	if((CheckIfPlayingAnimation(taz,"easytomedium")) && (e2m == false))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"easytomedium"))
		{
			e2m = true;
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_PIZZAS],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_PIZZAS],"easytomedium",1.0f,0,0,0.1f,NONE);
		}
	}
	else if (!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"easytomedium"))
	{
		e2m = false;
	}

	if((CheckIfPlayingAnimation(taz,"mediumtohard")) && (m2h == false))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtohard"))
		{
			m2h = true;
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_PIZZAS],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtohard",1.0f,0,0,0.1f,NONE);
		}
	}
	else if (!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtohard"))
	{
		m2h = false;
	}

	if((CheckIfPlayingAnimation(taz,"hardtomedium")) && (h2m == false))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"hardtomedium"))
		{
			h2m = true;
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_PIZZAS],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_PIZZAS],"hardtomedium",1.0f,0,0,0.1f,NONE);
		}
	}
	else if (!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"hardtomedium"))
	{
		h2m = false;
	}

	if((CheckIfPlayingAnimation(taz,"mediumtoeasy")) && (m2e == false))
	{
		if(!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtoeasy"))
		{
			m2e = true;
			FlushAnimationQueue(frontEndAnimInfo.incedentals[FE_PIZZAS],0);
			PlayAnimationByName(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtoeasy",1.0f,0,0,0.1f,NONE);
		}
	}
	else if (!CheckIfPlayingAnimation(frontEndAnimInfo.incedentals[FE_PIZZAS],"mediumtoeasy"))
	{
		m2e = false;
	}
	
	//not going to start any new anims
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: ChooseIdleAnimationForTazInFrontEnd
	Purpose 	: chooses an idle anim for taz from list at start of frontendanimation.cpp
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

TBActorAnimSegment *ChooseIdleAnimationForTazInFrontEnd(ACTORINSTANCE *actorInstance)
{
	return baFindAnimSegment(&actorInstance->actorInstance,idleAnims[bmRand()%NUM_IDLE_ANIMS]);
}

/*	--------------------------------------------------------------------------------
	Function 	: FrontEndRotateTazCallback
	Purpose 	: rotates taz upon completion of animation
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

int FrontEndRotateTazCallback(EBQueueCallbackReason reasonCode,TBActorInstance *actor,TBActorNodeInstance *node,
							  TBActorAnimSegment *segment,void *context)
{
	TBQuaternion	tempQuat,tempQuat2;

	switch(reasonCode)
	{
	case BANIMQUEUEREASON_ENTRYCOMPLETE:
	case BANIMQUEUEREASON_FLUSHED:
		bmQuatYRotation(tempQuat, RAD(200.0f));
		bmQuatCopy(tempQuat2, actor->orientation);
		bmQuatMultiply(actor->orientation, tempQuat, tempQuat2);
		baPrepActorInstance(actor, TRUE);
		return TRUE;
	}
	return FALSE;
}
