// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : profile.h
//   Purpose : profile functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_PROFILE_H__
#define __BKERNEL_PROFILE_H__


// --------------------------------------------------------------------------------
// NOTE:  #define the symbol BPROFILE to enable profiling support
// --------------------------------------------------------------------------------


// ********************************************************************************
// Constants and macros

// bkProfileDump sort modes
typedef enum EBProfileSort {
	EBPROFILESORT_FUNCCHILD,		// sort by function+child time
	EBPROFILESORT_FUNCONLY,			// sort by function only time
	EBPROFILESORT_CALLCOUNT,		// sort by function call count
	EBPROFILESORT_FUNCNAME,			// sort by function name
};


// ********************************************************************************
// Types, structures and classes

#ifdef BPROFILE

// profile function record
typedef struct _TBProfileFuncRecord {
	char						*funcName;			// function name
	uint32						funcId;				// function CRC
	TBTimerValue				totalTime;			// total time taken in this function
	TBTimerValue				localTime;			// time spent in just this function, not it's children
	struct _TBProfileFuncRecord	*next;				// next record in list
	int32						callCount;			// how many times this function has been called
	float						localPerc;			// local time percentage
} TBProfileFuncRecord;


// profile function instance class
typedef class CBProfileFunc {
	private:
		TBTimerValue			outerStartTime;
		TBTimerValue			innerStartTime;

	public:
								CBProfileFunc(uint32 funcCrc, char *funcName);
								~CBProfileFunc();
} CBProfileFunc;


#endif


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bkProfileGetTime
	Purpose : read current profile timer value
	Parameters : 
	Returns : timer value
	Info : 
*/

#ifdef BPROFILE
	TBTimerValue bkProfileGetTime();
#else
	#define bkProfileGetTime()	0
#endif


/*	--------------------------------------------------------------------------------
	Function : bkProfileEnterFunction
	Purpose : enter the timed section for a function
	Parameters : function ID, function name, ptr to timer var
	Returns : 
	Info : 
*/

#ifdef BPROFILE
	void bkProfileEnterFunction(uint32 funcId, char *funcName, TBTimerValue *timerPtr, TBTimerValue *outerTimerPtr);
#else
	#define bkProfileEnterFunction(A,B,C,D)
#endif


/*	--------------------------------------------------------------------------------
	Function : bkProfileExitFunction
	Purpose : exit the timed section for the last entered function
	Parameters : time taken in this function, entry time for this function
	Returns : 
	Info : 
*/

#ifdef BPROFILE
	void bkProfileExitFunction(TBTimerValue timeTaken, TBTimerValue entryTime);
#else
	#define bkProfileExitFunction(A, B)
#endif


/*	--------------------------------------------------------------------------------
	Function : bkProfileDump
	Purpose : dump the profile to the debug stream
	Parameters : sort mode
	Returns : 
	Info : 
*/

#ifdef BPROFILE
	void bkProfileDump(EBProfileSort sortMode);
#else
	#define bkProfileDump(A)
#endif


/*	--------------------------------------------------------------------------------
	Function : bkProfileResetTimers
	Purpose : reset the profile timers
	Parameters : 
	Returns : 
	Info : 
*/

#ifdef BPROFILE
	void bkProfileResetTimers();
#else
	#define bkProfileResetTimers()
#endif

#endif // __BKERNEL_PROFILE_H__
