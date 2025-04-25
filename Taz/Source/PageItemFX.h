// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PageItemFX.h
//   Purpose : Special effects for page items
// Component : Taz
//		Info : // PP: Come to me, My Pretties!!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __H_PAGEITEMFX
#define __H_PAGEITEMFX

#include "VideoFX.h"						// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGEITEMEFFECT

					Base class for effect parasites that influence the drawing of the PAGEITEMs to which they belong.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PAGEITEMEFFECT
{

private:

	float				delay;					// PP: delay before the effect starts
						
protected:
				
	// PP: PIE modes supported by this type.
	// PP: This must be the bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	const static uchar	supportedModes;
	
	PAGEITEM*			item;					// PP: item to which this effect belongs and upon which it acts


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTLIFE::getNumChars
		Purpose 	: find out how many characters are involved in the effect
		Parameters 	: 
		Returns 	: number of characters involved in the effect
		Info 		: // PP: Please don't call this on a non-textbox effect!
	*/
	inline ushort getNumChars(void) const
	{
		return ((TEXTBOX*)(this->item))->textLen;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::getItemWidth
		Purpose 	: get the width of the effect's item
		Parameters 	: 
		Returns 	: the width of the effect's item
		Info 		: 
	*/
	float getItemWidth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::getItemHeight
		Purpose 	: get the height of the effect's item
		Parameters 	: 
		Returns 	: the height of the effect's item
		Info 		: 
	*/
	float getItemHeight(void);


public:

	PIEMode				mode;					// PP: mode for the effect (see PIEMode enum PIEMODE_...)
	bool				finished;				// PP: is the effect finished?


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::supportsMode
		Purpose 	: determine if the effect supports the specified effect mode
		Parameters 	: effect mode
		Returns 	: true if the type supports the specified mode, else false
		Info 		: 
	*/
#define DECLARE_PIE_SUPPORTSMODE_METHOD \
	virtual bool supportsMode(const PIEMode modeIn) \
	{ \
		return ((this->supportedModes & (1<<modeIn)) != 0); \
	}

	DECLARE_PIE_SUPPORTSMODE_METHOD;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::setMode
		Purpose 	: set the mode of the effect
		Parameters 	: mode for effect (see PIEMode enum PIEMODE_...)
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: // PP: this gets called (with the appropriate mode) whenever the effect is started
	*/
	virtual PAGEITEMEFFECT* setMode(const PIEMode modeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::setItem
		Purpose 	: set the item for the effect
		Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act,
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: // PP: this gets called by PAGEITEM::setEffect
	*/
	virtual PAGEITEMEFFECT* setItem(PAGEITEM* const itemIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this should only get called after both setMode and setItem
	*/
	virtual void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::PAGEITEMEFFECT
		Purpose 	: constructor
		Parameters 	: 
		Returns 	: new PAGEITEMEFFECT instance
		Info 		: 
	*/
#define DECLARE_PIE_ITEM_AND_MODE_CONSTRUCTOR(_class) \
		_class(PAGEITEM* const itemIn, const PIEMode modeIn) \
		{ \
			this->init(); \
			this->setItem(itemIn); \
			this->setMode(modeIn); \
		} \

	DECLARE_PIE_ITEM_AND_MODE_CONSTRUCTOR(PAGEITEMEFFECT);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: 
	*/
	virtual bool update(void)
	{
		return true;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: the effect is wholly responsible for drawing the item to which it belongs.
						However, PAGEITEMS that don't have an effect just draw themselves as usual.
	*/
	virtual inline void draw(void)
	{
		// PP: simply draw the pageitem in the normal way
		// PP: NOTE: we can't use PAGEITEM::draw because it would pass control straight back here again!  "To me, to you..."
		this->item->drawWithoutEffect();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void shutdown(void){}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::processCharacter
		Purpose 	: process one of the textbox's characters
		Parameters 	: ptr to the first of the four verts forming the character,
						index of the character
		Returns 	: 
		Info 		: called for each pass of each character by TextFX_escapeCallback
						So, y'know, keep it lean.
	*/
	virtual void processCharacter(TBPrimVertex* const verts, const int polyIndex){};


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::PAGEITEMEFFECT
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: new PAGEITEMEFFECT instance
		Info 		: 
	*/
	PAGEITEMEFFECT::PAGEITEMEFFECT(void)
	{
		// PP: hmm, this doesn't seem to call the lowest override of init
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: 
		Purpose 	: default constructor: does fook all
		Parameters 	: 
		Returns 	: new PAGEITEMEFFECT instance
		Info 		: 
	*/

#define DECLARE_PIE_DEFAULT_CONSTRUCTOR(_class) \
	_class(void) \
	{ \
		/* PP: NEW TEST!*/ \
		this->init(); \
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::~PAGEITEMEFFECT
		Purpose 	: destructor: calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
#define DECLARE_PIE_DESTRUCTOR(_class) \
	virtual ~_class(void) \
	{ \
		this->shutdown(); \
	}

	DECLARE_PIE_DESTRUCTOR(PAGEITEMEFFECT);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEMEFFECT::copy
		Purpose 	: make a copy of this
		Parameters 	: 
		Returns 	: a PAGEITEMEFFECT pointer to a new replica of this
		Info 		: 
	*/
#define DECLARE_PIE_COPY_METHOD(_class) \
		virtual PAGEITEMEFFECT* _class::copy(void) const \
		{ \
			PAGEITEMEFFECT*		newEffect;\
			\
			newEffect=new _class(*this);\
			\
			/*temp test*/\
			extern PAGEITEMEFFECT* trapEffect; \
			if(newEffect == trapEffect) \
			{ \
				int z=0;\
				int crash=1/z; \
			}\
			\
			return newEffect; \
		}

	DECLARE_PIE_COPY_METHOD(PAGEITEMEFFECT);


/* PP:	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		IMPORTANT: WILL ALL PAGEITEMEFFECT SUBCLASSES PLEASE PUBLICLY USE
		'STANDARD_PIE_OVERRIDES' IN THEIR DEFINITONS - CHEERS.
		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: macro to declare standard overrides of PAGEITEMEFFECT base-class methods
#define STANDARD_PIE_OVERRIDES(_class) \
		DECLARE_PIE_COPY_METHOD(_class); \
		DECLARE_PIE_DEFAULT_CONSTRUCTOR(_class); \
		DECLARE_PIE_ITEM_AND_MODE_CONSTRUCTOR(_class); \
		DECLARE_PIE_DESTRUCTOR(_class); \
		DECLARE_PIE_SUPPORTSMODE_METHOD;


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PIE_FADE

							PAGEITEMEFFECT subclass that alpha-fades its PAGEITEM into or out of view.

				This is one of the first PAGEITEMEFFECT subclasses, and a good starting point I'm sure you'll agree.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PIE_FADE			:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	const static uchar		supportedModes;

	float					opacity;				// PP: (0..1) opacity of the fade effect - the pageitem's natural opacity is scaled by this
	float					maxOpacity;				// PP: (0..1) maximum opacity of fade, ie. opacity to which it fades up in PIEMODE_ENTER and from which it fades down in PIEMODE_EXIT
	float					duration;				// PP: duration of fade (seconds)
						
public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::setDuration
		Purpose 	: set the duration of the fade
		Parameters 	: the duration of the fade (in seconds)
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: // PP: this duration will apply to both PIEMODE_ENTER and PIEMODE_EXIT - you don't like it? CHANGE IT!
	*/
	PAGEITEMEFFECT* setDuration(float durationIn)
	{
		this->duration=durationIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::setMaxOpacity
		Purpose 	: specify the maximum opacity for the fade
		Parameters 	: the maximum opacity for the fade (0..1)
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: 
	*/
	PAGEITEMEFFECT* setMaxOpacity(float maxOpacityIn)
	{
		this->maxOpacity=maxOpacityIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FADE::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::shutdown, to return its item to full opacity
	*/
	void shutdown(void);


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_FADE);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PIE_SLIDE

							PAGEITEMEFFECT subclass that 'slides' its PAGEITEM onto or off of the screen.

				This is one of the first PAGEITEMEFFECT subclasses, and a good starting point I'm sure you'll agree.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: possible directions for a PIE_SLIDE effect.
// PP: might want to add diagonals at some point, ye never know
enum PIESlideDir
{
	// PP: NOTE: this order is relied upon by PIE_SLIDE
	PIESLIDEDIR_UP,
	PIESLIDEDIR_RIGHT,
	PIESLIDEDIR_DOWN,
	PIESLIDEDIR_LEFT,

	PIESLIDE_NUM_DIRECTIONS
};


struct PIE_SLIDE			:	public PAGEITEMEFFECT
{

private:

	float					accel;					// PP: current acceleration on axis of interest (units/sec/sec)
	float					srcOffset;				// PP: initial offset on axis of interest
	float					destOffset;				// PP: target offset on axis of interest

protected:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;			// PP: protected for the benefit of subclasses' macro-generated getSupportedModes overrides

	char					dir;					// PP: direction of movement (-1 or 1)
	uchar					axis			:1;		// PP: axis of movement (X or Y)
	float					speed;					// PP: current speed on axis of interest (units/sec)
	float					offset;					// PP: current offset on axis of interest, in direction of interest
	TBVector2D				xyOffset;				// PP: current XY offset	

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	virtual void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::setDir
		Purpose 	: set the direction of the slide
		Parameters 	: slide direction (PIESLIDEDIR_..., see PIESlideDir enum)
		Returns 	: this
		Info 		: 
	*/
	PIE_SLIDE* setDir(const PIESlideDir dirIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::setSpeed
		Purpose 	: set the speed of the slide
		Parameters 	: the speed of the slide
		Returns 	: this
		Info 		: 
	*/
	inline PIE_SLIDE* setSpeed(const float speedIn)
	{
		this->speed=speedIn;
		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	virtual void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	virtual bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SLIDE::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	virtual void draw(void);


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_SLIDE);


}BALIGN16;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PIE_ZOOM

							PAGEITEMEFFECT subclass that 'slides' its PAGEITEM onto or off of the screen.

				This is one of the first PAGEITEMEFFECT subclasses, and a good starting point I'm sure you'll agree.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct PIE_ZOOM			:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;			// PP: ENTER & EXIT - is this right John?
						
	float					speed;					// current speed on axis of interest (units/sec)
	float					target;
	float					originalScale;
	float					originalSpeed;
	float					rotat;
	float					scale;
	float					accel;
public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::setSpeed
		Purpose 	: sets the speed of the zoom
		Parameters 	: zoom speed
		Returns 	: this
		Info 		: if the speed is positive the item will zoom in, if negative: out
	*/
	PIE_ZOOM* setSpeed(const float speed);

	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::setSpeed
		Purpose 	: sets the speed of the zoom
		Parameters 	: zoom speed
		Returns 	: this
		Info 		: if the speed is positive the item will zoom in, if negative: out
	*/
	PIE_ZOOM* setAccel(const float accel);

	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::setTarget
		Purpose 	: sets the target size of the zoom effetc
		Parameters 	: zoom target
		Returns 	: this
		Info 		: if the speed of the zoom is positive this will be the final size of the item, if it's negative the item will start at this scale
	*/
	PIE_ZOOM* setOrigin(const float origin);

	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::setTarget
		Purpose 	: sets the target size of the zoom effetc
		Parameters 	: zoom target
		Returns 	: this
		Info 		: if the speed of the zoom is positive this will be the final size of the item, if it's negative the item will start at this scale
	*/
	PIE_ZOOM* setTarget(const float target);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	void draw(void);

	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ZOOM::getState
		Purpose 	: get the state of the zoom effect
		Parameters 	: 
		Returns 	: true if effect finished
		Info 		: 
	*/
	bool getState(void);


	// declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_ZOOM);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_FLASH

								ON-mode PAGEITEMEFFECT subclass that makes its PAGEITEM flash.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@pie_flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PIE_FLASH			:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;

	float					interval;			// PP: time interval between flashes (seconds)
	float					timer;				// PP: time since the effect started
	bool					on;					// PP: is the flash currently 'on', or 'off'?

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::setInterval
		Purpose 	: set the time interval between flashes (seconds)
		Parameters 	: the time interval between flashes (seconds)
		Returns 	: this
		Info 		: 
	*/
	PAGEITEMEFFECT* setInterval(const float intervalIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
//	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_FLASH::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::shutdown, to return its item to full opacity
	*/
	void shutdown(void);


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_FLASH);


}BALIGN16;


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

// PP: self-explanatory defaults - no need for comments here...

#define PIESPIN_DEFAULT_X_TURNS		0.0f
#define PIESPIN_DEFAULT_Y_TURNS		0.0f
#define PIESPIN_DEFAULT_Z_TURNS		2.0f
#define PIESPIN_DEFAULT_DURATION	0.4f
#define PIESPIN_DEFAULT_SCALING		true
#define PIESPIN_DEFAULT_CULLING		false
#define PIESPIN_DEFAULT_SOUND		false

struct PIE_SPIN		:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;
							
	bool					sound;				// PP: sound on/off (though I can imagine this getting more customisable later)

	VEC						totalRotation;		// PP: the rotation to be done by the effect on PIEMODE_EXIT (or the inverse for PIEMODE_ENTER)
	float					duration;			// PP: duration of the effect, in seconds
	TBVector2D				pos;				// PP: position from which the item appears in PIEMODE_ENTER, or to which it disappears in PIEMODE_EXIT
													// PP: eg. 0.5f, 0.5f = top right corner of screen

	bool					scaling;			// PP: true if we want to scale the item as well as rotating it; else false
	bool					culling;			// PP: true if we want to hide the flip-side of the item during rotation
												
	VEC						startAng;			// PP: item's initial x,y,z angles (NOT looped within TWOPI)
	VEC						finalAng;			// PP: item's final x,y,z angles (NOT looped within TWOPI)
												
	VEC						ang;				// PP: item's current x,y,z angles (NOT looped within TWOPI)
	float					scale;				// PP: item's current scale (1.0f=natural scale)
	bool					culled;				// PP: the item is has been culled out (always false unless culling is turned on)
	float					timer;				// PP: time the effect has been going for

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);

	
	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setTurns
		Purpose 	: set the number of turns to do on each axis
		Parameters 	: number of turns on X axis, (opt/0.0f)number of turns on Y axis,
						(opt/0.0f)number of turns on Z axis
		Returns 	: this
		Info 		: // PP: positive numbers of turns are clockwise.
						NOTE: DON'T pass in RADIANS or DEGREES, pass in NUMBERS OF TURNS!!!!
	*/
	PIE_SPIN* setTurns(const float xTurns, const float yTurns=0.0f, const float zTurns=0.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setTime
		Purpose 	: set the duration of the effect
		Parameters 	: duration of the effect, in seconds
		Returns 	: this
		Info 		: // PP: 'setTime' was quicker to type than 'setDuration'
	*/
	PIE_SPIN* setTime(const float time);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setPos
		Purpose 	: set the position from which the item appears in PIEMODE_ENTER,
						or to which it disappears in PIEMODE_EXIT
		Parameters 	: screen position, eg. 0.5f, 0.5f = top right corner of screen
		Returns 	: this
		Info 		: 
	*/
	PIE_SPIN* setPos(const float x, const float y);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setScaling
		Purpose 	: specify whether or not we want scaling in/out along with our rotations
		Parameters 	: true=scaling / false=no scaling
		Returns 	: this
		Info 		: // PP: NOTE: obviously, the only time you'd want to turn scaling off
						is when your rotations are such that they hide the entry/exit of the item.
						Eg., quarter-turns (which will turn the item 'side-on')
	*/
	PIE_SPIN* setScaling(const bool on);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setCulling
		Purpose 	: turn flipside culling on/off
		Parameters 	: true=culling / false=no culling
		Returns 	: this
		Info 		: // PP: when you set culling true, you won't see the reverse side of the item
						This is handy when you want 2 items to appear to be on each other's flip sides.
	*/
	PIE_SPIN* setCulling(const bool on);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SPIN::setSound
		Purpose 	: set sound on/off for the effect
		Parameters 	: true=sound / false=no sound
		Returns 	: this
		Info 		: // PP: I can imagine this getting more customisable later
	*/
	void setSound(const bool on)
	{
		this->sound=on;
	}



	// PP:  declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_SPIN);


}BALIGN16;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_ICONSHATTER

							EXIT-mode PAGEITEMEFFECT subclass that shatters an ICON into triangular shards.

			// PP: Not quite as nice as the Ngon-shard any-PAGEITEM ORT_PIE shatter that I had in mind originally,
									but time is a luxury that's getting very very scarce now.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@PIE_ICONSHATTER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PIESHATTER_SHARD
{
	TBPrimVertex	verts[3];

	TBVector2D		pos;					// PP: 2D centre position
	TBVector2D		vel;					// PP: 2D velocity

	COLOUR			colour;					// PP: colour
											
	VEC				angVel;					// PP: xyz angular velocity (dunno if that's a physically accurate term, not sure I care either, you know what I mean)
	VEC				ang;					// PP: xyz angle

}BALIGN16;


#define PIESHATTER_NUM_SHARDS_EACH_WAY		6

struct PIE_ICONSHATTER			:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;

	static const uint32		numShards;
	static const uint32		numVerts;

	// PP: there will be PIESHATTER_NUM_SHARDS_EACH_WAY*(PIESHATTER_NUM_SHARDS_EACH_WAY/2) of them
	PIESHATTER_SHARD*		shards;			// PP: array of shards

	bool					sound;			// PP: sound on/off

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::setSound
		Purpose 	: set sound on/off for the effect
		Parameters 	: true=sound / false=no sound
		Returns 	: this
		Info 		: I'm not a goth
	*/
	void setSound(const bool on);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_ICONSHATTER::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::shutdown
	*/
	void shutdown(void);


	// declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_ICONSHATTER);


}BALIGN16;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_SQUASHY

				PIE_SLIDE(PAGEITEMEFFECT) subclass that makes its PAGEITEM behave like a cute squashy thing.
									You'll want to take it home and give it a cuddle.


														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@PIE_SQUASHY
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#if 1

struct PIE_SQUASHY			:	public PIE_SLIDE
{

private:

	float					springOffset;		// PP: 
	float					springSpeed;		// PP: speed of spring end of item (on same scale as PIE_SLIDE::speed)
	float					springAccel;
	float					compression;		// PP: current compression
	TBVector2D				scale;				// PP: current x&y scale, resulting from compression
	float					squashyOffset;

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SQUASHY::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SQUASHY::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SQUASHY::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_SQUASHY::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	void draw(void);


	// declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_SQUASHY);


}BALIGN16;
#endif// PP:  0

#ifdef THIS_IS_JUST_A_TEMPLATE_TO_SAVE_A_BIT_OF_TYPING

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEMPLATE

							PAGEITEMEFFECT subclass that 

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

														@PIE_TEMPLATE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PIE_TEMPLATE			:	public PAGEITEMEFFECT
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar		supportedModes;

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEMPLATE::start
		Purpose 	: start the effect
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEMEFFECT::start
						// PP: this should only get called after both setMode and setItem
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEMPLATE::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEMPLATE::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEMPLATE::draw
		Purpose 	: draw the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::draw
	*/
	void draw(void);


	// declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_TEMPLATE);


}BALIGN16;

#endif// PP: def THIS_IS_JUST_A_TEMPLATE_TO_SAVE_A_BIT_OF_TYPING


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															TEXT_PIE

					Base-class for PAGEITEMEFFECT subclasses that manipulate the characters in a TEXTBOX.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@text_pie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: At present, on PS2, you can't move characters around when printing with bdPrintFontLite, without seeing connecting polys
// PP: Define TEXTPIE_FULL_PRINT to force textboxes with TEXT_PIEs on them to use the full bdPrintFont
/* PP: REMOUT: this should be fixed now	
#if(BPLATFORM == PS2)
#define TEXTPIE_FULL_PRINT
#endif// PP: ps2
*/

struct TEXT_PIE				:	public PAGEITEMEFFECT
{

protected:	

	// PP: see .cpp for struct definition
	struct TEXTPIE_CHARINFO*	baseCharInfo;		// PP: array of character info structs

	bool						counteractScale;	// PP: true if the text's final scale is to be counteracted when applying position offsets

public:


	/*	--------------------------------------------------------------------------------
		Function 	: TEXT_PIE::setItem
		Purpose 	: set the item for the effect
		Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: this overrides PAGEITEMEFFECT::setItem
	*/
	virtual PAGEITEMEFFECT* setItem(PAGEITEM* const itemIn);


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
	void processCharacter(TBPrimVertex* const verts, const int polyIndex);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXT_PIE::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);
	

	// declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(TEXT_PIE);


}BALIGN16;


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


struct PIE_TEXTLIFE			:	public TEXT_PIE
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar			supportedModes;
	
	// PP: see .cpp for struct definition
	struct TEXTLIFE_CHARINFO*	charInfo;		// PP: array of character info structs

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTLIFE::setItem
		Purpose 	: set the item for the effect
		Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act,
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: this overrides TEXT_PIE::setItem
						// PP: this gets called by PAGEITEM::setEffect
	*/
	PAGEITEMEFFECT* setItem(PAGEITEM* const itemIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTLIFE::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTLIFE::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_TEXTLIFE);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PIE_TEXTEXPLODE

			EXIT-mode TEXT_PIE (PAGEITEMEFFECT) subclass that makes a textbox explode, sending all the characters flying.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PIE_TEXTEXPLODE			:	public TEXT_PIE
{

private:

	// PP: PIE modes supported by this type.
	// PP: This must be a bitwise OR of 1 left-shifted by each of the supported PIEMode values.
	static const uchar				supportedModes;

	// PP: see .cpp for struct definition
	struct TEXTEXPLODE_CHARINFO*	charInfo;		// PP: array of character info structs
	
public:


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTEXPLODE::setItem
		Purpose 	: set the item for the effect
		Parameters 	: ptr to the PAGEITEM to which this effect belongs and upon which it will act,
		Returns 	: (PAGEITEMEFFECT*)this
		Info 		: this overrides TEXT_PIE::setItem
						// PP: this gets called by PAGEITEM::setEffect
	*/
	PAGEITEMEFFECT* setItem(PAGEITEM* const itemIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTEXPLODE::update
		Purpose 	: update the effect
		Parameters 	: 
		Returns 	: true if finished, else false
		Info 		: this overrides PAGEITEMEFFECT::update
	*/
	bool update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PIE_TEXTEXPLODE::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(PIE_TEXTEXPLODE);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															ORT_PIE

	Base-class for PAGEITEMEFFECT subclasses that need to draw their PAGEITEMs to an offscreen render target as part of their effect.

														> rows > items
										  book > pages					> *EFFECTS*
														> free items

															@ort_pie
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct ORT_PIE				:	public PAGEITEMEFFECT
{

private:

	// PP: hmm, let's say 256*256 maybe? 256*128? just don't know yet what I can get away with
	static TBRenderTarget*		renderTarget;

	// PP: info aboot wur render target
	static RENDERTARGETINFO		renderTargetInfo;

	// PP: render target is locked for drawing by an ORT_PIE
	static bool					renderTargetLocked;

	// PP: this'll get imcremented by useORT and decremented by releaseORT.
	// PP: when it gets back to zero, I'll probably free the render target so's to save slowdown or that
	static ushort				numORTUsers;

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: ORT_PIE::useORT
		Purpose 	: register this effect as a user of the offscreen render target
		Parameters 	: 
		Returns 	: false if there was some problem (ie ORT creation failed), else true
		Info 		: This'll get called when the effect starts
						// PP: DANGER: ORT CREATION AND DELETION IS *REAL* *SLOW* !!!!
	*/
	static bool useORT(void);
	

	/*	--------------------------------------------------------------------------------
		Function 	: ORT_PIE::releaseORT
		Purpose 	: unregister this effect as a user of the offscreen render target
		Parameters 	: 
		Returns 	: 
		Info 		: This'll get called when the effect finishes
						// PP: DANGER: ORT CREATION AND DELETION IS *REAL* *SLOW* !!!!
	*/
	static void releaseORT(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ORT_PIE::drawItemToORT
		Purpose 	: draw the effect's item to the offscreen render target
		Parameters 	: 
		Returns 	: false if there was a problem (eg. ORT is locked for drawing by another ORT_PIE), else true
		Info 		: // PP: TODO?: params like, um, blockyness, rotation etc?
	*/
	bool drawItemToORT(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ORT_PIE::init
		Purpose 	: initialise the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::init
	*/
	void init(void)
	{
		// PP: "I'm going to be using the render target, ja?"
		ORT_PIE::useORT();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ORT_PIE::shutdown
		Purpose 	: shut-down the effect
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEMEFFECT::shutdown
	*/
	void shutdown(void)
	{
		// PP: "I'm finished with the render target now, ta."
		ORT_PIE::releaseORT();
	}


	// PP: declare standard overrides of PAGEITEMEFFECT base-class methods
	STANDARD_PIE_OVERRIDES(ORT_PIE);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PIECHOOSER

											Base class for pageitem effect choosers.
							Effect choosers help choose the effects to use for the items on a page.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: Macro for a PAGEITEM to choose an effect, with the help of its PIECHOOSER, in the conventional way
#define PAGEITEM_CHOOSE_EFFECT(_itemType) \
	\
	SAFE_DELETE(this->effect);/* PP: NOTE: this is also done in PAGEITEM::startEffect, don't worry*/ \
	\
	if(this->presetEffects[mode] == NULL) \
	{ \
		if(this->effectChooser != NULL)/* PP chooser would be null eg. if the item had been selected before its container had been realigned (eg. opened) */ \
		{ \
			this->effectChooser->_itemType##Effect(this, mode); \
		} \
	} \
	else \
	{ \
		this->startEffect(this->presetEffects[mode]->copy()->setItem(this)->setMode(mode)); \
	}


struct PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: PIECHOOSER::itemEffect
		Purpose 	: choose and initialise an appropriate effect for a general PAGEITEM
		Parameters 	: ptr to item for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this base-class implementation chooses no effects
	*/
	virtual void itemEffect(struct TAG_PAGEITEM* const item, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PIECHOOSER::textboxEffect
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
		Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this base-class implementation chooses no effects
	*/
	virtual void textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PIECHOOSER::iconEffect
		Purpose 	: choose and initialise an appropriate effect for an ICON
		Parameters 	: ptr to icon for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this base-class implementation chooses no effects
	*/
	virtual void iconEffect(struct TAG_ICON* const item, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PIECHOOSER::pageEffect
		Purpose 	: choose and initialise an appropriate effect for a page
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this base-class implementation chooses no effects
	*/
	virtual void pageEffect(struct TAG_PAGE* const page, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PIECHOOSER::bookEffect
		Purpose 	: choose and initialise an appropriate effect for a book
		Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this base-class implementation chooses no effects
	*/
	virtual void bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const;


}BALIGN16;


// PP: PIECHOOSER base-class instance
extern PIECHOOSER		basePIEChooser;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MAINPIECHOOSER

										The main pageitem effect chooser that we use.

				// PP: If you find that you want a different set of effects for a particular sub-game or whatever,
				I'd suggest you make a subclass of MAINPIECHOOSER that overrides only the methods you want to change.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct MAINPIECHOOSER			:	public PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::itemEffect
		Purpose 	: choose and initialise an appropriate effect for a general PAGEITEM
		Parameters 	: ptr to item for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::itemEffect
	*/
	virtual void itemEffect(struct TAG_PAGEITEM* const item, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::textboxEffect_ON
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX in PIEMODE_ON
		Parameters 	: ptr to textbox for which to choose effect
		Returns 	: 
		Info 		: 
	*/
	void textboxEffect_ON(struct TAG_TEXTBOX* const textbox) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::textboxEffect
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
		Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::textboxEffect
	*/
	virtual void textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::iconEffect
		Purpose 	: choose and initialise an appropriate effect for an ICON
		Parameters 	: ptr to icon for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::iconEffect
	*/
	virtual void iconEffect(struct TAG_ICON* const icon, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::pageEffect
		Purpose 	: choose and initialise an appropriate effect for a PAGE
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::pageEffect
	*/
	virtual void pageEffect(struct TAG_PAGE* const page, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MAINPIECHOOSER::bookEffect
		Purpose 	: choose and initialise an appropriate effect for a BOOK
		Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::bookEffect
	*/
	virtual void bookEffect(BOOK* const book, const PIEMode mode) const;


}BALIGN16;

// PP: MAINPIECHOOSER instance (the default chooser set for pages on init)
extern MAINPIECHOOSER	mainPIEChooser;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SPINBOOKPIECHOOSER

						PIE chooser for books that spin into & out of view, and whose contents have no effects.

														@spinbookpiechooser
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct SPINBOOKPIECHOOSER			:	public PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: SPINBOOKPIECHOOSER::bookEffect
		Purpose 	: choose and initialise an appropriate effect for a book
		Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::bookEffect
	*/
	void bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const;


}BALIGN16;

// PP: the SPINBOOKPIECHOOSER instance
extern SPINBOOKPIECHOOSER	spinBookPIEChooser;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														POPUPPIECHOOSER

												PIE chooser for POPUP books.

														@popuppiechooser
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct POPUPPIECHOOSER			:	public PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: POPUPPIECHOOSER::pageEffect
		Purpose 	: choose and initialise an appropriate effect for a PAGE
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::pageEffect
	*/
	void pageEffect(struct TAG_PAGE* const page, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: POPUPPIECHOOSER::textboxEffect
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
		Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::textboxEffect
	*/
	void textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const;


}BALIGN16;

// PP: the POPUPPIECHOOSER instance
extern POPUPPIECHOOSER		popupPIEChooser;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														RACEPIECHOOSER

							PIE chooser for races and suchlike (countdowns, checkpoint messages, etc)

														@RACEPIECHOOSER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct RACEPIECHOOSER			:	public PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: RACEPIECHOOSER::textboxEffect
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
		Parameters 	: ptr to textbox for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::textboxEffect
	*/
	void textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const;


}BALIGN16;

// PP: the RACEPIECHOOSER instance
extern RACEPIECHOOSER		racePIEChooser;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MEMCARDPIECHOOSER

										The main pageitem effect chooser that we use.

				// If you find that you want a different set of effects for a particular sub-game or whatever,
				I'd suggest you make a subclass of MEMCARDPIECHOOSER that overrides only the methods you want to change.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct MEMCARDPIECHOOSER			:	public PIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDPIECHOOSER::textboxEffect
		Purpose 	: choose and initialise an appropriate effect for a TEXTBOX
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::textboxEffect
	*/
	void textboxEffect(struct TAG_TEXTBOX* const textbox, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDPIECHOOSER::pageEffect
		Purpose 	: choose and initialise an appropriate effect for a PAGE
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::pageEffect
	*/
	void MEMCARDPIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDPIECHOOSER::bookEffect
		Purpose 	: choose and initialise an appropriate effect for a book
		Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::bookEffect
	*/
	void bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const;


}BALIGN16;


extern MEMCARDPIECHOOSER	memcardPIEChooser;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														WANTEDPIECHOOSER
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct WANTEDPIECHOOSER			:	public MAINPIECHOOSER
{
	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDPIECHOOSER::pageEffect
		Purpose 	: choose and initialise an appropriate effect for a PAGE
		Parameters 	: ptr to page for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::pageEffect
	*/
	void WANTEDPIECHOOSER::pageEffect(PAGE* const page, const PIEMode mode) const;


	/*	--------------------------------------------------------------------------------
		Function 	: MEMCARDPIECHOOSER::bookEffect
		Purpose 	: choose and initialise an appropriate effect for a book
		Parameters 	: ptr to book for which to choose effect, mode of effect (PIEMODE_...), 
		Returns 	: 
		Info 		: this overrides PIECHOOSER::bookEffect
	*/
	void bookEffect(struct TAG_BOOK* const book, const PIEMode mode) const;


}BALIGN16;


extern WANTEDPIECHOOSER	WantedPIEChooser;


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
// PP: cat no. 455678291		£15.99
extern PIE_SPIN				PIE_flip;				
extern PIE_SPIN				PIE_hFlip;					// PP: Like PIE_flip but flips horizontally.
extern PIE_SPIN				PIE_popupSpin;				// PP: spin used on POPUPs
extern PIE_SPIN				PIE_iconSpin;				// PP: spin used for ICONs
extern PIE_SPIN				PIE_logoSpin;				// PP: audible spin-up used on frontend logo
extern PIE_TEXTLIFE			PIE_textLife;				// PP: 'dancing' text
extern PIE_TEXTEXPLODE		PIE_textExplode;			// PP: text explosion
extern PIE_SLIDE			PIE_upSlide;				// PP: default upward slide
extern PIE_SLIDE			PIE_downSlide;				// PP: default downward slide
extern PIE_SLIDE			PIE_leftSlide;				// PP: default leftward slide
extern PIE_SLIDE			PIE_rightSlide;				// PP: default rightward slide
extern PIE_SQUASHY			PIE_upSquashy;				// PP: default upward squashy
extern PIE_SQUASHY			PIE_downSquashy;			// PP: default downward squashy
extern PIE_SQUASHY			PIE_leftSquashy;			// PP: default leftward squashy
extern PIE_SQUASHY			PIE_rightSquashy;			// PP: default rightward squashy
extern PIE_FLASH			PIE_flash;					// PP: default flashing
extern PIE_FADE				PIE_fade;					// PP: default fade
extern PIE_ICONSHATTER		PIE_iconShatter;			// PP: default icon shatter


/*	--------------------------------------------------------------------------------
	Function 	: initPresetPIEs
	Purpose 	: initialise preset page item effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initPresetPIEs(void);


#endif// PP: ndef __H_PAGEITEMFX