#ifndef __H_PATHS
#define __H_PATHS



typedef struct TAG_PATHNODE
{
	TBVector		position;
	TBVector		scale;
	TBQuaternion	orientation;
	float			frame;
} PATHNODE BALIGN16;

enum EPathLoopMode
{
	PATHLOOP_NONE,
	PATHLOOP_LOOP,
	PATHLOOP_YOYO,
	PATHLOOP_RESET,
};

enum EPathType
{
	PATHTYPE_NORMAL,
	PATHTYPE_SPIN,
	PATHTYPE_SQUASH,
};

typedef struct TAG_PATH
{
	PATHNODE	*nodes;
	int			numNodes;
} PATH;

typedef struct TAG_PATHLISTENTRY
{
	struct TAG_PATHLISTENTRY	*next,*prev;
	PATH						*path;
	char						name[32];
	uint32						crc;
} PATHLISTENTRY;

typedef struct TAG_PATHLIST
{
	PATHLISTENTRY	head;
	int				numEntries;
} PATHLIST;

enum EPathInfoState
{
	PATHSTATE_STATIONARY,
	PATHSTATE_WAITING,
	PATHSTATE_CLOCKWISE,
	PATHSTATE_ANTICLOCKWISE,
};

#define PATHFLAG_CAMERACONTROL	(1<<0)

typedef struct TAG_PATHINFO
{
	TBVector		position;
	TBVector		scale;
	TBQuaternion	orientation;

	TBVector		oldPosition;		// change in position in last update
	TBVector		oldScale;			// change in scale in the last update
	TBQuaternion	oldOrientation;		// last orientation

	PATH			*path;
	EPathInfoState	state;
	EPathInfoState	lastState;
	int				destinationNode;

	int				lastNode;
	float			time;
	short			type;				// path type PATHTYPE_...
	short			loopMode;
	struct TAG_COLLISIONLISTENTRY	*collEntry;

	struct TAG_TRIGGERINFO	*start;		// triggered when the platform reaches or crosses the start of the path
	struct TAG_TRIGGERINFO	*end;		// triggered when the platform reaches or crosses the end of the path
	float			oldTime;
	int32			flags;

	float			cameraDist;			// TP: distance to keep the camera at if this platform uses camera control
	float			returnSpeed;
	int32			iPad[2];

	char			pathNameFixup[32];
} PATHINFO,_PATHINFO BALIGN16;

void CreateSpecialTypePath(CPMNode *node, struct TAG_MAPOBJECTATTRIBS *attribs,TBVector position,TBQuaternion quat, TBVector scale);

/*	--------------------------------------------------------------------------------
	Function 	: InitialisePathList
	Purpose 	: Initialise the path list
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void InitialisePathList(void);

/*	--------------------------------------------------------------------------------
	Function 	: DestroyPathList
	Purpose 	: Remove all paths and destroy path list
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void DestroyPathList(void);

/*	--------------------------------------------------------------------------------
	Function 	: AddPathToList
	Purpose 	: Add a path to the path list
	Parameters 	: PATH, name
	Returns 	: 
	Info 		: 
*/
void AddPathToList(PATH *path,char *name);

/*	--------------------------------------------------------------------------------
	Function 	: RemovePathFromList
	Purpose 	: Remove a path from the path list
	Parameters 	: PATH
	Returns 	: 
	Info 		: 
*/
void RemovePathFromList(PATHLISTENTRY *path);

/*	--------------------------------------------------------------------------------
	Function 	: FindPathInList
	Purpose 	: Find a path using name or crc
	Parameters 	: name,crc
	Returns 	: PATH*
	Info 		: 
*/
PATH *FindPathInList(char *name,uint32 crc);

/*	--------------------------------------------------------------------------------
	Function 	: CreatePath
	Purpose 	: Allocate memory for a path and setup default loop mode
	Parameters 	: PATH, number of nodes in path, PATHLOOP_NONE/PATHLOOP_LOOP/PATHLOOP_YOYO
	Returns 	: TRUE/FALSE
	Info 		: 
*/
int	 CreatePath(PATH *path, uint32 numNodes);

/*	--------------------------------------------------------------------------------
	Function 	: DestroyPath
	Purpose 	: Remove all memory allocated to a path
	Parameters 	: PATH
	Returns 	: 
	Info 		: 
*/
void DestroyPath(PATH *path);

/*	--------------------------------------------------------------------------------
	Function 	: SetPathNodeLocation
	Purpose 	: Fill in information for a specific node number
	Parameters 	: PATH, node number, pos, scale, orientation, frame number
	Returns 	: 
	Info 		: 
*/
void SetPathNodeLocation(PATH *path, uint32 nodeNumber, TBVector pos, TBVector scale, TBQuaternion orientation, int frame);

