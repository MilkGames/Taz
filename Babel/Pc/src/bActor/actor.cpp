// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : actor.cpp
//   Purpose : generic primary actor functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Globals

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitActor
	Purpose : initialise the actor module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitActor()
{
	if (!bInitActorPlatformSpecific()) {
		return FAIL;
	}

	if (!bInitLights()) {
		bShutdownActorPlatformSpecific();
		return FAIL;
	}

	if (!bInitSet()) {
		bShutdownLights();
		bShutdownActorPlatformSpecific();
		return FAIL;
	}

	if (!bInitCamera()) {
		bShutdownSet();
		bShutdownLights();
		bShutdownActorPlatformSpecific();
		return FAIL;
	}

	bInitExtras();
	return OK;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownActor
	Purpose : shutdown the actor module
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownActor()
{
	bShutdownExtras();
	bShutdownCamera();
	bShutdownSet();
	bShutdownLights();
	bShutdownActorPlatformSpecific();
}

/*	--------------------------------------------------------------------------------
	Function : baInitActorInstance
	Purpose : create and/or initialise an actor instance
	Parameters : ptr to actor instance or NULL for dynamic allocation, actor to assign, anim queue size
	Returns : ptr to actor instance or NULL for creation failure
	Info : 
*/
TBActorInstance *bInitActorInstance(TBActorInstance *actorInstance, TBActor *actor, int queueSize, char *file, int line)
{
        bkPrintf("*** WARNING *** bInitActorInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFreeActorInstance
	Purpose : delete an actor instance
	Parameters : ptr to actor instance
	Returns : 
	Info : will only free resources from static instances not the instance struct itself
*/
void baFreeActorInstance(TBActorInstance *actorInstance)
{
        bkPrintf("*** WARNING *** baFreeActorInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baPrepActorInstance
	Purpose : prepare an actor instance to reflect it's current state
	Parameters : ptr to actor instance, actor prep flags 
	Returns : 
	Info : 
*/
void baPrepActorInstance(TBActorInstance *actorInstance, int flags)
{
        bkPrintf("*** WARNING *** baPrepActorInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baDrawActorInstance
	Purpose : draw an actor instance
	Parameters : ptr to actor instance, alpha type, draw flags
	Returns : OK/FAIL
	Info : 
*/
int baDrawActorInstance(TBActorInstance *actorInstance, int alpha, int flags)
{
        bkPrintf("*** WARNING *** baDrawActorInstance was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bLoadActorByCRC
	Purpose : load an actor from storage
	Parameters : package index, crc
	Returns : ptr to actor instance or NULL for failure
	Info : 
*/
TBActor *bLoadActorByCRC(TBPackageIndex *index, uint32 crc)
{
	bkPrintf("*** WARNING *** bLoadActorByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bDeleteActor
	Purpose : delete an actor
	Parameters : ptr to actor
	Returns : 
	Info : 
*/
void bDeleteActor(TBActor *actor)
{
        bkPrintf("*** WARNING *** bDeleteActor was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeFrame
	Purpose : set a node frame
	Parameters : actor instance, node instance, frame, recurse
	Returns : 
	Info : 
*/
void baSetNodeFrame(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeFrame was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlend
	Purpose : set a node blend
	Parameters : actor instance, node instance, frame1, frame2, t, recurse
	Returns : 
	Info : 
*/
void baSetNodeBlend(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame1, uint32 frame2, float t, int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlend was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendTarget
	Purpose : set a node blend target
	Parameters : actor instance, node instance, frame, recurse
	Returns : 
	Info : 
*/
void baSetNodeBlendTarget(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlendTarget was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendValue
	Purpose : set a node blend value
	Parameters : actor instance, node instance, t, recurse
	Returns : 
	Info : 
*/
void baSetNodeBlendValue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, float t, int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlendValue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendTargetValue
	Purpose : set a node blend target value
	Parameters : actor instance, node instance, frame, t, recurse
	Returns : 
	Info : 
*/
void baSetNodeBlendTargetValue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, float t, int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlendTargetValue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baUpdateNodes
	Purpose : update nodes
	Parameters : actor instance, node instance, time delta, motion vector
	Returns : OK/FAIL
	Info : 
*/
int baUpdateNodes(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, float timeDelta, TBVector motionVector)
{
        bkPrintf("*** WARNING *** baUpdateNodes was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baFlushNodeQueue
	Purpose : flush node queue
	Parameters : actor instance, node instance, flush children, flush from
	Returns : 
	Info : 
*/
void baFlushNodeQueue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int flushChildren, int flushFrom)
{
        bkPrintf("*** WARNING *** baFlushNodeQueue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baRetargetActorAnimation
	Purpose : retarget actor animation
	Parameters : source actor, destination actor instance
	Returns : 
	Info : 
*/
void baRetargetActorAnimation(TBActor *source, TBActorInstance *dest)
{
        bkPrintf("*** WARNING *** baRetargetActorAnimation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baHideNode
	Purpose : hide a node
	Parameters : node instance, recurse
	Returns : 
	Info : 
*/
void baHideNode(TBActorNodeInstance *nodeInstance, int recurse)
{
        bkPrintf("*** WARNING *** baHideNode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baShowNode
	Purpose : show a node
	Parameters : node instance, recurse
	Returns : 
	Info : 
*/
void baShowNode(TBActorNodeInstance *nodeInstance, int recurse)
{
        bkPrintf("*** WARNING *** baShowNode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorRenderMode
	Purpose : set actor render mode
	Parameters : actor instance, stage, mode, parms
	Returns : 
	Info : 
*/
void baSetActorRenderMode(TBActorInstance *actorInstance, int stage, EBActRenderMode mode, void *parms)
{
        bkPrintf("*** WARNING *** baSetActorRenderMode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baGetNodesWorldPosition
	Purpose : get nodes world position
	Parameters : actor instance, node instance, position, offset
	Returns : 
	Info : 
*/
void baGetNodesWorldPosition(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos, TBVector offset)
{
        bkPrintf("*** WARNING *** baGetNodesWorldPosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baGetPositionInNodeSpace
	Purpose : get position in node space
	Parameters : actor instance, node instance, position
	Returns : 
	Info : 
*/
void baGetPositionInNodeSpace(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos)
{
        bkPrintf("*** WARNING *** baGetPositionInNodeSpace was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendMulti
	Purpose : set blended animation for a node and it's children
	Parameters : actor-instance, actor-node instance, [multiple counts of: frame, blend factor], recurse on children?
	Returns : 
	Info : 
*/
void baSetNodeBlendMulti(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
					int frame1,float t1,
					int frame2,float t2,
					int frame3,float t3,
					int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlendMulti was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

void baSetNodeBlendMulti(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
					int frame1,float t1,
					int frame2,float t2,
					int frame3,float t3,
					int frame4,float t4,
					int recurse)
{
        bkPrintf("*** WARNING *** baSetNodeBlendMulti was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnim
	Purpose : queue an animation on a node and its children
	Parameters : actor instance, actor-node instance, animation type, anim seg 1, anim seg 2, anim1 delta, anim2 delta, start frame 1,
				 start frame 2, deltaT, flags, callback, callback context
	Returns : 
	Info : start frames of -1 give start of segment
*/
void baQueueNodeAnim(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uchar animType, TBActorAnimSegment *anim1Seg,
					 TBActorAnimSegment *anim2Seg, int32 anim1Delta, int32 anim2Delta, int32 startFrame1, int32 startFrame2,
					 float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount)
{
        bkPrintf("*** WARNING *** baQueueNodeAnim was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimNormal
	Purpose : queue a normal animation on a node and its children
	Parameters : actor instance, actor-node instance, anim seg num, anim delta, start frame, flags, callback, callback context
	Returns : 
	Info : start frame of -1 give start of segment
*/
void baQueueNodeAnimNormal(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *animSeg, 
						   int32 animDelta, int32 startFrame, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount)
{
        bkPrintf("*** WARNING *** baQueueNodeAnimNormal was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlend
	Purpose : queue a blended animation on a node and its children
	Parameters : actor instance, actor-node instance, anim1 seg, anim2 seg, anim1 delta, anim2 delta, start frame 1, start frame 2,
				 deltaT, flags, callback, callback context
	Returns : 
	Info : start frames of -1 give start of segment
*/
void baQueueNodeAnimBlend(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *anim1Seg,
						  TBActorAnimSegment *anim2Seg, int32 anim1Delta, int32 anim2Delta, int32 startFrame1, int32 startFrame2,
						  float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount)
{
        bkPrintf("*** WARNING *** baQueueNodeAnimBlend was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlendNormalised
	Purpose : queue a normalised blend between two animations
	Parameters : actor instance, actor-node instance, anim1 seg, anim2 seg, anim delta, start frame 1, start frame 2, flags,
				 callback, callback context
	Returns : 
	Info : blends between two animations at the speed of the shorter one, start frames of -1 give start of segments
*/
void baQueueNodeAnimBlendNormalised(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *anim1Seg,
									TBActorAnimSegment *anim2Seg, int32 animDelta, int32 startFrame1, int32 startFrame2, uint32 flags,
									TBQueuedAnimCallback callback, void *context, int32 repeatCount)
{
        bkPrintf("*** WARNING *** baQueueNodeAnimBlendNormalised was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlendState
	Purpose : queue a blend from state animation on a node and its children
	Parameters : actor instance, actor-node instance, anim seg, start frame, anim delta, deltaT, flags, callback, callback context
	Returns : 
	Info : start frame of -1 gives start of segment
*/
void baQueueNodeAnimBlendState(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *animSeg,
							   int32 animDelta, int32 startFrame, float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount)
{
        bkPrintf("*** WARNING *** baQueueNodeAnimBlendState was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAnimSpeed
	Purpose : Change the animation deltas of the currently playing animation
	Parameters : node instance, recurse flag, anim 1 delta, anim 2 delta, t delta, t, flags (see BSETANIMSPEED_)
	Returns : 
	Info :
*/
void baSetAnimSpeed(TBActorNodeInstance *node, int recurse, int32 anim1Delta, int32 anim2Delta, float deltaT,
																		float t, uint32 flags)
{
        bkPrintf("*** WARNING *** baSetAnimSpeed was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baFindNodeByCRC
	Purpose : Find a node by its crc
	Parameters : node instance, crc
	Returns : node if found, or NULL if not found
	Info : 
*/
TBActorNodeInstance *baFindNodeByCRC(TBActorNodeInstance *nodeInstance, uint32 crc)
{
        bkPrintf("*** WARNING *** baFindNodeByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFindNode
	Purpose : Find a node by its name
	Parameters : node instance, name
	Returns : node if found, or NULL if not found
	Info : 
*/
TBActorNodeInstance *baFindNode(TBActorNodeInstance *nodeInstance, char *nodeName)
{
        bkPrintf("*** WARNING *** baFindNode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegmentByCRC
	Purpose : Find an animation segment by its crc
	Parameters : actor instance, crc
	Returns : animation segment, or NULL if not found
	Info : 
*/
TBActorAnimSegment *baFindAnimSegmentByCRC(TBActorInstance *actorInstance, uint32 crc)
{
        bkPrintf("*** WARNING *** baFindAnimSegmentByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegment
	Purpose : Find an animation segment
	Parameters : actor instance, segment name
	Returns : animation segment, or NULL if not found
	Info : 
*/
TBActorAnimSegment *baFindAnimSegment(TBActorInstance *actorInstance, char *segName)
{
        bkPrintf("*** WARNING *** baFindAnimSegment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegmentByCRC
	Purpose : Find an animation segment by its crc
	Parameters : actor, crc
	Returns : animation segment, or NULL if not found
	Info : 
*/
TBActorAnimSegment *baFindAnimSegmentByCRC(TBActor *actor, uint32 crc)
{
        bkPrintf("*** WARNING *** baFindAnimSegmentByCRC was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegment
	Purpose : Find an animation segment
	Parameters : actor, segment name
	Returns : animation segment, or NULL if not found
	Info : 
*/
TBActorAnimSegment *baFindAnimSegment(TBActor *actor, char *segName)
{
        bkPrintf("*** WARNING *** baFindAnimSegment was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baGetNodesLocalPosition
	Purpose : Get the position of a node relative to the actor instance
	Parameters : actor instance, node instance, returned position
	Returns : 
	Info : 
*/
void baGetNodesLocalPosition(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos)
{
        bkPrintf("*** WARNING *** baGetNodesLocalPosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baNodeLookAt
	Purpose : Orient a node to look at a point in world space
	Parameters : actor instance, node instance, focus point, X axis +ve/-ve constraints, X axis offset, 
				 Y axis +ve/-ve constraints, Y axis offset, normalised move factor, flags (see BNODELOOKAT_)
	Returns : 
	Info : constraints are supplied in radians [-PI..+PI]
*/
void baNodeLookAt(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector worldFocusPoint,
				  float xConstraintNeg,	float xConstraintPos, float xAngleOffset,
				  float yConstraintNeg,  float yConstraintPos, float yAngleOffset, float t, uint32 flags)
{
        bkPrintf("*** WARNING *** baNodeLookAt was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetGlobalAlpha
	Purpose : set the global alpha scale value for actor drawing
	Parameters : new alpha scale value (256=solid)
	Returns : 
	Info : 
*/
void baSetGlobalAlpha(int newAlphaScale)
{
        bkPrintf("*** WARNING *** baSetGlobalAlpha was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baNodeLookAtUnconstrained
	Purpose : Orient a node to look at a point in world space
	Parameters : actor instance, actor-node instance, focus point, normalised move factor, max angle factor [0..1],
				 flags (see BNODELOOKATUC_)
	Returns : 
	Info : 
*/
void baNodeLookAtUnconstrained(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector worldFocusPoint,
							   TBVector refVector, float t, float maxAngleFactor, uint32 flags)
{
        bkPrintf("*** WARNING *** baNodeLookAtUnconstrained was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : void baNodeLookAtAdditional
	Purpose : Make a node look at a point, relative to the current frame of animation.
	Parameters : [in] actor instance
				[in] actor node instance
				[in] the world focus point
				[in] negative X constraint
				[in] positive X constraint
				[in] negative Y constraint
				[in] positive Y constraint
				[in] time value for the slerp
				[in] flags
	Returns : 
	Info : 
*/
void baNodeLookAtAdditional(TBActorInstance *instance, TBActorNodeInstance *nodeInstance, TBVector worldFocus, float xConstraintNeg, float xConstraintPos, float yConstraintNeg, float yConstraintPos, float t, uint32 flags)
{
        bkPrintf("*** WARNING *** baNodeLookAtAdditional was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : void baNodeLookAtAdditionalUnconstrained
	Purpose : Make a node look at a point, relative to the current frame of animation.
	Parameters : [in] actor instance
				[in] actor node instance
				[in] the world focus point
				[in] time value for the slerp
				[in] flags
	Returns : 
	Info : 
*/
void baNodeLookAtAdditionalUnconstrained(TBActorInstance *instance, TBActorNodeInstance *nodeInstance, TBVector worldFocus, float t, uint32 flags)
{
        bkPrintf("*** WARNING *** baNodeLookAtAdditionalUnconstrained was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baGetActorNodeGeometry
	Purpose : retrieve the stored geometry from an actor node
	Parameters : actor node, stream number (0..3), ptr to fill with #vertices (can be NULL),
																					ptr to fill with #tris (can be NULL)
	Returns : ptr to stream data
	Info : 
*/
void *baGetActorNodeGeometry(TBActorNode *actorNode, int stream, int *noofVertices, int *noofTris)
{
        bkPrintf("*** WARNING *** baGetActorNodeGeometry was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baGetActorGeometry
	Purpose : retrieve the stored geometry from an actor node (sub)hierarchy
	Parameters : actor instance, root instance node or NULL for all nodes, stream number (0..3),
				 ptr to fill with #vertices (can be NULL), ptr to fill with #tris (can be NULL),
				 ptr to fill with buffer size (can be NULL), ptr to buffer (or NULL for dynamic),
				 flags (see BGETACTGEOMETRY_)
	Returns : ptr to resultant stream data
	Info : 
*/
void *baGetActorGeometry(TBActorInstance *actorInstance, TBActorNodeInstance *rootNode, int stream, int *noofVerticesPtr,
						 int *noofTrisPtr, int *bufSize, void *buffer, uint32 flags)
{
        bkPrintf("*** WARNING *** baGetActorGeometry was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorTextureFrames
	Purpose : set all animated textures on an actor to a specific frame
	Parameters : actor ptr, frame number, under-run behaviour, over-run behaviour
	Returns : ptr to resultant stream data
	Info : 
*/
void baSetActorTextureFrames(TBActor *actor, int frame)
{
        bkPrintf("*** WARNING *** baSetActorTextureFrames was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorTextureFramesByTime
	Purpose : set all animated textures on an actor to a specific time
	Parameters : actor ptr, time in ms, under-run behaviour, over-run behaviour
	Returns : ptr to resultant stream data
	Info : 
*/
void baSetActorTextureFramesByTime(TBActor *actor, int time)
{
        bkPrintf("*** WARNING *** baSetActorTextureFramesByTime was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baCreateShadowVolume
	Purpose : create a shadow volume for an actor
	Parameters : ptr to actor instance, shadow volume to fill, position of light, extrusion distance,
																							flags (see BCREATESHADOWVOL_)
	Returns : ptr to shadow volume or NULL for failure
	Info : 
*/
TBShadowVolume *baCreateShadowVolume(TBActorInstance *actorInstance, TBShadowVolume *shadowVolume, TBVector lightPos,
																					float extrudeDist, uint32 flags)
{
        bkPrintf("*** WARNING *** baCreateShadowVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : baDeleteShadowVolume
	Purpose : delete a shadow volume
	Parameters : ptr to shadow volume to delete
	Returns : 
	Info : 
*/
void baDeleteShadowVolume(TBShadowVolume *shadowVolume)
{
        bkPrintf("*** WARNING *** baDeleteShadowVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baRenderShadowVolume
	Purpose : render a shadow volume into the stencil buffer
	Parameters : shadow volume to render
	Returns : 
	Info : 
*/
void baRenderShadowVolume(TBShadowVolume *shadowVolume)
{
        bkPrintf("*** WARNING *** baRenderShadowVolume was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baFillStencilShadows
	Purpose : fill in any shadow volume pixels left in the stencil buffer with a colour
	Parameters : red, green, blue, alpha (all 0..255)
	Returns : 
	Info : 
*/
void baFillStencilShadows(int r, int g, int b, int a)
{
        bkPrintf("*** WARNING *** baFillStencilShadows was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetActorNodeFlags
	Purpose : set/clear flags for an actor instance node and optionally it's children
	Parameters : actor instance, root instance node or NULL for all nodes, new flag values, mask of flags to set, recurse?
	Returns : 
	Info : 
*/
void baSetActorNodeFlags(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 flagValues,
																							uint32 flagMask, int recurse)
{
        bkPrintf("*** WARNING *** baSetActorNodeFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baIsActorAnimPlaying
	Purpose : determine if an animation is playing on a node and optionally it's children
	Parameters : actor instance, root instance node or NULL for all nodes, recurse?,
				 return anim segment playing (or blending to)
	Returns : TRUE if anim playing or FALSE otherwise
	Info : 
*/
int baIsActorAnimPlaying(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int recurse, TBActorAnimSegment **anim)
{
        bkPrintf("*** WARNING *** baIsActorAnimPlaying was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlendToNode
	Purpose : queue a copy queue animation entry on a node and its children
	Parameters : actor instance to copy to, actor-node instance to copy to, node to copy anim queue from, 't' delta,
																						flags, callback, callback context
	Returns : 
	Info : start frame of -1 gives start of segment
*/
void baQueueNodeAnimBlendToNode(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
													TBActorNodeInstance *srcNodeInstance, float tDelta, uint32 flags,
													TBQueuedAnimCallback callback, void *context)
{
        bkPrintf("*** WARNING *** baQueueNodeAnimBlendToNode was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baCopyAnimQueue
	Purpose : copy the animation queue information from one node hierarchy to another
	Parameters : dest actor instance, dest node (or NULL for all), source actor instance, source node (or NULL for all),
				 recurse?
	Returns : 
	Info : start frame of -1 gives start of segment
*/
void baCopyAnimQueue(TBActorInstance *destActorInstance, TBActorNodeInstance *destNodeInstance,
								TBActorInstance *srcActorInstance, TBActorNodeInstance *srcNodeInstance, int recurse)
{
        bkPrintf("*** WARNING *** baCopyAnimQueue was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAdditionalNodePosition
	Purpose : Set a node's additional matrix with a translation.
	Parameters : [in] the node to set up
				[in] the new translation.
	Returns : 
	Info : 
*/
void baSetAdditionalNodePosition(TBActorNodeInstance * const node, const TBVector pos)
{
        bkPrintf("*** WARNING *** baSetAdditionalNodePosition was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAdditionalNodeOrientation
	Purpose : Set a node's additional matrix with an orientation.
	Parameters : [in] the node to set up
				[in] the new orientation.
	Returns : 
	Info : 
*/
void baSetAdditionalNodeOrientation(TBActorNodeInstance * const node, const TBQuaternion orientation)
{
        bkPrintf("*** WARNING *** baSetAdditionalNodeOrientation was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baDrawActorBoundingBox
	Purpose : Draw an actor instance's bounding box
	Parameters : actor instance, colour
	Returns : 
	Info : 
*/
void baDrawActorBoundingBox(TBActorInstance *actInstance, int r, int g, int b, int a)
{
        bkPrintf("*** WARNING *** baDrawActorBoundingBox was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAnimQueueCallback
	Purpose : update an callback function for an actor instance
	Parameters : actor instance, root instance node or NULL for all nodes, 
				 callback, context data, flags (BACTORCALLBACKFLAG_*), 
				 queue entry to set from
	Returns : 
	Info : 
*/
void baSetAnimQueueCallback(TBActorInstance *actorInstance,
							TBActorNodeInstance *nodeInstance,
							TBQueuedAnimCallback newCallback,
							void *newContext,
							uint32 flags,
							int setFrom)
{
        bkPrintf("*** WARNING *** baSetAnimQueueCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : baSetAnimQueueFlags
	Purpose : update the flags for entries in an actor instance's anim queue
	Parameters : actor instance, root instance node or NULL for all nodes, 
				 flags to change, new flag values, queue entry to set from, recurse?
	Returns : 
	Info : 
*/
void baSetAnimQueueFlags(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
												uint32 flagMask, uint32 newFlagValues, int setFrom, int recurse)
{
        bkPrintf("*** WARNING *** baSetAnimQueueFlags was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/* --------------------------------------------------------------------------------
   Function : baSetActorNodeController
   Purpose : attach controller to specified node
   Parameters : node to attach to, controller functions, controller context data
   Returns :
   Info : 
*/
void baSetActorNodeController(TBActorNodeInstance *node, TBNodeController controller, void *context)
{
        bkPrintf("*** WARNING *** baSetActorNodeController was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bActorCountNoofVerts
	Purpose : Count number of verts in an actor
	Parameters : actor ptr
	Returns : 
	Info : 
*/
int bActorCountNoofVerts(TBActor *actor)
{
        bkPrintf("*** WARNING *** bActorCountNoofVerts was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}
