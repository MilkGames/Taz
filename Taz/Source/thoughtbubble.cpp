// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : thoughtbubble.cpp
//   Purpose : functions to create update and draw thought bubbles.
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"

#include "display.h"
#include "main.h"
#include "camera.h"
#include "thoughtbubble.h"

#define BUBBLE_RADIUS	(1.0f*240.0f)

/* --------------------------------------------------------------------------------
   Function : InitThoughtBubbleSystem
   Purpose : Load the objects and textures needed to provide thought bubble support
   Parameters : package containing the objects and textures
   Returns : TRUE/FALSE
   Info : 
*/
int InitThoughtBubbleSystem(char *pakfile)
{
	if (!LoadActor("bubble.obe",pakfile)) return FALSE;
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : InitThoughtBubble
   Purpose : Initialise a thought bubble
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void InitThoughtBubble(THOUGHTBUBBLE *thoughtBubble)
{
	int i;

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		thoughtBubble->items[i].active = FALSE;
	}
	thoughtBubble->stage = THOUGHTBUBBLE_CLOSED;
	thoughtBubble->t = 0.0f;
}

/* --------------------------------------------------------------------------------
   Function : CreateThoughtBubble
   Purpose : Create and start a thought bubble instance
   Parameters : THOUGHTBUBBLE, fullsizeX, fullsizeY, max number of items it can contain,
				ptr to text-printing callback, // PP: (opt/NULL)context pointer to be passed
				to callbacks
   Returns : TRUE/FALSE
   Info : 
*/
int CreateThoughtBubble(THOUGHTBUBBLE *thoughtBubble,
						TBVector offset,
						float fullSizeX,
						float fullSizeY,
						int maxItems,
						PrintTextCallback PrintText,
						void* const contextIn)
{
	int	i;
	TBActor *bubble;

	if (thoughtBubble->stage != THOUGHTBUBBLE_CLOSED) return FALSE;

	bubble = FindActorInActorList("bubble.obe");
	if (!bubble) return FALSE;

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		thoughtBubble->items[i].active = FALSE;
	}
	thoughtBubble->stage = THOUGHTBUBBLE_OPENING;
	thoughtBubble->t = 0.0f;

	// PP: store the context value used for the text callback
	thoughtBubble->context=contextIn;

	bmVectorCopy(thoughtBubble->offsetFromActor,offset);

	baInitActorInstance(&thoughtBubble->bubbleSmall, bubble, 5);
	baPrepActorInstance(&thoughtBubble->bubbleSmall,TRUE);
	thoughtBubble->bubbleSmall.scale[X] = 0.04f;
	thoughtBubble->bubbleSmall.scale[Y] = 0.04f;
	thoughtBubble->bubbleSmall.scale[Z] = 0.04f;
	baInitActorInstance(&thoughtBubble->bubbleMedium, bubble, 5);
	baPrepActorInstance(&thoughtBubble->bubbleMedium,TRUE);
	thoughtBubble->bubbleMedium.scale[X] = 0.07f;
	thoughtBubble->bubbleMedium.scale[Y] = 0.07f;
	thoughtBubble->bubbleMedium.scale[Z] = 0.07f;
	baInitActorInstance(&thoughtBubble->bubbleLarge, bubble, 5);
	baPrepActorInstance(&thoughtBubble->bubbleLarge,TRUE);
	thoughtBubble->bubbleLarge.scale[X] = ((fullSizeX)/2.0f) / BUBBLE_RADIUS;
	thoughtBubble->bubbleLarge.scale[Y] = ((fullSizeY)/2.0f) / BUBBLE_RADIUS;
	thoughtBubble->bubbleLarge.scale[Z] = 0.2f;

	thoughtBubble->PrintText = PrintText;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CloseThoughtBubble
   Purpose : Start the thought bubble closing
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void CloseThoughtBubble(THOUGHTBUBBLE *thoughtBubble)
{
	if (thoughtBubble->stage != THOUGHTBUBBLE_CLOSED)
	{
		thoughtBubble->stage = THOUGHTBUBBLE_CLOSING;
	}
}

/* --------------------------------------------------------------------------------
   Function : AddObjectToThoughtBubble
   Purpose : Add an object to a thought bubble
   Parameters : THOUGHTBUBBLE, object name, xpos relative to center of bubble, ypos , scale
   Returns : handle number of -1 error
   Info : 
*/
int	AddObjectToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *objectName, float	xPos, float yPos, float scale)
{
	int i;
	TBActor *actor;

	actor = FindActorInActorList(objectName);
	if (actor==NULL) return -1;

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		if (thoughtBubble->items[i].active==FALSE)
		{
			baInitActorInstance(&thoughtBubble->items[i].actorInstance, actor, 5);
			thoughtBubble->items[i].actorInstance.scale[X] = 
				thoughtBubble->items[i].actorInstance.scale[Y] = 
				thoughtBubble->items[i].actorInstance.scale[Z] = scale;
			baPrepActorInstance(&thoughtBubble->items[i].actorInstance,TRUE);
			EnableActorCartoonRenderMode(&thoughtBubble->items[i].actorInstance);
			thoughtBubble->items[i].x = xPos;
			thoughtBubble->items[i].y = yPos;
			thoughtBubble->items[i].scale = scale;
			thoughtBubble->items[i].active = TRUE;
			return i;
		}
	}
	return -1;
}

