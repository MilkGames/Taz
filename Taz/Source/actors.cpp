// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : actors.cpp
//   Purpose : provides functions to aid in the use of actors
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"


#include "global.h"

#include "actors.h"
#include "physics.h"
#include "animation.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "status.h"
#include "paths.h"
#include "genericai.h"
#include "camera.h"
#include "control.h"
#include "maths.h"
#include "characters.h"
#include "shadow.h"
#include "scene.h"
#include "trigger.h"
#include "destructibles.h"
#include "water.h"
#include "projectile.h"
#include "springyobjects.h"
#include "mailbox.h"
#include "wantedposter.h"
#include "fallingobjects.h"
#include "main.h"
#include "lights.h"
#include "sparks.h"
#include "files.h"
#include "transporter.h"
#include "trail.h"
#include "laser.h"
#include "lightning.h"
#include "phonebox.h"
#include "phoneboxeffect.h"
#include "MonitorFX.h"							// PP: computer monitor effects
#include "flocking.h"
#include "vehicles.h"							// PP: generic vehicle functions
#include "shatter.h"
#include "display.h"
#include "haar.h"
#include "VideoFX.h"							// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "death.h"
#include "doors.h"
#include "musicandsavagebeasts.h"
#include "redshield.h"
#include "halo.h"
#include "speedboost.h"
#include "cutscene.h"							// PP: provides functions to initialise,play and free cutscenes
#include "tartantaz.h"
#include "transportereffect.h"
#include "musicandsavagebeasts.h"
#include "Cheats.h"								// PP: Cheats
#include "gui.h"

ACTORLIST		actorList;					// list of all loaded actors

char *specialTypeNames[] = 
{
	"PROJECTILE",
	"PROJECTILESLIDE",
	"FALLINGOBJECT",
	"DIVINGBOARD",
	"TRAMAMPOLINE",
	"STEPPINGSTONE",
	"TILTINGOBJECT",
	"SEESAWOBJECT",
	"SWINGINGOBJECT",
	"WATERYOBJECT",
	"DESTRUCTIBLE",
	"MAILBOX",
	"WANTEDPOSTER",
	"PNEUMATICPLUNGER",
	"PATH",
	"PLATFORM",
	"RESETPOINT",
	"TRIGGER",
	"HIGHVOLTAGE",
	"EFFECT",
	"CAMERACASE",
	"PRECIPITATION",
	"ELECTRICITY",
	"ZOOKEEPER",
	"FIRE",
	"TRANSPORTER",
	"ICE",
	"SPEEDBOOST",
	"SLIDE",
	"RANDOMFOOD",
	"ESCALATOR",
	"VEHICLE",
	"DOOR",
	"SCENIC",
	"PHONEBOX",
	"WATERFALL",
	"LASER",
	"FOUNTAIN",
	"SHATTER",
	"TARBUBBLE",
	"ENEMY",
	"LIGHTNING",
	"CHECKPOINT",
	"BUMPER",
	"MONITOR",
	"TARGET",
	"SAFEPOINT",
	"MAGNET",
	"FLOCKING",
	"BUZZARD",
	"FECAMERA",
	"MAPPOINT",
	"MIRROR",
	"LAVA",
	"SPLAT",
	"DEATH",
	"HAAR",
	"MUSICBOX",
	"MINECART",
	"MINECARTDUMMY",
	"PORTALWORLD",
	"FLAMETHROWA",
	"SECURITYBOX",
	"MECHATWEETY",
	"TAZBOSSITEM",
	"CLIPPLANE",
	"ROCKET",
	"XDOOR",
	"HALO",
	"ZAPPER",
	"SPAWNCRATE",
	"POLICEBOX",
	"EGG",
	"AIRCON",
	"TUMBLEWEED",
	"ARROW",
	"MIRRORBALL",
	"WATERRIPPLE",
#if BPLATFORM==GAMECUBE
	"REVERB",
#endif
};

float	standardLineThickness = 1.8f;

/* --------------------------------------------------------------------------------
   Function : InitActorList
   Purpose : Intialise actor list
   Parameters : 
   Returns : 
   Info : Initialise list for loaded actors
*/
void InitActorList(void)
{
	actorList.numEntries = 0;
	actorList.head.next = actorList.head.prev = &actorList.head;

}

/* --------------------------------------------------------------------------------
   Function : AddActorToActorList
   Purpose : Add a new actor to the actor list
   Parameters : TBActor, identifierName
   Returns : 
   Info : Add an actor to the actorlist and associates and identifier name to it for later use
*/
void AddActorToActorList(TBActor *actor,char *name,uint32 type)
{
	ACTORLISTENTRY *ptr;
	ACTORLISTENTRY *actorListEntry;

	ptr = actorList.head.next;

	actorListEntry = (ACTORLISTENTRY*)ZALLOC(sizeof(ACTORLISTENTRY));
	ASSERT(actorListEntry);

	actorListEntry->actor = actor;
	if (strlen(name) < ACTORLISTNAMELENGTH)
	{
		strcpy(actorListEntry->name,name);
	}
	else
	{
		STRNCPY(actorListEntry->name,name,ACTORLISTNAMELENGTH);
	}
	actorListEntry->crc = bkCRC32((uchar*)name,strlen(name),0);
	actorListEntry->type = type;

	actorListEntry->next = ptr;
	actorListEntry->prev = ptr->prev;
	ptr->prev->next = actorListEntry;
	ptr->prev = actorListEntry;
	actorList.numEntries++;
}

/* --------------------------------------------------------------------------------
   Function : AddActorToActorListByCRC
   Purpose : Add a new actor to the actor list
   Parameters : TBActor, identifierName
   Returns : 
   Info : Add an actor to the actorlist and associates and identifier name to it for later use
*/
void AddActorToActorListByCRC(TBActor *actor,uint32 crc,uint32 type)
{
	ACTORLISTENTRY *ptr;
	ACTORLISTENTRY *actorListEntry;

	ptr = actorList.head.next;

	actorListEntry = (ACTORLISTENTRY*)ZALLOC(sizeof(ACTORLISTENTRY));
	ASSERT(actorListEntry);

	actorListEntry->actor = actor;
	actorListEntry->crc = crc;
	actorListEntry->type = type;
	sprintf(actorListEntry->name,"%X",crc);

	actorListEntry->next = ptr;
	actorListEntry->prev = ptr->prev;
	ptr->prev->next = actorListEntry;
	ptr->prev = actorListEntry;
	actorList.numEntries++;
}

/* --------------------------------------------------------------------------------
   Function : SubActorFromActorList
   Purpose : Removes and actor from the actor list
   Parameters : identifierName
   Returns : pointer the the TBActor that has been removed from the list or NULL for could'nt find actor
   Info : Remove and actor from the actor list
*/
TBActor *SubActorFromActorList(char *name)
{
	ACTORLISTENTRY  *ptr;
	uint32			crc;
	TBActor			*tempActor;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	return SubActorFromActorList(crc);
}

