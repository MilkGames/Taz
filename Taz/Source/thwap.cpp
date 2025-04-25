// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : thwap.cpp
//   Purpose : handles thwap procedures
//			   Don't know what a thwap is?
//			   Ask yer project manager.
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"

#include "camera.h"
#include "status.h"
#include "control.h"
#include "playerstats.h"
#include "main.h"
#include "textfx.h"
#include "collectibles.h"
#include "pageitemfx.h"
#include "icon.h"
#include "thwap.h"
#include "timer.h"
#include "util.h"
#include "VideoFX.h"
#include "prompt.h"

BOOK	*thwapBook = 0, *thwapSubBook;

ICON	*posterIcon, *posterIcon2;
TEXTBOX	*legendBox;
TEXTBOX	*cashBox;

PIE_ZOOM	*zoomPoster, *zoomCash;

float thwapTime;

int	targetAmount;

float moneyInc;

float sloSpeed;

THWAPSTAGES thwapStage;

void initThwap()
{
	PAGE	*page;

	thwapBook = new BOOK();

	thwapBook->setEffectChooser(&WantedPIEChooser);

	page = thwapBook->addPage("page");

	page->insertFreeItem(posterIcon = new ICON("tazbiglogo.bmp"),RECTANGLE(0.0f, 0.0f, 0.15f,0.15f));

	// PP: NOTE: PAGEITEMs no longer delete the effects that you set with setEffect (for a good reason),
	// PP: so the easiest thing to do is probably just to pass it the address of a static effect.
	static PIE_ZOOM		zoomPoster;

	posterIcon->setEffect(&zoomPoster, PIEMODE_ENTER);	

	zoomPoster.setSpeed(1.1f);
	zoomPoster.setAccel(1.0f);
	zoomPoster.setOrigin(0.0f);
	zoomPoster.setTarget(1.0f);

	posterIcon->tilt = false;
	posterIcon->setScale(4.5f);

	thwapSubBook = new BOOK();
	
	thwapSubBook->setBookRect(RECTANGLE(-0.5f, 0.5f, -0.48f,-0.33f)); 

	page = thwapSubBook->addPage("legend page");
	
	page->insertItem(legendBox = new TEXTBOX());
	legendBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	legendBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	legendBox->setWrap(true);
	legendBox->setScale(1.0f);

	page = thwapSubBook->addPage("cash page");

	page->insertItem(cashBox = new TEXTBOX());
	
	// PP: NOTE: PAGEITEMs no longer delete the effects that you set with setEffect (for a good reason),
	// PP: so the easiest thing to do is probably just to pass it the address of a static effect.
	static PIE_ZOOM		zoomCash;

	cashBox->setEffect(&zoomCash, PIEMODE_ENTER);

	zoomCash.setSpeed(-0.8f);
	zoomCash.setAccel(-0.8f);
	zoomCash.setOrigin(1.5f);
	zoomCash.setTarget(1.0f);

	cashBox->setXAlign(PIFLAG_XALIGN_CENTRE);
	cashBox->setYAlign(PIFLAG_YALIGN_BOTTOM);
	cashBox->setScale(2.75f);

	thwapStage = Off;
}

void startThwap(EStringIndex string, int amount)
{
// PP: REMOUT: OOD	zoomCash->reset(cashBox, PIEMODE_ENTER);
// PP: REMOUT: OOD 	zoomPoster->reset(posterIcon, PIEMODE_ENTER);

	if (thwapStage != Off)
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty = targetAmount;

	char	cashString[16], tempString[16];

	int32	levelBounty = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty;

	if (amount < 0)
	{
		if (levelBounty == 0)
			return;

		if ((levelBounty + amount) < 0)
			amount = -levelBounty;
	}

	gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].levelBounty += amount;
	targetAmount = gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty + amount;

	if (amount < 0)
	{
		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].timesCaptured += amount;

		moneyInc = (float) (amount / 60.0f);
	}
	else
		moneyInc = (float) (amount / 120.0f);

	//SetGameState(&gameStatus, GAMESTATE_CUSTOMPAUSE, GAMESUBSTATE_NORMAL, (int32)DoThwap);

	legendBox->setText(string);

	utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty, cashString);
	
	cashBox->sprintf8("$%s", cashString);

	sloSpeed = 0.25f;

	if (amount < 0)
	{
		thwapSubBook->gotoPage("cash page");

		thwapSubBook->open();

		thwapTime = gameStatus.gameTime + 0.7f;

		thwapStage = CashZoom;
	}
	else
	{		
		thwapSubBook->gotoPage("legend page");

		if (string == STR_FILLER)
		{
			legendBox->sprintf8("%d / %d", gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].level[map.sceneNumber].postersCollected + 1, map.mapStats.numPosters);

			legendBox->setScale(2.75f);

			thwapStage = Slomo;
			
			thwapTime = gameStatus.gameTime + 0.5f;
		}
		else
		{
			legendBox->setScale(1.0f);

			thwapBook->open();
			
			thwapSubBook->open();

			thwapStage = PosterZoom;

			thwapTime = gameStatus.gameTime + 0.5f;
		}
	}
	// CMD: flush the rumble queue
	// CMD: any rumbles that are playing when we go into slo-mo might cause the pad to rumble for
	// CMD: a very looonnnng time
	FlushRumbleQueue(&controller1);
	FlushRumbleQueue(&controller2);
}

THWAPSTAGES getThwapStage()
{
	return thwapStage;
}

