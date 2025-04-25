// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : endgamecredits.cpp
//   Purpose : controls the credits at the end of the game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "camera.h"
#include "timer.h"
#include "main.h"
#include "bossgamecontinue.h"
#include "cutscene.h"
#include "attach.h"
#include "animation.h"
#include "util.h"
#include "redshield.h"
#include "characters.h"
#include "fireworks.h"
#include "lights.h"
#include "events.h"
#include "credits.h"
#include "rumble.h"
#include "files.h"
#include "textures.h"
#include "endgamecredits.h"
#include "music.h"
#include "status.h"
#include "VideoFX.h"					// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!

CREDITSINFO		*creditsInfo;

/* --------------------------------------------------------------------------------
   Function : MainCredits
   Purpose : Main loop for credits sequence
   Parameters : 
   Returns : 
   Info : 
*/

void MainCredits(void)
{
	applyFOV();

	bdSetViewClipPlanes(videoMode.nearPlane, videoMode.farPlane);
	bdSetViewport(int(videoMode.screens[0].viewport.left), int(videoMode.screens[0].viewport.top), 
					int(videoMode.screens[0].viewport.widthAbs()), int(videoMode.screens[0].viewport.heightAbs()));

	// PP: REMOUT: gotter do this for each eye in stereoscopic mode		SendCamera(&camera[0]);

	// CMD: update
	if(UpdateEndGameCredits())
	{
		if(gameStatus.player[0].controller) EnableRumble(gameStatus.player[0].controller);
		if(gameStatus.player[1].controller) EnableRumble(gameStatus.player[1].controller);
		bkGenerateEvent("scenechange", "_frontend");
	}

	// CMD: prep
	PrepEndGameCredits();

	// PP: in stereoscopic mode, draw everything once per eye per frame
	for(int i=0; i<videoMode.numDraws; i++)
	{
		// PP: start the stereoscopic draw pass
		if(VFX_effectActive(VFX_STEREO))
		{
			VFX_stereoPreDraw();
		}

		// PP: send the camera - must do this for each eye in stereoscopic mode
		SendCamera(&camera[0]);

		// prepare scene
		bdSetupView();
		bdBeginScene();

		// CMD: draw
		DrawEndGameCredits();
		DrawEndGameCredits2();

		//draw fade
		camera[0].fade.DrawFade();

		// PP: finalise the stereoscopic draw pass
		if(VFX_effectActive(VFX_STEREO))
		{
			VFX_stereoPostDraw();
		}

		bdEndScene();
	}

//	if(gameStatus.player[0].controller->l3DebounceChannel->value)
//	{
//		TakeScreenShot();
//	}

	// PP: finish timing the frame; calculate frames per second, frame length, etc.
	finishTimingFrame();
	
	// null time, anything between here and end is not included in the timer
	camera[0].fade.UpdateFade();

	// PP: start timing the frame
	startTimingFrame();
}

