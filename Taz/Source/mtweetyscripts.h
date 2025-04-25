#ifndef __H_MTWEETYSCRIPTS
#define __H_MTWEETYSCRIPTS

#include "fade.h"
#include "mechatweety.h"
#include "bossgamecontinue.h"

/************************************************************************************************/
/* structures used for mechatweety scripting are defined as below, in mechatweety.h				*/
/*																								*/
/*	typedef struct TAG_MTWEETY_EVENT															*/
/*	{																							*/
/*		EMTweetyAIState		action;																*/
/*		char				*targetInstance;													*/
/*		float				time;																*/	
/*		MTWEETY_RUMBLE		*rumble;															*/
/*	} MTWEETY_EVENT, _MTWEETY_EVENT;															*/
/*																								*/
/*	typedef struct TAG_MTWEETY_RUMBLE															*/
/*	{																							*/
/*		// CMD: rumble																			*/
/*		int			r_amplitude;																*/
/*		float		r_attack, r_sustain, r_duration, r_delay, r_frequency, r_freqRateOfChange;	*/
/*		EWaveform	r_waveform;																	*/
/*		// CMD: buzzer																			*/
/*		float		b_duration, b_delay, b_frequency, b_freqRateOfChange;						*/
/*		EWaveform	b_waveform;																	*/
/*	}MTWEETY_RUMBLE, _MTWEETYRUMBLE;															*/
/*																								*/
/*	enum EWaveform																				*/
/*	{																							*/
/*		WAVEFORM_NORMAL,																		*/
/*		WAVEFORM_SINUSOIDAL,																	*/
/*		WAVEFORM_SQUAREDSINUSOIDAL,																*/
/*		WAVEFORM_SQUARE,																		*/
/*		WAVEFORM_SAWTOOTH,																		*/
/*		WAVEFORM_RANDOM,																		*/
/*	};																							*/
/************************************************************************************************/

/*##############################################################################################*/
/*									Rumble and Buzzer Scripts									*/

static MTWEETY_RUMBLE tweetyHitRumble = 
{
	200,
	0.0f, 0.2f, 0.5f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_NORMAL,
	0.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_NONE,
};

static MTWEETY_RUMBLE openDoorRumble = 
{
	150,
	0.0f, 0.7f, 1.0f, 1.25f, 2.0f, 0.0f,
	WAVEFORM_SQUAREDSINUSOIDAL,
	0.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_NONE,
};

static MTWEETY_RUMBLE magnetRaiseRumble = 
{
	200,
	0.0f, 0.1f, 0.5f, 0.75f, 0.0f, 0.0f,
	WAVEFORM_NORMAL,
	0.0f, 0.0f, 0.0f, 0.0f, 
	WAVEFORM_NORMAL,
};

static MTWEETY_RUMBLE tweetyElectrocutedRumble = 
{
	225,
	0.0f, 1.6f, 2.2f, 0.0f, 6.0f, 0.0f,
	WAVEFORM_SQUARE,
	2.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_RANDOM,
};

static MTWEETY_RUMBLE flyToRocketsStart = 
{
	250,
	0.0f, 0.0f, 8.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_RANDOM,
	0.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_NONE,
};

static MTWEETY_RUMBLE standardRocket = 
{
	150,
	0.0f, 2.0f, 3.0f, 0.0f, 0.0f, 0.0f,
	WAVEFORM_RANDOM,
	0.0f,0.0f, 0.0f, 0.0f,
	WAVEFORM_NONE,
};

/*##############################################################################################*/
/*									MechaTweety Event Scripts									*/

