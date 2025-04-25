// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : costumes.cpp
//   Purpose : Taz costume code
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "costumes.h"
#include "attach.h"
#include "maths.h"
#include "mapfile.h"
#include "scene.h"
#include "characters.h"
#include "MonitorFX.h"				// PP: for the watch display
#include "Cheats.h"					// PP: Cheats
#include "genericai.h"
#include "status.h"
#include "textures.h"
#include "animation.h"
#include "display.h"

TBTexture	*costumeSprites[5];
float		costumeClock, costumeSpitClock;

/*	--------------------------------------------------------------------------------
	Function 	: SetupLevelCostume
	Purpose 	: Dress an ACTORINSTANCE in the appropriate attire for the current level
	Parameters 	: ACTORINSTANCE to dress
	Returns 	: 
	Info 		: // PP: 
*/
void SetupLevelCostume(ACTORINSTANCE* const actorInstance)
{
	switch(map.sceneNumber)
	{
	case SCENE_GRANDCANYON:
		SetupCostume(actorInstance,COSTUME_COWBOY);
		break;
	case SCENE_MUSEUM:
		SetupCostume(actorInstance,COSTUME_NINJA);
		break;
	case SCENE_GHOSTTOWN:
		SetupCostume(actorInstance,COSTUME_WERETAZ);
		break;
	case SCENE_GOLDMINE:
		SetupCostume(actorInstance,COSTUME_INDY);
		break;
	case SCENE_CONSTRUCT:
		SetupCostume(actorInstance,COSTUME_SWAT);
		break;
	case SCENE_DEPTSTR:
		SetupCostume(actorInstance,COSTUME_RAPPA);
		break;
	case SCENE_ICEDOME:
		SetupCostume(actorInstance,COSTUME_SNOWBOARD);
		break;
	case SCENE_AQUA:
		SetupCostume(actorInstance,COSTUME_SURFER);
		break;
	case SCENE_ZOOHUB:
		SetupCostume(actorInstance,COSTUME_REINDEER);
		break;
	case SCENE_SAFARI:
		SetupCostume(actorInstance,COSTUME_SKATER);
		break;
	case SCENE_TAZHUB:
		SetupCostume(actorInstance,COSTUME_TARZAN);
		break;
	case SCENE_TAZPREBOSS:
		SetupCostume(actorInstance,COSTUME_TRIPPY);
		break;
	case SCENE_MUSEUMVEHICLERACE:
		SetupCostume(actorInstance, COSTUME_NINJA);
		break;
	case SCENE_GRANDCVEHICLERACE:
		SetupCostume(actorInstance, COSTUME_COWBOY);
		break;
	case SCENE_DEPTSTRVEHICLERACE:
		SetupCostume(actorInstance, COSTUME_RAPPA);
		break;
	case SCENE_DESTRUCTIONAQUA:
		SetupCostume(actorInstance, COSTUME_SURFER);
		break;
	case SCENE_DESTRUCTIONCONSTRUCT:
		SetupCostume(actorInstance, COSTUME_SWAT);
		break;
	case SCENE_DESTRUCTIONTASMANIA:
		SetupCostume(actorInstance, COSTUME_WERETAZ);
		break;
	case SCENE_ICEDOMERACE:
		SetupCostume(actorInstance, COSTUME_SNOWBOARD);
		break;
	case SCENE_GOLDMINERACE:
		SetupCostume(actorInstance, COSTUME_INDY);
		break;
	case SCENE_SAFARIRACE:
		SetupCostume(actorInstance, COSTUME_SKATER);
		break;
	}
}


