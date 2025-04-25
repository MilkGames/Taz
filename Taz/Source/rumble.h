#ifndef __H_RUMBLE
#define __H_RUMBLE

#include "control.h"

// CMD: flags
#define RUMBLEFLAG_PAUSE		(1<<0)
#define RUMBLEFLAG_DISABLE		(1<<1)
#define RUMBLEFLAG_LOOP			(1<<2)
#define RUMBLEFLAG_YOYO			(1<<3)
#define RUMBLEFLAG_TRACKING		(1<<4)

// CMD: constants
#define RUMBLE_MAXTRACKINGDIST	(METERTOUNIT(15.0f))
#define RUMBLE_MINTRACKINGDIST	(METERTOUNIT(1.0f))
#define RUMBLE_MINMINUSMAX		(RUMBLE_MINTRACKINGDIST-RUMBLE_MAXTRACKINGDIST)

enum ERumbleStage
{
	RUMBLESTAGE_ATTACK,
	RUMBLESTAGE_SUSTAIN,
	RUMBLESTAGE_DECAY,
};

enum EWaveform
{
	// CMD: standard waveforms
	WAVEFORM_NONE,
	WAVEFORM_NORMAL,
	WAVEFORM_SINUSOIDAL,
	WAVEFORM_SQUAREDSINUSOIDAL,
	WAVEFORM_SQUARE,
	WAVEFORM_SAWTOOTH,
	WAVEFORM_RANDOM,

	// CMD: preset sequences, note to use these you must call 'RequestPresetRumble'
	WAVEFORM_EXPLOSION,
	WAVEFORM_BOOM,
	WAVEFORM_DRILL,
	WAVEFORM_SPIN,
};

typedef struct TAG_RUMBLEEFFECT
{
	uint32			posInstCRC;					// CMD: used for tracking rumbles
	int				amplitude;					// CMD: current amplitude of rumble (0..255)
	int				targetAmplitude;			// CMD: maximum amplitude for effect
	float			delay;						// CMD: remaing delay
	float			duration;					// CMD: remaining duration of rumble

	float			attack;						// CMD: remaining attack time
	float			sustain;					// CMD: remaining sustain time
	float			attackRate;					// CMD: rate of increase in amplitude (units per second)
	float			decayRate;					// CMD: rate of decay

	float			frequency;					// CMD: frequency of oscillating rumble
	float			frequencyRate;				// CMD: rate of change of frequency per second
	EWaveform		waveform;					// CMD:	type of wave form for rumble

	uint32			flags;						// CMD: local flags
	float			cutoffClock;				// CMD: clock used for effects such as square/sawtooth wave

	float			initialDelay;				// CMD: used if looping
	float			initialAttack;				// CMD: used if looping
	float			initialSustain;				// CMD: used if looping
	float			initialDuration;			// CMD: used if looping

	float			initialFrequency;			// CMD: used if looping
	ERumbleStage	stage;
}RUMBLEEFFECT, _RUMBLEEFFECT;

typedef struct TAG_BUZZEREFFECT
{
	uint32			posInstCRC;					// CMD: used for tracking buzzes
	float			delay;						// CMD: remaing delay
	float			duration;					// CMD: remaining duration of rumble

	float			frequency;					// CMD: frequency of oscillating rumble
	float			frequencyRate;				// CMD: rate of change of frequency per second
	EWaveform		waveform;					// CMD:	type of wave form for rumble

	uint32			flags;						// CMD: local flags
	float			cutoffClock;				// CMD: clock used for effects such as square/sawtooth wave

	float			initialDelay;				// CMD: used if looping
	float			initialDuration;			// CMD: used if looping
	float			initialFrequency;			// CMD: used if looping
}BUZZEREFFECT, _BUZZEREFFECT;

typedef struct TAG_RUMBLECONTROLLER
{
	RUMBLEEFFECT	*rumbleQueue;				// CMD: ptr to array of rumble effect structs
	BUZZEREFFECT	*buzzerQueue;				// CMD: ptr to array of buzzer effect structs (same size as rumble queue) (only on ps2)
	int				sizeQueue;					// CMD: number of effects in the queue
	uint32			flags;						// CMD: global flags

	int				rumbleLevel;				// CMD: actual level of rumble this frame, used for debug purposes only
	int				buzzerState;				// CMD: state of buzzer this frame, used for debug purposes only
}RUMBLECONTROLLER, _RUMBLECONTROLLER;

/*  --------------------------------------------------------------------------------
	Function	: CreateRumbleController
	Purpose		: Creates and initialises rumble controller for pad
	Parameters	: max number of effects in queue
	Returns		: ptr to the contoller
	Info		: 
*/

