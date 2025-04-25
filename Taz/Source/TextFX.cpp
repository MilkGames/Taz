// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : TextFX.cpp
//   Purpose : Text effects, eg. button glyphs, coloured text
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "maths.h"
#include "main.h"
#include "ThoughtBubble.h"		// PP: thought-bubbles
#include "SFX.h"				// PP: sound effects
#include "text2.h"				// PP: game text enumeration
#include "TextFX.h"				// PP: text effects
#include "display.h"
#include "VideoFX.h"			// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "control.h"			// PP: control stuff, for navigation
#include "sonystuff.h"
#include "util.h"
#include "PageItemFX.h"			// PP: Special effects for page items
#include "files.h"				// PP: file stuff
#include "languagemain.h"		// PP: main language stuff
#include "BookList.h"			// PP: List of books - handles drawing, update etc.
#include "Table.h"				// PP: Table page-item type - lets you line up items into a grid arrangement// PP: need to include this so's some overrides get picked up


enum EGlyph
{
	GLYPH_TRIANGLE,				// PP: ps2 triangle button
	GLYPH_CIRCLE,				// PP: ps2 circle button
	GLYPH_CROSS,				// PP: ps2 cross button
	GLYPH_SQUARE,				// PP: ps2 square button

	GLYPH_EAT,					// PP: eat button	// PP: // PP: mmmm, this is dodgy - had to introduce a seperate EAT glyph because Xbox TCR forces us to use B for back, not Y	// PP: This mess could've been avoided if I'd had a bit more foresight in the beginning - sorry!

	GLYPH_DLEFT,				// PP: ps2 d-pad left button					
	GLYPH_DRIGHT,				// PP: ps2 d-pad right button					
	GLYPH_DUP,					// PP: ps2 d-pad up button					
	GLYPH_DDOWN,				// PP: ps2 d-pad down button
				
	GLYPH_LEFT,					// PP: left					
	GLYPH_RIGHT,				// PP: right					
	GLYPH_UP,					// PP: up					
	GLYPH_DOWN,					// PP: down	
				
	GLYPH_L1,					// PP: l1					
	GLYPH_R1,					// PP: r1					
	GLYPH_L2,					// PP: l2					
	GLYPH_R2,					// PP: r2
	
	GLYPH_START,				// PP: start button					
	GLYPH_SELECT,				// PP: select/back button

	GLYPH_BULLETPOINT,			// PP: bullet point

	NUM_GLYPHS					// PP: Number of glyphs
};


static const char* TextFX_glyphEscapeCodes[NUM_GLYPHS]=
{
	// PP: NOTE: Ideally, the code strings would have been more purpose-oriented, like "jump", "spin" etc.

	"triangle",
	"circle",
	"cross",
	"square",

	"eat",

	"d left",
	"d right",
	"d up",
	"d down",
	
	"left",
	"right",
	"up",
	"down",

	"l1",
	"r1",
	"l2",
	"r2",

	"start",
	"select",

	"bullet",
};

// PP: crcs of the escape code strings
static uint32 TextFX_glyphCRCs[NUM_GLYPHS];

#if(BPLATFORM == XBOX)// PP: Xbox button glyphs look a bit wee

// TP: was 35
	#define BUTTON_GLYPH_WIDTH				43											// PP: width of button glyphs
	#define BUTTON_GLYPH_HEIGHT				43											// PP: height of button glyphs
	#define SPECIALBUTTON_GLYPH_WIDTH		100

#else// PP: if (BPLATFORM == XBOX) else

// TP: was 32
	#define BUTTON_GLYPH_WIDTH				40											// PP: width of button glyphs
	#define BUTTON_GLYPH_HEIGHT				40											// PP: height of button glyphs
	#define SPECIALBUTTON_GLYPH_WIDTH		90

#endif// PP: (BPLATFORM == XBOX)

#define BIGBUTTON_GLYPH_WIDTH			48
#define BIGBUTTON_GLYPH_HEIGHT			48

struct GLYPH
{
	char			imageName[256];
	TBTexture*		images[NUM_SUPPORTED_LANGUAGES];
	float			width;
	float			height;
};

// PP: all 5 textures of a glyph (1 per language) initialised to NULL
#define GLYPH_NULL_TEXTURES							{NULL, NULL, NULL, NULL, NULL}

#define INITIALISE_STANDARD_GLYPH(_image)			{#_image##".bmp", GLYPH_NULL_TEXTURES, BUTTON_GLYPH_WIDTH, BUTTON_GLYPH_HEIGHT}

#define INITIALISE_BIG_GLYPH(_image)				{#_image##".bmp", GLYPH_NULL_TEXTURES, BIGBUTTON_GLYPH_WIDTH, BIGBUTTON_GLYPH_HEIGHT}

#define INITIALISE_SPECIAL_GLYPH(_image)			{#_image##".bmp", GLYPH_NULL_TEXTURES, SPECIALBUTTON_GLYPH_WIDTH, BUTTON_GLYPH_HEIGHT}

static GLYPH		TextFX_glyphs[NUM_GLYPHS]=
{
#if(BPLATFORM == PS2)

	INITIALISE_STANDARD_GLYPH		(triangle),
	INITIALISE_STANDARD_GLYPH		(circle),
	INITIALISE_STANDARD_GLYPH		(cross),
	INITIALISE_STANDARD_GLYPH		(square),
									
	INITIALISE_STANDARD_GLYPH		(triangle),
									
	INITIALISE_STANDARD_GLYPH		(dpadleft),
	INITIALISE_STANDARD_GLYPH		(dpadright),
	INITIALISE_STANDARD_GLYPH		(dpadup),
	INITIALISE_STANDARD_GLYPH		(dpaddown),
									
	INITIALISE_STANDARD_GLYPH		(arrowleft),
	INITIALISE_STANDARD_GLYPH		(arrowright),
	INITIALISE_STANDARD_GLYPH		(arrowup),
	INITIALISE_STANDARD_GLYPH		(arrowdown),

	// PP: expand shoulder button glyphs a bit
	INITIALISE_STANDARD_GLYPH			(l1),
	INITIALISE_STANDARD_GLYPH			(r1),
	INITIALISE_STANDARD_GLYPH			(l2),
	INITIALISE_STANDARD_GLYPH			(r2),
									
	INITIALISE_SPECIAL_GLYPH		(start),
	INITIALISE_SPECIAL_GLYPH		(select),
									
	INITIALISE_STANDARD_GLYPH		(redbutton),			// PP: PLACEHOLDER? (bullet point)

#elif(BPLATFORM == XBOX)
									
	INITIALISE_STANDARD_GLYPH		(button_b),
	INITIALISE_STANDARD_GLYPH		(button_b),
	INITIALISE_STANDARD_GLYPH		(button_a),
	INITIALISE_STANDARD_GLYPH		(button_x),
									
	INITIALISE_STANDARD_GLYPH		(button_y),
									
	INITIALISE_STANDARD_GLYPH		(arrowleft),
	INITIALISE_STANDARD_GLYPH		(arrowright),
	INITIALISE_STANDARD_GLYPH		(arrowup),
	INITIALISE_STANDARD_GLYPH		(arrowdown),
									
	INITIALISE_STANDARD_GLYPH		(arrowleft),
	INITIALISE_STANDARD_GLYPH		(arrowright),
	INITIALISE_STANDARD_GLYPH		(arrowup),
	INITIALISE_STANDARD_GLYPH		(arrowdown),
									
	INITIALISE_STANDARD_GLYPH		(button_white),
	INITIALISE_STANDARD_GLYPH		(button_black),
	INITIALISE_STANDARD_GLYPH		(l2),
	INITIALISE_STANDARD_GLYPH		(r2),

	INITIALISE_SPECIAL_GLYPH		(button_start),
	INITIALISE_SPECIAL_GLYPH		(button_back),
									
	INITIALISE_STANDARD_GLYPH		(redbutton),			// PP: PLACEHOLDER? (bullet point)
									
#elif(BPLATFORM == PC)				
									
	INITIALISE_STANDARD_GLYPH		(icon_bite),
	INITIALISE_STANDARD_GLYPH		(icon_spin),
	INITIALISE_STANDARD_GLYPH		(icon_jump),
	INITIALISE_STANDARD_GLYPH		(icon_rant),
									
	INITIALISE_STANDARD_GLYPH		(icon_bite),
									
	INITIALISE_STANDARD_GLYPH		(icon_left),
	INITIALISE_STANDARD_GLYPH		(icon_right),
	INITIALISE_STANDARD_GLYPH		(icon_forward),
	INITIALISE_STANDARD_GLYPH		(icon_backward),
									
	INITIALISE_STANDARD_GLYPH		(icon_left),
	INITIALISE_STANDARD_GLYPH		(icon_right),
	INITIALISE_STANDARD_GLYPH		(icon_forward),
	INITIALISE_STANDARD_GLYPH		(icon_backward),
									
	INITIALISE_STANDARD_GLYPH		(icon_cameraleft),
	INITIALISE_STANDARD_GLYPH		(icon_cameraright),
	INITIALISE_STANDARD_GLYPH		(icon_tiptoe),
	INITIALISE_STANDARD_GLYPH		(icon_lookaround),
									
	INITIALISE_STANDARD_GLYPH		(icon_pause),
	INITIALISE_STANDARD_GLYPH		(icon_selectmap),
									
	INITIALISE_STANDARD_GLYPH		(redbutton),			// PP: PLACEHOLDER? (bullet point)
									
#else // IH: BPLATFORM == GAMECUBE
									
	INITIALISE_STANDARD_GLYPH		(button_b),
	INITIALISE_STANDARD_GLYPH		(button_b),
	INITIALISE_STANDARD_GLYPH		(button_a),
	INITIALISE_STANDARD_GLYPH		(button_x),
									
	INITIALISE_STANDARD_GLYPH		(button_y),
									
	INITIALISE_STANDARD_GLYPH		(dpadleft),
	INITIALISE_STANDARD_GLYPH		(dpadright),
	INITIALISE_STANDARD_GLYPH		(dpadup),
	INITIALISE_STANDARD_GLYPH		(dpaddown),
									
	INITIALISE_STANDARD_GLYPH		(arrowleft),
	INITIALISE_STANDARD_GLYPH		(arrowright),
	INITIALISE_STANDARD_GLYPH		(arrowup),
	INITIALISE_STANDARD_GLYPH		(arrowdown),

	INITIALISE_STANDARD_GLYPH		(l1),
	INITIALISE_STANDARD_GLYPH		(r1),
	INITIALISE_STANDARD_GLYPH		(l2),	// IH: Same as l1
	INITIALISE_STANDARD_GLYPH		(r2),	// IH: Same as r1
									
	INITIALISE_STANDARD_GLYPH		(start),
	INITIALISE_STANDARD_GLYPH		(button_z),
									
	INITIALISE_STANDARD_GLYPH		(redbutton),			// PP: PLACEHOLDER? (bullet point)

#endif// PP: platform
};


// PP: ///////////////////////////////////////////


PAGE*	g_tempPage;

// PP: index of the current character poly
ushort	TextFX_polyIndex;


// PP: Macro to fill-in a RECTANGLE struct with the vert positions of a text character (in a text callback)
#define BDVERTEX_GETCHARRECT(_verts, _rect) \
		{ \
			BDVERTEX_GETX(&(_verts)[0], (_rect).left); \
			BDVERTEX_GETX(&(_verts)[2], (_rect).right); \
			BDVERTEX_GETY(&(_verts)[0], (_rect).bottom); \
			BDVERTEX_GETY(&(_verts)[1], (_rect).top); \
		}

// PP: Macro to fill-in  the vert positions of a text character (in a text callback) from a RECTANGLE struct
#define BDVERTEX_SETCHARRECT(_verts, _rect) \
		{ \
			BDVERTEX_SETX(&(_verts)[0], (_rect).left); \
			BDVERTEX_SETY(&(_verts)[0], (_rect).bottom); \
			\
			BDVERTEX_SETX(&(_verts)[1], (_rect).left); \
			BDVERTEX_SETY(&(_verts)[1], (_rect).top); \
			\
  			BDVERTEX_SETX(&(_verts)[2], (_rect).right); \
			BDVERTEX_SETY(&(_verts)[2], (_rect).bottom); \
			\
  			BDVERTEX_SETX(&(_verts)[3], (_rect).right); \
			BDVERTEX_SETY(&(_verts)[3], (_rect).top); \
		}

#define TEXTFX_UPDATE_SCREEN_RECTS_AT_DRAW													// PP: update screen rectangles of items as they draw, rather than in a sep'rate stage


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_escapeCallback
	Purpose 	: handles text escape codes, eg. button glyphs, coloured text
	Parameters 	: reason code, info pointer
	Returns 	: 1
	Info 		: 
*/

enum
{
	POLYMARK_BUTTONGLYPH		=	2,
	POLYMARK_DPADGLYPH,
	POLYMARK_SELECTIONSTART,
	POLYMARK_SELECTIONEND,
	POLYMARK_TITLESTART,
	POLYMARK_TITLEEND,
	POLYMARK_COLOUR,
	POLYMARK_ALPHA,
	POLYMARK_TEMPSTART,
	POLYMARK_TEMPEND,
	POLYMARK_DISABLEDSTART,
	POLYMARK_DISABLEDEND,
	POLYMARK_ITALICSTART,
	POLYMARK_ITALICEND,

	NUM_POLY_MARKS
};

#define MARK_POLY(_num)					info->curGlyph->u[0]= (float)_num; \
										info->curGlyph->u[1]= (float)_num; \
										info->curGlyph->u[2]= (float)_num; \
										info->curGlyph->u[3]= (float)_num

#define GET_POLY_MARK(_mark)			BDVERTEX_GETU(&(info->curVertex[0]), _mark);

// PP: DIRTY BUT NECESSARY FRIGS: give this glyph's poly a distinguishing feature
//		so we can recognise it when we respond to the EBFONTCALLBACKREASON_POLY reason...

#define POLY_MARKED						((int)polyMark>=2)							// PP: find out if a poly is marked at all

#define MARK_POLY_AS_BUTTON_GLYPH		MARK_POLY(POLYMARK_BUTTONGLYPH)				// PP: mark poly as button glyph
#define POLY_MARKED_AS_BUTTON_GLYPH		((int)polyMark==POLYMARK_BUTTONGLYPH)		// PP: see if this poly is a button glyph

#define BUTTON_GLYPH_PULSE_PERIOD		0.6f										// PP: period of sinusoidal button-glyph pulse (seconds)
#define BUTTON_GLYPH_PULSE_DIST			0.065f										// PP: extent of pulse

#define TEXTFX_DEFAULT_ALPHA			128											// PP: default alpha value of text

