#ifndef __H_SHADOW
#define __H_SHADOW

#include "ShadowExtra.h"			// PP: Useful shadow stuff that needs to be included when shadow.h can't be
#include "Colour.h"					// PP: handy colour struct
#include "LoopArray.h"				// PP: Looping array class - please use it!
#include "textfx.h"

//#define SHADOW_COLLTEST// PP: What does this do? // TP: It draws the shadow on all polys around Taz, sometimes usefull for showing collision problems

// PP: camera offset from focus used when rendering shadow maps
extern TBVector				shadowMapViewpointOffset;

// must reduce number at some stage
#ifndef SHADOW_COLLTEST
	// PP: now using the same limit as PS2 - twas well dodge to have different limits for the same shadows
	#define MAX_SHADOW_POLYS		150// TP: Sorry I need the memory, boo hoo PS2 have no memory, when I get time I'll change the enemies so they only have shadows allocated when they are active
#else
	#define MAX_SHADOW_POLYS		(1000)
#endif
//60
extern BOOK	shadowLightEditBook;	// PP: book used to edit shadow-casting lights (this is going to be SO cool! :)


#define SHADOWFLAG_COLLECTPOLYS	(1<<0)		// TP: on by default, tells the createshadow routine to collect the polys for the shadow on each call
											// TP: by disabling this after the first call you can still change the size of the shadow but it wont
											// TP: do all the collision tests etc. used on the falling objects
#define SHADOWFLAG_DRAWONLY	(1<<1)			// TP: Do no calculations, only draw shadow

typedef struct TAG_SHADOW
{
	TBPrimVertex				vertices[MAX_SHADOW_POLYS*3];
								
	ShadowType					type;							// PP: shadow type (SHADOWTYPE_)
	COLOUR						colour;							// PP: shadow colour, i.e. colour to subtract from landscape - ALPHA SHOULD STAY AT 128!!!
																
	int32						numPolys;						
	float						shadowRadius;					
	float						shadowHeight;
	float						groundY;						// TP: y value of the polys the shadow is cast onto
	uchar						flags;							// TP: shadow flags SHADOWFLAG_...
																
	TBActorNodeInstance			*node;							// option node to cast shadow from
	ACTORINSTANCE				*actorInstance;					// PP: actor instance casting the shadow
								
	// PP: ~~~~					

	bool						useShadowMap;					// PP: generate a shadow map to use as the shadow texture
	float						shadowMapRadius;				// PP: radius of shadow map
	float						shadowMapFOV;					// PP: FOV used to render shadow map, calculated from constant camera offset and character's dynamic bounding sphere radius
	float						shadowMapDist;					// PP: (point lights only) distance from light to occluder
	struct TAG_SHADOWLIGHT*		light;							// PP: TEMP: pointer to a single shadow-casting lightsource (only used by 'true' type shadows)
	VEC							lightAngle;						// PP: directional light angle, or angle from point light to character (gets calculated in VFX_generateShadowMap)
	VEC							lightPos;						// PP: point light position, or effective position of directional light source (gets calculated in CreateTrueShadow)

} SHADOW BALIGN16;

/* --------------------------------------------------------------------------------
   Function : CreateActorShadows
   Purpose : Scans thought an actor list and creates any shadows as defined
   Parameters : actorInstanceList, collisioncache
   Returns : 
   Info : calls	CreateShadow
*/
void CreateActorShadows(ACTORINSTANCELIST *actorInstanceList, struct TAG_COLLISIONCACHE *collisionCache);

/* --------------------------------------------------------------------------------
   Function : SetShadowNode
   Purpose : Sets the node for the shadow to use
   Parameters : actorInstance, node (if NULL the actors position is used)
   Returns : 
   Info : 
*/
void SetShadowNode(ACTORINSTANCE *actorInstance, TBActorNodeInstance *node, char *nodeName);

/* --------------------------------------------------------------------------------
   Function : DrawActorShadows
   Purpose : Scans thought an actor list and draws any shadows listed
   Parameters : actorInstanceList
   Returns : 
   Info : calls	DrawShadow
*/
void DrawActorShadows(ACTORINSTANCELIST *actorInstanceList);

