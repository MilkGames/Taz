// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Icon.h
//   Purpose : Icon effects used by the front end
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_ICON
#define __H_ICON

#include "text2.h"
#include "TextFX.h"				//text effects
#include "display.h"			// PP: provide basic display setup and managment functions


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													ICON page item type
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define ICON_WIDTH			0.18f			// PP: REMOUT: must now be expressed as a fraction of screen height		80.0f			// PP: width of an icon
#define ICON_HEIGHT			ICON_WIDTH		// PP: height of an icon

#define ICON_TILT_PERIOD	1.0f			// PP: period of sinusoidal icon tilting (seconds)
#define ICON_MAX_TILT		0.2f			// PP: extend of icon tilting (radians each way)

// PP: set default (placeholder) texture for the icon
#define ICON_SET_PLACEHOLDER_TEXTURE \
		{ \
			char _texName[]={"tempicon.bmp"}; \
			bkLoadTexture(NULL, _texName, 0); \
		}


typedef struct TAG_ICON		:		public PAGEITEM
{
	friend struct			PIE_ICONSHATTER;			// PP: PAGEITEMEFFECT subclass that shatters an ICON into triangular shards.

	friend struct			TAG_PAPERBOOK;				// PP: mmm, yeah, icon-only paperbooks need to get a hold of their icons' texture ptrs y'see

	// PP: page-close func for Rule Book multiplayer pages
	// PP: NOTE: this is a BODGE
	friend void MP_RuleBook_pageCloseFunc(struct TAG_PAGE* const page, const int32 context);// PP: BODGE for the Rule book

private:

	float					tiltPhase;					// PP: phase 0..2PI for sinusoidal icon tilting
	float					tiltCounter;				// PP: counter used for icon tilting
	uint32					texNameCRC;					// JW: name of texture for update

	TBTexture*				tex;						// PP: texture of the icon
	float					maxTilt;					// TP: Max tilt each way in radians, replaces ICON_MAX_TILE
	float					maxTiltDecay;				// TP: Percentage of max tilt to take away each second approx
	float					lag;						// TP: tilt lag to apply, 0.0f infinite lag 1.0f no lag at all, stops the snap when changing max tilt


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this textbox
		Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
		Returns 	: 
		Info 		: this overrides PAGEITEM::chooseEffect
	*/
	virtual void chooseEffect(const PIEMode mode, const bool recur);


protected:

	
	/*	--------------------------------------------------------------------------------
		Function 	: ICON::mainDraw
		Purpose 	: the core of the draw method - actually draw the icon
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


public:

	bool					tilt;						// PP: should the icon do its sinusoidal tilt, or just keep still?
	TBVector				angle;						// PP: angle of the icon
	TBVector				offset;						// PP: offset of icon - used by ICONCHANGE


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setLag
		Purpose 	: sets the lag on the tilting operation 0.0f infinite lag 1.0f no lag or off
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setLag(const float lag)
	{
		this->lag = lag;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setMaxTilt
		Purpose 	: set the maximum tilt (radians each way)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setMaxTilt(const float maxTilt)
	{
		this->maxTilt = maxTilt;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setMaxTiltDecay
		Purpose 	: set the decay factor to reduce the max tilt by per second
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setMaxTiltDecay(const float maxTiltDecay)
	{
		this->maxTiltDecay = maxTiltDecay;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setTiltPhase
		Purpose 	: set the tilt phase of the icon
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setTiltPhase(const float phase)
	{
		this->tiltPhase=phase;
		this->tiltCounter=(phase/TWOPI)*ICON_TILT_PERIOD;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::update
		Purpose 	: update the icon
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::init
		Purpose 	: initialise the icon
		Parameters 	: (opt/NULL=placeholder)texture name, (opt/NULL=placeholder)name for icon
		Returns 	: 
		Info 		: this overrides PAGEITEM::init// PP: I wish Tech Team would use const now & again!
	*/
	void init(char* texName=NULL, const char* const itemName=NULL);
	

	/*	--------------------------------------------------------------------------------
		Function 	: ICON::TAG_ICON
		Purpose 	: [default] constructor
		Parameters 	: (opt/NULL=placeholder)texture name, (opt/NULL=placeholder)name for icon
		Returns 	: 
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_ICON(char* texName=NULL, const char* const itemName=NULL)
	{
		// TP: this->texName=NULL;// PP: 
		this->texNameCRC = 0;
		this->init(texName, itemName);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::getRequiredHeight
		Purpose 	: get the minimum required height for an ICON
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	inline float getRequiredHeight(void)
	{
		return (ICON_HEIGHT*this->getAlignScale());
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::getRequiredWidth
		Purpose 	: get the minimum required width for an ICON
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	inline float getRequiredWidth(void)
	{
		return (ICON_WIDTH*this->getAlignScale()/**(videoMode.yAspect/(float)videoMode.xAspect)*/);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setTexture
		Purpose 	: set the texture for the icon
		Parameters 	: 
		Returns 	: name of new texture for the icon
		Info 		: 
	*/
	bool setTexture(char* texName)
	{
		TBTexture*		rtn;

		if (texName)
		{
			// TP: this->texName = new char[256];
			this->texNameCRC = bkCRC32((uchar*)texName, strlen(texName),0);
			// TP: strcpy(this->texName, texName);
		}
		else
		{
			// TP: SAFE_FREE(this->texName);
		}

		rtn=bkLoadTexture(NULL, texName, 0);

		if(rtn == NULL)
		{
			return false;
		}
		else
		{
			this->tex=rtn;
			return true;
		}
	}

	/*	--------------------------------------------------------------------------------
		Function 	: ICON::setSelectable
		Purpose 	: enable/disable selectability for individual icons
		Parameters 	: true to enable / false to disable, (opt/false)selectable by mouse only (if parm1==true)
		Returns 	: this
		Info 		: 
	*/
	TAG_ICON* setSelectable(const bool on, const bool mouseOnly = false);


	/*	--------------------------------------------------------------------------------
		Function 	: ICON::shutdown
		Purpose 	: shutdown; called from destructors
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::shutdown, to free up name buffer
	*/
	void shutdown(void)
	{
		// PP: free up name buffer if it's allocated
		// TP: SAFE_DELETE(this->texName);
		this->texNameCRC = 0;
	}


}ICON, _ICON BALIGN16;


