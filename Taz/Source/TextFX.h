// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : TextFX.h
//   Purpose : Text effects, eg. button glyphs, coloured text
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_TEXTFX
#define __H_TEXTFX

#include "Main.h"			// PP: main stuff I suppose
#include "Text2.h"			// PP: game text enumeration if not already declared
#include "LinkedList.h"		// PP: linked list class from LUST, I swear by it
#include "maths.h"			// PP: maths stuff
#include "sfx.h"			// NH: Sound stuff (for beeps & clicks during navigation)


// PP: these 5 strings are to pacify that dirty rotten PS2 compiler.  WHY I OUGHDDA! ...
extern char*				DPadLeftTexture;				// PP: d-pad left texture name
extern char*				DPadRightTexture;				// PP: d-pad right texture name
extern char*				DPadUpTexture;					// PP: d-pad up texture name
extern char*				DPadDownTexture;				// PP: d-pad down texture name
extern char*				DPadTexture;					// PP: d-pad texture name

#define DPADPULSETIME		0.4f							// PP: interval between beginnings of d-pad glyph flashes

// PP: whether or not to show the highlighted versions of the dpad glyphs
#define DPADPULSE			(fmodf(gameStatus.appTime, DPADPULSETIME)>(DPADPULSETIME*0.5f))

#define	TEXTFX_DEFAULT_COLOUR1		COLOUR(128, 128, 0, 128)	// PP: standard colour for the top of vGrad text
#define	TEXTFX_DEFAULT_COLOUR2		COLOUR(110, 40, 0, 128)		// PP: standard colour for the bottom of vGrad text

// CMD: draw stages for text
enum ETextRenderStage
{
	TEXTFX_SCRIBBLE,										// CMD: draw scribble text effect
	TEXTFX_SCRIBBLETWO,										// CMD: second pass scribble
	TEXTFX_OUTLINE,											// CMD: draw black outline to text
	TEXTFX_INLINE,											// CMD: draw blck outline on inside of letters
	TEXTFX_OUTLINE_UPLEFT,									// CMD: draw black outline by 
	TEXTFX_OUTLINE_UPRIGHT,									// CMD: redrawing the text black
	TEXTFX_OUTLINE_DOWNLEFT,								// CMD: in four different positions
	TEXTFX_OUTLINE_DOWNRIGHT,
	TEXTFX_NORMAL,											// CMD: standard draw, should be drawn last
};

#define TEXTFX_SELECTOFFSET		(6)						// CMD: offset in pixels
enum ETextOffsetDirection
{
	TEXTFXOFFSET_UP,
	TEXTFXOFFSET_LEFT,
	TEXTFXOFFSET_DOWN,
	TEXTFXOFFSET_RIGHT,

	TEXTFX_NUM_OFFSETS// PP: Added this just so's TEXTFXOFFSET_RIGHT can ++ onto something legal before we loop it back to TEXTFXOFFSET_UP

}ENUMOPS(ETextOffsetDirection);// PP: declare operator overloads


// PP: ~~~~~~~~~~~~~~~ 'TEST_TEXT_FORMAT' - THIS IS A HANDY WAY OF TESTING BAGEL TEXT FORMATTING DATA ~~~~~~~~~~~~~~~

#define PLOT_MARKER(_format, _v, _h)	\
		{ \
			ushort		_string16[2]; \
			(_format).horzFormat = EBFONTFORMATTING_##_h; \
			(_format).vertFormat = EBFONTFORMATTING_##_v; \
			\
			bkString8to16(_string16, _buf); \
			\
			/* PP: print the text */ \
			bdPrintFont(standardFont,_string16,strlen(_buf),&(_format),0,0,0,127,NULL,NULL); \
		}

#define TEST_TEXT_FORMAT(_format) \
		{ \
			char	_buf[2]; \
			sprintf(_buf, "+"); \
			PLOT_MARKER(_format, TOP, LEFT); \
			PLOT_MARKER(_format, TOP, RIGHT); \
			PLOT_MARKER(_format, BOTTOM, LEFT); \
			PLOT_MARKER(_format, BOTTOM, RIGHT); \
			PLOT_MARKER(_format, CENTRE, CENTRE); \
		}

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#define TEXTFX_NAME_LENGTH		32			// PP: maximum length of names for pages/items etc.

#include "Colour.h"							// PP: handy colour struct
#include "Rectangle.h"						// PP: Handy rectangle class


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PAGEITEM

											Page items are held in rows on pages.
						This base class functionality can be inherited for use with pretty much anything:
				text boxes, memory card browsers, 3D models, bitmaps, icons, monitor effects, pages, books, whatever.

												There's no limit to the fun!™

												book > pages > rows > *ITEMS*

															@pageitem
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: possible modes for a page-item effect
enum PIEMode
{
	PIEMODE_ENTER,			// PP: effect is for a page-item coming into view
	PIEMODE_ON,				// PP: effect is for a page-item in view
	PIEMODE_EXIT,			// PP: effect is for a page-item going out of view

	NUM_PIE_MODES,			// PP: number of PAGEITEMEFFECT modes

	PIEMODE_ALL				// PP: all PAGEITEMEFFECT modes
};


enum PageItemFlag
{
	PIFLAG_NONE						=0,			// PP: no flag
	PIFLAG_PAD_X					=1,			// PP: has horizontal padding
	PIFLAG_PAD_Y					=1<<1,		// PP: has vertical padding
	PIFLAG_XALIGN_LEFT				=1<<2,		// PP: aligns to left of padding rectangle
	PIFLAG_XALIGN_CENTRE			=1<<3,		// PP: aligns to horizontal centre of padding rectangle
	PIFLAG_XALIGN_RIGHT				=1<<4,		// PP: aligns to right of padding rectangle
	PIFLAG_YALIGN_BOTTOM			=1<<5,		// PP: aligns to bottom of padding rectangle
	PIFLAG_YALIGN_CENTRE			=1<<6,		// PP: aligns to vertical centre of padding rectangle
	PIFLAG_YALIGN_TOP				=1<<7,		// PP: aligns to top of padding rectangle
	PIFLAG_SELECTABLE				=1<<10,		// PP: page item can be selected
	PIFLAG_MOUSEONLY				=1<<11,		// PP: if used in combination with PIFLAG_SELECTABLE, page item can be selected but only by mouse
	PIFLAG_MAINTAINASPECTRATIO		=1<<12,// PP: not used yet		// PP: (applying only to free items) specifies that the aspect ratio of the item should be kept 1:1 rather than stretching it to match that of the top-level book that holds it.  (Aspect ratios are always kept 1:1 for aligning items)
	PIFLAG_STRETCHTOFILL			=1<<13,// PP: not used yet		// PP: item stretches to fill all the space available to it after alignment
	PIFLAG_DISABLED					=1<<14,		// NH: item is disabled and has a suitable effect applied to it
	PIFLAG_ALIGNED					=1<<15,		// PP: the item has been aligned.  There's no need to calculate its alignment again.
	PIFLAG_WIDTHCALCULATED			=1<<16,		// PP: the item's width has already been calculated; no need to recalculate it
	PIFLAG_HEIGHTCALCULATED			=1<<17,		// PP: the item's height has already been calculated; no need to recalculate it
	PIFLAG_IGNOREINPUT				=1<<18,		// PP: the item is to ignore all pad input (eg. a slider will lock its knob [sounds painful])
};


#define PAGEITEM_XALIGN_FLAGS		(PIFLAG_XALIGN_LEFT|PIFLAG_XALIGN_CENTRE|PIFLAG_XALIGN_RIGHT)
#define PAGEITEM_YALIGN_FLAGS		(PIFLAG_YALIGN_BOTTOM|PIFLAG_YALIGN_CENTRE|PIFLAG_YALIGN_TOP)

// PP: default flags for page items
// PP: note that the selectable flag is default - but the item will only truly be selectable if the page that contains it is navigable (call PAGE::setNavigation)
#define PAGEITEM_DEFAULT_FLAGS		(PIFLAG_PAD_X|PIFLAG_PAD_Y|PIFLAG_XALIGN_CENTRE|PIFLAG_YALIGN_CENTRE|PIFLAG_SELECTABLE|PIFLAG_MAINTAINASPECTRATIO)

typedef void (*PageItemActivateFunc)(void);

// PP: ~~~~~~~~~~~~~~~

// PP: types of input that trigger page navigation
enum EPageNavInput
{
	PAGENAVINPUT_DEBOUNCE,									// PP: input from a debounced input channel
	PAGENAVINPUT_AUTOREPEAT									// PP: input from an auto-repeat input channel
};

// PP: structure used to pass data into and out of PAGEITEM::getNavigationCandidate
struct NAVCANDIDATE_INFO
{
	// PP: IN...
	TBVector2D				currentPos;	 					// PP: position from which we're navigating
	struct TAG_PAGEITEM*	currentItem;					// PP: item from which we're navigating
	float					navAng;							// PP: angle of navigation
	EPageNavInput			inputType;						// PP: input type that triggered the navigation (PAGENAVINPUT_...)
	bool					recur;							// PP: check recursively into sub-pages etc.
	bool					wrapIn;							// PP: currentPos is a fake, wrapped, pos							
	// PP: OUT...																		
	bool					wrapOut;						// PP: the candidate returned was the result of a navigation wrap within the page

}BALIGN16;

// PP: ~~~~~~~~~~~~~~~

