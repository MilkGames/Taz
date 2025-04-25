// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : floorpolisherwantedposter.cpp
//   Purpose : functions to calculate the tilt of objects when collided with
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "maths.h"
#include "status.h"
#include "vehicles.h"
#include "playerstats.h"
#include "floorpolisherwantedposter.h"
#include "timer.h"
#include "TextFX.h"
#include "sfx.h"
#include "Cheats.h"								// PP: Cheats
#include "PageItemFX.h"							// PP: Special effects for page items

WANTEDPOSTERDECALINFO		*floorPolisherWantedPosterInfo;

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO **decalInfo)
{
	*decalInfo = NULL;
}

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

WANTEDPOSTERDECALINFO *CreateFloorPolisherWantedPoster(TBVector position, float xDim, float yDim, float rotation)
{
	TBMatrix	scaleMatrix,rotMatrix, transMatrix, concMatrix, xRot;
	WANTEDPOSTERDECALINFO	*decalInfo;
	float					height;
	BOOK					*bookPtr;
	PAGE					*pagePtr;

	decalInfo = (WANTEDPOSTERDECALINFO*)ZALLOC(sizeof(WANTEDPOSTERDECALINFO));
	if(decalInfo == NULL) return NULL;

	bmVectorCopy(decalInfo->position, position);
	//correct height of decal
	//make sure position is above ground
 	decalInfo->position[1] += METERTOUNIT(1.0f);
 	height = GetHeightOfPoint(decalInfo->position);
	//set flush to ground
 	decalInfo->position[1] -= height;
	//set offset for decal
 	decalInfo->position[1] += FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET;// PP: FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET: see header

	decalInfo->xDim = UNITTOMETER(xDim);
	decalInfo->yDim = UNITTOMETER(yDim);
	decalInfo->rotation = rotation;
	decalInfo->completed = FALSE;

	decalInfo->texture = bkLoadTexture(NULL,"extras\\wantedlogomagenta.bmp",0);

	bmMatScale(scaleMatrix, decalInfo->xDim, 1.0f, decalInfo->yDim);
	bmMatYRotation(rotMatrix, decalInfo->rotation);
	bmMatTranslate(transMatrix, decalInfo->position[0],decalInfo->position[1],decalInfo->position[2]);
	bmMatMultiply(concMatrix, rotMatrix, scaleMatrix);
	bmMatMultiply(decalInfo->localToWorld, transMatrix, concMatrix);

	CreateFloorPolisherWantedPosterMesh(decalInfo);

	//create overlay for progress bar
	decalInfo->bookPtr = bookPtr = new BOOK;

	// PP: get rid of the default flags that adjust the book in various ways for 2D projection
	BOOK3D(decalInfo->bookPtr);
	decalInfo->bookPtr->flags &= ~BKFLAG_BOOKLIST;

	decalInfo->pagePtr = pagePtr = bookPtr->addPage();

	bookPtr->setBookRect(RECTANGLE(-100,100,-100,100));

	pagePtr->insertItem(decalInfo->textboxPtr = new TEXTBOX(),true);
	decalInfo->textboxPtr->flags |= TBFLAG_DONTPRINTLITE;
	decalInfo->textboxPtr->setScale(6.0f);// PP: WAS 3.0f - STUFF HAS CHANGED, ALTHOUGH I WOULDN'T'VE EXPECTED IT TO AFFECT THIS - HMMM...

	// PP: use a simple fade effect on the text appearing/disappearing
	decalInfo->textboxPtr->setEffect(&PIE_fade);

	//calculate local to world matrix for overlay
	bmMatScale(scaleMatrix,3.0f, 3.0f, 3.0f);
	bmMatYRotation(rotMatrix, decalInfo->rotation);
	bmMatXRotation(xRot, RAD(90.0f));
	bmMatTranslate(transMatrix,decalInfo->position[0],(decalInfo->position[1]+FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET),decalInfo->position[2]);// PP: FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET: see header
	bmMatMultiply(concMatrix, xRot, scaleMatrix);
	bmMatMultiply(scaleMatrix, rotMatrix, concMatrix);
	bmMatMultiply(decalInfo->overlayLocalToWorld, transMatrix, scaleMatrix);
	return decalInfo;
}