/* --------------------------------------------------------------------------------
   Function : InitialiseEndGameCredits
   Purpose : Does any initialisation required to run the end game credits
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseEndGameCredits(void)
{
	TBVector				position, rotation;
	TBQuaternion			orientation;
	TBMatrix				rotTheta, rotPhi, result;
	TBActorNodeInstance		*node;

	// CMD: create credits info
	creditsInfo = (CREDITSINFO*)MALLOC(sizeof(CREDITSINFO));
	ASSERTM(creditsInfo, "no memory left for credits info!");

	// CMD: hide everything
	HideActorInstanceList(&map.animatedInstances);
	HideActorInstanceList(&map.levelInstances);
	HideActorInstanceList(&map.characterInstances);
	HideActorInstanceList(&characterInstances);
	// CMD: now reveal Taz
	gameStatus.player[0].actorInstance->flags &= ~ACTORFLAG_DONTDRAW;

	// CMD: make sure that Taz is not spining
	ChangeActorInstance(gameStatus.player[0].actorInstance, FindActorInActorList("tazdevil.obe"),5,NULL);

	// CMD: place camera at the origin
	bmVectorSet(position, 0.0f, METERTOUNIT(0.25f), 0.0f, 1.0f);
	SetCameraPosition(&camera[0], position);
	// CMD: look down the z axis
	bmVectorSet(position, 0.0f, METERTOUNIT(0.25f), METERTOUNIT(-1.0f), 1.0f);
	SetCameraLookAtPosition(&camera[0], position);

	// CMD: place taz correct position
	bmVectorSet(creditsInfo->position, METERTOUNIT(1.0f), METERTOUNIT(-0.5f), METERTOUNIT(-2.0f), 1.0f);
	SetActorPosition(gameStatus.player[0].actorInstance, creditsInfo->position);
	bmVectorCopy(creditsInfo->initPos, creditsInfo->position);
	// CMD: taz needs to face the camera, which means a rotation of approx. pi rads
	bmVectorSet(rotation, 0.0f, 1.0f, 0.0f, (RAD(270.0f)+bmATan(creditsInfo->position[2]/creditsInfo->position[0])));
	bmRotationToQuat(orientation, rotation);
	SetActorOrientation(gameStatus.player[0].actorInstance, orientation);
	// CMD: leave crash helmet, and play idle animation
	while(ReduceShieldHealthByOne(gameStatus.player[0].actorInstance) > 2);
	FlushAnimationQueue(gameStatus.player[0].actorInstance);
	PlayAnimationByName(gameStatus.player[0].actorInstance, "idle1", 1.0f, 1, 0, 0.1f, IDLE);
	// CMD: turn off motion bone
	gameStatus.player[0].actorInstance->actorAnimation->useMotionBone= FALSE;

	creditsInfo->tazClock = 2.0f;
	creditsInfo->tazState = CREDITS_TAZ_INIT;
	creditsInfo->currentCostume = bmRand()%5;

	// CMD: set direction of cartoon light
	creditsInfo->theta = creditsInfo->initTheta = RAD(273.0f);
	creditsInfo->phi = creditsInfo->initPhi = RAD(186.0f);
	bmVectorSet(position, 0.0f, 0.0f, -1.0f, 1.0f);
	bmMatYRotation(rotTheta, creditsInfo->theta);
	bmMatYRotation(rotPhi, creditsInfo->phi);
	bmMatMultiply(result, rotPhi, rotTheta);
	bmMatMultiplyVector(result, position);
	baSetDirectionalLight(&cartoonLight, position);

	// CMD: fireworks
	creditsInfo->fireworkClock = CREDITS_FIREWORKTIME;

	// CMD: disable rumble
	if(gameStatus.player[0].controller) DisableRumble(gameStatus.player[0].controller);
	if(gameStatus.player[1].controller) DisableRumble(gameStatus.player[1].controller);

	// CMD: load splash screen
	LoadSplashScreen(&creditsInfo->splash, "credits.bmp", "splshscr");
	// CMD: Load Sounds
	LoadWholePackage("tazcreditssounds");
	LoadSamples("tazcreditssounds");

	// CMD: create book
	CreateEndCreditsBooks();
	creditsInfo->bookClock = CREDITS_STATS_TIME;
	creditsInfo->currentSection = CREDITS_BRUNO;
	creditsInfo->currentName = creditsInfo->actualName = 0;
	creditsInfo->bookState = CREDITS_STATSBOOK;

	musicSystem.ChooseMusic(0, MUSICTYPE_MAIN, MUSICSTATE_START, "taz theme tune.wav");

	// CMD: start circle fade up
//	if(node = baFindNode(gameStatus.player[0].actorInstance->actorInstance.rootNodeInstance, "spinetop"))
//	{
//		baGetNodesWorldPosition(&gameStatus.player[0].actorInstance->actorInstance, node, position);
//	}
	// CMD: get screen coords from world coords
	// CMD: its not that i'm paranoid, i just want to make sure
//	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
//	bdSetIdentityObjectMatrix();
//	bdProjectVertices(rotation, position, 1);
//	SetCircleFadeTo(&gameStatus.player[0].camera->fade, rotation[0], rotation[1]);
	gameStatus.player[0].camera->fade.StartFadeUp(FADETYPE_NORMAL, 1.0f);
}

/* --------------------------------------------------------------------------------
   Function : CalculateStatsTotals
   Purpose : calculates all the totals required
   Parameters : 
   Returns : 
   Info : 
*/
EScene IndexToScene(int index)
{
	switch(index)
	{
	case 0:
		return SCENE_ICEDOME;
	case 1:
		return SCENE_SAFARI;
	case 2:
		return SCENE_AQUA;
	case 3:
		return SCENE_DEPTSTR;
	case 4:
		return SCENE_MUSEUM;
	case 5:
		return SCENE_CONSTRUCT;
	case 6:
		return SCENE_GHOSTTOWN;
	case 7:
		return SCENE_GOLDMINE;
	case 8:
		return SCENE_GRANDCANYON;
	case 9:
		return SCENE_TAZHUB;
	default:
		// PP: hope you don't mind me shifting this here - it just removes a warning
		ASSERTM(0, "invalid index in IndexToScene, endgamecredits.cpp");
		return (EScene)0;
	}
}

/* --------------------------------------------------------------------------------
   Function : CalculateStatsTotals
   Purpose : calculates all the totals required
   Parameters : 
   Returns : 
   Info : 
*/

int	CalculateStatsTotals(ETotal type, int index)
{
	EScene		scene;
	int			total, i;
	SAVESLOT	*info = &gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot];

	if(type == TOTALS_GALERIES)
	{
		if(gameStatus.globalSettings.saveInfo.galleryFlags.galleryFlags[index])
		{
			return TRUE;
		}
		else return FALSE;
	}
	if(type == TOTALS_BOUNTY)
	{
		return info->bounty;
	}

	total = 0;
	for(i = 0;i < 10;++i)
	{
		scene = IndexToScene(i);
		switch(type)
		{
//		case TOTALS_BOUNTY:
//			total += info->level[scene].levelBounty;
//			break;
		case TOTALS_SANDWICHES:
			total += info->level[scene].pickupsCollected;
			break;
		case TOTALS_DESTRUCTIBLES:
			total += info->level[scene].objectsDestroyed;
			break;
		case TOTALS_CAPTURED:
			total -= info->level[scene].timesCaptured;
			break;
		case TOTALS_TIME:
			total += bmFloatToInt(info->level[scene].time);
			break;
		case TOTALS_STATUES:
			if(info->level[scene].secretItemCollected) total++;
			break;
		case TOTALS_BONUSGAMES:
			if(scene != SCENE_TAZHUB) if(info->level[scene].bonusgameOption) total++;
			break;
		}
	}
	if(type == TOTALS_DESTRUCTIBLES)
	{
		return total /= 10;
	}
	return total;
}

/* --------------------------------------------------------------------------------
   Function : TimeIntoHoursMinsAndSecs
   Purpose : converts a time in seconds to a time in hours minutes and seconds, and prints it into a string
   Parameters : ptr to stringBuf into which the result will be printed, time in seconds
   Returns : 
   Info : colon seperaots included free of charge
*/

void TimeIntoHoursMinsAndSecs(char *stringBuf, float time)
{
	float		mins, secs;

	secs = bmFMod(time, 60.0f);
	time -= secs;
	time /= 60.0f;

	mins = bmFMod(time, 60.0f);
	time -= mins;
	time /= 60.0f;

	sprintf(stringBuf, "%d:%02d:%02d", bmFloatToInt(time), bmFloatToInt(mins), bmFloatToInt(secs));
}

