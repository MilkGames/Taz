// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Fireball.h
//   Purpose : Particle emitters used for explosion effects, eg. by WestBossExp
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_FIREBALL
#define __H_FIREBALL

#include "LoopArray.h"				// PP: Looping array class - please use it!

// PP: define FB_FAIRY_EXPLOSIONS if you want to be able to use the "fairy" preset explosion type
// PP: #define FB_FAIRY_EXPLOSIONS

// PP: should we sort fireballs into a good draw order before drawing them?...
extern bool							Fireball_applyDrawOrder;

// PP: ...normally we don't need to
#define FB_NORMALLY_SORT_DRAW_ORDER	false

// CMD: flags
#define FIREBALLFLAG_NOCAMERASHAKE		(1<<0)
#define FIREBALLFLAG_NOSOUND			(1<<1)
#define FIREBALLFLAG_NOPADRUMBLE		(1<<2)

// PP: maximum number of particles that can be visible in a single fireball at any one time
#define FB_MAX_PARTICLES												100// PP: 100 is currently plenty for the West Boss// PP: REMOUT: 250

// PP: typedef for fireballs' particle arrays
typedef LOOPARRAY<class TAG_FBPARTICLE, FB_MAX_PARTICLES>				FB_ParticleArray;

// PP: default update callback for fireballs
#define FB_DEFAULT_UPDATE_CALLBACK				NULL

// PP: default callback context for fireballs
#define FB_DEFAULT_CALLBACK_CONTEXT				0

// PP: default emitter speed for fireballs
#define FB_DEFAULT_SPEED						METERTOUNIT(8)				// PP: currently the speed of west boss explosions

// PP: default scale for fireballs
#define FB_DEFAULT_SCALE						1.0f

// PP: default maximum heat of particles emitted by a fireball
#define FB_DEFAULT_HEAT							1.0f						// PP: full heat

// PP: default portion of emitter velocity to use for local wind velocity
#define FB_DEFAULT_LOCAL_WIND_VELOCITY_PORTION	0.0f

#define FBP_OPACITY_SMOKE		30// PP: 70

#if BPLATFORM==PS2
#define FBP_OPACITY_FIRE		120
#define FBP_COLOUR_HOT			COLOUR(255, 255, 0, FBP_OPACITY_FIRE)
#define FBP_COLOUR_COLD			COLOUR(90,0,0,FBP_OPACITY_FIRE)
#define FBP_COLOUR_SMOKE		COLOUR(25,25,25,FBP_OPACITY_SMOKE)
#define FBP_COLOUR_FINAL		COLOUR(30,30,30,0)
#else

// PP: oooh dear.  this looks so grotty on PC

#define FBP_OPACITY_FIRE		255
#define FBP_COLOUR_HOT			COLOUR(255, 255, 0, FBP_OPACITY_FIRE)
#define FBP_COLOUR_COLD			COLOUR(90,0,0,FBP_OPACITY_FIRE)
#define FBP_COLOUR_SMOKE		COLOUR(25,25,25,FBP_OPACITY_SMOKE)
#define FBP_COLOUR_FINAL		COLOUR(30,30,30,0)
#endif


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FBPARTICLE

										The particles emitted by / forming a fireball
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: states for fireball particles
enum FBP_State
{
	FBPSTATE_NOTUSED,						// PP: particle not used yet (invisible)
	FBPSTATE_EXPLODING,						// PP: particle burning/fading
	FBPSTATE_FINISHED						// PP: particle finished (invisible)
};

