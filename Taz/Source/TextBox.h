// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : TextBox.h
//   Purpose : Text box page-item - displays a text string
// Component : Taz
//		Info : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_TEXTBOX
#define __H_TEXTBOX

#define DONT_INCLUDE_PRINTBOX
#include "Textfx.h"					// PP: Text effects, eg. button glyphs, coloured text
#undef DONT_INCLUDE_PRINTBOX


#define TEXTFX_DEFAULT_RED		0				// PP: default red value for normal text
#define TEXTFX_DEFAULT_GREEN	0				// PP: default green value for normal text
#define TEXTFX_DEFAULT_BLUE		0				// PP: default blue value for normal text
#define TEXTFX_DEFAULT_ALPHA	128				// PP: default alpha value for normal text and glyphs


// PP: Text-printing settings - these can be specified by text boxes;
// PP: also some of them can be started and ended using escape codes in the text strings themselves
enum ETextFlag
{
	TEXTFX_SELECTED			=1<<0,
	TEXTFX_TITLE			=1<<1,
	TEXTFX_SMALL_TITLE		=1<<2,
	TEXTFX_CREDITS_TITLE	=1<<3,
	TEXTFX_INFO				=1<<4,
	TEXTFX_CONTROL			=1<<5,
	TEXTFX_LARGE_CONTROL	=1<<6,
	TEXTFX_TEMP				=1<<7,
	TEXTFX_DISABLED			=1<<8,
	TEXTFX_ITALIC			=1<<9,
};

// PP: current text-printing settings
struct TEXTSETTINGS
{
	int32						flags;			// PP: styling flags (combination of ETextFlag values)
	uchar						alpha;			// PP: global alpha value for text
	COLOUR						vertColours[4];	// PP: colours (including alpha) of each of the 4 verts in a character
	bool						clipLR;			// PP: is left/right text clipping enabled?
	float						clipLeft;		// PP: left clipping position
	float						clipRight;		// PP: right clipping position


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTSETTINGS::TEXTSETTINGS
		Purpose 	: constructor; initialises the new instance
		Parameters 	: 
		Returns 	: new TEXTSETTINGS instance
		Info 		: 
	*/
	TEXTSETTINGS(void);


}BALIGN16;

// PP: text-printing settings, used by TextFX_escapeCallback
extern TEXTSETTINGS					textSettings;


// PP: OOPS!  don't ask ok?
// PP: The intentions were good.  It was an effort to save memory, but it turned out to be more trouble than it was worth.
#define PRINTBOX					TAG_TEXTBOX


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TEXT STYLES
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct TEXTSTYLE
{
	float		fontSize;					// PP: 1.0f is natural size
	uchar		italic				:1;		// PP: style uses italic
	uchar		vGrad				:1;		// PP: style uses vertical colour gradient
	uchar		outline				:1;		// PP: style uses outlines
	float		outlineThickness;			// PP: thickness of outline
	COLOUR		colour1;					// PP: colour at top of v-grad
	COLOUR		colour2;					// PP: colour at bottom of v-grad
	COLOUR		outlineColour;				// PP: colour of outline


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTSTYLE::init
		Purpose 	: default initialisation; sets the style up as TS_normal
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTSTYLE::TEXTSTYLE
		Purpose 	: default constructor; calls init
		Parameters 	: 
		Returns 	: new TEXTSTYLE instance
		Info 		: 
	*/
	TEXTSTYLE(void)
	{
		this->init();
	}


}BALIGN16;


// PP: commonly used preset text styles (initialised at the beginning of the game by initTextStyles)...

extern TEXTSTYLE		TS_normal;
extern TEXTSTYLE		TS_title;
extern TEXTSTYLE		TS_smallTitle;
extern TEXTSTYLE		TS_creditsTitle;
extern TEXTSTYLE		TS_info;
extern TEXTSTYLE		TS_control;
extern TEXTSTYLE		TS_largeControl;