/*	--------------------------------------------------------------------------------
	Function 	: InitialiseShadow
	Purpose 	: intialise a shadow structure
	Parameters 	: ptr to ACTORINSTANCE whose shadow to initialise,
					radius of shadow, max cast range of shadow,
					(opt/default)max radius of object for shadow map rendering,
					(opt/default) type of shadow (SHADOWTYPE_)
	Returns 	: 
	Info 		: // PP: if my changing of the parameters has caused any problems, feel free to slap me
*/
void InitialiseShadow(ACTORINSTANCE *const	actorInstance,
					  float					shadowRadius,
					  float					shadowHeight,
					  const float			shadowMapRadius	=SHADOWMAP_DEFAULT_OCCLUDER_RADIUS,
					  const ShadowType		shadowType		=SHADOW_DEFAULT_TYPE);

/*	--------------------------------------------------------------------------------
	Function 	: CreateShadow
	Purpose 	: Fill in the shadow structure will all information needed to draw the shadow
	Parameters 	: ptr to ACTORINSTANCE, collisionCache to use, position of object casting shadow
	Returns 	: completed shadow structure
	Info 		: 
*/
void CreateShadow(ACTORINSTANCE* actorInstance, struct TAG_COLLISIONCACHE *collisionCache, TBVector pos);


/*	--------------------------------------------------------------------------------
	Function 	: CreateShadow
	Purpose 	: Fill in the shadow structure will all information needed to draw a 'True' shadow
	Parameters 	: ptr to ACTORINSTANCE, collisionCache to use, position of object casting shadow
	Returns 	: completed shadow structure
	Info 		: 
*/
void CreateTrueShadow(ACTORINSTANCE* actorInstance, struct TAG_COLLISIONCACHE *collisionCache, TBVector pos);


/*	--------------------------------------------------------------------------------
	Function 	: CreateZooBossShadow
	Purpose 	: Fill in the shadow structure with all information needed to draw the sunset shadow
	Parameters 	: shadow to fill in, collisionCache to use, position of object casting shadow
	Returns 	: completed shadow structure
	Info 		: // PP: added for the Zoo boss game
*/
void CreateZooBossShadow(SHADOW *shadow, struct TAG_COLLISIONCACHE *collisionCache, TBVector pos);

/*	--------------------------------------------------------------------------------
	Function 	: DrawShadow
	Purpose 	: Takes a shadow structure and renders the polys
	Parameters 	: Shadow structure
	Returns 	: 
	Info 		: 
*/
void DrawShadow(SHADOW *shadow);

/*	--------------------------------------------------------------------------------
	Function 	: PolyBoxIntersectXZ
	Purpose 	: Determines if a polygon intersects a box
	Parameters 	: box, p1, p2, p3 points on poly
	Returns 	: TRUE / FALSE
	Info 		: 
*/
BOOL PolyBoxIntersectXZ(TBVector box[4],TBVector p1,TBVector p2,TBVector p3);


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SHADOW LIGHTS

									Lightsources used to cast true character shadows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


extern bool		shadowLightDebug;

// PP: types of shadow lights
enum EShadowLightType
{
	SHADOWLIGHTTYPE_POINT,
	SHADOWLIGHTTYPE_DIRECTIONAL,
};

// PP: shadow blend modes - used this seperate enum (rather than BDALPHABLENDMODE_) because I might want to add other wacky blend modes, y'never know
enum EShadowBlendMode
{
	SHADOWBLEND_SUBTRACTIVE,
	SHADOWBLEND_BLEND
};


enum EShadowLightFlag
{
	// PP: ~~~~~~ SHADOW FLAGS ~~~~~~

	// PP: **** If you add any shadow flags, be sure to add their names to the SHADOWLIGHT::shadowFlagStrings array (Shadow.cpp) ****

	// PP: NOTE: these 'costs' are approximate guides
	// PP: NOTE: 'per poly' means for every poly that has passed the shadow bounds test.  So it doesn't mean for every poly in the collision cache!!

	// PP: cost: 3 dot products per poly
	// PP: most proper wrapping-shadow lights will need a bogus test, even on 'enemies' - but clear it when you can
	SHADOWLIGHTFLAG_BOGUS_TEST		=0,			// PP: reject polys that would bear bogus shadows (those on the near side of the occluder and behind the projector), by way of line-plane intersection tests

