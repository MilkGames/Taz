// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Fireball.cpp
//   Purpose : Particle emitters used for explosion effects, eg. by WestBossExp
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"			// PP: global stuff
#include "Fireball.h"		// PP: Particle emitters used for explosion effects, eg. by WestBossExp
#include "sfx.h"			// PP: sound effect rountines
#include "quakecam.h"		// PP: provides functions to shake camera
#include "projectile.h"		// PP: functions to calculate projectile path of object
#include "display.h"		// PP: provide basic display setup and managment functions
#include "util.h"			// PP: general utility functions


// PP: maximum number of fireballs that can co-exist at any time
#define MAX_FIREBALLS		8	// PP: the most I could get to co-exist in the West Boss was 6, but there may have been gaps
								// PP: the most particles I could get to co-exist in any fireball in the West Boss was 74

// PP: looping array of fireballs
static LOOPARRAY<FIREBALL, MAX_FIREBALLS>	fireballs;

// PP: must initialise the static fireball particle texture ptr
TBTexture*				FBPARTICLE::texture=NULL;

// PP: the active fireballs' indices sorted into desired reverse drawing order
static int32			Fireball_drawOrder[MAX_FIREBALLS];

// PP: should we sort fireballs into a good draw order before drawing them?
bool					Fireball_applyDrawOrder=FB_NORMALLY_SORT_DRAW_ORDER;

// PP: ~~~~~~~~~~~~~~~~~~~

#ifdef FB_FAIRY_EXPLOSIONS

// PP: fireball creation info for each of the different types of fairy explosion
static FIREBALLINFO		Fireball_fairyInfo[FB_NUM_FAIRIES];

#endif// PP: def FB_FAIRY_EXPLOSIONS

// PP: ~~~~~~~~~~~~~~~~~~

// PP: TEMP?
#define FB_POINTLIGHT_RANGE		500.0f



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FBPARTICLE methods

										The particles emitted by / forming a fireball
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FBPARTICLE::init
	Purpose 	: initialisation
	Parameters 	: ptr to fireball that owns the particle, position, velocity, life
	Returns 	: 
	Info 		: 
*/
#define FBP_DURATION_HOT		0.45f// PP: (0.35f)
#define FBP_DURATION_COLD		0.2f// PP: (0.15f)
#define FBP_DURATION_SMOKE		0.75f// PP: 5.0f// PP: (0.5f)

#define FBP_LIFETIME			(FBP_DURATION_HOT+FBP_DURATION_COLD+FBP_DURATION_SMOKE)// PP: 2.0f

#define FBP_MAX_HEADSTART		(0.4f*(FBP_DURATION_HOT/FBP_LIFETIME))// PP: (0.075f*(FBP_DURATION_HOT/FBP_LIFETIME))// PP: 0.125f

#define FBP_MAX_ROT_SPEED		0.8f// PP: 2.5f

#define FBP_NORMAL_SIZE			170.0f
#define FBP_FINAL_SIZE			1000.0f

#define FBP_FADEIN_TIME			0.1f

void FBPARTICLE::init(FIREBALL* const fireballIn, TBVector posIn, TBVector velIn, const float lifeIn)
{
	this->fireball=fireballIn;

	bmVectorCopy(this->pos, posIn);

	bmVectorCopy(this->velocity, velIn);

	this->ang=bmRandf()*TWOPI;

	this->rotSpeed=(bmRand()&1)?FBP_MAX_ROT_SPEED:(-FBP_MAX_ROT_SPEED);

	this->size=FBP_NORMAL_SIZE;

	this->opacity=0.0f;

	this->life=lifeIn;

	this->timer=(1.0f-this->life)*FBP_LIFETIME;

	this->state=FBPSTATE_EXPLODING;
}


/*	--------------------------------------------------------------------------------
	Function 	: FBPARTICLE::update
	Purpose 	: update
	Parameters 	: ref to loop-array of particles to which this belongs
	Returns 	: 
	Info 		: 
*/

#define FBP_RISE_SPEED			METERTOUNIT(1.0f)
								

