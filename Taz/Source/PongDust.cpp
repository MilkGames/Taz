// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PongDust.cpp
//   Purpose : Cartoon dust effect, for Elephant Pong
// Component : Taz
//      Info :	// PP: Dust particles are collectively outlined and collectively have a single level of transparency
//				// PP: (ie. they don't blend with each other).
//				// PP: PP: yes, I know we already have umpteen different smoke and dust effects...
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "LoopArray.h"			// PP: Looping array class - please use it!
#include "util.h"				// PP: general utility functions
#include "display.h"			// PP: provide basic display setup and managment functions
#include "PongDust.h"			// PP: Cartoon dust effect, for Elephant Pong

#include "VideoFX.h"// PP: // PP: TEMP TEST

// PP: define PONGDUST_LIMIT_TO_2_FRAMES to limit pong dust update to 30/25 fps.
// PP: This keeps it looking cartoony.
#define PONGDUST_LIMIT_TO_2_FRAMES

// PP: The frame length used by pong dust update
#ifdef PONGDUST_LIMIT_TO_2_FRAMES	
#define PONGDUST_FTIME				MAX(fTime, 0.03636f)// PP: that's roughly 1/27.5, 27.5 being roughly midway between PAL & NTSC '2 frames'.  So yes, the dust's life will be *slightly* shorter on NTSC - OH BOO HOO!!!!!
#else// PP: don't limit to 2 frames
#define PONGDUST_FTIME				(fTime)
#endif// PP: don't limit to 2 frames

// PP: Maximum number of pong dust particles in existence at any one time
#define PONGDUST_MAX_PARTICLES		400// PP: I'm sure there's plenty of slack in there; will take it down soon

// PP: Macimum number of pong dust emitters in existence at any one time
#define PONGDUST_MAX_EMITTERS		30

// PP: TIMING...

// PP: define PONGDUST_TIMERS to time pongdust stuff
#ifdef PHIL
#define PONGDUST_TIMERS
#endif// PP: def PHIL

#ifdef PONGDUST_TIMERS
#define PD_STARTTIMER		STARTTIMER
#define PD_ENDTIMER			ENDTIMER
#else// PP: ifndef PONGDUST_TIMERS
#define PD_STARTTIMER
#define PD_ENDTIMER	
#endif// PP: ndef PONGDUST_TIMERS

// PP: define TIME_PONGDUST_SORT to time the per-draw manual depth-sorting of pongdust particles
// PP: #define TIME_PONGDUST_SORT

// PP: the looping array of all pong dust particles
static LOOPARRAY<PONGDUST, PONGDUST_MAX_PARTICLES>			*PongDust_particles		=NULL;

// PP: the active particles' indices sorted into desired reverse drawing order
static int32*						PongDust_drawOrder=NULL;

// PP: the looping array of all pong dust emitters
static LOOPARRAY<PONGDUSTEMITTER, PONGDUST_MAX_EMITTERS>	*PongDust_emitters		=NULL;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													PONGDUST methods

							A particle of pong dust - each one is a little sprite dust cloud
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: twico
float	impScale=0.02f;
float	outImpScale=0.05f;

// PP: must initialise the static pong dust particle texture ptr
TBTexture*		PONGDUST::texture=NULL;

// PP: initialise static pong dust colour
COLOUR			PONGDUST::colour		=COLOUR(91, 57, 17, 80);
COLOUR			PONGDUST::outlineColour	=COLOUR(58,26,0,108);


// PP: pretty self-explanatory particle parameters...

#define PONGDUST_MAX_INITIAL_ROTSPEED		3.5f
#define PONGDUST_MIN_INITIAL_ROTSPEED		2.5f

#define PONGDUST_MAX_FINAL_ROTSPEED			0.2f
#define PONGDUST_MIN_FINAL_ROTSPEED			0.1f

#define PONGDUST_MAX_INITIAL_SIZE			240.0f// PP: 190.0f
#define PONGDUST_MIN_INITIAL_SIZE			220.0f// PP: 175.0f

#define PONGDUST_INITIAL_TIMER				2.5f