/* --------------------------------------------------------------------------------
   Function : AddObjectToThoughtBubble
   Purpose : Add an object to a thought bubble
   Parameters : THOUGHTBUBBLE, object name, xpos relative to center of bubble, ypos , scale
   Returns : handle number of -1 error
   Info : 
*/
int	AddObjectToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *objectName, float	xPos, float yPos, float scale, TBQuaternion quat)
{
	int i;
	TBActor *actor;

	actor = FindActorInActorList(objectName);
	if (actor==NULL) return -1;

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		if (thoughtBubble->items[i].active==FALSE)
		{
			baInitActorInstance(&thoughtBubble->items[i].actorInstance, actor, 5);
			thoughtBubble->items[i].actorInstance.scale[X] = 
				thoughtBubble->items[i].actorInstance.scale[Y] = 
				thoughtBubble->items[i].actorInstance.scale[Z] = scale;
			bmQuatCopy(thoughtBubble->items[i].actorInstance.orientation,quat);
			baPrepActorInstance(&thoughtBubble->items[i].actorInstance,TRUE);
			EnableActorCartoonRenderMode(&thoughtBubble->items[i].actorInstance);
			thoughtBubble->items[i].x = xPos;
			thoughtBubble->items[i].y = yPos;
			thoughtBubble->items[i].scale = scale;
			thoughtBubble->items[i].active = TRUE;
			return i;
		}
	}
	return -1;
}

/* --------------------------------------------------------------------------------
   Function : AddTextureToThoughtBubble
   Purpose : Add an texture to a thought bubble
   Parameters : THOUGHTBUBBLE, texture name, xpos 0-1024, ypos 0-1024, scale
   Returns : handle number
   Info : 
*/
int AddTextureToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *textureName, float xPos, float yPos, float scale)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : PrepThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE, CAMERAINFO, position
   Returns : 
   Info : 
