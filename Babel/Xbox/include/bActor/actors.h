// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : actors.h
//   Purpose : generic grouped actor functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_ACTORS_H__
#define __BACTOR_ACTORS_H__

// ********************************************************************************
// Constants and Macros

#define TBACTORGROUPFLAG_SORTED		0x00000001		// List is sorted

// an actor instance - information that is local to each occurance of an actor
typedef struct _TBActorGroupMember {
	TBActorInstance				*instance;
	struct _TBActorGroupMember	*prev;
	struct _TBActorGroupMember	*next;
} TBActorGroupMember;

// an actor instance - information that is local to each occurance of an actor
typedef struct _TBActorGroup {
	TBActorGroupMember	*head;
	int					flags;
} TBActorGroup;


// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes


/*	--------------------------------------------------------------------------------
	Function : baActorGroupCreate
	Purpose : Create a actor group object
	Parameters : group flags
	Returns : group object
	Info : 
*/

TBActorGroup *baActorGroupCreate(int flags);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupDelete
	Purpose : Delete a actor group object
	Parameters : group
	Returns : 
	Info : 
*/

void baActorGroupDelete(TBActorGroup *group);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupAddInstance
	Purpose : Add an actor instance to a group
	Parameters : group, actor instance to add
	Returns : 
	Info : 
*/

void baActorGroupAddInstance(TBActorGroup *group, TBActorInstance *instance);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupDeleteInstance
	Purpose : Remove an actor instance from a group
	Parameters : group, actor instance to remove
	Returns : OK/FAIL
	Info : 
*/

int baActorGroupDeleteInstance(TBActorGroup *group, TBActorInstance *instance);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupSort
	Purpose : Order a group of actor instance animations for faster processing
	Parameters : actor group
	Returns :
	Info : 
*/

void baActorGroupSort(TBActorGroup *group);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupUpdate
	Purpose : Update a group of actor instances animations
	Parameters : actor group, time delta, motion vector
	Returns : total number of nodes updated
	Info : 
*/

int baActorGroupUpdate(TBActorGroup *group, float timeDelta, TBVector motionVector);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupPrep
	Purpose : prepare a group of actor instances to reflect there current states
	Parameters : actor group, actor prep flags 
	Returns : 
	Info : 
*/

void baActorGroupPrep(TBActorGroup *group, int flags);


/*	--------------------------------------------------------------------------------
	Function : baActorGroupDraw
	Purpose : draw a group of actor instances using there current states
	Parameters : actor group, alpha type bitmask, draw flags
	Returns : number of actors draw (not clipped by bounding box tests)
	Info : 
*/

int baActorGroupDraw(TBActorGroup *group, int alpha = BACTPRIMALPHATYPE_ALL, int flags=BACTORDRAWFLAG_EARLYOUT);


#endif		// __BACTOR_ACTORS_H__
