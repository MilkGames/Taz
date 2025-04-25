// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : rumble.cpp
//   Purpose : pad rumbling controls
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "util.h"
#include "control.h"
#include "rumble.h"

// PP: define RUMBLE_DEBUG to get debug messages about pad rumbles
//#define RUMBLE_DEBUG

/*  --------------------------------------------------------------------------------
	Function	: CreateRumbleController
	Purpose		: Creates and initialises rumble controller for pad
	Parameters	: max number of effects in queue
	Returns		: ptr to the contoller
	Info		: 
*/

RUMBLECONTROLLER *CreateRumbleController(int effectsQueueSize)
{
	RUMBLECONTROLLER	*ptr;

	ptr = (RUMBLECONTROLLER*)ZALLOC(sizeof(RUMBLECONTROLLER));
	ASSERTM(ptr, "Could not malloc rumble controller");
	if(!ptr) return NULL;

	ptr->rumbleQueue = (RUMBLEEFFECT*)ZALLOC(sizeof(RUMBLEEFFECT)*effectsQueueSize);
	ASSERTM(ptr->rumbleQueue, "Could not malloc rumblequeue");

	ptr->buzzerQueue = (BUZZEREFFECT*)ZALLOC(sizeof(BUZZEREFFECT)*effectsQueueSize);
	ASSERTM(ptr->buzzerQueue, "Could not malloc buzzerqueue");

	for(int i=0;i<effectsQueueSize;i++)
	{
		ptr->rumbleQueue[i].delay = 0.0f;
		ptr->rumbleQueue[i].duration = 0.0f;
		ptr->rumbleQueue[i].flags = 0;

		ptr->buzzerQueue[i].delay = 0.0f;
		ptr->buzzerQueue[i].duration = 0.0f;
		ptr->buzzerQueue[i].flags = 0;
	}
	ptr->sizeQueue = effectsQueueSize;
	ptr->flags = 0;
	
#ifdef _TAZDEBUG
	ptr->rumbleLevel = 0;
	ptr->buzzerState = FALSE;
#endif

	return ptr;
}

/*  --------------------------------------------------------------------------------
	Function	: RequestRumbleEffect
	Purpose		: Sends a request for a rumble effect with given parameters
	Parameters	: 
	Returns		: handle to queue member, -1 if queue full
	Info		: 
*/

int RequestRumbleEffect(PADCONTROLSTATUS *controller,int amplitude,float attack,float sustain,float duration,float delay,EWaveform waveform,float frequency,float freqRateOfChange,uint32 flags, uint32 posInstCRC)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	ACTORINSTANCE		*actorInstance;
	RUMBLECONTROLLER	*ptr = controller->rumbleController;

	if(gameStatus.demoMode)	return -1;
	if(controller->rumbleController->flags & RUMBLEFLAG_DISABLE) return -1;

	if((controller->player == 1)							// PP: if the controller would belong to player 2 (zero-based index)...
		&& (gameStatus.multiplayer.versusCPU))				// PP: ...but we're only in single player
	{
		 return -1;											// PP: deny rumble request
	}

	// CMD: check to see if any of the effects have finished
	for(int i=0;i<ptr->sizeQueue;i++)
	{
		if(ptr->rumbleQueue[i].delay||ptr->rumbleQueue[i].duration
			||(ptr->rumbleQueue[i].flags&RUMBLEFLAG_LOOP)||(ptr->rumbleQueue[i].flags&RUMBLEFLAG_YOYO)) continue;
		else
		{
			if(flags & RUMBLEFLAG_TRACKING)
			{
				if((actorInstance = FindActorInstanceInMap(NULL, posInstCRC, &map))==NULL) continue;

				// CMD: do not accept if too far away
				if(bmVectorDistance(actorInstance->actorInstance.position, gameStatus.player[controller->padID].camera->cameraState.cameraPos) < RUMBLE_MAXTRACKINGDIST)
				{
					ptr->rumbleQueue[i].posInstCRC = posInstCRC;
				}
				else continue;
			}
			bkPrintf("*** Sucessful Rumble Request ***\n");

			// CMD: clamp amplitude to minimum detectible
			if((ptr->rumbleQueue[i].targetAmplitude = amplitude) < 100) ptr->rumbleQueue[i].amplitude = 100;
			ptr->rumbleQueue[i].amplitude = 0;
			ptr->rumbleQueue[i].attack = ptr->rumbleQueue[i].initialAttack = attack;
			ptr->rumbleQueue[i].sustain = ptr->rumbleQueue[i].initialSustain = sustain;
			ptr->rumbleQueue[i].duration = ptr->rumbleQueue[i].initialDuration = duration;
			ptr->rumbleQueue[i].delay = ptr->rumbleQueue[i].initialDelay = delay;
			ptr->rumbleQueue[i].waveform = waveform;
			ptr->rumbleQueue[i].frequency = ptr->rumbleQueue[i].initialFrequency = frequency;
			ptr->rumbleQueue[i].frequencyRate = freqRateOfChange;
			ptr->rumbleQueue[i].stage = RUMBLESTAGE_ATTACK;

			switch(waveform)
			{
			case WAVEFORM_SQUARE:
				ptr->rumbleQueue[i].cutoffClock = 2.0f/frequency;
				ASSERTM(frequency, "passed frequency = 0.0f for square waveform rumble");
				break;
			default:
				if(frequency) ptr->rumbleQueue[i].cutoffClock = 1.0f/frequency;
				else ptr->rumbleQueue[i].cutoffClock = 0.0f;
				break;
			}
			if(attack) ptr->rumbleQueue[i].attackRate = amplitude/attack;
			else ptr->rumbleQueue[i].attackRate = 0.0f;
			if(duration-attack-sustain) ptr->rumbleQueue[i].decayRate = amplitude/(duration-attack-sustain);
			else ptr->rumbleQueue[i].decayRate = 0.0f;
			ptr->rumbleQueue[i].flags = flags;
			return i;
		}
	}
	// CMD: could not find any free spaces
	return -1;
}