void DoThwap()
{
	updateThwap();
	drawThwap();

	UpdateCamera(&camera[0]);
}

void updateThwap()
{
	char	string[16], tempString[16];

	if (!thwapBook) return;

	if (thwapStage == Off) return;

	/*if (controller1.crossDebounceChannel->value)	// JW: OK, thwaps boring. lemme skip it.
	{
		biForceDebounce(controller1.crossDebounceChannel);

		gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty = targetAmount;

		// CMD: if we are catering for morons, cut delay on message
		CutMorronicMessageDelay(1.0f);

		thwapBook->close();
		thwapSubBook->close();

		thwapTime = 0.0f;

		thwapStage = Off;

		sloSpeed = 1.0f;

		TimeFX_setGlobalRate(1.0f);
		VFX_motionBlur(0.0f);
	}*/

// PP: REMOUT: Now Handled by the Book List				thwapBook->update();

	switch (thwapStage)
	{
	case Slomo:		if ((thwapTime < gameStatus.gameTime) || OKBUTTON)
					{	
						FORCE_OKBUTTON_DEBOUNCE;

						thwapBook->open();
						
						thwapSubBook->open();

						thwapStage = PosterZoom;

						thwapTime = gameStatus.gameTime + 0.3f;
					}
					else
					{
						sloSpeed -= 0.005f;

						if (sloSpeed < 0.1f)
							sloSpeed = 0.1f;

						//TimeFX_setGlobalRate(sloSpeed);
					}
					break;

	case PosterZoom:if ((thwapTime < gameStatus.gameTime) || OKBUTTON)
					{	
						FORCE_OKBUTTON_DEBOUNCE;

						thwapSubBook->nextPage();
	
						thwapTime = gameStatus.gameTime + 0.15f;

						thwapStage = CashZoom;

						if (moneyInc > 0)
						{
							utilFormatPrettyNumbers(targetAmount - gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty, tempString);

							sprintf(string, "$%s", tempString);

							CreateScreenScoreMember(gameStatus.player1->actorInstance.position, string, SCREENSCORE_REWARD);
						}
					}
					else
					{
						sloSpeed -= 0.005f;

						if (sloSpeed < 0.1f)
							sloSpeed = 0.1f;

						//TimeFX_setGlobalRate(sloSpeed);
					}

					break;

	case CashZoom:	if ((thwapTime < gameStatus.gameTime) || OKBUTTON)
					{	
						FORCE_OKBUTTON_DEBOUNCE;

						if (moneyInc < 0)
							thwapTime = gameStatus.gameTime + 3.0f;
						else
							thwapTime = gameStatus.gameTime + 0.3f;

						thwapStage = Steady;
					}

					break;

	case Steady:	if ((thwapTime < gameStatus.gameTime) || OKBUTTON)
					{	
						FORCE_OKBUTTON_DEBOUNCE;

						gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty = targetAmount;

						thwapBook->close();
						thwapSubBook->close();
						// CMD: if we are catering for morons, cut delay on message
						CutMorronicMessageDelay(0.5f);

						thwapStage = Out;
	
						thwapTime = gameStatus.gameTime + 0.075f;
					}
					else if ((moneyInc > 0.0f && gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty < targetAmount) ||
							 (moneyInc < 0.0f && gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty > targetAmount))

					{
						gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty += (int) (moneyInc *= 1.05f);
					}
					else if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty != targetAmount)
					{
						gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty = targetAmount;

						sloSpeed = 0.1f;
					}

					if (gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty < 0)
						gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty = 0;

					utilFormatPrettyNumbers(gameStatus.globalSettings.saveInfo.saveSlot[gameStatus.globalSettings.currentSaveSlot].bounty, string);
	
					cashBox->sprintf8("$%s", string);

					break;

	case Out:		if ((thwapTime < gameStatus.gameTime) || OKBUTTON)
					{	
						FORCE_OKBUTTON_DEBOUNCE;

						thwapTime = 0.0f;

						thwapStage = Off;

						sloSpeed = 1.0f;

						TimeFX_setGlobalRate(sloSpeed);

						VFX_motionBlur(0.0f);
					}
					else
					{
						sloSpeed += 0.02f;

						if (sloSpeed > 1.0f)
							sloSpeed = 1.0f;

		//				TimeFX_setGlobalRate(sloSpeed);
					}

					break;
	}

	if (moneyInc > 0)
	{
		if (thwapStage != Off)
		{
			VFX_motionBlur((1.0f - sloSpeed) / 3.0f);

			TimeFX_setGlobalRate(sloSpeed);
		}
	}
}

void drawThwap()
{
/* PP: REMOUT: Now Handled by the Book List				if (!thwapBook)	return;

	bdSetIdentityObjectMatrix();
	
	SAVESTATES;

	SETPROJECTION(2D);
	SETZTEST(ALWAYS);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);

	thwapBook->draw();

	SETPROJECTION(3D);
	RESTORESTATES;*/
}

void killThwap()
{
	// PP: REMOUT: BookList_closeAndFree should do the trick		delete(thwapBook);

	// PP: start the book closing and have it freed once it finishes closing
	
	if (thwapBook)
	{
		SAFE_DELETE(thwapBook);// PP: NOTE: Must be delete rather than free, otherwise destructor gets skipped

		thwapBook = 0;
	}

	if (thwapSubBook)
	{
		SAFE_DELETE(thwapSubBook);// PP: NOTE: Must be delete rather than free, otherwise destructor gets skipped

		thwapSubBook = 0;
	}
}