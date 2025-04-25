// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : cutscene.cpp
//   Purpose : provides functions to initialise,play and free cutscenes
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "cutscene.h"
#include "files.h"
#include "camera.h"
#include "textures.h"
#include "control.h"
#include "music.h"
#include "memcard.h"
#include "textfx.h"
#include "animation.h"
#include "spineffect.h"
#include "fireball.h"
#include "timer.h"			// PP: timer functions
#include "tweety.h"
#include "PongDust.h"		// PP: Cartoon dust effect, for Elephant Pong
#include "util.h"
#include "display.h"
#include "philscene.h"		// PP: custom scene information

#ifndef PHIL				// PP: I'd like to see the cutscenes for just now
#ifndef INFOGRAMES
//	#define DISABLE_CUTSCENES
//	#define DISABLEHUBINTROS
#endif
#endif// PP: ndef PHIL

#ifndef _MASTER
#define DEBUG_CUTSCENE
#endif

#ifdef DEBUG_CUTSCENE
static float	cutsceneTime;
#endif

ACTORINSTANCE		*CUTSCENE::spinInstance=NULL;	// TP: spin model
bool				CUTSCENE::overlayOn;

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::Init
	Purpose		: Create a cutscene
	Parameters	: 
	Returns		: TRUE/FALSE
	Info		: 
