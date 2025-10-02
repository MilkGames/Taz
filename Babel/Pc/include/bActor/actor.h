// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : actor.h
//   Purpose : generic primary actor functions
// Component : Generic Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_ACTOR_H__
#define __BACTOR_ACTOR_H__

// ********************************************************************************
// Constants and Macros

// hierarchy node types
#define TBACTORNODE_NULL			0
#define TBACTORNODE_SOFTBONE		1				// a SOFT bone
#define TBACTORNODE_MESH			2				// a mesh (synchronous texture)
#define TBACTORNODE_ASYNCMESH		4				// a mesh (asynchronous texture)


// actor flags
#define TBACTORFLAG_SOFTSKIN		0x00000001		// this actor has a soft skin
#define TBACTORFLAG_UNUSED			0x00000002
#define TBACTORFLAG_SEGMENTED		0x00000004		// this actor uses only segmented meshes
#define TBACTORFLAG_TRILINEAR		0x00000008		// actor using trilinear filtering
#define TBACTORFLAG_SINGLESTAGETEX	0x00000010		// actor uses only stage zero textures


// actor instance flags
#define BACTORINSTFLAG_DYNAMIC		0x0001			// this instance has been dynamically allocated
#define BACTORINSTFLAG_WTOOVALID	0x0002			// the world to object matrix for this instance is valid
#define BACTORINSTFLAG_DLCPUSKINNED	0x0004			// GameCube specific: Display list is invalid from CPU scaling of verts
#define BACTORINSTFLAG_DLCPUSCALED	0x0008			// GameCube specific: Display list is invalid from CPU skinning of verts


// actor prep flags (used with baPrepActorInstance)
#define BACTORPREPFLAG_PREPNODES	0x00000001		// prep actor nodes
#define BACTORPREPFLAG_EARLYOUT		0x00000006		// early out if actor isn't visible (implies BACTORPREPFLAG_TESTBB)
#define BACTORPREPFLAG_TESTBB		0x00000004		// test bounding box and set actorInstance->bbVisible
#define BACTORDRAWFLAG_CPU			0x00000008		// Use cpu rather than vertex shader

// actor draw flags
#define BACTORDRAWFLAG_EARLYOUT		0x00000006		// early out if actor isn't visible (implies BACTORDRAWFLAG_TESTBB)
#define BACTORDRAWFLAG_TESTBB		0x00000004		// test bounding box and set actorInstance->bbVisible

// vertex types
typedef enum EBActorVertType {
	// Standard vertex formats (hardware rendering on PC)
	EBACTORVERTTYPE_SINGLETEX = 0,
	EBACTORVERTTYPE_DUALTEX,
	EBACTORVERTTYPE_TRITEX,
	EBACTORVERTTYPE_QUADTEX,

	// Effects vertex formats (vertex shaders on PC - emulated if hardware not present)
	EBACTORVERTTYPE_SINGLETEX_EFFECTS,
	EBACTORVERTTYPE_DUALTEX_EFFECTS,
	EBACTORVERTTYPE_TRITEX_EFFECTS,
	EBACTORVERTTYPE_QUADTEX_EFFECTS,

	// GameCube specific faster rendering: display lists
	EBACTORVERTTYPE_DISPLAYLIST_SINGLECOLOUR,
	EBACTORVERTTYPE_DISPLAYLIST_DUALCOLOUR,
	EBACTORVERTTYPE_DISPLAYLIST_ENVIRONMENTMAPPED,

	EBACTORVERTTYPES_NOOF,
	EBACTORVERTTYPES_DWORD = 0x7fffffff
};


// render modes
/* TW: PC note:
	The stage zero modes MUST be able to be done in conjunction with the rendering of the 'normal' polygons,
	or override them totally. If this isn't the case, then we'll need to rewrite the bDrawSoftSkin
	and some of the shader selector functionality.

	Stage 1+ doesn't worry about such trivialities.
*/
typedef enum EBActRenderMode {
	BACTRENDERMODE_NONE,							// No renderer

	BACTRENDERMODE_NORMAL,							// Stage 0 modes
	BACTRENDERMODE_CARTOONLIT,
	BACTRENDERMODE_ZONLY,

	BACTRENDERMODE_OUTLINED,						// Stage 1+ modes
	BACTRENDERMODE_SHELL,
	BACTRENDERMODE_ENVIRONMENT,						// spherical environment map

	BACTRENDERMODE_CUSTOM,

	BACTRENDERMODE_SHADOW,							// shadow silhouette renderer

	BACTRENDERMODE_OUTLINED_TEXTURE,				// Outlined using texturing

	BACTRENDERMODE_NOOF								// symbolic constant indicating the number of defined render modes
};


// default animation queue defines/enums
#define BTICKSPERFRAME				1024			// 1024 ticks per animation frame
#define BTICKSPERFRAMESHIFT			10				// shift for above value


// animation types
typedef enum EBAnimTypes {
	BANIMTYPE_INVALID,

	BANIMTYPE_NORMAL,				// play anim
	BANIMTYPE_BLENDFRAMES,			// blend two frames
	BANIMTYPE_BLENDSTATE,			// blend from current state to frame
	BANIMTYPE_COPYQUEUE,			// copy the queue from another node and start playing that

	BANIMTYPE_FORCE32 = 0x7fffffff
}; 


// animation queue entry flags (16bit)
#define BANIMQUEUEFLAG_LOOPING			0x0001			// this queue entry should loop
#define BANIMQUEUEFLAG_LOOPPENDING		0x0002			// this queue entry becomes a looping NORMAL when finished
#define BANIMQUEUEFLAG_INHERITEDQUEUE	0x0004			// this queue entry was set by inherited recursion (i.e. it wasn't the node passed to the original 'Set' call)
#define BANIMQUEUEFLAG_FIRST			0x0008			// set for the first iteration on this entry
#define BANIMQUEUEFLAG_NORMALMOTION		0x0010			// always treat motion bone as normal animation