/*  --------------------------------------------------------------------------------
	Function	: RequestBuzzerEffect
	Purpose		: Sends a request for a buzzer effect with given parameters
	Parameters	: 
	Returns		: handle to queue member, -1 if queue full
	Info		: 
*/

int RequestBuzzerEffect(PADCONTROLSTATUS *controller,float duration,float delay,EWaveform waveform,float frequency,float freqRateOfChange,uint32 flags,uint32 posInstCRC)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	ACTORINSTANCE		*actorInstance;
	RUMBLECONTROLLER	*ptr = controller->rumbleController;

	if(gameStatus.demoMode)	return -1;
	if(waveform == WAVEFORM_NONE) return -1;
	if(controller->rumbleController->flags & RUMBLEFLAG_DISABLE) return -1;

	// CMD: check to see if any of the effects have finished
	for(int i=0;i<ptr->sizeQueue;i++)
	{
		if(ptr->buzzerQueue[i].delay||ptr->buzzerQueue[i].duration
			||(ptr->buzzerQueue[i].flags&RUMBLEFLAG_LOOP)||(ptr->buzzerQueue[i].flags&RUMBLEFLAG_YOYO)) continue;
		else
		{
			if(flags & RUMBLEFLAG_TRACKING)
			{
				if((actorInstance = FindActorInstanceInMap(NULL, posInstCRC, &map))==NULL) continue;

				// CMD: do not accept if too far away
				if(bmVectorDistance(actorInstance->actorInstance.position, gameStatus.player[controller->padID].camera->cameraState.cameraPos) < RUMBLE_MAXTRACKINGDIST)
				{
					ptr->buzzerQueue[i].posInstCRC = posInstCRC;
				}
				else continue;
			}

			bkPrintf("*** Sucessful Buzzer Request ***\n");

			ptr->buzzerQueue[i].duration = ptr->buzzerQueue[i].initialDuration = duration;
			ptr->buzzerQueue[i].delay = ptr->buzzerQueue[i].initialDelay = delay;
			ptr->buzzerQueue[i].waveform = waveform;
			ptr->buzzerQueue[i].frequency = ptr->buzzerQueue[i].initialFrequency = frequency;
			ptr->buzzerQueue[i].frequencyRate = freqRateOfChange;

			switch(waveform)
			{
			case WAVEFORM_SQUARE:
				ASSERTM(frequency, "passed frequency = 0.0f for square waveform buzzer");
				ptr->buzzerQueue[i].cutoffClock = 2.0f/frequency;
				break;
			default:
				if(frequency) ptr->buzzerQueue[i].cutoffClock = 1.0f/frequency;
				else ptr->buzzerQueue[i].cutoffClock = 0.0f;
				break;
			}
			ptr->buzzerQueue[i].flags = flags;
			return i;
		}
	}
	// CMD: could not find any free spaces
	return -1;
}

