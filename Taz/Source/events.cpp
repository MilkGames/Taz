// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : events.cpp
//   Purpose : functions to create and use events
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"

#include "scene.h"
#include "trigger.h"
#include "electrocution.h"
#include "fallingobjects.h"
#include "sparks.h"
#include "lightning.h"
#include "health.h"
#include "main.h"
#include "characters.h"
#include "control.h"

#include "fade.h"
#include "mapfile.h"
#include "sfx.h" 
#include "cameracase.h"

#include "paths.h"
#include "animation.h"
#include "shatter.h"
#include "invisibletaz.h"

#include "decals.h"
#include "maths.h"
#include "status.h"

#include "ZooBoss.h"				// PP: included so we can respond to Zoo Boss messages
#include "WestBoss.h"				// PP: included so we can respond to West Boss messages
#include "TazBoss1.h"				// SO: included so we can respond to Taz Boss messages
#include "playerstats.h"
#include "explode.h"

#include "scenicobjects.h"
#include "textures.h"
#include "destructibles.h"
#include "effects.h"
#include "projectile.h"
#include "frontendmenu.h"
#include "transporter.h"
#include "subtitle.h"

#include "cityboss.h"
#include "pause.h"
#include "snowball.h"
#include "debris.h"
#include "cheesegun.h"
#include "superburp.h"

#include "mechatweety.h"
#include "redshield.h"
#include "multiplayer.h"
#include "memcard.h"
#include "springyobjects.h"
#include "minecart.h"
#include "genericai.h"

#include "speedboost.h"
#include "fountain.h"
#include "musicandsavagebeasts.h"
#include "collectibles.h"
#include "floorpolisherwantedposter.h"
#include "special.h"
#include "rumble.h"
#include "rapperblast.h"
#include "holysmoke.h"
#include "thwap.h"
#include "booklist.h"

#include "destruct.h"
#include "escalator.h"
#include "watersplash.h"
#include "arrow.h"
#include "prompt.h"
#include "cutscene.h"
#include "tarbubble.h"

#define MAX_SOUND_EVENTS	(40)
#define MAX_TRIGGER_EVENTS	(100)
#define MAX_CAMERA_EVENTS	(10)
#define MAX_PLATFORM_EVENTS	(30)
#define MAX_ANIMATION_EVENTS (30)

TBEventClient	*soundEventClient;
TBEventClient	*sceneChangeEventClient;
TBEventClient	*triggerEventClient;
TBEventClient	*cameraEventClient;
TBEventClient	*platformEventClient;

TBEventClient	*animationEndLoopEventClient;
TBEventClient	*animationEventClient;
TBEventClient	*animationlEventClient;
TBEventClient	*animationqEventClient;
TBEventClient	*animationlqEventClient;
TBEventClient	*animationcEventClient;
TBEventClient	*animationclEventClient;
TBEventClient	*animationcqEventClient;
TBEventClient	*animationclqEventClient;
TBEventClient	*animationsetEndEventClient;

TBEventClient	*footstepsEventClient;
TBEventClient	*specialEventClient;
TBEventClient	*zooBossFootstepEventClient;		// PP: Zoo boss elephant footsteps // PP: TODO: change this
TBEventClient	*zooBossEventClient;				// PP: added for the Zoo boss game
TBEventClient	*westBossTNTeventClient;			// PP: West boss messages to do with Sam's interaction with the sticks of TNT that he throws
TBEventClient	*destroycollectEventClient;			// CPW: added for destruct/collect mode
TBEventClient	*minecartEventClient;				// CMD: messages from minecart
TBEventClient	*rumbleEventClient;					// CMD: messages requesting rumble effects
TBEventClient	*promptEventClient;					// CMD: messages requesting on screen prompts

static POPUP	*levelChangeConfirm = NULL;
static POPUP	*finishChangeConfirm = NULL;
int32			showHubStats = FALSE;				//THIS IS ONE HELL OF A CRAPPY BODGE
PLAYERSTATS		levelSelectPlayerStats;

//#define LEVELCHANGETEST
//list of strings from string table
int32			levelNameStrings[] = 
{
	-1,
	STR_LEVELNAME_FRONTEND,
	-1,
	STR_LEVELNAME_ZOOWORLD,
	STR_LEVELNAME_ICEDOME,
	STR_LEVELNAME_SAFARI,
	STR_LEVELNAME_AQUA,
	STR_LEVELNAME_ZOOBOSS,
	STR_LEVELNAME_CITYWORLD,
	STR_LEVELNAME_DEPTSTR,
	STR_LEVELNAME_MUSEUM,
	STR_LEVELNAME_CONSTRUCT,
	STR_LEVELNAME_CITYBOSS,
	STR_LEVELNAME_WESTWORLD,
	STR_LEVELNAME_GHOSTTOWN,
	STR_LEVELNAME_GOLDMINE,
	STR_LEVELNAME_GRANDC,
	STR_LEVELNAME_WESTBOSS,
	STR_LEVELNAME_TASMANIA,
	STR_LEVELNAME_TAZBOSS1,
	STR_LEVELNAME_TAZBOSS2,
	STR_LEVELNAME_MUSEUM,
	STR_LEVELNAME_GRANDC,
	STR_LEVELNAME_DEPTSTR,
	STR_LEVELNAME_AQUA,
	STR_LEVELNAME_CONSTRUCT,
	STR_LEVELNAME_GHOSTTOWN,
	STR_LEVELNAME_ICEDOME,
	STR_LEVELNAME_GOLDMINE,
	STR_LEVELNAME_SAFARI,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
};

