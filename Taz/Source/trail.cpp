// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : trail.cpp
//   Purpose : find out where a character has been
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "actors.h"
#include "characters.h"
#include "maths.h"
#include "debris.h"
#include "physics.h"

#include "trail.h"

#define TRAILFLAG_CAMERAIGNORE	(1)

typedef struct TAG_TRAILRECORD
{
	TBVector	pos;
	float		time;
	int			flags;
}TRAILRECORD;

typedef struct TAG_TRAIL
{
	int		trailStart;
	int		trailEnd;
	int		trailSize;
	TRAILRECORD	*pTrail;
}TRAIL;


/*
static int trailStart = 0;
static int trailEnd = 0;
*/

static TBTexture *pTexture = NULL;


/* --------------------------------------------------------------------------------
   Function : trailSetUp()
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void trailSetUp(ACTORINSTANCE *pActorInstance)
{
	if (!pActorInstance->characterInfo) return;

	pActorInstance->characterInfo->pTrail = (TRAIL *)ZALLOC(sizeof(TRAIL));
	ASSERT(pActorInstance->characterInfo->pTrail);
	
	pActorInstance->characterInfo->pTrail->pTrail = (TRAILRECORD *)ZALLOC(sizeof(TRAILRECORD) * (TRAIL_MAXBUFFERSIZE));

	pActorInstance->characterInfo->pTrail->trailStart = 0;
	pActorInstance->characterInfo->pTrail->trailEnd = 0;
	pActorInstance->characterInfo->pTrail->trailSize = 0;
}


void trailFree(ACTORINSTANCE *pActorInstance)
{
	if(pActorInstance->characterInfo->pTrail)
	{
		SAFE_FREE(pActorInstance->characterInfo->pTrail->pTrail);

		SAFE_FREE(pActorInstance->characterInfo->pTrail);
	}
}

void trailReset(ACTORINSTANCE *pActorInstance, int action)
{
	int c;

	switch(action)
	{
	case TRAIL_RESET_ALL:
		pActorInstance->characterInfo->pTrail->trailStart = 0;
		pActorInstance->characterInfo->pTrail->trailEnd = 0;
		pActorInstance->characterInfo->pTrail->trailSize = 0;
		break;

	case TRAIL_RESET_CAMIGNORE:
		for(c = pActorInstance->characterInfo->pTrail->trailStart; c != pActorInstance->characterInfo->pTrail->trailEnd;)
		{
			(pActorInstance->characterInfo->pTrail->pTrail + c)->flags |= TRAILFLAG_CAMERAIGNORE;
			c++;
			if(c >= TRAIL_MAXBUFFERSIZE)
			{
				c = 0;
			}
		}
		break;
	}
}


/* --------------------------------------------------------------------------------
   Function : trailAddPosition()
   Purpose :  Adds a position to the trail (well you did ask)
   Parameters : 
   Returns : 
   Info : 
*/