// actor callback flags
#define BACTORCALLBACKFLAG_CALLBACK		0x0000001		// change the callback
#define BACTORCALLBACKFLAG_CONTEXT		0x0000002		// change the context
#define BACTORCALLBACKFLAG_RECURSE		0x0000004		// recurse on all child nodes


// anim queue entry callback reason codes
typedef enum EBQueueCallbackReason {
	BANIMQUEUEREASON_ENTRYCOMPLETE,						// queue entry complete, parameters=callback context
	BANIMQUEUEREASON_FLUSHED,							// queue entry has been flushed
};


// node instance flags (16bit)
#define BNODEINSTFLAG_POSKEYS			0x0001			// evaluate position keys for this node
#define BNODEINSTFLAG_ORIENTKEYS		0x0002			// evaluate orientation keys for this node
#define BNODEINSTFLAG_SCALEKEYS			0x0004			// evaluate scale keys for this node
#define BNODEINSTFLAG_ALLKEYS			0x0007			// evaluate all keys for this node
#define BNODEINSTFLAG_HIDDEN			0x0010			// hide this node and it's children
#define BNODEINSTFLAG_DONTINHERITANIM	0x0020			// don't inherit animation queue postings onto this node
#define BNODEINSTFLAG_POSDIRTY			0x0040			// position has been updated by baUpdateNodes
#define BNODEINSTFLAG_ORIENTDIRTY		0x0080			// orientation has been updated by baUpdateNodes
#define BNODEINSTFLAG_SCALEDIRTY		0x0100			// scale has been updated by baUpdateNodes
#define BNODEINSTFLAG_DIRTYMASK			(BNODEINSTFLAG_POSDIRTY | BNODEINSTFLAG_ORIENTDIRTY | BNODEINSTFLAG_SCALEDIRTY)


// primitive flags (8bit)
#define BPRIMFLAG_UTILE					0x01
#define BPRIMFLAG_VTILE					0x02
#define BPRIMFLAG_NOOUTLINE				0x04

// maximum render stages
#define BMAXRENDERSTAGES				3

// baNodeLookAt() flags
#define BNODELOOKAT_PREP				0x00000001
#define BNODELOOKAT_PREPCHILDREN		0x00000002

// baNodeLookAtUnconstrained() flags
#define BNODELOOKATUC_PREP				0x00000001
#define BNODELOOKATUC_PREPCHILDREN		0x00000002

// baGetActorGeometry() flags
#define BGETACTGEOMETRY_COUNTONLY		0x00000001		// retrieve just the tri/vertex count
#define BGETACTGEOMETRY_ZEROPAD			0x00000002		// clear all the pad bytes to zero
#define BGETACTGEOMETRY_WORLDSPACE		0x00010000		// return coords in world space instead of local actor space
#define BGETACTGEOMETRY_NODESPACE		0x00020000		// return coords in node space instead of local actor space

// data stream component types
enum {
	EBDATASTREAMCOMP_NULL,

	// per-vertex stream components
	EBDATASTREAMCOMP_X,					// vertex X position
	EBDATASTREAMCOMP_Y,					// vertex Y position
	EBDATASTREAMCOMP_Z,					// vertex Z position
	EBDATASTREAMCOMP_VNX,				// vertex X normal
	EBDATASTREAMCOMP_VNY,				// vertex Y normal
	EBDATASTREAMCOMP_VNZ,				// vertex Z normal

	// per-tri stream components
	EBDATASTREAMCOMP_IDX1,				// tri index 1 (16bit)
	EBDATASTREAMCOMP_IDX2,				// tri index 2 (16bit)
	EBDATASTREAMCOMP_IDX3,				// tri index 3 (16bit)
	EBDATASTREAMCOMP_FNX,				// tri X normal
	EBDATASTREAMCOMP_FNY,				// tri Y normal
	EBDATASTREAMCOMP_FNZ,				// tri Z normal
	EBDATASTREAMCOMP_V1X,				// vert 1 X position
	EBDATASTREAMCOMP_V1Y,				// vert 1 Y position
	EBDATASTREAMCOMP_V1Z,				// vert 1 Z position
	EBDATASTREAMCOMP_V2X,				// vert 2 X position
	EBDATASTREAMCOMP_V2Y,				// vert 2 Y position
	EBDATASTREAMCOMP_V2Z,				// vert 2 Z position
	EBDATASTREAMCOMP_V3X,				// vert 3 X position
	EBDATASTREAMCOMP_V3Y,				// vert 3 Y position
	EBDATASTREAMCOMP_V3Z,				// vert 3 Z position
	EBDATASTREAMCOMP_V1NX,				// vert 1 X normal
	EBDATASTREAMCOMP_V1NY,				// vert 1 Y normal
	EBDATASTREAMCOMP_V1NZ,				// vert 1 Z normal
	EBDATASTREAMCOMP_V2NX,				// vert 2 X normal
	EBDATASTREAMCOMP_V2NY,				// vert 2 Y normal
	EBDATASTREAMCOMP_V2NZ,				// vert 2 Z normal
	EBDATASTREAMCOMP_V3NX,				// vert 3 X normal
	EBDATASTREAMCOMP_V3NY,				// vert 3 Y normal
	EBDATASTREAMCOMP_V3NZ,				// vert 3 Z normal
	EBDATASTREAMCOMP_U1,				// vert 1 texture u
	EBDATASTREAMCOMP_U2,				// vert 2 texture u
	EBDATASTREAMCOMP_U3,				// vert 3 texture u
	EBDATASTREAMCOMP_V1,				// vert 1 texture v
	EBDATASTREAMCOMP_V2,				// vert 2 texture v
	EBDATASTREAMCOMP_V3,				// vert 3 texture v

