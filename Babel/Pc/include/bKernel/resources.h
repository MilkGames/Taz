// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : resources.h
//   Purpose : resource management functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_RESOURCES_H__
#define __BKERNEL_RESOURCES_H__

// ********************************************************************************
// Constants and macros

// resource types
typedef enum EBResourceType {
	EBRESTYPE_NONE    = -1,			// none of the resource types
	EBRESTYPE_TEXTURE = 0,
	EBRESTYPE_ACTOR,
	EBRESTYPE_SAMPLE,
	EBRESTYPE_FONT,
	EBRESTYPE_STRINGTABLE,
	EBRESTYPE_SPLASH,
	EBRESTYPE_SET,
	EBRESTYPE_COLLISIONMESH,
	EBRESTYPE_ANIMATINGCOLLISION,
	EBRESTYPE_AUDIOSTREAM,
	EBRESTYPE_LIPSYNC,

	BNOOFRESTYPES,
	EBRESTYPE_ANY,					// symbolic value representing any resource type
};


// resource type bitfield values
#define BRESMASK_TEXTURE			(1<<EBRESTYPE_TEXTURE)
#define BRESMASK_ACTOR				(1<<EBRESTYPE_ACTOR)
#define BRESMASK_SAMPLE				(1<<EBRESTYPE_SAMPLE)
#define BRESMASK_FONT				(1<<EBRESTYPE_FONT)
#define BRESMASK_STRINGTABLE		(1<<EBRESTYPE_STRINGTABLE)
#define BRESMASK_SPLASH				(1<<EBRESTYPE_SPLASH)
#define BRESMASK_SET				(1<<EBRESTYPE_SET)
#define BRESMASK_COLLISIONMESH		(1<<EBRESTYPE_COLLISIONMESH)
#define BRESMASK_ANIMATINGCOLLISION	(1<<EBRESTYPE_ANIMATINGCOLLISION)
#define BRESMASK_AUDIOSTREAM		(1<<EBRESTYPE_AUDIOSTREAM)
#define BRESMASK_LIPSYNC			(1<<EBRESTYPE_LIPSYNC)
#define BRESMASK_ALL				0xffffffff

// special value to cause default group IDs to be used
#define BDEFAULTGROUP		((ushort)0xdefa)


#define BRESFLAG_NOSEARCH			0x00000001


// ********************************************************************************
// Types, structures and classes

// standard resource info structure
typedef struct _TBResourceInfo {
	union {
		TBPackageID			packageId;					// ID of the package this resource came from
		uint32				packageId32;
	};
	ushort					groupId;					// group ID for this resource
	uchar					type;						// type of resource (see EBRESTYPE_)
	uchar					pad1;
	union {
		int32				iContext;					// client context values
		uint32				uiContext;
		short				sContext[2];
		ushort				usContext[2];
		float				fContext;
		char				cContext[4];
		uchar				ucContext[4];
		void				*pContext;
	};
	uint32					crc;						// resource CRC

	struct _TBResourceInfo	*globalNext, *globalPrev;	// global resource list links
	struct _TBResourceInfo	*localNext, *localPrev;		// local resource list links
} TBResourceInfo;


// an entry in a resource snapshot structure
typedef struct _TBResourceSnapshotEntry {
	TBResourceInfo			*infoPtr;					// address of info structure we relate to
	TBResourceInfo			info;						// info struct itself
	int						found;						// used during comparisons
} TBResourceSnapshotEntry;


// a resource snapshot structure (followed by array of TBResourceSnapshotEntry structs to make a snapshot)
typedef struct _TBResourceSnapshot {
	int				noofRes[BNOOFRESTYPES];				// number of resources of each type
	uint32			typeMask;							// typeMask used to gather snapshot
	int				noofEntries;						// total number of entries in snapshot
} TBResourceSnapshot;



// Forward declarations of resource structures for find functions

typedef struct _TBTexture TBTexture;
typedef struct _TBActor TBActor;
typedef struct _TBSample TBSample;
typedef struct _TBFont TBFont;
typedef struct _TBStringTable TBStringTable;
typedef struct _TBSplash TBSplash;
typedef struct _TBSet TBSet;
typedef struct _TBCollisionMesh TBCollisionMesh;
typedef struct _TBACMesh TBACMesh;
typedef struct _TBAudioStream TBAudioStream;
typedef struct _TBLipSync TBLipSync;



// ********************************************************************************
// Globals

extern TBResourceInfo	bGlobalResourceList;				// global resource list root
extern TBResourceInfo	bLocalResourceList[BNOOFRESTYPES];	// local resource list roots

// Null package Id (for when you wish to locate resources in ANY package)
extern TBPackageID		bNULLPackageId;

// ********************************************************************************
// Resource Helper Macros

