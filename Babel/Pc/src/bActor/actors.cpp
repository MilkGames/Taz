// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : actors.cpp
//   Purpose : Defines structures and functions for managing groups of actor instances
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : baActorGroupCreate
	Purpose : Create a actor group object
	Parameters : flags
	Returns : group
	Info : 
*/
TBActorGroup *baActorGroupCreate(int flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupDelete
	Purpose : Delete a actor group object
	Parameters : group
	Returns : 
	Info : 
*/
void baActorGroupDelete(TBActorGroup *group)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupAddInstance
	Purpose : Add an actor instance to a group
	Parameters : group, actor instance to add
	Returns : 
	Info : 
*/
void baActorGroupAddInstance(TBActorGroup *group, TBActorInstance *instance)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupDeleteInstance
	Purpose : Remove an actor instance from a group
	Parameters : group, actor instance to remove
	Returns : OK/FAIL
	Info : 
*/
int baActorGroupDeleteInstance(TBActorGroup *group, TBActorInstance *instance)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupSort
	Purpose : Order a group of actor instance animations for faster processing
	Parameters : actor group
	Returns :
	Info : 
*/
void baActorGroupSort(TBActorGroup *group)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupUpdate
	Purpose : Update a group of actor instances
	Parameters : group, time delta, motion vector
	Returns : OK/FAIL
	Info : 
*/
int baActorGroupUpdate(TBActorGroup *group, float timeDelta, TBVector motionVector)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupPrep
	Purpose : Prepare a group of actor instances
	Parameters : group, flags
	Returns : 
	Info : 
*/
void baActorGroupPrep(TBActorGroup *group, int flags)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baActorGroupDraw
	Purpose : Draw a group of actor instances
	Parameters : group, alpha, flags
	Returns : OK/FAIL
	Info : 
*/
int baActorGroupDraw(TBActorGroup *group, int alpha, int flags)
{
    return 0;
}
