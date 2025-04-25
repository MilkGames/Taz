// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : cameracase.cpp
//   Purpose : camera case functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "camera.h"
#include "cameracase.h"
#include "mapfile.h"
#include "main.h"
#include "scene.h"
#include "maths.h"
#include "display.h"
#include "fade.h"
#include "control.h"
#include "KOScam.h"					// PP: "Keep-On-Screen" camera
#include "files.h"
#include "cutscene.h"
#include "trigger.h"
#include "status.h"
#include "music.h"
#include "physics.h"
#include "util.h"


CAMERACASELIST	cameraCaseList;
float			defaultPosSmooth = 0.06f,defaultLookatSmooth = 0.15f;


/* --------------------------------------------------------------------------------
   Function : InitCameraCaseList
   Purpose : intialise the camera case list
   Parameters : 
   Returns : 
   Info : 
*/
void InitCameraCaseList(void)
{
	cameraCaseList.numEntries = 0;
	cameraCaseList.head.next = cameraCaseList.head.prev = &cameraCaseList.head;
}

/* --------------------------------------------------------------------------------
   Function : CreateCameraCase
   Purpose : create a camera case structure and initialise
   Parameters : name, type
   Returns : cameraCase
   Info : 
*/
CAMERACASE *CreateCameraCase(char *name,int32 type)
{
	CAMERACASE *cameraCase,*ptr;

	ptr = &cameraCaseList.head;
	
	cameraCase = (CAMERACASE*)ZALLOC(sizeof(CAMERACASE));
	if (!cameraCase) return NULL;

	cameraCase->next = ptr;
	cameraCase->prev = ptr->prev;
	ptr->prev->next = cameraCase;
	ptr->prev = cameraCase;
	cameraCaseList.numEntries++;

	cameraCase->caseName = bkCRC32((uchar*)name,strlen(name),0);

	cameraCase->camera = NULL;

	cameraCase->type = type;
	
	cameraCase->cameraFollow = NULL;
	cameraCase->lookAtFollow = NULL;
	cameraCase->cameraNode = NULL;
	cameraCase->lookAtNode = NULL;

	cameraCase->caseTime = cameraCase->time = 0.0f;
	cameraCase->suspend = 0;
	cameraCase->flags = CAMERACASEFLAG_SMOOTH|CAMERACASE_TRANSITIONCONTROL;

	cameraCase->cameraDest = NULL;
	cameraCase->onStart = NULL;
	cameraCase->onEnd = NULL;
	cameraCase->lockTazCRC = 0;
	cameraCase->startDelay = 0.0f;

	// PP: initialise pointers to type-specific data
	cameraCase->typeSpecificData=NULL;

	return cameraCase;
}

/* --------------------------------------------------------------------------------
   Function : CameraCasePan
   Purpose : Create a pan camera
   Parameters : cameraCase, lookat,lookat node, start pos, end pos, time
   Returns : 
   Info : 
*/
void CameraCasePan(CAMERACASE *cameraCase,char *lookAt,char *lookAtNode,TBVector pan1,TBVector pan2,float time)
{
	ACTORINSTANCE *actorInstance;

	cameraCase->lookAtFollow = bkCRC32((uchar*)lookAt,strlen(lookAt),0);
	if (lookAtNode)
	{
		actorInstance = FindActorInstanceInMap(lookAt,NULL,&map);
		cameraCase->lookAtNode = bkCRC32((uchar*)lookAtNode,strlen(lookAtNode),0);
	}
	bmVectorCopy(cameraCase->pan1,pan1);
	bmVectorCopy(cameraCase->pan2,pan2);
	cameraCase->caseTime = time;
}

/* --------------------------------------------------------------------------------
   Function : CameraCasePan
   Purpose : Create a pan camera
   Parameters : cameraCase, lookat,lookat node, start pos, end pos, time
   Returns : 
   Info : 
*/
void CameraCasePan(CAMERACASE *cameraCase,uint32 lookAt,uint32 lookAtNode,uint32 pan1,uint32 pan2,float time)
{
	cameraCase->lookAtFollow = lookAt;
/*	if (lookAtNode)
	{
		actorInstance = FindActorInstanceInMap(NULL,lookAt,&map);
		if (actorInstance)
		{
			cameraCase->lookAtNode = lookAtNode;//baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance,lookAtNode);
		}
	}
*/	cameraCase->lookAtNode = lookAtNode;
	cameraCase->cameraFollow = pan1;
	cameraCase->cameraDest = pan2;
	cameraCase->caseTime = time;
}