/*	--------------------------------------------------------------------------------
	Function 	: CreateEventManagers
	Purpose 	: Setup event managers
	Parameters 	: 
	Returns 	: TRUE/FALSE
	Info 		:
*/
int CreateEventManagers(void)
{
	//levelChangeConfirm = new POPUP(POPUP_CONFIRM,0,STR_EXIT_TOHUB);
	// TP: these seem to be open to start with, causes problems with the navigatible flag being set stopping the debug controls working
	//levelChangeConfirm->close(0.0f, FALSE);
	//levelChangeConfirm->Update();
	//finishChangeConfirm = new POPUP(POPUP_CONFIRM,0,STR_PRESS_Y_TO_FINISH_LEVEL);
	//finishChangeConfirm->close(0.0f, FALSE);
	//finishChangeConfirm->Update();
	//finishChangeConfirm->setBookRect(RECTANGLE(-0.3f,0.3f,-0.2f,0.2f));

	if(bkCreateEvent("sound") == FAIL)	return FALSE;
	if(bkCreateEvent("trigger") == FAIL) return FALSE;
	if(bkCreateEvent("camera") == FAIL) return FALSE;
	if(bkCreateEvent("scenechange") == FAIL) return FALSE;
	if(bkCreateEvent("platform") == FAIL) return FALSE;

	if(bkCreateEvent("animationloopend") == FAIL) return FALSE;
	if(bkCreateEvent("animation") == FAIL) return FALSE;
	if(bkCreateEvent("animationl") == FAIL) return FALSE;
	if(bkCreateEvent("animationq") == FAIL) return FALSE;
	if(bkCreateEvent("animationlq") == FAIL) return FALSE;
	if(bkCreateEvent("animationc") == FAIL) return FALSE;
	if(bkCreateEvent("animationcl") == FAIL) return FALSE;
	if(bkCreateEvent("animationcq") == FAIL) return FALSE;
	if(bkCreateEvent("animationclq") == FAIL) return FALSE;
	if(bkCreateEvent("animationsetend") == FAIL) return FALSE;
	
	if(bkCreateEvent("FOOTSTEP") == FAIL) return FALSE;
	if(bkCreateEvent("footstep") == FAIL) return FALSE;				// PP: create an event manager for Zoo boss elephant footstep messages
	if(bkCreateEvent("tnt") == FAIL) return FALSE;					// PP: create an event manager for West boss TNT messages
	if(bkCreateEvent("elephant") == FAIL) return FALSE;				// PP: create an event manager for Zoo boss elephant messages
	if(bkCreateEvent("destroycollect") == FAIL) return FALSE;		// CPW: added for destruct/collect mode
	if(bkCreateEvent("special") == FAIL) return FALSE;
	if(bkCreateEvent("minecart") == FAIL) return FALSE;
	if(bkCreateEvent("rumble") == FAIL) return FALSE;
	if(bkCreateEvent("prompt") == FAIL) return FALSE;

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateEventHandlers
	Purpose 	: Setup event handlers
	Parameters 	: 
	Returns 	: TRUE/FALSE
	Info 		:
*/
int CreateEventHandlers(void)
{
	soundEventClient = bkTrapEventQueue("sound", MAX_SOUND_EVENTS,BEVENTQUEUEFLAG_NEWPRIORITY);
	sceneChangeEventClient = bkTrapEventQueue("scenechange", 3,BEVENTQUEUEFLAG_NEWPRIORITY);
	triggerEventClient = bkTrapEventQueue("trigger", MAX_TRIGGER_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	cameraEventClient = bkTrapEventQueue("camera", MAX_CAMERA_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	platformEventClient = bkTrapEventQueue("platform",MAX_PLATFORM_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);

	animationEndLoopEventClient = bkTrapEventQueue("animationloopend",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationEventClient = bkTrapEventQueue("animation",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationlEventClient = bkTrapEventQueue("animationl",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationqEventClient = bkTrapEventQueue("animationq",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationlqEventClient = bkTrapEventQueue("animationlq",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationcEventClient = bkTrapEventQueue("animationc",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationclEventClient = bkTrapEventQueue("animationcl",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationcqEventClient = bkTrapEventQueue("animationcq",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationclqEventClient = bkTrapEventQueue("animationclq",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);
	animationsetEndEventClient = bkTrapEventQueue("animationsetend",MAX_ANIMATION_EVENTS, BEVENTQUEUEFLAG_NEWPRIORITY);

	footstepsEventClient = bkTrapEventQueue("FOOTSTEP",2, BEVENTQUEUEFLAG_NEWPRIORITY);
	specialEventClient = bkTrapEventQueue("special",2, BEVENTQUEUEFLAG_NEWPRIORITY);
	zooBossFootstepEventClient = bkTrapEventQueue("footstep",2, BEVENTQUEUEFLAG_NEWPRIORITY);
	zooBossEventClient = bkTrapEventQueue("elephant", 1, BEVENTQUEUEFLAG_NEWPRIORITY);							// PP: added for the Zoo boss game
	westBossTNTeventClient = bkTrapEventQueue("tnt", 1, BEVENTQUEUEFLAG_NEWPRIORITY);							// PP: added for the West boss game
	destroycollectEventClient = bkTrapEventQueue("destroycollect", 1, 0);										// CPW: added for destruct/collect mode
	minecartEventClient = bkTrapEventQueue("minecart", 5, BEVENTQUEUEFLAG_NEWPRIORITY);
	rumbleEventClient = bkTrapEventQueue("rumble", 5, BEVENTQUEUEFLAG_NEWPRIORITY);
	promptEventClient = bkTrapEventQueue("prompt", 2, BEVENTQUEUEFLAG_NEWPRIORITY);

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: DeleteEventHandlers
	Purpose 	: Delete event handlers
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeleteEventHandlers(void)
{
	bkDeleteEventClient(soundEventClient);
	bkDeleteEventClient(sceneChangeEventClient);
	bkDeleteEventClient(triggerEventClient);
	bkDeleteEventClient(cameraEventClient);
	bkDeleteEventClient(platformEventClient);

	bkDeleteEventClient(animationEndLoopEventClient);
	bkDeleteEventClient(animationEventClient);
	bkDeleteEventClient(animationlEventClient);
	bkDeleteEventClient(animationqEventClient);
	bkDeleteEventClient(animationlqEventClient);
	bkDeleteEventClient(animationcEventClient);
	bkDeleteEventClient(animationclEventClient);
	bkDeleteEventClient(animationcqEventClient);
	bkDeleteEventClient(animationclqEventClient);
	
	bkDeleteEventClient(footstepsEventClient);
	bkDeleteEventClient(specialEventClient);
	bkDeleteEventClient(zooBossFootstepEventClient);
	bkDeleteEventClient(zooBossEventClient);					// PP: added for the Zoo boss game
	bkDeleteEventClient(westBossTNTeventClient);				// PP: added for the West boss game
	bkDeleteEventClient(destroycollectEventClient);				// CPW: added for destruct/collect mode
	bkDeleteEventClient(minecartEventClient);
	bkDeleteEventClient(rumbleEventClient);
	bkDeleteEventClient(promptEventClient);
}

/*	--------------------------------------------------------------------------------
	Function 	: DeleteEventManagers
	Purpose 	: Delete event managers
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void DeleteEventManagers(void)
{
	// TP: Left these in just for safety
	if (levelChangeConfirm) BookList_closeAndFree(levelChangeConfirm);
	if (finishChangeConfirm) BookList_closeAndFree(finishChangeConfirm);

	bkDeleteEvent("sound");
	bkDeleteEvent("trigger");
	bkDeleteEvent("camera");
	bkDeleteEvent("scenechange");
	bkDeleteEvent("platform");

	bkDeleteEvent("animationloopend");
	bkDeleteEvent("animation");
	bkDeleteEvent("animationl");
	bkDeleteEvent("animationq");
	bkDeleteEvent("animationlq");
	bkDeleteEvent("animationc");
	bkDeleteEvent("animationcl");
	bkDeleteEvent("animationcq");
	bkDeleteEvent("animationclq");
	bkDeleteEvent("animationsetend");

	bkDeleteEvent("FOOTSTEP");
	bkDeleteEvent("special");
	bkDeleteEvent("footstep");
	bkDeleteEvent("tnt");					// PP: added for the West boss game
	bkDeleteEvent("elephant");				// PP: added for the Zoo boss game
	bkDeleteEvent("destroycollect");		// CPW: added for destruct/collect mode
	bkDeleteEvent("minecart");
	bkDeleteEvent("rumble");
	bkDeleteEvent("prompt");
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateTriggerInfo
	Purpose 	: Malloc trigger info for maxMessages
	Parameters 	: maximum number of messages
	Returns 	: triggerInfo/NULL
	Info 		:
*/
TRIGGERINFO *CreateTriggerInfo(int maxMessages)
{
	TRIGGERINFO *triggerInfo;
	char	*ptr;
	int	i;

	// TP: allocate only one block of memory to store all info, this saves on the malloc memory overhead for small mallocs
	triggerInfo = (TRIGGERINFO*)ZALLOC(sizeof(TRIGGERINFO)+(maxMessages*sizeof(char*))+(maxMessages*sizeof(char*)));
	ptr = (char*)triggerInfo;

	if (!triggerInfo) return NULL;

	// TP: setup events pointer
	triggerInfo->events = (char**)(ptr += sizeof(TRIGGERINFO));

	// TP: setup message pointer
	triggerInfo->messages = (char**)(ptr += (maxMessages*sizeof(char*)));

	// TP: null em
	for (i=0;i<maxMessages;i++) triggerInfo->events[i] = NULL;
	for (i=0;i<maxMessages;i++) triggerInfo->messages[i] = NULL;

	triggerInfo->numMessages = 0;
	triggerInfo->maxMessages = maxMessages;
	triggerInfo->flags = 0;

	return triggerInfo;
}

TRIGGERINFO *CopyTriggerInfo(TRIGGERINFO *trigger)
{
	TRIGGERINFO *copy;
	int i;

	if (!trigger) return NULL;

	copy = CreateTriggerInfo(trigger->maxMessages);

	if (!copy) return NULL;

	for (i=0;i<trigger->numMessages;i++)
	{
		CreateTrigger(copy, trigger->events[i], trigger->messages[i],false);
	}

	copy->flags = trigger->flags;

	return copy;
}

/*	--------------------------------------------------------------------------------
	Function 	: CreateTrigger
	Purpose 	: Create trigger message to trigger info
	Parameters 	: event, message
	Returns 	: 
	Info 		:
*/
int CreateTrigger(TRIGGERINFO *triggerInfo,char *event,char *message,bool appendUnderscore)
{
	char	*type;
	char	*msg;
	char	modified[256];

	type = (char*)ZALLOC(strlen(event)+1+strlen(message)+2);
	ASSERT(type);
	msg = type + strlen(event) + 1;
	strcpy(type,event);

	if (appendUnderscore)
	{
		sprintf(msg, "_%s", message);
	}
	else
	{
		strcpy(msg, message);
	}

	AddTrigger(triggerInfo, type, msg);

	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: AddTrigger
	Purpose 	: Add trigger message to trigger info
	Parameters 	: event, message
	Returns 	: 
	Info 		:
*/
int AddTrigger(TRIGGERINFO *triggerInfo,char *event,char *message)
{
	char	*tempString;

	// TP: set flag if any sound messages
	if (strcmpi(event,"sound")==0)
	{
		triggerInfo->flags |= TRIGGERFLAG_CONTAINSSOUND;
	}

	// TP: set flag if any wanted posters found
	if (tempString = FindChar(message,'='))
	{
		*tempString = NULL;
		if (strcmp(message,"_wantedposter")==0)
		{
			triggerInfo->flags |= TRIGGERFLAG_CONTAINSWANTED;
		}
		*tempString = '=';
	}

	if (triggerInfo->numMessages < triggerInfo->maxMessages)
	{
		triggerInfo->events[triggerInfo->numMessages] = event;
		triggerInfo->messages[triggerInfo->numMessages] = message;
		triggerInfo->numMessages++;
		return TRUE;
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: SendTrigger
	Purpose 	: Send trigger
	Parameters 	: event, message
	Returns 	: true if a sound message was found, else false
	Info 		:
*/
bool SendTrigger(TRIGGERINFO *triggerInfo, TBVector position)
{
	int		i;

	if (position)
	{
		SetSoundPos(position);
	}

	if (!triggerInfo) return false;

	for (i=0;i<triggerInfo->numMessages;i++)
	{
		if ((triggerInfo->events[i])&&(triggerInfo->messages[i]))
		{
			bkGenerateEvent(triggerInfo->events[i],triggerInfo->messages[i]);
			bkPrintf("%s:%s\n",triggerInfo->events[i],triggerInfo->messages[i]);
		}
	}

	if (triggerInfo->flags&TRIGGERFLAG_CONTAINSSOUND)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: DeleteTriggerInfo
	Purpose 	: Delete all trigger info
	Parameters 	: triggerInfo
	Returns 	: 
	Info 		:
*/
void DeleteTriggerInfo(TRIGGERINFO *triggerInfo)
{
	int i;

	if (!triggerInfo) return;

	for (i=0;i<triggerInfo->maxMessages;i++)
	{
		if (triggerInfo->events[i])
		{
			if (triggerInfo->messages[i]) 
			{
				triggerInfo->messages[i] = NULL;
			}
			SAFE_FREE(triggerInfo->events[i]);
		}
	}

	SAFE_FREE(triggerInfo);
}


/*	--------------------------------------------------------------------------------
	Function 	: RespondToFootstepMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToFootstepMessages(void)
{
	char			parmStringBuf[256];
	char			*message;
	ACTORINSTANCE	*actor;
	int				temp;

	while (bkPopEvent(footstepsEventClient, parmStringBuf) == OK)
	{
		message = DecodeMessage(parmStringBuf, (TBActorInstance**)&actor, NULL);

#ifndef CONSUMERDEMO
		// PP: respond to zoo boss elephant footstep messages
		if(map.sceneNumber == SCENE_ZOOBOSS)
		{
			if(ZooBoss_game.respondToMessage(actor, message))
			{
				// PP: continue only if the message was recognised and handled by the Zoo Boss game
				continue;
			}
		}
#endif

		if (!actor->actorShadow) continue;

		if ((map.sceneNumber==SCENE_ICEDOME)&&(actor->actorStatus->timeWithoutIceCollision > 0.3f))
		{
			if(strcmpi("left", message)==0)
			{
				CreateDecal(actor->actorShadow,textureFootPrintLeft,
					GetAngleAroundYFromQuat(actor->actorInstance.orientation),3.0f,90,100,110,30);
			}
			else
			{
				if(strcmpi("right", message)==0)
				{
					CreateDecal(actor->actorShadow,textureFootPrintRight,
						GetAngleAroundYFromQuat(actor->actorInstance.orientation),3.0f,90,100,110,30);
				}
			}
		}
		else
		{
			if ((actor->actorStatus->timeWithoutIceCollision > 0.3f)&&(actor->actorStatus->timeWithoutIceCollision < 2.0f))
			{
				if(strcmpi("left", message)==0)
				{
					CreateDecal(actor->actorShadow,textureFootPrintLeft,
						GetAngleAroundYFromQuat(actor->actorInstance.orientation),3.0f,90,100,110,30);
				}
				else
				{
					if(strcmpi("right", message)==0)
					{
						CreateDecal(actor->actorShadow,textureFootPrintRight,
							GetAngleAroundYFromQuat(actor->actorInstance.orientation),3.0f,90,100,110,30);
					}
				}
			}
		}
	}
}

#ifndef CONSUMERDEMO
/*	--------------------------------------------------------------------------------
	Function 	: RespondToZooBossMessages
	Purpose 	: Respond to Zoo Boss messages generated
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: added for the Zoo boss game
*/
void RespondToZooBossMessages(void)
{
	char			parmStringBuf[256];
	char			*message;
	ACTORINSTANCE	*actor;
	int				temp;

	if (map.sceneNumber==SCENE_ZOOBOSS)			// PP: don't do any of this stuff unless we're on the zoo boss game
	{
		while (bkPopEvent(zooBossEventClient, parmStringBuf) == OK)
		{
			message = DecodeMessage(parmStringBuf, (TBActorInstance**)&actor, NULL);

			ZooBoss_game.respondToMessage(actor, message);
		}

		while (bkPopEvent(zooBossFootstepEventClient, parmStringBuf) == OK)
		{
			message = DecodeMessage(parmStringBuf, (TBActorInstance**)&actor, NULL);

			ZooBoss_game.respondToMessage(actor, message);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: RespondToZooBossMessages
	Purpose 	: Respond to Zoo Boss messages generated
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: added for the Zoo boss game
*/
void RespondToWestBossMessages(void)
{
	char			parmStringBuf[256];
	char			*message;
	ACTORINSTANCE	*actor;
	int				temp;

	if (map.sceneNumber==SCENE_WESTBOSS)			// PP: don't do any of this stuff unless we're on the west boss game
	{
		while (bkPopEvent(westBossTNTeventClient, parmStringBuf) == OK)
		{
			message = DecodeMessage(parmStringBuf, (TBActorInstance**)&actor, NULL);

			WestBoss_game.respondToMessage(actor, message);
		}
	}
}

#endif

/*  --------------------------------------------------------------------------------
	Function	: ExitLevelConfirm
	Purpose		: custom pause for when exiting an ingame level
	Parameters	: context
	Returns		: 
	Info		: 
*/
static void ExitLevelConfirm(const int32 context)
{
	int	retval;

	if (!levelChangeConfirm)
	{
		PauseRumble(&controller1);
		levelChangeConfirm = new POPUP(POPUP_CONFIRM,0,STR_EXIT_TOHUB);

				// TP: quick bodge
		if (map.sceneNumber!=SCENE_TAZHUB)
		{
			CreateStatsAtEndOfLevel(gameStatus.player[0].stats, 1);

			UpdateStatsAtEndOfLevel(gameStatus.player[0].stats);

			gameStatus.player[0].stats->endOfLevelStatsBook->setScale(0.7f);
			gameStatus.player[0].stats->endOfLevelStatsBook->setBookRect(RECTANGLE(-0.6f, 0.6f, -0.4f, 0.75f));

			gameStatus.player[0].stats->endOfLevelStatsBook->open();
		}

		CloseCurrentSubtitle();
	}

	// PP: get the player's response to the popup
	retval=levelChangeConfirm->getResponse();

/* PP: REMOUT: Now handled by the Book List				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);	
	retval = levelChangeConfirm->Update();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	levelChangeConfirm->draw();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
*/

	if (CANCELBUTTON || OKBUTTON)
	{
		gameStatus.player[0].stats->endOfLevelStatsBook->close();

		SAFE_DELETE(gameStatus.player[0].stats->endOfLevelStatsBook);

		switch (retval)
		{
		case 1:
			if (context!=-1)
			{
				camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,context,1.0f);
				SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
// PP: REMOUT: Now handled by the Book List				levelChangeConfirm->Update();
				if (levelChangeConfirm) BookList_closeAndFree(levelChangeConfirm);
			}
			else
			{
				if (IN_HUB)
				{
					// TP: if in hub then you can only exit if the cutscene exists or using openLevels cheat
					if ((map.sceneNumber==SCENE_ZOOHUB)&&(AllowedToEnterLevel(SCENE_ZOOBOSS)))
					{
						LevelCompleted(gameStatus.player1);
					}
					if ((map.sceneNumber==SCENE_CITYHUB)&&(AllowedToEnterLevel(SCENE_CITYBOSS)))
					{
						LevelCompleted(gameStatus.player1);
					}
					if ((map.sceneNumber==SCENE_WILDWESTHUB)&&(AllowedToEnterLevel(SCENE_WESTBOSS)))
					{
						LevelCompleted(gameStatus.player1);
					}
				}
				else
				{
					LevelCompleted(gameStatus.player1);
				}

				gameStatus.player[0].stats->destructState = OVERLAYSTATE_LEAVING;
				gameStatus.player[0].stats->swirlState = OVERLAYSTATE_LEAVING;
				gameStatus.player[0].stats->moneyState = OVERLAYSTATE_LEAVING;
				gameStatus.player[0].stats->posterState = OVERLAYSTATE_LEAVING;
			}

			break;
		
		default:
			SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
			UnPauseRumble(&controller1);
// PP: REMOUT: Now handled by the Book List					levelChangeConfirm->Update();
			if (levelChangeConfirm) BookList_closeAndFree(levelChangeConfirm);

			break;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: ExitLevelConfirm
	Purpose		: custom pause for when exiting an ingame level
	Parameters	: context
	Returns		: 
	Info		: 
*/
static void FinishLevelConfirm(const int32 context)
{
	int	retval;

	if (!finishChangeConfirm)
	{
		PauseRumble(&controller1);
		
		if (map.sceneNumber == SCENE_TAZHUB)
			finishChangeConfirm = new POPUP(POPUP_CONFIRM,1,STR_PRESS_Y_TO_FINISH_LEVEL_SHORT, STR_DIALOGUE_YES, STR_DIALOGUE_NO, STR_X_TO_SELECT_Y_TO_RETURN, 0, true, 0.8f);
		else
			finishChangeConfirm = new POPUP(POPUP_CONFIRM,1,STR_PRESS_Y_TO_FINISH_LEVEL, STR_DIALOGUE_YES, STR_DIALOGUE_NO, STR_X_TO_SELECT_Y_TO_RETURN, 0, true, 0.8f);

		finishChangeConfirm->setBookRect(RECTANGLE(-0.3f,0.3f,-0.2f,0.2f));

		// TP: quick bodge
		//if (map.sceneNumber!=SCENE_TAZHUB)
		//{
			CreateStatsAtEndOfLevel(gameStatus.player[0].stats, 1);

			UpdateStatsAtEndOfLevel(gameStatus.player[0].stats);

			gameStatus.player[0].stats->endOfLevelStatsBook->setScale(0.7f);
			gameStatus.player[0].stats->endOfLevelStatsBook->setBookRect(RECTANGLE(-0.6f, 0.6f, -0.35f, 0.8f));

			gameStatus.player[0].stats->endOfLevelStatsBook->open();

			CloseCurrentSubtitle();
		//}
	}
	
	// PP: get the player's response to the popup
	retval=finishChangeConfirm->getResponse();

/* PP: REMOUT: Now handled by the Book List				retval = finishChangeConfirm->Update();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	finishChangeConfirm->draw();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
*/
	if (finishChangeConfirm->controlDelay<=0.0f)
	{
		finishChangeConfirm->setNavigation(true);
		if (CANCELBUTTON || OKBUTTON)
		{
			gameStatus.player[0].stats->endOfLevelStatsBook->close();

			SAFE_DELETE(gameStatus.player[0].stats->endOfLevelStatsBook);

			switch (retval)
			{
			case 1:
	#ifndef STANDALONE
	#ifdef CONSUMERDEMO
				demoTimerBook->close();
	#endif
	#endif
				BookList_closeAndFree(finishChangeConfirm);
				if (context!=-1)
				{
					camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,context,1.0f);
					SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
					finishChangeConfirm->close(0.0f,FALSE);
	// PP: REMOUT: Now handled by the Book List							finishChangeConfirm->Update();
	// TP: 				BookList_closeAndFree(finishChangeConfirm);		
				}
				else
				{
					if (IN_HUB)
					{
						// TP: if in hub then you can only exit if the cutscene exists or using openLevels cheat
						if ((map.sceneNumber==SCENE_ZOOHUB)&&(AllowedToEnterLevel(SCENE_ZOOBOSS)))
						{
							LevelCompleted(gameStatus.player1);
						}
						if ((map.sceneNumber==SCENE_CITYHUB)&&(AllowedToEnterLevel(SCENE_CITYBOSS)))
						{
							LevelCompleted(gameStatus.player1);
						}
						if ((map.sceneNumber==SCENE_WILDWESTHUB)&&(AllowedToEnterLevel(SCENE_WESTBOSS)))
						{
							LevelCompleted(gameStatus.player1);
						}
					}
					else
					{
						LevelCompleted(gameStatus.player1);
					}

					gameStatus.player[0].stats->destructState = OVERLAYSTATE_LEAVING;
					gameStatus.player[0].stats->swirlState = OVERLAYSTATE_LEAVING;
					gameStatus.player[0].stats->moneyState = OVERLAYSTATE_LEAVING;
					gameStatus.player[0].stats->posterState = OVERLAYSTATE_LEAVING;
				}

				break;

			default:
				BookList_closeAndFree(finishChangeConfirm);
				SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
				UnPauseRumble(&controller1);
	// PP: REMOUT: Now handled by the Book List						finishChangeConfirm->Update();
	// TP: 			BookList_closeAndFree(finishChangeConfirm);
				
				break;
			}
		}
	}
	else
	{
		finishChangeConfirm->controlDelay -= fTime;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: EnterHubUpdate
	Purpose		: custom pause for when exiting an ingame level
	Parameters	: context
	Returns		: 
	Info		: 
*/
/* PP: REMOUT: Now handled by the Book List			
void EnterHubUpdate(const int32 context)
{
	enterLevelBook.update();

	 if (gameStatus.currentState == GAMESTATE_PAUSE)
		 return;

//	levelSelectPlayerStats.endOfLevelStatsBook->update();

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
//	levelSelectPlayerStats.endOfLevelStatsBook->bookDraw();
	enterLevelBook.draw();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

#if 0
  if(controller1.crossDebounceChannel->value)
	{
		FadeAndChangeScene(&camera[0].fade,&gameStatus,FADETYPE_NORMAL,context,0.5);
		SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
		levelSelectPlayerStats.endOfLevelStatsBook->close();
		SAFE_DELETE(levelSelectPlayerStats.endOfLevelStatsBook);
	}
	else
	if(CANCELBUTTON)
	{
		SetGameState(&gameStatus,GAMESTATE_INGAME,GAMESUBSTATE_NORMAL);
		levelSelectPlayerStats.endOfLevelStatsBook->close();
		SAFE_DELETE(levelSelectPlayerStats.endOfLevelStatsBook);
	}
#endif// PP: 0
}*/

/*  --------------------------------------------------------------------------------
	Function	: AllowedToEnterLevel
	Purpose		: Returns true/false if the player is allowed to enter the specified level
	Parameters	: level
	Returns		: true/false
	Info		: 
*/
bool	openLevels = false;	// TP: level roaming feature is disabled by default

bool AllowedToEnterLevel(int level)
{
	// TP: You can go anywhere from the front end
	if ((level==SCENE_FRONTEND)||(openLevels)) return true;

	switch(level)
	{
	case SCENE_ZOOBOSS:
		if (IsLevelComplete(SCENE_SAFARI)&&IsLevelComplete(SCENE_AQUA)&&IsLevelComplete(SCENE_ICEDOME)) return true;
		else return false;
		break;
	case SCENE_CITYBOSS:
		if (IsLevelComplete(SCENE_MUSEUM)&&IsLevelComplete(SCENE_DEPTSTR)&&IsLevelComplete(SCENE_CONSTRUCT)) return true;
		else return false;
		break;
	case SCENE_WESTBOSS:
		if (IsLevelComplete(SCENE_GRANDCANYON)&&IsLevelComplete(SCENE_GHOSTTOWN)&&IsLevelComplete(SCENE_GOLDMINE)) return true;
		else return false;
	case SCENE_TAZPREBOSS:
		if (IsLevelComplete(SCENE_TAZHUB)) return true;
		else return false;
	case SCENE_TAZBOSS:
		if (IsLevelComplete(SCENE_TAZPREBOSS)) return true;
		else return false;
	case SCENE_CITYHUB:
		if (IsLevelComplete(SCENE_AQUA)&&IsLevelComplete(SCENE_SAFARI)&&IsLevelComplete(SCENE_ICEDOME)&&IsLevelComplete(SCENE_ZOOBOSS)) return true;
		else return false;
	case SCENE_WILDWESTHUB:
		if (IsLevelComplete(SCENE_MUSEUM)&&IsLevelComplete(SCENE_DEPTSTR)&&IsLevelComplete(SCENE_CONSTRUCT)&&IsLevelComplete(SCENE_CITYBOSS)) return true;
		else return false;
	case SCENE_TAZHUB:
		if (IsLevelComplete(SCENE_GRANDCANYON)&&IsLevelComplete(SCENE_GHOSTTOWN)&&IsLevelComplete(SCENE_GOLDMINE)&&IsLevelComplete(SCENE_WESTBOSS)) return true;
		else return false;
	}

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: FindLevel
	Purpose		: Find a level number using a string
	Parameters	: 
	Returns		: 
	Info		: 
*/
int	FindLevel(char *levelName)
{
	int	i;

	for (i=0;i<ENDOFSCENES;i++)
	{
		if (strcmpi(levelName,sceneFileName[i]) == 0)
		{
			return i;
		}
	}

	return -1;
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSceneChangeMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToSceneChangeMessages(void)
{
	char	parmStringBuf[256];
	char	*message,*parmMessage;

	while (bkPopEvent(sceneChangeEventClient, parmStringBuf) == OK)
	{
		parmMessage = DecodeMessage(parmStringBuf, NULL, NULL);

		if(strcmpi("death", parmMessage)==0)
		{
			bkPrintf("Killing character 1\n");
			ResetCharacterPropertiesAfterDeath(gameStatus.player[0].actorInstance);

			gameStatus.player[0].camera->fade.SetPlayer(gameStatus.player[0].actorInstance);
			gameStatus.player[0].camera->fade.FadeAndChangeScene(&gameStatus, FADETYPE_CIRCLE, FADESCENE_DEATH, 1.0f, 0, true);
		}
		else
		{
			if(strcmpi("death2", parmMessage)==0)
			{
				bkPrintf("Killing character 2\n");
				ResetCharacterPropertiesAfterDeath(gameStatus.player[1].actorInstance);

				gameStatus.player[1].camera->fade.SetPlayer(gameStatus.player[1].actorInstance);
				gameStatus.player[1].camera->fade.FadeAndChangeScene(&gameStatus, FADETYPE_CIRCLE, FADESCENE_DEATH, 1.0f, 0, true);
			}
			else
			{
				if (strcmpi("reset",parmMessage)==0)
				{
					// TP: PROBLEM
					camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_CIRCLE,gameStatus.currentScene,1.0f);
					SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
				}
				else
				{
					// level change
					message = FindChar(parmMessage,'=');
					if (message)
					{
						*message++ = 0;
						// get start position
						map.startPos = bkCRC32((uchar*)message,strlen(message),0);
						map.startPos2 = bkCRC32((uchar*)message,strlen(message),0);
					}
					else
					{
						map.startPos = 0;
						map.startPos2 = 0;
					}
	

#ifndef LEVELCHANGETEST
#ifndef CONSUMERDEMO
					if ((!IN_MULTI_GAME)&&(!IN_BOSS_GAME)&&(!IN_HUB)&&(gameStatus.currentScene!=SCENE_LANGUAGESELECT)&&(gameStatus.currentScene!=SCENE_TAZINTROCUTSCENE)&&
						(gameStatus.currentScene!=SCENE_STARTGAMEINTRO)&&(gameStatus.currentScene!=SCENE_FRONTEND)&&(!gameStatus.demoMode))
					{
						
						int i = FindLevel(parmMessage);
						// TP: if level not found the continue
						if (i==-1) continue;

						// TP: If level is not accesable
						// TP: if (!AllowedToEnterLevel(i)) continue;
						// TP: do confirmation check and go into pause state
						if (gameStatus.globalSettings.saveInfo.stats[ConvertSceneRefToLevelScoreRef((EScene)(gameStatus.currentScene))].levelCompleted != TRUE)
						{
							SetGameState(&gameStatus,GAMESTATE_CUSTOMPAUSE,GAMESUBSTATE_NORMAL,(int32)ExitLevelConfirm,i);
/*							if (levelChangeConfirm)
							{
								PauseRumble(&controller1);
								levelChangeConfirm->open(NULL, true, true);
								levelChangeConfirm->Update();
							}*/
						}
						else
						{
							if (map.startPos)
							{
								// TP: if there is a map.startpos then the player exited by the door so get confirmation
								SetGameState(&gameStatus,GAMESTATE_CUSTOMPAUSE,GAMESUBSTATE_NORMAL,(int32)ExitLevelConfirm,i);

/*								if (levelChangeConfirm)
								{
									PauseRumble(&controller1);
									levelChangeConfirm->open(NULL, true, true);
									levelChangeConfirm->Update();
								}*/
							}
							else
							{
								// TP: exited by playing exit cutscene so dont get confirmation
								camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,i,1.0f);
								SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
							}
						}
					}
					else
					{
						if(IN_HUB)
						{
							if(strcmpi(parmMessage, "stats") == 0)
							{
								if(strcmpi(message, "false") == 0)
								{
									//enterLevelBook.shutdown();
									
									// PP: this is nicer
									enterLevelBook.close();

									showHubStats = FALSE;	
								}
								else
								{
									int i = FindLevel(message);
									// TP: if level not found the continue
									if (i==-1) continue;

									CreateEnterLevelStats(i);
									showHubStats = TRUE;
								}
							}
							else
							{
								int i = FindLevel(parmMessage);
								// TP: if level not found the continue
								if (i==-1) continue;

								if (!AllowedToEnterLevel(i)) continue;

								camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,i,1.0f);
								SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
								
								// PP: REMOUT: NO		enterLevelBook.shutdown();
								
								// PP: this is nicer
								enterLevelBook.close();

								showHubStats = FALSE;
							}
						}
						else
#endif
#endif
						{
							int i = FindLevel(parmMessage);
							// TP: if level not found the continue
							if (i==-1) continue;

							//if (!AllowedToEnterLevel(i)) continue;

							//strcpy(map.mapName,parmMessage);
							camera[0].fade.FadeAndChangeScene(&gameStatus,FADETYPE_NORMAL,i,1.0f);

							SetGameState(&gameStatus,GAMESTATE_FADING,GAMESUBSTATE_NORMAL);
						}
#ifndef CONSUMERDEMO
					}
#endif
				}
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToCameraMessages
	Purpose 	: Respond to camera messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToCameraMessages(void)
{
	char				parmStringBuf[256];
	char				*messageType;

	while(bkPopEvent(cameraEventClient, parmStringBuf) == OK)
	{
		messageType = parmStringBuf;

		// TP: Added this to remove the %X %X message header added by Babel on messages send from animations
		messageType = DecodeMessage(messageType, NULL, NULL);

		ProcessCameraMessages(messageType);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToTriggerMessages
	Purpose 	: Respond to sound messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToTriggerMessages(void)
{
	char				parmStringBuf[256];
	char				*messageType;
	char				*message;

	switch(map.allPostersDestroyed)
	{
	case 1:
		// TP: if there is an onlevelcomplete message then send it
		if (map.onLevelComplete)
		{
			// TP: make sure this is not triggered at the beginning of scene while on black screen
			if (!((gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_INIT)||
				(gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_RUNON)||
				(gameStatus.player[0].actorInstance->actorStatus->currentState==STATE_ENTERINGXDOOR)))
			{

				// TP: make sure there are no camera cases playing before I play the end of level scene
				if ((camera[0].cameraCase==NULL)&&(getThwapStage()==Off))
				{
//					SendTrigger(map.onLevelComplete, gameStatus.player[0].actorInstance->actorInstance.position);

					if(RequestMorronicMessage(MEVENT_ALLPOSTERS, 0.0f))
					{
						FlushAnimationQueue(gameStatus.player[0].actorInstance);
						PlayAnimationByName(gameStatus.player[0].actorInstance, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
						RequestStateChange(gameStatus.player[0].actorInstance, STATE_WAITFORTEXT);
					}
					// TP: dont want to re-trigger the end of level again
					map.allPostersDestroyed = 2;
				}
			}
		}
		else
		{
			LevelCompleted(gameStatus.player[0].actorInstance);
			// TP: dont want to re-trigger the end of level again
			map.allPostersDestroyed = 3;
		}
		break;
	case 2:
		if(!CheckIfDisplayingMorronicMessages(MEVENT_ALLPOSTERS))
		{
			SendTrigger(map.onLevelComplete, gameStatus.player[0].actorInstance->actorInstance.position);
			map.allPostersDestroyed = 3;
		}
		break;
	}

	RespondToCameraMessages();
	while(bkPopEvent(triggerEventClient, parmStringBuf) == OK)
	{
		bkPrintf("trigger:%s\n",parmStringBuf);

		messageType = parmStringBuf;
		message = FindChar(parmStringBuf, '=');
		if(message == NULL) return;
		*message++ = 0;

		ProcessTriggerForSpecialTypes(messageType, message);
	}
	RespondToPlatformMessages();
	RespondToAnimationMessages();
	RespondToFootstepMessages();

#ifndef CONSUMERDEMO
	RespondToZooBossMessages();				// PP: added for the Zoo boss game
	RespondToWestBossMessages();			// PP: added for the West boss game
	RespondToMinecartMessages();
#endif

	RespondToSpecialMessages();
	RespondToRumbleMessages();
	RespondToPromptMessages();
}

/*  --------------------------------------------------------------------------------
	Function	: DecodeMessage
	Purpose		: Returns the message and babel header info if available
	Parameters	: 
	Returns		: 
	Info		: 
*/

char *DecodeMessage(char *string, TBActorInstance **actorInstance, TBActorNodeInstance **actorInstanceNode)
{
	char				tempString[256];
	int32				parm1=0;
	int32				parm2=0;

/*	if ((string[0]>='0')&&(string[0]<='9'))
	{
		sscanf(string,"%X %X",&parm1,&parm2);
		string = FindChar(string,' ');
		string++;
		string = FindChar(string,' ');
		string++;
		strlwr(string);
	}*/

	// CMD: check to see that string is long enough to contain 2 eight digit nos. and two spaces
/*	if(strlen(string) > (8+1+8+1))
	{
		if((string[8] == ' ')&&(string[17] == ' '))
		{
			sscanf(string, "%X %X", &parm1, &parm2);
			string = FindChar(string, ' ');
			string++;
			string = FindChar(string, ' ');
			string++;
			strlwr(string);
		}
	}*/

	// CMD: if the string starts with an underscore then its one of ours
	if(string)
	{
		if(string[0] == '_')
		{
			// CMD: strip off the underscore
			string++;
			strlwr(string);
		}
		else
		{
			// CMD: must come via babel, needs decoding
			sscanf(string, "%X %X", &parm1, &parm2);
			string = FindChar(string, ' ');
			string++;
			string = FindChar(string, ' ');
			string++;
			strlwr(string);
		}

		if (actorInstance) *actorInstance = (TBActorInstance*)parm1;
		if (actorInstanceNode) *actorInstanceNode = (TBActorNodeInstance*)parm2;
	}
	
	return string;
}

/*	--------------------------------------------------------------------------------
	Function 	: ProcessTriggerForSpecialTypes
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ProcessTriggerForSpecialTypes(char *messageType, char *message)
{
	ACTORINSTANCE		*actorInstance;
	TBVector			tempVector;
	uint32				instanceCRC;
	float				tempFloat;
	int					tempInt;
	ACTORINSTANCE		*destructor;
	int					tempState;
	COLOUR				expCols[FB_NUM_STAGE_COLOURS]={FBP_COLOUR_HOT, FBP_COLOUR_COLD, FBP_COLOUR_SMOKE, FBP_COLOUR_FINAL};
	TBActorInstance		*messageInstance=NULL;
	TBActorNodeInstance	*messageNodeInstance=NULL;
	int					i, j;


	// TP: "%X %X message"
	messageType = DecodeMessage(messageType, &messageInstance, &messageNodeInstance);
	strlwr(message);

	if (strcmpi(messageType, "LEVELCOMPLETE")==0)
	{
		if (IN_HUB)
		{
			// TP: if in hub then you can only exit if the cutscene exists or using openLevels cheat
			if ((map.sceneNumber==SCENE_ZOOHUB)&&(AllowedToEnterLevel(SCENE_ZOOBOSS)))
			{
				LevelCompleted(gameStatus.player1);
			}
			if ((map.sceneNumber==SCENE_CITYHUB)&&(AllowedToEnterLevel(SCENE_CITYBOSS)))
			{
				LevelCompleted(gameStatus.player1);
			}
			if ((map.sceneNumber==SCENE_WILDWESTHUB)&&(AllowedToEnterLevel(SCENE_WESTBOSS)))
			{
				LevelCompleted(gameStatus.player1);
			}
		}
		else
		{

			// CMD: stop taz's spin sound
			StopChannel(&gameStatus.player[0].actorInstance->actorStatus->soundHandle[0]);

#ifdef CONSUMERDEMO
	#ifndef STANDALONE
			demoTimerBook->close();
	#endif
			LevelCompleted(gameStatus.player1);
#else
			SetGameState(&gameStatus,GAMESTATE_CUSTOMPAUSE,GAMESUBSTATE_NORMAL,(int32)FinishLevelConfirm,FADESCENE_NONE);
#endif
/*			if (finishChangeConfirm)
			{
				PauseRumble(&controller1);
				finishChangeConfirm->open(NULL, true, true);
				finishChangeConfirm->Update();
			}*/
		}
	}

#ifndef CONSUMERDEMO
	if(strcmpi(messageType, "PAD") == 0)
	{
		// CPW: player has gone over pad thing
		cityBossTouchedPad(atoi(message));
		return;
	}
#endif

	if (strcmpi(messageType, "WANTEDPOSTER") == 0)
	{
#ifndef STANDALONE
#ifdef CONSUMERDEMO
		if (demoGameTimer <= 0.0f)
			return;
#endif
#endif

		tempInt = atoi(message);

		AddWantedPosterToStats(tempInt,gameStatus.player[0].actorInstance,map.sceneNumber);

		if (characterSounds.sam == -1)
		{
			switch(bmRand()%3)
			{
			case 0:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samwanted_2.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
			case 1:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samwanted_3.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
			case 2:	CallbackWhenSampleFinished((characterSounds.sam=PlaySample("samwanted_5.wav",200)),CharacterSoundsFinishedCallback,&characterSounds.sam); break;
			}
		}

		return;
	}

#ifndef CONSUMERDEMO
	if(strcmpi(messageType, "floorpolisher") == 0)
	{
		if(strcmpi(message, "complete") == 0)
		{
//			floorPolisherWantedPosterInfo->completed = TRUE;
//			if(floorPolisherWantedPosterInfo->bookPtr)
//			{
//				floorPolisherWantedPosterInfo->bookPtr->close();
//			}
//			for(i = (WPDECAL_STRIPS-1);i >= 0;i--)
//			{
//				for(j = (WPDECAL_VERTS-1);j >= 0;j--)
//				{
//					BDVERTEX_SETA(&floorPolisherWantedPosterInfo->mesh[i][j], 0.0f);
//				}
//			}
			FreeFloorPolisherWantedPosterInfo(&floorPolisherWantedPosterInfo);
		}
		return;
	}
#endif

	if (strcmpi(messageType, "REMOVE") == 0)
	{
		// TP: TODO: take care of destructible counter if the object is a destructible
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			if (actorInstance->pathInfo)
			{
				InvalidatePlatform(actorInstance);
			}
			// TP: check if the removed object is a destructible that should be counted
			switch(actorInstance->special)
			{
			case SPECIALTYPE_DESTRUCTIBLE:
				if (actorInstance->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_COUNT)
				{
					if (gameStatus.player[0].stats) gameStatus.player[0].stats->numObjectsDestroyed += 1;
				}
				break;
			case SPECIALTYPE_MUSICBOX:
				break;
			}
			FreeActorInstance(actorInstance);
		}
		else
		{
			bkPrintf("*** WARNING *** Could not remove object %s\n",message);
		}
		// TP: incase the item is a falling object regen point
		RemoveObjectFromFallingListByCRC(instanceCRC);
		return;
	}	

	int	expType = -1;
	int flags = 0;

	if (strcmpi(messageType, "EXPLOSION")==0)
	{
		expType = 0;
	}
	else if (strcmpi(messageType, "EXPLOSIONNODE")==0)
	{
		expType = 1;
	}
	else if (strcmpi(messageType, "SMOKENODE")==0)
	{
		expType = 2;
		flags |= FIREBALLFLAG_NOCAMERASHAKE;
	}
	if (expType > -1)
	{
#define DEFAULT_EXPLOSION_TYPE		1	// PP: 'normal' explosion

		char	instanceName[256]	={""};
		char*	chSrc;
		char*	chDst;
		int		type;

		if (expType==2)
		{
			type	= 2;
		}
		else
		{
			type	= DEFAULT_EXPLOSION_TYPE;
		}
		
		chSrc= &message[0];
		chDst= &instanceName[0];

		// PP: read instance name, up to space...

		while(((*chSrc) != ' ') && ((*chSrc) != '\0'))
		{
			(*chDst)=(*chSrc);

			chSrc++;
			chDst++;
		}

		if (expType==0)
		{
			instanceCRC = bkCRC32((uchar*)instanceName,strlen(instanceName),0);

			actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		}

#define FBP_COLOUR_DUST		COLOUR(45,45,45,FBP_OPACITY_SMOKE)

		// PP: read optional type...
		COLOUR	expCols[4];
		if (expType==2)
		{
			expCols[0]=FBP_COLOUR_DUST;
			expCols[1]=FBP_COLOUR_FINAL;
			expCols[2]=FBP_COLOUR_DUST;
			expCols[3]=FBP_COLOUR_FINAL;
		}
		else
		{
			expCols[0]=FBP_COLOUR_HOT;
			expCols[1]=FBP_COLOUR_COLD;
			expCols[2]=FBP_COLOUR_SMOKE;
			expCols[3]=FBP_COLOUR_FINAL;
		}

		float		radius;

		if(expType==0)
		{
			if ((*chSrc) != '\0')
			{
				type=atoi(chSrc+1);

				chSrc+=2;// PP: skip the type number

				if((*chSrc) != '\0')
				{
					chSrc++;// PP: skip the space

					// PP: read optional colours
					for(int i=0; i<FB_NUM_STAGE_COLOURS; i++)
					{
						int	myint[3];

						// TP: I changed the , to a : because max can't export comma's
						sscanf(chSrc, "%d:%d:%d",&myint[0],&myint[1],&myint[2]);
						expCols[i].r = (uchar)myint[0];
						expCols[i].g = (uchar)myint[1];
						expCols[i].b = (uchar)myint[2];

						while((*chSrc) != '\0')
						{
							if((*chSrc) == '/')
							{
								chSrc++;
								goto nextCol;
							}

							chSrc++;
						}

						break;// PP: end of string - stop parsing about

nextCol:;// PP: go onto next colour

					}
				}
			}
		}
		else
		{
			if ((*chSrc) != '\0')
			{
				// TP: look for specified size
				radius = (float)atof(chSrc+1);
			}
			else
			{
				radius = 1.0f;
			}
		}

		if ((messageInstance)&&((expType==1)||(expType==2)))
		{
			if (messageNodeInstance)
			{	
				TBVector	position;

				baGetNodesWorldPosition(messageInstance, messageNodeInstance, position);
				radius = METERTOUNIT(radius); 
				// PP: SWITCH: temp?

				switch(type)
				{
				case 1:
					{
						Fireball_normalExplosion(position, radius, expCols[0], expCols[1], expCols[2], expCols[3],flags);
					}
					break;

				case 2:
					{
						Fireball_flatExplosion(position, radius, radius*2, expCols[0], expCols[1], expCols[2], expCols[3],flags);
					}
					break;
				}
			}
		}
		else
		{
			if (actorInstance)
			{	
				// PP: SWITCH: temp?

				switch(type)
				{
				case 1:
					{
						Fireball_normalExplosion(actorInstance, expCols[0], expCols[1], expCols[2], expCols[3],flags);
					}
					break;

				case 2:
					{
						Fireball_flatExplosion(actorInstance, expCols[0], expCols[1], expCols[2], expCols[3],flags);
					}
					break;
				}
			}
		}
		return;
	}

	if (strcmpi(messageType, "DESTROY") == 0)
	{
		actorInstance = map.animatedInstances.head.next;
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			switch(map.sceneNumber)
			{
#ifndef CONSUMERDEMO
			case SCENE_DESTRUCTIONCONSTRUCT:	
			case SCENE_DESTRUCTIONAQUA:
			case SCENE_DESTRUCTIONTASMANIA:
				// JW: It's a kludgey way of maintaining fairness but it works
				destructor = gameStatus.player[lastDestroyer].actorInstance;	break;
#endif

			default:
				// TP: better credit player 0 with destruction
				destructor = gameStatus.player[0].actorInstance;	break;
			}

			// TP: dirty bodge to simulate the effect of Taz spinning into the destructibles
			tempState = destructor->actorStatus->currentState;
			destructor->actorStatus->lastState = destructor->actorStatus->currentState;
			destructor->actorStatus->currentState = STATE_SMASH;
			if (actorInstance->special == SPECIALTYPE_DESTRUCTIBLE)
			{
				if (actorInstance->destructibleObjectInfo)
				{
					// TP: clear indestructible flag
					if (actorInstance->destructibleObjectInfo->flags&DESTRUCTIBLEFLAG_INDESTRUCTIBLE)
					{
						actorInstance->destructibleObjectInfo->flags &=~DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
						actorInstance->destructibleObjectInfo->flags |= DESTRUCTIBLEFLAG_NODELAY|DESTRUCTIBLEFLAG_NOBOUNCE;
						if (!RespondToDestructibleCollision(destructor,actorInstance))
						{
							actorInstance->destructibleObjectInfo->flags &= ~(DESTRUCTIBLEFLAG_NODELAY|DESTRUCTIBLEFLAG_NOBOUNCE);
							actorInstance->destructibleObjectInfo->flags |=DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
						}
					}
					else
					{
						actorInstance->destructibleObjectInfo->flags |= DESTRUCTIBLEFLAG_NOBOUNCE|DESTRUCTIBLEFLAG_NODELAY;
						if (!RespondToDestructibleCollision(destructor,actorInstance))
						{
							actorInstance->destructibleObjectInfo->flags &= ~(DESTRUCTIBLEFLAG_NOBOUNCE|DESTRUCTIBLEFLAG_NODELAY);
						}
					}
				}
			}
			destructor->actorStatus->lastState = destructor->actorStatus->currentState;
			destructor->actorStatus->currentState = (ECharState)tempState;
		}
		return;
	}
	if(strcmpi(messageType, "DEACTIVATE") == 0)
	{
		//search animated intsances for ACTORINSTANCE to deactivate
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			switch(actorInstance->special)
			{
			case SPECIALTYPE_ENEMY:
				actorInstance->genericAI->characterEnabled = false;
				break;
			case SPECIALTYPE_HIGHVOLTAGE:
				//deactivate dependant on the type of object
				actorInstance->highVoltageObjectInfo->currentState = HIGHVOLTAGE_OFF;
				break;
			case SPECIALTYPE_ELECTRICITY:
				actorInstance->flags |= ACTORFLAG_DONTDRAW;
				break;
			case SPECIALTYPE_TRIGGER:
				actorInstance->triggerObjectInfo->disabled = TRUE;
				break;
			case SPECIALTYPE_SPEEDBOOST:
				actorInstance->speedboostInfo->active = false;
				break;
			case SPECIALTYPE_SHATTER:
				//deactivate dependant on the type of object
				actorInstance->shatterInfo->collision = TRUE;
				actorInstance->shatterInfo->clock = actorInstance->shatterInfo->delay;
				break;
			case SPECIALTYPE_MIRROR:
				FreeMirror(actorInstance);
				break;
			case SPECIALTYPE_DESTRUCTIBLE:
				actorInstance->destructibleObjectInfo->flags |= DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
				break;
			case SPECIALTYPE_PROJECTILE:
			case SPECIALTYPE_PROJECTILESLIDE:
				actorInstance->projectileInfo->flags |= CANNONFLAG_INACTIVE;
				break;
			case SPECIALTYPE_TRANSPORTER:
				actorInstance->transporterInfo->active = false;
				break;
			case SPECIALTYPE_TRAMAMPOLINE:
				actorInstance->springyObjectInfo->active = false;
				break;
			case SPECIALTYPE_FOUNTAIN:
				if(actorInstance->fountainInfo->scenic)
				{
					actorInstance->fountainInfo->scenic = 2;
				}
				else
				{
					actorInstance->fountainInfo->active = FALSE;
				}
				break;
			case SPECIALTYPE_ESCALATOR:
				actorInstance->escalatorInfo->active = false;
				break;
			case SPECIALTYPE_ARROW:
				actorInstance->arrowInfo->state = ARROW_FADE;
				break;
#if BPLATFORM==GAMECUBE
			case SPECIALTYPE_REVERB:
				FadeOutEffect();
				break;
#endif
			}
		}
		else
		{
			// TP: see if it is a falling object generator
			SetFallingObjectState(instanceCRC,false);
		}
		return;
	}
	if(strcmpi(messageType, "DEACTIVATEONEXIT") == 0)
	{
		//search animated intsances for ACTORINSTANCE to deactivate
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			if (actorInstance->special == SPECIALTYPE_TRIGGER)
			{
				actorInstance->triggerObjectInfo->deactivateOnExit = TRUE;
			}
		}
		return;
	}
	if (strcmpi(messageType, "MASKNEXTTRIGGER")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			if (actorInstance->special == SPECIALTYPE_TRIGGER)
			{
				actorInstance->triggerObjectInfo->maskNextMessage = true;
			}
		}
		return;
	}

	if (strcmpi(messageType,"ACTIVATESECRET")==0)
	{
		ActivateSecretItem();
		return;
	}

	if (strcmpi(messageType,"DEACTIVATESECRET")==0)
	{
		DeactivateSecretItem();
		return;
	}

	if (strcmpi(messageType,"FRANK")==0)
	{
		StartFrankenMachine(message);
	}

	if (strcmpi(messageType,"ZEROHITS")==0)
	{
		//search animated intsances for ACTORINSTANCE to deactivate
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			if (actorInstance->special == SPECIALTYPE_DESTRUCTIBLE)
			{
				if (actorInstance->destructibleObjectInfo)
				{
					actorInstance->destructibleObjectInfo->hits = 0;
				}
			}
		}
	}

	if (strcmpi(messageType, "FORCEEXIT")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			switch(actorInstance->special)
			{
			case SPECIALTYPE_TRIGGER:
				if (actorInstance->triggerObjectInfo->active)
				{
					actorInstance->triggerObjectInfo->active = false;
					SendTrigger(actorInstance->triggerObjectInfo->exitRadius, actorInstance->actorInstance.position);
				}
				break;
			}
		}
	}

	if(strcmpi(messageType, "ACTIVATE") == 0)
	{
		// CMD: activate=minecart is a special case unlike any other
		if(strcmpi(message, "MINECART") == 0)
		{
			// CMD: search for mine cart in animated instances list
			for(actorInstance = map.animatedInstances.head.next;actorInstance != &map.animatedInstances.head;actorInstance = actorInstance->next)
			{
				if(actorInstance->special == SPECIALTYPE_MINECART)
				{
					if(actorInstance->mineCartInfo)
					{
						actorInstance->mineCartInfo->flags |= MINECART_MOVETOPOSITION;
						FlushAnimationQueue(actorInstance);
						PlayAnimationByName(actorInstance, "pull_up", 1.0f, 0, 0, 0.1f, REACT);
						return;
					}
				}
			}
		}
		//search animated intsances for ACTORINSTANCE to deactivate
		instanceCRC = bkCRC32((uchar*)(message),strlen(message),0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			switch(actorInstance->special)
			{
			case SPECIALTYPE_ENEMY:
				actorInstance->genericAI->characterEnabled = true;
				break;
			case SPECIALTYPE_TRIGGER:
				//deactivate dependant on the type of object
				actorInstance->triggerObjectInfo->disabled = FALSE;
				break;
			case SPECIALTYPE_DESTRUCTIBLE:
				actorInstance->destructibleObjectInfo->flags &=~DESTRUCTIBLEFLAG_INDESTRUCTIBLE;
				break;
			case SPECIALTYPE_SPEEDBOOST:
				actorInstance->speedboostInfo->active = true;
				break;
			case SPECIALTYPE_PROJECTILE:
			case SPECIALTYPE_PROJECTILESLIDE:
				actorInstance->projectileInfo->flags &= ~CANNONFLAG_INACTIVE;
				break;
			case SPECIALTYPE_TRANSPORTER:
				actorInstance->transporterInfo->active = true;
				break;
			case SPECIALTYPE_TRAMAMPOLINE:
				actorInstance->springyObjectInfo->active = true;
				break;
			case SPECIALTYPE_ESCALATOR:
				actorInstance->escalatorInfo->active = true;
				break;
			case SPECIALTYPE_ZAPPER:
				switch(actorInstance->zapperInfo->type)
				{
				case ZAPPER_MOUSE:
					if(gameStatus.player[0].actorInstance->actorStatus->currentState == STATE_MOUSE) return;
					break;
				case ZAPPER_BALL:
					if(gameStatus.player[0].actorInstance->actorStatus->currentState == STATE_BALL) return;
					break;
				case ZAPPER_TAZ:
					if((gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_MOUSE)
					&&(gameStatus.player[0].actorInstance->actorStatus->currentState != STATE_BALL))
					{
						return;
					}
					break;
				}
				bmVectorCopy(tempVector, gameStatus.player[0].actorInstance->actorInstance.position);
				tempVector[1] -= METERTOUNIT(0.5f);
				Fireball_normalExplosion(tempVector, METERTOUNIT(1.3f), expCols[0], expCols[1], expCols[2], expCols[3], FIREBALLFLAG_NOCAMERASHAKE); 
				RequestRumbleEffect(gameStatus.player[0].controller, 255, 0.0f, 0.5f, 1.0f, 0.0f);
				actorInstance->zapperInfo->flags |= ZAPPER_ACTIVE;
				actorInstance->zapperInfo->effectClock = ZAPPER_EFFECT_TIME;
				break;
			case SPECIALTYPE_MUSICBOX:
				if(actorInstance->musicBoxInfo)
				{
					actorInstance->musicBoxInfo->state = TRUE;
				}
				break;
			case SPECIALTYPE_FOUNTAIN:
				if(actorInstance->fountainInfo)
				{
					if(actorInstance->fountainInfo->scenic)
					{
						actorInstance->fountainInfo->scenic = 1;
					}
					else
					{
						actorInstance->fountainInfo->active = TRUE;
					}
				}
			#if BPLATFORM==GAMECUBE
			case SPECIALTYPE_REVERB:
				if(actorInstance->soundEffectType)
				FadeInEffect(actorInstance->soundEffectType);
				break;
			#endif
			}
		}
		else
		{
			// TP: see if it is a falling object generator
			SetFallingObjectState(instanceCRC,true);
		}

		return;
	}

	if (strcmpi(messageType, "SQUASHTAZ")==0)
	{
		if (EnterState(gameStatus.player[0].actorInstance, STATE_MOVESQUASHED))
		{
			PlayAnimationByName(gameStatus.player[0].actorInstance, "recover21a",1.0f, 1, 0, 0.1f, REACT);
			if (gameStatus.player[0].actorInstance)
			{
				gameStatus.player[0].actorInstance->flags &= ~ACTORFLAG_ALLOWUNSQUASHMOVE;
			}
		}
	}

	if (strcmpi(messageType, "UNSQUASHTAZ")==0)
	{
		if (gameStatus.player[0].actorInstance)
		{
			gameStatus.player[0].actorInstance->flags |= ACTORFLAG_ALLOWUNSQUASHMOVE;
		}
	}

#ifndef CONSUMERDEMO
	if (strcmpi(messageType, "TRANSPORT")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);
		if (actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map))
		{
			if(actorInstance->special == SPECIALTYPE_TRANSPORTER)
			{
				RespondToTransporterCollision(gameStatus.player1, actorInstance);
			}
		}
		return;
	}
#endif

	if (strcmpi(messageType, "PROJECT")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);
		if (actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map))
		{
			if(actorInstance->special == SPECIALTYPE_PROJECTILE)
			{
				RespondToCannonCollision(gameStatus.player1, actorInstance);
			}
		}
		return;
	}

	if (strcmpi(messageType, "SPEEDBOOST")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);
		if (actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map))
		{
			if(actorInstance->special == SPECIALTYPE_SPEEDBOOST)
			{
				bmVectorCopy(gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position);
				RespondToSpeedboostCollision(gameStatus.player1, actorInstance);
			}
		}
		return;
	}

	if(strcmpi(messageType, "DROP") == 0)
	{
		//search falling object list for actor to drop
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		if (actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map))
		{
			if((actorInstance->special == SPECIALTYPE_FALLINGOBJECT)&&(actorInstance->fallingObjectInfo))
			{
				//open the bombay doors
				actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
				actorInstance->fallingObjectInfo->waitOver = TRUE;
			}
		}
		return;
	}
	if(strcmpi(messageType, "LIGHTNING") ==0)
	{
		//search for lightning object
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		if(map.invisibleInstances.head.next)
		{
			for(actorInstance = map.invisibleInstances.head.next;actorInstance != &map.invisibleInstances.head;actorInstance = actorInstance->next)
			{
				if((actorInstance->special==SPECIALTYPE_LIGHTNING)&&(actorInstance->lightningInfo))
				{
					if(actorInstance->instanceCRC == instanceCRC)
					{
						actorInstance->lightningInfo->clock = 2.0f;
					}
				}
			}
		}
		return;
	}
	if(strcmpi(messageType, "QUAKE") ==0)
	{
#if(BPLATFORM == PS2)// PP: just taking a guess here
		tempFloat = (float)atoff(message);
#else// PP: not ps2
		tempFloat = (float)atof(message);
#endif// PP: not ps2

		if(tempFloat < 0.0f)
		{
			tempFloat = 0.0f;
		}
		if(tempFloat > 10.0f)
		{
			tempFloat = 10.0f;
		}
		tempFloat /= 100.0f;
		
		CAMERAINFO *cam = &camera[0];
		if ((cam->cameraCase)&&(cam->cameraCase->camera)) cam = cam->cameraCase->camera;

		cam->quakeOffsetLevel = tempFloat;
		cam->tiltOffsetLevel = RAD(tempFloat*50.0f);
		cam->flags |= CAMERA_QUAKEFADE;
		return;
	}
	if(strcmpi(messageType, "EXPLOSION") ==0)
	{
		//search for source object
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		if(map.levelInstances.head.next)
		{
			for(actorInstance = map.levelInstances.head.next;actorInstance != &map.levelInstances.head;actorInstance = actorInstance->next)
			{
				if(actorInstance->instanceCRC == instanceCRC)
				{
					StartExplosion(actorInstance->actorInstance.position,0.1f, 200.0f, "explosion.wav", EXPLOSIONTYPE_CARTOON);
					return;
				}
			}
		}
		if(map.invisibleInstances.head.next)
		{
			for(actorInstance = map.invisibleInstances.head.next;actorInstance != &map.invisibleInstances.head;actorInstance = actorInstance->next)
			{
				if(actorInstance->instanceCRC == instanceCRC)
				{
					StartExplosion(actorInstance->actorInstance.position,0.1f, 200.0f, "explosion.wav", EXPLOSIONTYPE_CARTOON);
					return;
				}
			}
		}
		return;
	}
	if(strcmpi(messageType, "ENABLECOLLISION") ==0)
	{
		//search for source object
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			if (!GetCollisionEntry(&collisionCache,actorInstance))
			{
				// TP: check not already on the collision list
				if (actorInstance->special==SPECIALTYPE_WATERYOBJECT)
				{
					AddActorToCollisionCache(&collisionCache,actorInstance,CACHEFLAGS_DOUBLESIDED);
				}
				else
				{
					AddActorToCollisionCache(&collisionCache,actorInstance,NULL);
				}
			}
		}
	}
	if(strcmpi(messageType, "DISABLECOLLISION") ==0)
	{
		//search for source object
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			RemoveActorFromCollisionCache(&collisionCache,actorInstance);
		}
	}
	if (strcmpi(messageType, "ENABLELIVEUPDATE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			ActivateLiveUpdate(GetCollisionEntry(&collisionCache, actorInstance));
		}
	}
	if (strcmpi(messageType, "DISABLELIVEUPDATE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			DeactivateLiveUpdate(GetCollisionEntry(&collisionCache, actorInstance));
		}
	}
	if (strcmpi(messageType, "ENABLELIVESCALEUPDATE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			COLLISIONLISTENTRY *collEntry = GetCollisionEntry(&collisionCache, actorInstance);
			if (collEntry) collEntry->flags |= CACHEFLAGS_NEEDSUPDATE|CACHEFLAGS_LIVESCALEUPDATE;
		}
	}
	if (strcmpi(messageType, "DISABLELIVESCALEUPDATE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);
		if (actorInstance)
		{
			COLLISIONLISTENTRY *collEntry = GetCollisionEntry(&collisionCache, actorInstance);
			if (collEntry) collEntry->flags &= ~CACHEFLAGS_LIVESCALEUPDATE;
		}
	}
	if(strcmpi(messageType, "PORTAL") == 0)
	{
		if(strcmpi(message, "TRUE") == 0)
		{
			gameStatus.player[0].actorInstance->flags |= ACTORFLAG_DONTCHECKBOUNDINGBOX;
		}
		if(strcmpi(message, "FALSE") == 0)
		{
			gameStatus.player[0].actorInstance->flags &= ~ACTORFLAG_DONTCHECKBOUNDINGBOX;
			gameStatus.player[0].actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_DIRGRAVITY;
			RequestStateChange(gameStatus.player[0].actorInstance, STATE_MOVE);
		}
	}
	if (strcmpi(messageType, "INVISIBLE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);

		if (actorInstance)
		{
			actorInstance->flags |= ACTORFLAG_DONTDRAW;
		}
	}
	if (strcmpi(messageType, "VISIBLE")==0)
	{
		instanceCRC = bkCRC32((uchar*)(message), strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL,instanceCRC,&map);

		if (actorInstance)
		{
			if (actorInstance->instanceList == &map.invisibleInstances)
			{
				RemoveInstanceFromInstanceList(actorInstance,&map.invisibleInstances);

				if (actorInstance->actorInstance.numAnimSegments)
				{
					AddInstanceToInstanceList(actorInstance,&map.animatedInstances);
				}
				else
				{
					AddInstanceToInstanceList(actorInstance,&map.levelInstances);
				}
			}

			actorInstance->flags &=~ACTORFLAG_DONTDRAW;
		}
	}
	if(strcmpi(messageType, "RESET") == 0)
	{
		instanceCRC = bkCRC32((uchar*)message, strlen(message), 0);

		actorInstance = FindActorInstanceInMap(NULL, instanceCRC, &map);

		if(actorInstance)
		{
#ifndef CONSUMERDEMO
			if(actorInstance->special == SPECIALTYPE_MINECART)
			{
				actorInstance->mineCartInfo->flags &= ~MINECART_CESTTOUT;
				ResetMinecartAfterCompletion(actorInstance);
			}
#endif
		}
		return;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToPlatformMessages
	Purpose 	: Respond to platform messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToPlatformMessages(void)
{
	char				parmStringBuf[256];
	char				*messageType;
	char				*message;
	ACTORINSTANCE		*actorInstance;
	PATH				*path;
	TBActorInstance		*messageInstance=NULL;
	TBActorNodeInstance	*messageNodeInstance=NULL;

	// TP: Format		

	while(bkPopEvent(platformEventClient, parmStringBuf) == OK)
	{
		bkPrintf("platform:%s\n",parmStringBuf);

		messageType = parmStringBuf;

		messageType = DecodeMessage(messageType, &messageInstance, &messageNodeInstance);

		message = FindChar(messageType, '=');
		if(message == NULL) return;
		*message++ = 0;

		if (strcmpi(messageType,"resetstart")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					SetPathToStart(actorInstance->pathInfo);
				}
			}
		}

		if (strcmpi(messageType,"stop")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					ChangePathState(actorInstance->pathInfo,PATHSTATE_WAITING);
				}
			}
		}
		if (strcmpi(messageType,"start")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					if (actorInstance->pathInfo->state==PATHSTATE_WAITING)
					{
						actorInstance->pathInfo->state = actorInstance->pathInfo->lastState;
					}
				}
			}
		}
		if (strcmpi(messageType,"clock")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					ChangePathState(actorInstance->pathInfo,PATHSTATE_CLOCKWISE);
				}
			}
		}
		if (strcmpi(messageType,"anticlock")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					ChangePathState(actorInstance->pathInfo,PATHSTATE_ANTICLOCKWISE);
				}
			}
		}
		if (strcmpi(messageType,"noloop")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					SetPathLoop(actorInstance->pathInfo,PATHLOOP_NONE);
				}
			}
		}
		if (strcmpi(messageType,"loop")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					SetPathLoop(actorInstance->pathInfo,PATHLOOP_LOOP);
				}
			}
		}
		if (strcmpi(messageType,"reset")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					SetPathLoop(actorInstance->pathInfo,PATHLOOP_RESET);
				}
			}
		}
		if (strcmpi(messageType,"yoyo")==0)
		{
			if (actorInstance = FindActorInstanceInMap(message,0,&map))
			{
				if (actorInstance->pathInfo)
				{
					SetPathLoop(actorInstance->pathInfo,PATHLOOP_YOYO);
				}
			}
		}
		if (actorInstance = FindActorInstanceInMap(messageType,0,&map))
		{
			if (actorInstance->pathInfo)
			{
				path = FindPathInList(message,NULL);
				if (path) actorInstance->pathInfo->path = path;
			}
		}
	}
}