#ifdef _DEBUG
	// Debug mode helper macros, these include source filename and line number information for verbose resource mode

	// Textures
	#define bkLoadTexture(PAK, FILE, GROUP)				(TBTexture*)bLoadResource(PAK, FILE, EBRESTYPE_TEXTURE, GROUP, __FILE__, __LINE__)
	#define bkLoadTextureByCRC(PAK, FILE, GROUP)		(TBTexture*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_TEXTURE, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageTextures(PAK)					bLoadPackageResources(PAK, BRESMASK_TEXTURE, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteTexture(PTR)						bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageTextures(PAKID)				bDeletePackageResources(PAKID, BRESMASK_TEXTURE, __FILE__, __LINE__)
	#define bkDeleteTextureGroup(GROUP)					bDeleteResourceGroup(BRESMASK_TEXTURE, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllTextures()						bDeleteResourceGroup(BRESMASK_TEXTURE, BALLGROUPS, __FILE__, __LINE__)
	#define bkListTextures()							bkListResources(BRESMASK_TEXTURE)
														
	// Actors											
	#define bkLoadActor(PAK, FILE, GROUP)				(TBActor*)bLoadResource(PAK, FILE, EBRESTYPE_ACTOR, GROUP, __FILE__, __LINE__)
	#define bkLoadActorByCRC(PAK, FILE, GROUP)			(TBActor*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_ACTOR, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageActors(PAK)					bLoadPackageResources(PAK, BRESMASK_ACTOR, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteActor(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageActors(PAKID)				bDeletePackageResources(PAKID, BRESMASK_ACTOR, __FILE__, __LINE__)
	#define bkDeleteActorGroup(GROUP)					bDeleteResourceGroup(BRESMASK_ACTOR, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllActors()							bDeleteResourceGroup(BRESMASK_ACTOR, BALLGROUPS, __FILE__, __LINE__)
	#define bkListActors()								bkListResources(BRESMASK_ACTOR)
														
	// Samples											
	#define bkLoadSample(PAK, FILE, GROUP)				(TBSample*)bLoadResource(PAK, FILE, EBRESTYPE_SAMPLE, GROUP, __FILE__, __LINE__)
	#define bkLoadSampleByCRC(PAK, FILE, GROUP)			(TBSample*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SAMPLE, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageSamples(PAK)					bLoadPackageResources(PAK, BRESMASK_SAMPLE, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteSample(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageSamples(PAKID)				bDeletePackageResources(PAKID, BRESMASK_SAMPLE, __FILE__, __LINE__)
	#define bkDeleteSampleGroup(GROUP)					bDeleteResourceGroup(BRESMASK_SAMPLE, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllSamples()						bDeleteResourceGroup(BRESMASK_SAMPLE, BALLGROUPS, __FILE__, __LINE__)
	#define bkListSamples()								bkListResources(BRESMASK_SAMPLE)
														
	// Fonts											
	#define bkLoadFont(PAK, FILE, GROUP)				(TBFont*)bLoadResource(PAK, FILE, EBRESTYPE_FONT, GROUP, __FILE__, __LINE__)
	#define bkLoadFontByCRC(PAK, FILE, GROUP)			(TBFont*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_FONT, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageFonts(PAK)						bLoadPackageResources(PAK, BRESMASK_FONT, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteFont(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageFonts(PAKID)					bDeletePackageResources(PAKID, BRESMASK_FONT, __FILE__, __LINE__)
	#define bkDeleteFontGroup(GROUP)					bDeleteResourceGroup(BRESMASK_FONT, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllFonts()							bDeleteResourceGroup(BRESMASK_FONT, BALLGROUPS, __FILE__, __LINE__)
	#define bkListFonts()								bkListResources(BRESMASK_FONT)
													
	// StringTables									
	#define bkLoadStringTable(PAK, FILE, GROUP)			(TBStringTable*)bLoadResource(PAK, FILE, EBRESTYPE_STRINGTABLE, GROUP, __FILE__, __LINE__)
	#define bkLoadStringTableByCRC(PAK, FILE, GROUP)	(TBStringTable*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_STRINGTABLE, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageStringTables(PAK)				bLoadPackageResources(PAK, BRESMASK_STRINGTABLE, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteStringTable(PTR)					bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageStringTables(PAKID)			bDeletePackageResources(PAKID, BRESMASK_STRINGTABLE, __FILE__, __LINE__)
	#define bkDeleteStringTableGroup(GROUP)				bDeleteResourceGroup(BRESMASK_STRINGTABLE, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllStringTables()					bDeleteResourceGroup(BRESMASK_STRINGTABLE, BALLGROUPS, __FILE__, __LINE__)
	#define bkListStringTables()						bkListResources(BRESMASK_STRINGTABLE)

	// Splashes
	#define bkLoadSplash(PAK, FILE, GROUP)				(TBSplash*)bLoadResource(PAK, FILE, EBRESTYPE_SPLASH, GROUP, __FILE__, __LINE__)
	#define bkLoadSplashByCRC(PAK, FILE, GROUP)			(TBSplash*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SPLASH, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageSplashes(PAK)					bLoadPackageResources(PAK, BRESMASK_SPLASH, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteSplash(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageSplashes(PAKID)				bDeletePackageResources(PAKID, BRESMASK_SPLASH, __FILE__, __LINE__)
	#define bkDeleteSplashGroup(GROUP)					bDeleteResourceGroup(BRESMASK_SPLASH, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllSplashes()						bDeleteResourceGroup(BRESMASK_SPLASH, BALLGROUPS, __FILE__, __LINE__)
	#define bkListSplashes()							bkListResources(BRESMASK_SPLASH)

	// Sets
	#define bkLoadSet(PAK, FILE, GROUP)					(TBSet*)bLoadResource(PAK, FILE, EBRESTYPE_SET, GROUP, __FILE__, __LINE__)
	#define bkLoadSetByCRC(PAK, FILE, GROUP)			(TBSet*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SET, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageSets(PAK)						bLoadPackageResources(PAK, BRESMASK_SET, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteSet(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageSets(PAKID)					bDeletePackageResources(PAKID, BRESMASK_SET, __FILE__, __LINE__)
	#define bkDeleteSetGroup(GROUP)						bDeleteResourceGroup(BRESMASK_SET, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllSets()							bDeleteResourceGroup(BRESMASK_SET, BALLGROUPS, __FILE__, __LINE__)
	#define bkListSets()								bkListResources(BRESMASK_SET)

	// Collision Meshes
	#define bkLoadCollisionMesh(PAK, FILE, GROUP)		(TBCollisionMesh*)bLoadResource(PAK, FILE, EBRESTYPE_COLLISIONMESH, GROUP, __FILE__, __LINE__)
	#define bkLoadCollisionMeshByCRC(PAK, FILE, GROUP)	(TBCollisionMesh*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_COLLISIONMESH, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageCollisionMeshes(PAK)			bLoadPackageResources(PAK, BRESMASK_COLLISIONMESH, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteCollisionMesh(PTR)					bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageCollisionMeshes(PAKID)		bDeletePackageResources(PAKID, BRESMASK_COLLISIONMESH, __FILE__, __LINE__)
	#define bkDeleteCollisionMeshGroup(GROUP)			bDeleteResourceGroup(BRESMASK_COLLISIONMESH, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllCollisionMeshes()				bDeleteResourceGroup(BRESMASK_COLLISIONMESH, BALLGROUPS, __FILE__, __LINE__)
	#define bkListCollisionMeshes()						bkListResources(BRESMASK_COLLISIONMESH)

	// Animating Collision Meshes
	#define bkLoadAnimatingCollisionMesh(PAK, FILE, GROUP)		(TBACMesh*)bLoadResource(PAK, FILE, EBRESTYPE_ANIMATINGCOLLISION, GROUP, __FILE__, __LINE__)
	#define bkLoadAnimatingCollisionMeshByCRC(PAK, FILE, GROUP)	(TBACMesh*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_ANIMATINGCOLLISION, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageAnimatingCollisionMeshes(PAK)			bLoadPackageResources(PAK, BRESMASK_ANIMATINGCOLLISION, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteAnimatingCollisionMesh(PTR)					bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageAnimatingCollisionMeshes(PAKID)		bDeletePackageResources(PAKID, BRESMASK_ANIMATINGCOLLISION, __FILE__, __LINE__)
	#define bkDeleteAnimatingCollisionMeshGroup(GROUP)			bDeleteResourceGroup(BRESMASK_ANIMATINGCOLLISION, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllAnimatingCollisionMeshes()				bDeleteResourceGroup(BRESMASK_ANIMATINGCOLLISION, BALLGROUPS, __FILE__, __LINE__)
	#define bkListAnimatingCollisionMeshes()					bkListResources(BRESMASK_ANIMATINGCOLLISION)

	// Audio Streams
	#define bkLoadAudioStream(PAK, FILE, GROUP)					(TBAudioStream*)bLoadResource(PAK, FILE, EBRESTYPE_AUDIOSTREAM, GROUP, __FILE__, __LINE__)
	#define bkLoadAudioStreamByCRC(PAK, FILE, GROUP)			(TBAudioStream*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_AUDIOSTREAM, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageAudioStreams(PAK)						bLoadPackageResources(PAK, BRESMASK_AUDIOSTREAM, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteAudioStream(PTR)							bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageAudioStreams(PAKID)					bDeletePackageResources(PAKID, BRESMASK_AUDIOSTREAM, __FILE__, __LINE__)
	#define bkDeleteAudioStreamGroup(GROUP)						bDeleteResourceGroup(BRESMASK_AUDIOSTREAM, GROUP, __FILE__, __LINE__)
	#define bkDeleteAllAudioStreams()							bDeleteResourceGroup(BRESMASK_AUDIOSTREAM, BALLGROUPS, __FILE__, __LINE__)
	#define bkListAudioStreams()								bkListResources(BRESMASK_AUDIOSTREAM)

	// Lip syncing
	#define bkLoadLipSync(PAK, FILE, GROUP)						(TBLipSync*)bLoadResource(PAK, FILE, EBRESTYPE_LIPSYNC, GROUP, __FILE__, __LINE__)
	#define bkLoadLipSyncByCRC(PAK, FILE, GROUP)				(TBLipSync*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_LIPSYNC, GROUP, __FILE__, __LINE__)
	#define bkLoadPackageLipSyncs(PAK)							bLoadPackageResources(PAK, EBRESTYPE_LIPSYNC, BDEFAULTGROUP, NULL, __FILE__, __LINE__)
	#define bkDeleteLipSync(PTR)								bDeleteResource(PTR, __FILE__, __LINE__);
	#define bkDeletePackageLipSyncs(PAKID)						bDeletePackageResources(PAKID, EBRESTYPE_LIPSYNC, __FILE__, __LINE__)
	#define bkDeleteLipSyncGroup(GROUP)							bDeleteResourceGroup(EBRESTYPE_LIPSYNC, GROU, __FILE__, __LINE__P)
	#define bkDeleteAllLipSyncs()								bDeleteResourceGroup(EBRESTYPE_LIPSYNC, BALLGROUPS, __FILE__, __LINE__)
	#define bkListLipSyncs()									bkListResources(EBRESTYPE_LIPSYNC)