	// PP: cost: 1 dot product per poly, shared with SHADOWLIGHTFLAG_BACKFACE_CULL
	SHADOWLIGHTFLAG_ANGULAR_FALLOFF,			// PP: apply backface culling and per-poly proportional angular falloff of 90° - gives nice results, but only if the pre-rendered lighting matches

	// PP: cost: 1 dot product per poly, shared with SHADOWLIGHTFLAG_ANGULAR_FALLOFF
	SHADOWLIGHTFLAG_BACKFACE_CULL,				// PP: apply backface culling at 90°

	// PP: cost: cheaper by 1 square root per poly than a normal point light
	SHADOWLIGHTFLAG_PARALLEL_POINT,				// PP: don't bother with any ray divergence on the point light

	// PP: cost: cheaper by hods - use this flag if your shadows are transforming too many verts, or if you know you don't want to cast onto objects
	SHADOWLIGHTFLAG_LANDSCAPE_ONLY,				// PP: only cast shadows onto the landscape, not onto any objects

	// PP: cost: cheaper by hods - use this flag if your shadows are transforming too many verts, or if you know you don't want to cast up walls etc.
	// PP: NOT IMPLEMENTED YET
	SHADOWLIGHTFLAG_BELOW_ONLY,					// PP: only cast shadows onto polys at the character's foot level and below

	// PP: cost: cheaper by hods - use this flag if you don't need, or don't want, the shadows to wrap up walls etc.
	// PP: Completely bypasses vert rotations.
	// PP: This flag implies the behaviour of SHADOWLIGHTFLAG_BELOW_ONLY and SHADOWLIGHTFLAG_UPWARDS_FACING_ONLY, but not SHADOWLIGHTFLAG_PARALLEL_POINT.
	// PP: NOT IMPLEMENTED YET
	SHADOWLIGHT_Y_PROJECTION_ONLY,				// PP: project the shadow map straight down the Y axis

	NUM_SHADOWLIGHT_SHADOW_FLAGS,				// PP: NUMBER OF SHADOWLIGHT SHADOW FLAGS

	// PP: ~~~~~~ EDITOR FLAGS ~~~~~~

	SHADOWLIGHTFLAG_SELECTED		=16,		// PP: light is selected
	SHADOWLIGHTFLAG_TARGETTED,					// PP: light is targeted, ie. it will become the current selection if the artist presses X

	FORCE_32BIT						=(1<<31)	// PP: force 32-bit size
};

// PP: type for storing combinations of EShadowLightFlag values
typedef uint32		ShadowLightFlags;

// PP: macro to get the combinable value of a shadow-light flag
#define SHADOWLIGHT_FLAG(_flag)					(1<<(_flag))

// PP: default flags with which a light is created if no flags are specified...

#define SHADOWLIGHT_DEFAULT_PLAYER_FLAGS \
										(SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_LANDSCAPE_ONLY) \
										|SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_BOGUS_TEST) \
										|SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_BACKFACE_CULL))

#define SHADOWLIGHT_DEFAULT_ENEMY_FLAGS	\
										(SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_LANDSCAPE_ONLY) \
										|SHADOWLIGHT_FLAG(SHADOWLIGHTFLAG_BOGUS_TEST))

#define SHADOWLIGHT_DEFAULT_EDITOR_FLAGS			0

#define SHADOWLIGHT_INFINITE_RANGE					0
													
#define SHADOWLIGHT_DEFAULT_TYPE					SHADOWLIGHTTYPE_POINT
#define SHADOWLIGHT_DEFAULT_RANGE					SHADOWLIGHT_INFINITE_RANGE
#define SHADOWLIGHT_DEFAULT_COLOUR					COLOUR(190, 190, 190)			// PP: rgb in range 0..255, alpha not used
#define SHADOWLIGHT_DEFAULT_SHADOW_DARKNESS			0.25f
#define SHADOWLIGHT_DEFAULT_ANGLE					ZVEC
#define SHADOWLIGHT_DEFAULT_CAST_RANGE				1.0f
#define SHADOWLIGHT_DEFAULT_SOFTNESS				0.0f							// PP: NOTE: not used yet
#define SHADOWLIGHT_DEFAULT_BLEND					SHADOWBLEND_SUBTRACTIVE			// PP: real shadows are subtractive

// PP: distance used to represent infinite distance (for rejection of near-side polys that would bear bogus shadows)
#define SHADOWLIGHT_INFINITE_DISTANCE				250000.0f