typedef struct TAG_PAGEITEM
{

friend struct				TAG_PAGEROW;					// PP: there's no good reason to hinder communication between rows and their items!
friend struct				TAG_PAGE;						// PP: there's no good reason to hinder communication between pages and their free items!
friend struct				TAG_TABLE;						// PP: subclass of PAGE	// PP: "Yeah I'm a mate of Steve's?"
															
friend struct				PAGEITEMEFFECT;					// PP: Page-item effects really need full access to the items that they work on - data that I would not want the client getting his filthy mits on
friend struct				PIE_FADE;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_SLIDE;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_ZOOM;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_FLASH;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_SPIN;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_ICONSHATTER;				// PP: subclass of PAGEITEMEFFECT
friend struct				ORT_PIE;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_SQUASHY;					// PP: subclass of ORT_PIE
friend struct				TEXT_PIE;						// PP: subclass of PAGEITEMEFFECT
friend struct				PIE_TEXTLIFE;					// PP: subclass of TEXT_PIE - jees is there no nicer way of doing this?
friend struct				PIE_TEXTEXPLODE;				// PP: subclass of TEXT_PIE

friend struct				PIECHOOSER;						// PP: pageitem effect choosers also need access to things that I wouldn't want the client using
friend struct				MAINPIECHOOSER;					// PP: subclass of PIECHOOSER
friend struct				MEMCARDPIECHOOSER;				// TP: Mmmm slidey
friend struct				WANTEDPIECHOOSER;				// TP: Mmmm slidey
friend struct				SPINBOOKPIECHOOSER;				// PP: subclass of PIECHOOSER// PP: Here goes another half-hour build...
friend struct				RACEPIECHOOSER;					// PP: subclass of PIECHOOSER

public:

	// PP: these public members are but at the beginning of the struct for watchability

	char					name[TEXTFX_NAME_LENGTH];		// PP: client-supplied name of item
	uint32					value;							// PP: client-supplied value, for whatever purpose; defaults to -1
															// PP: NOTE that casting a pageitem to an integer will yield its 'value'

private:													
															
	float					drawScale;						// PP: scale used when drawing the item, eg. 2.0f=double size
	float					alignScale;						// PP: scale used when reporting required width & height of the item

	// PP: These two are currently only used by PIECHOOSERs
	char					rowIndex;						// PP: index of row on which this item sits (-1 if its a free item)
	char					columnIndex;					// PP: this item's zero-based position in its row (numbered from left to right; -1 if it's a free item)

	struct TAG_PAGE*		container;						// PP: The page containing this item, or NULL if none


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::drawWithoutEffect
		Purpose 	: draw the page item, ignoring any effect parasite it might have on it
		Parameters 	: 
		Returns 	: 
		Info 		: this method is used by some parasites to draw the item in the normal way
						(eg. PIE_FADE, which simply works by talking to the item's opacity member
						rather than actually changing the way the item draws itself)
	*/
	void drawWithoutEffect(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getStringIndex
		Purpose 	: get the string index held by this item (if it does hold a string)
		Parameters 	: 
		Returns 	: NOSTRING, but in relevant subclasses, the EStringIndex held by the item
		Info 		: 
	*/
	virtual EStringIndex getStringIndex(void);


protected:

	// PP: NOTE: we don't use backgrounds and borders very extensively in Taz, so they're pretty basic at the moment
	// PP: these are protected rather than private because some subclasses (eg. BOOK) draw their own backgrounds & borders in their own way
	COLOUR					background;						// TP: background if the alpha portion is 0(default) then no attempt to draw background is made
	COLOUR					border;							// TP: border colour, if alpha is 0(default) then no attempt to draw border is made

	bool					exited;							// PP: an EXIT-mode effect on the item has finished; the item is therefore not to be drawn or updated.// PP: TODO: gotter make it so's ye can 'open' and 'close' items

	PAGEITEMEFFECT*			presetEffects[NUM_PIE_MODES];	// PP: ptrs to pre-initialised PAGEITEMEFFECTS for each of the effect modes
															// PP: - these will be memcp'ied into 'this->effect' as they are needed
															// PP: These allow the normal PAGEITEMEFFECT choosing to be overriden by the client on a per-item, per-mode basis

	RECTANGLE				optRect;						// PP: rectangle that snugly fits the item, and gets positioned within 'rect' according to alignment flags
	RECTANGLE				rect;							// PP: full area that the item has to play with
	RECTANGLE				freeRect;						// PP: rectangle specifying what portion of the item's page the item occupies.  This stays the same no matter how the item's page is reshaped, while obviously the item's 'rect' and 'optRect' will be changing.
															
// NH: Putting this public (Talk to me about Records and icon selection if you wanna find out why). Let me know if you want it done in a different way! 
	//bool					highlighted;					// PP: highlighted for whatever reason
															
	PageItemActivateFunc	onActivate;						// PP: function called when the item is activated
															// PP: eg. file browser - brings up 'are you sure' page
	float					opacity;						// PP: the opacity level of the item and its contents
															
	int						numInstances;					// PP: number of different times this item is included on a page - keeping a record of this allows pages to 
															// PP: delete the item only when no other pages are using it.
																														
	TBMatrix				objectMatrixCopy;				// PP: copy of the object matrix in effect before the item draws
															
	PAGEITEMEFFECT*			effect;							// PP: ptr to the item's current effect parasite, or NULL for none
	uchar					effectGroup;// PP: not used yet	// PP: index of the item's effect 'group'
															// PP: for example, when creating a page, you might set your header to effect group 0, body text to effect group 1, and footer text to effect group 2
															// PP: TODO: have effectGroups set automatically via TEXTBOX::setStyle?

	const PIECHOOSER*		effectChooser;					// PP: ptr to effect chooser to use for this item's effects// PP: NOTE: this will usually just be a pointer to the item's container's chooser

	RECTANGLE				boxRect;						// PP: rectangle drawn as the item's background and/or border
	uchar					tightBoxX		:1;				// PP: background & border fit tightly to the width of the item
	uchar					tightBoxY		:1;				// PP: background & border fit tightly to the height of the item

	float					requiredWidth;					// PP: precalculated minimum required width of the item
	float					requiredHeight;					// PP: precalculated minimum required height of the item


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::init
		Purpose 	: initialise the item
		Parameters 	: (opt/NULL=default) name
		Returns 	: 
		Info 		: 
	*/
	void init(const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::startDraw
		Purpose 	: set up conditions needed to draw the page item (eg. apply item's scale & position to object matrix)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void startDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::mainDraw
		Purpose 	: the core of the draw method - actually draw the page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::mainDrawDebug
		Purpose 	: the core of the debug-draw method - actually debug-draw the page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void mainDrawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::endDraw
		Purpose 	: do whatever needs to be done after drawing the page item (eg. de-apply item's scale & position from object matrix)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual inline void endDraw(void)
	{
		bdSetObjectMatrix(this->objectMatrixCopy);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setRect
		Purpose 	: set the item's rectangle
		Parameters 	: new rectangle
		Returns 	: this
		Info 		: this sorts out optRect as well as rect - all PAGEITEM::setRect overrides should call this first
	*/
	 virtual struct TAG_PAGEITEM* setRect(const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setLanguage
		Purpose 	: set the language of the item
		Parameters 	: new language of the item (BLANGUAGEID_...)
		Returns 	: 
		Info 		: 
	*/
	virtual void setLanguage(const EBLanguageID languageIn)
	{
		// PP: Nothing to do here: not all page items denote language.
		// PP: However, sub-classes such as BOOK, PAGE and TEXTBOX overload this in a useful way.
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this item
		Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
		Returns 	: 
		Info 		: 
	*/
	virtual void chooseEffect(const PIEMode mode, const bool recur);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this item,
						with no change of mode
		Parameters 	: 
		Returns 	: 
		Info 		: this will be called eg. when an item's selection status gets changed
	*/
	virtual void chooseEffect(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::drawBackgroundAndBorder
		Purpose 	: draw the page item's background and border if it has them
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void drawBackgroundAndBorder(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::updateBox
		Purpose 	: update the box to be used to draw the page item's background and/or border
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: to be called by setRect and setBoxStyle
	*/
	virtual void updateBox(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::markForRealignment
		Purpose 	: tell the pageitem that its alignment is out of date and will need
						to be updated in next align()
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this gets recurred down the contents hierarchy
	*/
	virtual void markForRealignment(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::align
		Purpose 	: calculate alignment for the page item and it's contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this gets recurred down the contents hierarchy
	*/
	virtual void align(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::effectsFinished
		Purpose 	: determine if all effects on the item and its children have finished
		Parameters 	: 
		Returns 	: true if all effects on the item and its children have finished, else false
		Info 		: 
	*/
	virtual bool effectsFinished(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::hasEffects
		Purpose 	: determine if the item or any of its contents have effects on them
		Parameters 	: 
		Returns 	: true if the item or any of its contents has an effect on it
		Info 		: 
	*/
	virtual bool hasEffects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::open
		Purpose 	: 'open' the page item; make it visible
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
// PP: TODO: 	virtual void open(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::open
		Purpose 	: 'open' the page item; make it visible
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
// PP: TODO: 	virtual void close(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::freeTempMemory
		Purpose 	: free-up temporary memory used by the item and its contents,
						eg. effects that have finished
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void freeTempMemory(void);


public:

	// PP: MOVED HERE AS A MILESTONE SHORTCUT// PP: TODO: RECONSIDER THIS
	bool					selected;					// PP: has input focus
	// NH: Taken from private (Talk to me about Records and icon selection if you wanna find out why).  May be a better way of doing it.  What do you recon.
	bool					highlighted;				// PP: highlighted for whatever reason

	RECTANGLE				screenRect;					// PP: item's final on-screen rectangle - gets updated when the item gets drawn
	int						pageItemFlags;				// PP: value of flags from the PAGEITEM base class.  The name is because sub-classes might need their own 'flags' member: this way I don't have to dream up new name variations each time they do, see?
	

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::startEffect
		Purpose 	: start an effect on the item
		Parameters 	: ptr to PAGEITEMEFFECT to start
		Returns 	: 
		Info 		: not to be confused with public 'setEffect' which is used to set the item's preset effects
	*/
	void startEffect(PAGEITEMEFFECT* const effectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setFreeRect
		Purpose 	: set a free item's rectangle within its container (eg. UORECT makes it completely fill its container)
		Parameters 	: new free-rectangle for the free item
		Returns 	: this
		Info 		: // PP: realign (eg. open) one of the item's containers for the change to take effect
	*/
	struct TAG_PAGEITEM* setFreeRect(const RECTANGLE freeRectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getRect
		Purpose 	: get the item's rectangle
		Parameters 	: 
		Returns 	: ref to the item's rectangle
		Info 		: // PP: if this gets called from outside the Book system,
						I'd like to know about it
	*/
	inline RECTANGLE& getRect(void)
	{
		// PP: "It's always fun to getRect™!"
		return this->rect;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setEffect
		Purpose 	: specify an effect for one of the item's effect modes (eg. PIEMODE_ENTER, PIEMODE_EXIT, etc.)
		Parameters 	: ptr to PAGEITEMEFFECT to use (or NULL not to override PIECHOOSER), (OPT/PIEMODE_ALL)effect mode in which to use the effect
		Returns 	: this
		Info 		: The effect you specify will override the PIECHOOSERs, unless you're setting NULL.
						You can set the same effect pointer onto as many of the item's different effect modes as you want;
							you might wanna make sure the effect supports all the modes you're setting it to though.
						// PP: NOTE: *** PAGEITEMS NO LONGER DELETE THE EFFECTS THAT YOU SET HERE!!! ***
	*/
	struct TAG_PAGEITEM* setEffect(PAGEITEMEFFECT* const effectIn, const PIEMode mode=PIEMODE_ALL);

	PIEMode getEffectMode();

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setEffectChooser
		Purpose 	: set the pageitem effect chooser for this item to use
		Parameters 	: ptr to chooser to use
		Returns 	: this
		Info 		: 
	*/
	virtual struct TAG_PAGEITEM* setEffectChooser(const struct PIECHOOSER* const chooser)
	{
		this->effectChooser=chooser;
		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getEffectChooser
		Purpose 	: get a ptr to the pageitem effect chooser for this item to use
		Parameters 	: 
		Returns 	: a ptr to the pageitem effect chooser for this item to use
		Info 		: // PP: just cos I wanna keep effectChooser protected so's people don't poke about with it
	*/
	inline const struct PIECHOOSER* getEffectChooser(void) const// PP: NOTE: many more of my accessors should be returning const *'s!  I'll remember next time, me promise.
	{
		return this->effectChooser;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::update
		Purpose 	: update the page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::draw
		Purpose 	: draw the page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::drawDebug
		Purpose 	: draw debug info for the page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void drawDebug(void)
	{
#ifndef INFOGRAMES// PP: don't want debug lines on a disk

		this->startDraw();
		this->mainDrawDebug();
		this->endDraw();

#endif// PP: ndef INFOGRAMES
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getItem
		Purpose 	: look within the item for another item
		Parameters 	: name of item to look for
		Returns 	: NULL, but in subclass overrides, a pointer to the specified item if found
		Info 		: 
	*/
	virtual inline struct TAG_PAGEITEM* getItem(const char* const itemName)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/

		return NULL;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getItem
		Purpose 	: get a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
		Parameters 	: x position, y position
		Returns 	: a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
		Info 		: this assumes that ye're drawing the book in 2D mode and with an identity object matrix
	*/
	virtual inline struct TAG_PAGEITEM* getItem(const float xPos, const float yPos)
	{
		if(this->screenRect.contains(xPos, yPos))
		{
			return this;
		}
		else
		{
			return NULL;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getTextbox
		Purpose 	: look within the item for a textbox holding a specified string
		Parameters 	: index of string to search for
		Returns 	: NULL, but in subclass overrides, a pointer to the first textbox holding the specified string (or NULL if not found)
		Info 		: // PP: Red Leicester is at its best when allowed to soften a while at room temperature.
	*/
	virtual inline struct TAG_TEXTBOX* getTextbox(const EStringIndex string)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/

		return NULL;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getNAvigationCandidate
		Purpose 	: get a pointer to this item, or to the item within this item which is
						the most suitable item to which to navigate, from a specified
						current position at a specified navigation angle
		Parameters 	: (in/out) ref to a NAVCANDIDATE_INFO structure (TextFX.h)
		Returns 	: a pointer to this item, or to the item within this item which is the
						most suitable item to which to navigate
		Info 		: 
	*/
	virtual inline struct TAG_PAGEITEM* getNavigationCandidate(NAVCANDIDATE_INFO& info)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/

		info.wrapOut=false;// PP: the candidate being returned is not the result of a navigation wrap within this item

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getNavigationScope
		Purpose 	: get the navigation scope of the item, or NULL if it's of a type that can't contain other items
		Parameters 	: 
		Returns 	: the navigation scope of the item, or NULL if it's of a type that can't contain other items
		Info 		: 
	*/
	virtual inline struct TAG_PAGE* getNavigationScope(void)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/

		return NULL;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::navigate
		Purpose 	: navigate in a given direction across the item
		Parameters 	: angle of navigation 0..2PI, whether or not to navigate recursively into sub-pages etc.,
						the EPageNavInput type that triggered the navigation (PAGENAVINPUT_...)
		Returns 	: 
		Info 		: 
	*/
	virtual inline void navigate(const float navAng, const bool recur, const EPageNavInput inputType)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getFirstSelectableItem
		Purpose 	: get a pointer to this, or to the first selectable sub-item inside this
		Parameters 	: whether to search inside this item or not (NOT USED BY THIS BASE-CLASS IMPLEMENTATION)
		Returns 	: ptr to first selectable item or NULL if none
		Info 		: 
	*/
	virtual inline struct TAG_PAGEITEM* getFirstSelectableItem(const bool recur)
	{
		/*Ordinary PAGEITEMs don't contain other PAGEITEMS you see.
		BOOK and PAGE override this method in a useful way.*/

		if(this->selectable())
		{
			return this;
		}
		else
		{
			return NULL;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::selectable
		Purpose 	: determine if this page item is selectable
		Parameters 	: whether or not this page item is selectable
		Returns 	: this
		Info 		: 
	*/
	inline bool selectable(void) const
	{
		return ((this->pageItemFlags&PIFLAG_SELECTABLE) != 0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setDrawScale
		Purpose 	: set the scale used to draw the item
		Parameters 	: new scale to use for drawing the item
		Returns 	: this
		Info 		: 
	*/

#define PAGEITEM_MIN_DRAW_SCALE		0.001f			// PP: TEMP?

	inline TAG_PAGEITEM* setDrawScale(const float scaleIn)
	{
		this->drawScale=MAX(PAGEITEM_MIN_DRAW_SCALE, scaleIn);

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setAlignScale
		Purpose 	: set the scale used when reporting the required width and height of the item
		Parameters 	: the scale used when reporting the required width and height of the item
		Returns 	: this
		Info 		: 
	*/

#define PAGEITEM_MIN_ALIGN_SCALE	0.0f			// PP: TEMP?

	inline TAG_PAGEITEM* setAlignScale(const float scaleIn)
	{
		this->alignScale=MAX(PAGEITEM_MIN_ALIGN_SCALE, scaleIn);

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setScale
		Purpose 	: set the scale for the item
		Parameters 	: new scale, used both for drawing, and reporting required width & height
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setScale(const float scaleIn)
	{
		setDrawScale(scaleIn);
		setAlignScale(scaleIn);

		return this;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getDrawScale
		Purpose 	: get the scale used to draw the item
		Parameters 	: 
		Returns 	: scale used for drawing the item
		Info 		: 
	*/
	inline float getDrawScale(void)
	{
		return this->drawScale;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getAlignScale
		Purpose 	: get the scale used when reporting the required width and height of the item
		Parameters 	: 
		Returns 	: scale used when reporting the required width and height of the item
		Info 		: 
	*/
	inline float getAlignScale(void)
	{
		return this->alignScale;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getScale
		Purpose 	: same as PAGEITEM::getDrawScale
		Parameters 	: 
		Returns 	: scale of the item
		Info 		: 
	*/
	inline float getScale(void)
	{
		return this->getDrawScale();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getX
		Purpose 	: get the apparent mid-x position of the item
		Parameters 	: 
		Returns 	: the apparent mid-x position of the item
		Info 		: 
	*/
	inline float getX(void)
	{
	//	return this->optRect.midX();

		// PP: TEST
		return this->screenRect.midX();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getY
		Purpose 	: get the apparent mid-y position of the item
		Parameters 	: 
		Returns 	: the apparent mid-y position of the item
		Info 		: 
	*/
	inline float getY(void)
	{
		//bkPrintf("PAGEITEM::getY returning %5.2f\n", this->optRect.midY());
	//	return this->optRect.midY();

		// PP: TEST
		return this->screenRect.midY();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getPos
		Purpose 	: get the apparent centre position of the item
		Parameters 	: TBVector2D to fill in
		Returns 	: 
		Info 		: 
	*/
	inline void getPos(TBVector2D pos)
	{
		pos[X]=this->getX();
		pos[Y]=this->getY();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setXAlign
		Purpose 	: set the X-axis alignment of the item
		Parameters 	: new X alignment mode (PIFLAG_XALIGN_)
		Returns 	: this
		Info 		: 
	*/
	virtual TAG_PAGEITEM* setXAlign(const PageItemFlag align);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setYAlign
		Purpose 	: set the Y-axis alignment of the item
		Parameters 	: new Y alignment mode (PIFLAG_YALIGN_)
		Returns 	: this
		Info 		: 
	*/
	virtual TAG_PAGEITEM* setYAlign(const PageItemFlag align);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setXPadding
		Purpose 	: enable/disable left & right padding
		Parameters 	: true to enable / false to disable
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setXPadding(const bool pad)
	{
		if(pad)
		{
			this->pageItemFlags |= PIFLAG_PAD_X;
		}
		else
		{
			this->pageItemFlags &= ~PIFLAG_PAD_X;
		}

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getXPadding
		Purpose 	: find out if the item wants X padding
		Parameters 	: true if the item wants X padding / false if not
		Returns 	: 
		Info 		: 
	*/
	inline bool getXPadding(void)
	{
		return (this->pageItemFlags & PIFLAG_PAD_X);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setYPadding
		Purpose 	: enable/disable top & bottom padding
		Parameters 	: true to enable / false to disable
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setYPadding(const bool pad)
	{
		if(pad)
		{
			this->pageItemFlags |= PIFLAG_PAD_Y;
		}
		else
		{
			this->pageItemFlags &= ~PIFLAG_PAD_Y;
		}

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setSelectable
		Purpose 	: enable/disable selectability for individual items
		Parameters 	: true to enable / false to disable, (opt/false)selectable by mouse only (if parm1==true)
		Returns 	: this
		Info 		: 
	*/
	TAG_PAGEITEM* setSelectable(const bool on, const bool mouseOnly = false);

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::enable
		Purpose 	: enable or disable the page item
		Parameters 	: true to enable, false to disable
		Returns 	: this
		Info 		: 
	*/
	TAG_PAGEITEM* enable(const bool state);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setBackgroundColour
		Purpose 	: set the colour of the background
		Parameters 	: the colour of the background
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setBackgroundColour(const COLOUR& colourIn)
	{
		this->background = colourIn;

		return this;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setBorderColour
		Purpose 	: set the colour of the border
		Parameters 	: COLOUR
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setBorderColour(const COLOUR& colourIn)
	{
		this->border = colourIn;

		return this;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getYPadding
		Purpose 	: find out if the item wants Y padding
		Parameters 	: true if the item wants Y padding / false if not
		Returns 	: 
		Info 		: 
	*/
	inline bool getYPadding(void)
	{
		return ((this->pageItemFlags & PIFLAG_PAD_Y) != 0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setOpacity
		Purpose 	: set the opacity of the item
		Parameters 	: new opacity (0..1)
		Returns 	: this
		Info 		: 
	*/
	inline TAG_PAGEITEM* setOpacity(const float opacityIn)
	{
		this->opacity=opacityIn;

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getOpacity
		Purpose 	: set the opacity of the item
		Parameters 	: 
		Returns 	: the opacity of the item
		Info 		: 
	*/
	inline float getOpacity(void) const
	{
		return this->opacity;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::onInsert
		Purpose 	: notifies the item that it's being inserted into a page
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline virtual void onInsert(void)
	{
		this->numInstances++;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::onRemove
		Purpose 	: notifies the item that it's being removed from a page
		Parameters 	: 
		Returns 	: true if the item is ready to be deleted
		Info 		: 
	*/
	inline bool onRemove(void)
	{
		this->numInstances--;

		if(this->numInstances == 0)// PP: was <=
		{
			return true;		// PP: yes, you can delete me
		}
		else
		{
			return false;		// PP: NO! there's other instances of me elsewhere!
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::TAG_PAGEITEM
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline TAG_PAGEITEM(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::select
		Purpose 	: select/deselect the page item
		Parameters 	: (opt/true)true to select, false to deselect
		Returns 	: 
		Info 		: 
	*/
	virtual void select(const bool state=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::highlight
		Purpose 	: highlight/unhighlight the page item
		Parameters 	: (opt/true)true to highlight, false to unhighlight
		Returns 	: 
		Info 		: 
	*/
	virtual void highlight(const bool state=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getRequiredHeight
		Purpose 	: get the minimum required height for the item
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: was const-this, but Babel put a stop to that as usual
	*/
	inline virtual float getRequiredHeight(void)
	{
		return 0.0f;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::getRequiredWidth
		Purpose 	: get the minimum required width for the item
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: was const-this, but Babel put a stop to that as usual
	*/
	inline virtual float getRequiredWidth(void)
	{
		return 0.0f;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::shutdown
		Purpose 	: shut down the item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::~TAG_PAGEITEM
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual ~TAG_PAGEITEM(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setName
		Purpose 	: set the name of the page item
		Parameters 	: the name of the page item!
		Returns 	: this
		Info 		: 
	*/
	TAG_PAGEITEM* setName(const char* const nameIn)
	{
		STRNCPY(this->name, nameIn, TEXTFX_NAME_LENGTH);

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setValue
		Purpose 	: set the value to be held by the page item
		Parameters 	: the value to be held by the page item
		Returns 	: this
		Info 		: 
	*/
	TAG_PAGEITEM* setValue(const uint32 valueIn)
	{
		this->value=valueIn;

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::drawscreenRects
		Purpose 	: draw the screen rects of the item and any items it contains
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void drawScreenRects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::<type>
		Purpose 	: return a <TYPE> ptr to this
		Parameters 	: 
		Returns 	: (<TYPE>*)this
		Info 		: // PP: WARNING: only call this on ptrs to <TYPE>s! (eg. PAGEITEM ptrs to <TYPE>s)
	*/
#define PAGEITEM_CAST_FUNCTION(_methodName, _type) \
	inline struct TAG_##_type* _methodName(void) const \
	{ \
		return (struct TAG_##_type*)this; \
	}


	// PP: Please see macro definition above
	// PP: Please add cast functions for your new PAGEITEM subclasses here (if you want to)
	// PP: Eeee, it's reet nice to have all 'family round 'table - lemme take a photo...
	PAGEITEM_CAST_FUNCTION(book, BOOK);
	PAGEITEM_CAST_FUNCTION(page, PAGE);
	PAGEITEM_CAST_FUNCTION(table, TABLE);
	PAGEITEM_CAST_FUNCTION(textbox, TEXTBOX);
	PAGEITEM_CAST_FUNCTION(checkbox, CHECKBOX);
	PAGEITEM_CAST_FUNCTION(spinner, SPINNER);
	PAGEITEM_CAST_FUNCTION(golfBar, GOLFBAR);
	PAGEITEM_CAST_FUNCTION(healthBar, HEALTHBAR);
	PAGEITEM_CAST_FUNCTION(icon, ICON);
	PAGEITEM_CAST_FUNCTION(iconChange, ICONCHANGE);
	PAGEITEM_CAST_FUNCTION(slider, SLIDER);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::setBoxStyle
		Purpose 	: set the background & border fitting mode
		Parameters 	: tight box X?, tight box Y?
		Returns 	: this
		Info 		: // PP: I can imagine backgrounds & borders getting much more
						complicated in another game, but this might be fine for Taz
	*/
	struct TAG_PAGEITEM* setBoxStyle(const bool tightBoxXin, const bool tightBoxYin);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::operator ==
		Purpose 	: compare the item to a string (ie. compare its name to a string)
		Parameters 	: string to which to compare this item's name
		Returns 	: true if the string matches this item's name
		Info 		: CASE INSENSITIVE
	*/
	inline bool operator == (const char* const string) const
	{
		return (stricmp(string, this->name) == 0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::operator uint32()
		Purpose 	: cast the page item to a uint32 in order to get its client-supplied 'value'
		Parameters 	: 
		Returns 	: item->value
		Info 		: 
	*/
	inline operator uint32()
	{
		return this->value;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::operator const uint32()
		Purpose 	: cast the page item to a const uint32 in order to get its client-supplied 'value'
		Parameters 	: 
		Returns 	: (const) item->value
		Info 		: 
	*/
	inline operator const uint32() const
	{
		return this->value;
	}

	
}PAGEITEM, _PAGEITEM BALIGN16;


// PP: SOME PAGEITEM TYPES...

// PP: TODO: #include "Spinner.h"	// PP: Spinner control page-item (lets you specify a numerical value)
#include "TextBox.h"				// PP: Text box page-item - displays a text string
#ifndef DONT_INCLUDE_PRINTBOX
// PP: REMOUT: OOD, but I'll keep it here for a "whee while", till I'm absolutely sure it's dead	 #include "PrintBox.h"				// PP: Modifiable text box page-item - displays a text string which can be edited and which can contain dynamic links to data to display
#endif// PP: ndef DONT_INCLUDE_PRINTBOX
#include "Slider.h"					// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGEROW

													A row holds items.
					By inserting a page into a row as an item, you can also create a column containing more rows.
					By inserting a book into a row as an item, you can create changeable panes within the row.
											
												There's no limit to the fun!™

												book > pages > *ROWS* > items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: row flags - not too many of them just yet
enum ERowFlag
{
	ROWFLAG_UNIFORM_X_SPACING		=	(1<<0),			// PP: all items on the row will be uniformly x-spaced - hurrah!
	ROWFLAG_HASEFFECTS				=	(1<<1),			// PP: the row contains at least one item with an effect
	ROWFLAG_ALLEFFECTSFINISHED		=	(1<<2)			// PP: all item effects on the row have finished (only valid if ROWFLAG_HASEFFECTS is high)
};


typedef struct TAG_PAGEROW
{

friend struct				TAG_PAGE;					// PP: there's no good reason to hinder communication between pages and their rows!

private:

	char					name[TEXTFX_NAME_LENGTH];	// PP: name of row

	RECTANGLE				rect;						// PP: rectangle used by the row

	LINKEDLIST<PAGEITEM>	items;						// PP: list of items on the row

	float					opacity;					// PP: the opacity level of the row and its contents

	uchar					index;						// PP: index of the row within its page

	// PP: row will be given the all the height it needs, not a height ration
	bool					priveleged;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::init
		Purpose 	: initialise the row
		Parameters 	: (opt/0) index of the row within its page
		Returns 	: 
		Info 		: 
	*/
	void init(const uint32 indexIn=0);


public:

	int						flags;						// PP: combination of flags for the row


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getColumnWidth
		Purpose 	: get the width of a specified column
		Parameters 	: index of column (they're numbered from 0 and from left to right)
		Returns 	: the width of the specified column!
		Info 		: will crash if specify an invalid column index
	*/
	inline float getColumnWidth(const int column)
	{
		return this->getItem(column)->optRect.width();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::setColumnWidths
		Purpose 	: set the widths of the columns in the row; reposition items accordingly
		Parameters 	: linked list of column widths, arranged from left to right
		Returns 	: 
		Info 		: 
	*/
	void setColumnWidths(LINKEDLIST<float> &columnWidths);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::TAG_PAGEROW
		Purpose 	: [default] constructor; calls init
		Parameters 	: index of the row within its page
		Returns 	: new PAGEROW instance
		Info 		: 
	*/
	TAG_PAGEROW(const uint32 indexIn=0);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::wantsYPadding
		Purpose 	: determine whether or not the row wants Y padding
		Parameters 	: 
		Returns 	: whether or not the row wants Y padding
		Info 		: 
	*/
	bool wantsYPadding(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::setRect
		Purpose 	: set the rectangle for the row and update its contents' rectangles accordingly
		Parameters 	: rectangle
		Returns 	: this
		Info 		: this is very similar to PAGE::setRect, only it works in a different axis
	*/
	struct TAG_PAGEROW* setRect(const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getItem
		Purpose 	: get the item with the specified name
		Parameters 	: name of item to find
		Returns 	: ptr to item if found, NULL otherwise
		Info 		: 
	*/
	PAGEITEM* getItem(const char* const itemName)// PP: const
	{
		PAGEITEM*	item;
		PAGEITEM*	item2;

		for(int i=this->getNumItems()-1;i>=0;i--)
		{
			item=this->getItem(i);

			if(strcmp(item->name, itemName)==0)
			{
				return item;
			}

			// PP: search within the page item for the page item that we're looking for
			item2=item->getItem(itemName);

			if(item2 != NULL)
			{
				return item2;// PP: found the page item within a page item
			}
		}

		return NULL;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getLastItem
		Purpose 	: get the last item added to the row
		Parameters 	: 
		Returns 	: ptr to last item if there are any, NULL otherwise
		Info 		: 
	*/
	inline PAGEITEM* getLastItem(void)
	{
		return this->getItem(this->getNumItems()-1);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getFirstItem
		Purpose 	: get the first item added to the row
		Parameters 	: 
		Returns 	: ptr to first item if there are any, NULL otherwise
		Info 		: 
	*/
	inline PAGEITEM* getFirstItem(void)// PP: const
	{
		return this->getItem(0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getFirstSelectableItem
		Purpose 	: get the first selectable item in the row, whether or not to recur the search into sub-items
		Parameters 	: 
		Returns 	: ptr to first selectable item if there are any, NULL otherwise
		Info 		: 
	*/
	PAGEITEM* getFirstSelectableItem(const bool recur);
	

	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::insertItem
		Purpose 	: insert an item onto the end of the row
		Parameters 	: item to insert
		Returns 	: 
		Info 		: 
	*/
	void insertItem(PAGEITEM* const item);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::drawDebug
		Purpose 	: draw debug info for the row and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void drawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::draw
		Purpose 	: draw the row and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::update
		Purpose 	: update the row and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getRequiredWidth
		Purpose 	: get the required width for the row
		Parameters 	: 
		Returns 	: minimum required width of the the row
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	float getRequiredWidth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getRequiredHeight
		Purpose 	: get the minimum required height of the row
		Parameters 	: 
		Returns 	: minimum required height of the row
		Info 		: 
	*/
	float getRequiredHeight(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getNumItems
		Purpose 	: get the number of items in the row
		Parameters 	: 
		Returns 	: the number of items in the row
		Info 		: inline all the way down - should just resolve to an int at the end of the day
	*/
	inline int getNumItems(void) const
	{
		return this->items.getSize();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::getItem
		Purpose 	: get the item at the specified index within the row
		Parameters 	: 
		Returns 	: the item at the specified index within the row, or NULL if index is invalid
		Info 		: 
	*/
	inline PAGEITEM* getItem(const int index)// PP: const
	{
		return this->items.get(index);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::select
		Purpose 	: select/deselect the row and its contents
		Parameters 	: (opt/true)true to select, false to deselect
		Returns 	: 
		Info 		: 
	*/
	void select(const bool state=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::setName
		Purpose 	: set the name of the row
		Parameters 	: new name for the row
		Returns 	: 
		Info 		: 
	*/
	inline void setName(const char* const nameIn)
	{
		strcpy(this->name, nameIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::shutdown
		Purpose 	: free up space used by the row and its items
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEROW::~TAG_PAGEROW
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_PAGEROW(void);


}PAGEROW, _PAGEROW BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															PAGE

													A page holds rows.
										Pages can also themselves be held in rows.

												There's no limit to the fun!™

												book > *PAGES* > rows > items

															@page
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: page->specific flags
enum EPageFlags
{
	PAGEFLAG_NAVIGABLE			=1,			// PP: selectable items on the page can be moved between
	PAGEFLAG_COLUMNNAVIGATION	=1<<1,		// PP: (if PAGEFLAG_NAVIGABLE is also set) columns (sub-pages) can be navigated between using the shoulder buttons, taking the navigation scope of their parent page one at a time
	PAGEFLAG_TOPLEVELPAGE		=1<<2,		// PP: top-level page (contained by a book rather than another page)
	PAGEFLAG_CLOSING			=1<<3,		// PP: the page is being closed or turned away from
	PAGEFLAG_CLOSED				=1<<4,		// PP: the page is fuly closed: no need to draw or update it
	PAGEFLAG_VSLACK				=1<<5,		// PP: page allows vertical slack between/around its contents, rather than fitting to their minimum total required height
	PAGEFLAG_HSLACK				=1<<6,		// PP: page allows horizonal slack between/around its contents, rather than fitting to their minimum total required width
	PAGEFLAG_IGNORESTARTBUTTON	=1<<7,		// PP: don't recognise START button as a means of triggering MENUACTION_PRESS (used on pages in the pause menu)
	PAGEFLAG_NAVWRAP_X			=1<<8,		// PP: (if PAGEFLAG_NAVIGABLE is also set) X navigation wraps from one edge of the page to the other
	PAGEFLAG_NAVWRAP_Y			=1<<9,		// PP: (if PAGEFLAG_NAVIGABLE is also set) Y navigation wraps from one edge of the page to the other
	PAGEFLAG_NAVWRAP_XDEBOUNCE	=1<<10,		// PP: (if PAGEFLAG_NAVWRAP_AUTOREPEAT is also set; otherwise it's like PAGEFLAG_NAVWRAP_X) like PAGEFLAG_NAVWRAP_X, but X navigation will only wrap in response to a debounced input channel, NOT an auto-repeat channel.  So you can hold RIGHT and it'll zip to the right hand end of a menu, then you'll have to press RIGHT again to wrap to the left side.
	PAGEFLAG_NAVWRAP_YDEBOUNCE	=1<<11,		// PP: (if PAGEFLAG_NAVWRAP_AUTOREPEAT is also set; otherwise it's like PAGEFLAG_NAVWRAP_Y) like PAGEFLAG_NAVWRAP_Y, but Y navigation will only wrap in response to a debounced input channel, NOT an auto-repeat channel.  So you can hold DOWN and it'll zip to the bottom of a menu, then you'll have to press DOWN again to wrap to the top.

	// PP: only TABLEs support these at present; normal PAGEs treat them like PAGEFLAG_NAVWRAP_X and PAGEFLAG_NAVWRAP_Y
	PAGEFLAG_NAVWRAP_NEWLINE	=1<<12,		// PP: (if PAGEFLAG_NAVIGABLE is also set) X navigation wraps from the end of one line to the start of the next. This overrides PAGEFLAG_NAVWRAP_X.  Behaves like PAGEFLAG_NAVWRAP_X if only one line.
	PAGEFLAG_NAVWRAP_NEWCOLUMN	=1<<13,		// PP: (if PAGEFLAG_NAVIGABLE is also set) Y navigation wraps from the bottom of one column to the top of the next. This overrides PAGEFLAG_NAVWRAP_Y.  Behaves like PAGEFLAG_NAVWRAP_Y if only one column.
};

// PP: all the flags that describe navigation wrapping behaviour
#define PAGE_NAVWRAP_FLAGS			(PAGEFLAG_NAVWRAP_X\
									|PAGEFLAG_NAVWRAP_Y\
									|PAGEFLAG_NAVWRAP_XDEBOUNCE\
									|PAGEFLAG_NAVWRAP_YDEBOUNCE\
									|PAGEFLAG_NAVWRAP_NEWLINE\
									|PAGEFLAG_NAVWRAP_NEWCOLUMN)

// PP: default page-specific flags
// PP: NOTE: no navigation wrapping at all on base PAGEs by default.
#define PAGE_DEFAULT_FLAGS			(PAGEFLAG_TOPLEVELPAGE|PAGEFLAG_HSLACK|PAGEFLAG_VSLACK)

// PP: type for custom open/update/close function pointers in pages
typedef void(*PageFunc)(struct TAG_PAGE* const page, const int32 context);

// PP: Menu actions that get picked up by the page navigation functions
enum EMenuAction
{
	MENUACTION_SELECT,				// PP: an item is moved-onto
	MENUACTION_DESELECT,			// PP: an item is moved-away-from
	MENUACTION_PRESS,				// PP: an item is pressed
	MENUACTION_BACK,				// PP: the menu is backed-out-of (eg. with PS2 triangle button)
	MENUACTION_START,				// PP: the 'start' button, or equivalent, is pressed
};

// PP: (relating to menu actions) the player index used to indicate that the action is not being performed by a player
#define MENUACTION_NO_PLAYER		(-1)

// PP: structure used to pass data into MenuActionFunc callbacks
struct MENUACTION_INFO
{
	// PP: IN...
	PAGEITEM*		item;			// PP: the item upon which the action is being performed (or NULL if none)
	EMenuAction		action;			// PP: the action being performed
	char			player;			// PP: zero-based index of the player performing the action, or MENUACTION_NO_PLAYER if not performed by a player
	char			port;			// PP: zero-based physical port number of the pad triggering the action
	void*			context;		// PP: client-supplied context value, specified (optionally) in calls to PAGE::setMenuActionFunc

}BALIGN16;

/*	--------------------------------------------------------------------------------
	Typedef 	: MenuActionFunc
	Purpose 	: Type for client-supplied callbacks that respond to menu actions
					(see EMenuAction enum above for possible menu actions)
	Parameters 	: ref to a MENUACTION_INFO structure (see above)
	Returns 	: 
	Info 		: 
*/
typedef void(*MenuActionFunc)(MENUACTION_INFO& info);

typedef struct TAG_PAGE		:	public PAGEITEM
{

friend struct						PAGETRAVERSAL;			// PP: class used to efficiently traverse the top level of a page// PP: friend because it's easier, and because I trust myself
friend struct						TAG_BOOK;				// PP: there's no good reason to hinder communication between books and their pages!
friend struct						TAG_PAPERBOOK;			// PP: subclass of BOOK
friend struct						MEMCARDPIECHOOSER;		// TP: Mmmm slidey
friend struct						WANTEDPIECHOOSER;		// TP: Mmmm slidey
friend struct						POPUPPIECHOOSER;		// PP: subclass of PIECHOOSER

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::playNavSound
		Purpose 	: Plays a navigation sound
		Parameters 	: Pointer to the menu sounds, sound type
		Returns 	: 
		Info 		: 
	*/
	void playNavSound(struct TAG_MENU_SOUNDS *sounds, int sound);

private:

	struct TAG_PAGE*				navigationScope;		// PP: ptr to the page that is the scope for this page's navigation.  Normally just 'this', but may be set to a sub-page, or any page for that matter.

	MenuActionFunc					menuActionFunc;			// PP: client-supplied callback that responds to menu actions (see EMenuAction enum above for possible menu actions)
	void*							menuActionContext;		// PP: client-supplied context ptr to be passed through to the menuActionFunc

	float							oldSafeWidthPortion;	// PP: safe width portion of the screen in effect when the page was originally designed
	float							oldSafeHeightPortion;	// PP: safe height portion of the screen in effect when the page was originally designed

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getFreeItem
		Purpose 	: get a ptr to the non-aligned item at a spcified index
		Parameters 	: index of non-aligned item
		Returns 	: a ptr to the non-aligned item at the spcified index
		Info 		: 
	*/
	inline PAGEITEM* getFreeItem(const int index)
	{
		return this->freeItems.get(index);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getNumFreeItems
		Purpose 	: find out how many non-aligned items the page has
		Parameters 	: 
		Returns 	: how many non-aligned items the page has
		Info 		: 
	*/
	inline int getNumFreeItems(void) const
	{
		return this->freeItems.getSize();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::navigate
		Purpose 	: navigate in a given direction across the page
		Parameters 	: angle of navigation 0..2PI, whether or not to navigate recursively into sub-pages etc.,
						the EPageNavInput type that triggered the navigation (PAGENAVINPUT_...)
		Returns 	: 
		Info 		: this overrides PAGEITEM::navigate
	*/
	void navigate(const float navAng, const bool recur, const EPageNavInput inputType);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::detectMenuActionPress
		Purpose 	: detect the 'press' menu action
		Parameters 	: 
		Returns 	: true if the 'press' menu action was detected, else false
		Info 		: 
	*/
	virtual bool detectMenuActionPress(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::debounceMenuActionPress
		Purpose 	: force-debounce the button(s) used to trigger the 'press' menu action
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void debounceMenuActionPress(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::detectMenuActionBack
		Purpose 	: detect the 'back' menu action
		Parameters 	: 
		Returns 	: true if the 'back' menu action was detected, else false
		Info 		: 
	*/
	virtual bool detectMenuActionBack(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::debounceMenuActionBack
		Purpose 	: force-debounce the button(s) used to trigger the 'back' menu action
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	virtual void debounceMenuActionBack(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::respondToNavigation
		Purpose 	: respond to the Player navigating around the page with the pad
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void respondToNavigation(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this PAGE
		Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
		Returns 	: 
		Info 		: this overrides PAGEITEM::chooseEffect, to provoke effect choices by the page's contents
	*/
	void chooseEffect(const PIEMode mode, const bool recur);


protected:

	char					controller;				// JW: ID of controller that can navigate this page
	int						flags;							// PP: page-specific flags
	bool					acceptInput;					// PP: should this page be accepting player input?  stack overflows / accidental input can occur if this is ignored
	float					margin;							// PP: thickness of margin round the inside of the page, as a fraction of screen height (or of safe-screen height; I haven't decided yet but I doubt it's *that* important)

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getNumRows
		Purpose 	: get the number of rows in the page
		Parameters 	: 
		Returns 	: the number of rows in the page
		Info 		: 
	*/
	inline int getNumRows(void)
	{
		return this->rows.getSize();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getRow
		Purpose 	: get a ptr to the row at the specified index
		Parameters 	: 
		Returns 	: ptr to the new row
		Info 		: 
	*/
	inline PAGEROW* getRow(const int index)
	{
		return this->rows.get(index);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getFirstRow
		Purpose 	: get hold of the first row added to the page
		Parameters 	: 
		Returns 	: ptr to the first row added, or NULL if there are no rows
		Info 		: 
	*/
	inline PAGEROW* getFirstRow(void) const
	{
		return this->rows.getFirst();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::init
		Purpose 	: initialise the page
		Parameters 	: (opt/NULL)name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function, (opt/NULL)has-closed function
		Returns 	: 
		Info 		: 
	*/
	void init(const char* const nameIn=NULL, PageFunc const updateFuncIn=NULL, PageFunc const openFuncIn=NULL, PageFunc const closeFuncIn=NULL, PageFunc const hasOpenedFuncIn=NULL, PageFunc const hasClosedFuncIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getItemsUnsuitabilityAsNavigationTarget
		Purpose 	: determine how unsuitable an item is as a navigation target for a page
		Parameters 	: ptr item to assess, position of currently selected item, angle of navigation in radians
		Returns 	: an unsuitability value
		Info 		: Do not take with any other paracetamol-containing products
	*/
	virtual float getItemsUnsuitabilityAsNavigationTarget(PAGEITEM* const item, const TBVector2D currentPos, const float navAng);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::mainDraw
		Purpose 	: the core of the draw method - actually draw the page and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::mainDrawDebug
		Purpose 	: the core of the debug-draw method - actually debug-draw the page and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDrawDebug
	*/
	virtual void mainDrawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setRect
		Purpose 	: use the page's current rectangle to update its contents' rectangles
		Parameters 	: 
		Returns 	: (PAGEITEM*)this
		Info 		: 
	*/
	PAGEITEM* setRect(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setRect
		Purpose 	: set the Rectangle for the page (and update its contents accordingly)
		Parameters 	: rectangle
		Returns 	: (PAGEITEM*)this
		Info 		: this is just exactly like PAGEROW::setRect but with Y padding taken into account rather than X padding
						this overrides PAGEITEM::setRect
	*/
	PAGEITEM* setRect(const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setLanguage
		Purpose 	: set the language of the page
		Parameters 	: new language of the page (BLANGUAGEID_...)
		Returns 	: 
		Info 		: this overrides PAGEITEM::setLanguage, to recur the language change through the page's contents
	*/
	void setLanguage(const EBLanguageID languageIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::markForRealignment
		Purpose 	: tell the pageitem that its alignment is out of date and will need
						to be updated in next align()
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGITEM::markForRealignment
						// PP: this gets recurred down the contents hierarchy
	*/
	void markForRealignment(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::align
		Purpose 	: calculate alignment for the page's contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::align
						// PP: this gets recurred down the contents hierarchy
	*/
	virtual void align(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::effectsFinished
		Purpose 	: determine if all effects on the page and its contents have finished
		Parameters 	: 
		Returns 	: true if all effects on the page and its contents have finished, else false
		Info 		: this overrides PAGEITEM::effectsFinished, to check that the page's contents' effects are also finished
	*/
	bool effectsFinished(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::hasEffects
		Purpose 	: determine if the page or any of its contents have effects on them
		Parameters 	: 
		Returns 	: true if the page or any of its contents has an effect on it
		Info 		: this overrides PAGEITEM::hasEffects, to check if the page's contents have any effects
	*/
	bool hasEffects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::freeTempMemory
		Purpose 	: free-up temporary memory used by the item and its contents,
						eg. effects that have finished
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::freeTempMemory
	*/
	void freeTempMemory(void);


public:

	// PP: yeh what the hell
	PAGEITEM*				selectedItem;					// PP: this single selected item of the page, or NULL if none is selected

	// PP: TODO: why is this all public?  was there a good reason for it?

	LINKEDLIST<PAGEROW>		rows;							// PP: list of rows of items// PP: public for the benefit of the PageFuncs, which the class can't anonymously befriend

	LINKEDLIST<PAGEITEM>	freeItems;						// PP: list of 'free' (non-aligning) items

	PageFunc				updateFunc;						// PP: custom update function for the page - to deal with player resonses etc.
	int32					updateFuncContext;				// PP: context value passed through to the update callback

	PageFunc				openFunc;						// PP: custom open function for the page - to play a sound or whatever
	int32					openFuncContext;				// PP: context value passed through to the open callback

	PageFunc				closeFunc;						// PP: custom close function for the page - to play a sound or whatever
	int32					closeFuncContext;				// PP: context value passed through to the close callback

	PageFunc				hasOpenedFunc;					// PP: custom has-opened function for the page - to remove the page or whatever
	int32					hasOpenedFuncContext;			// PP: context value passed through to the has-opened callback

	PageFunc				hasClosedFunc;					// PP: custom has-closed function for the page - to remove the page or whatever
	int32					hasClosedFuncContext;			// PP: context value passed through to the has-closed callback

	MENU_SOUNDS*			menuSounds;						// NH: Samples to play within page


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setEffectChooser
		Purpose 	: set the pageitem effect chooser for this page to use
		Parameters 	: ptr to chooser to use
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setEffectChooser,
						to get all the page's contents that don't have choosers
						of their own to point back to this chooser.
						// PP: TODO: make this recursion optional?
	*/
	struct TAG_PAGEITEM* setEffectChooser(const struct PIECHOOSER* const chooser);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::onInsert
		Purpose 	: notifies the page that it's being inserted into another page
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::onInsert
	*/
	 void onInsert(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setMenuActionFunc
		Purpose 	: set the function for this page to use to handle menu actions, such as selecting or clicking an item
		Parameters 	: ptr to the function for this page to use to handle menu actions (such as selecting or clicking an item),
						(opt/NULL) context ptr to pass through to the menu action callback
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* setMenuActionFunc(MenuActionFunc const func, void* const context=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setNavigationScope
		Purpose 	: set the navigation scope for the page
		Parameters 	: ptr to page that is to be the navigation scope for this page
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* setNavigationScope(struct TAG_PAGE* const scopeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setNavigationScope
		Purpose 	: set the navigation scope for the page
		Parameters 	: name of sub-page that is to be the navigation scope for this page
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* setNavigationScope(const char* const scopeName);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::resetNavigationScope
		Purpose 	: reset the navigation scope for the page back to itself
		Parameters 	: 
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* resetNavigationScope(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setHSlack
		Purpose 	: specify whether a page allows horizontal slack between/around its contents,
						or whether it instead fits to their minimum total required width
		Parameters 	: true=slack / false=no slack
		Returns 	: this
		Info 		: this method, and PAGE::setVSlack, jointly replace PAGE::setVacuumPack
	*/
	struct TAG_PAGE* setHSlack(const bool slack);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setVSlack
		Purpose 	: specify whether a page allows vertical slack between/around its contents,
						or whether it instead fits to their minimum total required height
		Parameters 	: true=slack / false=no slack
		Returns 	: this
		Info 		: this method, and PAGE::setHSlack, jointly replace PAGE::setVacuumPack
	*/
	struct TAG_PAGE* setVSlack(const bool slack);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setSlack
		Purpose 	: specify whether a page allows slack between/around its contents,
						or whether it instead fits to their minimum total required width & height
		Parameters 	: true=slack / false=no slack
		Returns 	: this
		Info 		: this replaces PAGE::setVacuumPack (inversely like)
	*/
	struct TAG_PAGE* setSlack(const bool slack);
	

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::update
		Purpose 	: update the page and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::open
		Purpose 	: called when the page is opened or turned to
		Parameters 	: (opt/false)open instantly (bypassing effects)
		Returns 	: 
		Info 		: 
	*/
	void open(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::close
		Purpose 	: called when the page is closed or turned away from
		Parameters 	: (opt/false)close instantly (bypassing effects)
		Returns 	: 
		Info 		: 
	*/
	void close(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::hasOpened
		Purpose 	: called when the page becomes fully opened or turned to
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void hasOpened(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::hasClosed
		Purpose 	: called when the page becomes fully closed or turned away from
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void hasClosed(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getItem
		Purpose 	: get the item with the specified name
		Parameters 	: name of item to find
		Returns 	: ptr to item if found, NULL otherwise
		Info 		: 
	*/
	PAGEITEM* getItem(const char* const itemName);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getItem
		Purpose 	: get a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
		Parameters 	: x position, y position
		Returns 	: a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
		Info 		: this assumes that ye're drawing the book in 2D mode and with an identity object matrix
						this overrides PAGEITEM::getItem(x, y)
	*/
	PAGEITEM* getItem(const float xPos, const float yPos);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getTextbox
		Purpose 	: look within the page for a textbox holding a specified string
		Parameters 	: index of string to search for
		Returns 	: a pointer to the first textbox holding the specified string (or NULL if not found)
		Info 		: // PP: this overrides PAGEITEM::getTextbox
	*/
	struct TAG_TEXTBOX* getTextbox(const EStringIndex string);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getNavigationCandidate
		Purpose 	: get a pointer to the item within this page which is
						the most suitable item to which to navigate, from a specified
						current position at a specified navigation angle
		Parameters 	: (in/out) ref to a NAVCANDIDATE_INFO structure (TextFX.h)
		Returns 	: a pointer to the item within this page which is the
						most suitable item to which to navigate
		Info 		: this overrides PAGEITEM::getNavigationCandidate
	*/
	virtual TAG_PAGEITEM* getNavigationCandidate(NAVCANDIDATE_INFO& info);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getNavigationScope
		Purpose 	: get the navigation scope of the page
		Parameters 	: 
		Returns 	: the navigation scope of the page
		Info 		: this overrides PAGEITEM::getNavigationScope
	*/
	inline struct TAG_PAGE* getNavigationScope(void)
	{
		return this->navigationScope;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::insertItem
		Purpose 	: insert an item onto the end of the page
		Parameters 	: ptr to new item, (opt/true) whether or not to create a new row
		Returns 	: ptr to new item
		Info 		: default parameters create a new floating flexible row for the item
	*/
	PAGEITEM* insertItem(PAGEITEM* const item, const int newRow=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::insertItem
		Purpose 	: insert an item onto the end of the page
		Parameters 	: index of string to insert, whether or not to insert a new row
		Returns 	: ptr to new textbox
		Info 		: creates a new floating flexible row for the item by default
	*/
	struct TAG_TEXTBOX* insertItem(const EStringIndex stringIndex, const int newRow=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::insertFreeItem
		Purpose 	: insert a 'free' (non-aligning) item into the page
		Parameters 	: ptr to new item
		Returns 	: ptr to new item
		Info 		: 
	*/
	PAGEITEM* insertFreeItem(PAGEITEM* const item);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::insertFreeItem
		Purpose 	: insert a 'free' (non-aligning) item into the page
		Parameters 	: ptr to new item, rectangle for the item (as a fraction of the page that holds it)
		Returns 	: ptr to new item
		Info 		: 
	*/
	PAGEITEM* insertFreeItem(PAGEITEM* const item, const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::highlightNone
		Purpose 	: unhighlight all items on the page
		Parameters 	: 
		Returns 	: (PAGEITEM*)this
		Info 		: 
	*/
	PAGEITEM* highlightNone(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::highlightExclusive
		Purpose 	: exclusively highlight an item on this page
		Parameters 	: 
		Returns 	: (PAGEITEM*)this
		Info 		: 
	*/
	PAGEITEM* highlightExclusive(PAGEITEM* const item)
	{
		this->highlightNone();

		item->highlight();

		return (PAGEITEM*)this;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::highlight
		Purpose 	: highlight an item on this page
		Parameters 	: 
		Returns 	: (PAGEITEM*)this
		Info 		: 
	*/
	PAGEITEM* highlight(PAGEITEM* const item)
	{
		item->highlight();

		return (PAGEITEM*)this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::select
		Purpose 	: select/deselect the page
		Parameters 	: (opt/true)true to select, false to deselect
		Returns 	: 
		Info 		: NEW
						This overrides PAGEITEM::select
	*/
/*// PP: REMOUT: OOD i think	inline void select(const bool state=true)
	{
		if(state)
		{
			this->selectItem(this->getFirstItem());
		}
		else
		{
			this->selectItem(NULL);
		}
	}*/


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::selectItem
		Purpose 	: Select a specified item
		Parameters 	: (opt/NULL=none) ptr to item to select
		Returns 	: ptr to item
		Info 		: 
	*/
	PAGEITEM* selectItem(PAGEITEM* const item=NULL);

	
	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::selectNone
		Purpose 	: deselect all items
		Parameters 	: 
		Returns 	: this
		Info 		: 
	*/
	inline struct TAG_PAGE* selectNone(void)
	{
		this->selectItem();

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::TAG_PAGE
		Purpose 	: constructor; initialises the page
		Parameters 	: 
		Returns 	: 
		Info 		: this used to have an (opt/null) name param - now handled by the next overload
	*/
	TAG_PAGE(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::TAG_PAGE
		Purpose 	: constructor; initialises the page
		Parameters 	: name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function
		Returns 	: new PAGE instance
		Info 		: 
	*/
	TAG_PAGE(const char* const nameIn, PageFunc const update=NULL, PageFunc const open=NULL, PageFunc const close=NULL)
	{
		this->init(nameIn, update, open, close);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getRequiredWidth
		Purpose 	: get the required width for the page (according to its items)
		Parameters 	: 
		Returns 	: minimum required width of the the page
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	float getRequiredWidth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getRequiredHeight
		Purpose 	: get the required height for the page (according to its items)
		Parameters 	: 
		Returns 	: minimum required height of the the page
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	float getRequiredHeight(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getRow
		Purpose 	: get the row with the specified name
		Parameters 	: (opt/NULL returns first row)name of row to find, (opt/NULL)ptr to storage of row index if found
		Returns 	: ptr to the row if found, or NULL
		Info 		: 
	*/
	PAGEROW* getRow(const char* const rowName=NULL, int* const rowIndexOut=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::selectRow
		Purpose 	: select the specified row and its items
		Parameters 	: (opt/0)row to select, (opt/NULL)name of row to start counting from,
						(opt/0 considers all)number of rows in the group of interest					
		Returns 	: true if the row was found
		Info 		: 
	*/
	bool selectRow(const int index=-1, const char* const firstRowName=NULL, const int numRowsInGroup=0);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getLastRow
		Purpose 	: get hold of the last row added to the page
		Parameters 	: 
		Returns 	: ptr to the last row added, or NULL if there are no rows
		Info 		: 
	*/
	inline PAGEROW* getLastRow(void) const
	{
		return this->rows.getLast();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::addRow
		Purpose 	: add a row to the page
		Parameters 	: 
		Returns 	: ptr to the new row
		Info 		: 
	*/
	PAGEROW* addRow(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getLastItem
		Purpose 	: get hold of the last item added to the page
		Parameters 	: 
		Returns 	: ptr to the last item added, or NULL if there are no items
		Info 		: 
	*/
	PAGEITEM* getLastItem(void)
	{
		PAGEROW*	row;

		row=this->getLastRow();

		if(row == NULL)
		{
			return NULL;
		}
		else
		{
			return row->getLastItem();
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getFirstItem
		Purpose 	: get the first item on the page
		Parameters 	: 
		Returns 	: ptr to item if found, NULL otherwise
		Info 		: 
	*/
	PAGEITEM* getFirstItem(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::getFirstSelectableItem
		Purpose 	: get the first selectable item on the page
		Parameters 	: whether to search into the page's items rather than just returning this
		Returns 	: ptr to first selectable item if found, NULL otherwise
		Info 		: this overrides PAGEITEM::getFirstSelectableItem
	*/
	PAGEITEM* getFirstSelectableItem(const bool recur);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::shutdown
		Purpose 	: free up space used by the page and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::shutdown
	*/
	virtual void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::~TAG_PAGE
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_PAGE(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setName
		Purpose 	: set the name of the page
		Parameters 	: the name of the page!
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setName
	*/
	PAGEITEM* setName(const char* const nameIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGEITEM::drawscreenRects
		Purpose 	: draw the screen rects of the page and its items
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::drawScreenRects
	*/
	void drawScreenRects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setNavigation
		Purpose 	: specify whether or not the page is navigable
		Parameters 	: whether or not the page is navigable
		Returns 	: this
		Info 		: you can only navigate betweeen selectable items on the page.
						Page items default to selectable.
	*/
	struct TAG_PAGE* setNavigation(const bool on);

	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setColumnNavigation
		Purpose 	: set column navigation mode true/false
		Parameters 	: whether or not navigation is column-stylee
		Returns 	: this
		Info 		: you can only navigate betweeen selectable items on the page.
						this defaults to false.
						*** YOU STILL HAVE TO CALL setNavigation !! ***
	*/
	struct TAG_PAGE* setColumnNavigation(const bool on);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::ignoreStartButton
		Purpose 	: tell the page whether or not to allow the START button to trigger MENUACTION_PRESS
		Parameters 	: (opt/true) true=ignore START / false=allow START
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* ignoreStartButton(const bool ignore=true)
	{
		SETFLAG(this->flags, PAGEFLAG_IGNORESTARTBUTTON, ignore);

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setSounds
		Purpose 	: Sets the sounds for the page (Defaults to using the sounds from the book creating the page)
		Parameters 	: MENU_SOUNDS*
		Returns 	: 
		Info 		: 
	*/
	void setSounds(MENU_SOUNDS*	menuSounds);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setController
		Purpose 	: Sets the controller able to navigate the page
		Parameters 	: index of the controller able to navigate the page
		Returns 	: 
		Info 		: -1 = global; 0 = pad 1; 1 = pad 2
	*/
	void setController(char con);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setMargin
		Purpose 	: set the width of the margin around the inside of the page
		Parameters 	: the width of the margin around the inside of the page,
						as a fraction of screen height
		Returns 	: this
		Info 		: "A weeble with a gun"?!  What?!
	*/
	struct TAG_PAGE* setMargin(const float marginIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGE::setNavigationWrap
		Purpose 	: set the flags describing the bahaviour of the page's navigation
		Parameters 	: one or more OR'd PAGEFLAG_NAVWRAP_ values to define navigation wrapping behaviour
		Returns 	: this
		Info 		: 
	*/
	struct TAG_PAGE* setNavigationWrap(const uint32 navFlags);


}PAGE, _PAGE BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGETRAVERSAL

								Class used to efficiently traverse the top level of a page.

														@PAGETRAVERSAL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PAGETRAVERSAL
{

private:

	struct TAG_PAGE*		page;
	struct TAG_PAGEROW*		row;

	enum
	{
		STAGE_ALIGNED,		// PP: aligned items (those held in rows)
		STAGE_FREE,			// PP: free items (those not held in rows)
		STAGE_FINISHED
	}						stage;

	uchar					rowIndex;
	uchar					columnIndex;
	uchar					freeItemIndex;
							
	uint32					itemIndex;

	struct TAG_PAGEITEM*	item;

public:


	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::setPage
		Purpose 	: set the page to traverse
		Parameters 	: ptr to the page to traverse
		Returns 	: a ptr to the item that the traversal is currently at, or NULL if finished
		Info 		: 
	*/
	void setPage(PAGE* pageIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::init
		Purpose 	: initialise the PAGETRAVERSAL
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: called by constructors
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::init
		Purpose 	: initialise the PAGETRAVERSAL and set its page
		Parameters 	: ptr to page to traverse
		Returns 	: 
		Info 		: // PP: called by constructors
	*/
	void init(PAGE* const pageIn);

	
	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::PAGETRAVERSAL
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: new PAGETRAVERSAL instance
		Info 		: 
	*/
	PAGETRAVERSAL(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::PAGETRAVERSAL
		Purpose 	: constructor; calls init and setPage
		Parameters 	: ptr to page to traverse
		Returns 	: new PAGETRAVERSAL instance
		Info 		: 
	*/
	PAGETRAVERSAL(PAGE* const pageIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAGETRAVERSAL::next
		Purpose 	: progress to the next item in the page
		Parameters 	: 
		Returns 	: ptr to the next item in the page, or NULL if finished
		Info 		: // PP: use this to get the first item as well
	*/
	PAGEITEM* next(void);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															BOOK

			Books are a sensible way of storing, displaying and navigating pages of visual data such as text or graphics.
			Deriving from this base class lets you easily create books that do whatever crazy stuff you want them to do.

												There's no limit to the fun!™

												*BOOK* > pages > rows > items

															@book
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: TODO: NEATEN AND COMMENT (NO TIME RIGHT NOW)
struct BOOKCHAPTER
{
	char	name[128];
	int		startPage;
	int		endPage;

	// PP: comment me
	BOOKCHAPTER(void)
	{
		this->startPage=(-1);
		this->endPage=(-1);
	}

	// PP: comment me
	BOOKCHAPTER(const char* const nameIn)
	{
		this->startPage=(-1);
		this->endPage=(-1);
		strcpy(this->name, nameIn);
	}
};


// PP: Use this on books that draw in 3D.
// PP: Gets rid of the default flags that adjust the book in various ways for 2D projection
#define BOOK3D(_book)				((_book)->flags &= ~(BKFLAG_BOOKLIST|BKFLAG_2D|BKFLAG_SCREENADJUST|BKFLAG_STEREOADJUST))

#define BOOK_NUM_PAGES				(this->pages.getSize())
#define BOOK_GET_PAGE(_p)			(this->pages.get(_p))

// PP: see if flags indicate that the book's progress needs to be updated
#define BOOK_TRANSITION_FLAGS		(BKFLAG_CLOSING|/*BKFLAG_OPENING|BKFLAG_CHANGINGPAGE|*/BKFLAG_REMODING|BKFLAG_REPOSITIONING)
#define BOOK_IN_TRANSITION			(this->flags&BOOK_TRANSITION_FLAGS)
#define BOOK_END_TRANSITION			(this->flags&= ~BOOK_TRANSITION_FLAGS)

// PP: default flags for a new book
#define BOOK_DEFAULT_FLAGS			(BKFLAG_BOOKLIST|BKFLAG_2D|BKFLAG_TOPLEVELBOOK|BKFLAG_SCREENADJUST|BKFLAG_SAFEAREA|BKFLAG_STEREOADJUST)

//#define BOOK_DURATION_PAGECHANGE	0.35f			// PP: default duration of the page change transition
//#define BOOK_DURATION_OPEN			0.35f			// PP: default duration of the open transition
//#define BOOK_DURATION_CLOSE			0.35f			// PP: default duration of the close transition

enum
{
//	BKFLAG_CLOSED					=0,				// PP: fully closed - almost definitely NOTHING to draw
	BKFLAG_PLEASEOPEN				=1<<0,			// PP: on first update, the book is to be opened instantly.  This flag is set by "creating a book in the open state", eg. new BOOK(true)
	BKFLAG_OPEN						=1<<1,			// PP: fully open - almost definitely something to draw
//	BKFLAG_OPENING					=1<<2,			// PP: opening - almost definitely something to draw
	BKFLAG_CLOSING					=1<<3,			// PP: closing - almost definitely something to draw
// 	BKFLAG_CHANGINGPAGE				=1<<4,			// PP: changing page
	BKFLAG_REMODING					=1<<5,			// PP: changing mode (eg. size)
	BKFLAG_REPOSITIONING			=1<<6,			// PP: changing position
	BKFLAG_TOPLEVELBOOK				=1<<7,			// PP: book is a top-level book, for example it may realign itself when the aspect ratio of the screen changes
	BKFLAG_2D						=1<<8,			// PP: book is drawn in 2D projection mode (this includes getting drawn to an offsecreen render target, even if that ORT is used for a 3D effect)
	BKFLAG_SCREENADJUST				=1<<9,			// PP: if used in combination with BKFLAG_TOPLEVELBOOK, book gets adjusted according to the resolution and aspect ratio of the screen
	BKFLAG_SAFEAREA					=1<<10,			// PP: if used in combination with BKFLAG_SCREENADJUST, book takes into account the 'safe area' of the screen (ie. a 1*1 book will fill it).  If not set, the top-level screen-adjusted 1*1 book will fill the whole screen or the whole viewport
	BKFLAG_OLDSAFEAREA				=1<<11,			// PP: if used in combination with BKFLAG_SCREENADJUST, book takes into account an old 'safe area' of the screen (specified by BOOK::setOldSafeArea) (ie. a 1*1 book will fill it).  If not set, the top-level screen-adjusted 1*1 book will fill the whole screen or the whole viewport
	BKFLAG_STEREOADJUST				=1<<12,			// PP: if used in combination with BKFLAG_TOPLEVELBOOK, book gets offset for each eye in stereoscopic mode, to give the impression that the book is close to the viewer
	BKFLAG_TURNEDONSIDE				=1<<13,			// PP: book draws at a 90° angle (useful for making the most of an offscreen render target's x&y resolutions)
	BKFLAG_BOOKLIST					=1<<14,			// PP: book will be automatically updated and drawn by the Book List
	BKFLAG_FREEWHENCLOSED			=1<<15,			// PP: book is to be freed by the Book List once it has finished closing
	BKFLAG_SHUTDOWNWHENCLOSED		=1<<16,			// PP: book is to be shut-down by the Book List once it has finished closing
	BKFLAG_FIRSTUPDATE				=1<<17,			// PP: book is being updated for the first time as it begins to open.  Navigation should therefore be skipped because debounce button signals will not have had a chance to reset.
	BKFLAG_FULLSCREENBACKGROUND		=1<<18,			// PP: when drawing the book's background colour, use the full screen/splitscreen area to do so.  Even ignore the safe area.  This flag is controlled by (public) BOOK::setFullScreenBackground
													
	BKFLAG_OPENEXCLUSIVE			=1<<24,			// PP: FOR CLIENT USE// PP: the book will always open as the exclusive book
	BKFLAG_COEXCLUSIVE				=1<<25,			// PP: FOR CLIENT USE// PP: books flagged as 'open-exclusive' and 'co-exclusive' will be able to co exist as joint exclusive books (eg. the two "missing controller" books)
	BKFLAG_OVERRIDE_OPENEXCLUSIVE	=1<<26,			// PP: TO BE USED ONLY BY THE BOOK SYSTEM!!!!!// PP: flag used by BOOK::openExclusive to temporarily override BKFLAG_OPENEXCLUSIVE so that the book can be opened as usual
	BKFLAG_EXCLUSIVE				=1<<27,			// PP: TO BE USED ONLY BY THE BOOK SYSTEM!!!!!// PP: this is the Book List's current exclusive book
	BKFLAG_OPENWHENNOEXCLUSIVE		=1<<28,			// PP: TO BE USED ONLY BY THE BOOK SYSTEM!!!!!// PP: book will open when the Book List has no exclusive book
};

// PP: screen indices for top-level books - used to indicate to which screen the book is to be drawn (used for rectangle adjustment)
enum EBookScreen
{
	BKSCREEN_NOCHANGE	= -1,						// PP: no change to the current screen index of the book
	BKSCREEN_ONE		=0,							// PP: full screen or player 1's screen
	BKSCREEN_TWO		=1,							// PP: full screen of player 2's screen
	BKSCREEN_FULL		=2							// PP: always full screen
};

// PP: default target screen index for a top-level book if none is specified: full screen
#define BOOK_DEFAULT_SCREEN		BKSCREEN_FULL

typedef struct TAG_BOOK		:	public PAGEITEM
{
private:

	// PP: PS2 BABEL WORKAROUND: width and height of render target to which a book is drawing
	static uint32			PS2BabelWorkaround_renderTargetWidth;
	static uint32			PS2BabelWorkaround_renderTargetHeight;

	bool					onBookList;				// PP: is the book on the book list?

	const PIECHOOSER*		effectChooser;			// PP: ptr to effect chooser for the book's contents// PP: NOTE: more than one thing can share the same effect chooser!

	// PP: REMOVE THESE 2 AFTER TAZ
	float					oldSafeWidthPortion;	// PP: safe area in effect when the pre-BKFLAG_SAFEAREA book was being designed
	float					oldSafeHeightPortion;	// PP: safe area in effect when the pre-BKFLAG_SAFEAREA book was being designed


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this BOOK
		Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents hierarchy
		Returns 	: 
		Info 		: this overrides PAGEITEM::chooseEffect, to provoke effect choices by the book's pages
	*/
	void chooseEffect(const PIEMode mode, const bool recur);


// PP: TODO: MAYBE MAKE SOME OF THIS PRIVATE?  SEE HOW IT GOES
protected:

	EBLanguageID			language;						// PP: the current language of the book, ie. the language in which its text is written

	LINKEDLIST<PAGE>		pages;							// PP: linked list of pages
							
	TBVector				pos;							// PP: 2d / 3d position of the book (depends on subclass)
							
	RECTANGLE				bookRect;						// PP: rectangle of the book as a portion of the viewport that it gets drawn in (it's actually more complicated than that but sod it work it out for yourself if you're that interested.  I'm damned If I'm going to sit here typing big long comments all day.)
							
	PAGE*					oldPage;						// PP: page being turned away from
	PAGE*					currentPage;					// PP: current page, or page being turned onto
	int						currentPageIndex;				// PP: index of the current page in the page list
							
	int						oldMode;						// PP: mode being changed from
	int						currentMode;					// PP: current mode or mode being changed to

/* PP: REMOUT: OOD							
	TBSample*				openSound;						// PP: sample to play as the book opens
	TBSample*				closeSound;						// PP: sample to play as the book closes
	TBSample*				pageChangeSound;				// PP: sample to play as the book changes page
*/							
	
	float					closeDelay;						// PP: seconds remaining till close is due
							
	bool					closeInstantly;					// PP: whether or not to close instantly (once the close timer has run out)
							
	bool					priveleged;						// PP: forgotten what this does. Presumably it means the book has some kind of special treat, like lollies or custard.
							
	bool					autoDraw;						// NH: automatically draw the book. Added so that the subtitles could be easily toggled on/off during game. May have other uses too
	
	float					aspectRatio;					// PP: the aspect ratio to which this book has adapted (top-level books only)
	int						screenIndex;					// PP: index of the screen to which this book gets (used for top-level book rectangle adjustment)
	ESplitScreenMode		splitScreenMode;				// PP: split screen mode to which the book has adapted
	uchar					numScreens;						// PP: number of screens to which the book has adapted

	LINKEDLIST<BOOKCHAPTER>	chapters;						// PP: list of chapters in the book
	BOOKCHAPTER*			restrictingChapter;				// PP: chapter to which we're restricted


	 /*	--------------------------------------------------------------------------------
		Function 	: BOOK::hasClosed
		Purpose 	: books call this once when they become fully closed
		Parameters 	: 
		Returns 	: 
		Info 		: if you want a BOOK subclass to do something when it has closed,
						(eg. change back to front page), override this method and the
						standard BOOK::update will call it fur ye.
	*/
	virtual void hasClosed(void)
	{
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::startDraw
		Purpose 	: set up conditions needed to draw the book (eg. apply book's scale & position to object matrix)
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::startDraw, to apply all manner of adjustments
	*/
	virtual void startDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::mainDraw
		Purpose 	: the core of the draw method - actually draw the book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::mainDrawDebug
		Purpose 	: the core of the debug-draw method - actually debug-draw the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDrawDebug
	*/
	virtual void mainDrawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setRect
		Purpose 	: set the book's rectangle (and update its contents accordingly)
		Parameters 	: rectangle (as portion of target viewport), screen index 0/1/2
		Returns 	: this
		Info 		: this overrides PAGEITEM::setRect
						screen index: (see EBookScreen enum)
	*/
	struct TAG_BOOK* setRect(const RECTANGLE& rectIn, const int screenIndexIn);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setRect
		Purpose 	: set the book's rectangle (and update its contents accordingly)
		Parameters 	: rectangle
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setRect
	*/
	PAGEITEM* setRect(const RECTANGLE& rectIn)
	{
		return (PAGEITEM*)(this->setRect(rectIn, BKSCREEN_NOCHANGE));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setLanguage
		Purpose 	: set the language for the book
		Parameters 	: new language for the book (BLANGUAGEID_...)
		Returns 	: 
		Info 		: this overrides PAGEITEM::setLanguage, to recur the language change through the book's contents
	*/
	void setLanguage(const EBLanguageID languageIn);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getLatestChapter
		Purpose 	: get a ptr to the latest chapter to be added to the book
		Parameters 	: 
		Returns 	: a ptr to the latest chapter to be added to the book!
		Info 		: 
	*/
	inline BOOKCHAPTER* getLatestChapter(void)
	{
		return this->chapters.getLast();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::update
		Purpose 	: update the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::update
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::draw
		Purpose 	: draw the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::draw, to check firstly that the book is visible
	*/
	inline void draw(void)
	{
		if(this->flags&(BKFLAG_OPEN/*|BKFLAG_OPENING|*/|BKFLAG_CLOSING))
		{
			PAGEITEM::draw();
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::updateBox
		Purpose 	: update the box to be used to draw the book's background and/or border
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::updateBox.
	*/
	void updateBox(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::markForRealignment
		Purpose 	: tell the pageitem that its alignment is out of date and will need
						to be updated in next align()
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGITEM::markForRealignment
						// PP: this gets recurred down the contents hierarchy
	*/
	void markForRealignment(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::align
		Purpose 	: calculate alignment for the book's contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::align
						// PP: this gets recurred down the contents hierarchy
	*/
	virtual void align(void);



	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::effectsFinished
		Purpose 	: determine if all effects on the book and its contents have finished
		Parameters 	: 
		Returns 	: true if all effects on the book and its contents have finished, else false
		Info 		: this overrides PAGEITEM::effectsFinished, to check that the book's contents' effects are also finished
	*/
	bool effectsFinished(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::hasEffects
		Purpose 	: determine if the book or any of its contents have effects on them
		Parameters 	: 
		Returns 	: true if the book or any of its contents has an effect on it
		Info 		: this overrides PAGEITEM::hasEffects, to check if the book's contents have any effects
	*/
	bool hasEffects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::freeTempMemory
		Purpose 	: free-up temporary memory used by the item and its contents,
						eg. effects that have finished
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::freeTempMemory
	*/
	void freeTempMemory(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::drawBackgroundAndBorder
		Purpose 	: draw the book's background and border if it has them
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::drawBackgroundAndBorder
	*/
	virtual void drawBackgroundAndBorder(void);


public:

	friend void BookList_closeAndFreeButDontNull(TAG_BOOK* const book);

	friend void BookList_closeAndShutdown(TAG_BOOK& book);

	friend void BookList_remove(TAG_BOOK* const book);

	// PP: Data hiding?  Er...  ¿Que?!  Sorry Señor, me no speeka da Eenglish!

	MENU_SOUNDS*	menuSounds;						// NH: Samples to play within book
	
// PP: REMOUT: not used			float			scale;							// PP: scale factor used when drawing the book
	uint32			flags;							// PP: flags that will be useful to most BOOK subclasses
//	float			durationPageChange;				// PP: duration of page changes
//	float			durationOpen;					// PP: duration of open operation
//	float			durationClose;					// PP: duration of close operation


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setEffectChooser
		Purpose 	: set the pageitem effect chooser for this book to use
		Parameters 	: ptr to chooser to use
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setEffectChooser,
						to get all the book's contents that don't have choosers
						of their own to point back to this chooser.
						// PP: TODO: make this recursion optional?
	*/
	struct TAG_PAGEITEM* setEffectChooser(const struct PIECHOOSER* const chooser);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::startChapter
		Purpose 	: start adding pages to a new chapter
		Parameters 	: (opt/NULL=default)name of chapter
		Returns 	: 
		Info 		: 
	*/
	void startChapter(const char* const chapterName=NULL)
	{
		this->chapters.append(new BOOKCHAPTER(chapterName));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::gotoChapter
		Purpose 	: turn to the start of a specific chapter
		Parameters 	: index of chapter to which to turn
		Returns 	: 
		Info 		: 
	*/
	void gotoChapter(const int index)
	{
		this->gotoPageNumber(this->chapters[index].startPage, true);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::restrictToChapter
		Purpose 	: turn to the start of a specific chapter and disallow page turns to pages outside that chapter
		Parameters 	: index of chapter to which to turn
		Returns 	: 
		Info 		: 
	*/
	void restrictToChapter(const int index)
	{
		this->restrictingChapter= &this->chapters[index];
		this->gotoChapter(index);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::dontRestrictToChapter
		Purpose 	: allow free movement throughout the book, rather than restricting it to one chapter
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void dontRestrictToChapter(void)
	{
		this->restrictingChapter=NULL;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setAspectRatio
		Purpose 	: set the aspect ratio of the book
		Parameters 	: x dimension, y dimension
		Returns 	: 
		Info 		: PLEASE DON'T USE THIS UNLESS YOU KNOW WHAT YOU'RE DOING
	*/
	void setAspectRatio(const float xAspect, const float yAspect);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getNumPages
		Purpose 	: get the number of pages in the book
		Parameters 	: 
		Returns 	: the number of pages in the book!
		Info 		: 
	*/
	inline int getNumPages(void) const
	{
		return this->pages.getSize();
	}

	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getCurrentPageIndex
		Purpose 	: get the index of the current page
		Parameters 	: 
		Returns 	: the index of current page
		Info 		: 
	*/
	inline int getCurrentPageIndex(void) const
	{
		return this->currentPageIndex;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setBookRect
		Purpose 	: set the book's rectangle (and update its contents accordingly)
		Parameters 	: rectangle (as portion of target viewport), (opt/BKSCREEN_NOCHANGE)screen index(BKSCREEN_...)
		Returns 	: this
		Info 		: 
	*/
	struct TAG_BOOK* setBookRect(const RECTANGLE& rectIn, const int screenIndexIn=BKSCREEN_NOCHANGE)
	{
		// PP: BOOK::setRect currently handles both top-level rectangles and page-item rectangles, but that could well change
		return this->setRect(rectIn, screenIndexIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::TAG_BOOK
		Purpose 	: constructor
		Parameters 	: name
		Returns 	: new BOOK instance
		Info 		: 
	*/
	TAG_BOOK(const char* const nameIn);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::TAG_BOOK
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: new BOOK instance
		Info 		: 
	*/
	TAG_BOOK(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::TAG_BOOK
		Purpose 	: constructor; calls init
		Parameters 	: flag to start the book in the open state, (opt/NULL=not set) name
		Returns 	: new BOOK instance
		Info 		: 
	*/
	TAG_BOOK(const bool open, const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::privateUpdate
		Purpose 	: update the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGEITEM::update
	*/
	inline void privateUpdate(void)
	{
		this->update();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::draw
		Purpose 	: draw the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::draw, to check firstly that the book is visible
	*/
	inline void privateDraw(void)
	{
		this->draw();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::onInsert
		Purpose 	: notifies the book that it's being inserted into a page
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::onInsert
	*/
	 void onInsert(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::init
		Purpose 	: one-time init / reset for the book
		Parameters 	: (opt/false)flag to start the book in the open state
		Returns 	: 
		Info 		: 
	*/
	 void init(const bool open=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::findPage
		Purpose 	: get a the index and optionally pointer to the page with a specified name
		Parameters 	: name of page to find, ptr to ptr to point to to page if found
		Returns 	: index of page if found
		Info 		: 
	*/
	int findPage(const char* const pageName, const PAGE** const page=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::open
		Purpose 	: open the book; make it visible
		Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly
		Returns 	: 
		Info 		: 
	*/
	 virtual void open(const char* const pageName=NULL, const bool instant=false);


	 /*	--------------------------------------------------------------------------------
		Function 	: BOOK::openExclusive
		Purpose 	: open the book and have all other books close, until this one is closed
		Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly
		Returns 	: 
		Info 		: 
	*/
	void openExclusive(const char* const pageName=NULL, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::close
		Purpose 	: close the book; make it invisible
		Parameters 	: (opt/0.0f) delay in seconds before closing, (opt/false) whether or not to close instantly
		Returns 	: 
		Info 		: 
	*/
	virtual void close(const float delay=0.0f, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setScreen
		Purpose 	: tell the (top-level) book to which screen will be drawing
		Parameters 	: screen index (see EBookScreen enum)
		Returns 	: this
		Info 		: for automatic on-the-fly rectangle adjustment
	*/
	inline struct TAG_BOOK* setScreen(const int screenIndexIn)
	{
		this->screenIndex=screenIndexIn;

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::addPage
		Purpose 	: add a page to the book
		Parameters 	: (opt/NULL)custom update function ptr, (opt/NULL) name of page,
						(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
		Returns 	: ptr to page
		Info 		: 
	*/
	virtual PAGE* addPage(PageFunc const update=NULL, const char * const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::addPage
		Purpose 	: add a page to the book
		Parameters 	: name of page, (opt/NULL)custom update function ptr, 
						(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
		Returns 	: ptr to page
		Info 		: 
	*/
	PAGE* addPage(const char* const pageName, PageFunc const update=NULL, PageFunc const open=NULL, PageFunc const close=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::addPage
		Purpose 	: add a page to the book
		Parameters 	: page item to insert INTO the new page,  (opt/NULL)custom update function ptr, (opt/NULL) name of page,
						(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
		Returns 	: ptr to page
		Info 		: *** NOTE *** 'addPage(page=new PAGE("MYPAGE"))' is generally wrong - instead say 'page=addPage("MYPAGE")'
	*/
	PAGE* addPage(PAGEITEM* const pageItem, PageFunc const update=NULL, const char* const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::addPage
		Purpose 	: add a page to the book
		Parameters 	: index of string to insert, (opt/NULL)custom update function ptr, (opt/NULL) name of page,
						(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
		Returns 	: ptr to page
		Info 		: 
	*/
	PAGE* addPage(const EStringIndex stringIndex, PageFunc const update=NULL, const char* const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::removePage
		Purpose 	: remove a page from the book
		Parameters 	: ptr to page to remove, (opt/false)whether or not to delete the page as well as removing it
		Returns 	: ptr to page, or NULL if deleted
		Info 		: 
	*/
	PAGE* removePage(PAGE* const page, const bool deletePage);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::removePage
		Purpose 	: remove a page from the book
		Parameters 	: index of page to get, (opt/false)whether or not to delete the page as well as removing it
		Returns 	: ptr to page, or NULL if either not found or deleted
		Info 		: 
	*/
	PAGE* removePage(const int pageIndex, const bool deletePage=false)
	{
		return this->removePage(this->getPage(pageIndex), deletePage);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::removePage
		Purpose 	: remove a page from the book
		Parameters 	: name of page to get, (opt/false)whether or not to delete the page as well as removing it
		Returns 	: ptr to page, or NULL if either not found or deleted
		Info 		: 
	*/
	PAGE* removePage(const char* const pageName, const bool deletePage=false)
	{
		return this->removePage(this->findPage(pageName), deletePage);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getPage
		Purpose 	: get a pointer to the page at the specified 0-based index
		Parameters 	: index of page to get or -1 to return last page
		Returns 	: pointer to specified page, or NULL if not found
		Info 		: 
	*/
	inline PAGE* getPage(const int pageIndex = -1)
	{
		if (pageIndex ==-1)
		{
			return BOOK_GET_PAGE(BOOK_NUM_PAGES-1);
		}
		else
		{
			return BOOK_GET_PAGE(pageIndex);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getPage
		Purpose 	: get a pointer to the page with the specified name
		Parameters 	: name of page to get
		Returns 	: pointer to specified page, or NULL if not found
		Info 		: 
	*/
	inline PAGE* getPage(const char* const pageName)
	{
		return this->getPage(this->findPage(pageName));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getCurrentPage
		Purpose 	: get a pointer to the book's current page
		Parameters 	: 
		Returns 	: a pointer to the book's current page
		Info 		: 
	*/
	inline PAGE* getCurrentPage(void)
	{
		return this->currentPage;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getLastPage
		Purpose 	: get a pointer to the last page in the book (ie. the last one added), or the one 'offset' pages back from the last page
		Parameters 	: (opt/0)index offset from last page, eg. 1=page before last; 2=2 pages before last...
		Returns 	: a pointer to the last page in the book (ie. the last one added), or the one 'offset' pages back from the last page
		Info 		: 
	*/
	PAGE* getLastPage(const uint32 offset=0)
	{
		return this->getPage(this->getNumPages()-1-offset);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getItem
		Purpose 	: get a pointer to an item in the book
		Parameters 	: name of the item
		Returns 	: ptr to the specified item, or NULL if not found
		Info 		: 
	*/
	PAGEITEM* getItem(const char* const itemName);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getItem
		Purpose 	: get a pointer to the item containing a specified screen position ON THE CURRENT PAGE, or NULL if there isn't one at that position
		Parameters 	: x position, y position
		Returns 	: a pointer to the item containing a specified screen position ON THE CURRENT PAGE, or NULL if there isn't one at that position
		Info 		: this assumes that ye're drawing the book in 2D mode and with an identity object matrix
						this overrides PAGEITEM::getItem(x, y)
	*/
	PAGEITEM* getItem(const float xPos, const float yPos);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getTextbox
		Purpose 	: look within book's CURRENT page for a textbox holding a specified string
		Parameters 	: index of string to search for
		Returns 	: a pointer to the first textbox holding the specified string (or NULL if not found)
		Info 		: // PP: this overrides PAGEITEM::getTextbox
						// PP: Edam is a much better 'melting' cheese than Cheddar.  It's got a nicer texture and a nicer taste.  Try it grated in a toastie - you won't be disappointed!
	*/
	struct TAG_TEXTBOX* getTextbox(const EStringIndex string);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getNavigationCandidate
		Purpose 	: get a pointer to the item within this book which is
						the most suitable item to which to navigate, from a specified
						current position at a specified navigation angle
		Parameters 	: (in/out) ref to a NAVCANDIDATE_INFO structure (TextFX.h)
		Returns 	: a pointer to the item within this page which is the
						most suitable item to which to navigate
		Info 		: this overrides PAGEITEM::getNavigationCandidate
	*/
	TAG_PAGEITEM* getNavigationCandidate(NAVCANDIDATE_INFO& info);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::nextPage
		Purpose 	: turn to the next page (page at current page index + 1)
		Parameters 	: (opt/false)change page instantly
		Returns 	: ptr to current page (after the page change)
		Info 		: 
	*/
	virtual PAGE* nextPage(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::prevPage
		Purpose 	: turn to the previous page (page at current page index - 1)
		Parameters 	: (opt/false)change page instantly
		Returns 	: ptr to current page (after the page change)
		Info 		: 
	*/
	virtual PAGE* prevPage(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::gotoPage
		Purpose 	: go to the page with the specified name
		Parameters 	: name of page to turn to, whether or not to change page instanly
		Returns 	: 
		Info 		: 
	*/
	void gotoPage(const char* const pageName, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::gotoPageNumer
		Purpose 	: go to the page at the specified index
		Parameters 	: page number to turn to, whether or not to change page instantly
		Returns 	: true if the page number is valid, else false
		Info 		: page numbers are zero-based
	*/
	virtual bool gotoPageNumber(const int index, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::changeMode
		Purpose 	: change the mode of the page, eg. size
		Parameters 	: new mode to change to, (opt/0=don't set) duration of transition
		Returns 	: 
		Info 		: 
	*/	
	void changeMode(const int newMode, const float durationIn=0.0f);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::drawscreenRects
		Purpose 	: show the screen rects of all the items on the current page of the book
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void drawScreenRects(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::drawDebug
		Purpose 	: draw debug info for the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::drawDebug, to check firstly that the book is visible etc.
	*/
	inline void drawDebug(void)
	{
		if((this->flags&BKFLAG_OPEN)/* && !(this->flags&BKFLAG_CHANGINGPAGE)*/)
		{
			PAGEITEM::drawDebug();
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getRequiredHeight
		Purpose 	: get the minimum required height of the book
		Parameters 	: 
		Returns 	: minimum required height of book
		Info 		: 
	*/
	float getRequiredHeight(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getRequiredWidth
		Purpose 	: get the minimum required width of the book
		Parameters 	: 
		Returns 	: minimum required width of book
		Info 		: *** NEW TEST *** Hmmmm
	*/
	float getRequiredWidth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getSelectedItem
		Purpose 	: get the selected item of the book's current page
		Parameters 	: 
		Returns 	: the selected item of the book's current page, or NULL if N/A
		Info 		: 
	*/
	PAGEITEM* getSelectedItem(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::realign
		Purpose 	: realign items in the book
		Parameters 	: 
		Returns 	: 
		Info 		: COSTLY!
	*/
	void realign(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::toggleOpenClose
		Purpose 	: open the book if it's closed / close the book if it's open
		Parameters 	: 
		Returns 	: 
		Info 		: I might add normal open/close parms at some point...
	*/
	void toggleOpenClose(void)
	{
		if(this->flags&(BKFLAG_OPEN/*|BKFLAG_OPENING*/))
		{
			this->close();
		}
		else
		{
			this->open();
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::selectItem
		Purpose 	: make the book's current page exclusively select the specified item
		Parameters 	: ptr to item, or NULL to select the first item
		Returns 	: 
		Info 		: 
	*/
	inline void selectItem(PAGEITEM* const item)
	{
		if(this->currentPage == NULL) return;

		this->currentPage->selectItem(item);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::shutdown
		Purpose 	: tidy up this mess!  Your tea's almost ready!
		Parameters 	: X-Wings, Action Men, small die-cast sports cars, all back into the box.
		Returns 	: 
		Info 		: Aw Mum, do I have to?!  -Yes!  Do you want beans or gravy?!  -Gravy!  And beans!
						// PP: this overrides PAGEITEM::shutdown
	*/
	virtual void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::~TAG_BOOK
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_BOOK(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setName
		Purpose 	: set the name of the book
		Parameters 	: the name of the book!
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setName
	*/
	PAGEITEM* setName(const char* const nameIn);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setNavigation
		Purpose 	: enable/disable navigation for all the pages in the book
		Parameters 	: true to enable / false to disable
		Returns 	: this
		Info 		: 
	*/
	struct TAG_BOOK* setNavigation(const bool on);

	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setSounds
		Purpose 	: Set the sounds used by the book (also passed onto all child pages as default)
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setSounds(MENU_SOUNDS* menuSounds);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setController
		Purpose 	: Sets the controller able to navigate the page
		Parameters 	: logical index of the controller (ie. player index): zero-based
		Returns 	: 
		Info 		: -1 = global; 0 = pad 1; 1 = pad 2
	*/
	void setController(const char con);

	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setAutoDraw
		Purpose 	: Sets autoDraw. If set to false (true by default), when the book is in the book list it will NOT automatically draw
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	struct TAG_BOOK* setAutoDraw(const bool on);

	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::getAutoDraw
		Purpose 	: returns a flag to show if we want to automatically draw the book or not
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	bool getAutoDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::setOldSafeArea
		Purpose 	: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
		Parameters 	: old safe width portion, old safe height portion
		Returns 	: 
		Info 		: // PP: dirty fix for pre-BKFLAG_SAFEAREA books that have to line up with splash screens
						// PP: NOTE: When creating NEW books that have to line up with splash screens, clear BKFLAG_SAFEAREA!!!!
						// PP: REMOVE THIS METHOD AFTER TAZ
	*/
	struct TAG_BOOK* setOldSafeArea(const float oldSafeWidthPortionIn, const float oldSafeHeightPortionIn);


	/*	--------------------------------------------------------------------------------
		Function 	: BOOK::PS2BabelWorkaround_setRenderTargetDimensions
		Purpose 	: specify the dimensions of the render target to which we're about to draw a book
		Parameters 	: RT width, RT height
		Returns 	: 
		Info 		: // PP: TODO: GET TECH TEAM TO FIX THIS FOR CHRIST'S SAKE - THIS BUG HAS WASTED TEN MILLION TONNES OF MY TIME AND MADE MY CODE LOOK VERY SHIT AT THE SAME TIME!!!!!!!!!
						// PP: The dimensions you specify only get used by books with BKFLAG_2D but not with BKFLAG_SCREENADJUST
	*/
	static inline void PS2BabelWorkaround_setRenderTargetDimensions(const uint32 width, const uint32 height)
	{
		TAG_BOOK::PS2BabelWorkaround_renderTargetWidth=width;
		TAG_BOOK::PS2BabelWorkaround_renderTargetHeight=height;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: setFullScreenBackground
		Purpose 	: set the colour for the book's full-screen background
		Parameters 	: the colour for the book's full-screen background
		Returns 	: 
		Info 		: 
	*/
	void setFullScreenBackground(const COLOUR& col);


	/*	--------------------------------------------------------------------------------
		Function 	: setFullScreenBackground
		Purpose 	: specify whether or not the book's background is full-screen/full-splitscreen (normally it's not)
		Parameters 	: (opt/true) whether or not the book's background is full-screen/full-splitscreen
		Returns 	: 
		Info 		: 
	*/
	void setFullScreenBackground(const bool fullScreen=true);


}BOOK, _BOOK BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														MAIN FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_escapeCallback
	Purpose 	: handles text escape codes, eg. button glyphs, coloured text
	Parameters 	: reason code, info pointer
	Returns 	: 1 if the code is recognised and a glyph is to be used (even if it's an invisible dummy glyph), 0 otherwise
	Info 		: 
*/
int TextFX_escapeCallback(EBFontCallbackReason reason, TBFontCallbackInfo *info);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_init
	Purpose 	: one-time init for text fx - loads textures, that sort of thing
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_init(void);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setAlpha
	Purpose 	: sets the alpha for text and glphs - this should modulate with the alpha escape code(s) if any
	Parameters 	: alpha calue 0..255
	Returns 	: 
	Info 		: 
*/
void TextFX_setAlpha(const unsigned char alpha);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetAlpha
	Purpose 	: resets the alpha for text and glphs to the default value (255)
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetAlpha(void);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_vGrad
	Purpose 	: set up a vertical colour graduation for normaltext
	Parameters 	: rgb for top of characters, rgb for bottom
	Returns 	: 
	Info 		: 
*/
void TextFX_vGrad(const unsigned char r1, const unsigned char g1, const unsigned char b1,
					 const unsigned char r2, const unsigned char g2, const unsigned char b2);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_vGrad
	Purpose 	: set up a vertical colour graduation for normaltext
	Parameters 	: COLOUR for top of characters, COLOUR for bottom
	Returns 	: 
	Info 		: 
*/
void TextFX_vGrad(const COLOUR& col1, const COLOUR& col2);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setColour
	Purpose 	: set text colour
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_setColour(const unsigned char r, const unsigned char g, const unsigned char b);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setColour
	Purpose 	: set text colour
	Parameters 	: colour
	Returns 	: 
	Info 		: 
*/
void TextFX_setColour(const COLOUR& colour);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_select
	Purpose 	: select/deslect subsequent text
	Parameters 	: (opt/true)true to select, false to deselect
	Returns 	: 
	Info 		: 
*/
void TextFX_select(const bool selectIn=true);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetColour
	Purpose 	: resets the vert colours used for normal text
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetColour(void);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setClipWidth
	Purpose 	: sets the width of the rectangle within which characters are visble
	Parameters 	: 
	Returns 	: 
	Info 		: This is not the same as setting a format width!
*/
void TextFX_setClipWidth(const float width);


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetClipping
	Purpose 	: get rid of the clipping rectangle: no text is clipped
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetClipping(void);


#include "BookList.h"		// PP: List of books - handles drawing, update etc.
#endif // PP: ndef __H_TEXTFX