#else

	// Release mode helper macros, these do not include any source filename or line number information as verbose
	// resource mode is not supported in release builds

	// Textures
	#define bkLoadTexture(PAK, FILE, GROUP)				(TBTexture*)bLoadResource(PAK, FILE, EBRESTYPE_TEXTURE, GROUP)
	#define bkLoadTextureByCRC(PAK, FILE, GROUP)		(TBTexture*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_TEXTURE, GROUP)
	#define bkLoadPackageTextures(PAK)					bLoadPackageResources(PAK, BRESMASK_TEXTURE, BDEFAULTGROUP, NULL)
	#define bkDeleteTexture(PTR)						bDeleteResource(PTR);
	#define bkDeletePackageTextures(PAKID)				bDeletePackageResources(PAKID, BRESMASK_TEXTURE)
	#define bkDeleteTextureGroup(GROUP)					bDeleteResourceGroup(BRESMASK_TEXTURE, GROUP)
	#define bkDeleteAllTextures()						bDeleteResourceGroup(BRESMASK_TEXTURE, BALLGROUPS)
	#define bkListTextures()							bkListResources(BRESMASK_TEXTURE)
														
	// Actors											
	#define bkLoadActor(PAK, FILE, GROUP)				(TBActor*)bLoadResource(PAK, FILE, EBRESTYPE_ACTOR, GROUP)
	#define bkLoadActorByCRC(PAK, FILE, GROUP)			(TBActor*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_ACTOR, GROUP)
	#define bkLoadPackageActors(PAK)					bLoadPackageResources(PAK, BRESMASK_ACTOR, BDEFAULTGROUP, NULL)
	#define bkDeleteActor(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageActors(PAKID)				bDeletePackageResources(PAKID, BRESMASK_ACTOR)
	#define bkDeleteActorGroup(GROUP)					bDeleteResourceGroup(BRESMASK_ACTOR, GROUP)
	#define bkDeleteAllActors()							bDeleteResourceGroup(BRESMASK_ACTOR, BALLGROUPS)
	#define bkListActors()								bkListResources(BRESMASK_ACTOR)
														
	// Samples											
	#define bkLoadSample(PAK, FILE, GROUP)				(TBSample*)bLoadResource(PAK, FILE, EBRESTYPE_SAMPLE, GROUP)
	#define bkLoadSampleByCRC(PAK, FILE, GROUP)			(TBSample*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SAMPLE, GROUP)
	#define bkLoadPackageSamples(PAK)					bLoadPackageResources(PAK, BRESMASK_SAMPLE, BDEFAULTGROUP, NULL)
	#define bkDeleteSample(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageSamples(PAKID)				bDeletePackageResources(PAKID, BRESMASK_SAMPLE)
	#define bkDeleteSampleGroup(GROUP)					bDeleteResourceGroup(BRESMASK_SAMPLE, GROUP)
	#define bkDeleteAllSamples()						bDeleteResourceGroup(BRESMASK_SAMPLE, BALLGROUPS)
	#define bkListSamples()								bkListResources(BRESMASK_SAMPLE)
														
	// Fonts											
	#define bkLoadFont(PAK, FILE, GROUP)				(TBFont*)bLoadResource(PAK, FILE, EBRESTYPE_FONT, GROUP)
	#define bkLoadFontByCRC(PAK, FILE, GROUP)			(TBFont*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_FONT, GROUP)
	#define bkLoadPackageFonts(PAK)						bLoadPackageResources(PAK, BRESMASK_FONT, BDEFAULTGROUP, NULL)
	#define bkDeleteFont(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageFonts(PAKID)					bDeletePackageResources(PAKID, BRESMASK_FONT)
	#define bkDeleteFontGroup(GROUP)					bDeleteResourceGroup(BRESMASK_FONT, GROUP)
	#define bkDeleteAllFonts()							bDeleteResourceGroup(BRESMASK_FONT, BALLGROUPS)
	#define bkListFonts()								bkListResources(BRESMASK_FONT)
													
	// StringTables									
	#define bkLoadStringTable(PAK, FILE, GROUP)			(TBStringTable*)bLoadResource(PAK, FILE, EBRESTYPE_STRINGTABLE, GROUP)
	#define bkLoadStringTableByCRC(PAK, FILE, GROUP)	(TBStringTable*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_STRINGTABLE, GROUP)
	#define bkLoadPackageStringTables(PAK)				bLoadPackageResources(PAK, BRESMASK_STRINGTABLE, BDEFAULTGROUP, NULL)
	#define bkDeleteStringTable(PTR)					bDeleteResource(PTR);
	#define bkDeletePackageStringTables(PAKID)			bDeletePackageResources(PAKID, BRESMASK_STRINGTABLE)
	#define bkDeleteStringTableGroup(GROUP)				bDeleteResourceGroup(BRESMASK_STRINGTABLE, GROUP)
	#define bkDeleteAllStringTables()					bDeleteResourceGroup(BRESMASK_STRINGTABLE, BALLGROUPS)
	#define bkListStringTables()						bkListResources(BRESMASK_STRINGTABLE)

	// Splashes
	#define bkLoadSplash(PAK, FILE, GROUP)				(TBSplash*)bLoadResource(PAK, FILE, EBRESTYPE_SPLASH, GROUP)
	#define bkLoadSplashByCRC(PAK, FILE, GROUP)			(TBSplash*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SPLASH, GROUP)
	#define bkLoadPackageSplashes(PAK)					bLoadPackageResources(PAK, BRESMASK_SPLASH, BDEFAULTGROUP, NULL)
	#define bkDeleteSplash(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageSplashes(PAKID)				bDeletePackageResources(PAKID, BRESMASK_SPLASH)
	#define bkDeleteSplashGroup(GROUP)					bDeleteResourceGroup(BRESMASK_SPLASH, GROUP)
	#define bkDeleteAllSplashes()						bDeleteResourceGroup(BRESMASK_SPLASH, BALLGROUPS)
	#define bkListSplashes()							bkListResources(BRESMASK_SPLASH)

	// Sets
	#define bkLoadSet(PAK, FILE, GROUP)					(TBSet*)bLoadResource(PAK, FILE, EBRESTYPE_SET, GROUP)
	#define bkLoadSetByCRC(PAK, FILE, GROUP)			(TBSet*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_SET, GROUP)
	#define bkLoadPackageSets(PAK)						bLoadPackageResources(PAK, BRESMASK_SET, BDEFAULTGROUP, NULL)
	#define bkDeleteSet(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageSets(PAKID)					bDeletePackageResources(PAKID, BRESMASK_SET)
	#define bkDeleteSetGroup(GROUP)						bDeleteResourceGroup(BRESMASK_SET, GROUP)
	#define bkDeleteAllSets()							bDeleteResourceGroup(BRESMASK_SET, BALLGROUPS)
	#define bkListSets()								bkListResources(BRESMASK_SET)

	// Collision Meshes
	#define bkLoadCollisionMesh(PAK, FILE, GROUP)		(TBCollisionMesh*)bLoadResource(PAK, FILE, EBRESTYPE_COLLISIONMESH, GROUP)
	#define bkLoadCollisionMeshByCRC(PAK, FILE, GROUP)	(TBCollisionMesh*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_COLLISIONMESH, GROUP)
	#define bkLoadPackageCollisionMeshes(PAK)			bLoadPackageResources(PAK, BRESMASK_COLLISIONMESH, BDEFAULTGROUP, NULL)
	#define bkDeleteCollisionMesh(PTR)					bDeleteResource(PTR);
	#define bkDeletePackageCollisionMeshes(PAKID)		bDeletePackageResources(PAKID, BRESMASK_COLLISIONMESH)
	#define bkDeleteCollisionMeshGroup(GROUP)			bDeleteResourceGroup(BRESMASK_COLLISIONMESH, GROUP)
	#define bkDeleteAllCollisionMeshes()				bDeleteResourceGroup(BRESMASK_COLLISIONMESH, BALLGROUPS)
	#define bkListCollisionMeshes()						bkListResources(BRESMASK_COLLISIONMESH)

	// Animating Collision Meshes
	#define bkLoadAnimatingCollisionMesh(PAK, FILE, GROUP)		(TBACMesh*)bLoadResource(PAK, FILE, EBRESTYPE_ANIMATINGCOLLISION, GROUP)
	#define bkLoadAnimatingCollisionMeshByCRC(PAK, FILE, GROUP)	(TBACMesh*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_ANIMATINGCOLLISION, GROUP)
	#define bkLoadPackageAnimatingCollisionMeshes(PAK)			bLoadPackageResources(PAK, BRESMASK_ANIMATINGCOLLISION, BDEFAULTGROUP, NULL)
	#define bkDeleteAnimatingCollisionMesh(PTR)					bDeleteResource(PTR);
	#define bkDeletePackageAnimatingCollisionMeshes(PAKID)		bDeletePackageResources(PAKID, BRESMASK_ANIMATINGCOLLISION)
	#define bkDeleteAnimatingCollisionMeshGroup(GROUP)			bDeleteResourceGroup(BRESMASK_ANIMATINGCOLLISION, GROUP)
	#define bkDeleteAllAnimatingCollisionMeshes()				bDeleteResourceGroup(BRESMASK_ANIMATINGCOLLISION, BALLGROUPS)
	#define bkListAnimatingCollisionMeshes()					bkListResources(BRESMASK_ANIMATINGCOLLISION)

	// Audio Streams
	#define bkLoadAudioStream(PAK, FILE, GROUP)					(TBAudioStream*)bLoadResource(PAK, FILE, EBRESTYPE_AUDIOSTREAM, GROUP)
	#define bkLoadAudioStreamByCRC(PAK, FILE, GROUP)			(TBAudioStream*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_AUDIOSTREAM, GROUP)
	#define bkLoadPackageAudioStreams(PAK)						bLoadPackageResources(PAK, BRESMASK_AUDIOSTREAM, BDEFAULTGROUP, NULL)
	#define bkDeleteAudioStream(PTR)							bDeleteResource(PTR);
	#define bkDeletePackageAudioStreams(PAKID)					bDeletePackageResources(PAKID, BRESMASK_AUDIOSTREAM)
	#define bkDeleteAudioStreamGroup(GROUP)						bDeleteResourceGroup(BRESMASK_AUDIOSTREAM, GROUP)
	#define bkDeleteAllAudioStreams()							bDeleteResourceGroup(BRESMASK_AUDIOSTREAM, BALLGROUPS)
	#define bkListAudioStreams()								bkListResources(BRESMASK_AUDIOSTREAM)

	// Lip syncing
	#define bkLoadLipSync(PAK, FILE, GROUP)						(TBLipSync*)bLoadResource(PAK, FILE, EBRESTYPE_LIPSYNC, GROUP)
	#define bkLoadLipSyncByCRC(PAK, FILE, GROUP)				(TBLipSync*)bLoadResourceByCRC(PAK, FILE, EBRESTYPE_LIPSYNC, GROUP)
	#define bkLoadPackageLipSyncs(PAK)							bLoadPackageResources(PAK, EBRESTYPE_LIPSYNC, BDEFAULTGROUP, NULL)
	#define bkDeleteLipSync(PTR)								bDeleteResource(PTR);
	#define bkDeletePackageLipSyncs(PAKID)						bDeletePackageResources(PAKID, EBRESTYPE_LIPSYNC)
	#define bkDeleteLipSyncGroup(GROUP)							bDeleteResourceGroup(EBRESTYPE_LIPSYNC, GROUP)
	#define bkDeleteAllLipSyncs()								bDeleteResourceGroup(EBRESTYPE_LIPSYNC, BALLGROUPS)
	#define bkListLipSyncs()									bkListResources(EBRESTYPE_LIPSYNC)
