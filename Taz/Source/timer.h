#ifndef __H_TIMER
#define __H_TIMER

#define MAX_TIMERS (12)
#define MAX_SLICES (10)

enum
{
	TIMER_STOP,
	TIMER_START,
};

typedef struct TAG_TIMER
{
	TBTimerValue	frameStartTime;
	TBTimerValue	startTime[MAX_SLICES];
	TBTimerValue	endTime[MAX_SLICES];
	char			name[32];
	char			state;
	char			currentSlice;
} TIMER;

typedef struct TAG_PROFILE
{
	TIMER			timers[MAX_TIMERS];
	TBTimerValue	frameStartTimes[MAX_TIMERS];
	TBTimerValue	startTimes[MAX_TIMERS][MAX_SLICES];
	TBTimerValue	endTimers[MAX_TIMERS][MAX_SLICES];
	char			active[MAX_TIMERS];
	char			draw[MAX_TIMERS];
	char			sliceCount[MAX_TIMERS];
} PROFILE;

extern char	showTimerBars;


// PP: ********************************** TEMPORARY FRIG *************************************
// PP: I'm using a stopwatch to smooth-out the framerate on Xbox.
// PP: This supresses the mystery framerate jitter which I'm still no closer to understanding.

#if(BPLATFORM == XBOX)

extern TBStopwatch			XboxJitterSuppressionStopwatch;

#define INIT_XBOX_JITTER_SUPPRESSION_STOPWATCH \
		bkInitStopwatch(&XboxJitterSuppressionStopwatch, 4); \
		START_XBOX_JITTER_SUPPRESSION_STOPWATCH

#define START_XBOX_JITTER_SUPPRESSION_STOPWATCH \
		bkStartStopwatch(&XboxJitterSuppressionStopwatch);

#define STOP_AND_READ_XBOX_JITTER_SUPPRESSION_STOPWATCH \
		bkStopStopwatch(&XboxJitterSuppressionStopwatch); \
		gameStatus.deltaTime = MAX(12222226, XboxJitterSuppressionStopwatch.value);// PP: 12222226 is the number of timer ticks in 1/60 of a second.  I stress again - this is a TEMPORARY frig.

#else// PP: if(BPLATFORM == XBOX) else

#define INIT_XBOX_JITTER_SUPPRESSION_STOPWATCH

#define START_XBOX_JITTER_SUPPRESSION_STOPWATCH

#define STOP_AND_READ_XBOX_JITTER_SUPPRESSION_STOPWATCH

#endif// PP: (BPLATFORM == XBOX)

// PP: ***************************************************************************************


/*	--------------------------------------------------------------------------------
	Function 	: InitialiseProfileTimer
	Purpose 	: Initialise a timer structure
	Parameters 	: TIMER
	Returns 	: 
	Info 		: 
*/
void InitialiseProfileTimer(TIMER *timer);

/*	--------------------------------------------------------------------------------
	Function 	: StartProfileTimer
	Purpose 	: Start the timer counting.
	Parameters 	: profile, timer number, name or NULL for use existing
	Returns 	: 
	Info 		: Called at the beginning of the code to be timed.
*/
void StartProfileTimer(PROFILE *profile,int timer,char *name);

/*	--------------------------------------------------------------------------------
	Function 	: EndProfileTimer
	Purpose 	: Stop the time counting.
	Parameters 	: profile, timer number
	Returns 	: 
	Info 		: Called at the end of the code to be timed.
*/
void EndProfileTimer(PROFILE *profile,int timer);

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseProfile
	Purpose 	: Initialises a profile and all timers within
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Can be called at any time to completely reset the profile
*/
void InitialiseProfile(PROFILE *profile);

/*	--------------------------------------------------------------------------------
	Function 	: StartProfile
	Purpose 	: Records the current time as a reference point for the timers.
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Should be called just after performing the screen flip.
*/
void StartProfile(PROFILE *profile);

/*	--------------------------------------------------------------------------------
	Function 	: EndProfile
	Purpose 	: Performs cleanup before displaying the profile info
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Should be called just before StartProfile
*/
void EndProfile(PROFILE *profile);

/*	--------------------------------------------------------------------------------
	Function 	: DisplayProfile
	Purpose 	: Draws the profile infomation to the screen.
	Parameters 	: PROFILE
	Returns 	: 
	Info 		: Called in a suitable place, note the information will be a frame out of date
*/
void DisplayProfile(PROFILE *profile);


/* --------------------------------------------------------------------------------
   Function		: startTimingFrame
   Purpose		: start timing the frame
   Parameters	: 
   Returns		: 
   Info			: 
*/
void startTimingFrame(void);


/* --------------------------------------------------------------------------------
   Function		: finishTimingFrame
   Purpose		: finish timing the frame
   Parameters	: 
   Returns		: 
   Info			: // PP: calculates frames per second, frame length, etc.
*/
void finishTimingFrame(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TIME EFFECTS

											Functions to fiddle about with time.

		NOW, BE REAL CAREFUL WHEN USING THESE - I DON'T EXPECT OUR PORTUGESE COLLISION SYSTEM TO STAND UP TO MUCH TORTURE.
									" WHAT DO YOU THINK I AM - SOME KIND OF 'SUPER-MAN'? "
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


extern float		TimeFX_jumpTime;		// PP: length of time to be skipped next frame - you can either set this directly or call TimeFX_jump
extern float		TimeFX_globalRate;		// PP: the 'normal' game speed to switch back to when queued rates end
extern bool			TimeFX_noQueue;			// PP: there are no queued time rates

// PP: time modes
enum ETimeMode
{
	TIMEMODE_RAW,							// PP: gameSpeed affects real-time length of time period
	TIMEMODE_REAL							// PP: gameSpeed compensated for, length of time period is a real-time length
};

// PP: the default time mode to be used by the TimeFX_stuff - set TimeFX_timeMode to change the way lengths of time are interpreted
#define TIMEFX_DEFAULT_TIME_MODE		TIMEMODE_RAW

// PP: the time mode used by the TimeFX_stuff - this determines the way lengths of time are interpreted.  You can set this directly.
extern ETimeMode	TimeFX_timeMode;


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_jump
	Purpose 	: instantly jump forward in time
	Parameters 	: length in seconds of jump
	Returns 	: 
	Info 		: 
*/
void TimeFX_jump(const float jump);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueJump
	Purpose 	: queue an instant forward jump in time
	Parameters 	: length in seconds of jump
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueJump(const float jump);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_setGlobalRate
	Purpose 	: set the 'normal' time rate
	Parameters 	: the 'normal; time rate
	Returns 	: 
	Info 		: NOTE THAT THIS CANCELS ALL QUEUED RATES AND SETS THE GLOBAL RATE AS CURRENT RATE
*/
void TimeFX_setGlobalRate(const float rate);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueGlobalRate
	Purpose 	: set the 'normal' time rate (to take effect when the queue empties)
	Parameters 	: the 'normal; time rate
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueGlobalRate(const float rate);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueRate
	Purpose 	: Queue a change in rate of time to last for a specified length of...time
	Parameters 	: new rate of time, length of real time to hold new rate of time for, (opt/0.0f=none) level of motion blur for the rate
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueRate(const float rate, const float length, const float motionBlur=0.0f);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_queueTimeslice
	Purpose 	: Queue a timeslice effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_queueTimeslice(void);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_update
	Purpose 	: update & apply time effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: TimeFX_cancelRates
	Purpose 	: cancel all queued time rates; apply global time rate
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TimeFX_cancelRates(void);


#endif// PP: ndef __H_TIMER