// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Boss.cpp
//   Purpose : General boss game stuff
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "files.h"				// PP: file stuff
#include "popup.h"				// PP: Popups windows used for things such as toggling options

#include "Boss.h"				// PP: General boss game stuff
#include "animation.h"			// PP: provides functions and structures to manage animations
#include "display.h"			// PP: display stuff
#include "multiplayer.h"
#include "text.h"

//#include "physics.h"			// PP: physics functions

#include "sfx.h"				// PP: sound effects


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													BOSSGAME methods

												Superclass for boss games.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::win
	Purpose 	: win the boss game
	Parameters 	: (opt/0)player to win
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::win(const int weiner)
{
	// PP: set winner & loser...

	this->winner=weiner;
	this->loser=weiner^1;

	// PP: go into outro state
	this->setState(BGSTATE_OUTRO);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::lose
	Purpose 	: lose the boss game
	Parameters 	: (opt/0)player to lose
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::lose(const int luzer)
{
	// PP: set winner & loser...

	this->loser=luzer;
	this->winner=luzer^1;

	// PP: go into outro state
	this->setState(BGSTATE_OUTRO);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::initpreCutscene
	Purpose 	: initialisation for the boss game, before the intro cutscene has run
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::initPreCutscene(void)
{
	// PP: set two-player mode if we have 2 players - just saves a bit of typing really
	this->twoPlayer=(gameStatus.multiplayer.numPlayers > 1);

	playAgainPopup=NULL;

	this->initMessageBook();
	this->initScoreBook();

	this->initInstructionBook();

	// PP: we haven't done proper init yet
	this->initialised=false;

	this->timeInState=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::init
	Purpose 	: initialise the boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::init(void)
{
	// PP: moved stuff from here into BOSsGAME::intiPreCutScene
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::shutdown
	Purpose 	: shutdown the boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::shutdown(void)
{
//*
	// PP: UNREMOUT: *** SURELY THIS SHOULD BE SAFE NOW (?) ***

	BookList_closeAndShutdown(this->messageBook);
	BookList_closeAndShutdown(this->scoreBook);
	BookList_closeAndShutdown(this->instructionBook);

	// PP: clean up the play-again popup
	BookList_closeAndFree(this->playAgainPopup);
	
	/*/

	this->messageBook.shutdown();
	this->scoreBook.shutdown();
	this->instructionBook.shutdown();

	// PP: clean up the play-again popup
	SAFE_DELETE(this->playAgainPopup);

//*/
}

/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::draw2
	Purpose 	: draw the Boss game alpha stuff
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::draw2(void)
{
	if(!CURRENTLY_DRAWING_FULL_SCREEN)
	{
		return;// PP: nothing to draw over BOTH split screens
	}

	bdSetIdentityObjectMatrix();

	// PP: REMOUT: THIS WAS 3D BOOK TEST STUFF...
/*	TBMatrix	scaleMatrix,rotMatrix, concMatrix, transMatrix;
	TBMatrix	mat;

	bmMatScale(scaleMatrix,1.0f, 1.0f, 1.0f);
	bmMatXRotation(rotMatrix, RAD(90.0f));
	bmMatTranslate(transMatrix,-3289, METERTOUNIT(0.05f), 1738);
	bmMatMultiply(concMatrix, transMatrix, rotMatrix);
	bmMatMultiply(mat, concMatrix, scaleMatrix);

	bdSetObjectMatrix(mat);*/

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);// PP: false
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

/* PP: REMOUT: Now handled by the Book List
	this->scoreBook.draw();

	// PP: TEMP 
#ifdef PHIL
	//this->scoreBook.drawDebug();
#endif// PP: def PHIL

	// PP: _______________________________________
	// PP: FIXME: REMOVE THIS: MILESTONE FRIG!!!!!
// PP: 	this->messageBook.flags |= (BKFLAG_TOPLEVELBOOK|BKFLAG_SCREENADJUST);
	// PP: FIXME: REMOVE THIS: MILESTONE FRIG!!!!!
	// PP: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

	this->messageBook.draw();
//	this->messageBook.drawDebug();

	this->instructionBook.draw();
*/
	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

/*#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);*/

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::update
	Purpose 	: update the Boss game
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::update(void)
{
	// PP: update time in state
	this->timeInState += fTime;
}




/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::setCamera
	Purpose 	: set the camera mode for the game
	Parameters 	: 
	Returns 	: 
	Info 		: use this to switch between camera cases
*/
/* PP: REMOUT: not much use, too generic

void BOSSGAME::setCamera(const int cam)
{
	switch(cam)
	{
	case BGCAM_NORMAL:
		{
			StopCameraCase(&camera[0], this->debugCameraCase);
			StartCameraCase(this->cameraCase, &camera[0]);
		}
		break;

	case BGCAM_DEBUG:
		{
			StopCameraCase(&camera[0], this->cameraCase);
			StartCameraCase(this->debugCameraCase, &camera[0]);	
		}
		break;
	}
}
*/


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::setupActorInstance
	Purpose 	: set-up an existing actor instance for use in the boss game
	Parameters 	: ptr to ACTORINSTANCE, flags
	Returns 	: ptr to the actor instance
	Info 		: 
*/
ACTORINSTANCE* BOSSGAME::setupActorInstance(ACTORINSTANCE* const instance, const int flags)
{
	if(!(flags&BGACT_COLLCACHE))
	{
		RemoveActorFromCollisionCache(&collisionCache, instance);
	}

	if(flags&BGACT_BEHAVIOUR)
	{
		CreateActorInstanceBehaviour(instance);
	}

	if(flags&BGACT_ANIMATION)
	{
		CreateActorInstanceAnimation(instance);
	}

	if(flags&BGACT_CARTOON)
	{
		EnableActorCartoonRenderMode(&(instance->actorInstance));
	}

	if(flags&BGACT_LINE)
	{
		EnableActorLineRenderMode(&(instance->actorInstance));
	}

	if(flags&BGACT_CHARINFO)
	{
		CreateActorInstanceCharacterInfo(instance);
	}

	return instance;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::createActorInstance
	Purpose 	: create an actor instance for use in the boss game
	Parameters 	: name of model, position for instance, (opt/0)y ang,
					(opt/BGACT_DEFAULTS)flags,
					(opt/default)instance list to which to add instance
	Returns 	: ptr to the new actor instance, or NULL if failed
	Info 		: actor instance is added to Taz's instance list, so's it won't fog or owt.
*/
ACTORINSTANCE* BOSSGAME::createActorInstance(char* const model,
												TBVector pos,
												const float yAng,
												const int flags,
												ACTORINSTANCELIST* const instanceList)
{
	TBActor*		actor;
	ACTORINSTANCE*	instance;
	char*			temp;
	char			tempName[256];

	actor = FindActorInActorList(model);
	if (!actor) 
	{
		strcpy(tempName,model);

		temp = FindChar(tempName,'.');
		if (temp!=NULL) *temp = 0;

		LoadWholePackage(tempName,1);
		LoadActor(model,tempName);
	}

	instance = CreateActorInstance(instanceList,model,model);	// TP: Using the model name as the instance name so I can hide then in cutscenes

	// PP: create and initialise actor instance status
	CreateActorInstanceStatus(instance);
	
	// PP: if the ACTORINSTANCE couldn't be created, just return NULL
	if (instance == NULL) return NULL;

	// PP: apply the specified flags to the new actor instance
	BOSSGAME::setupActorInstance(instance, flags);

	SetActorPosition(instance,pos);
	bmQuatYRotation(instance->actorInstance.orientation, yAng);

	return instance;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOSSGAME::handle2PlayerOutroState
	Purpose 	: handle 2player outro state
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOSSGAME::handle2PlayerOutroState(void)
{
	if(this->twoPlayer)
	{
		if(this->playAgainPopup == NULL)
		{
			if(this->timeInState > BOSS_OUTRO_POPUP_DELAY)
			{
				// PP: make the play again yes no popup
				this->playAgainPopup=new POPUP(POPUP_CONFIRM, 0, STR_PLAY_AGAIN, STR_DIALOGUE_YES, STR_DIALOGUE_NO, STR_PRESS_X_TO_CONTINUE, this->winner, false);
			}
		}
		else
		{
			if(this->playAgainPopup->getResponse() == 1)// PP: yes
			{
				// PP: restart level
				bkGenerateEvent("scenechange", "_reset");
			}
			else if(this->playAgainPopup->getResponse() == 2)// PP: no
			{
				// PP: back to the front-end
				bkGenerateEvent("scenechange","_frontend");
			}
		}
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													AIUSOUND methods

										Entries in an ACTORINSTANCEUSER's sound list.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: AIUSOUND::start
	Purpose 	: start this sound
	Parameters 	: 
	Returns 	: true if the sound could be played, else false
	Info 		: 
*/
bool AIUSOUND::start(void)
{
	// PP: TEMP BABEL WORKAROUND!!!!! TEMP!!!!!...
#if 0// PP: (TEMP MS FRIG)	// PP: (BPLATFORM != XBOX)
	if(this->flags & AIUSOUND_TRACK)
	{
		this->channel=bsPlayTrackingSample(this->sample, this->user->pos(), this->volume, 0, (this->flags&AIUSOUND_LOOP)?BSPLAYFLAG_FORCELOOPING:0, NULL, NULL, SFX_minDist, SFX_maxDist);
	}
	else if(this->flags & AIUSOUND_POSITION)
	{
		this->channel=bsPlayPositionalSample(this->sample, this->user->pos(), this->volume, 0, (this->flags&AIUSOUND_LOOP)?BSPLAYFLAG_FORCELOOPING:0, SFX_minDist, SFX_maxDist);
	}
	else
#endif// PP: (BPLATFORM != XBOX)
	{
		this->channel=bsPlaySample(this->sample, this->volume, 0, 0, (this->flags&AIUSOUND_LOOP)?BSPLAYFLAG_FORCELOOPING:0);
	}

	this->started=true;

	if(this->channel == (-1))
	{
		// PP: couldn't find/play sound
		this->finished=true;
		return false;
	}
	else
	{
		CallbackWhenSampleFinished(this->channel, ACTORINSTANCEUSER::sampleFinishCallback, (void*)this);

		return true;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: AIUSOUND::stop
	Purpose 	: stop this sound
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void AIUSOUND::stop(void)
{
	StopChannel(&this->channel);
	this->finished=true;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												ACTORINSTANCEUSER methods

									Base type for anything that uses an ACTORINSTANCE.
												Useful accessor methods, etc.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: initialise static default position for ACTORINSTANCEUSERS
TBVector ACTORINSTANCEUSER::defaultPosition={0,0,0,1};


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::init
	Purpose 	: initialisation
	Parameters 	: model name, position, (opt/0.0f)angle, (opt/default) creation flags, (opt/default)instance list to which to add instance
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::init(char* const model, TBVector pos, const float yAng, const int flags, ACTORINSTANCELIST* instanceList)
{
	this->actorInstance=TAG_BOSSGAME::createActorInstance(model, pos, yAng, flags, instanceList);
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::init
	Purpose 	: initialisation
	Parameters 	: model name, creation flags, (opt/default)instance list to which to add instance
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::init(char* const model, const int flags, ACTORINSTANCELIST* instanceList)
{
	this->actorInstance=TAG_BOSSGAME::createActorInstance(model, defaultPosition, 0.0f, flags, instanceList);
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::init
	Purpose 	: initialisation
	Parameters 	: ptr to ACTORINSTANCE, (opt/default)flags
	Returns 	: 
	Info 		: use this to set up an AIU from an instance already in the map
*/
void ACTORINSTANCEUSER::init(ACTORINSTANCE* const actorInstanceIn, const int flags)
{
	this->actorInstance=actorInstanceIn;

	BOSSGAME::setupActorInstance(this->actorInstance, flags);
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::init(void)
{
	this->actorInstance=NULL;
	this->lastAnim=NULL;
	this->currentAnim=NULL;
	this->animChanged=false;

	// PP: initialise sound list
	this->sounds.init();

	this->isPlayingSound=false;
	this->muted=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::udpateSounds
	Purpose 	: udpate the ACTORINSTANCEUSER's sounds
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::updateSounds(void)
{
	AIUSOUND*	sound;
	AIUSOUND*	queuedSound;

	queuedSound=NULL;

	this->isPlayingSound=false;// PP: until we find a sound being played

	for(int i=0; i<this->sounds.size; i++)
	{
		sound=this->sounds.getItem(i);

		if(sound->finished)
		{
			// PP: sound has finished - discard it
			this->sounds.discardOldest();

			// PP: bkPrintf("ACTORINSTANCEUSER::updateSounds: \"%s\" finished - discarding\n", sound->name);
		}
		else
		{
			if(sound->started)
			{
				// PP: currently-playing sound
				this->isPlayingSound=true;
			}
			else
			{
				// PP: first queued sound - keep a ptr to it in case it's free to start
				if(queuedSound == NULL)
				{
					queuedSound=sound;
				}
			}
		}
	}

	if(!this->isPlayingSound)// PP: if all sounds have finished...
	{
		if(queuedSound != NULL)// PP: ...and there's a queued sound...
		{
			queuedSound->start();// PP: ...start the queued sound...
			// PP: bkPrintf("ACTORINSTANCEUSER::updateSounds: queued \"%s\" starting\n", queuedSound->name);

			this->isPlayingSound=true;// PP: ...and flag this AIU as playing sound.
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::udpate
	Purpose 	: udpate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::update(void)
{
	this->lastAnim=this->currentAnim;
	this->currentAnim=CurrentAnimation(this->actorInstance);
	this->animChanged=(this->currentAnim != this->lastAnim);

	// PP: update sounds
	this->updateSounds();
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::shutdown
	Purpose 	: shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::shutdown(void)
{
	if(this->actorInstance != NULL)
	{
// PP: REMOUT: not needed apparently			FreeActorInstance(this->actorInstance);
		//bkPrintf("ACTORINSTANCEUSER::shutdown freeing 0x%08x\n", this->actorInstance);
		this->actorInstance=NULL;
	}

	this->flushSound();
	this->stopSound();
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::playAnim
	Purpose 	: play an animation
	Parameters 	: ptr to anim anim, (opt/false) whether or not to loop, (opt/1.0f)speed
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::playAnim(TBActorAnimSegment* const anim, const bool loop, const float speed)
{
#define BLEND_TIME	0.2f//0.5f

	// PP: I'm setting both currentAnim and lastAnim to this new segment,
	// PP: so that you can start an anim playing and immediately start checking for a change of anim

	this->currentAnim=this->lastAnim=anim;

	this->animChanged=false;

	if(this->currentAnim != NULL)
	{
		PlayAnimationBySegmentPointer(this->actorInstance, anim, speed, loop?1:0, 0, BLEND_TIME, DEATH);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::playAnim
	Purpose 	: play an animation
	Parameters 	: name of anim, (opt/false) whether or not to loop,
					(opt/false) whether or not to queue, (opt/1.0f)speed
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::playAnim(char* const animName, const bool loop, const float speed)
{
	TBActorAnimSegment*		anim;

	anim=baFindAnimSegment(&this->actorInstance->actorInstance, animName);

	if(anim != NULL)
	{
		this->playAnim(anim, loop, speed);
	}

//	bkPrintf("ACTORINSTANCEUSER::playAnim '%s'\n", animName);
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::queueAnim
	Purpose 	: queue an animation
	Parameters 	: name of anim, (opt/false) whether or not to loop, (opt/1.0f)speed
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::queueAnim(char* const animName, const bool loop, const float speed)
{
	TBActorAnimSegment*		anim;

	anim=baFindAnimSegment(&this->actorInstance->actorInstance, animName);

	if(anim != NULL)
	{
		this->queueAnim(anim, loop, speed);
	}

//	bkPrintf("ACTORINSTANCEUSER::queueAnim '%s'\n", animName);
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::queueAnim
	Purpose 	: queue an animation
	Parameters 	: queue to anim, (opt/false) whether or not to loop, (opt/1.0f)speed
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::queueAnim(TBActorAnimSegment* const anim, const bool loop, const float speed)
{
#define BLEND_TIME	0.2f//0.5f

	if(this->currentAnim != NULL)
	{
		PlayAnimationBySegmentPointer(this->actorInstance, anim, speed, loop?1:0, 1, BLEND_TIME, DEATH);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::getSound
	Purpose 	: get a ptr to a sound if it exists in the AIU's sound list
	Parameters 	: 
	Returns 	: AIUSOUND*, NULL if not found
	Info 		: 
*/
AIUSOUND* ACTORINSTANCEUSER::getSound(const char* const name)
{
	LA_FOREACH(this->sounds, AIUSOUND, sound);

		if(stricmp(sound->name, name) == 0)
		{
			return sound;
		}

	LA_ENDFOREACH(this->sounds);

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::sampleFinishCallback
	Purpose 	: callback triggered whan an AIU sound finishes
	Parameters 	: void ptr to AIUSOUND finishing
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::sampleFinishCallback(void* context)
{
	AIUSOUND*	sound;

	sound=(AIUSOUND*)context;

	sound->finished=true;

	// PP: also call this sound's custom finished callback if one is set
	if(sound->finishFunc != NULL)
	{
		sound->finishFunc(sound);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::playSound
	Purpose 	: play a sound
	Parameters 	: sound name, (opt/default)flags, (opt/255)volume(0..255), (opt/NULL) custom sample-finished callback (See AIUSoundFinishCallback typedef)
	Returns 	: 
	Info 		: Might add a frequency param when I find a need for it
*/
void ACTORINSTANCEUSER::playSound(char* const name, const int flags, const unsigned char volume, const AIUSoundFinishCallback finishFunc)
{
	TBSample*	sample;

	// PP: check for muting
	if(this->muted)
	{
		return;
	}

	sample=bkLoadSample(NULL, name, 0);

	if(sample == NULL)
	{
		// PP: sample not found
		return;
	}

	if(!(flags & AIUSOUND_QUEUE))// PP: exclusive flag is meaningless for queued samples
	{
		if(flags & AIUSOUND_EXCLUSIVE)
		{
			// PP: stop all samples first
			this->stopSound();
		}
	}

	AIUSOUND*	sound;

	sound=this->sounds.getNew();

	// PP: set custom finish func (will be called by ACTORINSTANCEUSER::sampleFinishCallback)
	sound->finishFunc=finishFunc;

	// PP: set sound user
	sound->user=this;

	// PP: fill in sound's details
	sound->sample=sample;
	strcpy(sound->name, name);
	sound->flags=flags;
	sound->volume=volume;
	sound->finished=false;
	sound->started=false;

	if(!(flags&AIUSOUND_QUEUE))
	{
		sound->start();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::stopSound
	Purpose 	: stop playing a sound from this ACTORINSTANCEUSER
	Parameters 	: (opt/NULL=all)name of sound to stop
	Returns 	: 
	Info 		: 
*/
void ACTORINSTANCEUSER::stopSound(const char* const name)
{
	if(name == NULL)
	{
		LA_FOREACHDO(this->sounds, stop());

		this->sounds.init();
	}
	else
	{
		AIUSOUND*	sound;

		sound=this->getSound(name);

		if(sound)
		{
			sound->stop();

			// PP: TODO?: remove the sound from the LOOPARRAY?  There isn't a LOOPARRAY method to do that yet
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ACTORINSTANCEUSER::flushSound
	Purpose 	: clear the sound queue of this ACTORINSTANCEUSER
	Parameters 	: 
	Returns 	: 
	Info 		: leaves be any samples that have already started
*/
void ACTORINSTANCEUSER::flushSound(void)
{
	AIUSOUND*	sound;

	for(int i=0; i<this->sounds.size; i++)
	{
		sound=this->sounds.getItem(i);				
	
		if(!sound->started)
		{
			this->sounds.discardOldest();
		}
	}
}