*/
void PrepThoughtBubble(THOUGHTBUBBLE *thoughtBubble, CAMERAINFO *cameraInfo, TBVector tpos)
{
	TBVector	pos,temp;

	if (thoughtBubble->stage!=THOUGHTBUBBLE_CLOSED)
	{
		bmVectorAdd(pos,tpos,thoughtBubble->offsetFromActor);
		bmVectorCopy(temp,pos);
		temp[W] = 1.0f;
		bdXFormWorldToView(pos,temp,1);
	}

	switch(thoughtBubble->stage)
	{
	case THOUGHTBUBBLE_CLOSED:
		return;
		break;
	case THOUGHTBUBBLE_OPENING:

		thoughtBubble->t += 2.0f/fps;
		if (thoughtBubble->t >= 1.0f)
		{
			thoughtBubble->t = 1.0f;
			thoughtBubble->stage = THOUGHTBUBBLE_OPEN;
			break;
		}

		SetUpBubbles(thoughtBubble,pos);
		break;
	case THOUGHTBUBBLE_OPEN:
		SetUpBubbles(thoughtBubble,pos);
		break;
	case THOUGHTBUBBLE_CLOSING:
		SetUpBubbles(thoughtBubble,pos);

		thoughtBubble->t -= 2.0f/fps;
		if (thoughtBubble->t <= 0.0f)
		{
			thoughtBubble->t = 0.0f;
			DestroyThoughtBubble(thoughtBubble);
		}
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : SetUpBubbles
   Purpose : Set the positions etc of the thought bubbles
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void SetUpBubbles(THOUGHTBUBBLE *thoughtBubble, TBVector tpos)
{
	int	i;
	TBVector pos;
	TBVector	offset[3] = {{20.0f,10.0f,0.0f},{50.0f,40.0f,0.0f},{200.0f,150.0f,0.0f}};

	bmVectorAdd(pos,tpos,offset[0]);
	bmVectorCopy(thoughtBubble->bubbleSmall.position,pos);
	baUpdateNodes(&thoughtBubble->bubbleSmall, thoughtBubble->bubbleSmall.rootNodeInstance, speedRatio, NULL);
	baPrepActorInstance(&thoughtBubble->bubbleSmall,TRUE);
	if (thoughtBubble->t >= 0.3f)
	{
		bmVectorAdd(pos,tpos,offset[1]);
		bmVectorCopy(thoughtBubble->bubbleMedium.position,pos);
		baUpdateNodes(&thoughtBubble->bubbleMedium, thoughtBubble->bubbleMedium.rootNodeInstance, speedRatio, NULL);
		baPrepActorInstance(&thoughtBubble->bubbleMedium,TRUE);
	}
	if (thoughtBubble->t >= 0.6f)
	{
		bmVectorAdd(pos,tpos,offset[2]);
		bmVectorCopy(thoughtBubble->bubbleLarge.position,pos);
		baUpdateNodes(&thoughtBubble->bubbleLarge, thoughtBubble->bubbleLarge.rootNodeInstance, speedRatio, NULL);
		baPrepActorInstance(&thoughtBubble->bubbleLarge,TRUE);
	}

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		if (thoughtBubble->items[i].active)
		{
			bmVectorAdd(pos,tpos,offset[2]);
			pos[X] += thoughtBubble->items[i].x;
			pos[Y] += thoughtBubble->items[i].y;
			bmVectorCopy(thoughtBubble->items[i].actorInstance.position,pos);
			baUpdateNodes(&thoughtBubble->items[i].actorInstance, thoughtBubble->items[i].actorInstance.rootNodeInstance, speedRatio, NULL);
			baPrepActorInstance(&thoughtBubble->items[i].actorInstance,TRUE);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void DrawThoughtBubble(THOUGHTBUBBLE *thoughtBubble,CAMERAINFO *cameraInfo)
{
	switch(thoughtBubble->stage)
	{
	case THOUGHTBUBBLE_CLOSED:
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		return;
		break;
	case THOUGHTBUBBLE_OPENING:
		DrawBubbles(thoughtBubble,cameraInfo);
		break;
	case THOUGHTBUBBLE_OPEN:
		DrawBubbles(thoughtBubble,cameraInfo);
		break;
	case THOUGHTBUBBLE_CLOSING:
		DrawBubbles(thoughtBubble,cameraInfo);
		break;
	}
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, FALSE);
}

/* --------------------------------------------------------------------------------
   Function : DrawBubbles
   Purpose : Draw relevent bubbles
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void DrawBubbles(THOUGHTBUBBLE *thoughtBubble,CAMERAINFO *cameraInfo)
{
	int i;

	bdSetViewPosition(0.0f,0.0f,0.0f);
	bdSetViewOrientation(0,0,0,BVPROTATE_XYZ);
	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane/2);
	bdSetupView();
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	baSetGlobalAlpha(200);

	baDrawActorInstance(&thoughtBubble->bubbleSmall,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
	if (thoughtBubble->t >= 0.3f)
	{
		baDrawActorInstance(&thoughtBubble->bubbleMedium,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
	}
	if (thoughtBubble->t >= 0.6f)
	{
		baDrawActorInstance(&thoughtBubble->bubbleLarge,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
		bdSetupView();
		for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
		{
			if (thoughtBubble->items[i].active)
			{
				baDrawActorInstance(&thoughtBubble->items[i].actorInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
			}
		}
		if (thoughtBubble->PrintText)
		{
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetObjectMatrix(thoughtBubble->bubbleLarge.objectToWorld);
			thoughtBubble->PrintText(thoughtBubble->bubbleLarge.objectToWorld, thoughtBubble->context);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		}
	}
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	SendCamera(cameraInfo);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	baSetGlobalAlpha(256);
	bdSetupView();
}


/* --------------------------------------------------------------------------------
   Function : DrawThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE, 3d world position to draw bubble at
   Returns : 
   Info : 
*/
void RemoveItemFromThoughtBubble(THOUGHTBUBBLE *thoughtBubble, int itemHandle)
{
	if ((itemHandle<0)||(itemHandle>=MAX_THOUGHTBUBBLE_ITEMS)) return;

	if (thoughtBubble->items[itemHandle].active == TRUE)
	{
		baFreeActorInstance(&thoughtBubble->items[itemHandle].actorInstance);
		thoughtBubble->items[itemHandle].active = FALSE;
	}
}

/* --------------------------------------------------------------------------------
   Function : DestroyThoughtBubble
   Purpose : Destroy a thought bubble and all items within and free memory
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void DestroyThoughtBubble(THOUGHTBUBBLE *thoughtBubble)
{
	int i;

	if (thoughtBubble->stage==THOUGHTBUBBLE_CLOSED) return;

	for (i=0;i<MAX_THOUGHTBUBBLE_ITEMS;i++)
	{
		if (thoughtBubble->items[i].active)
		{
			RemoveItemFromThoughtBubble(thoughtBubble,i);
		}
	}

	baFreeActorInstance(&thoughtBubble->bubbleSmall);
	baFreeActorInstance(&thoughtBubble->bubbleMedium);
	baFreeActorInstance(&thoughtBubble->bubbleLarge);
	thoughtBubble->stage = THOUGHTBUBBLE_CLOSED;
	return;
}

/* --------------------------------------------------------------------------------
   Function : DestroyThoughtBubbleSystem
   Purpose : Shuts down the thought bubble system
   Parameters : 
   Returns : 
   Info : 
*/
void DestroyThoughtBubbleSystem(void)
{
}