	EBDATASTREAMCOMP_MAX,
};

#define EBDATASTREAMCOMP_MAX_ALIGN4		(BROUNDUP_POW2(EBDATASTREAMCOMP_MAX,4))


// actor prim alpha style flags
#define BACTPRIMALPHATYPE_SOLID			0x00000001		// prims with no alpha at all - these are drawn first
#define BACTPRIMALPHATYPE_CKEY			0x00000002		// prims with colour key but no variable texture alpha or vertex alpha - these are drawn next
#define BACTPRIMALPHATYPE_ALPHA			0x00000004		// prims with variable texture alpha or vertex alpha - these are drawn last
#define BACTPRIMALPHATYPE_NONSOLID		(BACTPRIMALPHATYPE_CKEY | BACTPRIMALPHATYPE_ALPHA)	// draw non-solid types
#define BACTPRIMALPHATYPE_ALL			(BACTPRIMALPHATYPE_SOLID | BACTPRIMALPHATYPE_CKEY | BACTPRIMALPHATYPE_ALPHA)// draw all types


// baSetAnimSpeed flags
#define BSETANIMSPEED_ANIM1DELTA		0x00000001		// change deltaFrame1
#define BSETANIMSPEED_ANIM2DELTA		0x00000002		// change deltaFrame2
#define BSETANIMSPEED_DELTAT			0x00000004		// change delta-t
#define BSETANIMSPEED_T					0x00000008		// change 't'
#define BSETANIMSPEED_DELTAS			(BSETANIMSPEED_ANIM1DELTA | BSETANIMSPEED_ANIM2DELTA | BSETANIMSPEED_DELTAT)	// change deltas but not 't'
#define BSETANIMSPEED_ALL				0xffffffff		// change everything


// baCreateShadowVolume flags
#define BCREATESHADOWVOL_DIRECTIONAL	0x00000001		// treat light as directional rather than a point source


// TBActRenderMode_EnvironmentMap flags
#define BRENDERMODE_ENVMAP_ALPHAMASK	0x00000001		// use alpha channel in previous stage as a mask
#define BRENDERMODE_ENVMAP_ALPHATRANS	0x00000002		// as above but values between 0 and 1 are transparent
#define BRENDERMODE_ENVMAP_SINGLEPASS	0x00000004		// performing single pass rendering

// ********************************************************************************
// Types, Structures and Classes


// BACTRENDERMODE_CARTOONLIT render mode parameters
typedef struct _TBActRenderMode_CartoonLit {
	TBTexture					*rampTexture;				// texture to retrieve ramps from
	int32						rampNumber;					// ramp number from above texture (0..yRes-1)
	struct _TBLightsource		*lightsource;				// lightsource to light from (must be directional)
} TBActRenderMode_CartoonLit;


// BACTRENDERMODE_OUTLINED render mode parameters
typedef struct _TBActRenderMode_Outlined {
	float						thickness;					// thickness of line in world space
	int32						r,g,b, a;					// outline colour
} TBActRenderMode_Outlined;


// BACTRENDERMODE_OUTLINE_TEXTURE render mode parameters
typedef struct _TBActRenderMode_OutlinedTexture {
	int							thickness;					// thickness of line in lighting intensity 0-255
	int32						r,g,b, a;					// outline colour
} TBActRenderMode_OutlinedTexture;


// BACTRENDERMODE_SHELL render mode parameters
typedef struct _TBActRenderMode_Shell {
	float						thickness;					// thickness of shell
	int32						r,g,b, a;					// colour/alpha
} TBActRenderMode_Shell;


// BACTRENDERMODE_SHADOW render mode parameters
typedef struct _TBActRenderMode_Shadow {
	int32						r,g,b;						// shadow colour
} TBActRenderMode_Shadow;


// BACTRENDERMODE_ENVIRONMENT render mode parameters
typedef struct _TBActRenderMode_EnvironmentMap {
	TBTexture					*mapTexture;				// the texture for the sphere map.
	int32						alphaBlendMode;				// alpha blending mode to use
	int32						alphaValue;					// alpha value to use
	int32						stage;						// texture stage to use
	int32						flags;						// flag (see BRENDERMODE_ENVMAP_)
} TBActRenderMode_EnvironmentMap;


// a single polygon primitive
typedef struct _TBPolyPrim {
	uchar						primType;					// primitive type (BDPRIMTYPE_)
	uchar						flags;						// flags (see BPRIMFLAG_)
	ushort						noofVertices;				// #vertices in this primitive
	TBTexture					*texturePtr;				// texture to use
	TSkinVertexColour			*colours;					// this primitive's vertex colours
	float						*textureCoords;				// this primitive's vertex texture co-ordinates
} TBPolyPrim;


// TBACTORNODE_SOFTBONE node info
typedef struct _TBActorInfo_SoftBone {
	TBMatrix					invBoneReference;			// inverse of bone reference matrix (object space to bone space)

	int32						matrixPaletteIndex;			// index of this node into the matrix palette
	uint32						pad2[3];
} TBActorInfo_SoftBone;


// header for stored geometry data streams
typedef struct _TBStoredGeometryStreamHeader {
	uchar						noofComponents;				// #components
	uchar						streamType;					// stream type (0=per-vertex, 1=per-tri)
	ushort						recordSize;					// size of a stream record
	uchar						type[24];					// stream component types
	uchar						offset[24];					// offset to each component in bytes
	uchar						componentMap[EBDATASTREAMCOMP_MAX_ALIGN4];		// component offset map
} TBStoredGeometryStreamHeader;


