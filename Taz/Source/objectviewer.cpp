// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : objectviewer.cpp
//   Purpose : functions to display all objects in a scene
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// tazObjectViewerInstance->actorInstance.actor->yMin
// tazObjectViewerInstance->actorInstance.actor->maxRadius

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collision.h"
#include "collisioncache.h"
#include "physics.h"
#include "camera.h"
#include "objectviewer.h"
#include "maths.h"
#include "control.h"
#include "display.h"
#include "animation.h"
#include "characters.h"
#include "lights.h"
#include "main.h"
#include "costumes.h"
#include "files.h"

#include "rectangle.h"
#include "display.h"
#include "colour.h"

#include "philscene.h"			// PP: phil-specific stuff

//#define DEBUG_TEXT
//#define ROOT_OBJECTS
#define DEVELOPERS_VERSION

#if BPLATFORM==PC

#define	FORMAT(ptr)	((ptr)->format)
#define XDIM(ptr) ((ptr)->xDim)
#define YDIM(ptr) ((ptr)->yDim)

#define BTEXTUREFORMAT_1555ARGB  BTEXTUREFORMAT_1555ABGR	
#define BTEXTUREFORMAT_4444ARGB  BTEXTUREFORMAT_1555ABGR	
#define BTEXTUREFORMAT_256PALETTE BTEXTUREFORMAT_256PALETTE_8888ARGB	
#define BTEXTUREFORMAT_16PALETTE BTEXTUREFORMAT_16PALETTE_8888ARGB	

#else

#define	FORMAT(ptr)	((ptr)->textureInfo.textureFormat)
#define XDIM(ptr) ((ptr)->textureInfo.width)
#define YDIM(ptr) ((ptr)->textureInfo.height)
//#define BDISPLAY_0BPP	
#define BTEXTUREFORMAT_8888ARGB	BTEXTUREFORMAT_8888ABGR	
#define BTEXTUREFORMAT_1555ARGB  BTEXTUREFORMAT_1555ARGB	
#define BTEXTUREFORMAT_4444ARGB  BTEXTUREFORMAT_1555ARGB	
#define BTEXTUREFORMAT_256PALETTE BTEXTUREFORMAT_256PALETTE_1555ABGR	
#define BTEXTUREFORMAT_16PALETTE BTEXTUREFORMAT_16PALETTE_1555ABGR	


#endif


ACTORINSTANCELIST		objectViewerInstances;
ACTORINSTANCE			*tazObjectViewerInstance=NULL;
ACTORLISTENTRY			*viewerListPtr;

CAMERAINFO				viewerCameraInfo;
TBVector				viewerEye, viewerLookAt;
float					zoomDistance;					// distance to lookat point in meters

TBVector				viewerRotation;
TBQuaternion			tempViewerQuat;
char					statsMode = 0;

char					cartoonLightingStatus;

TBActorAnimSegment		*viewerAnimSegment;
uint32					viewerTotalAnims;
int32					viewerAnimNumber;
char					viewerAnimStatus;				// whether animation is playing, or not

uint32					viewerTotalTextureSize;

char					viewerBuf[256];
ushort					viewerString16[256];

int						noofViewerObjectPolys;
int						noofViewerObjectBones;

ViewerSpriteInfo		texturesFound[500];
int						numTexturesFound;
float					texViewerYInc = 0;
int						activeTexNum;
float					yRot,xRot;

//ViewerGridInfo			*viewerGridRootPtr, *viewerGridTempPtr;
TBPrimVertex			viewerGridInfo[101];
char					viewerGridStatus;
char					viewerMotionBone = FALSE;
int32					currentT;
int32					currentT2;

// Ground
TBPrimVertex			groundVerts[4];
float					groundHeight;
float					groundSize;
char					groundStatus = 1;

// Spin
char					spinStatus = 1;
float					spinSpeed;

// Zone
char					zoneStatus = 0;

// Resort animation segments
int						*segmentNumbers = NULL;
int						*startFrames = NULL;

TBTimerValue			startTime, endTime, deltaTime;			

// Recording info
char					recordStatus = 0;
char					frameName[32];
int						objViewerScreenShotNumber = 0;// PP: renamed to prevent (PS2?) link error (duplicate global symbol) - there's a 'screenShotNumber' in main.cpp, used for taking normal in-game screenshots I think

// Object List
char					objectListStatus = 0; // Object list on/off
int						objectListOffset = 0; // Object in center of list
int						currentObjectNumber = 0;
int						currentObjectTablePos = 0;
float					scrollPos;
char					actorFileName[32];		// The actors filename

// Packagfe Info
TBPackageIndex			*pakIndex;
TBPackageID				pakId;
TBTexture				*viewerTexture;

int listPos = 0;
int listStartPos = 0;
			
int slashPos;
int dotPos;

int actorNumber;
int actorTable[500];
int actorTableSize = 500;
int lastActor;
int actorTablePos;
int firstTazCostume;
int firstShedevilCostume;
int costume = 0;

char costumeName[15];

ACTORLISTENTRY			*startListPtr;
ACTORLISTENTRY			*endListPtr;

// NH: The text name of the costume. NOTE: Add all new costumes here in the order to match the costumes enum!! (But don't add 'None')
char *costumeNameText[] = 
{
	"Ninja",
	"Cowboy",
	"Construction",
	"Reindeer",	
	"Explorer",	
	"Surfer",
	"Rappa",
	"Weretaz",
	"Miner",	
	"Indy",
	"Tarzan",
	"Snowboard",
	"Swat",
	"Skater"
};

/* --------------------------------------------------------------------------------
   Function : InitObjectViewer
   Purpose : initialises object viewer instance list
   Parameters : 
   Returns : 
   Info : 
*/

void InitObjectViewer( void )
{
	ACTORLISTENTRY	*tempViewerListPtr;
	viewerTexture = bkLoadTexture(NULL, "viewerground.bmp", 0);

	InitActorInstanceList( &objectViewerInstances );
	
	viewerListPtr = actorList.head.next;

	cartoonLightingStatus = TRUE;

	// initialise new camera viewerCameraInfo
	// initialise eye and look at positions
	viewerEye[0] = METERTOUNIT(0.0f);
	viewerEye[1] = METERTOUNIT(0.0f);
	viewerEye[2] = METERTOUNIT(6.0f);
	viewerEye[3] = 1.0f;

	viewerLookAt[0] = METERTOUNIT(0.0f);
	viewerLookAt[1] = METERTOUNIT(0.45f);
	viewerLookAt[2] = METERTOUNIT(0.0f);
	viewerLookAt[3] = 1.0f;

	//initiallise zoom distance
	zoomDistance = 6.0;

	//initialise rotations
	yRot = xRot = 0.0f;

	InitCamera( &viewerCameraInfo, viewerEye, viewerLookAt);

	// Set camera locations
	EnableCameraAngleAboveHorizontal( &viewerCameraInfo, -xRot);  
	RotateCameraAroundLookAt(&viewerCameraInfo, -yRot, 0.0f);

	//initiallize grid points
	InitViewerGrid();

	//Get Package Info
#ifdef OBJECTVIEWONLY
	pakIndex	= FindPackage("chars");
	pakId		= bkGetPackageID(pakIndex);
#endif

	actorNumber = 0;
	actorTablePos = 0;

	startListPtr = NULL;

	tempViewerListPtr = viewerListPtr;
	
	memset(actorTable, -1, sizeof(int)*actorTableSize);

	// Build a table of usable actors
	while ((strcmp(tempViewerListPtr->name, "")) && (actorTablePos<actorTableSize))
	{
		sprintf(actorFileName, "%s", bkFindFilenameByCRC(pakId, tempViewerListPtr->crc));
		if (strcmp(actorFileName, "(null)") == 0)
		{
			sprintf(actorFileName, "%s", tempViewerListPtr->name);
		}
	
		// Check if the first . (which will be from .obe) is before the first \ (If it is the \ is from \n else it
		// must be something like costume\hat.obe\n so we want to ignore it (not a char)

		// If name is not blank...
		if (strcmp(actorFileName, "") != 0)
		{
#ifdef ROOT_OBJECTS
			slashPos = strcspn(actorFileName, "\\");
			dotPos = strcspn(actorFileName, ".");

			// ...and not got a slash before the .obe
			if (((slashPos > dotPos) || (slashPos == NULL)) && (strcmp(actorFileName, "nuts.obe") != 0) && (strcmp(actorFileName, "spindevil.obe") != 0))
			{
#endif
				// Store actor number in list
				actorTable[actorTablePos] = actorNumber;
				actorTablePos++;

				// Setup start ptr to first actor
				if (startListPtr==NULL)
					startListPtr = tempViewerListPtr;

				// Set end ptr to the last actor
				endListPtr = tempViewerListPtr;

				if (strcmp(actorFileName, "tazdevil.obe") == 0)
				{
					firstTazCostume = actorTablePos;

					while(costume <= COSTUME_SKATER)
					{
						// Store actor number in list
						actorTable[actorTablePos] = actorNumber;
						actorTablePos++;

						// Setup start ptr to first actor
						if (startListPtr==NULL)
							startListPtr = tempViewerListPtr;

						// Set end ptr to the last actor
						endListPtr = tempViewerListPtr;

						costume++;
					}					
				}						
				else if (strcmp(actorFileName, "shedevil.obe") == 0)
				{
					firstShedevilCostume = actorTablePos-1;
				}

#ifdef ROOT_OBJECTS
			}
#endif
		}
		actorNumber++;
		tempViewerListPtr = tempViewerListPtr->next;
	}

	viewerListPtr = startListPtr;

	PrepViewerForUpdate();

	lastActor = actorTablePos-1;
}

