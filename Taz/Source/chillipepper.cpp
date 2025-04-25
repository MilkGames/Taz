// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : bubblegum.cpp
//   Purpose : functions to update and draw bubblegum effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "main.h"
#include "actors.h"
#include "status.h"
#include "characters.h"
#include "maths.h"
#include "debris.h"
#include "physics.h"
#include "status.h"
#include "holysmoke.h"
#include "invisibletaz.h"
#include "control.h"
#include "events.h"
#include "destructibles.h"
#include "debris.h"
#include "genericai.h"
#include "lights.h"
#include "textures.h"
#include "chillipepper.h"
#include "animation.h"
#include "minecart.h"
#include "tazboss1.h"
#include "superburp.h"


/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void CreateCharacterChilliPepperSmoke(ACTORINSTANCE *actorInstance)
{
	TBVector		randVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter, numSprites;
	float			angle, size, life, fifty;
	TBVector		head,leftEar,rightEar, leftEarPos, rightEarPos;
	TBActorNodeInstance	*node;
	TBMatrix		matrix, tempMatrix;

	if((~actorInstance->flags & ACTORFLAG_DONTDRAW)&&(textureSmokeCloud))
	{
		if(fps >= 58.0f) numSprites = MAX_SMOKE_SPRITES;
		else
			if(fps >= 28.0f) numSprites = 2*MAX_SMOKE_SPRITES;
			else numSprites = 3*MAX_SMOKE_SPRITES;

		for(counter = 0; counter < numSprites; counter++)
		{
			angle = RAD((float)(bmRand()%360));
			//set velocity to be a general radial drift
			randVelocity[0] = SLOW_SMOKE_SPEED * bmCos(angle);
			randVelocity[1] = SLOW_SMOKE_SPEED*0.25f;
			randVelocity[2] = SLOW_SMOKE_SPEED * bmSin(angle);

			//find a general velocity out of taz's ears
			node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "hairstem");
			if(!node) return;
			baGetNodesWorldPosition(&actorInstance->actorInstance,node,head);
			SETVECTOR(leftEarPos,METERTOUNIT(0.128f), 0.0f, 0.0f, 1.0f);
			SETVECTOR(rightEarPos,METERTOUNIT(-0.128f), 0.0f, 0.0f, 1.0f);

			bmQuatToMatrix(matrix, node->animState.orientation);
			bmMatMultiply(tempMatrix, node->nodeToLocalWorld,matrix);
			bmMatMultiply(matrix, actorInstance->actorInstance.objectToWorld, tempMatrix);
			
			bmMatMultiplyVector(matrix, leftEarPos);
			bmMatMultiplyVector(matrix, rightEarPos);

			bmVectorSub(leftEar, leftEarPos, head);
			bmVectorScaleToLength(leftEar, leftEar, (CHILLIPEPPER_SMOKE_MIN_SPEED+CHILLIPEPPER_SMOKE_SPEED));

			bmVectorSub(rightEar, rightEarPos, head);
			bmVectorScaleToLength(rightEar, rightEar, (CHILLIPEPPER_SMOKE_MIN_SPEED+CHILLIPEPPER_SMOKE_SPEED));

			//add velocities
			bmVectorAdd(leftEar, randVelocity, leftEar);
			bmVectorAdd(rightEar, randVelocity, rightEar);
						  
			size = CHILLIPEPPER_SMOKE_SIZE;
			life = CHILLIPEPPER_SMOKE_LIFE;
			fifty = 50.0f;
			baCreateExtra(smokeSystem,leftEarPos,leftEar,textureSmokeCloud,&size,&life,&fifty,DEBRISFLAG_CHILLISMOKE|DEBRISFLAG_IMPACTCLOUD);
			baCreateExtra(smokeSystem,rightEarPos,rightEar,textureSmokeCloud,&size,&life,&fifty,DEBRISFLAG_CHILLISMOKE|DEBRISFLAG_IMPACTCLOUD);
			cloudCounter++;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateCharacterChilliPepperPowerUp(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO	*ptr = &actorInstance->characterInfo->powerUpInfo;

	if(ptr->chilliPepper)
	{
//		if(ptr->numFire > 0)
		if((ptr->clock -= fTime) < 0.0f)
		{
			FinishChilliPepper(actorInstance);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: FinishChilliPepper
	Purpose		: Reset Taz after a chilli pepper encounter
	Parameters	: actorinstance
	Returns		: 
	Info		: 
*/
void FinishChilliPepper(ACTORINSTANCE *actorInstance)
{
	POWERUPINFO	*ptr = &actorInstance->characterInfo->powerUpInfo;

	ptr->chilliPepper = FALSE;
	ptr->state = -1;

	// TP: Dont want to reset the character if they were in the middle of a jump etc.
	if (actorInstance->actorStatus->currentState==STATE_CHILLIPEPPER)
	{
		ResetCharacterAfterJump(actorInstance);
		RequestStateChange(actorInstance, STATE_MOVE);
	}
	actorInstance->actorStatus->currentMoveState = STATE_MOVE;
	switch(gameStatus.globalSettings.difficulty)
	{
	case DIFF_EASY:
		ptr->clock = CHILLIPEPPER_EASY;
		break;
	case DIFF_MEDIUM:
		ptr->clock = CHILLIPEPPER_MEDIUM;
		break;
	case DIFF_HARD:
		ptr->clock = CHILLIPEPPER_HARD;
		break;
	}
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/
void CreateChilliPepperFlameThrower(ACTORINSTANCE *actorInstance)
{
	TBVector		position, velocity, smallVel, delta, offset;
	TBMatrix		matrix;
	TBActorNodeInstance	*node;
	int				colour[3];
	float			size, life;

	if(actorInstance->characterInfo->powerUpInfo.state == POWERUPSTATE_CHILLION)
	{
		if(textureChilliFire)
		{
			if( map.sceneNumber == SCENE_TAZPREBOSS )
				life = MEGA_CHILLIPEPPER_FIRE_LIFE;
			else
				life = CHILLIPEPPER_FIRE_LIFE;

			SETVECTOR(velocity, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
								UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
								(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);
			//need small displacement to seperate the sprites early in their life time
			SETVECTOR(smallVel,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

			bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);

			bmMatMultiplyVector(matrix, velocity);
			bmVectorScaleToLength(velocity, velocity, CHILLIPEPPER_FIRE_SPEED);

			bmVectorSet(delta, 0.0f, 0.0f, 0.0f, 1.0f);
			bmVectorSet(offset, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(matrix, offset);
			bmVectorScaleToLength(offset, offset, CHILLIPEPPER_FIRE_SPEED*0.25f*fTime);
			
			for(int i=0;i<4;i++)
			{
				if((size = CHILLIPEPPER_FIRE_MAXSIZE*smallVel[2]/bmFloatToInt(CHILLIPEPPER_FIRE_RANDDISP)) < CHILLIPEPPER_FIRE_MINSIZE)
				{
					size = CHILLIPEPPER_FIRE_MINSIZE;
				}

				bmMatMultiplyVector(matrix, smallVel);

				node = baFindNode(actorInstance->actorInstance.rootNodeInstance, "toungemid");
				if(node)
				{
					baGetNodesWorldPosition(&actorInstance->actorInstance, node, position);
					bmVectorAdd(position, smallVel, position);

 
					if(bmRand()%2)
					{
						colour[0]=255;colour[1]=38;colour[2]=10;
					}
					else
					{
						colour[0]=255;colour[1]=186;colour[2]=10;
					}
					bmVectorAdd(position, position, delta);
					//texture
					float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
					baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&life,&rand,&size,colour[0],colour[1],colour[2],CHILLIPEPPER_MAX_ALPHA,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_DOCOLLISION);

					bmVectorAdd(delta, delta, offset);
				}
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void ChilliPepperExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
    CHILLIPEPPEREXTRAS		*data = (CHILLIPEPPEREXTRAS*)vData;
	float					*position;
	float					*velocity;

	position = va_arg(parms, float*);
	bmVectorCopy(data->position, position);
	velocity = va_arg(parms, float*);
	bmVectorCopy(data->velocity, velocity);
	data->texture = (TBTexture*)va_arg(parms, TBTexture*);
	data->life = *va_arg(parms, float*);
	data->rotation = *va_arg(parms, float*);
	data->size = *va_arg(parms, float*);
	data->colour[0] = (float)va_arg(parms, int);
	data->colour[1] = (float)va_arg(parms, int);
	data->colour[2] = (float)va_arg(parms, int);
	data->colour[3] = (float)va_arg(parms, int);
	data->flags = (uint32)va_arg(parms, int);
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperFire(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	CHILLIPEPPEREXTRAS		*data;
	float maxLife, maxVel, maxSize, colStep, rate;
	int i, j;
	TBVector		posInc;

	for (j=0;j<noofInstances;++j)
	{
		data = (CHILLIPEPPEREXTRAS*)dataPtrs[j];

		if( map.sceneNumber == SCENE_TAZPREBOSS )
		{
			maxLife = (MEGA_CHILLIPEPPER_FIRE_LIFE-(MEGA_CHILLIPEPPER_FIRE_LIFE*-0.5f));
			colStep = (20.0f/MEGA_CHILLIPEPPER_FIRE_LIFE);
			maxVel = 1.0f - (0.1f*(1.0f/MEGA_CHILLIPEPPER_FIRE_LIFE));
			maxSize = METERTOUNIT(1.0f)*(1.0f/MEGA_CHILLIPEPPER_FIRE_LIFE);
		}
		else
		{
			maxLife = (CHILLIPEPPER_FIRE_LIFE-(CHILLIPEPPER_FIRE_LIFE*-0.5f));
			colStep = (256.0f/CHILLIPEPPER_FIRE_LIFE);
			maxVel = 1.0f - (0.1f*(1.0f/CHILLIPEPPER_FIRE_LIFE));
			maxSize = METERTOUNIT(1.0f)*(1.0f/CHILLIPEPPER_FIRE_LIFE);
		}

		if((data->life -= fTime)  > 0.0f)
		{
			// CMD: update position;
			bmVectorCMul(posInc, data->velocity, fTime);
			bmVectorAdd(data->position, data->position, posInc);

			if(data->life < maxLife)
			{
				//update scale
				data->size += maxSize*fTime;
				//update colour
				if((data->flags&DEBRISFLAG_INCEDENTALFIRE)||(data->flags&DEBRISFLAG_DOCOLLISION))
				{
					switch(map.sceneNumber)
					{
					case SCENE_TAZPREBOSS:
						rate = 127.0f;
						break;
					default:
						rate = 225.0f;
						break;
					}
					if((data->colour[3] -= fTime*rate) < 0.0f)
					{
						data->colour[3] = 0.0f;
						baDeleteExtraInstance(eSystem, j);
						noofInstances--;
						continue;
					}
				}
				else
				{
					for(i=0;i<3;i++)
					{
						if(data->colour[i] < 127.0f)
						{
							if((i != 0)&&(data->colour[0] > 127.0f)) continue;
							if((i != 1)&&(data->colour[1] > 127.0f)) continue;
							if((i != 2)&&(data->colour[2] > 127.0f)) continue;
						}
						if((data->colour[i] -= (float)(i+1)*colStep*fTime ) < 40.0f)
						{
							switch(i)
							{
							case 0:
								data->colour[i] = 20.0f;
								break;
							case 1:
								data->colour[i] = 20.0f;
								break;
							case 2:
								data->colour[i] = 20.0f;
								break;
							}
						}
					}
				}
				data->rotation += UtilGenerateRandomNumber(RAD(90.0f), RAD(-90.0f))*fTime;
	
				//update velocity
				bmVectorCMul(data->velocity, data->velocity, maxVel);
			}
		}
		else
		{
			baDeleteExtraInstance(eSystem, j);
			noofInstances--;
			continue;
		}

		// SO: Fade them out
		if( data->life < CHILLIPEPPER_FADE_LIFE )
		{
			data->colour[3] = ((data->life/CHILLIPEPPER_FADE_LIFE) * CHILLIPEPPER_MAX_ALPHA);
		}

		if(~data->flags & DEBRISFLAG_DONTDOCOLLISION)
		{
			if(data->flags&DEBRISFLAG_INCEDENTALFIRE)
			{
				UpdateChilliPepperTazCollision(data);
			}
			else
			{
				UpdateChilliPepperFireCollision(data);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int ChilliPepperExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/

void ChilliPepperExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}

/* --------------------------------------------------------------------------------
   Function : ChilliPepperExtraRenderCallback
   Purpose : This function draws the chilli pepper powerup fire
   Parameters : 
   Returns : 
   Info : 
*/

void ChilliPepperExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	CHILLIPEPPEREXTRAS *data;
	int			i, bufIndex;
	
	if((int32)context != EXTRA_ALPHA) return;

	bufIndex = 0;
	for (i=0;i<noofInstances;i++)
	{
		data = (CHILLIPEPPEREXTRAS*)dataPtrs[i];
		data->position[3] = 1.0f;


		if(bdDrawFlatSprite(data->position,data->size,data->size,data->rotation,data->texture,0,0,bmFloatToInt(data->colour[0]),
						bmFloatToInt(data->colour[1]), bmFloatToInt(data->colour[2]), bmFloatToInt(data->colour[3])<<1, &global2DVertexBuffer[bufIndex],0))
		{
			bufIndex += 4;
		}
	}

	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,FALSE,FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();
	bdSetTexture(0, data->texture);
	if(bufIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, bufIndex,BVERTTYPE_SINGLE2D);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZWRITE,TRUE,FALSE);
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperFireCollision(CHILLIPEPPEREXTRAS *data)
{
#ifndef CONSUMERDEMO
	if( map.sceneNumber == SCENE_TAZPREBOSS )
	{
		// SO: Here is a fine example of some nasty hard-coding ;)
		tazBoss1Game.doChilliPepperFireCollision(data);
		return;
	}
#endif

	ACTORINSTANCE		*ptr, *temp;
	TBVector			tempVector;
	TRIGGERINFO			*triggerInfo;
	char				string[256];
	char				*tempChar;
	int					i;

	// TP: These functions do the same thing, pretty much anyway
	SmashStuff(&map.levelInstances,gameStatus.player1,CHILLIPEPPER_FIRE_RANGE,100,data->position,SMASHFLAG_CHILLI|SMASHFLAG_RANGEDATTACK);
	SmashStuff(&map.animatedInstances,gameStatus.player1,CHILLIPEPPER_FIRE_RANGE,100,data->position,SMASHFLAG_CHILLI|SMASHFLAG_RANGEDATTACK);
	SmashStuff(&map.invisibleInstances,gameStatus.player1,CHILLIPEPPER_FIRE_RANGE,100,data->position,SMASHFLAG_CHILLI|SMASHFLAG_RANGEDATTACK);

	//now search for characters
	for(ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		bmVectorSub(tempVector, data->position, ptr->actorInstance.position);

		if(bmVectorLen(tempVector) < CHILLIPEPPER_FIRE_RANGE)
		{
			AICharacterSpun(ptr, gameStatus.player[data->playerNo].actorInstance, ATTACKTYPE_SPINREAR);
		}
	}
}

static void myStateChange(ACTORINSTANCE *actorInstance)
{
	TBVector				yAxis = {0.0f, 1.0f, 0.0f, 1.0f};

	FlushAnimationQueue(actorInstance);
	PlayAnimationByName(actorInstance, "waterbounce", 1.0f, 1, 0, 0.1f, NONE);
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_NOMOVE;
	actorInstance->actorBehaviour->physics.velocity[1] = 0.0f;
	AddActorSpeed(actorInstance->actorBehaviour, yAxis, METERTOUNIT(10.0f));

	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	actorInstance->actorAnimation->useMotionBone = FALSE;
	ReduceCharactersHealth(actorInstance, 1.0f);
}

/* --------------------------------------------------------------------------------
   Function : 
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateChilliPepperTazCollision(CHILLIPEPPEREXTRAS *data)
{
	TBVector	tazVec;
	int			i;

	// CMD: check against taz and she devil
	for(i = 0;i < gameStatus.multiplayer.numPlayers;i++)
	{
		bmVectorSub(tazVec, data->position, gameStatus.player[i].actorInstance->actorInstance.position);

		if(bmVectorLen(tazVec) < CHILLIPEPPER_FIRE_RANGE*0.5f)
		{
			switch(gameStatus.player[i].actorInstance->actorStatus->currentState)
			{
#ifndef CONSUMERDEMO
			case STATE_ONMINECART:
				if(gameStatus.player[i].actorInstance->actorStatus->actorAttachedTo)
				{
					if(gameStatus.player[i].actorInstance->actorStatus->actorAttachedTo->special == SPECIALTYPE_MINECART)
					{
						if(~gameStatus.player[i].actorInstance->actorStatus->actorAttachedTo->mineCartInfo->flags & MINECART_ONFIRE)
						{
							StartMineCartFire(gameStatus.player[i].actorInstance->actorStatus->actorAttachedTo);
						}
					}
				}
				break;
#endif
			case STATE_VEHICLE:
				continue;
			default:
				RequestStateChangePlus(gameStatus.player[0].actorInstance,STATE_BURNT,myStateChange);
				break;
			}
		}
	}
}