// stored geometry structure for mesh nodes
typedef struct _TBStoredGeometry {
	int32						noofVertices;				// number of vertices in mesh
	int32						noofTriangles;				// number of triangles in mesh
	uint32						pad[2];

	float						*dataStreams[4];			// data stream pointers
} TBStoredGeometry;


// TBACTORNODE_MESH node info
typedef struct _TBActorInfo_Mesh {
	TBMesh						mesh;						// mesh data itself
	TBStoredGeometry			storedGeometry;				// stored geometry information
} TBActorInfo_Mesh;


// TBACTORNODE_LINK node info
typedef struct _TBActorInfo_Link {
} TBActorInfo_Link;


// TBACTORNODE_ASYNCMESH node info
typedef struct _TBActorInfo_AsyncMesh {
	TBAsyncMesh					asyncMesh;					// async-texture mesh data
	TBStoredGeometry			storedGeometry;				// stored geometry information
} TBActorInfo_AsyncMesh;


// an animation event record
typedef struct _TBActorAnimEvent {
	int32						frame;						// tick this event happens on
	char						*event;						// ptr to event name
	char						*argument;					// ptr to argument for event (can point to empty string but cannot be NULL)
	uint32						pad;
} TBActorAnimEvent;


// an actor hierarchy node
typedef struct _TBActorNode {
	TBAnimTrack3				position;					// position animation data
	TBAnimTrack3				scale;						// scale animation data
	TBAnimTrack4				orientation;				// orientation animation data

	union {
		TBActorInfo_SoftBone	softBone;					// info for TBACTORNODE_SOFTBONE nodes
		TBActorInfo_Mesh		mesh;						// info for TBACTORNODE_MESH nodes
		TBActorInfo_AsyncMesh	asyncMesh;					// info for TBACTORNODE_ASYNCMESH nodes
		TBActorInfo_Link		link;						// info for TBACTORNODE_LINK nodes
	};

	struct _TBActorNode			*next, *prev;				// sibling links
	struct _TBActorNode			*parent, *children;			// hierarchy links

	uchar						type;						// type of node (see TBACTORNODE_)
	uchar						flags;						// default flags for node instance - see BNODEINSTFLAG_
	ushort						pad;
	uint32						crc;						// node CRC (from lower-case name)
	TBActorAnimEvent			*actAnimEvents;				// anim events for this node (sorted by frame) or NULL for no events
	int32						noofActAnimEvents;			// #anim events in above array
} TBActorNode;


// actor animation segment
typedef struct _TBActorAnimSegment {
	uint32						crc;						// CRC of segment name
	int32						startFrame;					// start frame tick
	int32						endFrame;					// end frame tick
	int32						ticksPerFrame;				// how many ticks per frame this animation runs at (BTICKSPERSECOND ticks/sec)

	union {
		void					*pContext;					// client context value
		int32					iContext;					// client context value
		float					fContext;					// client context value
		short					sContext[2];				// client context value
		uchar					cContext[4];				// client context value
	};
	char						*name;						// Segment name (may be NULL is names not included)
	int32						pad1;
	int32						pad2;
} TBActorAnimSegment;


// an actor - information that is shared by all instances of an actor
typedef struct _TBActor {
	TBResourceInfo				resInfo;					// global resource info

	TBSoftSkin					softSkin;					// mesh to be held up by SOFT bones

	TBActorNode					*rootNode;					// root of the node heirarchy
	uint32						flags;						// flags for this actor
	uint32						lastFrame;					// last frame of animation
	int32						maxPrimVertices;			// maximum number of vertices on a single primitive

	int32						maxTotalPrimVertices;		// max total vertices used by all primitives
	TBActorAnimSegment			*animSegments;				// array of animation segments (sorted in ascending crc order)
	uint32						numAnimSegments;			// number of animation segments
	float						maxRadius;					// maximum radius

	float						xMin, xMax;					// X bounds
	float						yMin, yMax;					// Y bounds

	float						zMin, zMax;					// Z bounds
	uchar						matrixPaletteSize;			// number of matrices in the palette for this actor (=number of bones, soft-skin only)
	uchar						vertexType;					// type of vertex used by this actor (EBActorVertType)
	uchar						pad1;
	uchar						pad2;
	char						*animEventData;				// string buffer for anim events

	char						*animSegmentNames;			// name information for segments - NULL if not stored
} TBActor;


// callback for queued animation events
typedef int (*TBQueuedAnimCallback)(EBQueueCallbackReason reasonCode, struct _TBActorInstance *actor, struct _TBActorNodeInstance *node,
									 TBActorAnimSegment *segment, void *parameters);


// actor node queued animation
typedef struct _TBQueuedAnim {
	uchar						active;						// animation active
	uchar						pad1;
	ushort						flags;						// flags
	EBAnimTypes					type;						// animation type
	TBActorAnimSegment			*animSeg1;					// source anim segment
	TBActorAnimSegment			*animSeg2;					// dest anim segment

	float						t;							// current t
	float						deltaT;						// delta t value
	int32						frame1;						// source frame num
	int32						frame2;						// dest frame num

	int32						deltaFrame1;				// delta frame1 value
	int32						deltaFrame2;				// delta frame2 value
	TBQueuedAnimCallback		callback;					// client callback function
	void						*context;					// context for above callback function

	struct _TBActorNodeInstance	*srcNodeInstance;			// source actor node to copy from
//ahogg	uint32						pad2[3];
	int32						repeatCount;				// number of times to repeat an anim
	uint32						pad2[2];
} TBQueuedAnim;

