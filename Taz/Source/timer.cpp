// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : timer.cpp
//   Purpose : timer functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "timer.h"
#include "display.h"
#include "main.h"
#include "textfx.h"
#include "demo.h"			// PP: consumer demo specific stuff

// PP: ********************************** TEMPORARY FRIG *************************************
// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.
#if(BPLATFORM == XBOX)
TBStopwatch		XboxJitterSuppressionStopwatch;
#endif// PP: (BPLATFORM == XBOX)
// PP: ***************************************************************************************

char			showTimerBars = FALSE;

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseProfileTimer
	Purpose 	: Initialise a timer structure
	Parameters 	: TIMER
	Returns 	: 
	Info 		: 
*/

void InitialiseProfileTimer(TIMER *timer)
{
	if (timer->state==TIMER_STOP)
	{
		timer->frameStartTime = bkTimerRead();
	}
	timer->currentSlice = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: StartProfileTimer
	Purpose 	: Start the timer counting.
	Parameters 	: profile, timer number, name or NULL for use existing
	Returns 	: 
	Info 		: Called at the beginning of the code to be timed.
*/
void StartProfileTimer(PROFILE *profile,int timer,char *name)
{
	if (profile->timers[timer].currentSlice >= MAX_SLICES) return;

	if (profile->timers[timer].state == TIMER_START)
	{
		// TP: if timer already active then close
		EndProfileTimer(profile,timer);
		if (profile->timers[timer].currentSlice >= MAX_SLICES) return;
	}

	if (name) strcpy(profile->timers[timer].name,name);
	profile->active[timer] = TRUE;
	profile->timers[timer].startTime[profile->timers[timer].currentSlice] = bkTimerRead();
	profile->timers[timer].state = TIMER_START;
}

/*	--------------------------------------------------------------------------------
	Function 	: EndProfileTimer
	Purpose 	: Stop the time counting.
	Parameters 	: profile, timer number
	Returns 	: 
	Info 		: Called at the end of the code to be timed.
*/
void EndProfileTimer(PROFILE *profile, int timer)
{
	if (profile->timers[timer].state == TIMER_STOP) return;
	profile->timers[timer].endTime[profile->timers[timer].currentSlice++] = bkTimerRead();
	profile->timers[timer].state = TIMER_STOP;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseProfile
	Purpose 	: Initialises a profile and all timers within
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Can be called at any time to completely reset the profile
*/
void InitialiseProfile(PROFILE *profile)
{
	int i;

	for (i=0;i<MAX_TIMERS;i++)
	{
		profile->timers[i].state = TIMER_STOP;
		profile->timers[i].name[0] = 0;
		profile->active[i] = FALSE;
		profile->draw[i] = FALSE;
		InitialiseProfileTimer(&profile->timers[i]);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: StartProfile
	Purpose 	: Records the current time as a reference point for the timers.
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Should be called just after performing the screen flip.
*/
void StartProfile(PROFILE *profile)
{
	int i;

	for (i=0;i<MAX_TIMERS;i++)
	{
		profile->active[i] = FALSE;
		InitialiseProfileTimer(&profile->timers[i]);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: EndProfile
	Purpose 	: Performs cleanup before displaying the profile info
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Should be called just before StartProfile
*/
void EndProfile(PROFILE *profile)
{
	int i,j;

	for (i=0;i<MAX_TIMERS;i++)
	{
		if (profile->timers[i].state == TIMER_START)
		{
			EndProfileTimer(profile,i);
		}
		if (profile->active[i])
		{
			profile->frameStartTimes[i] = profile->timers[i].frameStartTime;
			for (j=0;j<profile->timers[i].currentSlice;j++)
			{
				profile->startTimes[i][j] = profile->timers[i].startTime[j];
				profile->endTimers[i][j] = profile->timers[i].endTime[j];
			}
			profile->sliceCount[i] = profile->timers[i].currentSlice;
			profile->active[i] = FALSE;
			profile->draw[i] = TRUE;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: DisplayProfile
	Purpose 	: Draws the profile infomation to the screen.
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Called in a suitable place, note the information will be a frame out of date
*/
void DisplayProfile(PROFILE *profile)
{
	int				i,j;
	TBTimerValue	tempTimer;
	float			tempFloat;
	char			buf[255];
	ushort			string16[255];
	float			scaleFactor;
	float			maxTime=0;
	float			ticksPerFrame;
	int				numFrames;
	char			col;

	if (showTimerBars==FALSE) return;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,FALSE,FALSE);
	bdSetTexture(0, NULL);

	DrawRectangle(RECTANGLE(-150.0f,250.0f,-150.0f,150.0f),COLOUR(0,0,0,64));

	for (i=0;i<MAX_TIMERS;i++)
	{
		if (profile->draw[i])
		{
			tempTimer = bkTimerDelta(profile->frameStartTimes[i],profile->endTimers[i][profile->sliceCount[i]-1]);
			tempFloat = (float)tempTimer;
			// print timers
			sprintf(buf, " %s[%d]", profile->timers[i].name, bkTimerDelta(profile->startTimes[i][0],profile->endTimers[i][profile->sliceCount[i]-1]));
			if (tempFloat>maxTime) maxTime = tempFloat;
			bkString8to16(string16, buf);
			bdPrintFontLite(standardFont, string16, strlen(buf), -320.0f,150.0f-(30.0f*i), 255,255,255, 255, NULL, NULL);
		}
	}

	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		ticksPerFrame = (float)(bkTimerGetFrequency()) / 60;
	}
	else
	{
		ticksPerFrame = (float)(bkTimerGetFrequency()) / 50;
	}

	maxTime /= ticksPerFrame;

	numFrames = (int)(maxTime)+1;

	scaleFactor = 400 / (ticksPerFrame*numFrames);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,FALSE,FALSE);
	bdSetTexture(0, NULL);

	for (i=0;i<(numFrames+1);i++)
	{
		DrawRectangle(RECTANGLE(-150.0f+(400/numFrames*i),-149.0f+(400/numFrames*i),150.0f,-150.0),COLOUR(255,0,0,255));
	}

	for (i=0;i<MAX_TIMERS;i++)
	{
		if (profile->draw[i])
		{
			col = 0;
			for (j=0;j<profile->sliceCount[i];j++)
			{
				DrawRectangle(RECTANGLE(-150.0f+(bkTimerDelta(profile->frameStartTimes[i],profile->startTimes[i][j])*scaleFactor),
					-150.0f+(bkTimerDelta((TBTimerValue)profile->frameStartTimes[i],(TBTimerValue)profile->startTimes[i][j])*scaleFactor)+(bkTimerDelta((TBTimerValue)profile->startTimes[i][j],(TBTimerValue)profile->endTimers[i][j])*scaleFactor),
					125.0f-(30*i),
					125.0f-(30*i)+20),COLOUR(col==0?255:0,col==1?255:0,col==2?255:0,255));


//				DrawRectangle(-150.0f+(bkTimerDelta(profile->frameStartTimes[i],profile->startTimes[i][j])*scaleFactor),125.0f-(30*i),
//					(bkTimerDelta(profile->startTimes[i][j],profile->endTimers[i][j])*scaleFactor),20,col==0?255:0,col==1?255:0,col==2?255:0,255);
				col++;
				col = col%3;
			}
			profile->draw[i] = FALSE;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_CLIPPING,TRUE,FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function		: startTimingFrame
   Purpose		: start timing the frame
   Parameters	: 
   Returns		: 
   Info			: 
*/
void startTimingFrame(void)
{
	gameStatus.startTime = bkTimerRead();

	// PP: ********************************** TEMPORARY FRIG *************************************
	// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
	// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.
	START_XBOX_JITTER_SUPPRESSION_STOPWATCH;
	// PP: ***************************************************************************************
}


/* --------------------------------------------------------------------------------
   Function		: finishTimingFrame
   Purpose		: finish timing the frame
   Parameters	: 
   Returns		: 
   Info			: // PP: calculates frames per second, frame length, etc.
*/
void finishTimingFrame(void)
{
	// sort out game timers
	gameStatus.endTime = bkTimerRead();
	gameStatus.deltaTime=bkTimerDelta(gameStatus.startTime,gameStatus.endTime);

	// PP: ********************************** TEMPORARY FRIG *************************************
	// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
	// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.
	STOP_AND_READ_XBOX_JITTER_SUPPRESSION_STOPWATCH;
	// PP: ***************************************************************************************

	rawFPS = bkTimerToFPS(gameStatus.deltaTime);
	
	// PP: prevent divide-by-zero...

	if(rawFPS == 0.0f)
	{
		rawFTime = 0.0f;
	}
	else
	{
		rawFTime = 1.0f/rawFPS;
	}

	// PP: Effect any game-speed alterations, such as the TURBO and SLOWMO cheats and the global game-speed factor which can be used for special effects
	ApplyGameSpeedAdjustments();

	fps = bkTimerToFPS(gameStatus.deltaTime);

	// PP: prevent divide-by-zero...

	if(fps == 0.0f)
	{
		fTime=0.0f;
		speedRatio=0.0f;
	}
	else
	{
		fTime = 1.0f/fps;

		// PP: NOTE: gameTime and appTime look suspiciously similar to me...

		gameStatus.gameTime += 1/fps;

		// PP: NEW: SURELY appTime WAS MEANT TO BE *REAL* TIME???!!!!

		gameStatus.appTime += rawFTime;// PP: REMOUT: fTime;

		speedRatio = 60/fps;
	}

#ifdef CONSUMERDEMO
	// PP: udpate the idle time for the consumer demo
	demoIdleTime+=fTime;
#endif
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TIME EFFECTS

											Functions to fiddle about with time.

		NOW, BE REAL CAREFUL WHEN USING THESE - I DON'T EXPECT OUR PORTUGESE COLLISION SYSTEM TO STAND UP TO MUCH TORTURE.
									" WHAT DO YOU THINK I AM - SOME KIND OF 'SUPER-MAN'? "
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include "LoopArray.h"										// PP: Looping array class - please use it!
#include "VideoFX.h"										// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!


float		TimeFX_jumpTime=0.0f;							// PP: length of time to be skipped next frame - you can either set this directly or call TimeFX_jump
float		TimeFX_globalRate=1.0f;							// PP: the 'normal' game speed to switch back to when queued rates end
bool		TimeFX_noQueue=true;							// PP: there are no queued time rates
ETimeMode	TimeFX_timeMode=TIMEFX_DEFAULT_TIME_MODE;		// PP: the time mode used by the TimeFX_stuff - this determines the way lengths of time are interpreted.  You can set this directly.

// PP: type for entries into the time-rate queue
struct TIMERATE
{
	float		rate;										// PP: time rate to apply
	float		length;										// PP: length of time rate in 'raw' (not real) time
	float		motionBlur;									// PP: level of motion blur to use
	bool		timeslice;									// PP: this is a timeslice effect not a time rate - ignore the other members

}BALIGN16;


#define MAX_TIMERATES						10				// PP: that'll be plenty I'm sure

static LOOPARRAY<TIMERATE, MAX_TIMERATES>	TimeFX_queue;	// PP: queue of time rates


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_jump
	Purpose 	: instantly jump forward in time
	Parameters 	: length in seconds of jump
	Returns 	: 
	Info 		: 
*/
void TimeFX_jump(const float jump)
{
	TimeFX_jumpTime=jump;
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueJump
	Purpose 	: queue an instant forward jump in time
	Parameters 	: length in seconds of jump
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueJump(const float jump)
{
	// PP: can't queue a jump of 0.0f - it's no use and it will be misinterpreted
	if(jump == 0.0f) return;

	TimeFX_queueRate(0.0f, jump);
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_setGlobalRate
	Purpose 	: set the 'normal' time rate
	Parameters 	: the 'normal; time rate
	Returns 	: 
	Info 		: NOTE THAT THIS CANCELS ALL QUEUED RATES AND SETS THE GLOBAL RATE AS CURRENT RATE
*/
void TimeFX_setGlobalRate(const float rate)
{
	TimeFX_globalRate=rate;
	TimeFX_cancelRates();
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueGlobalRate
	Purpose 	: set the 'normal' time rate (to take effect when the queue empties)
	Parameters 	: the 'normal; time rate
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueGlobalRate(const float rate)
{
	TimeFX_queueRate(rate, 0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueRate
	Purpose 	: Queue a change in rate of time to last for a specified length of...time
	Parameters 	: new rate of time, length of real time to hold new rate of time for, (opt/0.0f=none) level of motion blur for the rate
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueRate(const float rate, const float length, const float motionBlur)
{
	TIMERATE*		timeRate;
	
	timeRate=TimeFX_queue.getNew();

	timeRate->rate=rate;

	if(TimeFX_timeMode == TIMEMODE_RAW)
	{
		timeRate->length=length;
	}
	else// PP: (TimeFX_timeMode == TIMEMODE_REAL)
	{
		timeRate->length=length*rate;
	}

	timeRate->motionBlur=motionBlur;

	timeRate->timeslice=false;

	TimeFX_noQueue=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueTimeslice
	Purpose 	: Queue a timeslice effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueTimeslice(void)
{
	TIMERATE*		timeRate;
	
	timeRate=TimeFX_queue.getNew();

	timeRate->timeslice=true;

	TimeFX_noQueue=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: TimerFX_effectTimeRate
	Purpose 	: effect a time rate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
static bool TimeFX_effectTimeRate(TIMERATE* const timeRate)
{
	bool	nextRate;

	// PP: check for timeslice flag first
	if(timeRate->timeslice)// PP: timeslice
	{
		VFX_timeslice();
		nextRate=true;

		//bkPrintf("TimerFX_effectTimeRate: ****** TIMESLICE! *******\n");
	}
	else
	{
		if(timeRate->length == 0.0f)// PP: global rate
		{
			nextRate=true;

			//bkPrintf("TimerFX_effectTimeRate: ****** GLOBAL! (%5.2f) *******\n", timeRate->rate);

			TimeFX_globalRate=timeRate->rate;
		}
		else
		{
			if(timeRate->rate == 0.0f)// PP: jump
			{
				//bkPrintf("TimerFX_effectTimeRate: ****** JUMP! (%5.2f seconds) *******\n", timeRate->length);

				TimeFX_jump(timeRate->length);

				return true;// PP: proceed to next rate
			}
			else// PP: rate
			{
				//bkPrintf("TimerFX_effectTimeRate: ****** RATE! (%5.2f for %5.2f seconds) *******\n", timeRate->rate, timeRate->length);

				timeRate->length-=fTime;
				
				nextRate=(timeRate->length <= 0.0f);
			}
		}

		gameStatus.gameSpeed=timeRate->rate;

		VFX_motionBlur(timeRate->motionBlur);
	}

	return nextRate;
}



/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_update
	Purpose 	: update & apply time effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_update(void)
{
	static TIMERATE*	lastRate=NULL;
	TIMERATE*			timeRate;
	bool				nextRate;

	timeRate=TimeFX_queue.getOldest();

	if(timeRate == NULL)// PP: if the queue is empty
	{
		if(lastRate != NULL)// PP: only if we've just this second emptied the queue
		{
			TimeFX_cancelRates();
			lastRate=NULL;
		}

		return;// PP: no queued time rates
	}

	nextRate=TimeFX_effectTimeRate(timeRate);
	
	if(nextRate)// PP: move onto next rate
	{
		TimeFX_queue.discardOldest();
	}
	
	lastRate=timeRate;
}


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_cancelRates
	Purpose 	: cancel all queued time rates; apply global time rate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_cancelRates(void)
{
	TimeFX_queue.init();
	gameStatus.gameSpeed=TimeFX_globalRate;
	TimeFX_noQueue=true;

	// NH: Change speed of sound, so that that slows down/speeds up with the visuals
	// NH: Don't think this is the right place so I've taken it out till I can have a better look
	//	SetSoundSpeed(TimeFX_globalRate);
}