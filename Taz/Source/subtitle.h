#ifndef __H_SUBTITLE
#define __H_SUBTITLE

#include "textfx.h"
#include "text2.h"

#define SUBTITLE_QUEUE (1<<0)
#define	SUBTITLE_ALWAYS (1<<1)

extern EStringIndex	introCamSubtitles[][9];
extern EStringIndex	tutorialSubtitles[][9];
extern BOOK							subtitleBook;				// TP: Book to use for the subtitle
extern BOOK							subtitleAlwaysBook;
extern bool	wasSubTitleBookOpen;
extern bool wasSubTitleAlwaysBookOpen;


/*  --------------------------------------------------------------------------------
	Function	: Subtitle
	Purpose		: Draw the subtitle for given time
	Parameters	: int stringTableIndex, time in seconds for display
	Returns		: 
	Info		: 
*/
struct TAG_SUBTITLE *Subtitle(const EStringIndex text, const float time, const int flags = 0);

/*  --------------------------------------------------------------------------------
	Function	: GetSubtitleSize
	Purpose		: Get size of subtitle list
	Parameters	: 
	Returns		: 
	Info		: 
*/
int GetSubtitleSize();

/*  --------------------------------------------------------------------------------
	Function	: InitialiseSubtitles
	Purpose		: Initialise the subtitle list
	Parameters	: 
	Returns		: 
	Info		: 
*/
void InitialiseSubtitles(void);

void AddSubtitle(TAG_SUBTITLE *ptr, BOOK &subBook);

/*  --------------------------------------------------------------------------------
	Function	: UpdateSubtitles
	Purpose		: Update the subtitles and remove if necessary
	Parameters	: 
	Returns		: 
	Info		: 
*/
void UpdateSubtitles(void);

/*  --------------------------------------------------------------------------------
	Function	: DrawSubtitles
	Purpose		: Draw the subtitles
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DrawSubtitles(void);

/*  --------------------------------------------------------------------------------
	Function	: DestroySubtitles
	Purpose		: Destroy and remove all current subtitles
	Parameters	: 
	Returns		: 
	Info		: 
*/
void DestroySubtitles(void);

/*  --------------------------------------------------------------------------------
	Function	: CloseCurrentSubtitle
	Purpose		: close the current subtitle if active
	Parameters	: 
	Returns		: 
	Info		: 
*/
void CloseCurrentSubtitle(void);

void closeSubtitles();

void openSubtitles();

#endif