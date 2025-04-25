// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2001 Blitz Games Ltd.
//
//      File : consoleWindow.cpp
//   Purpose : encapsulated console window to output text to screen
// Component : generic display functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define _BDISPLAY_

#include <babel.h>

// ********************************************************************************
// Constants and Macros

// line width
#define NOOF_CHARS_PER_LINE		1024

// lines
#define NOOF_LINES				40

// formatting
#define FORMATDX				200								// width
#define FORMATDY				200								// height
#define FORMATXP				-(FORMATDX / 2)					// xpos
#define FORMATYP				(FORMATDY / 2)					// ypos


// ********************************************************************************
// Types, structures and classes

typedef class CBConsoleWindow
{
	public:
		static char			cbuf[2048];

		TBFont				*font;

		ushort				*lineBuffer[NOOF_LINES];
		int					freeLineIndex, freeCharIndex;

		int					r,g,b,a;
		int					xp, yp, width, height;
		int					show;
		float				scaler;

		void				Print();
		void				Render();
		void				Clear();
		void				SetFont(TBFont *font, float scaler, int r, int g, int b, int a);
		void				SetRect(int xp, int yp, int width, int height);
		void				Show(int show) {this->show = show;}

							// constructor
							CBConsoleWindow();

							// destructor
							~CBConsoleWindow();

} CBConsoleWindow;

// static class vars
char CBConsoleWindow::cbuf[2048];


// ********************************************************************************
// Globals

// our single console window instance
static CBConsoleWindow		*consoleWindow = NULL;


// ********************************************************************************
// CBConsoleWindow
// ********************************************************************************

/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::CBConsoleWindow
   Purpose : constructor
   Parameters : 
   Returns : 
   Info : 
*/