/* --------------------------------------------------------------------------------
   Function : SubActorFromActorList
   Purpose : Removes and actor from the actor list
   Parameters : identifierName
   Returns : pointer the the TBActor that has been removed from the list or NULL for could'nt find actor
   Info : Remove and actor from the actor list
*/
TBActor *SubActorFromActorList(uint32 crc)
{
	ACTORLISTENTRY  *ptr;
	TBActor			*tempActor;

	for (ptr = actorList.head.next;ptr != &actorList.head;ptr = ptr->next)
	{
		if (ptr->crc == crc)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			actorList.numEntries--;
			tempActor = ptr->actor;
			SAFE_FREE(ptr);
			return tempActor;
		}
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : LoadActor
   Purpose : Loads an actor from a package file and adds it to the actorList
   Parameters : file name, package, package index or NULL
   Returns : TRUE, FALSE
   Info : 
*/
int LoadActor(char *actorName, char *packageName,int flags)
{
	TBPackageIndex	*pakIndex;
	TBActor			*actor;

	if (FindActorInActorList(actorName)!=NULL)
	{
		bkPrintf("Actor %s already loaded.\n",actorName);
		return TRUE;
	}

	if (pakIndex = FindPackage(packageName))
	{
		bkPrintf("LoadActor(%s,%s) - %d\n",actorName,packageName,bkHeapFreeSpace(NULL));

		actor = bkLoadActor(pakIndex,actorName,0);

		if (!actor)
		{
			bkPrintf("*** WARNING *** Actor %s not found\n", actorName);
		}

		if (actor)
		{
			AddActorToActorList(actor,actorName,ACTORTYPE_ACTOR|flags);
		}
		else
		{
			bkPrintf("*** ERROR *** Could not load actor\n");
			return FALSE;
		}
		return TRUE;
	}
	else
	{
/*		bkPrintf("LoadActor(%s,%s)\n",actorName,packageName);

		if ((pakIndex = bkOpenPackage(packageName)))
		{
			actor = bkLoadActor(pakIndex, actorName,0);
			bkClosePackage(pakIndex);
			if (actor)
			{
				AddActorToActorList(actor,actorName,ACTORTYPE_ACTOR|ACTOR_MALLOCED|flags);
				return TRUE;
			}
			else
			{
				bkPrintf("*** ERROR *** Could not load actor\n");
				return FALSE;
			}
		}
		else*/
		{
			bkPrintf("*** ERROR *** Package %s not loaded\n",packageName);
			return FALSE;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : LoadActor
   Purpose : Loads an actor from a package file and adds it to the actorList
   Parameters : file name, package, package index or NULL
   Returns : TRUE, FALSE
   Info : 
*/
int LoadActor(char *actorName, char *packageName, TBPackageIndex *pakIndex2)
{
	TBActor			*actor;

	if(actor = FindActorInActorList(actorName)) return TRUE;

	bkPrintf("LoadActor(%s,%s) - %d\n",actorName,packageName,bkHeapFreeSpace(NULL));

	if (pakIndex2 = FindPackage(packageName))
	{
		actor = bkLoadActor(pakIndex2,actorName,0);

		if (!actor)
		{
			bkPrintf("*** WARNING *** Actor %s not found\n", actorName);
		}

		if(actor)
		{
			AddActorToActorList(actor,actorName,ACTORTYPE_ACTOR);
			return TRUE;
		}
	}
	else
	{
		// try and load from memory
/*		if (pakIndex2)
		{
			if ((actor = bkLoadActor(pakIndex2,actorName,0)))
			{
				AddActorToActorList(actor,actorName,ACTORTYPE_ACTOR);
				return TRUE;
			}
		}

		if ((pakIndex = bkOpenPackage(packageName)))
		{
			actor = bkLoadActor(pakIndex, actorName,0);
			bkClosePackage(pakIndex);
			if (actor)
			{
				AddActorToActorList(actor,actorName,ACTORTYPE_ACTOR|ACTOR_MALLOCED);
				return TRUE;
			}
			else
			{
				bkPrintf("*** ERROR *** Could not load actor\n");
				return FALSE;
			}
		}
		else*/
		{
			bkPrintf("*** ERROR *** Package %s not loaded\n",packageName);
			return FALSE;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : LoadSet
   Purpose : Loads an set from a package file and adds it to the actorList
   Parameters : file name, package
   Returns : TRUE, FALSE
   Info : 
*/
int LoadSet(char *actorName, char *packageName)
{
	TBPackageIndex	*pakIndex;
	TBSet			*set;

	bkPrintf("LoadSet(%s,%s)\n",actorName,packageName);

	if (pakIndex = FindPackage(packageName))
	{
		set = bkLoadSet(pakIndex, actorName,0);
		if (set)
		{
			AddActorToActorList((TBActor*)(set),actorName,ACTORTYPE_SET);
			return TRUE;
		}
		else
		{
			bkPrintf("*** ERROR *** Could not load set\n");
			return FALSE;
		}
	}
	else
	{
		if ((pakIndex = bkOpenPackage(packageName)))
		{
			set = bkLoadSet(pakIndex, actorName,0);
			bkClosePackage(pakIndex);
			if (set)
			{
				AddActorToActorList((TBActor*)(set),actorName,ACTORTYPE_SET);
				return TRUE;
			}
			else
			{
				bkPrintf("*** ERROR *** Could not load set\n");
				return FALSE;
			}
		}
		else
		{
			bkPrintf("*** ERROR *** Could not open set\n");
			return FALSE;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeActor
   Purpose : Removes an actor from the actor list and free the memory allocated completely
   Parameters : file name
   Returns : TRUE, FALSE
   Info : 
*/
int FreeActor(char *actorName)
{
	TBActor *actor;

	bkPrintf("FreeActor(%s)\n",actorName);

	actor = SubActorFromActorList(actorName);
	if (actor)
	{
		bkDeleteActor(actor);
		return TRUE;
	}

	bkPrintf("*** WARNING *** Could not free actor\n");
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeActor
   Purpose : Removes an actor from the actor list and free the memory allocated completely
   Parameters : file name
   Returns : TRUE, FALSE
   Info : 
*/
int FreeActor(uint32 crc)
{
	TBActor *actor;

	bkPrintf("FreeActor(%X)\n",crc);

	actor = SubActorFromActorList(crc);
	if (actor)
	{
		bkDeleteActor(actor);
		return TRUE;
	}

	bkPrintf("*** WARNING *** Could not free actor\n");
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeAllActors
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllActors(void)
{
	TBActor			*tempActor;
	ACTORLISTENTRY  *ptr,*temp;

	bkPrintf("FreeAllActors\n");

	for (ptr = actorList.head.next;ptr != &actorList.head;)
	{
		if (!(ptr->type &	ACTOR_PERMANENT))
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			actorList.numEntries--;
			tempActor = ptr->actor;
			if (ptr->type&ACTOR_MALLOCED)
			{
				bkDeleteActor(tempActor);
			}
			temp = ptr->next;
			SAFE_FREE(ptr);
			ptr = temp;
		}
		else
		{
			temp = ptr->next;
			ptr = temp;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeAllPermanentActors
   Purpose : Removes all actors from the actor list and free the memory allocated completely
   Parameters : 
   Returns : 
   Info : 
*/
void FreeAllPermanentActors(void)
{
	TBActor			*tempActor;
	ACTORLISTENTRY  *ptr,*temp;

	bkPrintf("FreeAllActors\n");

	for (ptr = actorList.head.next;ptr != &actorList.head;)
	{
		if ((ptr->type&ACTOR_PERMANENT))
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			actorList.numEntries--;
			tempActor = ptr->actor;
			temp = ptr->next;
			SAFE_FREE(ptr);
			ptr = temp;
		}
		else
		{
			temp = ptr->next;
			ptr = temp;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FindActorInstanceInInstanceList
   Purpose : Locates an actor instance in by name
   Parameters : instance name, or crc, instance list
   Returns : ACTORINSTANCE or NULL
   Info : 
*/
ACTORINSTANCE *FindActorInstanceInInstanceList(char *name, uint32 crc, ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;
	uint32	instanceCRC;

	if (name)
	{
		instanceCRC = bkCRC32((uchar*)(name),strlen(name),0);
	}
	else
	{
		instanceCRC = crc;
	}

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->instanceCRC == instanceCRC)
		{
			return ptr;
		}
	}

	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : FindActorInActorList
   Purpose : Locates an actor in the actorList by name
   Parameters : actor name
   Returns : TBActor, or NULL
   Info : 
*/
TBActor *FindActorInActorList(char *name)
{
	ACTORLISTENTRY  *ptr;
	uint32			crc;

	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = actorList.head.next;ptr != &actorList.head;ptr = ptr->next)
	{
		if (ptr->crc == crc)
		{
			return ptr->actor;
		}
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : FindActorInActorList
   Purpose : Locates an actor in the actorList by name
   Parameters : actor name
   Returns : TBActor, or NULL
   Info : 
*/
TBActor *FindActorInActorList(uint32 crc)
{
	ACTORLISTENTRY  *ptr;
//	uint32			crc;

//	crc = bkCRC32((uchar *)name,strlen(name),0);

	for (ptr = actorList.head.next;ptr != &actorList.head;ptr = ptr->next)
	{
		if (ptr->crc == crc)
		{
			return ptr->actor;
		}
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : InitActorInstanceList
   Purpose : Intialise actor instance list
   Parameters : ACTORINSTANCELIST
   Returns : 
   Info : Initialise list for actor instances
*/
void InitActorInstanceList(ACTORINSTANCELIST *actorInstanceList)
{
	actorInstanceList->numEntries = 0;
	actorInstanceList->flags = ACTORINSTLISTFLAG_ANIMATED;
	actorInstanceList->head.next = actorInstanceList->head.prev = &actorInstanceList->head;
}

/* --------------------------------------------------------------------------------
   Function : SetInstanceListParms
   Purpose : Set the instance list flags
   Parameters : ACTORINSTANCELIST, int flags
   Returns : 
   Info : 
*/
void SetInstanceListParms(ACTORINSTANCELIST *actorInstanceList,int flags)
{
	actorInstanceList->flags = flags;
}

/* --------------------------------------------------------------------------------
   Function : InitActorStructures
   Purpose : Intialise actor instance structures
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void InitActorStructures(ACTORINSTANCE *actorInstance)
{
	actorInstance->actorBehaviour = NULL;
	actorInstance->actorAnimation = NULL;
	actorInstance->actorStatus = NULL;
	actorInstance->characterInfo = NULL;
	actorInstance->actorShadow = NULL;
	actorInstance->RespondToCollisionRoutine = NULL;
	actorInstance->PreRespondToCollisionRoutine = NULL;
	actorInstance->outsideBoundingBox = NULL;
	actorInstance->instanceList = NULL;
	actorInstance->pathInfo = NULL;
	actorInstance->specialType = NULL;
	actorInstance->genericAI = NULL;
	actorInstance->special = (ESpecialType)-1;
	actorInstance->flags = ACTORFLAG_HASANIMATED;
	actorInstance->currentAlpha = actorInstance->alpha = 256;
	actorInstance->fadeSpeed = 0.0f;
	actorInstance->lineThickness = 3.5f;
	actorInstance->playerNo = -1;
	actorInstance->onScreen = TRUE;
	actorInstance->soundHandle = -1;
	actorInstance->soundNodeInstance = NULL;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstance
   Purpose : Creates,initialises and adds an actor instance to an actor instancelist
   Parameters : actorInstanceList, actor name, instance name
   Returns : pointer to the actor instance entry created.
   Info : 
*/
ACTORINSTANCE *CreateActorInstance(ACTORINSTANCELIST *actorInstanceList,char *actorName,char *instanceName,TBVector position, TBVector scale, TBQuaternion orientation, short flags)
{
	ACTORINSTANCE *actorInstanceEntry;
	TBActor	*actor;
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	actor = FindActorInActorList(actorName);
	if (actor)
	{
		actorInstanceEntry = (ACTORINSTANCE*)ZALLOC(sizeof(ACTORINSTANCE));
		if (!actorInstanceEntry)
		{
			bkPrintf("*** ERROR *** Could not allocate memory for instance\n");
			return NULL;
		}
		baInitActorInstance(&actorInstanceEntry->actorInstance, actor,5);
		if (position) bmVectorCopy(actorInstanceEntry->actorInstance.position, position);
		if (scale) bmVectorCopy(actorInstanceEntry->actorInstance.scale, scale);
		if (orientation) bmQuatCopy(actorInstanceEntry->actorInstance.orientation, orientation);
		baPrepActorInstance(&actorInstanceEntry->actorInstance,TRUE);
		baSetActorRenderMode(&actorInstanceEntry->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		baSetActorRenderMode(&actorInstanceEntry->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
		baSetNodeFrame(&actorInstanceEntry->actorInstance, actorInstanceEntry->actorInstance.rootNodeInstance, 0, TRUE);
		baFlushNodeQueue(&actorInstanceEntry->actorInstance, actorInstanceEntry->actorInstance.rootNodeInstance,TRUE);

		if (strlen(actorName)<32)
		{
			strcpy(actorInstanceEntry->actorName,actorName);
		}
		else
		{
			STRNCPY(actorInstanceEntry->actorName,actorName,32);
		}
		if (strlen(instanceName)<32)
		{
			strcpy(actorInstanceEntry->instanceName,instanceName);
		}
		else
		{
			STRNCPY(actorInstanceEntry->instanceName,instanceName,32);
		}
		actorInstanceEntry->actorCRC = bkCRC32((uchar*)actorName,strlen(actorName),0);
		actorInstanceEntry->instanceCRC = bkCRC32((uchar*)instanceName,strlen(instanceName),0);
		
		InitActorStructures(actorInstanceEntry);
		actorInstanceEntry->flags |= flags;
		actorInstanceEntry->instanceList = actorInstanceList;

		// add new instance to the specified actorInstanceList
		
		actorInstanceEntry->next = ptr;
		actorInstanceEntry->prev = ptr->prev;
		ptr->prev->next = actorInstanceEntry;
		ptr->prev = actorInstanceEntry;
		actorInstanceList->numEntries++;

		return actorInstanceEntry;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : AddInstanceToInstanceList
   Purpose : Add an actorInstance to the instanceList
   Parameters : actorInstance, actorInstanceList
   Returns : 
   Info : 
*/
void AddInstanceToInstanceList(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;
	actorInstance->next = ptr;
	actorInstance->prev = ptr->prev;
	ptr->prev->next = actorInstance;
	ptr->prev = actorInstance;
	actorInstanceList->numEntries++;
	actorInstance->instanceList = actorInstanceList;
}

/* --------------------------------------------------------------------------------
   Function : RemoveInstanceFromInstanceList
   Purpose : Remove an instance from the instance list
   Parameters : actorInstance, actorInstanceList
   Returns : 
   Info : 
*/
void RemoveInstanceFromInstanceList(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *actorInstanceList)
{
	actorInstance->prev->next = actorInstance->next;
	actorInstance->next->prev = actorInstance->prev;
	actorInstanceList->numEntries--;
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceFromActorPtr
   Purpose : Creates,initialises and adds an actor instance to an actor instancelist
   Parameters : actorInstanceList, actor name, instance name
   Returns : pointer to the actor instance entry created.
   Info : 
*/
ACTORINSTANCE *CreateActorInstanceFromActorPtr(ACTORINSTANCELIST *actorInstanceList,TBActor *actorPtr,uint32 actorCrc,char *instanceName)
{
	ACTORINSTANCE *actorInstanceEntry;
	//TBActor	*actor;
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

//	bkPrintf("CreateActorInstance( ,%s,%s)\n",actorName,instanceName);

	if (actorPtr)
	{
		actorInstanceEntry = (ACTORINSTANCE*)ZALLOC(sizeof(ACTORINSTANCE));
		if (!actorInstanceEntry)
		{
			bkPrintf("*** ERROR *** Could not allocate memory for instance\n");
			return NULL;
		}

		baInitActorInstance(&actorInstanceEntry->actorInstance, actorPtr,5);
		baPrepActorInstance(&actorInstanceEntry->actorInstance,TRUE);
		baSetActorRenderMode(&actorInstanceEntry->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		baSetActorRenderMode(&actorInstanceEntry->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
		baSetNodeFrame(&actorInstanceEntry->actorInstance, actorInstanceEntry->actorInstance.rootNodeInstance, 0, TRUE);
		baFlushNodeQueue(&actorInstanceEntry->actorInstance, actorInstanceEntry->actorInstance.rootNodeInstance, TRUE);

		if (strlen(instanceName)<32)
		{
			strcpy(actorInstanceEntry->instanceName,instanceName);
		}
		else
		{
			STRNCPY(actorInstanceEntry->instanceName,instanceName,32);
		}
		actorInstanceEntry->actorCRC = actorCrc;
		actorInstanceEntry->actorName[0] = 0;
		actorInstanceEntry->instanceCRC = bkCRC32((uchar*)instanceName,strlen(instanceName),0);
		
		InitActorStructures(actorInstanceEntry);
		actorInstanceEntry->instanceList = actorInstanceList;

		// add new instance to the specified actorInstanceList
		actorInstanceEntry->next = ptr;
		actorInstanceEntry->prev = ptr->prev;
		ptr->prev->next = actorInstanceEntry;
		ptr->prev = actorInstanceEntry;
		actorInstanceList->numEntries++;

		return actorInstanceEntry;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance\n");
		return NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceBehaviour
   Purpose : Creates and initialises the physic part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : TRUE, FALSE
*/
int CreateActorInstanceBehaviour(ACTORINSTANCE *actorInstance)
{
//	bkPrintf("CreateActorInstanceBehaviour( )\n");

	actorInstance->actorBehaviour = (ACTOR_BEHAVIOUR*)ZALLOC(sizeof(ACTOR_BEHAVIOUR));
	if (actorInstance->actorBehaviour)
	{
		InitActorBehaviour(actorInstance->actorBehaviour,METERTOUNIT(0.5f));
		actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLVELCORRECTION;
		actorInstance->actorBehaviour->collCylinder[0].offset[Y] = 
				actorInstance->actorBehaviour->collSphere.offset[Y] = 120.0f;
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance physics\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceGenericAI
   Purpose : Creates and initialises the ai part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : TRUE, FALSE
*/
int CreateActorInstanceGenericAI(ACTORINSTANCE *actorInstance,ACTORINSTANCELIST *activeList,ACTORINSTANCELIST *inactiveList)
{

	actorInstance->genericAI = (GENERICAI*)ZALLOC(sizeof(GENERICAI));
	if (actorInstance->genericAI)
	{
		InitActorGenericAI(actorInstance,activeList,inactiveList);
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance generic ai\n");
		return FALSE;
	}
}


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceAnimation
   Purpose : Creates and initialises the animation part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceAnimation(ACTORINSTANCE *actorInstance)
{
//	bkPrintf("CreateActorInstanceAnimation( )\n");

	actorInstance->actorAnimation = (ACTOR_ANIMATION*)ZALLOC(sizeof(ACTOR_ANIMATION));
	if (actorInstance->actorAnimation)
	{
		InitActorAnimation(actorInstance->actorAnimation);
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance animation\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceStatus
   Purpose : Creates and initialises the status part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceStatus(ACTORINSTANCE *actorInstance)
{
//	bkPrintf("CreateActorInstanceStatus( )\n");0

	actorInstance->actorStatus = (ACTOR_STATUS*)ZALLOC(sizeof(ACTOR_STATUS));
	if (actorInstance->actorStatus)
	{
		InitActorStatus(actorInstance->actorStatus);
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create instance status\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceShadow
   Purpose : Creates and initialises the shadow part of an actorInstance
   Parameters : ACTORINSTANCE, shadowRadius, shadowHeight, 
				(opt/default) max radius of object for shadow map rendering,
				(opt/DOWNWARD) shadow type (SHADOWTYPE_)
   Returns : 
   Info : // PP: added optional type parameter to cater for special occasions like the Zoo boss sunset
*/
int CreateActorInstanceShadow(ACTORINSTANCE	*actorInstance,
							  float			shadowRadius,
							  float			shadowHeight,
							  const float	shadowMapRadius,
							  ShadowType	shadowType)
{
//	bkPrintf("CreateActorInstanceShadow( ,%d,%d)\n",shadowRadius,shadowHeight);

	// PP: if the actor instance already has a shadow, free it before creating a new one
	if(actorInstance->actorShadow != NULL)
	{
		SAFE_FREE(actorInstance->actorShadow);
	}

	actorInstance->actorShadow = (SHADOW*)ZALLOC(sizeof(SHADOW));
	if (actorInstance->actorShadow)
	{
		InitialiseShadow(actorInstance,shadowRadius,shadowHeight,shadowMapRadius,shadowType);
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create shadow for actor\n");
		return FALSE;
	}
}


/* --------------------------------------------------------------------------------
   Function : CreateActorInstanceCharacterInfo
   Purpose : Creates and initialises the character info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstanceCharacterInfo(ACTORINSTANCE *actorInstance, uint32 createFlags)
{
//	bkPrintf("CreateActorInstanceCharacterInfo()\n");

	actorInstance->characterInfo = (CHARACTERINFO*)ZALLOC(sizeof(CHARACTERINFO));
	if (actorInstance->characterInfo)
	{
		InitialiseCharacterInfo(actorInstance, createFlags);
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create character info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : CreateActorInstancePathInfo
   Purpose : Creates and initialises the path info part of an actorInstance
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
int CreateActorInstancePathInfo(ACTORINSTANCE *actorInstance)
{
	actorInstance->pathInfo = (PATHINFO*)ZALLOC(sizeof(PATHINFO));
	if (actorInstance->pathInfo)
	{
		actorInstance->pathInfo->path = NULL;
		actorInstance->pathInfo->start = NULL;
		actorInstance->pathInfo->end = NULL;
		actorInstance->pathInfo->collEntry = NULL;
		actorInstance->pathInfo->start = NULL;
		actorInstance->pathInfo->end = NULL;
		actorInstance->pathInfo->flags = 0;
		actorInstance->pathInfo->cameraDist = 0.0f;
		actorInstance->pathInfo->lastState = actorInstance->pathInfo->state = PATHSTATE_STATIONARY;
		return TRUE;
	}
	else
	{
		bkPrintf("*** ERROR *** Could not create path info structure\n");
		return FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeActorInstanceByName
   Purpose : Free an actor instance using the instance name
   Parameters : actorInstanceList, instance name
   Returns : TRUE,FALSE
   Info : 
*/
int FreeActorInstanceByName(ACTORINSTANCELIST *actorInstanceList,char *instanceName)
{
	ACTORINSTANCE			*ptr;
	uint32					crc;

	ptr = actorInstanceList->head.next;
	crc = bkCRC32((uchar*)instanceName,strlen(instanceName),0);

//	bkPrintf("FreeActorInstanceByName( ,%s)\n",instanceName);

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->instanceCRC == crc)
		{
			// remove from instance list
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			actorInstanceList->numEntries--;

			// free actor instance
			FreeActorInstanceMemory(ptr);

			SAFE_FREE(ptr);
			return TRUE;
		}
	}
	bkPrintf("*** WARNING *** Could not free instance\n");
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeActorInstance
   Purpose : Free an actor instance using the actor instance
   Parameters : actorInstanceEntry
   Returns : TRUE,FALSE
   Info : 
*/
int FreeActorInstance(ACTORINSTANCE *actorInstanceEntry, int dontFreeFlags)
{
	ACTORINSTANCE *ptr;
	ACTORINSTANCELIST *actorInstanceList;

	actorInstanceList = actorInstanceEntry->instanceList;
	ptr = actorInstanceList->head.next;

	//bkPrintf("FreeActorInstance( , )\n");

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		// check the instance is on this actor instance list
		if (ptr == actorInstanceEntry)
		{
			RemoveActorFromCollisionCache(&collisionCache, ptr);

			// found so free
			// remove from instance list
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			actorInstanceList->numEntries--;

			// free actor instance
			FreeActorInstanceMemory(ptr, dontFreeFlags);

			SAFE_FREE(ptr);
			return TRUE;
		}
	}
	bkPrintf("*** WARNING *** Could not find instance in this instance list\n");
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : FreeDestructibleInfo
   Purpose : Frees all the memory used by the destructibleObjectInfo structure
   Parameters : DESTRUCTIBLEOBJECTINFO
   Returns : 
   Info : 
*/
void FreeDestructibleInfo(DESTRUCTIBLEOBJECTINFO *destructibleObjectInfo)
{
	if (destructibleObjectInfo->hitTriggers)
	{
		for (int i=destructibleObjectInfo->maxHits-1;i>=0;i--)
		{
			if (destructibleObjectInfo->hitTriggers[i])
			{
				DeleteTriggerInfo(destructibleObjectInfo->hitTriggers[i]);
			}
		}
		SAFE_FREE(destructibleObjectInfo->hitTriggers);
	}
	if(destructibleObjectInfo->debris)
	{
		SAFE_FREE(destructibleObjectInfo->debris);
		destructibleObjectInfo->debris = NULL;
	}
	if (destructibleObjectInfo->animation)
	{
		SAFE_FREE(destructibleObjectInfo->animation);
		destructibleObjectInfo->animation = NULL;
	}
	if (destructibleObjectInfo->explosion)
	{
		SAFE_FREE(destructibleObjectInfo->explosion);
	}
	if(destructibleObjectInfo->sparkInfo)
	{
		if(destructibleObjectInfo->sparkInfo->sparkTextureCRC) SAFE_FREE(destructibleObjectInfo->sparkInfo->sparkTextureCRC);
		if(destructibleObjectInfo->sparkInfo->sparkTexture) SAFE_FREE(destructibleObjectInfo->sparkInfo->sparkTexture);
		SAFE_FREE(destructibleObjectInfo->sparkInfo);
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeActorInstanceMemory
   Purpose : Frees all the memory assigned to an actor instance
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/
void FreeActorInstanceMemory(ACTORINSTANCE *ptr, int dontFreeFlags)
{
	int			i;

	// TP: sort out any looping sound effects assigned to this actorinstance
	if (ptr->soundHandle!=-1)
	{
		// TP: sound is still playing
		StopChannel(&ptr->soundHandle);
	}

	if (!(dontFreeFlags&DONTFREE_ACTORINSTANCE)) baFreeActorInstance(&ptr->actorInstance);

	if (ptr->actorBehaviour) SAFE_FREE(ptr->actorBehaviour);
	if (ptr->actorAnimation) SAFE_FREE(ptr->actorAnimation);
	if (ptr->actorStatus) 
	{
		if(ptr->actorStatus->zzzInfo) SAFE_FREE(ptr->actorStatus->zzzInfo);
		if(ptr->actorStatus->questionMarkInfo) SAFE_FREE(ptr->actorStatus->questionMarkInfo);
		if(ptr->actorStatus->postCatatonicTrigger)
		{
			DeleteTriggerInfo(ptr->actorStatus->postCatatonicTrigger);
			ptr->actorStatus->postCatatonicTrigger = NULL;
		}
		if(ptr->actorStatus->costumeChangeInfo) FREE(ptr->actorStatus->costumeChangeInfo);
		SAFE_FREE(ptr->actorStatus);
	}
	if (ptr->characterInfo)
	{
		DestroyCharacterInfo(ptr);
		SAFE_FREE(ptr->characterInfo);
	}
	if (ptr->genericAI) 
	{
		FreeActorGenericAI(ptr);
		SAFE_FREE(ptr->genericAI);
	}

	if (ptr->actorShadow) SAFE_FREE(ptr->actorShadow);

	if (ptr->pathInfo)
	{
		DeleteTriggerInfo(ptr->pathInfo->start);
		DeleteTriggerInfo(ptr->pathInfo->end);
		SAFE_FREE(ptr->pathInfo);
	}
	if (ptr->specialType) 
	{
		switch(ptr->special)
		{
		case SPECIALTYPE_SHATTER:
			//DeleteTriggerInfo(ptr->shatterInfo->onShatter);
			DeleteTriggerInfo(ptr->shatterInfo->onShatter);
			break;
		case SPECIALTYPE_DESTRUCTIBLE:
			if (!(dontFreeFlags&DONTFREE_DESTRUCTIBLEINFO))
			{
				FreeDestructibleInfo(ptr->destructibleObjectInfo);
			}
			break;
		case SPECIALTYPE_WATERYOBJECT:
			if(ptr->wateryObjectInfo->resetInfo)
			{
				FreeWaterResetPoints(ptr->wateryObjectInfo->resetInfo);
				ptr->wateryObjectInfo->resetInfo = NULL;
			}
			break;
		case SPECIALTYPE_DEATH:
			if (ptr->deathObjectInfo->resetInfo)
			{
				FreeWaterResetPoints(ptr->deathObjectInfo->resetInfo);
				ptr->deathObjectInfo->resetInfo = NULL;
			}
			break;
		case SPECIALTYPE_SPEEDBOOST:
			DeleteTriggerInfo(ptr->speedboostInfo->onBoost);
			break;
		case SPECIALTYPE_TRIGGER:
			DeleteTriggerInfo(ptr->triggerObjectInfo->onContact);
			DeleteTriggerInfo(ptr->triggerObjectInfo->enterRadius);
			DeleteTriggerInfo(ptr->triggerObjectInfo->exitRadius);
			break;
		case SPECIALTYPE_PROJECTILE:
			if(ptr->projectileInfo->onContact)
			{
				DeleteTriggerInfo(ptr->projectileInfo->onContact);
			}
			if(ptr->projectileInfo->onAim)
			{
				DeleteTriggerInfo (ptr->projectileInfo->onAim);
			}
			if(ptr->projectileInfo->onFire)
			{
				DeleteTriggerInfo(ptr->projectileInfo->onFire);
			}
			if(ptr->projectileInfo->onRecoil)
			{
				DeleteTriggerInfo(ptr->projectileInfo->onRecoil);
			}
			break;
		case SPECIALTYPE_TRAMAMPOLINE:
			if(ptr->springyObjectInfo->onContact)
			{
				DeleteTriggerInfo(ptr->springyObjectInfo->onContact);
			}
			break;
		case SPECIALTYPE_MAILBOX:
			if(ptr->mailboxInfo->onContact)
			{
				DeleteTriggerInfo(ptr->mailboxInfo->onContact);
			}
			if(ptr->mailboxInfo->onExcited)
			{
				DeleteTriggerInfo(ptr->mailboxInfo->onExcited);
			}
			if(ptr->mailboxInfo->onVeryExcited)
			{
				DeleteTriggerInfo(ptr->mailboxInfo->onVeryExcited);
			}
			break;
		case SPECIALTYPE_WANTEDPOSTER:
			if(ptr->wantedPosterInfo->onDestruction)
			{
				DeleteTriggerInfo(ptr->wantedPosterInfo->onDestruction);
			}
			break;
		case SPECIALTYPE_FALLINGOBJECT:
			if(ptr->fallingObjectInfo->cartoonScaleInfo)
			{
				SAFE_FREE(ptr->fallingObjectInfo->cartoonScaleInfo);
			}
			if(ptr->fallingObjectInfo->onDestruction)
			{
				DeleteTriggerInfo(ptr->fallingObjectInfo->onDestruction);
				ptr->fallingObjectInfo->onDestruction = NULL;
			}
			if(ptr->fallingObjectInfo->debris)
			{
				SAFE_FREE(ptr->fallingObjectInfo->debris);
				ptr->fallingObjectInfo->debris = NULL;
			}
			break;
		case SPECIALTYPE_ELECTRICITY:
			if(ptr->electricityInfo->pathNodes)
			{
				FreeAllElectricityPathNodes(ptr);
			}
			break;
		case SPECIALTYPE_LASER:
			if(ptr->laserInfo->pathNodes)
			{
				SAFE_FREE(ptr->laserInfo->pathNodes->objectNames);
				SAFE_FREE(ptr->laserInfo->pathNodes);
				ptr->laserInfo->pathNodes = NULL;
			}
			break;
		case SPECIALTYPE_TRANSPORTER:
			if(ptr->transporterInfo->onContact)
			{
				DeleteTriggerInfo(ptr->transporterInfo->onContact);
			}
			DeleteTriggerInfo(ptr->transporterInfo->onComplete);
			DeleteTriggerInfo(ptr->transporterInfo->onReachingDestination);
			break;
		case SPECIALTYPE_SWINGINGOBJECT:
			if(ptr->swingingObjectInfo->onContact)
			{
				DeleteTriggerInfo(ptr->swingingObjectInfo->onContact);
			}
			break;
		case SPECIALTYPE_LIGHTNING:
			if(ptr->lightningInfo->startPoints)
			{
				SAFE_FREE(ptr->lightningInfo->startPoints->objectNames);
				SAFE_FREE(ptr->lightningInfo->startPoints);
				ptr->lightningInfo->startPoints = NULL;
			}
			break;
		case SPECIALTYPE_PHONEBOX:
			if (ptr->phoneBoxInfo->effectInfo)
			{
				SAFE_FREE(ptr->phoneBoxInfo->effectInfo);
			}
/*			if (ptr->phoneBoxInfo->box)
			{
				FreeActorInstance(ptr->phoneBoxInfo->box);
				ptr->phoneBoxInfo->box = NULL;
			}*/
			break;
		case SPECIALTYPE_MONITOR:
			{
				SAFE_FREE(ptr->monitorInfo->effect);
				ptr->monitorInfo->effect = NULL;
			}
			break;
		case SPECIALTYPE_FLOCKING:
			if(ptr->flockingInfo)
			{
				for(i=0;i<ptr->flockingInfo->numFlockers;i++)
				{
					baFreeActorInstance(&ptr->flockingInfo->flockers[i]->actorInstance);
					SAFE_FREE(ptr->flockingInfo->flockers[i]);
				}
				SAFE_FREE(ptr->flockingInfo->flockers);
				SAFE_FREE(ptr->flockingInfo);
			}
			break;
		case SPECIALTYPE_VEHICLE:
			{
				if(ptr->vehicleInfo != NULL)
				{
					// PP: if any further malloced memory is pointed to by the type-specific data of a particular vehicle type, it should be freed here
					switch(ptr->vehicleInfo->type)
					{
					case VEHICLE_POLISHER:// PP: this 'case' gets rid of a warning
						SAFE_DELETE(ptr->vehicleInfo->polisherInfo);
						break;
						// TP: the new ps2 compiler gave a warning about this not being a pointer-to-object type
						// TP: I assume it wasn't sure what destructors to call
					/*default:
						{
							delete ptr->vehicleInfo->typeSpecificData;
						}*/
					}
				}
			}
			break;
		case SPECIALTYPE_HAAR:
			{
				FreeHaarObject(ptr);
			}
			break;
		case SPECIALTYPE_TAZBOSSITEM:
			if(ptr->tazBossItemInfo)
			{
				if(ptr->tazBossItemInfo->debris)
				{
					SAFE_FREE(ptr->tazBossItemInfo->debris);
				}
				SAFE_FREE(ptr->tazBossItemInfo);
			}
			break;
		case SPECIALTYPE_ROCKET:
			if(ptr->rocketInfo)
			{
				SAFE_FREE(ptr->rocketInfo);
			}
			break;
		case SPECIALTYPE_XDOOR:
			FreeXDoor(ptr);
			break;
		case SPECIALTYPE_MUSICBOX:
			MUSICBOXEXTRAINFO	*data;

			if(ptr->musicBoxInfo)
			{
				SAFE_FREE(ptr->musicBoxInfo->verts);
				SAFE_FREE(ptr->musicBoxInfo->polys);

				// CMD: spin through extra system to get NULL any ptrs to this instance
				for(i=0;i<staveExtraSystem->noofInstances;i++)
				{
					data = (MUSICBOXEXTRAINFO*)staveExtraSystem->dataPtrs[i];
					if(data->musicBox == ptr)
					{
						bkPrintf("deleted one stave\n");
						baDeleteExtraInstance(staveExtraSystem, i);
//						noofInstances--;
						continue;
					}
				}
			}
		case SPECIALTYPE_HALO:
			FreeHaloObjectMemory(ptr);
			break;
		case SPECIALTYPE_ZAPPER:
			if(ptr->zapperInfo)
			{
				SAFE_FREE(ptr->zapperInfo);
			}
			break;
		case SPECIALTYPE_BUZZARD:
			if(ptr->buzzardInfo)
			{
				SAFE_FREE(ptr->buzzardInfo);
			}
			break;
		case SPECIALTYPE_MINECART:
			if(ptr->mineCartInfo)
			{
				if(ptr->mineCartInfo->wheelSparks)
				{
					SAFE_FREE(ptr->mineCartInfo->wheelSparks);
				}
				SAFE_FREE(ptr->mineCartInfo);
			}
		}
		if (!(dontFreeFlags&DONTFREE_DESTRUCTIBLEINFO))
		{
			SAFE_FREE(ptr->specialType);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FreeAllActorInstances
   Purpose : Frees all actor instances on a given instance list
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/
void FreeAllActorInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr,*temp;
	
	ptr = actorInstanceList->head.next;

	bkPrintf("FreeAllActorInstances( )\n");

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
	{
		// found so free
		// remove from instance list
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		actorInstanceList->numEntries--;

		// free actor instance
		temp = ptr->next;
		FreeActorInstanceMemory(ptr);
		SAFE_FREE(ptr);
		ptr = temp;
	}
	return;
}

/* --------------------------------------------------------------------------------
   Function : DrawActorInstances(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baDrawActorInstance
*/
void DrawActorInstances(ACTORINSTANCELIST *actorInstanceList, uint32 flags)
{
	ACTORINSTANCE	*ptr;
	LightRamp		rampCopy;

	// PP: TEMP?: This prevents the texture-mapping problem on cartoon-lit actors on Xbox
	WRAPTEXTURE;

	ptr = actorInstanceList->head.next;

	// PP: apply 'airbrush' mode
	if(CheatActive(CHEAT_AIRBRUSH))
	{
		bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 0);
		bdSetRenderState(BDRENDERSTATE_LIGHTING, TRUE, 1);
	}

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		// PP: if we're in Taz-Cam, don't draw Taz.  We don't really want to see the inside of his mouth ta.
		if(ptr == gameStatus.player1)
		{
			// CMD: quicker to do the tests this way around	// PP: Yep, good point, sorry
			if(CheatActive(CHEAT_TAZCAM))
			{
				continue;
			}
		}

		// CMD: don't want to draw here if actorInstance is SPECIALTYPE_MIRRORBALL
		switch(ptr->special)
		{
		case SPECIALTYPE_MIRRORBALL:
			continue;
		default:
			break;
		}

		if ( (showInvisibleObjects || (!(ptr->flags & (ACTORFLAG_DONTDRAW|ACTORFLAG_CUSTOMDRAW))))&&(ptr->currentAlpha) )
		{
			lineParms.thickness = ptr->lineThickness;

			// PP: if the actorinstance is in shade, use a lowlight ramp
			if(ptr->flags & ACTORFLAG_INSHADE)
			{
				rampCopy=LightRamp(cartoonParms.rampNumber);

				// PP: NOTE: I'm not using SetLightRamp here becuase if SetLightRamp ever did anything more than simply assign the ramp number,
				// PP: I wouldn't want to be doing that here.

				if(cartoonParms.rampNumber == RAMP_SUNSET)
				{
					cartoonParms.rampNumber=RAMP_SUNSETSHADE;
				}
				else// PP: assume RAMP_TWOTONE
				{
					cartoonParms.rampNumber=RAMP_SHADE;
				}
			}

			bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);	
			if (ptr->flags&ACTORFLAG_DONTFOG)
			{
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
			}

			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

			// PP: --------------------------------------------------------------------------------------------------------------------------------

			baSetGlobalAlpha(ptr->currentAlpha);
			// TP: Only set onScreen don't remove
#if BPLATFORM == PC
			bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			bDisplayInfo.d3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
#endif

			if (flags&DRAWACTORFLAG_DONTCLEARONSCREEN)
			{
				// TP: Only set onScreen don't remove
				ptr->onScreen |= baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_SOLID);
			}
			else
			{
				if((ptr->characterInfo)&&(ptr->characterInfo->tartanInfo))
				{
					ptr->onScreen = UpdateTartanTazRenderTarget(ptr);
				}
#ifndef CONSUMERDEMO
				else
				if((ptr->characterInfo)&&(ptr->characterInfo->transportInfo))
				{
					ptr->onScreen = DrawCharacterInTransporter(ptr);
				}
#endif
				else	ptr->onScreen = baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_SOLID);
			}

			// PP: NEW TEST - was in the alpha stage
			{
				/* PP: LEAVIN: TEMP TEST - why can't I seem to enable blending on these colour keys?
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, 0);
				bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, 1);
				SETBLEND(BLEND);*/
				bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);
				baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_CKEY);// TP: ,2
			}

			bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
			bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);
			if (ptr->flags&ACTORFLAG_DONTFOG)
			{
				if (map.fogState==true)
				{
					bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
				}
			}

			// PP: if the actorinstance was in shade, return to normal ramp having now drawn it
			if(ptr->flags & ACTORFLAG_INSHADE)
			{
				cartoonParms.rampNumber=rampCopy;
			}
		}
	}

	// PP: turn off lighting again
	bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 0);
	bdSetRenderState(BDRENDERSTATE_LIGHTING, FALSE, 1);
	
	baSetGlobalAlpha(256);

	lineParms.thickness = standardLineThickness;
}


/* --------------------------------------------------------------------------------
   Function : DrawActorInstances2(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws all actor instance in the supplied list (CKEY and alpha)
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baDrawActorInstance
*/
void DrawActorInstances2(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;
	TBSavedRenderStates storedRenderState;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		// TP: drawing the solid part will dictate if object is visible
		if (!ptr->onScreen) continue;

#if BPLATFORM == PC
		if (ptr->actorInstance.actor->flags&TBACTORFLAG_SOFTSKIN) continue;
#endif

		if ((!(ptr->flags & (ACTORFLAG_DONTDRAW|ACTORFLAG_CUSTOMDRAW)))&&(ptr->currentAlpha))
		{
			lineParms.thickness = ptr->lineThickness;
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
			if (ptr->flags&ACTORFLAG_DONTFOG)
			{
				bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
			}

			baSetGlobalAlpha(ptr->currentAlpha);
/* PP: REMOUT: NEW TEST			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);
			baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_CKEY);// TP: ,2*/
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,0);
			baDrawActorInstance(&ptr->actorInstance,BSETPRIMALPHATYPE_ALPHA);// TP: ,2
			bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
			bdSetRenderState(BDRENDERSTATE_BACKFACECULL, FALSE, FALSE);

			if (ptr->flags&ACTORFLAG_DONTFOG)
			{
				if (map.fogState)
				{
					bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
				}
			}
		}
	}
	baSetGlobalAlpha(256);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,0);

	lineParms.thickness = standardLineThickness;

	if(gameStatus.instanceName)
	{
		// Draw Instance Names
		bdSaveRenderStates(&storedRenderState);
		DrawActorInstancesDetails(actorInstanceList);
		bdRestoreRenderStates(&storedRenderState);
		bdSetObjectMatrix(bIdentityMatrix);
	}

	if(gameStatus.instanceZone)
	{
		// Draw Instance Zones (Bounding box, axis, etc..)
		bdSaveRenderStates(&storedRenderState);
		DrawActorInstanceZones(actorInstanceList);
		bdRestoreRenderStates(&storedRenderState);
		bdSetObjectMatrix(bIdentityMatrix);
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawActorInstancesDetails(ACTORINSTANCELIST *actorInstanceList
   Purpose : Draws info for all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info :
*/
void DrawActorInstancesDetails(ACTORINSTANCELIST *actorInstanceList)
{
	char					viewerBuf[256];
	
	ACTORINSTANCE *ptr;
	COLLISIONLISTENTRY *collisionEntryPtr;

	TBMatrix			scale, trans, rot, tempMat, resultMat;
	TBVector			textLocation;
	ushort				string16[256];
	TBFontFormatting	format;
	float				instanceDistance;
	int32				polyCount;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		instanceDistance = bmVectorDistance(ptr->actorInstance.position , camera->cameraState.cameraPos);

		if ((!(ptr->flags & (ACTORFLAG_DONTDRAW|ACTORFLAG_CUSTOMDRAW)))&&(ptr->currentAlpha)&&(ptr->onScreen)&&(instanceDistance<4000.0f))
		{	
			// Copy format
			format.width =		320;
			format.height =		80;
			format.horzFormat = EBFONTFORMATTING_CENTRE;
			format.vertFormat = EBFONTFORMATTING_CENTRE;
			format.wrap=0;
			format.lineSpace = 0;	
			format.x = -160;
			format.y = -40;

			// Get collision entry info
			if ((collisionEntryPtr = GetCollisionEntry(&collisionCache, ptr))!=NULL)
			{
				// Get poly count from collision entry info
				polyCount = collisionEntryPtr->numVertices/3;
			}
			else
			{
				polyCount = 0;
			}

			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
			bdSetRenderState(BDRENDERSTATE_ZTEST,BDZTESTMODE_ALWAYS, FALSE);
		
			// Set location of text
			textLocation[X] = ptr->actorInstance.position[X];
			textLocation[Y] = ptr->actorInstance.position[Y] + ptr->actorInstance.actor->yMax;
			textLocation[Z] = ptr->actorInstance.position[Z];

			// Create instance / backing rectangle name matrix
			bdSetIdentityObjectMatrix();
			bmMatTranslate(trans, (float)textLocation[X], (float)textLocation[Y], (float)textLocation[Z]);
			bmMatYXZRotation(rot,camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[X],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Y],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Z]);
			bmMatScale(scale, 5.0f, 5.0f, 5.0f);
			bmMatMultiply(tempMat, trans, rot);
			bmMatMultiply(resultMat, tempMat, scale);
			bdSetObjectMatrix(resultMat);

			// Draw backing rectangle
			bdSetTexture(0,NULL);
			bdSetTexture(1,NULL);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
			DrawRectangle(RECTANGLE(-160, 160, -120, -20),COLOUR(0,0,0,40));
			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);

			// Draw instance name
			if (strlen(ptr->instanceName) < 255)
			{
				sprintf(viewerBuf, "%s", ptr->instanceName);
			}
			else
			{
				sprintf(viewerBuf, "Error");
			}
			bkString8to16(string16, viewerBuf);
			bdPrintFont(standardFont,string16, bkStringLength16(string16),&format,128,128,128,128,NULL,NULL);	

			// Create actor name matrix
			bdSetIdentityObjectMatrix();
			bmMatTranslate(trans, (float)textLocation[X], (float)textLocation[Y], (float)textLocation[Z]);
			bmMatYXZRotation(rot,camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[X],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Y],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Z]);
			bmMatMultiply(tempMat, trans, rot);
			bmMatScale(scale, 3.0f, 3.0f, 3.0f);
			bmMatMultiply(resultMat, tempMat, scale);
			bdSetObjectMatrix(resultMat);

			// Draw actor name
			if (strlen(ptr->actorName) < 255)
			{
				sprintf(viewerBuf, "%s", ptr->actorName);
			}
			else
			{
				sprintf(viewerBuf, "Error");
			}
			bkString8to16(string16, viewerBuf);
			bdPrintFont(standardFont,string16, bkStringLength16(string16),&format,128,0,0,128,NULL,NULL);	

			// Create special type / polygon count matrix
			bdSetIdentityObjectMatrix();

			bmMatTranslate(trans, (float)textLocation[X], (float)textLocation[Y]-310.0f, (float)textLocation[Z]);
			bmMatYXZRotation(rot,camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[X],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Y],camera[gameStatus.multiplayer.currentScreen].cameraState.cameraAngles[Z]);
			bmMatMultiply(tempMat, trans, rot);
			bmMatScale(scale, 3.0f, 3.0f, 3.0f);
			bmMatMultiply(resultMat, tempMat, scale);
			bdSetObjectMatrix(resultMat);

			// If this instance is a special type show it's special details
			if (ptr->special != -1)
			{
				// Draw special type & polygon count
				if (strlen(specialTypeNames[ptr->special]) < 255)
				{
					sprintf(viewerBuf, "%s (%d polys)", specialTypeNames[ptr->special], polyCount);
				}
				else
				{
					sprintf(viewerBuf, "Error");
				}
				bkString8to16(string16, viewerBuf);
				bdPrintFont(standardFont,string16, bkStringLength16(string16),&format,128,128,0,128,NULL,NULL);	
			}
			else
			{
				// Draw polygon count
				sprintf(viewerBuf, "(%d polys)", polyCount);
				bkString8to16(string16, viewerBuf);
				bdPrintFont(standardFont,string16, bkStringLength16(string16),&format,128,128,0,128,NULL,NULL);	
			}

			bdSetRenderState(BDRENDERSTATE_ZTEST,BDZTESTMODE_LESSTHAN, FALSE);		
		}
	}	
}

/*  --------------------------------------------------------------------------------
	Function	: CalculateCurrentAlphaActor
	Purpose		: Calculate the alpha for an actor
	Parameters	: actorInstance, min, max, minSize, maxSize, sizeMod
	Returns		: 
	Info		: 
*/

void CalculateCurrentAlphaActor(ACTORINSTANCE *actorInstance, float min, float max, float minSize, float maxSize, float sizeMod)
{
	TBVector	tempVector;
	float		dist;
	int			alpha=256;
	float		myMin,myMax;
	float		i;

	if(actorInstance->flags & ACTORFLAG_DONTDRAW)
	{
		actorInstance->currentAlpha = 0;
		return;
	}
	if (actorInstance->flags & ACTORFLAG_DONTFADE)
	{
		actorInstance->currentAlpha = actorInstance->alpha;
		actorInstance->lineThickness = lineParms.thickness = standardLineThickness;
		return;
	}

	actorInstance->currentAlpha = actorInstance->alpha;

	if (actorInstance->actorInstance.actor->maxRadius < minSize) return;

	if ((maxSize==0.0f)||(actorInstance->actorInstance.actor->maxRadius <= maxSize))
	{
		bmVectorSub(tempVector,gameStatus.viewPoint,actorInstance->actorInstance.position);

		dist = bmVectorLen(tempVector);

		actorInstance->lineThickness = lineParms.thickness = standardLineThickness * (dist/500);
		if (lineParms.thickness > standardLineThickness) actorInstance->lineThickness = lineParms.thickness = standardLineThickness;

		myMax = max;
		myMin = min;
		if (maxSize==0.0f)
		{
			i = actorInstance->actorInstance.actor->maxRadius*sizeMod;
			myMax += i;
			myMin += i;
		}

		if (dist > myMax) 
		{
			actorInstance->currentAlpha = 0;
			return;
		}
		if (dist > myMin)
		{
			actorInstance->currentAlpha = (int)(((myMax-dist) / (myMax-myMin))*actorInstance->alpha);
		}
	}
	else
	{
		actorInstance->lineThickness = lineParms.thickness = standardLineThickness;
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateCurrentAlpha
   Purpose : Calculate current alpha values
   Parameters : actorInstanceList, min, max, maxSize
   Returns : 
   Info : will fill in currentAlpha and lineThickness part of actor structure using gameStatus.viewPoint
*/
void CalculateCurrentAlpha(ACTORINSTANCELIST *actorInstanceList, float min, float max, float minSize, float maxSize, float sizeMod)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		CalculateCurrentAlphaActor(ptr, min, max, minSize, maxSize, sizeMod);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CalculateCurrentAlphaCharacter
	Purpose		: Calculate the current alpha for a character
	Parameters	: actorInstance list
	Returns		: 
	Info		: 
*/

void CalculateCurrentAlphaCharacter(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->genericAI)
		{
			CalculateCurrentAlphaActor(ptr, ptr->genericAI->deactivationRange-METERTOUNIT(2.0f), ptr->genericAI->deactivationRange, 0.0f, 0.0f, 0.0f);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepActorInstances(ACTORINSTANCELIST *actorInstanceList
   Purpose : Prepares all actor instance in the supplied list
   Parameters : actorInstanceList
   Returns : 
   Info : calls	baPrepActorInstance
*/
void PrepActorInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr,*temp;

	ptr = actorInstanceList->head.next;

	if(actorInstanceList->flags&ACTORINSTLISTFLAG_ANIMATED)
	{
		for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
		{
			temp = ptr->next;
			
			// TP: If not visible then don't need to prep
			if (ptr->flags&ACTORFLAG_FADEAFTERANIM)
			{
				if (!baIsActorAnimPlaying(&ptr->actorInstance,ptr->actorInstance.rootNodeInstance,FALSE))
				{
					ptr->flags&=~ACTORFLAG_FADEAFTERANIM;
				}
			}
			else
			{
				if (ptr->flags&ACTORFLAG_UPDATECOLLAFTERANIM)
				{
					if (!baIsActorAnimPlaying(&ptr->actorInstance,ptr->actorInstance.rootNodeInstance,FALSE))
					{
						ptr->flags&=~ACTORFLAG_UPDATECOLLAFTERANIM;
						RefreshCollisionEntry(&collisionCache,ptr);
					}
				}
				if (ptr->fadeSpeed!=0.0f)
				{
					ptr->alpha = (short)(ptr->alpha + (ptr->fadeSpeed/fps));
					if (ptr->alpha > 256) ptr->alpha = 256;
					if (ptr->alpha <= 0) 
					{
						ptr->alpha = 0;
						if (ptr->flags&ACTORFLAG_DELETEAFTERFADEOUT)
						{
							FreeActorInstance(ptr);
							ptr = temp;
							continue;
						}
					}
				}
			}

			if ((ptr->currentAlpha==0)&&(~ptr->flags&ACTORFLAG_ALWAYSPREP)&&(gameStatus.multiplayer.numScreens==1))
			{
				ptr = ptr->next;
				continue;
			}

			baPrepActorInstance(&ptr->actorInstance,TRUE);
			ptr = temp;
		}
	}
	else
	{
		for(ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;)
		{
			temp = ptr->next;
			// TP: If not visible then don't need to prep
			if (ptr->fadeSpeed!=0.0f)
			{
				ptr->alpha = (short)(ptr->alpha + (ptr->fadeSpeed/fps));
				if (ptr->alpha > 256) ptr->alpha = 256;
				if (ptr->alpha <= 0) 
				{
					ptr->alpha = 0;
					if (ptr->flags&ACTORFLAG_DELETEAFTERFADEOUT)
					{
						FreeActorInstance(ptr);
						ptr = temp;
						continue;
					}
				}
			}

			if ((ptr->currentAlpha==0)&&(~ptr->flags&ACTORFLAG_ALWAYSPREP)&&(gameStatus.multiplayer.numScreens==1))
			{
				ptr = ptr->next;
				continue;
			}

			baPrepActorInstance(&ptr->actorInstance,FALSE);
			ptr = temp;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: AnimateActorInstance
	Purpose		: Update the animation for an actorInstance
	Parameters	: actorInstance
	Returns		: 
	Info		: 
*/
void AnimateActorInstance(ACTORINSTANCE *actorInstance)
{
	TBVector		motion;
	int				nodesUpdated;
	float			oldSpRt = speedRatio;
	float			speedRatioToUse;				// PP: = speedRatio, but may get rounded - see comment below.
	float			delta;

	// PP: set the instance's orientation W to 1 if each & every element of the quat equals zero
	// PP: *** NOTE *** This fix is also done in PHYSICS.CPP
	// PP: NOTE: this problem doesn't show up on PS2
	if((actorInstance->actorInstance.orientation[0] == 0.0f)
		&&(actorInstance->actorInstance.orientation[1] == 0.0f)
		&&(actorInstance->actorInstance.orientation[2] == 0.0f)
		&&(actorInstance->actorInstance.orientation[3] == 0.0f))
	{
		actorInstance->actorInstance.orientation[3] = 1.0f;
	}

	if (actorInstance->actorStatus)
	{
		if (actorInstance->actorStatus->multiplayer.effect == PLAYEREFFECTSLOMO)
			speedRatio = 15/fps;
	}

	/* PP: TEST:
			If we're playing a cutscene, round the speed ratio to whole numbers,
			because the fastest anything can happen in a cutscene is one whole 60th of a second.
			We wouldn't want to catch, say, a camera somewhere BETWEEN two dicrete positions!
	*/

	/*
	//TP: Can't do this anymore, the music is timed exactly to the cutscene and this code can
	//TP: throw the cutscenes out by seconds on different platforms etc
	
	if(PLAYING_CUTSCENE)
	{
		speedRatioToUse=(float)ROUND(speedRatio);

		// PP: HMM: that's all fine and dandy, but what about cutscenes in slowmo?
		// PP: for just now, I'll enforce a minimum speed factor of 1.
		speedRatioToUse=MAX(1, speedRatioToUse);
	}
	else*/
	{
		speedRatioToUse=speedRatio;
	}

	// CMD: bodge ahoy
	// CMD: lets try to smooth out the ransition from taz's walk to his run
	if(((actorInstance == gameStatus.player[0].actorInstance)||(actorInstance == gameStatus.player[1].actorInstance))&&actorInstance->actorStatus->currentState == STATE_MOVE)
	{
		if(CheckIfPlayingAnimation(actorInstance, "run"))
		{
			if((delta = gameStatus.player[actorInstance->playerNo].controller->mag1) > 1.0f) delta = 1.0f;
			nodesUpdated = baUpdateNodes(&actorInstance->actorInstance, ROOTNODE(actorInstance->actorInstance.rootNodeInstance), 
										speedRatioToUse*delta, motion);
		}
		else
		{
			nodesUpdated = baUpdateNodes(&actorInstance->actorInstance, ROOTNODE(actorInstance->actorInstance.rootNodeInstance), speedRatioToUse, motion);
		}
	}
	else
	{
		nodesUpdated = baUpdateNodes(&actorInstance->actorInstance, ROOTNODE(actorInstance->actorInstance.rootNodeInstance), speedRatioToUse, motion);
	}

	if (nodesUpdated) actorInstance->flags|=ACTORFLAG_HASANIMATED;
	else actorInstance->flags&=~ACTORFLAG_HASANIMATED;

	if (actorInstance->actorAnimation)
	{
		if (actorInstance->actorBehaviour)
		{
			bmVectorCMul(actorInstance->actorAnimation->motion,motion,fps);
		}
		else
		{
			bmVectorCopy(actorInstance->actorAnimation->motion, motion);
		}

		UpdateActorAnimation(actorInstance);
	}

	speedRatio = oldSpRt;
}

/* --------------------------------------------------------------------------------
   Function : AnimateActorInstances
   Purpose : Updates all the animation information on an actorInstanceList using speedRatio
   Parameters : actorInstanceList
   Returns : 
   Info : calls baUpdateNodes
*/
void AnimateActorInstances(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;
	
	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		switch(ptr->special)
		{
		case SPECIALTYPE_MINECART:
			continue;
		default:
			AnimateActorInstance(ptr);
			break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : EnableActorCartoonRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorCartoonRenderMode(TBActorInstance *actorInstance, TBActRenderMode_CartoonLit *clParms, TBActRenderMode_Outlined *coParms)
{
	// PP: TEMP TEMP TEMP FRIG (I hope)
	// PP: if focus effects are being used, and if that doesn't agree with cel shading, use line mode instead...

#ifndef CARTOON_DOF

	if(VFX_effectActive(VFX_FOCUS))
	{
		EnableActorLineRenderMode(actorInstance);
		return;
	}

#endif// PP: def CARTOON_DOF

	if (lighting)
	{
		baSetLightColour(&cartoonLight,60,40,20);
		baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_NONE, NULL);
	}
	else
	{
		baSetLightColour(&cartoonLight,127,127,127);
		
		// NH: If cartoon lighting is not disabled
		if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLECARTOONLIGHTING))
#if(BPLATFORM==PC)
		&& ((TAG_ACTORINSTANCE *)actorInstance->actor->softSkin.bonesPerVertex)
#endif
		)
		{
			if (!clParms)
				clParms = &cartoonParms;

			// NH: Set Cartoon Lighting
			baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_CARTOONLIT, clParms);
		}
		else
		{
			baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
		}

		// NH: If cartoon outline is not disabled
		if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES))
#if(BPLATFORM==PC)
		&& ((TAG_ACTORINSTANCE *)actorInstance->actor->softSkin.bonesPerVertex)
#endif
		)
		{
			if (!coParms)
				coParms = &lineParms;

			baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_OUTLINED, coParms);
		}
		else
		{
			baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_NONE, NULL);
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : EnableActorLineRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorLineRenderMode(TBActorInstance *actorInstance, TBActRenderMode_Outlined *parms)
{
	// NH: Draw normally for first pass
	baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);

	// NH: If cartoon outline is not disabled draw the outline
	// NH: Babel on PC doesn't support non-softskinned actor outlining, so I check if it's skinned by checking the softSkin bones per vertex (0 on non-softskinned)
	if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES)) 
#if(BPLATFORM==PC)
		&& ((TAG_ACTORINSTANCE *)actorInstance->actor->softSkin.bonesPerVertex)
#endif
	)
	{
		if (!parms)
			parms = &lineParms;
		// NH: Draw Outline for second pass
		baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_OUTLINED, parms);
	}
	else
	{
		baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_NONE, NULL);
	}
}


/* --------------------------------------------------------------------------------
   Function : EnableActorLineOnlyRenderMode
   Purpose : Enable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void EnableActorLineOnlyRenderMode(TBActorInstance *actorInstance, TBActRenderMode_Outlined *parms)
{
	// NH: Write actor to Z buffer but not to screen
	baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
if ((!(gameStatus.customOptions&CUSTOMOPTION_DISABLEOUTLINES)) 
#if(BPLATFORM==PC)
		&& ((TAG_ACTORINSTANCE *)actorInstance->actor->softSkin.bonesPerVertex)
#endif
	)
	// NH: Draw Outline for second pass
	{
		if (!parms)
			parms = &lineParms;

		baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_OUTLINED, parms);
	}
}


/* --------------------------------------------------------------------------------
   Function : SetActorRenderMode
   Purpose : Set current render mode
   Parameters : actorInstance
   Returns : 
   Info : 
*/
/*
void SetActorRenderMode(TBActorInstance *actorInstance, TBActRenderModeCartoonParms	*cartoonParms)
{
#ifdef MAPBUILD
	return;
#endif
	baSetActorRenderMode(actorInstance, ACTRENDERMODE_CARTOON, cartoonParms);
}
*/
/* --------------------------------------------------------------------------------
   Function : DisableActorCartoonRenderMode
   Purpose : Disable cartoon rendering on an actor using the global cartoon light
   Parameters : actorInstance
   Returns : 
   Info : 
*/
void DisableActorCartoonRenderMode(TBActorInstance *actorInstance)
{
//	baSetActorRenderMode(actorInstance, ACTRENDERMODE_NORMAL, NULL);
	baSetActorRenderMode(actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
	baSetActorRenderMode(actorInstance, 1, BACTRENDERMODE_NONE, NULL);
}

/* --------------------------------------------------------------------------------
   Function : SetActorScale
   Purpose : Sets the current position for an actor
   Parameters : actorInstance, pos
   Returns : 
   Info : 
*/
void SetActorScale(ACTORINSTANCE *actorInstance,TBVector scale)
{
	bmVectorCopy(actorInstance->actorInstance.scale,scale);
}

/* --------------------------------------------------------------------------------
   Function : SetActorOrientation
   Purpose : Sets the current orientation for an actor
   Parameters : actorInstance, orientation as quaternion
   Returns : 
   Info : 
*/
void SetActorOrientation(ACTORINSTANCE *actorInstance,TBQuaternion quat)
{
	if (actorInstance->actorBehaviour)
	{
		bmQuatCopy(actorInstance->actorBehaviour->rotAim,quat);
	}
	bmQuatCopy(actorInstance->actorInstance.orientation,quat);
}

/* --------------------------------------------------------------------------------
   Function : SetActorOrientation
   Purpose : Sets the current orientation for an actor
   Parameters : actorInstance, orientation as vector
   Returns : 
   Info : 
*/
void SetActorOrientation(ACTORINSTANCE *actorInstance, const TBVector angleVector)
{
	if (actorInstance->actorBehaviour)
	{
		bmRotationToQuat(actorInstance->actorBehaviour->rotAim,angleVector);
	}
	bmRotationToQuat(actorInstance->actorInstance.orientation,angleVector);
}

/* --------------------------------------------------------------------------------
   Function : SetActorPosition
   Purpose : Sets the current position for an actor
   Parameters : actorInstance, pos
   Returns : 
   Info : 
*/
void SetActorPosition(ACTORINSTANCE *actorInstance,TBVector pos)
{
#ifdef PHIL// PP: TEMP TEST
	if(*((int*)&(pos[0])) == -1)
	{
		// PP: "not a number"
		bkPrintf("POICH!\n");
	}
#endif// PP: def PHIL

	bmVectorCopy(actorInstance->actorInstance.position,pos);
}

/* --------------------------------------------------------------------------------
   Function : ChangeActorInstance
   Purpose : Change an actor instance to use a different actor, used for costume change
   Parameters : actorInstance, new actor, anim queue size, retarget anim to actor
   Returns : TRUE / FALSE
   Info : By default this will enable cartoon lighting on the new actor
*/
int ChangeActorInstance(ACTORINSTANCE *actorInstance,TBActor *actor, int queueSize, TBActor *retarget, int copy)
{
	TBVector position;
	TBQuaternion orientation;
	TBVector scale;
	TBActorInstance tempActor;

	if (actor==NULL) return FALSE;

	DestroyAttachedInfo(actorInstance);

	// copy values from actor before
	bmVectorCopy(position,actorInstance->actorInstance.position);
	bmQuatCopy(orientation,actorInstance->actorInstance.orientation);
	bmVectorCopy(scale,actorInstance->actorInstance.scale);

	if (copy)
	{
		if (!baInitActorInstance(&tempActor, actor, queueSize)) // create new actor
		{
			bkPrintf("*** ERROR *** Not enough memory to change clothes\n");
			return FALSE;
		}
		if (retarget)
		{
			// retarget animation
			baRetargetActorAnimation(retarget, &tempActor);
		}
		baCopyAnimQueue(&tempActor,tempActor.rootNodeInstance,&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,TRUE);
	}

	baFreeActorInstance(&actorInstance->actorInstance);	// free old actor

	if (!baInitActorInstance(&actorInstance->actorInstance, actor,queueSize)) // create new actor
	{
		bkPrintf("*** ERROR *** Not enough memory to change clothes\n");
		return FALSE;
	}
	if (retarget)
	{
		// retarget animation
		baRetargetActorAnimation(retarget,&actorInstance->actorInstance);
	}
	if (copy)
	{
		baCopyAnimQueue(&actorInstance->actorInstance,actorInstance->actorInstance.rootNodeInstance,&tempActor,tempActor.rootNodeInstance,TRUE);
		baFreeActorInstance(&tempActor);
	}

	// copy values back
	bmVectorCopy(actorInstance->actorInstance.position,position);
	bmQuatCopy(actorInstance->actorInstance.orientation,orientation);
	bmVectorCopy(actorInstance->actorInstance.scale,scale);

	// TP: to fix Babel optimization issue
	baSetActorNodeFlags(&actorInstance->actorInstance, actorInstance->actorInstance.rootNodeInstance, BNODEINSTFLAG_ALLKEYS, BNODEINSTFLAG_ALLKEYS, TRUE);

	// prep new actor
	baPrepActorInstance(&actorInstance->actorInstance,TRUE);
	baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_NORMAL, NULL);
	baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_NONE, NULL);
	if (!copy)
	{
		baSetNodeFrame(&actorInstance->actorInstance, ROOTNODE(actorInstance->actorInstance.rootNodeInstance), 0, TRUE);
		baFlushNodeQueue(&actorInstance->actorInstance, ROOTNODE(actorInstance->actorInstance.rootNodeInstance), TRUE);
	}
	if(actorInstance->characterInfo)
	{
		if(actorInstance->characterInfo->powerUpInfo.invisible)
		{
			EnableActorLineOnlyRenderMode(&actorInstance->actorInstance, &invisLineParms);			
			//baSetActorRenderMode(&actorInstance->actorInstance, 0, BACTRENDERMODE_ZONLY, NULL);
			//baSetActorRenderMode(&actorInstance->actorInstance, 1, BACTRENDERMODE_OUTLINED, &invisLineParms);
		}
		else
		{
			EnableActorCartoonRenderMode(&actorInstance->actorInstance);
		}
		if((actorInstance->actorStatus)&&(actorInstance->actorStatus->currentState != STATE_SPINUP))
		{
#ifndef CONSUMERDEMO
			if(actorInstance->characterInfo->tazShieldInfo)
			{
				ReattachTazShield(actorInstance, actorInstance->characterInfo->tazShieldInfo);
			}
#endif
		}
	}

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : SetCollisionCallback
   Purpose : Set the function to be called when a collision occurs
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : The function provided will be called when a collision has occured
			the object with the callback is object and the colliding object is collider
*/
void SetCollisionCallback(ACTORINSTANCE *actorInstance,RespondToCollision callback)
{
	actorInstance->RespondToCollisionRoutine = callback;
}

/* --------------------------------------------------------------------------------
   Function : SetPreCollisionCallback
   Purpose : Set the function to be called when a collision occurs
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : The function provided will be called when a collision has occured
			the object with the callback is object and the colliding object is collider
*/

void SetPreCollisionCallback(ACTORINSTANCE *actorInstance, PreRespondToCollision callback )
{
	actorInstance->PreRespondToCollisionRoutine = callback;
}

/* --------------------------------------------------------------------------------
   Function : SetOutsideBoundingBoxCallback
   Purpose : Set the function to be called when leaving bounding box of an object previously stopped collision with
   Parameters : ACTORINSTANCE, function in format callback(ACTORINSTANCE *collider, ACTORINSTANCE *object)
   Returns : 
   Info : 
*/

void SetOutsideBoundingBoxCallback(ACTORINSTANCE *actorInstance, OutsideBoundingBox callback )
{
	actorInstance->outsideBoundingBox = callback;
}

/* --------------------------------------------------------------------------------
   Function : DrawActorInstanceZones
   Purpose : draws actor instance zones (yMin, maxRadius, etc..)
   Parameters : actorInstanceList
   Returns : 
   Info : 
*/

void DrawActorInstanceZones( ACTORINSTANCELIST *actorInstanceList )
{
	TBPrimVertex	zoneVerts[8];

	float yMin, yMax, xMin, xMax, zMin, zMax,maxRadius;
	float counter;
	TBMatrix			resultMat;
	float				instanceDistance;
	uint32				vertCounter;

	ACTORINSTANCE *ptr;
	
	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		instanceDistance = bmVectorDistance(ptr->actorInstance.position , camera->cameraState.cameraPos);

		if ((!(ptr->flags & (ACTORFLAG_DONTDRAW|ACTORFLAG_CUSTOMDRAW)))&&(ptr->currentAlpha)&&(ptr->onScreen)&&(instanceDistance<4000.0f))
		{	
			bdSetIdentityObjectMatrix();

			bmMatTranslate(resultMat, (float)ptr->actorInstance.position[X], (float)ptr->actorInstance.position[Y], (float)ptr->actorInstance.position[Z]);
			bdSetObjectMatrix(resultMat);

			xMin = -ptr->actorInstance.actor->xMin;
			xMax = -ptr->actorInstance.actor->xMax;
			yMin = ptr->actorInstance.actor->yMin;
			yMax = ptr->actorInstance.actor->yMax;
			zMin = ptr->actorInstance.actor->zMin;
			zMax = ptr->actorInstance.actor->zMax;

			maxRadius = ptr->actorInstance.actor->maxRadius;

			for (vertCounter = 0; vertCounter < 8; vertCounter++)
			{
				BDVERTEX_SETRGBA( zoneVerts + vertCounter,   255, 0, 0, 255 );
			}

			// Rear Lower Right
			BDVERTEX_SETXYZW( zoneVerts, xMin, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMin+25.0f, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMin, zMin+25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Rear Lower Left
			BDVERTEX_SETXYZW( zoneVerts, xMax, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMin+25.0f, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMin, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMin, zMin+25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Rear Upper Left
			BDVERTEX_SETXYZW( zoneVerts, xMax, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMax-25.0f, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMax, zMin+25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Rear Upper Right
			BDVERTEX_SETXYZW( zoneVerts, xMin, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMax-25.0f, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMax, zMin, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMax, zMin+25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Front Lower Right
			BDVERTEX_SETXYZW( zoneVerts, xMin, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMin+25.0f, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMin, zMax-25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Front Lower Left
			BDVERTEX_SETXYZW( zoneVerts, xMax, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMin+25.0f, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMin, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMin, zMax-25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Front Upper Left
			BDVERTEX_SETXYZW( zoneVerts, xMax, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMax-25.0f, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMax, zMax-25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			// Front Upper Right
			BDVERTEX_SETXYZW( zoneVerts, xMin, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMax-25.0f, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMax, zMax, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMax, zMax-25.0f, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			for (vertCounter = 0; vertCounter < 6; vertCounter++)
			{
				BDVERTEX_SETRGBA( zoneVerts + vertCounter,   0, 0, 255, 255 );
			}

			// X Axis
			BDVERTEX_SETXYZW( zoneVerts, maxRadius, 0.0f, 0.0f, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+1, -maxRadius, 0.0f, 0.0f, 1.0f );
			// Y Axis
			BDVERTEX_SETXYZW( zoneVerts+2, 0.0f, maxRadius, 0.0f, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+3, 0.0f, -maxRadius, 0.0f, 1.0f );
			// Z Axis
			BDVERTEX_SETXYZW( zoneVerts+4, 0.0f , 0.0f, maxRadius, 1.0f );
			BDVERTEX_SETXYZW( zoneVerts+5, 0.0f, 0.0f, -maxRadius, 1.0f );
			bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

			for (vertCounter = 0; vertCounter < 8; vertCounter++)
			{
				BDVERTEX_SETRGBA( zoneVerts + vertCounter,   255, 255, 0, 255 );
			}

			// Meter markers
			for (counter = 0; counter <= maxRadius; counter += (uint32)METERTOUNIT(1))
			{
				BDVERTEX_SETXYZW( zoneVerts, 10.0f , 0.0f, counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+1, -10.0f, 0.0f, counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+2, 0.0f , 10.0f, counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+3, 0.0f, -10.0f, counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+4, 10.0f , 0.0f, -counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+5, -10.0f, 0.0f, -counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+6, 0.0f , 10.0f, -counter, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+7, 0.0f, -10.0f, -counter, 1.0f );
				bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	

				BDVERTEX_SETXYZW( zoneVerts, 0.0f , counter, 10.0f , 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+1, 0.0f, counter, -10.0f , 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+2, 10.0f, counter, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+3, -10.0f, counter, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+4, 0.0f , -counter, 10.0f , 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+5, 0.0f, -counter, -10.0f , 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+6, 10.0f, -counter, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+7, -10.0f, -counter, 0.0f, 1.0f );
				bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	

				BDVERTEX_SETXYZW( zoneVerts, counter, 0.0f, 10.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+1, counter, 0.0f, -10.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+2, counter, 10.0f, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+3, counter, -10.0f, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+4, -counter, 0.0f, 10.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+5, -counter, 0.0f, -10.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+6, -counter, 10.0f, 0.0f, 1.0f );
				BDVERTEX_SETXYZW( zoneVerts+7, -counter, -10.0f, 0.0f, 1.0f );
				bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	
			}
		}
	}
}