/* --------------------------------------------------------------------------------
   Function : DrawObjectViewer
   Purpose : draws selected objects
   Parameters : 
   Returns : 
   Info : 
*/

void DrawObjectViewer( TBFont *standardFont )
{
	float			drawTime;
	TBSet			*set;
	TBMatrix		matrix;
	float			offset;
	ACTORLISTENTRY	*tempViewerListPtr;
	int				objectListCount = 0;
	int				objectItemCount = 0;
	char			objectName[32];
	char			actorFileNameTemp[32];		// The actors filename

	//read timer value
#if BPLATFORM==PS2
	*T0_COUNT = 0;
#else
	startTime = bkTimerRead();
#endif

	//draw current selected actor
	if (viewerListPtr->type&ACTORTYPE_SET)
	{
		bdSetObjectMatrix(tazObjectViewerInstance->actorInstance.objectToWorld);
		set = (TBSet*)viewerListPtr->actor;

		if (set) 
		{
			baDrawSet(set,BSETPRIMALPHATYPE_SOLID);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
			baDrawSet(set,BSETPRIMALPHATYPE_CKEY);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
			baDrawSet(set,BSETPRIMALPHATYPE_ALPHA);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
		}
	}
	else
	{
		DrawActorInstances(&objectViewerInstances);
		DrawActorInstances2(&objectViewerInstances);
		DrawInstancesCharacterInfo(&objectViewerInstances);
	}

	//read timer again
#if BPLATFORM == PS2
	endTime = *T0_COUNT;
#else
	endTime = bkTimerRead();
#endif
	// calculate time taken to draw actor instance
#if BPLATFORM == PS2
	drawTime = endTime;
#else
	deltaTime = bkTimerDelta( startTime, endTime );

	drawTime = 1/(bkTimerToFPS( deltaTime ));
#endif

	// Draw Ground
	if (groundStatus)
		DrawViewerGround();

	// Draw text
	bmMatScale(matrix,0.45f,0.45f,0.45f);

	bdSetObjectMatrix(matrix);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
#if BPLATFORM == PS2
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
#endif
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	switch(statsMode)
	{
	case 0:
		break;
	case 3:
		if (!objectListStatus)
		{
			bdSetTexture(0,NULL);
			bdSetTexture(1,NULL);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
			
			// NH: Draw background rect for stats screen
			DrawRectangle(RECTANGLE(-650.0f,-40.0f,345.0f,100.0f),COLOUR(0,0,0,96));
			
			strcpy(viewerBuf, "Number of polys:");
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+100, 0,127,0, 127, NULL, NULL);

			sprintf(viewerBuf, "%d", noofViewerObjectPolys );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-10,(float)((videoMode.yScreen)/2.0f)+100, 127,127,127, 127, NULL, NULL);

			strcpy(viewerBuf, "Number of bones:");
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+60, 0,127,0, 127, NULL, NULL);

			sprintf(viewerBuf, "%d", noofViewerObjectBones );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-5,(float)((videoMode.yScreen)/2.0f)+60, 127,127,127, 127, NULL, NULL);

			strcpy(viewerBuf, "Number of textures:");
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+20, 0,127,0, 127, NULL, NULL);

			sprintf(viewerBuf, "%d", numTexturesFound);
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+40,(float)((videoMode.yScreen)/2.0f)+20, 127,127,127, 127, NULL, NULL);

			strcpy(viewerBuf, "Size of textures:");
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-20, 0,127,0, 127, NULL, NULL);

			sprintf(viewerBuf, "%d k(%d)", viewerTotalTextureSize/1024 , viewerTotalTextureSize);
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-10,(float)((videoMode.yScreen)/2.0f)-20, 127,127,127, 127, NULL, NULL);

			strcpy(viewerBuf, "Draw time:");
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-60, 0,127,0, 127, NULL, NULL);

		#if BPLATFORM==PS2
			sprintf(viewerBuf, "%f", drawTime);//*1000
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-90,(float)((videoMode.yScreen)/2.0f)-60, 127,127,127, 127, NULL, NULL);

		#else
			sprintf(viewerBuf, "%f ms", drawTime*1000);//*1000
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-90,(float)((videoMode.yScreen)/2.0f)-60, 127,127,127, 127, NULL, NULL);

		#endif
			
			// TP: Print offset time from start of current animation

			// TP: currentFrame - startframe * 1/60;
			if (tazObjectViewerInstance->actorInstance.numAnimSegments)
			{
				offset = (float)tazObjectViewerInstance->actorInstance.rootNodeInstance->targetFrame - tazObjectViewerInstance->actorInstance.animSegments[segmentNumbers[viewerAnimNumber]].startFrame;
				offset /= 1024.0f;
				offset /= 60.0f;
				
				strcpy(viewerBuf, "Offset:");
				bkString8to16(viewerString16, viewerBuf);
				bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-100, 0,127,0, 127, NULL, NULL);

				sprintf(viewerBuf, "%f s", offset);
				bkString8to16(viewerString16, viewerBuf);
				bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-140,(float)((videoMode.yScreen)/2.0f)-100, 127,127,127, 127, NULL, NULL);
			}
	
			break;
		}

	// Joypad Key List
	case 1:
		if (!objectListStatus)
		{
			bdSetTexture(0,NULL);
			bdSetTexture(1,NULL);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
			DrawRectangle(RECTANGLE(-600.0f,600.0f,-395.0f,350.0f),COLOUR(0,0,0,96));

			// NH: Animation Toggle
			sprintf(viewerBuf, "Toggle Animation:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+120, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Square" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+120, 127,127,127, 127, NULL, NULL);

			// NH: Rotate
			sprintf(viewerBuf, "Rotate:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+80, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Right Analogue" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+80, 127,127,127, 127, NULL, NULL);

			// NH: Zoom
			sprintf(viewerBuf, "Zoom In / Out:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+40, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Left Analogue Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+40, 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev Anim
			sprintf(viewerBuf, "Prev / Next Anim:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f), 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Digital Left / Right (Whilst Animating)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f), 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev 10 Anim
			sprintf(viewerBuf, "Prev / Next Anim (x10):" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-40, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "l1 + Digital Left / Right (Whilst Animating)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-40, 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev Frame
			sprintf(viewerBuf, "Prev / Next Frame:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-80, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Digital Left / Right (Whilst Paused)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-80, 127,127,127, 127, NULL, NULL);
			
			// NH: Next/Prev Object
			sprintf(viewerBuf, "Prev / Next Object:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-120, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Digital Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-120, 127,127,127, 127, NULL, NULL);

			// NH: Toggle Object List
			sprintf(viewerBuf, "Toggle Object List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-160, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "L2" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-160, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Scroll List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-200, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Right Analogue Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-200, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Scroll List Fast:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-240, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Left Analogue Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-240, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Select From List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-280, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "R2" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-280, 127,127,127, 127, NULL, NULL);

			// NH: Toggle Autospin
			sprintf(viewerBuf, "Toggle Autospin:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-320, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Circle" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-320, 127,127,127, 127, NULL, NULL);
			
			// NH: Toggle Ground
			sprintf(viewerBuf, "Toggle Ground:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-360, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Triangle" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-360, 127,127,127, 127, NULL, NULL);
			
			// NH: Motion Bone
			sprintf(viewerBuf, "Toggle Motion Bone:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-400, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Cross" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-400, 127,127,127, 127, NULL, NULL);

			// NH: Cartoon Shading
			sprintf(viewerBuf, "Toggle Cartoon Shading:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-440, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "R1" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-440, 127,127,127, 127, NULL, NULL);

			// NH: Light Pos
			sprintf(viewerBuf, "Rotate Light:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-480, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Left Analogue Left / Right" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-480, 127,127,127, 127, NULL, NULL);

#ifdef DEVELOPERS_VERSION
			// NH: Toggle Zones
			sprintf(viewerBuf, "Toggle Zones:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-520, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Z" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-520, 127,127,127, 127, NULL, NULL);

			// NH: Extra Info
			sprintf(viewerBuf, "Extra Info:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-560, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "I" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-560, 127,127,127, 127, NULL, NULL);
#endif
														
			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

			break;
		}

	// Keyboard Key List
	case 2:
		if (!objectListStatus)
		{
			bdSetTexture(0,NULL);
			bdSetTexture(1,NULL);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
			DrawRectangle(RECTANGLE(-650.0f,380.0f,-410.0f,365.0f),COLOUR(0,0,0,96));

			// NH: Animation Toggle
			sprintf(viewerBuf, "Toggle Animation:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+120, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "A" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+120, 127,127,127, 127, NULL, NULL);

			// NH: Rotate
			sprintf(viewerBuf, "Rotate:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+80, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Cursors" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+80, 127,127,127, 127, NULL, NULL);

			// NH: Zoom
			sprintf(viewerBuf, "Zoom In / Out:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+40, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "+ / -" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)+40, 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev Anim
			sprintf(viewerBuf, "Prev / Next Anim:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f), 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "[ / ] (Whilst Animating)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f), 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev 10 Anim
			sprintf(viewerBuf, "Prev / Next Anim (x10):" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-40, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "LShift + [ / ] (Whilst Animating)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-40, 127,127,127, 127, NULL, NULL);

			// NH: Next/Prev Frame
			sprintf(viewerBuf, "Prev / Next Frame:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-80, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "[ / ] (Whilst Paused)" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-80, 127,127,127, 127, NULL, NULL);
			
			// NH: Next/Prev Object
			sprintf(viewerBuf, "Prev / Next Object:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-120, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "< / >" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-120, 127,127,127, 127, NULL, NULL);

			// NH: Toggle Object List
			sprintf(viewerBuf, "Toggle Object List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-160, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Tab" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-160, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Scroll List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-200, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Cursor Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-200, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Scroll List Fast:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-240, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "LShift + Cursor Up / Down" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-240, 127,127,127, 127, NULL, NULL);
			
			// NH: Select Object
			sprintf(viewerBuf, "Select From List:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-280, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Space" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-280, 127,127,127, 127, NULL, NULL);

			// NH: Toggle Autospin
			sprintf(viewerBuf, "Toggle Autospin:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-320, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "S" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-320, 127,127,127, 127, NULL, NULL);
			
			// NH: Toggle Ground
			sprintf(viewerBuf, "Toggle Ground:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-360, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "G" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-360, 127,127,127, 127, NULL, NULL);
			
			// NH: Motion Bone
			sprintf(viewerBuf, "Toggle Motion Bone:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-400, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "M" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-400, 127,127,127, 127, NULL, NULL);

			// NH: Cartoon Shading
			sprintf(viewerBuf, "Toggle Cartoon Shading:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-440, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "C" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-440, 127,127,127, 127, NULL, NULL);

			// NH: Light Pos
			sprintf(viewerBuf, "Rotate Light:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-480, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "LShift + Cursor Left / Right" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-480, 127,127,127, 127, NULL, NULL);

			// NH: Record
			sprintf(viewerBuf, "Start / Stop Recording:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-520, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "R" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-520, 127,127,127, 127, NULL, NULL);

#ifdef DEVELOPERS_VERSION
			// NH: Toggle Zones
			sprintf(viewerBuf, "Toggle Zones:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-560, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "Z" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-560, 127,127,127, 127, NULL, NULL);

			// NH: Extra Info
			sprintf(viewerBuf, "Extra Info:" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)-600, 127,127,0, 127, NULL, NULL);
			sprintf(viewerBuf, "I" );
			bkString8to16(viewerString16, viewerBuf);
			bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+100,(float)((videoMode.yScreen)/2.0f)-600, 127,127,127, 127, NULL, NULL);

#endif
			

			bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
			

			break;
		}
	}

	// NH: Ok, so this seems a bit odd, but I wanna draw a trans rectangle behind the text, but it has to be
	// a different size for mode 3 and 4, so this only does this size rect if the case is mode 3.
	
	bdSetTexture(0,NULL);
	bdSetTexture(1,NULL);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
		
	// NH: Draw background for info screen
#if BPLATFORM!=PC
	DrawRectangle(RECTANGLE(-600.0f,600.0f,465.0f,365.0f),COLOUR(0,0,0,96));
#else
	DrawRectangle(RECTANGLE(-650.0f,-110.0f,480.0f,380.0f),COLOUR(0,0,0,96));
#endif

	if (viewerListPtr->name[0]!=NULL)
	{
		sprintf(objectName, "%s", bkFindFilenameByCRC(pakId, viewerListPtr->crc), costumeName);
		
		if (strcmp(objectName, "(null)") == 0)
		{
			sprintf(objectName, "%s", viewerListPtr->name);
		}	
		
		// NH: If there is a name stored for the current costume, then display it.
		if (strcmp(costumeName, "") != 0)
		{
			strcat(objectName, " - ");
			strcat(objectName, costumeName);
		}

		// NH: Print 'filename'
		strcpy(viewerBuf, "FileName:");
		bkString8to16(viewerString16, viewerBuf);
		bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+230, 0,127,0, 127, NULL, NULL);
		
		// NH: Print the actual filename
		sprintf(viewerBuf, "%s", (char*)(&objectName) );
		bkString8to16(viewerString16, viewerBuf);
		bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-110,(float)((videoMode.yScreen)/2.0f)+230, 127,127,127, 127, NULL, NULL);
	}

	//find number of animations
	viewerTotalAnims = viewerListPtr->actor->numAnimSegments;
	
	// NH: Print anim no text
	strcpy(viewerBuf, "Animation number:");
	bkString8to16(viewerString16, viewerBuf);
	bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)-250,(float)((videoMode.yScreen)/2.0f)+190, 0,127,0, 127, NULL, NULL);

	// NH: Print the actaul anim no.
	sprintf(viewerBuf, "%d of %d", viewerAnimNumber+1, viewerTotalAnims );
	bkString8to16(viewerString16, viewerBuf);
	bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+20,(float)((videoMode.yScreen)/2.0f)+190, 127,127,127, 127, NULL, NULL);

	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	// NH: Take a screen shot
	if (recordStatus)
	{
		sprintf(objectName, "%s", bkFindFilenameByCRC(pakId, viewerListPtr->crc));
		
		if (strcmp(objectName, "(null)") == 0)
		{
			sprintf(objectName, "%s", tempViewerListPtr->name);	
		}

		if (strcmp(objectName, "") != 0)
		{
			STRNCPY(frameName,(char*)(&objectName),strlen((char*)(&objectName))-3);
		}
		else
		{
			sprintf(frameName,"%s",objectName);
		}

		// NH: If there is a name stored for the current costume, then add it to the recorded file string.
		if (strcmp(costumeName, "") != 0)
		{
			strcat(frameName, "_");
			strcat(frameName, costumeName);
		}

		sprintf(frameName,"%s%03d_%05d.bmp", frameName, viewerAnimNumber+1, objViewerScreenShotNumber++);
		bdScreenshot(frameName);

		bdSetTexture(0,NULL);
		bdSetTexture(1,NULL);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
		
		// NH: Draw background to recording text
		DrawRectangle(RECTANGLE(-30.0f,650.0f,380.0f,480.0f),COLOUR(0,0,0,96));

		// NH: Display 'Recording' text
		strcpy(viewerBuf, "Recording:");
		bkString8to16(viewerString16, viewerBuf);
		bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+400,(float)((videoMode.yScreen)/2.0f)+230, 127,0,0, 127, NULL, NULL);
		
		// NH: Display Object Name
		sprintf(viewerBuf, "%s", frameName);
		bkString8to16(viewerString16, viewerBuf);
		bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+400,(float)((videoMode.yScreen)/2.0f)+190, 127,0,0, 127, NULL, NULL);

		bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	}
	else if (objectListStatus)
	{
		statsMode = 0;
		// NH: SHOW OBJECT LIST

		tempViewerListPtr = startListPtr;

		bdSetTexture(0,NULL);
		bdSetTexture(1,NULL);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);

		// NH: Draw background rectangle for object list
#if BPLATFORM!=PC
		DrawRectangle(RECTANGLE(-600.0f,50.0f,-370.0f,290.0f),COLOUR(0,0,0,96));
#else
		DrawRectangle(RECTANGLE(-650.0f,50.0f,-330.0f,320.0f),COLOUR(0,0,0,96));
#endif

		int j = 0;

		objectListOffset = 0;
		listPos = listStartPos;

		int coltp = listStartPos;
		int colp = actorTable[0];
		
		BOOL endOfList = FALSE;

		// NH: Show List of actors
		while((objectListOffset<=15) && (!endOfList))
		{
			if ((coltp-20 < lastActor) && (coltp >= 0) && (coltp<actorTableSize) && (actorTable[coltp] != -1))
			{
				while(actorTable[coltp] > colp)
				{
					colp++;
					tempViewerListPtr = tempViewerListPtr->next;
				}

				sprintf(actorFileName, "%s", bkFindFilenameByCRC(pakId, tempViewerListPtr->crc));
				if (strcmp(actorFileName, "(null)") == 0)
				{
					sprintf(actorFileName, "%s", tempViewerListPtr->name);
				}
				bkString8to16(viewerString16, actorFileName);

				// Draw Costume Number if exists
				if ((coltp >= firstTazCostume) && (coltp <= firstTazCostume + COSTUME_SKATER))
				{	
					strcpy(actorFileNameTemp, actorFileName);
					strcat(actorFileNameTemp, " (");
					strcat(actorFileNameTemp, costumeNameText[coltp - firstTazCostume]);
					strcat(actorFileNameTemp, ")");
					strcpy(actorFileName, actorFileNameTemp);
					bkString8to16(viewerString16, actorFileName);					
				}

				// Draw Object Name in colour to indicate unselected, selected or active object
				if ((currentObjectNumber == coltp) && (objectListOffset==0))
					bdPrintFontLite(standardFont, viewerString16, strlen(actorFileName), (float)((-videoMode.xScreen)/2.0f)-250.0f,(float)((videoMode.yScreen)-170.0f-(40.0f * objectListOffset)), 127,127,0, 127, NULL, NULL);
				else if (currentObjectNumber == coltp)
					bdPrintFontLite(standardFont, viewerString16, strlen(actorFileName), (float)((-videoMode.xScreen)/2.0f)-250.0f,(float)((videoMode.yScreen)-170.0f-(40.0f * objectListOffset)), 127,0,0, 127, NULL, NULL);
				else if (objectListOffset==0)
					bdPrintFontLite(standardFont, viewerString16, strlen(actorFileName), (float)((-videoMode.xScreen)/2.0f)-250.0f,(float)((videoMode.yScreen)-170.0f-(40.0f * objectListOffset)), 0,127,0, 127, NULL, NULL);
				else
					bdPrintFontLite(standardFont, viewerString16, strlen(actorFileName), (float)((-videoMode.xScreen)/2.0f)-250.0f,(float)((videoMode.yScreen)-170.0f-(40.0f * objectListOffset)), 127,127,127, 127, NULL, NULL);

				coltp++;
				objectListOffset++;
			}
			else
			{
				endOfList = TRUE;
			}
		}

		bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	}

	bdSetTexture(0,NULL);
	bdSetTexture(1,NULL);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,1);
	
	// NH: Draw Help Button Text
#if BPLATFORM != PC
	DrawRectangle(RECTANGLE(-600.0f,600.0f,-405.0f,-585.0f),COLOUR(0,0,0,96));
#else
	DrawRectangle(RECTANGLE(-650.0f,650.0f,-420.0f,-500.0f),COLOUR(0,0,0,96));
#endif

#if BPLATFORM == PC
	sprintf(viewerBuf, "%s", "Press H for Help" );
	bkString8to16(viewerString16, viewerBuf);
	bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+220,(float)((videoMode.yScreen)/2.0f)-680, 127,127,0, 127, NULL, NULL);
#else
	sprintf(viewerBuf, "%s", "Press Start for Help" );
	bkString8to16(viewerString16, viewerBuf);
	bdPrintFontLite(standardFont, viewerString16, strlen(viewerBuf), (float)((-videoMode.xScreen)/2)+180,(float)((videoMode.yScreen)/2.0f)-680, 127,127,0, 127, NULL, NULL);
#endif

	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,1);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	
	

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
#if BPLATFORM == PS2
// PP: REMOUT: I really don't like this render state		bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,FALSE,FALSE);
#endif

}

/* --------------------------------------------------------------------------------
   Function : PrepViewerForUpdate
   Purpose : prepares actor instance for drawing
   Parameters : 
   Returns : 
   Info : 
*/

void PrepViewerForUpdate( void )
{
	int			i,tempInt;
	TBQuaternion	tempQuat;
	TBVector	tQuat = {0.0f,1.0f,0.0f,RAD(180.0f)};
	char		*name = "tazdevil.obe",change;

//	FreeAllActorInstances( &objectViewerInstances );
	FreeObjectViewer();

	tazObjectViewerInstance = CreateActorInstanceFromActorPtr( &objectViewerInstances, viewerListPtr->actor,0, "tazObjView" );
	CreateActorInstanceAnimation(tazObjectViewerInstance);
	CreateActorInstanceCharacterInfo(tazObjectViewerInstance);
	CreateActorInstanceBehaviour(tazObjectViewerInstance);

	if (cartoonLightingStatus)
	{
		EnableActorCartoonRenderMode(&tazObjectViewerInstance->actorInstance);
	}
	else
	{
		DisableActorCartoonRenderMode(&tazObjectViewerInstance->actorInstance);
	}

	if (bkCRC32((uchar*)name,strlen(name),0) == viewerListPtr->crc)
	{
		AttachObjectToCharacter(tazObjectViewerInstance,"hat.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR);
	}

	if (tazObjectViewerInstance->actorInstance.numAnimSegments > 0)
	{
		segmentNumbers = (int*)ZALLOC(tazObjectViewerInstance->actorInstance.numAnimSegments*sizeof(int));
		ASSERT(segmentNumbers);
		startFrames = (int*)ZALLOC(tazObjectViewerInstance->actorInstance.numAnimSegments*sizeof(int));
		ASSERT(startFrames);
		for (i=0;i<(int)(tazObjectViewerInstance->actorInstance.numAnimSegments);i++)
		{
			segmentNumbers[i] = i;
			startFrames[i] = tazObjectViewerInstance->actorInstance.animSegments[i].startFrame;
		}

		change = TRUE;
		while (change)
		{
			change = FALSE;
			for (i=0;i<(int)(tazObjectViewerInstance->actorInstance.numAnimSegments-1);i++)
			{
				if (startFrames[i] > startFrames[i+1])
				{
					change = TRUE;
					tempInt = segmentNumbers[i];
					segmentNumbers[i] = segmentNumbers[i+1];
					segmentNumbers[i+1] = tempInt;
					tempInt = startFrames[i];
					startFrames[i] = startFrames[i+1];
					startFrames[i+1] = tempInt;
				}
			}
		}
	}

	currentT = 0;

	bmRotationToQuat(tempQuat,tQuat);
	bmQuatCopy(tazObjectViewerInstance->actorInstance.orientation,tempQuat);

	FlushAnimationQueue(tazObjectViewerInstance);

	//update ptr to animation segment
	viewerAnimSegment = viewerListPtr->actor->animSegments;
	//reinitialize animation number
	viewerAnimNumber = 0;
	viewerAnimStatus = TRUE;

	// retreive info for new actor

	// initialize number of polys
	viewerTotalTextureSize = 0;
	numTexturesFound = 0;
	texViewerYInc = 0;
	activeTexNum = 0;

	// Get Geometry Info (Only extract the number of polys)
//	baGetActorGeometry(&tazObjectViewerInstance->actorInstance,NULL,0,NULL,&noofViewerObjectPolys,NULL,NULL,BGETACTGEOMETRY_COUNTONLY);

	// decide what type of model current actor is
	// and extract required info from appropriate place

	if (tazObjectViewerInstance->actorInstance.numAnimSegments > 0)
		PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );

	// NH: If costumed...
	if ((currentObjectNumber >= firstTazCostume) && (currentObjectNumber <= firstTazCostume + COSTUME_SKATER))
	{
		// NH: Remove current costume and add the new one.
		RemoveCostume(tazObjectViewerInstance);
		ChangeActorInstance(tazObjectViewerInstance,FindActorInActorList("tazdevil.obe"),5,NULL,TRUE);
		SetupCostume(tazObjectViewerInstance, currentObjectNumber - firstTazCostume);
		// NH: Get the costumes name from the list defined at the top of the page.
		sprintf(costumeName, "%s", costumeNameText[currentObjectNumber - firstTazCostume]);
	}
	else if (currentObjectNumber == firstShedevilCostume)
	{
		// NH: Attach Bowtie to head of SheDevil
		AttachObjectToCharacter(tazObjectViewerInstance,"costume\\shedevilbowtie.obe","spinetop",NULL,NULL,NULL,ATTACHFLAG_ANIMATEWITHCHAR|ATTACHFLAG_INHERITSCALE);			
	}
	else
	{
		strcpy(costumeName, "");
	}

	return;
}
/* --------------------------------------------------------------------------------
   Function : UpdateObjectViewer
   Purpose : updates object viewer, dependant on pad inputs
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateObjectViewer( void )
{
	static			int	costume=0;
	int j;

#if BPLATFORM != PC
	// Ground toggle
	if(CANCELBUTTON)
		groundStatus = !groundStatus;

	// Spin toggle
	if (controller1.circleDebounceChannel->value)
		spinStatus = !spinStatus;

	// Object selection list toggle
	if (controller1.l2DebounceChannel->value)
		objectListStatus = !objectListStatus;

	// Cartoon Shading Toggle
	if( controller1.r2DebounceChannel->value && !controller1.l2Channel->value)
		cartoonLightingStatus = !cartoonLightingStatus;

#ifdef DEVELOPERS_VERSION
	// Draw zones toggle
	if (controller1.l3DebounceChannel->value)
		zoneStatus = !zoneStatus;
#endif
#else
	// Object selection list toggle
	if (controller1.tabDebounceChannel->value)
		objectListStatus = !objectListStatus;

	// Ground toggle
	if( controller1.gDebounceChannel->value )
		groundStatus = !groundStatus;

	// Spin toggle
	if (controller1.sDebounceChannel->value)
		spinStatus = !spinStatus;

	// Cartoon Shading Toggle
//	if( controller1.cDebounceChannel->value)
//		cartoonLightingStatus = !cartoonLightingStatus;

#endif

	if (objectListStatus)
	{	
#if BPLATFORM != PC
		if (((controller1.y1) + (controller1.y2 / 8.0f) < 0) && (scrollPos <= 1))
			scrollPos = 0;
		else if (((controller1.y1) + (controller1.y2 / 8.0f) > 0) && (scrollPos >= lastActor-1))
			scrollPos = lastActor;
		else
			scrollPos+= (controller1.y1) + (controller1.y2 / 8.0f);
		
		listStartPos = bmFloatToInt(scrollPos);	// TP: on some platforms using bmFloatToInt instead of a cast is faster, doesn't really matter here though
#else
		if (controller1.lshiftChannel->value)
		{
			if (((controller1.y1) < 0) && (scrollPos <= 1))
				scrollPos = 0;
			else if (((controller1.y1) > 0) && (scrollPos >= lastActor-1))
				scrollPos = (float)lastActor;
			else
				scrollPos+= (controller1.y1);
		}
		else
		{
			if (((controller1.y1DebounceChannel->value) < 0) && (scrollPos <= 1))
				scrollPos = 0;
			else if (((controller1.y1DebounceChannel->value) > 0) && (scrollPos >= lastActor-1))
				scrollPos = (float)lastActor;
			else if (controller1.y1DebounceChannel->value > 0)
				scrollPos++;
			else if (controller1.y1DebounceChannel->value < 0)
				scrollPos--;
		}
		
		listStartPos = bmFloatToInt(scrollPos);	// TP: on some platforms using bmFloatToInt instead of a cast is faster, doesn't really matter here though
#endif

#if BPLATFORM != PC
		if (controller1.r2Channel->value)
#else
		if (controller1.startChannel->value)
#endif
		{
			if (listStartPos < currentObjectNumber)
			{
				// Decrement object to select in object list
				j = actorTable[currentObjectNumber];
				while(actorTable[listStartPos] < j)
				{
					viewerListPtr = viewerListPtr->prev;
					j--;
				}
				currentObjectNumber = listStartPos;		
			}
			else if (listStartPos > currentObjectNumber)
			{
				// Increment object to select in object list
				j = actorTable[currentObjectNumber];
				while(actorTable[listStartPos] > j)
				{
					viewerListPtr = viewerListPtr->next;
					j++;
				}
				currentObjectNumber = listStartPos;		
			}

			if(viewerListPtr->type & ACTORTYPE_ACTOR) PrepViewerForUpdate();
		}
		
	}
	else
	{
		objectListStatus = FALSE;
	}


	if (cartoonLightingStatus)
		EnableActorCartoonRenderMode(&tazObjectViewerInstance->actorInstance);
	else
		DisableActorCartoonRenderMode(&tazObjectViewerInstance->actorInstance);

#if BPLATFORM != PC
	if (controller1.startDebounceChannel->value)
	{
		objectListStatus = 0;

		if (statsMode != 1)
			statsMode = 1;
		else
			statsMode = 0;
	}
#else
	if (controller1.hDebounceChannel->value)
	{
		objectListStatus = 0;

		if (statsMode != 2)
			statsMode = 2;
		else
			statsMode = 0;
	}
#endif
	

#ifdef DEVELOPERS_VERSION

#if BPLATFORM != PC
	if (controller1.r1DebounceChannel->value && !objectListStatus)
#else
	if (controller1.iDebounceChannel->value && !objectListStatus)
#endif
	{
		if(statsMode != 3)
			statsMode = 3;
		else
			statsMode = 0;
	}
#endif

	if (controller1.rDebounceChannel->value)
	{
		recordStatus = !recordStatus;
		
		if (recordStatus)
		{
			speedRatio = 1;
			fps = 60;
			viewerAnimNumber = 0;
			objViewerScreenShotNumber = 0;
			PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
			currentT2 = tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1;

			AnimateActorInstances( &objectViewerInstances );
			UpdateInstancesCharacterInfo(&objectViewerInstances);
			Flip(64, 64, 64, 255);
		}
	}

	if (!recordStatus)
	{
#if BPLATFORM != PC
		if (controller1.x1)
#else
		if(controller1.lshiftChannel->value != 0 && controller1.x1)
#endif
		{
			bmVectorRotateY(cartoonLightVec,cartoonLightVec,controller1.x1*RAD(180.0f/fps));
			baSetDirectionalLight(&cartoonLight, cartoonLightVec);
			// PP: TEMP BABEL WORKAROUND - fixes light direction problem YAY!!!!
			cartoonLight.directional.unitDirection[W]=1.0f;
		}
	
		// NH: Step Up Object List
#if BPLATFORM != PC
		if(controller1.dPadYDebounceChannel->value < 0 )
#else
		if(controller1.periodDebounceChannel->value != 0 )
#endif
		{
			if (currentObjectNumber+1 <= lastActor)
			{
				// Increment object to select object in list
				j = actorTable[currentObjectNumber];
				while(actorTable[currentObjectNumber+1] > j)
				{
					viewerListPtr = viewerListPtr->next;
					j++;
				}
				currentObjectNumber = currentObjectNumber+1;		
				//currentObjectTablePos++;
			}
			else
			{
				currentObjectNumber = lastActor;
			}

			scrollPos = (float)currentObjectNumber;
			if(viewerListPtr->type & ACTORTYPE_ACTOR) PrepViewerForUpdate();			
		}

		// Step Down Object List
#if BPLATFORM != PC
		if(controller1.dPadYDebounceChannel->value > 0 )
#else
		if(controller1.commaDebounceChannel->value != 0 )
#endif
		{
			if (currentObjectNumber-1 >= 0)
			{
				// Decrement object to select object in list
				j = actorTable[currentObjectNumber];
				while(actorTable[currentObjectNumber-1] < j)
				{
					viewerListPtr = viewerListPtr->prev;
					j--;
				}
				currentObjectNumber = currentObjectNumber-1;
			}
			else
			{
				currentObjectNumber = 0;			
			}
		
			scrollPos = (float)currentObjectNumber;
			
			if(viewerListPtr->type & ACTORTYPE_ACTOR) PrepViewerForUpdate();														
		}


#if BPLATFORM == PC
		if (!objectListStatus && !controller1.lshiftChannel->value )
		{
			// Rotate camera around actor
			yRot = controller1.x1*RAD(180.0f/fps);
		
			if ((controller1.y1) && (!objectListStatus))
			{
				xRot += controller1.y1*RAD(180.0f/fps);
		
				// Limit rotations
				if (xRot < -1.0f)
					xRot = -1.0f;
				
				if (xRot > 1.0f)
					xRot = 1.0f;
			}
		}
#else
		// Rotate camera around actor
		yRot = controller1.x2*RAD(180.0f/fps);
		
		if (controller1.y2 && !objectListStatus)
		{
			xRot += controller1.y2*RAD(180.0f/fps);
	
			// Limit rotations
			if (xRot < -1.0f)
				xRot = -1.0f;
			
			if (xRot > 1.0f)
				xRot = 1.0f;
		}
#endif
		// Spin camera around actor
		EnableCameraAngleAboveHorizontal( &viewerCameraInfo, -xRot);  
		if (!spinStatus)
		{
			RotateCameraAroundLookAt(&viewerCameraInfo, -yRot, 0.0f);
		}
		else
		{
			// Auto spin around actor
			spinSpeed = -0.075f*RAD(180.0f/fps);
			RotateCameraAroundLookAt(&viewerCameraInfo, spinSpeed-yRot, 0.0f);
		}


		// zoom control
		// don't zoom in if too near
		if (!objectListStatus)
		{
#if BPLATFORM != PC
			if( controller1.y1 < 0 && zoomDistance >= ZOOM_MIN )
			{
				zoomDistance += ( ZOOM_INC*controller1.y1 );
			}
			else if( controller1.y1 > 0 && zoomDistance <= UNITTOMETER(tazObjectViewerInstance->actorInstance.actor->maxRadius * 2.0f)+2 )
			{
				zoomDistance += ( ZOOM_INC*controller1.y1 );
			}
#else
			if( controller1.minusChannel->value != 0 && zoomDistance >= ZOOM_MIN)
			{
				zoomDistance -= (ZOOM_INC / 10.0f);
			}
			else if ( controller1.equalsChannel->value != 0 && zoomDistance <= UNITTOMETER(tazObjectViewerInstance->actorInstance.actor->maxRadius * 2.0f)+2 )
			{
				zoomDistance += (ZOOM_INC / 10.0f);
			}
#endif
			
			SetFixedDistanceToLookAt( &viewerCameraInfo, METERTOUNIT(zoomDistance) );
		}	



		// cycle through animations

		if (viewerAnimStatus==TRUE)
		{
#if BPLATFORM != PC
			if (controller1.l1Channel->value)
#else
			if (controller1.lshiftChannel->value)
#endif
			{
#if BPLATFORM != PC
				if( ( controller1.dPadXDebounceChannel->value > 0 ) && ( (viewerAnimNumber+10) < ((int)viewerListPtr->actor->numAnimSegments-1) ) )
#else
				if( ( controller1.rbracketDebounceChannel->value != 0 ) && ( (viewerAnimNumber+10) < ((int)viewerListPtr->actor->numAnimSegments-1) ) )
#endif
				{
					viewerAnimNumber+=10;
					PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
					viewerAnimStatus = TRUE;
				}
#if BPLATFORM != PC
				if( ( controller1.dPadXDebounceChannel->value < 0 ) && ((viewerAnimNumber-10)>=0) )
#else
				if( ( controller1.lbracketDebounceChannel->value != 0 ) && ((viewerAnimNumber-10)>=0) )
#endif
				{
					viewerAnimNumber-=10;
					PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
					viewerAnimStatus = TRUE;
				}
			}
			else
			{
#if BPLATFORM != PC
				if( ( controller1.dPadXDebounceChannel->value > 0 ) && ( viewerAnimNumber < ((int)viewerListPtr->actor->numAnimSegments-1) ) )
#else
				if( ( controller1.rbracketDebounceChannel->value != 0 ) && ( viewerAnimNumber < ((int)viewerListPtr->actor->numAnimSegments-1) ) )
#endif
				{
					++viewerAnimNumber;
					PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
					viewerAnimStatus = TRUE;
				}
#if BPLATFORM != PC
				if( ( controller1.dPadXDebounceChannel->value < 0 ) && viewerAnimNumber )
#else
				if( ( controller1.lbracketDebounceChannel->value != 0 ) && viewerAnimNumber )
#endif
				{
					--viewerAnimNumber;
					PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
					viewerAnimStatus = TRUE;
				}
			}
		}

		// press square to cancel animation
#if BPLATFORM != PC
		if( controller1.squareDebounceChannel->value)
#else
		if( controller1.aDebounceChannel->value)
#endif
		{
			if ( !viewerAnimStatus && tazObjectViewerInstance->actorInstance.numAnimSegments != 0)
			{
				viewerAnimStatus = TRUE;
				PlayAnimationBySegmentPointer( tazObjectViewerInstance, &viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
			}
			else
			{
				viewerAnimStatus = FALSE;
			}
		}

#if BPLATFORM != PC
		if (controller1.crossDebounceChannel->value)
#else
		if (controller1.mDebounceChannel->value)
#endif
		{
			viewerMotionBone = !viewerMotionBone;
			if (tazObjectViewerInstance->actorInstance.motionBone == NULL || viewerMotionBone == FALSE)
			{
				SETVECTOR(tazObjectViewerInstance->actorInstance.position,0.0f,0.0f,0.0f,1.0f);
			}
		}
	}
	// sort out the camera
	UpdateCamera(&viewerCameraInfo);
	UpdateCartoonLight(&viewerCameraInfo);
	SendCamera(&viewerCameraInfo);	

	PrepActorInstances( &objectViewerInstances );
	PrepInstancesCharacterInfo(&objectViewerInstances);	

	if (!recordStatus)
	{
		if (viewerAnimStatus == TRUE)
		{
			AnimateActorInstances( &objectViewerInstances );
			UpdateInstancesCharacterInfo(&objectViewerInstances);
		}
		else
		{
			ZEROVECTOR(tazObjectViewerInstance->actorAnimation->motion);
			ZEROVECTOR(tazObjectViewerInstance->actorBehaviour->physics.velocity);
#if BPLATFORM != PC
			if(controller1.dPadXDebounceChannel->value > 0 )
#else
			if(controller1.rbracketDebounceChannel->value != 0 )
#endif
			{
				// advance the animation by one frame
				speedRatio = 1;
				fps = 60;
				AnimateActorInstances( &objectViewerInstances );
				UpdateInstancesCharacterInfo(&objectViewerInstances);
			}
#if BPLATFORM != PC
			if(controller1.dPadXDebounceChannel->value < 0 )
#else
			if(controller1.lbracketDebounceChannel->value != 0 )
#endif
			{
				// advance the animation by minus one frame
				speedRatio = -1;
				fps = -60;
				AnimateActorInstances( &objectViewerInstances );
				UpdateInstancesCharacterInfo(&objectViewerInstances);
			}
		}	

		if (zoneStatus)
			DrawZones();
	}
	else // Recording
	{	
		if (viewerAnimNumber < ((int)viewerListPtr->actor->numAnimSegments-1))
		{
			if (tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1 <= currentT2)
			{
				++viewerAnimNumber;
				PlayAnimationBySegmentPointer( tazObjectViewerInstance,&viewerAnimSegment[segmentNumbers[viewerAnimNumber]], 1, 1, 0, 0, NONE );
				objViewerScreenShotNumber = 0;
			}
			currentT2 = tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1;
		
			// advance the animation by one frame
			speedRatio = 1;
			fps = 60;
			AnimateActorInstances( &objectViewerInstances );
			UpdateInstancesCharacterInfo(&objectViewerInstances);
		}
		else
			recordStatus = FALSE;
	}

	// if motion bone
	if (viewerMotionBone && tazObjectViewerInstance->actorInstance.motionBone != NULL)
	{
		bmVectorCMul(tazObjectViewerInstance->actorBehaviour->physics.velocity,tazObjectViewerInstance->actorBehaviour->physics.velocity,1/fps);
		bmVectorAdd(tazObjectViewerInstance->actorInstance.position,tazObjectViewerInstance->actorInstance.position,tazObjectViewerInstance->actorBehaviour->physics.velocity);	

#if BPLATFORM != PC
		if(!(controller1.dPadXDebounceChannel->value < 0 ))
#else
		if(!(controller1.mDebounceChannel->value != 0 ))
#endif
		{
			if (tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1 < currentT)
			{
				// anim has looped
				SETVECTOR(tazObjectViewerInstance->actorInstance.position,0.0f,0.0f,0.0f,1.0f);
			}
			currentT = tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1;
		}
		else
		{
			if (tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1 > currentT)
			{
				// anim has looped
				SETVECTOR(tazObjectViewerInstance->actorInstance.position,0.0f,0.0f,0.0f,1.0f);
			}
			currentT = tazObjectViewerInstance->actorInstance.rootNodeInstance->animQueue[tazObjectViewerInstance->actorInstance.rootNodeInstance->currentQueue].frame1;
		}
	}
}



/* --------------------------------------------------------------------------------
   Function : FreeObjectViewer
   Purpose : frees object viewer instance list
   Parameters : 
   Returns : 
   Info : 
*/

void FreeObjectViewer( void )
{
	FreeAllActorInstances( &objectViewerInstances );
	tazObjectViewerInstance	= NULL;
	SAFE_FREE(segmentNumbers);
	SAFE_FREE(startFrames);
}

/* --------------------------------------------------------------------------------
   Function : InitViewerGrid
   Purpose : initialises grid
   Parameters : 
   Returns : 
   Info : 
*/

void InitViewerGrid( void )
{
	int counter,i;

	viewerGridStatus = 0;

	for( counter = 0; counter <= (int)(METERTOUNIT(100)); counter+=(int)(METERTOUNIT(1)) )
	{
		i = counter/250;
		BDVERTEX_SETXYZW( &viewerGridInfo[i], (float)(counter-METERTOUNIT(50)), 0.0, 0.0, 1.0 );
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawViewerGrid
   Purpose : draws grid
   Parameters : 
   Returns : 
   Info : 
*/

void DrawViewerGrid( void )
{
	int Xcounter, Ycounter;
	TBPrimVertex			tempPrimVertex[3];
	float					temp[4];

	for( Ycounter = (int)(METERTOUNIT(-50)); Ycounter <= (int)(METERTOUNIT(50)); Ycounter += (int)(METERTOUNIT(1)) )
	{
		for( Xcounter = 0; Xcounter <= 100; ++Xcounter )
		{
			// calculate temp vertices extended 1m in the +ve y axis and fill temp vertex array
			tempPrimVertex[0] = viewerGridInfo[Xcounter];
			tempPrimVertex[1] = viewerGridInfo[Xcounter+1];

			BDVERTEX_GETXYZW( &viewerGridInfo[Xcounter], temp[0], temp[1], temp[2], temp[2] );
			BDVERTEX_SETXYZW( &tempPrimVertex[0], temp[0], (float)Ycounter, 0.0f, 1.0f );

			BDVERTEX_GETXYZW( &viewerGridInfo[Xcounter+1], temp[0], temp[1], temp[2], temp[2] );
			BDVERTEX_SETXYZW( &tempPrimVertex[1], temp[0], (float)Ycounter, 0.0f, 1.0f );
			BDVERTEX_SETXYZW( &tempPrimVertex[2], temp[0], (float)(Ycounter+METERTOUNIT(1)), 0.0f, 1.0f );

			BDVERTEX_SETRGBA(&tempPrimVertex[0],0,0,0,255);
			BDVERTEX_SETRGBA(&tempPrimVertex[1],0,0,0,255);
			BDVERTEX_SETRGBA(&tempPrimVertex[2],0,0,0,255);
		
			bdDrawPrimitive( BDPRIMTYPE_LINESTRIP, &tempPrimVertex[0], 3, BVERTTYPE_SINGLE );
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawViewerGround
   Purpose : draws ground
   Parameters : 
   Returns : 
   Info : 
*/

void DrawViewerGround( void )
{
	int Xcounter, Zcounter;
	char	tileType = 0;
	char	startTileType = 0;
	
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	// CMD: if we don't explicitly set the texture, the ground will be drawn using the last texture used on the object
	// CMD: oh and if we do explicitly set the texture the viewer crashes
//	bdSetTexture(0, viewerTexture);
//	bdSetTexture(1, viewerTexture);

    //bdSetTexture(0,NULL);
	//bdSetTexture(1,NULL);
	bdSetObjectMatrix(bIdentityMatrix);

	if (tazObjectViewerInstance->actorInstance.numAnimSegments == 0)
		groundHeight = tazObjectViewerInstance->actorInstance.actor->yMin -0.2f;
	else
		groundHeight = -0.2f;

	groundSize = UNITTOMETER(tazObjectViewerInstance->actorInstance.actor->maxRadius * 2.0f);

	if (groundSize < 4.0f)
		groundSize = 4.0f;
	else if (groundSize > 20.0f)
		groundSize = 20.0f;
	
	for (Xcounter = (int)(METERTOUNIT(-groundSize)); Xcounter <= (int)(METERTOUNIT(groundSize)); Xcounter += (int)(METERTOUNIT(1)))
	{
		tileType = startTileType;
		startTileType = !startTileType;

		for (Zcounter = (int)(METERTOUNIT(-groundSize)); Zcounter <= (int)(METERTOUNIT(groundSize)); Zcounter += (int)(METERTOUNIT(1)))
		{
			if (tileType == 0)
			{
				/*
				// NH: Old Purple Colour
				BDVERTEX_SETRGBA( groundVerts,   200,0,200, 155 );
				BDVERTEX_SETRGBA( groundVerts+1, 200,0,200, 155 );
				BDVERTEX_SETRGBA( groundVerts+2, 200,0,200, 155 );
				BDVERTEX_SETRGBA( groundVerts+3, 200,0,200, 155 );
				*/
				// NH: Green colour
				/*
				BDVERTEX_SETRGBA( groundVerts,   0,180,0, 155 );
				BDVERTEX_SETRGBA( groundVerts+1, 0,180,0, 155 );
				BDVERTEX_SETRGBA( groundVerts+2, 0,180,0, 155 );
				BDVERTEX_SETRGBA( groundVerts+3, 0,180,0, 155 );
				*/	
				BDVERTEX_SETRGBA( groundVerts,   255,225,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+1, 255,225,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+2, 255,225,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+3, 255,225,255, 80 );				
				
				tileType = 1;
			}
			else
			{
				/*
				// NH: Old purple colour
				BDVERTEX_SETRGBA( groundVerts,   255,0,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+1, 255,0,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+2, 255,0,255, 80 );
				BDVERTEX_SETRGBA( groundVerts+3, 255,0,255, 80 );
				*/
				// NH: Green colour
				/*
				BDVERTEX_SETRGBA( groundVerts,   0,225,0, 80 );
				BDVERTEX_SETRGBA( groundVerts+1, 0,225,0, 80 );
				BDVERTEX_SETRGBA( groundVerts+2, 0,225,0, 80 );
				BDVERTEX_SETRGBA( groundVerts+3, 0,225,0, 80 );
				*/
				
				BDVERTEX_SETRGBA( groundVerts,   85,50,85, 100 );
				BDVERTEX_SETRGBA( groundVerts+1, 85,50,85, 100 );
				BDVERTEX_SETRGBA( groundVerts+2, 85,50,85, 100 );
				BDVERTEX_SETRGBA( groundVerts+3, 85,50,85, 100 );				
				
				tileType = 0;
			}
			
			BDVERTEX_SETXYZW( groundVerts, Xcounter-(METERTOUNIT(0.5f)), groundHeight, Zcounter-(METERTOUNIT(0.5f)), 1.0f );
			BDVERTEX_SETNORMAL( groundVerts, 0.0f, 1.0f, 0.0f );
			BDVERTEX_SETUV( groundVerts, 0.0f, 0.0f);

    		BDVERTEX_SETXYZW( groundVerts+1, Xcounter-(METERTOUNIT(0.5f)), groundHeight, Zcounter+(METERTOUNIT(0.5f)), 1.0f );
			BDVERTEX_SETNORMAL( groundVerts+1, 0.0f, 1.0f, 0.0f );
			BDVERTEX_SETUV( groundVerts+1, 1.0f, 0.0f);

			BDVERTEX_SETXYZW( groundVerts+2, Xcounter+(METERTOUNIT(0.5f)), groundHeight, Zcounter-(METERTOUNIT(0.5f)), 1.0f );
			BDVERTEX_SETNORMAL( groundVerts+2, 0.0f, 1.0f, 0.0f );
			BDVERTEX_SETUV( groundVerts+2, 0.0f, 1.0f);

			BDVERTEX_SETXYZW( groundVerts+3, Xcounter+(METERTOUNIT(0.5f)), groundHeight, Zcounter+(METERTOUNIT(0.5f)), 1.0f );
			BDVERTEX_SETNORMAL( groundVerts+3, 0.0f, 1.0f, 0.0f );
			BDVERTEX_SETUV( groundVerts+3, 1.0f, 1.0f);

			bdDrawPrimitive( BDPRIMTYPE_TRIANGLESTRIP, groundVerts, 4, BVERTTYPE_SINGLE );
		}
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

}

/* --------------------------------------------------------------------------------
   Function : DrawZones
   Purpose : draws zones (yMin, maxRadius, etc..)
   Parameters : 
   Returns : 
   Info : 
*/

void DrawZones( void )
{
	TBPrimVertex	zoneVerts[8];

	float yMin, yMax, xMin, xMax, zMin, zMax,maxRadius;
	float counter;

	xMin = -tazObjectViewerInstance->actorInstance.actor->xMin;
	xMax = -tazObjectViewerInstance->actorInstance.actor->xMax;
	yMin = tazObjectViewerInstance->actorInstance.actor->yMin;
	yMax = tazObjectViewerInstance->actorInstance.actor->yMax;
	zMin = tazObjectViewerInstance->actorInstance.actor->zMin;
	zMax = tazObjectViewerInstance->actorInstance.actor->zMax;

	maxRadius = tazObjectViewerInstance->actorInstance.actor->maxRadius;

	BDVERTEX_SETRGBA( zoneVerts,   255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+1, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+2, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+3, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+4, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+5, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+6, 255, 0, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+7, 255, 0, 0, 255 );

	// Rear Lower Right
	BDVERTEX_SETXYZW( zoneVerts, xMin, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMin+25.0f, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMin, zMin+25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Rear Lower Left
	BDVERTEX_SETXYZW( zoneVerts, xMax, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMin+25.0f, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMin, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMin, zMin+25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Rear Upper Left
	BDVERTEX_SETXYZW( zoneVerts, xMax, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMax-25.0f, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMax, zMin+25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Rear Upper Right
	BDVERTEX_SETXYZW( zoneVerts, xMin, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMax-25.0f, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMax, zMin, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMax, zMin+25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Front Lower Right
	BDVERTEX_SETXYZW( zoneVerts, xMin, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMin+25.0f, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMin, zMax-25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Front Lower Left
	BDVERTEX_SETXYZW( zoneVerts, xMax, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMin+25.0f, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMin, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMin, zMax-25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Front Upper Left
	BDVERTEX_SETXYZW( zoneVerts, xMax, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMax+25.0f, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMax, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMax, yMax-25.0f, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMax, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMax, yMax, zMax-25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	// Front Upper Right
	BDVERTEX_SETXYZW( zoneVerts, xMin, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, xMin-25.0f, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+2, xMin, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, xMin, yMax-25.0f, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+4, xMin, yMax, zMax, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, xMin, yMax, zMax-25.0f, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	BDVERTEX_SETRGBA( zoneVerts,   0, 0, 255, 255 );
	BDVERTEX_SETRGBA( zoneVerts+1, 0, 0, 255, 255 );
	BDVERTEX_SETRGBA( zoneVerts+2, 0, 0, 255, 255 );
	BDVERTEX_SETRGBA( zoneVerts+3, 0, 0, 255, 255 );
	BDVERTEX_SETRGBA( zoneVerts+4, 0, 0, 255, 255 );
	BDVERTEX_SETRGBA( zoneVerts+5, 0, 0, 255, 255 );

	// X Axis
	BDVERTEX_SETXYZW( zoneVerts, maxRadius, 0.0f, 0.0f, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+1, -maxRadius, 0.0f, 0.0f, 1.0f );
	// Y Axis
	BDVERTEX_SETXYZW( zoneVerts+2, 0.0f, maxRadius, 0.0f, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+3, 0.0f, -maxRadius, 0.0f, 1.0f );
	// Z Axis
	BDVERTEX_SETXYZW( zoneVerts+4, 0.0f , 0.0f, maxRadius, 1.0f );
	BDVERTEX_SETXYZW( zoneVerts+5, 0.0f, 0.0f, -maxRadius, 1.0f );
	bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 6, BVERTTYPE_SINGLE );	

	BDVERTEX_SETRGBA( zoneVerts,   255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+1, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+2, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+3, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+4, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+5, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+6, 255, 255, 0, 255 );
	BDVERTEX_SETRGBA( zoneVerts+7, 255, 255, 0, 255 );

	// Meter markers
	for (counter = 0; counter <= maxRadius; counter += (int)METERTOUNIT(1))
	{
		BDVERTEX_SETXYZW( zoneVerts, 10.0f , 0.0f, counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+1, -10.0f, 0.0f, counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+2, 0.0f , 10.0f, counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+3, 0.0f, -10.0f, counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+4, 10.0f , 0.0f, -counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+5, -10.0f, 0.0f, -counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+6, 0.0f , 10.0f, -counter, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+7, 0.0f, -10.0f, -counter, 1.0f );
		bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	

		BDVERTEX_SETXYZW( zoneVerts, 0.0f , counter, 10.0f , 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+1, 0.0f, counter, -10.0f , 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+2, 10.0f, counter, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+3, -10.0f, counter, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+4, 0.0f , -counter, 10.0f , 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+5, 0.0f, -counter, -10.0f , 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+6, 10.0f, -counter, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+7, -10.0f, -counter, 0.0f, 1.0f );
		bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	

		BDVERTEX_SETXYZW( zoneVerts, counter, 0.0f, 10.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+1, counter, 0.0f, -10.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+2, counter, 10.0f, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+3, counter, -10.0f, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+4, -counter, 0.0f, 10.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+5, -counter, 0.0f, -10.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+6, -counter, 10.0f, 0.0f, 1.0f );
		BDVERTEX_SETXYZW( zoneVerts+7, -counter, -10.0f, 0.0f, 1.0f );
		bdDrawPrimitive( BDPRIMTYPE_LINELIST, zoneVerts, 8, BVERTTYPE_SINGLE );	
	}
}