/*	--------------------------------------------------------------------------------
	Function 	: PONGDUST::init
	Purpose 	: initialise the pong dust particle
	Parameters 	: posizione, velocitah
	Returns 	: 
	Info 		: 
*/
void PONGDUST::init(const VEC& posIn, const VEC& velIn)
{
	this->pos=posIn;
	this->vel=velIn;
	this->ang=RANDOMANG;

	// PP: sizes...

	this->initialSize=PONGDUST_MIN_INITIAL_SIZE+(bmRandf()*(PONGDUST_MAX_INITIAL_SIZE-PONGDUST_MIN_INITIAL_SIZE));

	// PP: NOTE: 'initialSize' is a bit of a misleading name.  It's not the size that the particle starts at,
	// PP: it's the initial size of the particle after it has popped-in.
	// PP: The particle does, of course, pop-in from zero scale.
	this->size=0.0f;

	// PP: rotation speeds...

	float	rotDir;

	rotDir=(bmRand()&1)?1.0f:(-1.0f);

	this->initialRotSpeed=rotDir*(PONGDUST_MIN_INITIAL_ROTSPEED+(bmRandf()*(PONGDUST_MAX_INITIAL_ROTSPEED-PONGDUST_MIN_INITIAL_ROTSPEED)));
	this->finalRotSpeed=rotDir*(PONGDUST_MIN_FINAL_ROTSPEED+(bmRandf()*(PONGDUST_MAX_FINAL_ROTSPEED-PONGDUST_MIN_FINAL_ROTSPEED)));

	this->rotSpeed=this->initialRotSpeed;

	// PP: set timer ("...six minutes!"  "Six minutes - check!  ...  Close the door Alec, there's a draught!  ...  Alec?!"  "Come out with you hands above your head!"  "FINISH THE JOB JAMES, BLOW THEM ALL TO HELL!")
	this->timer=PONGDUST_INITIAL_TIMER;

	// PP: clear flags
	this->flags=0;
}


/*	--------------------------------------------------------------------------------
	Function 	: PONGDUST::update
	Purpose 	: update the pong dust particle
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PONGDUST::update(void)
{
	float	life;		// PP: fraction of life

	// PP: calculate remaining life fraction
	life=this->timer/PONGDUST_INITIAL_TIMER;

	// PP: apply velocity
	this->pos += (this->vel*PONGDUST_FTIME);

	// PP: update rotation speed
	this->rotSpeed=this->finalRotSpeed+(SQR(life)*(this->initialRotSpeed-this->finalRotSpeed));

	// PP: apply rotation speed...

	this->ang += (this->rotSpeed * PONGDUST_FTIME);

	// PP: tick down life timer
	this->timer -= PONGDUST_FTIME;

	if(this->timer <= 0.0f)
	{
		this->flags |= PONGDUSTFLAG_FINISHED;
	}

	// PP: update size...
	
	float	age;	// PP: age in seconds

	age=PONGDUST_INITIAL_TIMER-this->timer;

#define PONGDUST_POPIN_DURATION		0.5f// PP: 0.25f// PP: 0.15f
#define PONGDUST_POPOUT_DURATION	0.5f// PP: 0.35f

	if(age < PONGDUST_POPIN_DURATION)
	{
		this->size=this->initialSize*(age/PONGDUST_POPIN_DURATION);
	}
	else
	{
		if(this->timer < PONGDUST_POPOUT_DURATION)
		{
			this->size=this->initialSize*(this->timer/PONGDUST_POPOUT_DURATION);
		}
	}

	// PP: update imperfection scales...
/* PP: 
	this->imperfectionScale=1.0f+(bmRandfSigned()*impScale);
	this->outlineImperfectionScale=1.0f+(bmRandfSigned()*outImpScale);*/
}

// PP: TEMP for a-tweekin'
float ps2NormalZbias=(-285);
float ps2OutlineZbias=585;

	// PP: too low and you'll start to see sprite boundaries; too high and the outline will recede below the ground
#if(BPLATFORM == PS2)// PP: sodding ps2 - is there REALLY no way we can get a consistent z-bias scale between the platforms?!
#define PONGDUST_OUTLINE_ZBIAS		ps2OutlineZbias// PP: this will need tweaking - I have no idea what sort of value to use
#else// PP: not ps2
#define PONGDUST_OUTLINE_ZBIAS		10.5f
#endif// PP: not ps2


