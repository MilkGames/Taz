#ifndef __H_OBJECTVIEWER
#define __H_OBJECTVIEWER

#define ZOOM_INC (0.25f)
#define ZOOM_MIN (1.0f)		// will be redundant when bounding box measurements are available

/*********************> structures *<*/

// structure to hold data for points on measuring grid
typedef struct _ViewerSpriteInfo
{
	TBTexture			*texture;
	float				xDim;
	float				yDim;
	uint32				pad;
} ViewerSpriteInfo BALIGN16;

extern ACTORINSTANCE			*tazObjectViewerInstance;


/* --------------------------------------------------------------------------------
   Function : InitObjectViewer
   Purpose : initialises object viewer instance list
   Parameters : 
   Returns : 
   Info : 
*/

void InitObjectViewer( void );

/* --------------------------------------------------------------------------------
   Function : DrawObjectViewer
   Purpose : draws selected objects
   Parameters : 
   Returns : 
   Info : 
*/

void DrawObjectViewer( TBFont *standardFont );

/* --------------------------------------------------------------------------------
   Function : PrepViewerForUpdate
   Purpose : prepares actor instance for drawing
   Parameters : 
   Returns : 
   Info : 
*/

void PrepViewerForUpdate( void );

/* --------------------------------------------------------------------------------
   Function : UpdateObjectViewer
   Purpose : updates object viewer, dependant on pad inputs
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateObjectViewer( void );

/* --------------------------------------------------------------------------------
   Function : FreeObjectViewerActorInstanceList
   Purpose : frees object viewer instance list
   Parameters : 
   Returns : 
   Info : 
*/

void FreeObjectViewer( void );

/* --------------------------------------------------------------------------------
   Function : SearchForActorNodes
   Purpose : recursively searches for TBActorNodes
   Parameters : TBActorNode
   Returns : 
   Info : 
*/

void SearchForActorNodes( TBActorNode *node );

/* --------------------------------------------------------------------------------
   Function : InitViewerGrid
   Purpose : initialises grid
   Parameters : 
   Returns : 
   Info : 
*/

void InitViewerGrid( void );

/* --------------------------------------------------------------------------------
   Function : DrawViewerGrid
   Purpose : draws grid
   Parameters : 
   Returns : 
   Info : 
*/

void DrawViewerGrid( void );

/* --------------------------------------------------------------------------------
   Function : DrawZones
   Purpose : draws zones (yMin, maxRadius, etc...)
   Parameters : 
   Returns : 
   Info : 
*/

void DrawZones( void );

/* --------------------------------------------------------------------------------
   Function : DrawViewerGround
   Purpose : draws ground
   Parameters : 
   Returns : 
   Info : 
*/

void DrawViewerGround( void );

/* --------------------------------------------------------------------------------
   Function : CalcViewerObjectTextureInfo
   Purpose : calculates required texture info for primitive
   Parameters : TBTexture *
   Returns : primitiveTextureSize
   Info : 
*/

int CalcViewerObjectTextureInfo( TBTexture * );

/* --------------------------------------------------------------------------------
   Function : DrawViewerTextures
   Purpose : draws the currently used textures
   Parameters :
   Returns :
   Info : 
*/

void DrawViewerTextures( TBFont * );

#endif