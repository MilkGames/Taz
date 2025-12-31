// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbActor.h
//   Purpose : platform specific primary actor functions
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_PCXBACTOR_H__
#define __BACTOR_PCXBACTOR_H__

// ********************************************************************************
// Constants and Macros

// size of the matrix palette for the PC
#define MAXMATRIXPALETTEPC		12


// size of the matrix palette for the Xbox
#define MAXMATRIXPALETTEXBOX	47


// soft skin flags
#define BSOFTSKINFLAG_NOTILING			0x00000001			// indicates this skin does not use tiling textures


// mesh flags
#define BACTORMESHFLAG_TILING			0x00000001			// indicates this mesh uses tiling textures
#define BACTORMESHFLAG_SINGLETEXTURE	0x00000002			// indicates this mesh uses only one texture stage
#define BACTORMESHFLAG_NOTEXTURE		0x00000004			// indicates this mesh uses no textures
 

// mesh batch flags
#define BMESHBATCHFLAG_NOTILING			0x00000001			// indicates this batch doesn't use tiling textures


// ********************************************************************************
// Types, Structures and Classes

// type for skin vertex colours
typedef uint32		TSkinVertexColour;


// a mesh
typedef struct _TBAsyncMesh {
	uint32						pad[4];
} TBAsyncMesh;


// a single polygon primitive
typedef struct _TBSoftSkinPrim {
	uchar						primType;					// primitive type (BDPRIMTYPE_xxx)
	uchar						flags;						// prim flags (see BPRIMFLAG_)
	ushort						noofVertices;				// #vertices in this primitive
	uchar						noofMatrices;				// number of matrices this prim uses (max 12)
	uchar						pad;
	ushort						noofDrawPrims;				// #prim vertices (depends on type)
#if BPLATFORM == PC
	uchar						matrixIndices[MAXMATRIXPALETTEPC];	// array of matrix indices
#else
	uchar						matrixIndices[MAXMATRIXPALETTEXBOX];// temporary measure
#endif
} TBSoftSkinPrim;


// a soft skin prim batch (PC version)
typedef struct _TBSoftSkinBatch {
	int32						noofPrims;					// number of primitives in this batch
	union {
		TBTexture				*texture1;					// ptr to texture to use
		uint32					textureCRC1;				// CRC of texture to use
	};
	union {
		TBTexture				*texture2;					// ptr to texture to use
		uint32					textureCRC2;				// CRC of texture to use
	};

	uint32						pad;
} TBSoftSkinBatch;


// original vertex information for CPU skinning
typedef struct _TBSoftSkinVertexCPU 
{
	float		x,y,z;				// position
	float		xn, yn, zn;			// normal
} TBSoftSkinVertexCPU;


// a soft skin
typedef struct _TBSoftSkin {
	ushort						noofVertices;				// total #vertices in all prims in this skin
	ushort						noofBatches;				// #batches in this skin
	uchar						*vertices;					// vertices
	TBSoftSkinBatch				*batches;					// batch array
	TBSoftSkinPrim				*primitives;

	uchar						bonesPerVertex;				// #bones per vertex
	uchar						pad[3];
	uint32						flags;						// flags (see BSOFTSKINFLAG_)
	TBSoftSkinVertexCPU			*originalVerts;				// PC CPU skinning: original vertex list
	TBVertexBuffer				*vertexBuffer;				// vertex buffer

	uchar						padding[32];
} TBSoftSkin;	// Must be 64 bytes since in a union


// a non-async texture mesh prim
typedef struct _TBMeshPrim {
	uchar						primType;					// primitive type (BDPRIMTYPE_xxx+1)
	uchar						flags;						// prim flags (see BPRIMFLAG_)
	ushort						noofVertices;				// #vertices in this primitive
	ushort						noofDrawPrims;				// number of primitives to send to DrawPrimitive()
	ushort						pad;
} TBMeshPrim;


// a non-async texture mesh batch
typedef struct _TBMeshBatch {
	int32						noofPrims;					// number of primitives in this batch
	union {
		uint32					textureCRC1;				// texture for this batch
		TBTexture				*texture1;					// texture for this batch
	};
	union {
		uint32					textureCRC2;				// texture for this batch
		TBTexture				*texture2;					// texture for this batch
	};

	uint32						flags;						// mesh batch flags (see 
} TBMeshBatch;