static MTWEETY_EVENT crateRoomSlowAttackLeft[] = 
{
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_WALKTOTARGET, "cratepickup02", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "cratepickup02", 0.0, NULL, NULL},
	{MTWEETYAI_GETCRATE, "low", 0.0f, NULL, "mechacratepickup_1.wav"},
	{MTWEETYAI_DROPSHIELDFLICKER, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKCRATE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT crateRoomSlowAttackRight[] = 
{
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_WALKTOTARGET, "cratepickup01", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "cratepickup01", 0.0, NULL,NULL},
	{MTWEETYAI_GETCRATE, "low", 0.0f, NULL, "mechacratepickup_2.wav"},
	{MTWEETYAI_DROPSHIELDFLICKER, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKCRATE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT crateRoomQuickAttackLeft[] = 
{
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_RUNTOTARGET, "cratepickup02", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "cratepickup02", 0.0, NULL, NULL},
	{MTWEETYAI_GETCRATE, "low", 0.0f, NULL, "mechacratepickup_1.wav"},
	{MTWEETYAI_DROPSHIELDFLICKER, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKCRATE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_LOOP, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT crateRoomQuickAttackRight[] = 
{
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_RUNTOTARGET, "cratepickup01", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "cratepickup01", 0.0f, NULL, NULL},
	{MTWEETYAI_GETCRATE, "low", 0.0f, NULL, "mechacratepickup_2.wav"},
	{MTWEETYAI_DROPSHIELDFLICKER, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKCRATE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_LOOP, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT hitByCrate[] = 
{
	{MTWEETYAI_STUNNED, "react1", 0.0f, &tweetyHitRumble, "damage"},
	{MTWEETYAI_RUNTOTARGET, "home", 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT moveToMagnetRoom[] = 
{
	{MTWEETYAI_ANIM, "sulk", 0.0f, NULL, NULL},
	{MTWEETYAI_TRANSPORTOPP, "door01marker", 0.0f, NULL, NULL},
	{MTWEETYAI_OPENDOOR, "opendoor", 1.0f, &openDoorRumble, NULL},
//	{MTWEETYAI_COLLISIONOFF, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_NEXTCAMERA, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_RUNTOTARGET, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
///	{MTWEETYAI_COLLISIONON, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_RAISESHIELD, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, "mechataunt_4.wav"},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT powerUp[] = 
{
	{MTWEETYAI_WALKTOTARGET, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_MAGNETCHARGE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_RAISEMAGNET, NULL, 0.0f, &magnetRaiseRumble, NULL},
	{MTWEETYAI_STOPSAMPLE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKELECLEFT, NULL, 1.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKELECRIGHT, NULL, 1.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKELECLEFT, NULL, 1.0f, NULL, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ATTACKELECRIGHT, NULL, 1.0f, NULL, NULL},
	{MTWEETYAI_LOWERMAGNET, NULL, 0.0f, &magnetRaiseRumble, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT hurrahAndLowerMagnet[] = 
{
	{MTWEETYAI_ANIM, "celebrate", 0.0f, NULL, "celebrate"},
	{MTWEETYAI_LOWERMAGNET, NULL, 0.0f, &magnetRaiseRumble, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT hurrah[] = 
{
	{MTWEETYAI_ANIM, "celebrate", 0.0f, NULL, "celebrate"},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT grumpyStompAndLowerMagnet[] = 
{
	{MTWEETYAI_ANIM, "idle2", 2.0f, NULL, NULL},
	{MTWEETYAI_LOWERMAGNET, NULL, 0.0f, &magnetRaiseRumble, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT moveToRockets[] = 
{
	{MTWEETYAI_WALKTOTARGET, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_TURN, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_DROPSHIELDFLICKER, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_MAGNETCHARGE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_LOWERSHIELD, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "react2", 0.0f, &tweetyElectrocutedRumble, "mechataunt_10.wav"},
	{MTWEETYAI_TURNOPP, "magnetcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_WALKTOTARGET, "door02marker", 0.0f, NULL, NULL},
	{MTWEETYAI_TURNOPP, "door02marker", 0.0f, NULL, NULL},
	{MTWEETYAI_OPENDOOR, "opendoor", 2.0f, &openDoorRumble, NULL},
	{MTWEETYAI_NEXTCAMERA, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_RUNTOTARGET, "liftoff", 0.0f, NULL, NULL},
	{MTWEETYAI_FLYTOTARGET, "rocketfire", 0.0f, &flyToRocketsStart, NULL},
	{MTWEETYAI_TURN, "rocketfire", 0.0f, NULL, NULL},
	{MTWEETYAI_WALKTOTARGET, "rocketfire", 0.0f, NULL, NULL},
	{MTWEETYAI_SETHOMEPOS, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_SCALE, "defend", 2.0f, NULL, "mechataunt_14.wav"},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT rocketAttackLeft[] = 
{
	{MTWEETYAI_FLYTOTARGET, "chargeattack01", 0.0f, &standardRocket, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ROCKETATTACK, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "attack4", 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "startcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT rocketAttackMiddle[] = 
{
	{MTWEETYAI_FLYTOTARGET, "rocketfire", 0.0f, &standardRocket, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ROCKETATTACK, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "attack4", 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "startcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT rocketAttackRight[] = 
{
	{MTWEETYAI_FLYTOTARGET, "chargeattack02", 0.0f, &standardRocket, NULL},
	{MTWEETYAI_TURN, "facetaz", 0.0f, NULL, NULL},
	{MTWEETYAI_ROCKETATTACK, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "attack4", 0.0f, NULL, NULL},
	{MTWEETYAI_ANIM, "startcharge", 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT defeated[] = 
{
	{MTWEETYAI_ANIM, "sulk", 0.0f, NULL, NULL},
	{MTWEETYAI_LOOP, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT resetToHome[] = 
{
	{MTWEETYAI_TRANSPORT, "home", 0.0f, NULL, NULL},
	{MTWEETYAI_END, NULL, 0.0f, NULL, NULL},
};

static MTWEETY_EVENT holdAtPosition[] = 
{
	{MTWEETYAI_IDLE, NULL, 0.0f, NULL, NULL},
	{MTWEETYAI_LOOP, NULL, 0.0f, NULL, NULL},
};

//#ifdef _TAZDEBUG
extern int32	allowDebugSkip;
//#endif
/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void UpdateAndChooseMechaTweetySequences(struct TAG_ACTORINSTANCELIST *actorInstanceList);

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

void RequestNewMechaTweetySequence(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_MTWEETY_EVENT *newScript);

/* --------------------------------------------------------------------------------
   Function : UpdateAndChooseMechaTweetySequences
   Purpose : 
   Parameters : 
   Returns : 
   Info :
*/

int32 ResetMechaTweetyCallback(void *context, EBossGameResetChoice choice);

#endif