/* --------------------------------------------------------------------------------
   Function : CameraCaseFixedPoint
   Purpose : Create a fixed point camera
   Parameters : cameraCase, cameraFollow, lookAtFollow
   Returns : 
   Info : 
*/
void CameraCaseFixedPoint(CAMERACASE *cameraCase,char *cameraFollow,char *lookAtFollow,float time)
{
	cameraCase->cameraFollow = bkCRC32((uchar*)cameraFollow,strlen(cameraFollow),0);
	cameraCase->lookAtFollow = bkCRC32((uchar*)lookAtFollow,strlen(lookAtFollow),0);
	cameraCase->caseTime = time;
}

/* --------------------------------------------------------------------------------
   Function : CameraCaseFixedPoint
   Purpose : Create a fixed point camera
   Parameters : cameraCase, cameraFollow, lookAtFollow
   Returns : 
   Info : 
*/
void CameraCaseFixedPoint(CAMERACASE *cameraCase,uint32 cameraFollow,uint32 lookAtFollow,float time,uint32 lookAtNode,uint32 cameraNode)
{
	cameraCase->cameraFollow = cameraFollow;
	cameraCase->lookAtFollow = lookAtFollow;
	cameraCase->lookAtNode = lookAtNode;
	cameraCase->cameraNode = cameraNode;
	cameraCase->caseTime = time;
}

/* --------------------------------------------------------------------------------
   Function : CameraCaseRotateAround
   Purpose : Create a rotate around camera case
   Parameters : cameraCase, lookAtFollow, radius
   Returns : 
   Info : 
*/
void CameraCaseRotateAround(CAMERACASE *cameraCase,char *lookAtFollow,float radius,float angle,char flags)
{
	cameraCase->lookAtFollow = bkCRC32((uchar*)lookAtFollow,strlen(lookAtFollow),0);
	cameraCase->radius = radius;
	cameraCase->angle = angle;
	cameraCase->flags = flags;
}

/* --------------------------------------------------------------------------------
   Function : CameraCaseRotateAround
   Purpose : Create a rotate around camera case
   Parameters : cameraCase, lookAtFollow, radius
   Returns : 
   Info : 
*/
void CameraCaseRotateAround(CAMERACASE *cameraCase,uint32 lookAtFollow,float radius,float angle,char flags)
{
	cameraCase->lookAtFollow = lookAtFollow;
	cameraCase->radius = radius;
	cameraCase->angle = angle;
	cameraCase->flags = flags;
}


/* --------------------------------------------------------------------------------
   Function : CameraCaseKeepOnScreen
   Purpose : Create a camera that keeps a list of actor instances on-screen
   Parameters : cameraCase, camera angle
   Returns : 
   Info : 
*/
void CameraCaseKeepOnScreen(CAMERACASE* const cameraCase, const TBVector angle)
{
	cameraCase->KOScamData=new KOSCAMDATA(angle);
	ASSERT(cameraCase->KOScamData);
}

/* --------------------------------------------------------------------------------
   Function : CameraCaseAttribs
   Purpose : Set the camera case attribs
   Parameters : cameraCase, suspendflags
   Returns : 
   Info : 
*/
void CameraCaseAttribs(CAMERACASE *cameraCase, uint32 suspend, uint32 flags)
{
	cameraCase->suspend = suspend;
	cameraCase->flags = flags;
}

/*  --------------------------------------------------------------------------------
	Function	: CameraCaseStartDelay
	Purpose		: Sets the delay prior to the cameracase starting
	Parameters	: cameracase, time in seconds
	Returns		: 
	Info		: 
*/
void CameraCaseStartDelay(CAMERACASE *cameraCase, float time)
{
	cameraCase->startDelay = time;
}

