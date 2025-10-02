// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : lipSync.h
//   Purpose : Generic lip sync routines
// Component : Generic Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_LIPSYNC_H__
#define __BSOUND_LIPSYNC_H__

// ********************************************************************************
// Constants and macros

// pause flags
#define BLPAUSE						1							// pause playback
#define BLUNPAUSE					0							// unpause playback

// lip sync flags
#define BLIPSYNC_ATTACHED			0x00000001					// attached to a node


// ********************************************************************************
// Types, Structures and Classes

// LipSync header
typedef struct _TBLipSync {
	TBResourceInfo			resInfo;						// resource info header

	uint32					noofSamples;					// total #samples in lip sync track
	uint32					flags;							// current flags
	uint32					pad[2];
	// Lip sync Data follows header
} TBLipSync;

// orders of rotation for lip sync angles
typedef enum EBLipSyncRotationOrder {
	BLSROTATE_XYZ,
	BLSROTATE_XZY,
	BLSROTATE_YXZ,
	BLSROTATE_YZX,
	BLSROTATE_ZXY,
	BLSROTATE_ZYX,
};

// Controller information for lip sync node controller
typedef struct _TBLipSyncController {
	TBLipSync				*lipSync;						// lipSync header
	uint					flags;							// controller flags
	EBLipSyncRotationOrder	order;							// order of rotations
	uint					attached;						// number of nodes attached to

	TBVector				angularMultiplier;				// multiplier (influence) on bone angles
	TBVector				positionMultiplier;				// multiplier (influence) on bone position
	TBVector				threshold;						// threshold before triggering bone 0-255

	TBNodeController		userController;					// user controller
	UBContext32				userContext;					// user context
	uint					pad2[2];

	TBTimerValue			startTime;						// lipSync playback start time
	TBTimerValue			pauseTime;						// total pause duration
	TBTimerValue			pausing;						// this pause duration
} TBLipSyncController;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bLoadLipSyncByCRC
   Purpose : load lip sync data
   Parameters : package file, lip sync crc
   Returns : handle to lip sync data, 0 if fail
   Info : 
*/

TBLipSync *bLoadLipSyncByCRC(TBPackageIndex *index, uint32 crc);


/* --------------------------------------------------------------------------------
   Function : bDeleteLipSync
   Purpose : delete lip sync data
   Parameters : lip sync handle
   Returns :
   Info : 
*/

void bDeleteLipSync(TBLipSync *stream);


/* --------------------------------------------------------------------------------
   Function : bsSetupLipSyncController
   Purpose : prepare lip sync controller information
   Parameters : controller structure (pre alloced), flags, rotation order, 
				angle multipliers x,y,z, position multipliers x,y,z, data thresholds x,y,z (0-255)
   Returns :
   Info : 
*/

void bsSetupLipSyncController(TBLipSyncController *controller, int flags, EBLipSyncRotationOrder order,
							 float xaMult=1.0f, float yaMult=0.0f, float zaMult=0.0f,
							 float xMult=0.0f, float yMult=0.0f, float zMult=0.0f,
							 float xaThresh=0.0f, float yaThresh=0.0f, float zaThresh=0.0f);

							 
/* --------------------------------------------------------------------------------
   Function : bsAttachLipSyncToNode
   Purpose : attach lip sync to specified node
   Parameters : lip sync handle, node to attach to, controller data
   Returns :
   Info : 
*/

void bsAttachLipSyncToNode(TBLipSync *lipSync, TBActorNodeInstance *node, TBLipSyncController *controller);


/* --------------------------------------------------------------------------------
   Function : bsRemoveLipSyncFromNode
   Purpose : remove lip sync controller from specified node
   Parameters : node to attach to, controller data
   Returns : OK/FAIL
   Info : 
*/

int bsRemoveLipSyncFromNode(TBActorNodeInstance *node, TBLipSyncController *controller);


/* --------------------------------------------------------------------------------
   Function : bsPauseLipSync
   Purpose : pause lip syncing controller
   Parameters : controller data
   Returns : OK/FAIL
   Info : 
*/

int bsPauseLipSync(TBLipSyncController *controller, int pauseStatus);


/* --------------------------------------------------------------------------------
   Function : bsGetLipSyncPosition
   Purpose : get lip sync play cursor location
   Parameters : lip sync handle
   Returns : offset into data (-1 if ended)
   Info : 
*/

int bsGetLipSyncPosition(TBLipSyncController *controller);


/* --------------------------------------------------------------------------------
   Function : bLipSyncNodeController
   Purpose : attach lip sync to specified node
   Parameters : lip sync handle, node to attach to 
   Returns : TRUE if running, FALSE on termination
   Info : INTERNAL
*/

int bLipSyncNodeController(TBActorNodeInstance *node, void *context);


#endif //__BSOUND_LIPSYNC_H__
