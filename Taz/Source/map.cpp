// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : map.cpp
//   Purpose : control the function of ingame map
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "global.h"
#include "actors.h"
#include "map.h"
#include "main.h"
#include "maths.h"
#include "control.h"
#include "icon.h"
#include "util.h"				// PP: Utility functions
#include "globalsettings.h"
#include "pause.h"
#include "cameracase.h"
#include "subtitle.h"


#define	POLAROID_APPEAR			(0)
#define	POLAROID_DISAPPEARUP	(1)
#define	POLAROID_DISAPPEARDOWN	(2)


char						ShowFlyBy = true;	// NH: Stores if flyby needs showing
char						NowShowAPostIt = false;
float						mapScale;
float						yOff;
TBRenderTarget				*target;
char						polaroidMode;
int							tweetyChannel=-1;
CAMERAINFO					polaroidSaveCamera;

int	mapCameraCases[19][8] = 
{
	{0,-1},
	{0,-1},
	{0,-1},
	
	{0,-1},
	{4,6,1,5,2,0,3,-1},	// TP: SCENE_ICE
	{0,1,2,3,4,5,6,-1},	// TP: SCENE_SAFARI
	{0,1,2,3,4,5,6,-1},	// TP: SCENE_AQUA
	{0,-1},				// TP: SCENE_ZOOBOSS

	{0,-1},				// TP: SCENE_CITYHUB
#ifdef CONSUMERDEMO	
	{0,1,2,3,-1},	// TP: SCENE_DEPTSTR
#else
	{0,1,2,3,4,5,6,-1},	// TP: SCENE_DEPTSTR
#endif
	{5,0,1,2,3,6,4,-1},	// TP: SCENE_MUSEUM
	{1,3,2,5,6,0,4,-1},	// TP: SCENE_CONSTRUCT
	{0,-1},				// TP: SCENE_CITYBOSS

	{0,-1},				// TP: SCENE_WILDWESTHUB
	{0,6,1,5,3,4,2,-1},//**// TP: SCENE_GHOSTTOWN
	{0,1,2,3,4,5,6,-1},	// TP: SCENE_GOLDMINE
	{0,1,2,3,4,5,6,-1},	// TP: SCENE_GRANDCANYON
	{0,-1},				// TP: SCENE_WESTBOSS

	{0,1,2,3,4,5,6,-1},	// TP: SCENE_TAZ
};

#ifdef CONSUMERDEMO
	int mapDemoSubs [] = {0,6,2,5,-1};
#endif

/*  --------------------------------------------------------------------------------
	Function	: Map::Init
	Purpose		: Create and setup the map system
	Parameters	: 
	Returns		: TRUE/FALSE
	Info		: 
*/
int MAP::Init()
{
	int	i;
	char	mapName[256];

	bkPrintf("Creating map system.\n");

	pointer = NULL;

/*	sprintf(mapName, "maps\\%s.bmp", ::map.mapName);
	LoadSplashScreen(&map, mapName, "splshscr");
*/	
	pointer = bkLoadTexture(NULL, "flag.bmp", 0);
	ASSERT(pointer);
	poster = bkLoadTexture(NULL, "mapwanted.bmp", 0);
	ASSERT(poster);
	posterCross = bkLoadTexture(NULL, "mapcross.bmp", 0);
	ASSERT(posterCross);
	levelComplete = bkLoadTexture(NULL, "flagexit.bmp", 0);
	ASSERT(levelComplete);

	offset = 0.0f;
	arrowAng = 0.0f;
	phase = 0.0f;
	for (i=0;i<8;i++)
	{
		postPhase[i] = bmRandf()*RAD(180.0f);
	}
	levCompletePhase = 0.0f;

	ZEROVECTOR(arrowPos);

	lastPoint = NULL;

	selected = 0;
	numPosters = 0;
	availablePosters = 0;
	numPoints = 0;

	return TRUE;
}