/*	--------------------------------------------------------------------------------
	Function 	: Icon_update
	Purpose 	: per-frame update of normal icons
	Parameters 	: 
	Returns 	: 
	Info 		: should also be called if you're using any ICONCHANGE instances
*/
//void Icon_update(void);



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													ICONCHANGE page item type

				A kinda groovy way of toggling between two options, each represented by a circular icon type.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* PP: 

TODO: (?)

This effect is crying out for cartoony sound effects to go with the animation (played quite quietly cos the icons are only wee)

ICSTATE_MOVEBACK stage:		think Roadrunner-style huge rubber band being stretched, you know the one
ICSTATE_WHEELSPIN stage:	that mad slide-whistle-and-bongo-solo "spinning out of control" noise
ICSTATE_CHARGE stage:		this animation stage is probably too short, but if not, a bullet ricochet would be cool
ICSTATE_KNOCKED stage:		some meaty bash/clunk, or maybe the old bass-drum-and-crash-cymbal "falling on one's ass" noise, or maybe a ten pin bowling strike?  hmm

Suitable pad vibrations would be cool for when this effect is used to toggle vibration in the front end

*/

#define ICONCHANGE_DURATION_WHEELSPIN	0.15f								// PP: duration of the wheelspin stage(seconds)
#define ICONCHANGE_DURATION_CHARGE		0.15f								// PP: duration of the charge stage(seconds)
#define ICONCHANGE_DURATION_KNOCKED		0.15f								// PP: duration of the knocked stage(seconds)

#define ICONCHANGE_INITIAL_OFFSET		0.134f// PP: 60.0f// PP: was 100.0f				// PP: X offset of inactive icon

// PP: note that these two aren't framerate-independent
// PP: also note that I DON'T CARE - re-arranging s=ut+½at² in various perverse ways is too much effort for my fragile brain...
// PP: TODO: NAH, NAH it's a v=at thing, do it properly!

