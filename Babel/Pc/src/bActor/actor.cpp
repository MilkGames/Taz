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

int bGlobalActorAlphaScale = 256;

// ********************************************************************************
// Locals

typedef struct _TAnimUpdateCallbackInfo
{
    EBQueueCallbackReason  reason;         // +0x00
    TBActorInstance       *actorInstance;  // +0x04
    TBActorNodeInstance   *nodeInstance;   // +0x08
    TBActorAnimSegment    *animSegment;    // +0x0C
    void                  *context;        // +0x10
    TBQueuedAnimCallback   callback;       // +0x14
    uint32                 pad1;		   // +0x18
    uint32                 pad2;		   // +0x1C
} TAnimUpdateCallbackInfo;

// ********************************************************************************
// Local Functions

int CountActorNodes(TBActorNode *node)
{
    int count = 0;
    TBActorNode *p = node;

    do
    {
        ++count;

        if (p->children != NULL)
        {
            count += CountActorNodes(p->children);
        }

        p = p->next;
    } while (p != node);

    return count;
}

TBActorNodeInstance *InitActorNodeInstances(TBActorNode *node, TBActorNodeInstance *instanceBase, TBActorNodeInstance *parentInstance, int queueSize)
{
    TBActorNode         *curNode      = node;
    TBActorNodeInstance *curInstance  = instanceBase;

    const int nodeStride = sizeof(TBActorNodeInstance) + (queueSize << 6);

    do
    {
        curInstance->position    = &curNode->position;
        curInstance->scale       = &curNode->scale;
        curInstance->orientation = &curNode->orientation;

        curInstance->parent    = parentInstance;
        curInstance->animQueue = (TBQueuedAnim *)((uchar *)curInstance + 0x160);
        curInstance->actorNode = curNode;

        curInstance->queueSize = (uchar)queueSize;

        bmMatCopy(curInstance->referenceMatrix, bIdentityMatrix);

        TBActorNodeInstance *nextBase = (TBActorNodeInstance *)((uchar *)curInstance + nodeStride);

        // ----- children -----
        if (curNode->children == NULL)
        {
            curInstance->children = NULL;
        }
        else
        {
            curInstance->children = nextBase;
            nextBase = InitActorNodeInstances(curNode->children, nextBase, curInstance, queueSize);
        }

        if (curNode->next == node)
            curInstance->next = instanceBase;
        else
            curInstance->next = nextBase;

        curInstance->next->prev = curInstance;

        if (curInstance->position->keys == NULL)
        {
            curInstance->animState.position[0] = 0.0f;
            curInstance->animState.position[1] = 0.0f;
            curInstance->animState.position[2] = 0.0f;
        }
        else
        {
            baEvalAnimTrack3(curInstance->animState.position, curInstance->position, 0);
        }

        // scale
        if (curInstance->scale->keys == NULL)
        {
            curInstance->animState.scale[0] = 0.0f;
            curInstance->animState.scale[1] = 0.0f;
            curInstance->animState.scale[2] = 0.0f;
        }
        else
        {
            baEvalAnimTrack3(curInstance->animState.scale, curInstance->scale, 0);
        }

        // orientation
        if (curInstance->orientation->keys == NULL)
        {
            curInstance->animState.orientation[0] = 0.0f;
            curInstance->animState.orientation[2] = 0.0f;
            curInstance->animState.orientation[1] = 1.0f;
        }
        else
        {
            baEvalAnimTrack4(curInstance->animState.orientation, curInstance->orientation, 0);
        }

		bmMatCopy(curInstance->referenceMatrix, bIdentityMatrix);

        curInstance->flags          = (ushort)curNode->flags;
        curInstance->animEvents     = curNode->actAnimEvents;
        curInstance->noofAnimEvents = curNode->noofActAnimEvents;
        curInstance->controller     = NULL;
        curInstance->context.iContext = 0;

        curNode     = curNode->next;
        curInstance = nextBase;
    } while (curNode != node);

    return curInstance;
}

void FixupActorNodeList(uchar *basePtr, TBActorNode *startNode, int recursionDepth, TBActor *actor)
{
    (void)recursionDepth;

    TBActorNode *node = startNode;
    do {
        if (node->next)     node->next     = (TBActorNode *)(basePtr + (uint32)node->next);
        if (node->prev)     node->prev     = (TBActorNode *)(basePtr + (uint32)node->prev);
        if (node->parent)   node->parent   = (TBActorNode *)(basePtr + (uint32)node->parent);
        if (node->children) node->children = (TBActorNode *)(basePtr + (uint32)node->children);

        if (node->position.keys)     node->position.keys     = (TBAnimKey3 *)(basePtr + (uint32)node->position.keys);
        if (node->scale.keys)        node->scale.keys        = (TBAnimKey3 *)(basePtr + (uint32)node->scale.keys);
        if (node->orientation.keys)  node->orientation.keys  = (TBAnimKey4 *)(basePtr + (uint32)node->orientation.keys);

        if (node->actAnimEvents)
            node->actAnimEvents = (TBActorAnimEvent *)(basePtr + (uint32)node->actAnimEvents);

        if (node->noofActAnimEvents > 0) {
            uint32 i = 0;
            uint32 byteOffset = 0;
            while ((int)i < node->noofActAnimEvents) {
                uint32 *pEvent    = (uint32 *)((uchar *)node->actAnimEvents + 0x4 + byteOffset);
                uint32 *pArgument = (uint32 *)((uchar *)node->actAnimEvents + 0x8 + byteOffset);

                *pEvent    += (uint32)actor->animEventData;
                *pArgument += (uint32)actor->animEventData;

                ++i;
                byteOffset += 0x10;
            }
        }

        if (node->type == TBACTORNODE_MESH) {
            bFixupMesh(basePtr, node, actor);
            float **p = node->mesh.storedGeometry.dataStreams;
            for (int i = 0; i < 4; ++i, ++p) {
                if (*p) *p = (float *)(basePtr + (uint32)(*p));
            }
        } else if (node->type == TBACTORNODE_ASYNCMESH) {
            bFixupAsyncMesh(&node->asyncMesh.asyncMesh, basePtr);
            float **p = node->asyncMesh.storedGeometry.dataStreams;
            for (int i = 0; i < 4; ++i, ++p) {
                if (*p) *p = (float *)(basePtr + (uint32)(*p));
            }
        }

        if (node->children)
            FixupActorNodeList(basePtr, node->children, recursionDepth + 1, actor);

        node = node->next;
    } while (node != startNode);
}

void PrepActorNode(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int prepChildren)
{
	TBMatrix orientMat;
	TBMatrix localMat;
	TBActorNodeInstance *parent;
	float xPos;
	float yPos;
	float zPos;

	bmQuatToMatrix(orientMat, nodeInstance->animState.orientation);

	parent = nodeInstance->parent;
	if (parent != NULL)
	{
		xPos = parent->animState.scale[0] * nodeInstance->animState.position[0];
		yPos = parent->animState.scale[1] * nodeInstance->animState.position[1];
		zPos = parent->animState.scale[2] * nodeInstance->animState.position[2];

		bmMatTransOrScale(
			localMat,
			xPos, yPos, zPos,
			orientMat,
			nodeInstance->animState.scale[0],
			nodeInstance->animState.scale[1],
			nodeInstance->animState.scale[2]
		);

		bmMatMultiply(nodeInstance->nodeToLocalWorld, parent->inheritedXForm, localMat);
	}
	else
	{
		bmMatTransOrScale(
			nodeInstance->nodeToLocalWorld,
			nodeInstance->animState.position[0],
			nodeInstance->animState.position[1],
			nodeInstance->animState.position[2],
			orientMat,
			nodeInstance->animState.scale[0],
			nodeInstance->animState.scale[1],
			nodeInstance->animState.scale[2]
		);
	}

	if (nodeInstance->actorNode->type == TBACTORNODE_SOFTBONE)
	{
		bSetMatrixPaletteEntry(actorInstance, nodeInstance);
	}

	if (nodeInstance->children != NULL)
	{
		if (nodeInstance == actorInstance->motionBone)
		{
			bmMatCopy(nodeInstance->inheritedXForm, bIdentityMatrix);
		}
		else if (nodeInstance->parent == NULL)
		{
			bmMatTransOr(
				nodeInstance->inheritedXForm,
				nodeInstance->animState.position[0],
				nodeInstance->animState.position[1],
				nodeInstance->animState.position[2],
				orientMat
			);
		}
		else
		{
			bmMatMultiply(nodeInstance->inheritedXForm, nodeInstance->parent->inheritedXForm, orientMat);
		}

		if (prepChildren != 0)
		{
			bPrepActorNodeList(nodeInstance->children, nodeInstance->inheritedXForm, nodeInstance->animState.scale, actorInstance);
		}
	}
}

void DeleteActorNodeList(TBActorNode* actorNode)
{
    TBActorNode* root = actorNode;
    TBActorNode* node = root;

    do
    {
        if (node->type == TBACTORNODE_MESH) {
            bDeleteMesh(node);
        } else if (node->type == TBACTORNODE_ASYNCMESH) {
            bDeleteAsyncMesh(node);
        }

        if (node->children != NULL) {
            DeleteActorNodeList(node->children);
        }

        node = node->next;
    }
    while (node != root);
}

void bRetargetAnimNode(TBActorNode *actorNode, TBActorNodeInstance *nodeInstance)
{
	TBActorNode *node;

	node = actorNode;
	do {
		nodeInstance->position = &node->position;
		nodeInstance->scale = &node->scale;
		nodeInstance->orientation = &node->orientation;
		nodeInstance->animEvents = node->actAnimEvents;
		nodeInstance->noofAnimEvents = node->noofActAnimEvents;
		nodeInstance->flags = (ushort)node->flags;
		if (node->children != NULL) {
			bRetargetAnimNode(node->children,nodeInstance->children);
		}
		node = node->next;
		nodeInstance = nodeInstance->next;
	} while (node != actorNode);
}