void trailAddPosition(ACTORINSTANCE *pActorInstance)
{
	TBVector temp, realPos;
	int trailTest = pActorInstance->characterInfo->pTrail->trailEnd - 1;

	int c;

	if(trailTest < 0)
		trailTest = TRAIL_MAXBUFFERSIZE - 1;

	bmVectorAdd(realPos, pActorInstance->actorBehaviour->collSphere.offset, pActorInstance->actorInstance.position);
	bmVectorSub(temp, realPos, (pActorInstance->characterInfo->pTrail->pTrail + trailTest)->pos);

	if(gameStatus.gameTime != 0.0f)
	{
		if((pActorInstance->characterInfo->pTrail->trailStart!=pActorInstance->characterInfo->pTrail->trailEnd) && (bmVectorLen(temp) < METERTOUNIT(TRAIL_STEPSIZE)))
			return;
	}

	ASSERT(pActorInstance->characterInfo->pTrail->trailEnd >= 0);
	ASSERT(pActorInstance->characterInfo->pTrail->trailEnd < TRAIL_MAXBUFFERSIZE);

	bmVectorCopy((pActorInstance->characterInfo->pTrail->pTrail + pActorInstance->characterInfo->pTrail->trailEnd)->pos, realPos);
	(pActorInstance->characterInfo->pTrail->pTrail + pActorInstance->characterInfo->pTrail->trailEnd)->flags = 0;

	pActorInstance->characterInfo->pTrail->trailEnd++;
	pActorInstance->characterInfo->pTrail->trailSize++;

	if(pActorInstance->characterInfo->pTrail->trailEnd == TRAIL_MAXBUFFERSIZE)
		pActorInstance->characterInfo->pTrail->trailEnd = 0;

	if(pActorInstance->characterInfo->pTrail->trailEnd == pActorInstance->characterInfo->pTrail->trailStart)
	{
		pActorInstance->characterInfo->pTrail->trailStart++;
		pActorInstance->characterInfo->pTrail->trailSize--;

		if(pActorInstance->characterInfo->pTrail->trailStart == TRAIL_MAXBUFFERSIZE)
			pActorInstance->characterInfo->pTrail->trailStart = 0;
	}

	// CPW: check if our new point is too close to an older point
	if(pActorInstance->characterInfo->pTrail->trailSize > 2)
	{
		for(c = pActorInstance->characterInfo->pTrail->trailStart;;)
		{
			float size;
			int thisone = pActorInstance->characterInfo->pTrail->trailEnd - 1;
			int lastone = pActorInstance->characterInfo->pTrail->trailEnd - 2;

			if(thisone<0)
				thisone += TRAIL_MAXBUFFERSIZE;

			if(lastone<0)
				lastone += TRAIL_MAXBUFFERSIZE;

			size = bmVectorDistance((pActorInstance->characterInfo->pTrail->pTrail + c)->pos,
					(pActorInstance->characterInfo->pTrail->pTrail + thisone)->pos);

			if(c!=thisone)
			{
				if(size < (METERTOUNIT(TRAIL_STEPSIZE) * 0.9))
				{
// CPW: 					trailShorten(pActorInstance->characterInfo->pTrail, c);

					pActorInstance->characterInfo->pTrail->trailEnd = c;
					pActorInstance->characterInfo->pTrail->trailSize = (pActorInstance->characterInfo->pTrail->trailEnd - pActorInstance->characterInfo->pTrail->trailStart) - 1;
					if(pActorInstance->characterInfo->pTrail->trailSize < 0)
						pActorInstance->characterInfo->pTrail->trailSize += TRAIL_MAXBUFFERSIZE;

// CPW: 					bkPrintf("Pruning!\n");
					break;
				}
			}

			c++;

			if(c >= TRAIL_MAXBUFFERSIZE)
				c = 0;
			
			if(c == pActorInstance->characterInfo->pTrail->trailEnd)
				break;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : trailGetPosFromAgesAgo()
   Purpose :  
   Parameters : 
   Returns : 
   Info : 
*/

int trailGetPosFromFramesAgo(ACTORINSTANCE *pActorInstance, TBVector pPos, int framesAgo)
{
	int actualFrame;
	int couldSetIt = FALSE;

	if(framesAgo >= TRAIL_MAXBUFFERSIZE)
		framesAgo = TRAIL_MAXBUFFERSIZE - 1;

	if(framesAgo >= pActorInstance->characterInfo->pTrail->trailSize)
		framesAgo = pActorInstance->characterInfo->pTrail->trailSize;
	else
		couldSetIt = TRUE;

	actualFrame = (pActorInstance->characterInfo->pTrail->trailEnd - framesAgo) - 1;

	if(actualFrame < 0)
		actualFrame = TRAIL_MAXBUFFERSIZE - 1;

	bmVectorCopy(pPos, (pActorInstance->characterInfo->pTrail->pTrail + actualFrame)->pos);

	return couldSetIt;
}

/* --------------------------------------------------------------------------------
   Function : trailFindNearestPoint
   Purpose :  finds nearest point in trail to supplied vector
   Parameters : 
   Returns : 
   Info : 
*/

int trailFindNearestPoint(ACTORINSTANCE *pActorInstance, TBVector pPoint, TBVector pDest)
{
	int c;

	float smallestSize;
	int smallestIndex;
	float size;

	if(!pActorInstance->characterInfo)
		return -1;

	if(pActorInstance->characterInfo->pTrail->trailStart == pActorInstance->characterInfo->pTrail->trailEnd)
		return -1;

	c = pActorInstance->characterInfo->pTrail->trailStart;
	smallestSize = bmVectorSquaredDistance((pActorInstance->characterInfo->pTrail->pTrail + c)->pos, pPoint);
	smallestIndex = c;

	if(pActorInstance->characterInfo->pTrail->trailStart != pActorInstance->characterInfo->pTrail->trailEnd)
	{
		for(c = pActorInstance->characterInfo->pTrail->trailStart;;)
		{
			size = bmVectorDistance((pActorInstance->characterInfo->pTrail->pTrail + c)->pos, pPoint);

			if(size < smallestSize)
			{
				smallestSize = size;
				smallestIndex = c;
			}

			c++;

			if(c >= TRAIL_MAXBUFFERSIZE)
				c = 0;
			
			if(c == pActorInstance->characterInfo->pTrail->trailEnd)
				break;
		}
	}

	if(pDest)
		bmVectorCopy(pDest, (pActorInstance->characterInfo->pTrail->pTrail + smallestIndex)->pos);

	if((smallestIndex > TRAIL_MAXBUFFERSIZE) || (smallestIndex < 0))
	{
		bkPrintf("ERROR: There has been a problem of the highest order!\n");
	}

	return smallestIndex;

}

/* --------------------------------------------------------------------------------
   Function : trailFindBestPoint
   Purpose :  finds 'best' point in trail to supplied vector
   Parameters : 
   Returns : 
   Info : 
*/

int trailFindBestPoint(ACTORINSTANCE *pActorInstance, TBVector pPoint, TBVector pDest)
{
	int c;
	int bestDist = 4;

	if(!pActorInstance->characterInfo)
		return -1;

	if(pActorInstance->characterInfo->pTrail->trailSize <= bestDist)
		return -1;

	if(pActorInstance->characterInfo->pTrail->trailStart == pActorInstance->characterInfo->pTrail->trailEnd)
		return -1;

	c = pActorInstance->characterInfo->pTrail->trailEnd - bestDist;

	if(c < 0)
		c = TRAIL_MAXBUFFERSIZE - 1;

	if(pDest)
		bmVectorCopy(pDest, (pActorInstance->characterInfo->pTrail->pTrail + c)->pos);


	return c;
}


/* --------------------------------------------------------------------------------
   Function : trailGetIndexedPoint
   Purpose :  finds point 
   Parameters : 
   Returns : 
   Info : 
*/

int trailGetIndexedPoint(ACTORINSTANCE *pActorInstance, int index, TBVector pPoint)
{
	bmVectorCopy(pPoint, (pActorInstance->characterInfo->pTrail->pTrail + index)->pos);

	return 0;
}

void trailAdvanceToNext(ACTORINSTANCE *pActorInstance, int *pIndex)
{

	(*pIndex)++;

	if(*pIndex == pActorInstance->characterInfo->pTrail->trailEnd)
	{
		(*pIndex)--;
		return;
	}

	if(*pIndex == TRAIL_MAXBUFFERSIZE)
		*pIndex = 0;
}


/* --------------------------------------------------------------------------------
   Function : trailDraw()
   Purpose :  Draws trail
   Parameters : 
   Returns : 
   Info : 
*/

void trailDraw(ACTORINSTANCE *pActorInstance)
{
#ifdef CHRISCAM

	int c;
	TBVector pos, vel;
	float ang = gameStatus.gameTime * 2;

	if((pActorInstance->characterInfo->pTrail->trailStart != pActorInstance->characterInfo->pTrail->trailEnd) && pTexture)
	{
		ZEROVECTOR(vel);

		for(c = pActorInstance->characterInfo->pTrail->trailStart; c != pActorInstance->characterInfo->pTrail->trailEnd; c++)
		{
			if(c >= TRAIL_MAXBUFFERSIZE)
				c = -1;

			// CPW: draw sprite here
			bmVectorCopy(pos, (pActorInstance->characterInfo->pTrail->pTrail + c)->pos);
			pos[W] = 1.0f;

			bdSetIdentityObjectMatrix();
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);

			bdDrawFlatSprite(pos, METERTOUNIT(0.2), METERTOUNIT(0.2),
							ang, pTexture, 0, 0, 127, 127, 127, 127, NULL,0);
		
			bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
			bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
		}
	}

#endif
}

float trailHowFarBehind(ACTORINSTANCE *pActorInstance, TBVector pCameraPos)
{
	int camPoint;
	TBVector nearestPoint;

	if(!pActorInstance)
		return -1;

	camPoint = trailFindNearestPoint(pActorInstance, pCameraPos, nearestPoint);

	// CPW: bkPrintf("camPoint = %d\n", camPoint);

	if(~camPoint)
	{
		int dist;
		float fdist, fcamdist;

		dist = pActorInstance->characterInfo->pTrail->trailEnd - camPoint;
		if(dist < 0)
			dist+=TRAIL_MAXBUFFERSIZE;

		fdist = ((float)dist) * METERTOUNIT(TRAIL_STEPSIZE);
		fcamdist = bmVectorDistance(pCameraPos, nearestPoint);

		// CPW: bkPrintf("TOTAL dist = %f, trail dist = %f, cam dist = %f\n", fdist + fcamdist, fdist, fcamdist);

		return (fdist + fcamdist);
	}
	else
		return 0.0f;
}