/*  --------------------------------------------------------------------------------
	Function	: RequestRumbleEffect
	Purpose		: Sends a request for a rumble effect with given parameters
	Parameters	: 
	Returns		: handle to queue member, -1 if queue full
	Info		: 
*/

int RequestPresetRumble(PADCONTROLSTATUS *controller,int amplitude,float duration,EWaveform waveform,uint32 posInstCRC, uint32 flags)
{
	switch(waveform)
	{
	case WAVEFORM_EXPLOSION:
		RequestRumbleEffect(controller, amplitude, duration*0.05f, duration*0.5f, duration, 0.0f, WAVEFORM_RANDOM, 0.0f, 0.0f, flags, posInstCRC);
		RequestBuzzerEffect(controller, duration*0.5f, 0.0f, WAVEFORM_SQUARE, 20.0f, 0.0f, flags, posInstCRC);
		break;
	case WAVEFORM_BOOM:
		RequestRumbleEffect(controller, amplitude, 0.0f, duration*0.75f, duration, 0.0f,WAVEFORM_NORMAL,0.0f, 0.0f, flags, posInstCRC);
		break;
	case WAVEFORM_DRILL:
		RequestRumbleEffect(controller, amplitude, 0.0f, 0.0f, duration, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 6.0f, 0.0f, flags, posInstCRC);
		break;
	case WAVEFORM_SPIN:
		#if BPLATFORM!=GAMECUBE
			RequestRumbleEffect(controller, 255, 0.25f, 0.0f, 3.0f, 0.0f, WAVEFORM_SQUAREDSINUSOIDAL, 5.0f, 0.0f, flags, posInstCRC);
		#else
			RequestRumbleEffect(controller, 100, 0.25f, 0.0f, 3.0f, 0.0f, WAVEFORM_NORMAL, 5.0f, 0.0f, flags, posInstCRC);
		#endif
		break;
	}

	return  TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateRumble
	Purpose		: Updates the rumble effect for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateRumble(PADCONTROLSTATUS *controller, float t)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	ACTORINSTANCE	*actorInstance;
	RUMBLEEFFECT	*effect;
	int				totalAmp, i;
	float			tempFloat, tempAmp, length;

	totalAmp = 0;

	if(!gameStatus.globalSettings.vibration) return;

	// CMD: check for pause flag
	if(controller->rumbleController->flags&RUMBLEFLAG_PAUSE)
	{
#ifdef _TAZDEBUG
		controller->rumbleController->rumbleLevel = 0;
		controller->rumbleController->buzzerState = FALSE;
#endif

		// PP: NOTE: biFFSetParm is concerned with physical port numbers, not logical pad numbers
#if(BPLATFORM == PS2)
		biFFSetParm(controller->port,BIFEEDBACK_PS2BUZZER,FALSE);
		biFFSetParm(controller->port,BIFEEDBACK_PS2RUMBLE,0);
#elif(BPLATFORM == XBOX)
		biFFSetParm(controller->port,BIFEEDBACK_XBOXBOTHMOTORS, 0, 0);
#elif(BPLATFORM == PC)
		// CMD: sod PC force feed back
#elif(BPLATFORM == GAMECUBE)
		biFFSetParm(controller->port,BIFEEDBACK_GAMECUBERUMBLE, 0, 0);
#else
#error Help unknown format. This probably means im giving that Macintosh version a go!?!
#endif

#ifdef RUMBLE_DEBUG
		bkPrintf("UpdateRumble: PAUSING controller in port %d (zero-based)\n", controller->port);
#endif// PP: def RUMBLE_DEBUG

		return;				// CMD: no need to do any calculations
	}

	// CMD: update rumble from here on
	// CMD: ps2 also has an update for the buzzer

	/********************************************************/
	/*					Rumble Queue Update					*/
	/********************************************************/
	for(i = (controller->rumbleController->sizeQueue-1);i >= 0;i--)
	{
		effect = &controller->rumbleController->rumbleQueue[i];

		// CMD: check too see if effect has finished
		if((effect->delay == 0.0f)&&(effect->duration == 0.0f)
			&&(~effect->flags & RUMBLEFLAG_LOOP)&&(~effect->flags & RUMBLEFLAG_YOYO))	continue;

		// CMD: NOTE: does not take into acount time left in frame at end of each phase, just doesn't seem worthwhile
		// CMD: check if delay is over, and by how much
		if(effect->delay)
		{
			if((effect->delay -= t) > 0.0f) continue;
			else
			{
				effect->delay = 0.0f;
				continue;
			}
		}
		else
		{
			// CMD: decrement duration whatever happens after delay
			if((effect->duration -= t) < 0.0f)
			{
				if(effect->flags & RUMBLEFLAG_LOOP)
				{
					// CMD: reset timers
					effect->delay = effect->initialDelay;
					effect->attack = effect->initialAttack;
					effect->sustain = effect->initialSustain;
					effect->duration = effect->initialDuration;
					effect->frequency = effect->initialFrequency;
					effect->stage = RUMBLESTAGE_ATTACK;
					continue;
				}
				if(effect->flags & RUMBLEFLAG_YOYO)
				{
					if(effect->attack = effect->initialDuration-effect->initialAttack-effect->initialSustain) effect->attackRate = effect->targetAmplitude/effect->attack;
					else effect->attackRate = 0.0f;
					effect->sustain = effect->initialSustain;
					effect->duration = effect->initialDuration;
					effect->frequencyRate *= -1.0f;
					if(effect->initialDuration-effect->initialAttack-effect->initialSustain) effect->decayRate = effect->targetAmplitude/(effect->initialDuration-effect->initialAttack-effect->initialSustain);
					effect->stage = RUMBLESTAGE_ATTACK;
					continue;
				}
				// CMD: else
				effect->duration = 0.0f;
				continue;
			}
			// CMD: update frequency
			effect->frequency += (effect->frequencyRate*t);

			switch(effect->stage)
			{
			case RUMBLESTAGE_ATTACK:
				// CMD: attack phase
				if((effect->attack -= t) < 0.0f)
				{
					effect->attack = 0.0f;
					tempAmp = (float)(effect->amplitude = effect->targetAmplitude);
					effect->stage = RUMBLESTAGE_SUSTAIN;
					break;
				}
				tempAmp = (float)(effect->amplitude = effect->targetAmplitude - (int)(effect->attack*(float)effect->attackRate));
				break;
			case RUMBLESTAGE_SUSTAIN:
				// CMD: sustain phase
				if((effect->sustain -= t) < 0.0f)
				{
					effect->sustain = 0.0f;
					effect->stage = RUMBLESTAGE_DECAY;
					// CMD: don't break;
				}
				tempAmp = (float)(effect->amplitude);
				break;
			case RUMBLESTAGE_DECAY:
				// CMD: decay phase
				if((tempAmp = (float)(effect->amplitude = (int)(effect->duration*effect->decayRate))) < 0) tempAmp = 0;
				break;
			}
		}

		// CMD: do amplitude correction for distance, ir required
		if(effect->flags & RUMBLEFLAG_TRACKING)
		{
			// CMD: amplitude is calculated such that it is zero at RUMBLE_MAXTRACKINGDIST
			// CMD: and a maximum at RUMBLE_MINTRACKINGDIST
			if((actorInstance = FindActorInstanceInMap(NULL, effect->posInstCRC, &map))==NULL)
			{
				tempAmp = 0.0f;
			}
			else
			{
				if(length = bmVectorDistance(actorInstance->actorInstance.position, gameStatus.player[controller->padID].camera->cameraState.cameraPos) > RUMBLE_MAXTRACKINGDIST)
				{
					tempAmp = 0.0f;
				}
				else
				{
					if(length > RUMBLE_MINTRACKINGDIST)
					{
						tempAmp *= ((length/RUMBLE_MINMINUSMAX)-(RUMBLE_MAXTRACKINGDIST/RUMBLE_MINMINUSMAX));
					}
				}
			}
		}

		// CMD: now modulate amplitude depending on wave form chosen
		switch(effect->waveform)
		{
		case WAVEFORM_NORMAL:
			totalAmp += (int)(256.0f*tempAmp);
			break;
		case WAVEFORM_SINUSOIDAL:
			totalAmp += (int)(tempAmp *= (256.0f*(bmSin(RAD(360.0f*effect->frequency*effect->duration))+1.0f)*0.5f));
			break;
		case WAVEFORM_SQUAREDSINUSOIDAL:
			tempFloat= bmSin(RAD(360.0f*effect->frequency*effect->duration));
			totalAmp += (int)(tempAmp *= (256.0f*tempFloat*tempFloat));
			break;
		case WAVEFORM_SQUARE:
			if((effect->cutoffClock -= t) < 1.0f/effect->frequency)
			{
				// CMD: don't add rumble here
				if(effect->cutoffClock < 0.0f)
				{
					effect->cutoffClock = 2.0f/effect->frequency;
				}
			}
			else
			{
				totalAmp += (int)(256.0f*tempAmp);
			}
			break;
		case WAVEFORM_SAWTOOTH:
			if((effect->cutoffClock -= t) < 0.0f)
			{
				effect->cutoffClock = 1.0f/effect->frequency;
			}
			totalAmp += (int)(tempAmp *= (256.0f*effect->frequency*effect->cutoffClock));
			break;
		case WAVEFORM_RANDOM:
			totalAmp += (int)(tempAmp *= (256.0f*UtilGenerateRandomNumber(1.0f,0.0f)));
			break;
		}
	}

#if	BPLATFORM == PS2
	/********************************************************/
	/*					Buzzer Queue Update					*/
	/********************************************************/
	BUZZEREFFECT	*buzzer;
	int				buzzerState;

	buzzerState = FALSE;
	for(i = (controller->rumbleController->sizeQueue-1);i >= 0;i--)
	{
		buzzer = &controller->rumbleController->buzzerQueue[i];

		// CMD: check too see if effect has finished
		if((buzzer->delay == 0.0f)&&(buzzer->duration == 0.0f)
			&&(~buzzer->flags & RUMBLEFLAG_LOOP)&&(~buzzer->flags & RUMBLEFLAG_YOYO))	continue;

		// CMD: NOTE: does not take into acount time left in frame at end of each phase, just doesn't seem worthwhile
		// CMD: check if delay is over, and by how much
		if(buzzer->delay)
		{
			if((buzzer->delay -= t) > 0.0f) continue;
			else
			{
				buzzer->delay = 0.0f;
				continue;
			}
		}
		else
		{
			// CMD: decrement duration whatever happens after delay
			if((buzzer->duration -= t) < 0.0f)
			{
				if(buzzer->flags & RUMBLEFLAG_LOOP)
				{
					// CMD: reset timers
					buzzer->delay = buzzer->initialDelay;
					buzzer->duration = buzzer->initialDuration;
					buzzer->frequency = buzzer->initialFrequency;
					continue;
				}
				if(buzzer->flags & RUMBLEFLAG_YOYO)
				{
					buzzer->duration = buzzer->initialDuration;
					buzzer->frequencyRate *= -1.0f;
					continue;
				}
				// CMD: else
				buzzer->duration = 0.0f;
				continue;
			}
			// CMD: update frequency
			buzzer->frequency += (buzzer->frequencyRate*t);
		}

		// CMD: do amplitude correction for distance, ir required
		if(buzzer->flags & RUMBLEFLAG_TRACKING)
		{
			// CMD: amplitude is calculated such that it is zero at RUMBLE_MAXTRACKINGDIST
			// CMD: and a maximum at RUMBLE_MINTRACKINGDIST
			if((actorInstance = FindActorInstanceInMap(NULL, buzzer->posInstCRC, &map))==NULL)
			{
				buzzerState = FALSE;
			}
			else
			{
				if(length = bmVectorDistance(actorInstance->actorInstance.position, gameStatus.player[controller->padID].camera->cameraState.cameraPos) > RUMBLE_MAXTRACKINGDIST)
				{
					buzzerState = FALSE;
				}
				else
				{
					buzzerState = TRUE;
				}
			}
		}

		// CMD: now modulate amplitude depending on wave form chosen
		switch(buzzer->waveform)
		{
		case WAVEFORM_NORMAL:
			if(buzzerState) buzzerState = TRUE;
			break;
		case WAVEFORM_SQUARE:
			if((buzzer->cutoffClock -= t) < 1.0f/buzzer->frequency)
			{
				// CMD: don't add rumble here
				if(buzzer->cutoffClock < 0.0f)
				{
					buzzer->cutoffClock = 2.0f/buzzer->frequency;
				}
			}
			else
			{
				if(buzzerState) buzzerState = TRUE;
			}
			break;
		case WAVEFORM_RANDOM:
			if(buzzerState)
			{
				if(bmRand()%2) buzzerState = TRUE;
			}
			break;
		}
	}

	// CMD: clamp totalAmp at 255
	if(totalAmp > (255<<8)) totalAmp = (255<<8);

#ifdef _TAZDEBUG
	controller->rumbleController->rumbleLevel = totalAmp>>8;
	controller->rumbleController->buzzerState = buzzerState;
#endif
#endif

	// PP: NOTE: biFFSetParm is concerned with physical port numbers, not logical pad numbers
#if BPLATFORM == PS2
	switch(biQueryDevice(controller->port, NULL)) //controller->deviceType)
	{
	case EBIDEVICETYPE_PSDUALSHOCK:
	case EBIDEVICETYPE_PS2DUALSHOCK2:
		biFFSetParm(controller->port,BIFEEDBACK_PS2RUMBLE,totalAmp>>8);
		biFFSetParm(controller->port,BIFEEDBACK_PS2BUZZER,buzzerState);
		break;
	}
#elif BPLATFORM == XBOX
	
	int		rumbleSpeed=bmFloatToInt(BXBOX_MOTORSPEED*((float)(totalAmp>>8)/255.0f));

	biFFSetParm(controller->port,BIFEEDBACK_XBOXBOTHMOTORS, rumbleSpeed, rumbleSpeed);

#elif(BPLATFORM == GAMECUBE)
	biFFSetParm(controller->port,BIFEEDBACK_GAMECUBERUMBLE, totalAmp>>8, totalAmp>>8);
#else
	// CMD: as I mentioned above we can forget about the pc, no one likes it anyway!
#endif
}

