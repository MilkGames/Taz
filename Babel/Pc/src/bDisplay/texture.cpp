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
  byte bVar1;
  uint32 uVar2;
  int iVar3;
  int frameNumber;
  int *piVar4;
  
  uVar2 = texturePtr->loaderTag;
  if (uVar2 == 0) {
    iVar3 = bdSetTextureFrame(texturePtr,0);
    return iVar3;
  }
  bVar1 = texturePtr->noofFrames;
  if (bVar1 < 2) {
    iVar3 = bdSetTextureFrame(texturePtr,0);
    return iVar3;
  }
  if (milliseconds < 0) {
    milliseconds = uVar2 - -milliseconds % (int)uVar2;
  }
  iVar3 = 0;
  frameNumber = 0;
  if (bVar1 != 0) {
    piVar4 = (int *)texturePtr->frameMeta;
    do {
      iVar3 = iVar3 + *piVar4;
      if (milliseconds % (int)uVar2 < iVar3) {
        bdSetTextureFrame(texturePtr,frameNumber);
        iVar3 = bdSetTextureFrame(texturePtr,frameNumber);
        return iVar3;
      }
      frameNumber = frameNumber + 1;
      piVar4 = piVar4 + 1;
    } while (frameNumber < (int)(uint)bVar1);
  }
  iVar3 = bdSetTextureFrame(texturePtr,(uint)texturePtr->currentFrame);
  return iVar3;
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
