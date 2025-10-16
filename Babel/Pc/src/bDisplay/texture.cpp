// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : texture.cpp
//   Purpose : Defines texture unlock flags, texture data formats, texture flags, and texture callback types
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bdGetTextureFrameByTime
	Purpose : get texture frame by time
	Parameters : texturePtr, milliseconds
	Returns : frame
	Info : 
*/
int bdGetTextureFrameByTime(struct _TBTexture *texturePtr, int milliseconds)
{
        bkPrintf("*** WARNING *** bdGetTextureFrameByTime was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTextureFrameByTime
	Purpose : set texture frame by time
	Parameters : texturePtr, milliseconds
	Returns : frame
	Info : 
*/
int bdSetTextureFrameByTime(struct _TBTexture *texturePtr, int milliseconds)
{
        bkPrintf("*** WARNING *** bdSetTextureFrameByTime was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bdSetTextureCallback
	Purpose : set texture callback
	Parameters : texture, cbType, callback, context
	Returns : 
	Info : 
*/
void bdSetTextureCallback(struct _TBTexture *texture, EBTextureCallback cbType, TBTextureCallback callback, void *context)
{
        bkPrintf("*** WARNING *** bdSetTextureCallback was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}
