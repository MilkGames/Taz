// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : MonitorFX.h
//   Purpose : Computer monitor effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_MONITORFX
#define __H_MONITORFX


#include "main.h"		// PP: for gameStatus
#include "TextFX.h"		// PP: COLOUR, RECTANGLE etc.

#define NUM_WINDOWS_COLOURS				27		// PP: number of Windows colours (excluding black which is no use)

// PP: the standard Windows colours, for complete authenticity...
extern unsigned char WindowsColours[NUM_WINDOWS_COLOURS][3];

#define FIRST_PRETTY_WINDOWS_COLOUR		13		// PP: colours from here on are nice & lively

// PP: fill in a random Windows colour
#define MFX_RANDOM_WINDOWS_COLOUR(_r, _g, _b) \
		{ \
			int _index=bmRand()%NUM_WINDOWS_COLOURS; \
			(_r)=WindowsColours[_index][0]; \
			(_g)=WindowsColours[_index][1]; \
			(_b)=WindowsColours[_index][2]; \
		}

// PP: fill in a random Windows colour from the prettier side of the palette
#define MFX_RANDOM_PRETTY_WINDOWS_COLOUR(_r, _g, _b) \
		{ \
			int _index=FIRST_PRETTY_WINDOWS_COLOUR+(bmRand()%(NUM_WINDOWS_COLOURS-FIRST_PRETTY_WINDOWS_COLOUR)); \
			(_r)=WindowsColours[_index][0]; \
			(_g)=WindowsColours[_index][1]; \
			(_b)=WindowsColours[_index][2]; \
		}