static int DeactivateLiveUpdateCallback(
    EBQueueCallbackReason reasonCode,  // reason for the call
    TBActorInstance *actor,             // ptr to the actor that triggered the event
    TBActorNodeInstance *node,          // ptr to the node that triggered the event
    TBActorAnimSegment *segment,       // ptr to the anim segment that triggered the event
    void *parameters)                  // client supplied context value
{
	DeactivateLiveUpdate(GetCollisionEntry(&collisionCache,(ACTORINSTANCE*)actor));
	return true;
}

static int ActivateLiveUpdateCallback(
    EBQueueCallbackReason reasonCode,  // reason for the call
    TBActorInstance *actor,             // ptr to the actor that triggered the event
    TBActorNodeInstance *node,          // ptr to the node that triggered the event
    TBActorAnimSegment *segment,       // ptr to the anim segment that triggered the event
    void *parameters)                  // client supplied context value
{
	ActivateLiveUpdate(GetCollisionEntry(&collisionCache,(ACTORINSTANCE*)actor));
	return true;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAnimQueueFlags
	Purpose : update the flags for entries in an actor instance's anim queue
	Parameters : actor instance, root instance node or NULL for all nodes, 
				 flags to change, new values for flags, queue entry to set from, recurse?
	Returns : 
	Info : 
*/
#if 0
void baSetAnimQueueFlags(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
							uint32 flagMask, uint32 newFlagValues, int setFrom, int recurse)
{
	TBActorNodeInstance		*child;
	int				loop, at;

	// set all nodes
	if (nodeInstance == NULL)
	{
		nodeInstance = actorInstance->rootNodeInstance;
		do
		{
			baSetAnimQueueFlags(actorInstance, nodeInstance, flagMask, newFlagValues, setFrom, recurse);
			nodeInstance = nodeInstance->next;
		} while(nodeInstance != actorInstance->rootNodeInstance);
		
		return;
	}

	// set flags for each queue entry
	at = (nodeInstance->currentQueue + setFrom) % nodeInstance->queueSize;
	for(loop = setFrom; loop < nodeInstance->queueSize; loop ++)
	{
		if (nodeInstance->animQueue[at].active)
			nodeInstance->animQueue[at].flags = (nodeInstance->animQueue[at].flags & (~flagMask)) | (newFlagValues & flagMask);

		at = (at + 1) % nodeInstance->queueSize;
	}

	// recurse on children if required and possible
	if ((recurse) && (nodeInstance->children))
	{
		child = nodeInstance->children;
		do {
			baSetAnimQueueFlags(actorInstance, child, flagMask, newFlagValues, setFrom, TRUE);
			child = child->next;
		} while (child != nodeInstance->children);
	}
}
#endif

/*	--------------------------------------------------------------------------------
	Function 	: RespondToAnimationMessages
	Purpose 	: Respond to animation messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToAnimationMessages(void)
{
	char				parmStringBuf[256];
	char				*messageType;
	char				*message;
	ACTORINSTANCE		*actorInstance;
	TBEventClient		*client;

	while(bkPopEvent(animationEndLoopEventClient, parmStringBuf) == OK)
	{
		messageType = DecodeMessage(parmStringBuf,NULL,NULL);

		message = FindChar(parmStringBuf, '=');

		if(message == NULL) 
		{
			// TP: if no message then play first anim segment
			actorInstance = FindActorInstanceInMap(messageType,NULL,&map);
			if (actorInstance)
			{
				// TP: turn off the looping flag on this actorinstance
				baSetAnimQueueFlags(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance,
						BANIMQUEUEFLAG_LOOPING, 0, 0, true);
			}
		}
	}

	while(bkPopEvent(animationsetEndEventClient, parmStringBuf) == OK)
	{
		messageType = DecodeMessage(parmStringBuf,NULL,NULL);

		message = FindChar(parmStringBuf, '=');

		if(message) 
		{
			*message++ = 0;
			// TP: if no message then play first anim segment
			actorInstance = FindActorInstanceInMap(messageType,NULL,&map);
			if (actorInstance)
			{
				// TP: turn off the looping flag on this actorinstance
				SetToEndFrame(actorInstance, message);
			}
		}
	}

	// TP: animation:instance=anim
	for (int i=0;i<8;i++)
	{
		switch(i)
		{
		case 0:
			client = animationEventClient;
			break;
		case 1:
			client = animationlEventClient;
			break;
		case 2:
			client = animationqEventClient;
			break;
		case 3:
			client = animationlqEventClient;
			break;
		case 4:
			client = animationcEventClient;
			break;
		case 5:
			client = animationclEventClient;
			break;
		case 6:
			client = animationcqEventClient;
			break;
		case 7:
			client = animationclqEventClient;
			break;
		}

		while(bkPopEvent(client, parmStringBuf) == OK)
		{
			messageType = DecodeMessage(parmStringBuf,NULL,NULL);

			message = FindChar(parmStringBuf, '=');

			if(message == NULL) 
			{
				// TP: if no message then play first anim segment
				actorInstance = FindActorInstanceInMap(messageType,NULL,&map);
				if (actorInstance)
				{
					if (actorInstance->actorInstance.numAnimSegments)
					{
						baQueueNodeAnimNormal(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, 
									&actorInstance->actorInstance.animSegments[0], 512+(bmRand()%200), -1, BANIMQUEUEFLAG_LOOPING, NULL,NULL);
					}
				}
				else
				{
					bkPrintf("Could not find instance %s to play animation on.\n",messageType);
				}
				continue;
			}
			*message++ = 0;

			actorInstance = FindActorInstanceInMap(messageType,NULL,&map);
			if (actorInstance)
			{
				// TP: Must flush animation queue first
				if ((i==0)||(i==1)||(i==4)||(i==5))
				{
					FlushAnimationQueue(actorInstance);

					// TP: if there is live collision needed for this animation then turn it on
					if (i>3)
					{
						ActivateLiveUpdate(GetCollisionEntry(&collisionCache,actorInstance));
						// TP: queue deactive on completion
						PlayAnimationByNameAndSetCallback(&actorInstance->actorInstance,message,1.0f,i&1?1:0,i>1?1:0,0.1f,DeactivateLiveUpdateCallback,0);
					}
					else
					{
						PlayAnimationByName(actorInstance,message,1.0f,i&1?1:0,i>1?1:0,0.1f,NONE);
					}
				}
				else
				{
					// TP: if there is live collision needed for this animation then turn it on or queue it
					if (i>3)
					{
						if (CheckIfPlayingAnimation(actorInstance,NULL))
						{
							// TP: set callback to set live collision
							// TP: queue activate on completion of last entry 
							SetCallbackOnLastAnimInQueue(&actorInstance->actorInstance,ActivateLiveUpdateCallback,0);
							// TP: queue deactive on completion
							PlayAnimationByNameAndSetCallback(&actorInstance->actorInstance,message,1.0f,i&1?1:0,i>1?1:0,0.1f,DeactivateLiveUpdateCallback,0);
						}
						else
						{
							// TP: alter the live collision
							ActivateLiveUpdate(GetCollisionEntry(&collisionCache,actorInstance));
							// TP: queue deactive on completion
							PlayAnimationByNameAndSetCallback(&actorInstance->actorInstance,message,1.0f,i&1?1:0,i>1?1:0,0.1f,DeactivateLiveUpdateCallback,0);
						}
					}
					else
					{
						PlayAnimationByName(actorInstance,message,1.0f,i&1?1:0,i>1?1:0,0.1f,NONE);
					}
				}
			}
			else
			{
				bkPrintf("Could not find instance %s to play animation on.\n",messageType);
			}
		}
	}
}

void ghostTrappedCallback(ACTORINSTANCE *actorInstance)
{
	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "trapped", 1.0f, 1, 0, 0.1f, REACT);
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSpecialMessages
	Purpose 	: Respond to special messages generated by animations directly using notes
	Parameters 	: 
	Returns 	: 
	Info 		:
*/
void RespondToSpecialMessages(void)
{
	ACTORINSTANCE		*actorInstance,*tempInstance;
	char				parmStringBuf[256];
	char				*message;
	char				*equals;
	char				messageParmsBuf[256];
	char				*messageParms;
	int					temp, i;
	TBVector			pos,tempVector,velocity;
	TBMatrix			tempMatrix;
	TBQuaternion		quat, resultQuat;
	TBActorNodeInstance		*node, *nodeFromMessage;
	float				floatOne, floatTwo, floatThree;

	// TP: These messages will come in the form 0xXXXXXXX 0xXXXXXXXX firestart for example
	// TP: the first hex number is the actorInstance that sent the message, the second is the node
	while (bkPopEvent(specialEventClient, parmStringBuf) == OK)
	{
		bkPrintf("special:%s.\n",parmStringBuf);

		message = DecodeMessage(parmStringBuf, (TBActorInstance**)&actorInstance, &nodeFromMessage);

		// TP: take a copy of the message
		strcpy(messageParmsBuf, message);

		// TP: look for any equals, for example the message activate=instance
		equals = FindChar(message, '=');
		if(equals) 	equals[0] = 0;

		// TP: messageParmsBuf equals first part of the message, messageParms is the second
		messageParms = FindChar(messageParmsBuf, '=');
		messageParms++;


		if(strcmpi(message,"firestart") == 0)
		{
			if((actorInstance)&&(actorInstance->characterInfo))
			{
				PlaySample("chilliuse.wav",255,actorInstance->actorInstance.position);
				actorInstance->characterInfo->powerUpInfo.state = POWERUPSTATE_CHILLION;
			}
			continue;
		}
		if(strcmpi(message,"fireend") == 0)
		{
			if((actorInstance)&&(actorInstance->characterInfo))
			{
				actorInstance->characterInfo->powerUpInfo.state = POWERUPSTATE_CHILLIOFF;
			}
			continue;
		}

#ifndef CONSUMERDEMO
		if(strcmpi(message,"tvthump") == 0)
		{
			if(frontEndMenu.gameOptionsPage)
			{
				frontEndMenu.gameOptionsPage->resetScreen = TRUE;
			}
			continue;
		}
#endif

		if (strcmpi(message,"ontv")==0)
		{
			CutsceneTv(true);
			continue;
		}

		if (strcmpi(message,"offtv")==0)
		{
			CutsceneTv(false);
			continue;
		}

		// TP: Check for enemy character messages
		if (CheckForCharacterMessages(actorInstance, message)) continue;

		if(strcmpi(message, "keeperhit") == 0)
		{
			if(node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "handle"))
			{
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, pos);
				bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
				bmVectorSet(tempVector, 0.0f, 0.0f, METERTOUNIT(1.8f), 1.0f);
				bmMatMultiplyVector(tempMatrix, tempVector);
				bmVectorAdd(pos, pos, tempVector);
				if(bmVectorDistance(pos, gameStatus.player[0].actorInstance->actorInstance.position) 
										< (gameStatus.player1->actorBehaviour->collSphere.radius + METERTOUNIT(0.45f)))
				{
					if(gameStatus.player[0].actorInstance->actorStatus->currentState == STATE_SPIN)
					{
						SpinDown(&gameStatus.player[0]);
					}
					SetActorPosition(gameStatus.player[0].actorInstance, pos);
					RequestStateChangePlus(gameStatus.player[0].actorInstance, STATE_PLAYANIMATION, ghostTrappedCallback);
					CaptureTaz(NULL);
				}
			}
		}
		if (strcmpi(message,"throwhat")==0)
		{
			PlaySample("swipe.wav", 255);	

			// TP: taz throws snowball
			SETVECTOR(pos,METERTOUNIT(-0.303f),METERTOUNIT(0.414f),METERTOUNIT(-1.143f),1.0f);
			bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
			bmMatMultiplyVector(tempMatrix,pos);

			SETVECTOR(tempVector,0.0f,0.5f,-1.0f,1.0f);
			bmMatMultiplyVector(tempMatrix,tempVector);
			bmVectorScaleToLength(velocity,tempVector,METERTOUNIT(12.0f));

			bmVectorAdd(pos,pos,actorInstance->actorInstance.position);

			DEBRIS	*debris;

			debris = TazSpit(actorInstance,bkCRC32((uchar*)"costume\\indy_flyinghatobject.obe",strlen("costume\\indy_flyinghatobject.obe"),0),pos,velocity,0.0f,METERTOUNIT(0.5f));
			if (debris)
			{
				if (debris->actorStructure)
				{
					// TP: temp attrib to let us know that the debris came from a costume attack
					debris->actorStructure->actorStatus->costumeDebris = true;
				}
				SETVECTOR(debris->actorInstance->scale,1.0f,1.0f,1.0f,1.0f);
				EnableActorLineRenderMode(debris->actorInstance);
			}
			continue;
		}
		if (strcmpi(message,"throwsnowball")==0)
		{
			PlaySample("hitsnowball.wav", 255);	

			// TP: taz throws snowball
			SETVECTOR(pos,METERTOUNIT(-1.156f),METERTOUNIT(0.568f),METERTOUNIT(-0.347f),1.0f);
			bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
			bmMatMultiplyVector(tempMatrix,pos);

			SETVECTOR(tempVector,0.0f,0.2f,-1.0f,1.0f);
			bmMatMultiplyVector(tempMatrix,tempVector);
			bmVectorScaleToLength(velocity,tempVector,METERTOUNIT(20.0f));

			bmVectorAdd(pos,pos,actorInstance->actorInstance.position);

			DEBRIS	*debris;

			debris = TazSpit(actorInstance,bkCRC32((uchar*)"snowball.obe",strlen("snowball.obe"),0),pos,velocity,0.0f,METERTOUNIT(0.5f));
			if (debris)
			{
				if (debris->actorStructure)
				{
					// TP: temp attrib to let us know that the debris came from a costume attack
					debris->actorStructure->actorStatus->costumeDebris = true;
				}
				SETVECTOR(debris->actorInstance->scale,1.0f,1.0f,1.0f,1.0f);
				EnableActorLineRenderMode(debris->actorInstance);
			}
			continue;
		}

		if (strcmpi(message,"cowboyshoot")==0)
		{
			TBVector				pos,tempVector,velocity;
			TBMatrix				tempMatrix;
			TBActorNodeInstance		*node;

			// TP: must find correct start point for bullet
			bmVectorCopy(pos,actorInstance->actorInstance.position);
			pos[Y] += METERTOUNIT(0.5f);

			bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
			SETVECTOR(tempVector,0.0f,0.0f,-1.0f,1.0f);
			bmMatMultiplyVector(tempMatrix,tempVector);
			bmVectorScaleToLength(velocity,tempVector,METERTOUNIT(20.0f));

			DEBRIS	*debris;

			debris = TazSpit(actorInstance,bkCRC32((uchar*)"snowball.obe",strlen("snowball.obe"),0),pos,velocity,0.5f,METERTOUNIT(0.25f));
			if (debris)
			{
				if (debris->actorStructure)
				{
					// TP: temp attrib to let us know that the debris came from a costume attack
					debris->actorStructure->actorStatus->costumeDebris = true;
				}
				SETVECTOR(debris->actorInstance->scale,0.0f,0.0f,0.0f,1.0f);
				EnableActorCartoonRenderMode(debris->actorInstance);
			}
			// CMD: now for the exciting stuff, sorry tony, now for the smoke
			if(strcmpi(messageParms, "left") == 0)
			{
				bmVectorSet(tempVector, 0.0f, 0.0f, METERTOUNIT(-0.16f), 1.0f);
				baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance, "fingerl1a"), pos); //, tempVector);
			}
			if(strcmpi(messageParms, "right") == 0)
			{
				bmVectorSet(tempVector, 0.0f, 0.0f, METERTOUNIT(-0.16f), 1.0f);
				baGetNodesWorldPosition(&actorInstance->actorInstance, baFindNode(actorInstance->actorInstance.rootNodeInstance, "fingerr1a"), pos); //, tempVector);
			}
			// CMD: find a velocity in the direction of taz
			// CMD: taz's orientation and node's are close enough that taz's will do
			bmVectorSet(tempVector, 0.0f, 0.0f, METERTOUNIT(-3.0f), 1.0f);
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			bmMatMultiplyVector(tempMatrix, tempVector);

			CreateImpactClouds(pos, NULL, tempVector, METERTOUNIT(0.05f), 2.0f, 0.0f, SMOKE_COWBOYFINGER);

			if(bmRand()%2)	PlaySample("fart1.wav", 255, actorInstance->actorInstance.position, 0);
			else			PlaySample("fart2.wav", 255, actorInstance->actorInstance.position, 0);
		}