void LocaliseNumbers(char *stringBuf, int number)
{
	int		bounty, millions, thousands, ones;
	char	separator;


	switch(gameStatus.currentLanguage)
	{
	case BLANGUAGEID_F:
		separator = ' ';
		break;
	case BLANGUAGEID_E:
	case BLANGUAGEID_D:
	case BLANGUAGEID_IT:
		separator = '.';	
		break;	
	default:
		separator = ',';
		break;
	}

	millions = thousands = ones = 0;

	ones = number%1000;
	thousands = ((number-ones)%1000000)/1000;
	millions = (number-thousands-ones)/1000000;

	if(millions)	sprintf(stringBuf, "$%d%c%03d%c%03d", millions, separator, thousands, separator, ones);
	else
	if(thousands)	sprintf(stringBuf, "$%d%c%03d", thousands, separator, ones);
	else			sprintf(stringBuf, "$%d", ones);
}

/* --------------------------------------------------------------------------------
   Function : CreateEndCreditsBooks
   Purpose : does what it says ... note: no tins involved
   Parameters : 
   Returns : 
   Info : 
*/

void CreateEndCreditsBooks(void)
{
	PAGE			*pagePtr, *subPage;
	TEXTBOX			*dummyBox;
	ICON			*icon;
	float			time;
	char			stringBuf[64];
	int				i;
	bool			newLine;


	// CMD: add stats book
	creditsInfo->statsBook = new BOOK("CREDITS STATS BOOK");
	creditsInfo->statsBook->setYAlign(PIFLAG_YALIGN_TOP);
	creditsInfo->statsBook->setSelectable(false);
	creditsInfo->statsBook->setBookRect(RECTANGLE(0.0f, 0.4f, -0.5f, 0.35f));

	// CMD: bounty
	subPage = creditsInfo->statsBook->addPage("stats");
	subPage->insertItem(icon = new ICON("dollar.bmp"), false)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	LocaliseNumbers(stringBuf, CalculateStatsTotals(TOTALS_BOUNTY));
	dummyBox = new TEXTBOX(stringBuf);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: sandwiches
	subPage->insertItem(icon = new ICON("sandwich.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	dummyBox = new TEXTBOX();
	dummyBox->sprintf8("%d/1000", CalculateStatsTotals(TOTALS_SANDWICHES));
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: destructibles
	subPage->insertItem(icon = new ICON("destructibles02.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	dummyBox = new TEXTBOX();
	dummyBox->sprintf8("%d%%", CalculateStatsTotals(TOTALS_DESTRUCTIBLES));
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: captured
	subPage->insertItem(icon = new ICON("captured.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	LocaliseNumbers(stringBuf, CalculateStatsTotals(TOTALS_CAPTURED));
	dummyBox = new TEXTBOX(stringBuf);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: time taken
	subPage->insertItem(icon = new ICON("clock.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	time = (float)CalculateStatsTotals(TOTALS_TIME);
	TimeIntoHoursMinsAndSecs(stringBuf, time);
	dummyBox = new TEXTBOX(stringBuf);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: statues
	subPage->insertItem(icon = new ICON("samface.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	dummyBox = new TEXTBOX();
	dummyBox->sprintf8("%d/10", CalculateStatsTotals(TOTALS_STATUES));
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	// CMD: galeries
	for(i = 0;i <10;i++)
	{
		switch(i)
		{
		case 0:
		case 5:
			newLine = true;
			break;
		default:
			newLine = false;
		}
		if(CalculateStatsTotals(TOTALS_GALERIES, i))
		{
			subPage->insertItem(icon = new ICON("galleryunlocked.bmp"), newLine)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setSelectable(false);
		}
		else
		{
			subPage->insertItem(icon = new ICON("gallerylocked.bmp"), newLine)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setSelectable(false);
		}
	}
	// CMD: bonus games
	subPage->insertItem(icon = new ICON("bonusbox.bmp"), true)->setScale(CREDITS_ICON_SCALE)->setAlignScale(CREDITS_ICON_SCALE*1.3f)->setYAlign(PIFLAG_XALIGN_LEFT)->setSelectable(false);
	dummyBox = new TEXTBOX();
	dummyBox->sprintf8("%d/9", CalculateStatsTotals(TOTALS_BONUSGAMES));
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_XALIGN_RIGHT)->setSelectable(false);
	subPage->insertItem(dummyBox, false);
	

	// CMD:  Add Credits Title Sub Book
	creditsInfo->titleBook = new BOOK("CREDITS TITLE SUB BOOK");
	creditsInfo->titleBook->setYAlign(PIFLAG_YALIGN_TOP);
	creditsInfo->titleBook->setSelectable(false);
	creditsInfo->titleBook->setBookRect(RECTANGLE(0.0f, 0.4f, -0.5f, 0.35f));

	// CMD:  Bruno Presents
	subPage = creditsInfo->titleBook->addPage("CREDITS_BRUNO");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  NTSC (American) Version Only
	// CMD: only used in american version, but ...
	// CMD: needs to be created for PAL to prevent credit name moving out of sync
//	if(videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		// CMD:  Infogrames Interactive US
		subPage = creditsInfo->titleBook->addPage("US_CREDITS_I_INT");
		dummyBox = new TEXTBOX(STR_EX_CREDITS);
		dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
		subPage->insertItem(dummyBox);
		dummyBox = new TEXTBOX(STR_US_CREDITS_I_INTERACTIVE_INC);
		dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
		subPage->insertItem(dummyBox);

		// CMD:  Infogrames US
		subPage = creditsInfo->titleBook->addPage("US_CREDITS_I");
		dummyBox = new TEXTBOX(STR_EX_CREDITS);
		dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
		subPage->insertItem(dummyBox);
		dummyBox = new TEXTBOX(STR_US_CREDITS_I_INC);
		dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
		subPage->insertItem(dummyBox);
	}

	// CMD:  Blitz
	subPage = creditsInfo->titleBook->addPage("CREDITS_BLITZ");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(dummyBox);
	dummyBox = new TEXTBOX(STR_CREDITS_BLITZ);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  Infogrames
	subPage = creditsInfo->titleBook->addPage("CREDITS_INFOGRAMES");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(dummyBox);
	dummyBox = new TEXTBOX(STR_CREDIT_I);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  WB
	subPage = creditsInfo->titleBook->addPage("CREDITS_WARNERS");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(dummyBox);
	dummyBox = new TEXTBOX(STR_CREDITS_WB);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  Nimrod
	subPage = creditsInfo->titleBook->addPage("CREDITS_NIMROD");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(dummyBox);
	dummyBox = new TEXTBOX(STR_CREDITS_NAMES_NIMROD);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_smallTitle)->setYAlign(PIFLAG_YALIGN_TOP)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  Copyright
	subPage = creditsInfo->titleBook->addPage("CREDITS_LEGAL");
	dummyBox = new TEXTBOX(STR_EX_CREDITS);
	dummyBox->setFontSize(CREDITS_SCALE)->setStyle(TS_title)->setYPadding(FALSE)->setSelectable(false);
	subPage->insertItem(dummyBox);

	// CMD:  CREDITS
	// CMD:  Add Credits Text Sub Book
	creditsInfo->namesSubBook = new BOOK("CREDITS SUB BOOK");
	creditsInfo->namesSubBook->setYAlign(PIFLAG_YALIGN_BOTTOM);
	creditsInfo->namesSubBook->setYPadding(false)->setSelectable(false);
	creditsInfo->namesSubBook->setBookRect(RECTANGLE(0.0f, 0.4f, -0.5f, 0.35f));

	// CMD:  Intro
	CreateIntroCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	// NH: NTSC (American) Version Only
	if(videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		// NH: US Infogrames Int
		CreateUSInfogramesIntCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
		// NH: US Infogrames
		CreateUSInfogramesCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	}
	// CMD:  Blitz
	CreateBlitzCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	// CMD:  Infogrames
	CreateInfogramesCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	// CMD:  Warner Bros
	CreateWarnerCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	// CMD:  NIMROD
	CreateNimrodCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);
	// CMD:  Legal
	CreateLegalCredits(creditsInfo->namesSubBook, FALSE, CREDITS_SCALE);

	creditsInfo->statsBook->open();
//	creditsInfo->titleBook->open();
//	creditsInfo->namesSubBook->open();
}

/* --------------------------------------------------------------------------------
   Function : FreeEndGameCreditsAndChangeScene
   Purpose : does what it says ... note: no tins involved
   Parameters : 
   Returns : 
   Info : 
*/

void FreeEndGameCredits(void)
{
	if(creditsInfo)
	{
		SAFE_DELETE(creditsInfo->statsBook);
		SAFE_DELETE(creditsInfo->titleBook);
		SAFE_DELETE(creditsInfo->namesSubBook);

		FreeSplashScreen(&creditsInfo->splash);
		FreeWholePackage("tazcreditssounds");

		SAFE_FREE(creditsInfo);
		creditsInfo = NULL;
	}
}

/* --------------------------------------------------------------------------------
   Function : PrepEndGameCredits
   Purpose : preps everything needed for the credits
   Parameters : 
   Returns : 
   Info : 
*/

void PrepEndGameCredits(void)
{
	PrepActorInstances(&characterInstances);
	PrepAttachedObjects(gameStatus.player[0].actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : FinishedFadeFromCredits
   Purpose : fade down has finished, the game is over
   Parameters : 
   Returns : 
   Info : 
*/
void FinishedFadeFromCredits(FADE *fade)
{
	if(creditsInfo)
	{
		creditsInfo->bookState = CREDITS_BOOKCLOSED;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateEndGameCredits
   Purpose : Does any update required
   Parameters : 
   Returns : TRUE if sequence completed
   Info : 
*/

int UpdateEndGameCredits(void)
{
	ACTORINSTANCE	*taz = gameStatus.player[0].actorInstance;
	TBVector		position;

	AnimateActorInstance(gameStatus.player[0].actorInstance);
	UpdateAttachedObjects(gameStatus.player[0].actorInstance);

	UpdateTazInCredits();
	UpdateSpinEffectForActor(gameStatus.player[0].actorInstance, fTime);

	UpdateAnimatingTextures();

	UpdateCartoonLightDirection(gameStatus.player[0].controller);

	RespondToSceneChangeMessages();
	RespondToSpecialMessages();

	// CMD: update fireworks
	baUpdateExtraSystem(fireworksExtraSystem, fTime);
	baUpdateExtraSystem(smokeSystem, fTime);

	if((creditsInfo->fireworkClock -= fTime) < 0.0f)
	{
		creditsInfo->fireworkClock = UtilGenerateRandomNumber(CREDITS_FIREWORKTIME, CREDITS_FIREWORKTIME*0.5f);
		bmVectorSet(position, METERTOUNIT(UtilGenerateRandomNumber(10.0f, -10.0f)), METERTOUNIT(UtilGenerateRandomNumber(10.0f, 5.0f)), METERTOUNIT(-40.0f), 1.0f);
		CreateFireworkBurst(position, FIREWORK_RANDOMTYPE, 0,0,0,FIREWORKS_RANDOMCOLOUR);
	}
	if(gameStatus.player[0].controller->l1DebounceChannel->value)
	{
		bmVectorSet(position, METERTOUNIT(UtilGenerateRandomNumber(10.0f, -10.0f)), METERTOUNIT(UtilGenerateRandomNumber(10.0f, 5.0f)), METERTOUNIT(-40.0f), 1.0f);
		CreateFireworkBurst(position, FIREWORK_STREAK, 0,0,0,FIREWORKS_RANDOMCOLOUR);
	}
	if(gameStatus.player[0].controller->l2DebounceChannel->value)
	{
		bmVectorSet(position, METERTOUNIT(UtilGenerateRandomNumber(10.0f, -10.0f)), METERTOUNIT(UtilGenerateRandomNumber(10.0f, 5.0f)), METERTOUNIT(-40.0f), 1.0f);
		CreateFireworkBurst(position, FIREWORK_SPARKLE, 0,0,0,FIREWORKS_RANDOMCOLOUR);
	}
	if(gameStatus.player[0].controller->r1DebounceChannel->value)
	{
		bmVectorSet(position, METERTOUNIT(UtilGenerateRandomNumber(10.0f, -10.0f)), METERTOUNIT(UtilGenerateRandomNumber(10.0f, 5.0f)), METERTOUNIT(-40.0f), 1.0f);
		CreateFireworkBurst(position, FIREWORK_BANGER, 0,0,0,FIREWORKS_RANDOMCOLOUR);
	}
	if(gameStatus.player[0].controller->r2DebounceChannel->value)
	{
		bmVectorSet(position, METERTOUNIT(UtilGenerateRandomNumber(10.0f, -10.0f)), METERTOUNIT(UtilGenerateRandomNumber(10.0f, 5.0f)), METERTOUNIT(-40.0f), 1.0f);
		CreateFireworkBurst(position, FIREWORK_TWISTER, 0,0,0,FIREWORKS_RANDOMCOLOUR);
	}

	// CMD: update books
	switch(creditsInfo->bookState)
	{
	case CREDITS_STATSBOOK:
		if((creditsInfo->bookClock -= fTime) < 0.0f)
		{
			creditsInfo->statsBook->close();
			creditsInfo->titleBook->open();
			creditsInfo->namesSubBook->open();
			creditsInfo->bookClock = CREDITS_NAME_TIME;
			creditsInfo->bookState = CREDITS_BOOKOPEN;
		}
		break;
	case CREDITS_BOOKOPEN:
		if((creditsInfo->bookClock -= fTime) < 0.0f)
		{
			creditsInfo->bookClock = CREDITS_NAME_TIME;
			// CMD: cycle through names & sections
			++creditsInfo->actualName;
			switch(creditsInfo->currentSection)
			{
			case CREDITS_BRUNO:
				// CMD: 2 credits in this section
				if(++creditsInfo->currentName >= 2)
				{
					if(videoMode.flags & BDISPLAYFLAG_NTSC)
					{
						creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_INFOGRAMES_INT_INC));
					}
					else
					{
						creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_BLITZ));
					}
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			case CREDITS_INFOGRAMES_INT_INC:
				// CMD: 10 credits in this section
				if(++creditsInfo->currentName >= 10)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_INFOGRAMES_INC));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			case CREDITS_INFOGRAMES_INC:
#if BPLATFORM == PC
				// CMD: 10 credits in this section
				if(++creditsInfo->currentName >= 10)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_BLITZ));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
#else
				// CMD: 7 credits in this section
				if(++creditsInfo->currentName >= 7)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_BLITZ));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
#endif
				break;
			case CREDITS_BLITZ:
				// CMD: 26 credits in this section
				if(++creditsInfo->currentName >= 26)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_INFOGRAMES));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				if(creditsInfo->currentName == 4)
				{
					if(!CheckIfPlayingAnimation(taz, "walk2start")&&!CheckIfPlayingAnimation(taz, "walk2")&&!CheckIfPlayingAnimation(taz, "rapperghettoblast")&&
						!CheckIfPlayingAnimation(taz, "throwsnowball")&&!CheckIfPlayingAnimation(taz, "ninjakick")&&!CheckIfPlayingAnimation(taz, "cowboyshootfinger")&&
						!CheckIfPlayingAnimation(taz, "werewolfbiteclaw"))
					{
						FlushAnimationQueue(taz);
						PlayAnimationByName(taz, "happytaz", 1.0f, 0, 0, 0.1f, REACT);
					}
				}
				break;
			case CREDITS_INFOGRAMES:
				// CMD: 24 credits in this section
				if(++creditsInfo->currentName >= 23)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_WARNER));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			case CREDITS_WARNER:
				// CMD: 11 credits in this section
				if(++creditsInfo->currentName >= 10)
				{
					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_NIMROD));
					creditsInfo->currentName = 0;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			case CREDITS_NIMROD:
				// CMD: 1 credits in this section
				if(++creditsInfo->currentName >= 1)
				{
//					creditsInfo->titleBook->gotoPageNumber(uint32(creditsInfo->currentSection = CREDITS_LEGAL));
					creditsInfo->titleBook->close();
					creditsInfo->currentSection = CREDITS_LEGAL;
					creditsInfo->bookClock = CREDITS_NAME_TIME*CREDITS_LEGAL_TIME_MULTIPLIER;
					creditsInfo->currentName = 0;

					// TP: just in case we enter this state and taz is still spinning
					if (creditsInfo->tazState==CREDITS_TAZ_EXITSTAGELEFT)
					{
						ChangeActorInstance(gameStatus.player[0].actorInstance, FindActorInActorList("tazdevil.obe"),5,NULL);
						switch(creditsInfo->currentCostume)
						{
						case 0:
							SetupCostume(taz, COSTUME_COWBOY);
							creditsInfo->tazState = CREDITS_TAZ_BANDITO;
							break;
						case 1:
							SetupCostume(taz, COSTUME_NINJA);
							creditsInfo->tazState = CREDITS_TAZ_NINJA;
							break;
						case 2:
							SetupCostume(taz, COSTUME_RAPPA);
							FlushAnimationQueue(taz);
							creditsInfo->tazState = CREDITS_TAZ_RAPPER;
							break;
						case 3:
							SetupCostume(taz, COSTUME_WERETAZ);
							creditsInfo->tazState = CREDITS_TAZ_WERETAZ;
							break;
						case 4:
							SetupCostume(taz, COSTUME_SNOWBOARD);
							creditsInfo->tazState = CREDITS_TAZ_BOARDER;
							break;
						}
						FlushAnimationQueue(taz);
						PlayAnimationByName(taz, "gotosleep", 1.0f, 0, 0, 0.1f, REACT);
						PlayAnimationByName(taz, "sleepcycle", 1.0f, 1, 1, 0.1f, REACT);

						// CMD: quick update to avoid jesus pose
						AnimateActorInstance(taz);
						baPrepActorInstance(&taz->actorInstance, TRUE);
						UpdateAttachedObjects(taz);
						PrepAttachedObjects(taz);

						StopChannel(&creditsInfo->spinHandle);
					}
					else
					{
						FlushAnimationQueue(taz);
						PlayAnimationByName(taz, "gotosleep", 1.0f, 0, 0, 0.1f, REACT);
						PlayAnimationByName(taz, "sleepcycle", 1.0f, 1, 1, 0.1f, REACT);
					}

					// CMD: put Taz to sleep
					creditsInfo->tazState = CREDITS_TAZ_BYEBYE;
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			case CREDITS_LEGAL:
				// CMD: 1 credits in this section
				if(++creditsInfo->currentName >= 1)
				{
					creditsInfo->namesSubBook->close();
					creditsInfo->bookState = CREDITS_BOOKWAITING;
					// CMD: start fade
					camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
					camera[0].fade.SetFadeDownFinishedCallback(FinishedFadeFromCredits);
				}
				creditsInfo->namesSubBook->gotoPageNumber(creditsInfo->actualName);
				break;
			}
		}
		break;
	case CREDITS_BOOKWAITING:
		break;
	case CREDITS_BOOKCLOSED:
		// CMD: wait for book to close
		if((~creditsInfo->titleBook->flags & BKFLAG_OPEN)&&(~creditsInfo->titleBook->flags & BKFLAG_OPEN))
		{
			return TRUE;
		}
		break;
	}