/*  --------------------------------------------------------------------------------
	Function	: FlushRumbleQueue
	Purpose		: Flushes the entire rumble queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FlushRumbleQueue(PADCONTROLSTATUS *controller)
{
	if (!controller)
		return;

	ASSERTM(controller->rumbleController, "NULL rumble controller");

	bkPrintf("FlushRumbleQueue\n");

	for(int i=(controller->rumbleController->sizeQueue-1);i>=0;i--)
	{
		FlushRumbleQueueMember(controller,i);
		FlushBuzzerQueueMember(controller,i);
	}

	// CMD: update the controller to ensure that it is set to zero
	UpdateRumble(controller, 0.0f);
}

/*  --------------------------------------------------------------------------------
	Function	: FlushRumbleQueueMember
	Purpose		: Flushes a member of the rumble queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

int FlushRumbleQueueMember(PADCONTROLSTATUS *controller, int handle)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	if((handle > controller->rumbleController->sizeQueue)||
		(handle == -1)) return -1;

	controller->rumbleController->rumbleQueue[handle].delay = 0.0f;
	controller->rumbleController->rumbleQueue[handle].duration = 0.0f;
	controller->rumbleController->rumbleQueue[handle].flags = 0;
	
	// CMD: update the controller to ensure that it is set to zero
	UpdateRumble(controller, 0.0f);
	return -1;
}

/*  --------------------------------------------------------------------------------
	Function	: FlushBuzzerQueueMember
	Purpose		: Flushes a member of the buzzer queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

int FlushBuzzerQueueMember(PADCONTROLSTATUS *controller, int handle)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	if((handle > controller->rumbleController->sizeQueue)||
		(handle == -1)) return -1;

	controller->rumbleController->buzzerQueue[handle].delay = 0.0f;
	controller->rumbleController->buzzerQueue[handle].duration = 0.0f;
	controller->rumbleController->buzzerQueue[handle].flags = 0;

	// CMD: update the controller to ensure that it is set to zero
	UpdateRumble(controller, 0.0f);
	return -1;
}

/*  --------------------------------------------------------------------------------
	Function	: PauseRumble
	Purpose		: Pause the rumble update for a controller
	Parameters	: 
	Returns		: 
	Info		: // PP: IMPORTANT: YOU MUST CONTINUALLY CALL bkUpdate (or bdFlip) for this to have any effect
*/