/* --------------------------------------------------------------------------------
   Function : DeleteCameraCase
   Purpose : remove and delete a camera case
   Parameters : cameraCase
   Returns : 
   Info : 
*/
void DeleteCameraCase(CAMERACASE *cameraCase)
{
	// PP: It's handy to be able to call this function without having to check if the camera case is NULL, so check now
	if(cameraCase == NULL)
	{
		return;
	}

	cameraCase->prev->next = cameraCase->next;
	cameraCase->next->prev = cameraCase->prev;
	cameraCaseList.numEntries--;

	// PP: clear up any type-specific data...
	switch(cameraCase->type)
	{
	case CAMERACASETYPE_KEEPONSCREEN:
		{
			// PP: delete keep-on-screen data if appropriate
			SAFE_DELETE(cameraCase->KOScamData);
		}
	}

	if (cameraCase->onStart) 
	{
		DeleteTriggerInfo(cameraCase->onStart);
		cameraCase->onStart = NULL;
	}

	if (cameraCase->onEnd) 
	{
		DeleteTriggerInfo(cameraCase->onEnd);
		cameraCase->onEnd = NULL;
	}

	SAFE_FREE(cameraCase->camera);
	SAFE_FREE(cameraCase);
}

/* --------------------------------------------------------------------------------
   Function : DeleteAllCameraCases
   Purpose : remove and delete all camera cases
   Parameters : 
   Returns : 
   Info : 
*/
void DeleteAllCameraCases(void)
{
	CAMERACASE *ptr,*temp;
	
	ptr = cameraCaseList.head.next;

	for (ptr = cameraCaseList.head.next;ptr != &cameraCaseList.head;)
	{
		temp = ptr->next;

		DeleteCameraCase(ptr);

		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function : FindCameraCase
   Purpose : locate a camera case in the cameraCaseList
   Parameters : name
   Returns : CAMERACASE
   Info : 
*/
CAMERACASE *FindCameraCase(char *name)
{	
	CAMERACASE *ptr;
	uint32		crc;
	
	ptr = cameraCaseList.head.next;

	crc = bkCRC32((uchar*)name,strlen(name),0);

	for (ptr = cameraCaseList.head.next;ptr != &cameraCaseList.head;ptr = ptr->next)
	{
		if (ptr->caseName==crc) return ptr;
	}
	return NULL;
}


/* --------------------------------------------------------------------------------
   Function : FindCameraCase
   Purpose : locate a camera case in the cameraCaseList
   Parameters : name crc
   Returns : CAMERACASE
   Info : 
*/
CAMERACASE *FindCameraCase(uint32 name)
{
	CAMERACASE *ptr;
	uint32		crc;
	
	ptr = cameraCaseList.head.next;

	crc = name;

	for (ptr = cameraCaseList.head.next;ptr != &cameraCaseList.head;ptr = ptr->next)
	{
		if (ptr->caseName==crc) return ptr;
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : ProcessCameraMessages
   Purpose : process any camera messages sent
   Parameters : message
   Returns : 
   Info : 
*/
void ProcessCameraMessages(char *message)
{
	char *type,*mes;
	CAMERACASE *cameraCase;

	type = message;

	mes = FindChar(message, '=');
	if(mes == NULL) 
	{
		if (strcmpi(type,"caseend")==0)
		{
			// TP: PROBLEM need to know which player triggered
			if (camera[0].cameraCase)
			{
				StopCameraCase(&camera[0],camera[0].cameraCase);
			}
		}
		return;
	}
	*mes++ = 0;

	if (strcmpi(type,"case")==0)
	{
		cameraCase = FindCameraCase(mes);
		if (cameraCase)
		{
			if ((cameraCase->startDelay)&&(strcmpi(mes,"outrocam1")))
			{
				camera[0].crcQueuedCase = bkCRC32((uchar*)mes,strlen(mes),0);
				camera[0].queueDelay = cameraCase->startDelay;
			}
			else
			{
				if (map.sceneNumber == SCENE_DESTRUCTIONAQUA && gameStatus.multiplayer.versusCPU == false)
				{}	// JW: Whaddya know...another nasty kludge
				else
					StartCameraCase(cameraCase,&camera[0]);
			}
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : StartCameraCase
   Purpose : set up a camera case
   Parameters : cameraCase
   Returns : TRUE/FALSE if case could start
   Info : 
*/
int StartCameraCase(CAMERACASE *cameraCase,CAMERAINFO *camera)
{
	ACTORINSTANCE		*actorInstance,*tempInstance;
	TBVector			tempVector;
	TBActorNodeInstance	*node;
	char				message[256];
	bool				cameraPosSnap=true;
	bool				cameraLookatSnap=true;

/* PP: REMOUT: TEST.  I need to use camera cases in 2-player West Boss
	if (gameStatus.multiplayer.numPlayers == 2)
		return FALSE;*/

	if (camera->cameraCase)
	{
		// TP: cameracase is already playing so end old one
		StopCameraCase(camera, camera->cameraCase);

		// TP: what if StopCameraCase starts another camera case using an onend message
		if (camera->cameraCase)
		{
			// TP: dont allow this chain to continue
			bkPrintf("*** WARNING *** Camera chain interrupted\n");
			if (camera->cameraCase->camera)
			{
				DestroyCamera(camera->cameraCase->camera);
				SAFE_FREE(camera->cameraCase->camera);
				camera->cameraCase->camera = NULL;
				camera->cameraCase = NULL;
			}
		}
	}

	cameraCase->camera = (CAMERAINFO*)ZALLOC(sizeof(CAMERAINFO));
	if (!cameraCase->camera) return FALSE;

	InitCamera(cameraCase->camera,camera->pos,camera->lookAt);

	// TP: remove smart cam during camera cases
	cameraCase->camera->flags &= ~CAMERA_SMARTCAM;

	if (cameraCase->flags&CAMERACASE_CAMERAPOSSMOOTH) cameraPosSnap = false;
	if (cameraCase->flags&CAMERACASE_CAMERALOOKATSMOOTH) cameraLookatSnap = false;

	// TP: if player should be frozen in position
	if (cameraCase->lockTazCRC)
	{
		ACTORINSTANCE *actorInstance;

		actorInstance = FindActorInstanceInMap(NULL,cameraCase->lockTazCRC,&map);

		if (actorInstance)
		{
			bmVectorCopy(gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);
			bmQuatCopy(gameStatus.player1->actorInstance.orientation,actorInstance->actorInstance.orientation);
			bmVectorSet(gameStatus.player1->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			switch(gameStatus.player1->actorStatus->currentState)
			{
			case STATE_BALL:
				break;
			default:
				EnterState(gameStatus.player1, STATE_MOVE);
				break;
			}
			cameraCase->suspend|=SUSPENDFLAG_TAZ;
			baPrepActorInstance(&gameStatus.player1->actorInstance,0);

			// TP: turn off collision and velocity processing on taz to stop him from moving until cutscene is finished
			gameStatus.player1->flags |= ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;
		}
		else
		{
			bkPrintf("*** WARNING *** Could not find hold taz at point for cameracase.");
		}
	}

	if (cameraCase->suspend&SUSPENDFLAG_TAZ) gameStatus.controlSuspend = TRUE;

	if (cameraCase->onStart)
	{
		bmVectorCopy(tempVector,camera->pos);
		// send messages
		sprintf(message,"_POS=%d %d %d",bmFloatToInt(tempVector[X]),bmFloatToInt(tempVector[Y]),bmFloatToInt(tempVector[Z]));
		bkGenerateEvent("sound",message);
		if (cameraCase->onStart) SendTrigger(cameraCase->onStart);
	}

//	if (camera->fade.status == FADESTATE_OFF) StartFadeUp(&camera->fade,FADETYPE_NORMAL,0.5f);

	switch(cameraCase->type)
	{
	case CAMERACASETYPE_FIXEDPOINT:
		if (cameraCase->cameraFollow==NULL)
		{
			actorInstance = FindActorInstanceInMap(NULL,cameraCase->caseName,&map);
		}
		else
		{
			actorInstance = FindActorInstanceInMap(NULL,cameraCase->cameraFollow,&map);
		}
		
		if (actorInstance)
		{
			if (cameraCase->cameraNode)
			{
				node = baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance,cameraCase->cameraNode);
			}
			else
			{
				node = 0;
			}
			SetCameraPosFollowActorInstanceNode(cameraCase->camera,&actorInstance->actorInstance,node);
			if (cameraPosSnap)
			{
				if (node)
				{
					baGetNodesWorldPosition(cameraCase->camera->actorInstancePos, cameraCase->camera->actorInstanceNodePos, tempVector);
					SetCameraPosition(cameraCase->camera, tempVector);
				}
				else
				{
					SetCameraPosition(cameraCase->camera,actorInstance->actorInstance.position);
				}
			}
			if (cameraCase->lookAtFollow)
			{
				actorInstance = FindActorInstanceInMap(NULL,cameraCase->lookAtFollow,&map);
				if (actorInstance)
				{
					if (cameraCase->lookAtNode)
					{
						node = baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance,cameraCase->lookAtNode);
					}
					else
					{
						node = 0;
					}
					SetCameraLookAtFollowActorInstance(cameraCase->camera,actorInstance,&actorInstance->actorInstance,node);
					if (cameraLookatSnap)
					{
						if (node)
						{
							baGetNodesWorldPosition(cameraCase->camera->actorInstanceLookAt, cameraCase->camera->actorInstanceNodeLookAt, tempVector);
							SetCameraLookAtPosition(cameraCase->camera, tempVector);
						}
						else
						{
							SetCameraLookAtPosition(cameraCase->camera,actorInstance->actorInstance.position);
						}
					}
				}
				else
				{
					SetCameraLookAtFollowActorInstance(cameraCase->camera,actorInstance,&gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
					if (cameraLookatSnap)
					{
						SetCameraLookAtPosition(cameraCase->camera,gameStatus.player1->actorInstance.position);
					}
				}
			}
			else
			{
				if (map.sceneNumber == SCENE_DESTRUCTIONAQUA && gameStatus.multiplayer.versusCPU == false)
				{}	// JW: This is a nasty kludge but it'll do for test puposes...i hope
				else
				{
					SetCameraLookAtFollowActorInstance(cameraCase->camera,actorInstance,&gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
					if (cameraLookatSnap)
					{
						SetCameraLookAtPosition(cameraCase->camera,gameStatus.player1->actorInstance.position);
					}
				}
			}
			if (cameraCase->flags&CAMERACASEFLAG_SMOOTH)
			{
				SetCameraSmooth(cameraCase->camera,defaultPosSmooth,defaultLookatSmooth);
			}
			else
			{
				SetCameraSmooth(cameraCase->camera,1.0f,1.0f);
			}
			SetCameraCase(camera,cameraCase);
			return TRUE;
		}
		break;

	case CAMERACASETYPE_ROTATEAROUND:
		actorInstance = FindActorInstanceInMap(NULL,cameraCase->lookAtFollow,&map);
		if (actorInstance)
		{
			SetCameraLookAtFollowActorInstance(cameraCase->camera,actorInstance,&actorInstance->actorInstance,(TBActorNodeInstance *)NULL);
			if (cameraLookatSnap)
			{
				SetCameraLookAtPosition(cameraCase->camera,actorInstance->actorInstance.position);
			}

			SetCameraPosFollowActorInstance(cameraCase->camera,&gameStatus.player1->actorInstance,NULL);
			if (cameraPosSnap)
			{
				SetCameraPosition(cameraCase->camera,gameStatus.player1->actorInstance.position);
			}

			if (cameraCase->flags&CAMERACASEFLAG_SMOOTH)
			{
				SetCameraSmooth(cameraCase->camera,defaultPosSmooth,defaultLookatSmooth);
			}
			else
			{
				SetCameraSmooth(cameraCase->camera,1.0f,1.0f);
			}
			EnableCameraAngleAboveHorizontal(cameraCase->camera,cameraCase->angle);
			//if fixed distance from taz flagged, calculate appropiate distance
			if(cameraCase->flags & CAMERACASE_FIXEDDISTANCEFROMTAZ)
			{
				bmVectorSub(tempVector, gameStatus.player1->actorInstance.position, actorInstance->actorInstance.position);
				SetFixedDistanceToLookAt(cameraCase->camera, (cameraCase->radius+bmVectorLen(tempVector)));
			}
			else
			{
				SetFixedDistanceToLookAt(cameraCase->camera,cameraCase->radius);
			}

			SetCameraCase(camera,cameraCase);
			return TRUE;
		}
		break;

	case CAMERACASETYPE_PAN:
		actorInstance = FindActorInstanceInMap(NULL,cameraCase->lookAtFollow,&map);
		if (actorInstance)
		{
			tempInstance = NULL;
			if (cameraCase->cameraFollow) 
			{
				tempInstance = FindActorInstanceInMap(NULL,cameraCase->cameraFollow,&map);
				if (tempInstance) bmVectorCopy(cameraCase->pan1,tempInstance->actorInstance.position);
				else
				{
					bkPrintf("*** WARNING *** Could not find cameraFollow object in pan camera: %X \n",cameraCase->caseName);
				}
			}
			else
			{
				tempInstance = FindActorInstanceInMap(NULL,cameraCase->caseName,&map);
				if (tempInstance) bmVectorCopy(cameraCase->pan1,tempInstance->actorInstance.position);
				else
				{
					bkPrintf("*** WARNING *** Could not find cameraFollow object in pan camera: %X \n",cameraCase->caseName);
				}
			}
			if (cameraCase->cameraDest) 
			{
				tempInstance = FindActorInstanceInMap(NULL,cameraCase->cameraDest,&map);
				if (tempInstance) bmVectorCopy(cameraCase->pan2,tempInstance->actorInstance.position);
				else
				{
					bkPrintf("*** WARNING *** Could not find cameraDest object in pan camera: %X \n",cameraCase->caseName);
				}
			}

			if (cameraPosSnap)
			{
				SetCameraPosition(cameraCase->camera,cameraCase->pan1);
			}
			SetCameraPosFollowActorInstance(cameraCase->camera,NULL,NULL);
			if (cameraCase->lookAtFollow)
			{
				actorInstance = FindActorInstanceInMap(NULL,cameraCase->lookAtFollow,&map);
				if (cameraLookatSnap)
				{
					SetCameraLookAtPosition(cameraCase->camera,actorInstance->actorInstance.position);
				}
				if (actorInstance)
				{
					if (cameraCase->lookAtNode)
					{
						node = baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance,cameraCase->lookAtNode);
					}
					else
					{
						node = 0;
					}
					SetCameraLookAtFollowActorInstance(cameraCase->camera,actorInstance,&actorInstance->actorInstance,node);
				}
				else
				{
					SetCameraLookAtFollowActorInstance(cameraCase->camera,gameStatus.player1,&gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
				}
			}
			else
			{
				SetCameraLookAtFollowActorInstance(cameraCase->camera,gameStatus.player1,&gameStatus.player1->actorInstance,(TBActorNodeInstance *)NULL);
				if (cameraLookatSnap)
				{
					SetCameraLookAtPosition(cameraCase->camera,gameStatus.player1->actorInstance.position);
				}
			}
			if (cameraCase->flags&CAMERACASEFLAG_SMOOTH)
			{
				SetCameraSmooth(cameraCase->camera,defaultPosSmooth,defaultLookatSmooth);
			}
			else
			{
				SetCameraSmooth(cameraCase->camera,1.0f,1.0f);
			}
			SetCameraCase(camera,cameraCase);
			return TRUE;
		}
		break;

	case CAMERACASETYPE_KEEPONSCREEN:
		{
			SetCameraCase(camera,cameraCase);
			return TRUE;
		}
		break;
	}

	DestroyCamera(cameraCase->camera);
	SAFE_FREE(cameraCase->camera);

	return FALSE;
}


/*  --------------------------------------------------------------------------------
	Function	: SendCameraCaseMessages
	Purpose		: Send any messages associated with a cameracase start and end
	Parameters	: cameracase
	Returns		: 
	Info		: 
*/

void SendCameraCaseMessages(CAMERAINFO *camera, CAMERACASE *cameraCase)
{
	TBVector	tempVector;
	char		message[256];

	if (cameraCase->onStart)
	{
		if (cameraCase->onStart) SendTrigger(cameraCase->onStart, camera->pos);
	}
	if (cameraCase->onEnd)
	{
		if (cameraCase->onEnd) SendTrigger(cameraCase->onEnd, camera->pos);
	}
}


/* --------------------------------------------------------------------------------
   Function : StopCameraCase
   Purpose : end a camera case
   Parameters : camera
   Returns : 
   Info : 
*/

void StopCameraCase(CAMERAINFO *camera,CAMERACASE *cameraCase, bool newMessages)
{
	TBVector	tempVector;
	char		message[256];

	if ((camera==NULL)||(cameraCase==NULL)) return;
	if (cameraCase->suspend&SUSPENDFLAG_TAZ) gameStatus.controlSuspend = FALSE;
	if (cameraCase->camera==NULL) return;

	if (cameraCase->lockTazCRC)
	{
		// TP: turn collision and velocity functions back on player 1
		gameStatus.player1->flags &= (~(ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY));
	}

	if (cameraCase->onEnd)
	{
		// TP: Send all cameracase end messages
		if (cameraCase->onEnd) SendTrigger(cameraCase->onEnd, camera->pos);
	}

	if (!(cameraCase->suspend&SUSPENDFLAG_TAZ))
	{
		// TP: taz not suspended so smooth back by default
		if (!(cameraCase->flags&CAMERACASE_CAMERAPOSSNAPBACK)) bmVectorCopy(camera->pos,cameraCase->camera->pos);
		if (!(cameraCase->flags&CAMERACASE_CAMERALOOKATSNAPBACK)) bmVectorCopy(camera->lookAt,cameraCase->camera->lookAt);
	}

	if (cameraCase->flags&CAMERACASE_CAMERAPOSSMOOTHBACK)	bmVectorCopy(camera->pos,cameraCase->camera->pos);
	if (cameraCase->flags&CAMERACASE_CAMERALOOKATSMOOTHBACK) bmVectorCopy(camera->lookAt,cameraCase->camera->lookAt);

	SetCameraSmooth(cameraCase->camera,defaultPosSmooth,defaultLookatSmooth);

	cameraCase->time = 0.0f;
	SetCameraCase(camera,NULL);

	if (map.exitCutScene)
	{
		if (cameraCase->flags&CAMERACASE_EXITCUTSCENE)
		{
			// PP: set up and open the books to be displayed during loading
			// TP: openLoadingBooks();

			while (!map.exitCutScene->FinishedLoading())
			{
				DrawLoadLoop();
			}

			// PP: close the books that were displayed during loading
			// TP: Dont need this here closeLoadingBooks();

			//musicSystem.ChooseMusic(map.sceneNumber,MUSICTYPE_OUTRO,MUSICSTATE_START);
			musicSystem.RequestMood(0,STREAMMOOD_FAST);

			map.exitCutScene->Setup();
			map.exitCutScene->Play(&camera[0]);
		}
	}

	if(cameraCase->camera) DestroyCamera(cameraCase->camera);
	SAFE_FREE(cameraCase->camera);
	cameraCase->camera = NULL;

	if (newMessages)
	{
		RespondToCameraMessages();
	}
}


/* --------------------------------------------------------------------------------
   Function : UpdateCameraCase
   Purpose : update any camera case info
   Parameters : cameraCase
   Returns : 
   Info : 
*/

void UpdateCameraCase(CAMERAINFO *camera,CAMERACASE *cameraCase)
{
	float t;
	TBVector tempVector;
	ACTORINSTANCE	*tempInstance;

	if (cameraCase->lockTazCRC)
	{
		ACTORINSTANCE *actorInstance;

		actorInstance = FindActorInstanceInMap(NULL,cameraCase->lockTazCRC,&map);

		if (actorInstance)
		{
			actorInstance->flags |= ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;
			bmVectorCopy(gameStatus.player1->actorInstance.position,actorInstance->actorInstance.position);
			bmQuatCopy(gameStatus.player1->actorInstance.orientation,actorInstance->actorInstance.orientation);
			bmVectorSet(gameStatus.player1->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);
			cameraCase->suspend|=SUSPENDFLAG_TAZ;
		}
	}

	if (showDebugInfo)
	{
		utilDrawLine(cameraCase->camera->pos, cameraCase->camera->lookAt, COLOUR(0,255,0));
		utilDrawLine(cameraCase->camera->destinationPos, cameraCase->camera->destinationLookAt, COLOUR(255,0,0));
	}
	
	switch(cameraCase->type)
	{
	case CAMERACASETYPE_PAN:
		// update pan position
		if (cameraCase->caseTime)
		{
			t = cameraCase->time / cameraCase->caseTime;
		}
		else
		{
			bkPrintf("*** WARNING *** No time specified for panning camera\n");
			t = 0.0f;
		}
		bmVectorLerp(tempVector,cameraCase->pan1,cameraCase->pan2,t);
		SetCameraPositionDestination(cameraCase->camera,tempVector);
		break;
	case CAMERACASETYPE_ROTATEAROUND:
		if(cameraCase->flags & CAMERACASE_FIXEDDISTANCEFROMTAZ)
		{
			tempInstance = FindActorInstanceInMap(NULL,cameraCase->lookAtFollow,&map);
			if(tempInstance)
			{
				bmVectorSub(tempVector, gameStatus.player1->actorInstance.position, tempInstance->actorInstance.position);
				SetFixedDistanceToLookAt(cameraCase->camera, (cameraCase->radius+bmVectorLen(tempVector)));
			}
		}
		break;
	case CAMERACASETYPE_KEEPONSCREEN:
		{
			KOScam_update(cameraCase);
		}
		break;
	}

	UpdateCamera(cameraCase->camera);
	cameraCase->time += fTime;

	if (cameraCase->caseTime)
	{
//		cameraCase->time += 1.0f/fps;
		if (cameraCase->time > cameraCase->caseTime)
		{
			StopCameraCase(camera,cameraCase);
		}
	}
}
