#ifndef __H_THOUGHTBUBBLE
#define __H_THOUGHTBUBBLE

#define MAX_THOUGHTBUBBLE_ITEMS (5)

enum EThoughtStage
{
	THOUGHTBUBBLE_OPENING,
	THOUGHTBUBBLE_OPEN,
	THOUGHTBUBBLE_CLOSING,
	THOUGHTBUBBLE_CLOSED,
};

typedef struct TAG_THOUGHTBUBBLEITEM
{
	TBActorInstance	actorInstance;
	TBTexture		texture;

	float			x,y;
	float			scale;
	char			active;		// active or not?
	char			pad[3];
	uint32			pad2;
} THOUGHTBUBBLEITEM BALIGN16;


typedef void (*PrintTextCallback)(TBMatrix objectMatrix, /*PP:*/ void* const context);

typedef struct TAG_THOUGHTBUBBLE
{
	TBActorInstance		bubbleSmall;					// bubble 1
	TBActorInstance		bubbleMedium;					// bubble 2
	TBActorInstance		bubbleLarge;					// bubble 3
	TBVector			offsetFromActor;				// bubble offset from character position
	void*				context;						// PP: context pointer that gets passed through to callbacks

	THOUGHTBUBBLEITEM	items[MAX_THOUGHTBUBBLE_ITEMS];	// items contained in bubble
	
	float				t;								// t value of thought 0 - nothing 1 - full open bubble
	char				stage;							// current state of bubble
	char				pad[3];
	uint32				pad2[1];						// PP: was [2] - till I added the 'context' member

	PrintTextCallback	PrintText;						// print text callback
	uint32				pad3[3];
} THOUGHTBUBBLE BALIGN16;

/* --------------------------------------------------------------------------------
   Function : InitThoughtBubbleSystem
   Purpose : Load the objects and textures needed to provide thought bubble support
   Parameters : package containing the objects and textures
   Returns : TRUE/FALSE
   Info : 
*/
int InitThoughtBubbleSystem(char *pakfile);

/* --------------------------------------------------------------------------------
   Function : InitThoughtBubble
   Purpose : Initialise a thought bubble
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void InitThoughtBubble(THOUGHTBUBBLE *thoughtBubble);

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
						void* const contextIn=NULL);

/* --------------------------------------------------------------------------------
   Function : CloseThoughtBubble
   Purpose : Start the thought bubble closing
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void CloseThoughtBubble(THOUGHTBUBBLE *thoughtBubble);

/* --------------------------------------------------------------------------------
   Function : AddObjectToThoughtBubble
   Purpose : Add an object to a thought bubble
   Parameters : THOUGHTBUBBLE, object name, xpos 0-1024, ypos 0-1024, scale
   Returns : handle number
   Info : 
*/
int	AddObjectToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *objectName, float	xPos, float yPos, float scale);

/* --------------------------------------------------------------------------------
   Function : AddObjectToThoughtBubble
   Purpose : Add an object to a thought bubble
   Parameters : THOUGHTBUBBLE, object name, xpos relative to center of bubble, ypos , scale
   Returns : handle number of -1 error
   Info : 
*/
int	AddObjectToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *objectName, float	xPos, float yPos, float scale, TBQuaternion quat);

/* --------------------------------------------------------------------------------
   Function : AddTextureToThoughtBubble
   Purpose : Add an texture to a thought bubble
   Parameters : THOUGHTBUBBLE, texture name, xpos 0-1024, ypos 0-1024, scale
   Returns : handle number
   Info : 
*/
int AddTextureToThoughtBubble(THOUGHTBUBBLE *thoughtBubble, char *textureName, float xPos, float yPos, float scale);

/* --------------------------------------------------------------------------------
   Function : PrepThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE, CAMERAINFO, position
   Returns : 
   Info : 
*/
void PrepThoughtBubble(THOUGHTBUBBLE *thoughtBubble, struct TAG_CAMERAINFO *cameraInfo, TBVector tpos);

/* --------------------------------------------------------------------------------
   Function : DrawThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE, CAMERAINFO *cameraInfo
   Returns : 
   Info : 
*/
void DrawThoughtBubble(THOUGHTBUBBLE *thoughtBubble, struct TAG_CAMERAINFO *cameraInfo);

/* --------------------------------------------------------------------------------
   Function : DrawThoughtBubble
   Purpose : General update of thought bubble
   Parameters : THOUGHTBUBBLE, 3d world position to draw bubble at
   Returns : 
   Info : 
*/
void RemoveItemFromThoughtBubble(THOUGHTBUBBLE *thoughtBubble, int itemHandle);

/* --------------------------------------------------------------------------------
   Function : DestroyThoughtBubble
   Purpose : Destroy a thought bubble and all items within
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void DestroyThoughtBubble(THOUGHTBUBBLE *thoughtBubble);

/* --------------------------------------------------------------------------------
   Function : DestroyThoughtBubbleSystem
   Purpose : Shuts down the thought bubble system
   Parameters : 
   Returns : 
   Info : 
*/
void DestroyThoughtBubbleSystem(void);

/* --------------------------------------------------------------------------------
   Function : SetUpBubbles
   Purpose : Set the positions etc of the thought bubbles
   Parameters : THOUGHTBUBBLE
   Returns : 
   Info : 
*/
void SetUpBubbles(THOUGHTBUBBLE *thoughtBubble, TBVector tpos);

/* --------------------------------------------------------------------------------
   Function : DrawBubbles
   Purpose : Draw relevent bubbles
   Parameters : THOUGHTBUBBLE, CAMERAINFO
   Returns : 
   Info : 
*/
void DrawBubbles(THOUGHTBUBBLE *thoughtBubble,struct TAG_CAMERAINFO *cameraInfo);


#endif