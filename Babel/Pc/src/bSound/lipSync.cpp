// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : lipSync.h
//   Purpose : Generic lip sync routines
// Component : Generic Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bLoadLipSyncByCRC
   Purpose : load lip sync data
   Parameters : package file, lip sync crc
   Returns : handle to lip sync data, 0 if fail
   Info : 
*/

TBLipSync *bLoadLipSyncByCRC(TBPackageIndex *index, uint32 crc)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bDeleteLipSync
   Purpose : delete lip sync data
   Parameters : lip sync handle
   Returns :
   Info : 
*/

void bDeleteLipSync(TBLipSync *stream)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsSetupLipSyncController
   Purpose : prepare lip sync controller information
   Parameters : controller structure (pre alloced), flags, rotation order, 
				angle multipliers x,y,z, position multipliers x,y,z, data thresholds x,y,z (0-255)
   Returns :
   Info : 
*/

void bsSetupLipSyncController(TBLipSyncController *controller, int flags, EBLipSyncRotationOrder order,
							 float xaMult, float yaMult, float zaMult,
							 float xMult, float yMult, float zMult,
							 float xaThresh, float yaThresh, float zaThresh)
{
    return;
}

							 
/* --------------------------------------------------------------------------------
   Function : bsAttachLipSyncToNode
   Purpose : attach lip sync to specified node
   Parameters : lip sync handle, node to attach to, controller data
   Returns :
   Info : 
*/

void bsAttachLipSyncToNode(TBLipSync *lipSync, TBActorNodeInstance *node, TBLipSyncController *controller)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsRemoveLipSyncFromNode
   Purpose : remove lip sync controller from specified node
   Parameters : node to attach to, controller data
   Returns : OK/FAIL
   Info : 
*/

int bsRemoveLipSyncFromNode(TBActorNodeInstance *node, TBLipSyncController *controller)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPauseLipSync
   Purpose : pause lip syncing controller
   Parameters : controller data
   Returns : OK/FAIL
   Info : 
*/

int bsPauseLipSync(TBLipSyncController *controller, int pauseStatus)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsGetLipSyncPosition
   Purpose : get lip sync play cursor location
   Parameters : lip sync handle
   Returns : offset into data (-1 if ended)
   Info : 
*/

int bsGetLipSyncPosition(TBLipSyncController *controller)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bLipSyncNodeController
   Purpose : attach lip sync to specified node
   Parameters : lip sync handle, node to attach to 
   Returns : TRUE if running, FALSE on termination
   Info : INTERNAL
*/

int bLipSyncNodeController(TBActorNodeInstance *node, void *context)
{
    return 0;
}