#define ICONCHANGE_MOVEBACK_U			0.134f// PP: 6.0f								// PP: initial speed of new icon in moveback stage
#define ICONCHANGE_MOVEBACK_DECEL		0.0002f// PP: 0.95f								// PP: portion of speed retained per frame in moveback stage

#define ICONCHANGE_WHEELSPIN_TURNS		1.0f								// PP: number of z turns in wheelspin stage (angle at start of this stage is accounted for elsewhere)
#define ICONCHANGE_CHARGE_TURNS			1.0f								// PP: number of z turns in the charge stage
#define ICONCHANGE_SPINOUT_TURNS		1.0f								// PP: number of y turns in the knocked stage

#define ICONCHANGE_RADS_PER_DIST_UNIT	0.018f								// PP: number of radians an icon turns as it rolls 1 distance unit

// PP: these alpha values are combined with the ICONCHANGE's PAGEITEM::opacity...
#define ICONCHANGE_INACTIVE_ALPHA		0.4f									// PP: alpha value for inactive icons
#define ICONCHANGE_ACTIVE_ALPHA			1.0f							// PP: alpha value for active icons

#define ICONCHANGE_MIN_MOVEBACK_SPEED	0.0004f// PP: 0.2f								// PP: speed below which the moveback stage is terminated


typedef void(*ICSetFunc)(const int state);								// PP: type for callbacks called when the masterState of an ICONCHANGE is set with ICONCHANGE::set


// PP: behaviour flags...

#define BITS_FOR_ICFLAGS		5										// PP: number of bits required to hold any of the flags below

enum
{
	ICFLAG_NONE			=	0,											// PP: no flags

	ICFLAG_SOUNDS		=	(1<<0),										// PP: the icon change is accompanied by a standard set of sounds
	ICFLAG_CAMSHAKE		=	(1<<1),										// PP: the icon change causes the camera to shake a little
	ICFLAG_PADSHAKE1	=	(1<<2),										// PP: the icon change causes the pad to shake when switching to icon 1
	ICFLAG_PADSHAKE2	=	(1<<3),										// PP: the icon change causes the pad to shake when switching to icon 2
	ICFLAG_PADCONTROL	=	(1<<4),

	ICFLAG_THEWORKS		=	(1<<BITS_FOR_ICFLAGS)-1						// PP: the icon does everything it can do
};

									
#define ICONCHANGE_TEXTURE_NAME_LENGTH	13								// PP: length of texture names used for ICONCHANGE

