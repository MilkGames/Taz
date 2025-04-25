// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcxbStockExtras.h
//   Purpose : stock extra effects
// Component : PC/Xbox Actor
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BACTOR_PCSTOCKEXTRAS_H__
#define __BACTOR_PCSTOCKEXTRAS_H__


// *********************************************************************************
// Sparkle
// *********************************************************************************

#define BSTOCKEXTRASYS_SPARKLE(NOOF) "sparkle", bCreateSparkle, bUpdateSparkle, NULL, bRenderSparkle, bSysInitSparkle, sizeof(TBSparkleInstanceData), NOOF, 0, sizeof(TBSparkleSystemData)

// instance data block
typedef struct _TBSparkleInstanceData {
	TBVector			position;					// position
	TBVector			velocity;					// velocity
	TBVector			acceleration;				// acceleration
	short			r,g,b, a;					// colour and alpha
	int32			aDelta;						// alpha delta per update
	float			size, sizeDelta;			// size and size delta per update
	TBTexture		*texture;					// texture ptr
} TBSparkleInstanceData;


// system data block
typedef struct _TBSparkleSystemData {
	TBVertexBuffer	*vertexBuffer;				// vertex buffer for sprite vertices
} TBSparkleSystemData;


/*	--------------------------------------------------------------------------------
	Function : bCreateSparkle
	Purpose : create a new extra instance
	Parameters : system ptr, data block, parameters
	Returns : 
	Info : 
*/

void bCreateSparkle(TBExtraSystem *eSystem, uchar *vData, va_list parms);


/*	--------------------------------------------------------------------------------
	Function : bUpdateSparkle
	Purpose : update instances for this system
	Parameters : system ptr, #instances, data ptr array, 't' value
	Returns : 
	Info : 
*/

void bUpdateSparkle(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t);


/*	--------------------------------------------------------------------------------
	Function : bRenderSparkle
	Purpose : render instances for this system
	Parameters : system ptr, #instances, data ptr array, context value
	Returns : 
	Info : 
*/

void bRenderSparkle(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context);


/*	--------------------------------------------------------------------------------
	Function : bSysInitSparkle
	Purpose : system setup function
	Parameters : system ptr, reason code
	Returns : 
	Info : 
*/

void bSysInitSparkle(TBExtraSystem *eSystem, int reason);

// *********************************************************************************
// end of Sparkle
// *********************************************************************************

#endif		// __BACTOR_PCSTOCKEXTRAS_H__