/*  --------------------------------------------------------------------------------
	Function	: MAP::~TAG_MAP
	Purpose		: Default destructor
	Parameters	: 
	Returns		: 
	Info		: 
*/
MAP::~TAG_MAP()
{
	MAPPOINT	*ptr;
	int			i;

	if (map.resource)
	{
		FreeSplashScreen(&map);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: MAP::Draw
	Purpose		: Draw the map splash screen along with the pointer
	Parameters	: 
	Returns		: 
	Info		: 
*/
float myval=0.1f;
void MAP::Draw(TBVector pos)
{
	MAPPOINT	*point,*ptr;
	int			i;
	TBVector	tempVector;
	MAPPOINT	*levelCompletePt = NULL;
	MAPPOINT	*selectedPost = NULL;
	float		rand;

	if (map.resource)
	{
		DrawSplashScreen(&map);
	}

	bdSetObjectMatrix(bIdentityMatrix);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);			// PP: this should anti-alias the text in the book
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	if ((poster)&&(posterCross))
	{
		for (i=0;i<numPoints;i++)
		{
			// TP: spin points to find closest
			ptr = &pointList[i];

			if (ptr->flags&MAPFLAG_POSTER)
			{
				if (i != selected)
				{
					SETVECTOR(tempVector, ptr->mapX-320.0f, 224.0f - ptr->mapY, 0.5f, 1.0f);

					ScaleScreenVectorFromPS2(tempVector);

					//postAng = bmSin(postPhase[ptr->posterNumber]+=(fTime*8.0f)) * RAD(5.0f);
					//if (postPhase[ptr->posterNumber]>TWOPI) postPhase[ptr->posterNumber] -= TWOPI;
					
					utilDraw2DSprite(poster, tempVector, 40.0f*(videoMode.xScreen/640.0f), 40.0f*(videoMode.xScreen/640.0f), 0.0f);
				}
				else
				{
					// TP: this will be drawn last
					selectedPost = ptr;
				}
			}
			if (ptr->flags&MAPFLAG_POSTERCOMPLETE)
			{
				postAng = bmSin(postPhase[ptr->posterNumber]+=(fTime*8.0f)) * RAD(5.0f);
				if (postPhase[ptr->posterNumber]>TWOPI) postPhase[ptr->posterNumber] -= TWOPI;

				utilDraw2DSprite(posterCross, tempVector, 25.0f*(videoMode.xScreen/640.0f), 25.0f*(videoMode.xScreen/640.0f), postAng);
			}
			if (ptr->flags&MAPFLAG_LEVELCOMPLETE)
			{
				if (!availablePosters)
				{
					// TP: this will be drawn last
					levelCompletePt = ptr;
				}
			}
		}

	}

	// TP: make sure level complete poster is drawn last so it stands in front of other icons
	if ((poster)&&(posterCross))
	{
		if (selectedPost)
		{
			postAng = bmSin(postPhase[selectedPost->posterNumber]+=(fTime*8.0f)) * RAD(10.0f);
			if (postPhase[selectedPost->posterNumber]>TWOPI) postPhase[selectedPost->posterNumber] -= TWOPI;

			SETVECTOR(tempVector, selectedPost->mapX-320.0f, 224.0f - selectedPost->mapY, 0.5f, 1.0f);
			
			ScaleScreenVectorFromPS2(tempVector);

			utilDraw2DSprite(poster, tempVector, 60.0f*(videoMode.xScreen/640.0f), 60.0f*(videoMode.xScreen/640.0f), postAng);
		}
		if (levelCompletePt)
		{
			postAng = bmSin(levCompletePhase+=(fTime*10.0f)) * RAD(10.0f);
			if (levCompletePhase>TWOPI) levCompletePhase -= TWOPI;

			SETVECTOR(tempVector, levelCompletePt->mapX-320.0f, 224.0f - levelCompletePt->mapY, 0.5f, 1.0f);

			ScaleScreenVectorFromPS2(tempVector);

			utilDraw2DSprite(levelComplete, tempVector, 50.0f*(videoMode.xScreen/640.0f), 50.0f*(videoMode.xScreen/640.0f), postAng,COLOUR(128,128,128,128),false,false,NULL,0.0f,-1.0f,-1.0f);
		}
	}

	point = FindClosestPoint(pos);

	if (point)
	{
		// TP: Draw pointer
		TBVector	spritePos;
	
		SETVECTOR(spritePos, point->mapX-320.0f, 224.0f-point->mapY, 0.5f, 1.0f);

		ScaleScreenVectorFromPS2(tempVector);

		bmVectorCopy(arrowPos, spritePos);

		arrowAng = bmSin(phase+=(fTime*10.0f)) * RAD(10.0f);
		if (phase>TWOPI) phase -= TWOPI;

		if (pointer)
		{
			utilDraw2DSprite(pointer, arrowPos, (50.0f/640.0f)*videoMode.xScreen, (50.0f/640.0f)*videoMode.yScreen, arrowAng,COLOUR(128,128,128,128),false,false,NULL,0.0f,-1.0f,-1.0f);
		}
	}

}

void MAP::GetMapPositionOfCurrentPoster(TBVector2D result)
{
	MAPPOINT	*ptr;

	ptr = &pointList[selected];

	if (ptr)
	{
		result[X] = ptr->mapX - 320.0f;
			
		result[Y] = 224.0f - ptr->mapY;
	}
	else
	{
		// TP: just in case, had crash on disk and am desperate
		result[X] = 0.0f;
		result[Y] = 0.0f;
	}
}


void MAP::Update()
{
	if (availablePosters)
	{
		MAPPOINT	*pnt;

		int dir = 1, i;

		availablePosters = numPosters;

		TBVector	halfMapPos, pointVector;
	
		SETVECTOR(halfMapPos, 320.0f, 224.0f, 1.0f, 1.0f);

		ScaleScreenVectorFromPS2(halfMapPos);

		//float halfMapScreenX = ((videoMode.xScreen/2.0f)/videoMode.xScreen)*640;
		//float halfMapScreenY = ((-videoMode.yScreen/2.0f)/videoMode.yScreen)*448;

		for (i = 0; i < numPosters; i++)
		{
			pnt = &pointList[i];

			if (pnt->flags&MAPFLAG_POSTERCOMPLETE)
				availablePosters--;
#if BPLATFORM == PC
			else
			{
				SETVECTOR(pointVector, pnt->mapX, pnt->mapY, 1.0f, 1.0f);
				ScaleScreenVectorFromPS2(pointVector);
	
				// NH: If mouse is over the poster, select it.
				if (((halfMapPos[X] + mouse.mousePos[X]) < (pointVector[0] + 20.0f*(videoMode.xScreen/640.0f))) &&
					((halfMapPos[X] + mouse.mousePos[X]) > (pointVector[0] - 20.0f*(videoMode.xScreen/640.0f))) &&
					((halfMapPos[Y] - mouse.mousePos[Y]) < (pointVector[1] + 20.0f*(videoMode.xScreen/640.0f))) &&
					((halfMapPos[Y] - mouse.mousePos[Y]) > (pointVector[1] - 20.0f*(videoMode.xScreen/640.0f))))
				{
					// NH: If selected is not the current poster then select it.
					if (selected != i)
					{
						selected = i;
						ShowFlyBy = true;
						NowShowAPostIt = false;
					}
				}
			}
#endif
		}

#ifdef JWEEKS
		if (!controller1.l1Channel->value)
		{
#endif

		if (availablePosters)
		{
			// PP: REMOUT: not ideal	if (controller1.dPadXDebounceChannel->value < 0 || controller1.x1DebounceChannel->value < 0 || controller1.menuLeftRepeatChannel->value)
			if(controller1.menuLeftDebounceChannel->value != 0)
			{
				dir = -1;
				selected += dir;
				ShowFlyBy = true;
			}

			// PP: REMOUT: not ideal	if (controller1.dPadXDebounceChannel->value > 0 || controller1.x1DebounceChannel->value > 0 || controller1.menuRightRepeatChannel->value)
			if(controller1.menuRightDebounceChannel->value != 0)
			{
				dir = 1;
				selected += dir;
				ShowFlyBy = true;
			}

			do
			{
				if (selected == numPosters)
					selected = 0;
				else if (selected < 0)
					selected = numPosters - 1;

				pnt = &pointList[selected];

				if (pnt->flags&MAPFLAG_POSTERCOMPLETE)
					selected += dir;
				else
					dir = 0;
			}
			while(dir);

			if (OKBUTTON || (mouse.active && controller1.mb1DebounceChannel->value && ShowFlyBy))
			{
				StartIngameCameraCase(selected);
				
				// PP: Force-debounce ok button(s)
				FORCE_OKBUTTON_DEBOUNCE;
			}

			if (!ShowFlyBy || NowShowAPostIt)
			{
				int msoids = 7;
			}
			if (controller1.squareDebounceChannel->value || ((mouse.active && controller1.mb1DebounceChannel->value && !ShowFlyBy) || (NowShowAPostIt)))
			{
				ShowFlyBy = false;
				StartMapHint(selected);
				controller1.squareDebounceChannel->value = controller1.mb1DebounceChannel->value = 0;
			}
		}
#ifdef JWEEKS
		}
#endif
	}
	else
	{
		selected = -1;
	}
}

/*  --------------------------------------------------------------------------------
	Function	: MAP::FindClosestPoint
	Purpose		: Find the closest map point on level
	Parameters	: 
	Returns		: MAPPOINT
	Info		: 
*/
MAPPOINT *MAP::FindClosestPoint(TBVector pos)
{
	MAPPOINT	*point=NULL,*ptr;
	float		dist,closestDist;
	int			i;

	for (i=0;i<numPoints;i++)
	{
		// TP: spin points to find closest
		ptr = &pointList[i];

		if ((ptr->flags&MAPFLAG_POSTER)||(ptr->flags&MAPFLAG_LEVELCOMPLETE)) continue;

/*		if (IsPointWithinBox(&ptr->bbox, ptr->invMatrix, pos))
		{
			point = ptr;
			return point;
		}
*/		if (point==NULL)
		{
			point = ptr;
			closestDist = bmVectorDistance(pos,ptr->position);
		}
		else
		{
			if ((dist = bmVectorDistance(pos,ptr->position)) < closestDist)
			{
				closestDist = dist;
				point = ptr;
			}
		}
	}

	return point;
}

/*  --------------------------------------------------------------------------------
	Function	: MAP::AddPoint
	Purpose		: Adds a map marker to the current
	Parameters	: ActorInstance, map coords
	Returns		: 
	Info		: actorInstance can be free'd after this function is called
*/
void MAP::AddPoint(TBVector position, short mapX, short mapY, uchar flag, uchar posterNumber)
{
	MAPPOINT	*point;

	point = &pointList[numPoints];

	if (numPoints==MAP_MAXPOINTS)
	{
		bkPrintf("*** WARNING *** Could not allocate map point.\n");
		return;
	}

	if (position)
	{
		bmVectorCopy(point->position,position);
	}

	// TP: Get boundary information
//	GetBoxBoundsAndMatrix(actorInstance, &point->bbox, point->invMatrix);
	point->mapX = mapX;
	point->mapY = mapY;

	if (flag&MAPFLAG_POSTER)
	{
		// TP: check if poster has already been collected
		if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[::map.sceneNumber].posters[posterNumber-1])
		{
			flag |= MAPFLAG_POSTERCOMPLETE;
		}
		else
		{
			availablePosters++;
		}

		numPosters++;
	}
	point->flags = flag;
	
	point->posterNumber = posterNumber;

	numPoints++;
}

