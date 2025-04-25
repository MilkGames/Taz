// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbAnim.h
//   Purpose : animation code
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_PCANIM_H__
#define __BACTOR_PCANIM_H__

// ********************************************************************************
// Constants and Macros

// animation key types
#define BAKEYTYPE_NULL			0
#define BAKEYTYPE_LINEAR		1			// linear key
#define BAKEYTYPE_TCB			2			// Tension Continuity Basis curve


// ********************************************************************************
// Types, Structures and Classes

// a three-element key
typedef struct _TBAnimKey3 {
	ushort				time;						
	ushort				x,y,z;
} TBAnimKey3;


// a three-element keyframe track
typedef struct _TBAnimTrack3 {
	ushort				keyType;					// type of keys
	ushort				noofKeys;					// number of keys on track
	TBAnimKey3			*keys;						// key array
	float				xQuantBase;					// X quantisation base value
	float				yQuantBase;					// Y quantisation base value

	float				zQuantBase;					// Z quantisation base value
	float				xQuantScale;				// X quantisation scale
	float				yQuantScale;				// Y quantisation scale
	float				zQuantScale;				// Z quantisation scale
} TBAnimTrack3;


// a four-element key
#pragma pack(push, 2)
typedef struct _TBAnimKey4 {
	ushort				time;						// time for this key
	ushort				x,y,z, w;					// value
} TBAnimKey4;
#pragma pack(pop)


// a three-element keyframe track
typedef struct _TBAnimTrack4 {
	int32				keyType;					// type of keys
	int32				noofKeys;					// number of keys on track
	TBAnimKey4			*keys;						// key array
	uint32				pad;

	float				xQuantBase;					// X quantisation base value
	float				yQuantBase;					// Y quantisation base value
	float				zQuantBase;					// Z quantisation base value
	float				wQuantBase;					// Z quantisation base value

	float				xQuantScale;				// X quantisation scale
	float				yQuantScale;				// Y quantisation scale
	float				zQuantScale;				// Z quantisation scale
	float				wQuantScale;				// W quantisation scale
} TBAnimTrack4;


// saved animation state
typedef struct _TBAnimState {
	TBVector				position;
	TBVector				scale;
	TBVector				orientation;
} TBAnimState;


// ********************************************************************************
// Globals



// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : baEvalAnimTrack3
	Purpose : evaluate an three-element animation track for a time
	Parameters : vector to receive result, track ptr, time
	Returns : 
	Info : 
*/

void baEvalAnimTrack3(TBVector destVec, TBAnimTrack3 *track, int time);


/*	--------------------------------------------------------------------------------
	Function : baEvalAnimTrack4
	Purpose : evaluate an three-element animation track for a time
	Parameters : vector to receive result, track ptr, time
	Returns : 
	Info : 
*/

void baEvalAnimTrack4(TBVector destVec, TBAnimTrack4 *track, int time);

#endif		// __BACTOR_PCANIM_H__
