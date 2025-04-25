// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : prompt.cpp
//   Purpose : functions to calculate projectile path of object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "textfx.h"
#include "booklist.h"
#include "pageitemfx.h"
#include "characters.h"
#include "status.h"
#include "prompt.h"
#include "xboxstrings.h"

PROMPTBOOKINFO			promptBookInfo;
MORRONSUPPORTSTRUCTURE	*americanEvents;


/* --------------------------------------------------------------------------------
   Function : InitialisePromptBookInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialisePromptBookInfo(void)
{
	promptBookInfo.book = NULL;
	promptBookInfo.string = (EStringIndex)-1;
	promptBookInfo.style = (EPromptBookStyle)0;

	americanEvents = NULL;
}

/* --------------------------------------------------------------------------------
   Function : CreatePromtBook
   Purpose : 
   Parameters : style/screen position, reference to string table
   Returns : 
   Info : 
*/

int CreatePromtBook(EPromptBookStyle style, EStringIndex string)
{
	PAGE	*page;

	if(promptBookInfo.style != PROMPT_NONE)
	{
		bkPrintf("*** WARNING *** you are trying to create a prompt where one already exists, or you have specified an invalid type!");
		return FALSE;		// CMD: can only have one prompt at a time!
	}

	promptBookInfo.book = new BOOK("Prompt Book");
	page = promptBookInfo.book->addPage(string);

	promptBookInfo.style = style;
	promptBookInfo.string = string;

	switch(style)
	{
	case PROMPT_TITLE:
		page->setYAlign(PIFLAG_YALIGN_TOP);
//		promptBookInfo.book->setBookRect(RECTANGLE(-0.46875f, 0.46875f, 0.4464f, 0.2232f));
		break;
	case PROMPT_DIRECTION:
		page->setYAlign(PIFLAG_YALIGN_BOTTOM);
//		promptBookInfo.book->setBookRect(RECTANGLE(-0.46875f, 0.46875f, -0.4464f, -0.2232f));
		break;
	}
	page->setSlack(false);
	page->setMargin(0.025f);
	page->setBoxStyle(true, true);
	page->setBackgroundColour(COLOUR(0,0,0,64));
	page->setBorderColour(COLOUR(0,0,0,255));
	page->setEffect(&PIE_fade);


	promptBookInfo.book->open();

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdatePromtBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdatePromptBook(void)
{
	if(promptBookInfo.book)
	{
//		promptBookInfo.book->drawScreenRects();
	}
}

/* --------------------------------------------------------------------------------
   Function : KillPromtBook
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void KillPromptBook(void)
{
	if(promptBookInfo.book)
	{
		SAFE_DELETE(promptBookInfo.book);
		// TP: BookList_closeAndShutdown(*promptBookInfo.book);// PP: PLEASE let me know if there's a problem with BookList_closeAndShutdown.  I HATE seeing books popping out rather than closing gracefully!
	}
	promptBookInfo.style = PROMPT_NONE;
}

/* --------------------------------------------------------------------------------
   Function : StartHelpForMorons
   Purpose : 
   Parameters : 
   Returns : 
   Info : Morron is an affectionate Scottish term for 'American'
*/

void StartHelpForMorons(void)
{
	PAGE	*page;
	TEXTBOX	*textbox;
	int		i, allowMalloc;

	allowMalloc = FALSE;

	switch(map.sceneNumber)
	{
	case SCENE_SAFARI:
	case SCENE_DEPTSTR:
		if(gameStatus.globalSettings.difficulty == DIFF_EASY) allowMalloc = TRUE;
		break;
	case SCENE_ZOOHUB:
	case SCENE_GHOSTTOWN:
	case SCENE_GOLDMINE:
		allowMalloc = TRUE;
		break;
	}
	
	if(allowMalloc)
	{
		americanEvents = (MORRONSUPPORTSTRUCTURE*)MALLOC(sizeof(MORRONSUPPORTSTRUCTURE));
		ASSERTM(americanEvents, "out of memory, could not malloc americanEvents");

		americanEvents->book = new BOOK("American Message Book");
//		americanEvents->book->setEffectChooser(&mainPIEChooser);

		for(i = 18;i >= 0;--i)
		{
			switch(i)
			{
			case 0:
				page = americanEvents->book->addPage("objective1");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE1,false);
				break;
			case 1:
				page = americanEvents->book->addPage("objective2");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE2,false);
				break;
			case 2:
				page = americanEvents->book->addPage("objective3");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE3,false);
				break;
			case 3:
				page = americanEvents->book->addPage("objective4");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE4,false);
				break;
			case 4:
				page = americanEvents->book->addPage("objective5");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE5,false);
				break;
			case 5:
				page = americanEvents->book->addPage("firstZookeeper");
				textbox = page->insertItem(STR_SAFARI_OBJECTIVE6,false);
				break;
			case 6:
				page = americanEvents->book->addPage("firstPoster");
				textbox = page->insertItem(STR_SAFARI_FIRST_POSTER,false);
				break;
			case 7:
				page = americanEvents->book->addPage("lastPoster");
				textbox = page->insertItem(STR_SAFARI_ALL_POSTERS,false);
				break;
			case 8:
				page = americanEvents->book->addPage("sandwichHint");
				textbox = page->insertItem(STR_SAFARI_SANDWICH_HINT,false);
				break;
			case 9:
				page = americanEvents->book->addPage("destructionHint");
				textbox = page->insertItem(STR_SAFARI_DESTRUCTION_HINT,false);
				break;
			case 10:
				page = americanEvents->book->addPage("statueHint");
				textbox = page->insertItem(STR_SAFARI_SECRET_HINT,false);
				break;
			case 11:
				page = americanEvents->book->addPage("tiptoeHint");
				textbox = page->insertItem(STR_ZOOHUB_TILT_HINT,false);
				break;
			case 12:
				page = americanEvents->book->addPage("spin");
				textbox = page->insertItem(STR_SAFARI_SPIN_HINT,false);
				break;
			case 13:
				page = americanEvents->book->addPage("burp");
				textbox = page->insertItem(STR_SAFARI_BURP_HINT,false);
				break;
			case 14:
				page = americanEvents->book->addPage("spit");
				textbox = page->insertItem(STR_SAFARI_SPIT_HINT,false);
				break;
			case 15:
				page = americanEvents->book->addPage("costume");
				textbox = page->insertItem(STR_SAFARI_COSTUME_HINT,false);
				break;
			case 16:
				page = americanEvents->book->addPage("trolley");
				textbox = page->insertItem(STR_DEPT_TROLLEY_HINT,false);
				break;
			case 17:
				page = americanEvents->book->addPage("minecart");
				textbox = page->insertItem(STR_MINE_CART_HINT,false);
				break;
			case 18:
				page = americanEvents->book->addPage("ghostcart");
				textbox = page->insertItem(STR_GHOST_CART_HINT,false);
				break;
			}
				
			textbox->setWrap(true)
					->setEffect(&PIE_fade);

			page->setYAlign(PIFLAG_YALIGN_BOTTOM);
			page->setSlack(false);
			page->setMargin(0.025f);
			page->setBoxStyle(true, true);
			page->setBackgroundColour(COLOUR(0,0,0,64));
			page->setBorderColour(COLOUR(0,0,0,255));
		}

		americanEvents->clock = 0.0f;
		americanEvents->delay = 0.0f;
		americanEvents->localFlags = 0;
		americanEvents->currentEvent = MEVENT_NONE;
	}
	else
	{
		americanEvents = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateHelpForMorons
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateHelpForMorons(void)
{
	if (gameStatus.currentState == GAMESTATE_CAMERACASE)
		return;

	if(americanEvents)
	{
		if(americanEvents->delay)
		{
			if((americanEvents->delay -= fTime) <= 0.0f)
			{
				americanEvents->delay = 0.0f;
				RequestMorronicMessage(americanEvents->delayedEvent);
			}
		}

		if(americanEvents->localFlags & MORRON_BOOKOPEN)
		{
			if((americanEvents->clock -= fTime) < 0.0f)
			{
				switch(americanEvents->currentEvent)
				{
				case MEVENT_ENTER_1:
					RequestMorronicMessage(MEVENT_ENTER_2);
					break;
				case MEVENT_ENTER_2:
					RequestMorronicMessage(MEVENT_ENTER_3);
					break;
				case MEVENT_ENTER_3:
					RequestMorronicMessage(MEVENT_ENTER_4);
					break;
				case MEVENT_ENTER_4:
					RequestMorronicMessage(MEVENT_ENTER_5);
					break;
				default:
					RequestMorronicMessage(MEVENT_NONE);
					americanEvents->book->close();
					americanEvents->localFlags &= ~MORRON_BOOKOPEN;
					break;
				}
			}
		}
		else
		{
			if(americanEvents->localFlags & MORRON_REQUESTMADE)
			{
				// CMD: check for prompt book, and if it exists kill it
				if(promptBookInfo.book)
				{
					KillPromptBook();
				}
				americanEvents->book->open();
				americanEvents->localFlags |= MORRON_BOOKOPEN;
				americanEvents->localFlags &= ~MORRON_REQUESTMADE;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int RequestMorronicMessage(EMorronEvent event, float delay)
{
	uint32		flags;

	if((americanEvents)&&(!gameStatus.demoMode))
	{
		flags = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags;

		switch(event)
		{
		case MEVENT_NONE:
			americanEvents->clock = 0.0f;
			americanEvents->currentEvent = event;
			return FALSE;
		case MEVENT_ENTER_1:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_SAFARIVIRGIN))		americanEvents->book->gotoPage("objective1");
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_ENTER_2:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_SAFARIVIRGIN))		americanEvents->book->gotoPage("objective2");
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_ENTER_3:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_SAFARIVIRGIN))		americanEvents->book->gotoPage("objective3");
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_ENTER_4:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_SAFARIVIRGIN))		americanEvents->book->gotoPage("objective4");
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_ENTER_5:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_SAFARIVIRGIN))
			{
				americanEvents->book->gotoPage("objective5");
				flags &= ~MORRON_SAFARIVIRGIN;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_FIRSTKEEPER:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_KEEPERVIRGIN))
			{
				americanEvents->book->gotoPage("firstZookeeper");
				americanEvents->clock = MORRON_PAGE_TIME*2.0f;
				flags &= ~MORRON_KEEPERVIRGIN;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_FIRSTPOSTER:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_POSTERVIRGIN))
			{
				if(americanEvents->delay = delay) 
				{
					americanEvents->delayedEvent = event;
					return FALSE;
				}
				americanEvents->book->gotoPage("firstPoster");
				flags &= ~MORRON_POSTERVIRGIN;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_ALLPOSTERS:
			if((map.sceneNumber == SCENE_SAFARI)&&(~flags & MORRON_POSTERWHORE))
			{
				americanEvents->book->gotoPage("lastPoster");
				flags |= MORRON_POSTERWHORE;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_SANDWICHHINT:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_NOTALLSNACKS))
			{
				americanEvents->book->gotoPage("sandwichHint");
				flags &= ~MORRON_NOTALLSNACKS;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_DESTRUCTIONHINT:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_NOTALLDAMAGE))
			{
				americanEvents->book->gotoPage("destructionHint");
				flags &= ~MORRON_NOTALLDAMAGE;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_STATUEHINT:
			if((map.sceneNumber == SCENE_SAFARI)&&(flags & MORRON_NOSAMHEAD))
			{
				americanEvents->book->gotoPage("statueHint");
				flags &= ~MORRON_NOSAMHEAD;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_TIPTOEHINT:
			if((map.sceneNumber == SCENE_ZOOHUB)&&(~americanEvents->localFlags & MORRON_BOOKOPEN))
			{
				americanEvents->book->gotoPage("tiptoeHint");
			}
			else
				if(americanEvents->currentEvent == MEVENT_TIPTOEHINT)
				{
					americanEvents->clock = MORRON_PAGE_TIME*0.5f;
					return FALSE;
				}
				else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_HUTSPIN:
			if((map.sceneNumber == SCENE_SAFARI)&&(~flags & MORRON_HUTPOSTER))
			{
				americanEvents->book->gotoPage("spin");
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_CORKSPIN:
			if((map.sceneNumber == SCENE_SAFARI)&&(~flags & MORRON_CORKPOSTER))
			{
				americanEvents->book->gotoPage("spin");
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_BURP:
			if((map.sceneNumber == SCENE_SAFARI)&&(gameStatus.player[0].actorInstance->characterInfo)&&
				(gameStatus.player[0].actorInstance->characterInfo->currentBurp)&&(~flags & MORRON_HONEYPOSTER))
			{
				americanEvents->book->gotoPage("burp");
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_SPIT:
			if((map.sceneNumber == SCENE_SAFARI)&&(~flags & MORRON_KEEPERVIRGIN)&&(~flags & MORRON_BADMANNERED))
			{
				americanEvents->book->gotoPage("spit");
				flags |= MORRON_BADMANNERED;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_COSTUME:
			if((map.sceneNumber == SCENE_SAFARI)&&(~flags & MORRON_WELLDRESSED))
			{
				americanEvents->book->gotoPage("costume");
				flags |= MORRON_WELLDRESSED;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_TROLLEY:
			if((map.sceneNumber == SCENE_DEPTSTR)&&(~flags & MORRON_TROLLEYMASTER))
			{
				americanEvents->book->gotoPage("trolley");
				flags |= MORRON_TROLLEYMASTER;
			}
			else return RequestMorronicMessage(MEVENT_NONE);
			break;
		case MEVENT_MINECART:
			switch(map.sceneNumber)
			{
			case SCENE_GOLDMINE:
				americanEvents->book->gotoPage("minecart");
				break;
			case SCENE_GHOSTTOWN:
				americanEvents->book->gotoPage("ghostcart");
				break;
			default:
				return RequestMorronicMessage(MEVENT_NONE);
			}
			break;
		}

		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].helpFlags = flags;
		
		switch(event)
		{
		case MEVENT_FIRSTKEEPER:
		case MEVENT_FIRSTPOSTER:
		case MEVENT_ALLPOSTERS:
			americanEvents->clock = 2.0f*MORRON_PAGE_TIME;
			break;
		default:
			americanEvents->clock = MORRON_PAGE_TIME;
			break;
		}
		americanEvents->currentEvent = event;
		americanEvents->localFlags |= MORRON_REQUESTMADE;

		return TRUE;
	}
	// CMD: not in scene or on easy daffyculty so...
	return FALSE;
}

void closePromptBook()
{
	if (americanEvents && americanEvents->localFlags & MORRON_BOOKOPEN)
		americanEvents->book->close(0.0f, true);

	if (promptBookInfo.book)
		promptBookInfo.book->close(0.0f, true);
}

void openPromptBook()
{
	if (americanEvents && americanEvents->localFlags & MORRON_BOOKOPEN)
		americanEvents->book->open();

	if (promptBookInfo.book)
		promptBookInfo.book->open();
}

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeHelpForMorons(void)
{
	if(americanEvents)
	{
		SAFE_DELETE(americanEvents->book);
		SAFE_FREE(americanEvents);
	}
}

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CheckIfDisplayingMorronicMessages(EMorronEvent event)
{
	if(americanEvents)
	{
		switch(event)
		{
		case MEVENT_NONE:
			if(~americanEvents->localFlags & MORRON_BOOKOPEN)
			{
				return TRUE;
			}
			break;
		default:
			if(americanEvents->localFlags & MORRON_BOOKOPEN)
			{
				if(americanEvents->currentEvent == event) return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : RequestMorronicMessage
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CutMorronicMessageDelay(float newTime)
{
	if(americanEvents)
	{
		if(americanEvents->delay > newTime) americanEvents->delay = newTime;
	}
}