#endif


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bInitResources
   Purpose : initialise the resource lists
   Parameters : 
   Returns : 
   Info : 
*/

void bInitResources();


/* --------------------------------------------------------------------------------
   Function : bAddGlobalResource
   Purpose : add a new resource to the global list
   Parameters : ptr to resource, package it came from, type of resource, group ID
   Returns : 
   Info : 
*/

void bAddGlobalResource(TBResourceInfo *resPtr, TBPackageIndex *pakSrc, int type, int groupID);


/* --------------------------------------------------------------------------------
   Function : bDeleteGlobalResource
   Purpose : delete a resource from the global list
   Parameters : ptr to resource
   Returns : 
   Info : 
*/

void bDeleteGlobalResource(void *resPtr);


/* --------------------------------------------------------------------------------
   Function : bkListResources
   Purpose : list all resources that have been loaded
   Parameters : bitmask of resource types (see BRESMASK_)
   Returns : 
   Info : 
*/

void bkListResources(uint typeMask);


/* --------------------------------------------------------------------------------
   Function : bkLoadPackageResources
   Purpose : list all resources from a package
   Parameters : package index, bitmask of resource types (see BRESMASK_), group ID to assign, tag to match
   Returns : #resources loaded
   Info : 
*/

#ifdef _DEBUG
	int bLoadPackageResources(TBPackageIndex *pak, uint typeMask, int groupId, uint32 tagMatch, char *srcFile, int srcLine);
	#define bkLoadPackageResources(PAK, TYPEMASK, GROUPID, TAGMATCH)	bLoadPackageResources(PAK, TYPEMASK, GROUPID, TAGMATCH, __FILE__, __LINE__)