RUMBLECONTROLLER *CreateRumbleController(int effectsQueueSize);

/*  --------------------------------------------------------------------------------
	Function	: RequestRumbleEffect
	Purpose		: Sends a request for a rumble effect with given parameters
	Parameters	: 
	Returns		: FALSE if queue full
	Info		: 
*/

int RequestRumbleEffect(struct TAG_PADCONTROLSTATUS *controller,int amplitude,float attack,float sustain,float duration,float delay,EWaveform waveform=WAVEFORM_NORMAL,float frequency=0.0f,float freqRateOfChange=0.0f,uint32 flags=0,uint32 posInstCRC=0);

/*  --------------------------------------------------------------------------------
	Function	: RequestBuzzerEffect
	Purpose		: Sends a request for a buzzer effect with given parameters
	Parameters	: 
	Returns		: handle to queue member, -1 if queue full
	Info		: 
*/

int RequestBuzzerEffect(struct TAG_PADCONTROLSTATUS *controller,float duration,float delay,EWaveform waveform=WAVEFORM_NORMAL,float frequency=0.0f,float freqRateOfChange=0.0f,uint32 flags=0,uint32 posInstCRC=0);

/*  --------------------------------------------------------------------------------
	Function	: RequestRumbleEffect
	Purpose		: Sends a request for a rumble effect with given parameters
	Parameters	: 
	Returns		: handle to queue member, -1 if queue full
	Info		: 
*/

int RequestPresetRumble(struct TAG_PADCONTROLSTATUS *controller,int amplitude,float duration,EWaveform waveform, uint32 posInstCRC=0, uint32 flags=0);

/*  --------------------------------------------------------------------------------
	Function	: UpdateRumble
	Purpose		: Updates the rumble effect for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateRumble(struct TAG_PADCONTROLSTATUS *controller, float t=fTime);

/*  --------------------------------------------------------------------------------
	Function	: FlushRumbleQueue
	Purpose		: Flushes the entire rumble queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FlushRumbleQueue(struct TAG_PADCONTROLSTATUS *controller);

/*  --------------------------------------------------------------------------------
	Function	: FlushRumbleQueueMember
	Purpose		: Flushes a member of the rumble queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

int FlushRumbleQueueMember(struct TAG_PADCONTROLSTATUS *controller, int handle);

/*  --------------------------------------------------------------------------------
	Function	: FlushBuzzerQueueMember
	Purpose		: Flushes a member of the buzzer queue for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

int FlushBuzzerQueueMember(struct TAG_PADCONTROLSTATUS *controller, int handle);

/*  --------------------------------------------------------------------------------
	Function	: PauseRumble
	Purpose		: Pause the rumble update for a controller
	Parameters	: 
	Returns		: 
	Info		: // PP: IMPORTANT: YOU MUST CONTINUALLY CALL bkUpdate (or bdFlip) for this to have any effect
*/

void PauseRumble(struct TAG_PADCONTROLSTATUS *controller);

/*  --------------------------------------------------------------------------------
	Function	: UnPauseRumble
	Purpose		: Unpause the rumble update for a controller
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UnPauseRumble(struct TAG_PADCONTROLSTATUS *controller);

/*  --------------------------------------------------------------------------------
	Function	: DisableRumble
	Purpose		: disables the controller such that all existing rumbles will be paused and any new requests will be declined
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DisableRumble(struct TAG_PADCONTROLSTATUS *controller);

/*  --------------------------------------------------------------------------------
	Function	: EnableRumble
	Purpose		: undoes effect of DisableRumble
	Parameters	: 
	Returns		: 
	Info		: 
*/

void EnableRumble(struct TAG_PADCONTROLSTATUS *controller);

/*  --------------------------------------------------------------------------------
	Function	: CheckIfRumblePlaying
	Purpose		: checks to see if a rumble is still rumbling
	Parameters	: 
	Returns		: 
	Info		: 
*/

int CheckIfRumblePlaying(struct TAG_PADCONTROLSTATUS *controller, int handle);

/*  --------------------------------------------------------------------------------
	Function	: FreeRumbleController
	Purpose		: frees rumble controller memory
	Parameters	: 
	Returns		: 
	Info		: 
*/

void FreeRumbleController(struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : DrawRumbleDebugInfo
   Purpose : Crate mine cart specific stuff
   Parameters : 
   Returns : 
   Info : 
*/

void DrawRumbleDebugInfo(struct TAG_PADCONTROLSTATUS *controller);

#endif