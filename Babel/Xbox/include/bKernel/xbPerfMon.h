// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : xbPerfMon.h
//   Purpose : performance monitoring functions
// Component : Xbox Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_PERFORMANCE_MONITOR_H__
#define __BKERNEL_PERFORMANCE_MONITOR_H__


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/*	--------------------------------------------------------------------------------
	Function : bkPerfMonInit
	Purpose : Enable performance monitoring
	Parameters : number of events
	Returns : 
	Info : 
*/

#define bkPerfMonInit		BNULLFUNCTION_RET


/*	--------------------------------------------------------------------------------
	Function : bkPerfMonShutdown
	Purpose : Close down performance monitoring
	Parameters : 
	Returns : 
	Info : 
*/

#define bkPerfMonShutdown	BNULLFUNCTION


/*	--------------------------------------------------------------------------------
	Function : bkPerfMonEnable
	Purpose : Toggle active state of performance monitoring
	Parameters : 
	Returns : 
	Info : Init must be called first - then this can be used to enable/disable monitoring
*/

#define bkPerfMonEnable		BNULLFUNCTION


/*	--------------------------------------------------------------------------------
	Function : bkPerfMonPrepare
	Purpose : Starting performance monitoring
	Parameters : 
	Returns : 
	Info : INTERNAL
*/

#define bPerfMonPrepare		BNULLFUNCTION


/*	--------------------------------------------------------------------------------
	Function : bPerfMonStart
	Purpose : Start of event to be monitored
	Parameters : event number
	Returns : 
	Info : 
*/

#define bkPerfMonStart		BNULLFUNCTION_RET


/*	--------------------------------------------------------------------------------
	Function : bkPerfMonEnd
	Purpose : End of event to be monitored
	Parameters : event number
	Returns : 
	Info : 
*/

#define bkPerfMonEnd		BNULLFUNCTION_RET


/*	--------------------------------------------------------------------------------
	Function : bPerfMonFinish
	Purpose : End performance monitor frame
	Parameters : 
	Returns : 
	Info : INTERNAL
*/

#define bPerfMonFinish		BNULLFUNCTION


/*	--------------------------------------------------------------------------------
	Function : bPerfMonDraw
	Purpose : Drawing of performance graphs
	Parameters : 
	Returns : 
	Info : INTERNAL
*/

#define bPerfMonDraw		BNULLFUNCTION


#endif // __BKERNEL_PERFORMANCE_MONITOR_H__


