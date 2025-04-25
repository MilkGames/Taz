// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : holysmoke.cpp
//   Purpose : functions associated with smoke
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"

#include "actors.h"
#include "holysmoke.h"
#include "maths.h"
#include "precipitation.h"
#include "chillipepper.h"
#include "textures.h"
#include "debris.h"
#include "LoopArray.h"						// PP: Looping array class - please use it!
#include "util.h"

float		cloudCounter = 0.0f;

/*	--------------------------------------------------------------------------------
	Function 	: CreateImpactClouds
	Purpose 	: creates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int CreateImpactClouds(TBVector position, TBVector windVelocity, TBVector velocity, float maxScale, float life, float maxSpeed, uint32 smokeType)
{
	TBVector		tempVelocity, tempWindVelocity;

	if(!windVelocity)
	{
		SETVECTOR(tempWindVelocity, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		bmVectorCopy(tempWindVelocity, windVelocity);
	}
	//add map specified velocity
	if(velocity)
	{
		bmVectorAdd(tempVelocity, tempWindVelocity, velocity);
	}
	else
	{
		bmVectorCopy(tempVelocity, tempWindVelocity);
	}

	switch(smokeType)
	{
	case SMOKE_SKIDSTOP:
		InitSkidSmoke(position, tempVelocity, maxScale, life, maxSpeed);
		return TRUE;
	case SMOKE_TAZSPIN:
		InitTazSpinSmoke(position, gameStatus.player[0].actorInstance->actorBehaviour->physics.velocity, maxScale, life, maxSpeed);
	case SMOKE_SPIN:
		InitSpinSmoke(position, tempVelocity, maxScale, life, maxSpeed);
		return TRUE;
	case SMOKE_BIGFALL:
		InitFallSmoke(position, tempVelocity, maxScale, life, maxSpeed);
		return TRUE;
	case SMOKE_EXPLOSION:
		InitExplosionSmoke(position, tempVelocity, maxScale, life, maxSpeed);
		return TRUE;
	case SMOKE_COWBOYFINGER:
		InitFingerSmoke(position, tempVelocity, maxScale, life, maxSpeed);
		return TRUE;
	}
	
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: InitSkidSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitSkidSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		velocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter;
	float			angle, xDim, yDim, fifty;
	TBActor			*actor = NULL;
	PRECIPITATION	*precipitation = NULL;

	//angle = RAD(360.0f/MAX_SMOKE_SPRITES);
	//number = (float)((bmRand()%11)/100.0f);
	if((yDim = xDim = (maxScale + METERTOUNIT((cloudCounter/40)))) >= (2.0f*maxScale))
	{
		yDim = xDim = 2.0f*maxScale;
	}

	if(textureSmokeCloud)
	{
		fifty = 50.0f;
		for(counter = 0; counter < MAX_SMOKE_SPRITES; counter++)
		{
			angle = RAD((float)(bmRand()%360));
			//set velocity to be a general radial drift
			velocity[0] = SLOW_SMOKE_SPEED * bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = SLOW_SMOKE_SPEED * bmSin(angle);

			//add wind velocity
			bmVectorAdd(velocity, velocity, windVelocity);

			baCreateExtra(smokeSystem,position,velocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_IMPACTCLOUD);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: InitSpinSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitTazSpinSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		velocity, extra, zero;
	TBVector		tempPosition, tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter, numSprites;
	float			angle, xDim, yDim, fifty;
	TBActor			*actor = NULL;
	PRECIPITATION	*precipitation = NULL;

	//angle = RAD(360.0f/MAX_SMOKE_SPRITES);
	//number = (float)((bmRand()%11)/100.0f);
	//if((yDim = xDim = (maxScale + METERTOUNIT((cloudCounter/40)))) >= maxScale)
	{
		yDim = xDim = maxScale;
	}

	if(textureSmokeCloud)
	{
		fifty = 50.0f;

		if(fps >= 58.0f) numSprites = MAX_SMOKE_SPRITES;
		else
			if(fps >= 28.0f) numSprites = 2*MAX_SMOKE_SPRITES;
			else 
				if(fps >= 18.0f) numSprites = 3*MAX_SMOKE_SPRITES;
				else
					if(fps >= 14.0f) numSprites = 4*MAX_SMOKE_SPRITES;
					else
						numSprites = 5*MAX_SMOKE_SPRITES;

		bmVectorCMul(extra, windVelocity, -fTime);
		bmVectorCDiv(extra, extra, (float)numSprites);
		bmVectorSet(zero, 0.0f, 0.0f, 0.0f, 1.0f);

		for(counter = 0; counter < numSprites; counter++)
		{
			angle = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
			//set velocity to be a general radial drift
			velocity[0] = UtilGenerateRandomNumber(SLOW_SMOKE_SPEED, 0.0f)*bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = UtilGenerateRandomNumber(SLOW_SMOKE_SPEED, 0.0f)*bmSin(angle);

			//alter sprite position a little
//			bmVectorCopy(tempPosition, position);
			bmVectorAdd(tempPosition, velocity, position);
			bmVectorAdd(zero, zero, extra);
			bmVectorAdd(tempPosition, zero, tempPosition);

			baCreateExtra(smokeSystem,tempPosition,velocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_IMPACTCLOUD);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: InitSpinSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitSpinSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		velocity;
	TBVector		tempPosition, tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter, numSprites;
	float			angle, xDim, yDim, fifty;
	TBActor			*actor = NULL;
	PRECIPITATION	*precipitation = NULL;

	//angle = RAD(360.0f/MAX_SMOKE_SPRITES);
	//number = (float)((bmRand()%11)/100.0f);
	//if((yDim = xDim = (maxScale + METERTOUNIT((cloudCounter/40)))) >= maxScale)
	{
		yDim = xDim = maxScale;
	}

	if(textureSmokeCloud)
	{
		fifty = 50.0f;

		if(fps >= 58.0f) numSprites = MAX_SMOKE_SPRITES*3*2;
		else
			if(fps >= 28.0f) numSprites = 2*3*MAX_SMOKE_SPRITES;
			else numSprites = 3*3*MAX_SMOKE_SPRITES;

		for(counter = 0; counter < numSprites; counter++)
		{
			angle = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
			//set velocity to be a general radial drift
			velocity[0] = SLOW_SMOKE_SPEED * bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = SLOW_SMOKE_SPEED * bmSin(angle);

			//add wind velocity
			bmVectorAdd(velocity, velocity, windVelocity);

			//alter sprite position a little
			bmVectorCopy(tempPosition, position);
			bmVectorCDiv(tempVelocity, velocity, 7.0f);
			bmVectorAdd(tempPosition, velocity, tempPosition);

			baCreateExtra(smokeSystem,tempPosition,velocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_IMPACTCLOUD);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: InitFallSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitFallSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		velocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter;
	float			angle, xDim, yDim, number, fifty;
	TBActor			*actor = NULL;
	PRECIPITATION	*precipitation = NULL;

	angle = RAD(360.0f/MAX_BIG_FALL_SPRITES);
	number = (float)((bmRand()%11)/100.0f);
	yDim = xDim = maxScale + METERTOUNIT(number);		
	fifty = 50.0f;
	if(textureSmokeCloud)
	{
		for(counter = 0; counter < MAX_BIG_FALL_SPRITES; counter++)
		{
			if(speed)
			{
				if(speed > FAST_SMOKE_SPEED) speed = FAST_SMOKE_SPEED;
			}
			else
			{
				speed = FAST_SMOKE_SPEED;
			}

			angle = RAD((float)(bmRand()%360));
			//set velocity to be a general radial drift
			velocity[0] = speed * bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = speed * bmSin(angle);

			//add wind velocity
			bmVectorAdd(velocity, velocity, windVelocity);

//			baCreateExtra(chimneyPuffSystem,actor,textureSmokeCloud,precipitation,position,velocity,rotation,life,0.0f,xDim,yDim, 50,DEBRISFLAG_IMPACTCLOUD,0,0,0.0f);
//			CreateDebrisActor(texture, position, velocity, rotation, life, 1.0f, xDim, yDim, 50, DEBRISFLAG_IMPACTCLOUD);
			baCreateExtra(smokeSystem,position,velocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_IMPACTCLOUD);
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: InitExplosionSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitExplosionSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		velocity;
	TBVector		tempPosition, tempVelocity;
	TBVector		rotation = {0.0f, 0.0f, 0.0f, 1.0f};
	int				counter, numSprites;
	float			angle, xDim, yDim, fifty;

	{
		yDim = xDim = maxScale;
		fifty = 50.0f;
	}

	if(textureSmokeCloud)
	{
		if(fps >= 58.0f) numSprites = MAX_SMOKE_SPRITES;
		else
			if(fps >= 28.0f) numSprites = 2*MAX_SMOKE_SPRITES;
			else numSprites = 3*MAX_SMOKE_SPRITES;

		for(counter = 0; counter < numSprites; counter++)
		{
			angle = RAD((float)(bmRand()%360));
			//set velocity to be a general radial drift
			velocity[0] = SLOW_SMOKE_SPEED * bmCos(angle);
			velocity[1] = 0.0f;
			velocity[2] = SLOW_SMOKE_SPEED * bmSin(angle);

			//add wind velocity
			bmVectorAdd(velocity, velocity, windVelocity);

			//alter sprite position a little
			bmVectorCopy(tempPosition, position);
			bmVectorCDiv(tempVelocity, velocity, 10.0f);
			bmVectorAdd(tempPosition, velocity, tempPosition);
			bmVectorCMul(velocity,velocity,60.0f);

//			CreateDebrisActor(textureSmokeCloud, tempPosition, velocity, rotation, life, 0.0f, xDim, yDim, 50, DEBRISFLAG_EXPLOSIONSMOKE,0,0,0.0f);
			baCreateExtra(smokeSystem,tempPosition,velocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_EXPLOSIONSMOKE);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: InitFingerSmoke
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitFingerSmoke(TBVector position, TBVector windVelocity, float maxScale, float life, float speed)
{
	TBVector		cross, yAxis;
	TBVector		pert;
	int				counter;
	float			angle, xDim, yDim, fifty;

	{
		yDim = xDim = maxScale;
		fifty = 50.0f;
	}

	if(textureSmokeCloud)
	{
		bmVectorSet(yAxis, 0.0f, 1.0f, 0.0f, 1.0f);
		bmVectorCross(cross, yAxis, windVelocity);
	
		for(counter = 20;counter > 0;--counter)
		{
			// CMD: add some velocity to disperse the smoke from its trail
			bmVectorScaleToLength(pert, cross, METERTOUNIT(UtilGenerateRandomNumber(0.2f, -0.2f)));
			bmVectorAdd(windVelocity, windVelocity, pert);

			bmVectorSet(pert, 0.0f, METERTOUNIT(UtilGenerateRandomNumber(0.2f, -0.2f)), 0.0f, 1.0f);
			bmVectorAdd(windVelocity, windVelocity, pert);

			baCreateExtra(smokeSystem,position,windVelocity,textureSmokeCloud,&xDim,&life,&fifty,DEBRISFLAG_FINGERSMOKE);
			cloudCounter++;
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateImpactClouds
	Purpose 	: updates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateImpactClouds(SMOKEEXTRAINFO *data)
{
	float			number;

	//set random orientation for sprite
	//data->scale = RAD((float)(bmRand()%360));
	if(data->rotAngle)
	{
		data->rotAngle = (float)(bmRand()%360);
	}

	if(data->flags & DEBRISFLAG_CHILLISMOKE)
	{
		number = ((float)(bmRand()%10)/10.0f) + 0.5f;
	}
	else
	{
		number = ((float)(bmRand()%10)/10.0f) + 1.0f;
	}
	if(data->life < number)
	{
		//update scale
		if(data->flags & DEBRISFLAG_CHILLISMOKE)
		{
			data->size -= 3.0f*CHILLIPEPPER_SMOKE_SIZE*fTime;
		}
		else
		{
			data->size -= METERTOUNIT(0.6f)/fps;
		}
		if(data->size < 0.0f)
		{
			data->size = 0.0f;
			return TRUE;
		}
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateExplosionSmoke
	Purpose 	: updates clouds that appear when an explosion has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateExplosionSmoke(SMOKEEXTRAINFO *data)
{
	float			number;

	//set random orientation for sprite
	//data->scale = RAD((float)(bmRand()%360));
	if(data->rotAngle)
	{
		data->rotAngle = (float)(bmRand()%360);
	}

	number = ((float)(bmRand()%10)/10.0f) + 1.0f;
	if(data->life < number)
	{
		//update scale
		if((data->size += METERTOUNIT(1.0f)*fTime) < 0.0f)
		{
			data->size = 0.0f;
			return TRUE;
		}
		//update alpha
		if((data->alpha -= (127.0f*fTime)) < 0.0f)
		{
			data->alpha = 0.0f;
			return TRUE;
		}
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFingerSmoke
	Purpose 	: updates clouds that appear when taz does his cowboy shoot attack
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int32 UpdateFingerSmoke(SMOKEEXTRAINFO *data)
{
	TBVector		velInc;

	if((data->life -= fTime) > 0.0f)
	{
		// CMD: update velocity according to la resistence
		bmVectorScaleToLength(velInc, data->result, data->resistence*fTime);
		if(bmVectorLen(data->result) > bmVectorLen(velInc)) bmVectorSub(data->result, data->result, velInc);
		else bmVectorSet(data->result, 0.0f, 0.0f, 0.0f, 1.0f);

		// CMD: add a little upwards drift
		bmVectorSet(velInc, 0.0f, METERTOUNIT(UtilGenerateRandomNumber(0.1f, 0.0f)), 0.0f, 1.0f);
		bmVectorAdd(data->velocity, data->result, velInc);

		// CMD: update scale
		data->size += METERTOUNIT(0.1f)*fTime;

		// CMD: update alpha
		if((data->alpha -= 30.0f*fTime) < 0.0f) return TRUE;

		// CMD: update rotation
		if((data->clock -= fTime) < 0.0f)
		{
			data->clock = UtilGenerateRandomNumber(0.4f, 0.1f);
			data->rotAngle = RAD(UtilGenerateRandomNumber(360.0f, 0.0f));
		}
	}
	else return TRUE;
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: UpdateImpactClouds
	Purpose 	: updates clouds that appear when an impact has occured
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void DrawSmokeAtNodeInstance(ACTORINSTANCE *actorInstance, char *nodeName, uint32 nodeNameCRC, uint32 smokeType, float maxScale)
{
	TBActorNodeInstance		*node;
	TBVector				nodePosition;

	if(nodeName)	node = baFindNode(actorInstance->actorInstance.rootNodeInstance, nodeName);
	else			node = baFindNodeByCRC(actorInstance->actorInstance.rootNodeInstance, nodeNameCRC);

	if(node)
	{
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, nodePosition);

		CreateImpactClouds(nodePosition, NULL, NULL, maxScale, 0.5f, 0.0f, smokeType);
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FUSE SMOKE

				Cigarette-style continuous smoke trail, initially for dynamite fuses in the West Boss game.
									Could also be useful for the chain-smoking sub-game.
						It's a drifting 3D trail of camera-facing connected quads.
								Could be adapted for use as a smell trail, eg. for the cheese gun.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 

// PP: **** I'm blindly picking away at this while waiting for big builds.  Bloody slow compilers! ****


#define FUSESMOKE_MAX_SMOKES			5								// PP: maximum number of co-existing fuse smokes
																		
#define FUSESMOKE_MAX_SECTIONS			30								// PP: maximum number of sections in each fuse smoke trail

#define FUSESMOKE_NUM_VERTS				((FUSESMOKE_MAX_SECTIONS*2)+2)	// PP: number of verts in a fuse smoke trail

struct FUSESMOKESECTION
{
	VEC			pos;				// PP: leading position of section
	VEC			vel;				// PP: velocity of leading point of section


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKESECTION::init
		Purpose 	: initialise this fuse smoke section
		Parameters 	: leading position of section
		Returns 	: 
		Info 		: 
	*/
	void init(const VEC& posIn)
	{
		this->pos=posIn;
	}


}BALIGN16;

