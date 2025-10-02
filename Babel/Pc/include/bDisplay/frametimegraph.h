// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : frametimegraph.h
//   Purpose : encapsulated graph of frame time
// Component : Generic Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BDISPLAY_FRAMETIMEGRAPH_H__
#define __BDISPLAY_FRAMETIMEGRAPH_H__


// ********************************************************************************
// Constants and Macros

// types of memory graph
typedef enum {
	EBMEMORYGRAPHTYPE_BABEL,			// graph Babel memory
	EBMEMORYGRAPHTYPE_SYSTEM,			// graph System memory
} EBMemoryGraphType;


// ********************************************************************************
// Prototypes

/* --------------------------------------------------------------------------------
   Function : bdSetGraphMarker
   Purpose : mark this frame on any enabled graphs
   Parameters : colour of marker
   Returns : 
   Info : 
*/

void bdSetGraphMarker(int r=200, int g=200, int b=200);


/* --------------------------------------------------------------------------------
   Function : bdEnableFrameTimeGraph
   Purpose : enable/disable the frame time graph
   Parameters : TRUE to enable, FALSE to disable
   Returns : 
   Info : 
*/

void bdEnableFrameTimeGraph(int enable);


/* --------------------------------------------------------------------------------
   Function : bdEnableMemoryGraph
   Purpose : enable/disable the memory graph
   Parameters : TRUE to enable or FALSE to disable, graph type, Kb per marker line, baseline in Kb
   Returns : 
   Info : 
*/

void bdEnableMemoryGraph(int enable, EBMemoryGraphType graphType=EBMEMORYGRAPHTYPE_BABEL, int markerKb=5120, int baselineKb=0);


/* --------------------------------------------------------------------------------
   Function : bRenderGraphs
   Purpose : render any enabled graphs
   Parameters : 
   Returns : 
   Info : 
*/

void bRenderGraphs();

#endif		// __BDISPLAY_FRAMETIMEGRAPH_H__