CBConsoleWindow::CBConsoleWindow()
{
	int	i;


	// allocate our circular line list
	lineBuffer[0] = (ushort *)MALLOCEX(NOOF_LINES * NOOF_CHARS_PER_LINE * sizeof(ushort), (uint32)"ConsoleWindow");
	for (i=1; i<NOOF_LINES; i++)
	{
		lineBuffer[i] = lineBuffer[i-1] + NOOF_CHARS_PER_LINE;
	}

	Clear();

	SetFont(NULL, 1.0f, 127, 127, 127, 127);
	SetRect(FORMATXP, FORMATYP, FORMATDX, FORMATDY);
	Show(1);
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::~CBConsoleWindow
   Purpose : destructor
   Parameters : 
   Returns : 
   Info : 
*/

CBConsoleWindow::~CBConsoleWindow()
{
	FREE(lineBuffer[0]);
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::Print
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CBConsoleWindow::Print()
{
	int			slash;
	char		*cp, c;
	ushort		*lp;


	if (lineBuffer[0] == NULL)
		return;

	// print string into line buffers
	for (cp = cbuf, slash = 0;;)
	{
		lp = lineBuffer[freeLineIndex];

		c = *cp++;
		if (!c)
		{
			lp[freeCharIndex] = 0;
			break;
		}
		else if (c == '\\')
		{
			slash = 1;
		}
		else if ((c == 'n' && slash) || c == '\n')
		{
			slash = 0;

			lp[freeCharIndex] = 0;
			freeCharIndex = 0;

			freeLineIndex++;
			if (freeLineIndex >= NOOF_LINES)
			{
				freeLineIndex = 0;
			}
		}
		else
		{
			slash = 0;

			if (freeCharIndex < (NOOF_CHARS_PER_LINE-1))
			{
				lp[freeCharIndex] = (ushort)c;
				freeCharIndex++;
			}
		}
	}
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::Render
   Purpose : render text to screen
   Parameters : 
   Returns : 
   Info : 
*/

void CBConsoleWindow::Render()
{
	int			loop, noof, index, len, fontHeight;
	float		x, y, s, invs, h;
	TBMatrix	mat;


	if (font == NULL || lineBuffer[0] == NULL)
		return;

	bdPushProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	bdPushRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE);
	bdPushRenderState(BDRENDERSTATE_2DWORLDMATRIX, TRUE);

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND);

	// scale
	s = scaler;
	if (s == 0.0f)
		s = 1.0f;
	invs = 1.0f / s;

	bmMatScale(mat, s, s, 1.0f);
	bdSetObjectMatrix(mat);

	x = invs * (float)xp;
	y = invs * (float)yp;
	h = invs * (float)height;

	fontHeight = (int)font->lineHeight;
	BASSERT(BASSERT_GENERAL, (fontHeight > 0), "CBConsoleWindow::Render : Illegal font height: %0.3f\n", font->lineHeight);
	noof = (int)h / fontHeight;

	index = freeLineIndex - noof;
	while (index < 0)
		index += NOOF_LINES;

	loop = noof;
	while (loop--)
	{
		len = bkStringLength16(lineBuffer[index]);
		if (len > 0)
			bdPrintFontLite(font, lineBuffer[index], len, x,y, r,g,b,a, NULL, NULL);

		y -= font->lineHeight;

		index = (index+1) % NOOF_LINES;
	}

	bdPopRenderState(BDRENDERSTATE_2DWORLDMATRIX);
	bdPopRenderState(BDRENDERSTATE_ALPHAENABLE);
	bdPopProjectionMode();
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::Clear
   Purpose : clear all buffers
   Parameters : 
   Returns : 
   Info : 
*/

void CBConsoleWindow::Clear()
{
	int		i;
	ushort	*lp;


	if (lineBuffer[0] == NULL)
		return;

	// reset line buffer
	freeLineIndex = freeCharIndex = 0;
	for (i=0; i<NOOF_LINES; i++)
	{
		lp = lineBuffer[i];
		*lp = 0;
	}

	// reset static buffer
	cbuf[0] = 0;
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::SetRect
   Purpose : set display rectangle
   Parameters : xpos, ypos, width, height
   Returns : 
   Info : origin 0,0 at centre of screen with x+ right and +y up screen
*/

void CBConsoleWindow::SetRect(int xp, int yp, int width, int height)
{
	if (xp != -1)
		this->xp = xp;
	if (yp != -1)
		this->yp = yp;
	if (width != -1)
		this->width = width;
	if (height != -1)
		this->height = height;
}


/* --------------------------------------------------------------------------------
   Function : CBConsoleWindow::SetFont
   Purpose : set font to use
   Parameters : font pointer, font scaler, r,g,b,a (0-127 or -1 to ignore)
   Returns : 
   Info : 
*/

void CBConsoleWindow::SetFont(TBFont *font, float scaler, int r, int g, int b, int a)
{
	this->font = font;
	this->scaler = scaler;

	if (r != -1)
		this->r = r;
	if (g != -1)
		this->g = g;
	if (b != -1)
		this->b = b;
	if (a != -1)
		this->a = a;
}


// ********************************************************************************
// end of CBConsoleWindow
// ********************************************************************************


/* --------------------------------------------------------------------------------
   Function : bdEnableConsoleWindow
   Purpose : enable / disable the console window
   Parameters : TRUE to enable, FALSE to disable
   Returns : 
   Info : 
*/

void bdEnableConsoleWindow(int enable)
{
	if (enable == (consoleWindow != NULL))
		return;

	if (!enable)
	{
		delete consoleWindow;
		consoleWindow = NULL;
		return;
	}

	consoleWindow = new CBConsoleWindow;
}


/* --------------------------------------------------------------------------------
   Function : bdConsoleWindowPrintf
   Purpose : printf into console window
   Parameters : printf args
   Returns : 
   Info : 
*/

void bdConsoleWindowPrintf(char *format, ...)
{
	va_list		argp;
	int			noof;


	if (consoleWindow == NULL)
		return;

	// parse format
	va_start(argp, format);
	noof = vsprintf(consoleWindow->cbuf, format, argp);
	va_end(argp);

	consoleWindow->Print();
}


/* --------------------------------------------------------------------------------
   Function : bdResetConsoleWindow
   Purpose : resets the console window
   Parameters : 
   Returns : 
   Info : clears buffers
*/

void bdResetConsoleWindow()
{
	if (consoleWindow == NULL)
		return;

	consoleWindow->Clear();
}


/* --------------------------------------------------------------------------------
   Function : bdFormatConsoleWindow
   Purpose : format the console window
   Parameters : xp, yp, [height (-1 ignore) = -1]
   Returns : 
   Info : origin 0,0 at centre of screen with x+ right and +y up screen
*/

void bdFormatConsoleWindow(int xp, int yp, int height)
{
	if (consoleWindow == NULL)
		return;

	consoleWindow->SetRect(xp, yp, -1, height);
}


/* --------------------------------------------------------------------------------
   Function : bdSetConsoleWindowFont
   Purpose : set font to use
   Parameters : font pointer, [font scaler = 1.0f, r,g,b,a (0-127 or -1 ignore) = -1]
   Returns : 
   Info : 
*/

void bdSetConsoleWindowFont(TBFont *font, float scaler = 1.0f, int r=-1, int g=-1, int b=-1, int a=-1)
{
	if (consoleWindow == NULL)
		return;

	consoleWindow->SetFont(font, scaler, r,g,b,a);
}


/* --------------------------------------------------------------------------------
   Function : bdShowConsoleWindowFont
   Purpose : show console window
   Parameters : TRUE or FALSE to hide
   Returns : 
   Info : 
*/

void bdShowConsoleWindowFont(int show)
{
	if (consoleWindow == NULL)
		return;

	consoleWindow->Show(show);
}


/* --------------------------------------------------------------------------------
   Function : bRenderConsoleWindow
   Purpose : render console window into screen
   Parameters : 
   Returns : 
   Info : not called by user
*/

void bRenderConsoleWindow()
{
	if (consoleWindow == NULL)
		return;

	if (consoleWindow->show)
		consoleWindow->Render();
}
