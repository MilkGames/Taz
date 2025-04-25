// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PageItemFX.cpp
//   Purpose : Special effects for page items
// Component : Taz
//		Info : // PP: Come to me, My Pretties!!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "TextBox.h"			// PP: Text box page-item - displays a text string
#include "util.h"				// PP: general utility functions
// PP: #include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "TextFX.h"				// PP: Text effects, eg. button glyphs, coloured text
#include "Icon.h"				// PP: icon effects
#include "PageItemFX.h"			// PP: Special effects for page items
#include "display.h"			// PP: provide basic display setup and managment functions
#include "quakecam.h"
#include "stack.h"// PP: TEMP

// PP: temp test
PAGEITEMEFFECT* trapEffect=(PAGEITEMEFFECT*)NULL;//0x80f6fe70;


// PP: ftime used by page item effects; can be divided here to show what's going on in slow motion
#define PIE_FTIME			MIN(0.05f, rawFTime)// PP: 0.05f: max frame length 1/20 seconds


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													PAGEITEMEFFECT methods

					Base class for effect parasites that influence the drawing of the PAGEITEMs to which they belong.

											book > pages > rows > items > *EFFECTS*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: PIE modes supported by this type.
// PP: This must be the bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar	PAGEITEMEFFECT::supportedModes=0;


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::getItemWidth
	Purpose 	: get the width of the effect's item
	Parameters 	: 
	Returns 	: the width of the effect's item
	Info 		: 
