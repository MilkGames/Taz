// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : MonitorFX.cpp
//   Purpose : Computer monitor effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "maths.h"
#include "main.h"
#include "actors.h"				// PP: needed by control.h
#include "control.h"			// PP: pad input stuff
#include "Timer.h"				// PP: rectangles
#include "SFX.h"				// PP: sound effects
#include "text2.h"				// PP: game text enumeration
#include "TextFX.h"				// PP: text effects
#include "MonitorFX.h"			// PP: computer monitor effects
#include "attach.h"				// PP: attached objects
#include "scene.h"				// PP: scene stuff
#include "Cheats.h"				// PP: Cheats

#define MFX_SCREEN_WIDTH				4.0f						// PP: Screen width - but this is scaled to fit in the init methods
#define MFX_SCREEN_HEIGHT				3.0f						// PP: Screen height - but this is scaled to fit in the init methods
#define MFX_WATCH_WIDTH					2.4f
#define MFX_WATCH_HEIGHT				1.5f//1.8f


// PP: zbias for monitor fx
#if(BPLATFORM == PS2)// PP: sodding ps2
#define MFX_STANDARD_Z_BIAS				(-5000.0f)
#else// PP: !ps2
#define MFX_STANDARD_Z_BIAS				(-0.1f)
#endif// PP: !ps2


// PP: end-of-project bodge
// PP: array of monitor effects that are simply hardcoded into the level.
// PP: Who'd've thought that specialtype_monitor stuff would've been wasted work eh?!
static LINKEDLIST<MONITOREFFECT>		MonitorFX_levelMonitors;


unsigned char WindowsColours[NUM_WINDOWS_COLOURS][3]=		// PP: the standard Windows colours, for complete authenticity...
{
	{64,64,64},
	{64,0,0},
	{64,64,0},
	{0,64,0},
	{0,64,64},
	{0,0,64},
	{64,0,64},
	{64,64,32},
	{0,32,32},
	{0,64,128},
	{0,32,64},
	{32,0,128},
	{64,32,0},						
	// PP: ******** FIRST_PRETTY_WINDOWS_COLOUR (13) colours from here on are nice & lively
	{128,128,128},
	{96,96,96},
	{128,0,0},
	{128,128,0},
	{0,128,0},
	{0,128,128},
	{0,0,128},
	{128,0,128},
	{128,128,64},
	{0,128,64},
	{64,128,128},
	{64,64,128},
	{128,0,64},
	{128,64,32}
};

// PP: ~~~~~~~~~~~~~~~~~~ TEMPORARY TEST DATA ...

// PP: #define TEST_WATCH

#define MFX_TEST_XANG				0.243903f
#define MFX_TEST_YANG				0.943398f
#define MFX_TEST_ZANG				0.0f
									
#define MFX_TEST_XPOS				3110.0f
#define MFX_TEST_YPOS				2426.25f
#define MFX_TEST_ZPOS				(-11542.5f)
									
#define MFX_TEST_SCALE				49.5f

// PP: number of monitor effect types to choose from
#ifdef TEST_WATCH					
#define MFX_NUM_EFFECTS				5
#else								
#define MFX_NUM_EFFECTS				4
#endif								
									
static int effect=0;				
									
static MFXMARQUEE					testMarquee;
static MFXMYSTIFY					testMystify;
static MFXFLYINGWINDOWS				testFlyingWindows;
static MFXFLYINGOBJECTS				testFlyingObjects;
									
#define NUM_MONITOR_EFFECTS			4							// PP: number of monitor effect types to choose from (ones that we'd want on actual monitor screens that is, so not the likes of the watch)
									
#ifdef TEST_WATCH					
static MFXWATCH						testWatch;
#endif

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PP: initialise static texture ptrs...

TBTexture*							MFXFLYINGWINDOWS::texture=NULL;
TBTexture*							MFXFLYINGOBJECTS::texture=NULL;
TBTexture*							MFXWATCH::texture=NULL;

// PP: PLAYER'S DIGITAL WATCH
MFXWATCH							MonitorFX_watch;

#define MFX_FRAMES					4							// PP: framerate for all monitor effects - let's get that authentic 486 chug!

#define MFX_SCREENSAVER_BG_R		25							// PP: background colour component for the screen savers
#define MFX_SCREENSAVER_BG_G		25							// PP: background colour component for the screen savers
#define MFX_SCREENSAVER_BG_B		25							// PP: background colour component for the screen savers


/* --------------------------------------------------------------------------------
   Function : CreateMonitorObject
   Purpose : creates an object with a monitor effect on it
   Parameters : ACTORINSTANCE for which to create the effect
   Returns : TRUE for success, FALSE for failure
   Info : 
*/
#define MFX_XPOSOFFSET		METERTOUNIT(-0.016f)
#define MFX_YPOSOFFSET		METERTOUNIT(0.155f)
#define MFX_ZPOSOFFSET		METERTOUNIT(0.1655f)//METERTOUNIT(0.165f)

#define MFX_XANGOFFSET		0.244f//0.235f
#define MFX_YANGOFFSET		0.94f
#define MFX_ZANGOFFSET		0.0f

