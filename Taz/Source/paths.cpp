// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : paths.cpp
//   Purpose : functions to aid in the movement of obects and characters along paths
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collisioncache.h"
#include "mapfile.h"
#include "paths.h"
#include "main.h"
#include "status.h"
#include "maths.h"
#include "physics.h"
#include "scene.h"
#include "trigger.h"

PATHLIST pathList;

int			numPosKeys;
int			numRotKeys;
int			numScaleKeys;

PATHNODE	*posKeys;
PATHNODE	*rotKeys;
PATHNODE	*scaleKeys;

PATHNODE	*newKeys;

float		*point;


int KeyCount(CPMAttrib *attrib,void *context)
{
	if ((strcmpi(attrib->ident,"FrameNum")==0)||(strcmpi(attrib->ident,"FrNum")==0))
	{
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int PositionKeyFrame(CPMAttrib *attrib,void *context)
{
	if ((strcmpi(attrib->ident,"FrameNum")==0)||(strcmpi(attrib->ident,"FrNum")==0))
	{
		posKeys[((int*)context)[0]].frame = attrib->ival;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int PositionKeyX(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"X")==0)
	{
		posKeys[((int*)context)[0]].position[X] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int PositionKeyY(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Y")==0)
	{
		posKeys[((int*)context)[0]].position[Y] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int PositionKeyZ(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Z")==0)
	{
		posKeys[((int*)context)[0]].position[Z] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int ScaleKeyFrame(CPMAttrib *attrib,void *context)
{
	if ((strcmpi(attrib->ident,"FrameNum")==0)||(strcmpi(attrib->ident,"FrNum")==0))
	{
		scaleKeys[((int*)context)[0]].frame = attrib->ival;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int ScaleKeyX(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"X")==0)
	{
		scaleKeys[((int*)context)[0]].scale[X] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int ScaleKeyY(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Y")==0)
	{
		scaleKeys[((int*)context)[0]].scale[Y] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int ScaleKeyZ(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Z")==0)
	{
		scaleKeys[((int*)context)[0]].scale[Z] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int RotKeyFrame(CPMAttrib *attrib,void *context)
{
	if ((strcmpi(attrib->ident,"FrameNum")==0)||(strcmpi(attrib->ident,"FrNum")==0))
	{
		rotKeys[((int*)context)[0]].frame = attrib->ival;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int RotKeyX(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"X")==0)
	{
		rotKeys[((int*)context)[0]].orientation[X] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int RotKeyY(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Y")==0)
	{
		rotKeys[((int*)context)[0]].orientation[Y] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int RotKeyZ(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"Z")==0)
	{
		rotKeys[((int*)context)[0]].orientation[Z] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int RotKeyW(CPMAttrib *attrib,void *context)
{
	if (strcmpi(attrib->ident,"W")==0)
	{
		rotKeys[((int*)context)[0]].orientation[W] = attrib->fval;
		((int *)context)[0]++;
	}
	return PMENUM_CONTINUE;
}

int CollateAnimStream(CPMNode *node, void *context)
{
	char *string;
	int	numberAttribs;


	string = node->GetAttrib("Name");

	if ((strcmpi(string,"position")==0)||(strcmpi(string,"pos")==0))
	{
		// position
		numberAttribs = 0;
		node->EnumAttribs(KeyCount,&numberAttribs);
		if (numberAttribs)
		{
			posKeys = (PATHNODE*)ZALLOC(numberAttribs*sizeof(PATHNODE));
			ASSERT(posKeys);

			numPosKeys = numberAttribs;

			numberAttribs = 0;
			node->EnumAttribs(PositionKeyFrame,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(PositionKeyX,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(PositionKeyY,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(PositionKeyZ,&numberAttribs);
		}
	}
	if ((strcmpi(string,"rotation")==0)||(strcmpi(string,"rot")==0))
	{
		// rotation
		numberAttribs = 0;
		node->EnumAttribs(KeyCount,&numberAttribs);
		if (numberAttribs)
		{
			rotKeys = (PATHNODE*)ZALLOC(numberAttribs*sizeof(PATHNODE));
			ASSERT(rotKeys);

			numRotKeys = numberAttribs;

			numberAttribs = 0;
			node->EnumAttribs(RotKeyFrame,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(RotKeyX,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(RotKeyY,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(RotKeyZ,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(RotKeyW,&numberAttribs);
		}
	}
	if ((strcmpi(string,"scale")==0)||(strcmpi(string,"scl")==0))
	{
		// scale
		numberAttribs = 0;
		node->EnumAttribs(KeyCount,&numberAttribs);
		if (numberAttribs)
		{
			scaleKeys = (PATHNODE*)ZALLOC(numberAttribs*sizeof(PATHNODE));
			ASSERT(scaleKeys);

			numScaleKeys = numberAttribs;

			numberAttribs = 0;
			node->EnumAttribs(ScaleKeyFrame,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(ScaleKeyX,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(ScaleKeyY,&numberAttribs);
			numberAttribs = 0;
			node->EnumAttribs(ScaleKeyZ,&numberAttribs);
		}
	}
	return PMENUM_CONTINUE;
}


void CreateSpecialTypePath(CPMNode *node, MAPOBJECTATTRIBS *attribs,TBVector position,TBQuaternion quat, TBVector scale)
{
	int		i,numNewKeys,tempInt,j;
	char	change;
	int32	frame,to,from;
	float	slerp;
	PATH	*path;

	posKeys = NULL;
	scaleKeys = NULL;
	rotKeys = NULL;
	numPosKeys = 0;
	numRotKeys = 0;
	numScaleKeys = 0;

	node->EnumNodes(CollateAnimStream, NULL, "AnimStream");

	if ((numPosKeys+numScaleKeys+numRotKeys)==0)
	{
		bkPrintf("      *** WARNING *** No key frames found on path.\n");
 		return;
	}

	newKeys = (PATHNODE*)ZALLOC((numPosKeys+numScaleKeys+numRotKeys)*sizeof(PATHNODE));
	ASSERT(newKeys);
	
	numNewKeys = 0;
	for (i=0;i<numPosKeys;i++)
	{
		newKeys[numNewKeys++].frame = posKeys[i].frame;
	}
	for (i=0;i<numScaleKeys;i++)
	{
		newKeys[numNewKeys++].frame = scaleKeys[i].frame;
	}
	for (i=0;i<numRotKeys;i++)
	{
		newKeys[numNewKeys++].frame = rotKeys[i].frame;
	}

	change = TRUE;
	while(change)
	{
		change = FALSE;
		for (i=1;i<numNewKeys;i++)
		{
			if (newKeys[i].frame < newKeys[i-1].frame)
			{
				change = TRUE;
				tempInt = bmFloatToInt(newKeys[i].frame);
				newKeys[i].frame = newKeys[i-1].frame;
				newKeys[i-1].frame = tempInt;
			}
		}
	}

	// PP: check for newKeys[0].frame < 0
	if(newKeys[0].frame < 0)
	{
		newKeys[0].frame=0;
	}

	// remove duplicates
	for (i=1;i<numNewKeys;i++)
	{
		if ((newKeys[i].frame == newKeys[i-1].frame)||(newKeys[i].frame<0))
		{
			for (j=i;j<(numNewKeys-1);j++)
			{
				newKeys[j].frame = newKeys[j+1].frame;
			}
			numNewKeys--;
			i--;
		}
	}
	
	// create final set of keys
	for (i=0;i<numNewKeys;i++)
	{
		frame = (int32)(newKeys[i].frame);

		to = from = -1;
		if (posKeys)
		{	
			for (j=0;j<numPosKeys;j++)
			{
				if (posKeys[j].frame<=frame)
				{
					from = j;
				}
				if (posKeys[j].frame>=frame)
				{
					to = j;
					break;
				}
			}	
			if (to==-1)
			{
				if (from==-1)
				{
					bmVectorCopy(newKeys[i].position,position);
					newKeys[i].position[W] = 1.0f;
				}
				else
				{
					bmVectorCopy(newKeys[i].position,posKeys[from].position);
					newKeys[i].position[W] = 1.0f;
				}
			}
			else
			{
				if (from==-1)
				{
					bmVectorCopy(newKeys[i].position,posKeys[to].position);
					newKeys[i].position[W] = 1.0f;
				}
				else
				{
					if (to==from)
					{
						bmVectorCopy(newKeys[i].position,posKeys[from].position);
						newKeys[i].position[W] = 1.0f;
					}
					else
					{
						slerp = ((float)frame - (float)posKeys[from].frame)/((float)posKeys[to].frame - (float)posKeys[from].frame);
						bmVectorLerp(newKeys[i].position,posKeys[from].position,posKeys[to].position,slerp);
						newKeys[i].position[W] = 1.0f;
					}
				}
			}
		}
		else
		{
			// set pos to be object position
			bmVectorCopy(newKeys[i].position,position);
			newKeys[i].position[W] = 1.0f;
		}

		to = from = -1;
		if (scaleKeys)
		{
			for (j=0;j<numScaleKeys;j++)
			{
				if (scaleKeys[j].frame<=frame)
				{
					from = j;
				}
				if (scaleKeys[j].frame>=frame)
				{
					to = j;
					break;
				}
			}
			if (to==-1)
			{
				if (from==-1)
				{
					bmVectorCopy(newKeys[i].scale,scale);
					newKeys[i].scale[W] = 1.0f;
				}
				else
				{
					bmVectorCopy(newKeys[i].scale,scaleKeys[from].scale);
					newKeys[i].scale[W] = 1.0f;
				}
			}
			else
			{
				if (from==-1)
				{
					bmVectorCopy(newKeys[i].scale,scaleKeys[to].scale);
					newKeys[i].scale[W] = 1.0f;
				}
				else
				{
					if (to==from)
					{
						bmVectorCopy(newKeys[i].scale,scaleKeys[from].scale);
						newKeys[i].scale[W] = 1.0f;
					}
					else
					{
						slerp = ((float)frame - (float)scaleKeys[from].frame)/((float)scaleKeys[to].frame - (float)scaleKeys[from].frame);
						bmVectorLerp(newKeys[i].scale,scaleKeys[from].scale,scaleKeys[to].scale,slerp);
						newKeys[i].scale[W] = 1.0f;
					}
				}
			}
		}
		else
		{
			// set scale to be object scale
			bmVectorCopy(newKeys[i].scale,scale);
			newKeys[i].scale[W] = 1.0f;
		}

		to = from = -1;
		if (rotKeys)
		{
			for (j=0;j<numRotKeys;j++)
			{
				if (rotKeys[j].frame<=frame)
				{
					from = j;
				}
				if (rotKeys[j].frame>=frame)
				{
					to = j;
					break;
				}
			}
			if (to==-1)
			{
				if (from==-1)
				{
					bmQuatCopy(newKeys[i].orientation,quat);
				}
				else
				{
					bmQuatCopy(newKeys[i].orientation,rotKeys[from].orientation);
				}
			}
			else
			{
				if (from==-1)
				{
					bmQuatCopy(newKeys[i].orientation,rotKeys[to].orientation);
				}
				else
				{
					if (to==from)
					{
						bmQuatCopy(newKeys[i].orientation,rotKeys[from].orientation);
					}
					else
					{
						slerp = ((float)frame - (float)rotKeys[from].frame)/((float)rotKeys[to].frame - (float)rotKeys[from].frame);
						bmQuatSlerp(newKeys[i].orientation,rotKeys[from].orientation,rotKeys[to].orientation,slerp);
					}
				}
			}
		}
		else
		{
			// set rot to be object rotation
			bmQuatCopy(newKeys[i].orientation,quat);
		}
	}

	bkHeapCheck();
	// got final set of keys
	if (posKeys) SAFE_FREE(posKeys);
	if (scaleKeys) SAFE_FREE(scaleKeys);
	if (rotKeys) SAFE_FREE(rotKeys);

	if (numNewKeys)
	{
		path = (PATH*)ZALLOC(sizeof(PATH));
		ASSERT(path);

		CreatePath(path,numNewKeys);
		for (i=0;i<numNewKeys;i++)
		{
			SetPathNodeLocation(path,i,newKeys[i].position,newKeys[i].scale,newKeys[i].orientation,(int)(newKeys[i].frame));
		}
		AddPathToList(path,attribs->name);
	}
	else
	{
		bkPrintf("      *** WARNING *** No key frames found on path.\n");
	}
	if (newKeys) SAFE_FREE(newKeys);

}

/*	--------------------------------------------------------------------------------
	Function 	: InitialisePathList
	Purpose 	: Initialise the path list
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void InitialisePathList(void)
{
	pathList.numEntries = 0;
	pathList.head.next = pathList.head.prev = &pathList.head;
}

/*	--------------------------------------------------------------------------------
	Function 	: DestroyPathList
	Purpose 	: Remove all paths and destroy path list
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void DestroyPathList(void)
{
	PATHLISTENTRY	*ptr,*temp;

	for (ptr = pathList.head.next;ptr != &pathList.head;)
	{
		if (ptr->path)
		{
			RemovePathFromList(ptr);

			DestroyPath(ptr->path);
			SAFE_FREE(ptr->path);

			temp = ptr->next;
			SAFE_FREE(ptr);
			ptr = temp;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: AddPathToList
	Purpose 	: Add a path to the path list
	Parameters 	: PATH, name
	Returns 	: 
	Info 		: 
*/
void AddPathToList(PATH *path,char *name)
{
	PATHLISTENTRY *ptr;
	PATHLISTENTRY *pathListEntry;

	ptr = pathList.head.next;

	pathListEntry = (PATHLISTENTRY*)ZALLOC(sizeof(PATHLISTENTRY));
	ASSERT(pathListEntry);

	pathListEntry->path = path;
	if (strlen(name) < 32)
	{
		strcpy(pathListEntry->name,name);
	}
	else
	{
		STRNCPY(pathListEntry->name,name,32);
	}
	pathListEntry->crc = bkCRC32((uchar*)name,strlen(name),0);

	pathListEntry->next = ptr;
	pathListEntry->prev = ptr->prev;
	ptr->prev->next = pathListEntry;
	ptr->prev = pathListEntry;
	pathList.numEntries++;
}

/*	--------------------------------------------------------------------------------
	Function 	: RemovePathFromList
	Purpose 	: Remove a path from the path list
	Parameters 	: PATH
	Returns 	: 
	Info 		: 
*/
void RemovePathFromList(PATHLISTENTRY *path)
{
	path->prev->next = path->next;
	path->next->prev = path->prev;
	pathList.numEntries--;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindPathInList
	Purpose 	: Find a path using name or crc
	Parameters 	: name,crc
	Returns 	: PATH*
	Info 		: 
*/
PATH *FindPathInList(char *name,uint32 crc)
{
	uint32			myCrc;
	PATHLISTENTRY	*ptr;

	if (name) myCrc = bkCRC32((uchar*)name,strlen(name),0);
	else	myCrc = crc;

	for (ptr = pathList.head.next;ptr != &pathList.head;ptr = ptr->next)
	{
		if (ptr->crc == myCrc)
		{
			return ptr->path;
		}
	}
	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: CreatePath
	Purpose 	: Allocate memory for a path and setup default loop mode
	Parameters 	: PATH, number of nodes in path, PATHLOOP_NONE/PATHLOOP_LOOP/PATHLOOP_YOYO
	Returns 	: TRUE/FALSE
	Info 		: 
*/
int	 CreatePath(PATH *path, uint32 numNodes)
{
	// malloc all memory needed for path
	path->nodes = (PATHNODE*)ZALLOC(sizeof(PATHNODE)*numNodes);
	if (!path->nodes) return FALSE;
	path->numNodes = numNodes;
	return TRUE;
}

/*	--------------------------------------------------------------------------------
	Function 	: DestroyPath
	Purpose 	: Remove all memory allocated to a path
	Parameters 	: PATH
	Returns 	: 
	Info 		: 
*/
void DestroyPath(PATH *path)
{
	// delete all memory needed for path
	if (path->nodes) SAFE_FREE(path->nodes);
	path->nodes = NULL;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetPathNodeLocation
	Purpose 	: Fill in information for a specific node number
	Parameters 	: PATH, node number, pos, scale, orientation, frame number
	Returns 	: 
	Info 		: 
*/
void SetPathNodeLocation(PATH *path, uint32 nodeNumber, TBVector pos, TBVector scale, TBQuaternion orientation, int frame)
{
	path->nodes[nodeNumber].frame = frame;
	bmVectorCopy(path->nodes[nodeNumber].position,pos);
	bmVectorCopy(path->nodes[nodeNumber].scale,scale);
	bmQuatCopy(path->nodes[nodeNumber].orientation,orientation);
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void SetPathNodeAttributes(PATH *path, float speed)
{
}

/*	--------------------------------------------------------------------------------
	Function 	: ResetPathInfo
	Purpose 	: Initialise the pathinfo structure and assign a path to it
	Parameters 	: PATHINFO, PATH
	Returns 	: 
	Info 		: should be called when changing paths
*/
void ResetPathInfo(PATHINFO *pathInfo, PATH *path)
{
	// initialise pathinfo structure
	pathInfo->path = path;
	bmVectorCopy(pathInfo->position,path->nodes[0].position);
	bmVectorCopy(pathInfo->scale,path->nodes[0].scale);
	bmQuatCopy(pathInfo->orientation, path->nodes[0].orientation);
	pathInfo->time = 0.0f;
	pathInfo->lastState = pathInfo->state = PATHSTATE_STATIONARY;
	pathInfo->lastNode = 0;
	pathInfo->destinationNode = 0;
}

/*	--------------------------------------------------------------------------------
	Function 	: PrepInstancesPathInfo
	Purpose 	: Update an objects position etc using the information in the pathinfo structure
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		: should be called before most prep functions
*/
void PrepInstancesPathInfo(ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if ((ptr->pathInfo)&&(ptr->pathInfo->path))
		{
			GetPathPosition(ptr->pathInfo,ptr->actorInstance.position);
			GetPathOrientation(ptr->pathInfo,ptr->actorInstance.orientation);
			GetPathScale(ptr->pathInfo,ptr->actorInstance.scale);
		}

		if(	(ptr->actorInstance.orientation[0] == 0.0f) && (ptr->actorInstance.orientation[1] == 0.0f) &&
			(ptr->actorInstance.orientation[2] == 0.0f) && (ptr->actorInstance.orientation[3] == 0.0f))
		{
			bkPrintf("*** WARNING *** Null Orientation (FLUFF)! Actor Name:%s, Instance Name: %s \n",ptr->actorName, ptr->instanceName);
		}

	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePathInfoInstances
	Purpose 	: Advance a path by time check for looping
	Parameters 	: ACTORINSTANCELIST, time since last update
	Returns 	: 
	Info 		: scans though list and updates and pathinfo structure
*/
void UpdatePathInfoInstances(ACTORINSTANCELIST *actorInstanceList, float time)
{
	ACTORINSTANCE *ptr;

	ptr = actorInstanceList->head.next;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->pathInfo)
		{
			UpdatePathInfo(ptr,time);
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePathInfo
	Purpose 	: Advance a path by time check for looping
	Parameters 	: ACTORINSTANCE*, time since last update
	Returns 	: 
	Info 		: 
*/
void UpdatePathInfo(ACTORINSTANCE *actorInstance, float timein)
{
	float		slerp,from,to;
	char		message[128];
	bool		finished = false;
	bool		update = false;
	PATHINFO	*pathInfo = actorInstance->pathInfo;
	float		time = timein*60.0f;	// TP: key frames are stored at 60 fps

	if (pathInfo->path==NULL) return;

	switch (pathInfo->state)
	{
	case PATHSTATE_CLOCKWISE:
		update = true;
		pathInfo->oldTime = pathInfo->time;
		pathInfo->time += time;

		while ((pathInfo->time >= pathInfo->path->nodes[pathInfo->destinationNode].frame)&&(finished==false))
		{
			pathInfo->destinationNode++;
			pathInfo->lastNode++;
			if (pathInfo->destinationNode>=pathInfo->path->numNodes)
			{
				// have reached end of nodes
				switch(pathInfo->loopMode)
				{
				case PATHLOOP_NONE:
					pathInfo->destinationNode--;
					pathInfo->lastNode--;
					pathInfo->time = pathInfo->path->nodes[pathInfo->destinationNode].frame;

					if (pathInfo->oldTime < pathInfo->path->nodes[pathInfo->destinationNode].frame)
					{
						// TP: if oldTime was at end then i have reached the end of path for first time
						if (pathInfo->end)
							SendTrigger(pathInfo->end, pathInfo->position);
						bkPrintf("Finished - %s\n",actorInstance->instanceName);
					}
					finished = true;
					pathInfo->lastState = pathInfo->state;
					pathInfo->state = PATHSTATE_WAITING;
					break;

				case PATHLOOP_RESET:
					pathInfo->time = 0.0f;
					pathInfo->destinationNode = 1;
					pathInfo->lastNode = 0;

					// TP: if oldTime was at end then i have reached the end of path for first time
					if (pathInfo->end)
						SendTrigger(pathInfo->end, pathInfo->position);
					bkPrintf("Finished - %s\n",actorInstance->instanceName);
					finished = true;
					pathInfo->lastState = pathInfo->state;
					pathInfo->state = PATHSTATE_WAITING;
					break;

				case PATHLOOP_LOOP:
					pathInfo->time -= pathInfo->path->nodes[pathInfo->destinationNode-1].frame;
					pathInfo->destinationNode = 1;
					pathInfo->lastNode = 0;
					if (pathInfo->end)
						SendTrigger(pathInfo->end, pathInfo->position);
					break;

				case PATHLOOP_YOYO:
					pathInfo->time -= 2.0f*(pathInfo->time - pathInfo->path->nodes[pathInfo->destinationNode-1].frame);
					pathInfo->destinationNode-=2;
					pathInfo->state = PATHSTATE_ANTICLOCKWISE;
					finished = true;
					if (pathInfo->end)
						SendTrigger(pathInfo->end, pathInfo->position);
					break;
				}
			}
			if (pathInfo->lastNode == pathInfo->path->numNodes)
			{
				pathInfo->lastNode = 0;
			}
		}
		break;
	case PATHSTATE_ANTICLOCKWISE:
		update = true;
		pathInfo->oldTime = pathInfo->time;
		pathInfo->time -= time*pathInfo->returnSpeed;

		while ((pathInfo->time <= pathInfo->path->nodes[pathInfo->destinationNode].frame)&&(finished==false))
		{
			pathInfo->destinationNode--;
			pathInfo->lastNode--;
			if (pathInfo->destinationNode<0)
			{
				// have reached end of nodes
				switch(pathInfo->loopMode)
				{
				case PATHLOOP_NONE:
					pathInfo->destinationNode++;
					pathInfo->lastNode++;
					pathInfo->time = pathInfo->path->nodes[pathInfo->destinationNode].frame;
					if (pathInfo->oldTime>pathInfo->path->nodes[pathInfo->destinationNode].frame)
					{
						// TP: if laststate was not waiting then i have reached the end of path for first time
						if (pathInfo->start) 
							SendTrigger(pathInfo->start, pathInfo->position);
					}
					finished = true;
					pathInfo->lastState = pathInfo->state;
					pathInfo->state = PATHSTATE_WAITING;
					break;

				case PATHLOOP_RESET:
					pathInfo->time = pathInfo->path->nodes[pathInfo->path->numNodes-1].frame;
					pathInfo->destinationNode = pathInfo->path->numNodes-1;
					pathInfo->lastNode = 0;

					// TP: if laststate was not waiting then i have reached the end of path for first time
					if (pathInfo->start)
						SendTrigger(pathInfo->start, pathInfo->position);

					finished = true;
					pathInfo->lastState = pathInfo->state;
					pathInfo->state = PATHSTATE_WAITING;
					break;

				case PATHLOOP_LOOP:
					pathInfo->time += pathInfo->path->nodes[pathInfo->path->numNodes-1].frame;
					pathInfo->destinationNode = pathInfo->path->numNodes-1;
					pathInfo->lastNode = 0;

					if (pathInfo->start)
						SendTrigger(pathInfo->start, pathInfo->position);
					break;

				case PATHLOOP_YOYO:
					pathInfo->time = bmFAbs(pathInfo->time);
					pathInfo->destinationNode = 1;
					pathInfo->state = PATHSTATE_CLOCKWISE;
					finished = true;

					if (pathInfo->start)
						SendTrigger(pathInfo->start, pathInfo->position);
					break;
				}
			}
			if (pathInfo->lastNode < 0)
			{
				pathInfo->lastNode = pathInfo->path->numNodes-1;
			}
		}
		break;
	default:
		bmVectorCopy(pathInfo->oldPosition,pathInfo->position);
		bmQuatCopy(pathInfo->oldOrientation,pathInfo->orientation);
		bmVectorCopy(pathInfo->oldScale,pathInfo->scale);
		break;
	}

	if (update)
	{
		if (pathInfo->destinationNode == pathInfo->lastNode)
		{
			bmVectorCopy(pathInfo->oldPosition,pathInfo->position);
			bmVectorCopy(pathInfo->position,pathInfo->path->nodes[pathInfo->lastNode].position);

			bmQuatCopy(pathInfo->oldOrientation,pathInfo->orientation);
			bmQuatCopy(pathInfo->orientation,pathInfo->path->nodes[pathInfo->lastNode].orientation);

			bmVectorCopy(pathInfo->oldScale,pathInfo->scale);
			bmVectorCopy(pathInfo->oldScale,pathInfo->path->nodes[pathInfo->lastNode].scale);
		}
		else
		{
			//	calculate slerp
			from = pathInfo->path->nodes[pathInfo->lastNode].frame;
			to = pathInfo->path->nodes[pathInfo->destinationNode].frame;
			slerp = (pathInfo->time-from)/(to-from);

			// linear slerp between to keyframes
			bmVectorCopy(pathInfo->oldPosition,pathInfo->position);
			bmVectorLerp(pathInfo->position,pathInfo->path->nodes[pathInfo->lastNode].position,
				pathInfo->path->nodes[pathInfo->destinationNode].position,slerp);
			pathInfo->position[W] = 1.0f;

			// quat slerp
			bmQuatCopy(pathInfo->oldOrientation,pathInfo->orientation);
			bmQuatSlerp(pathInfo->orientation,pathInfo->path->nodes[pathInfo->lastNode].orientation,
				pathInfo->path->nodes[pathInfo->destinationNode].orientation,slerp);

			// scale slerp
			bmVectorCopy(pathInfo->oldScale,pathInfo->scale);
			bmVectorLerp(pathInfo->scale,pathInfo->path->nodes[pathInfo->lastNode].scale,
				pathInfo->path->nodes[pathInfo->destinationNode].scale,slerp);
			pathInfo->scale[W] = 1.0f;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: FollowPathClockwise
	Purpose 	: Start following a path in a clockwise direction
	Parameters 	: PATHINFO
	Returns 	: 
	Info 		: 
*/
void FollowPathClockwise(PATHINFO *pathInfo)
{
	pathInfo->lastNode = 0;
	pathInfo->destinationNode = 1;
	pathInfo->time = 0.0f;
	pathInfo->state = PATHSTATE_CLOCKWISE;

	bmVectorCopy(pathInfo->position, pathInfo->path->nodes[0].position);
	bmVectorCopy(pathInfo->scale, pathInfo->path->nodes[0].scale);
	bmQuatCopy(pathInfo->orientation, pathInfo->path->nodes[0].orientation);

	bmVectorCopy(pathInfo->oldPosition,pathInfo->position);
	bmVectorCopy(pathInfo->oldScale, pathInfo->scale);
	bmQuatCopy(pathInfo->oldOrientation, pathInfo->orientation);
}

/*	--------------------------------------------------------------------------------
	Function 	: FollowPathAntiClockwise
	Purpose 	: Start following a path in a anti-clockwise direction
	Parameters 	: PATHINFO
	Returns 	: 
	Info 		: 
*/
void FollowPathAntiClockwise(PATHINFO *pathInfo)
{
	pathInfo->lastNode = pathInfo->path->numNodes-1;
	pathInfo->destinationNode = pathInfo->path->numNodes-2;
	pathInfo->time = pathInfo->path->nodes[pathInfo->path->numNodes-1].frame;
	pathInfo->state = PATHSTATE_ANTICLOCKWISE;
	bmVectorCopy(pathInfo->position, pathInfo->path->nodes[pathInfo->path->numNodes-1].position);
	bmVectorCopy(pathInfo->scale, pathInfo->path->nodes[pathInfo->path->numNodes-1].scale);
	bmQuatCopy(pathInfo->orientation, pathInfo->path->nodes[pathInfo->path->numNodes-1].orientation);

	bmVectorCopy(pathInfo->oldPosition,pathInfo->position);
	bmVectorCopy(pathInfo->oldScale, pathInfo->scale);
	bmQuatCopy(pathInfo->oldOrientation, pathInfo->orientation);
}

/*	--------------------------------------------------------------------------------
	Function 	: GetPathPosition
	Purpose 	: Return the current position
	Parameters 	: PATHINFO, TBVector to fill in
	Returns 	: 
	Info 		: Use this to get the current position on path
*/
void GetPathPosition(PATHINFO *pathInfo, TBVector pos)
{
	bmVectorCopy(pos,pathInfo->position);
}

/*	--------------------------------------------------------------------------------
	Function 	: GetPathScale
	Purpose 	: Return the current scale
	Parameters 	: PATHINFO, TBVector to fill in
	Returns 	: 
	Info 		: Use this to get the current scale on path
*/
void GetPathScale(PATHINFO *pathInfo, TBVector scale)
{
	bmVectorCopy(scale,pathInfo->scale);
}

/*	--------------------------------------------------------------------------------
	Function 	: GetPathOrientation
	Purpose 	: Return the current orientation
	Parameters 	: PATHINFO, TBQuaternion to fill in
	Returns 	: 
	Info 		: Use this to get the current quaternion on path
*/
void GetPathOrientation(PATHINFO *pathInfo, TBQuaternion orientation)
{
	bmQuatCopy(orientation,pathInfo->orientation);
}

/*	--------------------------------------------------------------------------------
	Function 	: SetPathType
	Purpose 	: Called to set the path operation, normal or spin(responds to taz spinning)
	Parameters 	: PATHINFO, pathtype
	Returns 	: 
	Info 		: 
*/
void SetPathType(PATHINFO *pathInfo, EPathType type)
{
	pathInfo->type = type;
}

/*	--------------------------------------------------------------------------------
	Function 	: SetPathLoop
	Purpose 	: Set the current loop mode for the platform
	Parameters 	: PATHINFO, pathtype
	Returns 	: 
	Info 		: 
*/
void SetPathLoop(PATHINFO *pathInfo, EPathLoopMode type)
{
	pathInfo->loopMode = type;
}

/*  --------------------------------------------------------------------------------
	Function	: SetPathToStart
	Purpose		: Set a path to the beginning of it's path
	Parameters	: pathinfo
	Returns		: 
	Info		: 
*/
void SetPathToStart(PATHINFO *pathInfo)
{
	float		slerp,from,to;

	switch(pathInfo->state)
	{
	case PATHSTATE_CLOCKWISE:
		pathInfo->time = 0.0f;
		pathInfo->destinationNode = 1;
		pathInfo->lastNode = 0;
		pathInfo->state = PATHSTATE_WAITING;
		break;

	case PATHSTATE_ANTICLOCKWISE:
		pathInfo->time = pathInfo->path->nodes[pathInfo->path->numNodes-1].frame;
		pathInfo->destinationNode = pathInfo->path->numNodes-2;
		pathInfo->lastNode = pathInfo->path->numNodes-1;
		pathInfo->state = PATHSTATE_WAITING;
		break;

	default:
		pathInfo->time = 0.0f;
		pathInfo->destinationNode = 1;
		pathInfo->lastNode = 0;
		pathInfo->state = PATHSTATE_WAITING;
		break;
	}

	bmVectorCopy(pathInfo->position, pathInfo->path->nodes[pathInfo->lastNode].position);
	bmVectorCopy(pathInfo->scale, pathInfo->path->nodes[pathInfo->lastNode].scale);
	bmQuatCopy(pathInfo->orientation, pathInfo->path->nodes[pathInfo->lastNode].orientation);
}

/*	--------------------------------------------------------------------------------
	Function 	: ChangePathState
	Purpose 	: Called to change the state of a path
	Parameters 	: PATHINFO, new state (PATHSTATE_CLOCKWISE...)
	Returns 	: 
	Info 		: 
*/
void ChangePathState(PATHINFO *pathInfo, EPathInfoState state)
{
	int	tempInt;
	char	message[256];

	switch(pathInfo->state)
	{
	case PATHSTATE_STATIONARY:
		pathInfo->lastState = pathInfo->state;

		// TP: Send start message as soon as platform move for first time
		if (pathInfo->start)
			SendTrigger(pathInfo->start, pathInfo->position);

		switch(state)
		{
		case PATHSTATE_CLOCKWISE:
			FollowPathClockwise(pathInfo);
			break;
		case PATHSTATE_ANTICLOCKWISE:
			FollowPathAntiClockwise(pathInfo);
			break;
		}
		pathInfo->state = state;
		break;
	case PATHSTATE_CLOCKWISE:
		pathInfo->lastState = pathInfo->state;
		switch(state)
		{
		case PATHSTATE_WAITING:
			pathInfo->lastState = pathInfo->state;
			break;
		case PATHSTATE_ANTICLOCKWISE:
			tempInt = pathInfo->lastNode;
			pathInfo->lastNode = pathInfo->destinationNode;
			pathInfo->destinationNode = tempInt;
			break;
		}
		pathInfo->state = state;
		break;
	case PATHSTATE_ANTICLOCKWISE:
		pathInfo->lastState = pathInfo->state;
		switch(state)
		{
		case PATHSTATE_WAITING:
			pathInfo->lastState = pathInfo->state;
			break;
		case PATHSTATE_CLOCKWISE:
			tempInt = pathInfo->lastNode;
			pathInfo->lastNode = pathInfo->destinationNode;
			pathInfo->destinationNode = tempInt;
			break;
		}
		pathInfo->state = state;
		break;
	case PATHSTATE_WAITING:
		if (state != PATHSTATE_WAITING)
		{
			pathInfo->state = pathInfo->lastState;
			ChangePathState(pathInfo,state);
		}
		else
		{
			pathInfo->state = state;
		}
		break;
	}
}


/*  --------------------------------------------------------------------------------
	Function	: GetLastRotationFromPlatform
	Purpose		: Calculates the rotation performed by the platform in the last frame
	Parameters	: PATHINFO, quaternion to be filled in
	Returns		: 
	Info		: 
*/

void GetLastRotationFromPlatform(PATHINFO *pathInfo, TBQuaternion quat)
{
	float			yAngleOld,yAngleNew,theta;
	TBVector		tempVector;

	// rotate character based on y rot of platform
	yAngleOld = GetAngleAroundYFromQuat(pathInfo->oldOrientation);
	yAngleNew = GetAngleAroundYFromQuat(pathInfo->orientation);
	theta = FindShortestAngleSigned(yAngleOld,yAngleNew);
	tempVector[X] = 0.0f;
	tempVector[Y] = 1.0f;
	tempVector[Z] = 0.0f;
	tempVector[W] = -theta;
	bmRotationToQuat(quat,tempVector);
}


/*  --------------------------------------------------------------------------------
	Function	: MovePointOnPlaform
	Purpose		: Calculates the new position of a point based on the movement of a platform last frame
	Parameters	: PATHINFO *path, TBVector point
	Returns		: 
	Info		: 
*/

void MovePointOnPlatform(PATHINFO *pathInfo, TBVector point)
{
	TBVector		tempVector,oldVector,newVector;
	TBMatrix		lastMatrix,currentMatrix,tempQuat,scale;
// PP: REMOUT: not used		TBQuaternion	quat;

	// point rotate
	bmQuatToMatrix(tempQuat,pathInfo->oldOrientation);

	bmMatInverse(lastMatrix,tempQuat);

	bmQuatToMatrix(currentMatrix,pathInfo->orientation);
	bmVectorSub(oldVector,point,pathInfo->position);

	oldVector[W] = 1.0f;
	bmVectorCopy(newVector,oldVector);

	bmMatMultiplyVector(lastMatrix,newVector);
	bmMatMultiplyVector(currentMatrix,newVector);
	bmVectorAdd(point,pathInfo->position,newVector);

	// point position
	bmVectorSub(tempVector,pathInfo->position,pathInfo->oldPosition);

	bmVectorAdd(point,point,tempVector);

	// point scale
	bmMatScale(scale,pathInfo->scale[X]/pathInfo->oldScale[X],
		pathInfo->scale[Y]/pathInfo->oldScale[Y],
		pathInfo->scale[Z]/pathInfo->oldScale[Z]);

	bmVectorSub(tempVector, point, pathInfo->position);

	// PP: TEMP?: at present, bmVectorSub on Xbox does not initialise the W value of the result.
	// PP: This causes problems when you then try to multiply that result by a matrix for example.
	tempVector[W]=1.0f;

	bmMatMultiplyVector(scale, tempVector);
	bmVectorAdd(point, tempVector, pathInfo->position);
}


/*  --------------------------------------------------------------------------------
	Function	: UpdateActorOnPlatform
	Purpose		: Move the character on the platform
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: 
*/

void UpdateActorOnPlatform(ACTORINSTANCE *actorInstance)
{
	TBQuaternion	tempQuat;
	TBVector		tempVector;

	GetLastRotationFromPlatform(actorInstance->actorStatus->pathInfoCollidingWith->pathInfo, tempQuat);
	bmQuatMultiply(actorInstance->actorBehaviour->rotAim, actorInstance->actorInstance.orientation, tempQuat);// TP: maybe this should not use rotAim
	MovePointOnPlatform(actorInstance->actorStatus->pathInfoCollidingWith->pathInfo, actorInstance->actorInstance.position);

	if ((actorInstance->playerNo!=-1)&&((actorInstance->actorStatus->pathInfoCollidingWith->pathInfo->flags&PATHFLAG_CAMERACONTROL)||(gameStatus.player[actorInstance->playerNo].camera->flags&CAMERA_LOOKAROUND)))
	{
		// TP: handle camera pos
		bmVectorCopy(tempVector, gameStatus.player[actorInstance->playerNo].camera->pos);
		MovePointOnPlatform(actorInstance->actorStatus->pathInfoCollidingWith->pathInfo, gameStatus.player[actorInstance->playerNo].camera->pos);
		bmVectorSub(tempVector, gameStatus.player[actorInstance->playerNo].camera->pos, tempVector);
		bmVectorAdd(gameStatus.player[actorInstance->playerNo].camera->destinationPos, gameStatus.player[actorInstance->playerNo].camera->destinationPos, tempVector);

		// TP: handle camera lookat
		bmVectorCopy(tempVector, gameStatus.player[actorInstance->playerNo].camera->lookAt);
		MovePointOnPlatform(actorInstance->actorStatus->pathInfoCollidingWith->pathInfo, gameStatus.player[actorInstance->playerNo].camera->lookAt);
		bmVectorSub(tempVector, gameStatus.player[actorInstance->playerNo].camera->lookAt, tempVector);
		bmVectorAdd(gameStatus.player[actorInstance->playerNo].camera->destinationLookAt, gameStatus.player[actorInstance->playerNo].camera->destinationLookAt, tempVector);

		// TP: Set distance for camera
		if ((actorInstance->actorStatus->pathInfoCollidingWith->pathInfo->cameraDist)&&(~gameStatus.player[actorInstance->playerNo].camera->flags&CAMERA_LOOKAROUND))
		{
			gameStatus.player[actorInstance->playerNo].camera->defaultFixedDistance = actorInstance->actorStatus->pathInfoCollidingWith->pathInfo->cameraDist;
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: ClearPathInfo
	Purpose		: Remove the platform info for the given platform from all entries in the list provided
	Parameters	: platform, actorInstanceList
	Returns		: 
	Info		: 
*/

static void ClearPathInfo(ACTORINSTANCE *actorInstance, ACTORINSTANCELIST *actorInstanceList)
{
	ACTORINSTANCE *ptr;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if (ptr->actorStatus)
		{
			if (ptr->actorStatus->pathInfoCollidingWith==actorInstance)
			{
				ptr->actorStatus->pathInfoCollidingWith = NULL;
			}
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: InvalidatePlatform
	Purpose		: Run through some of the actors that might contain pointers to this platform
	Parameters	: actorInstance of platform
	Returns		: 
	Info		: 
*/

void InvalidatePlatform(ACTORINSTANCE *actorInstance)
{
	ClearPathInfo(actorInstance, &characterInstances);
	ClearPathInfo(actorInstance, &map.characterInstances);
}