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
        bkPrintf("*** WARNING *** baActorGroupCreate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupDelete was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupAddInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupDeleteInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupSort was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupUpdate was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupPrep was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** baActorGroupDraw was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