//	BookList_update();

	// CMD: check for start being pressed
	if(gameStatus.player[0].controller->startDebounceChannel->value)
	{
		creditsInfo->titleBook->close();
		creditsInfo->namesSubBook->close();
		creditsInfo->bookState = CREDITS_BOOKWAITING;
		// CMD: start fade
		camera[0].fade.StartFadeDown(FADETYPE_NORMAL, 1.0f);
		camera[0].fade.SetFadeDownFinishedCallback(FinishedFadeFromCredits);
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateTazInCredits
   Purpose : Does all taz related update
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateTazInCredits(void)
{
	ACTORINSTANCE		*taz = gameStatus.player[0].actorInstance;
	TBActorAnimSegment	*animPtr;
	int					switchState;

	switchState	= FALSE;
	switch(creditsInfo->tazState)
	{
	case CREDITS_TAZ_INIT:
		if((creditsInfo->tazClock -= fTime) < 0.0f) 
		{
			FlushAnimationQueue(taz);
			PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
			creditsInfo->tazState = CREDITS_TAZ_SPINUP;
		}
		break;
	case CREDITS_TAZ_SPINUP:
		if(!CheckIfPlayingAnimation(taz, "spinup"))
		{
			if(taz->characterInfo->tazShieldInfo)
			{
				RemoveObjectFromCharacter(taz, taz->characterInfo->tazShieldInfo->tazShieldHandle);
				taz->characterInfo->tazShieldInfo = FreeTazShield(taz->characterInfo->tazShieldInfo);
			}
			ChangeActorInstance(taz,FindActorInActorList("spindevil.obe"),5, NULL);
			FlushAnimationQueue(taz);
			PlayAnimationByName(taz, "spin1", 3.0f, 1, 0, 0.1f, IDLE);
			creditsInfo->spinHandle = PlaySample("tazspin.wav", 255, taz->actorInstance.position, 0);
			creditsInfo->tazClock = 1.0f;
			creditsInfo->tazState = CREDITS_TAZ_EXITSTAGELEFT;
		}
		break;
	case CREDITS_TAZ_MESMERISED:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			FlushAnimationQueue(taz);
			PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
			creditsInfo->tazState = CREDITS_TAZ_SPINUP;
		}
		if(!gameStatus.player[0].controller->circleChannel->value)
		{
			FlushAnimationQueue(taz);
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);
			creditsInfo->tazState = creditsInfo->tazPrevState;
		}
		break;
	case CREDITS_TAZ_BYEBYE:
		break;
	case CREDITS_TAZ_EXITSTAGELEFT:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(++creditsInfo->currentCostume >= CREDITS_NUM_COSTUMES) creditsInfo->currentCostume = 0;

			switch(creditsInfo->currentCostume)
			{
			case 0:
				SetupCostume(taz, COSTUME_COWBOY);
				creditsInfo->tazState = CREDITS_TAZ_BANDITO;
				break;
			case 1:
				SetupCostume(taz, COSTUME_NINJA);
				creditsInfo->tazState = CREDITS_TAZ_NINJA;
				break;
			case 2:
				SetupCostume(taz, COSTUME_RAPPA);
				FlushAnimationQueue(taz);
				creditsInfo->tazState = CREDITS_TAZ_RAPPER;
				break;
			case 3:
				SetupCostume(taz, COSTUME_WERETAZ);
				creditsInfo->tazState = CREDITS_TAZ_WERETAZ;
				break;
			case 4:
				SetupCostume(taz, COSTUME_SNOWBOARD);
				creditsInfo->tazState = CREDITS_TAZ_BOARDER;
				break;
			}
			FlushAnimationQueue(taz);
			PlayAnimationByName(taz, "spindown", 1.0f, 0, 0, 0.1f, REACT);
			PlayAnimationByName(taz, "idle6", 1.0f, 1, 0, 0.1f, IDLE);

			// CMD: quick update to avoid jesus pose
			AnimateActorInstance(taz);
			baPrepActorInstance(&taz->actorInstance, TRUE);
			UpdateAttachedObjects(taz);
			PrepAttachedObjects(taz);

			StopChannel(&creditsInfo->spinHandle);

			creditsInfo->tazClock = CREDITS_IDLE_TIME;
		}
		break;
	case CREDITS_TAZ_RAPPER:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(!CheckIfPlayingAnimation(taz, "rapperghettoblast"))
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
				creditsInfo->tazState = CREDITS_TAZ_SPINUP;
			}
		}
		if(!CheckIfPlayingAnimation(taz, NULL))
		{
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);

		}
		if(!CheckIfPlayingAnimation(taz, "rapperghettoblast"))
		{
			if(gameStatus.player[0].controller->squareDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "rapperghettoblast", 1.0f, 0, 0, 0.1f, REACT);
				PlaySample("scratchattack.wav", 255, taz->actorInstance.position, 0);
			}
			if(gameStatus.player[0].controller->circleChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "walk2start", 1.0f,0,0,0.1f,MOVE);
				PlayAnimationByName(taz, "walk2", 1.0f,1,1,0.1f,MOVE);
				creditsInfo->tazPrevState = CREDITS_TAZ_RAPPER;
				creditsInfo->tazState = CREDITS_TAZ_MESMERISED;
				break;
			}
			if(gameStatus.player[0].controller->triangleDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(taz, "wobble", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 1:
					PlayAnimationByName(taz, "slipslide", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 2:
					PlayAnimationByName(taz, "rant2", 1.0f, 0, 0, 0.1f, REACT);
					PlaySample("tazrant1.wav", 255, taz->actorInstance.position, 0);
					break;
				}
			}
		}
		break;
	case CREDITS_TAZ_BOARDER:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(!CheckIfPlayingAnimation(taz, "throwsnowball"))
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
				creditsInfo->tazState = CREDITS_TAZ_SPINUP;
			}
		}
		if(!CheckIfPlayingAnimation(taz, NULL))
		{
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);

		}
		if(!CheckIfPlayingAnimation(taz, "throwsnowball"))
		{
			if(gameStatus.player[0].controller->squareDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "throwsnowball", 1.0f, 0, 0, 0.1f, REACT);
				// CMD: sample played in events
			}
			if(gameStatus.player[0].controller->circleChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "walk2start", 1.0f,0,0,0.1f,MOVE);
				PlayAnimationByName(taz, "walk2", 1.0f,1,1,0.1f,MOVE);
				creditsInfo->tazPrevState = CREDITS_TAZ_BOARDER;
				creditsInfo->tazState = CREDITS_TAZ_MESMERISED;
				break;
			}
			if(gameStatus.player[0].controller->triangleDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(taz, "wobble", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 1:
					PlayAnimationByName(taz, "slipslide", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 2:
					PlayAnimationByName(taz, "rant2", 1.0f, 0, 0, 0.1f, REACT);
					PlaySample("tazrant1.wav", 255, taz->actorInstance.position, 0);
					break;
				}
			}
		}
		break;
	case CREDITS_TAZ_NINJA:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(!CheckIfPlayingAnimation(taz, "ninjakick"))
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
				creditsInfo->tazState = CREDITS_TAZ_SPINUP;
			}
		}
		if(!CheckIfPlayingAnimation(taz, NULL))
		{
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);

		}
		if(!CheckIfPlayingAnimation(taz, "ninjakick"))
		{
			if(gameStatus.player[0].controller->squareDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "ninjakick", 1.0f, 0, 0, 0.1f, REACT);
				if(bmRand()%2)	PlaySample("tazninja_1.wav", 255, taz->actorInstance.position, 0);
				else			PlaySample("tazninja_2.wav", 255, taz->actorInstance.position, 0);
			}
			if(gameStatus.player[0].controller->circleChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "walk2start", 1.0f,0,0,0.1f,MOVE);
				PlayAnimationByName(taz, "walk2", 1.0f,1,1,0.1f,MOVE);
				creditsInfo->tazPrevState = CREDITS_TAZ_NINJA;
				creditsInfo->tazState = CREDITS_TAZ_MESMERISED;
				break;
			}
			if(gameStatus.player[0].controller->triangleDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(taz, "wobble", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 1:
					PlayAnimationByName(taz, "slipslide", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 2:
					PlayAnimationByName(taz, "rant2", 1.0f, 0, 0, 0.1f, REACT);
					PlaySample("tazrant1.wav", 255, taz->actorInstance.position, 0);
					break;
				}
			}
		}
		break;
	case CREDITS_TAZ_BANDITO:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(!CheckIfPlayingAnimation(taz, "cowboyshootfinger"))
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
				creditsInfo->tazState = CREDITS_TAZ_SPINUP;
			}
		}
		if(!CheckIfPlayingAnimation(taz, NULL))
		{
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);

		}
		if(!CheckIfPlayingAnimation(taz, "cowboyshootfinger"))
		{
			if(gameStatus.player[0].controller->squareDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "cowboyshootfinger", 1.0f, 0, 0, 0.1f, REACT);
				// CMD: sample played in events
			}
			if(gameStatus.player[0].controller->circleChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "walk2start", 1.0f,0,0,0.1f,MOVE);
				PlayAnimationByName(taz, "walk2", 1.0f,1,1,0.1f,MOVE);
				creditsInfo->tazPrevState = CREDITS_TAZ_BANDITO;
				creditsInfo->tazState = CREDITS_TAZ_MESMERISED;
				break;
			}
			if(gameStatus.player[0].controller->triangleDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(taz, "wobble", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 1:
					PlayAnimationByName(taz, "slipslide", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 2:
					PlayAnimationByName(taz, "rant2", 1.0f, 0, 0, 0.1f, REACT);
					PlaySample("tazrant1.wav", 255, taz->actorInstance.position, 0);
					break;
				}
			}
		}
		break;
	case CREDITS_TAZ_WERETAZ:
		if((creditsInfo->tazClock -= fTime) < 0.0f)
		{
			if(!CheckIfPlayingAnimation(taz, "werewolfbiteclaw"))
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "spinup", 1.0f, 0, 0, 0.1f, REACT);
				creditsInfo->tazState = CREDITS_TAZ_SPINUP;
			}
		}
		if(!CheckIfPlayingAnimation(taz, NULL))
		{
			animPtr = ChooseIdleAnimationForCredits(taz);
			PlayAnimationBySegmentPointer(taz, animPtr, 1.0f, 0, 0, 0.1f, IDLE);

		}
		if(!CheckIfPlayingAnimation(taz, "werewolfbiteclaw"))
		{
			if(gameStatus.player[0].controller->squareDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "werewolfbiteclaw", 1.0f, 0, 0, 0.1f, REACT);
				PlaySample("swipe3.wav", 255, taz->actorInstance.position, 0);
			}
			if(gameStatus.player[0].controller->circleChannel->value)
			{
				FlushAnimationQueue(taz);
				PlayAnimationByName(taz, "walk2start", 1.0f,0,0,0.1f,MOVE);
				PlayAnimationByName(taz, "walk2", 1.0f,1,1,0.1f,MOVE);
				creditsInfo->tazPrevState = CREDITS_TAZ_WERETAZ;
				creditsInfo->tazState = CREDITS_TAZ_MESMERISED;
				break;
			}
			if(gameStatus.player[0].controller->triangleDebounceChannel->value)
			{
				FlushAnimationQueue(taz);
				switch(bmRand()%3)
				{
				case 0:
					PlayAnimationByName(taz, "wobble", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 1:
					PlayAnimationByName(taz, "slipslide", 1.0f, 0, 0, 0.1f, REACT);
					break;
				case 2:
					PlayAnimationByName(taz, "rant2", 1.0f, 0, 0, 0.1f, REACT);
					PlaySample("tazrant1.wav", 255, taz->actorInstance.position, 0);
					break;
				}
			}
		}
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateCartoonLightDirection
   Purpose : updates the direction of the cartoon light, dependant on user input
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCartoonLightDirection(PADCONTROLSTATUS *controller)
{
	TBMatrix	result, rotTheta, rotPhi;
	TBVector	position;

	// CMD: update angles according to input from right stick
	if((controller->mag2 > 0.2f)||(controller->r3DebounceChannel->value))
	{
		// CMD: update angles
		if((creditsInfo->theta += RAD(180.0f)*fTime*controller->x2) > RAD(360.0f)) creditsInfo->theta -= RAD(360.0f);
		if(creditsInfo->theta < 0.0f) creditsInfo->theta += RAD(360.0f);
		if((creditsInfo->phi += RAD(180.0f)*fTime*controller->y2) > RAD(360.0f)) creditsInfo->phi -= RAD(360.0f);
		if(creditsInfo->phi < 0.0f) creditsInfo->phi += RAD(360.0f);

		if(controller->r3DebounceChannel->value)
		{
			creditsInfo->theta = creditsInfo->initTheta;
			creditsInfo->phi = creditsInfo->initPhi;
		}

		bmVectorSet(position, 0.0f, 0.0f, -1.0f, 1.0f);
		bmMatYRotation(rotTheta, creditsInfo->theta);
		bmMatYRotation(rotPhi, creditsInfo->phi);
		bmMatMultiply(result, rotPhi, rotTheta);
		bmMatMultiplyVector(result, position);
		baSetDirectionalLight(&cartoonLight, position);
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawEndGameCredits
   Purpose : Does any drawing required
   Parameters : 
   Returns : 
   Info : 
*/

void DrawEndGameCredits(void)
{
	// IH: GameCube doesn't like drawing flat stuff much
#if BPLATFORM==GAMECUBE
	bdPushRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
#endif

	DrawSplashScreen(&creditsInfo->splash);

	// IH: GameCube doesn't like drawing flat stuff much
#if BPLATFORM==GAMECUBE
	bdPopRenderState(BDRENDERSTATE_ZWRITE);
#endif

	gameStatus.player[0].actorInstance->currentAlpha = 256;
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);

	DrawActorInstances(&characterInstances);
	DrawAttachedObjects(gameStatus.player[0].actorInstance);
}

/* --------------------------------------------------------------------------------
   Function : DrawEndGameCredits
   Purpose : Does any drawing required
   Parameters : 
   Returns : 
   Info : 
*/

void DrawEndGameCredits2(void)
{
	DrawActorInstances2(&characterInstances);
	DrawAttachedObjects2(gameStatus.player[0].actorInstance);

	baRenderExtraSystem(fireworksExtraSystem, (void*)EXTRA_ALPHA);
	baRenderExtraSystem(smokeSystem, (void*)EXTRA_ALPHA);

	DrawSpinEffectForActor(gameStatus.player[0].actorInstance);

	BookList_draw();
}