#ifndef CONSUMERDEMO
		if (strcmpi(message,"cheesegun")==0)
		{
			FireCheese(actorInstance);
		}
		if (strcmpi(message,"surfboardattack")==0)
		{
			PlaySample("surfboardwhack.wav", 255);

			TBActorInstance *attach;
			attach = ReturnAttachedObjectInstance(actorInstance,GetAttachedObjectsHandle(actorInstance,"costume\\aquasurfboard.obe"));

			TBVector	offset;
			SETVECTOR(offset,METERTOUNIT(-0.2f),METERTOUNIT(0.0f),METERTOUNIT(-0.5f),1.0f);

			bmMatMultiplyVector(attach->objectToWorld,offset);
			TazAttack(actorInstance, NULL, offset);

			TBMatrix	tempMatrix;
			// TP: find position in from of taz for smash move
			SETVECTOR(tempVector,0.0f,METERTOUNIT(0.5f),METERTOUNIT(-1.5f),1.0f);
			bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
			bmMatMultiplyVector(tempMatrix,tempVector);
			bmVectorAdd(tempVector,tempVector,actorInstance->actorInstance.position);

			SmashStuff(&map.levelInstances,actorInstance,METERTOUNIT(4.0f),100,tempVector,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.animatedInstances,actorInstance,METERTOUNIT(4.0f),100,tempVector,SMASHFLAG_RANGEDATTACK);
			SmashStuff(&map.invisibleInstances,actorInstance,METERTOUNIT(4.0f),100,tempVector,SMASHFLAG_RANGEDATTACK);
		}

		// SO: Special Taz Pre-Boss events...
		if (strcmpi(message,"lightening")==0)
		{
			tazBoss1Game.respondToEvent(TBEVENT_LIGHTNING);
		}
		if (strcmpi(message,"thump")==0)
		{
			tazBoss1Game.respondToEvent(TBEVENT_THUMP);
		}
		if (strcmpi(message,"bigbang")==0)
		{
			tazBoss1Game.respondToEvent(TBEVENT_BIGBANG);
		}

		// CMD: mecha tweety messages
