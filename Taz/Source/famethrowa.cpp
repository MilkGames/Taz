// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : famethrowa.cpp
//   Purpose : functions that control flame throwing objects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "main.h"
#include "chillipepper.h"
#include "debris.h"
#include "textures.h"
#include "util.h"
#include "famethrowa.h"


/* --------------------------------------------------------------------------------
   Function : CreateFlameThrowa
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateFlameThrowa(ACTORINSTANCE *actorInstance, float length, float period)
{
	actorInstance->special = SPECIALTYPE_FLAMETHROWA;

	actorInstance->flamethrowaInfo = (FLAMETHROWAINFO*)ZALLOC(sizeof(FLAMETHROWAINFO));
	ASSERTM(actorInstance->flamethrowaInfo, "flamethrowa info could not be malloced");

	actorInstance->flamethrowaInfo->clock = 2.0f*period;
	actorInstance->flamethrowaInfo->period = period;
	actorInstance->flamethrowaInfo->length = length;

	AddActorToCollisionCache(&collisionCache, actorInstance, 0);
	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : UpdateFlameThrowa
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

void UpdateFlameThrowa(ACTORINSTANCE *actorInstance)
{
	TBVector		position, velocity, smallVel, delta, offset;
	TBMatrix		matrix;
	int				colour[3];
	float			size;

	if(actorInstance->special != SPECIALTYPE_FLAMETHROWA) return;

	if((actorInstance->flamethrowaInfo->clock -= fTime) < actorInstance->flamethrowaInfo->period)
	{
		if(actorInstance->flamethrowaInfo->clock < 0.0f)
		{
			actorInstance->flamethrowaInfo->clock = actorInstance->flamethrowaInfo->period*2.0f;
		}

		SETVECTOR(velocity, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
								UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
								(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);

		bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);
		bmMatMultiplyVector(matrix, velocity);
		bmVectorScaleToLength(velocity, velocity, 6.0f*actorInstance->flamethrowaInfo->length);

		bmVectorSet(delta, 0.0f, 0.0f, 0.0f, 1.0f);
		bmVectorSet(offset, 0.0f, 0.0f, -1.0f, 1.0f);
		bmMatMultiplyVector(matrix, offset);
		bmVectorScaleToLength(offset, offset, 6.0f*actorInstance->flamethrowaInfo->length*0.25f*fTime);

		// CMD: main blast of flame
		for(int i=0;i<4;i++)
		{
			//need small displacement to seperate the sprites early in their life time
			SETVECTOR(smallVel,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

			if((size = CHILLIPEPPER_FIRE_MAXSIZE*smallVel[2]/(int)CHILLIPEPPER_FIRE_RANDDISP) < CHILLIPEPPER_FIRE_MINSIZE)
			{
				size = CHILLIPEPPER_FIRE_MINSIZE;
			}

			bmMatMultiplyVector(matrix, smallVel);

			bmVectorAdd(position, smallVel, actorInstance->actorInstance.position);

			if(bmRand()%2)
			{
				colour[0]=255;colour[1]=38;colour[2]=10;
			}
			else
			{
				colour[0]=255;colour[1]=186;colour[2]=10;
			}
			//texture
			bmVectorAdd(position, position, delta);
			float	tempFloat = CHILLIPEPPER_FIRE_LIFE;
			float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
			baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],140,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_INCEDENTALFIRE);
			bmVectorAdd(delta, delta, offset);
		}
	}
	else
	{
		// CMD: minimised bubbling away flame
		SETVECTOR(velocity, UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP)), 
							UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP)))), 
							(-1.0f-UNITTOMETER((float)((bmRand()%(CHILLIPEPPER_FIRE_DISP*2))-CHILLIPEPPER_FIRE_DISP))), 1.0f);
		//need small displacement to seperate the sprites early in their life time
		SETVECTOR(smallVel,0.0f,0.0f,-((float)(bmRand()%CHILLIPEPPER_FIRE_RANDDISP)),1.0f);

		if((size = CHILLIPEPPER_FIRE_MAXSIZE*smallVel[2]/(int)CHILLIPEPPER_FIRE_RANDDISP) < CHILLIPEPPER_FIRE_MINSIZE)
		{
			size = CHILLIPEPPER_FIRE_MINSIZE;
		}
		bmQuatToMatrix(matrix, actorInstance->actorInstance.orientation);
		bmMatMultiplyVector(matrix, velocity);
		bmVectorScaleToLength(velocity, velocity, METERTOUNIT(1.0f));

		bmMatMultiplyVector(matrix, smallVel);

		bmVectorAdd(position, smallVel, actorInstance->actorInstance.position);


		if(bmRand()%2)
		{
			colour[0]=255;colour[1]=38;colour[2]=10;
		}
		else
		{
			colour[0]=255;colour[1]=186;colour[2]=10;
		}
		//texture
//		baCreateExtra(chilliFireSystem,NULL,textureChilliFire,NULL,position,velocity,rotation,CHILLIPEPPER_FIRE_LIFE,(float)(bmRand()%360),size,size, 140,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_INCEDENTALFIRE,0,0,0.0f,colour[0],colour[1],colour[2],actorInstance->playerNo);
		float	tempFloat = CHILLIPEPPER_FIRE_LIFE;
		float	rand = UtilGenerateRandomNumber(RAD(360.0f), 0.0f);
		baCreateExtra(chilliFireSystem,position,velocity,textureChilliFire,&tempFloat,&rand,&size,colour[0],colour[1],colour[2],140,DEBRISFLAG_CHILLIFIRE|DEBRISFLAG_INCEDENTALFIRE);
	}
}