void PlayAnimEvents(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorNode *actorNode, int32 fromTick, int32 toTick,
					int32 direction, int32 includeEnds, TBActorAnimSegment *segment)
{
    (void)actorNode;

    int startFrame  = (fromTick + 0x200) >> 10;
    int endFrame    = (toTick   + 0x200) >> 10;

    if ((endFrame != startFrame) || (includeEnds != 0))
    {
        TBActorAnimEvent *eventList = nodeInstance->animEvents;
        int               numEvents = nodeInstance->noofAnimEvents;
        char              buffer[256];

        if (direction < 0)
        {
            if (fromTick < toTick)
            {
                int segStart = segment->startFrame >> 10;
                int segEnd   = segment->endFrame   >> 10;

                if (includeEnds == 0)
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((segStart <= frame && frame <= segEnd) &&
                                (frame < startFrame || endFrame <= frame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                    }
                }
                else
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((segStart <= frame && frame <= segEnd) &&
                                (frame <= startFrame || endFrame <= frame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
            }
            else
            {
                if (includeEnds == 0)
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((frame < startFrame) && (endFrame <= frame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
                else
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((frame <= startFrame) && (endFrame <= frame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
            }
        }
        else
        {
            if (toTick < fromTick)
            {
                int segStart = segment->startFrame >> 10; // iVar4
                int segEnd   = segment->endFrame   >> 10; // iVar3

                if (includeEnds == 0)
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((segStart <= frame && frame <= segEnd) &&
                                (startFrame < frame || frame <= endFrame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
                else
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((segStart <= frame && frame <= segEnd) &&
                                (startFrame <= frame || frame <= endFrame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
            }
            else
            {
                if (includeEnds == 0)
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((startFrame < frame) && (frame <= endFrame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
                else
                {
                    if (numEvents > 0)
                    {
                        TBActorAnimEvent *ev = eventList;
                        for (int i = 0; i < numEvents; ++i, ++ev)
                        {
                            int frame = ev->frame;
                            if ((startFrame <= frame) && (frame <= endFrame))
                            {
                                sprintf(buffer, "%08x %08x %s",
                                        (uint32)actorInstance,
                                        (uint32)nodeInstance,
                                        ev->argument);
                                bkGenerateEvent(ev->event, buffer, NULL, 1);
                            }
                        }
                        return;
                    }
                }
            }
        }
    }

    return;
}

int bUpdateNodes(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, float timeDelta, TBVector motionVector,
				 TAnimUpdateCallbackInfo *callbackInfo, int callbackCount, int *nodesUpdated)
{
	TBActorInstance *actor = actorInstance;
	TBActorNodeInstance *curNode = nodeInstance;
	TBActorNode *actorNode;
	TBQueuedAnim *curQueueEntry;
	TBQueuedAnim *srcQueueEntry;
	TBActorAnimSegment *seg;
	TAnimUpdateCallbackInfo *cbOut;
	ushort animFlags;
	uint flag8;
	uint32 dwordCount;
	int oldFrame1;
	int oldFrame2;
	int newFrame1;
	int advFrames;
	int advFrames2;
	float newT;
	float oldT;
	float timeLeft;
	TBVector basePos;
	TBVector targetPos;
	uint32 blendTargetFrame;
	uchar curQueueIndex;

LAB_00560b80:
	actorNode = curNode->actorNode;
	timeLeft = timeDelta;

	if ((curNode == actor->motionBone) && (motionVector != NULL))
	{
		motionVector[0] = 0.0f;
		motionVector[1] = 0.0f;
		motionVector[2] = 0.0f;
		motionVector[3] = 1.0f;
	}

	curNode->flags = (ushort)(curNode->flags & 0xfe3f);

LAB_00560bb6:
	if (curNode->animQueue[curNode->currentQueue].active == '\0')
	{
		goto switchD_00560c1b_default;
	}

	(*nodesUpdated)++;

	curQueueIndex = curNode->currentQueue;
	curQueueEntry = curNode->animQueue + curQueueIndex;
	animFlags = curQueueEntry->flags;
	oldFrame1 = curQueueEntry->frame1;
	oldFrame2 = curQueueEntry->frame2;
	flag8 = animFlags & 8;
	oldT = curQueueEntry->t;

	curQueueEntry->flags = (ushort)(animFlags & 0xfff7);

	switch (curQueueEntry->type)
	{
	case BANIMTYPE_NORMAL:
	{
		if ((curNode == actor->motionBone) && ((animFlags & 8) != 0))
		{
			baEvalAnimTrack3(curNode->animState.position, curNode->position, oldFrame1);
		}

		advFrames = (int)(curQueueEntry->deltaFrame1 * timeLeft);
		curQueueEntry->frame1 += advFrames;
		newFrame1 = curQueueEntry->frame1;
		seg = curQueueEntry->animSeg1;
		oldFrame2 = seg->endFrame;

		if ((curQueueEntry->flags & 1) == 0)
		{
			if (oldFrame2 < newFrame1)
			{
				if (curNode->noofAnimEvents != 0)
				{
					PlayAnimEvents(actor, curNode, actorNode, oldFrame1, oldFrame2, 1, (int)flag8, seg);
				}

				curQueueEntry->repeatCount--;

				if (curQueueEntry->repeatCount < 1)
				{
					timeLeft -= ((float)(curQueueEntry->animSeg1->endFrame - oldFrame1) / (float)(curQueueEntry->frame1 - oldFrame1)) * timeDelta;

					if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
					{
						cbOut = &callbackInfo[callbackCount++];
						cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
						cbOut->actorInstance = actor;
						cbOut->nodeInstance = curNode;
						cbOut->animSegment = curQueueEntry->animSeg1;
						cbOut->context = curQueueEntry->context;
						cbOut->callback = curQueueEntry->callback;
					}

					curQueueEntry->active = '\0';
					curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
					curNode->currentQueue = curQueueIndex;

					if (curNode->animQueue[curQueueIndex].active != '\0')
					{
						goto LAB_00560bb6;
					}

					curQueueEntry->frame1 = curQueueEntry->animSeg1->endFrame;
				}
				else
				{
					curQueueEntry->frame1 = (curQueueEntry->frame1 - seg->endFrame) % ((seg->endFrame - seg->startFrame) + 1) + seg->startFrame;

					if ((curNode == actor->motionBone) && (motionVector != NULL))
					{
						baEvalAnimTrack3(basePos, curNode->position, seg->endFrame);
						motionVector[0] += basePos[0] - curNode->animState.position[0];
						motionVector[1] += basePos[1] - curNode->animState.position[1];
						motionVector[2] += basePos[2] - curNode->animState.position[2];
						baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->startFrame);
					}
				}
			}

			int segStart = curQueueEntry->animSeg1->startFrame;

			if (curQueueEntry->frame1 < segStart)
			{
				if (curNode->noofAnimEvents != 0)
				{
					PlayAnimEvents(actor, curNode, actorNode, oldFrame1, segStart, -1, (int)flag8, curQueueEntry->animSeg1);
				}

				curQueueEntry->repeatCount--;

				if (curQueueEntry->repeatCount < 1)
				{
					timeLeft -= ((float)(oldFrame1 - curQueueEntry->animSeg1->endFrame) / (float)(oldFrame1 - curQueueEntry->frame1)) * timeDelta;

					if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
					{
						cbOut = &callbackInfo[callbackCount++];
						cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
						cbOut->actorInstance = actor;
						cbOut->nodeInstance = curNode;
						cbOut->animSegment = curQueueEntry->animSeg1;
						cbOut->context = curQueueEntry->context;
						cbOut->callback = curQueueEntry->callback;
					}

					curQueueEntry->active = '\0';
					curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
					curNode->currentQueue = curQueueIndex;

					if (curNode->animQueue[curQueueIndex].active != '\0')
					{
						goto LAB_00560bb6;
					}

					curQueueEntry->frame1 = curQueueEntry->animSeg1->startFrame;
				}
				else
				{
					int segEnd2 = curQueueEntry->animSeg1->endFrame;
					int segStart2 = curQueueEntry->animSeg1->startFrame;

					curQueueEntry->frame1 = segEnd2 - (segStart2 - curQueueEntry->frame1) % ((segEnd2 - segStart2) + 1);

					if ((curNode == actor->motionBone) && (motionVector != NULL))
					{
						baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->startFrame);
						motionVector[0] += basePos[0] - curNode->animState.position[0];
						motionVector[1] += basePos[1] - curNode->animState.position[1];
						motionVector[2] += basePos[2] - curNode->animState.position[2];
						baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->endFrame);
					}
				}
			}

			if (curNode->noofAnimEvents != 0)
			{
				PlayAnimEvents(actor, curNode, actorNode, oldFrame1, curQueueEntry->frame1, advFrames, (int)flag8, curQueueEntry->animSeg1);
			}
		}
		else
		{
			if (oldFrame2 < newFrame1)
			{
				int segStart = curQueueEntry->animSeg1->startFrame;
				curQueueEntry->frame1 = (curQueueEntry->frame1 - oldFrame2) % ((oldFrame2 - segStart) + 1) + segStart;

				if ((curNode == actor->motionBone) && (motionVector != NULL))
				{
					baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->endFrame);
					motionVector[0] += basePos[0] - curNode->animState.position[0];
					motionVector[1] += basePos[1] - curNode->animState.position[1];
					motionVector[2] += basePos[2] - curNode->animState.position[2];
					baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->startFrame);
				}
			}

			oldFrame2 = curQueueEntry->animSeg1->startFrame;

			if (curQueueEntry->frame1 < oldFrame2)
			{
				newFrame1 = curQueueEntry->animSeg1->endFrame;
				curQueueEntry->frame1 = newFrame1 - (oldFrame2 - curQueueEntry->frame1) % ((newFrame1 - oldFrame2) + 1);

				if ((curNode == actor->motionBone) && (motionVector != NULL))
				{
					baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->startFrame);
					motionVector[0] += basePos[0] - curNode->animState.position[0];
					motionVector[1] += basePos[1] - curNode->animState.position[1];
					motionVector[2] += basePos[2] - curNode->animState.position[2];
					baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->endFrame);
				}
			}

			if (curNode->noofAnimEvents != 0)
			{
				seg = curQueueEntry->animSeg1;
				oldFrame2 = curQueueEntry->frame1;
				PlayAnimEvents(actor, curNode, actorNode, oldFrame1, oldFrame2, advFrames, (int)flag8, seg);
			}
		}

		if ((curNode == actor->motionBone) && (motionVector != NULL))
		{
			basePos[0] = curNode->animState.position[0];
			basePos[1] = curNode->animState.position[1];
			basePos[2] = curNode->animState.position[2];

			baSetNodeFrame(actor, curNode, curQueueEntry->frame1, 0);

			motionVector[0] += curNode->animState.position[0] - basePos[0];
			motionVector[1] += curNode->animState.position[1] - basePos[1];
			motionVector[2] += curNode->animState.position[2] - basePos[2];
		}
		else
		{
			baSetNodeFrame(actor, curNode, curQueueEntry->frame1, 0);
		}

		goto switchD_00560c1b_default;
	}
	case BANIMTYPE_BLENDFRAMES:
	{
		if ((curNode == actor->motionBone) && ((animFlags & 8) != 0))
		{
			baEvalAnimTrack3(curNode->animState.position, curNode->position, oldFrame2);
		}

		newT = timeLeft * curQueueEntry->deltaT + curQueueEntry->t;
		curQueueEntry->t = newT;

		if (newT <= 1.0f)
		{
			if (newT >= 0.0f)
			{
				advFrames = (int)(curQueueEntry->deltaFrame1 * timeLeft);
				curQueueEntry->frame1 += advFrames;

				advFrames = (int)(curQueueEntry->deltaFrame2 * timeLeft);
				advFrames2 = advFrames;

				newFrame1 = curQueueEntry->frame1;
				curQueueEntry->frame2 += advFrames2;

				if ((curQueueEntry->flags & 1) == 0)
				{
					seg = curQueueEntry->animSeg1;

					if (seg->endFrame < newFrame1)
					{
						timeLeft -= ((float)(seg->endFrame - oldFrame1) / (float)(newFrame1 - oldFrame1)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						seg = curQueueEntry->animSeg1;
						curQueueEntry->frame1 = seg->endFrame;
					}

					if (curQueueEntry->frame1 < seg->startFrame)
					{
						timeLeft -= ((float)(oldFrame1 - seg->endFrame) / (float)(oldFrame1 - curQueueEntry->frame1)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						curQueueEntry->frame1 = curQueueEntry->animSeg1->startFrame;
					}

					seg = curQueueEntry->animSeg2;

					if (seg->endFrame < curQueueEntry->frame2)
					{
						if (curNode->noofAnimEvents != 0)
						{
							PlayAnimEvents(actor, curNode, actorNode, oldFrame2, seg->endFrame, 1, (int)flag8, seg);
						}

						timeLeft -= ((float)(curQueueEntry->animSeg2->endFrame - oldFrame2) / (float)(curQueueEntry->frame2 - oldFrame2)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						seg = curQueueEntry->animSeg2;
						curQueueEntry->frame2 = seg->endFrame;
					}

					if (curQueueEntry->frame2 < seg->startFrame)
					{
						if (curNode->noofAnimEvents != 0)
						{
							PlayAnimEvents(actor, curNode, actorNode, oldFrame2, seg->startFrame, -1, (int)flag8, seg);
						}

						timeLeft -= ((float)(oldFrame2 - curQueueEntry->animSeg2->endFrame) / (float)(oldFrame2 - curQueueEntry->frame2)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						curQueueEntry->frame2 = curQueueEntry->animSeg2->startFrame;
					}

					if (curNode->noofAnimEvents != 0)
					{
						PlayAnimEvents(actor, curNode, actorNode, oldFrame2, curQueueEntry->frame2, advFrames2, (int)flag8, curQueueEntry->animSeg2);
					}
				}
				else
				{
					seg = curQueueEntry->animSeg1;
					oldFrame1 = seg->endFrame;

					if (oldFrame1 < newFrame1)
					{
						curQueueEntry->frame1 = (curQueueEntry->frame1 - oldFrame1) % ((oldFrame1 - seg->startFrame) + 1) + seg->startFrame;
					}

					oldFrame1 = seg->startFrame;

					if (curQueueEntry->frame1 < oldFrame1)
					{
						curQueueEntry->frame1 = seg->endFrame - (oldFrame1 - curQueueEntry->frame1) % ((seg->endFrame - oldFrame1) + 1);
					}

					oldFrame1 = curQueueEntry->animSeg2->endFrame;

					if (oldFrame1 < curQueueEntry->frame2)
					{
                        int seg2Start = curQueueEntry->animSeg2->startFrame;
                        curQueueEntry->frame2 =
                            (curQueueEntry->frame2 - oldFrame1) % ((oldFrame1 - seg2Start) + 1) + seg2Start;

                        if ((curNode == actor->motionBone) && (motionVector != NULL))
                        {
                            baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->endFrame);
                            baEvalAnimTrack3(targetPos, curNode->position, curQueueEntry->animSeg2->endFrame);

                            basePos[0] = (targetPos[0] - basePos[0]) * curQueueEntry->t + basePos[0];
                            basePos[1] = (targetPos[1] - basePos[1]) * curQueueEntry->t + basePos[1];
                            basePos[2] = (targetPos[2] - basePos[2]) * curQueueEntry->t + basePos[2];

                            motionVector[0] += basePos[0] - curNode->animState.position[0];
                            motionVector[1] += basePos[1] - curNode->animState.position[1];
                            motionVector[2] += basePos[2] - curNode->animState.position[2];

                            baEvalAnimTrack3(curNode->animState.position, curNode->position, seg2Start);
                        }
					}

					oldFrame1 = curQueueEntry->animSeg2->startFrame;

					if (curQueueEntry->frame2 < oldFrame1)
					{
                        int seg2End = curQueueEntry->animSeg2->endFrame;
                        curQueueEntry->frame2 =
                            seg2End - (oldFrame1 - curQueueEntry->frame2) % ((seg2End - oldFrame1) + 1);

                        if ((curNode == actor->motionBone) && (motionVector != NULL))
                        {
                            baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->startFrame);
                            baEvalAnimTrack3(targetPos, curNode->position, curQueueEntry->animSeg2->endFrame);

                            basePos[0] = (targetPos[0] - basePos[0]) * curQueueEntry->t + basePos[0];
                            basePos[1] = (targetPos[1] - basePos[1]) * curQueueEntry->t + basePos[1];
                            basePos[2] = (targetPos[2] - basePos[2]) * curQueueEntry->t + basePos[2];

                            motionVector[0] += basePos[0] - curNode->animState.position[0];
                            motionVector[1] += basePos[1] - curNode->animState.position[1];
                            motionVector[2] += basePos[2] - curNode->animState.position[2];

                            baEvalAnimTrack3(curNode->animState.position, curNode->position, seg2End);
                        }						
					}

					if (curNode->noofAnimEvents != 0)
					{
						PlayAnimEvents(actor, curNode, actorNode, oldFrame2, curQueueEntry->frame2, advFrames2, (int)flag8, curQueueEntry->animSeg2);
					}
				}

				if ((curNode == actor->motionBone) && (motionVector != NULL))
				{
					basePos[0] = curNode->animState.position[0];
					basePos[1] = curNode->animState.position[1];
					basePos[2] = curNode->animState.position[2];

					baSetNodeBlend(actor, curNode, curQueueEntry->frame1, curQueueEntry->frame2, curQueueEntry->t, 0);

					motionVector[0] += curNode->animState.position[0] - basePos[0];
					motionVector[1] += curNode->animState.position[1] - basePos[1];
					motionVector[2] += curNode->animState.position[2] - basePos[2];
				}
				else
				{
					baSetNodeBlend(actor, curNode, curQueueEntry->frame1, curQueueEntry->frame2, curQueueEntry->t, 0);
				}

				goto switchD_00560c1b_default;
		}

		animFlags = curQueueEntry->flags;
		timeLeft -= (oldT / (oldT - newT)) * timeDelta;

		if (((animFlags & 2) != 0) || (curQueueEntry->repeatCount > 1))
		{
			goto LAB_005616ab;
		}
	}
	else
	{
		animFlags = curQueueEntry->flags;
		timeLeft -= ((1.0f - oldT) / (newT - oldT)) * timeDelta;

		if (((animFlags & 2) != 0) || (curQueueEntry->repeatCount > 1))
		{
			curQueueEntry->type = BANIMTYPE_NORMAL;
			curQueueEntry->animSeg1 = curQueueEntry->animSeg2;
			curQueueEntry->frame1 = curQueueEntry->frame2;
			curQueueEntry->deltaFrame1 = curQueueEntry->deltaFrame2;
			goto LAB_00560bb6;
		}
	}

	if ((curQueueEntry->callback != NULL) && ((animFlags & 4) == 0) && (callbackCount < 8))
	{
		cbOut = &callbackInfo[callbackCount++];
		cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
		cbOut->actorInstance = actor;
		cbOut->nodeInstance = curNode;
		cbOut->animSegment = curQueueEntry->animSeg1;
		cbOut->context = curQueueEntry->context;
		cbOut->callback = curQueueEntry->callback;
	}

	curQueueEntry->active = '\0';
	curNode->currentQueue = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
	goto LAB_00560bb6;
	}
	case BANIMTYPE_BLENDSTATE:
	{
		if ((curNode == actor->motionBone) && ((animFlags & 8) != 0))
		{
			baEvalAnimTrack3(curNode->animState.position, curNode->position, oldFrame2);
		}

		newT = timeLeft * curQueueEntry->deltaT + curQueueEntry->t;
		curQueueEntry->t = newT;

		if (newT <= 1.0f)
		{
			if (newT >= 0.0f)
			{
				advFrames = (int)(curQueueEntry->deltaFrame1 * timeLeft);
				oldFrame2 = advFrames;
				newFrame1 = curQueueEntry->frame1 + oldFrame2;
				curQueueEntry->frame1 = newFrame1;

				if (((curQueueEntry->flags & 1) == 0) && (curQueueEntry->repeatCount < 2))
				{
					seg = curQueueEntry->animSeg1;

					if (seg->endFrame < newFrame1)
					{
						if (curNode->noofAnimEvents != 0)
						{
							PlayAnimEvents(actor, curNode, actorNode, oldFrame1, seg->endFrame, 1, (int)flag8, seg);
						}

						timeLeft -= ((float)(curQueueEntry->animSeg1->endFrame - oldFrame1) / (float)(curQueueEntry->frame1 - oldFrame1)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						seg = curQueueEntry->animSeg1;
						curQueueEntry->frame1 = seg->endFrame;
					}

					if (curQueueEntry->frame1 < seg->startFrame)
					{
						if (curNode->noofAnimEvents != 0)
						{
							PlayAnimEvents(actor, curNode, actorNode, oldFrame1, seg->startFrame, -1, (int)flag8, seg);
						}

						timeLeft -= ((float)(oldFrame1 - curQueueEntry->animSeg1->startFrame) / (float)(oldFrame1 - curQueueEntry->frame1)) * timeDelta;

						if ((curQueueEntry->callback != NULL) && ((curQueueEntry->flags & 4) == 0) && (callbackCount < 8))
						{
							cbOut = &callbackInfo[callbackCount++];
							cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
							cbOut->actorInstance = actor;
							cbOut->nodeInstance = curNode;
							cbOut->animSegment = curQueueEntry->animSeg1;
							cbOut->context = curQueueEntry->context;
							cbOut->callback = curQueueEntry->callback;
						}

						curQueueEntry->active = '\0';
						curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
						curNode->currentQueue = curQueueIndex;

						if (curNode->animQueue[curQueueIndex].active != '\0')
						{
							goto LAB_00560bb6;
						}

						curQueueEntry->frame1 = curQueueEntry->animSeg1->startFrame;
					}

					if (curNode->noofAnimEvents != 0)
					{
						PlayAnimEvents(actor, curNode, actorNode, oldFrame1, curQueueEntry->frame1, oldFrame2, (int)flag8, curQueueEntry->animSeg1);
					}
				}
				else
				{
					int segEnd = curQueueEntry->animSeg1->endFrame;

					if (segEnd < newFrame1)
					{
						int segStart = curQueueEntry->animSeg1->startFrame;
						curQueueEntry->frame1 = (newFrame1 - segEnd) % ((segEnd - segStart) + 1) + segStart;

						if ((curNode == actor->motionBone) && (motionVector != NULL))
						{
							baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->endFrame);
							motionVector[0] += basePos[0] - curNode->animState.position[0];
							motionVector[1] += basePos[1] - curNode->animState.position[1];
							motionVector[2] += basePos[2] - curNode->animState.position[2];
							baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->startFrame);
						}
					}

					int segStart = curQueueEntry->animSeg1->startFrame;

					if (curQueueEntry->frame1 < segStart)
					{
						int segEnd2 = curQueueEntry->animSeg1->endFrame;
						curQueueEntry->frame1 = segEnd2 - (segStart - curQueueEntry->frame1) % ((segEnd2 - segStart) + 1);

						if ((curNode == actor->motionBone) && (motionVector != NULL))
						{
							baEvalAnimTrack3(basePos, curNode->position, curQueueEntry->animSeg1->startFrame);
							motionVector[0] += basePos[0] - curNode->animState.position[0];
							motionVector[1] += basePos[1] - curNode->animState.position[1];
							motionVector[2] += basePos[2] - curNode->animState.position[2];
							baEvalAnimTrack3(curNode->animState.position, curNode->position, curQueueEntry->animSeg1->endFrame);
						}
					}

					if (curNode->noofAnimEvents != 0)
					{
						PlayAnimEvents(actor, curNode, actorNode, oldFrame1, curQueueEntry->frame1, oldFrame2, (int)flag8, curQueueEntry->animSeg1);
					}

					if (curQueueEntry->repeatCount > 1)
					{
						curQueueEntry->repeatCount--;
					}
				}

				if ((curNode == actor->motionBone) && (motionVector != NULL))
				{
					basePos[0] = curNode->animState.position[0];
					basePos[1] = curNode->animState.position[1];
					basePos[2] = curNode->animState.position[2];

					baSetNodeBlendTargetValue(actor, curNode, curQueueEntry->frame1, curQueueEntry->t, 0);

					motionVector[0] += curNode->animState.position[0] - basePos[0];
					motionVector[1] += curNode->animState.position[1] - basePos[1];
					motionVector[2] += curNode->animState.position[2] - basePos[2];
				}
				else
				{
					oldT = curQueueEntry->t;
					blendTargetFrame = curQueueEntry->frame1;
LAB_00561eca:
					baSetNodeBlendTargetValue(actor, curNode, blendTargetFrame, oldT, 0);
				}

				goto switchD_00560c1b_default;
			}

			oldT = oldT / (oldT - newT);
		}
		else
		{
			oldT = (1.0f - oldT) / (newT - oldT);
		}

		timeLeft -= oldT * timeDelta;

		goto LAB_005616ab;
	}
LAB_005616ab:
		curQueueEntry->type = BANIMTYPE_NORMAL;

		goto LAB_00560bb6;
	case BANIMTYPE_COPYQUEUE:
	{
		srcQueueEntry = curQueueEntry->srcNodeInstance->animQueue + curQueueEntry->srcNodeInstance->currentQueue;

		if (srcQueueEntry->active != '\0')
		{
			oldT = curQueueEntry->deltaT + curQueueEntry->t;
			curQueueEntry->t = oldT;

			if (oldT <= 1.0f)
			{
				if (srcQueueEntry->type == BANIMTYPE_NORMAL)
				{
					curQueueEntry->animSeg1 = srcQueueEntry->animSeg1;
					blendTargetFrame = (uint32)srcQueueEntry->frame1;
					curQueueEntry->frame1 = (int)blendTargetFrame;
					curQueueEntry->deltaFrame1 = 0;

					goto LAB_00561eca;
				}

				if ((curQueueEntry->callback != NULL) && ((animFlags & 4) == 0) && (callbackCount < 8))
				{
					cbOut = &callbackInfo[callbackCount++];
					cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
					cbOut->actorInstance = actor;
					cbOut->nodeInstance = curNode;
					cbOut->animSegment = curQueueEntry->animSeg1;
					cbOut->context = curQueueEntry->context;
					cbOut->callback = curQueueEntry->callback;
				}
			}
			else if ((curQueueEntry->callback != NULL) && ((animFlags & 4) == 0) && (callbackCount < 8))
			{
				cbOut = &callbackInfo[callbackCount++];
				cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
				cbOut->actorInstance = actor;
				cbOut->nodeInstance = curNode;
				cbOut->animSegment = curQueueEntry->animSeg1;
				cbOut->context = curQueueEntry->context;
				cbOut->callback = curQueueEntry->callback;
			}

			curNode->currentQueue = curQueueEntry->srcNodeInstance->currentQueue;

			const uint32 *src = (const uint32 *)curQueueEntry->srcNodeInstance->animQueue;
			uint32 *dst = (uint32 *)curNode->animQueue;
			dwordCount = ((uint32)curNode->queueSize << 6) >> 2;

			while (dwordCount-- != 0) *dst++ = *src++;

			goto LAB_00560bb6;
		}

		if ((curQueueEntry->callback != NULL) && ((animFlags & 4) == 0) && (callbackCount < 8))
		{
			cbOut = &callbackInfo[callbackCount++];
			cbOut->reason = BANIMQUEUEREASON_ENTRYCOMPLETE;
			cbOut->actorInstance = actor;
			cbOut->nodeInstance = curNode;
			cbOut->animSegment = curQueueEntry->animSeg1;
			cbOut->context = curQueueEntry->context;
			cbOut->callback = curQueueEntry->callback;
		}

		curQueueEntry->active = '\0';
		curQueueIndex = (uchar)((curNode->currentQueue + 1) % (uint)curNode->queueSize);
		curNode->currentQueue = curQueueIndex;

		if (curNode->animQueue[curQueueIndex].active != '\0')
		{
			goto LAB_00560bb6;
		}

		goto switchD_00560c1b_default;
	}
	default:
		goto switchD_00560c1b_default;
	}


switchD_00560c1b_default:
	if ((curNode == actor->motionBone) && (motionVector != NULL))
	{
		bmMatMultiplyVector(actor->orientationMatrix, motionVector);
		motionVector[0] = actor->scale[0] * motionVector[0];
		motionVector[1] = actor->scale[1] * motionVector[1];
		motionVector[2] = actor->scale[2] * motionVector[2];
	}

	if (curNode->controller != NULL)
	{
		curNode->controller(curNode, curNode->context.pContext);
	}

	if (curNode->children != NULL)
	{
		callbackCount = bUpdateNodes(actor, curNode->children, timeDelta, motionVector, callbackInfo, callbackCount, nodesUpdated);
	}

	curNode = curNode->next;

	if (curNode == nodeInstance)
	{
		return callbackCount;
	}

	goto LAB_00560b80;
}

TBStoredGeometry* CountActorGeometry(TBActorInstance* instance, TBActorNodeInstance* nodeInst, int32 streamIndex, int32* outVertexCount, int32* outTriangleCount)
{
    TBActorNode* node = nodeInst->actorNode;
    TBStoredGeometry* geom = NULL;

    // Select stored geometry block based on node type
    if (node->type == 2) {
        geom = &node->mesh.storedGeometry;
    } else if (node->type == 4) {
        geom = &node->asyncMesh.storedGeometry;
    }

    // If we have stored geometry and the requested stream exists, accumulate counts
    if (geom != NULL)
    {
        if (geom->dataStreams[streamIndex] != NULL)
        {
            *outVertexCount   += geom->noofVertices;
            *outTriangleCount += geom->noofTriangles;
        }
    }

    // Recurse into children (ring list)
    {
        TBActorNodeInstance* child = nodeInst->children; // +0x14C
        if (child != NULL)
        {
            TBActorNodeInstance* it = child;
            do
            {
                TBStoredGeometry* childGeom =
                    CountActorGeometry(instance, it, streamIndex, outVertexCount, outTriangleCount);

                if (childGeom != NULL) {
                    geom = childGeom; // disasm keeps "last non-null" returned
                }

                it = it->next;
            } while (it != child);
        }
    }

    return geom;
}

static void TransformPoint3InPlace(float *x, float *y, float *z, const TBMatrix m)
{
    const float sx = *x;
    const float sy = *y;
    const float sz = *z;

    *x = (sx * m[0][0]) + (sy * m[1][0]) + (sz * m[2][0]) + m[3][0];
    *y = (sx * m[0][1]) + (sy * m[1][1]) + (sz * m[2][1]) + m[3][1];
    *z = (sx * m[0][2]) + (sy * m[1][2]) + (sz * m[2][2]) + m[3][2];
}

static void TransformVector3InPlace(float *x, float *y, float *z, const TBMatrix m)
{
    const float sx = *x;
    const float sy = *y;
    const float sz = *z;

    *x = (sx * m[0][0]) + (sy * m[1][0]) + (sz * m[2][0]);
    *y = (sx * m[0][1]) + (sy * m[1][1]) + (sz * m[2][1]);
    *z = (sx * m[0][2]) + (sy * m[1][2]) + (sz * m[2][2]);
}

static void TransformPositionFromComponents(char *rec, const TBStoredGeometryStreamHeader *hdr, int compX, int compY, int compZ, const TBMatrix m)
{
    const uchar offX = hdr->componentMap[compX];
    const uchar offY = hdr->componentMap[compY];
    const uchar offZ = hdr->componentMap[compZ];

    int32 hasAny = 0;
    float x = 0.0f, y = 0.0f, z = 0.0f;

    if (offX != 0xFF) { x = *(float *)(rec + offX); hasAny = 1; }
    if (offY != 0xFF) { y = *(float *)(rec + offY); hasAny = 1; }
    if (offZ != 0xFF) { z = *(float *)(rec + offZ); hasAny = 1; }

    if (!hasAny)
        return;

    TransformPoint3InPlace(&x, &y, &z, m);

    if (offX != 0xFF) *(float *)(rec + offX) = x;
    if (offY != 0xFF) *(float *)(rec + offY) = y;
    if (offZ != 0xFF) *(float *)(rec + offZ) = z;
}

static void TransformVectorFromComponents(char *rec, const TBStoredGeometryStreamHeader *hdr, int compX, int compY, int compZ, const TBMatrix m)
{
    const uchar offX = hdr->componentMap[compX];
    const uchar offY = hdr->componentMap[compY];
    const uchar offZ = hdr->componentMap[compZ];

    int32 hasAny = 0;
    float x = 0.0f, y = 0.0f, z = 0.0f;

    if (offX != 0xFF) { x = *(float *)(rec + offX); hasAny = 1; }
    if (offY != 0xFF) { y = *(float *)(rec + offY); hasAny = 1; }
    if (offZ != 0xFF) { z = *(float *)(rec + offZ); hasAny = 1; }

    if (!hasAny)
        return;

    TransformVector3InPlace(&x, &y, &z, m);

    if (offX != 0xFF) *(float *)(rec + offX) = x;
    if (offY != 0xFF) *(float *)(rec + offY) = y;
    if (offZ != 0xFF) *(float *)(rec + offZ) = z;
}

static void FixupIndexComponent(char *rec, const TBStoredGeometryStreamHeader *hdr, int compIdx, int32 baseVertex)
{
    const uchar off = hdr->componentMap[compIdx];
    if (off == 0xFF)
        return;

    ushort *idx = (ushort *)(rec + off);
    *idx = (ushort)(*idx + (ushort)baseVertex);
}

static char *GetActorGeometry(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int stream,
                             char *dst, int32 *baseVertexPtr, uint32 flags)
{
    TBActorNode *node;
    const TBStoredGeometry *stored;
    TBStoredGeometryStreamHeader *header;
    TBMatrix localMatrix;
    int32 baseVertexStart;

    if (nodeInstance == NULL)
        return dst;

    node = nodeInstance->actorNode;
    stored = NULL;

    if (node != NULL)
    {
        // 2 = mesh, 4 = async mesh
        if (node->type == 2)
            stored = &node->mesh.storedGeometry;
        else if (node->type == 4)
            stored = &node->asyncMesh.storedGeometry;
    }

    baseVertexStart = (baseVertexPtr != NULL) ? *baseVertexPtr : 0;

    if (stored != NULL && baseVertexPtr != NULL)
        *baseVertexPtr = baseVertexStart + stored->noofVertices;

    header = NULL;
    if (stored != NULL)
        header = (TBStoredGeometryStreamHeader *)stored->dataStreams[stream];

    if (header != NULL)
    {
        const int32 recordSize = (int32)header->recordSize;
        const char *srcBase = (const char *)(header + 1);
        int32 count;
        int32 i;

        if (flags & BGETACTGEOMETRY_WORLDSPACE)
        {
            bmMatMultiply(localMatrix, actorInstance->objectToWorld, nodeInstance->nodeToLocalWorld);
        }
        else if (flags & BGETACTGEOMETRY_NODESPACE)
        {
            bmMatIdentity(localMatrix);
        }
        else
        {
            bmMatCopy(localMatrix, nodeInstance->nodeToLocalWorld);
        }

        count = (header->streamType == 0) ? stored->noofVertices : stored->noofTriangles;

        if (flags & BGETACTGEOMETRY_COUNTONLY)
        {
            dst += (recordSize * count);
        }
        else
        {
            for (i = 0; i < count; i++)
            {
                const char *srcRec = srcBase + (recordSize * i);
                char *dstRec = dst;

                memcpy(dstRec, srcRec, recordSize);

                if (header->streamType == 0)
                {
                    TransformPositionFromComponents(dstRec, header, EBDATASTREAMCOMP_X,  EBDATASTREAMCOMP_Y,  EBDATASTREAMCOMP_Z,  localMatrix);
                    TransformVectorFromComponents  (dstRec, header, EBDATASTREAMCOMP_VNX, EBDATASTREAMCOMP_VNY, EBDATASTREAMCOMP_VNZ, localMatrix);
                }
                else
                {
                    FixupIndexComponent(dstRec, header, EBDATASTREAMCOMP_IDX1, baseVertexStart);
                    FixupIndexComponent(dstRec, header, EBDATASTREAMCOMP_IDX2, baseVertexStart);
                    FixupIndexComponent(dstRec, header, EBDATASTREAMCOMP_IDX3, baseVertexStart);

                    TransformVectorFromComponents  (dstRec, header, EBDATASTREAMCOMP_FNX,  EBDATASTREAMCOMP_FNY,  EBDATASTREAMCOMP_FNZ,  localMatrix);

                    TransformPositionFromComponents(dstRec, header, EBDATASTREAMCOMP_V1X,  EBDATASTREAMCOMP_V1Y,  EBDATASTREAMCOMP_V1Z,  localMatrix);
                    TransformPositionFromComponents(dstRec, header, EBDATASTREAMCOMP_V2X,  EBDATASTREAMCOMP_V2Y,  EBDATASTREAMCOMP_V2Z,  localMatrix);
                    TransformPositionFromComponents(dstRec, header, EBDATASTREAMCOMP_V3X,  EBDATASTREAMCOMP_V3Y,  EBDATASTREAMCOMP_V3Z,  localMatrix);

                    TransformVectorFromComponents  (dstRec, header, EBDATASTREAMCOMP_V1NX, EBDATASTREAMCOMP_V1NY, EBDATASTREAMCOMP_V1NZ, localMatrix);
                    TransformVectorFromComponents  (dstRec, header, EBDATASTREAMCOMP_V2NX, EBDATASTREAMCOMP_V2NY, EBDATASTREAMCOMP_V2NZ, localMatrix);
                    TransformVectorFromComponents  (dstRec, header, EBDATASTREAMCOMP_V3NX, EBDATASTREAMCOMP_V3NY, EBDATASTREAMCOMP_V3NZ, localMatrix);
                }

                dst += recordSize;
            }
        }
    }

    TBActorNodeInstance *child = nodeInstance->children;
    if (child != NULL)
    {
        do
        {
            dst = GetActorGeometry(actorInstance, child, stream, dst, baseVertexPtr, flags);
            child = child->next;
        } while (child != nodeInstance->children);
    }

    return dst;
}

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
    TBActorNode         *rootNode;
    TBActorNode         *node;
    int                  totalNodeInstances = 0;
    TBActorNodeInstance *rootNodeInstance;
    TBActorNodeInstance *inst;
    TBActorNodeInstance *searchInst;
    TBActorNodeInstance *motionBone = NULL;
    void                **renderParmPtr;
    int                  i;

    /* Allocate instance if not provided */
    if (actorInstance == NULL)
    {
        unsigned int allocSize =
            (unsigned int)actor->matrixPaletteSize * 0x40u + 0x170u;

        actorInstance = (_TBActorInstance *)MALLOC(allocSize);
        if (actorInstance == NULL)
            return NULL;

        actorInstance->flags = 1;

        if (actor->matrixPaletteSize == 0)
        {
            actorInstance->matrixPalette = NULL;
        }
        else
        {
            /* matrix palette is embedded directly after the instance */
            actorInstance->matrixPalette = (float *)(actorInstance + 1);
        }
    }
    else
    {
        actorInstance->flags = 0;

        if (actor->matrixPaletteSize == 0)
        {
            actorInstance->matrixPalette = NULL;
        }
        else
        {
            float *palette = (float *)MALLOC((unsigned int)actor->matrixPaletteSize << 6);

            actorInstance->matrixPalette = palette;
            if (palette == NULL)
                return NULL;
        }
    }

    /* Initial transform */
    actorInstance->position[0] = 0.0f;
    actorInstance->position[1] = 0.0f;
    actorInstance->position[2] = 0.0f;

    actorInstance->scale[0] = 1.0f;
    actorInstance->scale[1] = 1.0f;
    actorInstance->scale[2] = 1.0f;

    actorInstance->orientation[0] = 0.0f;
    actorInstance->orientation[1] = 0.0f;
    actorInstance->orientation[2] = 0.0f;
    actorInstance->orientation[3] = 1.0f;

    actorInstance->actor = actor;

    /* Count total node instances (including children chains) */
    rootNode = actor->rootNode;
    node     = rootNode;
    totalNodeInstances = 0;

    do
    {
        totalNodeInstances++;

        if (node->children != NULL)
            totalNodeInstances += CountActorNodes(node->children);

        node = node->next;
    }
    while (node != rootNode);

    /* Allocate node instances + per-node queue area */
    {
        unsigned int perNodeSize =
            (unsigned int)queueSize * 0x40u + 0x160u;
        unsigned int totalSize = perNodeSize * (unsigned int)totalNodeInstances;

        rootNodeInstance = (_TBActorNodeInstance *)CALLOC(totalSize, 0);
    }

    actorInstance->rootNodeInstance = rootNodeInstance;

    /* Build node-instance ring and children chains */
    InitActorNodeInstances(actor->rootNode, rootNodeInstance, NULL, queueSize);

    /* Find "motion" bone by CRC of the node's CRC field */
    {
        uint32 motionCrc = bkStringCRC("motion");

        inst = actorInstance->rootNodeInstance;
        searchInst = inst;

        do
        {
            TBActorNode *n = searchInst->actorNode;

            if (n->crc == motionCrc)
            {
                motionBone = searchInst;
                break;
            }

            if (searchInst->children != NULL)
            {
                TBActorNodeInstance *found = baFindNodeByCRC(searchInst->children, motionCrc);
                if (found != NULL)
                {
                    motionBone = found;
                    break;
                }
            }

            searchInst = searchInst->next;
        }
        while (searchInst != inst);
    }

    actorInstance->motionBone = motionBone;

    /* Render modes / parameters */
    actorInstance->renderMode[0]  = BACTRENDERMODE_NORMAL;
    actorInstance->renderParms[0] = NULL;

    renderParmPtr = actorInstance->renderParms;
    for (i = 1; i < 3; ++i)
    {
        actorInstance->renderMode[i]  = BACTRENDERMODE_NONE;
        actorInstance->renderParms[i] = NULL;
    }

    /* Animation segments */
    actorInstance->animSegments   = actor->animSegments;
    actorInstance->numAnimSegments = actor->numAnimSegments;

    return actorInstance;
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
    bkHeapFree(actorInstance->rootNodeInstance);
    actorInstance->rootNodeInstance = NULL;

    if (actorInstance->flags & BACTORINSTFLAG_DYNAMIC)
    {
        bkHeapFree(actorInstance);
        return;
    }

    if (actorInstance->matrixPalette != NULL)
    {
        bkHeapFree(actorInstance->matrixPalette);
        actorInstance->matrixPalette = NULL;
    }
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
	TBActor *actor = actorInstance->actor;

	// translation from actorInstance->position
	TBMatrix transMat;
	bmMatTranslate(transMat, actorInstance->position[0], actorInstance->position[1], actorInstance->position[2]);

	// scale from actorInstance->scale
	TBMatrix scaleMat;
	bmMatScale(scaleMat, actorInstance->scale[0], actorInstance->scale[1], actorInstance->scale[2]);

	// orientation quaternion -> matrix
	bmQuatToMatrix(actorInstance->orientationMatrix, actorInstance->orientation);

	// objectToWorldNoScale = T * R
	bmMatMultiply(actorInstance->objectToWorldNoScale, transMat, actorInstance->orientationMatrix);

	// objectToWorld = objectToWorldNoScale * S
	bmMatMultiply(actorInstance->objectToWorld, actorInstance->objectToWorldNoScale, scaleMat);

	actorInstance->flags &= ~2;

	if (flags & 4)
	{
		int vis = bdTestBBVisibility(actorInstance->objectToWorld, actor->xMin, actor->xMax, actor->yMin, actor->yMax, actor->zMin, actor->zMax);
		actorInstance->bbVisible = (ushort)vis;
	}

	if (((actorInstance->bbVisible != 0) || ((flags & 6) == 0)) && (flags & 1))
	{
		TBVector parentScale;
		parentScale[0] = 1.0f;
		parentScale[1] = 1.0f;
		parentScale[2] = 1.0f;
		parentScale[3] = 0.0f;

		bPrepActorNodeList(actorInstance->rootNodeInstance, bIdentityMatrix, parentScale, actorInstance);
	}
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
    TBActor *actor = actorInstance->actor;

    if (flags & BACTORDRAWFLAG_TESTBB)
    {
        int vis = bdTestBBVisibility(
            actorInstance->objectToWorld,
            actor->xMin, actor->xMax,
            actor->yMin, actor->yMax,
            actor->zMin, actor->zMax);

        actorInstance->bbVisible = (ushort)vis;
    }

    if ((!actorInstance->bbVisible) && (flags & BACTORDRAWFLAG_EARLYOUT))
        return FAIL;

    if (!(actor->flags & TBACTORFLAG_SOFTSKIN))
    {
        bSetupActorNodeList(actorInstance->rootNodeInstance, actorInstance, alpha);
        return OK;
    }

    if (flags & BACTORDRAWFLAG_CPU)
    {
        bDrawSoftSkinCPU(actorInstance, alpha);
        return OK;
    }

    bDrawSoftSkin(actorInstance, alpha);
    return OK;
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
	TBActor *actor = (TBActor*)bkLoadFileByCRC(index, crc, NULL, NULL, NULL, NULL);

	if (!actor) {
		return (TBActor*)0;
	}

	actor->resInfo.packageId = index->id;

	uchar *basePtr = (uchar*)actor;

	if (actor->rootNode != NULL) {
		actor->rootNode = (TBActorNode*)(basePtr + (int)actor->rootNode);
	}

	if (actor->animSegments != NULL) {
		actor->animSegments = (TBActorAnimSegment*)(basePtr + (int)actor->animSegments);
	}

	if (actor->animEventData != NULL) {
		actor->animEventData = (char*)(basePtr + (int)actor->animEventData);
	}

	if (actor->animSegmentNames != NULL) {
		uint32 i;

		actor->animSegmentNames = (char*)(basePtr + (int)actor->animSegmentNames);

		if (actor->numAnimSegments != 0 && actor->animSegments != NULL) {
			TBActorAnimSegment *seg = actor->animSegments;

			for (i = 0; i < actor->numAnimSegments; ++i, ++seg) {
				if (seg->name != NULL) {
					seg->name = (char*)(basePtr + (int)seg->name);
				}
			}
		}
	}

	FixupActorNodeList((uchar*)actor, actor->rootNode, 0, actor);

	if (actor->flags & TBACTORFLAG_SOFTSKIN) {
		bFixupSoftSkin(actor);
	}

	return actor;
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
    DeleteActorNodeList(actor->rootNode);

    if (actor->flags & TBACTORFLAG_SOFTSKIN) {
        bDeleteSoftSkin(actor);
    }

    if ((int32)actor->resInfo.packageId32 >= 0) {
        bkHeapFree(actor);
    }
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
	if (nodeInstance == NULL)
	{
		TBActorNodeInstance *node = actorInstance->rootNodeInstance;
		if (node != NULL)
		{
			TBActorNodeInstance *root = actorInstance->rootNodeInstance;
			do
			{
				baSetNodeFrame(actorInstance, node, frame, recurse);
				node = node->next;
			}
			while (node != root);
		}
	}
	else
	{
		nodeInstance->targetFrame = frame;
		nodeInstance->blendVal    = 0.0f;

		if (nodeInstance->flags & 0x01)
		{
			baEvalAnimTrack3(
				nodeInstance->animState.position,
				nodeInstance->position,
				frame
			);

			((byte *)&nodeInstance->flags)[0] |= 0x40;
		}

		if (nodeInstance->flags & 0x04)
		{
			baEvalAnimTrack3(
				nodeInstance->animState.scale,
				nodeInstance->scale,
				frame
			);

			((byte *)&nodeInstance->flags)[1] |= 0x01;
		}

		if (nodeInstance->flags & 0x02)
		{
			baEvalAnimTrack4(
				nodeInstance->animState.orientation,
				nodeInstance->orientation,
				frame
			);

			((byte *)&nodeInstance->flags)[0] |= 0x80;
		}

		if (recurse && nodeInstance->children != NULL)
		{
			TBActorNodeInstance *child = nodeInstance->children;
			TBActorNodeInstance *root  = nodeInstance->children;

			do
			{
				baSetNodeFrame(actorInstance, child, frame, 1);
				child = child->next;
			}
			while (child != root);
		}
	}
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
    float local_20[4];
    float local_10[4];

    if (nodeInstance == NULL)
    {
        TBActorNodeInstance *it = actorInstance->rootNodeInstance;

        do
        {
            baSetNodeBlend(actorInstance, it, frame1, frame2, t, recurse);
            it = it->next;
        } while (it != actorInstance->rootNodeInstance);

        return;
    }

    nodeInstance->targetFrame = (int32)frame2;
    nodeInstance->blendVal = t;

    if (nodeInstance->flags & 1)
    {
        baEvalAnimTrack3(local_20, nodeInstance->position, (int32)frame1);
        baEvalAnimTrack3(local_10, nodeInstance->position, (int32)frame2);

        nodeInstance->flags |= 0x40;

        nodeInstance->animState.position[0] = (local_10[0] - local_20[0]) * t + local_20[0];
        nodeInstance->animState.position[1] = (local_10[1] - local_20[1]) * t + local_20[1];
        nodeInstance->animState.position[2] = (local_10[2] - local_20[2]) * t + local_20[2];
    }

    if (nodeInstance->flags & 4)
    {
        baEvalAnimTrack3(local_20, nodeInstance->scale, (int32)frame1);
        baEvalAnimTrack3(local_10, nodeInstance->scale, (int32)frame2);

        nodeInstance->flags |= BNODEINSTFLAG_SCALEDIRTY;

        nodeInstance->animState.scale[0] = (local_10[0] - local_20[0]) * t + local_20[0];
        nodeInstance->animState.scale[1] = (local_10[1] - local_20[1]) * t + local_20[1];
        nodeInstance->animState.scale[2] = (local_10[2] - local_20[2]) * t + local_20[2];
    }

    if (nodeInstance->flags & 2)
    {
        baEvalAnimTrack4(local_20, nodeInstance->orientation, (int32)frame1);
        baEvalAnimTrack4(local_10, nodeInstance->orientation, (int32)frame2);

        bmQuatSlerp(nodeInstance->animState.orientation, local_20, local_10, t);

        nodeInstance->flags |= 0x80;
    }

    if (recurse)
    {
        TBActorNodeInstance *child = nodeInstance->children;

        if (child)
        {
            TBActorNodeInstance *it = child;

            do
            {
                baSetNodeBlend(actorInstance, it, frame1, frame2, t, 1);
                it = it->next;
            } while (it != child);
        }
    }
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
    if (nodeInstance == NULL)
    {
        TBActorNodeInstance *rootNode = actorInstance->rootNodeInstance;
        TBActorNodeInstance *curNode = rootNode;

        do
        {
            baSetNodeBlendValue(actorInstance, curNode, t, recurse);
            curNode = curNode->next;
        } while (curNode != rootNode);

        return;
    }

    nodeInstance->blendVal = t;

    if ((nodeInstance->flags & 0x01) != 0)
    {
        float tmp3[3];

        baEvalAnimTrack3(tmp3, nodeInstance->position, nodeInstance->targetFrame);

        ((byte *)&nodeInstance->flags)[0] |= 0x40;

        nodeInstance->animState.position[0] =
            (tmp3[0] - nodeInstance->savedState.position[0]) * t + nodeInstance->savedState.position[0];
        nodeInstance->animState.position[1] =
            (tmp3[1] - nodeInstance->savedState.position[1]) * t + nodeInstance->savedState.position[1];
        nodeInstance->animState.position[2] =
            (tmp3[2] - nodeInstance->savedState.position[2]) * t + nodeInstance->savedState.position[2];
    }

    if ((nodeInstance->flags & 0x04) != 0)
    {
        float tmp3[3];

        baEvalAnimTrack3(tmp3, nodeInstance->scale, nodeInstance->targetFrame);

        ((byte *)&nodeInstance->flags)[1] |= 0x01;

        nodeInstance->animState.scale[0] =
            (tmp3[0] - nodeInstance->savedState.scale[0]) * t + nodeInstance->savedState.scale[0];
        nodeInstance->animState.scale[1] =
            (tmp3[1] - nodeInstance->savedState.scale[1]) * t + nodeInstance->savedState.scale[1];
        nodeInstance->animState.scale[2] =
            (tmp3[2] - nodeInstance->savedState.scale[2]) * t + nodeInstance->savedState.scale[2];
    }

    if ((nodeInstance->flags & 0x02) != 0)
    {
        float tmp4[4];

        baEvalAnimTrack4(tmp4, nodeInstance->orientation, nodeInstance->targetFrame);
        bmQuatSlerp(nodeInstance->animState.orientation, nodeInstance->savedState.orientation, tmp4, t);

        ((byte *)&nodeInstance->flags)[0] |= 0x80;
    }

    if (recurse != 0)
    {
        TBActorNodeInstance *childRoot = nodeInstance->children;
        if (childRoot != NULL)
        {
            TBActorNodeInstance *child = childRoot;
            do
            {
                baSetNodeBlendValue(actorInstance, child, t, 1);
                child = child->next;
            } while (child != childRoot);
        }
    }
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
    TBVector local_10;

    if (nodeInstance == NULL)
    {
        TBActorNodeInstance *it = actorInstance->rootNodeInstance;

        do
        {
            baSetNodeBlendTargetValue(actorInstance, it, frame, t, recurse);
            it = it->next;
        } while (it != actorInstance->rootNodeInstance);

        return;
    }

    nodeInstance->targetFrame = (int32)frame;
    nodeInstance->blendVal = t;

    /* savedState = animState (в точности как серия MOV-ов в дизасме) */
    nodeInstance->savedState.position[0] = nodeInstance->animState.position[0];
    nodeInstance->savedState.position[1] = nodeInstance->animState.position[1];
    nodeInstance->savedState.position[2] = nodeInstance->animState.position[2];
    nodeInstance->savedState.position[3] = nodeInstance->animState.position[3];

    nodeInstance->savedState.scale[0] = nodeInstance->animState.scale[0];
    nodeInstance->savedState.scale[1] = nodeInstance->animState.scale[1];
    nodeInstance->savedState.scale[2] = nodeInstance->animState.scale[2];
    nodeInstance->savedState.scale[3] = nodeInstance->animState.scale[3];

    nodeInstance->savedState.orientation[0] = nodeInstance->animState.orientation[0];
    nodeInstance->savedState.orientation[1] = nodeInstance->animState.orientation[1];
    nodeInstance->savedState.orientation[2] = nodeInstance->animState.orientation[2];
    nodeInstance->savedState.orientation[3] = nodeInstance->animState.orientation[3];

    if (nodeInstance->flags & BNODEINSTFLAG_POSKEYS)
    {
        baEvalAnimTrack3(local_10, nodeInstance->position, nodeInstance->targetFrame);

        nodeInstance->flags |= BNODEINSTFLAG_POSDIRTY;

        nodeInstance->animState.position[0] =
            (local_10[0] - nodeInstance->savedState.position[0]) * t + nodeInstance->savedState.position[0];
        nodeInstance->animState.position[1] =
            (local_10[1] - nodeInstance->savedState.position[1]) * t + nodeInstance->savedState.position[1];
        nodeInstance->animState.position[2] =
            (local_10[2] - nodeInstance->savedState.position[2]) * t + nodeInstance->savedState.position[2];
    }

    if (nodeInstance->flags & BNODEINSTFLAG_SCALEKEYS)
    {
        baEvalAnimTrack3(local_10, nodeInstance->scale, nodeInstance->targetFrame);

        nodeInstance->flags |= BNODEINSTFLAG_SCALEDIRTY;

        nodeInstance->animState.scale[0] =
            (local_10[0] - nodeInstance->savedState.scale[0]) * t + nodeInstance->savedState.scale[0];
        nodeInstance->animState.scale[1] =
            (local_10[1] - nodeInstance->savedState.scale[1]) * t + nodeInstance->savedState.scale[1];
        nodeInstance->animState.scale[2] =
            (local_10[2] - nodeInstance->savedState.scale[2]) * t + nodeInstance->savedState.scale[2];
    }

    if (nodeInstance->flags & BNODEINSTFLAG_ORIENTKEYS)
    {
        baEvalAnimTrack4(local_10, nodeInstance->orientation, nodeInstance->targetFrame);
        bmQuatSlerp(nodeInstance->animState.orientation, nodeInstance->savedState.orientation, local_10, t);

        nodeInstance->flags |= BNODEINSTFLAG_ORIENTDIRTY;
    }

    if (recurse)
    {
        TBActorNodeInstance *child = nodeInstance->children;

        if (child)
        {
            TBActorNodeInstance *it = child;

            do
            {
                baSetNodeBlendTargetValue(actorInstance, it, frame, t, 1);
                it = it->next;
            } while (it != child);
        }
    }
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
    TAnimUpdateCallbackInfo  callbackInfo[8]; // 8 * 0x20 = 0x100
    int                      nodesUpdatedCount = 0;

    // orientation -> matrix
    bmQuatToMatrix(actorInstance->orientationMatrix, actorInstance->orientation);

    const int callbackCount = bUpdateNodes(
        actorInstance,
        nodeInstance,
        timeDelta,
        motionVector,
        callbackInfo,
        0,
        &nodesUpdatedCount
    );

    for (int i = 0; i < callbackCount; ++i)
    {
        TAnimUpdateCallbackInfo &info = callbackInfo[i];

        int result = info.callback(
            info.reason,
            info.actorInstance,
            info.nodeInstance,
            info.animSegment,
            info.context
        );

        if (result != 0) return nodesUpdatedCount;
    }

    return nodesUpdatedCount;
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
	nodeInstance->savedState.position[0] = nodeInstance->animState.position[0];
	nodeInstance->savedState.position[1] = nodeInstance->animState.position[1];
	nodeInstance->savedState.position[2] = nodeInstance->animState.position[2];
	nodeInstance->savedState.position[3] = nodeInstance->animState.position[3];

	nodeInstance->savedState.scale[0] = nodeInstance->animState.scale[0];
	nodeInstance->savedState.scale[1] = nodeInstance->animState.scale[1];
	nodeInstance->savedState.scale[2] = nodeInstance->animState.scale[2];
	nodeInstance->savedState.scale[3] = nodeInstance->animState.scale[3];

	nodeInstance->savedState.orientation[0] = nodeInstance->animState.orientation[0];
	nodeInstance->savedState.orientation[1] = nodeInstance->animState.orientation[1];
	nodeInstance->savedState.orientation[2] = nodeInstance->animState.orientation[2];
	nodeInstance->savedState.orientation[3] = nodeInstance->animState.orientation[3];

	{
		int queueSize = nodeInstance->queueSize;

		if (queueSize != 0)
		{
			int index = ((int)nodeInstance->currentQueue + flushFrom) % queueSize;
			int i     = flushFrom;

			if (flushFrom < queueSize)
			{
				do
				{
					TBQueuedAnim *queue = nodeInstance->animQueue;

					if (queue[index].active)
					{
						if (queue[index].callback != NULL)
						{
							EBAnimTypes type = queue[index].type;
							void *context    = queue[index].context;
							TBActorAnimSegment *seg;

							if (type == BANIMTYPE_NORMAL || type == BANIMTYPE_BLENDSTATE)
							{
								seg = queue[index].animSeg1;
							}
							else if (type == BANIMTYPE_BLENDFRAMES)
							{
								seg = queue[index].animSeg2;
							}
							else
							{
								goto skip_callback;
							}

							queue[index].callback(
								BANIMQUEUEREASON_FLUSHED,
								actorInstance,
								nodeInstance,
								seg,
								context
							);
						}

					skip_callback:
						nodeInstance->animQueue[index].active = 0;
					}

					index = (index + 1) % queueSize;
					++i;

				} while (i < queueSize);
			}

			if (flushFrom == 0)
			{
				nodeInstance->currentQueue = 0;
			}
		}
	}

	if (flushChildren && nodeInstance->children)
	{
		TBActorNodeInstance *child = nodeInstance->children;

		do
		{
			if ((child->flags & 0x20) == 0)
			{
				baFlushNodeQueue(actorInstance, child, 1, flushFrom);
			}

			child = child->next;
		}
		while (child != nodeInstance->children);
	}
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
	dest->animSegments = source->animSegments;
	dest->numAnimSegments = source->numAnimSegments;
	bRetargetAnimNode(source->rootNode, dest->rootNodeInstance);
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
	actorInstance->renderMode[stage] = mode;
	actorInstance->renderParms[stage] = parms;
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
    TBMatrix temp;

	if (offset == NULL) {
		pos[0] = 0.0;
		pos[1] = 0.0;
		pos[2] = 0.0;
		pos[3] = 1.0;
	} else {
		pos[0] = offset[0];
		pos[1] = offset[1];
		pos[2] = offset[2];
		pos[3] = offset[3];
	}

	bmMatMultiply(temp, actorInstance->objectToWorld, nodeInstance->nodeToLocalWorld);
	bmMatMultiplyVector(temp, pos);
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
  uchar uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  _TBActorNodeInstance *node;
  
  if (((flags & 0x10) == 0) || (uVar1 = '\x01', nodeInstance != actorInstance->motionBone)) {
    uVar1 = animType;
  }
  if (uVar1 == '\x01') {
    uVar3 = (uint)nodeInstance->queueSize;
    uVar2 = (uint)nodeInstance->currentQueue;
    iVar4 = 0;
    if (uVar3 != 0) {
      do {
        if (nodeInstance->animQueue[uVar2].active == '\0') {
          nodeInstance->animQueue[uVar2].active = '\x01';
          nodeInstance->animQueue[uVar2].type = BANIMTYPE_NORMAL;
          nodeInstance->animQueue[uVar2].animSeg1 = anim1Seg;
          iVar4 = startFrame1;
          if (startFrame1 == -1) {
            iVar4 = anim1Seg->startFrame;
          }
          nodeInstance->animQueue[uVar2].frame1 = iVar4;
          iVar4 = anim1Delta;
          if ((anim1Delta == 0) && (iVar4 = anim1Seg->ticksPerFrame, iVar4 == 0)) {
            iVar4 = 0x400;
          }
          nodeInstance->animQueue[uVar2].deltaFrame1 = iVar4;
          nodeInstance->animQueue[uVar2].flags = (ushort)flags | 8;
          nodeInstance->animQueue[uVar2].callback = callback;
          nodeInstance->animQueue[uVar2].context = context;
          nodeInstance->animQueue[uVar2].repeatCount = repeatCount;
          break;
        }
        uVar2 = (int)(uVar2 + 1) % (int)uVar3;
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)uVar3);
    }
  }
  else if (uVar1 == '\x02') {
    uVar3 = (uint)nodeInstance->queueSize;
    uVar2 = (uint)nodeInstance->currentQueue;
    iVar4 = 0;
    if (uVar3 != 0) {
      do {
        if (nodeInstance->animQueue[uVar2].active == '\0') {
          nodeInstance->animQueue[uVar2].active = '\x01';
          nodeInstance->animQueue[uVar2].type = BANIMTYPE_BLENDFRAMES;
          nodeInstance->animQueue[uVar2].animSeg1 = anim1Seg;
          nodeInstance->animQueue[uVar2].animSeg2 = anim2Seg;
          iVar4 = startFrame1;
          if (startFrame1 == -1) {
            iVar4 = anim1Seg->startFrame;
          }
          nodeInstance->animQueue[uVar2].frame1 = iVar4;
          iVar4 = startFrame2;
          if (startFrame2 == -1) {
            iVar4 = anim2Seg->startFrame;
          }
          nodeInstance->animQueue[uVar2].frame2 = iVar4;
          iVar4 = anim1Delta;
          if ((anim1Delta == 0) && (iVar4 = anim1Seg->ticksPerFrame, iVar4 == 0)) {
            iVar4 = 0x400;
          }
          nodeInstance->animQueue[uVar2].deltaFrame1 = iVar4;
          iVar4 = anim2Delta;
          if ((anim2Delta == 0) && (iVar4 = anim2Seg->ticksPerFrame, iVar4 == 0)) {
            iVar4 = 0x400;
          }
          nodeInstance->animQueue[uVar2].deltaFrame2 = iVar4;
          goto LAB_00562585;
        }
        uVar2 = (int)(uVar2 + 1) % (int)uVar3;
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)uVar3);
    }
  }
  else if (uVar1 == '\x03') {
    uVar3 = (uint)nodeInstance->queueSize;
    uVar2 = (uint)nodeInstance->currentQueue;
    iVar4 = 0;
    if (uVar3 != 0) {
      do {
        if (nodeInstance->animQueue[uVar2].active == '\0') {
          nodeInstance->animQueue[uVar2].active = '\x01';
          nodeInstance->animQueue[uVar2].type = BANIMTYPE_BLENDSTATE;
          nodeInstance->animQueue[uVar2].animSeg1 = anim1Seg;
          iVar4 = startFrame1;
          if (startFrame1 == -1) {
            iVar4 = anim1Seg->startFrame;
          }
          nodeInstance->animQueue[uVar2].frame1 = iVar4;
          iVar4 = anim1Delta;
          if ((anim1Delta == 0) && (iVar4 = anim1Seg->ticksPerFrame, iVar4 == 0)) {
            iVar4 = 0x400;
          }
          nodeInstance->animQueue[uVar2].deltaFrame1 = iVar4;
LAB_00562585:
          nodeInstance->animQueue[uVar2].t = 0.0;
          nodeInstance->animQueue[uVar2].deltaT = deltaT;
          nodeInstance->animQueue[uVar2].flags = (ushort)flags | 8;
          nodeInstance->animQueue[uVar2].callback = callback;
          nodeInstance->animQueue[uVar2].context = context;
          nodeInstance->animQueue[uVar2].repeatCount = repeatCount;
          break;
        }
        uVar2 = (int)(uVar2 + 1) % (int)uVar3;
        iVar4 = iVar4 + 1;
      } while (iVar4 < (int)uVar3);
    }
  }
  node = nodeInstance->children;
  if (node != (_TBActorNodeInstance *)0x0) {
    do {
      if ((node->flags & 0x20) == 0) {
        baQueueNodeAnim(actorInstance,node,animType,anim1Seg,anim2Seg,anim1Delta,anim2Delta,
                        startFrame1,startFrame2,deltaT,flags | 4,callback,context,repeatCount);
      }
      node = node->next;
    } while (node != nodeInstance->children);
  }
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
  TBActorNodeInstance *node;
  
  if (nodeInstance == (_TBActorNodeInstance *)0x0) {
    node = actorInstance->rootNodeInstance;
    do {
      if ((node->flags & 0x20) == 0) {
        baQueueNodeAnim(actorInstance,node,'\x01',animSeg,(_TBActorAnimSegment *)0x0,animDelta,0,
                        startFrame,-1,0.0,flags,callback,context,repeatCount);
      }
      node = node->next;
    } while (node != actorInstance->rootNodeInstance);
    return;
  }
  baQueueNodeAnim(actorInstance,nodeInstance,'\x01',animSeg,(_TBActorAnimSegment *)0x0,animDelta,0,
                  startFrame,-1,0.0,flags,callback,context,repeatCount);
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
  _TBActorNodeInstance *node;
  
  if (nodeInstance == (_TBActorNodeInstance *)0x0) {
    node = actorInstance->rootNodeInstance;
    do {
      if ((node->flags & 0x20) == 0) {
        baQueueNodeAnim(actorInstance,node,'\x02',anim1Seg,anim2Seg,anim1Delta,anim2Delta,
                        startFrame1,startFrame2,deltaT,flags,callback,context,repeatCount);
      }
      node = node->next;
    } while (node != actorInstance->rootNodeInstance);
    return;
  }
  baQueueNodeAnim(actorInstance,nodeInstance,'\x02',anim1Seg,anim2Seg,anim1Delta,anim2Delta,
                  startFrame1,startFrame2,deltaT,flags,callback,context,repeatCount);
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
  _TBActorNodeInstance *node;
  
  if (nodeInstance == (_TBActorNodeInstance *)0x0) {
    node = actorInstance->rootNodeInstance;
    do {
      if ((node->flags & 0x20) == 0) {
        baQueueNodeAnim(actorInstance,node,'\x03',animSeg,(_TBActorAnimSegment *)0x0,animDelta,0,
                        startFrame,-1,deltaT,flags,callback,context,repeatCount);
      }
      node = node->next;
    } while (node != actorInstance->rootNodeInstance);
    return;
  }
  baQueueNodeAnim(actorInstance,nodeInstance,'\x03',animSeg,(_TBActorAnimSegment *)0x0,animDelta,0,
                  startFrame,-1,deltaT,flags,callback,context,repeatCount);
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
    TBActorNodeInstance *inst = nodeInstance;

    do
    {
        TBActorNode *node = inst->actorNode;

        if (node->crc == crc)
            return inst;

        if (inst->children != NULL)
        {
            _TBActorNodeInstance *found = baFindNodeByCRC(inst->children, crc);
            if (found != NULL)
                return found;
        }

        inst = inst->next;

    } while (inst != nodeInstance);

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
    uint32 crc = bkStringCRC(nodeName);
    TBActorNodeInstance *root    = nodeInstance;
    TBActorNodeInstance *current = nodeInstance;

    for (;;)
    {
        TBActorNode *actorNode = current->actorNode;

        if (actorNode->crc == crc)
        {
            return current;
        }

        if (current->children != NULL)
        {
            TBActorNodeInstance *found = baFindNodeByCRC(current->children, crc);
            if (found != NULL)
            {
                return found;
            }
        }

        current = current->next;
        if (current == root)
        {
            return NULL;
        }
    }
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
  uint uVar1;
  _TBActorAnimSegment *p_Var2;
  
  uVar1 = 0;
  if (actorInstance->numAnimSegments != 0) {
    p_Var2 = actorInstance->animSegments;
    do {
      if (p_Var2->crc == crc) {
        return actorInstance->animSegments + uVar1;
      }
      uVar1 = uVar1 + 1;
      p_Var2 = p_Var2 + 1;
    } while (uVar1 < actorInstance->numAnimSegments);
  }
  return (_TBActorAnimSegment *)0x0;
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
  uint32 crc;
  uint i;
  _TBActorAnimSegment *seg;
  
  crc = bkStringCRC(segName);
  i = 0;
  if (actorInstance->numAnimSegments != 0) {
    seg = actorInstance->animSegments;
    do {
      if (seg->crc == crc) {
        return actorInstance->animSegments + i;
      }
      i = i + 1;
      seg = seg + 1;
    } while (i < actorInstance->numAnimSegments);
  }
  return (_TBActorAnimSegment *)0x0;
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
	bGlobalActorAlphaScale = newAlphaScale;
	bSetGlobalAlpha(newAlphaScale);
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
	TBMatrix combinedMat;
	TBMatrix worldToBase;
	TBVector localFocusPoint;
	TBVector delta;
	TBVector lookDir;
	TBVector axis;
	TBVector rotation;
	TBQuaternion targetQuat;
	TBQuaternion blendedQuat;
	float dot;
	float angle;

	if (nodeInstance->parent != NULL) {
		bmMatMultiply(combinedMat, actorInstance->objectToWorldNoScale, nodeInstance->parent->nodeToLocalWorld);
		bmMatInverse(worldToBase, combinedMat);
	} else {
		bmMatInverse(worldToBase, actorInstance->objectToWorldNoScale);
	}

	bmMatMultiplyVector2(localFocusPoint, worldToBase, worldFocusPoint);

	delta[0] = localFocusPoint[0] - nodeInstance->animState.position[0];
	delta[1] = localFocusPoint[1] - nodeInstance->animState.position[1];
	delta[2] = localFocusPoint[2] - nodeInstance->animState.position[2];
	delta[3] = 0.0f;

	bmVectorNorm(lookDir, delta);

	// axis = lookDir x refVector
	axis[0] = lookDir[1] * refVector[2] - lookDir[2] * refVector[1];
	axis[1] = lookDir[2] * refVector[0] - lookDir[0] * refVector[2];
	axis[2] = lookDir[0] * refVector[1] - lookDir[1] * refVector[0];
	axis[3] = 0.0f;

	rotation[3] = 1.0f;
	bmVectorNorm(rotation, axis);

	dot = lookDir[0] * refVector[0] + lookDir[1] * refVector[1] + lookDir[2] * refVector[2];
	angle = bmACos(dot);
	rotation[3] = angle * maxAngleFactor;

	bmRotationToQuat(targetQuat, rotation);
	bmQuatSlerp(blendedQuat, nodeInstance->animState.orientation, targetQuat, t);

	nodeInstance->animState.orientation[0] = blendedQuat[0];
	nodeInstance->animState.orientation[1] = blendedQuat[1];
	nodeInstance->animState.orientation[2] = blendedQuat[2];
	nodeInstance->animState.orientation[3] = blendedQuat[3];

	nodeInstance->flags |= BNODEINSTFLAG_ORIENTDIRTY;

	if ((flags & BNODELOOKATUC_PREP) != 0) {
		PrepActorNode(actorInstance, nodeInstance, (flags >> 1) & 1);
	}
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
	int32 localVerts = 0;
	int32 localTris = 0;

	TBStoredGeometry *stored = NULL;

	if (rootNode == NULL)
	{
		TBActorNodeInstance *n = actorInstance->rootNodeInstance;
		if (n)
		{
			do
			{
				stored = CountActorGeometry(actorInstance, n, stream, &localVerts, &localTris);
				n = n->next;
			} while (n != actorInstance->rootNodeInstance);
		}
	}
	else
	{
		stored = CountActorGeometry(actorInstance, rootNode, stream, &localVerts, &localTris);
	}

	// compute buffer bytes (EDI in disasm)
	uint32 bytes = 0;
	TBStoredGeometryStreamHeader *hdr = NULL;

	if (stored)
	{
		float *streamPtr = stored->dataStreams[stream];
		if (streamPtr)
		{
			hdr = (TBStoredGeometryStreamHeader *)streamPtr;
			if (hdr->streamType == 0)
				bytes = (uint32)hdr->recordSize * (uint32)localVerts;
			else
				bytes = (uint32)hdr->recordSize * (uint32)localTris;
		}
	}

	if (noofVerticesPtr)
		*noofVerticesPtr = localVerts;
	if (noofTrisPtr)
		*noofTrisPtr = localTris;
	if (bufSize)
		*bufSize = (int32)bytes;

	// disasm: if stored == NULL or (flags&1) or hdr == NULL -> return NULL
	if (!stored || (flags & BGETACTGEOMETRY_COUNTONLY) || !hdr)
		return NULL;

	char *dst = (char *)buffer;
	if (!dst)
		dst = (char *)MALLOC(bytes);

	if (!dst)
		return NULL;

	if (flags & BGETACTGEOMETRY_ZEROPAD)
	{
		uint32 dwords = bytes >> 2;
		uint32 *p32 = (uint32 *)dst;
		while (dwords--)
			*p32++ = 0;

		uint32 tail = bytes & 3;
		uchar *p8 = (uchar *)p32;
		while (tail--)
			*p8++ = 0;
	}

	int32 baseVertex = 0;

	if (rootNode != NULL)
	{
		(void)GetActorGeometry(actorInstance, rootNode, stream, dst, &baseVertex, flags);
		return dst;
	}

	TBActorNodeInstance *n = actorInstance->rootNodeInstance;
	char *p = dst;

	if (n)
	{
		do
		{
			p = GetActorGeometry(actorInstance, n, stream, p, &baseVertex, flags);
			n = n->next;
		} while (n != actorInstance->rootNodeInstance);
	}

	return dst;
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
  _TBActorNodeInstance *node;
  
  if (nodeInstance != (_TBActorNodeInstance *)0x0) {
    nodeInstance->flags = ~(ushort)flagMask & nodeInstance->flags | (ushort)flagValues;
    if ((recurse != 0) && (node = nodeInstance->children, node != (_TBActorNodeInstance *)0x0)) {
      do {
        baSetActorNodeFlags(actorInstance,node,flagValues,flagMask,1);
        node = node->next;
      } while (node != nodeInstance->children);
    }
    return;
  }
  node = actorInstance->rootNodeInstance;
  do {
    baSetActorNodeFlags(actorInstance,node,flagValues,flagMask,recurse);
    node = node->next;
  } while (node != actorInstance->rootNodeInstance);
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
    if (nodeInstance == 0)
    {
        TBActorNodeInstance *node = actorInstance->rootNodeInstance;
        while (baIsActorAnimPlaying(actorInstance, node, recurse, anim) == 0)
        {
            node = node->next;
            if (node == actorInstance->rootNodeInstance)
                return 0;
        }
        return 1;
    }

    // queued = node->animQueue + node->currentQueue (stride 0x40)
    TBQueuedAnim *queued = (TBQueuedAnim *)((char *)nodeInstance->animQueue +
                                            ((unsigned int)nodeInstance->currentQueue << 6));

    if (queued->active == 0)
    {
        if (recurse != 0)
        {
            TBActorNodeInstance *child = nodeInstance->children;
            if (child != 0)
            {
                do
                {
                    if (baIsActorAnimPlaying(actorInstance, child, 1, anim) != 0)
                        return 1;
                    child = child->next;
                } while (child != nodeInstance->children);
            }
        }
        return 0;
    }

    if (anim != 0)
    {
        if (queued->type == BANIMTYPE_BLENDFRAMES)
            *anim = queued->animSeg2;
        else
            *anim = queued->animSeg1;
    }

    return 1;
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
    TBActorNodeInstance *it;

    // If nodeInstance is NULL, apply to every root node (circular list).
    if (nodeInstance == NULL) {
        it = actorInstance->rootNodeInstance;
        do {
            if ((it->flags & 0x20) == 0) {
                baQueueNodeAnimBlendToNode(actorInstance, it, srcNodeInstance, tDelta, flags, callback, context);
            }
            it = it->next;
        } while (it != actorInstance->rootNodeInstance);
        return;
    }

    // Find a free slot in the queue, starting from currentQueue.
    if (nodeInstance->queueSize != 0) {
        uint32 queueSize = (uint32)nodeInstance->queueSize;
        uint32 idx = (uint32)nodeInstance->currentQueue;
        uint32 tries = 0;

        do {
            TBQueuedAnim *qa = &nodeInstance->animQueue[idx];
            if (qa->active == 0) {
                qa->active = 1;
                qa->type = BANIMTYPE_COPYQUEUE;
                qa->srcNodeInstance = srcNodeInstance;
                qa->flags = (ushort)(flags | 0x8);
                qa->callback = callback;
                qa->context = context;
                qa->t = 0.0f;
                qa->deltaT = tDelta;
                break;
            }
            idx = (idx + 1) % queueSize;
            tries++;
        } while (tries < queueSize);
    }

    // Recurse into children (circular list), adding flag 0x4 to indicate recursion.
    it = nodeInstance->children;
    if (it != NULL) {
        TBActorNodeInstance *child = it;
        do {
            if ((child->flags & 0x20) == 0) {
                baQueueNodeAnimBlendToNode(actorInstance, child, srcNodeInstance, tDelta, flags | 0x4, callback, context);
            }
            child = child->next;
        } while (child != it);
    }
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
    if (srcNodeInstance == NULL)
    {
        TBActorNodeInstance *srcIt = srcActorInstance->rootNodeInstance;
        TBActorNodeInstance *dstIt = destActorInstance->rootNodeInstance;

        do
        {
            baCopyAnimQueue(destActorInstance, dstIt, srcActorInstance, srcIt, recurse);
            srcIt = srcIt->next;
            dstIt = dstIt->next;
        } while (srcIt != srcActorInstance->rootNodeInstance);

        return;
    }

    {
        uint32 bytes = (uint32)srcNodeInstance->queueSize * (uint32)sizeof(TBQueuedAnim);
        memcpy(destNodeInstance->animQueue, srcNodeInstance->animQueue, bytes);
    }

    destNodeInstance->currentQueue = srcNodeInstance->currentQueue;

    memcpy(&destNodeInstance->animState,  &srcNodeInstance->animState,  sizeof(TBAnimState));
    memcpy(&destNodeInstance->savedState, &srcNodeInstance->savedState, sizeof(TBAnimState));

    destNodeInstance->blendVal    = srcNodeInstance->blendVal;
    destNodeInstance->targetFrame = srcNodeInstance->targetFrame;

    if ((recurse != 0) && (srcNodeInstance->children != NULL))
    {
        TBActorNodeInstance *srcCh = srcNodeInstance->children;
        TBActorNodeInstance *dstCh = destNodeInstance->children;

        do
        {
            baCopyAnimQueue(destActorInstance, dstCh, srcActorInstance, srcCh, 1);
            srcCh = srcCh->next;
            dstCh = dstCh->next;
        } while (srcCh != srcNodeInstance->children);
    }
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
