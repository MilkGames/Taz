// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : subtitle.cpp
//   Purpose : subtitle functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "actors.h"
#include "subtitle.h"
#include "linkedlist.h"
#include "textfx.h"
#include "control.h"
#include "main.h"
#include "sfx.h"
#include "scene.h"
#include "timer.h"
#include "display.h"
#include "PageItemFX.h"		// PP: Special effects for page items


#define SUBTITLE_NONE		((EStringIndex)-1)		// PP: just something to put into the unused elements of the introCamSubtitles array -
														// PP: PC compiler no like {} 
// PP: margin width for subtitle pages
#define SUBTITLE_MARGIN		0.025f

// TP: List of subtitle strings for intro cams, couldn't think of anywhere else to put it
EStringIndex	introCamSubtitles[22][9] =
{
	{SUBTITLE_NONE},	// TP: SCENE_LANGUAGE
	
	{SUBTITLE_NONE},	// TP: SCENE_FRONTEND

	{SUBTITLE_NONE},	// TP: SCENE_PLAYPEN

	{SUBTITLE_NONE},	// TP: SCENE_ZOOHUB

	{STR_ICEDOME_INTROCAM7,STR_ICEDOME_INTROCAM1,STR_ICEDOME_INTROCAM2,STR_ICEDOME_INTROCAM3,STR_ICEDOME_INTROCAM4,STR_ICEDOME_INTROCAM6,\
	STR_ICEDOME_INTROCAM5,SUBTITLE_NONE},	// TP: SCENE_ICEDOME

	{STR_SAFARI_INTROCAM1,STR_SAFARI_INTROCAM2,STR_SAFARI_INTROCAM3,STR_SAFARI_INTROCAM4,STR_SAFARI_INTROCAM5,\
	STR_SAFARI_INTROCAM6,STR_SAFARI_INTROCAM7,STR_SAFARI_INTROCAM8},	// TP: SCENE_SAFARI

	{STR_AQUA_INTROCAM1,STR_AQUA_INTROCAM2,STR_AQUA_INTROCAM3,STR_AQUA_INTROCAM4,STR_AQUA_INTROCAM5,STR_AQUA_INTROCAM6,\
	STR_AQUA_INTROCAM7,SUBTITLE_NONE},	// TP: SCENE_AQUA

	{SUBTITLE_NONE},	// TP: SCENE_ZOOBOSS

	{SUBTITLE_NONE},	// TP: SCENE_CITYHUB

	{STR_DEPTSTR_INTROCAM4,STR_DEPTSTR_INTROCAM5,STR_DEPTSTR_INTROCAM2,STR_DEPTSTR_INTROCAM1,STR_DEPTSTR_INTROCAM3,STR_DEPTSTR_INTROCAM6, \
	STR_DEPTSTR_INTROCAM7,SUBTITLE_NONE},	// TP: SCENE_DEPTSTR

	{STR_MUSEUM_INTROCAM1,STR_MUSEUM_INTROCAM2,STR_MUSEUM_INTROCAM3,STR_MUSEUM_INTROCAM4,STR_MUSEUM_INTROCAM5,\
		STR_MUSEUM_INTROCAM6,STR_MUSEUM_INTROCAM7,STR_MUSEUM_INTROCAM8},	// TP: SCENE_MUSEUM

	{STR_CONSTRUCT_INTROCAM1,STR_CONSTRUCT_INTROCAM2,STR_CONSTRUCT_INTROCAM3,STR_CONSTRUCT_INTROCAM4,STR_CONSTRUCT_INTROCAM5,\
	STR_CONSTRUCT_INTROCAM6,STR_CONSTRUCT_INTROCAM7,SUBTITLE_NONE},	// TP: SCENE_CONSTRUCT

	{SUBTITLE_NONE},	// TP: SCENE_CITYBOSS

	{SUBTITLE_NONE},	// TP: SCENE_WILDWESTHUB

	{STR_GHOST_INTROCAM2,STR_GHOST_INTROCAM3,STR_GHOST_INTROCAM5,STR_GHOST_INTROCAM7,STR_GHOST_INTROCAM1,STR_GHOST_INTROCAM6,STR_GHOST_INTROCAM4},	// TP: SCENE_GHOSTTOWN

	{STR_GOLDMINE_INTROCAM1,STR_GOLDMINE_INTROCAM2,STR_GOLDMINE_INTROCAM3,STR_GOLDMINE_INTROCAM6,STR_GOLDMINE_INTROCAM4,STR_GOLDMINE_INTROCAM5,\
	STR_GOLDMINE_INTROCAM7,SUBTITLE_NONE},	// TP: SCENE_GOLDMINE

	{STR_GRANDC_INTROCAM1,STR_GRANDC_INTROCAM3,STR_GRANDC_INTROCAM4,STR_GRANDC_INTROCAM2,STR_GRANDC_INTROCAM5,STR_GRANDC_INTROCAM6,\
	STR_GRANDC_INTROCAM7},	// TP: SCENE_GRANDCANYON

	{SUBTITLE_NONE},	// TP: SCENE_WESTBOSS

	{STR_TASMANIA_INTROCAM4,STR_TASMANIA_INTROCAM1,STR_TASMANIA_INTROCAM2,STR_TASMANIA_INTROCAM3,STR_TASMANIA_INTROCAM5,STR_TASMANIA_INTROCAM6,STR_TASMANIA_INTROCAM7},	// TP: SCENE_WESTBOSS

	{SUBTITLE_NONE},	// TP: SCENE_TAZHUB
	{SUBTITLE_NONE},	// TP: SCENE_PREBOSS
	{SUBTITLE_NONE},	// TP: SCENE_TAZBOSS
};