// a shadow volume face
typedef struct _TBShadowVolumeFace {
	float						xNorm, yNorm, zNorm;		// face normal
	ushort						v1, v2;						// vertex indices

	ushort						v3;							// vertex index
	ushort						pad;
	uint32						sign;						// 1=faces towards light, 0=faces away from light
} TBShadowVolumeFace;


// a shadow volume edge
typedef struct _TBShadowVolumeEdge {
	ushort						vert1, vert2;				// vertex indices
	ushort						face1, face2;				// face indices
} TBShadowVolumeEdge;


// a non-skinned mesh
typedef struct _TBMesh {
	int32						noofVertices;				// total #vertices in all prims in this mesh
	union {
		uchar					*vertices;					// vertices
		TBMeshVertexBuffer		*vertexBuffer;				// vb for vertices
	};
	int32						noofBatches;				// total #batches in this mesh
	TBMeshBatch					*batches;					// batch array

	TBMeshPrim					*primitives;				// ptr to primitive list
	int32						noofSolidBatches;			// #solid batches (these come first)
	int32						noofCKeyBatches;			// #colour-key batches (these come next)
	int32						noofAlphaBatches;			// #alpha batches (these come last)

	int32						firstCKeyPrim;				// index of first colour key prim
	int32						firstAlphaPrim;				// index of first alpha prim
	int32						firstCKeyPrimVert;			// index of first colour key prim vertex
	int32						firstAlphaPrimVert;			// index of first alpha prim vertex

	float						*svVerts;					// shadow volume vertices (x,y,z floats)
	TBShadowVolumeFace			*svFaces;					// shadow volume face indices
	TBShadowVolumeEdge			*svEdges;					// shadow volume edges
	int32						svNoofFaces;				// shadow volume total #faces

	uchar						*svEdgeFlags;				// shadow volume edge flag array (used at runtime)
	int32						svNoofEdges;				// shadow volume total #edges
	int32						svNoofVerts;				// shadow volume total #verts
	uint32						flags;						// mesh flags (see BACTORMESHFLAG_)
} TBMesh;


// BACTRENDERMODE_CUSTOM render mode parameters
typedef struct _TBActRenderMode_Custom {
	class CBShaderFragment		*shaderFragment;			// shader fragment describing our custom renderer
	void						*context;					// context for shader
} TBActRenderMode_Custom;


// a shadow volume
typedef struct _TBShadowVolume {
	TBVector					lightPos;					// light position used

	TBVertexBuffer				*vertexBuffer;				// vertex buffer for shadow volume geometry
	int32						dynamic;					// TRUE if this is a dynamically allocated struct, FALSE otherwise
	int32						noofVerts;					// #verts in shadow volume
	uint32						flags;						// flags used during creation

	struct _TBActorInstance		*actorInstance;				// actor instance that created this shadow volume
	float						xDirExtrude;				// directional extrusion vector
	float						yDirExtrude;
	float						zDirExtrude;
} TBShadowVolume;

// function pointer type for mesh batch renderers
typedef void (*TBDrawMeshCallback)(struct _TBActorInstance *actorInstance, int noofBatches, TBMeshBatch *batchPtr, TBMeshPrim *prim, int startVert, int isAlphaPass);


// ********************************************************************************
// Globals

extern uint64				bNoofSkinnedVertsProcessed;		// #skinned vertices processed
extern uint64				bNoofMeshVertsProcessed;		// #mesh vertices processed
extern uint64				bNoofSkinnedActorsDrawn;		// #skinned actors drawn
extern uint64				bNoofMeshActorsDrawn;			// #mesh actors drawn
extern int					bMeshVertTypeLUT[];				// look up table to convert actor vertex types to mesh vertex types


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bFixupAsyncMesh
	Purpose : fixup the pointers for an async mesh after loading
	Parameters : ptr to async mesh, data base ptr
	Returns : 
	Info : 
*/

void bFixupAsyncMesh(TBAsyncMesh *asyncMesh, uchar *basePtr);