// callback for node controller
typedef int (*TBNodeController)(struct _TBActorNodeInstance *node, void *context);

// actor nodes saved state
typedef struct BXBALIGNVAR16 _TBActorNodeInstance {
	TBAnimTrack3				*position;					// position animation data
	TBAnimTrack3				*scale;						// scale animation data
	TBAnimTrack4				*orientation;				// orientation animation data
	TBQueuedAnim				*animQueue;					// animation queue

	TBAnimState					animState;					// current animation state of this node (PSO)
	TBAnimState					savedState;					// saved anim state of this node (blend source) (PSO)

	float						blendVal;					// node's 't' blending value (0..1)
	int32						targetFrame;				// target frame number
	uchar						currentQueue;				// current animation in the queue
	uchar						queueSize;					// number of entries in the queue
	ushort						flags;						// flags (see BNODEINSTFLAG_)
	TBActorNode					*actorNode;					// the real actor node

	TBMatrix					nodeToLocalWorld;			// transformation from this node's space to world space relative to the actor instance
	TBMatrix					inheritedXForm;				// transform that should be inherited to children (scaleless xform inheritance)
	TBMatrix					referenceMatrix;			// used defined transform to affect the basic transform of the node. Combined with the inverse reference matrix.

	struct _TBActorNodeInstance	*next, *prev;				// sibling links
	struct _TBActorNodeInstance	*parent, *children;			// heirarchy links

	TBActorAnimEvent			*animEvents;				// anim events for this node (sorted by frame) or NULL for no events
	int32						noofAnimEvents;				// #anim events in above array

	TBNodeController			controller;					// controller function attached to this node
	UBContext32					context;					// data context associated with controller
} TBActorNodeInstance BALIGN16;


// an actor instance - information that is local to each occurance of an actor
typedef struct BXBALIGNVAR16 _TBActorInstance {
	TBMatrix					objectToWorld;					// object->world matrix from instance PSO
	TBMatrix					objectToWorldNoScale;			// object->world matrix from instance PO
	TBMatrix					worldToObject;					// world->object matrix from instance PSO (must be validated with baValidateActorInstanceWorldToObject())

	TBVector					position;						// instance position
	TBVector					scale;							// instance scale
	TBQuaternion				orientation;					// instance orientation
	TBMatrix					orientationMatrix;				// orientation matrix (matrix version of above quaternion calculated during baPrepActorInstance)

	TBActor						*actor;							// the actor to draw
	TBActorNodeInstance			*rootNodeInstance;				// our instance's node data
	ushort						flags;							// actor instance flags (see ACTORINSTFLAG_)
	ushort						bbVisible;						// bounding-box visibility code (0=OFF, 1=ON, 2=PARTIAL)
	TBActorAnimSegment			*animSegments;					// array of animation segments (sorted in ascending crc order)

	EBActRenderMode				renderMode[BMAXRENDERSTAGES];	// rendering mode for each stage
	uint32						numAnimSegments;				// number of animation segments

	void						*renderParms[BMAXRENDERSTAGES];	// parameters for render function
	TBActorNodeInstance			*motionBone;					// ptr to motion bone for this actor instance or NULL for no motion bone

	float						*matrixPalette;					// ptr to matrix palette for soft-skinning or NULL for not soft skinned (follows TBActorInstance in memory)
	UBContext32					context;						// user data context
	uint32						pad[2];
} TBActorInstance BALIGN16;


// ********************************************************************************
// Globals

extern int						bGlobalActorAlphaScale;						// global alpha sclae for actor drawing (0..256)


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitActor
	Purpose : initialise the actor module
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitActor();


/*	--------------------------------------------------------------------------------
	Function : bShutdownActor
	Purpose : shutdown the actor module
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownActor();


/*	--------------------------------------------------------------------------------
	Function : baInitActorInstance
	Purpose : create and/or initialise an actor instance
	Parameters : ptr to actor instance or NULL for dynamic allocation, actor to assign, anim queue size
	Returns : ptr to actor instance or NULL for creation failure
	Info : 
*/

TBActorInstance *bInitActorInstance(TBActorInstance *actorInstance, TBActor *actor, int queueSize, char *file, int line);
#define baInitActorInstance(ACTORINSTANCE, ACTOR, QUEUESIZE) bInitActorInstance(ACTORINSTANCE, ACTOR, QUEUESIZE, __FILE__, __LINE__)


/*	--------------------------------------------------------------------------------
	Function : baFreeActorInstance
	Purpose : delete an actor instance
	Parameters : ptr to actor instance
	Returns : 
	Info : will only free resources from static instances not the instance struct itself
*/

void baFreeActorInstance(TBActorInstance *actorInstance);


/*	--------------------------------------------------------------------------------
	Function : baPrepActorInstance
	Purpose : prepare an actor instance to reflect it's current state
	Parameters : ptr to actor instance, actor prep flags 
	Returns : 
	Info : 
*/

void baPrepActorInstance(TBActorInstance *actorInstance, int flags);


/*	--------------------------------------------------------------------------------
	Function : baDrawActorInstance
	Purpose : draw an actor instance using it's current state
	Parameters : ptr to actor instance, alpha type bitmask, draw flags
	Returns : TRUE if the actor was drawn, FALSE if it was rejected by the bounding box test
	Info : 
*/

int baDrawActorInstance(TBActorInstance *actorInstance, int alpha = BACTPRIMALPHATYPE_ALL, int flags=BACTORDRAWFLAG_EARLYOUT);