int CreateMonitorObject(struct TAG_ACTORINSTANCE *const actorInstance)
{
	TBVector		screenPos;
	TBVector		screenAng;
	TBVector		rotVec;
	TBVector		tempVec;

	actorInstance->special = SPECIALTYPE_MONITOR;
	if((actorInstance->monitorInfo = CreateMonitorInfo()) == NULL)
	{
		return FALSE;
	}

	// PP: set up the initial offset from base of monitor to screen
	SETVECTOR(screenPos, MFX_XPOSOFFSET, MFX_YPOSOFFSET, MFX_ZPOSOFFSET, 0.0f);

//	bkPrintf("CreateMonitorObject quat %5.2f, %5.2f, %5.2f, %5.2f\n", actorInstance->actorInstance.orientation[X], actorInstance->actorInstance.orientation[Y], actorInstance->actorInstance.orientation[Z], actorInstance->actorInstance.orientation[W]);

	// PP: convert the orientation of the monitpr
	bmQuatToRotation(rotVec, actorInstance->actorInstance.orientation);
	
//	bkPrintf("CreateMonitorObject vec %5.2f, %5.2f, %5.2f, %5.2f\n", rotVec[X], rotVec[Y], rotVec[Z], rotVec[W]);
	SETVECTOR(rotVec, rotVec[X]*rotVec[W], rotVec[Y]*rotVec[W], rotVec[Z]*rotVec[W], 1.0f);// PP: 1.0f?

	// PP: set up the angle of the screen
	SETVECTOR(screenAng, Aabs(MFX_XANGOFFSET+rotVec[X]), Aabs(MFX_YANGOFFSET+(TWOPI-rotVec[Y])), Aabs(MFX_ZANGOFFSET+rotVec[Z]), 1.0f);// PP: 1.0f?

	// PP: rotate the screen's positional offset by the the final angle of the screen
	bmVectorRotateXYZ(tempVec, screenPos, screenAng[X], screenAng[Y], screenAng[Z]);

	// PP: add the monitor's position to the rotated offset
	bmVectorAdd(screenPos, tempVec, actorInstance->actorInstance.position);


	
	// PP: create the actual effect
	switch(bmRand()%MFX_NUM_EFFECTS)
	{
		case 0:
		{
			actorInstance->monitorInfo->effect=new MFXMARQUEE(screenPos[X], screenPos[Y], screenPos[Z], screenAng[X], screenAng[Y], screenAng[Z], MFX_TEST_SCALE);
			ASSERT(actorInstance->monitorInfo->effect);
		}
		break;

		case 1:
		{
			actorInstance->monitorInfo->effect=new MFXMYSTIFY(screenPos[X], screenPos[Y], screenPos[Z], screenAng[X], screenAng[Y], screenAng[Z], MFX_TEST_SCALE);
			ASSERT(actorInstance->monitorInfo->effect);
		}
		break;

		case 2:
		{
			actorInstance->monitorInfo->effect=new MFXFLYINGOBJECTS(screenPos[X], screenPos[Y], screenPos[Z], screenAng[X], screenAng[Y], screenAng[Z], MFX_TEST_SCALE);
			ASSERT(actorInstance->monitorInfo->effect);
		}
		break;

		case 3:
		{
			actorInstance->monitorInfo->effect=new MFXFLYINGWINDOWS(screenPos[X], screenPos[Y], screenPos[Z], screenAng[X], screenAng[Y], screenAng[Z], MFX_TEST_SCALE);
			ASSERT(actorInstance->monitorInfo->effect);
		}
		break;

		default:
		{
			actorInstance->monitorInfo->effect=NULL;
		}
	}

	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
/*	SetCollisionCallback(actorInstance, RespondToShatterCollision);*/
	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : CreateMonitorInfo
   Purpose : creates a monitor info struct
   Parameters : 
   Returns : ptr to new monitor info struct, or NULL
   Info : 
*/

MONITOREFFECTINFO *CreateMonitorInfo(void)
{
	MONITOREFFECTINFO		*tempInfo;

	if(tempInfo = (MONITOREFFECTINFO*)ZALLOC(sizeof(MONITOREFFECTINFO)))
	{
		return tempInfo;
	}
	else
	{
		bkPrintf("   *** WARNING ***, could not create MONITOREFFECTINFO structure\n");
		return NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawInstanceList
	Purpose 	: draw all monitors in an instance list
	Parameters 	: actor instance list from which to draw monitors
	Returns 	: 
	Info 		: 
*/
void MonitorFX_drawInstanceList(struct TAG_ACTORINSTANCELIST* const actorInstanceList)
{
	ACTORINSTANCE *ptr;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->monitorInfo != NULL)
		{
			if(ptr->special == SPECIALTYPE_MONITOR)
			{
				if(ptr->onScreen && (ptr->currentAlpha > 0))
				{
					// PP: TODO: TAKE INTO ACCOUNT ptr->currentAlpha (0..256)?  Do I need to?

					ptr->monitorInfo->effect->draw();
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_update
	Purpose 	: update all monitors in an instance list
	Parameters 	: actor instance list in which to update monitors
	Returns 	: 
	Info 		: 
*/
void MonitorFX_update(ACTORINSTANCELIST* const actorInstanceList)
{
	ACTORINSTANCE *ptr;

	for (ptr = actorInstanceList->head.next;ptr != &actorInstanceList->head;ptr = ptr->next)
	{
		if(ptr->monitorInfo != NULL)
		{
			if(ptr->special == SPECIALTYPE_MONITOR)
			{
				if(ptr->onScreen && (ptr->currentAlpha > 0))
				{
					ptr->monitorInfo->effect->update();
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawTest
	Purpose 	: draw all test monitor effects
	Parameters 	: 
	Returns 	: 
	Info 		: FOR TESTING ONLY
*/
void MonitorFX_drawTest(void)
{
	switch(effect)
	{
	case 0:
		testMarquee.draw();
		break;
	case 1:
		testMystify.draw();
		break;
	case 2:
		testFlyingWindows.draw();
		break;
	case 3:
		testFlyingObjects.draw();
		break;
#ifdef TEST_WATCH
	case 4:
		testWatch.draw();
		break;
#endif
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_updateLevelMonitors
	Purpose 	: update all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_updateLevelMonitors(void)
{
	MONITOREFFECT*	effect;

	for(int i=MonitorFX_levelMonitors.getSize()-1; i>=0; i--)
	{
		effect=MonitorFX_levelMonitors.get(i);

		if(gameStatus.multiplayer.numPlayers == 1)
		{
			effect->updateVisibilityStatus();			
		}

		if((gameStatus.multiplayer.numPlayers == 2) || effect->visible)
		{
			MonitorFX_levelMonitors[i].update();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawLevelMonitors
	Purpose 	: draw all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_drawLevelMonitors(void)
{
	MONITOREFFECT*	effect;

	SETPROJECTION(3D);

	for(int i=MonitorFX_levelMonitors.getSize()-1; i>=0; i--)
	{
		effect=MonitorFX_levelMonitors.get(i);

		if(gameStatus.multiplayer.numPlayers == 2)
		{
			effect->updateVisibilityStatus();
		}

		if(effect->visible)
		{
			effect->draw();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_addLevelMonitor
	Purpose 	: add a hardcoded monitor effect to the level
	Parameters 	: xyz pos, xy ang
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_addLevelMonitor(const float x, const float y, const float z, const float ax, const float ay)
{
	static uchar numCalls=0;

	MONITOREFFECT*	effect;

	// PP: create the actual effect
	switch((bmRand()+numCalls++)%MFX_NUM_EFFECTS)// PP: +numCalls++: otherwise the 2 screens tended to start with the same effect on each; not too sure why
	{
		case 0:
		{
			effect=new MFXFLYINGWINDOWS(x, y, z, ax, ay, 0.0f, MFX_TEST_SCALE);
		}
		break;

#if((BPLATFORM != PS2)/* PP: REMOUT: ironically, BVERSION_CHECK itself won't exist till the next build of Babel!	|| (BVERSION_CHECK(63,01))*/)
		// PP: IFOUT: getting some wacko ASSERT about too many points in a strip???
		// PP: NOTE the handy BVERSION_CHECK macro - I use this here because I know that the next version of Babel (the one after 63.00) will have fixed the problem.
		case 1:
		{
			effect=new MFXMYSTIFY(x, y, z, ax, ay, 0.0f, MFX_TEST_SCALE);
		}
		break;
#endif// PP: !ps2

		case 2:
		{
			effect=new MFXFLYINGOBJECTS(x, y, z, ax, ay, 0.0f, MFX_TEST_SCALE);
		}
		break;

		default:
		case 3:
		{
			effect=new MFXMARQUEE(x, y, z, ax, ay, 0.0f, MFX_TEST_SCALE);
		}
		break;
	}

	MonitorFX_levelMonitors.append(effect);
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_deleteLevelMonitors
	Purpose 	: delete all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_deleteLevelMonitors(void)
{
	MonitorFX_levelMonitors.kill();
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_updateTest
	Purpose 	: update all monitor effects
	Parameters 	: 
	Returns 	: 
	Info 		: FOR TESTING ONLY
*/
void MonitorFX_updateTest(void)
{
	if(controller1.r3DebounceChannel->value != 0)								// PP: r3 re-initialises the test screen savers
	{
		MonitorFX_init();
	}

	if(controller1.r2DebounceChannel->value != 0)								// PP: l3 switches between the test screen savers
	{
		effect=LOOPCONSTRAIN(effect+1, 0, MFX_NUM_EFFECTS);
	}

	// PP: update each of the test screen savers
	testMarquee.update();
	testMystify.update();
	testFlyingWindows.update();
	testFlyingObjects.update();

#ifdef TEST_WATCH
	testWatch.update();
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_init
	Purpose 	: init stuff for monitor effects at the beginning of the scene
	Parameters 	: 
	Returns 	: 
	Info 		: must be called after the level package has finished loading
*/
void MonitorFX_init(void)
{
	MFXFLYINGWINDOWS::sceneInit();
	MFXFLYINGOBJECTS::sceneInit();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													MONITOR EFFECT BASE CLASS

										All monitor effects should inherit from this class
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::drawBackground
	Purpose 	: draw a monitor effect background
	Parameters 	: background r, g, b, width and height
	Returns 	: 
	Info 		: 
*/
void MONITOREFFECT::drawBackground(const unsigned char r, const unsigned char g, const unsigned char b,
								   const float width, const float height)
{
	float			penX, penY;									// PP: x & y position variables for plotting verts
	TBPrimVertex	vert[4];									// PP: vertex array


	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	// PP: ~~~~~~~~~~~~~~~~ SET OBJECT MATRIX

	bdSetObjectMatrix(this->objectMatrix);

	// PP: ~~~~~~~~~~~~~~~~ DRAW BACKGROUND

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, 1);
//	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);
	bdSetTexture(0, NULL);

	penX=-(width*0.5f);
	penY=(height*0.5f);
	BDVERTEX_SETXYZW(&vert[0],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[0],r,g,b,255);
	BDVERTEX_SETUV(&vert[0],0.0f,0.0f);
	
	penX+=width;
	BDVERTEX_SETXYZW(&vert[1],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[1],r,g,b,255);
	BDVERTEX_SETUV(&vert[1],1.0f,0.0f);

	penX-=width;
	penY-=height;
	BDVERTEX_SETXYZW(&vert[2],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[2],r,g,b,255);
	BDVERTEX_SETUV(&vert[2],0.0f,1.0f);

	penX+=width;
	BDVERTEX_SETXYZW(&vert[3],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[3],r,g,b,255);
	BDVERTEX_SETUV(&vert[3],1.0f,1.0f);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);	// PP: draw	
}


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::updateVisibilityStatus
	Purpose 	: update the visibility status of the effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MONITOREFFECT::updateVisibilityStatus(void)
{
// PP: range of a monitor effect, outside which it won't be drawn or updated
#define MFX_RANGE		METERTOUNIT(15)

	// PP: =============
	// PP: DISTANCE TEST
	// PP: =============

	this->visible=(bmVectorDistance(gameStatus.viewPoint, this->effectPos) < MFX_RANGE);

	if(this->visible)
	{
		// PP: ============
		// PP: FRUSTUM TEST
		// PP: ============

		// PP: radius of effect's visibility sphere
		#define MFX_RADIUS		100

		SETPROJECTION(3D);
		bdSetIdentityObjectMatrix();
			
		this->visible=(bdTestSphereVisibility(this->effectPos, MFX_RADIUS) != FALSE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::startDraw
	Purpose 	: prepare to draw a normal monitor effect
	Parameters 	: 
	Returns 	: true if the draw should proceed; else false
	Info 		: 
*/
bool MONITOREFFECT::startDraw(void)
{
	bdSetObjectMatrix(this->objectMatrix);

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::timeToUpdate
	Purpose 	: determine whether or not it is time to update a monitor effect
	Parameters 	: ref to variable to receive time since last update
	Returns 	: whether or not it is time to update a monitor effect
	Info 		: 
*/
bool MONITOREFFECT::timeToUpdate(float& timeSinceLastUpdate)
{
	float	overshoot;
	timeSinceLastUpdate=/*gameStatus.gameTime*/gameStatus.appTime-this->lastUpdateTime;

	overshoot=timeSinceLastUpdate-(MFX_FRAMES/60.0f);

	if(overshoot<0.0f)
	{
		return false;
	}
	else
	{
		this->lastUpdateTime=gameStatus.appTime;// PP: gameStatus.gameTime;

		return true;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														3D FLYING OBJECTS

								Pastiche of the Windows wavey-in-the-wind 3D logo screensaver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define FLYINGOBJECTS_WIDTH				0.9f										// PP: width of the flag
#define FLYINGOBJECTS_HEIGHT			1.1f										// PP: height of the flag
#define FLYINGOBJECTS_WAVE_SIZE			(FLYINGOBJECTS_HEIGHT*0.04f)				// PP: amplitude of sine waves
#define FLYINGOBJECTS_WAVE_PERIOD		0.9f										// PP: period of sine waves (seconds)
#define FLYINGOBJECTS_SPEED				0.025f										// PP: flag's x&y movemvent speed 
#define FLYINGOBJECTS_TEXTURE			"extras\\blitzflag.bmp"						// PP: texture for flag
#define FLYINGOBJECTS_GLINT_R			255											// PP: glint colour
#define FLYINGOBJECTS_GLINT_G			255											// PP: glint colour
#define FLYINGOBJECTS_GLINT_B			255//180									// PP: glint colour
#define FLYINGOBJECTS_GLINT_CUTOFF		120											// PP: intensity above which glint is effected
#define FLYINGOBJECTS_MIN_INTENSITY		75											// PP: minimum intensity
#define FLYINGOBJECTS_MAX_INTENSITY		128											// PP: maximum intensity (not including glint)


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGOBJECTS::sceneInit
	Purpose 	: initialise stuff for flying objects at the start of the scene
	Parameters 	: 
	Returns 	: 
	Info 		: gets a ptr to the static texture
*/
void MFXFLYINGOBJECTS::sceneInit(void)
{
	// PP: get an up-to-date pointer to that texture.
	// PP: need to now that the texture is in the museum package rather than in resident textures

	MFXFLYINGOBJECTS::texture=bkLoadTexture(NULL,FLYINGOBJECTS_TEXTURE,0);
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGOBJECTS::init
	Purpose 	: initialise a Flying Objects screen saver
	Parameters 	: x,y,z pos; x,y,z ang; scale
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGOBJECTS::init(const float x, const float y, const float z,
							const float ax, const float ay, const float az, const float scale)
{
	TBMatrix		rotMat;
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;

	if(MFXFLYINGOBJECTS::texture == NULL)
	{
		MFXFLYINGOBJECTS::texture=bkLoadTexture(NULL,FLYINGOBJECTS_TEXTURE,0);
	}

	// PP: store position
	this->effectPos=VEC(x,y,z);

	bmMatScale(scaleMat, scale, scale, scale);
	bmMatZYXRotation(rotMat, ax, ay, az);
	bmMatTranslate(posMat, x, y, z);

	bmMatMultiply(tempMat, scaleMat, rotMat);
	bmMatMultiply(this->objectMatrix, posMat,tempMat);

	this->lastUpdateTime=0.0f;

	this->pos[X]=0.0f;
	this->pos[Y]=0.0f;

	// PP: initialise the poles
	for(int p=0;p<FLYINGOBJECTS_NUM_POLES;p++)
	{
		poles[p].offset[X]=0.0f;
		poles[p].offset[Y]=0.0f;
	}

	this->velocity[X]=FLYINGOBJECTS_SPEED;
	this->velocity[Y]=FLYINGOBJECTS_SPEED;

	this->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGOBJECTS::draw
	Purpose 	: draw a Flying Objects screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGOBJECTS::draw(void)
{
	float			penX, penY;
	TBPrimVertex	vert[2*FLYINGOBJECTS_NUM_POLES];
 	int				v=0;

	// PP: check visibility then set object matrix
	if(!this->startDraw())
	{
		return;
	}

	// PP: ~~~~~~~~~~~~~~~~ DRAW FLAG

	unsigned char	r, g, b;

	for(int p=0;p<FLYINGOBJECTS_NUM_POLES;p++)
	{
		// PP: if the pole's intensity is greater than the cutoff, effect the glint on it...
		if(this->poles[p].intensity > FLYINGOBJECTS_GLINT_CUTOFF)
		{
			r=FLYINGOBJECTS_GLINT_R;
			g=FLYINGOBJECTS_GLINT_G;
			b=FLYINGOBJECTS_GLINT_B;
		}
		else
		{
			r=g=b=this->poles[p].intensity;
		}

		penX=this->pos[X]+((p/(float)FLYINGOBJECTS_NUM_SECTIONS)*FLYINGOBJECTS_WIDTH)+this->poles[p].offset[X];
		penY=this->pos[Y]+this->poles[p].offset[Y];
		BDVERTEX_SETXYZW(&vert[v], penX, penY, 0.0f, 1.0f);
		BDVERTEX_SETRGBA(&vert[v], r, g, b, 128);
		BDVERTEX_SETUV(&vert[v], p/(float)FLYINGOBJECTS_NUM_SECTIONS, 0.0f);

		v++;

		penY-=FLYINGOBJECTS_HEIGHT;
		BDVERTEX_SETXYZW(&vert[v], penX, penY, 0.0f, 1.0f);
		BDVERTEX_SETRGBA(&vert[v], r, g, b, 128);
		BDVERTEX_SETUV(&vert[v], p/(float)FLYINGOBJECTS_NUM_SECTIONS, 1.0f);

		v++;
	}

	bdSetZBias(MFX_STANDARD_Z_BIAS);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// PP: set texture, enforcing the Qwilson cheat while we're at it
	if(CheatActive(CHEAT_QWILSON))
	{
		bdSetTexture(0, QwilsonTexture);
	}
	else
	{
		bdSetTexture(0, MFXFLYINGOBJECTS::texture);
	}

	ASSERT(v);
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,v,BVERTTYPE_SINGLE);

	// PP: ~~~~~~~~~~~~~~~~~~~

	bdSetZBias(0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGOBJECTS::update
	Purpose 	: update a Flying Objects screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGOBJECTS::update(void)
{
	float		timeSinceLastUpdate;
	float		phase;
	float		temp;

	if(!this->timeToUpdate(timeSinceLastUpdate)) return;

	for(int p=0;p<FLYINGOBJECTS_NUM_POLES-1;p++)
	{
		this->poles[p]=this->poles[p+1];
	}

	phase=(fmodf(gameStatus.gameTime, FLYINGOBJECTS_WAVE_PERIOD)/FLYINGOBJECTS_WAVE_PERIOD)*TWOPI;

	this->poles[FLYINGOBJECTS_NUM_POLES-1].intensity=FLYINGOBJECTS_MIN_INTENSITY+(unsigned char)(((PI-bmFAbs(phase-PI))/PI)*(FLYINGOBJECTS_MAX_INTENSITY-FLYINGOBJECTS_MIN_INTENSITY));

	this->poles[FLYINGOBJECTS_NUM_POLES-1].offset[Y]=bmSin(phase)*FLYINGOBJECTS_WAVE_SIZE;
	this->poles[FLYINGOBJECTS_NUM_POLES-1].offset[X]=this->poles[0].offset[Y];

	this->pos[X]-=this->poles[FLYINGOBJECTS_NUM_POLES-1].offset[X];
	this->pos[Y]-=this->poles[FLYINGOBJECTS_NUM_POLES-1].offset[Y];
	
	temp=this->pos[X]+this->velocity[X];

	if((temp<(-MFX_SCREEN_WIDTH*0.5f)+(FLYINGOBJECTS_WAVE_SIZE*4.0f))||(temp>(MFX_SCREEN_WIDTH*0.5f)-FLYINGOBJECTS_WIDTH-(FLYINGOBJECTS_WAVE_SIZE*4.0f)))
	{
		this->velocity[X]= -(this->velocity[X]);
	}
	else
	{
		this->pos[X]=temp;
	}

	temp=this->pos[Y]+this->velocity[Y];

	if((temp<(-MFX_SCREEN_HEIGHT*0.5f)+FLYINGOBJECTS_HEIGHT+(FLYINGOBJECTS_WAVE_SIZE*4.0f))||(temp>(MFX_SCREEN_HEIGHT*0.5f)-(FLYINGOBJECTS_WAVE_SIZE*4.0f)))
	{
		this->velocity[Y]= -(this->velocity[Y]);
	}
	else
	{
		this->pos[Y]=temp;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FLYING WINDOWS

										Pastiche of the Windows logo-starfield screen saver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: set a flying window to a random start position and initialise it ready to go again
#define FLYINGWINDOWS_SET_TO_START_POSITION(_window) \
		{ \
				MFX_RANDOM_WINDOWS_COLOUR((_window).r, (_window).g, (_window).b); \
				(_window).pos[X]=(_window).screenPos[X]=((bmRandf())-0.5f)*MFX_SCREEN_WIDTH; \
				(_window).pos[Y]=(_window).screenPos[Y]=((bmRandf())-0.5f)*MFX_SCREEN_HEIGHT; \
				\
				(_window).pos[Z]=0.0f; \
				(_window).size[X]=0.0f; \
				(_window).size[Y]=0.0f; \
		}

#define FLYINGWINDOWS_SPEED				0.15f							// PP: speed at which particles fly towards the screen
#define FLYINGWINDOWS_FOV_FACTOR		1.0f							// PP: determines how much the particles appear to diverge as they approach the screen
#define FLYINGWINDOWS_MIN_WIDTH			0.0f							// PP: minimum on-screen width of a particle
#define FLYINGWINDOWS_MAX_WIDTH			1.1f							// PP: width of a particle when its Z position is 1
#define FLYINGWINDOWS_MIN_HEIGHT		0.0f							// PP: minimum on-screen height of a particle
#define FLYINGWINDOWS_MAX_HEIGHT		1.0f							// PP: height of a particle when its Z position is 1
#define FLYINGWINDOWS_TEXTURE			"extras\\blitzstar.bmp"			// PP: sprite to use for particles


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGWINDOWS::sceneInit
	Purpose 	: initialise stuff for flying windows at the start of the scene
	Parameters 	: 
	Returns 	: 
	Info 		: gets a ptr to the static texture
*/
void MFXFLYINGWINDOWS::sceneInit(void)
{
	// PP: get an up-to-date pointer to that texture.
	// PP: need to now that the texture is in the museum package rather than in resident textures

	MFXFLYINGWINDOWS::texture=bkLoadTexture(NULL,FLYINGWINDOWS_TEXTURE,0);
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGWINDOWS::init
	Purpose 	: initialise a Flying Windows screen saver
	Parameters 	: x,y,z pos; x,y,z ang; scale
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGWINDOWS::init(const float x, const float y, const float z,
					const float ax, const float ay, const float az, const float scale)
{
	TBMatrix		rotMat;
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;

	// PP: store position
	this->effectPos=VEC(x,y,z);

	bmMatScale(scaleMat, scale, scale, scale);
	bmMatZYXRotation(rotMat, ax, ay, az);
	bmMatTranslate(posMat, x, y, z);

	bmMatMultiply(tempMat, scaleMat, rotMat);
	bmMatMultiply(this->objectMatrix, posMat,tempMat);

	this->lastUpdateTime=0.0f;

	// PP: set up the windows with random colours and positions on the screen
	for(int i=0;i<FLYINGWINDOWS_NUM_WINDOWS;i++)
	{
		FLYINGWINDOWS_SET_TO_START_POSITION(this->windows[i]);
	}

	this->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGWINDOWS::update
	Purpose 	: update a Flying Windows screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGWINDOWS::update(void)
{
	float			timeSinceLastUpdate;

	if(!this->timeToUpdate(timeSinceLastUpdate)) return;

	for(int i=0;i<FLYINGWINDOWS_NUM_WINDOWS;i++)
	{
		this->windows[i].pos[Z]+=(FLYINGWINDOWS_SPEED*timeSinceLastUpdate);

		// PP: calculate size	
		this->windows[i].size[X]=FLYINGWINDOWS_MIN_WIDTH+(this->windows[i].pos[Z]*(FLYINGWINDOWS_MAX_WIDTH-FLYINGWINDOWS_MIN_WIDTH));
		this->windows[i].size[Y]=FLYINGWINDOWS_MIN_HEIGHT+(this->windows[i].pos[Z]*(FLYINGWINDOWS_MAX_HEIGHT-FLYINGWINDOWS_MIN_HEIGHT));

		// PP: calculate screen position
		this->windows[i].screenPos[X]=this->windows[i].screenPos[X]*(1.0f+(this->windows[i].pos[Z]*FLYINGWINDOWS_FOV_FACTOR));
		this->windows[i].screenPos[Y]=this->windows[i].screenPos[Y]*(1.0f+(this->windows[i].pos[Z]*FLYINGWINDOWS_FOV_FACTOR));

		// PP: limit particles to extents of screen
		if(this->windows[i].screenPos[X] < ((-MFX_SCREEN_WIDTH*0.5f)+(this->windows[i].size[X]*0.5f)))
		{
			FLYINGWINDOWS_SET_TO_START_POSITION(this->windows[i]);
		}
		else if(this->windows[i].screenPos[X] > ((MFX_SCREEN_WIDTH*0.5f)-(this->windows[i].size[X]*0.5f)))
		{
			FLYINGWINDOWS_SET_TO_START_POSITION(this->windows[i]);
		}
		else if(this->windows[i].screenPos[Y] < ((-MFX_SCREEN_HEIGHT*0.5f)+(this->windows[i].size[Y]*0.5f)))
		{
			FLYINGWINDOWS_SET_TO_START_POSITION(this->windows[i]);
		}
		else if(this->windows[i].screenPos[Y] > ((MFX_SCREEN_HEIGHT*0.5f)-(this->windows[i].size[Y]*0.5f)))
		{
			FLYINGWINDOWS_SET_TO_START_POSITION(this->windows[i]);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXFLYINGWINDOWS::draw
	Purpose 	: draw a Flying Windows screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXFLYINGWINDOWS::draw(void)
{
	float			penX, penY;
	TBPrimVertex	vert[4];

	// PP: check visibility then set object matrix
	if(!this->startDraw())
	{
		return;
	}

	// PP: ~~~~~~~~~~~~~~~~ DRAW WINDOWS

	// PP: set texture, enforcing the Qwilson cheat while we're at it
	if(CheatActive(CHEAT_QWILSON))
	{
		bdSetTexture(0, QwilsonTexture);
	}
	else
	{
		bdSetTexture(0, MFXFLYINGWINDOWS::texture);
	}
	
	// PP: REMOUT: not needed!	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);

	bdSetZBias(MFX_STANDARD_Z_BIAS);

	// PP: no filtering, to preserve sprite's hatching detail
	SETFILTERING(POINT);

	for(int i=0;i<FLYINGWINDOWS_NUM_WINDOWS;i++)
	{
		penX=this->windows[i].screenPos[X]-(this->windows[i].size[X]*0.5f);
		penY=this->windows[i].screenPos[Y]+(this->windows[i].size[Y]*0.5f);
		BDVERTEX_SETXYZW(&vert[0],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[0],this->windows[i].r,this->windows[i].g,this->windows[i].b,128);
		BDVERTEX_SETUV(&vert[0],0.0f,0.0f);
		
		penX+=this->windows[i].size[X];
		BDVERTEX_SETXYZW(&vert[1],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[1],this->windows[i].r,this->windows[i].g,this->windows[i].b,128);
		BDVERTEX_SETUV(&vert[1],1.0f,0.0f);

		penX-=this->windows[i].size[X];
		penY-=this->windows[i].size[Y];
		BDVERTEX_SETXYZW(&vert[2],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[2],this->windows[i].r,this->windows[i].g,this->windows[i].b,128);
		BDVERTEX_SETUV(&vert[2],0.0f,1.0f);

		penX+=this->windows[i].size[X];
		BDVERTEX_SETXYZW(&vert[3],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[3],this->windows[i].r,this->windows[i].g,this->windows[i].b,128);
		BDVERTEX_SETUV(&vert[3],1.0f,1.0f);

		// PP: NOTE: all this monitor code was written a long long time ago.
		// PP: I'd never use seperate drawprims like this nowadays.
		// PP: The monitor effects are all nicely culled anyway, so relax fella!
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);	// PP: draw			
	}

	// PP: restore normal filtering
	RESTOREFILTERING;

	// PP: ~~~~~~~~~~~~~~~~

	bdSetZBias(0.0f);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													MYSTIFY YOUR MIND

						Near-replica of the Windows bouncing lines screen saver.  Oooh, mystifying!
					I'm so mystified I might have to rest my head for a while before commenting this bit.
												There we are, better now.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define MYSTIFY_MIN_SPEED			0.1f				// PP: minimum x or y speed for a point
#define MYSTIFY_MAX_SPEED			0.6f				// PP: maximum x or y speed for a point
#define MYSTIFY_COLOUR_CHANGE_TIME	14.0f				// PP: number of seconds between one randomly chosen colour and the next


/*	--------------------------------------------------------------------------------
	Function 	: MFXMYSTIFY::init
	Purpose 	: initialise a Mystify screen saver
	Parameters 	: x,y,z pos; x,y,z ang; scale
	Returns 	: 
	Info 		: 
*/
void MFXMYSTIFY::init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale)
{
	TBMatrix		rotMat;
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;

	// PP: store position
	this->effectPos=VEC(x,y,z);

	bmMatScale(scaleMat, scale, scale, scale);
	bmMatZYXRotation(rotMat, ax, ay, az);
	bmMatTranslate(posMat, x, y, z);

	bmMatMultiply(tempMat, scaleMat, rotMat);
	bmMatMultiply(this->objectMatrix, posMat,tempMat);

	this->timer=0.0f;

	this->lastUpdateTime=0.0f;

	for(int e=0;e<MYSTIFY_NUM_ENTITIES;e++)
	{
		MFX_RANDOM_PRETTY_WINDOWS_COLOUR(this->entities[e].oldR, this->entities[e].oldG, this->entities[e].oldB);
		MFX_RANDOM_PRETTY_WINDOWS_COLOUR(this->entities[e].newR, this->entities[e].newG, this->entities[e].newB);

		for(int v=0;v<4;v++)
		{
			// PP: initialise the point with random values
			this->entities[e].polys[0].points[v].pos[X]=((bmRand()/RANDMAX)-0.5f)*MFX_SCREEN_WIDTH;
			this->entities[e].polys[0].points[v].pos[Y]=((bmRand()/RANDMAX)-0.5f)*MFX_SCREEN_HEIGHT;
			this->entities[e].polys[0].points[v].velocity[X]=MYSTIFY_MIN_SPEED+((bmRand()/RANDMAX)*(MYSTIFY_MAX_SPEED-MYSTIFY_MIN_SPEED));
			this->entities[e].polys[0].points[v].velocity[Y]=MYSTIFY_MIN_SPEED+((bmRand()/RANDMAX)*(MYSTIFY_MAX_SPEED-MYSTIFY_MIN_SPEED));
			this->entities[e].polys[0].points[v].velocity[X]*=((bmRand()&1)?1:-1);
			this->entities[e].polys[0].points[v].velocity[Y]*=((bmRand()&1)?1:-1);
		}

		// PP: duplicate this first line into the others to start with
		for(int p=1; p<MYSTIFY_NUM_POLYS;p++)
		{
			this->entities[e].polys[p]=this->entities[e].polys[0];
		}
	}

	this->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXMYSTIFY::update
	Purpose 	: update a Mystify screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXMYSTIFY::update(void)
{
	float		timeSinceLastUpdate;

	if(!this->timeToUpdate(timeSinceLastUpdate)) return;

	TBVector2D		tempPos;

	for(int e=0;e<MYSTIFY_NUM_ENTITIES;e++)
	{
		// PP: shuffle the quads back one position...
		for(int p=MYSTIFY_NUM_POLYS-1;p>=1;p--)
		{
			this->entities[e].polys[p]=this->entities[e].polys[p-1];
		}

		// PP: update the leading quad...
		for(int v=0;v<4;v++)
		{
			tempPos[X]=this->entities[e].polys[0].points[v].pos[X]+(this->entities[e].polys[0].points[v].velocity[X]*timeSinceLastUpdate);
			tempPos[Y]=this->entities[e].polys[0].points[v].pos[Y]+(this->entities[e].polys[0].points[v].velocity[Y]*timeSinceLastUpdate);

			if((tempPos[X] < (-MFX_SCREEN_WIDTH*0.5f)) || (tempPos[X] > (MFX_SCREEN_WIDTH*0.5f)))
			{
				this->entities[e].polys[0].points[v].velocity[X]= -this->entities[e].polys[0].points[v].velocity[X];
			}
			else 
			{
				this->entities[e].polys[0].points[v].pos[X]=tempPos[X];
			}

			if((tempPos[Y] < (-MFX_SCREEN_HEIGHT*0.5f)) || (tempPos[Y] > (MFX_SCREEN_HEIGHT*0.5f)))
			{
				this->entities[e].polys[0].points[v].velocity[Y]= -this->entities[e].polys[0].points[v].velocity[Y];
			}
			else 
			{
				this->entities[e].polys[0].points[v].pos[Y]=tempPos[Y];
			}
		}
	}

	this->timer+=timeSinceLastUpdate;

	// PP: choose a new random colour if it's that time again...
	if(timer>MYSTIFY_COLOUR_CHANGE_TIME)
	{
		for(int e=0;e<MYSTIFY_NUM_ENTITIES;e++)
		{
			this->entities[e].oldR=this->entities[e].newR;
			this->entities[e].oldG=this->entities[e].newG;
			this->entities[e].oldB=this->entities[e].newB;
			MFX_RANDOM_PRETTY_WINDOWS_COLOUR(this->entities[e].newR, this->entities[e].newG, this->entities[e].newB);
		}

		this->timer=0.0f;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXMYSTIFY::draw
	Purpose 	: draw a Mystify screen saver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXMYSTIFY::draw(void)
{
	TBPrimVertex	vert[8];									// PP: vertex array
	unsigned char	r,g,b;										// PP: entitiy colours
	float			colourChangeProgress;

	// PP: check visibility then set object matrix
	if(!this->startDraw())
	{
		return;
	}

	// PP: new test
	NOTEXTURE;

	// PP: ~~~~~~~~~~~~~~~~ DRAW LINES

	bdSetZBias(MFX_STANDARD_Z_BIAS);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

	for(int e=0;e<MYSTIFY_NUM_ENTITIES;e++)
	{
		// PP: calculate current colour...
		colourChangeProgress=(this->timer/(float)MYSTIFY_COLOUR_CHANGE_TIME);
		r=(int)this->entities[e].oldR+(unsigned char)(colourChangeProgress*(this->entities[e].newR-this->entities[e].oldR));
		g=(int)this->entities[e].oldG+(unsigned char)(colourChangeProgress*(this->entities[e].newG-this->entities[e].oldG));
		b=(int)this->entities[e].oldB+(unsigned char)(colourChangeProgress*(this->entities[e].newB-this->entities[e].oldB));

		for(int p=0; p<MYSTIFY_NUM_POLYS;p++)
		{
			BDVERTEX_SETXYZW(&vert[0], this->entities[e].polys[p].points[0].pos[X], this->entities[e].polys[p].points[0].pos[Y], 0.0f, 1.0f);
			BDVERTEX_SETRGBA(&vert[0], r, g, b, 255);

			BDVERTEX_SETXYZW(&vert[1], this->entities[e].polys[p].points[1].pos[X], this->entities[e].polys[p].points[1].pos[Y], 0.0f, 1.0f);
			BDVERTEX_SETRGBA(&vert[1], r, g, b, 255);

			for(int v=1;v<4;v++)
			{
				vert[v*2]=vert[(v*2)-1];

				BDVERTEX_SETXYZW(&vert[v*2+1], this->entities[e].polys[p].points[v].pos[X], this->entities[e].polys[p].points[v].pos[Y], 0.0f, 1.0f);
				BDVERTEX_SETRGBA(&vert[v*2+1], r, g, b, 255);
			}

			vert[7]=vert[0];

			bdDrawPrimitive(BDPRIMTYPE_LINELIST,vert,8,BVERTTYPE_SINGLE);	// PP: draw	
		}
	}

	// PP: ~~~~~~~~~~~~~~~~~~~~~

	bdSetZBias(0.0f);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SCROLLING MARQUEE

										Replica of the Windows scrolling text screen saver.
						This would work better with the default Arial font, or some cartoony version of it
								Randomly chooses messages from the MARQUEE_ block of the game text.
										Be sure to update MARQUEE_NUM_MESSAGES if you add any.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define MARQUEE_MIN_SPEED				45.0f										// PP: minimum scrolling speed for text
#define MARQUEE_MAX_SPEED				80.0f										// PP: maximum scrolling speed for text
#define MARQUEE_MIN_TEXT_INV_SCALE		80.0f										// PP: determines largest possible font size
#define MARQUEE_MAX_TEXT_INV_SCALE		100.0f										// PP: determines smallest possible font size
#define MARQUEE_NUM_MESSAGES			9											// PP: number of messages to choose from
#define MARQUEE_FIRST_MESSAGE			STR_MFX_MARQUEE1							// PP: start of marquee messages in the game text
#define MARQUEE_SCREEN_WIDTH			(MFX_SCREEN_WIDTH*this->textInvScale)		// PP: screen width
#define MARQUEE_SCREEN_HEIGHT			(MFX_SCREEN_HEIGHT*this->textInvScale)		// PP: screen height

// PP: set the text to its initial X (right hand side) and random Y position ready to start scrolling
#define MARQUEE_SET_TO_START_POSITION \
	{ \
		this->textPos[X]=MARQUEE_SCREEN_WIDTH*0.5f; \
		this->textPos[Y]=(MARQUEE_SCREEN_HEIGHT*0.5f)-((bmRand()/RANDMAX)*(MARQUEE_SCREEN_HEIGHT-this->height)); \
	}


/*	--------------------------------------------------------------------------------
	Function 	: MFXMARQUEE::init
	Purpose 	: initialise a Scrolling Marquee screen saver
	Parameters 	: x,y,z pos; x,y,z ang; scale
	Returns 	: 
	Info 		: 
*/
void MFXMARQUEE::init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale)
{
	TBMatrix		rotMat;						
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;

	// PP: store position
	this->effectPos=VEC(x,y,z);

	// PP: load up this marquee with one of the messages, randomly chosen...
	int			index;
	
	index=MARQUEE_FIRST_MESSAGE+(bmRand()%MARQUEE_NUM_MESSAGES);

	// PP: on Xbox & GameCube, maybe it's best if we don't use the old "My other computer's a PS2" gag!
	// PP: Amusingly enough though, I can't actually find any Xbox TCR prohibiting the promotion of rival brands!

	// PP: we can't even mention PS2 on the PS2 version!  Nuts.

#if(/* PP: REMOUT: see above (BPLATFORM != PS2) && */(BPLATFORM != PC))// PP: nothing wrong with advertising the PS2 version on PC
#define DONT_MENTION_PS2// PP: "DON'T - MENTION - THE WAR!!  I mentioned it once, but I *think* I got away with it..."
#endif// PP: platform

#ifdef DONT_MENTION_PS2
	do
#endif// PP: def DONT_MENTION_PS2
	{
		index=MARQUEE_FIRST_MESSAGE+(bmRand()%MARQUEE_NUM_MESSAGES);
	}
#ifdef DONT_MENTION_PS2
	while(index == STR_MFX_MARQUEE5);// PP: STR_MFX_MARQUEE5 = "My other computer is a PS2"
#endif// PP: def DONT_MENTION_PS2

	// PP: On xbox, the button combination for the Supreme Gullibility cheat had to be corrected for TCR compliance!  Wacky.
	if(index == STR_MFX_MARQUEE8)
	{
		index=STR_XBOX_MFX_MARQUEE8;
	}

	ushort*		str=mainStringTable->strings[index].string;
	bkStringCopy16(this->text, str);

	// PP: randomly choose a size for the text
	this->textInvScale=MARQUEE_MIN_TEXT_INV_SCALE+((bmRand()/RANDMAX)*(MARQUEE_MAX_TEXT_INV_SCALE-MARQUEE_MIN_TEXT_INV_SCALE));

	// PP: calculate object matrix
	bmMatScale(scaleMat, scale/this->textInvScale, scale/this->textInvScale, scale/this->textInvScale);
	bmMatZYXRotation(rotMat, ax, ay, az);// PP: was zyx
	bmMatTranslate(posMat,  x, y, z);

	bmMatMultiply(tempMat, scaleMat, rotMat);
	bmMatMultiply(this->objectMatrix, posMat,tempMat);

	// PP: randomly choose a nice colour for the text
	MFX_RANDOM_PRETTY_WINDOWS_COLOUR(this->r, this->g, this->b);

	// PP: randomly choose a speed for the text
	this->speed=MARQUEE_MIN_SPEED+((bmRand()/RANDMAX)*(MARQUEE_MAX_SPEED-MARQUEE_MIN_SPEED));

	// PP: initialise format stuff that won't change...
	this->format.width =		99999.0f;				// PP: I don't want the text to be squashed into the box
	this->format.height =		99999.0f;				// PP: I don't want the text to be squashed into the box
	this->format.lineSpace = 0;
	this->format.wrap=0;
	this->format.horzFormat = EBFONTFORMATTING_LEFT;
	this->format.vertFormat = EBFONTFORMATTING_TOP;

	// PP: calculate the physical dimensions of the string...
	this->format.x =this->textPos[X];
	this->format.y =this->textPos[Y];

	bdFontExtent(
					standardFont,
					this->text,
					bkStringLength16(this->text),
					&(this->length),
					&(this->height)
				);

	MARQUEE_SET_TO_START_POSITION;

	this->lastUpdateTime=0.0f;

//	this->ready=true;

	this->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXMARQUEE::update
	Purpose 	: update a Scrolling Marquee screensaver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXMARQUEE::update(void)
{
	float		timeSinceLastUpdate;

	if(!this->timeToUpdate(timeSinceLastUpdate)) return;

	this->textPos[X]-=this->speed*timeSinceLastUpdate;

	this->format.x =this->textPos[X];
	this->format.y =this->textPos[Y];

	if(this->textPos[X] < -(MARQUEE_SCREEN_WIDTH*0.5f)-this->length)
	{
		MARQUEE_SET_TO_START_POSITION;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXMARQUEE::draw
	Purpose 	: draw a Scrolling Marquee screensaver
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXMARQUEE::draw(void)
{
	float			height=MARQUEE_SCREEN_HEIGHT;

	// PP: check visibility then set object matrix
	if(!this->startDraw())
	{
		return;
	}

	// PP: ~~~~~~~~~~~~~~~~~~~~~ PRINT TEXT

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	bdSetZBias(MFX_STANDARD_Z_BIAS);

	TextFX_setColour(this->r, this->g, this->b);
	TextFX_setClipWidth(MARQUEE_SCREEN_WIDTH);

	bdPrintFont(standardFont,this->text,bkStringLength16(text),&(this->format),0,0,255,255,&TextFX_escapeCallback,NULL);

	// PP: ~~~~~~~~~~~~~~~~~~~~~

	TextFX_resetClipping();
	TextFX_resetColour();

	bdSetZBias(0.0f);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														DIGITAL WATCH

								Taz's digital watch when he's in his Secret Agent costume.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define WATCH_SCALE					2.3f							// PP: scale of the watch display			

// PP: that lovely lcd diffuse reflector colour - golden greenish grey// PP: could tweak this, it's a bit subdued at the moment
#define WATCH_BACKGROUND_R			128
#define WATCH_BACKGROUND_G			131
#define WATCH_BACKGROUND_B			125

// PP: colour of 7-segment digits
#define WATCH_DIGIT_R				126
#define WATCH_DIGIT_G				126
#define WATCH_DIGIT_B				126

#define WATCH_DIGIT_BLEND_MODE		BDALPHABLENDMODE_SUBTRACTIVE	// PP: blend mode used whan drawing digits
#define WATCH_MARGIN				0.05f							// PP: margin around digits (fraction of screen dimension)
#define WATCH_INTER_DIGIT_GAP		0.025f							// PP: gap between digits (fraction of screen width)
#define WATCH_TEXTURE_WIDTH			64								// PP: width in texels of texture containing characters 0123456789:
#define WATCH_TEXTURE_HEIGHT		16								// PP: height in texels of texture containing characters 0123456789:
#define WATCH_DIGIT_PIXEL_WIDTH		6								// PP: width in texels of each digit on the texture
#define WATCH_COLON_PIXEL_WIDTH		4								// PP: width in texels of the : on the texture
#define WATCH_DIGIT_PIXEL_HEIGHT	10								// PP: width in texels of each digit on the texture// PP: they're only 9 pixels high, but we need to allow an extra texel for proper filtering
#define WATCH_TEXTURE				"watchdigits.bmp"				// PP: texture containing characters 0123456789:
#define WATCH_24HOUR												// PP: defined if the watch displays time in 24 hour notation// PP: I can't stand 24-hour clocks, but I have the feeling they localise better?


/*	--------------------------------------------------------------------------------
	Function 	: MFXWATCH::init
	Purpose 	: initialise a digital watch display
	Parameters 	: x,y,z pos; x,y,z ang; scale
	Returns 	: 
	Info 		: 
*/
void MFXWATCH::init(const float x, const float y, const float z,
					const float ax, const float ay, const float az, const float scale)
{
	// PP: no use no more
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXWATCH::init
	Purpose 	: initialise a digital watch display
	Parameters 	: top-level actor instance to which the display is attached, attached object handle
	Returns 	: 
	Info 		: 
*/
void MFXWATCH::init(ACTORINSTANCE* const actorIn, const int handleIn)
{
	if(MFXWATCH::texture == NULL)
	{
		MFXWATCH::texture=bkLoadTexture(NULL,WATCH_TEXTURE,0);
	}

	this->handle=handleIn;

	this->actorInstance=ReturnAttachedObjectInstance(actorIn, this->handle);

	this->lastUpdateTime=0.0f;

	this->colour=COLOUR(WATCH_DIGIT_R, WATCH_DIGIT_G, WATCH_DIGIT_B);

	// PP: TEMP!!!!!!
	this->digits[0]=1;
	this->digits[1]=2;
	this->digits[2]=3;
	this->digits[3]=4;

	// PP: initialise world position vector
	bmVectorZero(this->pos);

	this->active=true;

	this->update();
}

float val = -19000.0f;
/*	--------------------------------------------------------------------------------
	Function 	: MFXWATCH::draw
	Purpose 	: draw a digital watch display
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#if(BPLATFORM == PS2)
// PP: #define WATCH_BACKGROUND_ZBIAS		(-5000.0f)
#define WATCH_DIGITS_ZBIAS			(-5000.0f)
#else// PP: !ps2
// PP: #define WATCH_BACKGROUND_ZBIAS		(-0.1f)
#define WATCH_DIGITS_ZBIAS			(-0.1f)
#endif// PP: !ps2

void MFXWATCH::draw(void)
{
	if(!this->active) return;

	float				penX, penY, u, v;
	TBPrimVertex		vert[4];
	static const float	digitWidth=((MFX_WATCH_WIDTH-(WATCH_MARGIN*MFX_WATCH_WIDTH*2)-(WATCH_INTER_DIGIT_GAP*MFX_WATCH_WIDTH*4))/5.0f);
	static const float	digitHeight=MFX_WATCH_HEIGHT-(WATCH_MARGIN*MFX_WATCH_HEIGHT*2);
	static const float	digitUWidth=WATCH_DIGIT_PIXEL_WIDTH/(float)WATCH_TEXTURE_WIDTH;
	static const float	digitVHeight=WATCH_DIGIT_PIXEL_HEIGHT/(float)WATCH_TEXTURE_HEIGHT;
	static const float	colonWidth=digitWidth*(WATCH_COLON_PIXEL_WIDTH/(float)WATCH_DIGIT_PIXEL_WIDTH);
	static const float	colonUWidth=WATCH_COLON_PIXEL_WIDTH/(float)WATCH_TEXTURE_WIDTH;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

// PP: 	bdSetZBias(WATCH_DIGITS_ZBIAS);

	// PP: set some render states
//	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);

	// PP: draw the background
	// PP: this->drawBackground(WATCH_BACKGROUND_R, WATCH_BACKGROUND_G, WATCH_BACKGROUND_B, MFX_WATCH_WIDTH, MFX_WATCH_HEIGHT);
	bdSetObjectMatrix(this->objectMatrix);

	// PP: draw the digits...
	bdSetZBias(WATCH_DIGITS_ZBIAS);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, WATCH_DIGIT_BLEND_MODE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, WATCH_DIGIT_BLEND_MODE, 1);

	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_WRAPV, FALSE);
	
	bdSetTexture(0, MFXWATCH::texture);

	penX= -MFX_WATCH_WIDTH*(0.5f-WATCH_MARGIN);
	penY=MFX_WATCH_HEIGHT*(0.5f-WATCH_MARGIN);

	v=-(1/(float)(WATCH_TEXTURE_HEIGHT*2.0f));		// PP: shift the V up by half a texel to sort the filtering

	int d;

#ifdef WATCH_24HOUR
	d=0;
#else // PP: ifdef WATCH_24HOUR else
	if(this->digits[0]==0)
	{
		d=1;
		penX+=digitWidth+(WATCH_INTER_DIGIT_GAP*MFX_WATCH_WIDTH);
	}
	else
	{
		d=0;
	}
#endif // PP: def WATCH_24HOUR

	for(; d<4; d++)
	{
		u=this->digits[d]*digitUWidth;
		u-=(1/(float)(WATCH_TEXTURE_WIDTH*2.0f));		// PP: shift the U left by half a texel to sort the filtering

		BDVERTEX_SETXYZW(&vert[0],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[0],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[0],u,v);
		
		u+=digitUWidth;
		penX+=digitWidth;
		BDVERTEX_SETXYZW(&vert[1],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[1],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[1],u,v);

		u-=digitUWidth;
		v+=digitVHeight;
		penX-=digitWidth;
		penY-=digitHeight;
		BDVERTEX_SETXYZW(&vert[2],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[2],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[2],u,v);

		u+=digitUWidth;
		penX+=digitWidth;
		BDVERTEX_SETXYZW(&vert[3],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[3],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[3],u,v);

		if(d==1)
		{
			penX+=digitWidth;
		}

		if(d<3)
		{
			penY+=digitHeight;
			penX+=(WATCH_INTER_DIGIT_GAP*MFX_WATCH_WIDTH);
			v-=digitVHeight;
		}

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);	// PP: draw	
	}

	if(this->colonFlash)
	{
		// PP: draw the colon, right in the middle...
		u=1.0f-colonUWidth;
		u-=(1/(float)(WATCH_TEXTURE_WIDTH*2.0f));		// PP: shift the U left by half a texel to sort the filtering
		v=0.0f;
		penX= -colonWidth*0.5f;
		penY=MFX_WATCH_HEIGHT*(0.5f-WATCH_MARGIN);
		BDVERTEX_SETXYZW(&vert[0],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[0],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[0],u,v);
		
		u+=colonUWidth;
		penX+=colonWidth;
		BDVERTEX_SETXYZW(&vert[1],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[1],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[1],u,v);

		u-=colonUWidth;
		v+=digitVHeight;
		penX-=colonWidth;
		penY-=digitHeight;
		BDVERTEX_SETXYZW(&vert[2],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[2],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[2],u,v);

		u+=colonUWidth;
		penX+=colonWidth;
		BDVERTEX_SETXYZW(&vert[3],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[3],this->colour.r,this->colour.g,this->colour.b,255);
		BDVERTEX_SETUV(&vert[3],u,v);

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);	// PP: draw	
	}

	bdSetZBias(0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXWATCH::update
	Purpose 	: update a digital watch display
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

// PP: position offset of the display relative to the watch object
#define WATCH_OFFSET_X			40.0f
#define WATCH_OFFSET_Y			7.5f
#define WATCH_OFFSET_Z			0.4f//0.6f

void MFXWATCH::update(void)
{
	if(!this->active) return;

	TBMatrix		rotMat;
	TBMatrix		scaleMat;
	TBMatrix		posMat;
	TBMatrix		tempMat;
	TBMatrix		tempMat2;

	// PP: update the watch display to show the current time...
	TBClock			clock;

	bkReadClock(&clock);

#ifdef WATCH_24HOUR
	this->digits[0]=clock.hour/10;
	this->digits[1]=clock.hour%10;
#else // PP: ifdef WATCH_24HOUR else
	this->digits[0]=((clock.hour%12)?(clock.hour%12):12)/10;
	this->digits[1]=((clock.hour%12)?(clock.hour%12):12)%10;
#endif // PP: def WATCH_24HOUR

	this->digits[2]=clock.minute/10;
	this->digits[3]=clock.minute%10;

	this->colonFlash=(bmFMod(gameStatus.gameTime, 1.0f)<0.5f);

	if(this->actorInstance == NULL)
	{
		bmMatCopy(this->objectMatrix, bIdentityMatrix);
	}
	else
	{
		bmMatScale(scaleMat, this->actorInstance->scale[X]*WATCH_SCALE, this->actorInstance->scale[Y]*WATCH_SCALE, this->actorInstance->scale[Z]*WATCH_SCALE);
		bmMatXYZRotation(rotMat, PI*0.5f, 0, 0);
		bmMatTranslate(posMat, WATCH_OFFSET_X, WATCH_OFFSET_Y, WATCH_OFFSET_Z);

		bmMatMultiply(tempMat, this->actorInstance->objectToWorld, posMat);
		bmMatMultiply(tempMat2, tempMat, rotMat);
		bmMatMultiply(this->objectMatrix, tempMat2, scaleMat);

		// PP: keep a vector copy of the watch's position
		bmVectorZero(this->pos);
		bmMatMultiplyVector(this->objectMatrix, this->pos);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MFXWATCH::shutdown
	Purpose 	: shutdown a digital watch display
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MFXWATCH::shutdown(void)
{
	this->active=false;
	this->actorInstance=NULL;
	this->handle=-1;
}