int TextFX_escapeCallback(EBFontCallbackReason reason, TBFontCallbackInfo *info)
{
	float					phase;
	float					texCoordDelta;

	static unsigned char	a=TEXTFX_DEFAULT_ALPHA;

	static int				numButtonGlyphs=0;
	static int				numButtonGlyphsDrawn=0;

	bool					setTextColour=true;

	float					polyMark;
	TEXTBOX					*textBox = (TEXTBOX*)info->context;
	TBVector				centre;
	TBVector				offset[4];
	int						i;
	uint					tazLang;

	if(reason == EBFONTCALLBACKREASON_ESCAPE)
	{
		tazLang=BLANG_TO_TAZLANG(gameStatus.currentLanguage);

		uint32	codeCRC;
		
		codeCRC=bkStringCRC(info->escapeString);

		// PP: Check the escape code against all our glyph codes...

		for(int i=0; i<NUM_GLYPHS; i++)
		{
			if(codeCRC == TextFX_glyphCRCs[i])
			{
				info->curGlyph->height=TextFX_glyphs[i].height;
				info->curGlyph->width=TextFX_glyphs[i].width;
				info->curGlyph->texture=TextFX_glyphs[i].images[tazLang];

				MARK_POLY_AS_BUTTON_GLYPH;

				if((!textBox)||(textBox && (textBox->stage == TEXTFX_NORMAL)))
				{
					numButtonGlyphs++;
				}

				break;
			}
		}
	}
	else
	{
		if(reason == EBFONTCALLBACKREASON_POLY)
		{
			GET_POLY_MARK(polyMark);

			// PP: APPLY ITALICS...

			if(!POLY_MARKED_AS_BUTTON_GLYPH)// PP: don't italic button glyphs!
			{
				// PP: distance to offset character verts for italic
#define TEXTFX_ITALIC_DISTANCE		3.5f

				if(textSettings.flags & TEXTFX_ITALIC)
				{
					float		left,right,newLeft,newRight;

					// PP: too tired for comments right now - it ain't rocket science so I'm sure you see what's going on.
					// PP: verts go in an N shape.
					
					BDVERTEX_GETX(&info->curVertex[0], left);
					BDVERTEX_GETX(&info->curVertex[2], right);

					newLeft=(left-TEXTFX_ITALIC_DISTANCE);
					newRight=(right-TEXTFX_ITALIC_DISTANCE);

					BDVERTEX_SETX(&info->curVertex[0], newLeft);
					BDVERTEX_SETX(&info->curVertex[2], newRight);

					newLeft=(left+TEXTFX_ITALIC_DISTANCE);
					newRight=(right+TEXTFX_ITALIC_DISTANCE);

					BDVERTEX_SETX(&info->curVertex[1], newLeft);
					BDVERTEX_SETX(&info->curVertex[3], newRight);
				}
			}

			if(textBox)
			{
				switch(textBox->stage)
				{
				case TEXTFX_SCRIBBLE:
					for(i=0;i<4;i++)
					{
						BDVERTEX_GETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
						switch(textBox->drawOffsetDirection[0])
						{
						case TEXTFXOFFSET_UP:
							offset[i][1] -= textBox->drawOffset;
							break;
						case TEXTFXOFFSET_DOWN:
							offset[i][1] += textBox->drawOffset;
							break;
						case TEXTFXOFFSET_LEFT:
							offset[i][0] -= textBox->drawOffset;
							break;
						case TEXTFXOFFSET_RIGHT:
							offset[i][0] += textBox->drawOffset;
							break;
						}
						BDVERTEX_SETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
					}
					break;
				case TEXTFX_SCRIBBLETWO:
					for(i=0;i<4;i++)
					{
						BDVERTEX_GETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
						switch(textBox->drawOffsetDirection[1])
						{
						case TEXTFXOFFSET_UP:
							offset[i][1] -= textBox->drawOffset;
							break;
						case TEXTFXOFFSET_DOWN:
							offset[i][1] += textBox->drawOffset;
							break;
						case TEXTFXOFFSET_LEFT:
							offset[i][0] -= textBox->drawOffset;
							break;
						case TEXTFXOFFSET_RIGHT:
							offset[i][0] += textBox->drawOffset;
							break;
						}
						BDVERTEX_SETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
					}
					break;
				case TEXTFX_OUTLINE:
					SETVECTOR(centre,0.0f,0.0f,0.0f,0.0f);

					for(i=0;i<4;i++)
					{
						BDVERTEX_GETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
						centre[0] += offset[i][0];
						centre[1] += offset[i][1];
						centre[2] += offset[i][2];
						centre[3] += offset[i][3];
					}
					bmVectorCDiv(centre,centre,4.0f);

					// CMD: find offsets from centre, rather than actual vert positions
					for(i=0;i<4;i++)
					{
						bmVectorSub(offset[i],offset[i],centre);
						bmVectorCMul(offset[i],offset[i],1.2f);
						// CMD: apply new values to poly
						BDVERTEX_SETXYZW(&info->curVertex[i],(offset[i][0]+centre[0]),(offset[i][1]+centre[1]),(offset[i][2]+centre[2]),1.0f);
					}
					break;
				case TEXTFX_INLINE:
					SETVECTOR(centre,0.0f,0.0f,0.0f,0.0f);

					for(i=0;i<4;i++)
					{
						BDVERTEX_GETXYZW(&info->curVertex[i],offset[i][0],offset[i][1],offset[i][2],offset[i][3]);
						centre[0] += offset[i][0];
						centre[1] += offset[i][1];
						centre[2] += offset[i][2];
						centre[3] += offset[i][3];
					}
					bmVectorCDiv(centre,centre,4.0f);

					// CMD: find offsets from centre, rather than actual vert positions
					for(i=0;i<4;i++)
					{
						bmVectorSub(offset[i],offset[i],centre);
						bmVectorCDiv(offset[i],offset[i],1.4f);
						// CMD: apply new values to poly
						BDVERTEX_SETXYZW(&info->curVertex[i],(offset[i][0]+centre[0]),(offset[i][1]+centre[1]),(offset[i][2]+centre[2]),1.0f);
					}
					break;
				case TEXTFX_OUTLINE_UPLEFT:
					{
						float outlineWidth=1.0f; //textBox->getOutlineWidth();

						for(i=0;i<4;i++)
						{
							BDVERTEX_GETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],centre[3]);

							// CMD: move a little in the desired direction
							centre[0] -= outlineWidth;
							centre[1] += outlineWidth;
							
							// CMD: apply new values to poly
							BDVERTEX_SETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],1.0f);
						}
					}
					break;
				case TEXTFX_OUTLINE_UPRIGHT:
					{
						float outlineWidth=1.0f; //textBox->getOutlineWidth();

						for(i=0;i<4;i++)
						{
							BDVERTEX_GETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],centre[3]);

							// CMD: move a little in the desired direction
							centre[0] += outlineWidth;
							centre[1] += outlineWidth;
							
							// CMD: apply new values to poly
							BDVERTEX_SETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],1.0f);
						}
					}
					break;
				case TEXTFX_OUTLINE_DOWNLEFT:
					{
						float outlineWidth=1.0f; //textBox->getOutlineWidth();

						for(i=0;i<4;i++)
						{
							BDVERTEX_GETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],centre[3]);

							// CMD: move a little in the desired direction
							centre[0] -= outlineWidth;
							centre[1] -= outlineWidth;
							
							// CMD: apply new values to poly
							BDVERTEX_SETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],1.0f);
						}
					}
					break;
				case TEXTFX_OUTLINE_DOWNRIGHT:
					{
						float outlineWidth=1.0f; //textBox->getOutlineWidth();

						for(i=0;i<4;i++)
						{
							BDVERTEX_GETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],centre[3]);

							// CMD: move a little in the desired direction
							centre[0] += outlineWidth;
							centre[1] -= outlineWidth;
							
							// CMD: apply new values to poly
							BDVERTEX_SETXYZW(&info->curVertex[i],centre[0],centre[1],centre[2],1.0f);
						}
					}
					break;
				}// PP: end switch

			}// PP: end if(textBox)

			if((textBox == NULL) || (textBox->stage == TEXTFX_NORMAL))
			{
				a=textSettings.alpha;

				if(POLY_MARKED)
				{
					if(POLY_MARKED_AS_BUTTON_GLYPH)
					{
					  	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);

						if (textSettings.flags & TEXTFX_DISABLED)
						{
							for(int v=0;v<4;v++)
							{
								BDVERTEX_SETRGBA(&info->curVertex[v], 90, 90, 90, (a/1.5f));
							}
						}
						else
						{
							for(int v=0;v<4;v++)
							{
								BDVERTEX_SETRGBA(&info->curVertex[v], 127, 127, 127, a);
							}
						}

						setTextColour=false;						// PP: we've already handled colour for this poly thank-YA.

						 // NH: TEMP Odd little divide by zero here for you to look at at some point Phil.  Put this in to stop crashes
						if(numButtonGlyphs)
						{
							phase=(fmodf(gameStatus.appTime+(BUTTON_GLYPH_PULSE_PERIOD*(numButtonGlyphsDrawn/(float)numButtonGlyphs)), BUTTON_GLYPH_PULSE_PERIOD)/BUTTON_GLYPH_PULSE_PERIOD)*TWOPI;
						}
						else
						{
							phase = 0.0f;
						}

						texCoordDelta=0.0f;//((bmSin(phase)-1.0f)*BUTTON_GLYPH_PULSE_DIST);
						
						BDVERTEX_SETUV(&info->curVertex[0], texCoordDelta, 1.0f-texCoordDelta);
						BDVERTEX_SETUV(&info->curVertex[1], texCoordDelta, texCoordDelta);
						BDVERTEX_SETUV(&info->curVertex[2], 1.0f-texCoordDelta, 1.0f-texCoordDelta);
						BDVERTEX_SETUV(&info->curVertex[3], 1.0f-texCoordDelta, texCoordDelta);


						RECTANGLE	rect;
					
						BDVERTEX_GETCHARRECT(info->curVertex, rect);

						float		factor=1.0f+((bmSin(phase)-1.0f)*BUTTON_GLYPH_PULSE_DIST);

						rect.mulWidth(factor);
						rect.mulHeight(factor);

						BDVERTEX_SETCHARRECT(info->curVertex, rect);

						if(++numButtonGlyphsDrawn == numButtonGlyphs)
						{
							// PP: we've drawn all the button glyphs in the text - reset counters

							numButtonGlyphs=0;
							numButtonGlyphsDrawn=0;
						}					  
					}
				}// PP: end if POLY_MARKED
			}

			// PP: set text colour (unless the poly is a button glyph)...
			if(setTextColour)
			{
				if(	(textBox != NULL)
					&&((textBox->stage == TEXTFX_OUTLINE_DOWNRIGHT)
					||(textBox->stage == TEXTFX_OUTLINE_DOWNLEFT)
					||(textBox->stage == TEXTFX_OUTLINE_UPRIGHT)
					||(textBox->stage == TEXTFX_OUTLINE_UPLEFT)))
				{
					COLOUR	outlineCol;

					outlineCol=textBox->getOutlineColour();

					outlineCol.a = (uchar)(outlineCol.a*(textSettings.alpha/128.0f));
						
					for(int v=0;v<4;v++)
					{
						BDVERTEX_SETCOLOUR(&info->curVertex[v], outlineCol);
					}
				}
				else
				{
					if(textSettings.flags & TEXTFX_SELECTED)
					{
						for(int v=0;v<4;v++)
						{
							// PP: BDVERTEX_SETRGBA(&info->curVertex[v], 229.5f, 15.3f, 15.3f, a);// PP: OH-SO-VERY-TEMP

							// PP: TEMP!!!!!!
							if(bmFMod(gameStatus.appTime, 0.4f)>0.2f)
							{
								BDVERTEX_SETRGBA(&info->curVertex[v], 128, 128, 128, a);// PP: OH-SO-VERY-TEMP
							}
							else
							{
								BDVERTEX_SETRGBA(&info->curVertex[v], 0, 128, 128, a);// PP: OH-SO-VERY-TEMP
							}
						}
					}
					else if(textSettings.flags & TEXTFX_DISABLED)
					{
						// PP: reduce the alpha, for a greyed-out kinda look
						for(int v=0;v<4;v++)
						{
							BDVERTEX_SETRGBA(&info->curVertex[v], textSettings.vertColours[v].r, textSettings.vertColours[v].g, textSettings.vertColours[v].b, bmFloatToInt(textSettings.alpha*0.3f));
						}
					}
					else
					{
						for(int v=0;v<4;v++)
						{
							BDVERTEX_SETRGBA(&info->curVertex[v], textSettings.vertColours[v].r, textSettings.vertColours[v].g, textSettings.vertColours[v].b, bmFloatToInt((textSettings.vertColours[v].a/128.0f)*textSettings.alpha));
						}
					}
				}
			}

			// PP: enforce text clipping...
			if(textSettings.clipLR)
			{
				float		vertX;
				float		polyWidth;
				float		minX;
				float		maxX;
				float		minU;
				float		maxU;
				float		vertU;
				int			v;						// PP: loop counter for vert loops

				// PP: get width of poly...
				BDVERTEX_GETX(&info->curVertex[0], minX);
				maxX=minX;
				for(v=1;v<4;v++)
				{
					BDVERTEX_GETX(&info->curVertex[v], vertX);

					minX=MIN(minX, vertX);
					maxX=MAX(maxX, vertX);
				}
				polyWidth=maxX-minX;

				// PP: get U extents...
				BDVERTEX_GETU(&info->curVertex[0], minU);
				maxU=minU;
				for(v=1;v<4;v++)
				{
					BDVERTEX_GETU(&info->curVertex[v], vertU);

					minU=MIN(minU, vertU);
					maxU=MAX(maxU, vertU);
				}

				if(polyWidth!=0.0f)
				{
					for(v=0;v<4;v++)
					{
						BDVERTEX_GETX(&info->curVertex[v], vertX);

						if(vertX < textSettings.clipLeft)
						{
							BDVERTEX_SETU(&info->curVertex[v], minU+(((textSettings.clipLeft-vertX)/polyWidth)*(maxU-minU)) );

							BDVERTEX_SETX(&info->curVertex[v], textSettings.clipLeft);
						}
						else if(vertX > textSettings.clipRight)
						{
							BDVERTEX_SETU(&info->curVertex[v], maxU-(((vertX-textSettings.clipRight)/polyWidth)*(maxU-minU)) );

							BDVERTEX_SETX(&info->curVertex[v], textSettings.clipRight);
						}
					}
				}
			}// PP: end if(LR clipping enabled)

			if(textBox != NULL)
			{
				// PP: RIGHT, NOW HAND THE CHARACTER OVER TO THE TEXTBOX'S EFFECT PARASITE, FOR POSSIBLE PROCESSING
				if(textBox->effect)
				{
					textBox->effect->processCharacter(&info->curVertex[0], TextFX_polyIndex);
				}
			}

			// PP: increment poly index
			TextFX_polyIndex++;

		}// PP: end if(POLY reason)
	}// PP: end if(not ESCAPE reason)

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function 	: TextFX_init
	Purpose 	: one-time init for text fx - loads textures, that sort of thing
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_init(void)
{
	// PP: INITIALISE COMMONLY-USED PRESET TEXT STYLES
	initTextStyles();

	// PP: LOAD GLYPH TEXTURES...

	int			tazLang;
	TBTexture	*allLanguagesTexture;
	TBTexture	*localisedTexture;

	for(int i=0; i<NUM_GLYPHS; i++)
	{
		// PP: get escape code crc
		TextFX_glyphCRCs[i]=bkStringCRC(TextFX_glyphEscapeCodes[i]);

		// PP: see if there's an all-languages version of the glyph first
		allLanguagesTexture=bkLoadTexture(NULL, TextFX_glyphs[i].imageName, 0);

		char	fullName[256];
		char	extension[8];
		char*	chPtr;

		for(tazLang=0; tazLang<NUM_SUPPORTED_LANGUAGES; tazLang++)
		{
			// PP: set the all-languages version (might be null, nema problema)
			TextFX_glyphs[i].images[tazLang]=allLanguagesTexture;
			
		// PP: not using localised glyphs in consumer demo
#ifndef CONSUMERDEMO
			
			// PP: whether there was an all-languages version or not, try to load each localised version...
			
			// PP: copy the filename into a temp buffer
			strcpy(fullName, TextFX_glyphs[i].imageName);

			// PP: find the filename extension
			chPtr=strrchr(fullName, '.');

			// PP: check there was one
			ASSERTM(chPtr, "No dot in glyph filename...  NMEUHHH!!!");

			// PP: copy the filename extension
			strcpy(extension, chPtr); 

			// PP: insert the language suffix before the extension.
			strcpy(chPtr, (languageSuffixes[tazLang]));

			// PP: shift filename extension position along by the length of language suffix
			chPtr += strlen(languageSuffixes[tazLang]);

			// PP: re-append filename extension
			strcat(chPtr, extension);

			// PP: try to load the texture
			localisedTexture=bkLoadTexture(NULL, fullName, 0);

			if(localisedTexture)
			{
				// PP: found a localised version
				TextFX_glyphs[i].images[tazLang]=localisedTexture;

				// PP: The way this is set up now is quite handy.
				// PP: you can override the all-languages version of a glyph for whatever combination of languages you want.
				// PP: eg., if only the French version was different, just add an "_f" version as well as the normal version.
				// PP: all the other languages would use the normal version - see?
			}

#endif// PP: ndef CONSUMERDEMO

		}
	}

	// PP: initialise preset page item effects
	initPresetPIEs();
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setAlpha
	Purpose 	: sets the alpha for text and glphs - this should modulate with the alpha escape code(s) if any
	Parameters 	: alpha calue 0..128
	Returns 	: 
	Info 		: 
*/
void TextFX_setAlpha(const unsigned char alpha)
{
	textSettings.alpha=alpha;
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetAlpha
	Purpose 	: resets the alpha for text and glphs to the default value (128)
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetAlpha(void)
{
	textSettings.alpha=TEXTFX_DEFAULT_ALPHA;
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_vGrad
	Purpose 	: set up a vertical colour graduation for normal text
	Parameters 	: rgb for top of characters, rgb for bottom
	Returns 	: 
	Info 		: 
*/
void TextFX_vGrad(const unsigned char r1, const unsigned char g1, const unsigned char b1,
					 const unsigned char r2, const unsigned char g2, const unsigned char b2)
{
	textSettings.vertColours[1]=textSettings.vertColours[3]=COLOUR(r1, g1, b1, TEXTFX_DEFAULT_ALPHA);
	textSettings.vertColours[0]=textSettings.vertColours[2]=COLOUR(r2, g2, b2, TEXTFX_DEFAULT_ALPHA);
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_vGrad
	Purpose 	: set up a vertical colour graduation for normaltext
	Parameters 	: COLOUR for top of characters, COLOUR for bottom
	Returns 	: 
	Info 		: 
*/
void TextFX_vGrad(const COLOUR& col1, const COLOUR& col2)
{
	textSettings.vertColours[1]=textSettings.vertColours[3]=col1;
	textSettings.vertColours[0]=textSettings.vertColours[2]=col2;
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setColour
	Purpose 	: set text colour
	Parameters 	: r,g,b
	Returns 	: 
	Info 		: 
*/
void TextFX_setColour(const unsigned char r, const unsigned char g, const unsigned char b)
{
	for(int v=0;v<4;v++)
	{
		textSettings.vertColours[v]=COLOUR(r,g,b, TEXTFX_DEFAULT_ALPHA);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_select
	Purpose 	: select/deslect subsequent text
	Parameters 	: (opt/true)true to select, false to deselect
	Returns 	: 
	Info 		: 
*/
void TextFX_select(const bool selectIn)
{
	SETFLAG(textSettings.flags, TEXTFX_SELECTED, selectIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setColour
	Purpose 	: set text colour
	Parameters 	: colour
	Returns 	: 
	Info 		: 
*/
void TextFX_setColour(const COLOUR& colour)
{
	for(int v=0;v<4;v++)
	{
		textSettings.vertColours[v]=colour;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetColour
	Purpose 	: resets the vert colours used for normal text
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetColour(void)
{
	for(int i=0;i<4;i++)
	{
		textSettings.vertColours[i]=COLOUR(TEXTFX_DEFAULT_RED, TEXTFX_DEFAULT_GREEN, TEXTFX_DEFAULT_BLUE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_setClipWidth
	Purpose 	: sets the width of the rectangle within which characters are visble
	Parameters 	: 
	Returns 	: 
	Info 		: This is not the same as setting a format width!
*/
void TextFX_setClipWidth(const float width)
{
	textSettings.clipLeft= -width*0.5f;
	textSettings.clipRight= width*0.5f;
	
	textSettings.clipLR=true;
}


/*	--------------------------------------------------------------------------------
	Function 	: TextFX_resetClipping
	Purpose 	: get rid of the clipping rectangle: no text is clipped
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void TextFX_resetClipping(void)
{
	textSettings.clipLR=false;
}

/*	--------------------------------------------------------------------------------
	Function 	: TextFX_drawHollowRect
	Purpose 	: Draw hollow rectangle Pahhh
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
/*void TextFX_drawHollowRect(const RECTANGLE &rect, const COLOUR &colour)
{
	DrawLine(rect.left, rect.top, rect.right, rect.top, colour);
	DrawLine(rect.right, rect.top, rect.right, rect.bottom, colour);
	DrawLine(rect.right, rect.bottom, rect.left, rect.bottom, colour);
	DrawLine(rect.left, rect.bottom, rect.left, rect.top, colour);
}

#define DRAW_HOLLOW_RECT TextFX_drawHollowRect*/

#define MARGIN					1


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGEITEM methods

											Page items are held in rows on pages.
						This base class functionality can be inherited for use with pretty much anything:
				text boxes, memory card browsers, 3D models, bitmaps, icons, monitor effects, pages, books, whatever.

												There's no limit to the fun!™

												book > pages > rows > *ITEMS*

															@pageitem
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::drawWithoutEffect
	Purpose 	: draw the page item, ignoring any effect parasite it might have on it
	Parameters 	: 
	Returns 	: 
	Info 		: this method is used by some parasites to draw the item in the normal way
					(eg. PIE_FADE, which simply works by talking to the item's opacity member
					rather than actually changing the way the item draws itself)
*/
void PAGEITEM::drawWithoutEffect(void)
{
	// PP: aw yeah, this is neat! :o)

	// PP: all these methods are virtual, so pageitem subclasses can pick and choose which bits of the draw they override...

	this->startDraw();
	this->drawBackgroundAndBorder();// PP: NEW
	this->mainDraw();
	this->endDraw();
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::getStringIndex
	Purpose 	: get the string index held by this item (if it does hold a string)
	Parameters 	: 
	Returns 	: NOSTRING, but in relevant subclasses, the EStringIndex held by the item
	Info 		: 
*/
EStringIndex PAGEITEM::getStringIndex(void)
{
	return NOSTRING;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::effectsFinished
	Purpose 	: determine if all effects on the item and its children have finished
	Parameters 	: 
	Returns 	: true if all effects on the item and its children have finished, else false
	Info 		: 
*/
bool PAGEITEM::effectsFinished(void)
{
	if(this->effect == NULL) return true;
	if(this->effect->mode == PIEMODE_ON) return true;// PP: ON-mode effects are 'finished' for the purposes of this method
	return (this->effect->finished);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::hasEffects
	Purpose 	: determine if the item or any of its contents have effects on them
	Parameters 	: 
	Returns 	: true if the item or any of its contents has an effect on it
	Info 		: 
*/
bool PAGEITEM::hasEffects(void)
{
	return (this->effect != NULL);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::freeTempMemory
	Purpose 	: free-up temporary memory used by the item and its contents,
					eg. effects that have finished
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::freeTempMemory(void)
{
	// PP: delete effect

	SAFE_DELETE(this->effect);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setSelectable
	Purpose 	: enable/disable selectability for individual items
	Parameters 	: true to enable / false to disable, (opt/false)selectable by mouse only (if parm1==true)
	Returns 	: this
	Info 		: 
*/
TAG_PAGEITEM* PAGEITEM::setSelectable(const bool on, const bool mouseOnly)
{
	if(on)
	{
		this->pageItemFlags |= PIFLAG_SELECTABLE;

		if(mouseOnly)
		{
			this->pageItemFlags |= PIFLAG_MOUSEONLY;
		}
		else
		{
			this->pageItemFlags &= ~PIFLAG_MOUSEONLY;
		}
	}
	else
	{
		this->pageItemFlags &= ~PIFLAG_SELECTABLE;
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::enable
	Purpose 	: enable or disable the page item
	Parameters 	: true to enable, false to disable
	Returns 	: this
	Info 		: 
*/
TAG_PAGEITEM* PAGEITEM::enable(const bool state)
{
	if(state)
	{
		this->pageItemFlags &= ~PIFLAG_DISABLED;
	}
	else
	{
		this->pageItemFlags |= PIFLAG_DISABLED;
	}

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::startDraw
	Purpose 	: set up conditions needed to draw the page item (eg. apply item's scale & position to object matrix)
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::startDraw(void)
{
	// PP: set the object matrix scale and translation according to those of the page item...
	TBMatrix		scaleMatrix;
	TBMatrix		transMatrix;
	TBMatrix		tempMatrix;
	TBMatrix		finalMatrix;

	// PP: TODO: optimise some of this?

	bdGetObjectMatrix(this->objectMatrixCopy);	
	bmMatScale(tempMatrix, this->getDrawScale(), this->getDrawScale(), this->getDrawScale());

	bmMatCopy(scaleMatrix, tempMatrix);

	bmMatTranslate(tempMatrix, this->optRect.midX(), this->optRect.midY(), 0.0f);// PP: use optRect centre pos to enforce alignment

	bmMatCopy(transMatrix, tempMatrix);

	bmMatMultiply(tempMatrix, transMatrix, scaleMatrix);
	bmMatMultiply(finalMatrix, this->objectMatrixCopy, tempMatrix);
	bdSetObjectMatrix(finalMatrix);

	// PP: multiply the corner positions of the optrect by the current object matrix, use dem for screen rect

	this->screenRect=this->optRect.toOrigin()*finalMatrix;

	// PP: wee temp test - show current screen rect
 	//this->optRect.toOrigin().draw(ORANGE, OUTLINE);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::mainDraw
	Purpose 	: the core of the draw method - actually draw the page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::mainDraw(void)
{
	// PP: nothing to draw in base class
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::drawDebug
	Purpose 	: draw debug info for the page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
#define PAGEITEM_PREDRAWDEBUG

#define PAGEITEM_MARGIN			(MARGIN*3)

void PAGEITEM::mainDrawDebug(void)
{
	// PP: DRAW MAXIMUM RECTANGLE

	RECTANGLE					tempRect;
	COLOUR						colour=COLOUR(255, 255, 255);

	tempRect=this->rect.toOrigin().removeMargin(PAGEITEM_MARGIN);

	// PP: counteract the draw scale that has been applied by PAGEITEM_PREDRAW - we want to see the REAL rect - and not in a 'Real Ghostbusters' way
	if(this->getDrawScale() != 0.0f)
	{
		tempRect/=this->getDrawScale();
	}

	// PP: counteract the translation applied by PAGEITEM_PREDRAW - we want to see the REAL rect - and not in a 'Real Ghostbusters' way
	tempRect=tempRect.moveX(-this->optRect.midX());
	tempRect=tempRect.moveY(-this->optRect.midY());

	tempRect=tempRect.moveX(this->rect.midX());
	tempRect=tempRect.moveY(this->rect.midY());

	tempRect.draw(colour, OUTLINE);

	// PP: DRAW USED RECTANGLE

	if(fmodf(gameStatus.appTime, 0.4f)<0.2f)
	{
		tempRect=this->optRect.toOrigin().removeMargin(PAGEITEM_MARGIN);

		// PP: counteract the draw scale that has been applied by PAGEITEM_PREDRAW - we want to see the REAL optRect - and not in a 'Real Ghostbusters' way
		if(this->getDrawScale() != 0.0f)
		{
			tempRect/=this->getDrawScale();
		}

		tempRect.draw(colour, OUTLINE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::init
	Purpose 	: initialise the item
	Parameters 	: (opt/NULL=default) name
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::init(const char* const nameIn)
{
	this->opacity=1.0f;

	this->rect.init();					// PP: probably not needed
	this->optRect.init();				// PP: probably not needed
	this->selected=false;
	this->highlighted=false;
	this->onActivate=NULL;
	this->setScale(1.0f);
	this->effect=NULL;
	this->effectGroup=0;				// PP: not used
	
	this->rowIndex= -1;
	this->columnIndex= -1;

	this->container=NULL;

	if(nameIn == NULL)
	{
		this->setName("PInoname");
	}
	else
	{
		this->setName(nameIn);
	}

	this->value= (uint32)(-1);// PP: D'oh, ok I'll change 'value' to be signed when I next fancy a big build

	// PP: there are no instances of this item inserted on pages
	this->numInstances=0;

	// PP: set default item flags
	this->pageItemFlags=PAGEITEM_DEFAULT_FLAGS;

	// PP: I don't think there's any need to set this true on init// PP: see declaration for descriptive comments
	this->exited=false;

	// PP: initialise preset effects to none
	for(int i=0; i<NUM_PIE_MODES; i++)
	{
		this->presetEffects[i]=NULL;
	}

	// PP: initialise background & border...

	this->background = COLOUR(0,0,0,0);
	this->border = COLOUR(0,0,0,0);
	this->tightBoxX=false;
	this->tightBoxY=false;

	// PP: set no effect chooser - some subclasses may subsequently override this in their inits, but that's cool
	this->effectChooser=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this item
	Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::chooseEffect(const PIEMode mode, const bool recur)
{
	// PP: NEW: use the mode of the effect to lock & unlock the item's controllability
	if(mode == PIEMODE_ON)
	{
		this->pageItemFlags &= (~PIFLAG_IGNOREINPUT);
	}
	else
	{
		this->pageItemFlags |= PIFLAG_IGNOREINPUT;
	}

	// PP: choose an effect for this item
	// PP: (See macro definition in PageItemFX.h)
	PAGEITEM_CHOOSE_EFFECT(item);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this item,
					with no change of mode
	Parameters 	: 
	Returns 	: 
	Info 		: this will be called eg. when an item's selection status gets changed
*/
void PAGEITEM::chooseEffect(void)
{
	if(this->effect == NULL)
	{
		// PP: no current effect mode; PIEMODE_ON is the only sensible default
		this->chooseEffect(PIEMODE_ON, true);// PP: true: recur the call down the contents hierarchy
	}
	else
	{
		this->chooseEffect(this->effect->mode, true);// PP: true: recur the call down the contents hierarchy
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::startEffect
	Purpose 	: start an effect on the item
	Parameters 	: ptr to PAGEITEMEFFECT to start
	Returns 	: 
	Info 		: not to be confused with public 'setEffect' which is used to set the item's preset effects
*/
void PAGEITEM::startEffect(PAGEITEMEFFECT* const effectIn)
{
	/*if(this->effect != NULL)
	{
		bkPrintf("eek\n");
	}*/

	// PP: clear away old effect first
	SAFE_DELETE(this->effect);

	this->effect=effectIn;
	
	if(this->effect != NULL)
	{
		// PP: NEW
		this->effect->start();
	}

	// PP: we're either starting an effect or setting no effect; either way the item ain't exited
	this->exited=false;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setEffect
	Purpose 	: specify an effect for one of the item's effect modes (eg. PIEMODE_ENTER, PIEMODE_EXIT, etc.)
	Parameters 	: ptr to PAGEITEMEFFECT to use (or NULL not to override PIECHOOSER), (opt/PIEMODE_ALL)effect mode in which to use the effect
	Returns 	: this
	Info 		: The effect you specify will override the PIECHOOSERs, unless you're setting NULL.
					You can set the same effect pointer onto as many of the item's different effect modes as you want;
						you might wanna make sure the effect supports all the modes you're setting it to though.
					// PP: NOTE: *** PAGEITEMS NO LONGER DELETE THE EFFECTS THAT YOU SET HERE!!! ***
*/
struct TAG_PAGEITEM* PAGEITEM::setEffect(PAGEITEMEFFECT* const effectIn, const PIEMode mode)
{
	// PP: NEW
	effectIn->setItem(this);

	if(mode == PIEMODE_ALL)
	{
		// PP: apply effect to all effect modes supported by the effect type
		for(int i=0; i<NUM_PIE_MODES; i++)
		{
			if(effectIn->supportsMode(PIEMode(i)))
			{
				// PP: effectIn->init2(this, (PIEMode)i);// PP: REMOUT: nonsense!
				this->presetEffects[i]=effectIn;
			}
			else
			{
				int x=0;
			}
		}
	}
	else
	{
		if(effectIn != NULL)
		{
			// PP: apply effect to single specified effect mode
			// PP: effectIn->init2(this, mode);// PP: REMOUT: no need to re-set item

			if(!effectIn->supportsMode(mode))
			{
#ifdef TAZDEBUG
				ASSERTM(0, "PP: this page item effect type does not support the specified PIE mode\n");
#endif// PP: debug

				return this;
			}

			// PP: NEW
			effectIn->setMode(mode);
		}
		
		this->presetEffects[mode]=effectIn;
	}

	return this;
}

PIEMode PAGEITEM::getEffectMode()
{
	if (this->effect)
		return this->effect->mode;
	return ((PIEMode) -1);
}

/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::update
	Purpose 	: update the page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::update(void)
{
	// PP: update the effect parasite if there is one...

	if(this->effect != NULL)
	{
		this->effect->update();

		if(this->effect->finished)
		{
			if(this->effect->mode == PIEMODE_EXIT)
			{
				// PP: progress from PIEMODE_EXIT to exitted

				// PP: item is only exited if it or any of its contents have effects and if so once it and all of its contents are finished exiting!
				// PP: REMOUT: we're already checking this->effect!=NULL	if(this->hasEffects())// PP: NEW
				{
					if(this->effectsFinished())
					{
						this->exited=true;
					}
				}
			}
			else if(this->effect->mode == PIEMODE_ENTER)
			{
				// PP: item is only entered if it or any of its contents have effects and if so once it and all of its contents are finished entering!
				// PP: REMOUT: we're already checking this->effect!=NULL	if(this->hasEffects())// PP: NEW
				{
					if(this->effectsFinished())
					{
						// PP: progress from PIEMODE_ENTER to PIEMODE_ON
						this->chooseEffect(PIEMODE_ON, true);// PP: was false
					}
				}
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::draw
	Purpose 	: draw the page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::draw(void)
{
	// PP: draw the item through its effect parasite if it has one...

	if(this->effect == NULL)
	{
		this->drawWithoutEffect();
	}
	else
	{
		this->effect->draw();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::drawBackgroundAndBorder
	Purpose 	: draw the page item's background and border if it has them
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::drawBackgroundAndBorder(void)
{
	// PP: NOTE: RECT::draw was designed for convenience and versatility, not for speed!
	// PP: You might wanna change this over to be faster if you use backgrounds and borders a lot

	if(this->background.a || this->border.a)
	{
		// TP: Draw background if needed
		bdSetTexture(0, NULL);
		if (this->background.a)
		{
			COLOUR	tempColour = this->background;
			tempColour.a = (uchar)(float(this->background.a * this->opacity));
			this->boxRect.draw(tempColour, SOLIDFILL);
		}

		if (this->border.a)
		{
			COLOUR	tempColour = this->border;
			tempColour.a = (uchar)(float(this->border.a * this->opacity));
			this->boxRect.draw(tempColour, OUTLINE);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setXAlign
	Purpose 	: set the X-axis alignment of the item
	Parameters 	: new X alignment mode (PIFLAG_XALIGN_)
	Returns 	: this
	Info 		: 
*/
PAGEITEM* PAGEITEM::setXAlign(const PageItemFlag align)
{
	// PP: clear x alignment flag
	this->pageItemFlags &= ~(PAGEITEM_XALIGN_FLAGS);

	// PP: set new x alignment flag
	this->pageItemFlags |= align;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setYAlign
	Purpose 	: set the Y-axis alignment of the item
	Parameters 	: new Y alignment mode (PIFLAG_YALIGN_)
	Returns 	: this
	Info 		: 
*/
PAGEITEM* PAGEITEM::setYAlign(const PageItemFlag align)
{
	// PP: clear y alignment flag
	this->pageItemFlags &= ~(PAGEITEM_YALIGN_FLAGS);

	// PP: set new y alignment flag
	this->pageItemFlags |= align;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setRect
	Purpose 	: set the item's rectangle
	Parameters 	: new rectangle
	Returns 	: this
	Info 		: this sorts out optRect as well as rect - all PAGEITEM::setRect overrides should call this first
*/
PAGEITEM* PAGEITEM::setRect(const RECTANGLE& rectIn)
{
	this->rect=rectIn;

	// PP: ******
	// PP: moved stuff from here to align
	// PP: ******

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setFreeRect
	Purpose 	: set a free item's rectangle within its container (eg. UORECT makes it completely fill its container)
	Parameters 	: new free-rectangle for the free item
	Returns 	: this
	Info 		: // PP: realign (eg. open) one of the item's containers for the change to take effect
*/
PAGEITEM* PAGEITEM::setFreeRect(const RECTANGLE freeRectIn)
{
	this->freeRect=freeRectIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::~TAG_PAGEITEM
	Purpose 	: destructor; calls shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
PAGEITEM::~TAG_PAGEITEM(void)
{
	this->shutdown();
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::drawscreenRects
	Purpose 	: draw the screen rects of the item and any items it contains
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::drawScreenRects(void)
{
	RECTANGLE	tempRect;

	tempRect=this->screenRect/*.removeMargin(PAGEITEM_MARGIN)*/;

	tempRect.draw(GREEN, OUTLINE);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::select
	Purpose 	: select/deselect the page item
	Parameters 	: (opt/true)true to select, false to deselect
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::select(const bool state)
{
	// PP: set the selected flag, obviously
	this->selected=state;

	// PP: Here's a chance for the item's effect to be changed to show that it's selected
	// PP:		From the item's current effect, the chooser will know what effect mode we're in (ENTER/ ON/ EXIT)
	// PP:		The new effect could also use info about the current effect to improve the changeover.

	if((this->effect == NULL) || (this->effect->mode == PIEMODE_ON))
	{
		this->chooseEffect(PIEMODE_ON, true);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::highlight
	Purpose 	: highlight/unhighlight the page item
	Parameters 	: (opt/true)true to highlight, false to unhighlight
	Returns 	: 
	Info 		: // PP: IS HIGHLIGHTING ACTUALLY USED FOR ANYTHING CONSTRUCTIVE NOWADAYS????
*/
void PAGEITEM::highlight(const bool state)
{
	// PP: quite simply
	this->highlighted=state;

	// PP: Here's a chance for the item's effect to be changed to show that it's highlighted
	// PP:		From the item's current effect, the chooser will know what effect mode we're in (ENTER/ ON/ EXIT)
	// PP:		The new effect could also use info about the current effect to improve the changeover.
	
	if((this->effect == NULL) || (this->effect->mode == PIEMODE_ON))
	{
		this->chooseEffect(PIEMODE_ON, true);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::shutdown
	Purpose 	: shut down the item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEITEM::shutdown(void)
{
	// PP: if Babel has shut down, we don't want to be trying to free anything - it's already gone
	if(BabelHasShutdown)
	{
		return;
	}

	this->name[0]='\0';

	// PP: delete current effect
	SAFE_DELETE(this->effect);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::updateBox
	Purpose 	: update the box to be used to draw the page item's background and/or border
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: to be called by setRect and setBoxStyle
*/
void PAGEITEM::updateBox(void)
{
	RECTANGLE	rect2;
	RECTANGLE	optRect2;

	rect2=this->rect.toOrigin();
	optRect2=this->optRect.toOrigin();

	if(this->tightBoxX)
	{
		this->boxRect.left=optRect2.left;
		this->boxRect.right=optRect2.right;
	}
	else
	{
		this->boxRect.left=rect2.left;
		this->boxRect.right=rect2.right;
	}

	if(this->tightBoxY)
	{
		this->boxRect.bottom=optRect2.bottom;
		this->boxRect.top=optRect2.top;
	}
	else
	{
		this->boxRect.bottom=rect2.bottom;
		this->boxRect.top=rect2.top;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::markForRealignment
	Purpose 	: tell the pageitem that its alignment is out of date and will need
					to be updated in next align()
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this gets recurred down the contents hierarchy
*/
void PAGEITEM::markForRealignment(void)
{
	this->pageItemFlags &= (~PIFLAG_ALIGNED);

	// PP: NEW optimisation
	this->pageItemFlags &= ~(PIFLAG_WIDTHCALCULATED|PIFLAG_HEIGHTCALCULATED);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setBoxStyle
	Purpose 	: set the background & border fitting mode
	Parameters 	: tight box X?, tight box Y?
	Returns 	: this
	Info 		: // PP: I can imagine backgrounds & borders getting much more
					complicated in another game, but this might be fine for Taz
*/
struct TAG_PAGEITEM* PAGEITEM::setBoxStyle(const bool tightBoxXin, const bool tightBoxYin)
{
	this->tightBoxX=tightBoxXin;
	this->tightBoxY=tightBoxYin;

	// PP: update the box to be used to draw the page item's background and/or border
	this->updateBox();

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::align
	Purpose 	: calculate alignment for the page item and it's contents
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this gets recurred down the contents hierarchy
*/
void PAGEITEM::align(void)
{
	// PP: X ALIGNMENT...

	if(this->pageItemFlags & PIFLAG_XALIGN_LEFT)
	{
		this->optRect.left=this->rect.left;
		this->optRect.right=this->optRect.left+this->getRequiredWidth();
	}
	else if(this->pageItemFlags & PIFLAG_XALIGN_RIGHT)
	{
		this->optRect.right=this->rect.right;
		this->optRect.left=this->optRect.right-this->getRequiredWidth();
	}
	else// PP: assume centre justification if none is specified
	{
		float	requiredWidth=this->getRequiredWidth();

		this->optRect.left=this->rect.midX()-(requiredWidth/2.0f);
		this->optRect.right=this->optRect.left+requiredWidth;
	}

	// PP: Y ALIGNMENT...

	if(this->pageItemFlags & PIFLAG_YALIGN_BOTTOM)
	{
		this->optRect.bottom=this->rect.bottom;
		this->optRect.top=this->optRect.bottom+this->getRequiredHeight();
	}
	else if(this->pageItemFlags & PIFLAG_YALIGN_TOP)
	{
		this->optRect.top=this->rect.top;
		this->optRect.bottom=this->optRect.top-this->getRequiredHeight();
	}
	else// PP: assume centre justification if none is specified
	{
		float	requiredHeight=this->getRequiredHeight();

		this->optRect.bottom=this->rect.midY()-(requiredHeight/2.0f);
		this->optRect.top=this->optRect.bottom+requiredHeight;
	}

	// PP: update the box to be used to draw the page item's background and/or border
	this->updateBox();

// PP: =====================================================================
// PP: REMOUT: REALLY GOTTER PUT THIS BACK IN AND GET IT WORKING RIGHT!!!!!!
// PP: =====================================================================
//	this->pageItemFlags |= PIFLAG_ALIGNED;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													PAGEROW methods

													A row holds items.
					By inserting a page into a row as an item, you can also create a column containing more rows.
					By inserting a book into a row as an item, you can create changeable panes within the row.
											
											There's no limit to the fun!™

											book > pages > *ROWS* > items
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::setColumnWidths
	Purpose 	: set the widths of the columns in the row; reposition items accordingly
	Parameters 	: linked list of column widths, arranged from left to right
	Returns 	: 
	Info 		: 
*/
void PAGEROW::setColumnWidths(LINKEDLIST<float> &columnWidths)
{
	float		leftPos;
	float		rightPos;
	PAGEITEM	*item;

	leftPos=this->rect.left;
	
	for(int c=0; c<this->getNumItems(); ++c)
	{
		rightPos=leftPos+columnWidths[c];

		item=this->getItem(c);

		item->setRect(RECTANGLE(leftPos, rightPos, this->rect.bottom, this->rect.top));

		// PP: tell the item to align its contents if it has any
		// PP: NOTE: the only time PAGEROW::setColumnWidths will be called is during alignment, so this is ok
		item->align();

		leftPos=rightPos;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::init
	Purpose 	: initialise the row
	Parameters 	: (opt/0) index of the row within its page
	Returns 	: 
	Info 		: 
*/
void PAGEROW::init(const uint32 indexIn)
{
	this->index=indexIn;

	strcpy(this->name, "row");
	this->opacity=1.0f;

	// PP: no effects running yet
	this->flags=ROWFLAG_ALLEFFECTSFINISHED;

	this->items.init("%s items", this->name);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::TAG_PAGEROW
	Purpose 	: default constructor; calls init
	Parameters 	: 
	Returns 	: new PAGEROW instance
	Info 		: 
*/
PAGEROW::TAG_PAGEROW(const uint32 indexIn)
{
	this->init(indexIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::wantsYPadding
	Purpose 	: determine whether or not the row wants Y padding
	Parameters 	: 
	Returns 	: whether or not the row wants Y padding
	Info 		: 
*/
bool PAGEROW::wantsYPadding(void)
{
	PAGEITEM*		item;

	for(int i=this->getNumItems()-1; i>=0; i--)
	{
		item=this->getItem(i);

		if(!item->getYPadding())
		{
			return false;
		}
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::setRect
	Purpose 	: set the rectangle for the row and update its contents' rectangles accordingly
	Parameters 	: rectangle
	Returns 	: this
	Info 		: this is very similar to PAGE::setRect, only it works in a different axis
*/
PAGEROW* PAGEROW::setRect(const RECTANGLE& rectIn)
{
	PAGEITEM*		item;
	int				numItems;

	int				numXPaddedItems=0;
	float			xPad;
	float			requiredWidth=0.0f;
	float			totalRequiredWidth=0.0f;
	float			totalRequiredWidthNotXPadded=0.0f;

	float			leftPos;
	float			rightPos;
	float			itemWidth;

	int				i;							// PP: loop counter

	this->rect=rectIn;

	// PP: so now we know the total width & height for this row - update the row's items accordingly...

	numItems=this->getNumItems();

	if(this->flags & ROWFLAG_UNIFORM_X_SPACING)
	{
		float	widthRation;

		widthRation=this->rect.widthAbs()/numItems;

		leftPos=this->rect.left;

		// PP: all items get the same amount of width
		for(i=0; i<numItems; i++)
		{
			item=this->getItem(i);

			rightPos=leftPos+widthRation;

			item->setRect(RECTANGLE(leftPos, rightPos, this->rect.bottom, this->rect.top));

			// PP: tell the item to align its contents if it has any
			// PP: NOTE: the only time PAGEROW::setRect will be called is during alignment, so this should be ok
			item->align();

			leftPos=rightPos;
		}
	}
	else
	{
		// PP: work out how much width is required and how much slack this gives us...
		for(i=0; i<numItems; i++)
		{
			item=this->getItem(i);

			// PP: before getting the required width of the item, tell it how much height it has to play with.  This only really affects things that wrap, eg textboxes
			item->setRect(this->rect);

			requiredWidth=item->getRequiredWidth();

			if(item->getXPadding())
			{
				++numXPaddedItems;
			}
			else
			{
				totalRequiredWidthNotXPadded+=requiredWidth;
			}

			totalRequiredWidth+=requiredWidth;
		}

		if(numXPaddedItems > 0)
		{
			xPad=(this->rect.width()-totalRequiredWidth)/(float)numXPaddedItems;
		}

		// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~
		// PP: NB: for the moment I assume that there's always going to be enough space for the items if they just share and play nicely
		// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~

		leftPos=this->rect.left;

		for(i=0; i<numItems; i++)
		{
			item=this->getItem(i);

			requiredWidth=item->getRequiredWidth();

			if(item->getXPadding())
			{
				itemWidth=requiredWidth+xPad;
			}
			else
			{
				itemWidth=requiredWidth;
			}

			rightPos=leftPos+itemWidth;

			item->setRect(RECTANGLE(leftPos, rightPos, this->rect.bottom, this->rect.top));

			// PP: tell the item to align its contents if it has any
			// PP: NOTE: the only time PAGEROW::setRect will be called is during alignment, so this should be ok
			item->align();

			leftPos=rightPos;
		}
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::getFirstSelectableItem
	Purpose 	: get the first selectable item in the row, whether or not to recur the search into sub-items
	Parameters 	: 
	Returns 	: ptr to first selectable item if there are any, NULL otherwise
	Info 		: 
*/
PAGEITEM* PAGEROW::getFirstSelectableItem(const bool recur)
{
	PAGEITEM*	item;

	for(int i=0; i<this->getNumItems(); i++)
	{
		item=this->getItem(i)->getFirstSelectableItem(recur);

		if(item != NULL)
		{
			return item;
		}
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::insertItem
	Purpose 	: insert an item onto the end of the row
	Parameters 	: item to insert
	Returns 	: 
	Info 		: 
*/
void PAGEROW::insertItem(PAGEITEM* const item)
{
	item->columnIndex=this->getNumItems();

	this->items.append(item);

	item->rowIndex=this->index;

	// PP: let the item know that it's being inserted
	item->onInsert();
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::drawDebug
	Purpose 	: draw debug info for the row and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

#define ROW_MARGIN				(MARGIN*2)

void PAGEROW::drawDebug(void)
{
	RECTANGLE					tempRect;
	COLOUR						colour=COLOUR(255, 160, 160);
	TBFontFormatting			format;							// PP: formatting info
	static ushort				string16[128];

	tempRect.init(this->rect.left+ROW_MARGIN,
					this->rect.right-ROW_MARGIN,
					this->rect.bottom+ROW_MARGIN,
					this->rect.top-ROW_MARGIN);

//	tempRect.draw(colour, OUTLINE);

	// PP: display current page number

	TextFX_setColour(colour);

	format.x =			this->rect.left;
	format.y =			this->rect.top;
	format.height =		this->rect.height();
	format.width =		this->rect.width();
	format.horzFormat = EBFONTFORMATTING_CENTRE;
	format.vertFormat = EBFONTFORMATTING_CENTRE;
	format.wrap=0;
	format.lineSpace = 0;

	// PP: convert the final text into wide characters
	bkString8to16(string16, this->name);

	// PP: print the text
//	bdPrintFont(standardFont,string16,strlen(this->name),&format,0,0,0,200,&TextFX_escapeCallback,NULL);

	TextFX_resetColour();

	// PP: TODO: draw debug data for all the page's items

	PAGEITEM*		item;

	for(int i=this->getNumItems()-1; i>=0; i--)
	{
		item=this->getItem(i);

		item->drawDebug();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::draw
	Purpose 	: draw the row and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEROW::draw(void)
{
	PAGEITEM*		item;

	for(int i=this->getNumItems()-1; i>=0; i--)
	{
		item=this->getItem(i);

		if(!item->exited)
		{
			item->draw();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::update
	Purpose 	: update the row and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEROW::update(void)
{
	PAGEITEM*	item;

	// PP: assume all item effects have finished, till we find one that isn't
	this->flags |= ROWFLAG_ALLEFFECTSFINISHED;

	for(int i=this->getNumItems()-1;i>=0;i--)
	{
		item=this->getItem(i);
		item->update();

		// PP: NEW
		if(item->hasEffects())
		{
			this->flags |= ROWFLAG_HASEFFECTS;
		}

		if(!item->effectsFinished())// PP: item->effect && !item->effect->finished
		{
			// PP: found an effect that isn't finished
			this->flags &= (~ROWFLAG_ALLEFFECTSFINISHED);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::getRequiredWidth
	Purpose 	: get the minimum required width of the row
	Parameters 	: 
	Returns 	: minimum required width of the row
	Info 		: 
*/
float PAGEROW::getRequiredWidth(void)
{
	float	maxRequiredWidth=0.0f;

	for(int i=this->getNumItems()-1;i>=0;i--)
	{
		maxRequiredWidth+=this->getItem(i)->getRequiredWidth();
	}

	return maxRequiredWidth;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::getRequiredHeight
	Purpose 	: get the minimum required height of the row
	Parameters 	: 
	Returns 	: minimum required height of the row
	Info 		: 
*/
float PAGEROW::getRequiredHeight(void)
{
	float	maxRequiredHeight=0.0f;

	for(int i=this->getNumItems()-1;i>=0;i--)
	{
		maxRequiredHeight=MAX(maxRequiredHeight, this->getItem(i)->getRequiredHeight());
	}

	return maxRequiredHeight;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::select
	Purpose 	: select/deselect the row and its contents
	Parameters 	: (opt/true)true to select, false to deselect
	Returns 	: 
	Info 		: 
*/
void PAGEROW::select(const bool state)
{
	// PP: row itself has no 'selected' property (should it have?) - just select/deselect all its contents

	PAGEITEM*		item;

	for(int p=this->getNumItems()-1; p>=0; p--)
	{
		item=this->getItem(p);

		item->select(state);				// PP: this allows for future time-based selection/deselection effects
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::shutdown
	Purpose 	: free up space used by the row and its items
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGEROW::shutdown(void)
{
	PAGEITEM*	item;

	for(int i=this->getNumItems()-1; i>=0; i--)
	{
		item=this->getItem(i);

		if(item->onRemove())// PP: if it's safe to delete this item
		{
			// PP: go for it
			delete item;
		}
	}

	this->items.shutdown();
	this->name[0]='\0';
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEROW::~TAG_PAGEROW
	Purpose 	: destructor; calls shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
PAGEROW::~TAG_PAGEROW(void)
{
	this->shutdown();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGE methods

													A page holds rows.
										Pages can also themselves be held in rows.

												There's no limit to the fun!™

												book > *PAGES* > rows > items

															@page
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


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
struct TAG_PAGEITEM* PAGE::setEffectChooser(const struct PIECHOOSER* const chooser)
{
	this->effectChooser=chooser;

	// PP: get all the page's contents that don't have choosers
	// PP:		of their own to point back to this chooser...

	PAGEROW*	row;
	PAGEITEM*	item;

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		row=this->getRow(r);

		for(int i=row->getNumItems()-1; i>=0; i--)
		{
			item=row->getItem(i);

			if(item->effectChooser == NULL)
			{
				// PP: the item doesn't have a chooser of its own

				item->setEffectChooser(this->effectChooser);
			}
		}
	}

	// PP: now same for free items...

	for(int f=this->getNumFreeItems()-1; f>=0; f--)
	{
		item=this->getFreeItem(f);

		if(item->effectChooser == NULL)
		{
			// PP: the item doesn't have a chooser of its own

			item->setEffectChooser(this->effectChooser);
		}
	}

	return (struct TAG_PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::onInsert
	Purpose 	: notifies the page that it's being inserted into another page
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::onInsert
*/
void PAGE::onInsert(void)
{
	PAGEITEM::onInsert();

	// PP: if the page is being inserted into another page, it ain't a top-level page
	this->flags &= (~PAGEFLAG_TOPLEVELPAGE);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setMenuActionFunc
	Purpose 	: set the function for this page to use to handle menu actions, such as selecting or clicking an item
	Parameters 	: ptr to the function for this page to use to handle menu actions (such as selecting or clicking an item),
					(opt/NULL) context ptr to pass through to the menu action callback
	Returns 	: this
	Info 		: 
*/
TAG_PAGE* PAGE::setMenuActionFunc(MenuActionFunc const func, void* const context)
{
	this->menuActionFunc=func;
	this->menuActionContext=context;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setNavigationScope
	Purpose 	: set the navigation scope for the page
	Parameters 	: ptr to page that is to be the navigation scope for this page
	Returns 	: this
	Info 		: 
*/
struct TAG_PAGE* PAGE::setNavigationScope(struct TAG_PAGE* const scopeIn)
{
	// PP: TEMP TEST
	if(scopeIn != this->navigationScope)
	{
		this->navigationScope->selectNone();
		//this->selectNone();
	}

	this->navigationScope=scopeIn;

	// PP: NEW TEST - select the first selectable item on the new navigation scope
	if(this->navigationScope != this)
	{
		this->selectItem(this->navigationScope/*->getFirstSelectableItem()*/);
	}

	// PP: NEWER TEST
	//this->selectItem(this->navigationScope);

	if(this->navigationScope->selectedItem == NULL)
	{
		this->navigationScope->selectItem(this->navigationScope->getFirstSelectableItem(true));
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setNavigationScope
	Purpose 	: set the navigation scope for the page
	Parameters 	: name of sub-page that is to be the navigation scope for this page
	Returns 	: this
	Info 		: 
*/
struct TAG_PAGE* PAGE::setNavigationScope(const char* const scopeName)
{
	struct TAG_PAGE* subPage;

	subPage=(PAGE*)this->getItem(scopeName);

	if(subPage != NULL)
	{
		this->navigationScope=subPage;
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::resetNavigationScope
	Purpose 	: reset the navigation scope for the page back to itself
	Parameters 	: 
	Returns 	: this
	Info 		: 
*/
struct TAG_PAGE* PAGE::resetNavigationScope(void)
{
	return this->setNavigationScope(this);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setHSlack
	Purpose 	: specify whether a page allows horizontal slack between/around its contents,
					or whether it instead fits to their minimum total required width
	Parameters 	: true=slack / false=no slack
	Returns 	: this
	Info 		: this method, and PAGE::setVSlack, jointly replace PAGE::setVacuumPack
*/
PAGE* PAGE::setHSlack(const bool slack)
{
	// PP: this method simply sets the HSLACK page flag
	SETFLAG(this->flags, PAGEFLAG_HSLACK, slack);

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setVSlack
	Purpose 	: specify whether a page allows vertical slack between/around its contents,
					or whether it instead fits to their minimum total required height
	Parameters 	: true=slack / false=no slack
	Returns 	: this
	Info 		: this method, and PAGE::setHSlack, jointly replace PAGE::setVacuumPack
*/
PAGE* PAGE::setVSlack(const bool slack)
{
	// PP: this method simply sets the VSLACK page flag
	SETFLAG(this->flags, PAGEFLAG_VSLACK, slack);

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setSlack
	Purpose 	: specify whether a page allows slack between/around its contents,
					or whether it instead fits to their minimum total required width & height
	Parameters 	: true=slack / false=no slack
	Returns 	: this
	Info 		: this replaces PAGE::setVacuumPack (inversely like)
*/
PAGE* PAGE::setSlack(const bool slack)
{
	// PP: this method simply sets the VSLACK and HSLACK page flags
	SETFLAG(this->flags, PAGEFLAG_HSLACK, slack);
	SETFLAG(this->flags, PAGEFLAG_VSLACK, slack);

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::init
	Purpose 	: initialise the page
	Parameters 	: (opt/NULL)name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function, (opt/NULL)has-closed function
	Returns 	: 
	Info 		: 
*/

#define PAGE_DEFAULT_PIECHOOSER			(&mainPIEChooser)// PP: (&basePIEChooser)

void PAGE::init(const char* const nameIn, PageFunc const updateFuncIn, PageFunc const openFuncIn, PageFunc const closeFuncIn, PageFunc const hasOpenedFuncIn, PageFunc const hasClosedFuncIn)
{
	PAGEITEM::init();

	// PP: set default page-specific flags
	this->flags=PAGE_DEFAULT_FLAGS;

	this->acceptInput=false;			// PP: DON'T FORGET ABOUT THIS DEFAULT!!!

	// PP: no selected item
	this->selectedItem=NULL;

	// PP: navigation scope is this page itself
	this->navigationScope=this;

	// PP: initialise callbacks - kinda important
	this->updateFunc=updateFuncIn;
	this->openFunc=openFuncIn;
	this->closeFunc=closeFuncIn;
	this->hasOpenedFunc=hasOpenedFuncIn;
	this->hasClosedFunc=hasClosedFuncIn;
	this->menuActionFunc=NULL;
	this->menuActionContext=NULL;

	// PP: initialise callback contexts
	this->updateFuncContext=NULL;
	this->openFuncContext=NULL;
	this->closeFuncContext=NULL;
	this->hasOpenedFuncContext=NULL;
	this->hasClosedFuncContext=NULL;

	// PP: TEST - set up pageitem properties
	this->setXAlign(PIFLAG_XALIGN_CENTRE);
	this->setYAlign(PIFLAG_YALIGN_CENTRE);
	this->setXPadding(true);
	this->setYPadding(true);

	this->menuSounds = NULL;
	this->controller = 0;		// PP: pages are only navigable by player 1 by default

	// PP: don't ask, ok?
	this->oldSafeWidthPortion=0.0f;
	this->oldSafeHeightPortion=0.0f;

	this->requiredWidth=0.0f;
	this->requiredHeight=0.0f;

	// PP: no margins by default
	this->setMargin(0.0f);

	if(nameIn != NULL)
	{
		// PP: NOTE: this was previously a strcpy!  nasty overflows were resulting
		this->setName(nameIn);
	}

	this->rows.init("%s rows", this->name);
	this->freeItems.init("%s free items", this->name);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setRect
	Purpose 	: set the Rectangle for the page (and update its contents accordingly)
	Parameters 	: 
	Returns 	: (PAGEITEM*)this
	Info 		: 
*/
PAGEITEM* PAGE::setRect(void)
{
	return this->setRect(this->rect);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setLanguage
	Purpose 	: set the language of the page
	Parameters 	: new language of the page (BLANGUAGEID_...)
	Returns 	: 
	Info 		: this overrides PAGEITEM::setLanguage, to recur the language change through the page's contents
*/
void PAGE::setLanguage(const EBLanguageID languageIn)
{
	int			i;		// PP: loop coonturrr
	PAGEROW*	row;	// PP: row ptr

	// PP: apply the language to all the aligned items...

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		row=this->getRow(r);

		for(i=row->getNumItems()-1; i>=0; i--)
		{
			row->getItem(i)->setLanguage(languageIn);
		}
	}

	// PP: apply the language to all the free items...

	for(i=this->getNumFreeItems()-1; i>=0; i--)
	{
		this->getFreeItem(i)->setLanguage(languageIn);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::markForRealignment
	Purpose 	: tell the pageitem that its alignment is out of date and will need
						to be updated in next align()
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGITEM::markForRealignment
					// PP: this gets recurred down the contents hierarchy
*/
void PAGE::markForRealignment(void)
{
	PAGEITEM::markForRealignment();

	// PP: recur the call down the contents hierarchy (there might be sub-books or sub-pages that need to be told)...

	PAGEROW*	row;

	for(int r=this->getNumRows()-1; r>=0 ;r--)
	{
		row=this->getRow(r);

		for(int i=row->getNumItems()-1; i>=0; i--)
		{
			row->getItem(i)->markForRealignment();
		}
	}

	// PP: and now the free items...

	for(int f=this->getNumFreeItems()-1; f>=0; f--)
	{
		this->getFreeItem(f)->markForRealignment();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::align
	Purpose 	: calculate alignment for the page's contents
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::align
					// PP: this gets recurred down the contents hierarchy
*/
void PAGE::align(void)
{
	PAGEROW*	row;
	int			numRows=this->getNumRows();

	int			numYPaddedRows=0;
	float		requiredHeight=0.0f;
	float		totalRequiredHeight=0.0f;
	float		totalRequiredHeightNotYPadded=0.0f;

	float		bottomPos;
	float		topPos;
	float		rowHeight;

	float		yPad;
	RECTANGLE	rect2;
	RECTANGLE	optRect2;

	int			r;									// PP: loop counter

	if((~this->pageItemFlags) & PIFLAG_ALIGNED)
	{
		// PP: base class alignment - calculates smallest optrect
		PAGEITEM::align();

		// PP: ++++++++++ BROUGHT IN FROM PAGE::SETRECT

		// PP: ADJUST optRect TO TAKE INTO ACCOUNT SLACK SETTINGS...

		if(this->flags & PAGEFLAG_HSLACK)
		{
			this->optRect.left=this->rect.left;
			this->optRect.right=this->rect.right;
		}

		if(this->flags & PAGEFLAG_VSLACK)
		{
			this->optRect.bottom=this->rect.bottom;
			this->optRect.top=this->rect.top;
		}

		// PP: +++++++++

		// PP: ~~~~~~~

		rect2=this->rect.toOrigin().removeMargin(this->margin);
		optRect2=this->optRect.toOrigin().removeMargin(this->margin);

		// PP: no slack allowed if vacuum-packing is on
		if(this->flags & PAGEFLAG_VSLACK)// PP: if((~this->flags) & PAGEFLAG_VACUUMPACK)
		{
			// PP: FIND OUT HOW MUCH HEIGHT SLACK THERE IS

			for(r=0; r<numRows; r++)
			{
				row=this->getRow(r);

				// PP: set the row dimensions to a maximum so it knows how much width it has to play with - this only really affects wrappy things like textboxes
				row->setRect(rect2);

				requiredHeight=row->getRequiredHeight();

				if(row->wantsYPadding())
				{
					++numYPaddedRows;
				}
				else
				{
					totalRequiredHeightNotYPadded+=requiredHeight;
				}

				totalRequiredHeight+=requiredHeight;
			}

			if(numYPaddedRows > 0)
			{
				yPad=(rect2.height()-totalRequiredHeight)/(float)numYPaddedRows;
			}
		}

		// PP: ~~~~~~~~~~~~

		if((~this->flags) & PAGEFLAG_VSLACK)// PP: if(this->flags & PAGEFLAG_VACUUMPACK)
		{
			// PP: all page contents are fitted inside the vertical boundaries of the page's optRect if vertical slack is turned off
			bottomPos=optRect2.bottom;
		}
		else
		{
			bottomPos=rect2.bottom;
		}

		float	reqHeight;

		for(r=numRows-1; r>=0; r--)
		{
			row=this->getRow(r);

			// PP: no slack allowed if vacuum-packing is on
			if((this->flags & PAGEFLAG_VSLACK) && row->wantsYPadding())// PP: if(((~this->flags) & PAGEFLAG_VACUUMPACK) && row->wantsYPadding())
			{
				reqHeight=row->getRequiredHeight();

				rowHeight=reqHeight+yPad;
			}
			else
			{
				// PP: no vslack - must set row dimensions to max before asking for required dimensions,

				// PP: set the row dimensions to a maximum so it knows how much width it has to play with - this only really affects wrappy things like textboxes
				row->setRect(rect2);

				reqHeight=rowHeight=row->getRequiredHeight();
			}

			topPos=bottomPos+rowHeight;

			row->setRect(RECTANGLE(optRect2.left, optRect2.right, bottomPos, topPos));

			bottomPos=topPos;
		}

		// PP: adjust the rectangles of free items...

		PAGEITEM*	item;

		for(int i=this->getNumFreeItems()-1;i>=0;i--)
		{
			item=this->getFreeItem(i);
			
			// PP: tell the free item what its new rect is
			item->setRect(rect2*(item->freeRect.move(0.5f, 0.5f)));

			// PP: then tell the item to align its contents if it has any
			item->align();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getItem
	Purpose 	: get the item with the specified name
	Parameters 	: name of item to find
	Returns 	: ptr to item if found, NULL otherwise
	Info 		: 
*/
PAGEITEM* PAGE::getItem(const char* const itemName)
{
	PAGEITEM*	item;

	for(int r=this->getNumRows()-1;r>=0;r--)
	{
		item=this->getRow(r)->getItem(itemName);
		if(item != NULL)
		{
			return item;
		}
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getTextbox
	Purpose 	: look within the page for a textbox holding a specified string
	Parameters 	: index of string to search for
	Returns 	: a pointer to the first textbox holding the specified string (or NULL if not found)
	Info 		: 
*/
struct TAG_TEXTBOX* PAGE::getTextbox(const EStringIndex string)
{
	PAGETRAVERSAL	trav(this);
	PAGEITEM*		item;

	while((item=trav.next()) != NULL)
	{
		if(item->getStringIndex() == string)
		{
			return (TEXTBOX*)item;
		}
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getNavigationCandidate
	Purpose 	: get a pointer to the item within this page which is
					the most suitable item to which to navigate, from a specified
					current position at a specified navigation angle
	Parameters 	: (in/out) ref to a PAGENAVCANDIDATE_INFO structure (TextFX.h)
	Returns 	: a pointer to the item within this page which is the
					most suitable item to which to navigate
	Info 		: this overrides PAGEITEM::getNavigationCandidate
*/
TAG_PAGEITEM* PAGE::getNavigationCandidate(NAVCANDIDATE_INFO& info)
{
	float						unsuitability;
	float						minUnsuitability	=FLOATMAX;
	PAGEITEM*					item;
	PAGEITEM*					target				=NULL;
	PAGEITEM*					reserveTarget		=NULL;
	PAGEROW*					row;
	int							i;								// PP: loop counter, y'know?
	NAVCANDIDATE_INFO			subInfo				(info);		// PP: info aboot each navigation candidate

	// PP: NEW TEST
	if(!this->selectable()) return NULL;

	PAGETRAVERSAL	trav(this);

	// PP: get the item with the minimum (PAGENAV_DIST_BIAS*dist[in navigation direction?]*angDiff)

	while((item=trav.next()) != NULL)
	{
		// PP: get the angle to each item

		if(info.recur)
		{
			item=item->getNavigationCandidate(subInfo);
		}

		if(item == NULL) continue;// PP: eg. empty page

		if(item == this->selectedItem)
		{
			continue;
		}

		// PP: and this
		if(item == this->navigationScope)
		{
			continue;
		}

		// PP: NEW: and this
		if(item == info.currentItem)
		{
			continue;
		}

		// NH: If Item is not selectable try the next one
		if((!(item->pageItemFlags & PIFLAG_SELECTABLE))||(item->pageItemFlags & PIFLAG_MOUSEONLY)) continue;

		if(reserveTarget == NULL)
		{
			/* PP: if a candidate was offered that was the result of a navigation wrap within its container,
			even if it is found to be 'unsuitable', keep a hold of it as a reserve target, in case we don't find
			a 'suitable' target.*/
			if(subInfo.wrapOut)
			{
				reserveTarget=item;
			}
		}

		unsuitability=this->getItemsUnsuitabilityAsNavigationTarget(item, info.currentPos, info.navAng);

		if(unsuitability <= 0.0f) continue;// PP: <= was <: new test

		if(unsuitability < minUnsuitability)
		{
			minUnsuitability=unsuitability;
			target=item;
		
		}
	}

	if(!info.wrapIn)// PP: can't wrap if we're already wrapping!  (stack-overflow-a-go-go)
	{
		// PP: UP/DOWN/LEFT/RIGHT NAVIGATION WRAPPING.
		if(target == NULL)
		{
			if(this->flags & PAGEFLAG_TOPLEVELPAGE)
			{
				uchar		dir;
				
				dir=bmFloatToInt(bmFloor((Aabs(info.navAng)+QUARTPI)/HALFPI));
				dir%=4;

				switch(dir)
				{
				case 0:// PP: up
					{
						if((this->flags&(PAGEFLAG_NAVWRAP_Y|PAGEFLAG_NAVWRAP_NEWCOLUMN))
							|| ((info.inputType==PAGENAVINPUT_DEBOUNCE)&&(this->flags&PAGEFLAG_NAVWRAP_YDEBOUNCE)))// PP: normal pages treat PAGEFLAG_NAVWRAP_NEWCOLUMN as PAGEFLAG_NAVWRAP_Y, for now at least
						{
							subInfo.currentPos[X]=info.currentPos[X];
							subInfo.currentPos[Y]=this->screenRect.bottom-(this->screenRect.top-info.currentPos[Y]);
							
							subInfo.wrapIn=true;

							target=this->getNavigationCandidate(subInfo);
						}
					}
					break;

				case 1:// PP: right
					{
						if((this->flags&(PAGEFLAG_NAVWRAP_X|PAGEFLAG_NAVWRAP_NEWLINE))
							|| ((info.inputType==PAGENAVINPUT_DEBOUNCE)&&(this->flags&PAGEFLAG_NAVWRAP_XDEBOUNCE)))// PP: normal pages treat PAGEFLAG_NAVWRAP_NEWCOLUMN as PAGEFLAG_NAVWRAP_Y, for now at least
						{
							subInfo.currentPos[X]=this->screenRect.left-(this->screenRect.right-info.currentPos[X]);
							subInfo.currentPos[Y]=info.currentPos[Y];

							subInfo.wrapIn=true;

							target=this->getNavigationCandidate(subInfo);
						}
					}
					break;

				case 2:// PP: down
					{
						if((this->flags&(PAGEFLAG_NAVWRAP_Y|PAGEFLAG_NAVWRAP_NEWCOLUMN))
							|| ((info.inputType==PAGENAVINPUT_DEBOUNCE)&&(this->flags&PAGEFLAG_NAVWRAP_YDEBOUNCE)))// PP: normal pages treat PAGEFLAG_NAVWRAP_NEWCOLUMN as PAGEFLAG_NAVWRAP_Y, for now at least
						{
							subInfo.currentPos[X]=info.currentPos[X];
							subInfo.currentPos[Y]=this->screenRect.top+(info.currentPos[Y]-this->screenRect.bottom);

							subInfo.wrapIn=true;

							target=this->getNavigationCandidate(subInfo);
						}
					}
					break;

				case 3:// PP: left
					{
						if((this->flags&(PAGEFLAG_NAVWRAP_X|PAGEFLAG_NAVWRAP_NEWLINE))
							|| ((info.inputType==PAGENAVINPUT_DEBOUNCE)&&(this->flags&PAGEFLAG_NAVWRAP_XDEBOUNCE)))// PP: normal pages treat PAGEFLAG_NAVWRAP_NEWCOLUMN as PAGEFLAG_NAVWRAP_Y, for now at least
						{
							subInfo.currentPos[X]=this->screenRect.right+(info.currentPos[X]-this->screenRect.left);
							subInfo.currentPos[Y]=info.currentPos[Y];

							subInfo.wrapIn=true;

							target=this->getNavigationCandidate(subInfo);
						}
					}
					break;

					NODEFAULT;
				}
			}
		}

		// PP: mmm, I *think* this wants to be inside the 'if(!info.wrapIn)', but I'm not sure, hmm...
		if(target == NULL)// PP: still
		{
			// PP: use reserve target (might be null, but that's cool)
			target=reserveTarget;
		}
	}

	// PP: returns NULL rather than 'this' if the page is empty - no point selecting an empty page man
	return target;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getItem
	Purpose 	: get a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
	Parameters 	: x position, y position
	Returns 	: a pointer to the item containing a specified screen position, or NULL if there isn't one at that position
	Info 		: this assumes that ye're drawing the book in 2D mode and with an identity object matrix
					this overrides PAGEITEM::getItem(x, y)
*/
PAGEITEM* PAGE::getItem(const float xPos, const float yPos)
{
	PAGEROW*	row;
	PAGEITEM*	item;
	PAGEITEM*	rtnItem;

	// PP: (I'm assuming this method is being used to pick items with the mouse)
	// PP: Can't pick an unselectable item
	if(!this->selectable())
	{
		return NULL;
	}

	for(int r=this->getNumRows()-1;r>=0;r--)
	{
		row=this->getRow(r);

		// PP: to hell with PAGEROW::getItem(x,y) I don't really think we'd benefit from it

		for(int i=row->getNumItems()-1; i>=0; i--)
		{
			item=row->getItem(i);

			// PP: find out if the item or any of its sub-items are at the position
			rtnItem=item->getItem(xPos, yPos);

			if(rtnItem != NULL)
			{
				return rtnItem;
			}
		}
	}

	// PP: now test the free items...

	for(int i=this->getNumFreeItems()-1; i>=0; i--)
	{
		item=this->getFreeItem(i);

		// PP: find out if the item or any of its sub-items are at the position
		rtnItem=item->getItem(xPos, yPos);

		if(rtnItem != NULL)
		{
			return rtnItem;
		}
	}

	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::insertItem
	Purpose 	: insert an item onto the end of the page
	Parameters 	: 
	Returns 	: ptr to new page item
	Info 		: creates a new floating flexible row for the item by default
*/
PAGEITEM* PAGE::insertItem(PAGEITEM* const item, const int newRow)
{
	PAGEROW*	row;			// PP: the row to insert into

	if(newRow || (this->getNumRows()==0))
	{
		// PP: create a new row for the item
		row=this->addRow();
	}
	else
	{
		row=this->getLastRow();
	}

	row->insertItem(item);

	// PP: if you insert an item into the page, it'll need to realign its contents at some point
	this->markForRealignment();

	// PP: set the item's container, as 'this'
	item->container=this;

	return item;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::insertItem
	Purpose 	: insert an item onto the end of the page
	Parameters 	: index of string to insert, whether or not to insert a new row
	Returns 	: ptr to new text box
	Info 		: creates a new floating flexible row for the item by default
*/
TEXTBOX* PAGE::insertItem(const EStringIndex stringIndex, const int newRow)
{
	return (TEXTBOX*)(this->insertItem(new TEXTBOX(stringIndex), newRow));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::Item
	Purpose 	: insert a 'free' (non-aligning) item into the page
	Parameters 	: ptr to new item
	Returns 	: ptr to new item
	Info 		: 
*/
PAGEITEM* PAGE::insertFreeItem(PAGEITEM* const item)
{
	this->freeItems.append(item);

	// PP: let the item know that it's being inserted	// PP: "Brace yourself, Stout Fellow..."
	item->onInsert();

	// PP: set the item's container, as 'this'
	item->container=this;

	return item;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::insertFreeItem
	Purpose 	: insert a 'free' (non-aligning) item into the page
	Parameters 	: ptr to new item, rectangle for the item (as a fraction of the page that holds it)
	Returns 	: ptr to new item
	Info 		: 
*/
PAGEITEM* PAGE::insertFreeItem(PAGEITEM* const item, const RECTANGLE& rectIn)
{
	this->insertFreeItem(item);

	item->freeRect=rectIn;

	return item;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::highlightNone
	Purpose 	: unhighlight all items on the page
	Parameters 	: 
	Returns 	: (PAGEITEM*)this
	Info 		: 
*/
PAGEITEM* PAGE::highlightNone(void)
{
	PAGEROW*	row;
	PAGEITEM*	item;

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		row=this->getRow(r);

		for(int i=row->getNumItems()-1; i>=0; i--)
		{
			item=row->getItem(i);

			item->highlight(false);
		}
	}

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::selectItem
	Purpose 	: Select a specified item
	Parameters 	: (opt/NULL=none) ptr to item to select
	Returns 	: ptr to item
	Info 		: 
*/
PAGEITEM* PAGE::selectItem(PAGEITEM* const item)
{
	MENUACTION_INFO		info;

//#if BPLATFORM==PC
	// NH: Deselect the selected item if no longer the current item (?!?)
	// NH: Ok, that wasn't too clear, but basically this will un-select (un-highlight) an item when the mouse moves off it (That's better!)
	if(item == NULL)
	{
// PP: REMOUT: not used		PAGEITEM*	tempItem;

		if(this->selectedItem != NULL)
		{
			this->selectedItem->select(false);
			this->selectedItem=NULL;
		}
		return NULL;
	}
/*#else
	// PP: select first item if none is specified
	if(item == NULL)
	{
		PAGEITEM*	tempItem;

		tempItem=this->getFirstItem();

		if(tempItem == NULL)
		{
			return NULL;
		}
		else
		{
			return this->selectItem(tempItem);
		}
	}
#endif*/

	// NH: If exlusive unselect the currently selected and set the new item to selected
	if(this->selectedItem != NULL)
	{
		this->selectedItem->select(false);

		// PP: new
		if(this->menuActionFunc != NULL)
		{
			if(item != NULL)
			{
				info.item=this->selectedItem;
				info.action=MENUACTION_DESELECT;
				info.context=this->menuActionContext;

				this->menuActionFunc(info);
			}
		}
	}

	this->selectedItem=item;

	this->selectedItem->select(true);

	// PP: new
	if(this->menuActionFunc != NULL)
	{
		if(item != NULL)
		{
			info.item=this->selectedItem;
			info.action=MENUACTION_SELECT;
			info.context=this->menuActionContext;

			this->menuActionFunc(info);
		}
	}

	if(this->navigationScope == this)// PP: only play sound on bottom level selection
	{
		// NH: Play Navigate Sound
		if ((this->menuSounds)&&(this->menuSounds->navigateSound.name != NULL))
		{
//			PlaySample(this->menuSounds->navigateSound.name,this->menuSounds->navigateSound.volume);
		}
	}
	else
	{
		// PP: NEW TEST
		this->navigationScope->selectItem(item);
	}

	return this->selectedItem;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::update
	Purpose 	: update the page and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::update
*/
void PAGE::update(void)
{
	PAGEROW*		row;

	// PP: IMPORTANT: PAGEITEM base-class update, including effects update
	PAGEITEM::update();
		
	if(this->updateFunc != NULL)
	{
		this->updateFunc(this, this->updateFuncContext);
	}

	// PP: respond to navigation if applicable
/* PP: REMOUT: now triggered directly from the book
	if((~this->flags) & PAGEFLAG_CLOSING)			// PP: NEW: can't navigate a page that's closing
	{
		if(this->flags & PAGEFLAG_TOPLEVELPAGE)		// PP: top-level pages start the navigation, navigation which will include items within navigable sub-pages
		{
			if(this->flags & PAGEFLAG_NAVIGABLE)
			{
				this->respondToNavigation();
			}
		}
	}
*/
	// PP: assume any transition has finished till we find contrary evidence

	bool	transitionEnd=PAGEITEM::effectsFinished();
	int		r,i;

	for(r=this->getNumRows()-1;r>=0;r--)
	{
		row=this->getRow(r);

		row->update();

		if((~row->flags) & ROWFLAG_ALLEFFECTSFINISHED)
		{
			// PP: ooh look, there's still transition effects in progress
			transitionEnd=false;
		}
	}

	// PP: update non-aligned items and set their opacities...

	PAGEITEM*		item;

	for(i=this->getNumFreeItems()-1; i>=0; i--)
	{
		item=this->getFreeItem(i);

		item->update();

		if(transitionEnd)
		{
			if(!item->effectsFinished())// PP: item->effect && !item->effect->finished
			{
				// PP: ooh look, there's still transition effects in progress
				transitionEnd=false;
			}
		}
	}
	
	if(this->flags & PAGEFLAG_CLOSING)
	{
		if(this->hasEffects() && transitionEnd)
		{
			// PP: page close complete
			this->flags &= ~PAGEFLAG_CLOSING;
			this->flags |= PAGEFLAG_CLOSED;

			this->hasClosed();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::open
	Purpose 	: called when the page is opened or turned to
	Parameters 	: (opt/false)open instantly (bypassing effects)
	Returns 	: 
	Info 		: 
*/
void PAGE::open(const bool instant)
{
	// PP: NEW TEST: align the page's contents as it opens for the first time
	// PP: NOTE: this setRect will do nothing unless the rectangle has changed :)
	// PP: REMOUT: now a seperate align method		this->setRect(this->rect);
	this->align();

	if(this->openFunc != NULL)
	{
		this->openFunc(this, this->openFuncContext);
	}

	// PP: NEW
	this->flags &= ~(PAGEFLAG_CLOSED|PAGEFLAG_CLOSING);

	// PP: bkPrintf("PAGE::open: clearing closed flag on page \"%s\"\n", this->name);

//	if(this->flags & PAGEFLAG_TOPLEVELPAGE)// PP: TEMP?

	if(instant)
	{
		// PP: straight onto the ON-mode effects

		// PP: REMOUT: NO		if(this->effectChooser != NULL)
		{
			this->chooseEffect(PIEMODE_ON, true);
		}
	}
	else
	{
		// PP: choose an effect for 'this' top-level page
		// PP: REMOUT: NO		if(this->effectChooser != NULL)
		{
			this->chooseEffect(PIEMODE_ENTER, true);
		}
	}

	// PP: when a top-level, navigable page opens, suspend controls for things other than menu navigation...

	if(this->flags & PAGEFLAG_TOPLEVELPAGE)
	{
		if(this->flags & PAGEFLAG_NAVIGABLE)
		{
			gameStatus.navigatingMenus=true;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::close
	Purpose 	: called when the page is closed or turned away from
	Parameters 	: (opt/false)open instantly (bypassing effects)
	Returns 	: 
	Info 		: 
*/
void PAGE::close(const bool instant)
{
	if(this->closeFunc != NULL)
	{
		this->closeFunc(this, this->closeFuncContext);
	}

	// PP: NEW TEST
	if(this->flags & PAGEFLAG_CLOSED)
	{
		return;
	}
	

//	if(this->flags & PAGEFLAG_TOPLEVELPAGE)// PP: TEMP?

	if(instant)
	{
		this->flags&=(~PAGEFLAG_CLOSING);
		this->flags|=PAGEFLAG_CLOSED;
	}
	else
	{
		// PP: flag the page as closing; when all its items' exit effects have finished,
		// PP:		it will no longer need to be drawn or updated
		this->flags |= PAGEFLAG_CLOSING;
		
		// PP: choose an effect for this page
		// PP: if instant, this just clears current effect
		// PP: REMOUT: NO		if(this->effectChooser != NULL)
		{
			this->chooseEffect(PIEMODE_EXIT, true);
		}
	}

	// PP: when a top-level, navigable page closes, re-enable controls for things other than menu navigation...

	if(this->flags & PAGEFLAG_TOPLEVELPAGE)
	{
		if(this->flags & PAGEFLAG_NAVIGABLE)
		{
			gameStatus.navigatingMenus=false;// PP: TODO: check that the order is ok when changing from a navigable page to another one, so that this doesn't undo the setting-true done by opening the second one bleh bleh bleh
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::hasOpened
	Purpose 	: called when the page becomes fully opened or turned to
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::hasOpened(void)
{
	if(this->hasOpenedFunc != NULL)
	{
		this->hasOpenedFunc(this, this->hasOpenedFuncContext);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::hasClosed
	Purpose 	: called when the page becomes fully closed or turned away from
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::hasClosed(void)
{
	if(this->hasClosedFunc != NULL)
	{
		this->hasClosedFunc(this, this->hasClosedFuncContext);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::mainDrawDebug
	Purpose 	: the core of the debug-draw method - actually debug-draw the page and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDrawDebug
*/

#define PAGE_MARGIN				(MARGIN*1)

void PAGE::mainDrawDebug(void)
{
	RECTANGLE					tempRect;
//	COLOUR						colour=COLOUR(255, 0, 0);
	TBFontFormatting			format;							// PP: formatting info
	static ushort				string16[128];
	PAGEROW*					row;
/*
	tempRect=this->rect.toOrigin().removeMargin(PAGE_MARGIN);

	tempRect.draw(colour, OUTLINE);

	tempRect=this->optRect.toOrigin
*/

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~
		// PP: DRAW MAXIMUM RECTANGLE

//	RECTANGLE					tempRect;
	COLOUR						colour=COLOUR(0, 255, 0);

	tempRect=this->rect.toOrigin().removeMargin(PAGEITEM_MARGIN);

	// PP: counteract the draw scale that has been applied by PAGEITEM_PREDRAW - we want to see the REAL rect - and not in a 'Real Ghostbusters' way
	if(this->getDrawScale() != 0.0f)
	{
		tempRect/=this->getDrawScale();
	}

	tempRect.draw(colour, OUTLINE);

	// PP: DRAW USED RECTANGLE

	if(fmodf(gameStatus.appTime, 0.4f)<0.2f)
	{
		tempRect=this->optRect.toOrigin().removeMargin(PAGEITEM_MARGIN);

		// PP: counteract the draw scale that has been applied by PAGEITEM_PREDRAW - we want to see the REAL optRect - and not in a 'Real Ghostbusters' way
		if(this->getDrawScale() != 0.0f)
		{
			tempRect/=this->getDrawScale();
		}

		tempRect.draw(colour, OUTLINE);
	}

// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~

	// PP: display current page number

	TextFX_setColour(colour);

	format.x =			this->rect.left;
	format.y =			this->rect.top;
	format.height =		this->rect.height();
	format.width =		this->rect.width();
	format.horzFormat = EBFONTFORMATTING_CENTRE;
	format.vertFormat = EBFONTFORMATTING_TOP;
	format.wrap=0;
	format.lineSpace = 0;

	// PP: convert the final text into wide characters
	bkString8to16(string16, this->name);

	// PP: print the text
//	bdPrintFont(standardFont,string16,strlen(this->name),&format,0,0,0,200,&TextFX_escapeCallback,NULL);

	TextFX_resetColour();

	// PP: draw debug data for all the page's rows
	for(int R=this->getNumRows()-1; R>=0; R--)			// PP: upper-case loop counters make you feel POWERFUL!  GRRRRRR!!!!
	{
		row=this->getRow(R);

		row->drawDebug();
	}

	// PP: draw-debug the page's free items
	for(int i=this->getNumFreeItems()-1;i>=0;i--)
	{
		this->getFreeItem(i)->drawDebug();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::mainDraw
	Purpose 	: the core of the draw method - actually draw the page and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void PAGE::mainDraw(void)
{
	PAGEROW*		row;
	PAGEITEM*		item;

	// PP: draw all the page's rows
	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		row=this->getRow(r);

		row->draw();
	}

	// PP: draw non-aligned items...

	for(int i=this->getNumFreeItems()-1; i>=0; i--)
	{
		item=this->getFreeItem(i);

		if(!item->exited)
		{
			item->draw();
		}
	}
}


// PP: MOVEME

// PP: RECTANGLE equality to RECTANGLE
inline bool operator == (const RECTANGLE& rect1, const RECTANGLE& rect2)
{
	return (	(rect1.left == rect2.left)
			&&	(rect1.right == rect2.right)
			&&	(rect1.bottom == rect2.bottom)
			&&	(rect1.top == rect2.top));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setRect
	Purpose 	: set the Rectangle for the page (and update its contents accordingly)
	Parameters 	: rectangle
	Returns 	: (PAGEITEM*)this
	Info 		: this is just exactly like PAGEROW::setRect but with Y padding taken into account rather than X padding
					this overrides PAGEITEM::setRect
*/
PAGEITEM* PAGE::setRect(const RECTANGLE& rectIn)
{
	if(rectIn == this->rect)
	{
		// PP: this rectangle has already been set, so ignore.  Remember, content alignment is now done seperately, in align()
		return this;
	}

	// PP: NEW TEST: rectangle has changed, so alignment must be recalculated
	this->markForRealignment();

	// PP: 
	PAGEITEM::setRect(rectIn);

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getRequiredWidth
	Purpose 	: get the required width for the page (according to its items)
	Parameters 	: 
	Returns 	: minimum required width of the the page
	Info 		: this overrides PAGEITEM::getRequiredWidth
*/
float PAGE::getRequiredWidth(void)
{
	if(this->pageItemFlags & PIFLAG_WIDTHCALCULATED)
	{
		return this->requiredWidth;
	}

	PAGEROW*	row;
	PAGEITEM*	item;
	int			numRows=this->getNumRows();
	RECTANGLE	rect2;

	// PP: take into account page margin
	rect2=this->rect.removeMargin(this->margin);

	this->requiredWidth=0.0f;

	// PP: go through each row in the page and find out which one needs the most width; return that...

	for(int r=0; r<numRows; r++)
	{
		row=this->getRow(r);

		// PP: THIS MAY WELL NOT BE A GODD IDEA - TEST ME
		// PP: set the row dimensions to a maximum so it knows how much height it has to play with

		row->setRect(rect2);// PP: this fixed an old wrapping bug to do with margins not being properly accounted for

		this->requiredWidth=MAX(this->requiredWidth, row->getRequiredWidth());
	}

	// PP: now the free items - don't forget the free items!
	for(int i=this->getNumFreeItems()-1; i>=0; i--)
	{
		item=this->getFreeItem(i);

		this->requiredWidth=MAX(this->requiredWidth, item->getRequiredWidth());		
	}

	// PP: apply margins before align scale - it just somehow makes sense
	this->requiredWidth += (this->margin*2.0f);

	// PP: Now apply align scale - though I imagine it'll get used less now that you can set absolute margins on pages
	this->requiredWidth *= this->getAlignScale();

	this->pageItemFlags |= PIFLAG_WIDTHCALCULATED;

	return this->requiredWidth;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getRequiredHeight
	Purpose 	: get the required height for the page (according to its items)
	Parameters 	: 
	Returns 	: minimum required height of the the page
	Info 		: this overrides PAGEITEM::getRequiredHeight
*/
float PAGE::getRequiredHeight(void)
{
	if(this->pageItemFlags & PIFLAG_HEIGHTCALCULATED)
	{
		return this->requiredHeight;
	}

	PAGEROW*	row;
	PAGEITEM*	item;
	int			numRows=this->getNumRows();
	int			numFlexibleRows=0;
	int			r;
	float		tempRequiredHeight;
	RECTANGLE	rect2;

	// PP: take into account page margin
	rect2=this->rect.removeMargin(this->margin);

	this->requiredHeight=0.0f;

	for(r=numRows-1; r>=0; r--)
	{
		row=this->getRow(r);

		// PP: THIS MAY WELL NOT BE A GODD IDEA - TEST ME
		// PP: set the row dimensions to a maximum so it knows how much width it has to play with
		row->setRect(rect2);

		tempRequiredHeight=row->getRequiredHeight();

		if(tempRequiredHeight == 0.0f)
		{
			++numFlexibleRows;
		}
		else
		{
			this->requiredHeight += tempRequiredHeight;
		}
	}

	// PP: NEW FIX: the required height of a page can be no less than the required height of the page's tallest free item
	for(int i=this->getNumFreeItems()-1; i>=0; i--)
	{
		item=this->getFreeItem(i);

		this->requiredHeight = MAX(this->requiredHeight, item->getRequiredHeight());
	}

	// PP: apply margins before align scale - it just somehow makes sense
	this->requiredHeight += (this->margin*2.0f);

	// PP: Now apply align scale - though I imagine it'll get used less now that you can set absolute margins on pages
	this->requiredHeight *= this->getAlignScale();

	this->pageItemFlags |= PIFLAG_HEIGHTCALCULATED;
	
	return this->requiredHeight;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getRow
	Purpose 	: get the row with the specified name
	Parameters 	: (opt/NULL returns first row)name of row to find, (opt/NULL)ptr to storage of row index if found
	Returns 	: ptr to the row if found, or NULL
	Info 		: 
*/
PAGEROW* PAGE::getRow(const char* const rowName, int* const rowIndexOut)
{
	if(rowName == NULL)
	{
		// PP: try to find the row by index...

		if(rowIndexOut != NULL)
		{
			*rowIndexOut=0;	
		}
		
		return this->getRow(0);
	}
	else
	{
		// PP: try to find the row by name...

		PAGEROW*	row;

		for(int r=this->getNumRows()-1; r>=0; r--)
		{
			row=this->getRow(r);

			if(strcmp(row->name, rowName) == 0)
			{
				if(rowIndexOut != NULL)
				{
					*rowIndexOut=r;
				}

				return row;
			}
		}

		// PP: didn't find it
		*rowIndexOut= -1;
		return NULL;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::selectRow
	Purpose 	: select the specified row and its items
	Parameters 	: (opt/0)row to select, (opt/NULL)name of row to start counting from,
					(opt/0 considers all)number of rows in the group of interest					
	Returns 	: true if the row was found
	Info 		: // PP: VERY OLD, PROBABLY OUT OF DATE
*/
bool PAGE::selectRow(const int index, const char* const firstRowName, const int numRowsInGroup)
{
	int			firstRowIndex;
	PAGEROW*	row;
	bool		found=false;

	if (index != -1)
	{
		if(firstRowName == NULL)
		{
			firstRowIndex=0;
		}
		else
		{
			this->getRow(firstRowName, &firstRowIndex);
		}
	}
	else
	{
		firstRowIndex = 0;
	}

	if(firstRowIndex == -1)
	{
		firstRowIndex=0;
	}

	for(int r=firstRowIndex+(numRowsInGroup?numRowsInGroup:(this->getNumRows()-firstRowIndex))-1; r>=firstRowIndex; r--)
	{
		row=this->getRow(r);

		if((r == (firstRowIndex+index))&&(index!=-1))
		{
			found=true;

			// PP: this is the one; select it
			row->select(true);
		}	
		else
		{
			// PP: this ain't the one; deselect it
			row->select(false);
		}
	}

	return found;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getFirstItem
	Purpose 	: get the first item on the page
	Parameters 	: 
	Returns 	: ptr to item if found, NULL otherwise
	Info 		: 
*/
PAGEITEM* PAGE::getFirstItem(void)
{
	PAGEROW*	row;
	PAGEITEM*	item;

	row=this->getFirstRow();

	if(row == NULL)
	{
		return this->getFreeItem(0);
	}

	item=row->getFirstItem();

	if(item == NULL)
	{
		return this->getFreeItem(0);
	}
	else
	{
		return item;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getFirstSelectableItem
	Purpose 	: get the first selectable item on the page
	Parameters 	: whether to search into the page's items rather than just returning this
	Returns 	: ptr to first selectable item if found, NULL otherwise
	Info 		: this overrides PAGEITEM::getFirstSelectableItem
*/
PAGEITEM* PAGE::getFirstSelectableItem(const bool recur)
{
	PAGEROW*	row;
	PAGEITEM*	item;

	if(!recur)
	{
		return PAGEITEM::getFirstSelectableItem(recur);// PP: param is inconsequential
	}
	
	for(int r=0; r<this->getNumRows(); r++)
	{
		row=this->getRow(r);

		item=row->getFirstSelectableItem(true);

		if(item != NULL)
		{
			return item;
		}
	}

	for(int i=0; i<this->getNumFreeItems(); i++)
	{
		item=this->getFreeItem(i)->getFirstSelectableItem(true);

		if(item != NULL)
		{
			return item;
		}
	}

	// PP: no selectable items
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::shutdown
	Purpose 	: free up space used by the page and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::shutdown
*/
void PAGE::shutdown(void)
{
	// PP: IMPORTANT: PAGEITEM base-class shutdown, including effects shutdown
	PAGEITEM::shutdown();

	bkPrintf("shutting down page '%s'\n", this->name);

	// PP: linked lists know their type and will call the appropriate destructor for their items
	this->rows.kill();

	// PP: free up free items too (kinda important??)
	this->freeItems.kill();

	this->updateFunc=NULL;
	this->updateFuncContext=0;

	this->openFunc=NULL;
	this->openFuncContext=0;

	this->closeFunc=NULL;
	this->closeFuncContext=0;

	this->name[0]='\0';
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::~TAG_PAGE
	Purpose 	: destructor; calls shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
PAGE::~TAG_PAGE(void)
{
	this->shutdown();
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getItemsUnsuitabilityAsNavigationTarget
	Purpose 	: determine how unsuitable an item is as a navigation target for a page
	Parameters 	: item to assess, position of currently selected item, angle of navigation in radians
	Returns 	: an unsuitability value, or sub-zero for completely unsuitable
	Info 		: Do not take with any other paracetamol-containing products
*/

#define PAGENAV_DIST_BIAS		900.0f
#define PAGENAV_MAX_ANGDIFF		RAD(45)			// PP: maximum allowable angle between stick direction and direction to an item, below which the item will be moved to - does that make sense?

float PAGE::getItemsUnsuitabilityAsNavigationTarget(PAGEITEM* const item, const TBVector2D currentPos, const float navAng)
{
	TBVector2D	itemPos;
	float		itemAng;
	float		angDiff;
	float		dist;

	//*

	// PP: GET THE ANGLE BETWEEN a) THE DIRECTION OF NAVIGATION AND b) THE DIRECTION TO THE ITEM'S CENTRE POINT
	// PP: REMOUT: not good enough		
	item->getPos(itemPos);
	
	/*/

	// PP: GET THE ANGLE BETWEEN a) THE DIRECTION OF NAVIGATION AND b) THE DIRECTION TO THE CLOSEST POINT ON THE ITEM'S PERIMETER TO THE LINE OF NAVIGATION
	{
		TBCollisionLine2D	line;

		line.start[X]=currentPos[X];
		line.start[Y]=currentPos[Y];
		line.end[X]=currentPos[X]+(10000*bmSin(navAng));
		line.end[Y]=currentPos[Y]+(10000*bmCos(navAng));

		item->screenRect.getClosestPointToLine(&line, itemPos);
	}

  //*/

	itemAng=bmATan2(itemPos[X]-currentPos[X], itemPos[Y]-currentPos[Y]);

	angDiff=FindShortestAngle(Aabs(itemAng), navAng);

	// PP: if angle difference is too big
	// NH: It was tto big, but I'm not gonna so there!  8)
	// PP: Don't make me get the duct tape!
	// NH: Never know, I might like that sort of thing! (only kiddin')
	if(angDiff > PAGENAV_MAX_ANGDIFF) return -1.0f;

	dist=bmVector2DDistance(itemPos, currentPos);

	return (PAGENAV_DIST_BIAS*dist)+angDiff;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::navigate
	Purpose 	: navigate in a given direction across the page
	Parameters 	: angle of navigation 0..2PI, whether or not to navigate recursively into sub-pages etc.,
					the EPageNavInput type that triggered the navigation (PAGENAVINPUT_...)
	Returns 	: 
	Info 		: 
*/
void PAGE::navigate(const float navAng, const bool recur, const EPageNavInput inputType)
{
	// PP: redirect navigation to the navigation scope if appropriate
	if(this->navigationScope != this)
	{
		this->navigationScope->navigate(navAng, recur, inputType);// PP: 2nd param was default true

		// PP: TODO: SHOULDN'T WE SURELY RETURN NOW???!!!
	}

	PAGEITEM*			target;
	NAVCANDIDATE_INFO	info;

	if(this->selectedItem == NULL)
	{
#if(BPLATFORM != PC)// PP: TEMP?
		this->selectItem(this->getFirstSelectableItem(recur));
#endif// PP: (BPLATFORM != PC)

		return;
	}

	this->selectedItem->getPos(info.currentPos);

	info.currentItem	=this->selectedItem;
	info.navAng			=navAng;
	info.recur			=recur;
	info.inputType		=inputType;
	info.wrapIn			=false;

	target=this->getNavigationCandidate(info);

	if(target != NULL)
	{
		if(target != this->selectedItem)// PP: NEW TEST
		{
			if(recur)
			{
				// PP: IMPORTANT: DON'T EVER CONFUSE 'select' AND 'selectItem' !!!
				this->selectItem(target);
			}
			else
			{
				// PP: try to set the target item as the navigation scope for the page; failing that just select it...

				PAGE*	tempScope;

				tempScope=target->getNavigationScope();

				if(tempScope == NULL)
				{
					// PP: it's not a page - reset navigation scope to top level and *select* the item instead...

					this->resetNavigationScope();

					this->selectItem(target);
				}
				else
				{
					// PP: yay! it's a page

					this->setNavigationScope(tempScope);
				}
			}
		}
		playNavSound(this->menuSounds, EMSFX_NAVIGATE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::detectMenuActionPress
	Purpose 	: detect the 'press' menu action
	Parameters 	: 
	Returns 	: true if the 'press' menu action was detected, else false
	Info 		: 
*/
bool PAGE::detectMenuActionPress(void) const
{
	// PP: pages used in the pause menu will have PAGEFLAG_IGNORESTARTBUTTON set,
	// PP: so they don't interfere with the START button's pause/unpause function
	if(this->flags & PAGEFLAG_IGNORESTARTBUTTON)
	{
		return((OKBUTTON_NOTSTART && ((this->controller == -1) || (this->controller == 0)))
				||(OKBUTTON2_NOTSTART && ((this->controller == -1) || (this->controller == 1))));
	}
	else
	{
		return((OKBUTTON && ((this->controller == -1) || (this->controller == 0)))
				||(OKBUTTON2 && ((this->controller == -1) || (this->controller == 1))));
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::debounceMenuActionPress
	Purpose 	: force-debounce the button(s) used to trigger the 'press' menu action
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::debounceMenuActionPress(void) const
{
	FORCE_OKBUTTON_DEBOUNCE;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::detectMenuActionBack
	Purpose 	: detect the 'back' menu action
	Parameters 	: 
	Returns 	: true if the 'back' menu action was detected, else false
	Info 		: 
*/
bool PAGE::detectMenuActionBack(void) const
{
	return ((CANCELBUTTON && ((this->controller == -1) || (this->controller == 0)))
			||(CANCELBUTTON2 && ((this->controller == -1) || (this->controller == 1))));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::debounceMenuActionBack
	Purpose 	: force-debounce the button(s) used to trigger the 'back' menu action
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::debounceMenuActionBack(void) const
{
	FORCE_CANCELBUTTON_DEBOUNCE;
}

/*	--------------------------------------------------------------------------------
	Function 	: PAGE::playNavSound
	Purpose 	: Plays a navigation sound
	Parameters 	: Pointer to the menu sounds, sound type
	Returns 	: 
	Info 		: 
*/
void PAGE::playNavSound(struct TAG_MENU_SOUNDS *sounds, int soundType)
{
	switch (soundType)
	{
	case EMSFX_SELECT:
		// NH: Play Select sound
		if ((sounds) && (sounds->selectSound.name != NULL))
		{
			PlaySample(sounds->selectSound.name,sounds->selectSound.volume);
		}
		break;
	case EMSFX_NAVIGATE:
		// NH: Play Select sound
		if ((sounds) && (sounds->navigateSound.name != NULL))
		{
			PlaySample(sounds->navigateSound.name,sounds->navigateSound.volume);
		}
		break;
	case EMSFX_BACK:
		// NH: Play Select sound
		if ((sounds) && (sounds->backupSound.name != NULL))
		{
			PlaySample(sounds->backupSound.name,sounds->backupSound.volume);
		}
		break;
	case EMSFX_ERROR:
		// NH: Play Select sound
		if ((sounds) && (sounds->errorSound.name != NULL))
		{
			PlaySample(sounds->errorSound.name,sounds->errorSound.volume);
		}
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: PAGE::respondToNavigation
	Purpose 	: respond to the Player navigating around the page with the pad
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::respondToNavigation(void)
{
	PAGEITEM*			item;
	bool				playSelectSound = false;
	EPageNavInput		inputType=PAGENAVINPUT_DEBOUNCE;// PP: just to be safe
	MENUACTION_INFO		info;


	if(this->menuActionFunc != NULL)
	{
		// PP: respond to the 'start' menu action (pad 1)...
		// NH: Note: On the PC the players don't have individual start buttons so this is being set to the Jump (Cross) channel

#if(BPLATFORM==PC) 
#define RESPOND_TO_MENUACTION_START(_1basedPadNum) \
	if(controller##_1basedPadNum.crossDebounceChannel->value != 0) \
		{ \
			info.player=controller##_1basedPadNum.player;\
			info.port=controller##_1basedPadNum.port;\
			\
			info.item=NULL;\
			info.action=MENUACTION_START;\
			info.context=this->menuActionContext;\
			\
			this->menuActionFunc(info);\
		}
#else // NH: #if(BPLATFORM==PC) 
#define RESPOND_TO_MENUACTION_START(_1basedPadNum) \
	if(controller##_1basedPadNum.startDebounceChannel->value != 0) \
		{ \
			info.player=controller##_1basedPadNum.player;\
			info.port=controller##_1basedPadNum.port;\
			\
			info.item=NULL;\
			info.action=MENUACTION_START;\
			info.context=this->menuActionContext;\
			\
			this->menuActionFunc(info);\
		}
#endif // NH: #if(BPLATFORM==PC) 

		RESPOND_TO_MENUACTION_START(1);
		RESPOND_TO_MENUACTION_START(2);

		if(NUM_CONTROLLER_PORTS == 4)
		{
			// PP: YES, THESE *ARE* NEEDED
			RESPOND_TO_MENUACTION_START(3);
			RESPOND_TO_MENUACTION_START(4);
		}

		if(this->detectMenuActionPress())
		{
			// PP: 'press' item
			if(this->selectedItem != NULL)
			{
				// PP: NEW - can't press an input-blocked item
				if((~this->selectedItem->pageItemFlags) & PIFLAG_IGNOREINPUT)
				{
					// PP: REMOUT: THIS *SHOULD* GO HERE.
					// PP: HOWEVER, BECAUSE I'M HAVING TO INTEGRATE PROPER MENU ACTION HANDLING
					// PP: WITH OTHER PEOPLE'S MENU'S, I'VE HAD TO SHIFT IT INTO
					// PP: THE MenuActionCallbacks THEMSELVES :o(				this->debounceMenuActionPress();

					info.item=this->selectedItem;
					info.action=MENUACTION_PRESS;
					info.context=this->menuActionContext;
					info.player=MENUACTION_NO_PLAYER;			// PP: TEMP?: player not determined

					this->menuActionFunc(info);
				}
			}

			// NH: For now I can use this to play a sample
			// NH: Play navigate sound
			/*if ((this->menuSounds)&&(this->menuSounds->navigateSound.name != NULL))
			{
				PlaySample(this->menuSounds->navigateSound.name,this->menuSounds->navigateSound.volume);
			}
			*/
			playNavSound(this->menuSounds, EMSFX_SELECT);
		}
		
		if(this->detectMenuActionBack())
		{
			// PP: cancel, or go 'back' or 'up'

			// PP: REMOUT: THIS *SHOULD* GO HERE.
			// PP: HOWEVER, BECAUSE I'M HAVING TO INTEGRATE PROPER MENU ACTION HANDLING
			// PP: WITH OTHER PEOPLE'S MENU'S, I'VE HAD TO SHIFT IT INTO
			// PP: THE MenuActionCallbacks THEMSELVES :o(				this->debounceMenuActionBack();

			info.item=NULL;
			info.action=MENUACTION_BACK;
			info.context=this->menuActionContext;
			info.player=MENUACTION_NO_PLAYER;			// PP: TEMP?: player not determined

			// PP: NOTE: passing NULL as the item because the 'back' action doesn't involve an item
			this->menuActionFunc(info);
			
			// NH: But for now I can use this to play a sample
			// NH: Play Deselection (backup) Sound
			/*if ((this->menuSounds)&&(this->menuSounds->backupSound.name != NULL))
			{
				PlaySample(this->menuSounds->backupSound.name,this->menuSounds->backupSound.volume);
			}
			*/
			playNavSound(this->menuSounds, EMSFX_BACK);
		}
	}

	if(this->flags & PAGEFLAG_NAVIGABLE)
	{

#if(BPLATFORM == PC)

		// PP: mouse control...

		// NH: Find the item that the mouse is over
		item = this->getItem(mouse.mousePos[X], mouse.mousePos[Y]);
			
		// NH: Select the item that the mouse was over
		if ((item != NULL) && (item->pageItemFlags & PIFLAG_SELECTABLE) && (item != this->selectedItem))
		{
			this->selectItem(item);
			// NH: Set to play select Sound
			//playSelectSound = true;
			playNavSound(this->menuSounds, EMSFX_NAVIGATE);
		}

#endif// PP: (BPLATFORM == PC)

		// PP: now directional navigation...
		{
			float	navAng;		// PP: angle of navigation
			float	xVal=0.0f;
			float	yVal=0.0f;
		
	#define DEBOUNCE_LEFT_AND_RIGHT \
			biForceDebounce(controller1.menuLeftRepeatChannel); \
			biForceDebounce(controller1.menuRightRepeatChannel); \
			biForceDebounce(controller1.menuLeftDebounceChannel); \
			biForceDebounce(controller1.menuRightDebounceChannel); \
			biForceDebounce(controller2.menuLeftRepeatChannel); \
			biForceDebounce(controller2.menuRightRepeatChannel); \
			biForceDebounce(controller2.menuLeftDebounceChannel); \
			biForceDebounce(controller2.menuRightDebounceChannel);

			if((controller1.menuUpDebounceChannel->value  && (this->controller == -1 || this->controller == 0))
				||(controller2.menuUpDebounceChannel->value  && (this->controller == -1 || this->controller == 1)))
			{
				yVal=-1.0f;
				xVal=0.0f;

				inputType=PAGENAVINPUT_DEBOUNCE;
			}
			else if((controller1.menuDownDebounceChannel->value  && (this->controller == -1 || this->controller == 0))
					||(controller2.menuDownDebounceChannel->value  && (this->controller == -1 || this->controller == 1)))
			{
				yVal=1.0f;
				xVal=0.0f;

				inputType=PAGENAVINPUT_DEBOUNCE;
			}
			else if((controller1.menuUpRepeatChannel->value  && (this->controller == -1 || this->controller == 0))
					||(controller2.menuUpRepeatChannel->value  && (this->controller == -1 || this->controller == 1)))
			{
				yVal=-1.0f;
				xVal=0.0f;

				inputType=PAGENAVINPUT_AUTOREPEAT;
			}
			else if((controller1.menuDownRepeatChannel->value  && (this->controller == -1 || this->controller == 0))
					||(controller2.menuDownRepeatChannel->value  && (this->controller == -1 || this->controller == 1)))
			{
				yVal=1.0f;
				xVal=0.0f;

				inputType=PAGENAVINPUT_AUTOREPEAT;
			}
			
			if(this->flags & PAGEFLAG_COLUMNNAVIGATION)
			{
				// PP: COLUMN NAVIGATION WITH SHOULDER BUTTONS...

				if((controller1.l1DebounceChannel->value)||(controller2.l1DebounceChannel->value)
					||(controller1.l2DebounceChannel->value)||(controller2.l2DebounceChannel->value))
				{
					this->resetNavigationScope();// PP: DUH! kinda important
					this->navigate(PI+HALFPI, false, PAGENAVINPUT_DEBOUNCE);
					return;
				}
				else if((controller1.r1DebounceChannel->value)||(controller2.r1DebounceChannel->value)
					||(controller1.r2DebounceChannel->value)||(controller2.r2DebounceChannel->value))
				{
					this->resetNavigationScope();// PP: DUH! kinda important
					this->navigate(HALFPI, false, PAGENAVINPUT_DEBOUNCE);
					return;
				}

				// PP: no recursive horizontal navigation
				xVal=0.0f;
			}
			else
			{
				if(yVal)
				{
					DEBOUNCE_LEFT_AND_RIGHT;
				}
				else
				{

	#define DEBOUNCE_UP_AND_DOWN \
			biForceDebounce(controller1.menuUpRepeatChannel); \
			biForceDebounce(controller1.menuDownRepeatChannel); \
			biForceDebounce(controller1.menuUpDebounceChannel); \
			biForceDebounce(controller1.menuDownDebounceChannel); \
			biForceDebounce(controller2.menuUpRepeatChannel); \
			biForceDebounce(controller2.menuDownRepeatChannel); \
			biForceDebounce(controller2.menuUpDebounceChannel); \
			biForceDebounce(controller2.menuDownDebounceChannel);

					if((controller1.menuRightDebounceChannel->value && (this->controller == -1 || this->controller == 0))
						||(controller2.menuRightDebounceChannel->value && (this->controller == -1 || this->controller == 1)))
					{
						xVal=1.0f;
						yVal=0.0f;

						inputType=PAGENAVINPUT_DEBOUNCE;
					}
					else if((controller1.menuLeftDebounceChannel->value && (this->controller == -1 || this->controller == 0))
							||(controller2.menuLeftDebounceChannel->value && (this->controller == -1 || this->controller == 1)))
					{
						xVal=-1.0f;
						yVal=0.0f;

						inputType=PAGENAVINPUT_DEBOUNCE;
					}
					else if((controller1.menuRightRepeatChannel->value && (this->controller == -1 || this->controller == 0))
							||(controller2.menuRightRepeatChannel->value && (this->controller == -1 || this->controller == 1)))
					{
						xVal=1.0f;
						yVal=0.0f;

						inputType=PAGENAVINPUT_AUTOREPEAT;
					}
					else if((controller1.menuLeftRepeatChannel->value && (this->controller == -1 || this->controller == 0))
							||(controller2.menuLeftRepeatChannel->value && (this->controller == -1 || this->controller == 1)))
					{
						xVal=-1.0f;
						yVal=0.0f;

						inputType=PAGENAVINPUT_AUTOREPEAT;
					}

					if(xVal)
					{
						DEBOUNCE_UP_AND_DOWN;
					}
				}
			}

			// PP: RECURSIVE NAVIGATION WITH STICK OR DPAD...

			if(xVal||yVal)
			{
				navAng=Aabs((float)bmATan2(float(xVal), float(-yVal)));

				this->navigate(navAng, true, inputType);// PP: recur was default true

		//		playSelectSound = true;
			}
		}

	}// PP: end if (navigable)

/*	if (playSelectSound)
	{
		// NH: Play Select sound
		if ((this->menuSounds)&&(this->menuSounds->selectSound.name != NULL))
		{
			playNavSound(this->menuSounds, EMSFX_NAVIGATE);//PlaySample(this->menuSounds->selectSound.name,this->menuSounds->selectSound.volume);
		}

		playSelectSound = false;
	}
*/
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this PAGE
	Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
	Returns 	: 
	Info 		: this overrides PAGEITEM::chooseEffect, to provoke effect choices by the page's contents
*/
void PAGE::chooseEffect(const PIEMode mode, const bool recur)
{
	// PP: choose an effect for this page...
	// PP: (See macro definition in PageItemFX.h)
	PAGEITEM_CHOOSE_EFFECT(page);

	if(recur)
	{
		// PP: *** get all the page's contents to choose an effect ***
		// PP: "Come on, choose a sweety - I've chosen mine, mmmm!"...

		PAGEROW*			row;
		PAGEITEM*			item;

		for(int r=this->getNumRows()-1; r>=0; r--)
		{
			row=this->getRow(r);

			for(int i=row->getNumItems()-1; i>=0; i--)
			{
				item=row->getItem(i);

				item->chooseEffect(mode, true);
			}
		}

		// PP: same for the free items...

		for(int f=this->getNumFreeItems()-1; f>=0; f--)
		{
			item=this->getFreeItem(f);

			item->chooseEffect(mode, true);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::addRow
	Purpose 	: add a row to the page
	Parameters 	: 
	Returns 	: ptr to the new row
	Info 		: 
*/
PAGEROW* PAGE::addRow(void)
{
	return this->rows.append(new PAGEROW(this->rows.getSize()));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::effectsFinished
	Purpose 	: determine if all effects on the page and its contents have finished
	Parameters 	: 
	Returns 	: true if all effects on the page and its contentss have finished, else false
	Info 		: this overrides PAGEITEM::effectsFinished, to check that the page's contents' effects are also finished
*/
bool PAGE::effectsFinished(void)
{
	if(!this->hasEffects())
	{
		// PP: no effects, so it's as if all effects are finished
		return true;
	}

	if(!PAGEITEM::effectsFinished())
	{
		return false;
	}

	// PP: check all rows' items' effects have finished...

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		if((~this->getRow(r)->flags) & ROWFLAG_ALLEFFECTSFINISHED)
		{
			return false;
		}
	}

	// PP: check all free items' effects have finished...

	for(int i=this->getNumFreeItems()-1; i>=0; i--)
	{
		if(!this->getFreeItem(i)->effectsFinished())
		{
			return false;
		}
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::hasEffects
	Purpose 	: determine if the page or any of its contents have effects on them
	Parameters 	: 
	Returns 	: true if the page or any of its contents has an effect on it
	Info 		: this overrides PAGEITEM::hasEffects, to check if the page's contents have any effects
*/
bool PAGE::hasEffects(void)
{
	// PP: check itself
	if(PAGEITEM::hasEffects())
	{
		return true;
	}

	// PP: check each row...

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		if(this->getRow(r)->flags & ROWFLAG_HASEFFECTS)
		{
			return true;
		}
	}

	// PP: check each free item...

	for(int f=this->getNumFreeItems()-1; f>=0; f--)
	{
		if(this->getFreeItem(f)->hasEffects())
		{
			return true;
		}
	}

	// PP: no effects found
	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::freeTempMemory
	Purpose 	: free-up temporary memory used by the item and its contents,
					eg. effects that have finished
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::freeTempMemory
*/
void PAGE::freeTempMemory(void)
{
	PAGETRAVERSAL	trav(this);

	PAGEITEM*		item;

	PAGEITEM::freeTempMemory();

	// PP: provoke the call in all the page's contents

	while((item=trav.next()) != NULL)
	{
		item->freeTempMemory();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::drawBackgroundAndBorder
	Purpose 	: draw the book's background and border if it has them
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::drawBackgroundAndBorder
*/
void BOOK::drawBackgroundAndBorder(void)
{
	// PP: NOTE: RECT::draw was designed for convenience and versatility, not for speed!
	// PP: You might wanna change this over to be faster if you use backgrounds and borders a lot

	if((this->flags & BKFLAG_FULLSCREENBACKGROUND)
		&&(this->screenIndex == BKSCREEN_FULL))			// PP: TEMP?: only support "full-screen" backgrounds for full-screen books
	{
		RECTANGLE	fullRect;
		COLOUR		tempColour;

		// PP: DRAW FULL-SCREEN BACKGROUND...

		NOTEXTURE;

		// PP: SCALE: 1 is always the height of the safe area
		// PP: so this ere rect should always cover the whole screen, in any res and any aspect ratio
		fullRect=RECTANGLE(-2.0f, 2.0f, -2.0f, 2.0f);

		tempColour = this->background;
		tempColour.a = (uchar)(float(this->background.a * this->opacity));
		fullRect.draw(tempColour, SOLIDFILL);

		// PP: DRAW BORDER AS USUAL (this is copied from PAGEITEM::drawBackgroundAndBorder; maybe I should split the two at some point?) ...

		if (this->border.a)
		{
			COLOUR	tempColour = this->border;
			tempColour.a = (uchar)(float(this->border.a * this->opacity));
			this->boxRect.draw(tempColour, OUTLINE);
		}
	}
	else
	{
		// PP: DRAW BACKGROUND AND BORDER AS USUAL
		PAGEITEM::drawBackgroundAndBorder();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setName
	Purpose 	: set the name of the page
	Parameters 	: the name of the page!
	Returns 	: (PAGEITEM*)this
	Info 		: this overrides PAGEITEM::setName
*/
PAGEITEM* PAGE::setName(const char* const nameIn)
{
	PAGEITEM::setName(nameIn);

	this->rows.setName("%s rows", this->name);
	this->freeItems.setName("%s free items", this->name);

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::drawscreenRects
	Purpose 	: draw the screen rects of the page and its items
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::drawScreenRects
*/
void PAGE::drawScreenRects(void)
{
	PAGEROW*	row;
	PAGEITEM*	item;

	PAGEITEM::drawScreenRects();

	for(int r=this->getNumRows()-1; r>=0; r--)
	{
		row=this->getRow(r);

		for(int i=row->getNumItems()-1; i>=0; i--)
		{
			item=row->getItem(i);

			item->drawScreenRects();
		}
	}

	// PP: draw the free items' screen rects...

	for(int i=this->getNumFreeItems()-1;i>=0;i--)
	{
		this->getFreeItem(i)->drawScreenRects();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setNavigation
	Purpose 	: specify whether or not the page is navigable
	Parameters 	: whether or not the page is navigable
	Returns 	: this
	Info 		: you can only navigate betweeen selectable items on the page.
					Page items default to selectable.
*/
PAGE* PAGE::setNavigation(const bool on)
{
	if(on)
	{
		this->flags |= PAGEFLAG_NAVIGABLE;
	}
	else
	{
		this->flags &= (~PAGEFLAG_NAVIGABLE);
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setColumnNavigation
	Purpose 	: set column navigation mode true/false
	Parameters 	: whether or not navigation is column-stylee
	Returns 	: this
	Info 		: you can only navigate betweeen selectable items on the page.
					this defaults to false.
					*** YOU STILL HAVE TO CALL setNavigation !! ***
*/
struct TAG_PAGE* PAGE::setColumnNavigation(const bool on)
{
	if(on)
	{
		this->flags |= PAGEFLAG_COLUMNNAVIGATION;
	}
	else
	{
		this->flags &= ~PAGEFLAG_COLUMNNAVIGATION;
	}

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGEITEM::setSounds
	Purpose 	: Sets the sounds for the page (Defaults to using the sounds from the book creating the page)
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PAGE::setSounds(MENU_SOUNDS* menuSounds)
{
	this->menuSounds = menuSounds;		
}

/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setController
	Purpose 	: Sets the controller able to navigate the page
	Parameters 	: 
	Returns 	: 
	Info 		: -1 = global; 0 = pad 1; 1 = pad 2
*/
void PAGE::setController(char con)
{
	this->controller = con;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setMargin
	Purpose 	: set the width of the margin around the inside of the page
	Parameters 	: the width of the margin around the inside of the page,
					as a fraction of screen height
	Returns 	: this
	Info 		: "A weeble with a gun"?!  What?!
*/
struct TAG_PAGE* PAGE::setMargin(const float marginIn)
{
	this->margin=marginIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::setNavigationWrap
	Purpose 	: set the flags describing the bahaviour of the page's navigation
	Parameters 	: one or more OR'd PAGEFLAG_NAVWRAP_ values to define navigation wrapping behaviour
	Returns 	: this
	Info 		: 
*/
struct TAG_PAGE* PAGE::setNavigationWrap(const uint32 navFlags)
{
	this->flags &= ~PAGE_NAVWRAP_FLAGS;

	this->flags |= navFlags;

	return this;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PAGETRAVERSAL

								Class used to efficiently traverse the top level of a page.

														@PAGETRAVERSAL
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::setPage
	Purpose 	: set the page to traverse
	Parameters 	: ptr to the page to traverse
	Returns 	: a ptr to the item that the traversal is currently at, or NULL if finished
	Info 		: 
*/
void PAGETRAVERSAL::setPage(PAGE* pageIn)
{
	this->page=pageIn;

	this->row=this->page->getFirstRow();

	if(this->row == NULL)
	{
		// PP: no rows - go straight onto free items then
		this->stage=STAGE_FREE;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::init
	Purpose 	: initialise the PAGETRAVERSAL
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: called by constructors
*/
void PAGETRAVERSAL::init(void)
{
	this->rowIndex=0;
	this->columnIndex=0;
	this->freeItemIndex=0;
	this->itemIndex=0;
	this->item=NULL;
	this->stage=STAGE_ALIGNED;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::init
	Purpose 	: initialise the PAGETRAVERSAL and set its page
	Parameters 	: ptr to page to traverse
	Returns 	: 
	Info 		: // PP: called by constructors
*/
void PAGETRAVERSAL::init(PAGE* const pageIn)
{
	this->init();
	this->setPage(pageIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::PAGETRAVERSAL
	Purpose 	: default constructor; calls init
	Parameters 	: 
	Returns 	: new PAGETRAVERSAL instance
	Info 		: 
*/
PAGETRAVERSAL::PAGETRAVERSAL(void)
{
	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::PAGETRAVERSAL
	Purpose 	: constructor; calls init and setPage
	Parameters 	: ptr to page to traverse
	Returns 	: new PAGETRAVERSAL instance
	Info 		: 
*/
PAGETRAVERSAL::PAGETRAVERSAL(PAGE* const pageIn)
{
	this->init(pageIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGETRAVERSAL::next
	Purpose 	: progress to the next item in the page
	Parameters 	: 
	Returns 	: ptr to the next item in the page, or NULL if finished
	Info 		: // PP: use this to get the first item as well
*/
PAGEITEM* PAGETRAVERSAL::next(void)
{
	if(this->stage == STAGE_ALIGNED)
	{
		// PP: handle empty rows
		if(this->columnIndex == 0)
		{
			while((this->item=this->row->getFirstItem()) == NULL)
			{
				this->row=this->page->getRow(++this->rowIndex);

				if(this->row == NULL)
				{
					this->stage=STAGE_FREE;
					goto freeItems;
				}
			}

			if(this->item == NULL)
			{
				this->stage=STAGE_FREE;
				goto freeItems;
			}
		}

		this->item=this->row->getItem(this->columnIndex++);

		if(this->columnIndex == this->row->getNumItems())
		{
			this->rowIndex++;

			if(this->rowIndex == this->page->getNumRows())
			{
				// PP: onto the free items
				this->stage=STAGE_FREE;
			}
			else
			{
				// PP: onto the next row
				
				this->row=this->page->getRow(this->rowIndex);
				this->columnIndex=0;
			}
		}
	}
	else if(this->stage == STAGE_FREE)
	{
freeItems:// PP: is this safe?  jumping from one if-block to another using a goto?

		this->item=this->page->getFreeItem(this->freeItemIndex);

		if(this->freeItemIndex == this->page->getNumFreeItems())
		{
			this->stage=STAGE_FINISHED;
		}

		this->freeItemIndex++;
	}
	else// PP: finished
	{
		this->item=NULL;
	}

	return this->item;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														BOOK methods

			Books are a sensible way of storing, displaying and navigating pages of visual data such as text or graphics.
			Deriving from this base class lets you easily create books that do whatever crazy stuff you want them to do.

												There's no limit to the fun!™

												*BOOK* > pages > rows > items

															@book
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: initialise static members...

// PP: **** PS2 BABEL WORKAROUND ****
// PP: dimensions of the render target to which a book is being drawn
uint32 BOOK::PS2BabelWorkaround_renderTargetWidth	=128;// PP: 128: just so's ye'll see something if you forget to call PS2BabelWorkaround_setRenderTargetDimensions
uint32 BOOK::PS2BabelWorkaround_renderTargetHeight	=128;// PP: 128: just so's ye'll see something if you forget to call PS2BabelWorkaround_setRenderTargetDimensions


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this BOOK
	Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
	Returns 	: 
	Info 		: this overrides PAGEITEM::chooseEffect, to provoke effect choices by the book's pages
*/
void BOOK::chooseEffect(const PIEMode mode, const bool recur)
{
	// PP: ***** NEW TEST ****
	// PP: don't choose a sodding effect unless the book is open
	if(this->flags & BKFLAG_OPEN)
	{
		// PP: choose an effect for this book...
		// PP: (See macro definition in PageItemFX.h)
		PAGEITEM_CHOOSE_EFFECT(book);

		if(recur)
		{
			// PP: get the book's current page to choose an effect...

			if(this->currentPage != NULL)
			{
				this->currentPage->chooseEffect(mode, recur);
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::effectsFinished
	Purpose 	: determine if all effects on the book and its contents have finished
	Parameters 	: 
	Returns 	: true if all effects on the book and its contents have finished, else false
	Info 		: this overrides PAGEITEM::effectsFinished, to check that the book's contents' effects are also finished
*/
bool BOOK::effectsFinished(void)
{
	// PP: check old page's effects are finished...

	if(this->oldPage != NULL)
	{
		// PP: FRIG.
		// PP: IT IS *DODGE* THAT THE SITUATION CAN ARISE WHEREBY CLOSED BOOKS HAVE EFFECTS RUNNING ON THEM!!!!!!
		// PP: TODO: FIX IT!!!!!!
		// PP: What I need is openness to be inherited down the contents hierarchy!
		// PP: The problem arises from sub-books, within closed books, being opened - they start an effect at this point.
		if((~this->oldPage->flags) & PAGEFLAG_CLOSED)
		{
			if(!this->oldPage->effectsFinished())
			{
				return false;
			}
		}
	}

	// PP: check current page's effects are finished...

	if(this->currentPage != NULL)
	{
		if(!this->currentPage->effectsFinished())
		{
			return false;
		}
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::hasEffects
	Purpose 	: determine if the book or any of its contents have effects on them
	Parameters 	: 
	Returns 	: true if the book or any of its contents has an effect on it
	Info 		: this overrides PAGEITEM::hasEffects, to check if the book's contents have any effects
*/
bool BOOK::hasEffects(void)
{
	// PP: check itself
	if(PAGEITEM::hasEffects())
	{
		return true;
	}

	// PP: check current page (that should be all we need)...

	if(this->currentPage != NULL)
	{
		if(this->currentPage->hasEffects())
		{
			return true;
		}
	}

	// PP: no effects found
	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::freeTempMemory
	Purpose 	: free-up temporary memory used by the item and its contents,
					eg. effects that have finished
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::freeTempMemory
*/
void BOOK::freeTempMemory(void)
{
	PAGEITEM::freeTempMemory();

	for(int i=this->getNumPages()-1; i>=0; i--)
	{
		this->getPage(i)->freeTempMemory();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::findPage
	Purpose 	: get a the index and optionally pointer to the page with a specified name
	Parameters 	: name of page to find, ptr to ptr to point to to page if found
	Returns 	: index of page if found
	Info 		: 
*/
int BOOK::findPage(const char* const pageName, const PAGE** const page)
{
	PAGE*	tempPagePtr;

	for(int i=this->pages.getSize()-1; i>=0; i--)
	{
		tempPagePtr=this->pages.get(i);

		if(stricmp(pageName, tempPagePtr->name)==0)
		{
			// PP: gotcha!

			if(page != NULL)
			{
				(*page)=tempPagePtr;
			}

			return i;
		}
	}

	// PP: didn't find it

	if(page != NULL)
	{
		(*page)=NULL;
	}

	return -1;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::TAG_BOOK
	Purpose 	: constructor
	Parameters 	: name
	Returns 	: new BOOK instance
	Info 		: 
*/
BOOK::TAG_BOOK(const char* const nameIn)
{
	this->init();
	this->setName(nameIn);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::TAG_BOOK
	Purpose 	: default constructor; calls init
	Parameters 	: 
	Returns 	: new BOOK instance
	Info 		: 
*/
BOOK::TAG_BOOK(void)
{
	this->init();
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::TAG_BOOK
	Purpose 	: constructor; calls init
	Parameters 	: flag to start the book in the open state, (opt/NULL=not set) name
	Returns 	: new BOOK instance
	Info 		: 
*/
BOOK::TAG_BOOK(const bool open, const char* const nameIn)
{
	this->init(open);

	if(nameIn != NULL)
	{
		this->setName(nameIn);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::onInsert
	Purpose 	: notifies the book that it's being inserted into a page
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::onInsert
*/
void BOOK::onInsert(void)
{
	PAGEITEM::onInsert();

	// PP: if the book is being inserted into a row, it can't possibly be a top-level book and therefore won't get added to the Book List...
	this->flags &= ~(BKFLAG_TOPLEVELBOOK|BKFLAG_BOOKLIST);

	// PP: NEW
	if(this->onBookList)
	{
		BookList_remove(this);
	}

	// PP: ...and its pages therefore can't possibly be top-level pages
	for(int p=this->getNumPages()-1; p>=0; p--)
	{
		this->getPage(p)->flags &= ~PAGEFLAG_TOPLEVELPAGE;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::init
	Purpose 	: one-time init / reset for the book
	Parameters 	: (opt/false)flag to start the book in the open state
	Returns 	: 
	Info 		: 
*/

#define BOOK_DEFAULT_EFFECT_CHOOSER				(&mainPIEChooser)

void BOOK::init(const bool open)
{
	PAGEITEM::init();

	// PP: book is not on the Book List yet
	this->onBookList=false;

	// PP: set the default effect chooser for the book
	// PP: best to do this here, nice & early
	this->effectChooser=BOOK_DEFAULT_EFFECT_CHOOSER;// PP: realign() will propogate this down the contents hierarchy, don't worry

//	this->pages.init("%s pages", this->name);
	this->pages.init("pages");
	this->currentPageIndex=-1;
	this->oldPage=NULL;
	this->currentPage=NULL;

	SETVECTOR(this->pos, 0.0f, 0.0f, 0.0f, 1.0f);
	this->opacity=1.0f;
	this->closeDelay=0.0f;
	this->closeInstantly=false;

	this->menuSounds = NULL;

	this->flags=(open?BKFLAG_PLEASEOPEN:0)|BOOK_DEFAULT_FLAGS;

	this->screenIndex=BOOK_DEFAULT_SCREEN;

	// PP: set 1:1 aspect ratio
	this->setAspectRatio(1.0f, 1.0f);

	// PP: books default to filling the full 'safe' area of the screen
	this->setBookRect(UORECT);

	// PP: initialise chapters
//	this->chapters.init("%s chapters", this->name);
	this->chapters.init("chapters");

	// PP: don't restrict page turns to the boundaries of any chapter
	this->dontRestrictToChapter();

	// PP: set current language
	this->setLanguage(GAMESTATUS::currentLanguage);

	// NH: set auto drawing to true
	this->setAutoDraw(true);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::open
	Purpose 	: open the book; make it visible
	Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly
	Returns 	: 
	Info 		: 
*/
void BOOK::open(const char* const pageName, const bool instant)
{
	// PP: if the book is flagged as always opening as the exclusive book, well, open it as the exclusive book
	if(this->flags & BKFLAG_OPENEXCLUSIVE)
	{
		// PP: prevent an infernal loop
		if((~this->flags) & BKFLAG_OVERRIDE_OPENEXCLUSIVE)
		{
			this->openExclusive(pageName, instant);
			return;
		}
	}

	if(this->flags & BKFLAG_OPEN)
	{
		if(pageName != NULL)
		{
			this->gotoPage(pageName, instant);
		}

		if((~this->flags)&BKFLAG_CLOSING)
		{
			// PP: book is already fully open
			return;
		}
	}

#ifdef PHIL
	bkPrintf("=================== BOOK::open (%s) ====================\n", this->name);
	STARTTIMER;
#endif// PP: def PHIL

	// PP: if this is not the exclusive book
	if((~this->flags) & BKFLAG_EXCLUSIVE)
	{
		// PP: respect the exclusive book 
		if(BookList_getExclusiveBook() != NULL)
		{
			// PP: flag the book as having to open once there is no exclusive book
			this->flags &= ~BKFLAG_OPENWHENNOEXCLUSIVE;

			return;// PP: opening has been postponed
		}
	}

	// PP: cancel delayed close
	this->closeDelay=0.0f;
	
	// PP: cancel please-open flag
	this->flags &= (~BKFLAG_PLEASEOPEN);

	// PP: cancel closing flag - though I hope & believe this could never be set high on a closed book
	this->flags &= (~BKFLAG_CLOSING);

	// PP: flag the book as being open before we go any further
	this->flags|=BKFLAG_OPEN;

	// PP: *** NEW TEST: BEFORE OPENING ANY PAGES, GET THE BOOK INTO THE CURRENT LANGUAGE ***
	{
		// PP: NOTE: this is also done in 
		// PP: update langauge if nec
		if(this->language != gameStatus.currentLanguage)// PP: book's language is out of date
		{
			// PP: update the book's language
			this->setLanguage(gameStatus.currentLanguage);

			// PP: mmm, better realign I suppose
			// PP: this->realign();
			this->markForRealignment();// PP: we align further down
		}
	}

	if(pageName != NULL)
	{
		this->gotoPage(pageName, true);// PP: yes, true...
	}

// PP: REMOUT: can't skip this now HAHAHAHAHAH	if(realign)
	{
		// PP: now just gonna be an alignContent() call		this->setRect(this->bookRect);// PP: REMOUT: this->realign();

		this->align();
	}
	
	// PP: NOTE: this only needs to be done on first open I think
	this->setEffectChooser(this->effectChooser);

	// PP: open current page
	if(this->currentPage != NULL)
	{
		this->currentPage->open(instant);// PP: IMPORTANT: FIXME: THIS TRIGGERS EFFECT CHOICES THAT GET OVERRIDDEN BY BOOK::CHOOSEEFFECT A BIT FURTHER DOWN!!!!!!
	}

	// PP: NEW: choose an open effect for this book

	// PP: NEW TEST: don't choose effects if opening instantly
	if(!instant)
	{
		// PP: REMOUT: NO		if(this->effectChooser != NULL)
		{
			this->chooseEffect(PIEMODE_ENTER, true);
		}
	}

	// PP: flag this book as being on its first update.
	// PP: On first update, you wouldn't want to be allowing any navigation.
	// PP: For example, as the book tried to open it might pick up the same debounced button press that opened it, and respond to it by closing!
	// PP: An' ye dinna wunt 'at noow, least of all in a Peterhead accent.
	this->flags |= BKFLAG_FIRSTUPDATE;

	// PP: gotter update now so's were ready for the next draw, in terms of beginning the opening transition
	// PP: Put this in because I was getting a flash of full opacity whenever opening a book
// PP: REMOUT: this update'd add the book onto the book list, even if it might be a sub-book
	this->update();


	// PP: ~~~~~~~~~~

	char	timerName[128];

#ifdef PHIL
	sprintf(timerName, "Opening book (%s)", this->name);
	ENDTIMER(timerName);
	bkPrintf("===================================================\n");
#endif// PP: def PHIL
}


 /*	--------------------------------------------------------------------------------
	Function 	: BOOK::openExclusive
	Purpose 	: open the book and have all other books close, until this one is closed
	Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly
	Returns 	: 
	Info 		: 
*/
void BOOK::openExclusive(const char* const pageName, const bool instant)
{
	if(this->flags & BKFLAG_OPEN)
	{
		// PP: already open
		return;
	}

	// PP: set this book as the exclusive book
	BookList_pushExclusiveBook(this);

	this->flags |= BKFLAG_OVERRIDE_OPENEXCLUSIVE;

	// PP: open the book as normal
	this->open(pageName, instant);

	this->flags &= ~BKFLAG_OVERRIDE_OPENEXCLUSIVE;
}



/*	--------------------------------------------------------------------------------
	Function 	: MEMCARDDISPLAY::close
	Purpose 	: close the book; make it invisible
	Parameters 	: (opt/0.0f) delay in seconds before closing, (opt/false) whether or not to close instantly
	Returns 	: 
	Info 		: 
*/
void BOOK::close(const float delay, const bool instant)
{
	// PP: NOTE: I'm allowing you to book delayed closes on books that aren't even open yet.
	// PP:			So if ye really wanted to, you could say ' book->close(5.0f); book->open(); '

	// PP: clear 'open-when-no-exclusive' flag (even if the book is already closing - y'see why?)
	// PP: NOTE: don't worry, when the Book List closes a non-exclusive book, it will set BKFLAG_OPENWHENNOEXCLUSIVE **AFTER THE CLOSE!**
	this->flags &= ~BKFLAG_OPENWHENNOEXCLUSIVE;

	// PP: NEW: if th book is busy closing, leave the poor sod alone!  (unless new close is instant)
	if(this->flags & BKFLAG_CLOSING)
	{
		if(!instant)
		{
			return;// PP: fook off!
		}
	}

	if(this->flags & BKFLAG_EXCLUSIVE)
	{
		// PP: YOUR MAJESTY!  FORGIVE ME!

		BookList_popExclusiveBook(this);
	}

	this->closeInstantly=instant;

	this->closeDelay=delay;

	if(this->closeDelay > 0.0f)
	{
		// PP: the close isn't ready yet
		return;
	}

	if((~this->flags)&BKFLAG_OPEN)
	{
		// PP: feck off!  can't close a book that ain't open!
		return;
	}

	// PP: close the book...

	BOOK_END_TRANSITION;

	if(instant)
	{
		// PP: book is now closed - no need for close effects
		this->flags&=(~BKFLAG_OPEN);
		
		// PP: NOW'D BE AN AWFULLY GOOD TIME TO CHECK FOR FREE-WHEN-CLOSED AND SHUTDOWN-WHEN-CLOSED...

		if(this->flags & BKFLAG_FREEWHENCLOSED)
		{
			this->flags &= ~BKFLAG_FREEWHENCLOSED;// PP: inconsequential?

			// PP: delete the book
			// PP: it will disappear and remove itself from the book list
			delete(this);
			return;
		}
		else if(this->flags & BKFLAG_SHUTDOWNWHENCLOSED)
		{
			this->flags &= ~BKFLAG_SHUTDOWNWHENCLOSED;// PP: inconsequential?

			// PP: shut-down the book
			// PP: it will disappear and remove itself from the book list
			this->shutdown();
			return;
		}
		else
		{
			// PP: remove the book from the book list
			BookList_remove(this);
		}
	}
	else
	{
		// PP: BOOK IS CLOSING
		this->flags |= BKFLAG_CLOSING;
		
		// PP: NEW: choose a close effect for this book
		// PP: REMOUT: NO		if(this->effectChooser != NULL)
		{
			this->chooseEffect(PIEMODE_EXIT, true);
		}
	}

	// PP: REMOUT: TEMP?
	if(this->oldPage != NULL)
	{
		this->oldPage->close(instant); 
	}

	if(this->currentPage != NULL)
	{
		this->currentPage->close(instant);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::startDraw
	Purpose 	: set up conditions needed to draw the book (eg. apply book's scale & position to object matrix)
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::startDraw, to apply all manner of adjustments
*/

// PP: NEW gonna draw book stuff at screen depth now - much cleaner and it means nothing pokes out the H safe area
#define BOOK_STEREO_ADJUST				0.0f//(-0.0027f/50.0f)// PP: expressed as a fraction of screen height

#define TEXTFX_NO_STEREO_ADJUST			// PP: no stereo adjust

void BOOK::startDraw(void)
{
	PAGEITEM::startDraw();

	TBMatrix		tempObjectMatrixCopy;
	TBMatrix		transMatrix;
	TBVector2D		trans;
	TBMatrix		rotMatrix;
	TBMatrix		screenScaleMatrix;
	TBMatrix		finalMatrix;
	TBMatrix		tempMatrix;
	RECTANGLE		tempRect;
	RECTANGLE*		borderRect=NULL;			// PP: rectangle to use to draw the book's border and/or background
	extern bool		g_overrideStereoscopy;		// PP: flag specifying that stereoscopy is to be overridden (display,cpp)


	if(this->flags & BKFLAG_TOPLEVELBOOK)
	{
		if(this->flags & BKFLAG_2D)
		{
			TBVector2D		scale;

			// PP: By default when you switch to 2D projection mode the object matrix is not applied to vertices.
			// PP: By setting this renderstate to TRUE you can then use the object matrix to scale, rotate and translate drawn geometry even in 2D projection mode.   
			bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);

			bdGetObjectMatrix(tempObjectMatrixCopy);

			// PP: okey, the rectangle to use to draw the book's border and/or background is just the book's 'rect'
			borderRect= &this->rect;

			if(this->flags & BKFLAG_SCREENADJUST)
			{
				// PP: correct for screen dimensions and aspect ratio...

				bmMatIdentity(transMatrix);

				float			vpAspectRatio;
				float			vpCounterScale;

				// PP: Ok, so what we have now is a rectangle with the following properties:
				// PP: Its dimensions are expressed as fractions of the useable height of the target viewport;
				// PP: It is centred around the origin.

				// PP: Lets form a scale matrix to blow this rectangle up to its proper size in pixels...

				// PP: firstly, multiply back up by the height of our current render target...

				scale[X]=(float)videoMode.yScreen;
				scale[Y]=(float)videoMode.yScreen;

				// PP: cool.  Now, apply our 'safe area'...

				if(this->flags & BKFLAG_OLDSAFEAREA)
				{
					scale[X]*=this->oldSafeWidthPortion;
					scale[Y]*=this->oldSafeHeightPortion;
				}
				else if(this->flags & BKFLAG_SAFEAREA)
				{
					scale[X]*=videoMode.safeWidthPortion;
					scale[Y]*=videoMode.safeHeightPortion;
				}

				// PP: That's all fine & dandy if you're in 4:3, but what about other aspect ratios such as 16:9?
				// PP: Adjust for whatever aspect ratio we're currently in.

				scale[X]*=((videoMode.yAspect/(float)videoMode.xAspect)/(videoMode.yScreen/(float)videoMode.xScreen));

				// PP: everything's getting drawn with 3D verts, which get scaled with the viewport, so counteract that scale now to avoid squashing

// PP: REMOUT: SEEMS TO BE FIXED NOW
				// PP: *** TEMPORARY BABEL WORKAROUND ***
				// PP: On PS2, we don't have to unscale for the viewport (PS2 is wrong)
//#if(BPLATFORM!=PS2)  

				scale[X]*=(videoMode.xScreen/CURRENT_VIEWPORT.widthAbs());
				scale[Y]*=(videoMode.yScreen/CURRENT_VIEWPORT.heightAbs());

//#endif// PP: (BPLATFORM!=PS2)


				bmMatScale(screenScaleMatrix, scale[X], scale[Y], 1.0f);

				if(gameStatus.multiplayer.numScreens > 1)
				{
					if(this->screenIndex == BKSCREEN_FULL)
					{
						bmMatIdentity(transMatrix);
					}
					else
					{
						// PP: init offsets
						trans[X]=trans[Y]=0.0f;

						if(gameStatus.multiplayer.splitScreenMode == SPLITSCREEN_HORIZONTAL)
						{
							if(this->screenIndex == BKSCREEN_ONE)
							{
								if(gameStatus.multiplayer.currentScreen == -1)
								{									// PP: if this is a splitscreen book being drawn over the whole screen, gotter offset it to be over the appropriate screen
									trans[Y]+=videoMode.yScreen*videoMode.safeHeightPortion*0.25f;
								}
								else
								{
									trans[Y]+=(-videoMode.yScreen)*(1.0f-videoMode.safeHeightPortion)*0.5f;
								}
							}
							else// PP: _TWO
							{
								if(gameStatus.multiplayer.currentScreen == -1)
								{
									// PP: if this is a splitscreen book being drawn over the whole screen, gotter offset it to be over the appropriate screen
									trans[Y]+=(-videoMode.yScreen)*videoMode.safeHeightPortion*0.25f;
								}
								else
								{
									trans[Y]+=videoMode.yScreen*(1.0f-videoMode.safeHeightPortion)*0.5f;
								}
							}
						}
						else// PP: vertical
						{
							if(this->screenIndex == BKSCREEN_ONE)
							{
								if(gameStatus.multiplayer.currentScreen == -1)
								{
									// PP: if this is a splitscreen book being drawn over the whole screen, gotter offset it to be over the appropriate screen
									trans[X]+=(-videoMode.xScreen)*videoMode.safeWidthPortion*0.25f;
								}
								else
								{
									trans[X]+=videoMode.xScreen*(1.0f-videoMode.safeWidthPortion)*0.5f;
								}
							}
							else// PP: _TWO
							{
								if(gameStatus.multiplayer.currentScreen == -1)
								{
									// PP: if this is a splitscreen book being drawn over the whole screen, gotter offset it to be over the appropriate screen
									trans[X]+=videoMode.xScreen*videoMode.safeWidthPortion*0.25f;
								}
								else
								{
									trans[X]+=(-videoMode.xScreen)*(1.0f-videoMode.safeWidthPortion)*0.5f;
								}
							}
						}
						
						bmMatTranslate(transMatrix, trans[X], trans[Y], 0.0f);
					}
				}

				bmMatMultiply(tempMatrix, tempObjectMatrixCopy, screenScaleMatrix);

				bmMatMultiply(finalMatrix, transMatrix, tempMatrix);

#ifndef TEXTFX_NO_STEREO_ADJUST
				if(this->flags & BKFLAG_STEREOADJUST)
				{
					if(VFX_effectActive(VFX_STEREO))
					{
						if(!g_overrideStereoscopy)
						{
							// PP: offset the book (and thus its contents) for stereoscopic effect...

							// PP: TODO?: bypass all the matrix multiplies and just adjust the x translation component?  Is that safe?

							TBMatrix	stTransMat;

							bmMatTranslate(stTransMat, (VFX_stereoEye?(-1.0f):(1.0f))*BOOK_STEREO_ADJUST*videoMode.yScreen, 0.0f, 0.0f);
							bmMatMultiply(finalMatrix, finalMatrix, stTransMat);
						}
					}
				}
#endif// PP: ndef TEXTFX_NO_STEREO_ADJUST

				bdSetObjectMatrix(finalMatrix);
			}
			else
			{
				if(this->flags & BKFLAG_TURNEDONSIDE)
				{
#if(BPLATFORM == PS2)
					// PP: *** TEMPORARY BABEL WORKAROUND (ps2 is wrong) ***

					scale[X]=this->bookRect.width()*(BOOK::PS2BabelWorkaround_renderTargetHeight/this->rect.width());
					scale[Y]=this->bookRect.height()*(BOOK::PS2BabelWorkaround_renderTargetWidth/this->rect.height());

#else// PP: if(BPLATFORM == PS2) else

					// PP: wow, cool, this works - nice & simple too
					scale[X]=this->bookRect.width()*(videoMode.yScreen/this->rect.width());
					scale[Y]=this->bookRect.height()*(videoMode.xScreen/this->rect.height());

#endif// PP: (BPLATFORM == PS2)

					// PP: form the scale matrix
					bmMatScale(screenScaleMatrix, scale[X], scale[Y], 1.0f);

					// PP: form the rotation matrix
					bmMatZRotation(rotMatrix, HALFPI);

					// PP: multiply scale matrix & rotation matrix
					bmMatMultiply(finalMatrix, rotMatrix, screenScaleMatrix);

					// PP: and set resultant matrix as the object matrix
					bdSetObjectMatrix(finalMatrix);
				}
				else
				{
					// PP: *** TEMPORARY BABEL WORKAROUND (ps2 is wrong) ***

#if(BPLATFORM == PS2)
					// PP: wow, cool, this works - nice & simple too
					scale[X]=this->bookRect.width()*(BOOK::PS2BabelWorkaround_renderTargetWidth/this->rect.width());
					scale[Y]=this->bookRect.height()*(BOOK::PS2BabelWorkaround_renderTargetHeight/this->rect.height());
#else// PP: not ps2
					// PP: wow, cool, this works - nice & simple too
					scale[X]=this->bookRect.width()*(videoMode.xScreen/this->rect.width());
					scale[Y]=this->bookRect.height()*(videoMode.yScreen/this->rect.height());
#endif// PP: not ps2

					// PP: form the scale matrix
					bmMatScale(screenScaleMatrix, scale[X], scale[Y], 1.0f);

					// PP: and set it as the object matrix
					bdSetObjectMatrix(screenScaleMatrix);
				}
			}

			// PP: TEMP TEST - show book rect
			// PP: (this->rect*0.95f).draw(BLUE, OUTLINE);
		}
		else// PP: top level but not 2d
		{
			TBMatrix	bookScaleMatrix;

			bdGetObjectMatrix(tempObjectMatrixCopy);
			bmMatScale(bookScaleMatrix, this->rect.width(), this->rect.height(), 1.0f);
			bmMatMultiply(finalMatrix, tempObjectMatrixCopy, bookScaleMatrix);
			bdSetObjectMatrix(finalMatrix);
		}
	}


	// PP: draw background & border, now that the object matrix is set...

	if(borderRect == NULL)
	{
		// PP: dokey, the rectangle to use to draw the book's border and/or background is the book's 'optRect' moved to the origin
		borderRect = &(tempRect=this->optRect.toOrigin());// PP: must assign to non-temporary rect!
	}
}



/*	--------------------------------------------------------------------------------
	Function 	: BOOK::mainDraw
	Purpose 	: the core of the draw method - actually draw the book
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void BOOK::mainDraw(void)
{
//	if(this->flags & BKFLAG_CHANGINGPAGE)
	// PP: TODO: some better way of determining if each page is to be drawn or not
	{
		if(this->oldPage != NULL)
		{
			if(!(this->oldPage->flags & PAGEFLAG_CLOSED))
			{
//				bkPrintf("BOOK::mainDraw: oldPage %x flags %d\n", this->oldPage, this->oldPage->flags);
				this->oldPage->draw();
			}
		}
	}

	if(this->currentPage != NULL)
	{
		// PP: TEST
		if((~this->currentPage->flags) & PAGEFLAG_CLOSED)
		{
			this->currentPage->draw();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setLanguage
	Purpose 	: set the language for the book
	Parameters 	: new language for the book (BLANGUAGEID_...)
	Returns 	: 
	Info 		: this overrides PAGEITEM::setLanguage, to recur the language change through the book's contents
*/
void BOOK::setLanguage(const EBLanguageID languageIn)
{
	this->language=languageIn;

	// PP: update language of all pages...

	for(int p=this->getNumPages()-1; p>=0; p--)
	{
		this->getPage(p)->setLanguage(languageIn);
	}

	// PP: keep it simple for just now: realign immediately
	// PP: REMOUT: language change will get detected & responded to in BOOK::update		this->realign();
}


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
struct TAG_PAGEITEM* BOOK::setEffectChooser(const struct PIECHOOSER* const chooser)
{
	this->effectChooser=chooser;

	PAGE*	page;

	for(int p=this->getNumPages()-1; p>=0; p--)
	{
		page=this->getPage(p);

		if(page->getEffectChooser() == NULL)
		{
			page->setEffectChooser(this->effectChooser);
		}
	}

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setAspectRatio
	Purpose 	: set the aspect ratio of the book
	Parameters 	: x dimension, y dimension
	Returns 	: 
	Info 		: PLEASE DON'T USE THIS UNLESS YOU KNOW WHAT YOU'RE DOING
*/
void BOOK::setAspectRatio(const float xAspect, const float yAspect)
{
	this->aspectRatio=(xAspect/yAspect);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setRect
	Purpose 	: set the book's rectangle (and update its contents accordingly)
	Parameters 	: rectangle (as portion of target viewport), (opt/BKSCREEN_NOCHANGE)screen index(BKSCREEN_...)
	Returns 	: this
	Info 		: 
*/
BOOK* BOOK::setRect(const RECTANGLE& rectIn, const int screenIndexIn)
{
	this->bookRect=rectIn;

	// PP: set screen index for book
	if(screenIndexIn != BKSCREEN_NOCHANGE)
	{
		this->screenIndex=screenIndexIn;
	}

	// PP: *********
	// PP: moved all other stuff to align()
	// PP: *********

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
PAGE* BOOK::addPage(PageFunc const update, const char* const pageName, PageFunc const open, PageFunc const close)
{
	PAGE*	newPage;

	newPage=new PAGE(pageName, update, open, close);

	// PP: if the book isn't top level, nor are its pages
	if((~this->flags) & BKFLAG_TOPLEVELBOOK)
	{
		newPage->flags &= ~PAGEFLAG_TOPLEVELPAGE;
	}

	// PP: newPage->rect=this->rect;
// PP: REMOUT: BOOK::align will get called by BOOK::open		newPage->setRect(this->rect);// PP: top level pages have setRect called on them at the insertion stage;

	// PP: set the page's language to be this book's language.
	newPage->setLanguage(this->language);
	
	// NH: TODO - Add a sounds active flag
	if (this->menuSounds != NULL)
		newPage->setSounds(this->menuSounds);

	this->pages.append(newPage);

	if(this->currentPage == NULL)
	{
		// PP: assume it's because this is the first page that's been added
		this->currentPage=newPage;
		this->currentPageIndex=0;
		//this->currentPage->acceptInput=true;
	}

	BOOKCHAPTER*	chapter;

	chapter=this->getLatestChapter();

	if(chapter != NULL)
	{
		// PP: set the new page as the start page for the latest chapter
		if(chapter->startPage == (-1))
		{
			chapter->startPage=this->getNumPages()-1;
		}

		// PP: set the new page as the last page of the latest chapter
		chapter->endPage=this->getNumPages()-1;
	}
	
	return newPage;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::addPage
	Purpose 	: add a page to the book
	Parameters 	: name of page, (opt/NULL)custom update function ptr, 
					(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
	Returns 	: ptr to page
	Info 		: 
*/
PAGE* BOOK::addPage(const char* const pageName, PageFunc const update, PageFunc const open, PageFunc const close)
{
	return(this->addPage(update, pageName, open, close));
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::addPage
	Purpose 	: add a page to the book
	Parameters 	: page item to insert INTO the new page, (opt/NULL)custom update function ptr, (opt/NULL) name of page,
					(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
	Returns 	: ptr to page
	Info 		: *** NOTE *** 'addPage(page=new PAGE("MYPAGE"))' is generally wrong - instead say 'page=addPage("MYPAGE")'
*/
PAGE* BOOK::addPage(PAGEITEM* const pageItem, PageFunc const update, const char* const pageName, PageFunc const open, PageFunc const close)
{
	PAGE*		page;

	page=this->addPage(update, pageName, open, close);
	
	page->insertItem(pageItem);

	return page;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::addPage
	Purpose 	: add a page to the book
	Parameters 	: index of string to insert, (opt/NULL)custom update function ptr, (opt/NULL) name of page,
					(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
	Returns 	: ptr to page
	Info 		: 
*/
PAGE* BOOK::addPage(const EStringIndex stringIndex, PageFunc const update, const char* const pageName, PageFunc const open, PageFunc const close)
{
	PAGE*		page;

	page=this->addPage(update, pageName, open, close);
	
	page->insertItem(stringIndex);

	return page;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::removePage
	Purpose 	: remove a page from the book
	Parameters 	: ptr to page to remove, (opt/false)whether or not to delete the page as well as removing it
	Returns 	: ptr to page, or NULL if deleted
	Info 		: 
*/
PAGE* BOOK::removePage(PAGE* const page, const bool deletePage)
{
	if(page == NULL)		// PP: page not found
	{
		return NULL;
	}
	else
	{
		this->pages.remove(page);

		if(deletePage)
		{
			delete page;
		}

		if(this->currentPage == page)
		{
			// PP: if the current page index is now off the end of the list...
			if(this->currentPageIndex >= BOOK_NUM_PAGES)
			{
				// PP: ...decrease it
				this->currentPageIndex--;
			}

			// PP: get a pointer to the new current page
			this->currentPage=this->getPage(this->currentPageIndex);
		}

		if(this->oldPage == page)
		{
			this->oldPage=NULL;
		}

		if(deletePage)
		{
			return NULL;
		}
		else
		{
			return page;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getItem
	Purpose 	: get a pointer to an item in the book
	Parameters 	: name of the item
	Returns 	: ptr to the specified item, or NULL if not found
	Info 		: I should have bought more cheese - this St. Paulin is delightful.  Sorry did I type that or just think it?  Oh God, now I'm typing EVERYTHING I think!  Shit what am I going to do?!  HELP!  SOMEBODY HELP!  They can't hear me!  Oh this is terrible.  I'll have to email for help.  Now what was that address again...ah this is it - 'help@type-what-you-think.com'
*/
PAGEITEM* BOOK::getItem(const char* const itemName)
{
	PAGE*			page;
	PAGEITEM*		item;

	// PP: look in the first page first - that's most likely where it is...
	page=this->getPage(0);

	item=page->getItem(itemName);

	if(item != NULL)
	{
		return item;
	}

	// PP: OK, it wasn't there - try the current page if that's not the first page that we just tested...
	if(this->currentPageIndex != 0)
	{
		page=this->currentPage;

		item=page->getItem(itemName);

		if(item != NULL)
		{
			return item;
		}
	}

	// PP: Right then, lets look at all the pages apart from the ones we've already tested...
	// PP: you asked for it!
	// PP: I'm going to give you a DAMN GOOD THRASHING!!
	for(int p=BOOK_NUM_PAGES-1; p>0; p--)
	{
		if(p==this->currentPageIndex) continue;

		page=BOOK_GET_PAGE(p);

		item=page->getItem(itemName);

		if(item != NULL)
		{
			return item;
		}
	}

	// PP: nope it ain't here - hope you're null-checking this
	return NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getItem
	Purpose 	: get a pointer to the item containing a specified screen position ON THE CURRENT PAGE, or NULL if there isn't one at that position
	Parameters 	: x position, y position
	Returns 	: a pointer to the item containing a specified screen position ON THE CURRENT PAGE, or NULL if there isn't one at that position
	Info 		: this assumes that ye're drawing the book in 2D mode and with an identity object matrix
					this overrides PAGEITEM::getItem(x, y)
*/
PAGEITEM* BOOK::getItem(const float xPos, const float yPos)
{
	// PP: (I'm assuming this method is being used to pick items with the mouse)
	// PP: Can't pick an unselectable item
	if(!this->selectable())
	{
		return NULL;
	}

	// PP: if the book isn't visible, it can't be showing an item at the specified position// PP: also if it's closing, we shouldn't be able to click on anything in it
	if(!(this->flags&BKFLAG_OPEN))
	{
		return NULL;
	}

	// PP: TODO?: a flag to say "yes you can click on me now" would be real handy, especially when new transitions types come along

	// PP: if we're changing page, we can select an item on the TARGET page

	if(this->currentPage == NULL)
	{
		return NULL;
	}
	else
	{
		return this->currentPage->getItem(xPos, yPos);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getTextbox
	Purpose 	: look within book's CURRENT page for a textbox holding a specified string
	Parameters 	: index of string to search for
	Returns 	: a pointer to the first textbox holding the specified string (or NULL if not found)
	Info 		: // PP: this overrides PAGEITEM::getTextbox
					// PP: Edam is a much better 'melting' cheese than Cheddar.  It's got a nicer texture and a nicer taste.  Try it grated in a toastie - you won't be disappointed!
*/
struct TAG_TEXTBOX* BOOK::getTextbox(const EStringIndex string)
{
	if(this->currentPage == NULL)
	{
		return NULL;
	}
	else
	{
		return this->currentPage->getTextbox(string);
	}
}


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
TAG_PAGEITEM* BOOK::getNavigationCandidate(NAVCANDIDATE_INFO& info)
{
	if(!this->selectable())
	{
		return NULL;
	}

	if(this->currentPage == NULL)
	{
		return NULL;
	}
	else
	{
		return(this->currentPage->getNavigationCandidate(info));
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::nextPage
	Purpose 	: turn to the next page (page at current page index + 1)
	Parameters 	: (opt/false)change page instantly
	Returns 	: ptr to current page (after the page change)
	Info 		: 
*/
PAGE* BOOK::nextPage(const bool instant)
{
	int newPageIndex=LOOPCONSTRAIN(this->currentPageIndex+1, 0, BOOK_NUM_PAGES-1);

	this->gotoPageNumber(newPageIndex, instant);

	return this->currentPage;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::prevPage
	Purpose 	: turn to the previous page (page at current page index - 1)
	Parameters 	: (opt/false)change page instantly
	Returns 	: ptr to current page (after the page change)
	Info 		: 
*/
PAGE* BOOK::prevPage(const bool instant)
{
	int newPageIndex=LOOPCONSTRAIN(this->currentPageIndex-1, 0, BOOK_NUM_PAGES-1);

	this->gotoPageNumber(newPageIndex, instant);

	return this->currentPage;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::gotoPage
	Purpose 	: go to the page with the specified name
	Parameters 	: name of page to turn to, whether or not to change page instanly
	Returns 	: 
	Info 		: 
*/
void BOOK::gotoPage(const char* const pageName, const bool instant)
{
	// PP: I've seen people calling this gotoPage every frame, so check we're not already on the specified page
	if(this->currentPage != NULL)
	{
		if(stricmp(pageName, this->currentPage->name) == 0)
		{
			return;
		}
	}

	this->gotoPageNumber(this->findPage(pageName), instant);
}


/*	--------------------------------------------------------------------------------
	Function 	: gotoPageNumer
	Purpose 	: go to the page at the specified index
	Parameters 	: page number to turn to, whether or not to change page instantly
	Returns 	: 
	Info 		: page numbers are zero-based
*/
bool BOOK::gotoPageNumber(const int index, const bool instant)
{
	// PP: check validity of target page number...
	
	if(index == this->currentPageIndex)// PP: this is one of the mistakes people seem to make with books - calling gotoPage every frame!
	{
	// PP: 	this->currentPage->open();
		return true;
	}

	if(index < 0) return false;
	if(index >= this->getNumPages()) return false;

	if(this->restrictingChapter != NULL)
	{
		if(index<this->restrictingChapter->startPage) return false;
		if(index>this->restrictingChapter->endPage) return false;
	}

	this->oldPage=this->currentPage;
	this->oldPage->acceptInput=false;
	this->currentPage=BOOK_GET_PAGE(index);
	this->currentPage->acceptInput=false;
	this->currentPageIndex=index;

	// PP: NEW TEST
	if((~this->flags & BKFLAG_OPEN) || this->exited)
	{
		this->currentPage->open(true);
	}
	else
	{
		this->currentPage->open(instant);
	}

	if(this->oldPage != NULL)
	{
		// PP: NEW TEST
		if((~this->flags & BKFLAG_OPEN) || this->exited)
		{
			this->oldPage->close(true);
		}
		else
		{
			this->oldPage->close(instant);
		}
	}

	// PP: update before drawing
// PP: REMOUT: hmmm stack overflows				this->update();

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::changeMode
	Purpose 	: change the mode of the page, eg. size
	Parameters 	: new mode to change to, (opt/0=don't set) duration of transition
	Returns 	: 
	Info 		: 
*/
void BOOK::changeMode(const int newMode, const float durationIn)
{
	if(newMode == this->currentMode)
	{
		return;
	}

	this->oldMode=this->currentMode;
	this->currentMode=newMode;
	this->flags|=BKFLAG_REMODING;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::update
	Purpose 	: update the book and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::update
*/

#define PULL_OUT_HAIR_AND_SCREAM(_str)				bkPrintf(_str)

// PP: slack used when comparing current aspect ratio with book's aspect ratio, to determine if the book needs to be realigned
#define BOOK_SCREENADJUST_ASPECTRATIO_THRESHOLD		0.001f

void BOOK::update(void)
{
	if(this==NULL)
	{
		return;
	}

	if(!(this->flags&(BKFLAG_OPEN|BKFLAG_PLEASEOPEN|BKFLAG_CLOSING)))
	{
		// PP: book isn't open in any way - no need to update
		return;
	}

	if(this->flags & BKFLAG_PLEASEOPEN)
	{
		// PP: created in open state:
		// PP: this is the first update, so open the book, instantly
		this->open(NULL, true);
	}

	// PP: base-class update (including effects update)
	PAGEITEM::update();	

	if(this->flags & BKFLAG_TOPLEVELBOOK)
	{
		if(this->flags & BKFLAG_BOOKLIST)
		{
			if(!this->onBookList)
			{
				// PP: add the book to the book list
				BookList_add(this);

				this->onBookList=true;
			}
		}

		// PP: update langauge if nec
		if(this->language != gameStatus.currentLanguage)// PP: book's language is out of date
		{
			// PP: update the book's language
			this->setLanguage(gameStatus.currentLanguage);

			// PP: mmm, better realign I suppose
			this->realign();
		}

		if(this->flags & BKFLAG_SCREENADJUST)
		{
			if(bmFAbs((videoMode.xAspect/(float)videoMode.yAspect)-this->aspectRatio) > BOOK_SCREENADJUST_ASPECTRATIO_THRESHOLD)
			{
				// PP: if the aspect ratio of the screen has changed, we'd better realign the book ja?...

				bkPrintf("%f != %f, realigning\n", (videoMode.xAspect/(float)videoMode.yAspect), this->aspectRatio);

				this->realign();
			}
			else if(gameStatus.multiplayer.numScreens != this->numScreens)
			{
				// PP: if the number of screens has changed, we'd better realign the book ja?...
				this->realign();
			}
			else if(gameStatus.multiplayer.numScreens > 1)
			{
				// PP: if the split-screen type has changed, we'd better realign the book ja?...

				if(gameStatus.multiplayer.splitScreenMode != this->splitScreenMode)
				{
					this->realign();
				}
			}
		}
	}

	// PP: TODO: some nice way of determining if each page is to be updated// PP: This is fine for just now though :)
	if(this->oldPage != NULL)
	{
		if((~this->oldPage->flags) & PAGEFLAG_CLOSED)
		{
			this->oldPage->update();
		}
	}

	bool	bookClosed;

	if(this->currentPage == NULL)
	{
		// PP: NO CURRENT PAGE - BOOK IS CLOSED
		this->flags &= ~BKFLAG_OPEN;

		bookClosed=true;
	}
	else
	{
		// PP: NOTE: this 'acceptInput' lark is probably OOD by now
		if(this->flags & BKFLAG_OPEN)
		{
			this->currentPage->acceptInput= !BOOK_IN_TRANSITION;
		}
		else
		{
			// PP: TEST
			this->currentPage->acceptInput=false;
		}


		if(this->flags & BKFLAG_CLOSING)
		{
			if(this->currentPage->hasEffects())
			{
				bookClosed=this->effectsFinished();
			}
			else
			{
				bookClosed=PAGEITEM::effectsFinished();
			}
		}
		else
		{
			bookClosed=false;
		}

		if(!bookClosed)
		{
			this->currentPage->update();

			// PP: there are some cases where that there update will have set our currentPage to NULL, so we gotter check
			if(this->currentPage != NULL)
			{
				// PP: don't trigger navigation if this is the first update after being opened (debounce button values won't have had a chance to reset)
				if((~this->flags) & BKFLAG_FIRSTUPDATE)
				{
					// PP: don't trigger navigation if the book is opening or closing!!
					if((~this->flags) & BKFLAG_CLOSING)
					{
						// PP: trigger navigation of current page

						this->currentPage->respondToNavigation();
					}
				}
			}
		}
	}

	// PP: if(this->currentPage->flags & PAGEFLAG_CLOSED)
	if(bookClosed)// PP: NEW
	{
		// PP: alrighty, nothing left to draw, book is really closed now
		BOOK_END_TRANSITION;
		this->flags &= ~BKFLAG_OPEN;

		// PP: NOW'D BE AN AWFULLY GOOD TIME TO CHECK FOR FREE-WHEN-CLOSED AND SHUTDOWN-WHEN-CLOSED...

		if(this->flags & BKFLAG_FREEWHENCLOSED)
		{
			this->flags &= ~BKFLAG_FREEWHENCLOSED;// PP: inconsequential?

			// PP: delete the book
			// PP: it will disappear and remove itself from the book list
			delete(this);
			return;
		}
		else if(this->flags & BKFLAG_SHUTDOWNWHENCLOSED)
		{
			this->flags &= ~BKFLAG_SHUTDOWNWHENCLOSED;// PP: inconsequential?

			// PP: shut-down the book
			// PP: it will disappear and remove itself from the book list
			this->shutdown();
			return;
		}
		else
		{
			// PP: NEW: remove the book from the booklist when it finishes closing
			BookList_remove(this);

			// PP: also free up temporary memory used by the book, eg. expired page-item effects
			this->freeTempMemory();
		}
	}

	if(this->closeDelay > 0.0f)								// PP: if we still have some delay remaining before a close is due...
	{													
		this->closeDelay -= fTime;							// PP: ...just tick it down...
														
		if(this->closeDelay <= 0.0f)						// PP: ...and if that ticks it down to zero...
		{
			this->close(0.0f, this->closeInstantly);		// PP: ...close now!
		}
	}

	// PP: clear first-update flag
	this->flags &= ~BKFLAG_FIRSTUPDATE;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::drawscreenRects
	Purpose 	: show the screen rects of all the items on the current page of the book
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::drawScreenRects
*/
void BOOK::drawScreenRects(void)
{
	if(fmodf(gameStatus.appTime, 0.4f)>0.2f)
	{
		if(this->currentPage != NULL)
		{
			bdSetIdentityObjectMatrix();

			this->currentPage->drawScreenRects();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::mainDrawDebug
	Purpose 	: the core of the debug-draw method - actually debug-draw the book and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDrawDebug
*/
void BOOK::mainDrawDebug(void)
{
	RECTANGLE					rect;
	COLOUR						colour=COLOUR(0, 0, 255);
	TBFontFormatting			format;							// PP: formatting info
	static ushort				string16[128];
	static char					string8[128];

	// PP: display current page number

	TextFX_setColour(colour);

	format.x =			this->rect.left;
	format.y =			this->rect.top;
	format.height =		this->rect.height();
	format.width =		this->rect.width();
	format.horzFormat = EBFONTFORMATTING_CENTRE;
	format.vertFormat = EBFONTFORMATTING_BOTTOM;
	format.wrap=0;
	format.lineSpace = 0;

	sprintf(string8, "Page %d of %d", this->currentPageIndex+1, BOOK_NUM_PAGES);

	// PP: convert the final text into wide characters
	bkString8to16(string16, string8);

	// PP: print the text
//	bdPrintFont(standardFont,string16,strlen(string8),&format,0,0,0,200,&TextFX_escapeCallback,NULL);

	TextFX_resetColour();


	RECTANGLE					tempRect;

	tempRect=this->rect.toOrigin();

	tempRect.draw(colour, OUTLINE);

	// PP: draw debug data for the book's current page

	if(this->currentPage != NULL)
	{
		this->currentPage->drawDebug();
	}

	// PP: draw screen rects why not - this sets an identity object matrix, so don't draw nuffin after it a'ight?
	this->drawScreenRects();
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getRequiredHeight
	Purpose 	: get the minimum required height of the book
	Parameters 	: 
	Returns 	: minimum required height of book
	Info 		: 
*/
float BOOK::getRequiredHeight(void)
{
	if(this->pageItemFlags & PIFLAG_HEIGHTCALCULATED)
	{
		return this->requiredHeight;
	}

	PAGE*		page;

	this->requiredHeight=0.0f;

	bkPrintf("BOOK::getRequiredHeight (%s)\n", this->name);

	for(int p=BOOK_NUM_PAGES-1;p>=0;p--)
	{
		page=BOOK_GET_PAGE(p);
		this->requiredHeight=MAX(this->requiredHeight, page->getRequiredHeight());
	}

	this->pageItemFlags |= PIFLAG_HEIGHTCALCULATED;

	return this->requiredHeight;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getRequiredWidth
	Purpose 	: get the minimum required width of the book
	Parameters 	: 
	Returns 	: minimum required width of book
	Info 		: *** NEW TEST *** Hmmmm
*/
float BOOK::getRequiredWidth(void)
{
	if(this->pageItemFlags & PIFLAG_WIDTHCALCULATED)
	{
		return this->requiredWidth;
	}

	PAGE*		page;

	this->requiredWidth=0.0f;

	bkPrintf("BOOK::getRequiredWidth (%s)\n", this->name);

	for(int p=BOOK_NUM_PAGES-1;p>=0;p--)
	{
		page=BOOK_GET_PAGE(p);
		this->requiredWidth=MAX(this->requiredWidth, page->getRequiredWidth());
	}

	this->pageItemFlags |= PIFLAG_WIDTHCALCULATED;

	return this->requiredWidth;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getSelectedItem
	Purpose 	: get the selected item of the book's current page
	Parameters 	: 
	Returns 	: the selected item of the book's current page, or NULL if N/A
	Info 		: // PP: TODO: add a select/don't select paramter if it's needed
*/
PAGEITEM* BOOK::getSelectedItem(void)
{
	PAGE*	page;

	page=this->getCurrentPage();

	if(page == NULL) return NULL;

	if(page->selectedItem == NULL)
	{
		return page->selectItem();
	}
	else
	{
		return page->selectedItem;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::realign
	Purpose 	: realign items in the book
	Parameters 	: 
	Returns 	: 
	Info 		: COSTLY!
					Also propogates book's effect chooser ptr down the hierarchy of contents
*/
void BOOK::realign(void)
{
	// PP: NEW: realign is now only called to FORCE realignment of ALL PAGES
	this->markForRealignment();

	// PP: REMOUT: content realignment is now done seperately		this->setRect(this->bookRect);
	this->align();

	// PP: Let's say someone has added pages to a book while the book's open,
	// PP: and consequently has to realign the book.
	// PP: The new pages they've added will have null effect choosers because
	// PP: they will have missed the BOOK::open that propogated the book's
	// PP: chooser down the contents hierarchy.
	// PP: So we'd better fix things here I suppose.
	// PP: I'm not happy about it though.
	// PP: On the next game I'm going to litter my code with ASSERTs
	this->setEffectChooser(this->effectChooser);
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::shutdown
	Purpose 	: tidy up this mess!  Your tea's almost ready!
	Parameters 	: X-Wings, Action Men, small die-cast sports cars, all back into the box.
	Returns 	: 
	Info 		: Aw Mum, do I have to?!  -Yes!  Do you want beans or gravy?!  -Gravy!  And beans!
					// PP: this overrides PAGEITEM::shutdown
*/
void BOOK::shutdown(void)
{
/* PP: REMOUT: sometimes you need to be able to shut-down a book straight away, because its resources (eg. icon textures are going to become unavailable)

  #ifndef INFOGRAMES
	if(this->flags & BKFLAG_BOOKLIST)
	{
		if(~this->flags & BKFLAG_SHUTDOWNWHENCLOSED)// PP: unless this is being deleted by the Book system
		{
			// PP: cause a crash if someone is trying to shut-down the book
			ASSERTM(false, "PP: Instead of shutting-down this book, please pass it into BookList_closeAndShutdown, which will close it gracefully and then free it for you.\n");
		}
	}
#endif// PP: ndef INFOGRAMES

  */

	if(BabelHasShutdown)
	{
		return;
	}
	
	PAGEITEM::shutdown();

	this->close(0.0f, true);
	this->pages.kill();				// PP: kill pages
	this->chapters.kill();			// PP: kill chapters
	this->oldPage=NULL;
	this->currentPage=NULL;
	
	if (this->menuSounds)
		delete this->menuSounds;

	if(this->onBookList)
	{
		BookList_remove(this);
	}

	this->flags = 0;

	this->menuSounds=NULL;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::~TAG_BOOK
	Purpose 	: destructor; calls shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
BOOK::~TAG_BOOK(void)
{
/* PP: REMOUT: sometimes you need to be able to shut-down a book straight away, because its resources (eg. icon textures are going to become unavailable)

#ifndef INFOGRAMES
	if(this->flags & BKFLAG_BOOKLIST)
	{
		if(~this->flags & BKFLAG_FREEWHENCLOSED)// PP: unless this is being deleted by the Book system
		{
			// PP: cause a crash if someone is trying to delete the book
			ASSERTM(false, "PP: Instead of deleting this book, please pass it into BookList_closeAndFree, which will close it gracefully and then free it for you.\n");
		}
	}
#endif// PP: ndef INFOGRAMES
*/

	this->shutdown();
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setName
	Purpose 	: set the name of the book
	Parameters 	: the name of the book!
	Returns 	: (PAGEITEM*)this
	Info 		: this overrides PAGEITEM::setName
*/
PAGEITEM* BOOK::setName(const char* const nameIn)
{
	PAGEITEM::setName(nameIn);

	this->pages.setName("%s pages", this->name);

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setNavigation
	Purpose 	: enable/disable navigation for all the pages in the book
	Parameters 	: true to enable / false to disable
	Returns 	: this
	Info 		: 
*/
BOOK* BOOK::setNavigation(const bool on)
{
	PAGE*	page;

	for(int p=BOOK_NUM_PAGES-1; p>=0; p--)
	{
	
		
		page=BOOK_GET_PAGE(p);

		page->setNavigation(on);
	}

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setSounds
	Purpose 	: set the sounds used by the book
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BOOK::setSounds(MENU_SOUNDS *menuSounds)
{
	this->menuSounds = menuSounds;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setController
	Purpose 	: Sets the controller able to navigate the page
	Parameters 	: logical index of the controller (ie. player index): zero-based
	Returns 	: 
	Info 		: -1 = global; 0 = pad 1; 1 = pad 2
*/
void BOOK::setController(const char con)
{
	// PP: Not sure this is just the *best* way of doing things, but at least the function's here...

	for(int p=this->getNumPages()-1; p>=0; p--)
	{
		this->getPage(p)->setController(con);
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setAutoDraw
	Purpose 	: Sets autoDraw. If set to false (true by default), when the book is in the book list it will NOT automatically draw
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
BOOK* BOOK::setAutoDraw(const bool on)
{
	this->autoDraw = on;

	return this;
}

/*	--------------------------------------------------------------------------------
	Function 	: BOOK::getAutoDraw
	Purpose 	: returns a flag to show if we want to automatically draw the book or not
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
bool BOOK::getAutoDraw(void)
{
	return this->autoDraw;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::setOldSafeArea
	Purpose 	: specify the safe area that was in effect when the book was being designed pre-BKFLAG_SAFEAREA
	Parameters 	: old safe width portion, old safe height portion
	Returns 	: 
	Info 		: // PP: dirty fix for pre-BKFLAG_SAFEAREA books that have to line up with splash screens
					// PP: NOTE: When creating NEW books that have to line up with splash screens, clear BKFLAG_SAFEAREA!!!!
					// PP: REMOVE THIS METHOD AFTER TAZ
*/
struct TAG_BOOK* BOOK::setOldSafeArea(const float oldSafeWidthPortionIn, const float oldSafeHeightPortionIn)
{
	this->flags |= BKFLAG_OLDSAFEAREA;

	// PP: just in case you forgot
	this->flags &= ~BKFLAG_SAFEAREA;

	this->oldSafeWidthPortion=oldSafeWidthPortionIn;
	this->oldSafeHeightPortion=oldSafeHeightPortionIn;

	return this;
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::updateBox
	Purpose 	: update the box to be used to draw the book's background and/or border
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::updateBox.
*/
void BOOK::updateBox(void)
{
	if((~this->flags) & BKFLAG_TOPLEVELBOOK)// PP: if it's a sub-book
	{
		// PP: it's just a normal page item really
		PAGEITEM::updateBox();

		return;
	}

	RECTANGLE	rect2;
	RECTANGLE	optRect2;

	rect2=this->rect;
	optRect2=this->optRect;

	if(this->tightBoxX)
	{
		this->boxRect.left=optRect2.left;
		this->boxRect.right=optRect2.right;
	}
	else
	{
		this->boxRect.left=rect2.left;
		this->boxRect.right=rect2.right;
	}

	if(this->tightBoxY)
	{
		this->boxRect.bottom=optRect2.bottom;
		this->boxRect.top=optRect2.top;
	}
	else
	{
		this->boxRect.bottom=rect2.bottom;
		this->boxRect.top=rect2.top;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::markForRealignment
	Purpose 	: tell the pageitem that its alignment is out of date and will need
						to be updated in next align()
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGITEM::markForRealignment
					// PP: this gets recurred down the contents hierarchy
*/
void BOOK::markForRealignment(void)
{
	PAGEITEM::markForRealignment();

	// PP: pass the call onto all the book's pages...

	for(int i=this->getNumPages()-1; i>=0; i--)
	{
		this->getPage(i)->markForRealignment();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BOOK::align
	Purpose 	: calculate alignment for the book and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGEITEM::align
					// PP: this gets recurred down the contents hierarchy
*/
void BOOK::align(void)
{
	if((~this->pageItemFlags) & PIFLAG_ALIGNED)
	{
		// PP: base class alignment
		// PP: REMOUT: don't think so		PAGEITEM::align();

		// PP: ***** BROUGHT IN FROM BOOK::SETRECT...

		if((this->flags & BKFLAG_TOPLEVELBOOK) && (this->flags & BKFLAG_2D))
		{	
			float	newWidth;

			if(this->flags & BKFLAG_SCREENADJUST)
			{
				/* PP: REMOUT: back in setRect

				// PP: set screen index for book
				if(screenIndexIn != BKSCREEN_NOCHANGE)
				{
					this->screenIndex=screenIndexIn;
				}
				*/

				// PP: adjust for the aspect ratio of the screen...

				float	x,y;// PP: test

				x=videoMode.xAspect;
				y=videoMode.yAspect;

				// PP: record current aspect ratio so's we can trigger auto-realignment if that should change
				// PP: REMOUT: now recorded in align()
				this->aspectRatio=x/y;

				// PP: this'ere realAspectRatio takes into account differing X&Y safe portions, and will determine the shape of the book's react that it'll use to align its items
				//float	realAspectRatio=(x*SAFE_SCREEN_WIDTH_PORTION)/(y*SAFE_SCREEN_HEIGHT_PORTION);
			}

			// PP: calculate the width of the book (for alignment purposes) based on the width passed in and the aspect ratio
			newWidth=this->bookRect.width()*this->aspectRatio;

			this->rect=this->bookRect;

			// PP: form the rectangle of the book (for alignment purposes) using that new width we just calculated
			this->rect=RECTANGLE(this->rect.midX()-(newWidth*0.5f), this->rect.midX()+(newWidth*0.5f), this->rect.bottom, this->rect.top);

			if(this->flags & BKFLAG_SCREENADJUST)
			{
				// PP: but hang on, that rectangle will have to be halved in some way if we're in a split-screen mode...

				if(gameStatus.multiplayer.numScreens > 1)
				{
					if(gameStatus.multiplayer.splitScreenMode == SPLITSCREEN_HORIZONTAL)
					{
						if(this->screenIndex == 0)
						{
							this->rect=this->rect.topHalf().toOrigin();// PP: moveY(-videoMode.yScreen/4);
						}
						else if(this->screenIndex == 1)
						{
							this->rect=this->rect.bottomHalf().toOrigin();// PP: moveY(videoMode.yScreen/4);
						}
					}
					else// PP: vertical
					{
						if(this->screenIndex == 0)
						{
							this->rect=this->rect.leftHalf().toOrigin();// PP: moveX(videoMode.yScreen/4);
						}
						else if(this->screenIndex == 1)
						{
							this->rect=this->rect.rightHalf().toOrigin();// PP: moveX(-videoMode.yScreen/4);
						}
					}
				}

				this->numScreens=gameStatus.multiplayer.numScreens;

				this->splitScreenMode=gameStatus.multiplayer.splitScreenMode;
			}
		}
		else// PP: if not top-level
		{
			// PP: did say this		this->rect=rectIn;

			// PP: meaning this
			this->rect=this->bookRect;
			// PP: bookRect was set by setRect - confusing, no?
		}

		PAGE*	page;

		// PP: inform all top-level pages of their new rect
			
		for(int p=this->getNumPages()-1;p>=0;p--)
		{
			page=this->getPage(p);

			page->setRect(this->rect);
		}

		// PP: update box for outline/background drawing now that we've determined real rect
		this->updateBox();

// PP: =====================================================================
// PP: REMOUT: REALLY GOTTER PUT THIS BACK IN AND GET IT WORKING RIGHT!!!!!!
// PP: =====================================================================
	//	this->pageItemFlags |= PIFLAG_ALIGNED;
	}

	// PP: ***** THE ABOVE WAS BROUGHT IN FROM BOOK::SETRECT

	// PP: now just tell the current page to align ITS contents

	if(this->currentPage != NULL)
	{
		this->currentPage->align();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: setFullScreenBackground
	Purpose 	: set the colour for the book's full-screen background
	Parameters 	: the colour for the book's full-screen background
	Returns 	: 
	Info 		: 
*/
void BOOK::setFullScreenBackground(const COLOUR& col)
{
	this->flags |= BKFLAG_FULLSCREENBACKGROUND;

	this->setBackgroundColour(col);
}


/*	--------------------------------------------------------------------------------
	Function 	: setFullScreenBackground
	Purpose 	: specify whether or not the book's background is full-screen/full-splitscreen (normally it's not)
	Parameters 	: (opt/true) whether or not the book's background is full-screen/full-splitscreen
	Returns 	: 
	Info 		: 
*/
void BOOK::setFullScreenBackground(const bool fullScreen)
{
	SETFLAG(this->flags, BKFLAG_FULLSCREENBACKGROUND, fullScreen);
}