/*	--------------------------------------------------------------------------------
	Function : baDrawActorInstanceAlpha
	Purpose : draw all non-solid prims for an actor
	Parameters : ptr to actor instance
	Returns : 
	Info : 
*/

#define baDrawActorInstanceAlpha(actorInstance) baDrawActorInstance((actorInstance), BACTPRIMALPHATYPE_NONSOLID)


/*	--------------------------------------------------------------------------------
	Function : bLoadActorByCRC
	Purpose : load an actor from storage
	Parameters : package index, crc
	Returns : ptr to actor instance or NULL for failure
	Info : 
*/

TBActor *bLoadActorByCRC(TBPackageIndex *index, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : bDeleteActor
	Purpose : delete an actor
	Parameters : actor ptr
	Returns : 
	Info : 
*/

void bDeleteActor(TBActor *actor);


/*	--------------------------------------------------------------------------------
	Function : baSetNodeFrame
	Purpose : set animation frame for a node and it's children
	Parameters : actor-instance, actor-node instance, frame#, recurse on children?
	Returns : 
	Info : 
*/

void baSetNodeFrame(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlend
	Purpose : set blended animation for a node and it's children
	Parameters : actor-instance, actor-node instance, frame#1, frame#2, blend factor, recurse on children?
	Returns : 
	Info : 
*/

void baSetNodeBlend(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame1, uint32 frame2, float t, int recurse);


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
					int recurse);

void baSetNodeBlendMulti(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
					int frame1,float t1,
					int frame2,float t2,
					int frame3,float t3,
					int frame4,float t4,
					int recurse);


/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendTarget
	Purpose : Set target animation for node and children
	Parameters : actor instance, actor-node instance, frame number, recurse on children?
	Returns : 
	Info : 
*/

void baSetNodeBlendTarget(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendValue
	Purpose : Set blending value for target animation
	Parameters : actor instance, actor-node instance, blend value, recurse on children?
	Returns : 
	Info : 
*/

void baSetNodeBlendValue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, float t, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baSetNodeBlendTargetValue
	Purpose : Set target animation and blending value for node and children
	Parameters : actor instance, actor-node instance, frame number, blending value, recurse on children?
	Returns : 
	Info : 
*/

void baSetNodeBlendTargetValue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 frame, float t, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baUpdateNodes
	Purpose : Update all node animations
	Parameters : actor instance, root actor-node instance, time delta, motion vector
	Returns : number of nodes updated
	Info : 
*/

int baUpdateNodes(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, float timeDelta, TBVector motionVector);


/*	--------------------------------------------------------------------------------
	Function : baFlushNodeQueue
	Purpose : flush node & childrens anim queue & stores current state
	Parameters : actor instance, actor-node instance, flush children? (TRUE/FALSE), flush from
	Returns : 
	Info : Set flush from to 1 to keep one element in the queue
*/

void baFlushNodeQueue(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int flushChildren, int flushFrom=0);


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
					 float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount = 1);


/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimNormal
	Purpose : queue a normal animation on a node and its children
	Parameters : actor instance, actor-node instance, anim seg num, anim delta, start frame, flags, callback, callback context
	Returns : 
	Info : start frame of -1 give start of segment
*/

void baQueueNodeAnimNormal(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *animSeg, 
						   int32 animDelta, int32 startFrame, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount = 1);


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
						  float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount = 1);


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
									TBQueuedAnimCallback callback, void *context, int32 repeatCount = 1);


/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlendState
	Purpose : queue a blend from state animation on a node and its children
	Parameters : actor instance, actor-node instance, anim seg, start frame, anim delta, deltaT, flags, callback, callback context
	Returns : 
	Info : start frame of -1 gives start of segment
*/

void baQueueNodeAnimBlendState(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBActorAnimSegment *animSeg,
							   int32 animDelta, int32 startFrame, float deltaT, uint32 flags, TBQueuedAnimCallback callback, void *context, int32 repeatCount = 1);


/*	--------------------------------------------------------------------------------
	Function : baSetAnimSpeed
	Purpose : Change the animation deltas of the currently playing animation
	Parameters : node instance, recurse flag, anim 1 delta, anim 2 delta, t delta, t, flags (see BSETANIMSPEED_)
	Returns : 
	Info :
*/

void baSetAnimSpeed(TBActorNodeInstance *node, int recurse, int32 anim1Delta, int32 anim2Delta, float deltaT,
																		float t=0.0f, uint32 flags=BSETANIMSPEED_DELTAS);


/*	--------------------------------------------------------------------------------
	Function : baRetargetActorAnimation
	Purpose : retarget animation data from one actor to another
	Parameters : source actor, destination actor instance
	Returns : 
	Info : both actors must have the same skeleton, otherwise bad things will happen :(
*/

void baRetargetActorAnimation(TBActor *source, TBActorInstance *dest);


/*	--------------------------------------------------------------------------------
	Function : baFindNodeByCRC
	Purpose : Find a node by its crc
	Parameters : node instance, crc
	Returns : node if found, or NULL if not found
	Info : 
*/

