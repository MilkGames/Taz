// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Blitz Games Ltd.
//
//      File : timeattack.cpp
//   Purpose : multiplayer race stuff
// Component : Taz
//    Author : Chris W
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"
#include "main.h"

#include "actors.h"
#include "status.h"
#include "events.h"
#include "util.h"
#include "textfx.h"
#include "scene.h"

#include "multiplayer.h"

#define TB_TIMER (0)

static struct
{
	float timeLimit;
	float startTime;
}levelData;

int timeattackHandler(int reason)
{
	char parmStringBuf[256];

	switch(reason)
	{
	case MPREASON_INITIALISE:
		{	// CPW: called once
			int i;

			// CPW: set up level specfic stuff, (timers etc).
			memset(&levelData, 0, sizeof(levelData));
			// CPW: levelData.gameType = GAMETYPE_TARGET;
			levelData.timeLimit = 30.0f;
			levelData.startTime = gameStatus.gameTime;

			// CPW: create book
			for(i=0; i<mpNumPlayers; i++)
			{
				BOOK *pStatsBook;
				ACTORINSTANCE *pActorInstance = mpPlayer[i];
				pActorInstance->actorStatus->multiplayer.numPickups = 0;
				pActorInstance->actorStatus->multiplayer.timeCredit = 0;

				// PP: Colour for the text in the stats book
				COLOUR		statsBookColour=COLOUR(128, 128, 128);

				pActorInstance->actorStatus->multiplayer.pStatsBook = pStatsBook = new BOOK;

				// PP: Set the size of the book
				pStatsBook->setRect(RECTANGLE(-300, 300, -220, 220));

				
				// PP: 'Timer' textbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

				// PP: Create and add the textbox
				PAGE *pPage = pStatsBook->addPage(pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER] = new TEXTBOX());
				TEXTBOX *pTextBox = pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER];

				// PP: Set the font size
				pTextBox->setScale(1.0f);

				// PP: Set the font colour and specify no vertical gradient
				pTextBox->setVGrad(false);
				pTextBox->setColour(statsBookColour);

				// PP: Align to top-centre
				pTextBox->setXAlign(PIFLAG_XALIGN_CENTRE);
				pTextBox->setYAlign(PIFLAG_YALIGN_TOP);

				pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER]->sprintf("xxxxxxxx");
				pStatsBook->open();
			}
		}
		break;

	case MPREASON_PREP:
		// CPW: called once
		break;

	case MPREASON_POLL:
		{
			// CPW: called once
			int i;

			for(i=0; i<mpNumPlayers; i++)
			{
				float actualTime = gameStatus.gameTime/* - levelData.startTime*/;
				ACTORINSTANCE *pActorInstance = mpPlayer[i];

				if(bmRand()%100 == 1)
				{
					pActorInstance->actorStatus->multiplayer.timeCredit+=2.0f;
				}

				actualTime-=pActorInstance->actorStatus->multiplayer.timeCredit;
				actualTime = levelData.timeLimit - actualTime;

				if(actualTime > 0.0f)
					pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER]->sprintf("%0.2f", actualTime);
				else
					pActorInstance->actorStatus->multiplayer.pTextBox[TB_TIMER]->sprintf("TIME UP!");

				pActorInstance->actorStatus->multiplayer.pStatsBook->update();
			}
		}

		break;

	case MPREASON_DRAW:
		// CPW: called (num players) times
		{
			ACTORINSTANCE *pActorInstance = mpDrawCam->ourActorInstanceLookAt;

			bdSetIdentityObjectMatrix();
			
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE2D);

			// PP: use a 2d world matrix on ps2
			#if BPLATFORM == PS2
// PP: REMOUT: I really don't like this render state					bdSetRenderState(BDRENDERSTATE_2DWORLDMATRIX,FALSE,FALSE);
			#endif

			bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);

			pActorInstance->actorStatus->multiplayer.pStatsBook->draw();
			pActorInstance->actorStatus->multiplayer.pStatsBook->drawDebug();
			
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		}
		break;

	case MPREASON_KILL:
		{	// CPW: called once
			int i;

			for(i=0; i<mpNumPlayers; i++)
			{
				ACTORINSTANCE *pActorInstance = mpPlayer[i];
				BookList_closeAndFree(pActorInstance->actorStatus->multiplayer.pStatsBook);
			}
		}

		break;
	}

	return 0;
}