/*	--------------------------------------------------------------------------------
	Function 	: initTextStyles
	Purpose 	: initialise commonly-used preset text styles
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initTextStyles(void);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													TEXTBOX page item type

											A text box holds text and formatting data.

												book > pages > rows > *ITEMS*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// NH: Text style to use for text box
/* PP: REMOUT: OOD	enum ETextStyle
{
	TEXTSTYLE_NORMAL,										// NH: Normal style (Default)
	TEXTSTYLE_TITLE,										// NH: Title style
	TEXTSTYLE_SMALL_TITLE,									// NH: Style for a pages subtitle (under title)
	TEXTSTYLE_CREDITS_TITLE,								// NH: Credits Title style
	TEXTSTYLE_INFO,											// NH: Info style (for text equivalent to tool tips in Windows)
	TEXTSTYLE_CONTROL,										// NH: Control Style (for text specifying which buttons/keys to use)
	TEXTSTYLE_LARGE_CONTROL,								// NH: Large Control Style (for LARGE text specifying which buttons/keys to use)
	TEXTSTYLE_SUBTITLE,										// NH: Subtitle text (for in game subtitles)
//	TEXTSTYLE_DISABLED,										// NH: Text is disabled
	TEXTSTYLE_BOUNTY										// PP: Text is in the style of a bounty display
};*/

// PP: possible character-cases for a TEXTBOX
enum TBCase
{
	TBCASE_NONE,											// PP: case is unchanged from original string
	TBCASE_LOWER,											// PP: lower case
	TBCASE_UPPER,											// PP: UPPER CASE
	TBCASE_TITLE,											// PP: Title Case
	TBCASE_SENTENCE											// PP: Sentence case
};

#define NOSTRING					((EStringIndex)-1)	// PP: value representing no string index

// PP: text box flags
enum
{	
	TBFLAG_NONE						=0,						// PP: no flags
	TBFLAG_VGRAD					=1,						// PP: text has a vertical colour gradient
	TBFLAG_SCRIBBLE					=1<<1,					// PP: text has a shakey scribble thing behind it
	TBFLAG_OUTLINE					=1<<2,					// PP: text has an outline
	TBFLAG_DROPSHADOW				=1<<3,					// PP: text has a drop-shadow **NOT SUPPORTED YET**
//	TBFLAG_TITLE					=1<<4,					// NH: text is of title type
//	TBFLAG_CREDITS_TITLE			=1<<5,					// NH: text is of credits title type
//	TBFLAG_INFO						=1<<6,					// NH: text is of info type
//	TBFLAG_CONTROL					=1<<7,					// NH: text is of control type
	TBFLAG_DISABLED					=1<<8,					// NH: text is disabled
	TBFLAG_ITALIC					=1<<9,					// PP: text is italic
//	TBFLAG_SMALL_TITLE				=1<<10,					// NH: text is of small title type
//	TBFLAG_LARGE_CONTROL			=1<<11,					// NH: text is of a large control type
	TBFLAG_CONCEALED				=1<<12,					// NH: Concealed/Hidden text (greyed out, replaced with questionmarks and unselectable)
	TBFLAG_DONTPRINTLITE			=1<<14,					// PP: don't use lite printing

	// PP: default flags for text box text
	TBFLAG_DEFAULTS					=0// PP: REMOUT: OOD	(TBFLAG_VGRAD|TBFLAG_OUTLINE|TBFLAG_ITALIC),
};

#define TEXTFX_STRING_LENGTH		256						// PP: I'm told that Excel imposes a limit of (255?) characters per string, so this should be fine

// PP: page item flags for any textbox - determine how the subclass works as a page item// PP: NOT USED YET
#define TEXTBOX_PIFLAGS				(PIFLAG_STRETCH_X|PIFLAG_STRETCH_Y)

#define TEXTBOX_MAX_ALPHA			128						// PP: maximum allowable alpha value for text