/*	--------------------------------------------------------------------------------
	Function : bDrawMesh
	Purpose : draw a mesh
	Parameters : ptr to mesh, ptr to actor instance, draw alpha prims?
	Returns : 
	Info : 
*/

void bDrawMesh(TBMesh *mesh, struct _TBActorInstance *actorInstance, int alpha, struct _TBActorNodeInstance *nodeInstance);


/*	--------------------------------------------------------------------------------
	Function : bInitActorPlatformSpecific
	Purpose : platform specific actor init code
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitActorPlatformSpecific();


/*	--------------------------------------------------------------------------------
	Function : bShutdownActorPlatformSpecific
	Purpose : platform specific actor shutdown code
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownActorPlatformSpecific();


/*	--------------------------------------------------------------------------------
	Function : bSetupActorNodeList
	Purpose : setup a list of actor nodes for drawing
	Parameters : ptr to root node (contains data - not dummy head), actor instance being drawn
	Returns : 
	Info : 
*/

void bSetupActorNodeList(struct _TBActorNodeInstance *rootNode, struct _TBActorInstance *actorInstance, int alpha);


/*	--------------------------------------------------------------------------------
	Function : bFixupSoftSkin
	Purpose : fixup a softskin after loading
	Parameters : actor ptr
	Returns : 
	Info : 
*/

void bFixupSoftSkin(struct _TBActor *actor);


/*	--------------------------------------------------------------------------------
	Function : bMatMultiplyTranspose
	Purpose : matrix multiplication/transposition.
	Parameters : ptr to destination matrix, ptr to source #1, ptr to source #2, rows in final matrix
	Returns : 
	Info : multiplication is performed so source#2 takes effect first
*/

void bMatMultiplyTranspose(float *dest, const TBMatrix src1, const TBMatrix src2, int rows);


/*	--------------------------------------------------------------------------------
	Function : bSetMatrixPaletteEntry
	Purpose : set matrix palette entry
	Parameters : actor instance, actor instance node
	Returns : 
	Info : 
*/

void bSetMatrixPaletteEntry(struct _TBActorInstance *actorInstance, struct _TBActorNodeInstance *nodeInstance);


/*	--------------------------------------------------------------------------------
	Function : bFixupMesh
	Purpose : fix pointers for normal mesh after loading
	Parameters : base address of actor, ptr to node to fixup, ptr to actor being fixed up
	Returns : 
	Info : 
*/

void bFixupMesh(uchar *basePtr, struct _TBActorNode *node, struct _TBActor *actor);


/*	--------------------------------------------------------------------------------
	Function : bPrepActorNodeList
	Purpose : calculate xform matrices for a list of actor node instances
	Parameters : actor ptr, ptr to root node (contains data - not dummy head), parent XForm, parent scale, actor instance being drawn
	Returns : 
	Info : 
*/

void bPrepActorNodeList(struct _TBActorNodeInstance *rootNode, TBMatrix parentXForm, TBVector parentScale, struct _TBActorInstance *actorInstance);


/*	--------------------------------------------------------------------------------
	Function : bDeleteMesh
	Purpose : delete any platform specific data for a mesh node
	Parameters : node to delete
	Returns : 
	Info : 
*/

void bDeleteMesh(struct _TBActorNode *meshNode);


/*	--------------------------------------------------------------------------------
	Function : bDeleteAsyncMesh
	Purpose : delete any platform specific data for an async mesh node
	Parameters : node to delete
	Returns : 
	Info : 
*/

void bDeleteAsyncMesh(struct _TBActorNode *meshNode);


/*	--------------------------------------------------------------------------------
	Function : bDeleteSoftSkin
	Purpose : delete any platform specific data for a soft skin
	Parameters : actor being deleted
	Returns : 
	Info : 
*/

void bDeleteSoftSkin(struct _TBActor *actor);


/*	--------------------------------------------------------------------------------
	Function : baSetActorRenderCallbacks
	Purpose : override the inner draw loops with user defined functions
	Parameters : mesh draw function, soft skin draw function
	Returns : OK/FAIL
	Info : Pass NULL for either function to reset back to use defaults.
*/

int baSetActorRenderCallbacks(TBDrawMeshCallback meshDraw, TBDrawSoftSkinCallback softSkinDraw);

#endif		// __BACTOR_PCXBACTOR_H__