*/
int CUTSCENE::Init(char *objectSource, int flags)
{
	entries.setName("cutscene");
	camera = NULL;
	camLookat = NULL;
	camEye = NULL;
	state = CUTSCENE_INACTIVE;
	if (strlen(objectSource) < 100)
	{
		strcpy(this->objectSourceDir, objectSource);
		sprintf(this->soundSourceDir, "%ssound", objectSource);
		AppendCountryCode(this->soundSourceDir, this->soundSourceDir);
	}
	this->flags = flags;
	pakIndex = NULL;
	soundPakIndex = NULL;
	spinObe[0] = 0;
	spinNode[0] = 0;
	spinEntry = 0;

	tazObe[0] = 0;
	tazNode[0] = 0;
	tazEntry = 0;

	music[0] = 0;
	farPlane = 0.0f;

	overlay.texture = NULL;
	overlay.texName[0] = 0;
	overlayOn = false;
	SetupCallback = NULL;

	fog = true;

	// TP: Add 
	this->AddEntry(ADDINVISIBLEOBJECT,"sound.obe");

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::Load
	Purpose		: Load objects
	Parameters	: 
	Returns		: true/false if file found
	Info		: 
*/
bool CUTSCENE::Load()
{
	char	sound[256];
	bkPrintf("Memory free %i\n",bkHeapFreeSpace(NULL));
	if (~flags&CUTSCENEFLAG_PLACEHOLDER)
	{
		if (!LoadWholePackageBkg(this->objectSourceDir)) return false;

		if (map.sceneNumber!=SCENE_LANGUAGESELECT)
		{
			// TP: cant pre-load sound for language select exit cutscene because it won't be in the right language on exit
			LoadWholePackageBkg(this->soundSourceDir);
		}

		state = CUTSCENE_LOADED;

		// PP: Hmm, I thought I'd added this already(?)
		return true;// PP: success
	}
	else
	{
		return true;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::FinishedLoading
	Purpose		: Test if the cutscene has finished loading
	Parameters	: 
	Returns		: TRUE if finished
	Info		: 
*/
int CUTSCENE::FinishedLoading()
{
	if (GetCurrentBkgLoadingStatus()==TRUE) return FALSE;
	else return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::FinishedPlaying
	Purpose		: Test if the cutscene has finished playing
	Parameters	: 
	Returns		: TRUE if finished
	Info		: 
*/
int CUTSCENE::FinishedPlaying()
{
	if (flags&CUTSCENEFLAG_PLACEHOLDER) return true;
	if (!camLookat) return TRUE;

	if (state==CUTSCENE_READY) return false;

	if (flags&CUTSCENEFLAG_FADEOUT)
	{
		if (camera->fade.GetFadeState()!=FADESTATE_ON)
		{
			if ((camera->fade.GetFadeState()!=FADESTATE_DOWN)&&((!baIsActorAnimPlaying(&camLookat->actorInstance, camLookat->actorInstance.rootNodeInstance,TRUE)||(flags&CUTSCENEFLAG_SKIP))))
			{
				// TP: cutscene has finished so start fade down
				camera->fade.StartFadeDown(FADETYPE_NORMAL, 0.5f);
			}
			return false;
		}
		return true;
	}
	else
	{
		if (flags&CUTSCENEFLAG_SKIP)
		{
			return true;
		}
		else
		{
			return (1-baIsActorAnimPlaying(&camLookat->actorInstance, camLookat->actorInstance.rootNodeInstance,TRUE));
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::Setup
	Purpose		: Create and remove all entries in the cutscene
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CUTSCENE::Setup()
{
	CUTSCENEENTRY	*entry;
	ACTORINSTANCE	*actorInstance;
	int				i;
	int32			lastHandle;
	uint32			crc;
	TBActor			*actor;

	if ((state==CUTSCENE_ACTIVE)||(state==CUTSCENE_READY)||(state==CUTSCENE_PLAYING))
	{
		bkPrintf("*** WARNING *** Attempt to setup already playing cutscene.\n");
		return;
	}

	if (!FinishedLoading()) return;

	this->pakIndex = FindPackage(this->objectSourceDir);

	if (map.sceneNumber!=SCENE_LANGUAGESELECT)
	{
		// TP: cant pre-load sound for language select exit cutscene because it won't be in the right language on exit
		this->soundPakIndex = FindPackage(this->soundSourceDir);
	}

	if (overlay.texName[0]!=0)
	{
		overlay.texture = bkLoadTexture(NULL, overlay.texName, 0);
	}

	// TP: Ready the sound
	if (soundPakIndex)
	{
		StopAllChannels();
#if BPLATFORM!=GAMECUBE
		bkDeleteAllSamples();
#endif
		bkLoadPackageSamples(soundPakIndex);

		// TP: Free the samples
	#if BPLATFORM==PS2
		FreeWholePackage(soundSourceDir);
	#endif

	#if BPLATFORM == GAMECUBE
		// IH: Can only free a pak that exists
		if( soundPakIndex )
		{
			// IH: Loop until the DMA has finished doing its magic
			while(!bkFreePackageMemory(&soundPakIndex))
			{
				bkUpdate();
			}
		}
	#endif
	}
	else
	{
		if (map.sceneNumber == SCENE_LANGUAGESELECT)
		{
			sprintf(this->soundSourceDir, "%ssound", objectSourceDir);
			AppendCountryCode(this->soundSourceDir, this->soundSourceDir);

			if (LoadWholePackageBkg(this->soundSourceDir))
			{
				while(GetCurrentBkgLoadingStatus()==TRUE)
				{
					DrawLoadLoop(0);
				}
				this->soundPakIndex = FindPackage(this->soundSourceDir);

				StopAllChannels();
#if BPLATFORM!=GAMECUBE
				bkDeleteAllSamples();
#endif

				bkLoadPackageSamples(soundPakIndex);

				// TP: Free the samples
			#if BPLATFORM==PS2
				FreeWholePackage(soundSourceDir);
			#endif

			#if BPLATFORM == GAMECUBE
				// IH: Can only free a pak that exists
				if( soundPakIndex )
				{
					// IH: Loop until the DMA has finished doing its magic
					while(!bkFreePackageMemory(&soundPakIndex))
					{
						bkUpdate();
					}
				}
			#endif
			}
		}
	}

	if (flags&CUTSCENEFLAG_AUTOLOAD)
	{
		if (pakIndex)
		{
			lastHandle = 0;
			while ((lastHandle = bkEnumPackageContents(pakIndex, lastHandle, FOURCC("ACTR"), &crc, NULL)))
			{
				if (crc != bkCRC32((uchar*)"camera.obe",strlen("camera.obe"),0))
				{
					AddEntry(ADDCARTOONOBJECT, crc);
				}
			}
		}
	}

	if (SetupCallback) SetupCallback(this);

	for (i=entries.getSize()-1;i>=0;i--)
	{
		entry = entries.get(i);

		switch(entry->type)
		{
		case REMOVEOBJECT:
			actorInstance = FindActorInstanceInMap(entry->actor, 0, &map);
			if (actorInstance)
			{
				FreeActorInstance(actorInstance);
				actorInstance = NULL;
			}
			else
			{
				bkPrintf("*** WARNING *** could not find instance %s to free in cutscene.\n",entry->actor);
			}
			break;
		case HIDEOBJECT:
		case HIDEOBJECTPERM:
			actorInstance = FindActorInstanceInMap(entry->actor, entry->crc, &map);

			if (actorInstance)
			{
				actorInstance->flags |= ACTORFLAG_DONTDRAW;
			}
			else
			{
				bkPrintf("*** WARNING *** could not find instance %s to hide in cutscene.\n",entry->actor);
			}
			break;

		case UNHIDEOBJECT:
			actorInstance = FindActorInstanceInMap(entry->actor, entry->crc, &map);

			if (actorInstance)
			{
				actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			}
			else
			{
				bkPrintf("*** WARNING *** could not find instance %s to hide in cutscene.\n",entry->actor);
			}
			break;

		case ADDINVISIBLEOBJECT:
		case ADDNORMALOBJECT:
		case ADDCARTOONOBJECT:
		case ADDLINEOBJECT:
			if (entry->crc)
			{
				actor = bkLoadActorByCRC(pakIndex, entry->crc,0);
				if (actor)
				{
					AddActorToActorListByCRC(actor,entry->crc,ACTORTYPE_ACTOR);
					actor = FindActorInActorList(entry->crc);
					if (actor)
					{
						entry->actorInstance = CreateActorInstanceFromActorPtr(&map.animatedInstances, actor, entry->crc, entry->actor);
					}
				}
				else
				{
					bkPrintf("*** WARNING *** could not find instance %s to create in cutscene.\n",entry->actor);
				}
			}
			else
			{
				LoadActor(entry->actor, this->objectSourceDir);
				entry->actorInstance = CreateActorInstance(&map.animatedInstances, entry->actor, entry->actor);
			}
			if (entry->actorInstance)
			{
				ZEROVECTOR(entry->actorInstance->actorInstance.position);
				SETVECTOR(entry->actorInstance->actorInstance.scale,1.0f,1.0f,1.0f,1.0f);
				SETVECTOR(entry->actorInstance->actorInstance.orientation,0.0f,0.0f,0.0f,1.0f);
			}
			else
			{
				bkPrintf("*** WARNING *** could not create instance %s in cutscene.\n",entry->actor);
			}
			break;
		}

		if ((entry->type == ADDCARTOONOBJECT)&&(entry->actorInstance))
		{
			EnableActorCartoonRenderMode(&entry->actorInstance->actorInstance);
			entry->actorInstance->lineThickness = 1.8f;
		}

		if ((entry->type == ADDLINEOBJECT)&&(entry->actorInstance))
		{
			EnableActorLineRenderMode(&entry->actorInstance->actorInstance);
			entry->actorInstance->lineThickness = 1.8f;
		}

		if (entry->type == ADDINVISIBLEOBJECT)
		{
			if (entry->actorInstance)
			{
				entry->actorInstance->flags |= ACTORFLAG_DONTDRAW|ACTORFLAG_ALWAYSPREP;
			}
		}
	}

	// TP: default load camera objects
	LoadActor("camera.obe",this->objectSourceDir);
	camLookat = CreateActorInstance(&map.animatedInstances, "camera.obe", "camera.obe");
	if (camLookat)
	{
				ZEROVECTOR(camLookat->actorInstance.position);
				SETVECTOR(camLookat->actorInstance.scale,1.0f,1.0f,1.0f,1.0f);
				SETVECTOR(camLookat->actorInstance.orientation,0.0f,0.0f,0.0f,1.0f);
				camLookat->flags |= ACTORFLAG_CUSTOMDRAW;
	}
	else
	{
		bkPrintf("*** WARNING *** Could not create camera for cutscene.\n");
	}

	state = CUTSCENE_ACTIVE;
}


/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::StartAnimations
	Purpose		: Start all the cutscene objects animating
	Parameters	: 
	Returns		: 
	Info		: 
*/

void CUTSCENE::StartAnimations()
{
	int		i;
	CUTSCENEENTRY	*entry;

	for (i=entries.getSize()-1;i>=0;i--)
	{
		entry = entries.get(i);

		if ((entry->type == ADDNORMALOBJECT)||(entry->type == ADDCARTOONOBJECT)||(entry->type==ADDLINEOBJECT)||(entry->type == ADDINVISIBLEOBJECT))
		{
			if (entry->actorInstance)
			{
				// TP: play animation
				if (entry->actorInstance->actorInstance.numAnimSegments)
				{
					baQueueNodeAnimNormal(&entry->actorInstance->actorInstance, entry->actorInstance->actorInstance.rootNodeInstance, 
								&entry->actorInstance->actorInstance.animSegments[0], 1024, -1, 0, NULL,NULL);
					baPrepActorInstance(&entry->actorInstance->actorInstance,TRUE);
				}
				else
				{
					bkPrintf("*** WARNING *** Could not find any anim segments on cutscene object %s.\n",entry->actor);
				}
			}
		}
	}

	if (camLookat)
	{
		if (camLookat->actorInstance.numAnimSegments)
		{
			baQueueNodeAnimNormal(&camLookat->actorInstance, camLookat->actorInstance.rootNodeInstance, 
						&camLookat->actorInstance.animSegments[0], 1024, -1, 0, NULL,NULL);
			baPrepActorInstance(&camLookat->actorInstance,TRUE);
		}
		else
		{
			bkPrintf("*** WARNING *** Could not find any anim segments on cutscene camera.\n");
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::Play
	Purpose		: Start playing the cutscene
	Parameters	: camera to apply the  cutscene to
	Returns		: 
	Info		: 
*/

void CUTSCENE::Play(struct TAG_CAMERAINFO *camera)
{
	TBActorNodeInstance *node;
	TBActor				*actor;
	CUTSCENEENTRY	*entry;
	int				i;
	char			tempText[256];

	if ((state==CUTSCENE_READY)||(state==CUTSCENE_PLAYING))
	{
		bkPrintf("*** WARNING *** Attempt to replay already playing cutscene.\n");
		return;
	}

	if (flags&CUTSCENEFLAG_PLACEHOLDER)
	{
		// TP: display placeholder message
		sprintf(tempText,"Cutscene: %s",objectSourceDir);
		DrawPlaceHolderLoop(tempText);
		state = CUTSCENE_PLAYING;
		return;
	}

	if (!pakIndex) return;

	if (state != CUTSCENE_ACTIVE) return;

	// TP: play music
	if (music[0] != 0)
	{
		musicSystem.ChooseMusic(0, MUSICTYPE_MAIN, MUSICSTATE_START, music);
	}

	if(strcmpi(objectSourceDir, "cityhub_intro") == 0)
	{
		bkGenerateEvent("trigger", "_deactivate=fountainvan");
	}

	if (farPlane)
	{
		oldFarPlane = videoMode.farPlane;
		videoMode.farPlane = farPlane;
	}

	this->camera = camera;

	if (camLookat)
	{
		SaveCameraSettings(&this->saveCamera,this->camera);
		SetCameraLookAtFollowActorInstance(this->camera, camLookat, &camLookat->actorInstance,"camtarget");
		SetCameraPosFollowActorInstance(this->camera, &camLookat->actorInstance, "camorigin");
		SetFixedDistanceToLookAt(this->camera,0.0f);
		DisableCameraAngleAboveHorizontal(this->camera);

		this->camera->flags &= ~CAMERA_PADCONTROL;
		this->camera->flags &= ~CAMERA_CATCHUP;
		this->camera->flags &= ~CAMERA_SMARTCAM;

		this->camera->flags |= CAMERA_NOCOLLISION;
		this->camera->flags |= CAMERA_CUTSCENE;
		
		gameStatus.chrisCam = FALSE;

		SetCameraSmooth(this->camera,1.f,1.f);

	}

	gameStatus.controlSuspend = TRUE;

	if (fog==false)
	{
		currentFogState = map.fogState;
		map.fogState = false;
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, FALSE, FALSE);
	}

	for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if (gameStatus.player[i].actorInstance)
		{
			RemoveInstanceFromInstanceList(gameStatus.player[i].actorInstance, &characterInstances);
		}
	}

	Fireball_applyDrawOrder=true;

	CreateSpinEffectForCutScene();
	if (spinEntry)
	{
		spinInstance = CreateActorInstance(&map.animatedInstances,"spindevil.obe","spinny");
		EnableActorCartoonRenderMode(&spinInstance->actorInstance);
		PlayAnimationByName(spinInstance,"spin1",1.0f,1,0,0.0f,NONE);
	}

	if ((gameStatus.enableMusic)&&(music[0]!=0))
	{
		// TP: must wait for the music to kick of before starting the animations
		state = CUTSCENE_READY;
		camera->fade.SetFadeDown();
	}
	else
	{
		StartAnimations();
		FadeUp();
		state = CUTSCENE_PLAYING;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::Stop
	Purpose		: Stop playing the cutscene and free all components
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CUTSCENE::Stop()
{
	CUTSCENEENTRY	*entry;
	ACTORINSTANCE	*actorInstance;
	int				i;

	if (farPlane)
	{
		videoMode.farPlane = oldFarPlane;
	}

	if (!pakIndex)
	{
		// TP: cutscene had not loaded fully so try free the packages
		FreeWholePackage(objectSourceDir);

		StopAllChannels();
#if BPLATFORM!=GAMECUBE
		bkDeleteAllSamples();
#endif
		if (soundSourceDir[0]!=0)
		{
			FreeWholePackage(soundSourceDir);
		}
		return;
	}

	// TP: play music
	if (music[0] != 0)
	{
		musicSystem.ChooseMusic(map.sceneNumber, MUSICTYPE_MAIN, MUSICSTATE_START);
	}

	if (tazEntry)
	{
		switch (map.sceneNumber)
		{
		case SCENE_WILDWESTHUB:	
			
			switch (map.lastScene)
			{
			default:				SETVECTOR(gameStatus.player[0].actorInstance->actorInstance.position, 5158.75f, 2780.24f, 2657.53f, 1.0f);	
									SETQUAT(gameStatus.player[0].actorInstance->actorInstance.orientation, 0.0f, 0.999979f, 0.0f, 0.0065017f);
									break;
			
			case SCENE_GHOSTTOWN:	SETVECTOR(gameStatus.player[0].actorInstance->actorInstance.position, -6184.39f, 2780.24f, 3078.41f, 1.0f);	
									SETQUAT(gameStatus.player[0].actorInstance->actorInstance.orientation, 0.0f, 0.872044f, 0.0f, 0.489427f);
									break;

			case SCENE_GRANDCANYON:	SETVECTOR(gameStatus.player[0].actorInstance->actorInstance.position, -1935.42f, 2821.41f, 8528.73f, 1.0f);	
									SETQUAT(gameStatus.player[0].actorInstance->actorInstance.orientation, 0.0f, 0.616351f, 0.0f, 0.787472f);
									break;
			
			case SCENE_GOLDMINE:	SETVECTOR(gameStatus.player[0].actorInstance->actorInstance.position, 117.439f, 2789.2f, 3334.51f, 1.0f);	
									SETQUAT(gameStatus.player[0].actorInstance->actorInstance.orientation, 0.0f, 0.971003f, 0.0f, -0.239066f);
									break;
			}
			
			break;

		default:	uint32	frame;
		
					if (tazEntry->actorInstance->actorInstance.numAnimSegments)
					{
						frame = tazEntry->actorInstance->actorInstance.animSegments[0].endFrame;
					}
					else
					{
						frame = 0;
					}

					FlushAnimationQueue(tazEntry->actorInstance);

					baSetNodeFrame(&tazEntry->actorInstance->actorInstance, NULL, frame, true);
					baUpdateNodes(&tazEntry->actorInstance->actorInstance,tazEntry->actorInstance->actorInstance.rootNodeInstance,0,NULL);
					baPrepActorInstance(&tazEntry->actorInstance->actorInstance,true);

					taznodeInstance = baFindNode(tazEntry->actorInstance->actorInstance.rootNodeInstance,tazNode);
						
					if (taznodeInstance)
					{
						// JW: Get Taz's final position
						baGetNodesWorldPosition(&tazEntry->actorInstance->actorInstance, taznodeInstance, tazPos);

						bmVectorCopy(gameStatus.player[0].actorInstance->actorInstance.position, tazPos);
					
						// JW: Get Taz's orientation
						TBVector	tempOrig, tempTarg;

						ZEROVECTOR(tempOrig); ZEROVECTOR(tempTarg);

						if (map.lastScene == SCENE_MUSEUM)
							tempTarg[Z] = 1.0f;
						else
							tempTarg[Z] = -1.0f;

						bmMatMultiplyVector(taznodeInstance->nodeToLocalWorld,tempOrig);
						bmMatMultiplyVector(taznodeInstance->nodeToLocalWorld,tempTarg);

						tempOrig[Y] = 0.0f;
						tempTarg[Y] = 0.0f;

						CreateQuatFromPoints(gameStatus.player[0].actorInstance->actorInstance.orientation, tempOrig, tempTarg, 0.0f);
					}

					break;
		}
	}

	if ((map.sceneNumber == SCENE_ZOOHUB && map.lastScene == SCENE_STARTGAMEINTRO)&&(strcmpi(objectSourceDir,"zoohub_intro")==0))
	{
		SETVECTOR(gameStatus.player[0].actorInstance->actorInstance.position, -3493.5f, -4290.1f, 298.9f, 1.0f);	
		SETQUAT(gameStatus.player[0].actorInstance->actorInstance.orientation, 0.0f, 0.925745f, 0.0f, -0.378149f);
		StartTweetyIntro();
	}

	if (state==CUTSCENE_INACTIVE)
	{
		bkPrintf("*** WARNING *** Attempt to stop a cutscene not currently playing\n");
		return;
	}

	if (state==CUTSCENE_LOADED)
	{
		FreeWholePackage(this->objectSourceDir);
		if (soundPakIndex)
		{
			FreeWholePackage(this->soundSourceDir);
		}
		state = CUTSCENE_INACTIVE;
		return;
	}
	for (i=entries.getSize()-1;i>=0;i--)
	{
		entry = entries.get(i);

		if ((entry->type == ADDNORMALOBJECT)||(entry->type == ADDCARTOONOBJECT)||(entry->type == ADDLINEOBJECT)||(entry->type==ADDINVISIBLEOBJECT))
		{
			if (entry->actorInstance)
			{
				FreeActorInstance(entry->actorInstance);
				if (entry->crc)	FreeActor(entry->crc);
				else FreeActor(entry->actor);
				entry->actorInstance = NULL;
			}
		}
		if (entry->type == HIDEOBJECT)
		{
			actorInstance = FindActorInstanceInMap(entry->actor, entry->crc, &map);

			if (actorInstance)
			{
				actorInstance->flags &= ~ACTORFLAG_DONTDRAW;
			}
		}

	}

	if ((this->camera)&&(camLookat))
	{
		RestoreCameraSettings(this->camera,&this->saveCamera,true,false);
	}

	if (camLookat) 
	{
		FreeActorInstance(camLookat);
		FreeActor("camera.obe");
		camLookat = NULL;
	}

	if ((fog==false)&&(currentFogState==true))
	{
		bdSetRenderState(BDRENDERSTATE_DISTANCEFOG, TRUE, FALSE);
		map.fogState = currentFogState;
	}

	Fireball_applyDrawOrder=FB_NORMALLY_SORT_DRAW_ORDER;

	gameStatus.chrisCam = TRUE;

	gameStatus.controlSuspend = FALSE;

	for (i=0;i<gameStatus.multiplayer.numPlayers;i++)
	{
		if (gameStatus.player[i].actorInstance)
		{
			AddInstanceToInstanceList(gameStatus.player[i].actorInstance, &characterInstances);
		}
	}

	// TP: note must update the animated texture list.
	FreeAnimatingTextures();

	FreeWholePackage(this->objectSourceDir);
	if (soundPakIndex)
	{
		FreeWholePackage(this->soundSourceDir);
	}

	ListAnimatingTextures();

	if (spinEntry)
	{
		FreeActorInstance(spinInstance);
	}

	state = CUTSCENE_INACTIVE;
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::~TAG_CUTSCENE
	Purpose		: Default destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
CUTSCENE::~TAG_CUTSCENE()
{
	int	i;
	CUTSCENEENTRY *ptr;

	if (state != CUTSCENE_INACTIVE)
	{
		Stop();
	}

	entries.kill();
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::AddEntry
	Purpose		: Add an entry to the cutscene entry list
	Parameters	: type of entry, crc of item to add or remove
	Returns		: 
	Info		: 
*/
void CUTSCENE::AddEntry(EEntryType type, char *name)
{
	CUTSCENEENTRY	*entry;

	entry = new CUTSCENEENTRY;

	entry->type = type;

	if (strlen(name) < 31)	sprintf(entry->actor,"%s",name);
	entry->crc = 0;
	entry->actorInstance = NULL;

	entries.append(entry);
}

/*  --------------------------------------------------------------------------------
	Function	: CUTSCENE::AddEntry
	Purpose		: Add an entry to the cutscene entry list
	Parameters	: type of entry, crc of item to add or remove
	Returns		: 
	Info		: 
*/
void CUTSCENE::AddEntry(EEntryType type, uint32 crc)
{
	CUTSCENEENTRY	*entry;

	entry = new CUTSCENEENTRY;

	entry->type = type;

	sprintf(entry->actor,"%X",crc);
	entry->crc = crc;

	sprintf(entry->actor,"noname");
	
	entries.append(entry);
}

/*  --------------------------------------------------------------------------------
	Function	: SetSpinNode
	Purpose		: Sets the spin node to be used
	Parameters	: obe name, node name
	Returns		: 
	Info		: 
*/
void CUTSCENE::SetSpinNode(char *obe,char *node)
{
	int i;
	CUTSCENEENTRY	*entry;

	strcpy(spinObe,obe);
	strcpy(spinNode,node);

	for (i=entries.getSize()-1;i>=0;i--)
	{
		entry = entries.get(i);
		if (entry->actor)
		{
			if (strcmpi(entry->actor,spinObe)==0)
			{
				spinEntry = entry;
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: SetSpinNode
	Purpose		: Sets the spin node to be used
	Parameters	: obe name, node name
	Returns		: 
	Info		: 
*/
void CUTSCENE::SetTazNode(char *obe,char *node)
{
	int i;
	CUTSCENEENTRY	*entry;

	strcpy(tazObe,obe);
	strcpy(tazNode,node);

	for (i=entries.getSize()-1;i>=0;i--)
	{
		entry = entries.get(i);
		if (entry->actor)
		{
			if (strcmpi(entry->actor,tazObe)==0)
			{
				tazEntry = entry;
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: Update
	Purpose		: Update the cutscene
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CUTSCENE::Update(void)
{
	TBVector			velocity, position;
	TBActorNodeInstance *node;
	char				*tune;

	if (state==CUTSCENE_READY)
	{
		// TP: Check if music is ready
		if (music[0]!=0)
		{
			// TP:TODO: need safety net incase music never starts properly
			if (musicSystem.IsMusicPlaying(music))
			{
				state = CUTSCENE_PLAYING;
				StartAnimations();
				FadeUp();
			}
			else
			{
				// TP: check the music has been queued and the current tune is fading out
				tune = musicSystem.CurrentTuneQueued();

				if ((!tune)||(strcmpi(tune, music)!=0))
				{
					state = CUTSCENE_PLAYING;
					StartAnimations();
					FadeUp();
				}
			}
		}
		else
		{
			state = CUTSCENE_PLAYING;
			StartAnimations();
			FadeUp();
		}
#ifdef DEBUG_CUTSCENE
		cutsceneTime = 0.0f;
#endif
	}

	if (state!=CUTSCENE_PLAYING) return;

#ifdef DEBUG_CUTSCENE
	cutsceneTime += fTime;
#endif

#ifdef PHIL
	// PP: update my stuff; I wanna look at sumfin
	UpdatePhilScene(map.sceneNumber);
#endif// PP: def PHIL

	if (spinEntry)
	{
		if (spinEntry->actorInstance)
		{
			node = baFindNode(spinEntry->actorInstance->actorInstance.rootNodeInstance,spinNode);
			if (node)
			{
				UpdateCutSceneSpinEffect(&spinEntry->actorInstance->actorInstance,node);
				if (spinInstance)
				{
					bmMatMultiply(spinInstance->actorInstance.objectToWorld,spinEntry->actorInstance->actorInstance.objectToWorld,node->nodeToLocalWorld);
				}
			}
		}
	}

	if (tazEntry)
	{
		if (tazEntry->actorInstance)
		{
			taznodeInstance = baFindNode(tazEntry->actorInstance->actorInstance.rootNodeInstance,tazNode);
			
			if (taznodeInstance)
			{
				baGetNodesWorldPosition(&tazEntry->actorInstance->actorInstance, taznodeInstance, tazPos);
			}
		}
	}

#ifndef CONSUMERDEMO
	// PP: hmmmm, special case, bit dodge
	if(map.sceneNumber == SCENE_ZOOBOSS)
	{
		// PP: update zoo boss dust
		PongDust_update();
	}
#endif
}

/*  --------------------------------------------------------------------------------
	Function	: Draw
	Purpose		: Draw any extras for the cutscene
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CUTSCENE::Draw(void)
{
	TBActorNodeInstance *node;

	if (state!=CUTSCENE_PLAYING) return;

	if (spinEntry)
	{
		if (spinEntry->actorInstance)
		{
			node = baFindNode(spinEntry->actorInstance->actorInstance.rootNodeInstance,spinNode);
			if (node)
			{
				if (spinInstance)
				{
					bmMatMultiply(spinInstance->actorInstance.objectToWorld,spinEntry->actorInstance->actorInstance.objectToWorld,node->nodeToLocalWorld);
				}
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: Draw
	Purpose		: Draw any extras for the cutscene
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CUTSCENE::Draw2(void)
{
	TBActorNodeInstance *node;

	if (state!=CUTSCENE_PLAYING) return;

	if (spinEntry)
	{
		if (spinEntry->actorInstance)
		{
			DrawCutSceneSpinEffect();
		}
	}

	if ((overlayOn)&&(overlay.texture))
	{
		bdSetObjectMatrix(bIdentityMatrix);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
		SETZWRITE(FALSE);
		SETZTEST(ALWAYS);
		ENABLEBLEND(TRUE);
		SETBLEND(BLEND);
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);

		TBVector2D	pos;
		pos[X] = overlay.x;
		pos[Y] = overlay.y;
		utilDraw2DSprite(overlay.texture,pos,96.0f,0.0f,0.0f,COLOUR(128,128,128,128),false,false,NULL,0.0f,1.0f,1.0f);

		SETZWRITE(TRUE);
		SETZTEST(LESSTHAN);
		ENABLEBLEND(TRUE);
		SETBLEND(BLEND);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}

#ifdef DEBUG_CUTSCENE
	// TP: print timer
	char	tempChar[64];
	ushort	string16[64];

	bdSetObjectMatrix(bIdentityMatrix);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);

	sprintf(tempChar, "%0.2f", cutsceneTime);
	bkString8to16(string16, tempChar);
	TextFX_vGrad(128,128,0,110,40,0);
	bdPrintFontLite(standardFont, string16, strlen(tempChar), (float)((videoMode.xScreen-250)/2),(float)((videoMode.yScreen-100)/2), 128,128,128, 128, TextFX_escapeCallback, NULL);
	TextFX_resetColour();

	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
#endif
}

enum ECutScene
{
	CUTSCENE_MAINGAMEINTRO,

	CUTSCENE_ZOOHUBGAMESTART,
	CUTSCENE_ZOOHUBINTRO,
	CUTSCENE_ZOOHUBSAFARI,
	CUTSCENE_ZOOHUBICEDOME,
	CUTSCENE_ZOOHUBAQUA,
	CUTSCENE_ZOOHUBOUTRO,
	CUTSCENE_ZOOBOSSINTRO,
	CUTSCENE_ZOOBOSSOUTRO,

	CUTSCENE_CITYHUBINTRO,
	CUTSCENE_CITYHUBMUSEUM,
	CUTSCENE_CITYHUBCONSTRUCT,
	CUTSCENE_CITYHUBDEPTSTR,
	CUTSCENE_CITYHUBOUTRO,
	CUTSCENE_CITYBOSSINTRO,
	CUTSCENE_CITYBOSSOUTRO,

	CUTSCENE_WESTHUBINTRO,
	CUTSCENE_WESTHUBGRANDC,
	CUTSCENE_WESTHUBGOLDMINE,
	CUTSCENE_WESTHUBGHOST,
	CUTSCENE_WESTHUBOUTRO,
	CUTSCENE_WESTBOSSINTRO,
	CUTSCENE_WESTBOSSOUTRO,

	CUTSCENE_TASMANIAINTRO,

	CUTSCENE_BOSS1INTRO,
	CUTSCENE_BOSS1OUTRO,

	CUTSCENE_BOSS2INTRO,
	CUTSCENE_BOSS2OUTRO,
	CUTSCENE_ENDOFGAME,
};

/*  --------------------------------------------------------------------------------
	Function	: HideActorInstanceList
	Purpose		: Hide all actors on the given instance list
	Parameters	: instance list
	Returns		: 
	Info		: 
*/

void HideActorInstanceList(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;
	
	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		ptr->flags |= ACTORFLAG_DONTDRAW;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ClearBoss2Level
	Purpose		: Hide all objects in the boss2 level
	Parameters	: cutscene
	Returns		: 
	Info		: 
*/

int ClearBoss2Level(CUTSCENE *cutScene)
{
	HideActorInstanceList(&map.levelInstances);
	HideActorInstanceList(&map.animatedInstances);
	map.landscape = NULL;

	// PP: must return a value; you know the drill...
	return 0xB16B00B5;
}


/*  --------------------------------------------------------------------------------
	Function	: SetupCutscene
	Purpose		: Create the cutscene specified
	Parameters	: cutscene
	Returns		: 
	Info		: 
*/
CUTSCENE* SetupCutscene(ECutScene scene)
{
	CUTSCENE	*cutScene = NULL;	

	switch(scene)
	{
	case CUTSCENE_MAINGAMEINTRO:
		cutScene = new CUTSCENE("maingameintro");
		cutScene->AddEntry(ADDNORMALOBJECT,"scenary.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"spindummy.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"pigeon.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"sylvestor.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"wile.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"daffy.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"elmer.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"sam.obe");
		cutScene->SetSpinNode("spindummy.obe","spindummy");
		cutScene->SetMusic("gameintro.wav");
		cutScene->FadeOut(true);
		break;

	case CUTSCENE_ZOOHUBGAMESTART:
		cutScene = new CUTSCENE("zoohub_startgame");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs02_scenicobjects.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs02_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs02_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs02_yosamitesam.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs02_spindevil.obe");
		cutScene->SetSpinNode("cs02_spindevil.obe","spindummy");
		cutScene->FadeOut(true);
		cutScene->Fog(false);
		cutScene->FarPlane(METERTOUNIT(600.0f));
		cutScene->SetOverlay("yosamitvlogo.bmp", (videoMode.xScreen/2)-((videoMode.xScreen/2)*(1.0f-videoMode.safeWidthPortion)), (videoMode.yScreen/2)-((videoMode.yScreen/2)*(1.0f-videoMode.safeHeightPortion)));
		cutScene->SetMusic("taz capture cs.wav");
		break;

	case CUTSCENE_ZOOHUBINTRO:
		cutScene = new CUTSCENE("zoohub_intro");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs03_scenicbits.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs03_spindevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs03_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs03_yosamite.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs03_zookeeper.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs03_posterpusher.obe");
		//cutScene->AddEntry(ADDCARTOONOBJECT,"cs03_zookeeper-hammer.obe");
		//cutScene->AddEntry(ADDCARTOONOBJECT,"cs03_zookeeper-spare hand.obe");
		cutScene->AddEntry(HIDEOBJECT,"tazcage");
		cutScene->SetSpinNode("cs03_spindevil.obe","spindummy");
		cutScene->SetTazNode("cs03_tazdevil.obe","motion_less");
		cutScene->Fog(false);
		cutScene->SetMusic("taz escape cs.wav");
		break;

	case CUTSCENE_ZOOHUBSAFARI:
		cutScene = new CUTSCENE("zoohub_safaridone");
		cutScene->AddEntry(ADDNORMALOBJECT,"gates.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"shadow.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"elephant.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(HIDEOBJECT,"gate");
		cutScene->AddEntry(HIDEOBJECT,"gates01");
		cutScene->AddEntry(HIDEOBJECT,"stompy01");
		cutScene->AddEntry(HIDEOBJECT,"elephant collision box01");
		//cutScene->AddEntry(TRACKBONE, "locomotion");
		cutScene->SetTazNode("taz.obe","locomotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("safari 2 hub.wav");
		break;

	case CUTSCENE_ZOOHUBICEDOME:
		cutScene = new CUTSCENE("zoohub_icedomedone");
		cutScene->AddEntry(ADDNORMALOBJECT,"snowblower.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(HIDEOBJECT,"sbbody");
		//cutScene->AddEntry(TRACKBONE, "lotion");
		cutScene->SetTazNode("taz.obe","lotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("ice 2 hub.wav");
		break;

	case CUTSCENE_ZOOHUBAQUA:
		cutScene = new CUTSCENE("zoohub_aquadone");
		cutScene->AddEntry(ADDNORMALOBJECT,"dinghy.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"dinghytaz.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(HIDEOBJECT,"dinghy");
		//cutScene->AddEntry(TRACKBONE, "lotion");
		cutScene->SetTazNode("taz.obe","lotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("aqua 2 hub.wav");
		break;

	case CUTSCENE_ZOOHUBOUTRO:
		cutScene = new CUTSCENE("zoohub_outro");
		cutScene->AddEntry(REMOVEOBJECT,"gate");
		cutScene->AddEntry(REMOVEOBJECT,"gates01");
		cutScene->AddEntry(REMOVEOBJECT,"stompy01");
		cutScene->AddEntry(REMOVEOBJECT,"elephant collision box01");
		cutScene->AddEntry(REMOVEOBJECT,"dinghy");
		cutScene->AddEntry(REMOVEOBJECT,"sbbody");
		cutScene->AddEntry(ADDNORMALOBJECT,"clouds.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"dinghy.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"gates.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"snowblower.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"stompy.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->FadeOut(true);
		cutScene->SetMusic("zoo hub outro cs.wav");
		break;

	case CUTSCENE_ZOOBOSSINTRO:
		cutScene = new CUTSCENE("zooboss_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs05_elephant.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs05_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs05_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs05_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs05_yosamite.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs05_smokeeffectbox.obe");
		cutScene->AddEntry(HIDEOBJECT,"gossamer.obe");
		cutScene->AddEntry(HIDEOBJECT,"tazdevil.obe");
		cutScene->AddEntry(HIDEOBJECT,"elephant.obe");
		cutScene->AddEntry(HIDEOBJECTPERM,"goal_taz elephant net");
		cutScene->SetMusic("elephant pong intro cs.wav");
		break;

	case CUTSCENE_ZOOBOSSOUTRO:
		cutScene = new CUTSCENE("zooboss_outro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_shedevil.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs06_spindevil.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs06_smokeeffects.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs06_yosamitesam.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_truck-shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_zookeeper1.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_zookeeper2.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs06_zookeeper3.obe");
		cutScene->AddEntry(HIDEOBJECT,"gossamer.obe");
		cutScene->AddEntry(HIDEOBJECT,"tazdevil.obe");
		cutScene->AddEntry(HIDEOBJECT,"elephant.obe");
		cutScene->AddEntry(HIDEOBJECT,"yosemite.obe");
		cutScene->AddEntry(HIDEOBJECT,"truck01");
		cutScene->AddEntry(HIDEOBJECT,"goal_gos");
		cutScene->AddEntry(UNHIDEOBJECT,"goal_taz elephant net");
		cutScene->SetSpinNode("cs06_spindevil.obe","spindummy");
		cutScene->Fog(false);
		cutScene->FadeOut(true);
		cutScene->SetMusic("elephant pong outro cs.wav");
		break;

	case CUTSCENE_CITYHUBINTRO:
		cutScene = new CUTSCENE("cityhub_intro");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs07_spindevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs07_tazdevil.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs07_objects.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs07_armouredvan.obe");
		cutScene->AddEntry(HIDEOBJECT,"all_cars_null");
		cutScene->AddEntry(HIDEOBJECT,"mesh04");
		cutScene->AddEntry(HIDEOBJECT,"tram(reversed)");
		cutScene->AddEntry(HIDEOBJECT,"van_wheels01");

		cutScene->SetSpinNode("cs07_spindevil.obe","spindummy");
		cutScene->SetTazNode("cs07_tazdevil.obe","motion_taz");
		cutScene->SetMusic("city hub intro cs.wav");
		break;

	case CUTSCENE_CITYHUBMUSEUM:
		cutScene = new CUTSCENE("cityhub_museumdone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"tree.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"hotdog.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"doors.obe");
		cutScene->AddEntry(HIDEOBJECT,"hotdog");
		cutScene->AddEntry(HIDEOBJECT,"sausage tree");
		cutScene->AddEntry(HIDEOBJECT,"all_cars_null");
		cutScene->AddEntry(HIDEOBJECT,"mesh04");
		cutScene->AddEntry(HIDEOBJECT,"tram(reversed)");
		cutScene->SetTazNode("taz.obe","lotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("museum 2 hub.wav");
		break;

	case CUTSCENE_CITYHUBDEPTSTR:
		cutScene = new CUTSCENE("cityhub_deptstrdone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"bounce.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"hole.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"trolley.obe");
		cutScene->AddEntry(HIDEOBJECT,"shoptrolleysamsign");
		cutScene->AddEntry(HIDEOBJECT,"shoptrolleynull");
		cutScene->AddEntry(HIDEOBJECT,"trolley02");
		cutScene->AddEntry(HIDEOBJECT,"all_cars_null");
		cutScene->AddEntry(HIDEOBJECT,"mesh04");
		cutScene->AddEntry(HIDEOBJECT,"tram(reversed)");
		cutScene->SetTazNode("taz.obe","lotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("looningdales 2 hub.wav");
		break;

	case CUTSCENE_CITYHUBCONSTRUCT:
		cutScene = new CUTSCENE("cityhub_constructdone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"mixer.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"shadow.obe");
		cutScene->AddEntry(HIDEOBJECT,"cement mixer");
		cutScene->AddEntry(HIDEOBJECT,"all_cars_null");
		cutScene->AddEntry(HIDEOBJECT,"mesh04");
		cutScene->AddEntry(HIDEOBJECT,"tram(reversed)");
		cutScene->SetTazNode("taz.obe","lotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("construction 2 hub.wav");
		break;

	case CUTSCENE_CITYHUBOUTRO:
		cutScene = new CUTSCENE("cityhub_outro");
		cutScene->AddEntry(REMOVEOBJECT,"hotdog");
		cutScene->AddEntry(REMOVEOBJECT,"sausage tree");
		cutScene->AddEntry(REMOVEOBJECT,"shoptrolleysamsign");
		cutScene->AddEntry(REMOVEOBJECT,"shoptrolleynull");
		cutScene->AddEntry(REMOVEOBJECT,"cement mixer");
		cutScene->AddEntry(REMOVEOBJECT,"trolley02");
		cutScene->AddEntry(REMOVEOBJECT,"bank_bigmonkeytree_i");
		cutScene->AddEntry(REMOVEOBJECT,"lamp_i05");
		cutScene->AddEntry(REMOVEOBJECT,"halo06");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"zoocatcher.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"zookeeper.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"hotdog.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"hotdogtree.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"mixer.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"post.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"tree.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"trolley.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"van01.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"van02.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"van03.obe");
		cutScene->AddEntry(HIDEOBJECT,"mesh04");
		cutScene->AddEntry(HIDEOBJECT,"tram(reversed)");
		cutScene->AddEntry(HIDEOBJECT,"all_cars_null");
		cutScene->FadeOut(true);
		cutScene->SetMusic("city hub outro cs.wav");
		break;

	case CUTSCENE_CITYBOSSINTRO:
		cutScene = new CUTSCENE("cityboss_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_daffyduck.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_elmerfudd.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_tazcatcher1.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_tazcatcher2.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_tazdevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_wileecoyote.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs09_yosamitesam.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs09_announcerspeakers.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs09_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper01");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper02");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper03");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper04");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper05");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper06");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper07");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper08");
		cutScene->AddEntry(HIDEOBJECT,"daffy");
		cutScene->AddEntry(HIDEOBJECT,"elmer");
		cutScene->AddEntry(HIDEOBJECT,"atlassphere0");
		cutScene->AddEntry(HIDEOBJECT,"atlassphere1");
		cutScene->AddEntry(HIDEOBJECT,"breakable_glass_right01");
		cutScene->SetMusic("gladiatoons intro cs.wav");
		break;

	case CUTSCENE_CITYBOSSOUTRO:
		cutScene = new CUTSCENE("cityboss_outro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_daffyduck.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_elmerfudd.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_gossamerscrate.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_taskforce1.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_taskforce2.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs10_yosamitesam.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs10_scenicobjects.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs10_announcerspeakers.obe");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper01");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper02");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper03");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper04");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper05");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper06");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper07");
		cutScene->AddEntry(HIDEOBJECT,"mushroom_bumper08");
		cutScene->AddEntry(HIDEOBJECT,"daffy");
		cutScene->AddEntry(HIDEOBJECT,"elmer");
		cutScene->AddEntry(HIDEOBJECT,"atlassphere0");
		cutScene->AddEntry(HIDEOBJECT,"atlassphere1");
		cutScene->FadeOut(true);
		cutScene->SetMusic("gladiatoons outro cs.wav");
		break;

	case CUTSCENE_WESTHUBINTRO:
		cutScene = new CUTSCENE("westhub_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_tazcatcher1.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_tazcatcher2.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_tazcatcher3.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_tazcatcher4.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_wileecoyote.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs11_tazdevil.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs11_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT,"train_root_object01");
		cutScene->AddEntry(HIDEOBJECT,"train_smoke01");
		cutScene->AddEntry(HIDEOBJECT,"cannontrigger");
		cutScene->AddEntry(HIDEOBJECT,"zepllin_null");
		cutScene->SetTazNode("cs11_tazdevil.obe","motion_tazdevil");
		cutScene->Fog(false);
		cutScene->FarPlane(METERTOUNIT(600.0f));
		cutScene->SetMusic("west hub intro cs.wav");
		break;

	case CUTSCENE_WESTHUBOUTRO:
		cutScene = new CUTSCENE("westhub_outro");
		cutScene->AddEntry(ADDNORMALOBJECT,"barriers.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"catcher01.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"catcher02.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"catcher03.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(REMOVEOBJECT,"cart static");
		cutScene->AddEntry(REMOVEOBJECT,"cart move");
		cutScene->AddEntry(REMOVEOBJECT,"minecartdummy");
		cutScene->AddEntry(HIDEOBJECT,"cartgo light");
		cutScene->AddEntry(HIDEOBJECT,"cartstop light");
		cutScene->FadeOut(true);
		cutScene->SetMusic("west hub outro cs.wav");
		break;

	case CUTSCENE_WESTHUBGRANDC:
		cutScene = new CUTSCENE("westhub_grandcdone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"barrier01.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"barrier02.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"dirtybitch.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"dustcloud01.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"dustcloud02.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"rock.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"sign01.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"sign02.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"trackramp.obe");
		cutScene->AddEntry(HIDEOBJECT,"exitrock");
		cutScene->AddEntry(HIDEOBJECT,"mine large buffer01");
		cutScene->AddEntry(HIDEOBJECT,"barrier01");
		cutScene->AddEntry(HIDEOBJECT,"barrier02");
		cutScene->AddEntry(HIDEOBJECT,"barrier01up");
		cutScene->AddEntry(HIDEOBJECT,"barrier02up");
		cutScene->SetTazNode("taz.obe","vlotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("canyon 2 hub.wav");
		break;

	case CUTSCENE_WESTHUBGOLDMINE:
		cutScene = new CUTSCENE("westhub_goldminedone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"minecart.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"track.obe");
		cutScene->AddEntry(HIDEOBJECT,"barrier01");
		cutScene->AddEntry(HIDEOBJECT,"barrier02");
		cutScene->AddEntry(HIDEOBJECT,"barrier01up");
		cutScene->AddEntry(HIDEOBJECT,"barrier02up");
		cutScene->AddEntry(HIDEOBJECT,"cart static");
		cutScene->AddEntry(HIDEOBJECT,"minecartdummy");
		cutScene->SetTazNode("taz.obe","vlotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("goldmine 2 hub.wav");
		break;

	case CUTSCENE_WESTHUBGHOST:
		cutScene = new CUTSCENE("westhub_ghostdone");
		cutScene->AddEntry(ADDCARTOONOBJECT,"taz.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"barrier.obe");
		cutScene->AddEntry(HIDEOBJECT,"barrier01");
		cutScene->AddEntry(HIDEOBJECT,"barrier02");
		cutScene->AddEntry(HIDEOBJECT,"barrier01up");
		cutScene->AddEntry(HIDEOBJECT,"barrier02up");
		cutScene->SetTazNode("taz.obe","vlotion");
		cutScene->FadeOut(true);
		cutScene->SetMusic("ghost 2 hub.wav");
		break;

	case CUTSCENE_WESTBOSSINTRO:
		cutScene = new CUTSCENE("westboss_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs13_tazdevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs13_tnt.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs13_yosamitesam.obe");
		cutScene->AddEntry(HIDEOBJECT,"yosemite.obe");
		cutScene->AddEntry(HIDEOBJECT,"checkpoint");
		cutScene->Fog(false);
		cutScene->SetMusic("bomberman intro cs.wav");
		break;

	case CUTSCENE_WESTBOSSOUTRO:
		cutScene = new CUTSCENE("westboss_outro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs14_tazdevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs14_shedevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs14_yosamitesam.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs14_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT,"yosemite.obe");
		cutScene->AddEntry(HIDEOBJECT,"checkpoint");
		cutScene->FadeOut(true);
		cutScene->Fog(false);
		cutScene->SetMusic("bomberman outro cs.wav");
		break;

	case CUTSCENE_TASMANIAINTRO:
		cutScene = new CUTSCENE("tasmania_intro");
		cutScene->SetTazNode("cs15_tazdevil.obe","motion_taz");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs15_tazdevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs15_tazcatcher01.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs15_tazcatcher02.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs15_spindevil.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs15_scenicobjects.obe");
		cutScene->SetSpinNode("cs15_spindevil.obe","spindummy");
		cutScene->Fog(false);
		cutScene->FadeOut(true);
		cutScene->FarPlane(METERTOUNIT(1500.0f));
		cutScene->SetMusic("tasmania intro cs.wav");
		break;

	case CUTSCENE_BOSS1INTRO:
		cutScene = new CUTSCENE("boss1_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs16_gossamer.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs16_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs16_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs16_tycoonsam.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs16_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT, "glitterball-new01");
		cutScene->AddEntry(HIDEOBJECT, "disco_null");
		cutScene->AddEntry(HIDEOBJECT,"platform_centre01");
		cutScene->Fog(false);
		cutScene->SetMusic("disco volcano intro cs.wav");
		break;

	case CUTSCENE_BOSS1OUTRO:
		cutScene = new CUTSCENE("boss1_outro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs17_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs17_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs17_tycoonsam.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs17_scenicobjects.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs17_rippingscreen.obe");
		cutScene->FadeOut(true);
		cutScene->Fog(false);
		cutScene->FarPlane(METERTOUNIT(600.0f));
		cutScene->SetMusic("disco volcano outro cs.wav");
		break;

	case CUTSCENE_BOSS2INTRO:
		cutScene = new CUTSCENE("boss2_intro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs18_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs18_tazdevil.obe");
		cutScene->AddEntry(ADDLINEOBJECT,"cs18_tycoonsam.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs18_mechatweety.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs18_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT,"mymechatweety");
		cutScene->AddEntry(HIDEOBJECT,"metalgates_main_null_bank01");
		cutScene->AddEntry(HIDEOBJECT,"metalgates_main_null_bank02");
		cutScene->AddEntry(HIDEOBJECT,"shutters_main_04");
		cutScene->Fog(false);
		cutScene->FadeOut(true);
		cutScene->FarPlane(METERTOUNIT(700.0f));
		cutScene->SetMusic("tweety boss intro cs.wav");
		break;

	case CUTSCENE_BOSS2OUTRO:
		cutScene = new CUTSCENE("boss2_outro");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs19_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs19_tazdevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs19_blind.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs19_mechatweety.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs19_scenicobjects.obe");
		cutScene->AddEntry(HIDEOBJECT,"mymechatweety");
		cutScene->Fog(false);
		cutScene->FadeOut(true);
		cutScene->FarPlane(METERTOUNIT(600.0f));
		cutScene->SetMusic("tweety boss outro cs.wav");
		break;

	case CUTSCENE_ENDOFGAME:
		cutScene = new CUTSCENE("endofgame");

		cutScene->SetupCallback = ClearBoss2Level;

		cutScene->AddEntry(ADDCARTOONOBJECT,"cs20_shedevil.obe");
		cutScene->AddEntry(ADDCARTOONOBJECT,"cs20_tazdevil.obe");
		cutScene->AddEntry(ADDINVISIBLEOBJECT,"cs20_spindevil.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs20_staticscenics.obe");
		cutScene->AddEntry(ADDNORMALOBJECT,"cs20_movingscenics.obe");
		cutScene->AddEntry(HIDEOBJECT,"mymechatweety");

		cutScene->SetSpinNode("cs20_spindevil.obe","spindummy");
		cutScene->FadeOut(true);
		cutScene->Fog(false);
		cutScene->FarPlane(METERTOUNIT(600.0f));
		cutScene->SetMusic("tweety boss outro2 cs.wav");
		break;
	}

	return cutScene;
}

/*  --------------------------------------------------------------------------------
	Function	: LoadLevelCutscene
	Purpose		: If the level needs a cutscene to play on starting then this function will start a background load
					if the pakfile and setup the cutscene ready to play.
	Parameters	: sceneNumber
	Returns		: CUTSCENE or NULL
	Info		: 
*/
/*
#define TESTMAP SCENE_SAFARI
#define TESTMAP2 SCENE_AQUA
#define TESTMAP3 SCENE_ICEDOME
*/
/*
#define TESTMAP SCENE_GRANDCANYON
#define TESTMAP2 SCENE_GHOSTTOWN
#define TESTMAP3 SCENE_GOLDMINE
*/
/*
#define TESTMAP SCENE_MUSEUM
#define TESTMAP2 SCENE_DEPTSTR
#define TESTMAP3 SCENE_CONSTRUCT
*/
CUTSCENE* LoadLevelCutScene(int sceneNumber)
{
	CUTSCENE *cutScene = NULL;
	CUTSCENE *cutScenePt2 = NULL;

	map.introCutScene = NULL;
	map.introCutScenePt2 = NULL;
	map.exitCutScene = NULL;
	map.exitCutScenePt2 = NULL;

	if ((map.sceneNumber==SCENE_WESTBOSS)||(map.sceneNumber==SCENE_ZOOBOSS))
	{
		if (gameStatus.multiplayer.numPlayers>1) return NULL;
	}
	if (map.sceneNumber==SCENE_CITYBOSS)
	{
		if (!gameStatus.multiplayer.versusCPU) return NULL;
	}

#ifdef DISABLE_CUTSCENES
	return NULL;
#endif

#ifdef TESTMAP
	gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[TESTMAP].completed = true;
#endif
#ifdef TESTMAP2
	gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[TESTMAP2].completed = true;
#endif
#ifdef TESTMAP3
	gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[TESTMAP3].completed = true;
#endif

	// TP: Don't have any cutscenes in normal levels
	switch(sceneNumber)
	{
	case SCENE_LANGUAGESELECT:
		map.exitCutScene = SetupCutscene(CUTSCENE_MAINGAMEINTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}
		break;

	case SCENE_ZOOHUB:
		// TP: check if all levels are complete then load outro ready if needed
		if (IsLevelComplete(SCENE_SAFARI)&&IsLevelComplete(SCENE_ICEDOME)&&IsLevelComplete(SCENE_AQUA))
		{
			map.exitCutScene = SetupCutscene(CUTSCENE_ZOOHUBOUTRO);
			if (map.exitCutScene) 
			{
				if (!map.exitCutScene->Load())
				{
					SAFE_DELETE(map.exitCutScene);
				}
			}
		}

		// TP: Check for playing safari level complete cutscene
		if (IsLevelComplete(SCENE_SAFARI)&&(map.lastScene==SCENE_SAFARI)&&(!map.startPos))
		{
			// TP: Play safari complete cutscene
			cutScene = SetupCutscene(CUTSCENE_ZOOHUBSAFARI);
			break;
		}

		// TP: Check for playing icedome level complete cutscene
		if (IsLevelComplete(SCENE_ICEDOME)&&(map.lastScene==SCENE_ICEDOME)&&(!map.startPos))
		{
			// TP: Play icedome complete cutscene
			cutScene = SetupCutscene(CUTSCENE_ZOOHUBICEDOME);
			break;
		}

		// TP: Check for playing aqua level complete cutscene
		if (IsLevelComplete(SCENE_AQUA)&&(map.lastScene==SCENE_AQUA)&&(!map.startPos))
		{
			// TP: Play aqua complete cutscene
			cutScene = SetupCutscene(CUTSCENE_ZOOHUBAQUA);
			break;
		}

		// TP: Check if player has not seen the hub intro first
#ifndef DISABLEHUBINTROS
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_ZOOHUBINTRO);
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
#endif
		break;

	case SCENE_ZOOBOSS:
#ifndef DISABLEHUBINTROS
		// TP: get the zoo boss intro ready
		cutScene = SetupCutscene(CUTSCENE_ZOOBOSSINTRO);
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
#endif

		// TP: get the outro ready also
		map.exitCutScene = SetupCutscene(CUTSCENE_ZOOBOSSOUTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}
		break;

	case SCENE_CITYHUB:
		// TP: check if all levels are complete then load outro ready if needed
		if (IsLevelComplete(SCENE_MUSEUM)&&IsLevelComplete(SCENE_DEPTSTR)&&IsLevelComplete(SCENE_CONSTRUCT))
		{
			map.exitCutScene = SetupCutscene(CUTSCENE_CITYHUBOUTRO);
			if (map.exitCutScene) 
			{
				if (!map.exitCutScene->Load())
				{
					SAFE_DELETE(map.exitCutScene);
				}
			}
		}

		// TP: Check for playing museum level complete cutscene
		if (IsLevelComplete(SCENE_MUSEUM)&&(map.lastScene==SCENE_MUSEUM)&&(!map.startPos))
		{
			// TP: Play safari complete cutscene
			cutScene = SetupCutscene(CUTSCENE_CITYHUBMUSEUM);
			break;
		}

		// TP: Check for playing icedome level complete cutscene
		if (IsLevelComplete(SCENE_DEPTSTR)&&(map.lastScene==SCENE_DEPTSTR)&&(!map.startPos))
		{
			// TP: Play icedome complete cutscene
			cutScene = SetupCutscene(CUTSCENE_CITYHUBDEPTSTR);
			break;
		}

		// TP: Check for playing aqua level complete cutscene
		if (IsLevelComplete(SCENE_CONSTRUCT)&&(map.lastScene==SCENE_CONSTRUCT)&&(!map.startPos))
		{
			// TP: Play aqua complete cutscene
			cutScene = SetupCutscene(CUTSCENE_CITYHUBCONSTRUCT);
			break;
		}

		// TP: Check if player has not seen the hub intro first
#ifndef DISABLEHUBINTROS
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_CITYHUBINTRO);
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
#endif
		break;

	case SCENE_CITYBOSS:

#ifndef DISABLEHUBINTROS
		// TP: get the zoo boss intro ready
		cutScene = SetupCutscene(CUTSCENE_CITYBOSSINTRO);
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
#endif

		// TP: get the outro ready also
		map.exitCutScene = SetupCutscene(CUTSCENE_CITYBOSSOUTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}
		break;

	case SCENE_WILDWESTHUB:
		// TP: check if all levels are complete then load outro ready if needed
		if (IsLevelComplete(SCENE_GRANDCANYON)&&IsLevelComplete(SCENE_GRANDCANYON)&&IsLevelComplete(SCENE_GHOSTTOWN))
		{
			map.exitCutScene = SetupCutscene(CUTSCENE_WESTHUBOUTRO);
			if (map.exitCutScene) 
			{
				if (!map.exitCutScene->Load())
				{
					SAFE_DELETE(map.exitCutScene);
				}
			}
		}

		// TP: Check for playing grandc level complete cutscene
		if (IsLevelComplete(SCENE_GRANDCANYON)&&(map.lastScene==SCENE_GRANDCANYON)&&(!map.startPos))
		{
			// TP: Play safari complete cutscene
			cutScene = SetupCutscene(CUTSCENE_WESTHUBGRANDC);
			break;
		}

		// TP: Check for playing goldmine level complete cutscene
		if (IsLevelComplete(SCENE_GOLDMINE)&&(map.lastScene==SCENE_GOLDMINE)&&(!map.startPos))
		{
			// TP: Play icedome complete cutscene
			cutScene = SetupCutscene(CUTSCENE_WESTHUBGOLDMINE);
			break;
		}

		// TP: Check for playing aqua level complete cutscene
		if (IsLevelComplete(SCENE_GHOSTTOWN)&&(map.lastScene==SCENE_GHOSTTOWN)&&(!map.startPos))
		{
			// TP: Play aqua complete cutscene
			cutScene = SetupCutscene(CUTSCENE_WESTHUBGHOST);
			break;
		}

		// TP: Check if player has not seen the hub intro first
#ifndef DISABLEHUBINTROS
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_WESTHUBINTRO);
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
#endif
		break;

	case SCENE_WESTBOSS:
#ifndef DISABLEHUBINTROS
		// TP: get the zoo boss intro ready
		cutScene = SetupCutscene(CUTSCENE_WESTBOSSINTRO);
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
#endif

		// TP: get the outro ready also
		map.exitCutScene = SetupCutscene(CUTSCENE_WESTBOSSOUTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}

		break;

	case SCENE_TAZHUB:
		// TP: Check if player has not seen the hub intro first
#ifndef DISABLEHUBINTROS
		break;
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_TASMANIAINTRO);
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
#endif
		break;

	case SCENE_TAZINTROCUTSCENE:
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_TAZHUB].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_TASMANIAINTRO);
			gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_TAZHUB].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
		break;

	case SCENE_STARTGAMEINTRO:
		if (!(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_ZOOHUB].flags & LEVELFLAG_SEENINTRO))
		{
			// TP: Load and setup the zoo intro cutscene
			cutScene = SetupCutscene(CUTSCENE_ZOOHUBGAMESTART);
			// TP: leave this to the zoo hub intro cutscene
			// TP: gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_TAZHUB].flags |= LEVELFLAG_SEENINTRO;
			break;
		}
		break;

	case SCENE_TAZPREBOSS:
#ifndef DISABLEHUBINTROS
		// TP: get the zoo boss intro ready
		cutScene = SetupCutscene(CUTSCENE_BOSS1INTRO);
#endif

		// TP: get the outro ready also
		map.exitCutScene = SetupCutscene(CUTSCENE_BOSS1OUTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}
		break;

	case SCENE_TAZBOSS:
#ifndef DISABLEHUBINTROS
		// TP: get the zoo boss intro ready
		cutScene = SetupCutscene(CUTSCENE_BOSS2INTRO);
#endif

		// TP: get the outro ready also
		map.exitCutScene = SetupCutscene(CUTSCENE_BOSS2OUTRO);
		if (map.exitCutScene) 
		{
			if (!map.exitCutScene->Load())
			{
				SAFE_DELETE(map.exitCutScene);
			}
		}

		// TP: get the outro ready also
		map.exitCutScenePt2 = SetupCutscene(CUTSCENE_ENDOFGAME);
		if (map.exitCutScenePt2) 
		{
			if (!map.exitCutScenePt2->Load())
			{
				SAFE_DELETE(map.exitCutScenePt2);
			}
		}
		break;
	}

	if (cutScene)
	{
		if (!cutScene->Load())
		{
			SAFE_DELETE(cutScene);
		}
	}
	map.introCutScene = cutScene;

	if (cutScenePt2)
	{
		if (!cutScenePt2->Load())
		{
			SAFE_DELETE(cutScenePt2);
		}
	}
	map.introCutScenePt2 = cutScenePt2;

	// PP: I'm guessing here
	return NULL;
}

static void RemoveInstance(char *instance)
{
	ACTORINSTANCE *actorInstance;

	actorInstance = FindActorInstanceInMap(instance,NULL,&map);

	if (actorInstance) FreeActorInstance(actorInstance);
	else bkPrintf("***WARNING*** Could not free %s\n",instance);
}

static void CartoonLight(char *instance)
{
	ACTORINSTANCE *actorInstance;

	actorInstance = FindActorInstanceInMap(instance,NULL,&map);

	if (actorInstance) 
	{
		ENABLE_LINE_OR_CARTOON_RENDER_MODE(&actorInstance->actorInstance);
	}
	else bkPrintf("***WARNING*** Could not free %s\n",instance);
}

/*  --------------------------------------------------------------------------------
	Function	: SetupHub
	Purpose		: This function is called to setup the special hub objects based on which levels have been completed
	Parameters	: sceneNumber
	Returns		: 
	Info		: 
*/

void SetupHub(int sceneNumber)
{
	ACTORINSTANCE *tempInstance;

	switch(sceneNumber)
	{
	case SCENE_ZOOBOSS:
		break;

	case SCENE_ZOOHUB:
		if (!IsLevelComplete(SCENE_SAFARI))
		{
			// TP: level in-complete
			RemoveInstance("stompy01");
			RemoveInstance("gates01");
			RemoveInstance("elephant collision box01");
			RemoveInstance("worldchangerzooboss");
		}
		else
		{
			// TP: level complete
			RemoveInstance("gates");
			CartoonLight("stompy01");
		}
		if (!IsLevelComplete(SCENE_ICEDOME))
		{
			// TP: level in-complete
			RemoveInstance("sbbody");
			RemoveInstance("worldchangerzooboss");
		}
		else
		{
			// TP: level complete
		}
		if (!IsLevelComplete(SCENE_AQUA))
		{
			// TP: level in-complete
			RemoveInstance("dinghy");
			RemoveInstance("worldchangerzooboss");
		}
		else
		{
			// TP: level complete
		}

		// TP: If you have already seen the safari then open these doors
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_SAFARI].flags & LEVELFLAG_HASVISITEDLEVEL)
		{
			// TP: if the last scene was not the frontend then open all the gates
			ACTORINSTANCE *instance;
			instance = FindActorInstanceInMap("levelgatesnull01",0,&map);
			if (instance)
			{
				SetToEndFrame(instance,"levelgates open");
			}
			instance = FindActorInstanceInMap("levelgatesnull02",0,&map);
			if (instance)
			{
				SetToEndFrame(instance,"levelgates open");
			}
			RemoveInstance("gate cam trigger01");
			bkGenerateEvent("trigger","_activate=worldchangersafari");
		}

		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_ZOOBOSS].flags & LEVELFLAG_SEENINTRO)
		{
			RemoveInstance("stompy01");
			RemoveInstance("elephant collision box01");
			tempInstance = FindActorInstanceInMap("gates01",NULL,&map);
			if (tempInstance)
			{
				FlushAnimationQueue(tempInstance);
			}
		}
		else
		{
			RemoveInstance("worldchangerzooboss");
		}

		break;

	case SCENE_CITYHUB:
		if (!IsLevelComplete(SCENE_DEPTSTR))
		{
			// TP: level in-complete
			RemoveInstance("shoptrolleysamsign");
			RemoveInstance("shoptrolleynull");
			RemoveInstance("worldchangercityboss");
		}
		else
		{
			// TP: level complete
			RemoveInstance("trolley02");
		}
		if (!IsLevelComplete(SCENE_CONSTRUCT))
		{
			// TP: level in-complete
			RemoveInstance("cement mixer");
			RemoveInstance("worldchangercityboss");
		}
		else
		{
			// TP: level complete
		}
		if (!IsLevelComplete(SCENE_MUSEUM))
		{
			// TP: level in-complete
			RemoveInstance("hotdog");
			RemoveInstance("worldchangercityboss");
		}
		else
		{
			// TP: level complete
		}
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_CITYBOSS].flags & LEVELFLAG_SEENINTRO)
		{
		}
		else
		{
			RemoveInstance("worldchangercityboss");
		}
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].flags & LEVELFLAG_SEENINTRO)
		{
			bkGenerateEvent("trigger", "_activate=fountainvan");
		}
		break;

	case SCENE_WILDWESTHUB:
		if (!IsLevelComplete(SCENE_GRANDCANYON))
		{
			// TP: level in-complete
			RemoveInstance("exitrock");
			RemoveInstance("cartendlevel_trigger");
			RemoveInstance("worldchangerwestboss");
		}
		else
		{
			// TP: level complete
		}
		if (!IsLevelComplete(SCENE_GHOSTTOWN))
		{
			// TP: level in-complete
			RemoveInstance("barrier01up");
			RemoveInstance("barrier02up");
			RemoveInstance("cartgo light");
			RemoveInstance("cartgo light01");
			RemoveInstance("cartendlevel_trigger");
			RemoveInstance("worldchangerwestboss");
		}
		else
		{
			// TP: level complete
			RemoveInstance("barrier01");
			RemoveInstance("barrier02");
			RemoveInstance("weed null");
			RemoveInstance("cartstop light");
			RemoveInstance("cartstop light01");
		}
		if (!IsLevelComplete(SCENE_GOLDMINE))
		{
			// TP: level in-complete
			RemoveInstance("cart static");
			RemoveInstance("cart move");
			RemoveInstance("minecartdummy");
			RemoveInstance("cartendlevel_trigger");
			RemoveInstance("worldchangerwestboss");
		}
		else
		{
			// TP: level complete
			if (IsLevelComplete(SCENE_GHOSTTOWN))
			{
				RemoveInstance("cart static");
				RemoveInstance("cartstoppendulum");
			}
			else
			{
				RemoveInstance("cartendlevel_trigger");
				RemoveInstance("cart move");
				RemoveInstance("minecartdummy");
			}
		}
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[SCENE_WESTBOSS].flags & LEVELFLAG_SEENINTRO)
		{
		}
		else
		{
			RemoveInstance("worldchangerwestboss");
		}
		break;

	}
}

/* --------------------------------------------------------------------------------
   Function : DrawPlaceHolderLoop
   Purpose : Display message for 3 seconds, allow skip using x button
   Parameters : 
   Returns : 
   Info : 
*/
void DrawPlaceHolderLoop(char *text)
{
	float		time = 3.0f;
	ushort		string16[256];
	TBMatrix	matrix;

	while((time > 0.0f)&&(!OKBUTTON))
	{
		musicSystem.UpdateMusicSystem();

		// PP: update memory card display
		MemCard_update();

		biReadDevices();

		// prepare scene
		bdSetupView();
		bdBeginScene();

		//draw fade
		bdSetIdentityObjectMatrix();
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
// PP: REMOUT: Now handled by the Book List					MemCard_draw();

		bdSetObjectMatrix(bIdentityMatrix);
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		bkString8to16(string16, text);
		TextFX_vGrad(128,128,0,110,40,0);
		bdPrintFontLite(standardFont, string16, strlen(text), -250.0f,0.0f, 128,128,128, 128, TextFX_escapeCallback, NULL);
		TextFX_resetColour();

		bdEndScene();

		// PP: finish timing the frame; calculate frames per second, frame length, etc.
		finishTimingFrame();

		if (controller1.f10DebounceChannel->value)
		{
			// take screen shot
			TakeScreenShot();
		}

		time -= fTime;
		
		// PP: start timing the frame
		startTimingFrame();

		Flip(0, 0, 0, 128);
	}
}


/*  --------------------------------------------------------------------------------
	Function	: CutsceneTv
	Purpose		: Turns the sam cutscene 
	Parameters	: true/false
	Returns		: 
	Info		: 
*/

void CutsceneTv(bool state)
{
	CUTSCENE::overlayOn = state;
}


/* --------------------------------------------------------------------------------
   Function : ChooseMusicForOutroCutscene
   Purpose : Choose the correct music for the exit cutscene
   Parameters : cutscene
   Returns : 
   Info : 
*/
void ChooseMusicForOutroCutscene(CUTSCENE *cutScene)
{
	switch(map.sceneNumber)
	{
	case SCENE_SAFARI:
		cutScene->SetMusic("safari outro cs.wav");
		break;
	case SCENE_ICEDOME:
		cutScene->SetMusic("ice outro cs.wav");
		break;
	case SCENE_AQUA:
		cutScene->SetMusic("aqua outro cs.wav");
		break;
	case SCENE_MUSEUM:
		cutScene->SetMusic("museum outro cs.wav");
		break;
	case SCENE_DEPTSTR:
		cutScene->SetMusic("looningdales outro cs.wav");
		break;
	case SCENE_CONSTRUCT:
		cutScene->SetMusic("construction outro cs.wav");
		break;
	case SCENE_GRANDCANYON:
		cutScene->SetMusic("canyon outro cs.wav");
		break;
	case SCENE_GHOSTTOWN:
		cutScene->SetMusic("ghost outro cs.wav");
		break;
	case SCENE_GOLDMINE:
		cutScene->SetMusic("goldmine outro cs.wav");
		break;
	}
}
