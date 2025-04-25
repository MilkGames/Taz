// JW: Just some text up here for the sake of it
#include "languagemain.h"
#include "languageanimation.h"
#include "memcard.h"
#include "font.h"
#include "util.h"
#include "demo.h"
#include "multiplayer.h"
#include "playerstats.h"


// PP: suffixes used on localised versions of filenames.
// PP: the ordering here matches the TAZLANG enum (languagemain.h).
// PP: yes I do know about Gizmo's language options, but that's not always the best approach (see button glyphs).
// PP: and yes, the plural of 'suffix' IS 'suffixes'.
const char* languageSuffixes[NUM_SUPPORTED_LANGUAGES]=
{
	"_uk",
	"_f",
	"_d",
	"_e",
	"_it"
};

#if ((BPLATFORM==XBOX)||(BPLATFORM==PC)||(BPLATFORM==GAMECUBE))
char skipLanguage = 1;
#else
char skipLanguage = 0;
#endif

// PP: when this is true, we'll skip the main game intro cutscene.
// PP: it'll be set true when we've been to the language select from the frontend
bool skipMainGameIntroCutscene=false;

ACTORINSTANCE	*paPodium;
ACTORINSTANCE	*paSylvester;

LANGUAGESELECTINFO	*LanguageSelectInfo;

BOOK				*chselangBook;
BOOK				*languageBook;
BOOK				*slctlangBook;

float				sylvesterTaunt;
int					sylvesterChannel;

SPLASHSCREEN		langSplash;