struct FUSESMOKE
{

private:

	const float*		trackPos;											// PP: emitter position to track
	float				timer;												// PP: general purpose timer


	LOOPARRAY<FUSESMOKESECTION, FUSESMOKE_MAX_SECTIONS>		sections;

	TBPrimVertex		verts[FUSESMOKE_MAX_SECTIONS*4];					// PP: array of verts to be drawn

public:

	bool				finished;


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::update
		Purpose 	: update this fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::draw
		Purpose 	: draw this fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::init
		Purpose 	: set-up the fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(const TBVector trackPosIn)
	{
		this->trackPos=trackPosIn;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::start
		Purpose 	: start the fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void start(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::endEmission
		Purpose 	: stop emitting this fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void stop(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FUSESMOKE::shutdown
		Purpose 	: completely shut-down this fuse smoke
		Parameters 	: 
		Returns 	: 
		Info 		: called by destructor
	*/
	void shutdown(void);


}BALIGN16;


static LOOPARRAY<FUSESMOKE, FUSESMOKE_MAX_SMOKES>		fuseSmokes;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FUSESMOKESECTION methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FUSESMOKESECTION::getVerts
	Purpose 	: get the verts to use to draw this fuse smoke section
	Parameters 	: ptr t
	Returns 	: 
	Info 		: 
*/
void init(const VEC posIn);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FUSESMOKE methods
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define FUSESMOKE_SECTION_INTERVAL		0.3f							// PP: time interval between emitting smoke sections


/*	--------------------------------------------------------------------------------
	Function 	: FUSESMOKE::update
	Purpose 	: update this fuse smoke
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void FUSESMOKE::update(void)
{
	if(this->timer > FUSESMOKE_SECTION_INTERVAL)
	{
		// PP: Add a new section...

		this->sections.getNew()->init(this->trackPos);

		this->timer=0.0f;
	}

	this->timer += fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: FUSESMOKE::update
	Purpose 	: draw this fuse smoke
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void FUSESMOKE::draw(void)
{
	static TBPrimVertex		verts[FUSESMOKE_NUM_VERTS];// PP: yeech, need to copy from the loop-array into this
	int						numVerts;

//	LA_FOREACHDO(this->sections, getVerts(verts, &numVerts));

	numVerts=0;

	if(numVerts) bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, numVerts, BVERTTYPE_SINGLE);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL FUSE SMOKE FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: InitFuseSmoke
	Purpose 	: start a fuse smoke emitter
	Parameters 	: position vector to track (must not move in memory)
	Returns 	: 
	Info 		: more params to follow soon
*/
void InitFuseSmoke(TBVector pos)
{
//	fuseSmokes.getNew()->init(pos);
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateFuseSmoke
	Purpose 	: update all fuse smoke
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void UpdateFuseSmoke(void)
{
	if(fuseSmokes.size == 0)
	{
		return;
	}

	LA_FOREACH(fuseSmokes, FUSESMOKE, smoke)

		smoke->update();

		if(smoke->finished)
		{
			fuseSmokes.discardOldest();
		}

	LA_ENDFOREACH(fuseSmokes)
}


/*	--------------------------------------------------------------------------------
	Function 	: DrawFuseSmoke
	Purpose 	: draw all fuse smoke
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void DrawFuseSmoke(void)
{
	if(fuseSmokes.size == 0)
	{
		return;
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	LA_FOREACHDO(fuseSmokes, draw());
}


// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 
// PP: WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** WORK IN PROGRESS *** 

// PP: _____________________________________________________________________________________________________________________________