#else
	int bLoadPackageResources(TBPackageIndex *pak, uint typeMask, int groupId, uint32 tagMatch);
	#define bkLoadPackageResources(PAK, TYPEMASK, GROUPID, TAGMATCH)	bLoadPackageResources(PAK, TYPEMASK, GROUPID, TAGMATCH)
#endif


/* --------------------------------------------------------------------------------
   Function : bDeletePackageResources
   Purpose : delete all resources that came from a package
   Parameters : package ID, bitmask of resource types (see BRESMASK_)
   Returns : 
   Info : 
*/

#ifdef _DEBUG
	void bDeletePackageResources(TBPackageID packageId, uint typeMask, char *srcFile, int srcLine);
	#define bkDeletePackageResources(PACKAGEID, TYPEMASK)	bDeletePackageResources(PACKAGEID, TYPEMASK, __FILE__, __LINE__)
#else
	void bDeletePackageResources(TBPackageID packageId, uint typeMask);
	#define bkDeletePackageResources(PACKAGEID, TYPEMASK)	bDeletePackageResources(PACKAGEID, TYPEMASK)
#endif

/* --------------------------------------------------------------------------------
   Function : bDeleteResourceGroup
   Purpose : delete all resources with a specific group
   Parameters : bitmask of resource types (see BRESMASK_), group ID
   Returns : 
   Info : 
*/