/* --------------------------------------------------------------------------------
   Function : SetupCostume
   Purpose : Changes Taz into costume Taz by attaching new objects
   Parameters : ACTORINSTANCE, (opt/none)costume
   Returns : 
   Info : Must be called inside phone box and after exiting spin.
*/
int SetupCostume(ACTORINSTANCE *actorInstance,char costume)
{
	TBActorInstance		*attachedInstance;
	int					handle;				// PP: handle variable used in initialising the watch display
	
	switch(costume)
	{
	case COSTUME_NINJA:
		LoadActor("costume\\tazninja.obe",map.mapName);
		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazninja.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);

		LoadActor("costume\\ninjabelt.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\ninjabelt.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\bandana.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\bandana.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);// INHERIT SCALE

		LoadActor("costume\\ninjakatana.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\ninjakatana.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);// INHERIT SCALE
		break;
	case COSTUME_INDY:
		LoadActor("costume\\indyhat.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\indyhat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\indywhip.obe",map.mapName);
		TBVector position;
		SETVECTOR(position,-0.233f,0.f,0.f,1.0f);
		AttachObjectToCharacter(actorInstance,"costume\\indywhip.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);// INHERIT SCALE
		break;
	case COSTUME_TARZAN:
		LoadActor("costume\\taztarzan.obe",map.mapName);
		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\taztarzan.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		break;
	case COSTUME_COWBOY:
		LoadActor("costume\\tazstubblecowboy.obe",map.mapName);
		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazstubblecowboy.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);

		LoadActor("costume\\cowboybelt.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\cowboybelt.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\cowboyhat.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\cowboyhat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE);// INHERIT SCALE

		LoadActor("costume\\cowboymask.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\cowboymask.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);// INHERIT SCALE

		LoadActor("costume\\cowboymoustache.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\cowboymoustache.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);// INHERIT SCALE
		break;
	case COSTUME_CONSTRUCTION:
		LoadActor("costume\\constructionhat.obe",map.mapName);
		LoadActor("costume\\constructionbelt.obe",map.mapName);

		AttachObjectToCharacter(actorInstance,"costume\\constructionhat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\constructionbelt.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		break;
	case COSTUME_REINDEER:
		LoadActor("costume\\iceantlers.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\iceantlers.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);

		LoadActor("costume\\icerednose.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\icerednose.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		break;
	case COSTUME_EXPLORER:
		LoadActor("costume\\explorertaz.obe",map.mapName);
		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\explorertaz.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);

		LoadActor("costume\\explorermachete.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\explorermachete.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);

		LoadActor("costume\\explorerbackpack.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\explorerbackpack.obe","spinemid",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\explorerbelt.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\explorerbelt.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\explorerhat.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\explorerhat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\explorermonacle.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\explorermonacle.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		break;
	case COSTUME_SURFER:
		LoadActor("costume\\tazaquasurfer.obe",map.mapName);
		LoadActor("costume\\aquashades.obe",map.mapName);
		LoadActor("costume\\aquasurfboard.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazaquasurfer.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\aquashades.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		//if (map.sceneNumber == SCENE_AQUA)
			AttachObjectToCharacter(actorInstance,"costume\\aquasurfboard.obe","handright",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR);
		break;
	case COSTUME_RAPPA:
		LoadActor("costume\\tazrappa.obe",map.mapName);
		LoadActor("costume\\baseballcap.obe",map.mapName);
		LoadActor("costume\\dex.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazrappa.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\baseballcap.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		
		actorInstance->characterInfo->costumeHandle = AttachObjectToCharacter(actorInstance,"costume\\dex.obe","motion",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR);
		attachedInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->characterInfo->costumeHandle);
		// CMD: find speaker nodes, from which effect will originate
		if(attachedInstance)
		{
			if(actorInstance->characterInfo->rapperBlastInfo)
			{
				actorInstance->characterInfo->rapperBlastInfo->speakers[0] = baFindNode(attachedInstance->rootNodeInstance, "speakercase01");
				actorInstance->characterInfo->rapperBlastInfo->speakers[1] = baFindNode(attachedInstance->rootNodeInstance, "speakercase02");
				actorInstance->characterInfo->rapperBlastInfo->speakers[2] = baFindNode(attachedInstance->rootNodeInstance, "speakercase03");
				actorInstance->characterInfo->rapperBlastInfo->speakers[3] = baFindNode(attachedInstance->rootNodeInstance, "speakercase04");
			}
		}
		break;
	case COSTUME_WERETAZ:
		LoadActor("costume\\tazwerewolf.obe",map.mapName);
		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazwerewolf.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		break;
	case COSTUME_MINER:
		LoadActor("costume\\minerpickaxe.obe",map.mapName);
		SETVECTOR(position,0.127f,0.103f,0.088f,1.0f);
		AttachObjectToCharacter(actorInstance,"costume\\minerpickaxe.obe","root",position,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		
		LoadActor("costume\\minerbelt.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\minerbelt.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		LoadActor("costume\\minerhelmet.obe",map.mapName);
		AttachObjectToCharacter(actorInstance,"costume\\minerhelmet.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		break;
	case COSTUME_SNOWBOARD:
		LoadActor("costume\\tazsnowboarder.obe",map.mapName);
		LoadActor("costume\\snowboardhat.obe",map.mapName);
		LoadActor("costume\\snowboardgoggles.obe",map.mapName);
		LoadActor("costume\\snowboard.obe",map.mapName);
		LoadActor("costume\\snowball.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazsnowboarder.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\snowboardhat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\snowboardgoggles.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\snowboard.obe","spinemid",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\snowball.obe","root",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		break;
	case COSTUME_SWAT:
		LoadActor("costume\\swattaz.obe",map.mapName);
		LoadActor("costume\\swatcap.obe",map.mapName);
		LoadActor("costume\\swatbackpack.obe",map.mapName);
		LoadActor("costume\\swatshades.obe",map.mapName);
		LoadActor("costume\\swatelbowpadl.obe",map.mapName);
		LoadActor("costume\\swatelbowpadr.obe",map.mapName);
		LoadActor("costume\\swatcheesegun.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\swattaz.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\swatcap.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\swatbackpack.obe","spinemid",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		AttachObjectToCharacter(actorInstance,"costume\\swatshades.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\swatelbowpadl.obe","armforeleft",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		AttachObjectToCharacter(actorInstance,"costume\\swatelbowpadr.obe","armforeright",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);

		//if (map.sceneNumber == SCENE_CONSTRUCT)
			AttachObjectToCharacter(actorInstance,"costume\\swatcheesegun.obe","handleft",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		break;
	case COSTUME_SKATER:
		LoadActor("costume\\tazskater.obe",map.mapName);
		LoadActor("costume\\skaterelbowpadl.obe",map.mapName);
		LoadActor("costume\\skaterelbowpadr.obe",map.mapName);
		LoadActor("costume\\skaterskateboard.obe",map.mapName);
		LoadActor("costume\\skaterbaseballcap.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\tazskater.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\skaterelbowpadl.obe","armforeleft",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		AttachObjectToCharacter(actorInstance,"costume\\skaterelbowpadr.obe","armforeright",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		AttachObjectToCharacter(actorInstance,"costume\\skaterbaseballcap.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		AttachObjectToCharacter(actorInstance,"costume\\skaterskateboard.obe","spinemid",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);
		break;
	case COSTUME_TRIPPY:
		LoadActor("costume\\taztrippy.obe",map.mapName);
		LoadActor("costume\\trippyshades.obe",map.mapName);

		ChangeActorInstance(actorInstance,FindActorInActorList("costume\\taztrippy.obe"),5,FindActorInActorList("tazdevil.obe"),TRUE);
		AttachObjectToCharacter(actorInstance,"costume\\trippyshade.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);
		break;
	}

	if(CheatActive(CHEAT_DIGITALWATCH))
	{
		// PP: WATCH CAN ONLY BE WORN BY PLAYER 1!
		if(actorInstance == gameStatus.player[0].actorInstance)
		{
			// PP: make sure the watch isn't already set up
			if(MonitorFX_watch.actorInstance == NULL)
			{
				// PP: attach the watch model from the RESIDENT OBJECTS package
				handle=AttachObjectToCharacter(actorInstance,"agentwatch.obe","armforeleft",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE|ATTACHFLAG_NOBLACKOUTLINE);

				// PP: set up the watch display effect
				MonitorFX_watch.init(actorInstance, handle);
			}
		}
	}

	actorInstance->characterInfo->costume = costume;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : RemoveCostume
   Purpose : Remove any costume Taz is currently wearing
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int RemoveCostume(ACTORINSTANCE *actorInstance)
{
	if (actorInstance->characterInfo->costume==COSTUME_NONE) return FALSE;

	DestroyAttachedInfo(actorInstance);

	if(actorInstance->actorStatus->currentState != STATE_SPIN)
	{
		ChangeActorInstance(actorInstance,FindActorInActorList(actorInstance->actorName),5,NULL,TRUE);
	
		// PP: re-attach any costume accessories specified by the currently-active cheats.
		// PP: SetupCostume by default also sets costume to COSTUME_NONE
		SetupCostume(actorInstance);
	}
	else
	{
		actorInstance->characterInfo->costume = COSTUME_NONE;
	}

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int ElephantHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
{
	ACTORINSTANCE	*instance;
	float			range;
	int				i;

	if (debris->flags & DEBRISFLAG_SCALEUP)
	{
		// TP: hold ele in place until scaled up
		debris->velocity[Y] = 0.0f;
	}

	// TP: check it is a collision with a normal mesh
	for (i=0;i<MAX_COLL_MESHES;i++)
	{
		if (collInfo->mesh[i].Index==-1) return true;

		if (~collInfo->mesh[i].flags&RETFLAG_NORESPONCE)
		{
			// TP: if valid object to stick to then break
			break;
		}
	}

	if (i==MAX_COLL_MESHES) return true;

	debris->rotationSpeed[X] = (bmRand()%3)*PI;
	debris->rotationSpeed[Y] = (bmRand()%5)*PI;
	debris->rotationSpeed[Z] = 0.0f;
	debris->rotationSpeed[W] = 1.0f;

	camera[0].quakeOffsetLevel = 0.05f;
	camera[0].tiltOffsetLevel = RAD(2.0f);
	camera[0].flags |= CAMERA_QUAKEFADE;
	PlaySample("elebounce.wav",255);

	range = METERTOUNIT(3.0f);
	
	TBActor *parrot = FindActorInActorList("parrot.obe");
	instance = FindClosestEnemyToActor(debris->position,range,NULL,parrot);
	
	if ((!instance)||(range > METERTOUNIT(3.0f))) return true;

	AICharacterCollision(debris->actorStructure, instance);

	return true;
}

static bool eleActive = false;

/*  --------------------------------------------------------------------------------
	Function	: EleLifeOverCallback
	Purpose		: Called when the ele dies
	Parameters	: the debris structure containing the ele
	Returns		: 
	Info		: 
*/

static void EleLifeOverCallback(DEBRIS *debris)
{
	PlaySample("gumpopballoon.wav",255);
	eleActive = false;
}

/*  --------------------------------------------------------------------------------
	Function	: myPreCollCallback
	Purpose		: used to stop the elephant from really colliding with any characters
	Parameters	: the two collision objects
	Returns		: false if character collision, true if not
	Info		: 
*/

static int myPreCollCallback(ACTORINSTANCE *collider, ACTORINSTANCE *collider2)
{
	if ((collider->genericAI)||(collider2->genericAI)) return false;

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: DoTazanCostumeMove
	Purpose		: Called when the player summons the elephants
	Parameters	: ACTORINSTANCE for player
	Returns		: 
	Info		: 
*/

void DoTazanCostumeMove(ACTORINSTANCE *actorInstance)
{
	TBVector	tempVector;
	ACTORINSTANCE *instance;
	DEBRIS		*debris;
	TBVector	velocity, rotation;
	int			flag = DEBRISFLAG_PHYSICS|DEBRISFLAG_SCALEOUT|DEBRISFLAG_SCALEUP;
	float		range;
	
	// TP: if there is already an elephant active then you can yell but you'll get no elephant
	if (eleActive) return;

	LoadActor("costume\\elephant.obe",map.mapName);

	range = METERTOUNIT(10.0f);

	TBActor *parrot = FindActorInActorList("parrot.obe");
	instance = FindClosestEnemyToActor(actorInstance->actorInstance.position,range,NULL,parrot);
	
	if ((!instance)||(range > METERTOUNIT(10.0f))) return;

	bmVectorCopy(tempVector,instance->actorInstance.position);
	tempVector[Y] += METERTOUNIT(5.0f);
	tempVector[X] += METERTOUNIT(0.2f);

	bmVectorZero(velocity);
	bmVectorZero(rotation);

	debris = CreateDebrisActor("costume\\elephant.obe", tempVector, velocity, rotation,5.0f, 2.0f, METERTOUNIT(1.0f), METERTOUNIT(1.0f), 128, flag,0,0,RAD((float)(bmRand()%720)-360.0f));

	// TP: If we failed to conjure an elefun
	if (!debris) return;

	eleActive = true;

	bmVectorSet(debris->actorInstance->scale, 0.0f, 0.0f, 0.0f, 1.0f);
	baPrepActorInstance(debris->actorInstance,false);
	debris->actorStructure->actorBehaviour->physics.bounce = 1.0f;
	debris->actorStructure->actorBehaviour->collSphere.offset[Y] = 0.0f;
	debris->impactCallback = ElephantHitCallback;
	debris->lifeOverCallback = EleLifeOverCallback;

	CreateActorInstanceStatus(debris->actorStructure);
	debris->actorStructure->actorStatus->lastState = debris->actorStructure->actorStatus->currentState;
	debris->actorStructure->actorStatus->currentState = STATE_SMASH;

	debris->actorStructure->playerNo = actorInstance->playerNo;
	debris->actorStructure->flags |= ACTORFLAG_DEBRISACTOR;

	debris->actorStructure->actorStatus->costumeDebris = true;
	ENABLE_LINE_OR_CARTOON_RENDER_MODE(debris->actorInstance);

	PlaySample("elephant.wav",255);
}

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

LOSECOSTUMEINFO *CreateLoseCostumeEffect(ACTORINSTANCE *actorInstance, char *nodeName, float duration)
{
	LOSECOSTUMEINFO			*info;

	info = (LOSECOSTUMEINFO*)MALLOC(sizeof(LOSECOSTUMEINFO));
	ASSERTM(info, "unable to allocate memory for 'lose costume effect");

	if(info->node = baFindNode(actorInstance->actorInstance.rootNodeInstance, nodeName))
	{
		info->clock = LOSECOSTUME_GLOWBALLTIME;
		info->state = LOSECOSTUME_GLOW;
		info->size = 0.0f;
		info->alpha = 127.0f;
		info->sizeRateOfChange = LOSECOSTUME_GLOWBALLSIZE/LOSECOSTUME_GLOWBALLTIME;
		info->spellEffectInfo = NULL;

		// PP: I added this because the info's position vector wasn't ready when it came to draw the effect for the first time.
		// PP: This was causing a crash because it was trying to draw a sprite at a position whose W value was 0.0f.
		UpdateLoseCostumeEffect(actorInstance, info);

		return info;
	}

	SAFE_FREE(info);
	return NULL;
}

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

LOSECOSTUMEINFO *UpdateLoseCostumeEffect(ACTORINSTANCE *actorInstance, LOSECOSTUMEINFO *info)
{
	TBVector	offset, nodePos;

	switch(info->state)
	{
	case LOSECOSTUME_GLOW:
		baPrepActorInstance(&actorInstance->actorInstance, TRUE);
		baGetNodesWorldPosition(&actorInstance->actorInstance, info->node, info->position);
		if((info->size += info->sizeRateOfChange*fTime) > LOSECOSTUME_GLOWBALLSIZE)
		{
			info->size = LOSECOSTUME_GLOWBALLSIZE;
			info->spellEffectInfo = CreateVoodooSpellEffect(actorInstance, "spinemid", 1.5f, METERTOUNIT(3.0f));
			info->state = LOSECOSTUME_BEAMS;
		}
		break;
	case LOSECOSTUME_BEAMS:
		baGetNodesWorldPosition(&actorInstance->actorInstance, info->node, info->position);
		if((info->spellEffectInfo = UpdateVoodooSpellEffect(info->spellEffectInfo)) == NULL)
		{
			info->alpha = 255.0f;
			info->size = LOSECOSTUME_WHITEOUTSIZE;
//			info->state = LOSECOSTUME_WHITEOUT;
			// CMD: don't go to the nuclear winter that is whiteout

			actorInstance->fadeSpeed = -128.0f;
			actorInstance->flags |= ACTORFLAG_DELETEAFTERFADEOUT;

			// CMD: only if not going to whiteout
			SAFE_FREE(info);
			return NULL;
		}
		break;
	case LOSECOSTUME_WHITEOUT:
		// CMD: calculate position for sprite
		baGetNodesWorldPosition(&actorInstance->actorInstance, info->node, nodePos);
		bmVectorSub(offset, nodePos, gameStatus.player[0].camera->cameraState.cameraPos);
		bmVectorScaleToLength(offset, offset, METERTOUNIT(2.0f));
		bmVectorAdd(info->position, offset, gameStatus.player[0].camera->cameraState.cameraPos);
		if((info->alpha -= 127.0f*fTime) < 0.0f)
		{
			info->alpha = 0.0f;
			SAFE_FREE(info);
			return NULL;
		}
		break;
	}
	return info;
}

/*  --------------------------------------------------------------------------------
	Function	: ElephantHitCallback
	Purpose		: Called when the falling elephant hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/

void DrawLoseCostumeEffect(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE		*ptr;
	LOSECOSTUMEINFO		*info;
	COLOUR				white, orig;

	for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		// PP: I was getting a null pointer crash here, just as scene loading began
		if(ptr == NULL)
		{
			break;
		}

		if((ptr->actorStatus)&&(info = ptr->actorStatus->costumeChangeInfo))
		{
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			switch(info->state)
			{
			case LOSECOSTUME_WHITEOUT:
//				if((map.fogColour.r == 0)&&(map.fogColour.g == 0)&&(map.fogColour.b == 0))
//				{
					orig.r = map.fogColour.r; orig.g = map.fogColour.g; orig.b = map.fogColour.b; orig.a = map.fogColour.a;
					white.r = 246; white.g = 123; white.b = 2; white.a = 255;
					SetFog(map.fogMin, map.fogMax, white);
//				}
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				break;
			default:
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
				bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
				break;
			}
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	
			bdSetIdentityObjectMatrix();
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		
			bdDrawFlatSprite(info->position, info->size, info->size, 0.0f, textureLoseCostumeGlow, 0, 0, 127, 127, 127, bmFloatToInt(info->alpha), NULL, METERTOUNIT(-2.5f));
	
			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			if(map.fogState)
			{
				if(info->state == LOSECOSTUME_WHITEOUT)
				{
					SetFog(map.fogMin, map.fogMax, orig);
				}
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
			}
			else
			{
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
			}
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

			if(ptr->actorStatus->costumeChangeInfo->spellEffectInfo)
			{
				DrawVoodooSpellEffect(ptr->actorStatus->costumeChangeInfo->spellEffectInfo);
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: GetNaked
	Purpose		: For when Taz wants to undress
	Parameters	: 
	Returns		: 
	Info		: 
*/

int GetNaked(ACTORINSTANCE *actorInstance, EScene scene)
{
	TBVector	position, velocity, rotationSpeed;
	int			i, num;

	bmVectorAdd(position, actorInstance->actorInstance.position, actorInstance->actorBehaviour->collSphere.offset);

	if((costumeSpitClock -= fTime) < 0.0f)
	{
		costumeSpitClock = LOSECOSTUME_SPITTIME;

		for (i = 3;i >= 0;--i)
		{
			num = bmRand()%5;
	
			if(costumeSprites[num])
			{
				bmVectorSet(velocity, METERTOUNIT(UtilGenerateRandomNumber(2.0f, -2.0f)), METERTOUNIT(UtilGenerateRandomNumber(7.0f, 4.0f)), METERTOUNIT(UtilGenerateRandomNumber(2.0f, -2.0f)), 1.0f);
				bmVectorSet(rotationSpeed, 0.0f, 0.0f, 0.0f, 1.0f);
		
				CreateDebrisActor(costumeSprites[num], position, velocity, rotationSpeed, UtilGenerateRandomNumber(2.5f, 1.5f), 
									RAD(UtilGenerateRandomNumber(360.0f, 0.0f)), METERTOUNIT(0.2f), METERTOUNIT(0.2f), 
									128, DEBRISFLAG_GRAVITY|DEBRISFLAG_ROTATESPRITE, 0, 0, RAD(UtilGenerateRandomNumber(360.0f, -360.0f)));
			}
		}
	}
	if((costumeClock -= fTime) < 0.0f)
	{
		costumeClock = LOSECOSTUME_TOTALTIME;
		return TRUE;
	}
	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: GetNaked
	Purpose		: For when Taz wants to undress
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitialiseNakedness(EScene scene)
{
	costumeClock = LOSECOSTUME_TOTALTIME;
	costumeSpitClock = LOSECOSTUME_SPITTIME;

	switch(scene)
	{
	case SCENE_ZOOHUB:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\rednose.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\snowbit01.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\snowbit02.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\snowbit03.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		return;
	case SCENE_ICEDOME:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\beanie.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\skigogs.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\snowbit03.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\snowjoke.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		return;
	case SCENE_SAFARI:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\larmpad.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\rarmpad.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\thawk.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\yellacap.bmp", 0);
		return;
	case SCENE_AQUA:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\aquatrunks.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\fishyellow.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\goldnugget03.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\raybanspecs.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\starfish.bmp", 0);
		return;
	case SCENE_DEPTSTR:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\basketball.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\blaccap.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\jordan.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\sneaker.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		return;
	case SCENE_MUSEUM:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\bandana.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\ninjajacket.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\ninjapants.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\shurikan.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		return;
	case SCENE_CONSTRUCT:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\barmpad.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\capswat.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\cheese.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\swatbackpack.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\swatshades.bmp", 0);
		return;
	case SCENE_GHOSTTOWN:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\eyeball.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\othereyeball.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\toothy.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\werepants.bmp", 0);
		return;
	case SCENE_GOLDMINE:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\adder.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\diamond.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\indyhat.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\necklace.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\spottypants.bmp", 0);
		return;
	case SCENE_GRANDCANYON:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\banditohat.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\banditomask.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\bullet.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\cowbelt.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\goldnugget03.bmp", 0);
		return;
	case SCENE_TAZHUB:
		costumeSprites[0] = bkLoadTexture(NULL, "costume\\caterpillar.bmp", 0);
		costumeSprites[1] = bkLoadTexture(NULL, "costume\\leafgreen.bmp", 0);
		costumeSprites[2] = bkLoadTexture(NULL, "costume\\leaftree.bmp", 0);
		costumeSprites[3] = bkLoadTexture(NULL, "costume\\tarzanbra.bmp", 0);
		costumeSprites[4] = bkLoadTexture(NULL, "costume\\tarzanpants.bmp", 0);
		return;
	default:
		for(int i = 4;i >= 0;--i)
		{
			costumeSprites[i] = NULL;
		}
		return;
	}
}