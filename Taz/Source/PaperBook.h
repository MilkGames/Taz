// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PaperBook.h
//   Purpose : 'Paper' book type, ie. one that looks and acts like a real physical book
// Component : Taz
//		Info : Some things have just gotta be done!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __PAPERBOOK_H__
#define __PAPERBOOK_H__

#include "Textfx.h"							// PP: Text effects, eg. button glyphs, coloured text

extern TEXTSTYLE	TS_paperBookHeading;	// PP: text style for paper book headings
extern TEXTSTYLE	TS_paperBookBody;		// PP: text style for paper book body text


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAPERBOOK

									(Paper) book simulation, with page turning effects.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define PAPERPAGE_NUM_LINKS					30					// PP: number of points forming the curve of each page

struct PAPERPAGELINK
{
	VEC						pos;								// PP: 2D position of the link (on the cross-section of the page)
	VEC						lastPos;							// PP: last update's 2D position of the link (on the cross-section of the page)
	float					realAng;							// PP: (signed radians) real angle of the link
	float					relativeAng;						// PP: (signed radians) angle of the link relative to the link before it (before=closer to spine)
	VEC						normal;								// PP: 2D unit normal of the link (will be used for lighting effect(s))

	// PP: the distribution of momentum over the paper page is assumed to be cancelled by air resistance - reasonable, no?

}BALIGN16;


enum EPaperPageState
{
	PAPERPAGESTATE_RESTINGLEFT,									// PP: page is entirely at rest on the left-hand stack
	PAPERPAGESTATE_RESTINGRIGHT,								// PP: page is entirely at rest on the right-hand stack
	PAPERPAGESTATE_TURNINGLEFT,									// PP: page is turning to the left (forward through the book)
	PAPERPAGESTATE_TURNINGRIGHT,								// PP: page is turning to the right (backward through the book)
	PAPERPAGESTATE_FLOATINGLEFT,								// PP: page has finished being turned to the left, but the paper has not yet come to rest
	PAPERPAGESTATE_FLOATINGRIGHT,								// PP: page has finished being turned to the right, but the paper has not yet come to rest
};

// PP: Link rest data
enum
{
	LRI_POSX,
	LRI_POSY,
	LRI_RELANG,
	LRI_ANG,
	LRI_NORMX,
	LRI_NORMY,

	LRI_NUM_FIELDS
};

struct PAPERPAGEINFO// PP: 'PAPERPAGE' just sounded far too much like a PAGE subclass
{
private:
																	
	float				oldSpineRestAngle;							// PP: (signed radians) angle at which the spine hinge used to rest (before the current turn) (-HALFPI+PAPERPAGE_MIN_SPINE_REST_ANGLE .. HALFPI-PAPERPAGE_MIN_SPINE_REST_ANGLE)
	float				spineRestAngle;								// PP: (signed radians) angle at which the spine hinge will rest (-HALFPI+PAPERPAGE_MIN_SPINE_REST_ANGLE .. HALFPI-PAPERPAGE_MIN_SPINE_REST_ANGLE)
																	
	EPaperPageState		state;										// PP: state of the paper page (PAPERPAGESTATE_...)

	static const float	linkWidth;
	static const float	linkBendResistance;
	static const float	linkWeight;

//#ifdef PAPERBOOK_READ_LINK_REST_POSITIONS
	// PP: x pos, y pos, relative ang, real ang, normal x, normal Y
	static const float	linkRestInfo[PAPERPAGE_NUM_LINKS][LRI_NUM_FIELDS];	// PP: array of link resting positions and angles, written out by the game ifndef PAPERBOOK_READ_LINK_REST_POSITIONS
//#endif// PP: def #ifndef PAPERBOOK_READ_LINK_REST_POSITIONS

public:

	float				floatSpeed;									// PP: speed value for paper floating

	float				height;										// PP: total of all links' Y positions

	uint32				heightIndex;								// PP: very hard to explain

	PAPERPAGELINK		links[PAPERPAGE_NUM_LINKS];					// PP: the curve links that allow the page to page to bend: links[0] is the spine hinge

	float				leftShadowCastDistance;
	float				rightShadowCastDistance;

