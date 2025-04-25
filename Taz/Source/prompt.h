#ifndef __H_PROMPT
#define __H_PROMPT

#include "textfx.h"

enum EPromptBookStyle
{
	PROMPT_NONE,
	PROMPT_TITLE,
	PROMPT_DIRECTION,
};

enum EMorronEvent
{
	MEVENT_NONE,
	MEVENT_ENTER_1,
	MEVENT_ENTER_2,
	MEVENT_ENTER_3,
	MEVENT_ENTER_4,
	MEVENT_ENTER_5,
	MEVENT_FIRSTKEEPER,
	MEVENT_FIRSTPOSTER,
	MEVENT_ALLPOSTERS,
	MEVENT_SANDWICHHINT,
	MEVENT_DESTRUCTIONHINT,
	MEVENT_STATUEHINT,
	MEVENT_TIPTOEHINT,
	MEVENT_HUTSPIN,
	MEVENT_CORKSPIN,
	MEVENT_BURP,
	MEVENT_SPIT,
	MEVENT_COSTUME,
	MEVENT_TROLLEY,
	MEVENT_MINECART,
};

typedef struct TAG_PROMPTBOOKINFO
{
	BOOK				*book;
	EPromptBookStyle	style;
	EStringIndex		string;				// CMD: index into string table;
}PROMPTBOOKINFO, _PROMTBOOKINFO;

// CMD: constants
#define MORRON_PAGE_TIME	(3.0f)

// CMD: flags
#define MORRON_SAFARIVIRGIN		(1<<0)		// CMD: its the players first time ... in safari
#define MORRON_KEEPERVIRGIN		(1<<1)		// CMD: player has not seen a keeper before
#define MORRON_POSTERVIRGIN		(1<<2)		// CMD: player has not done a poster before
#define MORRON_POSTERWHORE		(1<<3)		// CMD: player has done ALL the poster
#define MORRON_NOTALLSNACKS		(1<<4)		// CMD: player has not collected all of the sandwiches
#define MORRON_NOTALLDAMAGE		(1<<5)		// CMD: player has not destroyed all of the destructibles
#define MORRON_NOSAMHEAD		(1<<6)		// CMD: player has not collected the secret statue of sam
#define MORRON_HUTPOSTER		(1<<7)		// CMD: this flag set if poster in hut has been destroyed
#define MORRON_CORKPOSTER		(1<<8)		// CMD: this flag set if poster in hollow log has been destroyed
#define MORRON_HONEYPOSTER		(1<<9)		// CMD: this flag set if honey poster has been destroyed
#define MORRON_BADMANNERED		(1<<10)		// CMD: player knows how to spit
#define MORRON_WELLDRESSED		(1<<11)		// CMD: player knows haw to use the costume attack
#define MORRON_TROLLEYMASTER	(1<<12)		// CMD: player is well versed in using trolleys

#define MORRON_BOOKOPEN			(1<<15)
#define MORRON_REQUESTMADE		(1<<16)
typedef struct TAG_MORRONSUPPORTSTRUCTURE
{
	BOOK				*book;
	float				clock;
	float				delay;
	EMorronEvent		currentEvent;
	EMorronEvent		delayedEvent;
	uint32				localFlags;
}MORRONSUPPORTSTRUCTURE, _MORRONSUPPORTSTRUCTURE;

extern PROMPTBOOKINFO	promptBookInfo;
extern MORRONSUPPORTSTRUCTURE	*americanEvents;

/* --------------------------------------------------------------------------------
   Function : InitialisePromptBookInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePromptBookInfo(void);

/* --------------------------------------------------------------------------------
   Function : CreatePromtBook
   Purpose : 
   Parameters : style/screen position, reference to string table
   Returns : 
   Info : 
*/

int CreatePromtBook(EPromptBookStyle style, EStringIndex string);

/* --------------------------------------------------------------------------------
   Function : UpdatePromtBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePromptBook(void);

/* --------------------------------------------------------------------------------
   Function : KillPromtBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void KillPromptBook(void);

/* --------------------------------------------------------------------------------
   Function : StartHelpForMorons
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void StartHelpForMorons(void);

/* --------------------------------------------------------------------------------
   Function : UpdateHelpForMorons
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateHelpForMorons(void);

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RequestMorronicMessage(EMorronEvent event, float delay=0.0f);

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHelpForMorons(void);

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CheckIfDisplayingMorronicMessages(EMorronEvent event);

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CutMorronicMessageDelay(float newTime);

/* --------------------------------------------------------------------------------
   Function : closePromptBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void closePromptBook();

/* --------------------------------------------------------------------------------
   Function : openPromptBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void openPromptBook();

#endif