#ifdef _DEBUG
	void bDeleteResourceGroup(uint typeMask, uint groupID, char *srcFile, int srcLine);
	#define bkDeleteResourceGroup(TYPEMASK, GROUPID)	bDeleteResourceGroup(TYPEMASK, GROUPID, __FILE__, __LINE__)
#else
	void bDeleteResourceGroup(uint typeMask, uint groupID);
	#define bkDeleteResourceGroup(TYPEMASK, GROUPID)	bDeleteResourceGroup(TYPEMASK, GROUPID)
#endif


/* --------------------------------------------------------------------------------
   Function : bDeleteAllResources
   Purpose : delete all resources
   Parameters : 
   Returns : 
   Info : 
*/

#ifdef _DEBUG
	void bDeleteAllResources(char *srcFile, int srcLine);
	#define bkDeleteAllResources()	bDeleteAllResources(__FILE__, __LINE__)
#else
	void bDeleteAllResources();
	#define bkDeleteAllResources()	bDeleteAllResources()
#endif


/* --------------------------------------------------------------------------------
   Function : bDeleteResource
   Purpose : delete a single resource
   Parameters : ptr to resource
   Returns : 
   Info : 
*/

#ifdef _DEBUG
	void bDeleteResource(void *resPtr, char *srcFile, int srcLine);
	#define bkDeleteResource(RESPTR)	bDeleteResource(RESPTR, __FILE__, __LINE__)