//		if(strcmpi(message, "magnetattack")==0)
		if(strcmpi(message, "handsout") == 0)
		{
			if(actorInstance->mechaTweetyInfo)
			{
				RequestBuzzerEffect(&controller1, 0.5f, 0.0, WAVEFORM_RANDOM);
				switch(actorInstance->mechaTweetyInfo->aiState)
				{
				case MTWEETYAI_ATTACKELECLEFT:
					actorInstance->mechaTweetyInfo->leftElectricAttack->state = TRUE;
					PlaySample("laser1.wav", 255);
					break;
				case MTWEETYAI_ATTACKELECRIGHT:
					actorInstance->mechaTweetyInfo->rightElectricAttack->state = TRUE;
					PlaySample("laser2.wav", 255);
					break;
				}
			}
		}
		if(strcmpi(message, "handsback") == 0)
		{
			if(actorInstance->mechaTweetyInfo)
			{
				switch(actorInstance->mechaTweetyInfo->aiState)
				{
				case MTWEETYAI_ATTACKELECLEFT:
					actorInstance->mechaTweetyInfo->leftElectricAttack->state = FALSE;
					break;
				case MTWEETYAI_ATTACKELECRIGHT:
					actorInstance->mechaTweetyInfo->rightElectricAttack->state = FALSE;
					break;
				}
			}
		}
		if(strcmpi(message, "grabcrate")==0)
		{
			if(actorInstance->mechaTweetyInfo)
			{
				SETVECTOR(tempVector, METERTOUNIT(0.416f), METERTOUNIT(-0.426f), METERTOUNIT(-0.842), 1.0f);
				actorInstance->mechaTweetyInfo->attachHandle = AttachObjectToCharacter(actorInstance, "extras\\ninjathrowingcratetester.obe", "armr_lower", tempVector, NULL, NULL, 0);
				actorInstance->mechaTweetyInfo->flags |= MTWEETY_GOTCRATE;
			}
		}
		if(strcmpi(message, "throwcrate")==0)
		{
			if(actorInstance->mechaTweetyInfo)
			{
				bmVectorCopy(actorInstance->mechaTweetyInfo->targetPos, gameStatus.player[0].actorInstance->actorInstance.position);
				actorInstance->mechaTweetyInfo->flags |= MTWEETY_THROWCRATE;
				actorInstance->mechaTweetyInfo->flags &= ~MTWEETY_VULNERABLE;
				RequestRedShieldStateChange(actorInstance->mechaTweetyInfo->shieldInfo, REDSHIELD_ACTIVE);
			}
		}
		if(strcmpi(message, "grabtaz")==0)
		{
		}
		if(strcmpi(message, "throwtaz")==0)
		{
		}
		if(strcmpi(message, "vulnerable") == 0)
		{
			if(actorInstance->mechaTweetyInfo)
			{
				RequestRedShieldStateChange(actorInstance->mechaTweetyInfo->shieldInfo, REDSHIELD_INACTIVE);
				actorInstance->mechaTweetyInfo->flags |= MTWEETY_VULNERABLE;
			}
		}
		if(strcmpi(message, "magnetattack") == 0)
		{
			RequestRumbleEffect(&controller1, 200, 0.25f, 1.0f, 2.0f, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 2.0f);
			RequestBuzzerEffect(&controller1, 1.0f, 0.0, WAVEFORM_SQUARE, 4.0f);
			actorInstance->mechaTweetyInfo->flags |= MTWEETY_MAGNETCHARGE;
			actorInstance->mechaTweetyInfo->magnetSoundHandle = PlaySample("loop\\electricloop.wav", 255);
		}
