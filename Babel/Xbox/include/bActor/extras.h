// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : extras.h
//   Purpose : generic Extra functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_EXTRAS_H__
#define __BACTOR_EXTRAS_H__

// ********************************************************************************
// Constants and Macros

// extra system flags
#define BEXSYSFLAG_NEWPRIORITY		0x00000001		// new instances take priority over existing ones
#define BEXSYSFLAG_GLOBALEXTRAS		0x00000002		// extras exist in global world space rather than system space
#define BEXSYSFLAG_XFORMDIRTY		0x00000004		// the xform for this system is dirty and needs recalculating


// init func reason codes
enum {
	BESYSINIT_INIT,
	BESYSINIT_SHUTDOWN
};


// ********************************************************************************
// Types, Structures and Classes

// extra creation function
typedef void (*TBExtraCreateFunc)(struct _TBExtraSystem *eSystem, uchar *data, va_list parms);


// extra update function
typedef void (*TBExtraUpdateFunc)(struct _TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);


// extra delete function
typedef int (*TBExtraDeleteFunc)(struct _TBExtraSystem *eSystem, uchar *data);


// extra render function
typedef void (*TBExtraRenderFunc)(struct _TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);


// system creation function
typedef void (*TBExtraSystemInitFunc)(struct _TBExtraSystem *eSystem, int reason);


// an Extra system
typedef struct _TBExtraSystem {
	uint32						crc;						// identifier CRC
	uint32						flags;						// flags (see BEXSYSFLAG_)
	struct _TBExtraSystem		*next;						// next system in list
	struct _TBExtraSystem		*prev;						// previous system in list

	int32						maxInstances;				// maximum number of instances
	int32						dataSize;					// size of an instance data structure
	int32						noofInstances;				// number of instances currently active
	int32						nextInstance;				// index of next instance to be allocated

	TBExtraCreateFunc			createFunc;					// extra creation function
	TBExtraUpdateFunc			updateFunc;					// extra update function
	TBExtraDeleteFunc			deleteFunc;					// extra deletion function
	TBExtraRenderFunc			renderFunc;					// extra render function

	TBExtraSystemInitFunc		sysInitFunc;				// system init function
	int32						*activeFlags;				// active flag array
	uchar						**dataPtrs;					// data ptr array for rendering
	uchar						*data;						// data block array

	uchar						*sysData;					// ptr to system data
	int32						sysDataSize;				// size of system data
	int32						highestNoofInstances;		// highest number of instances that ever existed simultaneously
	int32						pad1;

	TBVector					position;					// position of system in world space or relative to actor/node
	TBQuaternion				orientation;				// orientation of system in world space or relative to actor/node
	TBMatrix					systemToWorld;				// system->world transform matrix

	TBActorInstance				*actorAttachedTo;			// ptr to actor instance we are attached to or NULL for none
	TBActorNodeInstance			*nodeAttachedTo;			// ptr to actor instance node we are attached to or NULL for none
	int32						pad2;
	int32						pad3;
} TBExtraSystem;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitExtras
	Purpose : initialise the Extras sub-module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitExtras();


/*	--------------------------------------------------------------------------------
	Function : bShutdownExtras
	Purpose : shutdown the Extras sub-module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownExtras();


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
								   uint32 flags, int32 systemDataSize);


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraSystem
	Purpose : delete a new extra system
	Parameters : ptr to system to delete or NULL for all systems
	Returns : 
	Info : 
*/

void baDeleteExtraSystem(TBExtraSystem *eSystem);


/*	--------------------------------------------------------------------------------
	Function : baFindExtraSystem
	Purpose : find an extra system from it's ident
	Parameters : ident
	Returns : ptr to system or NULL for failure
	Info : 
*/

TBExtraSystem *baFindExtraSystem(char *ident);


/*	--------------------------------------------------------------------------------
	Function : baCreateExtra
	Purpose : create a new extra instance
	Parameters : system ptr, parameters...
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCreateExtra(TBExtraSystem *eSystem, ...);


/*	--------------------------------------------------------------------------------
	Function : baCloneExtra
	Purpose : create a new extra instance from an existing parameter template
	Parameters : system ptr, parameter template
	Returns : ptr to parms for success or NULL for failure
	Info : 
*/

uchar *baCloneExtra(TBExtraSystem *eSystem, void *parmTemplate);


/*	--------------------------------------------------------------------------------
	Function : baResetExtraSystem
	Purpose : delete all extras currently active within a system
	Parameters : ptr to system or NULL for all systems
	Returns : 
	Info : 
*/

void baResetExtraSystem(TBExtraSystem *eSystem);


/*	--------------------------------------------------------------------------------
	Function : baUpdateExtraSystem
	Purpose : update all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, 't' value to update by
	Returns : 
	Info : 
*/

void baUpdateExtraSystem(TBExtraSystem *eSystem, float t);


/*	--------------------------------------------------------------------------------
	Function : baRenderExtraSystem
	Purpose : render all extras in a system or systems
	Parameters : ptr to system or NULL for all systems, context value to pass to render function
	Returns : 
	Info : 
*/

void baRenderExtraSystem(TBExtraSystem *eSystem, void *context=NULL);


/*	--------------------------------------------------------------------------------
	Function : baDeleteExtraInstance
	Purpose : delete an extra instance from a system
	Parameters : ptr to system, index of extra (0..)
	Returns : 1=ok, 0=fail
	Info : 
*/

int baDeleteExtraInstance(TBExtraSystem *eSystem, int instanceIndex);


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemLimit
	Purpose : set a new max limit for an extras system
	Parameters : system ptr, new limit
	Returns : new system ptr or NULL for failure
	Info : will cause the system to be reset
*/

TBExtraSystem *baSetExtraSystemLimit(TBExtraSystem *eSystem, int newMaxInstances);


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemPosition
	Purpose : set the position of an extra system
	Parameters : ptr to system, position
	Returns : 
	Info : 
*/

void baSetExtraSystemPosition(TBExtraSystem *eSystem, float x, float y, float z);


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemOrientation
	Purpose : set the orientation of an extra system
	Parameters : ptr to system, orientation
	Returns : 
	Info : 
*/

void baSetExtraSystemOrientation(TBExtraSystem *eSystem, TBQuaternion orientation);


/*	--------------------------------------------------------------------------------
	Function : baSetExtraSystemAttachment
	Purpose : set the attachment for an extra system
	Parameters : ptr to system, actor instance, actor node instance
	Returns : 
	Info : 
*/

void baSetExtraSystemAttachment(TBExtraSystem *eSystem, TBActorInstance *actInstance, TBActorNodeInstance *nodeInstance);

#endif		// __BACTOR_EXTRAS_H__