// PP: default font size for text boxes
#define TEXTBOX_DEFAULT_FONT_SIZE	1.0f

typedef struct TAG_TEXTBOX	:	public PAGEITEM
{
	friend struct			PIECHOOSER;						// PP: pageitem effect choosers also need access to things that I wouldn't want the client using
	friend struct			MAINPIECHOOSER;					// PP: subclass of PIECHOOSER
	friend struct			MEMCARDPIECHOOSER;				// TP: Mmmm slidey
	friend struct			WANTEDPIECHOOSER;				// TP: Mmmm slidey
	friend struct			PAGEITEMEFFECT;
	friend struct			TEXT_PIE;						// PP: subclass of PAGEITEMEFFECT
	friend struct			PIE_ZOOM;

	// PP: the text-printing callback knows things about textboxes that I wouldn't want anyone else to poke about with
	friend int				TextFX_escapeCallback(EBFontCallbackReason reason, TBFontCallbackInfo *info);


private:

	TBFont*					font;							// PP: font
	TBFontFormatting		format;							// PP: text formatting info
	bool					hasEscapeCodes;					// PP: text contains one or more escape codes (this is set by TEXTBOX::findEscapeCodes)
															
	void					*ppArgPtr[6];					// CPW: list of arg pointers
	int						numArgPtr;						// CPW: number of arg pointers
															
	EStringIndex			stringIndex;					// PP: string-table index of string held by the textbox, or NOSTRING if it's not in the string table
	ushort					text[TEXTFX_STRING_LENGTH];		// PP: wide-character string
	int						textLen;						// CPW: length of string (saves calling strlen)
															
	// PP: REMOUT: OOD		ETextStyle				style;							// PP: style of text, eg. heading, body, footnote

	TEXTSTYLE*				style;							// PP: ptr to text style structure to use, eg. title, info, control...

	TBCase					characterCase;					// PP: Case for text (see TBCase enum)	// PP: 'case' is a keyword :o(


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::updateActualFontSize
		Purpose 	: update the textbox's actual font size, based on its normal font size and its style
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateActualFontSize(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getWrap
		Purpose 	: determine if the textbox has wrapping enabled
		Parameters 	: 
		Returns 	: true if the textbox has wrapping enabled, else false
		Info 		: 
	*/
	inline bool getWrap(void) const
	{
		return (this->format.wrap != 0);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::quickPrint
		Purpose 	: determine if the textbox can be printed using bdPrintFontLite rather than the full bdPrintFont
		Parameters 	: 
		Returns 	: true if the textbox can be printed using bdPrintFontLite rather than the full bdPrintFont, else false
		Info 		: bdPrintFontLite is much faster than the full bdPrintFont
	*/
	inline bool quickPrint(void) const
	{
		if(this->flags & TBFLAG_DONTPRINTLITE) return false;
		return !(this->getWrap() || this->hasEscapeCodes);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::findEscapeCodes
		Purpose 	: search the text for escape codes
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void findEscapeCodes(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::updateAfterTextChange
		Purpose 	: perform any update that needs to be done in response the the text changing
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateAfterTextChange(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::drawStage
		Purpose 	: perform one of the textbox's draw stages
		Parameters 	: final text to print, stage to draw (ETextRenderStage TEXTFX_...)
		Returns 	: 
		Info 		: 
	*/
	void drawStage(const ushort* const finalText, const ETextRenderStage stageIn);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getOutlineWidth
		Purpose 	: get the width of the textbox's outline (assuming that it does have an outline)
		Parameters 	: 
		Returns 	: the width of the textbox's outline (assuming that it does have an outline)!
		Info 		: I didn't want to give per-textbox control over this, but different styles may use different outline thicknesses if they want
	*/
	float getOutlineWidth(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getOutlineColour
		Purpose 	: get the colour of the textbox's outline (assuming that it does have an outline)
		Parameters 	: 
		Returns 	: ref to the colour of the textbox's outline (assuming that it does have an outline)!
		Info 		: I didn't want to give per-textbox control over this, but different styles may use different outline colours if they want
	*/
	COLOUR& getOutlineColour(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::chooseEffect
		Purpose 	: choose and initialise a PAGEITEMEFFECT for this textbox
		Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
		Returns 	: 
		Info 		: this overrides PAGEITEM::chooseEffect
	*/
	virtual void chooseEffect(const PIEMode mode, const bool recur);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getStringIndex
		Purpose 	: get the string index held by this item (if it does hold a string)
		Parameters 	: 
		Returns 	: the EStringIndex held by the item
		Info 		: // PP: this overrides PAGEITEM::getStringIndex
	*/
	inline EStringIndex getStringIndex(void)
	{
		return this->stringIndex;
	}


protected:

	float					fontSize;					// PP: base font size (font size if text style is normal)
	float					actualFontSize;				// PP: actual font size (taking into account style)


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::applyFontSize
		Purpose 	: apply the text box's (real/actual/final) font size into the current object matrix
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void applyFontSize(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::startDraw
		Purpose 	: set up conditions needed to draw the textbox (eg. apply item's scale & position to object matrix)
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::startDraw to apply actual-font scale on top of draw scale
	*/
	virtual void startDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::mainDraw
		Purpose 	: the core of the draw method - actually draw the textbox
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::mainDrawDebug
		Purpose 	: the core of the debug-draw method - actually debug-draw the textbox
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDrawDebug
	*/
	virtual void mainDrawDebug(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getTextRect
		Purpose 	: get the rectangle into which the text must be fitted
		Parameters 	: 
		Returns 	: the rectangle into which the text must be fitted
		Info 		:  
	*/
	virtual inline RECTANGLE getTextRect(void) const
	{
		return this->optRect;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setRect
		Purpose 	: set the item's rectangle
		Parameters 	: new rectangle
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGEITEM::setRect
	*/
	PAGEITEM* setRect(const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::align
		Purpose 	: calculate alignment for the textbox
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::align
	*/
	void align(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setLanguage
		Purpose 	: set the language of the textbox
		Parameters 	: new language of the textbox (BLANGUAGEID_...)
		Returns 	: 
		Info 		: this overrides PAGEITEM::setLanguage, to re-fetch the box's text, in the specified language, from the string table
	*/
	void setLanguage(const EBLanguageID languageIn);


public:

	int						drawOffset;					// CMD: draw offset for selected text overdraw
	ETextOffsetDirection	drawOffsetDirection[2];
	float					clock;						// CMD: clock used for scribble
	float					mexicanClock;				// CMD: clock used for mexican wave effect
	int						stage;						// CMD: render stage for text
	COLOUR					colours[2];					// PP: colours - only element 0 is used for single-coloured text
	int						flags;						// PP: flags specific to the text box item type, eg. whether or not the text has a colour gradient

	// PP: NEW HANDY TEST
	static ushort			maxStringLength;			// PP: length of longest string to have been held in a textbox so far


	/*	--------------------------------------------------------------------------------
		Function 	: TExTBOX::update
		Purpose 	: update the page item
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::update, but currently does nothing different.
						It is included to ensure that, should TEXTBOX-specific update be introduced
						in the future, all TEXTBOX subclasses will inherit that update.
						(Assuming they all call TEXTBOX::update as they should!)
	*/
	virtual inline void update(void)
	{
		PAGEITEM::update();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::init
		Purpose 	: initialise the text box
		Parameters 	: index of text string, (opt/false) text wrap flag,
						(opt/centre)horizontal alignment, (opt/centre)vertical ailignment,
						(opt/default)first colour, (opt/default)second colour,
						(opt/default)text box flags, (opt/standardFont) font
		Returns 	: 
		Info 		: There is a very good reason for wrapping defaulting to false, however inconvenient it might seem:
						You may not want wrapping, but find it suddenly and horribly happens once the text has been localised;
						on the other hand if you DO want it it'll be pretty obvious whether you've switched it on or not.
						See?
						More importantly, there's can be a significant speed benefit in not allowing for wrapping on items that don't need it.
	*/
	void init(	const EStringIndex		string = NOSTRING
				/* PP: REMOUT: OOD,
				const bool				wrap		=false,
				const EBFontFormatting	hPos		=EBFONTFORMATTING_CENTRE,
				const EBFontFormatting	vPos		=EBFONTFORMATTING_CENTRE,
				const COLOUR			colour1		=TEXTFX_DEFAULT_COLOUR1,
				const COLOUR			colour2		=TEXTFX_DEFAULT_COLOUR2,
				const int				flags		=TBFLAG_DEFAULTS,
				TBFont* const			fontIn		=standardFont*/);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::TAG_TEXTBOX
		Purpose 	: constructor; calls init
		Parameters 	: index of text string,  (opt/true) text wrap flag,
						(opt/default)first colour, opt/default)second colour, (opt/default)text box flags,
						(opt/centre)horizontal ailignment, (opt/centre)vertical ailignment,
						 (opt/standardFont) font
		Returns 	: new textbox instance
		Info 		: sets up page item flags for the text box item type
						There is a very good reason for wrapping defaulting to false, however inconvenient it might seem:
						You may not want wrapping, but find it suddenly and horribly happens once the text has been localised;
						on the other hand if you DO want it it'll be pretty obvious whether you've switched it on or not.
						See?
	*/
	TAG_TEXTBOX(const EStringIndex		string = NOSTRING
				/* PP: REMOUT: OOD,
				const COLOUR			colour1		=TEXTFX_DEFAULT_COLOUR1,
				const COLOUR			colour2		=TEXTFX_DEFAULT_COLOUR2,
				const int				flags		=TBFLAG_DEFAULTS,
				const bool				wrap		=false,
				const EBFontFormatting	hPos		=EBFONTFORMATTING_CENTRE,
				const EBFontFormatting	vPos		=EBFONTFORMATTING_CENTRE,
				TBFont* const			fontIn		=standardFont*/)
	{
		this->init(string/* PP: REMOUT: OOD,wrap,hPos,vPos,colour1,colour2,flags,fontIn*/);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getRequiredWidth
		Purpose 	: get the minimum required width of the the textbox
		Parameters 	: 
		Returns 	: minimum required width of the the textbox
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	float getRequiredWidth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getRequiredHeight
		Purpose 	: get the minimum required height of the the textbox
		Parameters 	: 
		Returns 	: minimum required height of the the textbox
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	float getRequiredHeight(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setColour
		Purpose 	: set the colour of the text in the textbox
		Parameters 	: the colour for the text in the textbox
		Returns 	: 
		Info 		: 
	*/
	inline void setColour(const COLOUR& colourIn)
	{
		this->colours[0]=colourIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setColour
		Purpose 	: set the colour or colours of the text in the textbox
		Parameters 	: colour 1, colour2 (ie top then bottom if using a vertical gradient)
		Returns 	: 
		Info 		: 
	*/
	inline void setColour(const COLOUR& colour1in, const COLOUR& colour2in)
	{
		this->colours[0]=colour1in;
		this->colours[1]=colour2in;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setFontSize
		Purpose 	: set the size of the font in the text box
		Parameters 	: (opt/default) the new size of the font in the text box
		Returns 	: this
		Info 		: 
	*/

// PP: Ah well ye see, once upon a time, book rectangles were specified in absolute screen coordinates rather than as fractions of the viewport dimensions
#define TEXTBOX_FONT_SIZE_ADJUST		0.002f		// PP: i.e. roughly (1.0f/448.0f), 448 being the height of the PS2 display we were using at the time

	inline TAG_TEXTBOX* setFontSize(const float size=TEXTBOX_DEFAULT_FONT_SIZE)
	{
		this->fontSize=size*TEXTBOX_FONT_SIZE_ADJUST;

		// PP: calculate ACTUAL font size, based on 'normal' font size and on style
		this->updateActualFontSize();

		return this;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::getFontSize
		Purpose 	: get the actual size of the font for the text box, taking into account style
		Parameters 	: 
		Returns 	: the actual size of the font for the text box, taking into account style
		Info 		: *** NOTE!! *** this returns ACTUAL font size, taking into account style!!
	*/
	inline float getFontSize(void) const
	{
		return this->actualFontSize;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setVGrad
		Purpose 	: enable/disable vertical gradient
		Parameters 	: true to enable, false to disable
		Returns 	: 
		Info 		: 
	*/
	inline void setVGrad(const bool vGrad)
	{
		if(vGrad)
		{
			this->flags |= TBFLAG_VGRAD;
		}
		else
		{
			this->flags &= ~TBFLAG_VGRAD;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setXAlign
		Purpose 	: set the X-axis alignment of the text in the textbox
		Parameters 	: new X alignment mode (PIFLAG_XALIGN_)
		Returns 	: this
		Info 		: This overrides PAGEITEM::setXAlign
						// PP: PC: This has to return a PAGEITEM*because it overrides a *virtual* base class method (error C2555: 'TAG_TEXTBOX::setXAlign' : overriding virtual function differs from 'TAG_PAGEITEM::setXAlign' only by return type or calling convention)
	*/
	TAG_PAGEITEM* setXAlign(const PageItemFlag align);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setYAlign
		Purpose 	: set the Y-axis alignment of the text in the textbox
		Parameters 	: new Y alignment mode (PIFLAG_YALIGN_)
		Returns 	: this
		Info 		: This overrides PAGEITEM::setYAlign
						// PP: PC: This has to return a PAGEITEM*because it overrides a *virtual* base class method (error C2555: 'TAG_TEXTBOX::setXAlign' : overriding virtual function differs from 'TAG_PAGEITEM::setXAlign' only by return type or calling convention)
	*/
	TAG_PAGEITEM* setYAlign(const PageItemFlag align);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::checkCharacters
		Purpose 	: check the textbox's text for dodge characters and repair where necessary
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void checkCharacters(void);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setWrap
		Purpose 	: turns wrapping on & off
		Parameters 	: true to turn on, false to turn off
		Returns 	: this
		Info 		: 
	*/
	inline TAG_TEXTBOX* setWrap(const bool wrap)
	{
		this->format.wrap= wrap?1:0;

		return this;
	}

	
	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::enable
		Purpose 	: enable or disable the textbox
		Parameters 	: true to enable, false to disable
		Returns 	: this
		Info 		: // PP: might move this up to PAGEITEM base class if I find a use for it there
	*/
/*	inline TAG_TEXTBOX* enable(const bool state)
	{
		if(state)
		{
			this->flags &= ~TBFLAG_DISABLED;
		}
		else
		{
			this->flags |= TBFLAG_DISABLED;
		}

		return this;
	}
*/
	
	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setText
		Purpose 	: sets the text for the textbox
		Parameters 	: index of string to use from the string table
		Returns 	: 
		Info 		: 
	*/
//	void setText(const EStringIndex index);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setText
		Purpose 	: sets the text for the textbox
		Parameters 	: index of format string to use from the string table, argument list
		Returns 	: this
		Info 		: 
	*/
	struct TAG_TEXTBOX* setText(const EStringIndex index, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setText
		Purpose 	: sets the text for the textbox
		Parameters 	: string to place before string table entry, index of string to use from the string table
		Returns 	: this
		Info 		: // PP: TODO: replace examples of this overload with the Wilson Pointer Method (without breaking anything).
						// PP: the Wilson Pointer Method eliminates the risk of bad localisation, because it ensures correct sentence structure in all languages.
						// PP: DUPLICATED CODE
	*/
	struct TAG_TEXTBOX* setText(char *string, const EStringIndex index);


	/*  --------------------------------------------------------------------------------
		Function	: TEXTBOX::setFont
		Purpose		: Used to set the current font after initialisation
		Parameters	: TBFont
		Returns		: 
		Info		: 
	*/
	void setFont(TBFont *fontIn)
	{
		font = fontIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setStyle
		Purpose 	: Sets the text style for the font in this text box
		Parameters 	: ref to text style (see the TS_ presets extern'd in Textbox.h, or use your own *static* TEXTSTYLE)
		Returns 	: this
		Info 		: 
	*/
	struct TAG_TEXTBOX* setStyle(TEXTSTYLE &styleIn);


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setConcealed
		Purpose 	: Sets the text to concealed so that it can't be selected and it is replaced with ??? (or something similar)
		Parameters 	: true=concealed / false=not concealed
		Returns 	: this
		Info 		: // PP: Also changed this to return a TEXTBOX ptr: much more useful than a PAGEITEM ptr
	*/
	struct TAG_TEXTBOX* setConcealed(bool concealed=true);


	// PP: **** OLD PRINTBOX STUFF FOLLOWS (oh, the shame!) **** 


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::PRINTBOX
		Purpose 	: constructor
		Parameters 	: byte format string, argument list
		Returns 	: new PRINTBOX instance containing the text passed in
		Info 		: contains a source of phenylalanine
	*/
	PRINTBOX(const char * const fmt, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::setText
		Purpose 	: sets the text for the textbox
		Parameters 	: ptr to char string to put into the textbox
		Returns 	: 
		Info 		: 
	*/
	inline void setText(char* const string)
	{
		bkString8to16(this->text, string);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::sprintf8
		Purpose 	: write formatted data to the text box
		Parameters 	: format string, argument list
		Returns 	: this
		Info 		: sounds like a pop group of some sort
	*/
	PRINTBOX* sprintf8(const char * const fmt, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::sprintf16
		Purpose 	: write formatted data to the text box
		Parameters 	: format string, argument list
		Returns 	: this
		Info 		: 
	*/
	PRINTBOX* sprintf16(const unsigned short * const fmt, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::readPointers
		Purpose 	: WILSON DEEMED THIS TO BE UNNECESSARY INFORMATION - GOOD OLD WILSON
		Parameters 	: WILSON DEEMED THIS TO BE UNNECESSARY INFORMATION - GOOD OLD WILSON
		Returns 	: 
		Info 		: 
	*/
	void readPointers(va_list, unsigned short *);


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::strcatf
		Purpose 	: append formatted data into the text box
		Parameters 	: format string, argument list
		Returns 	: 
		Info 		: 
	*/
	void strcatf(const char * const fmt, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: WILSON DEEMED THIS TO BE UNNECESSARY INFORMATION - GOOD OLD WILSON
		Purpose 	: WILSON DEEMED THIS TO BE UNNECESSARY INFORMATION - GOOD OLD WILSON
		Parameters 	: WILSON DEEMED THIS TO BE UNNECESSARY INFORMATION - GOOD OLD WILSON
		Returns 	: 
		Info 		: 
	*/
	void setPointers(void *p0, ...);


	/*	--------------------------------------------------------------------------------
		Function 	: PRINTBOX::clear
		Purpose 	: clear the text from the text box
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void clear(void)
	{
		this->text[0]=0;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TEXTBOX::setCase
		Purpose 	: set the case of the textbox's string
		Parameters 	: format string, argument list
		Returns 	: 
		Info 		: // PP: case is reset to CASE_NONE by setText, sprintf, etc.
						// PP: Case persists through changes in language
	*/
	void setCase(const TBCase caseIn);


}TEXTBOX, _TEXTBOX BALIGN16;


#endif// PP: ndef __H_TEXTBOX