#endif
//		if(strcmpi(message, "handsback") == 0)
//		{
//			actorInstance->mechaTweetyInfo->flags &= ~MTWEETY_MAGNETCHARGE;
//		}
		if(strcmpi(message, "activate") == 0)
		{
			tempInstance = FindActorInstanceInMap(messageParms, 0, &map);
			if(tempInstance->special != -1)
			{
				switch(tempInstance->special)
				{
				case SPECIALTYPE_FOUNTAIN:
					if(tempInstance->fountainInfo)
					{
						tempInstance->fountainInfo->fountainState = FOUNTAIN_MAXIMUM;
					}
					break;
				}
			}
		}
		if(strcmpi(message, "sonicstart") == 0)
		{
			CreateRapperBlast(actorInstance);
		}
		if(strcmpi(message, "shakeleft") == 0)
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "hairstem");
			if(node)
			{
				// CMD: calculate matrix representing taz's oreintation
				// CMD: and find the node's position
				bmVectorSet(tempVector, 0.0f, 1.0f, 0.0f, RAD(145.0f));
				bmRotationToQuat(quat, tempVector);
				bmQuatMultiply(resultQuat, quat, actorInstance->actorInstance.orientation);
				bmQuatToMatrix(tempMatrix, resultQuat);
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, pos);

				floatOne = METERTOUNIT(0.02f);
				floatTwo = METERTOUNIT(0.0f);
				floatThree = 1.0f;
				for(i = 5;i > 0;--i)
				{
					bmVectorSet(velocity, METERTOUNIT(UtilGenerateRandomNumber(0.5f, -0.5f)), METERTOUNIT(UtilGenerateRandomNumber(0.5f, 0.0f)),
														METERTOUNIT(UtilGenerateRandomNumber(2.0f, 1.5f)), 1.0f);
					bmMatMultiplyVector(tempMatrix, velocity);

					baCreateExtra(squishExtraSystem, pos, velocity, &floatOne, &floatTwo, &floatThree);
				}
			}
		}
		if(strcmpi(message, "shakeright") == 0)
		{
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "hairstem");
			if(node)
			{
				// CMD: and find the node's position
				bmVectorSet(tempVector, 0.0f, 1.0f, 0.0f, RAD(215.0f));
				bmRotationToQuat(quat, tempVector);
				bmQuatMultiply(resultQuat, quat, actorInstance->actorInstance.orientation);
				bmQuatToMatrix(tempMatrix, resultQuat);
				baGetNodesWorldPosition(&actorInstance->actorInstance, node, pos);

				floatOne = METERTOUNIT(0.02f);
				floatTwo = METERTOUNIT(0.0f);
				floatThree = 1.0f;
				for(i = 5;i > 0;--i)
				{
					bmVectorSet(velocity, METERTOUNIT(UtilGenerateRandomNumber(0.5f, -0.5f)), METERTOUNIT(UtilGenerateRandomNumber(0.5f, 0.0f)),
														METERTOUNIT(UtilGenerateRandomNumber(2.0f, 1.5f)), 1.0f);
					bmMatMultiplyVector(tempMatrix, velocity);

					baCreateExtra(squishExtraSystem, pos, velocity, &floatOne, &floatTwo, &floatThree);
				}
			}
		}
		if(strcmpi(message, "waterfountain") == 0)
		{
			if(actorInstance->actorAnimation)
			{
				actorInstance->actorAnimation->waterspoutClock = 0.5f;
				actorInstance->actorAnimation->flags |= ANIMFLAG_WATERSPOUT;
			}
		}
		if(strcmpi(message, "watersplash") == 0)
		{
			if((actorInstance)&&(nodeFromMessage))
			{
				baGetNodesWorldPosition(&actorInstance->actorInstance, nodeFromMessage, pos);
				CreateWaterSplashes(pos);
			}
		}
	}	
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToSpecialMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