// PP: struct for icon-change operations
typedef struct TAG_ICONCHANGE	:	public PAGEITEM
{
private:

	// PP: the stages of the icon change operation
	typedef enum _ICState
	{
		ICSTATE_NONE				=0,									// PP: no icon change operation in process
		ICSTATE_MOVEBACK,												// PP: new icon rolls out from behind old icon
		ICSTATE_WHEELSPIN,												// PP: new icon wheelspins ready to charge at old icon
		ICSTATE_CHARGE,													// PP: new icon charges towards old icon
		ICSTATE_POSTCHARGE,												// PP: new icon has come to rest in the active position

		ICSTATE_KNOCKED,												// PP: old icon is knocked away by new icon, spins on Y axis and comes to rest behind new icon
		ICSTATE_POSTKNOCKED,											// PP: old icon has come to rest in the inactive position
		
		NUM_ICSTATES													// PP: number of icon change states

	}ICState;

	// PP:					~~~~~~~~~~~~~~~~

	unsigned int	flags				:BITS_FOR_ICFLAGS;				// PP: behaviour flags

	ICState			state1;												// PP: state of new icon
	ICState			state2;												// PP: state of old icon

	float			counter1;											// PP: counter for new icon
	float			counter2;											// PP: counter for old icon

	//char			tex1[ICONCHANGE_TEXTURE_NAME_LENGTH];				// PP: texture name for one of the icons
	//char			tex2[ICONCHANGE_TEXTURE_NAME_LENGTH];				// PP: texture name for the other icon

	ICON			icons[2];											// PP: the two icons that can be toggled between
	ICON*			activeIcon;
	ICON*			inactiveIcon;
	ICON*			backmostIcon;
	ICON*			foremostIcon;

	// PP: MOVEBACK stage variables relating to new icon...

	float			moveBackSpeed;										// PP: current speed								
	float			moveBackOffset;										// PP: current x offset added by the moveback stage
	float			moveBackAng;										// PP: current angle

	// PP: CHARGE stage variables relating to new icon...

//	float			chargePos;											// PP: true x pos: used to detect collision

	unsigned int	masterState			:1;								// PP: which icon is active (0=tex1/1=tex2)

	ICSetFunc		setFunc;											// PP: pointers to relative functions called when the ICONCHANGE toggles to master state 0 or 1

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::mainDraw
		Purpose 	: the core of the draw method - actually draw the icon-changer
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual inline void mainDraw(void)
	{
		this->backmostIcon->draw();
		this->foremostIcon->draw();
	}


public:

	static int		counter;											// PP: shared counter variable for all icon change instances
	int				busy				:1;								// PP: is this icon change in progress?


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::getRequiredHeight
		Purpose 	: get the minimum required height for an ICONCHANGE
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	float getRequiredHeight(void)
	{
		return this->getAlignScale()*ICON_HEIGHT;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::getRequiredWidth
		Purpose 	: get the minimum required width for an ICONCHANGE
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	float getRequiredWidth(void)
	{
		return this->getAlignScale()*(ICON_WIDTH + (ICONCHANGE_INITIAL_OFFSET * 2.0f));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::setRect
		Purpose 	: set the icon changer's rectangle
		Parameters 	: new rectangle
		Returns 	: 
		Info 		: this overrides PAGEITEM::setRect.  It's needed so the rectangles
						of the component ICONS can be updated at the same time.
	*/
	// PP: REMOUT: individual icons' rects should relative to the ICONCHANGE midpoint
/*	 void setRect(const RECTANGLE& rectIn)
	 {
		 PAGEITEM::setRect(rectIn);

		 this->icons[0].setRect(this->rect);
		 this->icons[1].setRect(this->rect);
	 }*/


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::init
		Purpose 	: initialise the icon change struct
		Parameters 	: texture 1 name, texture 2 name,
						(opt/NULL)set function, (opt/NULL=default)name, (opt/ICFLAG_NONE) behaviour flags,
						(opt/0) master state ie. which icon is active
		Returns 	: 
		Info 		: 
	*/
	void init(char* tex1in, char* tex2in, ICSetFunc const func=NULL, const char* const nameIn=NULL, const unsigned int flagsIn=ICFLAG_PADCONTROL, const unsigned int masterStateIn=0);


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::TAG_ICONCHANGE
		Purpose 	: default constructor; does bugger all
		Parameters 	: 
		Returns 	: new ICONCHANGE instance
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_ICONCHANGE(void)
	{
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::TAG_ICONCHANGE
		Purpose 	: constructor; calls init
		Parameters 	: texture 1 name, texture 2 name,
						(opt/NULL)set function, (opt/NULL=default) name, (opt/ICFLAG_NONE) behaviour flags,
		Returns 	: new ICONCHANGE instance
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_ICONCHANGE(char* tex1in, char* tex2in, ICSetFunc const func=NULL, const char* nameIn=NULL, const unsigned int flagsIn=ICFLAG_PADCONTROL, const unsigned int masterStateIn=0)
	{
		this->init(tex1in, tex2in, func, nameIn, flagsIn, masterStateIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::update
		Purpose 	: update the icon change
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::set
		Purpose 	: set the master state of the icon change
		Parameters 	: (opt/false)flag to specify immediate change
		Returns 	: 
		Info 		: This sets masterState, not state.
	*/
	void set(const unsigned int stateIn, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: ICONCHANGE::toggle
		Purpose 	: toggle the master state of the icon change
		Parameters 	: (opt/false)flag to specify immediate change
		Returns 	: 
		Info 		: 
	*/
	void toggle(const bool instant=false);


}ICONCHANGE, _ICONCHANGE BALIGN16;


#endif // PP: ndef __H_ICON