#else
	void bDeleteResource(void *resPtr);
	#define bkDeleteResource(RESPTR)	bDeleteResource(RESPTR)
#endif


/* --------------------------------------------------------------------------------
   Function : bLoadResource
   Purpose : load a single resource
   Parameters : ptr to package index (or NULL to search RAM), filename, resource type, group ID
   Returns : ptr to resource or NULL for failure
   Info : 
*/

#ifdef _DEBUG
	TBResourceInfo *bLoadResource(TBPackageIndex *index, char *filename, EBResourceType resType, int groupID, char *srcFile, int srcLine);
	#define bkLoadResource(INDEX, FILENAME, RESTYPE, GROUPID)	bLoadResource(INDEX, FILENAME, RESTYPE, GROUPID, __FILE__, __LINE__)
#else
	TBResourceInfo *bLoadResource(TBPackageIndex *index, char *filename, EBResourceType resType, int groupID);
	#define bkLoadResource(INDEX, FILENAME, RESTYPE, GROUPID)	bLoadResource(INDEX, FILENAME, RESTYPE, GROUPID)
#endif


/* --------------------------------------------------------------------------------
   Function : bLoadResourceByCRC
   Purpose : load a single resource from it's CRC
   Parameters : ptr to package index (or NULL to search RAM), crc, resource type, group ID
   Returns : ptr to resource or NULL for failure
   Info : 
*/

#ifdef _DEBUG
	TBResourceInfo *bLoadResourceByCRC(TBPackageIndex *index, uint crc, EBResourceType resType, int groupID, char *srcFile, int srcLine);
	#define bkLoadResourceByCRC(INDEX, CRC, RESTYPE, GROUPID)	bLoadResourceByCRC(INDEX, CRC, RESTYPE, GROUPID, __FILE__, __LINE__)
#else
	TBResourceInfo *bLoadResourceByCRC(TBPackageIndex *index, uint crc, EBResourceType resType, int groupID);
	#define bkLoadResourceByCRC(INDEX, CRC, RESTYPE, GROUPID)	bLoadResourceByCRC(INDEX, CRC, RESTYPE, GROUPID)
#endif


/* --------------------------------------------------------------------------------
   Function : bkFindResourceByCRC
   Purpose : find a loaded resource
   Parameters : resource type, crc, package id, group id
   Returns : ptr to resource or NULL for failure
   Info : 
*/

TBResourceInfo *bkFindResourceByCRC(EBResourceType resType, uint32 crc, TBPackageID packageId, uint32 groupId, uint32 flags = 0);


/* --------------------------------------------------------------------------------
   Function : bkWalkResourceList
   Purpose : walk the local resource list
   Parameters : resource type, resource to walk from/NULL to start
   Returns : ptr to next resource of that type, or NULL if none/end of list
   Info : 
*/

TBResourceInfo *bkWalkResourceList(EBResourceType resType, TBResourceInfo *resource);


/* --------------------------------------------------------------------------------
   Function : bkWalkGlobalResourceList
   Purpose : walk the global resource list
   Parameters : resource to walk from/NULL to start
   Returns : ptr to next resource, or NULL if none/end of list
   Info : 
*/

TBResourceInfo *bkWalkGlobalResourceList(TBResourceInfo *resource);


/* --------------------------------------------------------------------------------
   Function : bkTakeResourceSnapshot
   Purpose : take a snapshot of the current resource list of a specified type(s)
   Parameters : walk the global resource list
   Returns : ptr to snapshot
   Info : 
*/

TBResourceSnapshot *bkTakeResourceSnapshot(uint typeMask);


/* --------------------------------------------------------------------------------
   Function : bkDeleteResourceSnapshot
   Purpose : delete a resource snapshot
   Parameters : ptr to snapshot
   Returns : 
   Info : 
*/

void bkDeleteResourceSnapshot(TBResourceSnapshot *snapshot);


/* --------------------------------------------------------------------------------
   Function : bkCompareResourceSnapshot
   Purpose : compare a resource snapshot against the current resource list
   Parameters : snapshot
   Returns : 1=there were differences, 0=they were identical
   Info : 
*/

int bkCompareResourceSnapshot(TBResourceSnapshot *snap);