TBActorNodeInstance *baFindNodeByCRC(TBActorNodeInstance *nodeInstance, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : baFindNode
	Purpose : Find a node by its name
	Parameters : node instance, name
	Returns : node if found, or NULL if not found
	Info : 
*/

TBActorNodeInstance *baFindNode(TBActorNodeInstance *nodeInstance, char *nodeName);


/*	--------------------------------------------------------------------------------
	Function : baHideNode
	Purpose : Hide a node (non-skinned)
	Parameters : node instance, recurse down hierarchy
	Returns :
	Info : 
*/

void baHideNode(TBActorNodeInstance *nodeInstance, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baShowNode
	Purpose : Show a node (non-skinned)
	Parameters : node instance, recurse down hierarchy
	Returns :
	Info : 
*/

void baShowNode(TBActorNodeInstance *nodeInstance, int recurse);


/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegmentByCRC
	Purpose : Find an animation segment by its crc
	Parameters : actor instance, crc
	Returns : animation segment, or NULL if not found
	Info : 
*/

TBActorAnimSegment *baFindAnimSegmentByCRC(TBActorInstance *actorInstance, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegment
	Purpose : Find an animation segment
	Parameters : actor instance, segment name
	Returns : animation segment, or NULL if not found
	Info : 
*/

TBActorAnimSegment *baFindAnimSegment(TBActorInstance *actorInstance, char *segName);


/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegmentByCRC
	Purpose : Find an animation segment by its crc
	Parameters : actor, crc
	Returns : animation segment, or NULL if not found
	Info : 
*/

TBActorAnimSegment *baFindAnimSegmentByCRC(TBActor *actor, uint32 crc);


/*	--------------------------------------------------------------------------------
	Function : baFindAnimSegment
	Purpose : Find an animation segment
	Parameters : actor, segment name
	Returns : animation segment, or NULL if not found
	Info : 
*/

TBActorAnimSegment *baFindAnimSegment(TBActor *actor, char *segName);


/*	--------------------------------------------------------------------------------
	Function : baSetActorRenderMode
	Purpose : Set the render mode for an actor
	Parameters : actor instance, render stage (0..BMAXRENDERSTAGES-1), render mode, parameters for mode
	Returns : 
	Info : 
*/

void baSetActorRenderMode(TBActorInstance *actorInstance, int stage, EBActRenderMode mode, void *parms);


/*	--------------------------------------------------------------------------------
	Function : baValidateActorInstanceWorldToObject
	Purpose : ensure that the world->object matrix for an actor instance is valid
	Parameters : ptr to actor instance
	Returns : 
	Info : 
*/

#define baValidateActorInstanceWorldToObject(ACTINST) {	if (!((ACTINST)->flags & BACTORINSTFLAG_WTOOVALID))						\
														{																		\
															bmMatInverse((ACTINST)->worldToObject, (ACTINST)->objectToWorld);	\
															(ACTINST)->flags |= BACTORINSTFLAG_WTOOVALID;						\
														} }


/*	--------------------------------------------------------------------------------
	Function : baGetNodesWorldPosition
	Purpose : Get the world position of a node
	Parameters : actor instance, node instance, returned position, offset from node location
	Returns : 
	Info : 
*/

void baGetNodesWorldPosition(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos, TBVector offset = 0);


/*	--------------------------------------------------------------------------------
	Function : baGetPositionInNodeSpace
	Purpose : Return the location of a point in node space position
	Parameters : actor instance, node instance, returned position
	Returns : 
	Info : (worldspace vector into nodespace)
*/

void baGetPositionInNodeSpace(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos);


/*	--------------------------------------------------------------------------------
	Function : baGetNodesLocalPosition
	Purpose : Get the position of a node relative to the actor instance
	Parameters : actor instance, node instance, returned position
	Returns : 
	Info : 
*/

void baGetNodesLocalPosition(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector pos);


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
				  float yConstraintNeg,  float yConstraintPos, float yAngleOffset, float t, uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : baSetGlobalAlpha
	Purpose : set the global alpha scale value for actor drawing
	Parameters : new alpha scale value (256=solid)
	Returns : 
	Info : 
*/

void baSetGlobalAlpha(int newAlphaScale);


/*	--------------------------------------------------------------------------------
	Function : baNodeLookAtUnconstrained
	Purpose : Orient a node to look at a point in world space
	Parameters : actor instance, node instance, focus point, normalised move factor, max angle factor [0..1],
				 flags (see BNODELOOKATUC_)
	Returns : 
	Info : 
*/

void baNodeLookAtUnconstrained(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, TBVector worldFocusPoint,
							   TBVector refVector, float t, float maxAngleFactor, uint32 flags);


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

void baNodeLookAtAdditional(TBActorInstance *instance, TBActorNodeInstance *nodeInstance, TBVector worldFocus, float xConstraintNeg, float xConstraintPos, float yConstraintNeg, float yConstraintPos, float t, uint32 flags);


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

void baNodeLookAtAdditionalUnconstrained(TBActorInstance *instance, TBActorNodeInstance *nodeInstance, TBVector worldFocus, float t, uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : baGetActorNodeGeometry
	Purpose : retrieve the stored geometry from an actor node
	Parameters : actor node, stream number (0..3), ptr to fill with #vertices (can be NULL),
																					ptr to fill with #tris (can be NULL)
	Returns : ptr to stream data
	Info : 
*/

void *baGetActorNodeGeometry(TBActorNode *actorNode, int stream, int *noofVertices, int *noofTris);


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
						 int *noofTrisPtr, int *bufSize, void *buffer, uint32 flags);


/*	--------------------------------------------------------------------------------
	Function : baSetActorTextureFrames
	Purpose : set all animated textures on an actor to a specific frame
	Parameters : actor ptr, frame number, under-run behaviour, over-run behaviour
	Returns : ptr to resultant stream data
	Info : 
*/

void baSetActorTextureFrames(TBActor *actor, int frame);


/*	--------------------------------------------------------------------------------
	Function : baSetActorTextureFramesByTime
	Purpose : set all animated textures on an actor to a specific time
	Parameters : actor ptr, time in ms, under-run behaviour, over-run behaviour
	Returns : ptr to resultant stream data
	Info : 
*/

void baSetActorTextureFramesByTime(TBActor *actor, int time);


/*	--------------------------------------------------------------------------------
	Function : baCreateShadowVolume
	Purpose : create a shadow volume for an actor
	Parameters : ptr to actor instance, shadow volume to fill, position of light, extrusion distance,
																							flags (see BCREATESHADOWVOL_)
	Returns : ptr to shadow volume or NULL for failure
	Info : 
*/

TBShadowVolume *baCreateShadowVolume(TBActorInstance *actorInstance, TBShadowVolume *shadowVolume, TBVector lightPos,
																					float extrudeDist, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function : baDeleteShadowVolume
	Purpose : delete a shadow volume
	Parameters : ptr to shadow volume to delete
	Returns : 
	Info : 
*/

void baDeleteShadowVolume(TBShadowVolume *shadowVolume);


/*	--------------------------------------------------------------------------------
	Function : baRenderShadowVolume
	Purpose : render a shadow volume into the stencil buffer
	Parameters : shadow volume to render
	Returns : 
	Info : 
*/

void baRenderShadowVolume(TBShadowVolume *shadowVolume);


/*	--------------------------------------------------------------------------------
	Function : baFillStencilShadows
	Purpose : fill in any shadow volume pixels left in the stencil buffer with a colour
	Parameters : red, green, blue, alpha (all 0..255)
	Returns : 
	Info : 
*/

void baFillStencilShadows(int r, int g, int b, int a);


/*	--------------------------------------------------------------------------------
	Function : baSetActorNodeFlags
	Purpose : set/clear flags for an actor instance node and optionally it's children
	Parameters : actor instance, root instance node or NULL for all nodes, new flag values, mask of flags to set, recurse?
	Returns : 
	Info : 
*/

void baSetActorNodeFlags(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, uint32 flagValues,
																							uint32 flagMask, int recurse);

/*	--------------------------------------------------------------------------------
	Function : baIsActorAnimPlaying
	Purpose : determine if an animation is playing on a node and optionally it's children
	Parameters : actor instance, root instance node or NULL for all nodes, recurse?,
				 return anim segment playing (or blending to)
	Returns : TRUE if anim playing or FALSE otherwise
	Info : 
*/

int baIsActorAnimPlaying(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance, int recurse, TBActorAnimSegment **anim=0);


/*	--------------------------------------------------------------------------------
	Function : baQueueNodeAnimBlendToNode
	Purpose : queue a copy queue animation entry on a node and its children
	Parameters : actor instance to copy to, actor-node instance to copy to, node to copy anim queue from, 't' delta,
																						flags, callback, callback context
	Returns : 
	Info : start frame of -1 gives start of segment
*/

void baQueueNodeAnimBlendToNode(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
													TBActorNodeInstance *srcNodeInstance, float tDelta, uint32 flags=0,
													TBQueuedAnimCallback callback=NULL, void *context=NULL);


/*	--------------------------------------------------------------------------------
	Function : baCopyAnimQueue
	Purpose : copy the animation queue information from one node hierarchy to another
	Parameters : dest actor instance, dest node (or NULL for all), source actor instance, source node (or NULL for all),
				 recurse?
	Returns : 
	Info : start frame of -1 gives start of segment
*/

void baCopyAnimQueue(TBActorInstance *destActorInstance, TBActorNodeInstance *destNodeInstance,
								TBActorInstance *srcActorInstance, TBActorNodeInstance *srcNodeInstance, int recurse=TRUE);


/*	--------------------------------------------------------------------------------
	Function : baSetAdditionalNodePosition
	Purpose : Set a node's additional matrix with a translation.
	Parameters : [in] the node to set up
				[in] the new translation.
	Returns : 
	Info : 
*/

void baSetAdditionalNodePosition(TBActorNodeInstance * const node, const TBVector pos);


/*	--------------------------------------------------------------------------------
	Function : baSetAdditionalNodeOrientation
	Purpose : Set a node's additional matrix with an orientation.
	Parameters : [in] the node to set up
				[in] the new orientation.
	Returns : 
	Info : 
*/

void baSetAdditionalNodeOrientation(TBActorNodeInstance * const node, const TBQuaternion orientation);


/*	--------------------------------------------------------------------------------
	Function : baDrawActorBoundingBox
	Purpose : Draw an actor instance's bounding box
	Parameters : actor instance, colour
	Returns : 
	Info : 
*/

void baDrawActorBoundingBox(TBActorInstance *actInstance, int r=255, int g=255, int b=255, int a=255);


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
							uint32 flags = (BACTORCALLBACKFLAG_CALLBACK|BACTORCALLBACKFLAG_CONTEXT),
							int setFrom = 0);


/*	--------------------------------------------------------------------------------
	Function : baSetAnimQueueFlags
	Purpose : update the flags for entries in an actor instance's anim queue
	Parameters : actor instance, root instance node or NULL for all nodes, 
				 flags to change, new flag values, queue entry to set from, recurse?
	Returns : 
	Info : 
*/

void baSetAnimQueueFlags(TBActorInstance *actorInstance, TBActorNodeInstance *nodeInstance,
												uint32 flagMask, uint32 newFlagValues, int setFrom = 0, int recurse=TRUE);


/* --------------------------------------------------------------------------------
   Function : baSetActorNodeController
   Purpose : attach controller to specified node
   Parameters : node to attach to, controller functions, controller context data
   Returns :
   Info : 
*/

void baSetActorNodeController(TBActorNodeInstance *node, TBNodeController controller, void *context);


/*	--------------------------------------------------------------------------------
	Function : bActorCountNoofVerts
	Purpose : Count number of verts in an actor
	Parameters : actor ptr
	Returns : 
	Info : 
*/

int bActorCountNoofVerts(TBActor *actor);


#endif		// __BACTOR_ACTOR_H__
