// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : optionsmenu.cpp
//   Purpose : functions to create and control the game optionsmenu
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "TextFX.h"
#include "sfx.h"
#include "control.h"
#include "animation.h"
#include "frontendmain.h"
#include "Icon.h"				// PP: icon effects
#include "popup.h"
#include "files.h"
#include "XboxStrings.h"		// PP: Xbox versions of some strings


// PP: define WIDESCREEN_OPTION to present the toggle-widescreen option in the options menu
// PP: platform: Xbox TCR 1-15, 'Widescreen Display', can be taken to suggest that an ingame widescreen OPTION isn't allowed.
// PP: I'm a bit unsure though.
#if(BPLATFORM != XBOX)
#define WIDESCREEN_OPTION
#endif// PP: platform

int retval = -1;

EDifficulty	tempDifficulty;
bool tempVibration, tempSubtitles;

int difficultyPopupSelection;
EOptionsPopups currentOptionPopup;

bool animOnSubtitles, animToSubtitles;
int AlarmClockSoundHandle=-1, AdjustScreenSoundHandle=-1, HitTVSoundHandle=-1;
float oldScrX, oldScrY;


/*	--------------------------------------------------------------------------------
	Function 	: optionsMenuActionFunc
	Purpose 	: callback that responds to menu actions on the 'options' front-end menu
	Parameters 	: ref to a MENUACTION_INFO structure (TextFX.h)
	Returns 	: 
	Info 		: // PP: Must match the MenuActionFunc typedef (TextFX.h)
					// PP: I realise I'm being a bit unsociable here, splitting my little option off into its own callback,
					// PP: it's just that I think this way is nicer.
					// PP: I could change the whole options menu over to use this system, but it'd be kinda pointless at this late stage.
*/
void optionsMenuActionFunc(MENUACTION_INFO& info)
{
	if(info.item != NULL)
	{
		if(*(info.item) == "WIDESCREEN")
		{
			switch(info.action)
			{
			case MENUACTION_SELECT:
				{
					// PP: Widescreen option has been selected.
					
					// NH: Move Taz to TV
					frontEndAnimInfo.requestState = OM_TV_IDLE;

					// PP: show whether widescreen is on or off (because some people might not know)
					if(gameStatus.wideScreenWilson)
					{
						frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_ON");
					}
					else
					{
						frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_OFF");
					}
				}
				break;

			case MENUACTION_PRESS:
				{
					// PP: Widescreen option has been pressed.
					
					// PP: Bring up a popup to allow widescreen to be turned on or off
					// PP: NH: If no popup, then create one
					if (!frontEndMenu.popUpOptions)
					{
						currentOptionPopup = OPTIONS_POPUP_WIDESCREEN;
						frontEndMenu.frontEndMenuBook->getPage(menuNames[MENU_OPTIONS])->setNavigation(false);
						frontEndMenu.gameOptionsPage->infoSubBook->close();
						frontEndMenu.gameOptionsPage->controlSubBook->close();

						frontEndMenu.popUpOptions = new POPUP(POPUP_ON_OFF, gameStatus.wideScreenWilson);
					}
				}
				break;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: CreateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateGameOptionsMenuPage(FRONTENDMENU *menu)
{
	GAMEOPTIONSPAGE		*ptr;
	PAGE				*pagePtr, *subPage, *optionsFreePage;
	TEXTBOX				*dummyBox;
	ICON				*vibrationIcon=NULL;
	BOOK				*optionsFreeBook;

	RECTANGLE freePos;
	
#if BPLATFORM == PC
	freePos.left		= -0.5f;
	freePos.right		= 0.5f;
	freePos.top			= 0.12f;
	freePos.bottom		= -0.35f;
#elif BPLATFORM == XBOX
	freePos.left		= -0.5f;
	freePos.right		= 0.5f;
	freePos.top			= 0.06f;
	freePos.bottom		= -0.35f;
#else
	freePos.left		= -0.5f;
	freePos.right		= 0.5f;
	freePos.top			= 0.12f;
	freePos.bottom		= -0.35f;
#endif

	ptr = menu->gameOptionsPage = (GAMEOPTIONSPAGE*)ZALLOC(sizeof(GAMEOPTIONSPAGE));	
	if(!ptr)
	{
		bkPrintf("*** WARNING ***, could not malloc game options page/n");
		return;
	}
	// NH: Add Page (Options)
	pagePtr = ptr->page = menu->frontEndMenuBook->addPage(menuNames[MENU_OPTIONS]);
	pagePtr->setNavigation(true);

	// PP: navigation Y-wrapping is allowed, but only in response to a debounced input, NOT an auto-repeat input
	pagePtr->setNavigationWrap(PAGEFLAG_NAVWRAP_YDEBOUNCE);

	// PP: set the page's menu-action callback (this'll recieve notification when an item is pressed or that)
	pagePtr->setMenuActionFunc(optionsMenuActionFunc);

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_GAME_OPTIONS),false);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);
	
	// NH: Add Menu Free Book (Was just so much eaiser to do the positioning)
	//pagePtr->insertFreeItem(optionsFreeBook = new BOOK(), freePos);
	pagePtr->insertFreeItem(optionsFreePage = new PAGE(), freePos);

	// NH: Add Menu Items (Difficulty)
	optionsFreePage->insertItem(ptr->difficultyTb = new TEXTBOX(STR_GO_DIFFICULTY),true);
	ptr->difficultyTb->setYPadding(false);

#if BPLATFORM != PC
	// NH: Add Menu Items (Vibration)
	optionsFreePage->insertItem(ptr->vibrationTb = new TEXTBOX(STR_GO_VIBRATION),true);
	ptr->vibrationTb->setYPadding(false);		
#if BPLATFORM != XBOX

	// NH: Add Menu Items (Screen Adjust)
	optionsFreePage->insertItem(ptr->screenPosTb = new TEXTBOX(STR_GO_ADJUST_SCREEN),true);
	ptr->screenPosTb->setYPadding(false);
#endif // NH: !XBOX
#endif // NH: !PC

	// NH: Add Menu Items (Sub-titles)
	optionsFreePage->insertItem(ptr->subtitlesTb = new TEXTBOX(STR_GO_SUBTITLES),true);
	ptr->subtitlesTb->setYPadding(false);

#ifdef WIDESCREEN_OPTION

	// PP: Add Widescreen option
	optionsFreePage->insertItem(STR_WIDESCREEN)
		->setName("WIDESCREEN")
		->setYPadding(false);

#endif// PP: def WIDESCREEN_OPTION

//	optionsFreeBook->open();

	// NH: INFO
	// NH: Add Info Text Sub Book
	pagePtr->insertItem(ptr->infoSubBook=new BOOK("INFO SUB BOOK"));
	ptr->infoSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);

	// NH: Add Info Text (Using Macro)
#define ADD_INFOSUBBOOK_PAGE(name, infoText) \
	subPage=ptr->infoSubBook->addPage(#name); \
	subPage->insertItem(infoText) \
		->setWrap(true) \
		->setStyle(TS_info) \
		->setYAlign(PIFLAG_YALIGN_BOTTOM) \
		->setAlignScale(1.2f) \
		->setSelectable(false);	

	// NH: Add all info pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_INFOSUBBOOK_PAGE(DIFF_EASY,STR_GO_DIFF_EASY);
	ADD_INFOSUBBOOK_PAGE(DIFF_MEDIUM,STR_GO_DIFF_MEDIUM);
	ADD_INFOSUBBOOK_PAGE(DIFF_HARD,STR_GO_DIFF_HARD);
	ADD_INFOSUBBOOK_PAGE(FRONTEND_ON,STR_FRONTEND_ON);
	ADD_INFOSUBBOOK_PAGE(FRONTEND_OFF,STR_FRONTEND_OFF);
	ADD_INFOSUBBOOK_PAGE(SCREEN_ADJUST,STR_GO_ADJUST_SCREEN);

	// NH: Add Null Info
	subPage=ptr->infoSubBook->addPage("NULL");
	subPage->insertItem(dummyBox = new TEXTBOX(), true)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setSelectable(false);
	dummyBox->setText("");
	dummyBox->setStyle(TS_info);

	// NH: CONTROL
	// NH: Add Control Text Sub Book
	pagePtr->insertItem(ptr->controlSubBook=new BOOK("CONTROL SUB BOOK"));
	ptr->controlSubBook->setYPadding(false)
		->setSelectable(false);

	// NH: Add Control Text (Using Macro)
#define ADD_CONTROLSUBBOOK_PAGE(name, controlText) \
	subPage=ptr->controlSubBook->addPage(#name); \
	subPage->insertItem(controlText) \
		->setStyle(TS_control) \
		->setYPadding(false) \
		->setSelectable(false);

	// NH: Add all control pages (using the macro).  Seperate pages allow for fading between the actions
	ADD_CONTROLSUBBOOK_PAGE(RETURN,STR_X_TO_SELECT_Y_TO_RETURN);
	ADD_CONTROLSUBBOOK_PAGE(CONTINUE,STR_PRESS_X_TO_CONTINUE);
	ADD_CONTROLSUBBOOK_PAGE(TOGGLE,STR_PRESS_X_TO_TOGGLE);
//	ADD_CONTROLSUBBOOK_PAGE(ADJ_MOVE,STR_GO_ADJ_MOVE);
//	ADD_CONTROLSUBBOOK_PAGE(ADJ_RESET,STR_GO_ADJ_RESET);

	ptr->infoSubBook->open();// PP: books default to closed
	ptr->controlSubBook->open();

	ptr->page->selectItem(ptr->difficultyTb);

	// NH: Add Page (Adjust Screen)
	pagePtr = ptr->adjustScreenPage = menu->frontEndMenuBook->addPage("AdjustScreen");
	pagePtr->setNavigation(true);

	RECTANGLE freeTitleRect;
	freeTitleRect.left		= -0.4f;
	freeTitleRect.right		= 0.4f;
	freeTitleRect.top		= -0.5f;
	freeTitleRect.bottom	= 0.5f;

	// NH: Add Title
	pagePtr->insertItem(ptr->titleTb = new TEXTBOX(STR_GO_ADJUST_SCREEN));//freeTitleRect);
	ptr->titleTb->setStyle(TS_title)
		->setYAlign(PIFLAG_YALIGN_TOP)
		->setSelectable(false);

	//ptr->titleTb->setWrap(true);


	subPage=ptr->infoSubBook->addPage("ADJ_MOVE");
	pagePtr->insertItem(STR_GO_ADJ_MOVE)
		->setWrap(true)
		->setStyle(TS_info)
		->setYAlign(PIFLAG_YALIGN_BOTTOM)
		->setAlignScale(1.2f)
		->setSelectable(false);	

	subPage=ptr->controlSubBook->addPage("ADJ_RESET");
	pagePtr->insertItem(STR_GO_ADJ_RESET)
		->setStyle(TS_control)
		->setYPadding(false)
		->setSelectable(false);

	// NH: Set initial states
	ptr->corner = bkLoadTexture(NULL,"corneradjust.bmp",0);
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateGameOptionsMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateGameOptionsMenuPage(FRONTENDMENU *menu, PADCONTROLSTATUS *controller)
{
	GAMEOPTIONSPAGE *ptr = menu->gameOptionsPage;
	static bool returning = false;
	static int switchWideScreen = -1;

	int			number;
	
	 // NH: 'Grey out' vibration option if not available

#if BPLATFORM == PS2
	switch(biQueryDevice(controller->port, NULL))
	{
	case EBIDEVICETYPE_PSDUALSHOCK:
	case EBIDEVICETYPE_PS2DUALSHOCK2:
		//rumble option ok
		ptr->vibrationTb->enable(true);
		ptr->vibrationTb->setSelectable(true);
		break;
	default:
		//grey out rumble
		if (ptr->page->selectedItem == ptr->vibrationTb)
		{
			ptr->page->selectItem(ptr->difficultyTb);
		}
		ptr->vibrationTb->enable(false);
		ptr->vibrationTb->setSelectable(false);

		break;
	}
#endif

	// NH: Play vibration sound
	if (gameStatus.globalSettings.vibration == true)				
	{
		if ((AlarmClockSoundHandle==(-1))// PP: if the alarm clock sound is not currently playing// PP: (was != -1)
			&& CheckIfPlayingAnimation(gameStatus.player1, "idlevibrationon"))// PP: and if the alarm clock anim is currently playing
		{
			// PP: play the alarm clock sound
			AlarmClockSoundHandle = PlaySample("loop\\alarmclock.wav", 255);
		}
	}
	else 
	{
		StopChannel(&AlarmClockSoundHandle);
	}

	// NH: If popup open
	if (frontEndMenu.popUpOptions)
	{
		// PP: get the player's response to the popup
		retval = frontEndMenu.popUpOptions->getResponse();

		if (retval != -1)
		{
			if (CANCELBUTTON || OKBUTTON)
			{
				// NH: Close pop up
				BookList_closeAndFree(frontEndMenu.popUpOptions);
				menu->frontEndMenuBook->getPage(menuNames[MENU_OPTIONS])->setNavigation(true);
				menu->gameOptionsPage->infoSubBook->open();
				menu->gameOptionsPage->controlSubBook->open();

				if (currentOptionPopup == OPTIONS_POPUP_DIFFICULTY)
				{
					switch (retval)
					{
					case 0:
						// NH: Canceled
						gameStatus.globalSettings.difficulty = tempDifficulty;
						break;
					case 1:
						gameStatus.globalSettings.difficulty = DIFF_EASY;				
						break;
					case 2:
						gameStatus.globalSettings.difficulty = DIFF_MEDIUM;
						break;
					case 3:
						gameStatus.globalSettings.difficulty = DIFF_HARD;
						break;
					}
				}
				else if (currentOptionPopup == OPTIONS_POPUP_VIBRATION)
				{
					switch (retval)
					{
					case 0:
						// NH: Canceled
						gameStatus.globalSettings.vibration = tempVibration;
						break;
					case 1:
						gameStatus.globalSettings.vibration = true;				
						break;
					case 2:						
						// CMD: must flush the queue before turning off vibration
						// CMD: otherwise the rumble controller will not update
						FlushRumbleQueue(gameStatus.player[0].controller);
						gameStatus.globalSettings.vibration = false;
						break;
					}
				}
				else if (currentOptionPopup == OPTIONS_POPUP_SUBTITLES)
				{
					switch (retval)
					{
					case 0:
						// NH: Canceled
						gameStatus.globalSettings.subtitles = tempSubtitles;
						break;
					case 1:
						gameStatus.globalSettings.subtitles = true;				
						if (AdjustScreenSoundHandle==-1)
							AdjustScreenSoundHandle = PlaySample("loop\\tvtuning.wav", 255);
						
						break;
					case 2:
						gameStatus.globalSettings.subtitles = false;
						if (AdjustScreenSoundHandle==-1)
							AdjustScreenSoundHandle = PlaySample("loop\\tvtuning.wav", 255);
						
						break;
					}
				}
				else if (currentOptionPopup == OPTIONS_POPUP_WIDESCREEN)
				{
					// PP: Widescreen on/off popup

					switch (retval)
					{
					case 1:
						{
							// PP: on
							//setWidescreen(true);

							// NH: Hit TV (Just looks nice when Taz does something)
							frontEndAnimInfo.requestState = OM_TV_HIT;
							switchWideScreen = 1;
						}
						break;
//						goto updateInfoBook;

					case 2:
						{
							// PP: off
							//setWidescreen(false);

							// NH: Hit TV (Just looks nice when Taz does something)
							frontEndAnimInfo.requestState = OM_TV_HIT;					

							switchWideScreen = 0;
						}
						break;
//						goto updateInfoBook;
					}

					// PP: info book may need updating to show a new widescreen status
/*updateInfoBook:

					// PP: show whether widescreen is on or off (because some people might not know)
					if(gameStatus.wideScreenWilson)
					{
						frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_ON");
					}
					else
					{
						frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_OFF");
					}
*/				}
			}
		}
	}
	// NH: Switch to whichever mode (widescreen/normal) has been choosen
	else if (switchWideScreen != -1)
	{
		if (ptr->resetScreen)
		{
			setWidescreen(switchWideScreen != 0);
			switchWideScreen = -1;
			ptr->resetScreen = false;
			
			HitTVSoundHandle = PlaySample("thudlight.wav", 255);

			// PP: show whether widescreen is on or off (because some people might not know)
			if(gameStatus.wideScreenWilson)
			{
				frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_ON");
			}
			else
			{
				frontEndMenu.gameOptionsPage->infoSubBook->gotoPage("FRONTEND_OFF");
			}
		}
	}
	else
	{
	
		// NH: Stop playing alarm sound
		if ((AlarmClockSoundHandle!=-1) && (ptr->page->selectedItem != ptr->vibrationTb))
		{
			StopChannel(&AlarmClockSoundHandle);
		}
		
		// NH: Stop Playing Fuzzy TV tweak sound
		if ((AdjustScreenSoundHandle!=-1) && (!CheckIfPlayingAnimation(gameStatus.player1, "tvadjust1") && !CheckIfPlayingAnimation(gameStatus.player1, "tvadjust2") && !CheckIfPlayingAnimation(gameStatus.player1, "tvadjust3")))
		{
			StopChannel(&AdjustScreenSoundHandle);		
		}

		//triangle to return back to main menu
		if ((ptr->subState != GO_SUB_MOVESCREEN)&&(ptr->subState != GO_WAITFORRESET)&&(CANCELBUTTON))
		{
			frontEndAnimInfo.requestState = GOING_HOME;

			menu->currentPage = MENU_MAIN;
//			ChooseFrontEndAnimationPath(MENU_MAIN);

			menu->frontEndMenuBook->gotoPage(menuNames[MENU_MAIN]);
		
			SetFrontEndCameraDestination(FECAM_OPTIONS,FECAM_MAIN);
			
			if (AlarmClockSoundHandle!=-1)
			{
				StopChannel(&AlarmClockSoundHandle);
			}

			if (AdjustScreenSoundHandle!=-1)
			{
				StopChannel(&AdjustScreenSoundHandle);
			}
		}
		// NH: Daffyculty selected
		else if (ptr->page->selectedItem == ptr->difficultyTb)
		{
			if (frontEndAnimInfo.currentState != OM_PIZZA_IDLE)
			{
				frontEndAnimInfo.requestState = OM_PIZZA_IDLE;
			}

			if (ptr->subState != GO_DIFFICULTY)
			{
				ptr->controlSubBook->gotoPage("RETURN");
				ptr->subState = GO_DIFFICULTY;
			}

			switch(gameStatus.globalSettings.difficulty)
			{
				case DIFF_EASY:
					difficultyPopupSelection = 1;
					ptr->infoSubBook->gotoPage("DIFF_EASY");
					break;
				case DIFF_MEDIUM:
					difficultyPopupSelection = 2;
					ptr->infoSubBook->gotoPage("DIFF_MEDIUM");
					break;
				case DIFF_HARD:
					difficultyPopupSelection = 3;
					ptr->infoSubBook->gotoPage("DIFF_HARD");
					break;
			}
			
			// NH: If difficulty clicked
			if(OKBUTTON)
			{
				// NH: Store difficulty
				tempDifficulty = gameStatus.globalSettings.difficulty;

				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentOptionPopup = OPTIONS_POPUP_DIFFICULTY;
					menu->frontEndMenuBook->getPage(menuNames[MENU_OPTIONS])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					frontEndMenu.popUpOptions = new POPUP(POPUP_DIFFICULTY, difficultyPopupSelection);
				}
			}				
		}
		// NH: Vibration selected
#if BPLATFORM != PC
		else if (ptr->page->selectedItem == ptr->vibrationTb)
		{
			switch(gameStatus.globalSettings.vibration)
			{
				case true:
					ptr->infoSubBook->gotoPage("FRONTEND_ON");
					break;
				case false:
					ptr->infoSubBook->gotoPage("FRONTEND_OFF");
					break;
			}
			
			frontEndAnimInfo.requestState = OM_CLOCK_IDLE;
			
			if (ptr->subState != GO_VIBRATION)
			{
				ptr->controlSubBook->gotoPage("RETURN");
			}
			else if(gameStatus.globalSettings.vibration)
			{
				if ((AlarmClockSoundHandle==(-1))// PP: if the alarm clock sound is not playing	// PP: (was != -1)
					&& CheckIfPlayingAnimation(gameStatus.player1, "idlevibrationon"))//pickupclock") && !CheckIfPlayingAnimation(gameStatus.player1, "leavepizza"))
					AlarmClockSoundHandle = PlaySample("loop\\alarmclock.wav", 255);
			
				ptr->infoSubBook->gotoPage("FRONTEND_ON");
			}
			else
			{
				if (AlarmClockSoundHandle!=-1)
				{
					StopChannel(&AlarmClockSoundHandle);
				}

				ptr->infoSubBook->gotoPage("FRONTEND_OFF");
			}

			ptr->subState = GO_VIBRATION;
			
			// NH: Vibration Clicked
			if(OKBUTTON)
			{
				// NH: Store vibration
				tempVibration = gameStatus.globalSettings.vibration;

				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentOptionPopup = OPTIONS_POPUP_VIBRATION;
					menu->frontEndMenuBook->getPage(menuNames[MENU_OPTIONS])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					frontEndMenu.popUpOptions = new POPUP(POPUP_ON_OFF, gameStatus.globalSettings.vibration);
				}
			}
		}
#endif
		// NH: Subtitles selected
		else if (ptr->page->selectedItem == ptr->subtitlesTb)
		{
			switch(gameStatus.globalSettings.subtitles)
			{
				case true:
					ptr->infoSubBook->gotoPage("FRONTEND_ON");
					break;
				case false:
					ptr->infoSubBook->gotoPage("FRONTEND_OFF");
					break;
			}
			
			frontEndAnimInfo.requestState = OM_TV_IDLE;
			
			if (ptr->subState != GO_SUBTITLES)
			{
				ptr->controlSubBook->gotoPage("RETURN");
				ptr->subState = GO_SUBTITLES;
			}
			
			if(gameStatus.globalSettings.subtitles)
			{
				ptr->infoSubBook->gotoPage("FRONTEND_ON");
			}

			// NH: Subtitles clicked
			if(OKBUTTON)
			{
				// NH: Store subtitles
				tempSubtitles = (gameStatus.globalSettings.subtitles == TRUE);

				// NH: If no popup, then create one
				if (!frontEndMenu.popUpOptions)
				{
					currentOptionPopup = OPTIONS_POPUP_SUBTITLES;
					menu->frontEndMenuBook->getPage(menuNames[MENU_OPTIONS])->setNavigation(false);
					ptr->infoSubBook->close();
					ptr->controlSubBook->close();

					frontEndMenu.popUpOptions = new POPUP(POPUP_ON_OFF, gameStatus.globalSettings.subtitles);
				}
			}
		}

#if BPLATFORM != PC && BPLATFORM != XBOX
		// NH: Adjust Screen selection
		else if ((ptr->page->selectedItem == ptr->screenPosTb) && (ptr->subState != GO_SUB_MOVESCREEN) && (ptr->subState != GO_WAITFORRESET))
		{		
			if (ptr->subState != GO_SCREENPOS)
			{
				frontEndAnimInfo.requestState = OM_TV_IDLE;
				ptr->infoSubBook->gotoPage("SCREEN_ADJUST");
				ptr->controlSubBook->gotoPage("RETURN");
				ptr->subState = GO_SCREENPOS;
			}
			
			if(OKBUTTON)
			{
				ptr->page->setNavigation(false);
				ptr->subState = GO_SUB_MOVESCREEN;
				oldScrX = float(gameStatus.globalSettings.scrXadj);
				oldScrY = float(gameStatus.globalSettings.scrYadj);
				menu->frontEndMenuBook->gotoPage("AdjustScreen");	
			}
		}
		// Adjust Screen Selected (Moving Screen)
		else if (ptr->subState == GO_SUB_MOVESCREEN)
		{
			if(CANCELBUTTON)
			{
				// NH: Play bowhomb sound. Would be nice to be in PageFX, but not sure how to do it so this will work for now
				menu->gameOptionsPage->page->playNavSound(menu->gameOptionsPage->page->menuSounds, EMSFX_BACK);
				ChangeScreenOffset(gameStatus.globalSettings.scrXadj=bmFloatToInt(oldScrX), gameStatus.globalSettings.scrYadj=bmFloatToInt(oldScrY));
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_OPTIONS]);
				ptr->subState = GO_BACKTO_SCREENPOS;			
				ptr->page->setNavigation(true);
				return;
			}
#if BPLATFORM!=GAMECUBE
			else if((controller1.circleDebounceChannel->value))
#else
			else if((controller1.triangleDebounceChannel->value))
#endif

			{
				ptr->subState = GO_WAITFORRESET;
				frontEndAnimInfo.requestState = OM_TV_HIT;
			}
			else if(OKBUTTON)
			{
				menu->frontEndMenuBook->gotoPage(menuNames[MENU_OPTIONS]);
				ptr->subState = GO_BACKTO_SCREENPOS;			
				ptr->page->setNavigation(true);	
			}
			else if((controller1.dPadXChannel->value)||(controller1.x1)
				||(controller1.dPadYChannel->value)||(controller1.y1))
			{
				// NH: Play TV tweak sound if not already
				if (AdjustScreenSoundHandle==-1)
					AdjustScreenSoundHandle = PlaySample("loop\\tvtuning.wav", 255);

				ChangeScreenOffset((controller1.dPadXChannel->value+((int)(controller1.x1*2.0f))),
					((-controller1.dPadYChannel->value)+((int)(controller1.y1*2.0f))));
				number = bmRand()%3;
				if (!CheckIfPlayingAnimation(gameStatus.player1, "tvadjust1") && !CheckIfPlayingAnimation(gameStatus.player1, "tvadjust2") && !CheckIfPlayingAnimation(gameStatus.player1, "tvadjust3"))
				{
					switch(number)
					{
					case 0:
						frontEndAnimInfo.requestState = OM_TV_TWIDDLE;
						bkPrintf("Do1\n");
						break;
					case 1:
						frontEndAnimInfo.requestState = OM_TV_TWIDDLE2;
						bkPrintf("Do2\n");
						break;
					case 2:
						frontEndAnimInfo.requestState = OM_TV_TWIDDLE3;
						bkPrintf("Do3\n");
						break;
					}
				}
			}
			ptr->infoSubBook->gotoPage("ADJ_MOVE");
			ptr->controlSubBook->gotoPage("ADJ_RESET");

			return;		
		}
		
		// NH: Adjust Screen (Reset)
		else if (ptr->subState == GO_WAITFORRESET)
		{
			if(ptr->resetScreen)
			{
				HitTVSoundHandle = PlaySample("thudlight.wav", 255);

				ChangeScreenOffset(gameStatus.globalSettings.scrXadj=0,gameStatus.globalSettings.scrYadj=0);
				ptr->resetScreen = FALSE;
				ptr->subState = GO_SUB_MOVESCREEN;
			}
			return;		
		}
#endif // NH: !PC & !XBOX
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: ChangeScreenOffset
	Purpose 	: Changes Screen offset
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void ChangeScreenOffset(int x, int y)
{
	//cannot move more than one pixel in a direction per call
	if(x > 1) x = 2;
	if(x < -1) x = -2;
	if(y > 1) y = 2;
	if(y < -1) y = -2;

#if BPLATFORM == PS2 || BPLATFORM==GAMECUBE
	if(bmFAbs((gameStatus.globalSettings.scrXadj += x)) > 50) gameStatus.globalSettings.scrXadj = (int)((float)gameStatus.globalSettings.scrXadj/bmFAbs((float)gameStatus.globalSettings.scrXadj)*50);
	if(bmFAbs((gameStatus.globalSettings.scrYadj += y)) > 50) gameStatus.globalSettings.scrYadj = (int)((float)gameStatus.globalSettings.scrYadj/bmFAbs((float)gameStatus.globalSettings.scrYadj)*50);
	bdSetScreenOffset(gameStatus.globalSettings.scrXadj,gameStatus.globalSettings.scrYadj);
	bkPrintf("x offset: %d, y offset: %d.\n",(gameStatus.globalSettings.scrXadj),(gameStatus.globalSettings.scrYadj));
#elif BPLATFORM == PC
	bkPrintf("x offset: %d, y offset: %d.\n",(gameStatus.globalSettings.scrXadj+=x),(gameStatus.globalSettings.scrYadj+=y));
	bkPrintf("Screen Offset Adjust not available on pc.\n");
#else
	bkPrintf("Screen Offset Adjust not available on this platform.\n");
#endif
}

/*	--------------------------------------------------------------------------------
	Function 	: FrontEndRumbleCallback
	Purpose 	: callback to rumble pad at correct time
	Parameters 	: 
	Returns 	: anim segment ptr to chosen anim
	Info 		:
*/

void FrontEndRumbleCallback(EBQueueCallbackReason reasonCode,TBActorInstance *actor,TBActorNodeInstance *node,
							  TBActorAnimSegment *segment,void *context)
{
	GAMEOPTIONSPAGE *ptr = (GAMEOPTIONSPAGE*)context;

	switch(reasonCode)
	{
	case BANIMQUEUEREASON_ENTRYCOMPLETE:
	case BANIMQUEUEREASON_FLUSHED:
		if(gameStatus.globalSettings.vibration)
		{
			ptr->rumbleHandle = RequestRumbleEffect(&controller1,255,0.0f,0.5f,1.0f,0.0f,WAVEFORM_NORMAL,0.0f,0.0f,0);
		}
		else
		{
			if(ptr->rumbleHandle != -1)
			{
				ptr->rumbleHandle = FlushRumbleQueueMember(&controller1, ptr->rumbleHandle);
			}
		}
	}
}