/* --------------------------------------------------------------------------------
   Function : bkIsResource
   Purpose : determine if a pointer is a valid resource
   Parameters : resource type (can be EBRESTYPE_ANY), ptr
   Returns : TRUE if a valid resource, FALSE otherwise
   Info : 
*/

int bkIsResource(EBResourceType resType, void *ptr);


/* --------------------------------------------------------------------------------
   Function : bkFindTexture
   Purpose : used to locate any loaded texture in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBTexture *bkFindTexture(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBTexture *)bkFindResourceByCRC(EBRESTYPE_TEXTURE, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindTextureByCRC
   Purpose : used to locate any loaded texture in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBTexture *bkFindTextureByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBTexture *)bkFindResourceByCRC(EBRESTYPE_TEXTURE, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindActor
   Purpose : used to locate any loaded actor in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBActor *bkFindActor(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBActor *)bkFindResourceByCRC(EBRESTYPE_ACTOR, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindActorByCRC
   Purpose : used to locate any loaded actor in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBActor *bkFindActorByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBActor *)bkFindResourceByCRC(EBRESTYPE_ACTOR, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSample
   Purpose : used to locate any loaded sample in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSample *bkFindSample(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSample *)bkFindResourceByCRC(EBRESTYPE_SAMPLE, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSampleByCRC
   Purpose : used to locate any loaded sample in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSample *bkFindSampleByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSample *)bkFindResourceByCRC(EBRESTYPE_SAMPLE, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindFont
   Purpose : used to locate any loaded font in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBFont *bkFindFont(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBFont *)bkFindResourceByCRC(EBRESTYPE_FONT, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindFontByCRC
   Purpose : used to locate any loaded font in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBFont *bkFindFontByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBFont *)bkFindResourceByCRC(EBRESTYPE_FONT, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindStringTable
   Purpose : used to locate any loaded string table in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBStringTable *bkFindStringTable(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBStringTable *)bkFindResourceByCRC(EBRESTYPE_STRINGTABLE, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindStringTableByCRC
   Purpose : used to locate any loaded string table in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBStringTable *bkFindStringTableByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBStringTable *)bkFindResourceByCRC(EBRESTYPE_STRINGTABLE, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSplash
   Purpose : used to locate any loaded splash in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSplash *bkFindSplash(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSplash *)bkFindResourceByCRC(EBRESTYPE_SPLASH, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSplashByCRC
   Purpose : used to locate any loaded splash in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSplash *bkFindSplashByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSplash *)bkFindResourceByCRC(EBRESTYPE_SPLASH, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSet
   Purpose : used to locate any loaded set in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSet *bkFindSet(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSet *)bkFindResourceByCRC(EBRESTYPE_SET, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindSetByCRC
   Purpose : used to locate any loaded set in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBSet *bkFindSetByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBSet *)bkFindResourceByCRC(EBRESTYPE_SET, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindCollisionMesh
   Purpose : used to locate any loaded collision mesh in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBCollisionMesh *bkFindCollisionMesh(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBCollisionMesh *)bkFindResourceByCRC(EBRESTYPE_COLLISIONMESH, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindCollisionMeshByCRC
   Purpose : used to locate any loaded collision mesh in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBCollisionMesh *bkFindCollisionMeshByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBCollisionMesh *)bkFindResourceByCRC(EBRESTYPE_COLLISIONMESH, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindAnimatingCollisionMesh
   Purpose : used to locate any loaded animating collision mesh in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBACMesh *bkFindAnimatingCollisionMesh(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBACMesh *)bkFindResourceByCRC(EBRESTYPE_ANIMATINGCOLLISION, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindAnimatingCollisionMeshByCRC
   Purpose : used to locate any loaded animating collision mesh in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBACMesh *bkFindAnimatingCollisionMeshByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBACMesh *)bkFindResourceByCRC(EBRESTYPE_ANIMATINGCOLLISION, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindAudioStream
   Purpose : used to locate any loaded audio stream in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBAudioStream *bkFindAudioStream(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBAudioStream *)bkFindResourceByCRC(EBRESTYPE_AUDIOSTREAM, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindAudioStreamByCRC
   Purpose : used to locate any loaded audio stream in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBAudioStream *bkFindAudioStreamByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBAudioStream *)bkFindResourceByCRC(EBRESTYPE_AUDIOSTREAM, crc, pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindLipSync
   Purpose : used to locate any loaded lip sync in memory
   Parameters : filename, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBLipSync *bkFindLipSync(char *filename, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBLipSync *)bkFindResourceByCRC(EBRESTYPE_LIPSYNC, bkStringCRC(filename), pak, group, flags);
}


/* --------------------------------------------------------------------------------
   Function : bkFindLipSyncByCRC
   Purpose : used to locate any loaded lip sync in memory
   Parameters : CRC, package IDs, resource group, BRESFLAG_xxx
   Returns : ptr to resource or NULL
   Info : 
*/
inline TBLipSync *bkFindLipSyncByCRC(uint32 crc, TBPackageID pak, uint32 group, uint32 flags = 0)
{
	return (TBLipSync *)bkFindResourceByCRC(EBRESTYPE_LIPSYNC, crc, pak, group, flags);
}


#endif		// __BKERNEL_RESOURCES_H__