void PauseRumble(PADCONTROLSTATUS *controller)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	controller->rumbleController->flags |= RUMBLEFLAG_PAUSE;
	// CMD: update the controller to ensure it is set to zero
	UpdateRumble(controller, 0.0f);
}

/*  --------------------------------------------------------------------------------
	Function	: UnPauseRumble
	Purpose		: Unpause the rumble update for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UnPauseRumble(PADCONTROLSTATUS *controller)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	if (controller->rumbleController)
		controller->rumbleController->flags &= ~(RUMBLEFLAG_PAUSE|RUMBLEFLAG_DISABLE);
}

/*  --------------------------------------------------------------------------------
	Function	: DisableRumble
	Purpose		: disables the controller such that all existing rumbles will be paused and any new requests will be declined
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DisableRumble(PADCONTROLSTATUS *controller)
{
	if(controller)
	{
		ASSERTM(controller->rumbleController, "NULL rumble controller");

		controller->rumbleController->flags |= RUMBLEFLAG_DISABLE;
		controller->rumbleController->flags |= RUMBLEFLAG_PAUSE;

		// CMD: update the controller to ensure it is set to zero
		UpdateRumble(controller, 0.0f);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: EnableRumble
	Purpose		: undoes effect of DisableRumble
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EnableRumble(PADCONTROLSTATUS *controller)
{
	if(controller)
	{
		ASSERTM(controller->rumbleController, "NULL rumble controller");

		controller->rumbleController->flags &= ~RUMBLEFLAG_DISABLE;
		controller->rumbleController->flags &= ~RUMBLEFLAG_PAUSE;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: CheckIfRumblePlaying
	Purpose		: checks to see if a rumble is still rumbling
	Parameters	: 
	Returns		: 
	Info		: 
*/

