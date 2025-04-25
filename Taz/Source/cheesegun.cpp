// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : cheesegun.cpp
//   Purpose : swat taz cheesegun code and drop elephant functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "main.h"
#include "actors.h"
#include "clouds.h"
#include "scene.h"
#include "maths.h"
#include "debris.h"
#include "characters.h"
#include "cheesegun.h"
#include "textures.h"
#include "genericai.h"
#include "status.h"
#include "sfx.h"

TBExtraSystem	*cheeseExtraSystem;

/*  --------------------------------------------------------------------------------
	Function	: CheeseHitCallback
	Purpose		: Gets called when the flying cheese hits something
	Parameters	: DEBRIS, COLLINFO from collision
	Returns		: FALSE to kill debris, TRUE to allow debris to continue
	Info		: 
*/
static int CheeseHitCallback(DEBRIS *debris, COLL_INFO *collInfo)
{
	// TP: check it is a collision with a normal mesh
	for (int i=0;i<MAX_COLL_MESHES;i++)
	{
		if ((collInfo->mesh[i].Index!=-1)&&(!(collInfo->mesh[i].flags&RETFLAG_NORESPONCE)))
		{
			// TP: if valid object to stick to then break
			break;
		}
		else
		{
			return TRUE;
		}
	}

	// TP: Create cheese
	new	STINKYCHEESE(debris->actorInstance->position,debris->actorInstance->orientation,debris->actorInstance->scale,10.0f);

	return FALSE;
}

/*  --------------------------------------------------------------------------------
	Function	: FireCheese
	Purpose		: Launch a piece of cheese
	Parameters	: ACTORINSTANCE throwing the cheese
	Returns		: true/false if the cheese was fired
	Info		: 
*/
int FireCheese(ACTORINSTANCE *actorInstance)
{
	TBVector				pos,tempVector,velocity;
	TBMatrix				tempMatrix;

	if (STINKYCHEESE::list.getSize()) return false;

	// TP: must find correct start point for cheese
	bmVectorCopy(pos,actorInstance->actorInstance.position);
	pos[Y] += METERTOUNIT(0.5f);

	bmQuatToMatrix(tempMatrix,actorInstance->actorInstance.orientation);
	SETVECTOR(tempVector,0.0f,0.0f,-1.0f,1.0f);
	bmMatMultiplyVector(tempMatrix,tempVector);
	bmVectorScaleToLength(velocity,tempVector,METERTOUNIT(20.0f));

	DEBRIS	*debris;

	PlaySample("cannonpoptaz.wav",255);

	if (gameStatus.player[actorInstance->playerNo].camera->flags & CAMERA_BLINKEYES)
	{
		debris = TazSpit(actorInstance,bkCRC32((uchar*)"cheeese.obe",strlen("cheeese.obe"),0),NULL,NULL,0.0f,METERTOUNIT(0.25f));
	}
	else
	{
		debris = TazSpit(actorInstance,bkCRC32((uchar*)"cheeese.obe",strlen("cheeese.obe"),0),pos,NULL,0.0f,METERTOUNIT(0.25f));
	}
	if (debris)
	{
		if (debris->actorStructure)
		{
			// TP: temp attrib to let us know that the debris came from a costume attack
			debris->actorStructure->actorStatus->costumeDebris = true;
		}
		debris->flags |= DEBRISFLAG_STICKY;
		debris->flags &=~DEBRISFLAG_AFFECTOBJECTS;
		SETVECTOR(debris->actorInstance->scale,1.0f,1.0f,1.0f,1.0f);
		EnableActorLineRenderMode(debris->actorInstance);
		debris->impactCallback = CheeseHitCallback;
	}

	return true;
}

/*  --------------------------------------------------------------------------------
	Function	: STINKYCHEESE::Init
	Purpose		: Create the cheese
	Parameters	: position, orientation, scale, life
	Returns		: 
	Info		: 
*/
void STINKYCHEESE::Init(TBVector position,TBQuaternion orientation,TBVector scale,float life)
{
	cheese = CreateActorInstance(&map.levelInstances,"cheeese.obe","stickydebris");
	if (cheese)
	{
		EnableActorCartoonRenderMode(&cheese->actorInstance);
		bmVectorCopy(cheese->actorInstance.position,position);
		bmVectorCopy(cheese->actorInstance.scale,scale);
		bmQuatCopy(cheese->actorInstance.orientation,orientation);
	}
	this->life = maxLife = life;
	lastSmoke = 0.0f;

	list.append(this);
}

