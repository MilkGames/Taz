// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : taztoproulette.cpp
//   Purpose : taztop roulette related functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "actors.h"
#include "main.h"
#include "trigger.h"
#include "maths.h"
#include "fallingobjects.h"
#include "scene.h"
#include "playerstats.h"
#include "phonebox.h"
#include "sfx.h"
#include "phoneboxeffect.h"
#include "animation.h"
#include "characters.h"
#include "status.h"
#include "shadow.h"
#include "costumes.h"
#include "control.h"
#include "physics.h"
#include "animation.h"
#include "MonitorFX.h"					// PP: for the watch
#include "music.h"
#include "genericai.h"
#include "glowaroundplinth.h"
#include "prompt.h"
#include "Cheats.h"						// PP: Cheats

#include "superburp.h"					// PP: functions to create the super burp
#include "invisibletaz.h"				// PP: functions to update and draw invisibility effects
#include "hiccup.h"						// PP: functions to update and draw hiccup effects
#include "chillipepper.h"				// PP: functions to update and draw chilli-breath effects
#include "bubblegum.h"					// PP: functions to update and draw bubblegum effects

/*
Underground
Arrive1
Idle1
Tazenter
Changeclothes1
Changeclothes2
Tazexit1
Tazexit2
Tazexit3
Disappear1
Disappear2
*/
enum EPhoneState
{
	PHONESTATE_INACTIVE,
	PHONESTATE_ACTIVATING,
	PHONESTATE_ACTIVE,
	PHONESTATE_TAZENTER,
	PHONESTATE_CHANGING,
	PHONESTATE_ENTERUSINGPOLICEBOX,
	PHONESTATE_TAZEXIT,
	PHONESTATE_STARTEFFECT,
	PHONESTATE_WAITFOREXIT,
};

float	phoneBoxesActiveTimer = 0.0f;		// current time left for phone boxes active
float	phoneboxBookTimer = 0.0f;

int		ringSoundHandle=-1;					// sound handle for ring sound

int		PhoneBoxLoopChannel, PhoneBoxRingChannel;

BOOK	*phoneboxBook = 0;

/*  --------------------------------------------------------------------------------
	Function	: IsPoliceBoxAccessible
	Purpose		: 
	Parameters	: level name that police box is trying to enter
	Returns		: true if ok to enter
	Info		: 
*/

