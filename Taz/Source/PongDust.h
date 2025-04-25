// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PongDust.h
//   Purpose : Cartoon dust effect, for Elephant Pong
// Component : Taz
//      Info :	// PP: Dust particles are collectively outlined and collectively have a single level of transparency
//				// PP: (ie. they don't blend with each other).
//				// PP: PP: yes, I know we already have umpteen different smoke and dust effects...
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_PONGDUST
#define __H_PONGDUST


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PONGDUST

							A particle of pong dust - each one is a little sprite dust cloud
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


enum EPongDustFlag
{
	PONGDUSTFLAG_FINISHED	=1<<0,				// PP: particle has expired and disappeared
};


typedef struct TAG_PONGDUST
{
	// PP: hmmmm, this all looks a bit familiar.
	// PP: I wonder if (on the next game) there'd be some mileage in having generic SpriteParticle and SpriteParticleManager classes?

	static TBTexture*		texture;					// PP: sprite to use for particles (128*128 solid white-on-magenta blob)
	static COLOUR			colour;						// PP: TEMP?: dust body colour
	static COLOUR			outlineColour;				// PP: TEMP?: dust outline colour
														
	VEC						pos;						// PP: 3d position of particle
	VEC						vel;						// PP: 3d velocity of particle
	float					ang;						// PP: Z angle of particle
														
	float					initialRotSpeed;			// PP: initial (fastest) Z rotation speed
	float					finalRotSpeed;				// PP: final (slowest) Z rotation speed
	float					rotSpeed;					// PP: current Z rotation speed
														
	float					initialSize;				// PP: initial size of particle (once popped-in)
	float					size;						// PP: current size of particle

	float					imperfectionScale;			// PP: scale factor for main sprite, creating imperfections
	float					outlineImperfectionScale;	// PP: scale factor for outline sprite, creating imperfections
							
	float					timer;						// PP: downwards life timer (seconds)
														
	float					zDepth;						// PP: distance of particle from camera, in direction of camera
														
	uchar					flags;						// PP: combination of EPongDustFlags


	/*	--------------------------------------------------------------------------------
		Function 	: PONGDUST::init
		Purpose 	: initialise the pong dust particle
		Parameters 	: posizione, velocitah
		Returns 	: 
		Info 		: 
	*/
	void init(const VEC& posIn, const VEC& velIn);


	/*	--------------------------------------------------------------------------------
		Function 	: PONGDUST::update
		Purpose 	: update the pong dust particle
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: PONGDUST::draw
		Purpose 	: draw the pong dust particle
		Parameters 	: drawing outline (2nd) pass?, ptr to start of vertex buffer to which to draw the particle,
						(in&out)ptr to current vert index (updated by this method)
		Returns 	: 
		Info 		: 
	*/
	void draw(const bool outline, TBPrimVertex2D* const vertBuf, uint32* const vertIndex);


}PONGDUST BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														PONGDUSTEMITTER

													An emitter of pong dust.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


struct PONGDUSTEMITTER
{
	VEC			pos;			// PP: position
	VEC			lastPos;		// PP: position on last update (used to interpolate dust path)
	float		timer;			// PP: downwards timer till end of emission
	float		freqeuency;		// PP: particles per second

}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL PONG DUST FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_sceneInit
	Purpose 	: initialise the pong dust system at the beginning of a scene that's gonna use it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_sceneInit(void);


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_sceneShutdown
	Purpose 	: shut-down the pong dust system at the end of a scene that's been using it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_sceneShutdown(void);


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

// PP: "don't use a frequency - just create all my particles right now!"
#define PONGDUST_NO_FREQUENCY				0.0f

// PP: number of particles to create with PongDust_create, if a number isn't specified
#define PONGDUST_DEFAULT_NUM_PARTICLES		1

void PongDust_create(const VEC&				pos,
					 const uchar			numParticles	=PONGDUST_DEFAULT_NUM_PARTICLES,
					 const VEC&				vel				=bIdentityVector/*ZVEC*/,
					 const float			frequency		=PONGDUST_NO_FREQUENCY);


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_create
	Purpose 	: kick up some pong dust at a node's position
	Parameters 	: ACTORINSTANCE*, name of node, (opt/default)number of particles,
					(opt/ZVEC)rough velocity vector(random deviation and drift will be applied),
					(opt/PONGDUST_NO_FREQUENCY)frequency of particle creation (eg. 10.0f = create 10 times a second)
	Returns 	: 
	Info 		: 
*/
void PongDust_create(ACTORINSTANCE* const	actorInstance,
					 char* const			nodeName,
					 const uchar			numParticles	=PONGDUST_DEFAULT_NUM_PARTICLES,
					 const VEC&				vel				=bIdentityVector/*ZVEC*/,
					 const float			frequency		=PONGDUST_NO_FREQUENCY);


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_draw
	Purpose 	: draw all pong dust
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_draw(void);


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_update
	Purpose 	: update all pong dust
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PongDust_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: PongDust_ring
	Purpose 	: create a ring of pong dust, as if something/someone has hit the ground
	Parameters 	: centre position of ring
	Returns 	: 
	Info 		: 
*/
void PongDust_ring(const VEC& pos);


#endif// PP: ndef __H_PONGDUST