static LINKEDLIST<TAG_SUBTITLE>		subtitleList("subtitle");
BOOK							subtitleBook;				// TP: Book to use for the subtitle
BOOK							subtitleAlwaysBook;

typedef struct TAG_SUBTITLE
{
	PAGE			*page;
	EStringIndex	text;			// TP: index into string table for subtitle to use
	float			time;			// TP: time in seconds before subtitle fades away
	int				flags;			// TP: subtitle flags SUBTITLE_...
} SUBTITLE;

bool	wasSubTitleBookOpen;		// CMD: we need to clost the book when paused, was it open beforehand?
bool	wasSubTitleAlwaysBookOpen;

/*  --------------------------------------------------------------------------------
	Function	: PageClosed
	Purpose		: Called when the page is closed
	Parameters	: 
	Returns		: 
	Info		: 
*/
void PageClosed(struct TAG_PAGE* const page, const int32 context)
{
	if (context)
	{
		SUBTITLE	*ptr = (SUBTITLE*)context;

		bkPrintf("Page closed callback\n");

		// TP: Remove page now it's closed
		
		if (ptr->flags & SUBTITLE_ALWAYS)
			subtitleAlwaysBook.removePage(page,TRUE);
		else
			subtitleBook.removePage(page,TRUE);

		subtitleList.remove(ptr);
		FREE(ptr);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: Subtitle
	Purpose		: Draw the subtitle for given time
	Parameters	: int stringTableIndex, time in seconds for display
	Returns		: SUBTITLE
	Info		: 
*/
SUBTITLE *Subtitle(const EStringIndex text, const float time, const int flags)
{
	SUBTITLE	*ptr;

	if (((unsigned int)text) >= mainStringTable->noofStrings) return NULL;

	if (!(ptr = (SUBTITLE*)ZALLOC(sizeof(SUBTITLE))))
	{
		bkPrintf("*** WARNING *** Could not malloc memory for subtitle.\n");
		return NULL;
	}

	ptr->flags = flags;
	ptr->time = time;
	ptr->page = NULL;
	ptr->text = text;

	subtitleList.append(ptr);
	return ptr;
}

/*  --------------------------------------------------------------------------------
	Function	: InitialiseSubtitles
	Purpose		: Initialise the subtitle list
	Parameters	: 
	Returns		: 
	Info		: 
*/

int GetSubtitleSize()
{
	return subtitleList.getSize();
}

/*  --------------------------------------------------------------------------------
	Function	: InitialiseSubtitles
	Purpose		: Initialise the subtitle list
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitialiseSubtitles(void)
{
	static const RECTANGLE	rect(-0.46875f, 0.46875f, -0.4464f, -0.2232f);// PP: REMOUT: must now be expressed as fractions of the screen dimensions		rect(-300,300,-200,-100);

	subtitleList.init();

	// PP: REMOUT: setting NULL effect chooser seemed to stop the subtitles opening; I'll look into it		subtitleBook.setEffectChooser(NULL);
	// PP: REMOUT: setting NULL effect chooser seemed to stop the subtitles opening; I'll look into it		subtitleBook.setEffectChooser(NULL);

	// PP: REMOUT: this doesn't neccessarily give enough room for the text		subtitleBook.setBookRect(rect);

	// PP: REMOUT: the textbox itself is now tinted								subtitleBook.setBackgroundColour(COLOUR(0,0,0,64));

//	subtitleBook.setBorderColour(COLOUR(0,0,255,255));
	wasSubTitleBookOpen = false;
	wasSubTitleAlwaysBookOpen = false;
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateSubtitles
	Purpose		: Update the subtitles and remove if necessary
	Parameters	: 
	Returns		: 
	Info		: 
*/
void UpdateSubtitles(void)
{
	SUBTITLE	*ptr;
	TEXTBOX		*text;
	int			i,num;
	bool		close = TRUE;
	PAGE		*subPage;

	for (num=i=subtitleList.getSize()-1;i>=0;i--)
	{
		ptr = subtitleList.get(i);

		if (ptr->page)
		{
			// TP: is subtitle active
			if (ptr->time)
			{
				ptr->time -= fTime;
				
				if (ptr->time <= 0.0f)
				{
					ptr->time = 0.0f;

					if (ptr->flags & SUBTITLE_ALWAYS)
					{
						subtitleAlwaysBook.close();
						wasSubTitleAlwaysBookOpen = false;
					}
					else
					{
						subtitleBook.close();
						wasSubTitleBookOpen = false;
					}

					bkPrintf("Subtitle closed\n");
				}
			}
		}

		if (ptr->flags & SUBTITLE_ALWAYS)
		{
			if (!subtitleAlwaysBook.getCurrentPage())
			{
				AddSubtitle(ptr, subtitleAlwaysBook);
				wasSubTitleAlwaysBookOpen = true;
			}
		}
		else
		{
			if (!subtitleBook.getCurrentPage())
			{
				AddSubtitle(ptr, subtitleBook);
				wasSubTitleBookOpen = true;

				subtitleBook.setAutoDraw(gameStatus.globalSettings.subtitles != 0);
			}
		}
	}

// PP: REMOUT: Now Handled by the Book List				subtitleBook.update();
}

/*  --------------------------------------------------------------------------------
	Function	: DrawSubtitles
	Purpose		: Draw the subtitles
	Parameters	: 
	Returns		: 
	Info		: 
*/

void DrawSubtitles(void)
{
/* PP: REMOUT: Now Handled by the Book List				if (!gameStatus.globalSettings.subtitles)
	{
		// JW: Subtitles are set to off
		//bkPrintf("*** WARNING *** Subtitles are set to off.\n");
		return;
	}

	TBSavedRenderStates	saveBlock;
	RECTANGLE			rect(-300,300,-100,-200);

	bdSaveRenderStates(&saveBlock);

	bdSetIdentityObjectMatrix();
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

	bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,TRUE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

	subtitleBook.draw();

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bdRestoreRenderStates(&saveBlock);*/
}

void AddSubtitle(SUBTITLE * ptr, BOOK &subBook)
{
	TEXTBOX		*text;
	PAGE		*subPage;

	bkPrintf("Subtitle open\n");

	// TP: Add page and open book
	ptr->page = subBook.addPage(subPage=new PAGE());
				
	subPage->insertItem(text = new TEXTBOX(ptr->text));// PP: REMOUT: OOD	,TEXTFX_DEFAULT_COLOUR1,TEXTFX_DEFAULT_COLOUR2,TBFLAG_DEFAULTS,TRUE));
	
	text->setWrap(true);
	text->setEffect(&PIE_fade);// PP: these subtitles don't seem to suit any fancy effects; just use a bog-standard fade
				
	// PP: I've made the subtitle boxes size themselves according to what's in them - let me know what you think
	// PP: This stops text spilling out, and I also think it's a bit nicer to look at
				
	// PP: align to bottom of container
	subPage->setYAlign(PIFLAG_YALIGN_BOTTOM);
				
	// PP: no height slack please
	subPage->setVSlack(false);
			
	// PP: Add a bit of breathing space round the text
	subPage->setMargin(SUBTITLE_MARGIN);
				
	// PP: specify that the page's outline box will be loose on the X but tight on the Y
	subPage->setBoxStyle(false, true);
				
	// PP: set page's background and border
	subPage->setBackgroundColour(COLOUR(0,0,0,75));
	subPage->setBorderColour(COLOUR(0,0,0,255));
				
	ptr->page->hasClosedFunc = PageClosed;
	ptr->page->hasClosedFuncContext = (int32)ptr;
	subBook.open();
				
	// TP: play audio file
	if (mainStringTable->strings[ptr->text].audioFilename)
	{
		StopChannel(&characterSounds.tweety);
		characterSounds.tweety = PlaySample(mainStringTable->strings[ptr->text].audioFilename,200);
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DestroySubtitles
	Purpose		: Destroy and remove all current subtitles
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DestroySubtitles(void)
{
	PAGE		*page;

	while(page = subtitleBook.getCurrentPage())
	{
		subtitleBook.removePage(page,TRUE);
		// TP: PageClosed(page,0);
	}

	while(page = subtitleAlwaysBook.getCurrentPage())
	{
		subtitleAlwaysBook.removePage(page,TRUE);
		// TP: PageClosed(page,0);
	}
	
	// PP: FREE MEMORY USED BY THE LIST AND ITS ITEMS
	// PP: REMOUT: nah		subtitleBook.close(0.0f,TRUE);
	subtitleList.kill();
}

/*  --------------------------------------------------------------------------------
	Function	: CloseCurrentSubtitle
	Purpose		: close the current subtitle if active
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CloseCurrentSubtitle(void)
{
	subtitleAlwaysBook.close();
	subtitleBook.close();
	wasSubTitleBookOpen = false;
	wasSubTitleAlwaysBookOpen = false;

	StopChannel(&characterSounds.tweety);
}

void closeSubtitles()
{
	if (wasSubTitleBookOpen)
		subtitleBook.close();
	if (wasSubTitleAlwaysBookOpen)
		subtitleAlwaysBook.close();
}

void openSubtitles()
{
	if (wasSubTitleBookOpen)
		subtitleBook.open();
	if (wasSubTitleAlwaysBookOpen)
		subtitleAlwaysBook.open();
}