// PP: fill in a random Windows colour from the prettier side of the palette, into a COLOUR struct
#define RANDOM_PRETTY_WINDOWS_COLOUR(_col)		MFX_RANDOM_PRETTY_WINDOWS_COLOUR((_col).r, (_col).g, (_col).b)


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												MONITOREFFECTINFO SPECIAL-TYPE DATA

											Data pointed to by monitor-type objects.
										Not to be confused with MONITORINFO in WinUser.h!
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_MONITOREFFECTINFO
{
	struct TAG_MONITOREFFECT*		effect;			// PP: ptr to a monitor effect sub-class instance

} MONITOREFFECTINFO, _MONITOREFFECTINFO BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													MONITOR EFFECT BASE CLASS

										All monitor effects should inherit from this class
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_MONITOREFFECT
{
protected:

	float				lastUpdateTime;		// PP: game time at which the effect was last updated
	TBMatrix			objectMatrix;		// PP: object matrix in effect when drawing the effect - this represents the size, position and angle of the monitor screen


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::drawBackground
	Purpose 	: draw a monitor effect background
	Parameters 	: background r, g, b, width and height
	Returns 	: 
	Info 		: 
*/
void drawBackground(const unsigned char r, const unsigned char g, const unsigned char b,
								   const float width, const float height);


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::startDraw
	Purpose 	: prepare to draw a normal monitor effect
	Parameters 	: 
	Returns 	: true if the draw should proceed; else false
	Info 		: 
*/
bool startDraw(void);


/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::timeToUpdate
	Purpose 	: determine whether or not it is time to update a monitor effect
	Parameters 	: ref to variable to receive time since last update
	Returns 	: whether or not it is time to update a monitor effect
	Info 		: 
*/
bool timeToUpdate(float& timeSinceLastUpdate);


public:	

	VEC					effectPos;			// PP: world space position
	bool				visible;			// PP: is the thing visible?


	/*	--------------------------------------------------------------------------------
	Function 	: MONITOREFFECT::updateVisibilityStatus
	Purpose 	: update the visibility status of the effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
	*/
	void updateVisibilityStatus(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MONITOREFFECT::TAG_MONITOREFFECT
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MONITOREFFECT instance
		Info 		: 
	*/
	inline TAG_MONITOREFFECT(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: MONITOREFFECT::TAG_MONITOREFFECT
		Purpose 	: constructor
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: new MONITOREFFECT instance
		Info 		: 
	*/
	inline TAG_MONITOREFFECT(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale)
	{
		this->init(x, y, z, ax, ay, az, scale);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MONITOREFFECT::init
		Purpose 	: initialise a monitor effect
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale)			{}


	/*	--------------------------------------------------------------------------------
		Function 	: MONITOREFFECT::draw
		Purpose 	: draw a monitor effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void draw(void)			{}
	

	/*	--------------------------------------------------------------------------------
		Function 	: MONITOREFFECT::update
		Purpose 	: update a monitor effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void update(void)		{}


}MONITOREFFECT, _MONITOREFFECT BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														3D FLYING OBJECTS

								Pastiche of the Windows wavey-in-the-wind 3D logo screensaver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define FLYINGOBJECTS_NUM_SECTIONS		10									// PP: number of rectangular sections making up the flag
#define FLYINGOBJECTS_NUM_POLES			(FLYINGOBJECTS_NUM_SECTIONS+1)		// PP: number of 'poles' I can't explain it too tired


typedef struct TAG_MFXFLYINGOBJECTS		:	public MONITOREFFECT
{
	TBVector2D			pos;			// PP: position of top-left corner of the flag
	TBVector			velocity;		// PP: overall x-y velocity of the flag

	// PP: array of 'poles'
	struct
	{
		unsigned char	intensity;		// PP: intensity at this pole
		TBVector2D		offset;			// PP: offset from where the pole 'should' be

	}poles[FLYINGOBJECTS_NUM_POLES];

	static TBTexture*	texture;		// PP: texture used for the flying objects


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::sceneInit
		Purpose 	: initialise stuff for flying objects at the start of the scene
		Parameters 	: 
		Returns 	: 
		Info 		: gets a ptr to the static texture
	*/
	static void sceneInit(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::init
		Purpose 	: initialise a Flying Objects screen saver
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::TAG_MFXFLYINGOBJECTS
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXFLYINGOBJECTS instance
		Info 		: 
	*/
	inline TAG_MFXFLYINGOBJECTS(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::TAG_MFXFLYINGOBJECTS
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXFLYINGOBJECTS instance
		Info 		: 
	*/
	inline TAG_MFXFLYINGOBJECTS(const float x, const float y, const float z,
							const float ax, const float ay, const float az, const float scale)
	{
		this->init(x, y, z, ax, ay, az, scale);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::draw
		Purpose 	: draw a Flying Objects screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);
	

	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGOBJECTS::update
		Purpose 	: update a Flying Objects screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}MFXFLYINGOBJECTS, _MFXFLYINGOBJECTS BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FLYING WINDOWS

										Pastiche of the Windows logo-starfield screen saver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define FLYINGWINDOWS_NUM_WINDOWS		25					// PP: number of particles on screen at any time - is 25 on the steep side?

typedef struct TAG_MFXFLYINGWINDOWS	:	MONITOREFFECT
{
	// PP: array of particles
	struct
	{
		TBVector			pos;							// PP: pseudo-3D position of particle
		TBVector2D			screenPos;						// PP: x-y screen position of particle
		TBVector2D			size;							// PP: height & width of particle, determined only by position
		unsigned char		r;								// PP: particle colour
		unsigned char		g;								// PP: particle colour	
		unsigned char		b;								// PP: particle colour

	}windows[FLYINGWINDOWS_NUM_WINDOWS];

	static TBTexture*		texture;						// PP: texture used for each star


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::sceneInit
		Purpose 	: initialise stuff for flying windows at the start of the scene
		Parameters 	: 
		Returns 	: 
		Info 		: gets a ptr to the static texture
	*/
	static void sceneInit(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::init
		Purpose 	: initialise a Flying Windows screen saver
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::TAG_MFXFLYINGWINDOWS
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXFLYINGWINDOWS instance
		Info 		: 
	*/
	inline TAG_MFXFLYINGWINDOWS(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::TAG_MFXFLYINGWINDOWS
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXFLYINGWINDOWS instance
		Info 		: 
	*/
	inline TAG_MFXFLYINGWINDOWS(const float x, const float y, const float z,
							const float ax, const float ay, const float az, const float scale)
	{
		this->init(x, y, z, ax, ay, az, scale);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::draw
		Purpose 	: draw a Flying Windows screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);
	

	/*	--------------------------------------------------------------------------------
		Function 	: MFXFLYINGWINDOWS::update
		Purpose 	: update a Flying Windows screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}MFXFLYINGWINDOWS, _FLYINGWINDOWS BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													MYSTIFY YOUR MIND

						Near-replica of the Windows bouncing lines screen saver.  Oooh, mystifying!
					I'm so mystified I might have to rest my head for a while before commenting this bit.
												There we are, better now.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define MYSTIFY_NUM_ENTITIES		2						// PP: number of bouncy shape things
#define MYSTIFY_NUM_POLYS			5						// PP: number of polys in each entity (trail size)

typedef struct TAG_MFXMYSTIFY	:	MONITOREFFECT 
{
	float					timer;							// PP: timer for colour-changing purposes

	// PP: array of bouncy shape things
	struct
	{
		unsigned char		oldR;							// PP: old colour
		unsigned char		oldG;							// PP: old colour
		unsigned char		oldB;							// PP: old colour

		unsigned char		newR;							// PP: new colour
		unsigned char		newG;							// PP: new colour
		unsigned char		newB;							// PP: new colour

		// PP: array of quads for each bouncy shape thing
		struct
		{
			// PP: array of points for each of the bouncy shape thing's quads
			struct
			{
				TBVector2D	pos;							// PP: x-y position of point
				TBVector2D	velocity;						// PP: x-y velocity of point

			}points[4];
			
		}polys[MYSTIFY_NUM_POLYS];

	}entities[MYSTIFY_NUM_ENTITIES];


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMYSTIFY::init
		Purpose 	: initialise a Mystify screen saver
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMYSTIFY::TAG_MFXMYSTIFY
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXMYSTIFY instance
		Info 		: 
	*/
	inline TAG_MFXMYSTIFY(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMYSTIFY::TAG_MFXMYSTIFY
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXMYSTIFY instance
		Info 		: 
	*/
	inline TAG_MFXMYSTIFY(const float x, const float y, const float z,
							const float ax, const float ay, const float az, const float scale)
	{
		this->init(x, y, z, ax, ay, az, scale);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMYSTIFY::draw
		Purpose 	: draw a Mystify screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);
	

	/*	--------------------------------------------------------------------------------
		Function 	: MFXMYSTIFY::update
		Purpose 	: update a Mystify screen saver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}MFXMYSTIFY, _MFXMYSTIFY BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SCROLLING MARQUEE

										Replica of the Windows scrolling text screen saver.
						This would work better with the default Arial font, or some cartoony version of it
								Randomly chooses messages from the MARQUEE_ block of the game text.
										Be sure to update MARQUEE_NUM_MESSAGES if you add any.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define MARQUEE_TEXT_LENGTH				512					// PP: MAXIMUM LENGTH OF MARQUEE STRING but I believe it's all checked for overflowing

typedef struct TAG_MFXMARQUEE	:	MONITOREFFECT 
{
	ushort				text[MARQUEE_TEXT_LENGTH];			// PP: wide-character text message
	unsigned char		r;									// PP: text colour
	unsigned char		g;									// PP: text colour
	unsigned char		b;									// PP: text colour
	TBVector2D			textPos;							// PP: position of top-left corner of message
	float				speed;								// PP: scrolling speed for text
	float				length;								// PP: physical length of text
	float				height;								// PP: physical height of text
	float				textInvScale;						// PP: determines font size
	TBFontFormatting	format;								// PP: text formatting info


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMARQUEE::init
		Purpose 	: initialise a Scrolling Marquee screen saver
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMARQUEE::TAG_MFXMARQUEE
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXMARQUEE instance
		Info 		: 
	*/
	inline TAG_MFXMARQUEE(void)	{};


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMARQUEE::TAG_MFXMARQUEE
		Purpose 	: default constructor
		Parameters 	: 
		Returns 	: new MFXMARQUEE instance
		Info 		: 
	*/
	inline TAG_MFXMARQUEE(const float x, const float y, const float z,
							const float ax, const float ay, const float az, const float scale)
	{
		this->init(x, y, z, ax, ay, az, scale);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMARQUEE::draw
		Purpose 	: draw a Scrolling Marquee screensaver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXMARQUEE::update
		Purpose 	: update a Scrolling Marquee screensaver
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}MFXMARQUEE, _MFXMARQUEE BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														DIGITAL WATCH

								Taz's digital watch when he's in his Secret Agent costume.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_MFXWATCH		:	MONITOREFFECT
{

private:

	COLOUR					colour;						// PP: colour of 7-segment displays
	unsigned char			digits[4];					// PP: digits displayed
	bool					colonFlash;					// PP: is the colon between the hours and minutes visible?
	bool					active;						// PP: is the display active (should it be updated and drawn?)

	static TBTexture*		texture;					// PP: texture used for the watch digits

public:

	// PP: this may well need to go into the superclass at some point
	TBActorInstance*		actorInstance;				// PP: the watch model upon which the watch display effect is drawn
	
	int						handle;						// PP: Taz's attached-object handle for watch model upon which the watch display effect is drawn

	TBVector				pos;						// PP: actual world position of the watch - hurrah!
	

	/*	--------------------------------------------------------------------------------
		Function 	: MFXWATCH::init
		Purpose 	: initialise a digital watch display
		Parameters 	: x,y,z pos; x,y,z ang; scale
		Returns 	: 
		Info 		: 
	*/
	void init(const float x, const float y, const float z,
						const float ax, const float ay, const float az, const float scale);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXWATCH::init
		Purpose 	: initialise a digital watch display
		Parameters 	: top-level actor instance to which the display is attached, attached object handle
		Returns 	: 
		Info 		: 
	*/
	void init(ACTORINSTANCE* const actorIn, const int handleIn);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXWATCH::draw
		Purpose 	: draw a digital watch display
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXWATCH::update
		Purpose 	: update a digital watch display
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: MFXWATCH::shutdown
		Purpose 	: shutdown a digital watch display
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void shutdown(void);


}MFXWATCH, _MFXWATCH BALIGN16;

extern MFXWATCH		MonitorFX_watch;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* --------------------------------------------------------------------------------
   Function : CreateMonitorObject
   Purpose : creates an object with a monitor effect on it
   Parameters : ACTORINSTANCE for which to create the effect
   Returns : TRUE for success, FALSE for failure
   Info : 
*/
int CreateMonitorObject(struct TAG_ACTORINSTANCE* const actorInstance);


/* --------------------------------------------------------------------------------
   Function : CreateMonitorInfo
   Purpose : creates a monitor info struct
   Parameters : 
   Returns : ptr to new monitor info struct, or NULL
   Info : 
*/

MONITOREFFECTINFO* CreateMonitorInfo(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawInstanceList
	Purpose 	: draw all monitors in an instance list
	Parameters 	: actor instance list from which to draw monitors
	Returns 	: 
	Info 		: 
*/
void MonitorFX_drawInstanceList(struct TAG_ACTORINSTANCELIST* const actorInstanceList);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_update
	Purpose 	: update all monitors in an instance list
	Parameters 	: actor instance list in which to update monitors
	Returns 	: 
	Info 		: 
*/
void MonitorFX_update(struct TAG_ACTORINSTANCELIST* const actorInstanceList);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_update
	Purpose 	: update a single monitor
	Parameters 	: monitor actor instance to update
	Returns 	: 
	Info 		: 
*/
inline void MonitorFX_update(struct TAG_ACTORINSTANCE* const actorInstance)
{
	actorInstance->monitorInfo->effect->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_updateLevelMonitors
	Purpose 	: update all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_updateLevelMonitors(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawLevelMonitors
	Purpose 	: draw all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_drawLevelMonitors(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_addLevelMonitor
	Purpose 	: add a hardcoded monitor effect to the level
	Parameters 	: xyz pos, xy ang
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_addLevelMonitor(const float x, const float y, const float z, const float ax, const float ay);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_deleteLevelMonitors
	Purpose 	: delete all hardcoded monitor effects in the level
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: end of project bodge, forgive me
*/
void MonitorFX_deleteLevelMonitors(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_drawTest
	Purpose 	: draw all test monitor effects
	Parameters 	: 
	Returns 	: 
	Info 		: FOR TESTING ONLY
*/
void MonitorFX_drawTest(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_updateTest
	Purpose 	: update all monitor effects
	Parameters 	: 
	Returns 	: 
	Info 		: FOR TESTING ONLY
*/
void MonitorFX_updateTest(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_init
	Purpose 	: init stuff for monitor effects at the beginning of the scene
	Parameters 	: 
	Returns 	: 
	Info 		: must be called after the level package has finished loading
*/
void MonitorFX_init(void);


/*	--------------------------------------------------------------------------------
	Function 	: MonitorFX_shutdown
	Purpose 	: shutdown stuff for monitor effects
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void MonitorFX_shutdown(void);


#endif // PP: ndef __H_MONITORFX