int InitialiseLanguageSelect(int startFlag, char skip)
{
	ACTORINSTANCE	*paCamera;
	
	TBPackageIndex	*pakIndex;
	
	TBActorNodeInstance	*posNode, *latNode;
	
	PAGE			*page;
	TEXTBOX			*text;
	
	//skipLanguage = skip;
	
	LanguageSelectInfo = new LANGUAGESELECTINFO;

	// PP: I moved this here to prevent a pile of bodges from toppling down
	LanguageSelectInfo->SelectMade	= 0;

	if (skipLanguage)
		return 0;
	
	TBVector latVec, posVec;
	
	InitialiseLanguageAnimationPaths();
	
	if (videoMode.flags & BDISPLAYFLAG_NTSC)
	{
		LoadActor("objects\\podiumusa.obe", map.mapName);
		paPodium = CreateActorInstance(&map.animatedInstances, "objects\\podiumusa.obe", "podium");
	}
	else
	{
		LoadActor("objects\\podiumeur.obe", map.mapName);
		paPodium = CreateActorInstance(&map.animatedInstances, "objects\\podiumeur.obe", "podium");
	}
	
	ResetAnimPathToStart(paPodium,"english");
	ResetAnimPathToStart(paPodium,"french");
	ResetAnimPathToStart(paPodium,"german");
	ResetAnimPathToStart(paPodium,"spanish");
	ResetAnimPathToStart(paPodium,"italian");
	
	LanguageSelectInfo->currentFlag = startFlag;
	LanguageSelectInfo->targetFlag	= startFlag;
	LanguageSelectInfo->nextFlag	= startFlag;
	
	AddActorToCollisionCache(&collisionCache, paPodium, CACHEFLAGS_DOUBLESIDED|CACHEFLAGS_LIVEUPDATE);
	
	LoadActor("objects\\languagecamera.obe", map.mapName);
	LoadActor("objects\\sylvester.obe", map.mapName);
	
	LoadWholePackage("tweety", 1);
	LoadActor("tweety.obe", "tweety");
	
	paCamera = CreateActorInstance(&map.levelInstances, "objects\\languagecamera.obe", "camera");
	
	paSylvester = CreateActorInstance(&map.animatedInstances, "objects\\sylvester.obe", "sylvester");
	CreateActorInstanceStatus(paSylvester);
	
	EnableActorCartoonRenderMode(&paSylvester->actorInstance);
	
	CreateActorInstanceShadow(paSylvester,METERTOUNIT(1.0f),METERTOUNIT(2.5f));
	
	ResetAnimPathToStart(paSylvester,"runleftupper");
	ResetAnimPathToStart(paSylvester,"runrightupper");
	ResetAnimPathToStart(paSylvester,"runleftlower");
	ResetAnimPathToStart(paSylvester,"runrightlower");
	ResetAnimPathToStart(paSylvester,"jumpup");
	ResetAnimPathToStart(paSylvester,"jumpdown");
	ResetAnimPathToStart(paSylvester,"idle1");
	ResetAnimPathToStart(paSylvester,"fallthruupper");
	ResetAnimPathToStart(paSylvester,"fallthrulower");
	ResetAnimPathToStart(paSylvester,"grabtweety(upper)1");
	ResetAnimPathToStart(paSylvester,"grabtweety(upper)2");
	ResetAnimPathToStart(paSylvester,"grabtweety(upper)3");
	ResetAnimPathToStart(paSylvester,"grabtweety(upper)4");
	ResetAnimPathToStart(paSylvester,"grabtweety(lower)1");
	ResetAnimPathToStart(paSylvester,"grabtweety(lower)2");
	ResetAnimPathToStart(paSylvester,"grabtweety(lower)3");
	ResetAnimPathToStart(paSylvester,"grabtweety(lower)4");
	
	latNode = baFindNode(paCamera->actorInstance.rootNodeInstance, "camera lookat");
	posNode = baFindNode(paCamera->actorInstance.rootNodeInstance, "camera pos");
	
	baGetNodesWorldPosition(&paCamera->actorInstance, latNode, latVec);
	baGetNodesWorldPosition(&paCamera->actorInstance, posNode, posVec);
	
	CreateTweety(vFlagPos[LanguageSelectInfo->targetFlag], bIdentityQuaternion);
	SetTweetyDest(vFlagPos[LanguageSelectInfo->targetFlag]);
	
	InitCamera(&camera[0], posVec, latVec, FALSE);
	
	bmVectorCopy(paSylvester->actorInstance.position, vFlagPos[LanguageSelectInfo->currentFlag]);
	
	if (startFlag < 3)
		PlayAnimationByName(paSylvester, "grabtweety(upper)1",  1.0f, 0, 0, 0.0f, NONE);
	else
		PlayAnimationByName(paSylvester, "grabtweety(lower)1",  1.0f, 0, 0, 0.0f, NONE);
	
	FreeActorInstance(paCamera);
	FreeActor("objects\\languagecamera.obe");
	
	chselangBook = new BOOK;
	languageBook = new BOOK;
	slctlangBook = new BOOK;

#define ADD_CHOOSELANG_PAGE(name, chseText)\
	page = chselangBook->addPage(#name); \
	page->insertItem(text = new TEXTBOX(chseText)); \
	text->setYAlign(PIFLAG_YALIGN_TOP);

#if (BPLATFORM == XBOX)
	#define ADD_SELECTLANG_PAGE(name, slctText, backText)\
		page = slctlangBook->addPage(#name); \
		page->insertItem(text = new TEXTBOX(slctText)); \
		text->setXAlign(PIFLAG_XALIGN_RIGHT); \
		text->setYAlign(PIFLAG_YALIGN_BOTTOM); \
		text->setStyle(TS_control); \
		text->setAlignScale(1.1f); \
		text->setYPadding(false); \
		text->setSelectable(false);\
		page->insertItem(text = new TEXTBOX(backText), false); \
		text->setXAlign(PIFLAG_XALIGN_LEFT); \
		text->setYAlign(PIFLAG_YALIGN_BOTTOM); \
		text->setStyle(TS_control); \
		text->setAlignScale(1.1f); \
		text->setYPadding(false); \
		text->setSelectable(false);
#else
	#define ADD_SELECTLANG_PAGE(name, slctText)\
		page = slctlangBook->addPage(#name); \
		page->insertItem(text = new TEXTBOX(slctText)); \
		text->setXAlign(PIFLAG_XALIGN_CENTRE); \
		text->setYAlign(PIFLAG_YALIGN_BOTTOM); \
		text->setStyle(TS_control); \
		text->setYPadding(false); \
		text->setSelectable(false);
#endif
	
	// JW: Add Languge Page
#define ADD_LANGUAGEBOOK_PAGE(name, infoText) \
	page=languageBook->addPage(#name); \
	page->insertFreeItem(text = new TEXTBOX(infoText), RECTANGLE(-0.5f, 0.5f, -0.44f, -0.34f)); \
	text->setStyle(TS_info); \
	text->setXAlign(PIFLAG_XALIGN_CENTRE); \
	text->setYAlign(PIFLAG_YALIGN_BOTTOM); \
	text->setAlignScale(1.2f); \
	text->setSelectable(false);	

	ADD_CHOOSELANG_PAGE(ENGLISH,STR_LS_CHOOSE_LANGUAGE_ENGLISH);
	ADD_CHOOSELANG_PAGE(FRENCH,STR_LS_CHOOSE_LANGUAGE_FRENCH);
	ADD_CHOOSELANG_PAGE(SPANISH,STR_LS_CHOOSE_LANGUAGE_SPANISH);
	ADD_CHOOSELANG_PAGE(GERMAN,STR_LS_CHOOSE_LANGUAGE_GERMAN);
	ADD_CHOOSELANG_PAGE(ITALIAN,STR_LS_CHOOSE_LANGUAGE_ITALIAN);

#if (BPLATFORM == XBOX)
	ADD_SELECTLANG_PAGE(ENGLISH,STR_LS_SELECT_ENGLISH, STR_LS_BACK_ENGLISH);
	ADD_SELECTLANG_PAGE(FRENCH,STR_LS_SELECT_FRENCH, STR_LS_BACK_FRENCH);
	ADD_SELECTLANG_PAGE(SPANISH,STR_LS_SELECT_SPANISH, STR_LS_BACK_SPANISH);
	ADD_SELECTLANG_PAGE(GERMAN,STR_LS_SELECT_GERMAN, STR_LS_BACK_GERMAN);
	ADD_SELECTLANG_PAGE(ITALIAN,STR_LS_SELECT_ITALIAN, STR_LS_BACK_ITALIAN);
#else
	ADD_SELECTLANG_PAGE(ENGLISH,STR_LS_SELECT_ENGLISH);
	ADD_SELECTLANG_PAGE(FRENCH,STR_LS_SELECT_FRENCH);
	ADD_SELECTLANG_PAGE(SPANISH,STR_LS_SELECT_SPANISH);
	ADD_SELECTLANG_PAGE(GERMAN,STR_LS_SELECT_GERMAN);
	ADD_SELECTLANG_PAGE(ITALIAN,STR_LS_SELECT_ITALIAN);
#endif
	
	ADD_LANGUAGEBOOK_PAGE(ENGLISH,STR_LANGUAGE_ENGLISH);
	ADD_LANGUAGEBOOK_PAGE(FRENCH,STR_LANGUAGE_FRENCH);
	ADD_LANGUAGEBOOK_PAGE(SPANISH,STR_LANGUAGE_SPANISH);
	ADD_LANGUAGEBOOK_PAGE(GERMAN,STR_LANGUAGE_GERMAN);
	ADD_LANGUAGEBOOK_PAGE(ITALIAN,STR_LANGUAGE_ITALIAN);
	
	switch (LanguageSelectInfo->currentFlag)
	{
	case FL_USA_ENGLISH:	chselangBook->gotoPage("ENGLISH");	
							languageBook->gotoPage("ENGLISH");
							slctlangBook->gotoPage("ENGLISH");
							break;
	
	case FL_USA_FRENCH:		chselangBook->gotoPage("FRENCH");	
							languageBook->gotoPage("FRENCH");
							slctlangBook->gotoPage("FRENCH");
							break;

	case FL_USA_GERMAN:		chselangBook->gotoPage("GERMAN");	
							languageBook->gotoPage("GERMAN");
							slctlangBook->gotoPage("GERMAN");
							break;

	case FL_USA_SPANISH:	chselangBook->gotoPage("SPANISH");	
							languageBook->gotoPage("SPANISH");
							slctlangBook->gotoPage("SPANISH");
							break;

	case FL_USA_ITALIAN:	chselangBook->gotoPage("ITALIAN");	
							languageBook->gotoPage("ITALIAN");
							slctlangBook->gotoPage("ITALIAN");
							break;
	}
	
	chselangBook->open();
	languageBook->open();
	slctlangBook->open();
	
	LoadSplashScreen(&langSplash, "languageselect.bmp", "splshscr");
	
	sylvesterTaunt = gameStatus.gameTime - 1;
	
	// PP: override widescreen (the scene has to fit exactly onto a splash screen
	overrideWidescreen();
	
	return 0;
}

int PrepLanguageSelect()
{
	PrepActorInstances(&map.levelInstances);
	PrepActorInstances(&map.animatedInstances);
	
	CreateActorShadows(&map.animatedInstances, &collisionCache);
	
	return 0;
}

int DrawLanguageSelect()
{
	if (!LanguageSelectInfo)	return 0;

	// IH: Had a little problem with sylvester and tweety being hidden behind the splash
#if BPLATFORM==GAMECUBE
	bdPushRenderState(BDRENDERSTATE_ZWRITE, FALSE, 0);
	DrawSplashScreen(&langSplash);
	bdPopRenderState(BDRENDERSTATE_ZWRITE);
#else
	DrawSplashScreen(&langSplash);
#endif

	DrawTweety();

	DrawActorInstances(&map.animatedInstances);
	DrawActorInstances(&map.levelInstances);

	DrawActorShadows(&map.animatedInstances);

	/* PP: REMOUT: Now handled by the Book List		
	bdSetIdentityObjectMatrix();
	
	  bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);
	  
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		
		  chselangBook->draw();
		  languageBook->draw();
		  
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	*/
	return 0;
}

int FreeLanguageSelect()
{
	if (!LanguageSelectInfo) return 0;
	
	if (skipLanguage)
	{
		SAFE_DELETE(LanguageSelectInfo);
		return 0;
	} 
	
	FreeSplashScreen(&langSplash);
	
	DestroyTweety();
	
	if (paSylvester)
	{
		FreeActorInstance(paSylvester);
		paSylvester = 0;
	}
	
	if (paPodium)
	{
		FreeActorInstance(paPodium);
		paPodium = 0;
	}
	
	FreeActorInstance(LanguageSelectInfo->Path);
	
	FreeActor("objects\\podiumeur.obe");
	FreeActor("objects\\sylvester.obe");
	
	SAFE_DELETE(chselangBook);
	SAFE_DELETE(languageBook);
	SAFE_DELETE(slctlangBook);
	
	SAFE_DELETE_ARRAY(vFlagPos);				// PP: maybe best to use this array version of SAFE_DELETE
	
	FreeActor("objects\\path_usa_all.obe");
	
	SAFE_DELETE(LanguageSelectInfo);

	skipLanguage = true;

	// PP: Now's a perfect time to cancel widescreen override (teehee - no-one will even know!)
	overrideWidescreen(false);

	// PP: finish overriding stereoscopy here too
	overrideStereoscopy(false);
	
	return 0;
}

static void LoadGameFadeCallback(FADE *fade)
{
	if (!skipLanguage)
	{
		chselangBook->close();
		languageBook->close();
		slctlangBook->close();

		SwitchLanguage();

		FreeSplashScreen(&langSplash);
		
		DestroyTweety();
		
		FreeActorInstance(paSylvester);
		FreeActorInstance(paPodium);
		
		paSylvester = 0;
		paPodium = 0;
	}
	
	if (map.lastScene != SCENE_FRONTEND)
	{
		CheckMemCardOnBoot();
	}
	
	fade->SetFadeDownFinishedCallback(NULL);
	
	// PP: Now's a perfect time to cancel widescreen override (teehee - no-one will even know!)
	// PP: REMOUT: moving this to FreeLanguageSelect() overrideWidescreen(false);
}

void SwitchLanguage()
{
	EBLanguageID	language;
			
	switch(LanguageSelectInfo->currentFlag)
	{
		case FL_USA_ENGLISH:	language = BLANGUAGEID_UK;	break;
		case FL_USA_FRENCH:		language = BLANGUAGEID_F;	break;
		case FL_USA_GERMAN:		language = BLANGUAGEID_D;	break;
		case FL_USA_SPANISH:	language = BLANGUAGEID_E;	break;
		case FL_USA_ITALIAN:	language = BLANGUAGEID_IT;	break;
	}
			
	if (gameStatus.currentLanguage!=language)
	{
		gameStatus.currentLanguage = language;
		FreeStringTable(mainStringTable);
		bkSetLanguage(language);
		mainStringTable = LoadStringTable("main.txt","text");
	}
}

int UpdateLanguageSelect()
{
	if (!LanguageSelectInfo)	return 0;

	if (skipLanguage)
	{
		if (LanguageSelectInfo->SelectMade == 2)
		{
			//SetFadeUp(&camera[0].fade);
			//StartFadeDown(&camera[0].fade, FADETYPE_NORMAL, 1.0f);
			
			// TP: made changes here to allow the main game intro to be tagged onto the end of this scene, maybe even background load it while the player is selecting language
			camera[0].fade.SetSceneToLoad(FADESCENE_NONE);
			camera[0].fade.SetGameStatus(&gameStatus);
			//camera[0].fade.fadeUpOnCompletion = TRUE;
			
#ifdef CONSUMERDEMO
			//SetFadeDownFinishedCallback(&camera[0].fade, StartDemoMenuFadeDownCallback);

			StartDemoMenuFadeDownCallback(&camera[0].fade);
#else
			//SetFadeDownFinishedCallback(&camera[0].fade, LoadGameFadeCallback);
			//camera[0].fade.levelCompleteOnFadeDown = true;

			LevelCompleted(gameStatus.player1);
#endif
			
			LanguageSelectInfo->SelectMade++;
		}
		else
		{
			if (LanguageSelectInfo->SelectMade<=2)	LanguageSelectInfo->SelectMade++;
		}
		AnimateActorInstances(&map.animatedInstances);
		AnimateActorInstances(&map.levelInstances);
		
		return 0;
		
	}

	// PP: override stereoscopy during language select, because everything in the scene is farther away than the splash screen so it'd look cack
	// PP: The end game credits are a bit different because Taz is nice & close, closer than the splash
	overrideStereoscopy(true);

#if (BPLATFORM == PS2)
	if (controller1.idleTime > 30.0f && !gameStatus.demoMode && LanguageSelectInfo->Tweety)
	{
		BookList_closeAll();

#ifdef CONSUMERDEMO
		bkPrintf("StartDemoAttract called from Language Select timeout");

		StartDemoAttract();
#else
		gameStatus.demoMode = 1;

		bkPrintf("demoMode set to Disk Play in Language Select timeout");

		if (map.lastScene != SCENE_FRONTEND)
		{
			CheckMemCardOnBoot();
		}

		// PP: override stereoscopy - otherwise the demo'd slow down
		overrideStereoscopy(true);

		bkGenerateEvent("scenechange","_safari");
#endif
	}
#endif

#if (BPLATFORM == XBOX)
	if (CANCELBUTTON)
	{
		bkGenerateEvent("scenechange","_frontend");
	}
#endif
	
	if (!LanguageSelectInfo->SelectMade)
	{
		if (sylvesterTaunt < gameStatus.gameTime)
		{
			switch (bmRand()%3)
			{
			case 0:	sylvesterChannel = PlaySample("sylvestergen_1.wav", 255);	break;
			case 1: sylvesterChannel = PlaySample("sylvestergen_3.wav", 255);	break;
			case 2:	sylvesterChannel = PlaySample("sylvestergen_5.wav", 255);	break;
			}
			
			sylvesterTaunt = gameStatus.gameTime + UtilGenerateRandomNumber(4, 6);
		}
		
		if (controller1.dPadXDebounceChannel->value < 0 || controller1.x1DebounceChannel->value < 0.0f)
		{
			LanguageSelectInfo->targetFlag  = GetNextFlag(LanguageSelectInfo->targetFlag, FL_GO_LEFT);
			
			SetTweetyDest(vFlagPos[LanguageSelectInfo->targetFlag]);
			ChooseLanguagePage();
		}
		
		if (controller1.dPadXDebounceChannel->value > 0 || controller1.x1DebounceChannel->value > 0.0f)
		{
			LanguageSelectInfo->targetFlag  = GetNextFlag(LanguageSelectInfo->targetFlag, FL_GO_RIGHT);
			
			SetTweetyDest(vFlagPos[LanguageSelectInfo->targetFlag]);
			ChooseLanguagePage();
		}
		
		if (controller1.dPadYDebounceChannel->value < 0 || controller1.y1DebounceChannel->value < 0.0f)
		{
			LanguageSelectInfo->targetFlag  = GetNextFlag(LanguageSelectInfo->targetFlag, FL_GO_DOWN);
			
			SetTweetyDest(vFlagPos[LanguageSelectInfo->targetFlag]);
			ChooseLanguagePage();
		}
		
		if (controller1.dPadYDebounceChannel->value > 0 || controller1.y1DebounceChannel->value > 0.0f)
		{
			LanguageSelectInfo->targetFlag  = GetNextFlag(LanguageSelectInfo->targetFlag, FL_GO_UP);
			
			SetTweetyDest(vFlagPos[LanguageSelectInfo->targetFlag]);
			ChooseLanguagePage();
		}
		
		if (OKBUTTON)
		{
			if ((LanguageSelectInfo->currentFlag == LanguageSelectInfo->targetFlag) &&
				(LanguageSelectInfo->currentFlag == LanguageSelectInfo->targetFlag))
			{
				StopChannel(&sylvesterChannel);
				
				PlaySample("sylvester fall.wav", 255);
				
				switch ((int) UtilGenerateRandomNumber(0, 3))
				{
				case 0:	characterSounds.tweety = PlaySample("tweetygen_9.wav", 255);			break;
				case 1: characterSounds.tweety = PlaySample("tweetygen_9 alt 1.wav", 255);	break;
				case 2:	characterSounds.tweety = PlaySample("tweetygen_9 alt 2.wav", 255);	break;
				}
				
				//Drop Sylvester
				LanguageSelectInfo->SelectMade = 1;
				
				LanguageSelectInfo->Tweety->state = TWEETYSTATE_LAUGH;
				
				bmQuatCopy(paSylvester->actorInstance.orientation, bIdentityQuaternion);
				
				ChooseAnimationForPodium(paPodium);
				
				if (LanguageSelectInfo->currentFlag == FL_USA_ENGLISH ||
					LanguageSelectInfo->currentFlag == FL_USA_FRENCH ||
					LanguageSelectInfo->currentFlag == FL_USA_SPANISH)
				{
					if (!CheckIfPlayingAnimation(paSylvester, "fallthruupper"))
					{
						FlushAnimationQueue(&paSylvester->actorInstance);
						PlayAnimationByName(paSylvester, "fallthruupper", 0.7f, 0, 0, 0.2f, NONE);
					}
				}
				else
				{
					if (!CheckIfPlayingAnimation(paSylvester, "fallthrulower"))
					{
						FlushAnimationQueue(&paSylvester->actorInstance);
						PlayAnimationByName(paSylvester, "fallthrulower", 0.7f, 0, 0, 0.2f, NONE);
					}
				}
			}
		}
	}
	else if (LanguageSelectInfo->SelectMade == 1)
	{
		if (!CheckIfPlayingAnimation(paSylvester, NULL))
		{	//Sylvester has finished falling			
			TBActorNodeInstance	*node;
			
			TBVector			worldSpace, screenSpace;
			
			node = baFindNode(LanguageSelectInfo->Tweety->actorInstance->actorInstance.rootNodeInstance, "root");
			
			if(node)
				baGetNodesWorldPosition(&LanguageSelectInfo->Tweety->actorInstance->actorInstance, node, worldSpace);
			else
				bmVectorCopy(worldSpace, LanguageSelectInfo->Tweety->actorInstance->actorInstance.position);
			
			camera[0].fade.SetPlayer(LanguageSelectInfo->Tweety->actorInstance);
			worldSpace[3] = 1.0f;
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
			bdSetIdentityObjectMatrix();
			bdProjectVertices(screenSpace, worldSpace, 1);
			
			camera[0].fade.SetCircleFadeTo( screenSpace[0], screenSpace[1] + 5);
			camera[0].fade.StartFadeDown(FADETYPE_CIRCLE, 1.0f);
			
			// TP: made changes here to allow the main game intro to be tagged onto the end of this scene, maybe even background load it while the player is selecting language
			camera[0].fade.SetSceneToLoad(FADESCENE_NONE);
			camera[0].fade.SetGameStatus(&gameStatus);
			//camera[0].fade.fadeUpOnCompletion = TRUE;
			
#ifdef CONSUMERDEMO
			camera[0].fade.SetFadeDownFinishedCallback(StartDemoMenuFadeDownCallback);
#else
			camera[0].fade.SetFadeDownFinishedCallback(LoadGameFadeCallback);
			camera[0].fade.LevelCompleteOnFadeDown(true);
#endif
			
			LanguageSelectInfo->SelectMade = 2;
			
			chselangBook->close();
			languageBook->close();
			slctlangBook->close();
		}
	}
	
	LanguageSelectInfo->Tweety = UpdateTweetyLanguage(0.0f);
	
	ChooseSylvesterAnimationToPlay(paSylvester);
	
	UpdatePathAnimation(paSylvester);
	UpdateSylvesterPosition(paSylvester);
	
	AnimateActorInstances(&map.animatedInstances);
	AnimateActorInstances(&map.levelInstances);
	
	return 0;
}

void ChooseLanguagePage()
{
	switch (LanguageSelectInfo->targetFlag)
	{
	case FL_USA_ENGLISH:	chselangBook->gotoPage("ENGLISH");	
							languageBook->gotoPage("ENGLISH");
							slctlangBook->gotoPage("ENGLISH");
							break;
	
	case FL_USA_FRENCH:		chselangBook->gotoPage("FRENCH");	
							languageBook->gotoPage("FRENCH");
							slctlangBook->gotoPage("FRENCH");
							break;

	case FL_USA_GERMAN:		chselangBook->gotoPage("GERMAN");	
							languageBook->gotoPage("GERMAN");
							slctlangBook->gotoPage("GERMAN");
							break;

	case FL_USA_SPANISH:	chselangBook->gotoPage("SPANISH");	
							languageBook->gotoPage("SPANISH");
							slctlangBook->gotoPage("SPANISH");
							break;

	case FL_USA_ITALIAN:	chselangBook->gotoPage("ITALIAN");	
							languageBook->gotoPage("ITALIAN");
							slctlangBook->gotoPage("ITALIAN");
							break;
	}
}