/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisherWantedPosterMesh
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateFloorPolisherWantedPosterMesh(WANTEDPOSTERDECALINFO *decalInfo)
{
	TBVector	currentPoint, texCoords;
	int			i, j;
	float		texRes, worldRes;

	worldRes = METERTOUNIT(1.0f)/ WPDECAL_STRIPS;
	texRes = 1.0f / (WPDECAL_STRIPS);

	for(i=0;i<WPDECAL_STRIPS;i++)
	{
		//set start point of strip
		SETVECTOR(currentPoint, (METERTOUNIT(-0.5f)+(i*worldRes)), 0.0f, (METERTOUNIT(-0.5f)), 1.0f);
		SETVECTOR(texCoords, (0.0f+(i*texRes)), 0.0f, 1.0f, 0.0f);

		for(j=0;j<WPDECAL_VERTS;j++)
		{
			if(j%2)
			{
				//position
				currentPoint[0] += worldRes;
				//texture
				texCoords[0] += texRes;
			}

			BDVERTEX_SETXYZW(&decalInfo->mesh[i][j], currentPoint[0],currentPoint[1],currentPoint[2],1.0f);
			BDVERTEX_SETNORMAL(&decalInfo->mesh[i][j], 0.0f, 1.0f, 0.0f);
			BDVERTEX_SETRGBA(&decalInfo->mesh[i][j], 128, 128, 128, 128);
			BDVERTEX_SETUV(&decalInfo->mesh[i][j], texCoords[0], texCoords[2]);

			if(j%2)
			{
				//position
				currentPoint[0] -= worldRes;
				currentPoint[2] += worldRes;
				//texture
				texCoords[0] -= texRes;
				texCoords[2] -= texRes;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : UpdateFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO *decalInfo)
{
	TBVector		polisherPos;
	TBVector		tempVector;
	TBVector		maxBounds, minBounds;
	int				i, j,random;
	int				counter;
#if BPLATFORM == PS2
	float			alpha;
#else
	int				alpha;
#endif


	if(decalInfo == NULL) return;

	counter = 0;

	if((gameStatus.player1->actorStatus->currentState == STATE_VEHICLE)
		&&(gameStatus.player1->actorStatus->vehicleInfo))
	{
		if(gameStatus.player1->actorStatus->vehicleInfo->type == VEHICLE_POLISHER)
		{
			//fist check that player is remotely near decal
			bmVectorCopy(polisherPos, gameStatus.player1->actorInstance.position);
			bmVectorSub(tempVector, polisherPos, decalInfo->position);
			if(bmFAbs(tempVector[1]) > METERTOUNIT(0.2f))
			{
				if(decalInfo->bookPtr)
				{
					decalInfo->bookPtr->close();
				}
				return;
			}
			if(bmVectorLen(tempVector) > METERTOUNIT(decalInfo->xDim*1.5f))
			{
				if(decalInfo->bookPtr)
				{
					decalInfo->bookPtr->close();
				}
				return;
			}

			//open progressbar book
			if(decalInfo->bookPtr)
			{
				decalInfo->bookPtr->open();
			}

			//create bounds for box test on polisher
			maxBounds[0] = polisherPos[0] + FLOORPOLISHER_RADIUS;
			maxBounds[2] = polisherPos[2] + FLOORPOLISHER_RADIUS;
			minBounds[0] = polisherPos[0] - FLOORPOLISHER_RADIUS;
			minBounds[2] = polisherPos[2] - FLOORPOLISHER_RADIUS;

			for(i=0;i<WPDECAL_STRIPS;i++)
			{
				for(j=0;j<WPDECAL_VERTS;j++)
				{
					BDVERTEX_GETA(&decalInfo->mesh[i][j], alpha);
					if (!alpha)
					{
						counter++;
						continue;
					}

					BDVERTEX_GETXYZW(&decalInfo->mesh[i][j], tempVector[0], tempVector[1], tempVector[2], tempVector[3]);
					bmMatMultiplyVector(decalInfo->localToWorld, tempVector);
					if((tempVector[0] > maxBounds[0])||(tempVector[0] < minBounds[0])) continue;
					if((tempVector[2] > maxBounds[2])||(tempVector[2] < minBounds[2])) continue;
					//now do vector length test to avoid being square dude
					bmVectorSub(tempVector, polisherPos ,tempVector);
					if(bmVectorLen(tempVector) > FLOORPOLISHER_RADIUS) continue;

					//now can start buffing

#if BPLATFORM == PS2
					if((alpha -= POLISHER_BUF_RATE*fTime) < 0.0f)
					{
						alpha = 0.0f;
						counter++;
					}
#else
					alpha = alpha<<8;
					alpha -= bmFloatToInt(POLISHER_BUF_RATE*fTime);
					if((alpha = alpha>>8) < 0)
					{
						alpha = 0;
						counter++;
					}
#endif
					BDVERTEX_SETA(&decalInfo->mesh[i][j], alpha);
				}
			}
		}
	}
	else
	{
		if(decalInfo->bookPtr)
		{
			decalInfo->bookPtr->close();
		}
	}

	//check state of decal
	if(!decalInfo->completed)
	{
		if(counter > (int)(WPDECAL_NEEDED*(float)((WPDECAL_VERTS-2)*WPDECAL_STRIPS)))
		{
			if(characterSounds.sam == -1)
			{
				random = bmRand()%2;
				switch(random)
				{
				case 0:
					CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_13a.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
					break;
				case 1:
					CallbackWhenSampleFinished((characterSounds.sam=PlaySample("sam_voc_14d.wav",255)),CharacterSoundsFinishedCallback,&characterSounds.sam);
					break;
				}
			}
//			AddMoneyToBounty(gameStatus.player1,NULL, 5000);
			// TP: floor polisher is wanted poster 1
			AddWantedPosterToStats(1,gameStatus.player[0].actorInstance,gameStatus.currentScene);
			//bkGenerateEvent("trigger","_wantedposter=4");
			if(decalInfo->bookPtr)
			{
				decalInfo->bookPtr->close();
//				delete decalInfo->bookPtr;
//				decalInfo->bookPtr = NULL;
			}
			decalInfo->completed = TRUE;
		}
		//update progress bar book
		if(decalInfo->bookPtr)
		{
			decalInfo->textboxPtr->sprintf8("%d%%",(int)((float)counter/(float)((WPDECAL_VERTS-2)*WPDECAL_STRIPS)*100.0f/WPDECAL_NEEDED));
			decalInfo->bookPtr->privateUpdate();
		}
	}
	else
	{
		if(decalInfo->bookPtr)
		{
			BookList_closeAndShutdown(*decalInfo->bookPtr);
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : DrawFloorPolisherWantedPoster
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void DrawFloorPolisherWantedPoster(WANTEDPOSTERDECALINFO *decalInfo)
{
	int			i;
	TBVector	tempVector;
	float		tempFloat;

	if(decalInfo == NULL) return;
	
	bmVectorSub(tempVector,gameStatus.viewPoint,decalInfo->position);
	tempFloat = bmVectorLen(tempVector);

	if (tempFloat > METERTOUNIT(30.0f)) return;

	bdSetObjectMatrix(decalInfo->localToWorld);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, FALSE);
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_CLAMPU|BDTEXTUREWRAP_CLAMPV, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, TRUE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	// PP: if the Qwilson cheat is active...
	if(CheatActive(CHEAT_QWILSON))
	{
		// PP: ...use Qwilson as the decal...
		bdSetTexture(0,QwilsonTexture);
	}
	else
	{
		// PP: ...otherwise use the proper Wanted texture.
		bdSetTexture(0,decalInfo->texture);
	}

	for(i=0;i<WPDECAL_STRIPS;i++)
	{
		ASSERT(WPDECAL_VERTS);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, &decalInfo->mesh[i][0], WPDECAL_VERTS, BVERTTYPE_SINGLE);
	}

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, TRUE);

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	if(decalInfo->bookPtr)
	{
		bdSetObjectMatrix(decalInfo->overlayLocalToWorld);

		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);	
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, TRUE);

		//draw progress bar
		decalInfo->bookPtr->privateDraw();
	
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, TRUE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, TRUE);
	}

	bdSetIdentityObjectMatrix();
}

/* --------------------------------------------------------------------------------
   Function : FreeFloorPolisherWantedPosterInfo
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void FreeFloorPolisherWantedPosterInfo(WANTEDPOSTERDECALINFO **decalInfo)
{
	if(*decalInfo)
	{
		delete (*decalInfo)->bookPtr;
		FREE(*decalInfo);
		*decalInfo = NULL;
	}
}