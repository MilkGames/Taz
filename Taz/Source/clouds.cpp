// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : collectibles.cpp
//   Purpose : functions for creating and updating collectibles
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "clouds.h"
#include "scene.h"
#include "maths.h"

CLOUDBANK		cloudBank;

/* --------------------------------------------------------------------------------
   Function : InitialiseCloudBank
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void InitialiseCloudBank(void)
{
	cloudBank.numEntries = 0;
	cloudBank.head.next = cloudBank.head.prev = &cloudBank.head;
}

/* --------------------------------------------------------------------------------
   Function : CreateCloudsInSky
   Purpose : creates clouds in the sky, doh!
   Parameters : bounds[0]=minRadius, bounds[1]=maxRadius, bounds[2]=minHeight, bounds[3]=maxHeight
   Returns : 
   Info : 
*/

int CreateCloudsInSky(TBVector bounds, int number, float speed, int maxAlpha, int minAlpha, uint32 type, ACTORINSTANCE *followActor, TBActorNodeInstance *followNode, TBVector offset)
{
	SKYCLOUD		*tempCloud;
	int				radiusExtents, heightExtents;
	int				counter;
	TBActor			*actor;
	float			scale;
	TBVector		yAxis = {0.0f, 1.0f, 0.0f, 1.0f};
	TBVector		tempVector;

	radiusExtents = (int)(bounds[1]-bounds[0]);
	heightExtents = (int)(bounds[3]-bounds[2]);

	for(counter = 0; counter < number; counter++)
	{
		//create actorInstance according to the type of cloud
		switch(type)
		{
		case SKYCLOUD_WACKY:
			actor = FindActorInActorList("objects\\cloud.obe");
			if(actor)
			{
				if((tempCloud = AddEntryToCloudBank()) == NULL) continue;
				baInitActorInstance(&tempCloud->cloudInstance, actor, 0);
				scale = 1.0f + ((float)(bmRand()%10)/10.0f);
				SETVECTOR(tempCloud->cloudInstance.scale, scale, scale, scale, 1.0f);
				baPrepActorInstance(&tempCloud->cloudInstance,TRUE);
				baSetActorRenderMode(&tempCloud->cloudInstance, 0, BACTRENDERMODE_NORMAL, NULL);
				baSetActorRenderMode(&tempCloud->cloudInstance, 1, BACTRENDERMODE_NONE, NULL);
				tempCloud->drawFlags = DRAW_CLOUDS_IN_BACKGROUND;
				tempCloud->drawFlags |= DRAW_CLOUDS_FIXED_HEIGHT;
			}
			else
			{
				return FALSE;
			}
			break;
		case PLAYER_STARS:
			actor = FindActorInActorList("injuredstar.obe");
			//actor = FindActorInActorList("buzzybee.obe");
			if(actor)
			{
				if((tempCloud = AddEntryToCloudBank()) == NULL) continue;
				baInitActorInstance(&tempCloud->cloudInstance, actor, 0);
				scale = 1.0f + ((float)(bmRand()%10)/10.0f);
				SETVECTOR(tempCloud->cloudInstance.scale, scale, scale, scale, 1.0f);
				baPrepActorInstance(&tempCloud->cloudInstance, TRUE);
				baSetActorRenderMode(&tempCloud->cloudInstance, 0, BACTRENDERMODE_NORMAL, NULL);
				baSetActorRenderMode(&tempCloud->cloudInstance, 1, BACTRENDERMODE_NONE, NULL);
				tempCloud->drawFlags = DRAW_CLOUDS_IN_FOREGROUND;
			}
			else
			{
				return FALSE;
			}
			break;
		}

		if(tempCloud)
		{
			//set initial position allong z axis
			if(heightExtents > 0.0f)
			{
				tempCloud->initialHeight = bounds[2] + (float)(bmRand()%heightExtents);
			}
			else
			{
				tempCloud->initialHeight = bounds[2];
			}
			if(radiusExtents > 0.0f)
			{
				tempCloud->initialRadius = bounds[0] + (float)(bmRand()%radiusExtents);
			}
			else
			{
				tempCloud->initialRadius = bounds[0];
			}

			//set initial Angle
			tempCloud->initialAngle = RAD((float)(bmRand()%360));
			tempCloud->currentAngle = tempCloud->initialAngle;
			if(type == PLAYER_STARS)
			{
				if(bmRand()%2)
				{
					tempCloud->angularVelocity = speed;
				}
				else
				{
					tempCloud->angularVelocity = -speed;
				}
				//calculate off vertical rotation axis
				tempVector[0] = (float)(bmRand()%50)/100.0f;
				tempVector[1] = 1.0f;
				tempVector[2] = (float)(bmRand()%50)/100.0f;
				tempVector[3] = 1.0f;
				bmVectorNorm(tempCloud->rotationAxis, tempVector);
			}
			else
			{
				tempCloud->angularVelocity = speed*((float)(bmRand()%10)/10.0f);
				bmVectorCopy(tempCloud->rotationAxis, yAxis);
			}
			tempCloud->type = type;
			//calculate alpha
			tempCloud->alpha = maxAlpha - (int)((float)((maxAlpha-minAlpha)*((tempCloud->initialHeight-bounds[2])/(heightExtents))));

			if(followActor)
			{
				tempCloud->followActor = followActor;
				if(followNode)
				{
					tempCloud->followNode = followNode;
				}
				else tempCloud->followNode = NULL;
			}
			else tempCloud->followActor = NULL;

			if(offset != NULL)
			{
				bmVectorCopy(tempCloud->offset, offset);
			}
			else
			{
				SETVECTOR(tempCloud->offset, 0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : AddEntryToCloudBank
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

SKYCLOUD *AddEntryToCloudBank(void)
{
	SKYCLOUD		*newEntry;
	SKYCLOUD		*tempPtr;

	tempPtr = cloudBank.head.next;

	//create new structure for incoming information
	if((newEntry = (SKYCLOUD *)ZALLOC(sizeof(SKYCLOUD))) == NULL)
	{
		bkPrintf("**** ERROR **** cloud bank MALLOC failed\n");
		return NULL;
	}

	newEntry->next = tempPtr;
	newEntry->prev = tempPtr->prev;
	tempPtr->prev->next = newEntry;
	tempPtr->prev = newEntry;
	cloudBank.numEntries++;
	return newEntry;
}

/* --------------------------------------------------------------------------------
   Function : AddEntryToCloudBank
   Purpose : does as the name suggests
   Parameters : 
   Returns : 
   Info : 
*/

int DeleteEntryFromCloudBank(SKYCLOUD *remPtr)
{
	SKYCLOUD  *ptr;

	if(remPtr == NULL)
	{
		bkPrintf("**** ERROR **** pointer to be removed from cloud list is NULL");
		return FALSE;
	}

	for (ptr = cloudBank.head.next;ptr != &cloudBank.head;ptr = ptr->next)
	{
		if (ptr == remPtr)
		{
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			cloudBank.numEntries--;
			FREE(remPtr);
			return TRUE;
		}
	}
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function :UpdateCloudsInSky
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void UpdateCloudsInSky(void)
{
	SKYCLOUD		*ptr;
	TBVector		nodePosition;
	
	if(cloudBank.head.next != NULL)
	{
		for (ptr = cloudBank.head.next;ptr != &cloudBank.head;ptr = ptr->next)
		{
			if(&ptr->cloudInstance)
			{
				if((ptr->currentAngle+=(ptr->angularVelocity/fps)) > RAD(360.0f))
				{
					ptr->currentAngle -= RAD(360.0f);
				}
				ptr->rotationAxis[3] = -ptr->currentAngle;
				bmRotationToQuat(ptr->cloudInstance.orientation, ptr->rotationAxis);
				if(ptr->followActor)
				{
					if(ptr->followNode)
					{
						baGetNodesWorldPosition(&ptr->followActor->actorInstance, ptr->followNode, nodePosition);
						ptr->cloudInstance.position[0] = nodePosition[0] + (ptr->initialRadius*(bmSin(ptr->currentAngle)));
						ptr->cloudInstance.position[2] = nodePosition[2] + (ptr->initialRadius*(bmCos(ptr->currentAngle)));
					}
					else
					{
						ptr->cloudInstance.position[0] = ptr->followActor->actorInstance.position[0] + (ptr->initialRadius*(bmSin(ptr->currentAngle)));
						ptr->cloudInstance.position[2] = ptr->followActor->actorInstance.position[2] + (ptr->initialRadius*(bmCos(ptr->currentAngle)));
					}
				}
				if(ptr->drawFlags & DRAW_CLOUDS_FIXED_HEIGHT)
				{
					ptr->cloudInstance.position[1] = ptr->initialHeight;
				}
				else
				{
					if(ptr->followActor)
					{
						if(ptr->followNode)
						{
							ptr->cloudInstance.position[1] = nodePosition[1] + ptr->initialHeight;
						}
						else
						{
							ptr->cloudInstance.position[1] = ptr->followActor->actorInstance.position[1] + ptr->initialHeight;
						}
					}
				}
				//always add offset, if passed as null when created, it will equal zero
				bmVectorAdd(ptr->cloudInstance.position, ptr->cloudInstance.position, ptr->offset);
				ptr->cloudInstance.position[3] = 1.0f;
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function :FreeCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void FreeCloudBank()
{
	SKYCLOUD		 *ptr,*temp;

	bkPrintf("FreeCloudBank\n");

	for (ptr = cloudBank.head.next;ptr != &cloudBank.head;)
	{
		if(&ptr->cloudInstance) baFreeActorInstance(&ptr->cloudInstance);

		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
		cloudBank.numEntries--;

		temp = ptr->next;
		FREE(ptr);
		ptr = temp;
	}
}

/* --------------------------------------------------------------------------------
   Function :PrepCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void PrepCloudBank(void)
{
	SKYCLOUD		 *ptr;

	for (ptr = cloudBank.head.next;ptr != &cloudBank.head;ptr = ptr->next)
	{
		baPrepActorInstance(&ptr->cloudInstance,TRUE);
	}
}

/* --------------------------------------------------------------------------------
   Function :DrawCloudBank
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DrawCloudBankBackground(void)
{
	SKYCLOUD		 *ptr;

	if (cloudBank.numEntries==0) return;

	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	for (ptr = cloudBank.head.next;ptr != &cloudBank.head;ptr = ptr->next)
	{
		if(ptr->drawFlags & DRAW_CLOUDS_IN_BACKGROUND)
		{
			baSetGlobalAlpha(ptr->alpha);
			baDrawActorInstance(&ptr->cloudInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
		}
	}

	baSetGlobalAlpha(256);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function :DrawCloudBankForeground
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DrawCloudBankForeground(void)
{
	SKYCLOUD		*ptr;

//	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);

	for(ptr = cloudBank.head.next; ptr !=&cloudBank.head; ptr = ptr->next)
	{
		if(ptr->drawFlags & DRAW_CLOUDS_IN_FOREGROUND)
		{
//			baSetGlobalAlpha(ptr->alpha);
			baDrawActorInstance(&ptr->cloudInstance,BSETPRIMALPHATYPE_SOLID|BSETPRIMALPHATYPE_CKEY|BSETPRIMALPHATYPE_ALPHA);
		}
	}

//	baSetGlobalAlpha(255);
//	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
}

/* --------------------------------------------------------------------------------
   Function :CreateInjuryStars
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void CreateInjuryStars(int number, ACTORINSTANCE *actorInstance, char *bone, TBVector offset, TBVector bounds)
{
	TBActorNodeInstance		*node;
	//TBVector	starsVec = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), METERTOUNIT(0.8f), METERTOUNIT(1.0f)};
	//TBVector	starsVec = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};

	if(bone)
	{
		node = baFindNode(actorInstance->actorInstance.rootNodeInstance, bone);
	}
	else node = NULL;

	CreateCloudsInSky(bounds, number, RAD(300.0f), 127, 80, PLAYER_STARS, actorInstance, node, offset);
}


/* --------------------------------------------------------------------------------
   Function		: CreateInjuryStars
   Purpose		: Create standard injury stars
   Parameters	: (opt/player 1) ptr to ACTORINSTANCE for whom to create stars,
					(opt/default suitable for Taz) y offset
   Returns		: 
   Info			: // PP: 
*/
void CreateInjuryStars(ACTORINSTANCE* const actorInstance, const float yOffset)
{
	TBVector		starsBounds = {METERTOUNIT(0.3f), METERTOUNIT(0.4f), 0.0f, 0.0f};
	TBVector		starsOffset = {0.0f, yOffset, 0.0f, 1.0f};

	CreateInjuryStars(10, actorInstance, "spinetop", starsOffset, starsBounds);
}


/* --------------------------------------------------------------------------------
   Function :DestroyInjuryStars
   Purpose :
   Parameters :
   Returns : 
   Info : 
*/

void DestroyInjuryStars(ACTORINSTANCE *actorInstance)
{
	SKYCLOUD		*ptr,*temp;

	for(ptr = cloudBank.head.next; ptr !=&cloudBank.head;)
	{
		temp = ptr->next;
		if(ptr->type == PLAYER_STARS)
		{
			if(actorInstance)
			{
				if(ptr->followActor == actorInstance)
				{
					if(&ptr->cloudInstance) baFreeActorInstance(&ptr->cloudInstance);
					DeleteEntryFromCloudBank(ptr);
				}
			}
			else
			{
				if(&ptr->cloudInstance) baFreeActorInstance(&ptr->cloudInstance);
				DeleteEntryFromCloudBank(ptr);
			}
		}
		ptr = temp;
	}
}