	float				shadowDistance;
	

	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::init
		Purpose 	: initialise the paper model for this page
		Parameters 	: new resting angle for the page's spine hinge, whether or not the leaf is to float into position rather than starting at rest
		Returns 	: 
		Info 		: sets up initial link positions, and a zero angle on all but the spine hinge, which rests
	*/
	void init(const float spineRestAngleIn, bool floating);


	inline bool resting(void) const
	{
		return ((this->state == PAPERPAGESTATE_RESTINGLEFT)||(this->state == PAPERPAGESTATE_RESTINGRIGHT));
	}


	inline bool restingLeft(void) const
	{
		return (this->state == PAPERPAGESTATE_RESTINGLEFT);
	}


	inline bool restingRight(void) const
	{
		return (this->state == PAPERPAGESTATE_RESTINGRIGHT);
	}


	inline bool floating(void) const
	{
		return ((this->state == PAPERPAGESTATE_FLOATINGLEFT)||(this->state == PAPERPAGESTATE_FLOATINGRIGHT));
	}


	inline bool floatingLeft(void) const
	{
		return (this->state == PAPERPAGESTATE_FLOATINGLEFT);
	}


	inline bool floatingRight(void) const
	{
		return (this->state == PAPERPAGESTATE_FLOATINGRIGHT);
	}


	inline bool turningLeft(void) const
	{
		return (this->state == PAPERPAGESTATE_TURNINGLEFT);
	}


	inline bool turningRight(void) const
	{
		return (this->state == PAPERPAGESTATE_TURNINGRIGHT);
	}