/*  --------------------------------------------------------------------------------
	Function	: MAP::PosterCompleted
	Purpose		: Mark the poster on the map as completed
	Parameters	: poster number
	Returns		: 
	Info		: 
*/
void MAP::PosterCompleted(int number)
{
	MAPPOINT	*ptr;
	int			i;

	for (i=0;i<numPoints;i++)
	{
		// TP: spin points to find closest
		ptr = &pointList[i];

		if ((ptr->flags&MAPFLAG_POSTER)&&(ptr->posterNumber==number))
		{
			ptr->flags |= MAPFLAG_POSTERCOMPLETE;
			return;
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: DrawPolaroid
	Purpose		: like it sounds
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawPolaroid(void)
{
	bdSetViewport(0, 0, videoMode.xScreen, videoMode.yScreen);
	bdSetupView();

	popRenderTarget(0,0,0,0, 1, BSETRENDERTARGET_CLEARZ);

	DefaultPauseUpdateFunc(0);

	TBVector2D	pos;	// TP: changed this 'cause its not a good idea to rely on the return address of a local
	map.pauseMap->GetMapPositionOfCurrentPoster(pos);

	TBVector2D		TVPos;			// PP: destination position of the photo
	TBVector2D		PPos;			// PP: source position of the photo
	TBVector2D		EndPos;			// PP: position of photo


	PPos[X] = pos[X];
	PPos[Y] = pos[Y];

	if (PPos[X] > 0)
		TVPos[X] = (-160.0f / 640.0f) * videoMode.xScreen;
	else
		TVPos[X] = (160.0f / 640.0f) * videoMode.xScreen;
	
	if (PPos[Y] > 0)
		TVPos[Y] = (-90.0f / 448.0f) * videoMode.yScreen;
	else
		TVPos[Y] = (90.0f / 448.0f) * videoMode.yScreen;
	
	// PP: save render states
	SAVESTATES;

	bdBeginScene();

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdSetIdentityObjectMatrix();
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	EndPos[X] = PPos[X] + ((TVPos[X] - PPos[X]) * mapScale);
	EndPos[Y] = PPos[Y] + ((TVPos[Y] - PPos[Y]) * mapScale);

	bdSetTexture(0, NULL);

	// PP: I wouldn't normally make a silly macro for something like this, but I had a hellish whole-day bug and this made it much easier to track down.  Now that it's here I'll just leave it because it does make things a bit cleaner.
#define DRAW_POLAROID_COMPONENT(_width, _height, _colour, _centreOffX, _centreOffY, _centreOffYFactor) \
	utilDraw2DSprite(NULL, EndPos, (((_width) * mapScale)/640.0f) * videoMode.xScreen, (((_height) * mapScale)/448.0f) * videoMode.yScreen, (TVPos[X] / TVPos[Y]) * 0.125f * mapScale, (_colour), false, false, NULL, 0, (_centreOffX), (_centreOffY) + ((TVPos[Y] < 0 ? 1 : -1) * (_centreOffYFactor) * yOff));

	// PP: shadow
	DRAW_POLAROID_COMPONENT(288.0f, 258.0f, COLOUR(0, 0, 0, 64), -0.1f, 0.3f, 1.0f);

	// PP: outline round card
	DRAW_POLAROID_COMPONENT(288.0f, 258.0f, COLOUR(32, 32, 32, 128), 0.0f, 0.15f, 1.0f);

	// PP: card
	DRAW_POLAROID_COMPONENT(280.0f, 250.0f, COLOUR(225, 225, 200, 128), 0.0f, 0.15f, 1.0f);

	// PP: outline round picture
	DRAW_POLAROID_COMPONENT(243.0f, 183.0f, COLOUR(32, 32, 32, 128), 0.0f, 0.0f, 1.4f);

	// PP: picture...

	// PP: There seems to be some problem with wrapping even pow-2 linear textures on Xbox (nasty horrible lock)	// PP: TODO: investigate this.  for now, I'll just clamp
	SETWRAP(CLAMP);

	bdSetRenderTargetAsTexture(target, 0);
	DRAW_POLAROID_COMPONENT(240.0f, 180.0f, COLOUR(128, 128, 128, 128), 0.0f, 0.0f, 1.4f);

	//utilDraw2DSprite(NULL, TVPos, 240, 180, (TVPos[X] / TVPos[Y]) * 0.125);

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bdEndScene();

	// PP: IMPORTANT: unset the render target as the current texture before potentially putting texture wrapping on
	NOTEXTURE;

	// PP: restore render states
	RESTORESTATES;
}


/*  --------------------------------------------------------------------------------
	Function	: UpdateInGameCameraCase
	Purpose		: Update the map camera case
	Parameters	: 
	Returns		: 
	Info		: 
*/

void UpdateInGameCameraCase()
{
	bool exitGameIntro = false;

	if (polaroidMode == POLAROID_APPEAR)
	{
		if (mapScale < 1.0f)
			mapScale += 0.05f;
	
		if (mapScale > 1.0f)
			mapScale = 1.0f;
	}
	else if (polaroidMode == POLAROID_DISAPPEARUP)
	{
		if (yOff > -0.5f)
			yOff -= (1 - (yOff / -0.5f)) * 0.2f;
	
		if (yOff < -0.4f)
		{
			yOff = -0.4f;
			polaroidMode = POLAROID_DISAPPEARDOWN;
		}
	}
	else
	{
		if (yOff < 3.0f)
			yOff += 0.15f;
	
		if (yOff > 3.0f)
			yOff = 3.0f;
	}

	if ((camera[0].cameraCase)&&(camera[0].cameraCase->time > camera[0].cameraCase->caseTime-1.0f)&&(camera[0].fade.GetFadeState()==FADESTATE_OFF))
	{	
		polaroidMode = POLAROID_DISAPPEARUP;

		camera[0].fade.StartFadeDown(FADETYPE_NORMAL,1.0f);
	}

	if (controller1.startDebounceChannel->value || 
		controller1.crossDebounceChannel->value ||
#if BPLATFORM == PC
		controller1.mb1DebounceChannel->value ||
#endif // NH: #if BPLATFORM == PC
		CANCELBUTTON)
	{
		StopChannel(&tweetyChannel);
		
		if (controller1.mb1DebounceChannel->value)
			NowShowAPostIt = true;

		exitGameIntro=true;
		//StartFadeDown(&camera[0].fade,FADETYPE_NORMAL,1.0f);
		StopCameraCase(&camera[0],camera[0].cameraCase);

#if BPLATFORM == PC
		// NH: If mouse is active, but the pointer isn't being drawn, start to draw it again as the CameraCase has finished
		if (mouse.active)
			mouse.draw = true;
#endif // NH: #if BPLATFORM == PC

		while(map.intro.currentPan < map.intro.numPans)
		{
			SendCameraCaseMessages(&camera[0], map.intro.camCase[map.intro.currentPan++]);
		}
	}
	else
	{
		if ((camera[0].fade.GetFadeState() == FADESTATE_ON)||(camera[0].cameraCase==NULL))
		{
			if (camera[0].cameraCase==NULL)
			{
				// TP: Finished playing intro cams
				exitGameIntro=true;
			}
		}
	}

	if (exitGameIntro)
	{	
		bdDeleteRenderTarget(target);

		//CloseCurrentSubtitle();

		//camera[0].fade.fadeUpOnCompletion = true;
		
		camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);

		// TP: restore the old camera
		RestoreCameraSettings(&camera[0], &polaroidSaveCamera, true, true);
	
		SetGameState(&gameStatus,GAMESTATE_PAUSE,GAMESUBSTATE_NORMAL);

		//EnterPauseMode();
			
		if (map.sceneNumber!=SCENE_FRONTEND && map.sceneNumber!=SCENE_LANGUAGESELECT)
		{
			gameStatus.player1->flags &= ~(ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY);
		}
	}
}


/*  --------------------------------------------------------------------------------
	Function	: StartIngameCameraCase
	Purpose		: Play the map camera case
	Parameters	: camera number
	Returns		: 
	Info		: 
*/

void StartIngameCameraCase(int cameraNum)
{
	int width, height, rgbDepth, zDepth;

	if (gameStatus.currentState == GAMESTATE_CAMERACASE)
		return;

// PP: platform : TEMP TEST
#if(BPLATFORM == XBOX)
	// create a shared target
	if (!(target = bdCreateRenderTarget(256, 256, 16, 32)))
	{
		bkPrintf("StartIngameCameraCase: couldn't create render target\n");
		return;
	}
#elif(BPLATFORM == PC)
	// create a shared target (256x256) with colour & Z matching the current screen colour depth
	if (!(target = bdCreateRenderTarget(256, 256, videoMode.bppScreen, videoMode.bppScreen)))
	{
		bkPrintf("StartIngameCameraCase: couldn't create render target\n");
		return;
	}
#else
	// create a shared target (256x256) with 16bit colour & 32bit Z
	if (!(target = bdCreateRenderTarget(256, 256, 16, 32)))
	{
		bkPrintf("StartIngameCameraCase: couldn't create render target\n");
		return;
	}
#endif

	// TP: Save existing camera so we can return
	SaveCameraSettings(&polaroidSaveCamera, &camera[0]);
	// TP: dont let the new cameracase stop the camera case that was playing on the camera when entering the polaroid
	camera[0].cameraCase = NULL;

	// PP: You gotta push before ya pop!™
	pushRenderTarget(target, 0,0,0,255, 1, BSETRENDERTARGET_CLEAR);

	PlaySample("cameraclick.wav", 255);

#ifdef CONSUMERDEMO
	tweetyChannel = PlaySample(mainStringTable->strings[introCamSubtitles[map.sceneNumber][mapDemoSubs[cameraNum]]].audioFilename,255);
#else
	tweetyChannel = PlaySample(mainStringTable->strings[introCamSubtitles[map.sceneNumber][mapCameraCases[map.sceneNumber][cameraNum]]].audioFilename,255);
#endif

	// retrieve the dimensions and depths that were actually used
	bdGetRenderTargetInfo(target, &width,&height, &rgbDepth, &zDepth);
	
	gameStatus.player1->flags |= ACTORFLAG_DONTDRAW|ACTORFLAG_NOCOLLISION|ACTORFLAG_NOVELOCITY;
	SetGameState(&gameStatus,GAMESTATE_CAMERACASE,GAMESUBSTATE_NORMAL);
	//ExitPauseMode();
	
	map.intro.currentPan = cameraNum;

#if BPLATFORM == PC
		// NH: If mouse is active, leave it alone but stop drawing the pointer as a cameracase has started.
		if (mouse.active)
			mouse.draw = false;
#endif // NH: #if BPLATFORM == PC

	//Subtitle(introCamSubtitles[map.sceneNumber][cameraNum],map.intro.camCase[cameraNum]->caseTime-1.0f);
	StartCameraCase(map.intro.camCase[mapCameraCases[map.sceneNumber][cameraNum]],&camera[0]);

	camera[0].fade.StartFadeUp(FADETYPE_NORMAL,1.0f);

	mapScale = 0;
	yOff = 0;

	polaroidMode = POLAROID_APPEAR;
}


/*  --------------------------------------------------------------------------------
	Function	: MAP::LoadMapSplash
	Purpose		: Load the map splash screen
	Parameters	: nothing
	Returns		: 
	Info		: Moved from the header file, cos I couldn't debug it properly there
*/
#if BPLATFORM==GAMECUBE
void MAP::LoadMapSplash(void)
{
	// IH: This is a bit of a hack to remove the "_nocol" bit from the end of the mapname
	int		i, stringLen;
	char	mapName[256];
	char	finalName[256];

	stringLen = strlen(::map.mapName) - strlen("_nocol");

	if( strcmp("_nocol", (::map.mapName+stringLen))==0 )
	{
		for( i = 0; i < stringLen; i++ )
		{
			mapName[i] = ::map.mapName[i];
		}
		mapName[i] = '\0';
		sprintf(finalName, "maps\\%s.bmp", mapName);
		bkPrintf("finalName: %s\n", finalName);
	}
	else
	{
		sprintf(finalName, "maps\\%s.bmp", ::map.mapName);
	}

	LoadSplashScreen(&map, finalName, "splshscr");
}
#endif