// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : footalign.cpp
//   Purpose : routines to align Taz's feet to the ground
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "global.h"
#include "actors.h"
#include "footalign.h"
#include "maths.h"
#include "physics.h"

float	GetAngleAboveHorizontalFromVectors(TBVector src, TBVector dest)
{
	TBVector	v1,v2;
	float	xzDist,yDist;

	bmVectorCopy(v1, src);
	bmVectorCopy(v2, dest);

	yDist = v2[Y] - v1[Y];

	v1[Y] = v2[Y] = 0.0f;

	xzDist = bmVectorDistance(v1, v2);

	return (bmATan2(yDist, xzDist));
}
/* --------------------------------------------------------------------------------
   Function : AlignNodeWithPolys
   Purpose : Align a node with the supplied polys
   Parameters : actorInstance containing nodes, nodeName to affect, TBVector poly verts, TBVector list of poly normals, num of normals
   Returns : TRUE if node was aligned, FALSE if node could not be aligned
   Info : 
*/

float	xmin = -HALFPI;
float	xmax = HALFPI;
float	xoffset = 0.0f;

float	ymin = -HALFPI;
float	ymax = HALFPI;
float	yoffset = 0.0f;

void AlignFeet(ACTORINSTANCE *actorInstance)
{
	return;
	AlignNodeWithPolys(&actorInstance->actorInstance,"calfright",actorInstance->actorBehaviour->physics.lastAverageNormal, yoffset);
	AlignNodeWithPolys(&actorInstance->actorInstance,"calfleft",actorInstance->actorBehaviour->physics.lastAverageNormal, xoffset);
}

bool AlignNodeWithPolys(TBActorInstance *actorInstance, char *nodeName, TBVector normals, float offset)
{
	TBVector			avNormal, nodePos, tipPos, lookAt, tempVector;
	TBMatrix			tempMatrix;
	TBActorNodeInstance	*node;
	int					i;


	baPrepActorInstance(actorInstance, TRUE);

	// TP: find node
	node = baFindNode(actorInstance->rootNodeInstance, nodeName);
	if (!node) return FALSE;

	// TP: Find point to look at
	baGetNodesWorldPosition(actorInstance, node, nodePos);

	bmVectorSet(lookAt, 0.0f, 0.0f, METERTOUNIT(1.f), 1.0f);
	float yang = Aabs(-GetAngleAroundYFromQuat(actorInstance->orientation)+PI+offset);
	bmVectorRotateY(lookAt, lookAt, yang);

	ClipVector(lookAt, lookAt, normals);

	bmVectorAdd(lookAt, lookAt, nodePos);

	// TP: adjust node in x axis only
	TBQuaternion	orientation;
	bmQuatXRotation(orientation, GetAngleAboveHorizontalFromVectors(nodePos, lookAt));
	
	baSetAdditionalNodeOrientation(node, orientation);

	return TRUE;
}