bool IsPoliceBoxAccessible(char *level)
{
	// TP: Check the player has collected enough food
	int levelnum = FindLevel(level);
#ifndef INFOGRAMES
	//return true;
#endif

	if (CheatActive(CHEAT_OPEN_BONUS_GAMES)) return true;

	switch(levelnum)
	{
	case SCENE_MUSEUMVEHICLERACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_MUSEUM].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_GRANDCVEHICLERACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_GRANDCANYON].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_DEPTSTRVEHICLERACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_DEPTSTR].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_DESTRUCTIONAQUA:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_AQUA].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_DESTRUCTIONCONSTRUCT:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_CONSTRUCT].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_DESTRUCTIONTASMANIA:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_GHOSTTOWN].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_ICEDOMERACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_ICEDOME].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_GOLDMINERACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_GOLDMINE].pickupsCollected<100)
		{
			return false;
		}
		break;
	case SCENE_SAFARIRACE:
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_SAFARI].pickupsCollected<100)
		{
			return false;
		}
		break;
	}

	return true;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePhoneBox
   Purpose : Called to create the phone box
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
int CreateActorInstancePhoneBox(ACTORINSTANCE *actorInstance,ACTORINSTANCE *collisionInstance,int type,char *level)
{
	actorInstance->phoneBoxInfo = (PHONEBOXINFO*)ZALLOC(sizeof(PHONEBOXINFO));
	if (actorInstance->phoneBoxInfo==NULL)
	{
		bkPrintf("   *** ERROR *** Could not allocate memory for phone box.\n");
		return FALSE;
	}
	collisionInstance->phoneBoxInfo = (PHONEBOXINFO*)ZALLOC(sizeof(PHONEBOXINFO));
	if (collisionInstance->phoneBoxInfo==NULL)
	{
		bkPrintf("   *** ERROR *** Could not allocate memory for phone box.\n");
		return FALSE;
	}

	actorInstance->special = SPECIALTYPE_PHONEBOX;
	collisionInstance->special = SPECIALTYPE_PHONEBOX;
	actorInstance->phoneBoxInfo->collisionInstance = collisionInstance;
	collisionInstance->phoneBoxInfo->actorInstance = actorInstance;
	actorInstance->phoneBoxInfo->effectInfo = NULL;
	collisionInstance->phoneBoxInfo->effectInfo = NULL;

	actorInstance->phoneBoxInfo->type = type;
	
	if (type==BOXTYPE_PHONE)
	{
		actorInstance->phoneBoxInfo->box = CreateActorInstance(&map.animatedInstances,"phonehiderbox.obe","phonehidebox");
	}
	else
	{
		actorInstance->phoneBoxInfo->box = CreateActorInstance(&map.animatedInstances,"extras\\policehiderbox.obe","policehidebox");
		if (level) strcpy(actorInstance->phoneBoxInfo->level,level);
	}
	if (actorInstance->phoneBoxInfo->box)
	{
		// PP: on some platforms, phone boxes are outlined
		if(MORE_CARTOON_RENDERING)
		{
			EnableActorLineRenderMode(&actorInstance->actorInstance);
			EnableActorLineRenderMode(&actorInstance->phoneBoxInfo->box->actorInstance);
		}

		bmVectorCopy(actorInstance->phoneBoxInfo->box->actorInstance.position,actorInstance->actorInstance.position);
		bmVectorCopy(actorInstance->phoneBoxInfo->box->actorInstance.scale,actorInstance->actorInstance.scale);
		bmQuatCopy(actorInstance->phoneBoxInfo->box->actorInstance.orientation,actorInstance->actorInstance.orientation);
		AddActorToCollisionCache(&collisionCache,actorInstance->phoneBoxInfo->box,0);
		PlayAnimationByName(actorInstance->phoneBoxInfo->box,"static",1.0f,0,0,0.0f,NONE);
	}
	collisionInstance->phoneBoxInfo->box = NULL;

	PlayAnimationByName(actorInstance,"underground",1.0f,1,0,0.0f,NONE);

	actorInstance->phoneBoxInfo->state = PHONESTATE_INACTIVE;
	actorInstance->phoneBoxInfo->time = 0.0f;
	actorInstance->flags |= ACTORFLAG_DONTDRAW;

	actorInstance->phoneBoxInfo->actorInstance = NULL;
	collisionInstance->phoneBoxInfo->collisionInstance = NULL;

	SetCollisionCallback(collisionInstance,RespondToPhoneBoxCollision);

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: PhoneBoxCamera
	Purpose		: Called once per frame to hold the camera in the correct position
	Parameters	: actorInstance of phone box
	Returns		: 
	Info		: 
*/
void PhoneBoxCamera(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;

	SETVECTOR(tempVector,METERTOUNIT(-0.5f),METERTOUNIT(1.5f),METERTOUNIT(-4.0f),1.0f);
	bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(tempVector,actorInstance->actorInstance.position,tempVector);
	SetCameraPositionDestination(&camera[0], tempVector);
	SetCameraPosition(&camera[0], tempVector);
}

/* --------------------------------------------------------------------------------
   Function : ActivatePhoneBoxes
   Purpose : Called when the player has collected 10 taz tops in quick succession
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
void ActivatePhoneBoxes(ACTORINSTANCE *actorInstance)
{
}

/*  --------------------------------------------------------------------------------
	Function	: EnterSceneUsingPoliceBox
	Purpose		: If player enters the scene from one of the bonus games
	Parameters	: actorInstance, lastScene
	Returns		: 
	Info		: 
*/
bool EnterSceneUsingPoliceBox(ACTORINSTANCE *actorInstance, int lastScene)
{
	ACTORINSTANCE *ptr,*phoneBox=NULL;
	TBVector	tempVector;
	TBMatrix	tempMatrix;
	float		tempFloat,ang;

	// TP: find correct police box

	for (ptr = map.phoneBoxInstances.head.next;ptr != &map.phoneBoxInstances.head;ptr = ptr->next)
	{
		if (ptr->phoneBoxInfo->type == BOXTYPE_POLICE)
		{
			if (strcmp(ptr->phoneBoxInfo->level,sceneFileName[lastScene])==0)
			{
				phoneBox = ptr;
				break;
			}
		}
	}

	if (!phoneBox) return false;

	// TP: position taz
	// TP: play anim on taz
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-1.066f),1.0f);
	bmQuatToMatrix(tempMatrix,phoneBox->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(gameStatus.player1->actorInstance.position,phoneBox->actorInstance.position,tempVector);

	// TP: face phone box
	bmVectorSub(tempVector,phoneBox->actorInstance.position,gameStatus.player1->actorInstance.position);
	tempFloat = GetAngleAroundYFromVector(tempVector);
	CharacterRotateToAngle(gameStatus.player1,0,-tempFloat,0.0f,0.0f);
	gameStatus.player1->flags |= ACTORFLAG_DONTDRAW;

	// TP: setup police box state
	// TP: play anim on police box
	PlayAnimationByName(phoneBox,"arrive2",1.0f,0,0,0.0f,NONE);
	phoneBox->phoneBoxInfo->occupier = actorInstance;
	phoneBox->flags &= ~ACTORFLAG_DONTDRAW;
	gameStatus.player1->flags |= ACTORFLAG_DONTDRAW;
	PlayAnimationByName(phoneBox->phoneBoxInfo->box,"arrive1",1.0f,0,0,0.0f,NONE);

	// TP: Move or snap camera into correct position
	SETVECTOR(tempVector,METERTOUNIT(-0.5f),METERTOUNIT(1.5f),METERTOUNIT(-4.0f),1.0f);
	bmQuatToMatrix(tempMatrix,phoneBox->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(tempVector,phoneBox->actorInstance.position,tempVector);
	SetCameraPositionDestination(&camera[0], tempVector);
	SetCameraPosition(&camera[0], tempVector);

	// TP: Set shadow to follow taz root node
	SetShadowNode(actorInstance,actorInstance->actorInstance.rootNodeInstance->children,NULL);

	// TP: set correct state
	phoneBox->phoneBoxInfo->state = PHONESTATE_ENTERUSINGPOLICEBOX;

	return true;
}

/* --------------------------------------------------------------------------------
   Function : RespondToPhoneBoxCollision
   Purpose : Called when the player touches a phone box
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
int RespondToPhoneBoxCollision(ACTORINSTANCE *collider, ACTORINSTANCE *phoneCollBox)
{
	TBVector	tempVector;
	TBMatrix	tempMatrix;
	float		tempFloat,ang;
	ACTORINSTANCE *phoneBox = phoneCollBox->phoneBoxInfo->actorInstance;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,phoneCollBox);
		return FALSE;
	}
	if (collider->flags&ACTORFLAG_DEBRISACTOR) return FALSE;

	// PP: if collider has no character info, we shouldn't go any further
	if(collider->characterInfo == NULL)
	{
		return FALSE;
	}

	// CMD: no power ups permitted past this point
	if(collider->characterInfo)
	{
		if(collider->characterInfo->powerUpInfo.bubbleGum)
		{
			FinishBubbleGum(collider);
		}
		if(collider->characterInfo->powerUpInfo.chilliPepper)
		{
			FinishChilliPepper(collider);
		}
		if(collider->characterInfo->powerUpInfo.hiccup)
		{
			FinishHiccup(collider);
		}
		if(collider->characterInfo->powerUpInfo.invisible)
		{
			FinishInvisibility(collider);
		}
		if(collider->characterInfo->currentBurp)
		{
			FinishBurp(collider);
		}
	}

	// TP: Best not allow enter unless in MOVE state
	if (collider->actorStatus->currentState != STATE_MOVE) return FALSE;

	if ((collider->characterInfo->costume != -1)&&(phoneBox->phoneBoxInfo->type==BOXTYPE_PHONE)) return FALSE;

	// TP: Check phonebox is open
	if (phoneBox->phoneBoxInfo->state != PHONESTATE_ACTIVE) return FALSE;

	// TP: Check taz is in front of the phonebox
	bmVectorSub(tempVector,collider->actorInstance.position,phoneBox->actorInstance.position);
	tempFloat = Aabs(GetAngleAroundYFromVector(tempVector) + PI);
	ang = GetAngleAroundYFromQuat(phoneBox->actorInstance.orientation);
	tempFloat = FindShortestAngleSigned(tempFloat,ang);
	if (bmFAbs(tempFloat) > PI/4) return FALSE;

	// TP: Move or snap Taz into correct position
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-1.066f),1.0f);
	bmQuatToMatrix(tempMatrix,phoneBox->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(collider->actorInstance.position,phoneBox->actorInstance.position,tempVector);

	// TP: face phone box
	bmVectorSub(tempVector,phoneBox->actorInstance.position,collider->actorInstance.position);
	tempFloat = GetAngleAroundYFromVector(tempVector);
	CharacterRotateToAngle(collider,0,-tempFloat,0.0f,0.0f);

	// TP: Move or snap camera into correct position
	SETVECTOR(tempVector,0.0f,0.0f,METERTOUNIT(-4.0f),1.0f);
	bmQuatToMatrix(tempMatrix,phoneBox->actorInstance.orientation);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorAdd(tempVector,phoneBox->actorInstance.position,tempVector);
	SetCameraPosition(&camera[0], tempVector);
	RotateCameraAroundLookAt(&camera[0], RAD(30.0f), RAD(30.0f));
	
	// TP: Queue correct animations for Taz and phonebox with collision
	phoneBox->phoneBoxInfo->state = PHONESTATE_TAZENTER;
	FlushAnimationQueue(collider);
	PlayAnimationByName(phoneBox,"tazenter",1.0f,0,0,0.0f,NONE);
	PlayAnimationByName(collider,"enterphonebox",1.0f,0,0,0.0f,DEATH);

	SetShadowNode(collider,collider->actorInstance.rootNodeInstance->children,NULL);

	phoneBox->phoneBoxInfo->occupier = collider;

	// TP: Disable player control
	RequestStateChange(collider,STATE_ENTERINGPHONEBOX);

	return FALSE;
}

static			int playpen=0;
/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBox
   Purpose : Update phone box
   Parameters : ACTORINSTANCE *actorInstance
   Returns : nothing
   Info : 
*/
void UpdatePhoneBox(ACTORINSTANCE *actorInstance)
{
	PHONEBOXINFO	*phone = actorInstance->phoneBoxInfo;
	char			*name = NULL;
	int				scene = map.sceneNumber, i;
	ACTORINSTANCE	*pActorInstance;
	TBVector		tempVector;
	char			message[33];

	if (actorInstance->phoneBoxInfo->actorInstance) return;

	switch(scene)
	{
	case SCENE_GRANDCANYON:
		name = "costumecowboy";
		break;
	case SCENE_MUSEUM:
		name = "costumeninja";
		break;
	case SCENE_GHOSTTOWN:
		name = "costumewerewolf";
		break;
	case SCENE_GOLDMINE:
		name = "costumeindianna";
		break;
	case SCENE_CONSTRUCT:
		name = "costumeswat";
		break;
	case SCENE_DEPTSTR:
		name = "costumerappa";
		break;
	case SCENE_ICEDOME:
		name = "costumesnowboard";
		break;
	case SCENE_AQUA:
		name = "costumesurfer";
		break;
	case SCENE_ZOOHUB:
		name = "costumereindeer";
		break;
	case SCENE_SAFARI:
		name = "costumeskater";
		break;
	case SCENE_TAZHUB:
		name = "costumetarzan";
		break;
	case SCENE_WILDWESTHUB:
	case SCENE_CITYHUB:
		name = "costumesurfer";
		break;
	}

	if (phone->type==BOXTYPE_POLICE) name = "exitphonebox";

	if (phone->state!=PHONESTATE_INACTIVE)
	{
		AnimateActorInstance(actorInstance);
	}

	switch(phone->state)
	{
	case PHONESTATE_INACTIVE:
		// TP: if player is near and not wearing a costume then activate phonebox
		for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
		{
			pActorInstance = gameStatus.player[i].actorInstance;
			if ((pActorInstance)&&((pActorInstance->characterInfo->costume==COSTUME_NONE)||(phone->type==BOXTYPE_POLICE)))
			{
				#define BOX_ACTIVATERANGE	(METERTOUNIT(10.0f)*METERTOUNIT(10.0f))

				bmVectorSub(tempVector, pActorInstance->actorInstance.position, actorInstance->actorInstance.position);
				if (bmVectorSquaredLen(tempVector) < BOX_ACTIVATERANGE)
				{
					phone->state = PHONESTATE_STARTEFFECT;
					
					StartPoliceBoxGlowEffect(actorInstance);					
				}
			}
		}
		break;
	case PHONESTATE_STARTEFFECT:
		if((phone->time -= fTime) < 0.0f)
		{
			// TP: Activate phone box
			actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			AddActorToCollisionCache(&collisionCache,actorInstance->phoneBoxInfo->collisionInstance,0);
			PlayAnimationByName(actorInstance,"arrive1",1.0f,0,0,0.0f,NONE);
			if (actorInstance->phoneBoxInfo->box)
			{
				PlayAnimationByName(actorInstance->phoneBoxInfo->box,"arrive1",1.0f,0,0,0.0f,NONE);
				PlayTrackingSample("phoneboxappear.wav", 255, actorInstance->phoneBoxInfo->box->actorInstance.position);
			}
			phone->time = 0.0f;
			phone->state = PHONESTATE_ACTIVATING;
		}
		break;
	case PHONESTATE_ACTIVATING:
		if (!CheckIfPlayingAnimation(actorInstance,"arrive1"))
		{
			// TP: Finished playing activate anim
			PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.0f,NONE);
			PhoneBoxLoopChannel = PlayTrackingSample("phoneboxloop.wav", 255, actorInstance->phoneBoxInfo->box->actorInstance.position);
			PhoneBoxRingChannel = PlayTrackingSample("phonering.wav", 255, actorInstance->phoneBoxInfo->box->actorInstance.position);
			phone->state = PHONESTATE_ACTIVE;
		}
		break;
	case PHONESTATE_ACTIVE:
		// TP: if player is near and not wearing a costume then activate phonebox
		for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
		{
			pActorInstance = gameStatus.player[i].actorInstance;
			if (pActorInstance)
			{
				bmVectorSub(tempVector, pActorInstance->actorInstance.position, actorInstance->actorInstance.position);
				if (bmVectorSquaredLen(tempVector) < BOX_ACTIVATERANGE)
				{
					break;
				}
			}
		}

		if (i==gameStatus.multiplayer.numPlayers)
		{
			// TP: no player in range of phone box so shut down
			//if (bmRand()%2 == 0)
			{
				if (actorInstance->phoneBoxInfo->type==BOXTYPE_POLICE)
				{
					PlayAnimationByName(actorInstance,"disappear2",1.0f,0,0,0.0f,NONE);
				}
				else
				{
					PlayAnimationByName(actorInstance,"disappear1",1.0f,0,0,0.0f,NONE);
				}
				if (actorInstance->phoneBoxInfo->box)
				{
					if (actorInstance->phoneBoxInfo->type==BOXTYPE_POLICE)
					{
						PlayAnimationByName(actorInstance->phoneBoxInfo->box,"disappear2",1.0f,0,0,0.0f,NONE);
					}
					else
					{
						PlayAnimationByName(actorInstance->phoneBoxInfo->box,"disappear1",1.0f,0,0,0.0f,NONE);
					}

					StopChannel(&PhoneBoxLoopChannel);
					StopChannel(&PhoneBoxRingChannel);
					
					PlayTrackingSample("phoneboxshrink.wav", 255, actorInstance->phoneBoxInfo->box->actorInstance.position);
				}
			}
			PlayAnimationByName(actorInstance,"underground",1.0f,1,1,0.0f,NONE);
			RemoveActorFromCollisionCache(&collisionCache,actorInstance->phoneBoxInfo->collisionInstance);
			phone->state = PHONESTATE_WAITFOREXIT;
		}
		
		break;
	case PHONESTATE_WAITFOREXIT:

		for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
		{
			pActorInstance = gameStatus.player[i].actorInstance;
			if (pActorInstance)
			{
				bmVectorSub(tempVector, pActorInstance->actorInstance.position, actorInstance->actorInstance.position);
				if (bmVectorSquaredLen(tempVector) < BOX_ACTIVATERANGE)
				{
					// TP: player has run back into area during shutdown
					SetToEndFrame(actorInstance, "disappear");
					if (actorInstance->phoneBoxInfo->box) SetToEndFrame(actorInstance->phoneBoxInfo->box, "disappear1");

					EndPoliceBoxGlowEffect();

					phone->state = PHONESTATE_INACTIVE;
					actorInstance->flags |= ACTORFLAG_DONTDRAW;
					RefreshCollisionEntry(&collisionCache,actorInstance->phoneBoxInfo->box);
					break;
				}
			}
		}

		if ((!CheckIfPlayingAnimation(actorInstance, "disappear"))&&(!CheckIfPlayingAnimation(actorInstance, "disappear1"))&&(!CheckIfPlayingAnimation(actorInstance, "disappear2")))
		{
			if ((!actorInstance->phoneBoxInfo->box)||((!CheckIfPlayingAnimation(actorInstance->phoneBoxInfo->box, "disappear1"))&&(!CheckIfPlayingAnimation(actorInstance->phoneBoxInfo->box, "disappear2"))))
			{
				EndPoliceBoxGlowEffect();

				phone->state = PHONESTATE_INACTIVE;
				actorInstance->flags |= ACTORFLAG_DONTDRAW;
				RefreshCollisionEntry(&collisionCache,actorInstance->phoneBoxInfo->box);
			}
		}
		break;
	case PHONESTATE_TAZENTER:
		PhoneBoxCamera(actorInstance);

		if (!CheckIfPlayingAnimation(actorInstance,"tazenter"))
		{
			// TP: close all other phone boxes
			phoneBoxesActiveTimer = 0.01f;
			StopChannel(&ringSoundHandle);

			phone->occupier->flags |= ACTORFLAG_DONTDRAW;

			if (phone->type == BOXTYPE_PHONE)
			{
				if (bmRand()%2 == 0)
				{
					PlayAnimationByName(actorInstance,"changeclothes1",1.0f,0,0,0.0f,NONE);
				}
				else
				{
					PlayAnimationByName(actorInstance,"changeclothes2",1.0f,0,0,0.0f,NONE);
				}
			}
			else
			{
				PlayAnimationByName(actorInstance,"disappear",1.0f,0,0,0.0f,NONE);
			}

			StopChannel(&PhoneBoxRingChannel);

			PlayTrackingSample("phonepickup.wav", 255, actorInstance->phoneBoxInfo->box->actorInstance.position);

			phone->state = PHONESTATE_CHANGING;

			// PP: if we're wearing the watch, take it off now...
			if(CheatActive(CHEAT_DIGITALWATCH))
			{
				// PP: detach the watch model
				DestroyAttachedInfo(gameStatus.player[0].actorInstance);

				// PP: deactivate the watch display
				MonitorFX_watch.shutdown();
			}
		}
		break;

	case PHONESTATE_CHANGING:
		PhoneBoxCamera(actorInstance);

		if (CheckIfPlayingAnimation(actorInstance,NULL)==FALSE)
		{
			int	rand = bmRand() % 2;

			if (phone->type == BOXTYPE_POLICE)
			{
				sprintf(message, "_%s", phone->level);
				bkGenerateEvent("scenechange",message);
				phone->state = PHONESTATE_CHANGING;
				break;
			}

			if (phone->type == BOXTYPE_POLICE)
			{
				PlayAnimationByName(actorInstance,"tazexit",1.0f,0,0,0.0f,NONE);
				PlayAnimationByName(actorInstance,"idle2",1.0f,0,1,0.0f,NONE);
				PlayAnimationByName(gameStatus.player1,"costumesnowboard",1.0f,0,0,0.0f,NONE);
			}
			else
			{
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(actorInstance,"tazexit1",1.0f,0,0,0.0f,NONE);
					break;
				case 1:
					PlayAnimationByName(actorInstance,"tazexit2",1.0f,0,0,0.0f,NONE);
					break;
				case 2:
					PlayAnimationByName(actorInstance,"tazexit3",1.0f,0,0,0.0f,NONE);
					break;
				}
				// PP: Change Taz into the appropriate attire for the current level
				SetupLevelCostume(phone->occupier);

				PlayAnimationByName(phone->occupier,name,1.0f,0,0,0.0f,NONE);
			}

			phone->occupier->flags &= ~ACTORFLAG_DONTDRAW;

			phone->state = PHONESTATE_TAZEXIT;

			if(characterSounds.taz == -1)
			{
				switch(map.sceneNumber)
				{
				case SCENE_ZOOHUB:	CallbackWhenSampleFinished(characterSounds.taz=PlaySample("reindeerbells.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_SAFARI:	if (rand)
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazskate_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
									else
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazskate_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_AQUA:	if (rand)
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazsurf_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
									else
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazsurf_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_ICEDOME:	if (rand)
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazsnowboard_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
									else
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazsnowboard_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_DEPTSTR:	PlaySample("hiphoptaz.wav", 255);
					
									if (rand)
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazrap_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
									else
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazrap_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_MUSEUM:	if (rand)
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazninja_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
									else
										CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazninja_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

									break;

				case SCENE_CONSTRUCT:	if (rand)
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazswat_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										else
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazswat_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

										break;

				case SCENE_GRANDCANYON:	if (rand)
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazbandit_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										else
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazbandit_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

										break;

				case SCENE_GOLDMINE:	if (rand)
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazindy_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										else
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazindy_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										break;

				case SCENE_GHOSTTOWN:	if (rand)
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazwerewolf_1_alt 1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										else
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("tazwerewolf_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

										break;

				case SCENE_TAZHUB:		if (rand)
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("taztarzan_1.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);
										else
											CallbackWhenSampleFinished(characterSounds.taz=PlaySample("taztarzan_2.wav", 255, actorInstance->actorInstance.position),CharacterSoundsFinishedCallback,&characterSounds.taz);

										break;
				}
			}
		}
		break;

	case PHONESTATE_ENTERUSINGPOLICEBOX:
		PhoneBoxCamera(actorInstance);

		if (CheckIfPlayingAnimation(actorInstance,NULL)==FALSE)
		{
			PlayAnimationByName(actorInstance,"tazexit",1.0f,0,0,0.0f,NONE);
			PlayAnimationByName(actorInstance,"idle2",1.0f,0,1,0.0f,NONE);
			PlayAnimationByName(gameStatus.player1,name,1.0f,0,0,0.0f,NONE);
			SetToStartFrame(gameStatus.player1,name);

			phone->occupier->flags &= ~ACTORFLAG_DONTDRAW;
			phone->state = PHONESTATE_TAZEXIT;
		}
		break;

	case PHONESTATE_TAZEXIT:
		PhoneBoxCamera(actorInstance);
		if (CheckIfPlayingAnimation(actorInstance,NULL)==FALSE)
		{
			if ((name==NULL)||(CheckIfPlayingAnimation(phone->occupier,name)==FALSE))
			{
				TBVector		tempVector;
				TBQuaternion	tempQuat;
				float			angle;

				phoneBoxesActiveTimer = 0.0f;
				SetShadowNode(phone->occupier,NULL,NULL);
				RequestStateChange(phone->occupier,STATE_MOVE);

				// TP: Must rotate character 180 degrees
				angle = GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);
				tempVector[X] = 0;
				tempVector[Y] = 1;
				tempVector[Z] = 0;
				tempVector[A] = angle;
				bmRotationToQuat(tempQuat,tempVector);

				ActorRotate( phone->occupier, tempQuat,  0.0f);
				PlayAnimationByName(phone->occupier, "idle1",1.0f,1,0,0.0f,NONE);
				baUpdateNodes(&phone->occupier->actorInstance,phone->occupier->actorInstance.rootNodeInstance,1,NULL);
				baPrepActorInstance(&phone->occupier->actorInstance,TRUE);

				PlayAnimationByName(actorInstance,"idle1",1.0f,1,0,0.0f,NONE);

				phone->state = PHONESTATE_ACTIVE;

				RequestMorronicMessage(MEVENT_COSTUME);
			}
		}
		break;
	}

	UpdatePhoneBoxBook();
}


/* --------------------------------------------------------------------------------
   Function : UpdatePhoneBoxes
   Purpose : Update all phone boxes on list
   Parameters : ACTORINSTANCELIST *actorInstanceList
   Returns : nothing
   Info : 
*/
void UpdatePhoneBoxes(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE	*ptr;

	ptr = actorInstanceList->head.next;

	if (phoneBoxesActiveTimer > 0.0f) 
	{
		phoneBoxesActiveTimer -= fTime;
		if (phoneBoxesActiveTimer <= 0.0f)
		{
			phoneBoxesActiveTimer = 0.0f;
			if (ringSoundHandle!=-1)
			{
				StopChannel(&ringSoundHandle);
				PlaySample("obj_gen_03a.wav",255,0,0,NULL,0);
			}
		}
	}
}

void CreatePhoneBoxBook()
{
	if (map.sceneNumber == SCENE_ZOOHUB || 
		map.sceneNumber == SCENE_CITYHUB || 
		map.sceneNumber == SCENE_WILDWESTHUB)
	{
		PAGE	*page;

		TEXTBOX	*textbox;

		phoneboxBook = new BOOK();

		phoneboxBook->setBookRect(RECTANGLE(-0.3f, 0.3f, -0.3f, 0.3f));

		page = phoneboxBook->addPage("Police Box Message");

		page->insertItem(textbox = new TEXTBOX(STR_POLICEBOX_NOT_OPEN));
		textbox->setWrap(true);
	}
}

void UpdatePhoneBoxBook()
{
	if (phoneboxBook)
	{ 
		if (gameStatus.gameTime > phoneboxBookTimer && phoneboxBookTimer)
		{
			phoneboxBook->close();
			phoneboxBookTimer = 0.0f;
		}

// PP: REMOUT: Now handled by the Book List					phoneboxBook->update();
	}
}
/* PP: REMOUT: Now handled by the Book List			
void DrawPhoneBoxBook()
{
	if (phoneboxBook)
	{
		bdSetIdentityObjectMatrix();
	
		SAVESTATES;
		SETPROJECTION(2D);
		SETZTEST(ALWAYS);

		phoneboxBook->draw();

		SETPROJECTION(3D);
		RESTORESTATES;
	}
}
*/
void KillPhoneBoxBook()
{
	// TP: this should probably be a delete
	SAFE_DELETE(phoneboxBook);
	//BookList_closeAndFree(phoneboxBook);

	phoneboxBook = 0;
}