#ifndef CONSUMERDEMO

void RespondToMinecartMessages(void)
{
	MINECARTINFO		*info;
	ACTORINSTANCE		*actorInstance;
	ACTORINSTANCE		*tempInstance;
	char				parmStringBuf[256];
	char				*message;
	char				*equals;
	char				messageParmsBuf[256];
	char				*messageParms;
	int					temp;

	while (bkPopEvent(minecartEventClient, parmStringBuf) == OK)
	{
		bkPrintf("minecart event: %s.\n",parmStringBuf);

		message = DecodeMessage(parmStringBuf,(TBActorInstance**)&actorInstance, NULL);
		
		strcpy(messageParmsBuf, message);
		equals = FindChar(message, '=');
		if(equals) 	equals[0] = 0;
		messageParms = FindChar(messageParmsBuf, '=');
		messageParms++;

		info = actorInstance->mineCartInfo;

		// CMD: Junctions
		if(strcmpi(message, "default") == 0)
		{
			if(strcmpi(messageParms, "left") == 0)
			{
				info->defaultPath = MINECART_PATH_LEFT;
			}
			else
			if(strcmpi(messageParms, "middle") == 0)
			{
				info->defaultPath = MINECART_PATH_MIDDLE;
			}
			else
			if(strcmpi(messageParms, "right") == 0)
			{
				info->defaultPath = MINECART_PATH_RIGHT;
			}
		}
		if(strcmpi(message, "leftpath") == 0)
		{
			info->left = baFindAnimSegment(&actorInstance->actorInstance, messageParms);
		}
		if(strcmpi(message, "middlepath") == 0)
		{
			info->middle = baFindAnimSegment(&actorInstance->actorInstance, messageParms);
		}
		if(strcmpi(message, "rightpath") == 0)
		{
			info->right = baFindAnimSegment(&actorInstance->actorInstance, messageParms);
		}

		// CMD: boost
		if(strcmpi(message, "startgap") == 0)
		{
			switch(info->boostAnimState)
			{
			case MINECART_BOOST:
			case MINECART_JUMP:
				break;
			default:
				info->flags |= MINECART_GONNADROP;
				break;
			}
		}
		if(strcmpi(message, "endgap") == 0)
		{
			switch(info->boostAnimState)
			{
			case MINECART_BOOST:
			case MINECART_JUMP:
				break;
			default:
				StartMineCartDrop(actorInstance);
				break;
			}
		}
		if(strcmpi(message, "droppoint") == 0)
		{
			if(info->flags & MINECART_GONNADROP)
			{
				StartMineCartDrop(actorInstance);
			}
		}

		// CMD: jump
		if(strcmpi(message, "startjump") == 0)
		{
			if(info->boostAnimState != MINECART_JUMP)
			{
				StartMineCartTumble(actorInstance);
			}
		}
		if(strcmpi(message, "endjump") == 0)
		{
			if(info->boostAnimState != MINECART_JUMP)
			{
				StartMineCartTumble(actorInstance);
			}
		}

		// CMD: tilt at corners
		if(strcmpi(message, "tiltright") == 0)
		{
			info->tiltRebound = 1.0f;
		}
		if(strcmpi(message, "tiltleft") == 0)
		{
			info->tiltRebound = -1.0f;
		}
		if(strcmpi(message, "endtilt") == 0)
		{
			info->tiltRebound = 0.0f;
		}

		// CMD: tilt to avoid missing rails
		if(strcmpi(message, "missingrail") == 0)
		{
			if(strcmpi(messageParms, "left") == 0)
			{
				info->flags |= MINECART_LEFTMISSING;
				info->flags &= ~MINECART_RIGHTMISSING;
			}
			if(strcmpi(messageParms, "right") == 0)
			{
				info->flags |= MINECART_RIGHTMISSING;
				info->flags &= ~MINECART_LEFTMISSING;
			}
			if(strcmpi(messageParms, "end") == 0)
			{
				info->flags &= ~MINECART_LEFTMISSING;
				info->flags &= ~MINECART_RIGHTMISSING;
			}
		}

		// CMD: tilt to avoid obsticles
		if(strcmpi(message, "obstacle") == 0)
		{
			if(strcmpi(messageParms, "left") == 0)
			{
				info->flags |= MINECART_LEFTOBSTACLE;
				info->flags &= ~MINECART_RIGHTOBSTACLE;
			}
			if(strcmpi(messageParms, "right") == 0)
			{
				info->flags |= MINECART_RIGHTOBSTACLE;
				info->flags &= ~MINECART_LEFTOBSTACLE;
			}
			if(strcmpi(messageParms, "end") == 0)
			{
				info->flags &= ~MINECART_LEFTOBSTACLE;
				info->flags &= ~MINECART_RIGHTOBSTACLE;
			}
		}

		// CMD: land point
		if(strcmpi(message, "endpath") == 0)
		{
			info->landpointCRC = bkCRC32((uchar*)messageParms, strlen(messageParms), 0);
			info->flags |= MINECART_CESTTOUT;
		}
		// CMD: boing
		if(strcmpi(message, "boing") == 0)
		{
			if(actorInstance->mineCartInfo->characterOnCart)
			{
				FlushAnimationQueue(actorInstance->mineCartInfo->dummyInstance);
				PlayAnimationByName(actorInstance->mineCartInfo->dummyInstance, "boing", 1.0f, 0, 0, 0.1f, REACT);
	
				FlushAnimationQueue(actorInstance->mineCartInfo->characterOnCart);
	
				info->flags |= MINECART_BOING;
			}
		}
		if(strcmpi(message, "animate") == 0)
		{
			// CMD: find actor and play any animation that it has
			tempInstance = FindActorInstanceInInstanceList(messageParms, 0, &map.animatedInstances);
			if(tempInstance)
			{
				if(tempInstance->actorInstance.numAnimSegments)
				{
					baSetNodeFrame(&tempInstance->actorInstance, tempInstance->actorInstance.rootNodeInstance, 0, TRUE);
					baQueueNodeAnimNormal(&tempInstance->actorInstance, tempInstance->actorInstance.rootNodeInstance, 
								&tempInstance->actorInstance.animSegments[0], 1024, -1, 0, NULL,NULL);
				}
			}
		}
		// CMD: play jump land sound effect
		if(strcmpi(message, "land") == 0)
		{
			PlaySample("minecartland.wav", 255);
		}
	}	
}