// PP: WILL PROBABLY CHANGE: lightsource type for the purpose of casting true character shadows
typedef struct TAG_SHADOWLIGHT
{
	// PP: array of description strings for the shadow flags
	static const char* const shadowFlagStrings[NUM_SHADOWLIGHT_SHADOW_FLAGS];

	EShadowLightType	type;						// PP: light type (SHADOWLIGHTTYPE_...)
	ShadowLightFlags	playerFlags;				// PP: shadow flags (comibination of SHADOWLIGHTFLAG_.... values) used when this light casts a shadow of the player
	ShadowLightFlags	enemyFlags;					// PP: shadow flags (comibination of SHADOWLIGHTFLAG_.... values) used when this light casts a shadow of a non-player character
	ShadowLightFlags	editorFlags;				// PP: editor flags (comibination of SHADOWLIGHTFLAG_.... values), eg. to flag the light as selected or whatever
													
	VEC					pos;						// PP: will be used even by directional lights, for debug-draw
	VEC					ang;						// PP: angle of light
													
	float				range;						// PP: falloff distance
	float				castRange;					// PP: number of times the occluder radius that a shadow from this light can cast over - purely an optimisation thang - should be kept pretty low except in special cases where you know the shadows have to go a long way (eg. zoo boss sunset)
													
	COLOUR				col;						// PP: light colour, hence shadow shadow colour
	float				shadowDarkness;				// PP: 0..1 shadow darkness (1-ambience)
													
	float				softness;					// PP: 0 (as sharp as possible) upwards	// PP: not used yet

	EShadowBlendMode	blend;						// PP: blend mode used by the light's shadows (subtractive by default)

	// PP: gets calculated within VFX_generateShadowMap
	VEC					dir;						// PP: Normalised only when it needs to be	// PP: light direction vector - I'm boiling down point lights into character-facing directional lights as well


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::setFlag
		Purpose 	: set one of the light's flags either high or low
		Parameters 	: ref to flag group, flag to set, (opt/true)state to which to set flag
		Returns 	: 
		Info 		: 
	*/
	static inline void setFlag(ShadowLightFlags& flags, const EShadowLightFlag flag, const bool state=true);


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::flagSet
		Purpose 	: check if a flag is set
		Parameters 	: ref to flag group, flag to test
		Returns 	: non-zero if the flag is high, zero otherwise
		Info 		: 
	*/
	static inline ShadowLightFlags flagSet(ShadowLightFlags& flags, const EShadowLightFlag flag)
	{
		return (flags&SHADOWLIGHT_FLAG(flag));
	}


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::draw
		Purpose 	: debug-draw the shadow light
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void draw(void);


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::infinite
		Purpose 	: determine if the shadow light has infinite range
		Parameters 	: 
		Returns 	: whether or not the shadow light has infinite range
		Info 		: 
	*/
	inline bool infinite(void) const
	{
		return (this->range == SHADOWLIGHT_INFINITE_RANGE);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::getShadowFlags
		Purpose 	: get the appropriate shadow flags for a specified occluder
		Parameters 	: ptr to accluder ACTORINSTANCE
		Returns 	: the appropriate shadow flags for the specified occluder
		Info 		: 
	*/
	inline ShadowLightFlags getShadowFlags(const ACTORINSTANCE* const actorInstance) const
	{
		if(actorInstance == gameStatus.player1)
		{
			return this->playerFlags;
		}
		else
		{
			return this->enemyFlags;
		}
	}


	// PP: EDITOR-SPECIFIC METHODS FOLLOW...


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::select
		Purpose 	: select or deselect the light
		Parameters 	: (opt/true)true to select, false to deselect
		Returns 	: this
		Info 		: 
	*/
	inline struct TAG_SHADOWLIGHT* select(const bool state=true);


	/*	--------------------------------------------------------------------------------
		Function 	: SHADOWLIGHT::target
		Purpose 	: target or untarget the light
		Parameters 	: (opt/true)true to target, false to untarget
		Returns 	: this
		Info 		: you target a light by pointing the camera at it, and select it by pressing X
	*/
	inline struct TAG_SHADOWLIGHT* target(const bool state=true);


}SHADOWLIGHT, _SHADOWLIGHT BALIGN16;

// PP: maximum allowable number of shadow lights in a level
#define	MAX_SHADOW_LIGHTS						10// PP: ?

#ifdef PHIL
#ifndef INFOGRAMES
// PP: ********* Enables shadow-light debugging
#define SHADOWLIGHT_DEBUG
// PP: *********
#endif// PP: ndef INFOGRAMES
#endif// PP: def PHIL


// PP: loop-array of all shadow lights in the level
extern LOOPARRAY<SHADOWLIGHT, MAX_SHADOW_LIGHTS>		shadowLights;


/*	--------------------------------------------------------------------------------
	Function 	: initShadowLights
	Purpose 	: initialise shadow lights before loading a scene
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void initShadowLights(void);


/*	--------------------------------------------------------------------------------
	Function 	: setShadowLightDebug
	Purpose 	: turn shadow light debugging on/off
	Parameters 	: true=on, false=off
	Returns 	: 
	Info 		: 
*/
void setShadowLightDebug(const bool state);


/*	--------------------------------------------------------------------------------
	Function 	: toggleShadowLightDebug
	Purpose 	: toggle shadow light debugging
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void toggleShadowLightDebug(void);


/*	--------------------------------------------------------------------------------
	Function 	: addShadowLight
	Purpose 	: Add a shadow light to the level
	Parameters 	: ref to pos VEC, (opt/point)type(SHADOWLIGHTTYPE_), (opt/0,0,0)ref to ang VEC,
					(opt/grey)ref to COLOUR, (opt/default)shadow darkness 0..1,
					(opt/infinite)range, (opt/default)player flags, (opt/default)enemy flags,
					(opt/default)cast range, (opt/default)softness, (opt/subtractive)blend type (SHADOWBLEND_)
	Returns 	: 
	Info 		: 
*/
SHADOWLIGHT* addShadowLight(const VEC&				pos,
							const EShadowLightType	type			=SHADOWLIGHT_DEFAULT_TYPE,
							const VEC&				ang				=SHADOWLIGHT_DEFAULT_ANGLE,
							const COLOUR&			col				=SHADOWLIGHT_DEFAULT_COLOUR,
							const float				shadowDarkness	=SHADOWLIGHT_DEFAULT_SHADOW_DARKNESS,
							const float				range			=SHADOWLIGHT_DEFAULT_RANGE,
							const int				playerFlags		=SHADOWLIGHT_DEFAULT_PLAYER_FLAGS,
							const int				enemyFlags		=SHADOWLIGHT_DEFAULT_ENEMY_FLAGS,
							const float				castRange		=SHADOWLIGHT_DEFAULT_CAST_RANGE,
							const float				softness		=SHADOWLIGHT_DEFAULT_SOFTNESS,
							const EShadowBlendMode	blend			=SHADOWLIGHT_DEFAULT_BLEND);


/*	--------------------------------------------------------------------------------
	Function 	: drawShadowLights
	Purpose 	: debug-draw all shadow lights in the level
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void drawShadowLights(void);


/*	--------------------------------------------------------------------------------
	Function 	: pickShadowLight
	Purpose 	: pick a lightsource based on the camera position & orientation
	Parameters 	: 
	Returns 	: 
	Info 		: uses flyCam
*/
SHADOWLIGHT* pickShadowLight(void);


/*	--------------------------------------------------------------------------------
	Function 	: targetShadowLight
	Purpose 	: exclusively target the specified light
	Parameters 	: ptr to light to target
	Returns 	: 
	Info 		: 
*/
void targetShadowLight(SHADOWLIGHT* const light);


/*	--------------------------------------------------------------------------------
	Function 	: selectShadowLight
	Purpose 	: exclusively select the specified light
	Parameters 	: ptr to light to select
	Returns 	: 
	Info 		: 
*/
void selectShadowLight(SHADOWLIGHT* const light);


/*	--------------------------------------------------------------------------------
	Function 	: updateShadowLights
	Purpose 	: update shadow light stuff
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void updateShadowLights(void);


/*	--------------------------------------------------------------------------------
	Function 	: getShadowLightForCharacter
	Purpose 	: TEMP
	Parameters 	: TEMP
	Returns 	: TEMP
	Info 		: TEMP
*/
SHADOWLIGHT* getShadowLightForCharacter(const ACTORINSTANCE* const actorInstance);


// PP: _____________________
#endif// PP: ndef __H_SHADOW