void FBPARTICLE::update(FB_ParticleArray& particles)
{
	// PP: haha! feel your puny mortality earthling
	this->life-=(fTime/FBP_LIFETIME);
	
	if(this->life < 0.0f)
	{
		this->state=FBPSTATE_FINISHED;
		return;
	}

	// PP: TEMP TEMP TEST
//	SETVECTOR(this->velocity, 0,0,0,0);

	// PP: apply rotation
	this->ang=Aabs(this->ang+(this->rotSpeed*this->life*fTime));

	// PP: update size
	this->size=FBP_NORMAL_SIZE+((1.0f-this->life)*(FBP_FINAL_SIZE-FBP_NORMAL_SIZE));

	bmVectorCMul(this->velocity, this->velocity, MIN(1.0f, this->life));

	// PP: apply real velocity and rise
	TBVector	temp;

	bmVectorCopy(temp, this->velocity);

	//temp[Y]+=(this->rise=FBP_RISE_SPEED*this->life);// PP: test

	// PP: ...and local wind...

	TBVector	windVec;

	bmVectorCMul(windVec, this->fireball->localWindVelocity, this->life);

	bmVectorAdd(temp, temp, windVec);

	bmVectorCMul(temp, temp, fTime);

	bmVectorAdd(this->pos, this->pos, temp);

	// PP: NEW TEST - skip a wad of update if it's just smoke
	if(this->timer < (FBP_DURATION_HOT+FBP_DURATION_COLD))
	{
		// PP: TEST: update opacity for quick fade-in
	//	this->opacity=((1.0f-this->life)*FBP_LIFETIME)/FBP_FADEIN_TIME;
	//	this->opacity=CONSTRAIN(this->opacity, 0.0f, 1.0f);


		// PP: ~~~~~~~~~~


		TBVector	distVec;
		float		dist;	

	//	float		minDist=FBP_INITIAL_SIZE*0.25f;//FBP_INITIAL_SIZE*0.4f;
		float		attractionRange=METERTOUNIT(1.5f);// PP: ?
		float		optDist=this->size*2.25f;// PP: *3.0f
		float		attractionSpeed=METERTOUNIT(1.25f)/**MIN(1.0f, this->life)*/;//METERTOUNIT(0.7f);//METERTOUNIT(0.01f);// PP: ?
		float		neighbourage=0.0f;
		TBVector	tempVec;
		TBVector	cohesionVelocity;

		// PP: BALLS ARE ATTRACTED TO NEIGHBOURS ACCORDING TO THEIR PROXIMITY...

		int			numBalls=0;

		bmVectorZero(cohesionVelocity);

		// PP: update particles, from the oldest to the youngest...

		LA_FOREACH(particles, FBPARTICLE, particle)

			if(particle==this) goto nextBall;

			// PP: get distance to particle...

			bmVectorSub(distVec, particle->pos, this->pos);

			dist=bmVectorLen(distVec);

			// PP: TEST: 
			if(dist>attractionRange) goto nextBall;

			if(dist != 0.0f)
			{
				bmVectorScaleToLength(tempVec, distVec, ((dist-optDist)/optDist)*attractionSpeed*MAX(0.0f, MIN(this->life, particle->life)*0.3f));// PP: life: test
				bmVectorAdd(cohesionVelocity, cohesionVelocity, tempVec);
			}

			numBalls++;

	nextBall:

		LA_ENDFOREACH(particles)

		// PP: TEMP TEST
	//	cohesionVelocity[Y]*=this->life;

		// PP: apply velocity
	//	TBVector	temp;

		bmVectorCDiv(temp, cohesionVelocity, fps);
		bmVectorAdd(this->pos, this->pos, temp);
	}
//*/
	// PP: calculate colour...

//	float age=(1.0f-this->life)*FBP_LIFETIME;
	float duration;
	float startTime=0.0f;
	
	if(this->timer > (duration=FBP_DURATION_HOT))
	{
		// PP: finished hot-cold stage
		startTime+=duration;
		if(this->timer-startTime > (duration=FBP_DURATION_COLD))
		{
			// PP: finished cold-smoke stage: now smoke-final stage
			startTime+=duration;
			duration=FBP_DURATION_SMOKE;
			this->colour=COLOUR(this->fireball->colours[FBSTAGECOLOUR_SMOKE], this->fireball->colours[FBSTAGECOLOUR_FINAL], (this->timer-startTime)/duration);
		}
		else
		{
			// PP: cold-smoke stage
			this->colour=COLOUR(this->fireball->colours[FBSTAGECOLOUR_COLD], this->fireball->colours[FBSTAGECOLOUR_SMOKE], (this->timer-startTime)/duration);
		}
	}
	else
	{
		// PP: hot-cold stage
		this->colour=COLOUR(this->fireball->colours[FBSTAGECOLOUR_HOT], this->fireball->colours[FBSTAGECOLOUR_COLD], this->timer/duration);
	}

//	this->colour.a = (unsigned char)(this->colour.a*this->opacity);

	// PP: update timer
	// PP: IMPORTANT: this timer value may be used even after the fireball has finished emitting (and is instead fading)
	this->timer+=fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: FBPARTICLE::draw
	Purpose 	: draw
	Parameters 	: ptr to start of vertex buffer to which to draw the particle, (in&out)ptr to current vert index (updated by this method)
	Returns 	: 
	Info 		: 
*/
void FBPARTICLE::draw(TBPrimVertex2D* const vertBuf, int32* const vertIndex)
{
	if(this->life > 1.0f) return;
	if(this->state == FBPSTATE_FINISHED) return;

	// PP: (in the style of a Top Gear presenter)  Ah that's more like it.  Sprites™.  So reliable, so economical, so very...Czechoslovakian.

	// PP: TEMP TEST: force W value of 1
	TBVector	spritePos;

	bmVectorCopy(spritePos, this->pos);

	spritePos[W]=1.0f;

	// PP: must set size because sprite drawing pays no attention to object matrix scale! ...

	float		spriteSize;

	spriteSize=this->size*this->fireball->scale[X];

/*	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_POINT, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_POINT, 1);

	//if(true)//((this->colour.r>128)||(this->colour.g>128)||(this->colour.b>128)||(this->colour.a>128))

	if(this->colour.a>128)
	{
		// PP: TEMP TEST
		COLOUR testColour;

			testColour=this->colour;//.inverse();
			//testColour.a=this->colour.a;

	#if((BPLATFORM == XBOX)||(BPLATFORM == PC))

			D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT); 
			D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);// PP: one
			D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);// PP: srcalpha

	#endif// PP: platform

		bdDrawFlatSprite(
			spritePos,
			spriteSize,
			spriteSize,
			this->ang,//test
			FBPARTICLE::texture,
			FALSE,
			FALSE,
			testColour.r,
			testColour.g,
			testColour.b,
			testColour.a,
			NULL,
			0.0f
		);

			testColour=this->colour;

			
	#if((BPLATFORM == XBOX)||(BPLATFORM == PC))

			D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
			D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);// PP: srcalpha

	#endif// PP: platform

		bdDrawFlatSprite(
			spritePos,
			spriteSize,
			spriteSize,
			this->ang,//test
			FBPARTICLE::texture,
			FALSE,
			FALSE,
			testColour.r,
			testColour.g,
			testColour.b,
			testColour.a,
			NULL,
			0.0f
		);
	}
	else*/
	{
//		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
//		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

		if(bdDrawFlatSprite(
			spritePos,
			spriteSize,
			spriteSize,
			this->ang,//test
			FBPARTICLE::texture,
			FALSE,
			FALSE,
			this->colour.r,
			this->colour.g,
			this->colour.b,
			this->colour.a,
			&vertBuf[*vertIndex],
			0.0f
		) != FAIL)
		{
			// PP: sprite has drawn; update vert index accordingly
			(*vertIndex)+=4;
		}
	}

}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FIREBALL methods

								An emitter of fiery particles, as used for explosion effects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FIREBALL::init
	Purpose 	: initialise the fireball
	Parameters	: 
	Returns		: 
	Info		: 
*/
void FIREBALL::init(void)
{
	this->timer=0.0f;
	this->distTraveled=0.0f;
	this->particles.init();

	bmVectorZero(this->pos);
	bmVectorZero(this->vel);
	this->range=0.0f;
	this->lifespan=0.0f;// PP: 0.0f = not used
	this->lifeTimer=0.0f;
	this->heat=FB_DEFAULT_HEAT;			// PP: hot as hell
	bmVectorZero(this->localWindVelocity);

	this->updateCallback=NULL;
	this->callbackContext=0;

#ifdef FB_LIGHTS
	// PP: initialise light
	baCreateLight(&this->lightSource, 0);
#endif// PP: def FB_LIGHTS
}


/*	--------------------------------------------------------------------------------
	Function 	: FIREBALL::update
	Purpose 	: update the fireball
	Parameters	: 
	Returns		: 
	Info		: 
*/
#define FB_EMISSION_INTERVAL			0.01f

int32 maxFBParticles=0;

void FIREBALL::update(void)
{
	int		b;			// PP: loop counter

	#ifdef PHIL
#ifdef _TAZDEBUG
#ifndef INFOGRAMES
	if(this->particles.size > maxFBParticles)
	{
		maxFBParticles=this->particles.size;
		bkPrintf("FIREBALL::update: %d particles\n", maxFBParticles);
	}
#endif// PP: infogrames
#endif// PP: debug
#endif// PP: def PHIL

	// PP: apply velocity
	if(this->state == FBSTATE_EMITTING)
	{
		TBVector	tempVec;

		bmVectorCDiv(tempVec, this->vel, fps);

		bmVectorAdd(this->pos, this->pos, tempVec);

		this->distTraveled+=bmVectorLen(tempVec);

		if(this->distTraveled > this->range)
		{
			// PP: reached max range - stop emitting
			this->state=FBSTATE_FADING;
		}
		else if((this->lifespan>0.0f) && (this->lifeTimer > this->lifespan))
		{
			this->state=FBSTATE_FADING;
		}

		if(this->state == FBSTATE_EMITTING)
		{		
			// PP: find out how many particles to create this frame

			int numBalls=int(this->timer/FB_EMISSION_INTERVAL);

			// PP: TEST - limit numballs
			//numBalls=MIN(numBalls, 1);

			for(b=0;b<numBalls;b++)
			{
				// PP: *** START A NEW BALL ***

				// PP: generate random life for new ball...

				float	life;

				life=1.0f-(((1.0f-this->heat)*(FBP_DURATION_HOT+FBP_DURATION_COLD))/FBP_LIFETIME)-(bmRandf()*FBP_MAX_HEADSTART);

				// PP: init new ball at appropriate index: younger particles added 'before' older ones...

				FBPARTICLE*	newBall;

				newBall=this->particles.getNew();

				TBVector	ballVel;
				TBVector	ballVelOffset;
				TBVector	ballPos;

				// PP: work out velocity for ball...

#define FB_BALL_FORWARD_SPEED			METERTOUNIT(5.0f)
#define FB_BALL_SIDEWARD_SPEED		METERTOUNIT(1.0f)//METERTOUNIT(5.0f)

				bmVectorScaleToLength(ballVel, this->vel, FB_BALL_FORWARD_SPEED);


				// PP: ball gets emitted at a random angle around branch's direction vector

				float	ballAng;

				ballAng=RANDOMANG;

				/* PP: REMOUT: THIS ASSUMED X/Z AXIS ALIGNMENT

				if(this->vel[X] == 0.0f)// PP: traveling along the Z axis
				{
					// PP: give ball velocity random X & Y offset

					SETVECTOR(ballVelOffset, FB_BALL_SIDEWARD_SPEED*bmSin(ballAng), FB_BALL_SIDEWARD_SPEED*bmCos(ballAng), 0.0f, 0.0f);
				}
				else// PP: traveling along the X axis
				{
					// PP: give ball velocity random Z & Y offset

					SETVECTOR(ballVelOffset, 0.0f, FB_BALL_SIDEWARD_SPEED*bmSin(ballAng), FB_BALL_SIDEWARD_SPEED*bmCos(ballAng), 0.0f);
				}

				*/

				// PP: CHEAP TEST
				SETVECTOR(ballVelOffset, bmRandfSigned()*METERTOUNIT(2), bmRandfSigned()*METERTOUNIT(2), bmRandfSigned()*METERTOUNIT(2), 0.0f);

				// PP: apply offset to ball velocity
				bmVectorAdd(ballVel, ballVel, ballVelOffset);

#define FBP_MAX_OFFSET		100.0f//500.0f

				TBVector	tempVec;

				// PP: ****** APPLY INVERSE SCALE TO PARTICLE POS ******
				bmVectorDiv(tempVec, this->pos, this->scale);

				SETVECTOR(ballPos, tempVec[X]+(bmRandfSigned()*FBP_MAX_OFFSET),
									tempVec[Y]+(bmRandfSigned()*FBP_MAX_OFFSET),
									tempVec[Z]+(bmRandfSigned()*FBP_MAX_OFFSET),
									1.0f);
				
				newBall->init(this, ballPos, ballVel, life);
				
				this->timer=0.0f;
			}
		}

		// PP: do any custom update...

		if(this->updateCallback != NULL)
		{
			this->updateCallback(this, this->callbackContext);
		}


	}// PP: end if emitting

	// PP: udpate particles, from the oldest to the youngest...

	bool	finished=true;

	this->boundingBox.init();

	LA_FOREACH(this->particles, FBPARTICLE, particle)

		particle->update(this->particles);

		if(particle->state == FBPSTATE_FINISHED)
		{
			this->particles.discardOldest();
		}
		else
		{
			this->boundingBox.expandToContain(particle->pos);
		
			finished=false;
		}

	LA_ENDFOREACH(this->particles)

	if(this->state == FBSTATE_FADING)
	{
		if(finished)
		{
			this->state=FBSTATE_FINISHED;
		}
	}

#ifdef FB_LIGHTS
	// PP: update the fireball's light source
	baSetPointLight(&this->lightSource, this->pos, FB_POINTLIGHT_RANGE);
#endif// PP: def FB_LIGHTS

	this->timer+=fTime;
	this->lifeTimer+=fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: FIREBALL::draw
	Purpose 	: draw the fireball
	Parameters	: 
	Returns		: 
	Info		: 
*/
void FIREBALL::draw(void)
{
	static TBPrimVertex2D	verts[FB_MAX_PARTICLES*4];

	if(!this->boundingBox.visible(scaleMat))
	{
		// PP: effect is off-screen, don't draw it!
		return;
	}

	int32					vertIndex=0;

	// PP: apply the XYZ scales of the fireball here at draw time (this saves so much hassle! :)

	bdSetObjectMatrix(this->scaleMat);

	// 3D mode
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	// PP: draw particles, from the oldest to the youngest...

	LA_FOREACH(this->particles, FBPARTICLE, particle)

		particle->draw(verts, &vertIndex);

	LA_ENDFOREACH(this->particles)

	bdSetTexture(0, FBPARTICLE::texture);

	bdSetIdentityObjectMatrix();

	// set sprite projection mode
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

	if(vertIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, verts, vertIndex,BVERTTYPE_SINGLE2D);

	// PP: TEMP TEST - show bounding box
//	this->boundingBox.draw();

	// PP: TEMP TEST - show timer

//	util3DText(this->pos, "%5.2f", this->timer);

	// PP: TEMP TEST - show emitter pos
	/*
#ifndef INFOGRAMES
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, 0);
	utilDrawSphere(this->pos, METERTOUNIT(0.75f), GREEN);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
#endif
	*/
	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENERAL FIREBALL FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_init
	Purpose 	: one-time initialisation for the fireball system
	Parameters	: 
	Returns		: 
	Info		: gets a ptr to the flame texture
*/
#define FBPARTICLE_TEXTURE		"chilliflame.tga"//"ckeyflame16.bmp"//"extras\\ball.bmp"//"objects\\tempcloud.bmp"

void Fireball_init(void)
{
	FBPARTICLE::texture=bkLoadTexture(NULL,FBPARTICLE_TEXTURE,0);

#ifdef FB_FAIRY_EXPLOSIONS

	// PP: INIT FAIRY FIREBALL INFOS...

#define FBFAIRY_SCALE		0.19f	// PP: scale on each axis for fairy explosions
#define FBFAIRY_LIFESPAN	0.1f	// PP: lifespan for fairy explosion emitters, in seconds
#define FBFAIRY_HEAT		1.0f	// PP: heat value for fairy explosions (1.0f=normal=start at very beginning of 'hot' stage)

	// PP: Cosmo...

    Fireball_fairyInfo[FBFAIRY_COSMO].flags=FBCREATEFLAG_LIFESPAN;
    Fireball_fairyInfo[FBFAIRY_COSMO].heat=FBFAIRY_HEAT;
    Fireball_fairyInfo[FBFAIRY_COSMO].scale.set(FBFAIRY_SCALE, FBFAIRY_SCALE, FBFAIRY_SCALE);
    Fireball_fairyInfo[FBFAIRY_COSMO].lifespan=FBFAIRY_LIFESPAN;
    Fireball_fairyInfo[FBFAIRY_COSMO].colours[0]=COLOUR(70,120,255,180);
    Fireball_fairyInfo[FBFAIRY_COSMO].colours[1]=COLOUR(70,255,70,180);
    Fireball_fairyInfo[FBFAIRY_COSMO].colours[2]=COLOUR(180,70,70,0);
    Fireball_fairyInfo[FBFAIRY_COSMO].colours[3]=COLOUR(0,0,0,0);

	// PP: Wanda...

    Fireball_fairyInfo[FBFAIRY_WANDA].flags=FBCREATEFLAG_LIFESPAN;
    Fireball_fairyInfo[FBFAIRY_WANDA].heat=FBFAIRY_HEAT;
    Fireball_fairyInfo[FBFAIRY_WANDA].scale.set(FBFAIRY_SCALE, FBFAIRY_SCALE, FBFAIRY_SCALE);
    Fireball_fairyInfo[FBFAIRY_WANDA].lifespan=FBFAIRY_LIFESPAN;
    Fireball_fairyInfo[FBFAIRY_WANDA].colours[0]=COLOUR(255,70,255,180);
    Fireball_fairyInfo[FBFAIRY_WANDA].colours[1]=COLOUR(70,70,255,180);
    Fireball_fairyInfo[FBFAIRY_WANDA].colours[2]=COLOUR(70,180,70,0);
    Fireball_fairyInfo[FBFAIRY_WANDA].colours[3]=COLOUR(0,0,0,0);

#endif// PP: def FB_FAIRY_EXPLOSIONS
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_shutdown
   Purpose		: shutdown the fireball list
   Parameters	: 
   Returns		: 
   Info			: this is called on exiting a level
*/
void Fireball_shutdown(void)
{
	//LA_FOREACHDO(fireballs, shutdown());

	// PP: restart the filreball list
	fireballs.init();
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_update
   Purpose		: update all fireballs
   Parameters	: 
   Returns		: 
   Info			: 
*/

uint32	maxFireballs=0;

void Fireball_update(void)
{
#ifdef PHIL
#ifdef _TAZDEBUG
#ifndef INFOGRAMES

	uint32 temp=0;

	LA_FOREACH(fireballs, FIREBALL, fireball);

		if(fireball->state != FBSTATE_FINISHED)
		{
			temp++;
		}

	LA_ENDFOREACH(fireballs);

	if(temp > maxFireballs)
	{
		maxFireballs=temp;
		bkPrintf("Fireball_update: %d fireballs\n", temp);
	}
#endif// PP: infogrames
#endif// PP: debug
#endif// PP: def PHIL

	LA_FOREACH(fireballs, FIREBALL, fireball)

		fireball->update();

	/* PP: REMOUT: Oooooh no no no.  Unless all the elements have identical lifetimes, we can't do this safely

		if(fireball->state == FBSTATE_FINISHED)
		{
			fireballs.discardOldest();
		}*/

	LA_ENDFOREACH(fireballs)
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_drawOrderCompare
   Purpose		: compare two fireballs to determine which is to show in front
   Parameters	: indices of the two fireballs (within draw order array)
   Returns		: true if a is to show in front of b, else false
   Info			: compares both the depths and states of the two fireballs;
					ensures fire shows through smoke
*/
static bool Fireball_drawOrderCompare(const int32 a, const int32 b)
{
	static FIREBALL*	fireball1;
	static FIREBALL*	fireball2;

	fireball1=fireballs.getItem(Fireball_drawOrder[a]);
	fireball2=fireballs.getItem(Fireball_drawOrder[b]);

	if(fireball1->state == FBSTATE_EMITTING)
	{
		if(fireball2->state == FBSTATE_EMITTING)
		{
			// PP: both fire, sort on depth
			// PP: TODO: FIND OUT IF THIS IS REALLY NEEDED, ie. TRY TAKING IT OUT AND SEE IF THERE'S A NOTICEABLE DIFFERENCE
			return (bmVectorSquaredDistance(fireball1->pos, gameStatus.viewPoint)
					<bmVectorSquaredDistance(fireball2->pos, gameStatus.viewPoint));
		}
		else
		{
			// PP: fire shows through smoke
			return true;// PP: fireball 2 is 'farther' cos its smoke
		}
	}
	else
	{
		if(fireball2->state == FBSTATE_EMITTING)
		{
			// PP: fire shows through smoke
			return false;// PP: fireball 1 is 'farther' cos its smoke	
		}
		else
		{
			// PP: both smoke - sort younger, hotter, smoke to the front - works a treat
			return (fireball1->timer < fireball2->timer);
		}
	}
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_sortIntoDrawOrder
   Purpose		: sort the indices of the used fireballs into the desired draw order
   Parameters	: (opt/very start)start of array portion to sort,
					(opt/very end)end of array portion to sort
   Returns		: 
   Info			: quicksort
*/
static void Fireball_sortIntoDrawOrder(const int32 left=0, const int32 right=fireballs.size-1)
{
	int32 i, last;

	if(left >= right) return;// PP: feck off

	utilSwap(Fireball_drawOrder, left, (left+right)/2);
	last=left;

	for(i=left+1;i<=right;i++)
	{
		if(Fireball_drawOrderCompare(i,left))
		{
			utilSwap(Fireball_drawOrder, ++last, i);
		}
	}

	utilSwap(Fireball_drawOrder, left, last);

	Fireball_sortIntoDrawOrder(left, last-1);
	Fireball_sortIntoDrawOrder(last+1, right);
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_draw
   Purpose		: draw all fireballs
   Parameters	: 
   Returns		: 
   Info			: 
*/

// PP: apply a good draw order to the fireballs
#define Z_SORT_FIREBALLS

void Fireball_draw(void)
{
	if(fireballs.size == 0)
	{
		return;
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);// PP: FALSE
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 0);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

	if(Fireball_applyDrawOrder)
	{
		int					i;

		// PP: list the indices of all used fireballs
		for(i=fireballs.size-1;i>=0;i--)
		{
			Fireball_drawOrder[i]=i;
		}

		// PP: sort the used fireballs' indices into our desired draw order
		Fireball_sortIntoDrawOrder();

		// PP: draw the fireballs, in the order that we've applied to them
		for(i=fireballs.size-1; i>=0; i--)
		{
			fireballs.getItem(Fireball_drawOrder[i])->draw();
		}
	}
	else
	{
		LA_FOREACHDO(fireballs, draw());
	}
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: ref to FIREBALLINFO struct
   Returns		: 
   Info			: 
*/
void Fireball_create(const FIREBALLINFO& info)
{
	FIREBALL*	fireball;

	fireball=fireballs.getNew();

	fireball->init();
	
//	fireball->flags=info.flags;


	// PP: TEMP TEST
/*	if(fireballs.size > 18)
	{
		bkPrintf("Schtøpp!\n");
	}*/

	// PP: copy valid members into fireball...

	// PP: pos is always copied in
	bmVectorCopy(fireball->pos, info.pos);
	
	// PP: expiry modes...

	if(info.flags & FBCREATEFLAG_LIFESPAN)
	{
		fireball->lifespan=info.lifespan;
	}
	else if(info.flags & FBCREATEFLAG_RANGE)
	{
		fireball->range=info.range;
	}

	// PP: movement modes...

	if(info.flags & FBCREATEFLAG_TARGETPOS)
	{
		TBVector	distVec;
		float		tempSpeed;

		bmVectorSub(distVec, info.targetPos, fireball->pos);
		
		if(info.flags & FBCREATEFLAG_SPEED)
		{
			// PP: use speed member
			tempSpeed=info.speed;
		}
		else if(info.flags & FBCREATEFLAG_LIFESPAN)
		{
			// PP: calculate speed from lifespan and distance to target pos...
			tempSpeed=(bmVectorLen(distVec)/fireball->lifespan);
		}
		else
		{
			// PP: use default speed
			tempSpeed=FB_DEFAULT_SPEED;
		}

		if(!(info.flags & FBCREATEFLAG_LIFESPAN))// PP: range is irrelevant if a lifespan has been specified
		{
			if(!(info.flags & FBCREATEFLAG_RANGE))// PP: if no range has been specified...
			{
				// PP: ...use the range from pos to targetPos
				fireball->range=bmVectorLen(distVec);
			}
		}

		// PP: calculate velocity
		bmVectorScaleToLength(fireball->vel, distVec, tempSpeed);
	}
	else if(info.flags & FBCREATEFLAG_VEL)
	{
		bmVectorCopy(fireball->vel, info.vel);
	}
	else if(info.flags & FBCREATEFLAG_ANG2D)
	{
		float			tempSpeed;

		if(info.flags & FBCREATEFLAG_SPEED)
		{
			// PP: use speed member
			tempSpeed=info.speed;
		}
		else
		{
			// PP: use default speed
			tempSpeed=FB_DEFAULT_SPEED;
		}

		// PP: calculate velocity from ang2D & speed
		TBVector	tempVec;

		SETVECTOR(tempVec, 0.0f, 0.0f, tempSpeed, 0.0f);
		
		bmVectorRotateXY(fireball->vel, tempVec, info.ang2D[X], info.ang2D[Y]);
	}

	// PP: local wind...

	if(info.flags & FBCREATEFLAG_LOCALWINDFROMVEL)
	{
		// PP: use the specified portion of the fireball's initial velocity as the local wind velocity
		bmVectorCMul(fireball->localWindVelocity, fireball->vel, info.localWindVelocityPortion);
	}

	// PP: other...

/* PP: REMOUT: TODO: 

	if(info.flags & FBCREATEFLAG_PARTICLEVELOCITY)
	{
		bmVectorCopy(fireball->particleVelocity, info.particleVelocity);
	}

*/

	bmVectorCopy(fireball->scale, info.scale);
	bmMatScale(fireball->scaleMat, info.scale[X], info.scale[Y], info.scale[Z]);

	fireball->heat=info.heat;
	fireball->updateCallback=info.updateCallback;
	fireball->callbackContext=info.callbackContext;

	for(int i=0; i<FB_NUM_STAGE_COLOURS; i++)
	{
		fireball->colours[i]=info.colours[i];
	}

	// PP: TEMP?: force the opacities to values I know will work....
/* PP: REMOUT: NO NO NO NO NO NO NO NO NO!
	fireball->colours[FBSTAGECOLOUR_HOT].a=FBP_COLOUR_HOT.a;
	fireball->colours[FBSTAGECOLOUR_COLD].a=FBP_COLOUR_COLD.a;
	fireball->colours[FBSTAGECOLOUR_SMOKE].a=FBP_COLOUR_SMOKE.a;
	fireball->colours[FBSTAGECOLOUR_FINAL].a=FBP_COLOUR_FINAL.a;*/

#ifdef FB_LIGHTS
	// PP: update the fireball's light source
	baSetPointLight(&fireball->lightSource, fireball->pos, FB_POINTLIGHT_RANGE);
	// PP: and enable it
	baEnableLight(&fireball->lightSource, TRUE);
#endif// PP: def FB_LIGHTS

	// PP: start the fireball emitting
	fireball->state=FBSTATE_EMITTING;
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: position, velocity, range, (opt/default)scale, (opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback, (opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context, (all opt/default) 4 colours for explosion
   Returns		: 
   Info			: 
*/
void Fireball_create(TBVector pos, TBVector vel, const float range, const float scale, const float heat, const FB_UpdateCallback updateCallback, const long32 context, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol)
{
	FIREBALLINFO	info;

	info.flags=(FBCREATEFLAG_VEL|FBCREATEFLAG_RANGE);
	bmVectorCopy(info.pos, pos);
	bmVectorCopy(info.vel, vel);
	info.range=range;
	SETVECTOR(info.scale, scale, scale, scale, 0.0f);
	info.heat=heat;
	info.updateCallback=updateCallback;
	info.callbackContext=context;

	Fireball_create(info);
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: position, x angle, y angle, speed, range, (opt/FB_DEFAULT_SCALE)scale,
					(opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback,
					(opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context,
					(opt/default)portion of emitter velocity to use for local wind velocity,
					(all opt/default) 4 colours for explosion
   Returns		: 
   Info			: just saves a bit of typing eh?
*/
void Fireball_create(	TBVector				pos,
						const float				xAngle,
						const float				yAngle,
						const float				speed,
						const float				range,
						const float				scale,
						const float				heat,
						const FB_UpdateCallback updateCallback,
						const long32			context,
						const float				localWindVelocityPortion,
						const COLOUR&			hotCol,
						const COLOUR&			coldCol,
						const COLOUR&			smokeCol,
						const COLOUR&			finalCol)
{

	FIREBALLINFO	info;

	// PP: start position...

	bmVectorCopy(info.pos, pos);

	// PP: angle...

	info.ang2D[X]=xAngle;
	info.ang2D[Y]=yAngle;

	info.flags|=FBCREATEFLAG_ANG2D;

	// PP: speed...	

	info.speed=FB_DEFAULT_SPEED;

	info.flags|=FBCREATEFLAG_SPEED;

	// PP: range...

	info.range=range;

	info.flags|=FBCREATEFLAG_RANGE;

	// PP: scale...

	SETVECTOR(info.scale, scale, scale, scale, 1.0f);

	// PP: heat...

	info.heat=heat;

	// PP: callback...

	info.updateCallback=updateCallback;
	info.callbackContext=context;

	// PP: local wind...

	if(localWindVelocityPortion != 0.0f)
	{
		info.flags|=FBCREATEFLAG_LOCALWINDFROMVEL;

		info.localWindVelocityPortion=localWindVelocityPortion;
	}

	// PP: colours...

	info.colours[FBSTAGECOLOUR_HOT]=hotCol;
	info.colours[FBSTAGECOLOUR_COLD]=coldCol;
	info.colours[FBSTAGECOLOUR_SMOKE]=smokeCol;
	info.colours[FBSTAGECOLOUR_FINAL]=finalCol;

	// PP: GO!!!

	Fireball_create(info);
}


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: speed (unts/sec), start position, target position, (opt/FB_DEFAULT_SCALE)scale, (opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback, (opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context, (all opt/default) 4 colours for explosion
   Returns		: 
   Info			: param order not ideal - sorry - had to avoid conflict with another overload
*/
void Fireball_create(const float speed, TBVector pos, TBVector targetPos, const float scale, const float heat, const FB_UpdateCallback updateCallback, const long32 context, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol)
{
	FIREBALLINFO	info;

	info.flags=(FBCREATEFLAG_TARGETPOS|FBCREATEFLAG_SPEED);

	bmVectorCopy(info.pos, pos);
	bmVectorCopy(info.targetPos, targetPos);
	info.speed=speed;
	SETVECTOR(info.scale, scale, scale, scale, 0.0f);
	info.heat=heat;
	info.updateCallback=updateCallback;
	info.callbackContext=context;

	info.colours[FBSTAGECOLOUR_HOT]=hotCol;
	info.colours[FBSTAGECOLOUR_COLD]=coldCol;
	info.colours[FBSTAGECOLOUR_SMOKE]=smokeCol;
	info.colours[FBSTAGECOLOUR_FINAL]=finalCol;

	Fireball_create(info);
}


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_explosionSupportEffects
	Purpose 	: start effects to support an explosion - boom noise, camera shake, pad rumble
	Parameters 	: explosion position
	Returns 	: 
	Info 		: hmm not yet sure about this
*/
void Fireball_explosionSupportEffects(TBVector pos, uint32 flags)
{
	if(~flags & FIREBALLFLAG_NOPADRUMBLE)
	{
		// PP: MAKE A BIG BOOM NOISE
		PlaySample("boom.wav",255);
		PlaySample("boom.wav",255, pos, 0);// PP: TEST, probably gonna be shit
	}


	if(~flags & FIREBALLFLAG_NOSOUND)
	{
		// PP: NEW TEST: RUMBLE THE PAD***************

//		RequestRumbleEffect(&controller1, 255, 1.0f, 0.0f, 1.0f, 0.0f, WAVEFORM_NORMAL, 0.0f, 0.0f, 0);

		// PP: ***************************************
	}


	if(~flags & FIREBALLFLAG_NOCAMERASHAKE)
	{
		// PP: name OOD
#define BE_CAMSHAKE_STRENGTH		1.7f

		// PP: MANDITORY camera shake
		rumbleCam_explosion(&camera[0], pos, BE_CAMSHAKE_STRENGTH);
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
											READY-MADE EFFECTS THAT USE FIREBALLS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_normalExplosion
	Purpose 	: create a 'normal' explosion using a fireball
	Parameters 	: explosion position, (opt/default)explosion radius, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: TODO?: ceiling control.  NOTE: radius of effect can only ever ROUGHLY match radius specified
*/

#define FB_NORMALEXPLOSION_HEIGHT		METERTOUNIT(2.5f)
#define FB_NORMALEXPLOSION_SPEED		METERTOUNIT(3.5f)
#define FB_NORMALEXPLOSION_RADIUS		METERTOUNIT(2.0f)

void Fireball_normalExplosion(TBVector pos, const float radius, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol, uint32 flags)
{
/*	// PP: TEMP TEST!
	Fireball_flatExplosion(pos, radius);
	return;

	// PP: ~~~~~~~~~*/

	TBVector	tempVec;

	bmVectorCopy(tempVec, pos);
	tempVec[Y]+=FB_NORMALEXPLOSION_HEIGHT;

	Fireball_create(FB_NORMALEXPLOSION_SPEED,
					pos,
					tempVec,
					radius/FB_NORMALEXPLOSION_RADIUS,
					FB_DEFAULT_HEAT,
					FB_DEFAULT_UPDATE_CALLBACK,
					FB_DEFAULT_CALLBACK_CONTEXT,
					hotCol,
					coldCol,
					smokeCol,
					finalCol);

	// PP: start effects to support an explosion - boom noise, camera shake, pad rumble
	Fireball_explosionSupportEffects(tempVec, flags);
}



/*	--------------------------------------------------------------------------------
	Function 	: Fireball_normalExplosion
	Purpose 	: create a 'normal' explosion using a fireball
	Parameters 	: actorinstance upon which to base explosion, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: TODO?: ceiling control.  NOTE: radius of effect can only ever ROUGHLY match radius specified
*/
void Fireball_normalExplosion(ACTORINSTANCE* const actorInstance, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol, uint32 flags)
{
	Fireball_normalExplosion(actorInstance->actorInstance.position,
							actorInstance->actorInstance.actor->maxRadius * actorInstance->actorInstance.scale[X],
							hotCol, coldCol, smokeCol, finalCol, flags);
}


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_flatExplosion
	Purpose 	: create a flat explosion using fireball(s)
	Parameters 	: explosion position, (opt/default)explosion radius, (opt/default)explosion height, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: !!!effect to be finalised!!!
*/

#define FB_FLATEXPLOSION_SPEED				METERTOUNIT(8)
// PP: REMOUT: I use the actor instance's Y size now		#define FB_FLATEXPLOSION_SCALE				1.0f

void Fireball_flatExplosion(TBVector pos, const float radius, const float height, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol, uint32 flags)
{
	// PP: TEMP?: just fire out four branches in a horizontal cross...

	for(int i=0; i<4; i++)
	{
		Fireball_create(	pos,
							0.0f,
							i*HALFPI,
							FB_FLATEXPLOSION_SPEED,
							radius,
							height/FB_NORMALEXPLOSION_RADIUS,// PP: use the same scale conversion factor as for normal explosions
							FB_DEFAULT_HEAT,
							FB_DEFAULT_UPDATE_CALLBACK,
							FB_DEFAULT_CALLBACK_CONTEXT,
							FB_DEFAULT_LOCAL_WIND_VELOCITY_PORTION,
							hotCol, coldCol, smokeCol, finalCol);
	}

	// PP: start effects to support an explosion - boom noise, camera shake, pad rumble
	Fireball_explosionSupportEffects(pos, flags);
}


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_flatExplosion
	Purpose 	: create a flat explosion using fireball(s)
	Parameters 	: actorinstance upon which to base explosion, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: !!!effect to be finalised!!!
*/
void Fireball_flatExplosion(ACTORINSTANCE* const actorInstance, const COLOUR& hotCol, const COLOUR& coldCol, const COLOUR& smokeCol, const COLOUR& finalCol, uint32 flags)
{
	Fireball_flatExplosion(	actorInstance->actorInstance.position,
							actorInstance->actorInstance.actor->maxRadius * actorInstance->actorInstance.scale[X],
							actorInstance->actorInstance.actor->maxRadius * actorInstance->actorInstance.scale[Y],
							hotCol, coldCol, smokeCol, finalCol, flags);
}


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_cannonExplosion
	Purpose 	: create a cannon explosion using a fireball
	Parameters 	: cannon actorinstance from which to emit explosion
	Returns 	: 
	Info 		: 
*/

#define CANNON_FIREABLL_RANGE							METERTOUNIT(1.9f)
#define CANNON_FIREBALL_SCALE							0.5f
#define CANNON_FIREBALL_SPEED							METERTOUNIT(8)	
#define CANNON_FIREBALL_LOCAL_WIND_VELOCITY_PORTION		0.25f
#define CANNON_FIREBALL_HEAT							1.0f
#define CANNON_FIREBALL_OFFSET							METERTOUNIT(1.3f)
#define CANNON_FIREBALL_YANG_ADJUST						HALFPI

void Fireball_cannonExplosion(ACTORINSTANCE* const actorInstance)
{
	VEC				tempVec;
	float			xAngle, yAngle;
	VEC				pos;

	// PP: angle...

	if(actorInstance->special == SPECIALTYPE_PROJECTILE)
	{
		xAngle= -actorInstance->projectileInfo->angle;
	}
	else// PP: for debugging, use -45°
	{
		xAngle=RAD(-45);
	}

	yAngle= CANNON_FIREBALL_YANG_ADJUST-GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);

	// PP: offset start position...

	pos=VEC(0,0,CANNON_FIREBALL_OFFSET).rotate(xAngle, yAngle)+actorInstance->actorInstance.position;

	// PP: create...

	Fireball_create(pos.v,
					xAngle,
					yAngle,
					CANNON_FIREBALL_SPEED,
					CANNON_FIREABLL_RANGE,
					CANNON_FIREBALL_SCALE,
					CANNON_FIREBALL_HEAT,
					NULL,
					0,
					CANNON_FIREBALL_LOCAL_WIND_VELOCITY_PORTION);

	// PP: start effects to support an explosion - boom noise, camera shake, pad rumble
	Fireball_explosionSupportEffects(pos.v);
}


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_cementMixerExplosion
	Purpose 	: create a puff of dust for a cement-mixer (cannon type), using a fireball
	Parameters 	: cement-mixer actorinstance from which to emit puff
	Returns 	: 
	Info 		: 
*/

#define CEMENTMIXER_FIREBALL_RANGE							METERTOUNIT(1.0f)
#define CEMENTMIXER_FIREBALL_SCALE							0.5f
#define CEMENTMIXER_FIREBALL_SPEED							METERTOUNIT(8)	
#define CEMENTMIXER_FIREBALL_LOCAL_WIND_VELOCITY_PORTION	0.25f
#define CEMENTMIXER_FIREBALL_HEAT							1.0f
#define CEMENTMIXER_FIREBALL_OFFSET							METERTOUNIT(1)
#define CEMENTMIXER_FIREBALL_COLOUR_HOT						COLOUR(150, 140, 124, 35)
#define CEMENTMIXER_FIREBALL_COLOUR_COLD					CEMENTMIXER_FIREBALL_COLOUR_HOT
#define CEMENTMIXER_FIREBALL_COLOUR_SMOKE					CEMENTMIXER_FIREBALL_COLOUR_HOT
#define CEMENTMIXER_FIREBALL_COLOUR_FINAL					COLOUR(120, 120, 120, 0)

void Fireball_cementMixerExplosion(ACTORINSTANCE* const actorInstance)
{
	VEC				tempVec;
	float			xAngle, yAngle;
	VEC				pos;

	// PP: angle...

	if(actorInstance->special == SPECIALTYPE_PROJECTILE)
	{
		xAngle= -actorInstance->projectileInfo->angle-HALFPI;
	}
	else// PP: for debugging, use -45°
	{
		xAngle=RAD(-45);
	}

	yAngle= -GetAngleAroundYFromQuat(actorInstance->actorInstance.orientation);

	// PP: offset start position...

	pos=VEC(0,0,CEMENTMIXER_FIREBALL_OFFSET).rotate(xAngle, yAngle)+actorInstance->actorInstance.position;

	// PP: create...

	Fireball_create(pos.v,
					xAngle,
					yAngle,
					CEMENTMIXER_FIREBALL_SPEED,
					CEMENTMIXER_FIREBALL_RANGE,
					CEMENTMIXER_FIREBALL_SCALE,
					CEMENTMIXER_FIREBALL_HEAT,
					NULL,
					0,
					CEMENTMIXER_FIREBALL_LOCAL_WIND_VELOCITY_PORTION,
					CEMENTMIXER_FIREBALL_COLOUR_HOT,
					CEMENTMIXER_FIREBALL_COLOUR_COLD,
					CEMENTMIXER_FIREBALL_COLOUR_SMOKE,
					CEMENTMIXER_FIREBALL_COLOUR_FINAL);// PP: CYAN: TEMP TEST

	// PP: start effects to support an explosion - boom noise, camera shake, pad rumble
	Fireball_explosionSupportEffects(pos.v);
}


#ifdef FB_FAIRY_EXPLOSIONS

/*	--------------------------------------------------------------------------------
	Function 	: Fireball_fairyExplosion
	Purpose 	: create a fairy explosion
	Parameters 	: actorinstance at whose position which to emit the puff,
					type of fairy explosion (FBFAIRY_COSMO/FBFAIRY_WANDA)
	Returns 	: 
	Info 		: 
*/
void Fireball_fairyExplosion(const ACTORINSTANCE* const actorInstance, const FBFairy fairy)
{
	// PP: create the explosion at the position of the actorinstance
	Fireball_fairyInfo[fairy].pos=actorInstance->actorInstance.position;

	// PP: adjust Y pos if required...

static int FBFAIRY_YOFFSET		=80;// PP: temp, for tweety

	Fireball_fairyInfo[fairy].pos[Y]+=FBFAIRY_YOFFSET;

	// PP: Fireball_fairyInfo is set up in Fireball_init
	Fireball_create(Fireball_fairyInfo[fairy]);

	// PP: TODO?: play sample here.  I reckon the sound that usually gets used for magical transformations
	// PP:	is that of an olden-days camera flash (the gunpowder on the stand thing)
}

#endif// PP: def FB_FAIRY_EXPLOSIONS