*/
float PAGEITEMEFFECT::getItemWidth(void)
{
	return this->item->optRect.width()/this->item->alignScale;		// PP: TODO: OPTIMISE
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::getItemHeight
	Purpose 	: get the height of the effect's item
	Parameters 	: 
	Returns 	: the height of the effect's item
	Info 		: 
*/
float PAGEITEMEFFECT::getItemHeight(void)
{
	return this->item->optRect.height()/this->item->alignScale;		// PP: TODO: OPTIMISE
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::setMode
	Purpose 	: set the mode of the effect
	Parameters 	: mode for effect (see PIEMode enum PIEMODE_...)
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: // PP: this gets called (with the appropriate mode) whenever the effect is started
*/
PAGEITEMEFFECT* PAGEITEMEFFECT::setMode(const PIEMode modeIn)
{
	this->mode=modeIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this should only get called after both setMode and setItem
*/
void PAGEITEMEFFECT::start(void)
{
	// PP: effect has not finished running yet
	this->finished=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::setItem
	Purpose 	: set the item for the effect
	Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act,
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: // PP: this gets called by PAGEITEM::setEffect
*/
PAGEITEMEFFECT* PAGEITEMEFFECT::setItem(PAGEITEM* const itemIn)
{
	this->item=itemIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEMEFFECT::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEMEFFECT::init(void)
{
	// PP: no delay on starting the effect
	this->delay=0.0f;

	this->finished=false;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_FADE methods

							PAGEITEMEFFECT subclass that alpha-fades its PAGEITEM into or out of view.

				This is one of the first PAGEITEMEFFECT subclasses, and a good starting point I'm sure you'll agree.

											book > pages > rows > items > *EFFECTS*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* PP: NOTE:

If you're going to be using fades, you'll have to ensure that opacity gets properly inherited all the way down the page hierarchy.
At the moment it doesn't, but it wouldn't be a huge task to make it do so.

Since I'm planning to move Taz's text & overlays away from fades anyway (they're not cartoony),
I can't be bothered looking into it right now.

Next game maybe!

*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_FADE::supportedModes=	(1<<PIEMODE_ENTER)
											|(1<<PIEMODE_EXIT);


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FADE::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_FADE::start(void)
{
	PAGEITEMEFFECT::start();

	if(this->mode == PIEMODE_ENTER)
	{
		// PP: fading in: start from complete transparency	// PP: (temp, gie's a sodding chance, would ye?!)
		this->opacity=0.0f;

		this->item->opacity=0.0f;
	}
	else// PP: PIEMODE_EXIT or PIEMODE_ON
	{
		// PP: maximum opacity	// PP: (temp, gie's a sodding chance, would ye?!)
		//this->opacity=this->maxOpacity;	
		
		// PP: NEW TEST
		this->opacity=this->item->opacity;
	}

	// PP: update item's opacity
	// PP: REMOUT: this->item->opacity=this->opacity;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FADE::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_FADE::init(void)
{
	PAGEITEMEFFECT::init();

// PP: default duration for a page-item fade effect, in seconds
#define PIEFADE_DEFAULT_DURATION			0.3f

// PP: default max opacity for a page-item fade effect (full opacity)
#define PIEFADE_DEFAULT_MAX_OPACITY			1.0f

	// PP: set default fade time for the effect
	this->duration=PIEFADE_DEFAULT_DURATION;

	// PP: well, you can see what this line does
	this->maxOpacity=PIEFADE_DEFAULT_MAX_OPACITY;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FADE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_FADE::update(void)
{
	if(this->finished)
	{
		return true;
	}

	if(this->mode == PIEMODE_ENTER)
	{
		// PP: fading in
		this->opacity += (PIE_FTIME/this->duration);

		if(this->opacity >= this->maxOpacity)
		{
			this->opacity=this->maxOpacity;
			this->finished=true;
		}
	}
	else if(this->mode == PIEMODE_EXIT)
	{
		// PP: fading out
		this->opacity -= (PIE_FTIME/this->duration);

		if(this->opacity <= 0.0f)
		{
			this->opacity=0.0f;
			this->finished=true;
		}
	}
	else// PP: on
	{
		this->finished=true;
	}

	// PP: LOOK: this effect works by talking to its item's 'opacity' member.  Groovy.
	// PP: A frog is looking at you from under the water.
	this->item->opacity=this->opacity;

	// PP: bkPrintf("PIE_FADE: setting item's opacity to %5.2f\n", this->item->opacity);

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FADE::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::shutdown, to return its item to full opacity
*/
void PIE_FADE::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}

	// PP: return item to full opacity
	this->item->opacity=1.0f;// PP: yes, FULL opacity, not this->maxOpacity

	// PP: perform base-class shutdown
	PAGEITEMEFFECT::shutdown();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_SLIDE methods

							PAGEITEMEFFECT subclass that 'slides' its PAGEITEM onto or off of the screen.

				This is one of the first PAGEITEMEFFECT subclasses, and a good starting point I'm sure you'll agree.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@pie_slide
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_SLIDE::supportedModes=	(1<<PIEMODE_ENTER)
											|(1<<PIEMODE_EXIT);


// PP: default speed for a PIE_SLIDE effect
#define PIESLIDE_DEFAULT_SPEED			1200.0f


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_SLIDE::start(void)
{
	PAGEITEMEFFECT::start();

	this->accel=0.0f;

	if(this->mode == PIEMODE_ENTER)
	{
		// PP: going to slide into view from offstage...

		// PP: negative source offset in direction of interest
		this->srcOffset=float(-videoMode.xScreen);// PP: not too sure
		this->destOffset=0.0f;
	}
	else if(this->mode == PIEMODE_EXIT)
	{
		// PP: going to slide out of view from onstage...

		// PP: positive target offset in direction of interest
		this->srcOffset=0.0f;
		this->destOffset=float(videoMode.xScreen);// PP: not too sure
	}

	this->offset=this->srcOffset;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::setDir
	Purpose 	: set the direction of the slide
	Parameters 	: slide direction (PIESLIDEDIR_..., see PIESlideDir enum)
	Returns 	: this
	Info 		: 
*/
PIE_SLIDE* PIE_SLIDE::setDir(const PIESlideDir dirIn)
{
	// PP: set effect's axis and its direction along that axis...

	this->axis=((dirIn&1)==0);
	this->dir=((dirIn&2)?(-1):1);

	// PP: on the axis on which the PIE_SLIDE is NOT sliding, there is no offset
	this->xyOffset[this->axis^1]=0.0f;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_SLIDE::init(void)
{
	PAGEITEMEFFECT::init();

// PP: default direction for a PIE_SLIDE effect
#define PIESLIDE_DEFAULT_DIRECTION		PIESLIDEDIR_LEFT

	// PP: set up effect defaults...
	this->setDir(PIESLIDE_DEFAULT_DIRECTION);
	this->setSpeed(PIESLIDE_DEFAULT_SPEED);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_SLIDE::update(void)
{
	if(this->finished)
	{
		return true;
	}

	this->offset += (this->speed * PIE_FTIME);

	this->speed += (this->accel * PIE_FTIME);

	if(this->offset > this->destOffset)
	{
		this->offset = this->destOffset;
		this->finished=true;
	}

	this->xyOffset[this->axis]=this->offset*this->dir;

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_SLIDE::draw(void)
{
	// PP: apply translation matrix to offset the item as appropriate, then draw the item

	TBMatrix	transMat;
	TBMatrix	objMat;
	TBMatrix	finalMat;

	bmMatTranslate(transMat, this->xyOffset[X], this->xyOffset[Y], 0.0f);

	// PP: have the pageitem alter the object matrix ready to draw
	this->item->startDraw();

	// PP: now multiply-in our screen-space offset matrix
	bdGetObjectMatrix(objMat);
	bmMatMultiply(finalMat, transMat, objMat);

	bdSetObjectMatrix(finalMat);

	// PP: draw the item's background and border if it has them
	this->item->drawBackgroundAndBorder();

	// PP: draw the item
	this->item->mainDraw();

	// PP: end the draw; restore object matrix to state before the item started to draw
	this->item->endDraw();
}

/* JW: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_ZOOM methods

							PAGEITEMEFFECT subclass that scales its PAGEITEM into or out of the screen.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_ZOOM::supportedModes=	(1<<PIEMODE_ENTER)
													|(1<<PIEMODE_EXIT);// PP: ENTER & EXIT - is this right John?


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ZOOM::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_ZOOM::start(void)
{
	PAGEITEMEFFECT::start();

	this->scale = this->originalScale;

	this->speed = this->originalSpeed;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::setDir
	Purpose 	: set the direction of the slide
	Parameters 	: slide direction (PIESLIDEDIR_..., see PIESlideDir enum)
	Returns 	: this
	Info 		: 
*/
PIE_ZOOM* PIE_ZOOM::setSpeed(const float speed)
{
	this->originalSpeed = speed;

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::setAccel
	Purpose 	: set the direction of the slide
	Parameters 	: slide direction (PIESLIDEDIR_..., see PIESlideDir enum)
	Returns 	: this
	Info 		: 
*/
PIE_ZOOM* PIE_ZOOM::setAccel(const float accel)
{
	this->accel = accel;

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: PIE_ZOOM::setTarget
	Purpose 	: sets the target size of the zoom effetc
	Parameters 	: zoom target
	Returns 	: this
	Info 		: if the speed of the zoom is positive this will be the final size of the item, if it's negative the item will start at this scale
*/

PIE_ZOOM* PIE_ZOOM::setOrigin(const float origin)
{
	this->originalScale = origin;

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: PIE_ZOOM::setTarget
	Purpose 	: sets the target size of the zoom effetc
	Parameters 	: zoom target
	Returns 	: this
	Info 		: if the speed of the zoom is positive this will be the final size of the item, if it's negative the item will start at this scale
*/

PIE_ZOOM* PIE_ZOOM::setTarget(const float target)
{
	this->target = target;

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: PIE_ZOOM::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_ZOOM::init(void)
{
	// PP: base-class initialisation
	PAGEITEMEFFECT::init();
	
	// effect defaults...

// default target for a PIE_ZOOM effect
#define PIEZOOM_DEFAULT_TARGET		1.0f
// default target for a PIE_ZOOM effect
#define PIEZOOM_DEFAULT_ORIGIN		0.0f
// default acceleration for a PIE_ZOOM effect
#define PIEZOOM_DEFAULT_ACCEL		1.0f
// default speed for a PIE_ZOOM effect
#define PIEZOOM_DEFAULT_SPEED		1.0f

	this->setAccel(PIEZOOM_DEFAULT_ACCEL);

	this->setSpeed(PIEZOOM_DEFAULT_SPEED);

	this->setOrigin(PIEZOOM_DEFAULT_ORIGIN);

	this->setTarget(PIEZOOM_DEFAULT_TARGET);
}

/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_ZOOM::update(void)
{
	if(this->finished)
	{
		return true;
	}

	this->scale += (this->speed * PIE_FTIME);

	this->speed += (this->accel * PIE_FTIME);

	if(this->scale > this->target)
	{
		this->scale = this->target;
		this->finished=true;

		rumbleCam_explosion(&camera[0], gameStatus.player1->actorInstance.position, 1.5f);

		PlaySample("thud.wav", 255);
	}

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SLIDE::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_ZOOM::draw(void)
{
	// PP: apply translation matrix to offset the item as appropriate, then draw the item

	TBMatrix	transMat;
	TBMatrix	objMat;
	TBMatrix	finalMat;

	bmMatScale(transMat, this->scale, this->scale, this->scale);

	// PP: have the pageitem alter the object matrix ready to draw
	this->item->startDraw();

	// PP: now multiply-in our screen-space offset matrix
	bdGetObjectMatrix(objMat);
	bmMatMultiply(finalMat, transMat, objMat);// PP: not sure about the ordering here

	bdSetObjectMatrix(finalMat);

	// PP: draw the item's background and border if it has them
	this->item->drawBackgroundAndBorder();

	// PP: draw the item
	//this->item->drawWithoutEffect();
	this->item->mainDraw();

	// PP: TEMP TEST
	//this->item->mainDrawDebug();

	// PP: end the draw; restore object matrix to state before the item started to draw
	this->item->endDraw();
}

bool PIE_ZOOM::getState()
{
	return this->finished;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_FLASH

								ON-mode PAGEITEMEFFECT subclass that makes its PAGEITEM flash.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@pie_flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_FLASH::supportedModes=	(1<<PIEMODE_ON);


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::setInterval
	Purpose 	: set the time interval between flashes (seconds)
	Parameters 	: the time interval between flashes (seconds)
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: 
*/
PAGEITEMEFFECT* PIE_FLASH::setInterval(const float intervalIn)
{
	this->interval=intervalIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_FLASH::start(void)
{
	PAGEITEMEFFECT::start();

	this->timer=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_FLASH::init(void)
{
	PAGEITEMEFFECT::init();

#define PIEFLASH_DEFAULT_INTERVAL		0.7f

	this->setInterval(PIEFLASH_DEFAULT_INTERVAL);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_FLASH::update(void)
{
	PAGEITEMEFFECT::update();

	this->on=(bmFMod(this->timer, this->interval)<(this->interval/2.0f));

	if(this->on)
	{
		this->item->opacity=1.0f;
	}
	else
	{
		this->item->opacity=0.5f;
	}

	this->timer += PIE_FTIME;

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
/*void PIE_FLASH::draw(void)
{
	if(this->on)
	{
		this->item->drawWithoutEffect();
	}
}*/


/*	--------------------------------------------------------------------------------
	Function 	: PIE_FLASH::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::shutdown, to return its item to full opacity
*/
void PIE_FLASH::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}
		
	// PP: return item to full opacity
	this->item->opacity=1.0f;// PP: yes, FULL opacity, not this->maxOpacity

	// PP: perform base-class shutdown
	PAGEITEMEFFECT::shutdown();
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PIE_SPIN

						PAGEITEMEFFECT subclass that spins and optionally scales its item into or out of view.

				// PP: Not likely to blow anyone away, but then there is VERY little time left on this game (Taz).
												It's better than nothing, ok?!

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@pie_spin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* PP:	*** PIEModes supported by this effect: ***

		enter, exit
*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_SPIN::supportedModes=	(1<<PIEMODE_ENTER)
											|(1<<PIEMODE_EXIT);

#define PIESPIN_SPINUP_VOLUME				220
#define PIESPIN_IMPACT_VOLUME				210


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_SPIN::start(void)
{
	PAGEITEMEFFECT::start();

	if(this->sound)
	{
		if(this->mode == PIEMODE_ENTER)
		{
			PlaySample("swipe.wav", PIESPIN_SPINUP_VOLUME);
		}
	}

	this->timer=0.0f;

	if(this->mode == PIEMODE_ENTER)
	{
		this->scale=0.0f;
		this->finalAng.zero();
		this->startAng= (-totalRotation);
	}
	else// PP: assume PIEMODE_EXIT
	{
		this->scale=1.0f;
		this->startAng.zero();
		this->finalAng=totalRotation;
	}

	// PP: NOTE: there might be a draw before the first update (i think)...
	this->ang=this->startAng;

	// PP: ...in which case this'll be fooked!
	this->culled=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_SPIN::init(void)
{
	this->setTime(PIESPIN_DEFAULT_DURATION);
	this->setTurns(PIESPIN_DEFAULT_X_TURNS, PIESPIN_DEFAULT_Y_TURNS, PIESPIN_DEFAULT_Z_TURNS);
	this->setScaling(PIESPIN_DEFAULT_SCALING);
	this->setCulling(PIESPIN_DEFAULT_CULLING);
	this->setSound(PIESPIN_DEFAULT_SOUND);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_SPIN::update(void)
{
	float	progress=this->timer/this->duration;

	bmVectorLerp(this->ang, this->startAng, this->finalAng, progress);

	if(this->scaling)
	{
		if(this->mode == PIEMODE_ENTER)
		{
			this->scale=progress;
		}
		else// PP: assume PIEMODE_EXIT
		{
			this->scale=(1.0f-progress);
		}

		// PP: give the impression of movement along the Z axis rather than change in size
		this->scale = SQR(this->scale);
	}

	this->timer += PIE_FTIME;

	this->finished=(progress>1.0f);

	if(this->sound)
	{
		if(this->mode == PIEMODE_ENTER)
		{
			if(this->finished)
			{
				PlaySample("thud1.wav", PIESPIN_IMPACT_VOLUME);
			}
		}
	}

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_SPIN::draw(void)
{
	// PP: apply rotate & scale as appropriate, then draw the item

	TBMatrix	objMat;
	TBMatrix	rotMat;
	TBMatrix	scaleMat;
	TBMatrix	finalMat;
	TBMatrix	transMat;
	TBMatrix	tempMat, tempMat2;

	if(this->scaling)
	{
		// PP: rotate & scale...

		bmMatZXYRotation(rotMat, this->ang[X], this->ang[Y], this->ang[Z]);// PP: was xyz
		bmMatScale(scaleMat, this->scale, this->scale, this->scale);
		bmMatMultiply(tempMat, rotMat, scaleMat);
	}
	else
	{
		// PP: just rotate
		bmMatZXYRotation(tempMat, this->ang[X], this->ang[Y], this->ang[Z]);
	}

	if(this->culling)
	{
		// PP: determine whether or not the item is facing the viewer
		// PP: Do this by transforming a Z-vector by the rotation/scale matrix and then checking the sign of the resulting Z component

		// PP: TODO: **************************************************************
		// PP: ****** FIND OUT WHY THE HELL THIS LINE DOESN'T WORK ON PS2!!!!!!!!!!
		// PP: ********************************************************************
		//this->culled=((VEC(0,0,1)*tempMat)[Z] < 0.0f);

		VEC		tempVEC(0,0,1);

		tempVEC*=tempMat;

		this->culled=(tempVEC[Z] < 0.0f);
	}

	// PP: do nothing more if the item is culled
	if(!this->culled)
	{
		// PP: have the pageitem alter the object matrix ready to draw
		this->item->startDraw();

		// PP: now multiply into that object matrix our rotation/scale matrix
		bdGetObjectMatrix(objMat);
		bmMatMultiply(tempMat2, objMat, tempMat);

		// PP: squish everything on the Z to prevent clipping hoy
		bmMatScale(scaleMat, 1.0f, 1.0f, 0.0f);
		bmMatMultiply(finalMat, scaleMat, tempMat2);

		// PP: set the new object matrix ready to draw the item
		bdSetObjectMatrix(finalMat);

		// PP: draw the item's background and border if it has them
		this->item->drawBackgroundAndBorder();

		// PP: draw the item
		this->item->mainDraw();

		// PP: end the draw; restore object matrix to state before the item started to draw
		this->item->endDraw();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::setTurns
	Purpose 	: set the number of turns to do on each axis
	Parameters 	: number of turns on X axis, (opt/0.0f)number of turns on Y axis,
					(opt/0.0f)number of turns on Z axis
	Returns 	: this
	Info 		: // PP: positive numbers of turns are clockwise.
					NOTE: DON'T pass in RADIANS or DEGREES, pass in NUMBERS OF TURNS!!!!
*/
PIE_SPIN* PIE_SPIN::setTurns(const float xTurns, const float yTurns, const float zTurns)
{
	this->totalRotation = VEC(xTurns, yTurns, zTurns)*TWOPI;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::setTime
	Purpose 	: set the duration of the effect
	Parameters 	: duration of the effect, in seconds
	Returns 	: this
	Info 		: // PP: 'setTime' was quicker to type than 'setDuration'
*/
PIE_SPIN* PIE_SPIN::setTime(const float time)
{
	this->duration=time;
	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::setPos
	Purpose 	: set the position from which the item appears in PIEMODE_ENTER,
					or to which it disappears in PIEMODE_EXIT
	Parameters 	: screen position, eg. 0.5f, 0.5f = top right corner of screen
	Returns 	: this
	Info 		: 
*/
PIE_SPIN* PIE_SPIN::setPos(const float x, const float y)
{
	this->pos[X]=x;
	this->pos[Y]=y;
	return this;
}


// PP: TEMP TEST
bool*	g_scalingPtr=NULL;


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::setScaling
	Purpose 	: specify whether or not we want scaling in/out along with our rotations
	Parameters 	: true=scaling / false=no scaling
	Returns 	: this
	Info 		: // PP: NOTE: obviously, the only time you'd want to turn scaling off
					is when your rotations are such that they hide the entry/exit of the item.
					Eg., quarter-turns (which will turn the item 'side-on')
*/
PIE_SPIN* PIE_SPIN::setScaling(const bool on)
{
	this->scaling=on;

	g_scalingPtr= &this->scaling;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SPIN::setCulling
	Purpose 	: turn flipside culling on/off
	Parameters 	: true=culling / false=no culling
	Returns 	: this
	Info 		: // PP: when you set culling true, you won't see the reverse side of the item
					This is handy when you want 2 items to appear to be on each other's flip sides.
*/
PIE_SPIN* PIE_SPIN::setCulling(const bool on)
{
	this->culling=on;
	return this;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_ICONSHATTER

							PAGEITEMEFFECT subclass that shatters an ICON into triangular shards.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

  
														@PIE_ICONSHATTER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* PP:	*** PIEModes supported by this effect: ***

		exit
*/

// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_ICONSHATTER::supportedModes		=(1<<PIEMODE_EXIT);

const uint32	PIE_ICONSHATTER::numShards			=PIESHATTER_NUM_SHARDS_EACH_WAY*(PIESHATTER_NUM_SHARDS_EACH_WAY/2);
const uint32	PIE_ICONSHATTER::numVerts			=SQR((PIESHATTER_NUM_SHARDS_EACH_WAY/2)+1);


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/

// PP: TEMPO POR TWICO
float	shatterSpinSpeed	=17.0f;// PP: 12
float	shatterSpeed		=3.0f;
float	shatterGravity		=(-5.0f);

void PIE_ICONSHATTER::start(void)
{
	// PP: right then - chop the item up into wee triangles...

	TBVector			pos, tPos;
	RECTANGLE			tempRect;
	float				shardWidth, shardHeight;
	float				width, tWidth;
	float				height, tHeight;
	COLOUR				colour;
	PIESHATTER_SHARD*	shard;
	float				tempFloat;

	// PP: new the shards
	this->shards=new PIESHATTER_SHARD[PIE_ICONSHATTER::numShards];

	// PP: initial colour of each shard
	colour=COLOUR(128,128,128,bmFloatToInt(this->item->opacity*128));

	// PP: ***** NOTE ***** DIVISION BY ALIGN SCALE *****
	tempRect=(this->item->optRect/this->item->alignScale).toOrigin();

	width=tempRect.width();
	height=tempRect.height();

	shardWidth=width/(PIESHATTER_NUM_SHARDS_EACH_WAY/2);
	shardHeight=height/(PIESHATTER_NUM_SHARDS_EACH_WAY/2);

	width*=0.5f;
	height*=0.5f;

	tWidth=1.0f/(PIESHATTER_NUM_SHARDS_EACH_WAY/2);
	tHeight=1.0f/(PIESHATTER_NUM_SHARDS_EACH_WAY/2);

	shard= &this->shards[0];

	pos[Y]=height;
	tPos[Y]=0.0f;

	for(int i=0; i<(PIESHATTER_NUM_SHARDS_EACH_WAY/2); i++)
	{	
		pos[X]=(-width);
		tPos[X]=0.0f;

		for(int j=0; j<(PIESHATTER_NUM_SHARDS_EACH_WAY/2); j++)
		{
			BDVERTEX_SETZ(&shard->verts[0],		0.0f);
			BDVERTEX_SETW(&shard->verts[0],		1.0f);
			BDVERTEX_SETX(&shard->verts[0],		pos[X]);
			BDVERTEX_SETU(&shard->verts[0],		tPos[U]);
			BDVERTEX_SETY(&shard->verts[0],		pos[Y]);
			BDVERTEX_SETV(&shard->verts[0],		tPos[V]);

			BDVERTEX_SETZ(&shard->verts[1],		0.0f);
			BDVERTEX_SETW(&shard->verts[1],		1.0f);
			BDVERTEX_SETX(&shard->verts[1],		pos[X]+shardWidth);
			BDVERTEX_SETU(&shard->verts[1],		tPos[U]+tWidth);
			BDVERTEX_SETY(&shard->verts[1],		pos[Y]-shardHeight);
			BDVERTEX_SETV(&shard->verts[1],		tPos[V]+tHeight);

			BDVERTEX_SETZ(&shard->verts[2],		0.0f);
			BDVERTEX_SETW(&shard->verts[2],		1.0f);
			BDVERTEX_SETX(&shard->verts[2],		pos[X]);
			BDVERTEX_SETU(&shard->verts[2],		tPos[U]);
			BDVERTEX_SETY(&shard->verts[2],		pos[Y]-shardHeight);
			BDVERTEX_SETV(&shard->verts[2],		tPos[V]+tHeight);

			// PP: ~~~

			shard++;

			BDVERTEX_SETZ(&shard->verts[0],		0.0f);
			BDVERTEX_SETW(&shard->verts[0],		1.0f);
			BDVERTEX_SETX(&shard->verts[0],		pos[X]);
			BDVERTEX_SETU(&shard->verts[0],		tPos[U]);
			BDVERTEX_SETY(&shard->verts[0],		pos[Y]);
			BDVERTEX_SETV(&shard->verts[0],		tPos[V]);

			BDVERTEX_SETZ(&shard->verts[1],		0.0f);
			BDVERTEX_SETW(&shard->verts[1],		1.0f);
			BDVERTEX_SETX(&shard->verts[1],		pos[X]+shardWidth);
			BDVERTEX_SETU(&shard->verts[1],		tPos[U]+tWidth);
			BDVERTEX_SETY(&shard->verts[1],		pos[Y]);
			BDVERTEX_SETV(&shard->verts[1],		tPos[V]);

			BDVERTEX_SETZ(&shard->verts[2],		0.0f);
			BDVERTEX_SETW(&shard->verts[2],		1.0f);
			BDVERTEX_SETX(&shard->verts[2],		pos[X]+shardWidth);
			BDVERTEX_SETU(&shard->verts[2],		tPos[U]+tWidth);
			BDVERTEX_SETY(&shard->verts[2],		pos[Y]-shardHeight);
			BDVERTEX_SETV(&shard->verts[2],		tPos[V]+tHeight);

			shard++;

			tPos[X]+=tWidth;
			pos[X]+=shardWidth;
		}

		tPos[Y]+=tHeight;
		pos[Y]-=shardHeight;	
	}

	// PP: for each shard...
	for(int s=0; s<PIE_ICONSHATTER::numShards; s++)
	{
		shard= &this->shards[s];

		// PP: set initial colour
		BDVERTEX_SETCOLOUR(&shard->verts[0], colour);
		BDVERTEX_SETCOLOUR(&shard->verts[1], colour);
		BDVERTEX_SETCOLOUR(&shard->verts[2], colour);

		// PP: calculate centre pos...

		BDVERTEX_GETX(&shard->verts[0], tempFloat);	shard->pos[X] = tempFloat;
		BDVERTEX_GETX(&shard->verts[1], tempFloat);	shard->pos[X] += tempFloat;
		BDVERTEX_GETX(&shard->verts[2], tempFloat);	shard->pos[X] += tempFloat;

		BDVERTEX_GETY(&shard->verts[0], tempFloat);	shard->pos[Y] = tempFloat;
		BDVERTEX_GETY(&shard->verts[1], tempFloat);	shard->pos[Y] += tempFloat;
		BDVERTEX_GETY(&shard->verts[2], tempFloat);	shard->pos[Y] += tempFloat;

		bmVector2DCDiv(shard->pos, shard->pos, 3.0f);

		// PP: now translate the shard's verts back over the origin
		
		BDVERTEX_GETX(&shard->verts[0], tempFloat);	tempFloat -= shard->pos[X];	BDVERTEX_SETX(&shard->verts[0], tempFloat);
		BDVERTEX_GETX(&shard->verts[1], tempFloat);	tempFloat -= shard->pos[X];	BDVERTEX_SETX(&shard->verts[1], tempFloat);
		BDVERTEX_GETX(&shard->verts[2], tempFloat);	tempFloat -= shard->pos[X];	BDVERTEX_SETX(&shard->verts[2], tempFloat);

		BDVERTEX_GETY(&shard->verts[0], tempFloat);	tempFloat -= shard->pos[Y];	BDVERTEX_SETY(&shard->verts[0], tempFloat);
		BDVERTEX_GETY(&shard->verts[1], tempFloat);	tempFloat -= shard->pos[Y];	BDVERTEX_SETY(&shard->verts[1], tempFloat);
		BDVERTEX_GETY(&shard->verts[2], tempFloat);	tempFloat -= shard->pos[Y];	BDVERTEX_SETY(&shard->verts[2], tempFloat);

		// PP: set initial angle
		shard->ang.zero();

		// PP: random velocity...

#define PIESHATTER_MAX_SPEED		shatterSpeed

		shard->vel[X]=bmRandfSigned()*PIESHATTER_MAX_SPEED;
		shard->vel[Y]=bmRandf()*PIESHATTER_MAX_SPEED;/*Signed*/

		// PP: NEW TEST
		bmVector2DCDiv(shard->vel, shard->vel, this->item->alignScale/**this->item->drawScale*/);

		// PP: random spin...

#define PIESHATTER_MAX_SPIN_SPEED	shatterSpinSpeed

		shard->angVel.set(	bmRandfSigned()*PIESHATTER_MAX_SPIN_SPEED,
							bmRandfSigned()*PIESHATTER_MAX_SPIN_SPEED,
							bmRandfSigned()*PIESHATTER_MAX_SPIN_SPEED);
	}

	// PP: play sound if that what ya wanna du
	if(this->sound)
	{
		PlaySample("crashchina.wav", 255);
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_ICONSHATTER::init(void)
{
	this->setSound(false);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_ICONSHATTER::update(void)
{
	PIESHATTER_SHARD*	shard;
	TBVector2D			tempVec2D;

	// PP: assume effect is finished till we discover otherwise
	this->finished=true;

#define PIESHATTER_GRAVITY		shatterGravity

	float				gravity=PIESHATTER_GRAVITY/(this->item->alignScale);

	// PP: for each shard...
	for(int s=0; s<PIE_ICONSHATTER::numShards; s++)
	{
		shard= &this->shards[s];

		// PP: update rotation
		shard->ang += (shard->angVel*PIE_FTIME);

		// PP: update velocity (apply gravity)

		shard->vel[Y]+= (gravity*PIE_FTIME);

		// PP: update position...
		
		bmVector2DCMul(tempVec2D, shard->vel, PIE_FTIME);

		bmVector2DAdd(shard->pos, shard->pos, tempVec2D);
		
		// PP: check if shard is still on screen
		if(shard->pos[Y] > (-0.3f))
		{
			// PP: yes it might be: effect ain't finished then
			this->finished=false;
		}
	}

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_ICONSHATTER::draw(void)
{
	PIESHATTER_SHARD*	shard;

	TBMatrix			objMat;
	TBMatrix			rotMat;
	TBMatrix			transMat;
	TBMatrix			scaleMat;
	TBMatrix			tempMat, tempMat2;
	TBMatrix			finalMat;
	float				scale;

	SETTEXTURE(((ICON*)(this->item))->tex);

	// PP: have the pageitem alter the object matrix ready to draw
	this->item->startDraw();

	bdGetObjectMatrix(objMat);
	
	scale=1/this->item->drawScale;

	for(int s=0; s<PIE_ICONSHATTER::numShards; s++)
	{
		shard= &this->shards[s];

		// PP: rot
		bmMatZXYRotation(rotMat, shard->ang[X], shard->ang[Y], shard->ang[Z]);

		// PP: trans
		bmMatTranslate(transMat, shard->pos[X], shard->pos[Y], shard->pos[Z]);
		//bmMatTranslate(transMat, 0.0f, 0.0f, 0.0f);

		// PP: combine rot & trans
		bmMatMultiply(tempMat, transMat, rotMat);

		// PP: combine rot-trans & obj
		bmMatMultiply(tempMat2, objMat, tempMat);

		// PP: squish everything on the Z to prevent clipping hoy
		bmMatScale(scaleMat, 1.0f, 1.0f, 0.0f);						// PP: dunno why this shouldn't be 1,1,0
		bmMatMultiply(finalMat, scaleMat, tempMat2);

		bdSetObjectMatrix(finalMat);

	//	RECTANGLE tempRect=RECTANGLE(-ICON_WIDTH*0.5f, ICON_WIDTH*0.5f, -ICON_WIDTH*0.5f, ICON_WIDTH*0.5f);

	//	tempRect.draw();

	//	item->mainDraw();

		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, &shard->verts, 3, BVERTTYPE_SINGLE);
	}

	// PP: end the draw; restore object matrix to state before the item started to draw
	this->item->endDraw();
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::setSound
	Purpose 	: set sound on/off for the effect
	Parameters 	: true=sound / false=no sound
	Returns 	: this
	Info 		: I'm not a goth
*/
void PIE_ICONSHATTER::setSound(const bool on)
{
	this->sound=on;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_ICONSHATTER::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::shutdown
*/
void PIE_ICONSHATTER::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}
		
	delete this->shards;
// PP: 	delete this->verts;
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_SQUASHY

						PAGEITEMEFFECT subclass that makes its PAGEITEM behave like a cute squashy thing.
										You'll want to take it home and give it a cuddle.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@PIE_SQUASHY
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* *** PIEModes supported by this effect: ***

	enter, exit
*/


// PP: default speed for a PIE_SQUASHY effect
#define PIESQUASHY_DEFAULT_SPEED			1500.0f


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SQUASHY::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_SQUASHY::start(void)
{
	// call base-class start() first
	PIE_SLIDE::start();

	// PP: assume no unbalanced forces at start...

	this->compression=0.0f;
	this->springOffset=this->offset;

	this->springSpeed=0.0f;

	this->squashyOffset=0.0f;

	// PP: SET SCALE TO ZERO (BODGE): prevents a wee flicker I couldn't be arsed looking into
	this->scale[X]=0.0f;
	this->scale[Y]=0.0f;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SQUASHY::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_SQUASHY::init(void)
{
	PIE_SLIDE::init();

	PIE_SQUASHY::setSpeed(PIESQUASHY_DEFAULT_SPEED);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SQUASHY::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/

#define PIESQUASHY_SPRING			150.0f		// PP: speed of springing
#define PIESQUASHY_DAMP				0.80f		// PP: damping factor for the spring.  1=no damping, 0=full damping
#define PIESQUASHY_BULGE			1.5f		// PP: degree of bulge on axis in which the item isn't being squashed
#define PIESQUASHY_SQUASH			0.007f		// PP: how easily the spring can be compressed; how weak it is
#define PIESQUASHY_MIN				0.5f		// PP: minimum compression below which forces are assumed to be balanced

bool PIE_SQUASHY::update(void)
{
	PIE_SLIDE::update();

	// PP: override 'finished' state set by PIE_SLIDE update
	this->finished=false;
	
	this->springAccel=(this->offset-springOffset)*PIESQUASHY_SPRING;

	this->springSpeed+=springAccel*PIE_FTIME;

	this->springSpeed*=PIESQUASHY_DAMP;

	this->springOffset += (springSpeed*PIE_FTIME);

	this->compression= -(this->offset-springOffset)*PIESQUASHY_SQUASH;

	this->squashyOffset= this->dir*this->compression*this->getItemWidth()*0.5f;

	// PP: calculate x & y scale based on current compression...

	float	temp;
	
	temp=1.0f-this->compression;
	this->scale[this->axis]=MAX(0.0f, temp);

	temp=1.0f+(this->compression*PIESQUASHY_BULGE);
	this->scale[this->axis^1]=MAX(0.0f, temp);

	if(bmFAbs(this->compression) < PIESQUASHY_MIN)// PP: compression almost zero
	{
		if(bmFAbs(this->springSpeed) < PIESQUASHY_MIN)// PP: compression speed almost zero
		{
			this->finished=true;
		}
	}

	// NH: This should finish an SQUASHY effect if it's on the way out and wants to start bouncing (previously bounced onto the screen again in some occasions)
	// NH: Looks like it works, may want to tweak it if it causes any probs
	if(this->compression > 0.0f && this->mode==PIEMODE_EXIT)
		this->finished=true;

	// return whether or not the PIE_SQUASHY effect has finished
	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_SQUASHY::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_SQUASHY::draw(void)
{
	// PP: apply translation & scale as appropriate, then draw the item

	TBMatrix	objMat;
	TBMatrix	scaleMat;
	TBMatrix	finalMat;
	TBMatrix	transMat;
	TBMatrix	transMat2;
	TBMatrix	tempMat, tempMat2;

	// PP: scale x&y according to compression
	bmMatScale(scaleMat, this->scale[X], this->scale[Y], 1.0f);

	bmMatTranslate(transMat, this->xyOffset[X], this->xyOffset[Y], 0.0f);

	// PP: have the pageitem alter the object matrix ready to draw
	this->item->startDraw();

	// PP: now multiply into that object matrix our scale matrix
	bdGetObjectMatrix(objMat);

	// PP: translate to compensate for the squashing of the item.
	// PP: This keeps one side of the the item 'anchored' so only the other end springs around.
	bmMatIdentity(transMat2);
	transMat2[3][this->axis]=this->squashyOffset;// PP: dunno just how safe this is...

	bmMatMultiply(tempMat, objMat, transMat2);// PP: squashy offset is scaled by object matrix because its in pageitem scale

	bmMatMultiply(tempMat2, transMat, tempMat);// PP: normal pageitem transformations first, then scale-independent translate

	bmMatMultiply(finalMat, tempMat2, scaleMat);// PP: scale first, then translate

	// PP: set the new object matrix ready to draw the item
	bdSetObjectMatrix(finalMat);

	// PP: draw the item's background and border if it has them
	this->item->drawBackgroundAndBorder();

	// PP: draw the item
	this->item->mainDraw();

	// PP: end the draw; restore object matrix to state before the item started to draw
	this->item->endDraw();
}


#ifdef THIS_IS_JUST_A_TEMPLATE_TO_SAVE_A_BIT_OF_TYPING_USE_CTRL_H_TO_REPLACE_TEMPLATE_WITH_THE_NAME_OF_THE_TYPE

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEMPLATE

								PAGEITEMEFFECT subclass that 

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@PIE_TEMPLATE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* *** PIEModes supported by this effect: ***

	enter, exit
*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_TEMPLATE::supportedModes=	(1<<PIEMODE_ENTER)
														|(1<<PIEMODE_EXIT);


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEMPLATE::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_TEMPLATE::start(void)
{
	// call base-class start() first
	PAGEITEMEFFECT::start();


}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEMPLATE::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_TEMPLATE::init(void)
{
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEMPLATE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_TEMPLATE::update(void)
{


	// return whether or not the PIE_TEMPLATE effect has finished
	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEMPLATE::draw
	Purpose 	: draw the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::draw
*/
void PIE_TEMPLATE::draw(void)
{
}

#endif// PP: template


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															TEXT_PIE

					Base-class for PAGEITEMEFFECT subclasses that manipulate the characters in a TEXTBOX.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@text_pie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: information held by the effect about each character in the textbox
struct TEXTPIE_CHARINFO
{
	TBVector2D		posOffset;
	float			angOffset;

	float			angOffsetSin;
	float			angOffsetCos;

}BALIGN16;


#define TEXTPIE_PRECALC_SINE_AND_COSINE(_info) \
		(_info)->angOffsetCos=bmCosApprox((_info)->angOffset); \
		(_info)->angOffsetSin=bmSinApprox((_info)->angOffset);


/*	--------------------------------------------------------------------------------
	Function 	: TEXT_PIE::setItem
	Purpose 	: set the item for the effect
	Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: this overrides PAGEITEMEFFECT::setItem
*/
PAGEITEMEFFECT* TEXT_PIE::setItem(PAGEITEM* const itemIn)
{
	TEXTPIE_CHARINFO*		info;

	// PP: base class setItem (actually stores the item ptr)
	PAGEITEMEFFECT::setItem(itemIn);

	// PP: malloc character info
	this->baseCharInfo=new TEXTPIE_CHARINFO[TEXTFX_STRING_LENGTH];

	for(int i=this->getNumChars()-1; i>=0; i--)
	{
		info= &(this->baseCharInfo[i]);

		// PP: zero offsets...

		info->posOffset[X]=0.0f;
		info->posOffset[Y]=0.0f;

		// PP: zero angle...

		info->angOffset=0.0f;
		info->angOffsetSin=0.0f;
		info->angOffsetCos=1.0f;
	}

#ifdef TEXTPIE_FULL_PRINT
	// PP: set DONTPRINTLITE flag cos yadda
	((TEXTBOX*)this->item)->flags |= TBFLAG_DONTPRINTLITE;
#endif// PP: def TEXTPIE_FULL_PRINT

	return (PAGEITEMEFFECT*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXT_PIE::processCharacter
	Purpose 	: process one of the textbox's characters
	Parameters 	: ptr to the first of the four verts forming the character,
					index of the character
	Returns 	: 
	Info 		: called for each pass of each character by TextFX_escapeCallback
					So, y'know, keep it lean.
					This overrides PAGEITEMEFFECT::processCharacter.
*/
void TEXT_PIE::processCharacter(TBPrimVertex* const verts, const int polyIndex)
{
	TBVector2D				tempVec, tempVec2;
	TBVector2D				firstVec;
	TBVector2D				centrePos;
	TEXTPIE_CHARINFO*		info;

	float					inverseScale;
	TBVector2D				posOffset;

	BDVERTEX_GETX(&verts[0], centrePos[X]);
	BDVERTEX_GETY(&verts[0], centrePos[Y]);	

#define ADD_TO_CENTREPOS(_i) \
	BDVERTEX_GETX(&verts[_i], tempVec[X]); \
	BDVERTEX_GETY(&verts[_i], tempVec[Y]); \
	bmVector2DAdd(centrePos, centrePos, tempVec);

/*

	// PP: BASE CENTRE

	ADD_TO_CENTREPOS(2);

	bmVector2DCDiv(centrePos, centrePos, 2.0f);

/*/

	// PP: TRUE CENTRE

	ADD_TO_CENTREPOS(1);
	ADD_TO_CENTREPOS(2);
	ADD_TO_CENTREPOS(3);

	bmVector2DCMul(centrePos, centrePos, 0.25f);

//*/

	info= &(this->baseCharInfo[polyIndex]);
	
	BDVERTEX_GETX(&verts[0], firstVec[X]);
	BDVERTEX_GETY(&verts[0], firstVec[Y]);

	for(int i=0; i<4; i++)
	{		
		BDVERTEX_GETX(&verts[i], tempVec[X]);
		BDVERTEX_GETY(&verts[i], tempVec[Y]);
		
		bmVector2DSub(tempVec2, tempVec, centrePos);
		
		tempVec[X]=(info->angOffsetCos*tempVec2[X])+(info->angOffsetSin*tempVec2[Y]);
		tempVec[Y]=(info->angOffsetCos*tempVec2[Y])-(info->angOffsetSin*tempVec2[X]);
		
		bmVector2DAdd(tempVec2, tempVec, centrePos);

		bmVector2DAdd(tempVec, tempVec2, info->posOffset);
		
		BDVERTEX_SETX(&verts[i], tempVec[X]);
		BDVERTEX_SETY(&verts[i], tempVec[Y]);
	}

// PP: TODO: on PS2, gotter re-form the degenerate quads between the characters (think this might only be needed with bdPrintFontLite?)

#if(BPLATFORM == PS2)

/*	if(((TEXTBOX*)this->item)->quickPrint())
	{
		if(polyIndex > 0)
		{
			BDVERTEX_SETX(&verts[-1], firstVec[X]);
			BDVERTEX_SETY(&verts[-1], firstVec[Y]);

			BDVERTEX_SETX(&verts[-2], firstVec[X]);
			BDVERTEX_SETY(&verts[-2], firstVec[Y]);

			if(polyIndex < (this->getNumChars()-1))
			{
				BDVERTEX_SETX(&verts[4], tempVec[X]);
				BDVERTEX_SETY(&verts[4], tempVec[Y]);

				BDVERTEX_SETX(&verts[5], tempVec[X]);
				BDVERTEX_SETY(&verts[5], tempVec[Y]);
			}
		}

	}*/

#endif// PP: (BPLATFORM == PS2)
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXT_PIE::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TEXT_PIE::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}

	SAFE_DELETE(this->baseCharInfo);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEXTLIFE

			TEXT_PIE (PAGEITEMEFFECT) subclass for TEXTBOXes that adds some movement, or 'life', to the characters.
							I haven't quite decided on the exact nature of the movement yet,
								and even if I had it'd be likely to change at some point,
										so I've used this generic name to save hassle.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar			PIE_TEXTLIFE::supportedModes=	(1<<PIEMODE_ON);


// PP: information held by the effect about each character in the textbox
struct TEXTLIFE_CHARINFO
{
	float			phase;
	float			speed;

}BALIGN16;


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTLIFE::setItem
	Purpose 	: set the item for the effect
	Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: this overrides TEXT_PIE::setItem
*/
PAGEITEMEFFECT* PIE_TEXTLIFE::setItem(PAGEITEM* const itemIn)
{
	TEXTLIFE_CHARINFO*		info;

	TEXT_PIE::setItem(itemIn);

	// PP: malloc character info
	this->charInfo=new TEXTLIFE_CHARINFO[TEXTFX_STRING_LENGTH];

#define TEXTLIFE_MIN_SPEED			1.6f
#define TEXTLIFE_MAX_SPEED			2.0f

	for(int i=/*this->getNumChars()*/TEXTFX_STRING_LENGTH-1; i>=0; i--)// PP: TEXTFX_STRING_LENGTH is a good idea here cos string length may change after this init
	{
		info= &(this->charInfo[i]);

		// PP: randomise phase offset
		info->phase=bmRandf();

		// PP: randomise speed
		info->speed=TEXTLIFE_MIN_SPEED+(bmRandf()*(TEXTLIFE_MAX_SPEED-TEXTLIFE_MIN_SPEED));
	}

	return (PAGEITEMEFFECT*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTLIFE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_TEXTLIFE::update(void)
{
	// PP: TEMP TEST

#define TEXTLIFE_ANGLE_RANGE		0.17f
#define TEXTLIFE_RADIUS				25.0f

	TEXTPIE_CHARINFO*		baseInfo;
	TEXTLIFE_CHARINFO*		info;
	float					tempFloat;

	for(int i=this->getNumChars()-1; i>=0; i--)
	{
		baseInfo= &(this->baseCharInfo[i]);
		info= &(this->charInfo[i]);

		// PP: update character's phase...

		info->phase += PIE_FTIME*info->speed;

		if(info->phase > 1.0f)
		{
			info->phase -= 1.0f;
		}

		if(info->phase > 0.5f)// PP: right
		{
			if(info->phase > 0.75f)// PP: down from right
			{
				tempFloat=1.0f-((info->phase-0.75f)/0.25f);
			}
			else// PP: up to right
			{
				tempFloat=((info->phase-0.5f)/0.25f);
			}

			baseInfo->angOffset=tempFloat*TEXTLIFE_ANGLE_RANGE;

			TEXTPIE_PRECALC_SINE_AND_COSINE(baseInfo);

			baseInfo->posOffset[X]=(-TEXTLIFE_RADIUS*baseInfo->angOffsetCos)+TEXTLIFE_RADIUS;
			baseInfo->posOffset[Y]=(TEXTLIFE_RADIUS*baseInfo->angOffsetSin);
		}
		else// PP: left
		{
			if(info->phase > 0.25f)// PP: down from left
			{
				tempFloat=1.0f-((info->phase-0.25f)/0.25f);
			}
			else// PP: up to left
			{
				tempFloat=(info->phase/0.25f);
			}

			baseInfo->angOffset=tempFloat*(-TEXTLIFE_ANGLE_RANGE);

			TEXTPIE_PRECALC_SINE_AND_COSINE(baseInfo);

			baseInfo->posOffset[X]=(TEXTLIFE_RADIUS*baseInfo->angOffsetCos)-TEXTLIFE_RADIUS;
			baseInfo->posOffset[Y]=(-TEXTLIFE_RADIUS*baseInfo->angOffsetSin);
		}
	}

#ifdef TEXTPIE_FULL_PRINT

	if(this->finished)
	{
		// PP: clear item's DONTPRINTLITE flag again now the effect has finished
		((TEXTBOX*)this->item)->flags &= ~TBFLAG_DONTPRINTLITE;
	}

#endif// PP: def TEXTPIE_FULL_PRINT

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTLIFE::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides TEXT_PIE::shutdown
*/
void PIE_TEXTLIFE::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}
	
	SAFE_DELETE(this->charInfo);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEXTEXPLODE

			EXIT-mode TEXT_PIE (PAGEITEMEFFECT) subclass that makes a textbox explode, sending all the characters flying.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar				PIE_TEXTEXPLODE::supportedModes=	(1<<PIEMODE_EXIT);


// PP: information held by the effect about each character in the textbox
struct TEXTEXPLODE_CHARINFO
{
	TBVector2D		vel;
	float			rotSpeed;

}BALIGN16;


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTEXPLODE::PIE_TEXTEXPLODE 
	Purpose 	: copy constructor
	Parameters 	: ptr to PIE_TEXTEXPLODE to copy
	Returns 	: 
	Info 		: this overloads the implicit copy constructor, to reproduce malloced members
*/
/* PP: REMOUT: not yet	void PIE_TEXTEXPLODE::PIE_TEXTEXPLODE(const PIE_TEXTEXPLODE* const base)
{

}*/


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTEXPLODE::setItem
	Purpose 	: set the item for the effect
	Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act
	Returns 	: (PAGEITEMEFFECT*)this
	Info 		: this overrides TEXT_PIE::setItem
*/
PAGEITEMEFFECT* PIE_TEXTEXPLODE::setItem(PAGEITEM* const itemIn)
{
	TEXT_PIE::setItem(itemIn);

	// PP: malloc character info
	this->charInfo=new TEXTEXPLODE_CHARINFO[TEXTFX_STRING_LENGTH];

	TEXTEXPLODE_CHARINFO*	info;

	for(int i=TEXTFX_STRING_LENGTH-1; i>=0; i--)// PP: TEXTFX_STRING_LENGTH is a good idea here cos string length may change after this init
	{
		info= &this->charInfo[i];

		// PP: give the character a random rotation speed/direction

#define TEXTEXPLODE_MAX_ROTSPEED	48.0f// PP: radians/sec

		info->rotSpeed=bmRandfSigned()*TEXTEXPLODE_MAX_ROTSPEED;

		// PP: give the character a random velocity

#define TEXTEXPLODE_MAX_SPEED		1000.0f

		info->vel[X]=bmRandfSigned()*TEXTEXPLODE_MAX_SPEED;
		info->vel[Y]=bmRandf/*Signed*/()*TEXTEXPLODE_MAX_SPEED;// PP: REMOUT: UPWARDS initial velocity

		// PP: TODO?: scale the velocity by the inverse of the draw scale and final font size of the text
	}

	return (PAGEITEMEFFECT*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTEXPLODE::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_TEXTEXPLODE::update(void)
{
	// PP: update the flying characters' velocities, angles and positions...

	TEXTPIE_CHARINFO*		baseInfo;
	TEXTEXPLODE_CHARINFO*	info;

	if(this->finished)
	{
		return true;
	}

	// PP: assume effect is finished till we discover otherwise
	this->finished=true;

	for(int i=this->getNumChars()-1; i>=0; i--)
	{
		info=		&this->charInfo[i];
		baseInfo=	&this->baseCharInfo[i];

		// PP: apply gravity

#define TEXTEXPLODE_GRAVITY		(-1800.0f)			// PP: ?

		info->vel[Y]+= (TEXTEXPLODE_GRAVITY*PIE_FTIME);

		baseInfo->posOffset[X] += (info->vel[X]*PIE_FTIME);
		baseInfo->posOffset[Y] += (info->vel[Y]*PIE_FTIME);

		// PP: check if character is still on screen
		if(baseInfo->posOffset[Y] > (-videoMode.yScreen))
		{
			// PP: yes it might be: effect ain't finished then
			this->finished=false;
		}

		baseInfo->angOffset+=(info->rotSpeed*PIE_FTIME);

		baseInfo->angOffset=Aabs(baseInfo->angOffset);

		TEXTPIE_PRECALC_SINE_AND_COSINE(baseInfo);
	}

#ifdef TEXTPIE_FULL_PRINT

	if(this->finished)
	{
		// PP: clear item's DONTPRINTLITE flag again now the effect has finished
		((TEXTBOX*)this->item)->flags &= ~TBFLAG_DONTPRINTLITE;
	}

#endif// PP: def TEXTPIE_FULL_PRINT

	return this->finished;
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTEXPLODE::shutdown
	Purpose 	: shut-down the effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PIE_TEXTEXPLODE::shutdown(void)
{
	if(BabelHasShutdown)
	{
		return;
	}
	
	SAFE_DELETE(this->charInfo);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEXTFALL

			TEXT_PIE (PAGEITEMEFFECT) subclass that makes each letter either drop into place or drop off the screen

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#ifdef NOTYET

/* *** PIEModes supported by this effect: ***

	enter, exit
*/


// PP: PIE modes supported by this type.
// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
const uchar		PIE_TEXTFALL::supportedModes=	(1<<PIEMODE_ENTER)
												|(1<<PIEMODE_EXIT);


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTFALL::start
	Purpose 	: start the effect
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEMEFFECT::start
					// PP: this should only get called after both setMode and setItem
*/
void PIE_TEXTFALL::start(void)
{
	// call base-class start() first
	PAGEITEMEFFECT::start();


}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTFALL::init
	Purpose 	: initialise the effect
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEMEFFECT::init
*/
void PIE_TEXTFALL::init(void)
{
}


/*	--------------------------------------------------------------------------------
	Function 	: PIE_TEXTFALL::update
	Purpose 	: update the effect
	Parameters 	: 
	Returns 	: true if finished, else false
	Info 		: this overrides PAGEITEMEFFECT::update
*/
bool PIE_TEXTFALL::update(void)
{
	// PP: update the flying characters' velocities, angles and positions...

	TEXTPIE_CHARINFO*		baseInfo;
	TEXTEXPLODE_CHARINFO*	info;

	if(this->finished)
	{
		return true;
	}

	// PP: assume effect is finished till we discover otherwise
	this->finished=true;

	for(int i=this->getNumChars()-1; i>=0; i--)
	{
		info=		&this->charInfo[i];
		baseInfo=	&this->baseCharInfo[i];

		// PP: apply gravity

#define TEXTEXPLODE_GRAVITY		(-1800.0f)			// PP: ?

		info->vel[Y]+= (TEXTEXPLODE_GRAVITY*PIE_FTIME);

		baseInfo->posOffset[X] += (info->vel[X]*PIE_FTIME);
		baseInfo->posOffset[Y] += (info->vel[Y]*PIE_FTIME);

		// PP: check if character is still on screen
		if(baseInfo->posOffset[Y] > (-videoMode.yScreen))
		{
			// PP: yes it might be: effect ain't finished then
			this->finished=false;
		}

		baseInfo->angOffset+=(info->rotSpeed*PIE_FTIME);

		baseInfo->angOffset=Aabs(baseInfo->angOffset);

		TEXTPIE_PRECALC_SINE_AND_COSINE(baseInfo);
	}

	// return whether or not the PIE_TEXTFALL effect has finished
	return this->finished;
}

#endif// PP: def NOTYET


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															ORT_PIE

	Base-class for PAGEITEMEFFECT subclasses that need to draw their PAGEITEMs to an offscreen render target as part of their effect.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@ort_pie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: initialise static ORT_PIE members...
ushort				ORT_PIE::numORTUsers=0;
TBRenderTarget*		ORT_PIE::renderTarget=NULL;
RENDERTARGETINFO	ORT_PIE::renderTargetInfo;
bool				ORT_PIE::renderTargetLocked=false;


/*	--------------------------------------------------------------------------------
	Function 	: ORT_PIE::useORT
	Purpose 	: register this effect as a user of the offscreen render target
	Parameters 	: 
	Returns 	: false if there was some problem (ie ORT creation failed), else true
	Info 		: This'll get called when the effect starts
					// PP: DANGER: ORT CREATION AND DELETION IS *REAL* *SLOW* !!!!
*/

#define TIME_RT_CREATION		// PP: like it says on the tin

#if (BPLATFORM==PS2)
#define RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ// PP: BCREATERENDERTARGET_LOCKABLE creates targets that badly leak/fragment memory - should maybe talk to tech team at some point
#define RENDER_TARGET_COLOUR_DEPTH		16

#define RENDER_TARGET_WIDTH				256
#define RENDER_TARGET_HEIGHT			128

#elif (BPLATFORM==PC)
#define RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
#define RENDER_TARGET_COLOUR_DEPTH		videoMode.bppScreen

#define RENDER_TARGET_WIDTH				256// PP: 256
#define RENDER_TARGET_HEIGHT			128// PP: 128

#else
#define RENDER_TARGET_CREATION_FLAGS	BCREATERENDERTARGET_PRIVATEZ
#define RENDER_TARGET_COLOUR_DEPTH		32

// PP: teehee eat my res
#define RENDER_TARGET_WIDTH				640// PP: 256
#define RENDER_TARGET_HEIGHT			480// PP: 128

#endif// PP: (BPLATFORM==PS2)

bool ORT_PIE::useORT(void)
{
	// PP: create the ORT if it doesn't exist...

	if(ORT_PIE::renderTarget == NULL)
	{
#ifdef TIME_RT_CREATION
		STARTTIMER;
#endif// PP: TIME_RT_CREATION

		// create a shared target with 16bit colour & 16bit Z
		if (ORT_PIE::renderTarget = bdCreateRenderTarget(RENDER_TARGET_WIDTH, RENDER_TARGET_HEIGHT, RENDER_TARGET_COLOUR_DEPTH, RENDER_TARGET_COLOUR_DEPTH, RENDER_TARGET_CREATION_FLAGS))
		{
			// retrieve the dimensions and depths that were actually used
			bdGetRenderTargetInfo(ORT_PIE::renderTarget,
									&(ORT_PIE::renderTargetInfo.width),
									&(ORT_PIE::renderTargetInfo.height),
									&(ORT_PIE::renderTargetInfo.rgbDepth),
									&(ORT_PIE::renderTargetInfo.zDepth));

			bkPrintf("ORT_PIE::useORT: created render target successfully (%d*%d, %d bit RGB, %d bit Z)\n",
				ORT_PIE::renderTargetInfo.width, ORT_PIE::renderTargetInfo.height, ORT_PIE::renderTargetInfo.rgbDepth, ORT_PIE::renderTargetInfo.zDepth);
		}
		else
		{
			bkPrintf("ORT_PIE::useORT: failed to create render target\n");

			// PP: failure
			return false;
		}

#ifdef TIME_RT_CREATION
		ENDTIMER("ORT_PIE render target creation");
#endif// PP: TIME_RT_CREATION
	}

	// PP: one more effect using the ORT
	ORT_PIE::numORTUsers++;

	// PP: success
	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: ORT_PIE::releaseORT
	Purpose 	: unregister this effect as a user of the offscreen render target
	Parameters 	: 
	Returns 	: 
	Info 		: This'll get called when the effect finishes
					// PP: DANGER: ORT CREATION AND DELETION IS *REAL* *SLOW* !!!!
*/
void ORT_PIE::releaseORT(void)
{
	// PP: TEMP!!!
	return;

	// PP: one less effect using the ORT
	ORT_PIE::numORTUsers--;

	if(ORT_PIE::numORTUsers == 0)
	{
		// PP: free the ORT here

		// PP: NOTE: It is essential that the specified render target not be current on any texture stage when deleted, if this occurs a crash is likely.

		bdSetTexture(0, NULL);

		bdDeleteRenderTarget(ORT_PIE::renderTarget);

		ORT_PIE::renderTarget=NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ORT_PIE::drawItemToORT
	Purpose 	: draw the effect's item to the offscreen render target
	Parameters 	: 
	Returns 	: false if there was a problem (eg. ORT is locked for drawing by another ORT_PIE), else true
	Info 		: // PP: TODO?: params like, um, blockyness, rotation etc?
*/
bool ORT_PIE::drawItemToORT(void)
{
	if(ORT_PIE::renderTargetLocked)
	{
		// PP: oops - render target is already being drawn to (presumably by the PAGEITEM containing this's PAGEITEM)

		// PP: But that's not necessarily a bad thing -
		// PP: maybe there's, like, a page whose effect requires it to draw itself AND ALL ITS CONTENTS to the ORT in a wunner.
		// PP: In that case, let's just abort the effect draw of this sub-item, and instead draw it in the normal way.
		// PP: It's the best thing we can do without allocating multiple render targets, which is not practical at all.

		// PP: simply draw the pageitem in the normal way
		this->item->drawWithoutEffect();

		return false;
	}

	// PP: lock the ORT for drawing, so's no sub-items of this item try to use it for their own wee effects.
	// PP: The little tykes - get out of it!!
	ORT_PIE::renderTargetLocked=true;

	pushRenderTarget(ORT_PIE::renderTarget, 255,255,255,0, 1.0f, BSETRENDERTARGET_CLEARRGB);

	// PP: apply a scale matrix so's the item uses every bit of space available to it on the render target

	TBMatrix		objMat;
	TBMatrix		scaleMat;
	TBVector2D		scale;

	bdGetObjectMatrix(objMat);

	scale[X]=videoMode.xScreen/this->item->optRect.width();
	scale[Y]=videoMode.yScreen/this->item->optRect.height();

	bmMatScale(scaleMat, scale[X], scale[Y], 0.0f);

	bdSetObjectMatrix(scaleMat);

	// PP: A'ight check dis.  Ya need da alpha framewrite massif.
	SETFRAMEWRITE(rgbFramewriteChannels|BDFRAMEWRITE_ALPHA);

	// PP: draw the item's background and border if it has them
	this->item->drawBackgroundAndBorder();

	this->item->mainDraw();

	// PP: alpha framewrite off again - it's not a pretty thing to leave switched on
	SETFRAMEWRITE(rgbFramewriteChannels);

	bdSetObjectMatrix(objMat);

	bdSetRenderTargetAsTexture(ORT_PIE::renderTarget, 0);

	popRenderTarget();

	// PP: a'ight, render target is free again now
	ORT_PIE::renderTargetLocked=false;

	// PP: success
	return true;
}





/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIECHOOSER methods

											Base class for pageitem effect choosers.
							Effect choosers help choose the effects to use for the items on a page.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: macro to start a copy of a preset effect on a page item
#define PIECHOOSE(_item, _effect)		(_item)->startEffect((_effect).copy()->setItem(_item)->setMode(mode))


// PP: PIECHOOSER base-class instance
PIECHOOSER					basePIEChooser;


/*	--------------------------------------------------------------------------------
	Function 	: PIECHOOSER::itemEffect
	Purpose 	: choose and initialise an appropriate effect for a general PAGEITEM
	Parameters 	: ptr to item for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this base-class implementation chooses no effects
*/
void PIECHOOSER::itemEffect(struct TAG_PAGEITEM* const item, const PIEMode mode) const
{
	item->startEffect(NULL);

	// PP: TODO: in subclasses, look at the members of the item such as 'selected' and 'highlighted' when deciding on an effect
	// PP: maybe even consider screen position?  size?
}


/*	--------------------------------------------------------------------------------
	Function 	: PIECHOOSER::textboxEffect
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
	Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this base-class implementation chooses no effects
*/
void PIECHOOSER::textboxEffect(TEXTBOX* const textbox, const PIEMode mode) const
{
	// PP: IMPORTANT: some PIECHOOSER subclasses might not override textboxEffect, in which case we should call their itemEffect override
	this->itemEffect((PAGEITEM*)textbox, mode);

	// PP: TODO: in subclasses, look at the members of the textbox such as 'style', 'selected' and 'highlighted' when deciding on an effect
	// PP: maybe even consider screen position?  size?  wrapping?  selectability?
}


/*	--------------------------------------------------------------------------------
	Function 	: PIECHOOSER::iconEffect
	Purpose 	: choose and initialise an appropriate effect for an ICON
	Parameters 	: ptr to icon for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this base-class implementation chooses no effects
*/
void PIECHOOSER::iconEffect(ICON* const icon, const PIEMode mode) const
{
	// PP: IMPORTANT: some PIECHOOSER subclasses might not override iconEffect, in which case we should call their itemEffect override
	this->itemEffect((PAGEITEM*)icon, mode);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIECHOOSER::pageEffect
	Purpose 	: choose and initialise an appropriate effect for a page
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this base-class implementation chooses no effects
*/
void PIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const
{	
	// PP: IMPORTANT: some PIECHOOSER subclasses might not override pageEffect, in which case we should call their itemEffect override
	this->itemEffect((PAGEITEM*)page, mode);
}


/*	--------------------------------------------------------------------------------
	Function 	: PIECHOOSER::bookEffect
	Purpose 	: choose and initialise an appropriate effect for a book
	Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this base-class implementation chooses no effects
*/
void PIECHOOSER::bookEffect(BOOK* const book, const PIEMode mode) const
{
	// PP: IMPORTANT: some PIECHOOSER subclasses might not override bookEffect, in which case we should call their itemEffect override
	this->itemEffect((PAGEITEM*)book, mode);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MAINPIECHOOSER

										The main pageitem effect chooser that we use.

				// PP: If you find that you want a different set of effects for a particular sub-game or whatever,
				I'd suggest you make a subclass of MAINPIECHOOSER that overrides only the methods you want to change.

														@mainpiechooser
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: MAINPIECHOOSER instance (the default chooser set for pages on init)
MAINPIECHOOSER	mainPIEChooser;

// PP: TEMP TEST
//#define NOFADES

/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::itemEffect
	Purpose 	: choose and initialise an appropriate effect for a general PAGEITEM
	Parameters 	: ptr to item for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::itemEffect
*/
void MAINPIECHOOSER::itemEffect(PAGEITEM* const item, const PIEMode mode) const
{
	if(mode == PIEMODE_ON)
	{
		item->startEffect(NULL);
	}
#ifndef NOFADES
	else
	{
		PIECHOOSE(item, PIE_fade);// PP: VERY TEMP, DON'T WORRY
	}
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::textboxEffect_ON
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX in PIEMODE_ON
	Parameters 	: ptr to textbox for which to choose effect
	Returns 	: 
	Info 		: 
*/
void MAINPIECHOOSER::textboxEffect_ON(TEXTBOX* const textbox) const
{
	if(textbox->selected)
	{
		textbox->startEffect(PIE_textLife.copy()->setItem(textbox)->setMode(PIEMODE_ON));
	}
	else
	{
		textbox->startEffect(NULL);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::textboxEffect
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
	Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::textboxEffect
*/
void MAINPIECHOOSER::textboxEffect(TEXTBOX* const textbox, const PIEMode mode) const
{
	if(mode == PIEMODE_ON)
	{
		this->textboxEffect_ON(textbox);
	}
	else if(mode == PIEMODE_ENTER)
	{
		// PP: LOOK! effects determined by text style!  wicked.
		if((textbox->style == &TS_title)||(textbox->style == &TS_smallTitle)||(textbox->style == &TS_creditsTitle))
		{
			PIECHOOSE(textbox, PIE_flip);
		}
		else if(textbox->style == &TS_info)
		{
			PIECHOOSE(textbox, PIE_flip);
		}
		else if((textbox->style == &TS_control)||(textbox->style == &TS_largeControl))
		{
			PIECHOOSE(textbox, PIE_hFlip);
		}
		else
		{
			if(textbox->rowIndex&1)
			{
				PIECHOOSE(textbox, PIE_leftSquashy);
			}
			else
			{
				PIECHOOSE(textbox, PIE_rightSquashy);
			}
		}
	}
	else if(mode == PIEMODE_EXIT)
	{
		if(textbox->selected)
		{
			PIECHOOSE(textbox, PIE_textExplode);
		}
		else
		{
			// PP: LOOK! effects determined by text style!  wicked.
			if((textbox->style == &TS_title)||(textbox->style == &TS_smallTitle)||(textbox->style == &TS_creditsTitle))
			{
				PIECHOOSE(textbox, PIE_flip);
			}
			else if(textbox->style == &TS_info)
			{
				PIECHOOSE(textbox, PIE_flip);
			}
			else if((textbox->style == &TS_control)||(textbox->style == &TS_largeControl))
			{
				PIECHOOSE(textbox, PIE_hFlip);
			}
			else
			{
				if(textbox->rowIndex&1)
				{
					PIECHOOSE(textbox, PIE_rightSquashy);
				}
				else
				{
					PIECHOOSE(textbox, PIE_leftSquashy);
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::iconEffect
	Purpose 	: choose and initialise an appropriate effect for an ICON
	Parameters 	: ptr to icon for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::iconEffect
*/
void MAINPIECHOOSER::iconEffect(ICON* const icon, const PIEMode mode) const
{
	// PP: this->itemEffect((PAGEITEM*)icon, mode);// PP: VERY TEMP, DON'T WORRY
//*/
	if((mode == PIEMODE_ENTER) || (mode == PIEMODE_EXIT))
	{
		PIECHOOSE(icon, PIE_iconSpin);
	}
	/*/

	this->itemEffect(icon, mode);
	//*/
}


/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::pageEffect
	Purpose 	: choose and initialise an appropriate effect for a PAGE
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::pageEffect
*/
void MAINPIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const
{
#ifndef NOFADES
	if(mode != PIEMODE_ON)// PP: CAUTION: I get a crash when I take this check out - not had time to look into why yet, but just be aware of it// PP: I don't imagine it'll be anything too cryptic
	{
		PIECHOOSE(page, PIE_fade);// PP: VERY TEMP, DON'T WORRY
	}
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: MAINPIECHOOSER::bookEffect
	Purpose 	: choose and initialise an appropriate effect for a BOOK
	Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::bookEffect
*/
void MAINPIECHOOSER::bookEffect(BOOK* const book, const PIEMode mode) const
{
#ifndef NOFADES
	if(mode != PIEMODE_ON)// PP: CAUTION: I get a crash when I take this check out - not had time to look into why yet, but just be aware of it// PP: I don't imagine it'll be anything too cryptic
	{
		PIECHOOSE(book, PIE_fade);// PP: VERY TEMP, DON'T WORRY
	}
#endif
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SPINBOOKPIECHOOSER

						PIE chooser for books that spin into & out of view, and whose contents have no effects.

														@spinbookpiechooser
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: the SPINBOOKPIECHOOSER instance
SPINBOOKPIECHOOSER		spinBookPIEChooser;


/*	--------------------------------------------------------------------------------
	Function 	: SPINBOOKPIECHOOSER::bookEffect
	Purpose 	: choose and initialise an appropriate effect for a book
	Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::bookEffect
*/
void SPINBOOKPIECHOOSER::bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const
{
	static TBVector		turns	={0.25f,0.25f,1};
	static float		time	=0.4f;

	if(book->flags & BKFLAG_TOPLEVELBOOK)
	{
		if((mode == PIEMODE_ENTER) || (mode == PIEMODE_EXIT))
		{
			book->startEffect((new PIE_SPIN(book, mode))
								->setTurns(turns[X], turns[Y], turns[Z])
								->setTime(time));
		}
	}
}



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														POPUPPIECHOOSER

												PIE chooser for POPUP books.

														@popuppiechooser
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: the POPUPPIECHOOSER instance
POPUPPIECHOOSER		popupPIEChooser;


/*	--------------------------------------------------------------------------------
	Function 	: POPUPPIECHOOSER::pageEffect
	Purpose 	: choose and initialise an appropriate effect for a PAGE
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::pageEffect
*/
void POPUPPIECHOOSER::pageEffect(struct TAG_PAGE* const page, const PIEMode mode) const
{
	if(page->flags & PAGEFLAG_TOPLEVELPAGE)
	{
		if((mode == PIEMODE_ENTER) || (mode == PIEMODE_EXIT))
		{
			PIECHOOSE(page, PIE_popupSpin);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: POPUPPIECHOOSER::textboxEffect
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
	Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::textboxEffect
*/
void POPUPPIECHOOSER::textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const
{
	// PP: treat on-mode textboxes as mainPIEChooser does
	if((mode == PIEMODE_ON) || (mode == PIEMODE_ENTER))// PP: shit, I don't seem to be getting a textbox ON-mode call
	{
		mainPIEChooser.textboxEffect_ON(textbox);
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														RACEPIECHOOSER

							PIE chooser for races and suchlike (countdowns, checkpoint messages, etc)

														@RACEPIECHOOSER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: the RACEPIECHOOSER instance
RACEPIECHOOSER		racePIEChooser;


/*	--------------------------------------------------------------------------------
	Function 	: RACEPIECHOOSER::textboxEffect
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
	Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::textboxEffect
*/
void RACEPIECHOOSER::textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const
{
	// PP: treat on-mode textboxes as mainPIEChooser does
	if(mode == PIEMODE_ENTER)
	{
		PIECHOOSE(textbox, PIE_iconSpin);
	}
	else if(mode == PIEMODE_ON)
	{
		PIECHOOSE(textbox, PIE_textLife);
	}
	else// PP: exit
	{
		PIECHOOSE(textbox, PIE_textExplode);
	}
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MEMCARDPIECHOOSER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


MEMCARDPIECHOOSER	memcardPIEChooser;

/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDPIECHOOSER::textboxEffect
	Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::textboxEffect
*/
void MEMCARDPIECHOOSER::textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const
{
	textbox->startEffect(new PIE_FADE(textbox, mode));
}

/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDPIECHOOSER::pageEffect
	Purpose 	: choose and initialise an appropriate effect for a PAGE
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::pageEffect
*/
void MEMCARDPIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const
{
	if(page->flags&PAGEFLAG_TOPLEVELPAGE)
	{
		page->startEffect(new PIE_FADE(page, mode));
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDPIECHOOSER::bookEffect
	Purpose 	: choose and initialise an appropriate effect for a book
	Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::bookEffect
*/
void MEMCARDPIECHOOSER::bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const
{
	if(mode == PIEMODE_EXIT)
	{
		bkPrintf("stop\n");
	}

	if(book->flags & BKFLAG_TOPLEVELBOOK)
	{
		book->startEffect(new PIE_FADE(book, mode));
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WANTEDPIECHOOSER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


WANTEDPIECHOOSER	WantedPIEChooser;


/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDPIECHOOSER::pageEffect
	Purpose 	: choose and initialise an appropriate effect for a PAGE
	Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::pageEffect
*/
void WANTEDPIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const
{
	/*if (page->flags&PAGEFLAG_TOPLEVELPAGE)
	{
		page->setEffect(
	}*/

	page->startEffect(NULL);
}



/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDPIECHOOSER::bookEffect
	Purpose 	: choose and initialise an appropriate effect for a book
	Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
	Returns 	: 
	Info 		: this overrides PIECHOOSER::bookEffect
*/
void WANTEDPIECHOOSER::bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const
{
	if(book->flags & BKFLAG_TOPLEVELBOOK)
	{
		book->startEffect(NULL);
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															WELCOME TO

														*** THE PIE SHOP ***

													where we have a wide variety of
														PRESET PAGE ITEM EFFECTS
														for your conwenience.

														Now THERE'S handy!™
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: Use this compact little PIE_SPIN on pages - and watch in amazement
// PP: as each page literally FLIPS over to reveal another on the reverse side!
// PP: Flips vertically.
// PP: cat no. 455678291		£15.99
PIE_SPIN			PIE_flip;
PIE_SPIN			PIE_hFlip;					// PP: Like PIE_flip but flips horizontally.
PIE_SPIN			PIE_popupSpin;				// PP: spin used on POPUPs
PIE_SPIN			PIE_iconSpin;				// PP: spin used for ICONs
PIE_SPIN			PIE_logoSpin;				// PP: audible spin-up used on frontend logo
PIE_TEXTLIFE		PIE_textLife;				// PP: 'dancing' text
PIE_TEXTEXPLODE		PIE_textExplode;			// PP: text explosion
PIE_SLIDE			PIE_upSlide;				// PP: default upward slide
PIE_SLIDE			PIE_downSlide;				// PP: default downward slide
PIE_SLIDE			PIE_leftSlide;				// PP: default leftward slide
PIE_SLIDE			PIE_rightSlide;				// PP: default rightward slide
PIE_SQUASHY			PIE_upSquashy;				// PP: default upward squashy
PIE_SQUASHY			PIE_downSquashy;			// PP: default downward squashy
PIE_SQUASHY			PIE_leftSquashy;			// PP: default leftward squashy
PIE_SQUASHY			PIE_rightSquashy;			// PP: default rightward squashy
PIE_FLASH			PIE_flash;					// PP: default flashing
PIE_FADE			PIE_fade;					// PP: default fade
PIE_ICONSHATTER		PIE_iconShatter;			// PP: default icon shatter


/*	--------------------------------------------------------------------------------
	Function 	: initPresetPIEs
	Purpose 	: initialise preset page item effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initPresetPIEs(void)
{
	(&PIE_flip)
	->setTurns(0.5f)
	->setTime(0.3f)
	->setCulling(true)
	->setScaling(false);

	(&PIE_hFlip)
	->setTurns(0.0f, 0.5f)
	->setTime(0.3f)
	->setCulling(true)
	->setScaling(false);

	(&PIE_popupSpin)
	->setTurns(0.3f, 0.0f, 0.3f)
	->setTime(0.3f)
	->setScaling(true);

	(&PIE_iconSpin)
	->setTurns(0.25f, 0.0f, 1.3f)
	->setTime(0.37f);

	(&(PIE_logoSpin=PIE_iconSpin))
	->setSound(true);

	// PP: ~~~

	(&PIE_upSlide)
	->setDir(PIESLIDEDIR_UP);

	(&PIE_downSlide)
	->setDir(PIESLIDEDIR_DOWN);

	(&PIE_leftSlide)
	->setDir(PIESLIDEDIR_LEFT);

	(&PIE_rightSlide)
	->setDir(PIESLIDEDIR_RIGHT);

	// PP: ~~~

	(&PIE_upSquashy)
	->setDir(PIESLIDEDIR_UP);

	(&PIE_downSquashy)
	->setDir(PIESLIDEDIR_DOWN);

	(&PIE_leftSquashy)
	->setDir(PIESLIDEDIR_LEFT);

	(&PIE_rightSquashy)
	->setDir(PIESLIDEDIR_RIGHT);

	// PP: ~~~

	(&PIE_iconShatter)
	->setSound(true);
}
