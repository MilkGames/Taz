// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbActor.cpp
//   Purpose : platform specific primary actor functions
// Component : Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bFixupAsyncMesh
	Purpose : fixup async mesh
	Parameters : asyncMesh, basePtr
	Returns : 
	Info : 
*/
void bFixupAsyncMesh(TBAsyncMesh *asyncMesh, uchar *basePtr)
{
        bkPrintf("*** WARNING *** bFixupAsyncMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bDrawMesh
	Purpose : draw mesh
	Parameters : mesh, actorInstance, alpha, nodeInstance
	Returns : 
	Info : 
*/
void bDrawMesh(TBMesh *mesh, struct _TBActorInstance *actorInstance, int alpha, struct _TBActorNodeInstance *nodeInstance)
{
        bkPrintf("*** WARNING *** bDrawMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bInitActorPlatformSpecific
	Purpose : initialise actor platform specific
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitActorPlatformSpecific()
{
    return OK; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownActorPlatformSpecific
	Purpose : shutdown actor platform specific
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownActorPlatformSpecific()
{
    return; // empty, confirmed
}

/*	--------------------------------------------------------------------------------
	Function : bSetupActorNodeList
	Purpose : setup actor node list
	Parameters : rootNode, actorInstance, alpha
	Returns : 
	Info : 
*/
void bSetupActorNodeList(struct _TBActorNodeInstance *rootNode, struct _TBActorInstance *actorInstance, int alpha)
{
        bkPrintf("*** WARNING *** bSetupActorNodeList was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bFixupSoftSkin
	Purpose : fixup soft skin
	Parameters : actor
	Returns : 
	Info : 
*/
void bFixupSoftSkin(struct _TBActor *actor)
{
        bkPrintf("*** WARNING *** bFixupSoftSkin was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bMatMultiplyTranspose
	Purpose : matrix multiply transpose
	Parameters : dest, src1, src2, rows
	Returns : 
	Info : 
*/
void bMatMultiplyTranspose(float *dest, const TBMatrix src1, const TBMatrix src2, int rows)
{
        bkPrintf("*** WARNING *** bMatMultiplyTranspose was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bSetMatrixPaletteEntry
	Purpose : set matrix palette entry
	Parameters : actorInstance, nodeInstance
	Returns : 
	Info : 
*/
void bSetMatrixPaletteEntry(struct _TBActorInstance *actorInstance, struct _TBActorNodeInstance *nodeInstance)
{
        bkPrintf("*** WARNING *** bSetMatrixPaletteEntry was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bFixupMesh
	Purpose : fixup mesh
	Parameters : basePtr, node, actor
	Returns : 
	Info : 
*/
void bFixupMesh(uchar *basePtr, struct _TBActorNode *node, struct _TBActor *actor)
{
        bkPrintf("*** WARNING *** bFixupMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bPrepActorNodeList
	Purpose : prep actor node list
	Parameters : rootNode, parentXForm, parentScale, actorInstance
	Returns : 
	Info : 
*/
void bPrepActorNodeList(struct _TBActorNodeInstance *rootNode, TBMatrix parentXForm, TBVector parentScale, struct _TBActorInstance *actorInstance)
{
        bkPrintf("*** WARNING *** bPrepActorNodeList was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteMesh
	Purpose : delete mesh
	Parameters : meshNode
	Returns : 
	Info : 
*/
void bDeleteMesh(struct _TBActorNode *meshNode)
{
        bkPrintf("*** WARNING *** bDeleteMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteAsyncMesh
	Purpose : delete async mesh
	Parameters : meshNode
	Returns : 
	Info : 
*/
void bDeleteAsyncMesh(struct _TBActorNode *meshNode)
{
        bkPrintf("*** WARNING *** bDeleteAsyncMesh was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteSoftSkin
	Purpose : delete soft skin
	Parameters : actor
	Returns : 
	Info : 
*/
void bDeleteSoftSkin(struct _TBActor *actor)
{
        bkPrintf("*** WARNING *** bDeleteSoftSkin was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorRenderCallbacks
	Purpose : set actor render callbacks
	Parameters : meshDraw, softSkinDraw
	Returns : OK/FAIL
	Info : 
*/
int baSetActorRenderCallbacks(TBDrawMeshCallback meshDraw, TBDrawSoftSkinCallback softSkinDraw)
{
        bkPrintf("*** WARNING *** baSetActorRenderCallbacks was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