#endif

/*	--------------------------------------------------------------------------------
	Function 	: RespondToRumbleMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToRumbleMessages(void)
{
	ACTORINSTANCE		*actorInstance;
	char				parmStringBuf[256];
	char				messageBuf[256];
	char				*message;
	char				*space;
	char				durStrBuf[256];
	char				*durStr;
	char				*ampStr;
	int					amplitude;
	float				duration;
	EWaveform			style;

	while (bkPopEvent(rumbleEventClient, parmStringBuf) == OK)
	{
		bkPrintf("rumble event: %s.\n",parmStringBuf);

		message = DecodeMessage(parmStringBuf,(TBActorInstance**)&actorInstance, NULL);
		
		strcpy(messageBuf, message);
		space = FindChar(messageBuf, ' ');
		if(space) space[0] = 0;

		strcpy(durStrBuf, ++(durStr = FindChar(message, ' ')));
		space = FindChar(durStrBuf, ' ');
		if(space) space[0] = 0;

		ampStr = FindChar(++(ampStr = FindChar(message, ' ')), ' ');
		ampStr++;

		sscanf(durStrBuf, "%f", &duration);
		sscanf(ampStr, "%d", &amplitude);

		if(strcmpi(messageBuf, "explosion") == 0)	style = WAVEFORM_EXPLOSION;
		if(strcmpi(messageBuf, "boom") == 0)		style = WAVEFORM_BOOM;
		if(strcmpi(messageBuf, "drill") == 0)		style = WAVEFORM_DRILL;
		if(strcmpi(messageBuf, "spin") == 0)		style = WAVEFORM_SPIN;

		if(actorInstance)
		{
			RequestPresetRumble(&controller1, amplitude, duration, style, actorInstance->instanceCRC, RUMBLEFLAG_TRACKING);
		}
		else
		{
			RequestPresetRumble(&controller1, amplitude, duration, style, 0, 0);
		}
	}	
}

/*	--------------------------------------------------------------------------------
	Function 	: RespondToPromptMessages
	Purpose 	: Respond to special messages generated
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void RespondToPromptMessages(void)
{
	ACTORINSTANCE		*actorInstance;
	char				parmStringBuf[256];
	char				messageBuf[256];
	char				*message;
	char				*equals;
	char				durStrBuf[256];
	char				*durStr;
	int					stringIdx;
	EPromptBookStyle	style;

	while (bkPopEvent(promptEventClient, parmStringBuf) == OK)
	{
		bkPrintf("prompt event: %s.\n",parmStringBuf);

		message = DecodeMessage(parmStringBuf,(TBActorInstance**)&actorInstance, NULL);
		
		strcpy(messageBuf, message);
		equals = FindChar(messageBuf, '=');
		if(equals) equals[0] = 0;

		if(durStr = FindChar(message, '='))
		{
			strcpy(durStrBuf, ++(durStr));
			sscanf(durStrBuf, "%d", &stringIdx);
		}

		style = PROMPT_NONE;
		if(strcmpi(messageBuf, "title") == 0)			style = PROMPT_TITLE;
		if(strcmpi(messageBuf, "direction") == 0)		style = PROMPT_DIRECTION;
		
		if(strcmpi(messageBuf, "message") == 0)
		{
			switch(stringIdx)
			{
			case 0:
				RequestMorronicMessage(MEVENT_HUTSPIN);
				break;
			case 1:
				RequestMorronicMessage(MEVENT_CORKSPIN);
				break;
			case 2:
				RequestMorronicMessage(MEVENT_BURP);
				break;
			}
		}
		if(strcmpi(messageBuf, "notmessage") == 0)
		{
			switch(stringIdx)
			{
			case 0:
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags |= MORRON_HUTPOSTER;
				break;
			case 1:
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags |= MORRON_CORKPOSTER;
				break;
			case 2:
				gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags |= MORRON_HONEYPOSTER;
				break;
			}
		}
		
		if(strcmpi(messageBuf, "kill") == 0)
		{
			KillPromptBook();
			return;
		}

		if(style != PROMPT_NONE)
		{
			// CMD: do not allow prompts when americanEventsBook Is open
			if((americanEvents)&&(americanEvents->localFlags & MORRON_BOOKOPEN)) return;
			
			// CMD: must be ok to open prompt
			CreatePromtBook(style, (EStringIndex)(stringIdx-2));
		}
	}	
}