/*	--------------------------------------------------------------------------------
	Function 	: ResetPathInfo
	Purpose 	: Initialise the pathinfo structure and assign a path to it
	Parameters 	: PATHINFO, PATH
	Returns 	: 
	Info 		: should be called when changing paths
*/
void ResetPathInfo(PATHINFO *pathInfo, PATH *path);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePathInfoInstances
	Purpose 	: Advance a path by time check for looping
	Parameters 	: ACTORINSTANCELIST, time since last update
	Returns 	: 
	Info 		: scans though list and updates and pathinfo structure
*/
void UpdatePathInfoInstances(ACTORINSTANCELIST *actorInstanceList, float time);

/*	--------------------------------------------------------------------------------
	Function 	: UpdatePathInfo
	Purpose 	: Advance a path by time check for looping
	Parameters 	: ACTORINSTANCE*, time since last update
	Returns 	: 
	Info 		: 
*/
void UpdatePathInfo(ACTORINSTANCE *actorInstance, float time);

/*	--------------------------------------------------------------------------------
	Function 	: FollowPathClockwise
	Purpose 	: Start following a path in a clockwise direction
	Parameters 	: PATHINFO
	Returns 	: 
	Info 		: 
*/
void FollowPathClockwise(PATHINFO *pathInfo);

/*	--------------------------------------------------------------------------------
	Function 	: FollowPathAntiClockwise
	Purpose 	: Start following a path in a anti-clockwise direction
	Parameters 	: PATHINFO
	Returns 	: 
	Info 		: 
*/
void FollowPathAntiClockwise(PATHINFO *pathInfo);

/*	--------------------------------------------------------------------------------
	Function 	: GetPathPosition
	Purpose 	: Return the current position
	Parameters 	: PATHINFO, TBVector to fill in
	Returns 	: 
	Info 		: Use this to get the current position on path
*/
void GetPathPosition(PATHINFO *pathInfo, TBVector pos);

/*	--------------------------------------------------------------------------------
	Function 	: GetPathScale
	Purpose 	: Return the current scale
	Parameters 	: PATHINFO, TBVector to fill in
	Returns 	: 
	Info 		: Use this to get the current scale on path
*/
void GetPathScale(PATHINFO *pathInfo, TBVector scale);

/*	--------------------------------------------------------------------------------
	Function 	: GetPathOrientation
	Purpose 	: Return the current orientation
	Parameters 	: PATHINFO, TBQuaternion to fill in
	Returns 	: 
	Info 		: Use this to get the current quaternion on path
*/
void GetPathOrientation(PATHINFO *pathInfo, TBQuaternion orientation);

/*	--------------------------------------------------------------------------------
	Function 	: PrepInstancesPathInfo
	Purpose 	: Update an objects position etc using the information in the pathinfo structure
	Parameters 	: ACTORINSTANCELIST
	Returns 	: 
	Info 		: 
*/
void PrepInstancesPathInfo(ACTORINSTANCELIST *actorInstanceList);

/*	--------------------------------------------------------------------------------
	Function 	: SetPathLoop
	Purpose 	: Set the current loop mode for the platform
	Parameters 	: PATHINFO, pathtype
	Returns 	: 
	Info 		: 
*/
void SetPathLoop(PATHINFO *pathInfo, EPathLoopMode type);

/*	--------------------------------------------------------------------------------
	Function 	: ChangePathState
	Purpose 	: Called to change the state of a path
	Parameters 	: PATHINFO, new state (PATHSTATE_CLOCKWISE...)
	Returns 	: 
	Info 		: 
*/
void ChangePathState(PATHINFO *pathInfo, EPathInfoState state);

/*	--------------------------------------------------------------------------------
	Function 	: SetPathType
	Purpose 	: Called to set the path operation, normal or spin(responds to taz spinning)
	Parameters 	: PATHINFO, pathtype
	Returns 	: 
	Info 		: 
*/
void SetPathType(PATHINFO *pathInfo, EPathType type);

/*  --------------------------------------------------------------------------------
	Function	: UpdateActorOnPlatform
	Purpose		: Move the character on the platform
	Parameters	: ACTORINSTANCE
	Returns		: 
	Info		: 
*/
void UpdateActorOnPlatform(ACTORINSTANCE *actorInstance);

/*  --------------------------------------------------------------------------------
	Function	: SetPathToStart
	Purpose		: Set a path to the beginning of it's path
	Parameters	: pathinfo
	Returns		: 
	Info		: 
*/
void SetPathToStart(PATHINFO *pathInfo);

/*  --------------------------------------------------------------------------------
	Function	: InvalidatePlatform
	Purpose		: Run through some of the actors that might contain pointers to this platform
	Parameters	: actorInstance of platform
	Returns		: 
	Info		: 
*/

void InvalidatePlatform(ACTORINSTANCE *actorInstance);

#endif