/*  --------------------------------------------------------------------------------
	Function	: STINKYCHEESE::Update
	Purpose		: updates the cheese
	Parameters	: 
	Returns		: 
	Info		: 
*/
void STINKYCHEESE::Update(void)
{
	float		length, five;

	life -= fTime;
	if (life <= 0.0f) life = 0.0f;

	lastSmoke += fTime;

	// TP: create green smoke
	if (lastSmoke > 0.25f)
	{
		length = METERTOUNIT(0.05f);
		five = 5.0f;
		baCreateExtra(cheeseExtraSystem, cheese->actorInstance.position, &length, bkLoadTexture(NULL,"chsmoke.tga",0), &five);
		lastSmoke = 0.0f; 
	}

	// TP: start affecting enemies in range with stun moves, when I have stun moves that is....
	StunEnemies( ((maxLife-life)/maxLife)*METERTOUNIT(5.0f) );
}

/*  --------------------------------------------------------------------------------
	Function	: STINKYCHEESE::StunEnemies
	Purpose		: find all enemies within range and stun them
	Parameters	: range of stun
	Returns		: 
	Info		: 
*/
void STINKYCHEESE::StunEnemies(float range)
{
	ACTORINSTANCE *ptr;

	// TP: knock enemies over
	for (ptr = map.characterInstances.head.next;ptr != &map.characterInstances.head;ptr = ptr->next)
	{
		if (bmVectorDistance(ptr->actorInstance.position,cheese->actorInstance.position) < range)
		{
			AICharacterSpun(ptr,gameStatus.player1,ATTACKTYPE_COSTUMEREAR);
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: STINKYCHEESE::Draw
	Purpose		: draw the cheese
	Parameters	: 
	Returns		: 
	Info		: 
*/
void STINKYCHEESE::Draw(void)
{
}

LINKEDLIST<STINKYCHEESE>	STINKYCHEESE::list;

/*  --------------------------------------------------------------------------------
	Function	: KillStinkyCheese
	Purpose		: Removes all cheese from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void KillStinkyCheese(void)
{
	STINKYCHEESE::list.kill();
}

/*  --------------------------------------------------------------------------------
	Function	: UpdateStinkyCheese
	Purpose		: Removes all cheese from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void UpdateStinkyCheese(void)
{
	STINKYCHEESE	*cheese;

	for (int i=STINKYCHEESE::list.getSize()-1;i>=0;i--)
	{
		cheese = STINKYCHEESE::list.get(i);

		cheese->Update();

		if (cheese->life == 0.0f) 
		{
			STINKYCHEESE::list.remove(cheese);
			delete cheese;
		}
	}
}

/*  --------------------------------------------------------------------------------
	Function	: DrawStinkyCheese
	Purpose		: Removes all cheese from a level
	Parameters	: void
	Returns		: 
	Info		: 
*/
void DrawStinkyCheese(void)
{
	STINKYCHEESE	*cheese;

	for (int i=STINKYCHEESE::list.getSize()-1;i>=0;i--)
	{
		cheese = STINKYCHEESE::list.get(i);

		cheese->Draw();
	}
}

/* --------------------------------------------------------------------------------
   Function : InitCheeseExtraSystem
   Purpose : call this function to create the debris extra system
   Parameters : 
   Returns : 
   Info : 
*/
void InitCheeseExtraSystem(void)
{
	cheeseExtraSystem = baCreateExtraSystem("Cheese",CheeseExtraCreateCallback,
		CheeseExtraUpdateCallback,CheeseExtraDeleteCallback,CheeseExtraRenderCallback,
		CheeseExtraSystemInitCallback,sizeof(CHEESEEXTRAINFO),MAX_CHEESE,0,0);
}

/* --------------------------------------------------------------------------------
   Function : CheeseExtraCreateCallback
   Purpose : This function is called when a debris is created
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraCreateCallback(TBExtraSystem *eSystem, uchar *vData, va_list parms)
{
	CHEESEEXTRAINFO	*data = (CHEESEEXTRAINFO*)vData;
	float			*position;
	float			scale;
	TBTexture		*texture;
	float			life;

    if(parms)
	{
		position = va_arg(parms, float*);
		scale = *va_arg(parms, float*);
		texture = (TBTexture*)va_arg(parms, TBTexture*);
		life = *va_arg(parms, float*);

		bmVectorCopy(data->position, position);
		//data->position[1] += METERTOUNIT(0.3f);
		data->scale = scale;
		data->texture = texture;
		data->maxLife = data->life = life;
		data->angle = RAD((float)(bmRand()%360));
		data->radius = 0.0f;
		data->height = 0.0f;
		data->alpha = 0;
	}
}

/* --------------------------------------------------------------------------------
   Function : CheeseExtraDeleteCallback
   Purpose : This function is called when a debris is deleted
   Parameters : 
   Returns : 
   Info : 
*/
int CheeseExtraDeleteCallback(TBExtraSystem *eSystem, uchar *vdata)
{
	CHEESEEXTRAINFO *data = (CHEESEEXTRAINFO*)vdata;

	return TRUE;
}

/* --------------------------------------------------------------------------------
   Function : CheeseExtraUpdateCallback
   Purpose : This function is called each frame to update the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraUpdateCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, float t)
{
	CHEESEEXTRAINFO		*data;
	int				i;
	
	for (i=0;i<noofInstances;i++)
	{
		data = (CHEESEEXTRAINFO*)dataPtrs[i];

		if((data->life -= fTime) > 0.0f)
		{
			//update radius
			data->radius += CHEESE_RADIUSRATE*fTime;
			//update height
			data->height += CHEESE_RADIUSRATE*fTime;
			//update angle
			data->angle += CHEESE_ANGULARVEL*fTime;
			//update scale
			data->scale += CHEESE_SCALERATE*fTime;
			//update alpha
//			if (data->falpha > 0) data->falpha -= 128/fTime;
			// TP: calc alpha
			if ((data->maxLife-data->life) < 0.5f)
			{
				data->alpha = 20.0f*(data->maxLife-data->life)*2.0f;
			}
			else
			{
				if (data->alpha)
				{
					data->alpha = 20.0f*(data->life/data->maxLife);
				}
			}
		}
		else
		{
			baDeleteExtraInstance(eSystem, i);
			noofInstances--;
			continue;
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : CheeseExtraRenderCallback
   Purpose : This function is called each frame to render the debriss
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraRenderCallback(TBExtraSystem *eSystem, int noofInstances, uchar **dataPtrs, void *context)
{
	CHEESEEXTRAINFO	*data;
	TBVector		tempPos;
	int		i;

	if((int32)context != EXTRA_ALPHA) return;

	for (i=0;i<noofInstances;i++)
	{
		data = (CHEESEEXTRAINFO*)dataPtrs[i];
	
		//calculate current position
		tempPos[0] = data->position[0] + data->radius*bmSin(data->angle);
		tempPos[1] = data->position[1] + data->height;
		tempPos[2] = data->position[2] + data->radius*bmCos(data->angle);
		tempPos[3] = 1.0f;

		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, FALSE);
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		// PP: No z-write for these particles - they're translucent, so we don't want them blocking out anything behind them (or each other)
		SETZWRITE(FALSE);
		
		bdDrawFlatSprite(tempPos,data->scale, data->scale,0.0f,data->texture,0,0,255,255,255,bmFloatToInt(data->alpha)<<1,NULL,0.0f);

		bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, FALSE, FALSE);
	}
}

/* --------------------------------------------------------------------------------
   Function : CheeseExtraSystemInitCallback
   Purpose : This function is called when the system is intialised
   Parameters : 
   Returns : 
   Info : 
*/
void CheeseExtraSystemInitCallback(TBExtraSystem *eSystem, int reason)
{
}
