// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : frametimegraph.cpp
//   Purpose : Defines an enum for memory graph types. Declares functions for setting graph markers and enabling/disabling frame time and memory graphs
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

int stopSpam3 = 0;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdSetGraphMarker
	Purpose : set graph marker
	Parameters : r, g, b
	Returns : 
	Info : 
*/
void bdSetGraphMarker(int r, int g, int b)
{
        bkPrintf("*** WARNING *** bdSetGraphMarker was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdEnableFrameTimeGraph
	Purpose : enable/disable frame time graph
	Parameters : enable
	Returns : 
	Info : 
*/
void bdEnableFrameTimeGraph(int enable)
{
        bkPrintf("*** WARNING *** bdEnableFrameTimeGraph was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bdEnableMemoryGraph
	Purpose : enable/disable memory graph
	Parameters : enable, graphType, markerKb, baselineKb
	Returns : 
	Info : 
*/
void bdEnableMemoryGraph(int enable, EBMemoryGraphType graphType, int markerKb, int baselineKb)
{
        bkPrintf("*** WARNING *** bdEnableMemoryGraph was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bRenderGraphs
	Purpose : render graphs
	Parameters : 
	Returns : 
	Info : 
*/
void bRenderGraphs()
{
	if (!stopSpam3) {
        bkPrintf("*** WARNING *** bRenderGraphs was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
		stopSpam3++;
	}
    return;
}
