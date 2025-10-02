// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : extras.cpp
//   Purpose : Defines constants, function pointer types, and the TBExtraSystem struct for managing extra effects systems
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitExtras
	Purpose : initialise the Extras sub-module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitExtras()
{
	return FAIL;
}


/*	--------------------------------------------------------------------------------
	Function : bShutdownExtras
	Purpose : shutdown the Extras sub-module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownExtras()
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baCreateExtraSystem
	Purpose : create a new extra system
	Parameters : ident, creation function, update function, delete function, render function, system init function, size of data block,
				 max simultaneous instances, flags, system data size
	Returns : ptr to system or NULL for failure
	Info : 
*/

TBExtraSystem *baCreateExtraSystem(char *ident, TBExtraCreateFunc create, TBExtraUpdateFunc update, TBExtraDeleteFunc del, 
								   TBExtraRenderFunc render, TBExtraSystemInitFunc sysInit, int32 dataSize, int32 maxInstances,
								   uint32 flags, int32 systemDataSize)
{
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraSystem
	Purpose : delete a new extra system
	Parameters : ptr to system to delete or NULL for all systems
	Returns : 
	Info : 
*/

void baDeleteExtraSystem(TBExtraSystem *eSystem)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baFindExtraSystem
	Purpose : find an extra system from it's ident
	Parameters : ident
	Returns : ptr to system or NULL for failure
	Info : 
*/

TBExtraSystem *baFindExtraSystem(char *ident)
{
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baCreateExtra
	Purpose : create a new extra instance
	Parameters : system ptr, parameters...
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCreateExtra(TBExtraSystem *eSystem, ...)
{
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : baCloneExtra
	Purpose : create a new extra instance from an existing parameter template
	Parameters : system ptr, parameter template
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCloneExtra(TBExtraSystem *eSystem, void *parmTemplate)
{
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function : baResetExtraSystem
	Purpose : delete all extras currently active within a system
	Parameters : ptr to system or NULL for all systems
	Returns : 
	Info : 
*/

void baResetExtraSystem(TBExtraSystem *eSystem)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baUpdateExtraSystem
	Purpose : update all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, 't' value to update by
	Returns : 
	Info : 
*/

void baUpdateExtraSystem(TBExtraSystem *eSystem, float t)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baRenderExtraSystem
	Purpose : render all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, context value to pass to render function
	Returns : 
	Info : 
*/

void baRenderExtraSystem(TBExtraSystem *eSystem, void *context)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraInstance
	Purpose : delete an extra instance from a system
	Parameters : ptr to system, index of extra (0..)
	Returns : 1=ok, 0=fail
	Info : 
*/

int baDeleteExtraInstance(TBExtraSystem *eSystem, int instanceIndex)
{
	return 0;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemLimit
	Purpose : set a new max limit for an extras system
	Parameters : system ptr, new limit
	Returns : new system ptr or NULL for failure
	Info : will cause the system to be reset
*/

TBExtraSystem *baSetExtraSystemLimit(TBExtraSystem *eSystem, int newMaxInstances)
{
	TBExtraSystem *example;
	return example;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemPosition
	Purpose : set the position of an extra system
	Parameters : ptr to system, position
	Returns : 
	Info : 
*/

void baSetExtraSystemPosition(TBExtraSystem *eSystem, float x, float y, float z)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemOrientation
	Purpose : set the orientation of an extra system
	Parameters : ptr to system, orientation
	Returns : 
	Info : 
*/

void baSetExtraSystemOrientation(TBExtraSystem *eSystem, TBQuaternion orientation)
{
	return;
}


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemAttachment
	Purpose : set the attachment for an extra system
	Parameters : ptr to system, actor instance, actor node instance
	Returns : 
	Info : 
*/

void baSetExtraSystemAttachment(TBExtraSystem *eSystem, TBActorInstance *actInstance, TBActorNodeInstance *nodeInstance)
{
	return;
}