	inline void floatPlease(void)
	{
		if(this->spineRestAngle < 0.0f)
		{
			this->state = PAPERPAGESTATE_FLOATINGLEFT;
		}
		else
		{
			this->state = PAPERPAGESTATE_FLOATINGRIGHT;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::rest
		Purpose 	: go into a resting state
		Parameters 	: 
		Returns 	: 
		Info 		: ifndef PAPERBOOK_READ_LINK_POSITIONS, this writes out link positions
	*/
	void rest(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::turn
		Purpose 	: turn the page
		Parameters 	: new resting angle for the page's spine hinge
		Returns 	: 
		Info 		: 
	*/
	void turn(const float spineRestAngleIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::draw
		Purpose 	: draw the page
		Parameters 	: which side to draw (0/1), is the page upright (portrait)?,
						number of floating leaves above the right-hand leaf stack
		Returns 	: 
		Info 		: 
	*/
	void draw(const uchar side, const bool uprightPage, const uint32 numRightFloatingLeaves) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::drawDebug
		Purpose 	: debug-draw the page
		Parameters 	: colour in which to debug-draw the page, (opt/false)whether or not to debug-draw the contents of the page (no need to draw them if they're hidden)
		Returns 	: 
		Info 		: // PP: drawContents not used yet
	*/
	void drawDebug(const COLOUR& colourIn, const bool drawContents=false) const;


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERPAGEINFO::update
		Purpose 	: update the paper model for this page
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: page contents get updated completely independently of the paper model, so that's cool
	*/
	void update(void);


}BALIGN16;


extern CAMERAINFO				PaperBook_camera;

// PP: maximum length of a filename for a paperbook background
#define PAPERBOOK_BACKGROUND_NAME_LENGTH		64
// NH: "extras\\" removed as it's defined in splashscreen.cpp so different systems res's can be handled
#define PAPERBOOK_DEFAULT_BACKGROUND			"guide.bmp"

// PP: define a type for pointers to paperbook close callbacks
typedef void(*PaperBookCloseCallback)(struct TAG_PAPERBOOK* const paperBook);

// NH: define a type for pointers to paperbook page change callbacks
typedef void(*PaperBookChangePageCallback)(void);

// PP: PAPERBOOK ORIENTATIONS	// PP: NOTE: I changed this enum because it made no sense to me before!
enum PBOrientation
{
	PBO_PORTRAIT,				// PP: portrait pages turned left and right (eg. tutorial)
	PBO_LANDSCAPE				// PP: landscape pages turned up and down (eg. gallery)
};

// PP: button(s) to use to close the paper book
enum PBCloseButton
{
	PBCLOSEBUTTON_NONE		=0,														// PP: no close button
	PBCLOSEBUTTON_FORWARD	=1<<0,													// PP: 'ok' buttons close the book
	PBCLOSEBUTTON_BACK		=1<<1,													// PP: 'cancel' buttons close the book
	PBCLOSEBUTTON_ANY		=(PBCLOSEBUTTON_FORWARD|PBCLOSEBUTTON_BACK)				// PP: 'ok' or 'cancel' buttons close the book
};

// PP: default button(s) to use to close a paper book
#define PAPERBOOK_DEFAULT_CLOSE_BUTTON		PBCLOSEBUTTON_ANY

// PP: flags specific to paper books
enum PBFlag
{
	PBFLAG_ICONONLY			=1<<0,// PP: indicates that the paperbook only holds big icons, ie. it's the Taz gallery book!
};

// PP: default paper book flags
#define DEFAULT_PBFLAGS			0

typedef struct TAG_PAPERBOOK	:	public BOOK// PP: why not
{

private:

	LINKEDLIST<PAPERPAGEINFO>	paperPages;											// PP: linked list of paper-page info
	
	static bool					cameraState;										// PP: you got a problem with that name?!
																				
	float						spineHingeRestAngleStep;							// PP: angle gap between spine hinge rest angles

	PAGE*						leftTopPage;
	PAGE*						rightTopPage;

	char						backgroundName[PAPERBOOK_BACKGROUND_NAME_LENGTH];	// PP: name of background to use for the paperbook
	SPLASHSCREEN				background;											// PP: background to use for the paperbook

	PBOrientation				orientation;										// NH: The type of book this is

	PaperBookCloseCallback		closeCallback;										// PP: ptr to function to be called when this paper book closes (returning the game to GAMESTATE_INGAME) 
	
	PaperBookChangePageCallback	firstPageCallback;									
	PaperBookChangePageCallback	lastPageCallback;									
	PaperBookChangePageCallback	centralPageCallback;									

	uint32						numRightFloatingLeaves;								// PP: number of floating leaves above the right-hand stack

	PBCloseButton				closeButton;										// PP: button(s) that can be used to close the paper book


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::getPaperPage
		Purpose 	: get a ptr to the paper page info at the specified index
		Parameters 	: index of page
		Returns 	: a ptr to the paper page info at the specified index!
		Info 		: 
	*/
	inline PAPERPAGEINFO* getPaperPage(const int index)
	{
		return this->paperPages.get(index);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::getNumPaperPages
		Purpose 	: find out how many paper pages (leafs) are in the book
		Parameters 	: 
		Returns 	: how many paper pages (leafs) are in the book!
		Info 		: 
	*/
	inline int getNumPaperPages(void) const
	{
		return this->paperPages.getSize();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::turnPaperPageToRight
		Purpose 	: turn the paper page at the specified index to the right (to move backwards through the book)
		Parameters 	: index of page to turn to right
		Returns 	: 
		Info 		: 
	*/
	void turnPaperPageToRight(const int index);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::turnPaperPageToLeft
		Purpose 	: turn the paper page at the specified index to the left (to move forwards through the book)
		Parameters 	: index of page to turn to left
		Returns 	: 
		Info 		: 
	*/
	void turnPaperPageToLeft(const int index);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::iconOnly
		Purpose 	: determine if the paper book is icon-only (ie. gallery)
		Parameters 	: 
		Returns 	: true if the paper book is icon-only (ie. gallery), else false
		Info 		: 
	*/
	inline bool iconOnly(void) const
	{
		return ((this->paperBookFlags & PBFLAG_ICONONLY) != 0);
	}


protected:


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::startDraw
		Purpose 	: set up conditions needed to draw the book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::startDraw, which doesn't really apply to paper books
	*/
	inline void startDraw(void)
	{
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::endDraw
		Purpose 	: do whatever needs to be done after drawing the paper book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::endDraw
	*/
	inline void endDraw(void)
	{
	}


public:

	// PP: public for ease of access - no point getting anal at this late stage
	uchar						paperBookFlags;										// PP: combination of PBFlag values (PBFLAG_...)


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setCloseButton
		Purpose 	: specify the button(s) that can be used to close the paper book
		Parameters 	: PBCloseButton (PBCLOSEBUTTON_) value indicating the button(s) that can be used to close the paper book
		Returns 	: 
		Info 		: // PP: It's a long way to T'Prairie
	*/
	void setCloseButton(const PBCloseButton button)
	{
		this->closeButton=button;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::cameraActive
		Purpose 	: find out if the paperbook camera is currently active (ie. if a paperbook is drawing)
		Parameters 	: 
		Returns 	: true if the paperbook camera is currently active (ie. if a paperbook is drawing), else false
		Info 		: 
	*/
	static inline bool cameraActive(void)
	{
		// TP: Sorry Phil, had to add the TAG_ to get this to work with the PS2 compiler
		return TAG_PAPERBOOK::cameraState;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::getCamera
		Purpose 	: get a ptr to the camera used to draw paperbooks
		Parameters 	: 
		Returns 	: a ptr to the camera used to draw paperbooks!
		Info 		: 
	*/
	static inline CAMERAINFO* getCamera(void)
	{
		return &(PaperBook_camera);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setBackground
		Purpose 	: specify the background image for the paperbook
		Parameters 	: name of background (full name of a splash screen, eg. "extras\\bookcover.bmp")
		Returns 	: 
		Info 		: loads the background from the level pak current when the book OPENS;
						Therefore it IS safe to call this before the level gets loaded or anything
	*/
	void setBackground(const char* const backgroundNameIn)
	{
		STRNCPY(this->backgroundName, backgroundNameIn, PAPERBOOK_BACKGROUND_NAME_LENGTH);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::init
		Purpose 	: one-time init / reset for the book
		Parameters 	: (opt/false)flag to start the book in the open state
		Returns 	: 
		Info 		: this overrides BOOK::init
	*/
	 void init(const bool open=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::addPage
		Purpose 	: add a page to the book
		Parameters 	: (opt/NULL)custom update function ptr, (opt/NULL) name of page,
						(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
		Returns 	: ptr to page
		Info 		: this overrides BOOK::addPage to add the accompanying paper info for the page
	*/
	PAGE* addPage(PageFunc const update=NULL, const char* const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL);

	PAGE* addPage(const char* const pageName, PageFunc const update=NULL, PageFunc const open=NULL, PageFunc const close=NULL){return BOOK::addPage(pageName, update, open, close);}
	PAGE* addPage(PAGEITEM* const pageItem, PageFunc const update=NULL, const char* const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL){return BOOK::addPage(pageItem, update, pageName, open, close);}
	PAGE* addPage(const EStringIndex stringIndex, PageFunc const update=NULL, const char* const pageName=NULL, PageFunc const open=NULL, PageFunc const close=NULL){return BOOK::addPage(stringIndex, update, pageName, open, close);}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::TAG_PAPERBOOK
		Purpose 	: constructor
		Parameters 	: name
		Returns 	: new PAPERBOOK instance
		Info 		: this overloads BOOK::TAG_BOOK
	*/
	inline TAG_PAPERBOOK(const char* const nameIn)
	{
		this->init();
		this->setName(nameIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::TAG_PAPERBOOK
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: new PAPERBOOK instance
		Info 		: this overloads BOOK::TAG_BOOK
	*/
	inline TAG_PAPERBOOK(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::TAG_PAPERBOOK
		Purpose 	: constructor; calls init
		Parameters 	: flag to start the book in the open state, (opt/NULL=not set) name
		Returns 	: new PAPERBOOK instance
		Info 		: this overloads BOOK::TAG_BOOK
	*/
	inline TAG_PAPERBOOK(const bool open, const char* const nameIn=NULL)
	{
		this->init(open);

		if(nameIn != NULL)
		{
			this->setName(nameIn);
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::mainDraw
		Purpose 	: the core of the draw method - actually draw the book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::mainDrawDebug
		Purpose 	: the core of the debug-draw method - actually debug-draw the paper book
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::mainDrawDebug
	*/
	void mainDrawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::update
		Purpose 	: update the book and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::update
						// PP: NOTE!: Paper books don't call update() on their pages!
						// PP: (they tend to hold simple content and therefore don't need to)
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::open
		Purpose 	: open the book; make it visible
		Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly,
						(opt/true[?!])whether or not to do a very costly book realignment as we open
		Returns 	: 
		Info 		: this overrides BOOK::open to calculate the stacking of the paper pages (maybe move this process?)
	*/
	 void open(const char* const pageName=NULL, const bool instant=false, const int realign=true);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::close
		Purpose 	: close the book; make it invisible
		Parameters 	: (opt/0.0f) delay in seconds before closing, (opt/false) whether or not to close instantly
		Returns 	: 
		Info 		: this overrides BOOK::close to turn off the PAPERBOOK camera
	*/
	void close(const float delay=0.0f, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::gotoPageNumer
		Purpose 	: go to the page at the specified index
		Parameters 	: page number to turn to, whether or not to change page instantly
		Returns 	: true if the page index is valid, else false
		Info 		: this overrides BOOK::gotoPageNumber; page numbers are zero-based
	*/
	bool gotoPageNumber(const int index, const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::nextPage
		Purpose 	: turn to the next leaf
		Parameters 	: (opt/false)change page instantly (currently ignored by this overload)
		Returns 	: ptr to current page (after the page change)
		Info 		: this overrides BOOK::nextPage, to skip a page if nec, to ensure a leaf turn
	*/
	PAGE* nextPage(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::prevPage
		Purpose 	: turn to the previous page leaf
		Parameters 	: (opt/false)change page instantly (currently ignored by this overload)
		Returns 	: ptr to current page (after the page change)
		Info 		: this overrides BOOK::nextPage, to skip a page if nec, to ensure a leaf turn
	*/
	PAGE* prevPage(const bool instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::shutdown
		Purpose 	: shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::shutdown
	*/
	void shutdown(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::~TAG_PAPERBOOK
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides BOOK::~TAG_BOOK 
	*/
	~TAG_PAPERBOOK(void)
	{
		this->shutdown();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::getPaperPageIndex
		Purpose 	: get the index of the paper page (leaf) that holds the page at the specified index
		Parameters 	: index of page
		Returns 	: index of the paper page (leaf) that holds the page at the specified index!
		Info 		: 
	*/
	static inline int getPaperPageIndex(const int index)
	{
		return (index+1)/2;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::pauseUpdateFunc
		Purpose 	: custom pause update function used while a paperbook is open
		Parameters 	: ptr to paperbook that is open
		Returns 	: 
		Info 		: must match the PauseUpdateFunc typedef (main.h)
	*/
// PP: REMOUT: OOD	static void pauseUpdateFunc(const int32 parameter);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setOrientation
		Purpose 	: set the orientation of the book
		Parameters 	: orientation for the book (PBO_PORTRAIT/PBO_LANDSCAPE)
		Returns 	: 
		Info 		: 
	*/
	void setOrientation(const PBOrientation orientationIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setCloseCallback
		Purpose 	: specifies a function to be called when the paperbook closes (returning the game to GAMESTATE_INGAME)
		Parameters 	: ptr to close callback (must match PaperBookCloseCallback typedef, PaperBook.h)
		Returns 	: 
		Info 		: // PP:	A 'li' is a Chinese measure of weight (one thousandth of a liang),
								also a Chinese measure of distance (rather more than one third of a mile),
								also a very useful word in Scrabble!
	*/
	void setCloseCallback(PaperBookCloseCallback const closeCallbackIn)
	{
		// PP: kinda important
		this->closeCallback=closeCallbackIn;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setFirstPageCallback
		Purpose 	: specifies a function to be called when the paperbook turns to the first page
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setFirstPageCallback(PaperBookChangePageCallback const firstPageCallbackIn)
	{
		this->firstPageCallback=firstPageCallbackIn;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setLastPageCallback
		Purpose 	: specifies a function to be called when the paperbook turns to the last page
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setLastPageCallback(PaperBookChangePageCallback const lastPageCallbackIn)
	{
		this->lastPageCallback=lastPageCallbackIn;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: PAPERBOOK::setCentralPageCallback
		Purpose 	: specifies a function to be called when the paperbook turns to a central page
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void setCentralPageCallback(PaperBookChangePageCallback const centralPageCallbackIn)
	{
		this->centralPageCallback=centralPageCallbackIn;
	}


}PAPERBOOK BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL PAPERBOOK FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_draw
	Purpose 	: draw the current paper book
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_draw(void);


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_update
	Purpose 	: update the current paper book
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_init
	Purpose 	: initalise stuff for paper books
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_init(void);


#endif// PP: ndef __PAPERBOOK_H__