#if(BPLATFORM == PS2)// PP: sodding ps2 - is there REALLY no way we can get a consistent z-bias scale between the platforms?!
#define PONGDUST_NORMAL_ZBIAS		ps2NormalZbias// PP: this will need tweaking - I have no idea what sort of value to use
#else// PP: not ps2
#define PONGDUST_NORMAL_ZBIAS		(-2.0f)
#endif// PP: not ps2


/*	--------------------------------------------------------------------------------
	Function 	: PONGDUST::draw
	Purpose 	: draw the pong dust particle
	Parameters 	: drawing outline (2nd) pass?, ptr to start of vertex buffer to which to draw the particle,
					(in&out)ptr to current vert index (updated by this method)
	Returns 	: 
	Info 		: 
*/
void PONGDUST::draw(const bool outline, TBPrimVertex2D* const vertBuf, uint32* const vertIndex)
{
	float		spriteSize;
	COLOUR		drawCol;
	float		zBias=0.0f;

	if(outline)
	{
		// PP: The width of each particle's outline, as a fraction of the screen height.
		// PP: Perspective is counteracted so that this width is maintained at all distances.
#define PONGDUST_OUTLINE_WIDTH		0.002f	

		spriteSize=((this->size/**this->imperfectionScale*/)+((PONGDUST_OUTLINE_WIDTH*this->zDepth*CURRENT_SCREEN.tanFov)/**this->outlineImperfectionScale*/));
		
		// PP: outline fades to main colour during popout// PP: Not cartoony I know, but it has to be done
		if(this->timer < PONGDUST_POPOUT_DURATION)
		{
			float colMix=this->timer/PONGDUST_POPOUT_DURATION;
			colMix=MAX(0.0f, colMix);
			drawCol=COLOUR(PONGDUST::colour, PONGDUST::outlineColour, colMix);
		}
		else
		{
			drawCol=PONGDUST::outlineColour;
		}

#if(BPLATFORM == PS2)
		zBias=PONGDUST_NORMAL_ZBIAS+PONGDUST_OUTLINE_ZBIAS;
#endif// PP: ps2

	}
	else
	{
		spriteSize=this->size/**this->imperfectionScale*/;
		drawCol=PONGDUST::colour;

#if(BPLATFORM == PS2)
		zBias=PONGDUST_NORMAL_ZBIAS;
#endif// PP: ps2

	}

	if(bdDrawFlatSprite(
		this->pos,
		spriteSize,
		spriteSize,
		this->ang,
		PONGDUST::texture,
		FALSE,
		FALSE,
		drawCol.r,
		drawCol.g,
		drawCol.b,
		drawCol.a,
		&vertBuf[*vertIndex],
		zBias
	) != FAIL)
	{
		// PP: sprite has drawn; update vert index accordingly
		(*vertIndex)+=4;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENERAL PONG DUST FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: texture for pong dust particles	// PP: TODO?: animated gif for real cartoon authenticity?
#define PONGDUST_TEXTURE		"extras\\pongdust.bmp"// PP: "fovtest.bmp"

// PP: normalised camera direction vector (should really store this in gameStatus but I don't much fancy a half-hour build)
static VEC						PongDust_camDirNorm;


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_sceneInit
	Purpose 	: initialise the pong dust system at the beginning of a scene that's gonna use it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_sceneInit(void)
{
	// PP: new the particle array
	PongDust_particles=new LOOPARRAY<PONGDUST, PONGDUST_MAX_PARTICLES>;

	// PP: new the particle draw order array
	PongDust_drawOrder=new int32[PONGDUST_MAX_PARTICLES];

	// PP: get ptr to pong dust particle texture
	PONGDUST::texture=bkLoadTexture(NULL,PONGDUST_TEXTURE,0);
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_sceneShutdown
	Purpose 	: shut-down the pong dust system at the end of a scene that's been using it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_sceneShutdown(void)
{
	// PP: delete the particle array
	SAFE_DELETE(PongDust_particles);

	// PP: delete the particle draw order array
	SAFE_DELETE(PongDust_drawOrder);
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_getNumParticlesToCreate
	Purpose 	: determine the number of particles for PongDust_create to add
	Parameters 	: number of particles requested, creation frequency
	Returns 	: the number of particles for PongDust_create to add
	Info 		: 
*/
static uint32 PongDust_getNumParticlesToCreate(const uint32 numParticles, const float frequency)
{
	uint32		rtn;
	float		interval;
	float		intervalPerParticle;

	if(numParticles < 1)
	{
		ASSERTM(0, "Awa' biyl yer heed!");
	}
	
	if(frequency == PONGDUST_NO_FREQUENCY)
	{
		rtn=numParticles;
	}
	else
	{
		interval = 1/frequency;

		intervalPerParticle=(numParticles*interval);

		if(intervalPerParticle < PONGDUST_FTIME)
		{
			// PP: we can fit a few of these babies into this update

			rtn=bmFloatToInt(numParticles*(PONGDUST_FTIME/interval));

			ASSERTM(rtn, "OOPS\n");
		}
		else
		{
			// PP: we might get one in, let's see...

			// PP: NOTE: If intervalPerParticle were only slightly above PONGDUST_FTIME,
			// PP: you would tend to get bunches of 'on' frames and
			// PP: bunches of 'off' frames using the fmod method.
			// PP: So in these cases use a weighted random on/off...

			// PP: Might seem a bit overkill, but I don't want any clumpy shite in MY dust! ...

			if(intervalPerParticle > (PONGDUST_FTIME * 2.0f))
			{
				if(bmFMod(gameStatus.gameTime, intervalPerParticle) > (intervalPerParticle/2.0f))
				{
					rtn=1;
				}
				else
				{
					rtn=0;
				}
			}
			else
			{
				// PP: intervalPerParticle is between PONGDUST_FTIME and 2*PONGDUST_FTIME

				// PP: with intervalPerParticle nearer PONGDUST_FTIME, hitProbability tends towards 1
				// PP: with intervalPerParticle nearer 2*PONGDUST_FTIME, hitProbability tends towards 0.5f

				float hitProbability=1.0f-(0.5f*((intervalPerParticle-PONGDUST_FTIME)/PONGDUST_FTIME));

				rtn=((bmRandf()<hitProbability)?1:0);
			}
		}
	}

	return rtn;
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_create
	Purpose 	: kick up some pong dust
	Parameters 	: rough position (random deviation will be applied),
					(opt/default)number of particles,
					(opt/ZVEC)rough velocity vector(random deviation and drift will be applied),
					(opt/PONGDUST_NO_FREQUENCY)frequency of particle creation (eg. 10.0f = create 10 times a second)
	Returns 	: 
	Info 		: 
*/
void PongDust_create(const VEC& pos, const uchar numParticles, const VEC& vel, const float frequency)
{
	uint32		realNumParticles;

	// PP: determine number of particles to add
	if((realNumParticles=PongDust_getNumParticlesToCreate(numParticles, frequency)) < 1)
	{
		return;
	}

	// PP: without this check, longish frames would cause a crash, for obvious reasons
	realNumParticles=MIN(realNumParticles, PongDust_particles->totalArraySize);

	PongDust_particles->getNew(realNumParticles);

	// PP: maximum velocity deviation on each axis
#define PONGDUST_MAX_VEL_DEVIATION		100.0f// PP: 200.0f

	// PP: drift velocity
#define PONGDUST_VEL_ADJUST				VECM(0,0.6f,0)

	// PP: maximum position deviation on each axis
#define PONGDUST_MAX_POS_DEVIATION		30.0f

	for(int i=realNumParticles-1; i>=0; i--)// PP: I'm sure I heard Tommi G saying something down the pub about downward for's being faster(?)
	{
		PongDust_particles->getYoungest(i)->init(pos+RANDOMVEC(PONGDUST_MAX_POS_DEVIATION),
												vel+PONGDUST_VEL_ADJUST+RANDOMVEC(PONGDUST_MAX_VEL_DEVIATION));
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_create
	Purpose 	: kick up some pong dust at a node's position
	Parameters 	: ACTORINSTANCE*, name of node, (opt/default)number of particles,
					(opt/ZVEC)rough velocity vector(random deviation and drift will be applied),
					(opt/PONGDUST_NO_FREQUENCY)frequency of particle creation (eg. 10.0f = create 10 times a second)
	Returns 	: 
	Info 		: 
*/
void PongDust_create(ACTORINSTANCE* const actorInstance, char* const nodeName, const uchar numParticles, const VEC& vel, const float frequency)
{
	TBActorNodeInstance		*node;
	VEC						nodePos;
	uint32					realNumParticles;					

	// PP: determine number of particles to add
	if((realNumParticles=PongDust_getNumParticlesToCreate(numParticles, frequency)) < 1)
	{
		return;
	}

	// PP: find the node
	node = baFindNode(actorInstance->actorInstance.rootNodeInstance, nodeName);

	if(node)
	{
		// PP: get the node's position
		baGetNodesWorldPosition(&actorInstance->actorInstance, node, nodePos);

		// PP: create dust at that position
		PongDust_create(nodePos, realNumParticles, vel, PONGDUST_NO_FREQUENCY);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_sort
	Purpose 	: sort all pong dust particles into their correct draw order
	Parameters 	: (opt/very start)start of array portion to sort,
					(opt/very end)end of array portion to sort
	Returns 	: 
	Info 		: // PP: draw order is based solely on depth.
					// PP: The depth sort is required to prevent particles from blending with each other.
					// PP: This non-interblending (along with the outlining) is the key to the cartoony look of the pong dust.
					// PP: If you know of a nicer way of preventing the inter-blending PLEASE tell me!!
					// PP: I'm using a 'quick sort' algorithm here.  I just hope it's 'quick' enough!
*/
static void PongDust_sort(const int32 left=0, const int32 right=PongDust_particles->size-1)
{
	int32	i, last;

	if(left >= right) return;// PP: feck off

	utilSwap(PongDust_drawOrder, left, (left+right)/2);
	last=left;

	PONGDUST	*particle1, *particle2;

	for(i=left+1;i<=right;i++)
	{
		particle1=PongDust_particles->getItem(PongDust_drawOrder[i]);
		particle2=PongDust_particles->getItem(PongDust_drawOrder[left]);

		// PP: to prevent nasty inter-blending, we gotter use distance from camera
		// PP: IN DIRECTION OF CAMERA to sort the particles.
		// PP: This is because a farther-away sprite can still put some of its pixels closer in the
		// PP: z-buffer than a closer sprite, at certain angles....

		// PP: NOTE: Because of the way Quick Sort works, no particle will ever have its zdepth calculated or tested more than once...

		particle1->zDepth=(particle1->pos-gameStatus.viewPoint).dotProduct(PongDust_camDirNorm);
		particle2->zDepth=(particle2->pos-gameStatus.viewPoint).dotProduct(PongDust_camDirNorm);

		if(particle1->zDepth > particle2->zDepth)
		{
			utilSwap(PongDust_drawOrder, ++last, i);
		}
	}

	utilSwap(PongDust_drawOrder, left, last);

	PongDust_sort(left, last-1);
	PongDust_sort(last+1, right);	
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_draw
	Purpose 	: draw all pong dust
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_draw(void)
{
	// PP: check there are particles to draw
	if(PongDust_particles == NULL)
	{
		return;
	}

	if(PongDust_particles->size == 0)
	{
		return;
	}


	// PP: depth-sort the pong dust particles on each draw (this is integral to the effect!)...

	int					i;

PD_STARTTIMER;

	// PP: list the indices of all used particles
	for(i=PongDust_particles->size-1;i>=0;i--)
	{
		PongDust_drawOrder[i]=i;
	}

	// PP: get normalised camera direction vector (should really store this in gameStatus but I don't much fancy a half-hour build)
	PongDust_camDirNorm=VEC(0.0f, 0.0f, 1.0f).rotate(gameStatus.viewAng);

	PongDust_sort();

PD_ENDTIMER("pongdust sort");

	// PP: save render states
	SAVESTATES;

	// PP: set render states...

	SETPROJECTION(3D);
	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_POINT, 0);

	bdSetIdentityObjectMatrix();

	bdSetTexture(0, PONGDUST::texture);

	uint32	vertIndex=0;

	// PP: MAIN (TRANSLUCENT) PASS...

PD_STARTTIMER;

#if(BPLATFORM == PS2)
	bdSetZBias(0.0f);
#else// PP: not ps2
	bdSetZBias(PONGDUST_NORMAL_ZBIAS);
#endif// PP: not ps2

	// PP: draw the pong dust particles, in the order that we've applied to them
	for(i=PongDust_particles->size-1; i>=0; i--)
	{
		PongDust_particles->getItem(PongDust_drawOrder[i])->draw(false, global2DVertexBuffer, &vertIndex);
	}

#if(BPLATFORM != PS2)
	bdSetZBias(0.0f);
#endif// PP: not ps2

	// set sprite projection mode
	SETPROJECTION(SPRITE);

	// PP: draw the verts representing the particles
	if(vertIndex > 0)
	{	
		// PP: set an object matrix to correct the appearance of the sprites
		setSpriteObjMat();

		bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, vertIndex, BVERTTYPE_SINGLE2D);
	}

PD_ENDTIMER("pongdust alpha pass");

PD_STARTTIMER;

	SETPROJECTION(3D);

	bdSetIdentityObjectMatrix();

	// PP: SECOND (OUTLINE) PASS...

	// PP: pretty fookin important
	vertIndex=0;

	// PP: z-bias the outlines back a bit so's you can't make out the outlines of individual particles
	// PP: In this way we're collectively outlining CLUSTERS of particles, which makes them look like a real hand-drawn cartoon clouds
	// PP: see?

#if(BPLATFORM != PS2)
	bdSetZBias(PONGDUST_NORMAL_ZBIAS+PONGDUST_OUTLINE_ZBIAS);
#endif// PP: not ps2

	// PP: draw the pong dust particles, in the order that we've applied to them

	for(i=PongDust_particles->size-1; i>=0; i--)
	{
		PongDust_particles->getItem(PongDust_drawOrder[i])->draw(true, global2DVertexBuffer, &vertIndex);
	}

#if(BPLATFORM != PS2)
	bdSetZBias(0.0f);
#endif// PP: not ps2

	// set sprite projection mode
	SETPROJECTION(SPRITE);

	// PP: draw the verts representing the particles
	if(vertIndex > 0)
	{
		// PP: set an object matrix to correct the appearance of the sprites
		setSpriteObjMat();

		bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, vertIndex, BVERTTYPE_SINGLE2D);
	}

PD_ENDTIMER("pongdust outline pass");

	bdSetIdentityObjectMatrix();

	// PP: restore render states
	RESTORESTATES;
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_update
	Purpose 	: update all pong dust
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_update(void)
{
	if(PongDust_particles == NULL)
	{
		return;
	}

PD_STARTTIMER

#ifdef PONGDUST_LIMIT_TO_2_FRAMES

	static bool skip=1;

	// PP: limit update rate of pong dust to a cartoony 30/25fps...

	if(bFPS > 45)// PP: if we're running in a frame// PP: 30 fps is the fastest framerate achievable when running in 2 frames
	{
		if(skip^=1)
		{
			return;
		}
	}

#endif// PP: def PONGDUST_LIMIT_TO_2_FRAMES

	// PP: check there are particles to update
	if(PongDust_particles->size == 0)
	{
		return;
	}

#ifdef PONGDUST_TIMERS
	bkPrintf("num PONGDUST PARTICLES: %d (max %d)\n", PongDust_particles->size, PONGDUST_MAX_PARTICLES);
#endif// PP: def PONGDUST_TIMERS

	if(PongDust_particles->size > (PONGDUST_MAX_PARTICLES*0.9f))
	{
		bkPrintf("################################  EEEEKKK YOU NEED TO ALLOW FOR MORE PONG DUST PARTICLES!!!!! ###########################\n");
	}

	// PP: update the particles, discarding them each as they expire...

	LA_FOREACH(*PongDust_particles, PONGDUST, particle)

		particle->update();

		if(particle->flags & PONGDUSTFLAG_FINISHED)
		{
			// PP: NOTE: This is only safe because particle expiry rates are all identical
			// PP: If this wasn't the case, ye'd see some old particles popping out prematurely
			PongDust_particles->discardOldest();
		}

	LA_ENDFOREACH(*PongDust_particles)

PD_ENDTIMER("pongdust update");
}


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_ring
	Purpose 	: create a ring of pong dust, as if something/someone has hit the ground
	Parameters 	: centre position of ring
	Returns 	: 
	Info 		: 
*/
void PongDust_ring(const VEC& pos)
{
#define PONGDUST_RING_NUM_PARTICLES  50

	VEC		vel			=VEC(0, 0, 400);
	float	angStep		=TWOPI/PONGDUST_RING_NUM_PARTICLES;
	float	ang			=0.0f;

	for(int i=PONGDUST_RING_NUM_PARTICLES-1; i>=0; i--)
	{
		PongDust_create(pos, 1, vel.rotateY(ang), PONGDUST_NO_FREQUENCY);

		ang+=angStep;
	}
}