int CheckIfRumblePlaying(PADCONTROLSTATUS *controller, int handle)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	if((handle > controller->rumbleController->sizeQueue)||
		(handle == -1)) return FALSE;

	if(controller->rumbleController->rumbleQueue[handle].delay||
		controller->rumbleController->rumbleQueue[handle].duration||
		(controller->rumbleController->rumbleQueue[handle].flags&RUMBLEFLAG_LOOP)||
		(controller->rumbleController->rumbleQueue[handle].flags&RUMBLEFLAG_YOYO))
	{
		return TRUE;
	}
	else return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: FreeRumbleController
	Purpose		: frees rumble controller memory
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FreeRumbleController(PADCONTROLSTATUS *controller)
{
	ASSERTM(controller->rumbleController, "NULL rumble controller");

	if(controller->rumbleController)
	{
		if(controller->rumbleController->rumbleQueue)
		{
			SAFE_FREE(controller->rumbleController->rumbleQueue);
		}
		if(controller->rumbleController->buzzerQueue)
		{
			SAFE_FREE(controller->rumbleController->buzzerQueue);
		}
		SAFE_FREE(controller->rumbleController);
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawRumbleDebugInfo
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawRumbleDebugInfo(PADCONTROLSTATUS *controller)
{
	char			stringBuf[64];
	ushort			string16Buf[64];
	TBMatrix		matrix;

	bmMatScale(matrix, 0.5f, 0.5f, 0.5f);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetObjectMatrix(matrix);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	sprintf(stringBuf, "Rumble = %d", controller->rumbleController->rumbleLevel);
	bkString8to16(string16Buf, stringBuf);
	bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 250.0f, 127,127,127, 127, NULL, NULL);

	if(controller->rumbleController->buzzerState)	sprintf(stringBuf, "Buzzer = ON");
	else											sprintf(stringBuf, "Buzzer = OFF");
	bkString8to16(string16Buf, stringBuf);
	bdPrintFontLite(standardFont, string16Buf, strlen(stringBuf), -500.0f, 200.0f, 127,127,127, 127, NULL, NULL);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}