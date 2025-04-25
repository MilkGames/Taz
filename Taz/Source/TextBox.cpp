// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : TextBox.cpp
//   Purpose : Text box page-item - displays a text string
// Component : Taz
//		Info : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
//#include "control.h"			// PP: provide basic input and control functions

#include "TextBox.h"			// PP: Text box page-item - displays a text string
#include "util.h"				// PP: general utility functions
#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "PageItemFX.h"			// PP: Special effects for page items


#define TEXTFX_PTOKEN_FLOAT		(0x1)
#define TEXTFX_PTOKEN_LONG		(0x2)
#define TEXTFX_PTOKEN_INT		(0x3)
#define TEXTFX_PTOKEN_STRING	(0x4)
#define TEXTFX_PTOKEN_TIMECLOCK	(0x5)


// PP: text-printing settings, used by TextFX_escapeCallback
TEXTSETTINGS					textSettings;

// PP: index of the current character poly
extern ushort					TextFX_polyIndex;

// PP: initialise static TEXTBOX members...
ushort							TEXTBOX::maxStringLength=0;


/*	--------------------------------------------------------------------------------
	Function 	: TEXTSETTINGS::TEXTSETTINGS
	Purpose 	: constructor; initialises the new instance
	Parameters 	: 
	Returns 	: new TEXTSETTINGS instance
	Info 		: 
*/
TEXTSETTINGS::TEXTSETTINGS(void)
{
	// PP: opaque text
	for(int i=0; i<4; i++)
	{
		this->vertColours[i]=COLOUR(TEXTFX_DEFAULT_RED,TEXTFX_DEFAULT_GREEN,TEXTFX_DEFAULT_BLUE,TEXTFX_DEFAULT_ALPHA);
	}

	// PP: opaque text
	this->alpha=TEXTFX_DEFAULT_ALPHA;

	// PP: no left/right clipping
	this->clipLR=false;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TEXT STYLES
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: commonly used preset text styles...

TEXTSTYLE		TS_normal;
TEXTSTYLE		TS_title;
TEXTSTYLE		TS_smallTitle;
TEXTSTYLE		TS_creditsTitle;
TEXTSTYLE		TS_info;
TEXTSTYLE		TS_control;
TEXTSTYLE		TS_largeControl;


/*	--------------------------------------------------------------------------------
	Function 	: TEXTSTYLE::init
	Purpose 	: default initialisation; sets the style up as TS_normal
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TEXTSTYLE::init(void)
{
	this->fontSize=1.0f;
	this->italic=true;
	this->vGrad=true;
	this->outline=true;
	this->outlineThickness=1.2f;
	this->colour1=TEXTFX_DEFAULT_COLOUR1;
	this->colour2=TEXTFX_DEFAULT_COLOUR2;
	this->outlineColour=COLOUR(0,0,0,128);
}


/*	--------------------------------------------------------------------------------
	Function 	: initTextStyles
	Purpose 	: initialise commonly-used preset text styles
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initTextStyles(void)
{
	// PP: ************************************************
	// PP: TODO: have 'selectable' as a TEXTSTYLE attribute
	// PP: ************************************************

	// PP: NOTE: TS_normal is already properly initialised by the TEXTSTYLE constructor.
	// PP: All styles will inherit the default properties of TS_normal.

	TS_title.fontSize=1.5f;
	TS_title.italic=false;
	TS_title.vGrad=false;
	TS_title.outlineThickness=1.6f;
	TS_title.colour1=COLOUR(225, 64, 0, 128);

	TS_smallTitle=TS_title;
	TS_smallTitle.fontSize=1.2f;

	TS_creditsTitle.fontSize=1.0f;
	TS_creditsTitle.italic=false;
	TS_creditsTitle.vGrad=false;
	TS_creditsTitle.colour1=COLOUR(64, 225, 0, 128);

	TS_info.fontSize=0.75f;
	TS_info.vGrad=false;
	TS_info.colour1=COLOUR(84, 84, 0, 128);

	TS_control.fontSize=0.75f;
	TS_control.vGrad=false;
	TS_control.colour1=COLOUR(128, 128, 128, 128);

	TS_largeControl=TS_control;
	TS_largeControl.fontSize=1.0f;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														TEXTBOX methods

											A text box holds text and formatting data.

												book > pages > rows > *ITEMS*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::updateActualFontSize
	Purpose 	: update the textbox's actual font size, based on its normal font size and its style
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
/* PP: REMOUT: OOD
#define TEXTBOX_FONTSIZE_FACTOR_TITLE			1.5f
#define TEXTBOX_FONTSIZE_FACTOR_SMALL_TITLE		1.2f
#define TEXTBOX_FONTSIZE_FACTOR_CREDITS_TITLE	1.0f
#define TEXTBOX_FONTSIZE_FACTOR_INFO			0.75f
#define TEXTBOX_FONTSIZE_FACTOR_CONTROL			0.75f
#define TEXTBOX_FONTSIZE_FACTOR_LARGE_CONTROL	1.0f
*/
void TEXTBOX::updateActualFontSize(void)
{
	this->actualFontSize=this->fontSize*this->style->fontSize;

/* PP: REMOUT: OOD
	// PP: macro to check for a style on the textbox and apply a font size adjustment accordingly
#define STYLE_FONTSIZE(_style) \
	case TEXTSTYLE_##_style: \
	{ \
		this->actualFontSize*=TEXTBOX_FONTSIZE_FACTOR_##_style; \
	} \
	break;

	switch(this->style)
	{
		// PP: please see macro definition above
		STYLE_FONTSIZE(TITLE);
		STYLE_FONTSIZE(SMALL_TITLE);
		STYLE_FONTSIZE(INFO);
		STYLE_FONTSIZE(CONTROL);
		STYLE_FONTSIZE(LARGE_CONTROL);
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setLanguage
	Purpose 	: set the language of the textbox
	Parameters 	: new language of the textbox (BLANGUAGEID_...)
	Returns 	: 
	Info 		: this overrides PAGEITEM::setLanguage, to re-fetch the box's text, in the specified language, from the string table
*/
void TEXTBOX::setLanguage(const EBLanguageID languageIn)
{
	this->setText(this->stringIndex);

	// PP: Apply chosen character-case for the textbox
	this->setCase(this->characterCase);
}



/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::init
	Purpose 	: initialise the text box
	Parameters 	: index of text string, (opt/true) text wrap flag,
					(opt/centre)horizontal ailignment, (opt/centre)vertical ailignment,
					(opt/default)first colour, (opt/default)second colour,
					(opt/default)text box flags, (opt/standardFont) font
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::init(	const EStringIndex		string
					/* PP: REMOUT: OOD,
					const bool				wrap,
					const EBFontFormatting	hPos,
					const EBFontFormatting	vPos,
					const COLOUR			colour1,
					const COLOUR			colour2,
					const int				flags,
					TBFont* const			fontIn*/)
{
	this->format.horzFormat = EBFONTFORMATTING_CENTRE;// PP: REMOUT: OOD	hPos;
	this->format.vertFormat = EBFONTFORMATTING_CENTRE;// PP: REMOUT: OOD	vPos;

	//PP: new test
	this->format.x=0.0f;
	this->format.y=0.0f;
	this->format.width=100.0f;
	this->format.height=100.0f;

	this->format.lineSpace = 0;
	this->format.wrap=false;// PP: REMOUT: OOD	wrap;

	this->flags=TBFLAG_DEFAULTS;// PP: REMOUT: OOD		flags;

	// PP: set default style
	this->setStyle(TS_normal);

	// PP: **** FLAGS ARE NOW SET - SAFE TO SET FONT SIZE NOW!!!

	// PP: set font size to default
	this->setFontSize();

/* PP: REMOUT: OOD
	this->colours[0]=colour1;
	this->colours[1]=colour2;
*/
	this->font=standardFont;// PP: REMOUT: OOD	fontIn;

	this->clock = 0.0f;
	this->drawOffset = TEXTFX_SELECTOFFSET;
	this->drawOffsetDirection[0] = TEXTFXOFFSET_UP;
	
	// PP: this can only be done after some of the other stuff is done
	this->setText(string);

	// PP: no character-case adjustment
	this->setCase(TBCASE_NONE);

	// PP: specify that textbox backgrounds/borders should fit tightly round the text
	this->setBoxStyle(true, true);
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this textbox
	Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
	Returns 	: 
	Info 		: this overrides PAGEITEM::chooseEffect
*/
void TEXTBOX::chooseEffect(const PIEMode mode, const bool recur)
{
	// PP: choose an effect for this textbox...
	// PP: (See macro definition in PageItemFX.h)
	PAGEITEM_CHOOSE_EFFECT(textbox);
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::applyFontSize
	Purpose 	: apply the text box's (real/actual/final) font size into the current object matrix
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::applyFontSize(void) const
{
	// PP: apply font scale...

	TBMatrix	localObjMatCopy;
	TBMatrix	fontScaleMat;
	TBMatrix	finalMat;

	bdGetObjectMatrix(localObjMatCopy);	

	bmMatScale(fontScaleMat, this->getFontSize(), this->getFontSize(), this->getFontSize());
	bmMatMultiply(finalMat, localObjMatCopy, fontScaleMat);
	bdSetObjectMatrix(finalMat);
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::startDraw
	Purpose 	: set up conditions needed to draw the textbox (eg. apply item's scale & position to object matrix)
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::startDraw to apply actual-font scale on top of draw scale
*/
void TEXTBOX::startDraw(void)
{
	PAGEITEM::startDraw();
//	this->optRect.toOrigin().draw(ORANGE, OUTLINE);
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::mainDrawDebug
	Purpose 	: the core of the debug-draw method - actually debug-draw the textbox
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDrawDebug
*/
void TEXTBOX::mainDrawDebug(void)
{
	PAGEITEM::mainDrawDebug();// PP: NB this won't take font size into account: TODO: fix this`

//	ushort						string16[TEXTFX_STRING_LENGTH];
/*	TBFontFormatting			tempFormat;
	RECTANGLE					tempRect;
	COLOUR						colour=COLOUR(128, 128, 128);

	tempRect.init(this->rect.left+(this->rect.width()*(MARGIN*2)),
					this->rect.right-(this->rect.width()*(MARGIN*2)),
					this->rect.bottom+(this->rect.height()*(MARGIN*2)),
					this->rect.top-(this->rect.height()*(MARGIN*2)));

	DRAW_HOLLOW_RECT(tempRect, colour);

	tempFormat.width=this->rect.width();
	tempFormat.height=this->rect.height();
	tempFormat.lineSpace = 0;
	tempFormat.horzFormat=EBFONTFORMATTING_CENTRE;
	tempFormat.vertFormat=EBFONTFORMATTING_CENTRE;
	tempFormat.wrap=0;*/

//	bkString8to16(string16, this->name);

//	TEST_TEXT_FORMAT(tempFormat);
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::mainDraw
	Purpose 	: the core of the draw method - actually draw the textbox
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void TEXTBOX::mainDraw(void)
{
	// PP: apply font size
	this->applyFontSize();

	// PP: Apply textbox's style...

/* PP: REMOUT: OOD	switch(this->style)
	{
#define APPLY_TEXT_STYLE(_style) \
	case TEXTSTYLE_##_style: \
		{ \
			textSettings.flags=TEXTFX_##_style; \
		} \
		break; \

		// PP: Please see macro definition above
		APPLY_TEXT_STYLE(TITLE);
		APPLY_TEXT_STYLE(SMALL_TITLE);
		APPLY_TEXT_STYLE(CREDITS_TITLE);
		APPLY_TEXT_STYLE(INFO);
		APPLY_TEXT_STYLE(CONTROL);
		APPLY_TEXT_STYLE(LARGE_CONTROL);
		// PP: REMOUT: not yet		APPLY_TEXT_STYLE(SUBTITLE);
		// PP: REMOUT: not yet		APPLY_TEXT_STYLE(BOUNTY);

	default:
		textSettings.flags=0;
	}*/

	textSettings.flags=0;

	// PP: Apply italic
	if(this->flags & TBFLAG_ITALIC)
	{
		textSettings.flags |= TEXTFX_ITALIC;
	}

	// PP: Apply enabled/disabled styling
	if(this->pageItemFlags & PIFLAG_DISABLED)
	{
		textSettings.flags |= TEXTFX_DISABLED;
	}

	// PP: apply scale for font size member...

	unsigned short	sText[TEXTFX_STRING_LENGTH], *sT;
	char			sArgBuff[16];
	int				aCtr;

	// CPW: do Chris's fancy pointer thing
//	if (this->flags & TBFLAG_CONCEALED)
//		this->textLen = bkStringLength16(mainStringTable->strings[STR_CONCEALED_SECRET].string);
//	else
	this->textLen = bkStringLength16(this->text);
	
	aCtr = 0;
	sT = sText;

	for(int c=0; c<this->textLen; c++)
	{
		unsigned short	chr;
		int				argLen;

//		if (this->flags & TBFLAG_CONCEALED)
//			chr = mainStringTable->strings[STR_CONCEALED_SECRET].string[c];
//		else
		chr = this->text[c];

		switch(chr)
		{
		case TEXTFX_PTOKEN_FLOAT:
			argLen = ::sprintf(sArgBuff, "%f", *((float *)this->ppArgPtr[aCtr++]));
			break;

		case TEXTFX_PTOKEN_LONG:
		case TEXTFX_PTOKEN_INT:
			// CPW: argLen = ::sprintf(sArgBuff, "%d", *((long *)this->ppArgPtr[aCtr++]));
			argLen = utilLongtoBaseString(sArgBuff, *((int32 *)this->ppArgPtr[aCtr++]), BASE_DECIMAL);
			break;

		case TEXTFX_PTOKEN_STRING:
			strcpy(sArgBuff, (char *)this->ppArgPtr[aCtr++]);
			argLen = this->textLen;
			break;

		case TEXTFX_PTOKEN_TIMECLOCK:
			argLen = utilGetPrettyTime(sArgBuff, *((float *)this->ppArgPtr[aCtr++]), NULL);
			break;

		default:
			argLen = 0;
			*sT = chr;
			sT++;
		}


		if(argLen)
		{
			bkString8to16(sT, sArgBuff);
			sT+=argLen;
		}
	}
	*sT = 0;

	if((this->selected)||(this->highlighted)/* PP: temp! */)
	{
		TextFX_select(true);
	}
	else
	{
		if(this->flags&TBFLAG_VGRAD)
		{
			TextFX_vGrad(this->colours[0], this->colours[1]);
		}
		else
		{
			TextFX_setColour(this->colours[0]);
		}
	}

	TextFX_setAlpha((unsigned char)(this->opacity*TEXTBOX_MAX_ALPHA));

/* PP: REMOUT: SCRIBBLE EFFECT IS OOD, NOT USED.	If we want to use it again I'll get it working (efficiently: with bdPrintFontLite where possible), till then it's wasted work, no? ...

	if(this->flags&TBFLAG_SCRIBBLE)
	{
		// CMD: scribble
		this->stage = TEXTFX_SCRIBBLE;

		if(!(VFX_effectActive(VFX_STEREO)&&VFX_stereoEye))
		{
			if((this->clock += fTime) > 0.04f)
			{
				this->clock = 0.0f;
				this->drawOffsetDirection[1] = this->drawOffsetDirection[0];

				if((++this->drawOffsetDirection[0]) > TEXTFXOFFSET_RIGHT) this->drawOffsetDirection[0] = TEXTFXOFFSET_UP;
			}
		}

		bdPrintFont(this->font, sText, bkStringLength16(sText),&this->format,0,0,0,(int)((float)TextFX_alpha*0.5f),&TextFX_escapeCallback,(void*)this);
		this->stage = TEXTFX_SCRIBBLETWO;
		bdPrintFont(this->font, sText, bkStringLength16(sText),&this->format,0,0,0,(int)((float)TextFX_alpha*0.25f),&TextFX_escapeCallback,(void*)this);
	}
	*/

	if(this->flags&TBFLAG_OUTLINE)
	{
		// CMD: outline
		this->drawStage(sText, TEXTFX_OUTLINE_UPLEFT);
		this->drawStage(sText, TEXTFX_OUTLINE_UPRIGHT);
		this->drawStage(sText, TEXTFX_OUTLINE_DOWNLEFT);
		this->drawStage(sText, TEXTFX_OUTLINE_DOWNRIGHT);
	}
	
	// PP: print the text
	this->drawStage(sText, TEXTFX_NORMAL);

	TextFX_select(false);

	TextFX_resetAlpha();

	TextFX_resetColour();
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::drawStage
	Purpose 	: perform one of the textbox's draw stages
	Parameters 	: final text to print, stage to draw (ETextRenderStage TEXTFX_...)
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::drawStage(const ushort* const finalText, const ETextRenderStage stageIn)
{
	this->stage = stageIn;

	// PP: reset current character poly index
	TextFX_polyIndex=0;

	// PP: NEW TEST - widen the text format box a touch to prevent unexpected wrapping!
	TBFontFormatting	adjustedFormat;

	// PP: total extra width added to the format box used to print the text, in pixels (at 1:1 scale) I think
	// PP: Keep this as low as it can be without allowing the text wrapping glitch
#define TBDRAW_BOX_SLACK	4.0f// PP: 1.0f

	// PP: ============================
	// PP: NOTE: the '(A) ok' string on the demo outro blurb is an odd one - look into it.
	// PP: With TBDRAW_BOX_SLACK at 1, it'll only wrap if you've exited the demo from the pause menu rather than completed the posters.!!
	// PP: ============================

	// PP: NOTE: I'm firstly copying the WHOLE formatting structure, for future-proofness...
	adjustedFormat=this->format;

	// PP: ...then just adjusting the x and width
	adjustedFormat.x			=this->format.x-(TBDRAW_BOX_SLACK*0.5f);
	adjustedFormat.width		=this->format.width+TBDRAW_BOX_SLACK;

	if(this->quickPrint())
	{
		// PP: can increase speed by using bdPrintFontLite
		bdPrintFontLite(this->font, finalText, bkStringLength16(finalText),adjustedFormat.x,this->format.y,0,0,0,128,&TextFX_escapeCallback,(void*)this);
	}
	else
	{
		// PP: need to use the advanced features of the full bdPrintFont
		bdPrintFont(this->font, finalText, bkStringLength16(finalText),&adjustedFormat,0,0,0,128,&TextFX_escapeCallback,(void*)this);
	}

/* PP: LEAVE IN: this draws the real and adjusted format boxes so's ye can see the difference

	// PP: NOTE: TEST_TEXT_FORMAT alters the sodding format struct!

	TBFontFormatting	tempFormat;

	tempFormat=this->format;
		
	// PP: hmmm not used this in a while!
	TEST_TEXT_FORMAT(tempFormat);
	TEST_TEXT_FORMAT(adjustedFormat);*/
	
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::findEscapeCodes
	Purpose 	: search the text for escape codes
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::findEscapeCodes(void)
{
	ushort*				ptr;
	static ushort		atSign=0;// PP: definitely not an @

	// PP: find out what '@' is in wide-character form
	if(atSign == 0)
	{
		ushort	tempStr[2];

		bkString8to16(tempStr, "@");

		atSign=tempStr[0];
	}

	ptr=this->text;

	// PP: assume no escape codes till we find one
	this->hasEscapeCodes=false;

	while((*ptr) != 0)// PP: (null terminator)
	{
		if((*ptr) == atSign)// PP: check for @
		{
			if((*(ptr+1)) != atSign)// PP: check it's not a double '@', which would NOT indicate an escape code
			{
				this->hasEscapeCodes=true;
				return;
			}
		}
		else
		{
			// TP: Oops, infernal loop.
			// PP: Yes, sorry, all the devkits started locking up, and I didn't realise it was because I'd caused an infinite loop, so I went home.
			// PP: I'll remember next time!
			ptr++;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::updateAfterTextChange
	Purpose 	: perform any update that needs to be done in response the the text changing
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::updateAfterTextChange(void)
{
	// PP: search the textbox's new text for escape codes
	this->findEscapeCodes();

	// PP: NEW: x-realign the textbox (why didn't I think of this before)...

/*	if(this->pageItemFlags & PIFLAG_XALIGN_CENTRE)
	{
		float	midX;
		float	halfWidth;

		midX=this->rect.midX();
		halfWidth=this->getRequiredWidth()/2.0f;

		this->optRect.left=midX-halfWidth;
		this->optRect.right=midX+halfWidth;
	}
	else if(this->pageItemFlags & PIFLAG_XALIGN_LEFT)
	{
		this->optRect.left=this->rect.left;
		this->optRect.right=optRect.left+this->getRequiredWidth();
	}
	else if(this->pageItemFlags & PIFLAG_XALIGN_RIGHT)
	{
		this->optRect.right=this->rect.right;
		this->optRect.left=optRect.right-this->getRequiredWidth();
	}*/

	// PP: *** NEW ***
	this->textLen=bkStringLength16(this->text);

	// PP: realign (do this last)
	// PP: REMOUT: NEW TEST		this->setRect(this->rect);
	// PP: NEW TESTS
	// PP: IMPORTANT NOTE!: REALIGNMENT ISN'T ACTUALLY NEEDED UNTIL THE ITEM BECOMES VISIBLE
	// PP: AT THE MOMENT THOUGH, THERE IS NO WAY OF KNOWING IF THE ITEM IS 'OPEN' OR NOT!!!!!!!!!!!
	this->markForRealignment();
	this->align();// PP: TODO: group these two into PAGEITEM::realign()

#ifdef _TAZDEBUG

	// PP: see if this is the longest recorded string to have been held by a textbox...

	ushort	tempLen;
	
	tempLen=bkStringLength16(this->text);// PP: debug only - who gives a shit

	if(tempLen > TEXTBOX::maxStringLength)
	{
		TEXTBOX::maxStringLength=tempLen;

		bkPrintf("TEXTBOX::updateAfterTextChange: LONGEST TEXTBOX STRING SO FAR IS %d CHARACTERS (%d max)\n", tempLen, TEXTFX_STRING_LENGTH);
	}

#endif// PP: def _TAZDEBUG
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::getOutlineWidth
	Purpose 	: get the width of the textbox's outline (assuming that it does have an outline)
	Parameters 	: 
	Returns 	: the width of the textbox's outline (assuming that it does have an outline)!
	Info 		: I didn't want to give per-textbox control over this, but different styles may use different outline thicknesses if they want
*/

#define TEXTBOX_OUTLINEWIDTH_TITLE		2.0f
#define TEXTBOX_OUTLINEWIDTH_NORMAL		1.2f

float TEXTBOX::getOutlineWidth(void) const
{
	return this->style->outlineThickness;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::getOutlineColour
	Purpose 	: get the colour of the textbox's outline (assuming that it does have an outline)
	Parameters 	: 
	Returns 	: ref to the colour of the textbox's outline (assuming that it does have an outline)!
	Info 		: I didn't want to give per-textbox control over this, but different styles may use different outline colours if they want
*/
COLOUR& TEXTBOX::getOutlineColour(void) const
{
	static COLOUR	rtn;

	// PP: TODO: white outlines can look kinda nice; outline colour could also be used to highlight text
	rtn=COLOUR(0,0,0,128);

	return rtn;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::align
	Purpose 	: calculate alignment for the textbox
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::align
*/
void TEXTBOX::align(void)
{
	if((~this->pageItemFlags) & PIFLAG_ALIGNED)
	{
		// PP: first, give the text as much space as we can - let it spread out and get reeel comfy like...

		float	newFormatWidth, newFormatHeight;

		if((this->getAlignScale() == 0.0f)||(this->getFontSize() == 0.0f))
		{
			this->format.x=0.0f;
			this->format.y=0.0f;
			this->format.width=0.0f;
			this->format.height=0.0f;
		}
		else
		{
			this->format.x=(-this->rect.width()*0.5f)/this->getFontSize()/this->getAlignScale();// PP: left side of format box
			this->format.y=(this->rect.height()*0.5f)/this->getFontSize()/this->getAlignScale();// PP: top side of format box
			this->format.width=this->rect.width()/this->getFontSize()/this->getAlignScale();
			this->format.height=this->rect.height()/this->getFontSize()/this->getAlignScale();
		}

		// PP: THESE FLAGS MUST BE CLEARED WHENEVER THE FORMAT BOX CHANGES (as it probably just has)
		this->pageItemFlags &= ~(PIFLAG_WIDTHCALCULATED|PIFLAG_HEIGHTCALCULATED);// PP: TODO: CHECK THAT IT HAS CHANGED?

		// PP: ...then, find out the dimensions that it spread out to...
		PAGEITEM::align();

		// PP: ...then, give it those dimensions, but in the correct position...

		if((this->getAlignScale() == 0.0f)||(this->getFontSize() == 0.0f))
		{
			// PP: prevent db0

			this->format.x=0.0f;
			this->format.y=0.0f;
			this->format.width=0.0f;
			this->format.height=0.0f;
		}
		else
		{
			// PP: rectangle into which to fit text - this is 'optRect' for a textbox, but is different eg. for a checkbox
			RECTANGLE	textRect;

			textRect=this->getTextRect();

			float	xOffset;// PP: the x offset of the text rect

			xOffset=textRect.midX()-this->optRect.midX();

			// PP: form the format box for the text

			this->format.x=((((xOffset-textRect.width())*0.5f)/this->getFontSize()/this->getAlignScale()));// PP: left side of format box
			this->format.y=((textRect.height()*0.5f)/this->getFontSize()/this->getAlignScale());// PP: top side of format box
			this->format.width=(textRect.width()/this->getFontSize()/this->getAlignScale());
			this->format.height=(textRect.height()/this->getFontSize()/this->getAlignScale());
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setRect
	Purpose 	: set the item's rectangle
	Parameters 	: new rectangle
	Returns 	: (PAGEITEM*)this
	Info 		: this overrides PAGEITEM::setRect
*/
PAGEITEM* TEXTBOX::setRect(const RECTANGLE& rectIn)
{
	// PP: NEW - optimisations
	if(this->getWrap())
	{
		// PP: a change in width may impact required height
		if(rectIn.width() != this->rect.width())
		{
			this->pageItemFlags &= ~PIFLAG_HEIGHTCALCULATED;
		}

		// PP: NOTE: since textboxes always spread out horizontally as far as they can,
		// PP: a change in height will never impact required width!! PHEW.

		// PP: CONTRARY: TEMP TEST
		if(rectIn.height() != this->rect.height())
		{
			this->pageItemFlags &= ~PIFLAG_WIDTHCALCULATED;
		}
	}

	// PP: moved alignment out into align()

	PAGEITEM::setRect(rectIn);

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::getRequiredWidth
	Purpose 	: get the minimum required width of the the textbox
	Parameters 	: 
	Returns 	: minimum required width of the the textbox
	Info 		: this overrides PAGEITEM::getRequiredWidth
*/
float TEXTBOX::getRequiredWidth(void)
{
#ifdef TEXTBOX_TIME_GETDIMENSIONS
	STARTTIMER;
#endif// PP: def TEXTBOX_TIME_GETDIMENSIONS

	// PP: NEW optimisation
	if(this->pageItemFlags & PIFLAG_WIDTHCALCULATED)
	{
		return this->requiredWidth;
	}

	if(this->font == NULL)
	{
		this->requiredWidth=0.0f;

		goto done;
	}

	if(this->quickPrint())
	{
		float	height;		// PP: width and height of the text

		bdFontExtent(this->font, this->text, bkStringLength16(this->text), &this->requiredWidth, &height);
	}
	else
	{
		float		textRect[4];

		// PP: calculate required dimensions of text box
		int rtn=bdFontFormattedRect(
			&textRect[0],
			this->font,
			this->text,
			bkStringLength16(this->text),// PP: hmm is this right?
			&this->format,
			TextFX_escapeCallback,
			NULL);

		if(rtn == TRUE)
		{
			this->requiredWidth=(textRect[2]-textRect[0]);
		}
		else
		{
			this->requiredWidth=0.0f;

			goto done;
		}
	}

	// PP: Now take into account font size and align scale
	this->requiredWidth *= (this->getFontSize()*this->getAlignScale());

done:

	// PP: NEW - optimisation
	this->pageItemFlags |= PIFLAG_WIDTHCALCULATED;

#ifdef TEXTBOX_TIME_GETDIMENSIONS
	ENDTIMER("TEXTBOX::getRequiredWidth");
#endif// PP: def TEXTBOX_TIME_GETDIMENSIONS

	return this->requiredWidth;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::getRequiredHeight
	Purpose 	: get the minimum required height of the the textbox
	Parameters 	: 
	Returns 	: minimum required height of the the textbox
	Info 		: this overrides PAGEITEM::getRequiredHeight
*/
float TEXTBOX::getRequiredHeight(void)
{
#ifdef TEXTBOX_TIME_GETDIMENSIONS
	STARTTIMER;
#endif// PP: def TEXTBOX_TIME_GETDIMENSIONS

	// PP: NEW optimisation
	if(this->pageItemFlags & PIFLAG_HEIGHTCALCULATED)
	{
		return this->requiredHeight;
	}

	if(this->font == NULL)
	{
		this->requiredHeight=0.0f;

		goto done;
	}

	if(this->quickPrint())
	{
		float	width;

		bdFontExtent(this->font, this->text, bkStringLength16(this->text), &width, &this->requiredHeight);
	}
	else
	{
		float		textRect[4];

		// PP: calculate required dimensions of text box
		int rtn=bdFontFormattedRect(
			&textRect[0],
			this->font,
			this->text,
			bkStringLength16(this->text),
			&this->format,
			TextFX_escapeCallback,
			NULL);

		if(rtn == TRUE)
		{
			this->requiredHeight=(textRect[3]-textRect[1]);
		}
		else
		{
			this->requiredHeight=0.0f;

			goto done;
		}
	}

	// PP: Now take into account font size and align scale
	this->requiredHeight *= (this->getFontSize()*this->getAlignScale());

done:

	// PP: NEW - optimisation
	this->pageItemFlags |= PIFLAG_HEIGHTCALCULATED;

#ifdef TEXTBOX_TIME_GETDIMENSIONS
	ENDTIMER("TEXTBOX::getRequiredHeight");
#endif// PP: def TEXTBOX_TIME_GETDIMENSIONS

	return this->requiredHeight;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setXAlign
	Purpose 	: set the X-axis alignment of the text in the textbox
	Parameters 	: new X alignment mode (PIFLAG_XALIGN_)
	Returns 	: this
	Info 		: This overrides PAGEITEM::setXAlign
*/
PAGEITEM* TEXTBOX::setXAlign(const PageItemFlag align)
{
	PAGEITEM::setXAlign(align);

	if(align == PIFLAG_XALIGN_LEFT)
	{
		this->format.horzFormat=EBFONTFORMATTING_LEFT;
	}
	else if(align == PIFLAG_XALIGN_RIGHT)
	{
		this->format.horzFormat=EBFONTFORMATTING_RIGHT;
	}
	else // PP: if(align == PIFLAG_XALIGN_CENTRE)
	{
		this->format.horzFormat=EBFONTFORMATTING_CENTRE;
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setYAlign
	Purpose 	: set the Y-axis alignment of the text in the textbox
	Parameters 	: new Y alignment mode (PIFLAG_YALIGN_)
	Returns 	: this
	Info 		: This overrides PAGEITEM::setYAlign
					// PP: PC: This has to return a PAGEITEM*because it overloads a *virtual* base class method (error C2555: 'TAG_TEXTBOX::setXAlign' : overriding virtual function differs from 'TAG_PAGEITEM::setXAlign' only by return type or calling convention)
*/
PAGEITEM* TEXTBOX::setYAlign(const PageItemFlag align)
{
	PAGEITEM::setYAlign(align);

	if(align == PIFLAG_YALIGN_BOTTOM)
	{
		this->format.vertFormat=EBFONTFORMATTING_BOTTOM;
	}
	else if(align == PIFLAG_YALIGN_TOP)
	{
		this->format.vertFormat=EBFONTFORMATTING_TOP;
	}
	else // PP: if(align == PIFLAG_YALIGN_CENTRE)
	{
		this->format.vertFormat=EBFONTFORMATTING_CENTRE;
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::checkCharacters
	Purpose 	: check the textbox's text for dodge characters and repair where necessary
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

// PP: illegal characters...
#define BLOODY_ELLIPSIS			0x0085// PP: 0xff85
//#define BLOODY_APOSTROPHE		dunno
#define BLOODY_QUOTES			0x0094

void TEXTBOX::checkCharacters(void)
{
	static ushort	wcTemp[TEXTFX_STRING_LENGTH];

	// PP: check for Excel Auto-Corrupt action...

	ushort*	ptr;

	for(ptr=this->text; *ptr != 0; ptr++)
	{
		if(*ptr == BLOODY_ELLIPSIS)// PP: ellipsis character found
		{
			// PP: replace the ellipsis with "..." ...

			// PP: keep a copy of all the text after the ellipsis

			if(*(ptr+1) == NULL)
			{
				wcTemp[0]=0x0000;
			}
			else
			{
				bkStringCopy16(wcTemp, ptr+1);
			}

			// PP: write the "..." into the string
			bkString8to16(ptr, "...");
			ptr+=4;

			// PP: copy the remainder of the string back in
			if(wcTemp[0] != 0x0000)
			{
				bkStringCopy16(ptr, wcTemp);
			}
		}
/*		else if(*ptr == BLOODY_APOSTROPHE)
		{
			// PP: replace the illegal apostrophe with a legal one
			bkString8to16(wcTemp, "'");
			*ptr=wcTemp[0];
		}
*/		else if(*ptr == BLOODY_QUOTES)// PP: nasty quotes found
		{
			// PP: replace the illegal quotes with legal ones
			bkString8to16(wcTemp, "\"");
			*ptr=wcTemp[0];
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setText
	Purpose 	: sets the text for the textbox
	Parameters 	: index of string to use from the string table
	Returns 	: 
	Info 		: 
*/
/* PP: REMOUT: SUPERCEEDED	void TEXTBOX::setText(const EStringIndex index)
{
	// PP: keep a record of the string-table index of the string, so's we can switch language
	// NH: But only if not concealed
	if (~flags & TBFLAG_CONCEALED)
		this->stringIndex=index;

	if(index == NOSTRING)
	{
		// PP: put a character into the box so's the alignment code knows what height to expect from a single line of text
		// PP: if you think this is dirty then suggest me an alternative and sod off while you're at it!
		this->setText("X");
	}
	else
	{
		if(uint32(index) >= mainStringTable->noofStrings)
		{
			this->setText("OOB STRING");
			bkPrintf("TEXTBOX::setText: string index %d out of range (only %d strings in table)\n", index, mainStringTable->noofStrings);
		}
		else
		{
			bkStringCopy16(this->text, mainStringTable->strings[index].string);

			// PP: repair any dodge characters from Excel or that
			this->checkCharacters();
		}
	}

	// PP: perform any update that needs to be done in response the the text changing
	this->updateAfterTextChange();
}*/


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setText
	Purpose 	: sets the text for the textbox
	Parameters 	: index of format string to use from the string table, argument list
	Returns 	: this
	Info 		: 
*/
TEXTBOX* TEXTBOX::setText(const EStringIndex index, ...)
{
	unsigned short	tempStrW[TEXTFX_STRING_LENGTH];
	char			tempStr[TEXTFX_STRING_LENGTH];
	char			tempStr2[TEXTFX_STRING_LENGTH];
	va_list			argp;

	// PP: keep a record of the string-table index of the string, so's we can switch language
	// NH: But only if not concealed
	if (~flags & TBFLAG_CONCEALED)
		this->stringIndex=index;

	// PP: clear character-case
	this->characterCase = TBCASE_NONE;

	if(index == NOSTRING)
	{
		// PP: put a character into the box so's the alignment code knows what height to expect from a single line of text
		// PP: if you think this is dirty then suggest me an alternative and sod off while you're at it!
		this->setText("X");
	}
	else
	{
		if(uint32(index) >= mainStringTable->noofStrings)
		{
			this->setText("OOB STRING");
			bkPrintf("TEXTBOX::setText: string index %d out of range (only %d strings in table)\n", index, mainStringTable->noofStrings);
		}
		else
		{
			// PP: read immediate arguments (%) and incorporate them into the string...

			bkString16to8(tempStr, mainStringTable->strings[index].string);

			va_start(argp, index);// PP: 2nd param is the param before the variable arguments!!

			// TP: Should be ok to use this yes?, I'll try it on PS2 for a bit
			// PP: I suppose it must've worked or else someone would've said something; use it on all platforms then
			bkStringVSprintf16(this->text, mainStringTable->strings[index].string, argp);

			va_end(argp);

			bkString16to8(tempStr, this->text);// PP: TEMP TEST

#ifdef _TAZDEBUG
			
//			bkPrintf("TEXTBOX::setText \"%s\"\n", tempStr);
#endif// PP: def _TAZDEBUG

			// PP: now with that resulting string, read dynamic arguments (@d) and incorporate them into the string...

/* REMOUT: not quite ready yet
			va_start(argp, tempStrW);
			this->readPointers(argp, tempStrW);
			va_end(argp);
*/

			// PP: repair any dodge characters from Excel or that
			this->checkCharacters();
		}
	}

	// PP: perform any update that needs to be done in response the the text changing
	this->updateAfterTextChange();

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setText
	Purpose 	: sets the text for the textbox
	Parameters 	: string to place before string table entry, index of string to use from the string table
	Returns 	: this
	Info 		: // PP: TODO: replace examples of this overload with the Wilson Pointer Method (without breaking anything).
					// PP: the Wilson Pointer Method eliminates the risk of bad localisation, because it ensures correct sentence structure in all languages.
					// PP: DUPLICATED CODE
*/
TEXTBOX* TEXTBOX::setText(char *string, const EStringIndex index)
{
	static char errorText[TEXTFX_STRING_LENGTH+256]={""};
	static char textboxText[TEXTFX_STRING_LENGTH]={""};

	static char tempstring[TEXTFX_STRING_LENGTH]={""};
	static char temperstring[TEXTFX_STRING_LENGTH]={""};

	// PP: keep a record of the string-table index of the string, so's we can switch language
	// NH: But only if not concealed
	if (~flags & TBFLAG_CONCEALED)
		this->stringIndex=index;

	// PP: clear character-case
	this->characterCase = TBCASE_NONE;

	if(index == NOSTRING && *string == 0)
	{
		// PP: put a character into the box so's the alignment code knows what height to expect from a single line of text
		// PP: if you think this is dirty then suggest me an alternative and sod off while you're at it!
		this->setText("X");
	}
	else
	{
		if(uint32(index) >= mainStringTable->noofStrings)
		{
			this->setText("OOB STRING");
			bkPrintf("TEXTBOX::setText: string index %d out of range (only %d strings in table)\n", index, mainStringTable->noofStrings);
		}
		else
		{
			bkString16to8(tempstring, mainStringTable->strings[index].string);

			sprintf(temperstring, "%s%s", string, tempstring);

			bkString8to16(this->text, temperstring);

			// PP: repair any dodge characters from Excel or that
			this->checkCharacters();
		}
	}

	// PP: perform any update that needs to be done in response the the text changing
	this->updateAfterTextChange();

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setStyle
	Purpose 	: Sets the text style for the font in this text box
	Parameters 	: ref to text style (see the TS_ presets extern'd in Textbox.h, or use your own *static* TEXTSTYLE)
	Returns 	: this
	Info 		: 
*/
TEXTBOX* TEXTBOX::setStyle(TEXTSTYLE &styleIn)
{
	// PP: keep a ptr to the style that this textbox is to use
	this->style= &styleIn;

	// PP: copy the overrideable attributes of the style out into the textbox itself...

	SETFLAG(this->flags, TBFLAG_ITALIC, this->style->italic);
	SETFLAG(this->flags, TBFLAG_OUTLINE, this->style->outline);
	SETFLAG(this->flags, TBFLAG_VGRAD, this->style->vGrad);

	this->colours[0]=this->style->colour1;
	this->colours[1]=this->style->colour2;

	updateActualFontSize();// PP: TEMP TEST

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setConcealed
	Purpose 	: Sets the text to concealed so that it can't be selected and it is replaced with ??? (or something similar)
	Parameters 	: true=concealed / false=not concealed
	Returns 	: this
	Info 		: // PP: changed this to return a TEXTBOX ptr: much more useful than a PAGEITEM ptr
*/
TEXTBOX* TEXTBOX::setConcealed(bool concealed)
{
	if (concealed)
	{
		this->flags |= TBFLAG_CONCEALED;
		this->pageItemFlags |= PIFLAG_DISABLED;
		this->setSelectable(false);
		this->setText(STR_CONCEALED_SECRET);
	}
	else
	{
		this->flags &= ~TBFLAG_CONCEALED;
		this->pageItemFlags &= ~PIFLAG_DISABLED;
		this->setSelectable(true);
		this->setText(this->stringIndex);	
	}


	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::TEXTBOX
	Purpose 	: constructor
	Parameters 	: byte format string, argument list
	Returns 	: new TEXTBOX instance containing the text passed in
	Info 		: contains a source of phenylalanine
*/
TEXTBOX::TAG_TEXTBOX(const char * const fmt, ...)
{
	this->init();

	// PP: TEXTBOX::sprintf8...
	// PP: (I can't pass variable arguments on to sprintf8 otherwise I would)
		
	char			tempStr[TEXTFX_STRING_LENGTH];
	unsigned short	tempStrW[TEXTFX_STRING_LENGTH];

	va_list			argp;

	va_start(argp, fmt);
	vsprintf(&tempStr[0], fmt, argp);
	va_end(argp);

	bkString8to16(tempStrW, tempStr);

	// PP: recalc src text length
// PP: 	this->sourceTextLen=bkStringLength16(this->sourceTextBuffer);

	// PP: read in pointers from the same varg list.
	// PP: NOTE - therefore you can't mix vargs for the string with permanent pointers for the printbox

	// PP: eg. in the CityBoss BOOK SETUP, Chris just says 'pTextBox[TB_COLLECTED]->sprintf8("@d", &pStatus->multiplayer.numStars);'
	// PP: So the contents update automatically throughout the game.  Pretty nifty Wilson!

	va_start(argp, fmt);
	this->readPointers(argp, tempStrW);
	va_end(argp);

	// PP: (should be safe to omit checkCharacters and udpateAfterTextChange)
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::sprintf8
	Purpose 	: write formatted data to the text box
	Parameters 	: format string, argument list
	Returns 	: this
	Info 		: sounds like a pop group of some sort
*/
TEXTBOX* TEXTBOX::sprintf8(const char * const fmt, ...)
{
	char			tempStr[TEXTFX_STRING_LENGTH];
	unsigned short	tempStrW[TEXTFX_STRING_LENGTH];

	va_list			argp;

	if(this == NULL)
	{
		// PP: people are always trying to sprintf into NULL textboxes
		return NULL;
	}

	// PP: clear character-case
	this->characterCase = TBCASE_NONE;

	// PP: clear string index
	this->stringIndex=NOSTRING;

	va_start(argp, fmt);
	vsprintf(&tempStr[0], fmt, argp);
	va_end(argp);

	bkString8to16(tempStrW, tempStr);

	// PP: recalc src text length
// PP: 	this->sourceTextLen=bkStringLength16(this->sourceTextBuffer);

	// PP: read in pointers from the same varg list.
	// PP: NOTE - therefore you can't mix vargs for the string with permanent pointers for the printbox

	// PP: eg. in the CityBoss BOOK SETUP, Chris just says 'pTextBox[TB_COLLECTED]->sprintf8("@d", &pStatus->multiplayer.numStars);'
	// PP: So the contents update automatically throughout the game.  Pretty nifty Wilson!

	va_start(argp, fmt);
	this->readPointers(argp, tempStrW);
	va_end(argp);

	// PP: perform any update that needs to be done in response the the text changing
	this->updateAfterTextChange();

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::sprintf16
	Purpose 	: write formatted data to the text box
	Parameters 	: format string, argument list
	Returns 	: this
	Info 		: 
*/

TEXTBOX* TEXTBOX::sprintf16(const unsigned short * const fmt, ...)
{
	unsigned short	tempStrW[TEXTFX_STRING_LENGTH];
	va_list			argp;

	// PP: clear character-case
	this->characterCase = TBCASE_NONE;

	// PP: clear string index
	this->stringIndex=NOSTRING;

	va_start(argp, fmt);
	// LH: vswprintf is not present on the gamecube - use bkStringSprintf16
	//vswprintf(&tempStrW[0], fmt, argp);
	bkStringVSprintf16(&tempStrW[0], fmt, argp);
	va_end(argp);

	va_start(argp, fmt);
	this->readPointers(argp, tempStrW);
	va_end(argp);

	return this;
}


/*
void TEXTBOX::setPointers(void *p0, ...)
{
	int i;
	va_list argp;
	void *p;

	this->numArgPtr = 0;
	this->ppArgPtr[this->numArgPtr++] = p0;

	va_start(argp, p0);

	for(;;)
	{
		p = this->ppArgPtr[this->numArgPtr++];
		if(!p)
			break;
	}



	va_end(argp);
}
*/



void TEXTBOX::readPointers(va_list argp, unsigned short *sw)
{
	unsigned short	*s;
	unsigned short	*r = this->text;

	this->numArgPtr = 0;
	// CPW: some of my fancy stuff to scan for pointers and dereference them on the fly
	for(s=sw;; s++, r++)
	{
		if((*s)==0)
		{
			*r = 0;
			break;
		}

		/* PP: REMOUT: we don't use this any more; we now need to be able to sprint button codes in, eg @(cross)
		
		if((*s)=='@')
		{
			// CPW: There should be a letter after this!
			s++;

			switch(*s)
			{
			case 'f': 
				*r = TEXTFX_PTOKEN_FLOAT;
				break;
			case 'l':
				*r = TEXTFX_PTOKEN_LONG;
				break;
			case 'd':
				*r = TEXTFX_PTOKEN_INT;
				break;
			case 's':
				*r = TEXTFX_PTOKEN_STRING;
				break;
			case 't':
				*r = TEXTFX_PTOKEN_TIMECLOCK;
				break;
			}
			//*r = (*s) | 0x80;	// CPW: set top bit here, so we know it's a token and not a character

			this->ppArgPtr[this->numArgPtr] = va_arg(argp, void *);
			this->numArgPtr++;
		}
		else*/
		{
			*r=*s;	// CPW: copy the character to the new buffer
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::strcatf
	Purpose 	: append formatted data into the text box
	Parameters 	: format string, argument list
	Returns 	: 
	Info 		: 
*/
void TEXTBOX::strcatf(const char * const fmt, ...)
{
	char			tempStr[TEXTFX_STRING_LENGTH];
	va_list			argp;
	ushort*			wCharPtr;

	// PP: clear character-case
	this->characterCase = TBCASE_NONE;

	// PP: clear string index
	this->stringIndex=NOSTRING;

	va_start(argp, fmt);
	vsprintf(&tempStr[0], fmt, argp);
	va_end(argp);

	// PP: get to the end of the string
	wCharPtr= &this->text[0];

	while((*wCharPtr) != 0)
	{
		wCharPtr++;
	}

	// PP: append the new text
	bkString8to16(wCharPtr, tempStr);

	// PP: (should be safe to omit checkCharacters)

	// PP: perform any update that needs to be done in response the the text changing
	this->updateAfterTextChange();
}


/*	--------------------------------------------------------------------------------
	Function 	: TEXTBOX::setCase
	Purpose 	: set the case of the textbox's string
	Parameters 	: format string, argument list
	Returns 	: 
	Info 		: // PP: case is reset to CASE_NONE by setText, sprintf, etc.
					// PP: Case persists through changes in language
*/

// PP: wide-character code for a space
#define WIDESTRING_SPACE		0x20

void TEXTBOX::setCase(const TBCase caseIn)
{
	// TP: Good guess Phil, _wcslwr does'nt exist on PS2
#if (BPLATFORM == PC) || (BPLATFORM == XBOX)
	this->characterCase=caseIn;

	if(this->text[0] == 0x00)
	{
		// PP: no string to work with
		return;
	}

	switch(this->characterCase)
	{
	case TBCASE_NONE:
		{
			// PP: read in the original string again to clear the case back to normal

			if(this->stringIndex != NOSTRING)
			{
				this->setText(this->stringIndex);
			}
		}
		break;

	case TBCASE_LOWER:
		{
			// PP: convert the text into lower-case

			// PP: NOTE: If _wcslwr doesn't exist on your platform, just exclude this function from compilation on your platform; don't worry about it

			_wcslwr(this->text);
		}
		break;

	case TBCASE_UPPER:
		{
			// PP: convert the text into lower-case

			// PP: NOTE: If _wcsupr doesn't exist on your platform, just exclude this function from compilation on your platform; don't worry about it

			_wcsupr(this->text);
		}
		break;

	case TBCASE_TITLE:
		{
			// PP: convert the text into title-case...

			ushort		tempChr[2];
			ushort		*chrPtr;

			// PP: null-terminate tempChr
			tempChr[1]=0x00;

			for(chrPtr= &this->text[0]; *chrPtr != 0x00; chrPtr++)
			{
				if((chrPtr == &this->text[0]) || (*(chrPtr-1) == WIDESTRING_SPACE))
				{
					// PP: This is the first character of a word

					tempChr[0]= *chrPtr;

					// PP: Convert this character to upper-case.
					// PP: NOTE: If _wcsupr doesn't exist on your platform, just exclude this function from compilation on your platform; don't worry about it
					_wcsupr(tempChr);

					// PP: Copy it back into this->text
					*chrPtr=tempChr[0];
				}
			}
		}
		break;

	case TBCASE_SENTENCE:
		{
			// PP: convert the text into sentence-case...

			// PP: NOTE!: At present, this only supports single-sentence strings, and none of yer fancy "¡ AYE CARAMBA !" nonsense!
			// PP: If you need it to support multi-sentence strings or strings beginning with punctuation, PLEASE MAKE IT DO SO!!

			ushort		tempChr[2];
			ushort		*chrPtr;

			// PP: null-terminate tempChr
			tempChr[1]=0x00;

			chrPtr = &this->text[0];

			if(*chrPtr != NULL)
			{
				// PP: This is the first character of the sentence

				tempChr[0]= *chrPtr;

				// PP: Convert this character to upper-case.
				// PP: NOTE: If _wcsupr doesn't exist on your platform, just exclude this function from compilation on your platform; don't worry about it
				_wcsupr(tempChr);

				// PP: Copy it back into this->text
				*chrPtr=tempChr[0];

				for(chrPtr= &this->text[1]; *chrPtr != 0x00; chrPtr++)
				{
					if(*(chrPtr-1) == WIDESTRING_SPACE)
					{
						// PP: This is the first character of a word

						tempChr[0]= *chrPtr;

						// PP: Convert this character to lower-case.
						// PP: NOTE: If _wcslwr doesn't exist on your platform, just exclude this function from compilation on your platform; don't worry about it
						_wcslwr(tempChr);

						// PP: Copy it back into this->text
						*chrPtr=tempChr[0];
					}
				}
			}
		}
		break;

		NODEFAULT;
	}
#endif
}