// PP: fireball particle
typedef class TAG_FBPARTICLE
{
public:

	TBVector			velocity;			// PP: current velocity of particle
	float				rotSpeed;			// PP: z rotation speed of particle
	float				ang;				// PP: z angle fo particle
	float				size;				// PP: size of particle sprite
	float				rise;				// PP: rise speed of particle
	FBP_State			state;				// PP: current state of particle
	float				life;				// PP: amount of life remaining (0..1)
	float				timer;
	COLOUR				colour;				// PP: current particle colour
	float				opacity;			// PP: opacity, used for quick fade-in
	static TBTexture*	texture;			// PP: sprite to use for particles

	class TAG_FIREBALL*	fireball;			// PP: ptr to fireball that owns the particle

	TBVector			pos;				// PP: current position of particle


	/*	--------------------------------------------------------------------------------
		Function 	: FBPARTICLE::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void)
	{
		this->state=FBPSTATE_NOTUSED;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: FBPARTICLE::TAG_FBPARTICLE
		Purpose 	: default constructor: calls init
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	TAG_FBPARTICLE(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: FBPARTICLE::init
		Purpose 	: initialisation
		Parameters 	: ptr to fireball that owns the particle, position, velocity, life
		Returns 	: 
		Info 		: 
	*/
	void init(class TAG_FIREBALL* const fireballIn, TBVector posIn, TBVector velIn, const float lifeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: FBPARTICLE::update
		Purpose 	: update
		Parameters 	: ref to loop-array of particles to which this belongs
		Returns 	: 
		Info 		: 
	*/
	void update(FB_ParticleArray& particles);


	/*	--------------------------------------------------------------------------------
		Function 	: FBPARTICLE::draw
		Purpose 	: draw
		Parameters 	: ptr to start of vertex buffer to which to draw the particle, (in&out)ptr to current vert index (updated by this method)
		Returns 	: 
		Info 		: 
	*/
	void draw(TBPrimVertex2D* const vertBuf, int32* const vertIndex);


}FBPARTICLE BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FIREBALL

								An emitter of fiery particles, as used for explosion effects
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


// PP: states for fireballs
enum FB_State
{
	FBSTATE_EMITTING,													// PP: emitting particles
	FBSTATE_FADING,														// PP: finished emitting particles, but particles still exist
	FBSTATE_FINISHED													// PP: all particles finished
};


// PP: MOVEME
enum EFireballStageColour
{
	FBSTAGECOLOUR_HOT,
	FBSTAGECOLOUR_COLD,
	FBSTAGECOLOUR_SMOKE,
	FBSTAGECOLOUR_FINAL,

	FB_NUM_STAGE_COLOURS

}ENUMOPS(EFireballStageColour);


typedef void(*FB_UpdateCallback)(class TAG_FIREBALL* const fireball, const long32 context);

// PP: fireball type
typedef class TAG_FIREBALL
{
public:

	FB_ParticleArray						particles;					// PP: looping array of particles
	TBVector								pos;						// PP: emitter position
	TBVector								vel;						// PP: emitter velocity
	FB_State								state;						// PP: emitter state
	float									timer;						// PP: general-purpose timer
	float									lifeTimer;					// PP: how long the fireball has been emitting for
	float									lifespan;					// PP: emitter lifespan
	float									range;						// PP: distance to travel
	float									distTraveled;				// PP: distance traveled so far
	float									heat;						// PP: (smoke 0..1 fire) maximum heat of new particles
	TBVector								localWindVelocity;			// PP: velocity of local wind - eg. cannons use this to waft the smoke out from the cannon even after the fireball has stopped emitting
	float									riseSpeed;					// PP: speed at which the particles rise

	COLOUR									colours[FB_NUM_STAGE_COLOURS];		

	// PP: TODO: do I need BOTH of these?
	TBVector								scale;						// PP: scale factors for explosion (1.0f gives a 'normal' explosion)
	TBMatrix								scaleMat;					// PP: scale factors for explosion (1.0f gives a 'normal' explosion)

	FB_UpdateCallback						updateCallback;				// PP: update callback
	long32									callbackContext;			// PP: context value for callback(s)

	BOUNDINGBOX								boundingBox;				// PP: bounding box of the fireball

	TBLightsource							lightSource;				// PP: light source belonging to the fireball


	/*	--------------------------------------------------------------------------------
		Function 	: FIREBALL::init
		Purpose 	: initialise the fireball
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FIREBALL::update
		Purpose 	: update the fireball
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FIREBALL::draw
		Purpose 	: draw the fireball
		Parameters	: 
		Returns		: 
		Info		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FIREBALL::stop
		Purpose 	: stop the fireball emitting - let it fade
		Parameters	: 
		Returns		: 
		Info		: "Schtøpp!"
	*/
	inline void stop(void)
	{
		baEnableLight(&this->lightSource, FALSE);
		this->state=FBSTATE_FADING;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: FIREBALL::sortCompareCallback
		Purpose 	: comparison callback for utilQuickSort: determines if one FIREBALL is farther from the camera than another
		Parameters	: uchar ptr to first fireball, uchar ptr to second fireball
		Returns		: true if the first fireball is farther from the camera than the second, else false
		Info		: must match the QuickSortCallback typedef (util.h)
	*/
	static inline bool sortCompareCallback(uchar* const i, uchar* const j);


}FIREBALL BALIGN16;


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
void Fireball_init(void);


/* --------------------------------------------------------------------------------
   Function		: Fireball_shutdown
   Purpose		: shutdown the fireball list
   Parameters	: 
   Returns		: 
   Info			: this is called on exiting a level
*/
void Fireball_shutdown(void);


/* --------------------------------------------------------------------------------
   Function		: Fireball_update
   Purpose		: update all fireparticles
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Fireball_update(void);


/* --------------------------------------------------------------------------------
   Function		: Fireball_draw
   Purpose		: draw all fireparticles
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Fireball_draw(void);


// PP: flags used when creating FIREBALLs
enum FB_CreationFlag
{
	// PP: movement modes...
	FBCREATEFLAG_TARGETPOS			=(1<<0),	// PP: fireball travels to a specified 3D position
	FBCREATEFLAG_VEL				=(1<<1),	// PP: fireball travels at a specified 3D velocity
	FBCREATEFLAG_ANG2D				=(1<<2),	// PP: fireball travels at a specified 2D (XY) angle
											
	// PP: expiry modes...					
	FBCREATEFLAG_RANGE				=(1<<3),	// PP: fireball travels a specified scalar distance
	FBCREATEFLAG_LIFESPAN			=(1<<4),	// PP: fireball emits for a specified time
											
	// PP: other...
	FBCREATEFLAG_LOCALWINDFROMVEL	=(1<<5),	// PP: use a specified portion of the fireball's initial emitter velocity for the initial local-wind velocity
	FBCREATEFLAG_SPEED				=(1<<6),	// PP: fireball travels at a specified scalar speed towards target pos

// PP: REMOUT: TODO: 	FBCREATEFLAG_PARTICLEVELOCITY	=(1<<6),	// PP: fireball emits particles with a specified velocity (plus random variation) - could be used for a flamethrower effect (NOTE we already have one!)
};


// PP: struct used when creating FIREBALLs
struct FIREBALLINFO
{
	int					flags;						// PP: creation flags
													
	VEC					pos;						// PP: start position
	TBVector2D			ang2D;						// PP: XY angle
	VEC					vel;						// PP: velocity
	float				range;						// PP: distance to travel
	float				lifespan;					// PP: period of time for which to exist
	VEC					targetPos;					// PP: position to move to
	VEC					particleVelocity;			// PP: NOT USED YET, velocity around which to emit particles
	VEC					scale;						// PP: scale factors for fireball
	float				heat;						// PP: max heat of particles emitted by the fireball
	float				localWindVelocityPortion;	// PP: portion of fireball's initial velocity to use for initial local wind velocity
													
	float				speed;						// PP: emitter speed
	FB_UpdateCallback	updateCallback;				// PP: update callback
	long32				callbackContext;			// PP: callback context

	COLOUR				colours[FB_NUM_STAGE_COLOURS];		
	

	/* --------------------------------------------------------------------------------
	   Function		: FIREBALLINFO::FIREBALLINFO
	   Purpose		: default constructor, does default constructor type things, y'know?
	   Parameters	: 
	   Returns		: 
	   Info			: 
	*/
	FIREBALLINFO(void)
	{
		this->flags=0;
		this->heat=FB_DEFAULT_HEAT;
		this->updateCallback=FB_DEFAULT_UPDATE_CALLBACK;
		this->callbackContext=FB_DEFAULT_CALLBACK_CONTEXT;

		this->colours[FBSTAGECOLOUR_HOT]=FBP_COLOUR_HOT;
		this->colours[FBSTAGECOLOUR_COLD]=FBP_COLOUR_COLD;
		this->colours[FBSTAGECOLOUR_SMOKE]=FBP_COLOUR_SMOKE;
		this->colours[FBSTAGECOLOUR_FINAL]=FBP_COLOUR_FINAL;
	}


}BALIGN16;


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: ref to FIREBALLINFO struct
   Returns		: 
   Info			: 
*/
void Fireball_create(const FIREBALLINFO& info);


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: position, velocity, range, (opt/default)scale, (opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback, (opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context
   Returns		: 
   Info			: 
*/
void Fireball_create(	TBVector					pos,
						TBVector					vel,
						const float					range,
						const float					scale			=FB_DEFAULT_SCALE,
						const float					heat			=FB_DEFAULT_HEAT,
						const FB_UpdateCallback		updateCallback	=FB_DEFAULT_UPDATE_CALLBACK,
						const long32				context			=FB_DEFAULT_CALLBACK_CONTEXT,
						const COLOUR&				hotCol			=FBP_COLOUR_HOT,
						const COLOUR&				coldCol			=FBP_COLOUR_COLD,
						const COLOUR&				smokeCol		=FBP_COLOUR_SMOKE,
						const COLOUR&				finalCol		=FBP_COLOUR_FINAL);


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: speed (unts/sec), start position, target position, (opt/FB_DEFAULT_SCALE)scale, (opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback, (opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context
   Returns		: 
   Info			: param order not ideal - sorry - had to avoid conflict with another overload
*/
void Fireball_create(	const float					speed,
						TBVector					pos,
						TBVector					targetPos,
						const float					scale			=FB_DEFAULT_SCALE,
						const float					heat			=FB_DEFAULT_HEAT,
						const FB_UpdateCallback		updateCallback	=FB_DEFAULT_UPDATE_CALLBACK,
						const long32				context			=FB_DEFAULT_CALLBACK_CONTEXT,
						const COLOUR&				hotCol			=FBP_COLOUR_HOT,
						const COLOUR&				coldCol			=FBP_COLOUR_COLD,
						const COLOUR&				smokeCol		=FBP_COLOUR_SMOKE,
						const COLOUR&				finalCol		=FBP_COLOUR_FINAL);


/* --------------------------------------------------------------------------------
   Function		: Fireball_create
   Purpose		: create a fireball and set it going
   Parameters	: position, x angle, y angle, speed, range, (opt/FB_DEFAULT_SCALE)scale,
					(opt/FB_DEFAULT_HEAT)heat, (opt/FB_DEFAULT_UPDATE_CALLBACK)update callback,
					(opt/FB_DEFAULT_CALLBACK_CONTEXT)callback context,
					(opt/default)portion of emitter velocity to use for local wind velocity
   Returns		: 
   Info			: just saves a bit of typing eh?
*/
void Fireball_create(	TBVector					pos,
						const float					xAngle,
						const float					yAngle,
						const float					speed,
						const float					range,
						const float					scale						=FB_DEFAULT_SCALE,
						const float					heat						=FB_DEFAULT_HEAT,
						const FB_UpdateCallback		updateCallback				=FB_DEFAULT_UPDATE_CALLBACK,
						const long32				context						=FB_DEFAULT_CALLBACK_CONTEXT,
						const float					localWindVelocityPortion	=FB_DEFAULT_LOCAL_WIND_VELOCITY_PORTION,
						const COLOUR&				hotCol						=FBP_COLOUR_HOT,
						const COLOUR&				coldCol						=FBP_COLOUR_COLD,
						const COLOUR&				smokeCol					=FBP_COLOUR_SMOKE,
						const COLOUR&				finalCol					=FBP_COLOUR_FINAL);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_explosionSupportEffects
	Purpose 	: start effects to support an explosion - boom noise, camera shake, pad rumble
	Parameters 	: explosion position
	Returns 	: 
	Info 		: hmm not yet sure about this
*/
void Fireball_explosionSupportEffects(TBVector pos, uint32 flags=0);


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

#define FB_NORMALEXPLOSION_DEFAULT_RADIUS		METERTOUNIT(2.5f)// PP: NOT USED YET

void Fireball_normalExplosion(TBVector pos, const float radius=FB_NORMALEXPLOSION_DEFAULT_RADIUS, const COLOUR& hotCol=FBP_COLOUR_HOT, const COLOUR& coldCol=FBP_COLOUR_COLD, const COLOUR& smokeCol=FBP_COLOUR_SMOKE, const COLOUR& finalCol=FBP_COLOUR_FINAL, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_normalExplosion
	Purpose 	: create a 'normal' explosion using a fireball
	Parameters 	: actorinstance upon which to base explosion, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: TODO?: ceiling control.  NOTE: radius of effect can only ever ROUGHLY match radius specified
*/
void Fireball_normalExplosion(ACTORINSTANCE* const actorInstance, const COLOUR& hotCol=FBP_COLOUR_HOT, const COLOUR& coldCol=FBP_COLOUR_COLD, const COLOUR& smokeCol=FBP_COLOUR_SMOKE, const COLOUR& finalCol=FBP_COLOUR_FINAL, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_flatExplosion
	Purpose 	: create a flat explosion using fireball(s)
	Parameters 	: explosion position, (opt/default)explosion radius, (opt/default)explosion height, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: !!!effect to be finalised!!!
*/

#define FB_FLATEXPLOSION_DEFAULT_RADIUS			METERTOUNIT(20)
#define FB_FLATEXPLOSION_DEFAULT_HEIGHT			FB_NORMALEXPLOSION_DEFAULT_RADIUS

void Fireball_flatExplosion(TBVector pos, const float radius=FB_FLATEXPLOSION_DEFAULT_RADIUS, const float height=FB_FLATEXPLOSION_DEFAULT_HEIGHT, const COLOUR& hotCol=FBP_COLOUR_HOT, const COLOUR& coldCol=FBP_COLOUR_COLD, const COLOUR& smokeCol=FBP_COLOUR_SMOKE, const COLOUR& finalCol=FBP_COLOUR_FINAL, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_flatExplosion
	Purpose 	: create a flat explosion using fireball(s)
	Parameters 	: actorinstance upon which to base explosion, (all opt/default) 4 colours for explosion
	Returns 	: 
	Info 		: !!!effect to be finalised!!!
*/
void Fireball_flatExplosion(ACTORINSTANCE* const actorInstance, const COLOUR& hotCol=FBP_COLOUR_HOT, const COLOUR& coldCol=FBP_COLOUR_COLD, const COLOUR& smokeCol=FBP_COLOUR_SMOKE, const COLOUR& finalCol=FBP_COLOUR_FINAL, uint32 flags=0);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_cannonExplosion
	Purpose 	: create a cannon explosion using a fireball
	Parameters 	: cannon actorinstance from which to emit explosion
	Returns 	: 
	Info 		: 
*/
void Fireball_cannonExplosion(ACTORINSTANCE* const actorInstance);


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_cementMixerExplosion
	Purpose 	: create a puff of dust for a cement-mixer (cannon type), using a fireball
	Parameters 	: cement-mixer actorinstance from which to emit puff
	Returns 	: 
	Info 		: 
*/
void Fireball_cementMixerExplosion(ACTORINSTANCE* const actorInstance);


#ifdef FB_FAIRY_EXPLOSIONS

// PP: the different types of fairy, with regard to fairy explosions
enum FBFairy
{
	FBFAIRY_COSMO,		// PP: Cosmo
	FBFAIRY_WANDA,		// PP: Wanda

	FB_NUM_FAIRIES		// PP: number of fairy types, with regard to fairy explosions
};


/*	--------------------------------------------------------------------------------
	Function 	: Fireball_fairyExplosion
	Purpose 	: create a fairy explosion
	Parameters 	: actorinstance at whose position which to emit the puff,
					type of fairy explosion (FBFAIRY_COSMO/FBFAIRY_WANDA)
	Returns 	: 
	Info 		: 
*/
void Fireball_fairyExplosion(const ACTORINSTANCE* const actorInstance, const FBFairy fairy);

#endif// PP: def FB_FAIRY_EXPLOSIONS


#endif // PP: ndef __H_FIREBALL