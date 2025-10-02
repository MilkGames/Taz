// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : floorpolisher.cpp
//   Purpose : floorpolisher functions
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "physics.h"
#include "scene.h"
#include "status.h"
#include "vehicles.h"
#include "attach.h"
#include "animation.h"
#include "maths.h"
#include "main.h"
#include "control.h"
#include "characters.h"
#include "Colour.h"						// PP: handy colour struct
#include "vehicles.h"					
#include "sfx.h"						
#include "genericai.h"					
#include "Util.h"						// PP: general utility functions
#include "display.h"					// PP: provide basic display setup and managment functions
#include "floorpolisherwantedposter.h"	// PP: the longest filename in the game?


// PP: initialise static floor polisher members...

TBTexture*				POLISHERINFO::trailReflectionTexture	=NULL;
TBTexture*				POLISHERINFO::trailDustTexture			=NULL;
TBTexture*				POLISHERINFO::trailZMaskTexture			=NULL;
TBTexture*				POLISHERINFO::trailZTemplateTexture		=NULL;
TBActorInstance*		POLISHERINFO::worldZMask				=NULL;
float					POLISHERINFO::minGroundClearance		=0.0f;

// PP: trail colour
COLOUR POLISHERINFO::colour=COLOUR(32, 32, 32, 128);

// PP: star texture
TBTexture* FPTRAILSTAR::texture=NULL;
						
static TBPrimVertex		FPTrail_contigVerts[FPTRAIL_NUM_VERTS];	// PP: contiguous buffer of trail verts

//float	this->sectionYang;

// PP: define FPTRAIL_DEGENERATE_QUADS to use degenerate quads between trail parts (shouldn't need this soon)
// PP: #define FPTRAIL_DEGENERATE_QUADS

// PP: define FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION to force the trail section at the very tail of the trail
// PP: to always be flagged as 'trailing edge'.
// PP: This prevented a funny disappearing thang on gamecube.
// PP: Hopefully it shouldn't be needed now, the drawing of the trail is so vastly different (and nicer) now.
#if(BPLATFORM == GAMECUBE)
// PP: #define FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION
#endif// PP: (BPLATFORM == GAMECUBE)

//#ifndef INFOGRAMES// PP: just to be safe
#ifdef PHIL
//#define FPTRAIL_DEBUG// PP: debug the floor-polisher trail effect
#endif// PP: def PHIL
//#endif// PP: ndef INFORGRAMES

// PP: define FPTRAIL_TIMERS to time polisher trail stuff
//#define FPTRAIL_TIMERS

// PP: MOVE ME!!!!

// PP: macro to copy the position of one TBPrimVertex* to another
#define COPY_VERT_POS(_dest, _src) \
		{ \
			TBVector	_tempVec; \
			\
			BDVERTEX_GETPOS(_src, _tempVec); \
			BDVERTEX_SETPOS(_dest, _tempVec); \
		}

// PP: macro to raise or lower a TBPrimVertex*'s position
#define ADJUST_VERT_YPOS(_vert, _yAdjust) \
		{ \
			float	_y; \
			\
			BDVERTEX_GETY(_vert, _y); \
			BDVERTEX_SETY(_vert, _y+(_yAdjust)); \
		}


#define FPTRAIL_Y_ADJUST						0.0f				// PP: OOD?
#define FPTRAIL_RADIUS							70.0f				// PP: buffing radius of the polisher, hence half the width of the trail
#define FPTRAIL_SECTION_DISTANCE				30					// PP: minimum distance between trail section centres (section = quad)
#define FPTRAIL_TAIL_LENGTH						METERTOUNIT(2.5f)	// PP: length of the tail of the trail (the converging end bit)
#define FPTRAIL_MAX_LENGTH						(FPTRAIL_SECTION_DISTANCE*FPTRAIL_NUM_VERTS/4)// PP: maximum total length of a polisher trail// PP: no more accurate than it has to be


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													POLISHERINFO methods

								Info about a floor polisher vehicle - so far just trail data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: base z-bias at which the trail's complete z-template is drawn to the z-buffer
#define FPTRAIL_ZTEMPLATE_ZBIAS			0

// PP: base z-bias at which the trail's lower (dark) layer is drawn
#if(BPLATFORM == PS2)
		static int lowertwico= -10;
#elif BPLATFORM == GAMECUBE
		static int lowertwico = -1.0f;
#else // PP: xbox, pc
		static float lowertwico= -0.1f;
#endif// PP: platform
#define FPTRAIL_LOWER_LAYER_ZBIAS		lowertwico

// PP: z-bias seperating the bottom (dark) layer from the upper (light) layer
#if(BPLATFORM == PS2)
		static int uppertwico= -20;
#elif BPLATFORM == GAMECUBE
		static int uppertwico = -2.0f;
#else // PP: xbox, pc
		static float uppertwico= -0.2f;
#endif// PP: platform
#define FPTRAIL_UPPER_LAYER_ZBIAS		uppertwico

// PP: z-bias seperating the rounded-edge zmasks from the bottom (dark) layer
#if(BPLATFORM == PS2)
		static int masktwico= -30;
#elif BPLATFORM==GAMECUBE
		static int masktwico = -3.0f;
#else// PP: xbox, pc
		static float masktwico= -0.3f;
#endif// PP: platform
#define FPTRAIL_ZMASK_ZBIAS				masktwico

// PP: zbias gap between each trail 'part'
// PP: this stops rounded edges fighting with each other
#if(BPLATFORM == PS2)	
	static float	zjump=(-30.0f);
#else
	static float	zjump=(-0.3f);
#endif
#define FPTRAIL_INTER_PART_ZBIAS_STEP		zjump

// PP: z-bias with which twinkles are drawn
#if BPLATFORM == PS2
	// PP: twico
	static int starzbias= -1e+08;// PP: See what I mean?  JEES!!
#elif BPLATFORM==GAMECUBE
	static float starzbias= -3.0f;// IH: Tweaked.
#else
	static float starzbias= -8.0f;
#endif
#define FPTRAIL_STAR_ZBIAS			starzbias

// PP: twico for FPTRAIL_Y_OFFSET_STEP (see floorpolisher.h)
// PP: keeping this as low as possible hides trail overlaps; makes the whole effect look flat and consistent.
// PP: too low though, and you'll see some z-fighting on the bends of the trail
float				yOffsetStepTwico=0.1f;// PP: 0.1f is perfect on xbox & ps2


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::init(void)
{
	this->visibility=FPTRAILVIS_NO;	
	this->sectionProgress=0.0f;
	this->starTimer=0.0f;
	this->rider=NULL;
	this->lastOnGround=false;
	this->lastSectionPos.zero();
	this->sectionBeforeLastPos.zero();
	this->oldSectionYang=0.0f;
	this->oldXZOffset[X]=0.0f;
	this->oldXZOffset[Y]=0.0f;
	this->lastUpdatePos.zero();

	// PP: set looparray names if applicable
	this->trailVerts.init("trailVerts");
	this->trailStars.init("trailStars");

	// PP: colour all the verts...

	LA_FORARRAY(this->trailVerts, TBPrimVertex, vert);

		BDVERTEX_SETCOLOUR(vert, POLISHERINFO::colour);

	LA_ENDFORARRAY(this->trailVerts);
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::visible
	Purpose 	: determine whether or not a polisher trail is visible
	Parameters 	: (opt/false) re-test visibility (otherwise just use last result),
					(opt/true) whether visibility on ANY screen counts
	Returns 	: whether or not the polisher trail is visible
	Info 		: 
*/

#define FPTRAIL_FADEOUT_DISTANCE		METERTOUNIT(10)

bool POLISHERINFO::visible(const bool test, const bool anyScreen)
{
	// PP: *** NOTE: THIS ASSUMES THAT THE NUMBER OF SCREENS WON'T CHANGE MID-LEVEL!! ***

	if(test && (videoMode.drawNum == 0))// PP: don't test visibility on second draw pass; no need
	{
		float	dist;

		if(this->trailVerts.size == 0)
		{
			this->visibility &= ~(1<<gameStatus.multiplayer.currentScreen);
			goto rtn;
		}

		dist=this->trailBoundingBox.minDist(gameStatus.viewPoint);

		if(dist > FPTRAIL_FADEOUT_DISTANCE)
		{
			// PP: too far away
			this->visibility &= ~(1<<gameStatus.multiplayer.currentScreen);
			goto rtn;
		}

		if(!this->trailBoundingBox.visible())// PP: NOTE: the bounding box visibility isn't as clever as this trail visibility is - it has to re-test every time
		{
			this->visibility &= ~(1<<gameStatus.multiplayer.currentScreen);
			goto rtn;
		}

		this->visibility |= (1<<gameStatus.multiplayer.currentScreen);
	}

rtn:

	if(anyScreen)
	{
		return (this->visibility != FPTRAILVIS_NO);
	}
	else
	{
		return ((this->visibility & (1<<gameStatus.multiplayer.currentScreen)) != FPTRAILVIS_NO);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::updateBoundingBox
	Purpose 	: update the bounding box of the polisher trail effect
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::updateBoundingBox(void)
{
	// PP: UPDATE BOUNDING BOX OF EFFECT...

#ifdef FPTRAIL_TIMERS
	STARTTIMER;
#endif// PP: def FPTRAIL_TIMERS

	if(this->trailVerts.size != 0)// PP: don't worry, BB isn't used if there are no trail verts
	{
		VEC		vertPos;

		this->trailBoundingBox.init();

		LA_FOREACH(this->trailVerts, TBPrimVertex, vert);

			BDVERTEX_GETVEC(vert, vertPos);

			this->trailBoundingBox.expandToContain(vertPos);

		LA_ENDFOREACH(this->trailVerts);
	}

	this->boundingBoxTimer=0.0f;

#ifdef FPTRAIL_TIMERS
	ENDTIMER("POLISHERINFO::updateBoundingBox");
#endif// PP: def FPTRAIL_TIMERS
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::updateTrailTail
	Purpose 	: make the polisher trail 'tail-off' by getting thinner at the trailing end
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::updateTrailTail(void)
{
	// PP: *** TRAIL GETS THINNER TOWARDS THE END... ***

 	TBVector		loopVertPos;
 	TBPrimVertex*	partnerVert;
 	TBVector		partnerVertPos;
 	TBVector		midPos;
 	TBVector		distVec;
 	float			dist;
 	TBVector		tempVec;
	int				v=0;
 	float			fraction;
	COLOUR			col;
	float			step;
	float			offset;

	// PP: 'step' is the fraction of the tail formed by each section
	step=1/(FPTRAIL_TAIL_LENGTH/float(FPTRAIL_SECTION_DISTANCE));

	// PP: la la la can't really explain this dooby doo cha cha
	if(this->trailVerts.size == FPTRAIL_NUM_VERTS)
	{
		offset=this->sectionProgress;
	}
	else
	{
		offset=0.0f;
	}

	// PP: NOTE: LA_FOREACH starts at the end of the trail
	LA_FOREACH(this->trailVerts, TBPrimVertex, loopVert)

		// PP: get the distance of this section from the end of the trail
		fraction= (((v/2)-1)-offset)*step;// PP: was -1 + offset
		fraction=CONSTRAIN(fraction, 0.0f, 1.0f);

		BDVERTEX_GETPOS(loopVert, loopVertPos);

		if(v&1)
		{
			if(dist == 0.0f)
			{
				bmVectorCopy(loopVertPos, midPos);
				// IH: This was causing me grief
#ifdef FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION
				POLISHERINFO::flagVert(loopVert, FPTRAILVERT_TRAILINGEDGE);
#endif// PP: def FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION
			}
			else
			{
				bmVectorAdd(loopVertPos, midPos, tempVec);
			}
		}
		else
		{
			partnerVert=LA_FOREACH_NEXT(this->trailVerts);

			BDVERTEX_GETPOS(partnerVert, partnerVertPos);
			
			bmVectorSub(distVec, partnerVertPos, loopVertPos);

			bmVectorLerp(midPos, loopVertPos, partnerVertPos, 0.5f);

			dist=bmVectorLen(distVec);

			if(dist == 0.0f)
			{
				bmVectorCopy(loopVertPos, midPos);
				// IH: This was causing me grief
#ifdef FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION
				POLISHERINFO::flagVert(loopVert, FPTRAILVERT_TRAILINGEDGE);
#endif// PP: def FPTRAIL_FORCE_TRAILINGEDGE_TAILSECTION
			}
			else
			{
				bmVectorScaleToLength(tempVec, distVec, fraction*FPTRAIL_RADIUS);

				bmVectorSub(loopVertPos, midPos, tempVec);
			}
		}

		BDVERTEX_SETPOS(loopVert, loopVertPos);

		v++;

	LA_ENDFOREACH(this->trailVerts);
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::getLength
	Purpose 	: get the total length of the polisher trail
	Parameters 	: 
	Returns 	: the total length of the polisher trail
	Info 		: 
*/
float POLISHERINFO::getLength(void) const
{
	return ((this->trailVerts.size-2)/2.0f)*FPTRAIL_SECTION_DISTANCE;// PP: see how that works?  It's not a an accurate result in practice, but it doesn't need to be.
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENERAL FLOOR-POLISHER FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* --------------------------------------------------------------------------------
   Function		: InitFloorPolishers
   Purpose		: initialise stuff for any floor polishers
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void InitFloorPolishers(void)
{
	FPTRAILSTAR::texture=bkLoadTexture(NULL, FPTRAILSTAR_TEXTURE, 0);
	POLISHERINFO::trailDustTexture=bkLoadTexture(NULL, FPTRAIL_DUST_TEXTURE, 0);
	POLISHERINFO::trailReflectionTexture=bkLoadTexture(NULL, FPTRAIL_REFLECTION_TEXTURE, 0);
	POLISHERINFO::trailZMaskTexture=bkLoadTexture(NULL,FPTRAIL_ZMASK_TEXTURE,0);
	POLISHERINFO::trailZTemplateTexture=bkLoadTexture(NULL,FPTRAIL_ZTEMPLATE_TEXTURE,0);

	// PP: PREPARE WORLD Z-MASK...

	char			worldZMaskName[128];
	TBActor*		actor=NULL;

	if(map.sceneNumber == SCENE_MUSEUMVEHICLERACE)
	{
		strcpy(worldZMaskName, "extras\\polishertrailzmask.obe");
	}
	else
	{
		worldZMaskName[0]='\0';
	}

	if(worldZMaskName[0] == '\0')
	{
		POLISHERINFO::worldZMask=NULL;
	}
	else// PP: zmask specified, cool
	{
		LoadActor(worldZMaskName,map.mapName,0);

		actor=FindActorInActorList(worldZMaskName);

		POLISHERINFO::worldZMask=baInitActorInstance(NULL, actor, 0);

		// PP: must prep before first draw
		baPrepActorInstance(POLISHERINFO::worldZMask,BACTORPREPFLAG_PREPNODES);
	}

	// PP: SET MINIMUM GROUND CLEARANCE...

	if(map.sceneNumber == SCENE_MUSEUM)
	{
		// PP: gotter clear that floor poster
		POLISHERINFO::minGroundClearance=FLOORPOLISHERWANTEDPOSTER_DECAL_Y_OFFSET;
	}
	else
	{
		// PP: nothing we need to clear; start right on the floor
		POLISHERINFO::minGroundClearance=0.0f;
	}
}


/* --------------------------------------------------------------------------------
   Function		: ShutdownFloorPolishers
   Purpose		: Shutdown stuff for any floor polishers
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void ShutdownFloorPolishers(void)
{
	// PP: clear world zmask object ptr
	if(POLISHERINFO::worldZMask != NULL)
	{
		baFreeActorInstance(POLISHERINFO::worldZMask);
		POLISHERINFO::worldZMask=NULL;
	}
}


/* --------------------------------------------------------------------------------
   Function		: CreateFloorPolisherInfo
   Purpose		: creates a floor polisher info struct
   Parameters	: 
   Returns		: ptr to new floor polisher info struct, or NULL if creation failed
   Info			: // PP: 
*/

POLISHERINFO *CreateFloorPolisherInfo(void)
{
	POLISHERINFO		*tempInfo;

	if(tempInfo = new POLISHERINFO())
	{
		return tempInfo;
	}
	else
	{
		//bkPrintf("   *** WARNING ***, could not create POLISHERINFO structure\n");
		return NULL;
	}
}


/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisher
   Purpose : Creates and initialises actorInstances that act as floor polishers
   Parameters : 
   Returns : 
   Info : 
*/

void CreateFloorPolisher(ACTORINSTANCE *polisher)
{
	AddActorToCollisionCache(&collisionCache, polisher, CACHEFLAGS_LIVEUPDATE);
	SetCollisionCallback(polisher, RespondToFloorPolisherCollision);

	polisher->vehicleInfo->polisherInfo=CreateFloorPolisherInfo();
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateFloorPolisher
	Purpose 	: update anything to do with the floor polisher
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: 
*/
void UpdateFloorPolisher(ACTORINSTANCE *const polisher)
{
#ifdef FPTRAIL_TIMERS
	STARTTIMER;
#endif// PP: def FPTRAIL_TIMERS

	POLISHERINFO*		info=polisher->vehicleInfo->polisherInfo;		// PP: pointer straight to the polisher's info
	VEC					pos;											// PP: polisher position
	TBVector2D			XZoffset;										// PP: X & Z offsets of one of the edges of the leading trail section
	bool				onGround;										// PP: is the polisher on the ground?
	bool				addedSection=false;								// PP: did we add a section this update?
	VEC					newSectionPos;									// PP: position for new trail section
	bool				forceBoundingBoxUpdate=false;					// PP: should we force a bounding box recalc?
	uint32				oldNumVerts;

	// PP: Get the real position of the polisher...

	if(info->rider == NULL)
	{
		// PP: no rider, use the polisher's map position instead
		pos=polisher->actorInstance.position;
	}
	else
	{
		// PP: use rider's pos
		pos=info->rider->actorInstance.position;
	}

	if(polisher->vehicleInfo->state == VEHICLE_STATE_ON)// PP: only do this is the polisher is switched on & therefore moving
	{
		oldNumVerts=info->trailVerts.size;

		// PP: find out if the polisher is on terra firma

		if(!(onGround=(info->rider == NULL)))
		{
			float	yValue;
			VEC		ceiling;

			static float hopceil=10.0f;// PP: twico
#define FPTRAIL_HOP_CEILING_ADJUST			hopceil				// PP: distance by which HOP ceiling is raised

			// PP: keeping this lowish should prevent problems when rebounding from a wall
			// PP: however, there are various bits of unexpectedly uneven ground in the museum which will
			// PP: cause a problem if too low a value is used.
			// PP: eg: entrance to floor poster room
			// PP: eg: target floor decals at bottom of stairs with all the falling objects
			static float minoff=20.0f;// PP: 20// PP: twico
#define FPTRAIL_HOP_MIN_OFFGROUND_HEIGHT	minoff				// PP: minimum height above ground that is classed as being off the ground

			(ceiling=pos)[Y]+=FPTRAIL_HOP_CEILING_ADJUST;

#ifdef FPTRAIL_TIMERS
			STARTTIMER;
#endif// PP: def FPTRAIL_TIMERS

			if(QuickHeightOfPoint(ceiling, info->rider->actorShadow, &yValue))// PP: this uses Taz's shadow as the geometry to test the height of his position; a normal GetHeightOfPoint would take way too long
			{
				onGround =((ceiling[Y]-yValue-FPTRAIL_HOP_CEILING_ADJUST)<=FPTRAIL_HOP_MIN_OFFGROUND_HEIGHT);

				// PP: NEW
				pos[Y]=yValue;
			}
			else
			{
				onGround=false;
			}

#ifdef FPTRAIL_TIMERS
			ENDTIMER("FPTRAIL HOP");
#endif// PP: def FPTRAIL_TIMERS
		}

		// PP: get y angle of section...
		
		if(onGround)
		{
			if(info->lastOnGround)
			{
				info->sectionYang=bmATan2Approx(info->lastSectionPos[Z]-pos[Z], info->lastSectionPos[X]-pos[X]);
			}
			else
			{
				// PP: gotter use velocity angle
				if(info->rider != NULL)
				{
					info->sectionYang=PI+bmATan2Approx(info->rider->actorBehaviour->physics.velocity[Z], info->rider->actorBehaviour->physics.velocity[X]);
				}
			}

			if(info->lastOnGround)
			{
				// PP: work out how far through the current section we are (ie. how close we are to laying a new section)
				info->sectionProgress=(pos.distance(info->sectionBeforeLastPos) / FPTRAIL_SECTION_DISTANCE);

				if(info->sectionProgress > 1.0f)
				{
					// PP: create new sectionBeforeLastPos from lastSectionPos
					info->sectionBeforeLastPos=info->lastSectionPos;

					// PP: create new leading section (2 verts)...
					info->trailVerts.getNew(2);

					info->sectionProgress=0.0f;
					addedSection=true;
				}
			}
			else
			{
				// PP: polisher has just landed: must also finish degenerate quad and start edge quad

#ifdef FPTRAIL_DEGENERATE_QUADS
				info->trailVerts.getNew(6);
#else// PP: ifndef FPTRAIL_DEGENERATE_QUADS
				info->trailVerts.getNew(4);
#endif// PP: ndef FPTRAIL_DEGENERATE_QUADS

				info->sectionProgress=0.0f;
				addedSection=true;
			}
		}
		else// PP: not on ground
		{
			if(info->lastOnGround)
			{
#ifdef FPTRAIL_DEGENERATE_QUADS	
				// PP: start the degenerate quad
				info->trailVerts.getNew(2);
#endif// PP: def FPTRAIL_DEGENERATE_QUADS

				info->sectionProgress=0.0f;
				addedSection=true;
			}
		}

		// PP: reposition the youngest verts to be right under the polisher every frame...

		if(info->trailVerts.size >= 2)
		{
			TBPrimVertex*	vert;
			
			if(info->rider != NULL)// PP: just leave the trail be if no-one's riding the polisher - should be real cute, especially if it does end up reflecting taz
			{
				if(onGround || addedSection)
				{
					int		youngestOffset;				// PP: offset from youngest element

					if(onGround)
					{
						newSectionPos=pos;
						newSectionPos[Y]+=FPTRAIL_Y_ADJUST;

						if(!info->lastOnGround)// PP: landing
						{
							// PP: shift pos along to the back edge of the polisher
							newSectionPos+=(VEC(0,0,FPTRAIL_RADIUS).rotateY(HALFPI-Aabs(info->sectionYang)));
						}
					}
					else
					{
						if(info->trailVerts.size == 2)
						{
							// PP: use polisher position
							newSectionPos=pos;
						}
						else
						{
							// PP: only just left the ground - use last update's pos which was still on the ground
							newSectionPos=info->sectionBeforeLastPos;// PP: info->lastUpdatePos;
						}

						// PP: but shift that pos along to the front edge of the polisher
						newSectionPos+=(VEC(0,0,FPTRAIL_RADIUS).rotateY(PI+HALFPI-Aabs(info->oldSectionYang/*info->sectionYang*/)));
					}

					// PP: calculate X & Z offsets of newest section verts
					XZoffset[X]=FPTRAIL_RADIUS*bmCosApprox(info->sectionYang-HALFPI);
					XZoffset[Y]=FPTRAIL_RADIUS*bmSinApprox(info->sectionYang-HALFPI);

					if(onGround)
					{
						if(info->lastOnGround)
						{
							// PP: on ground as usual
							youngestOffset=1;
						}
						else
						{
#ifdef FPTRAIL_DEGENERATE_QUADS

							// PP: landing
							youngestOffset=5;

#else// PP: ifndef FPTRAIL_DEGENERATE_QUADS

							// PP: landing
							youngestOffset=3;

#endif// PP: ndef FPTRAIL_DEGENERATE_QUADS
						}
					}
					else// PP: not on ground
					{
						if(info->trailVerts.size == 2)
						{
							// PP: taking off while being switched on for first time
							youngestOffset=1;
						}
						else
						{
#ifdef FPTRAIL_DEGENERATE_QUADS

							// PP: taking off
							youngestOffset=3;

#else// PP: ifndef FPTRAIL_DEGENERATE_QUADS

							// PP: taking off
							youngestOffset=1;

#endif// PP: ndef FPTRAIL_DEGENERATE_QUADS
						}
					}

					if((!onGround)&&(info->lastOnGround))
					{
						if(info->trailVerts.size > 2)
						{
							// PP: TAKING OFF: ADD AN EDGE QUAD (ie. one that will be z-masked to create a round edge) ...

							vert=info->trailVerts.getYoungest(youngestOffset--);
							
							BDVERTEX_SETXYZW(vert, newSectionPos[X]+info->oldXZOffset[X],
													newSectionPos[Y],
													newSectionPos[Z]+info->oldXZOffset[Y],
													0.0f);

							POLISHERINFO::flagVert(vert, FPTRAILVERT_LEADINGEDGE);

							vert=info->trailVerts.getYoungest(youngestOffset--);
							
							BDVERTEX_SETXYZW(vert, newSectionPos[X]-info->oldXZOffset[X],
													newSectionPos[Y],
													newSectionPos[Z]-info->oldXZOffset[Y],
													0.0f);

							POLISHERINFO::flagVert(vert, FPTRAILVERT_LEADINGEDGE);
						}

#ifdef FPTRAIL_DEGENERATE_QUADS

						// PP: TAKING OFF: START DEGENERATE QUAD...

						vert=info->trailVerts.getYoungest(youngestOffset--);

						BDVERTEX_SETPOS(vert, newSectionPos);
						POLISHERINFO::flagVert(vert, FPTRAILVERT_DEGENERATE);

						vert=info->trailVerts.getYoungest(youngestOffset--);

						BDVERTEX_SETPOS(vert, newSectionPos);
						POLISHERINFO::flagVert(vert, FPTRAILVERT_DEGENERATE);

#endif// PP: def FPTRAIL_DEGENERATE_QUADS
					}
					else
					{
						if(onGround&&(!info->lastOnGround))
						{
#ifdef FPTRAIL_DEGENERATE_QUADS

							// PP: LANDING: FINISH DEGENERATE QUAD...

							vert=info->trailVerts.getYoungest(youngestOffset--);

							BDVERTEX_SETPOS(vert, newSectionPos);
							POLISHERINFO::flagVert(vert, FPTRAILVERT_DEGENERATE);

							vert=info->trailVerts.getYoungest(youngestOffset--);

							BDVERTEX_SETPOS(vert, newSectionPos);
							POLISHERINFO::flagVert(vert, FPTRAILVERT_DEGENERATE);

#endif// PP: def FPTRAIL_DEGENERATE_QUADS

							// PP: LANDING: START EDGE QUAD (ie. one that will be z-masked to create a round edge)...

							vert=info->trailVerts.getYoungest(youngestOffset--);

							BDVERTEX_SETXYZW(vert, newSectionPos[X]+XZoffset[X],
													newSectionPos[Y],
													newSectionPos[Z]+XZoffset[Y],
													0.0f);

							POLISHERINFO::flagVert(vert, FPTRAILVERT_TRAILINGEDGE);

							vert=info->trailVerts.getYoungest(youngestOffset--);

							BDVERTEX_SETXYZW(vert, newSectionPos[X]-XZoffset[X],
													newSectionPos[Y],
													newSectionPos[Z]-XZoffset[Y],
													0.0f);

							POLISHERINFO::flagVert(vert, FPTRAILVERT_TRAILINGEDGE);

							// PP: reset newSectionPos
							(newSectionPos=pos)[Y]+=FPTRAIL_Y_ADJUST;
						}

						// PP: DO THE NORMAL VERT PAIR...

						vert=info->trailVerts.getYoungest(youngestOffset--);

						BDVERTEX_SETXYZW(vert, newSectionPos[X]+XZoffset[X],
												newSectionPos[Y],
												newSectionPos[Z]+XZoffset[Y],
												0.0f);

						POLISHERINFO::flagVert(vert, FPTRAILVERT_NORMAL);

						vert=info->trailVerts.getYoungest(youngestOffset--);

						BDVERTEX_SETXYZW(vert, newSectionPos[X]-XZoffset[X],
												newSectionPos[Y],
												newSectionPos[Z]-XZoffset[Y],
												0.0f);

						POLISHERINFO::flagVert(vert, FPTRAILVERT_NORMAL);
					}
				}

				// PP: reposition newest section to be at polisher pos each frame
				info->lastSectionPos=pos;

				// PP: UPDATE STUFF FOR NEXT UPDATE...

				info->lastUpdatePos=newSectionPos;
				info->lastOnGround=onGround;

				if(oldNumVerts == 0)
				{
					if(addedSection)
					{
						// PP: these are the very first verts to be added to the trail - force a bounding box recalc
						forceBoundingBoxUpdate=true;
					}
				}

				if(addedSection)
				{
					info->updateBoundingBox();

					// PP: store x&z vert offsets for possible use next section on a leading edge
					// PP: (leading edges are forced to be at the same angle as the section before them)
					info->oldXZOffset[X]=XZoffset[X];
					info->oldXZOffset[Y]=XZoffset[Y];

					info->oldSectionYang=info->sectionYang;
				}

				// PP: make the trail thinner towards the end
				info->updateTrailTail();

			}// PP: end if(rider)

		}// PP: end if >= 2 verts

	}// PP: end if(polisher switched on)

	if(info->trailVerts.size >= 2)
	{
		// PP: RANDOMLY PLACE STARS...

		static float twico=130.0f;
#define FPTRAIL_STAR_INTERVAL	twico

		if(info->starTimer>(FPTRAIL_STAR_INTERVAL/info->getLength()))// PP: keep twinkle density constant for any trail length
		{

			// PP: CREATE A TWINKLE SOMEWHERE ON THE TRAIL

#define FPTRAILSTAR_PLACEMENT_RADIUS		(FPTRAIL_RADIUS*0.85f)
#define FPTRAILSTAR_Y_OFFSET				15.0f					// PP: this compensates for the trail getting higher towards the end

			int				vert1Index, vert2Index;
			VEC				vert1Pos, vert2Pos;
			TBPrimVertex*	vert;
			
			vert1Index=(bmRand()%(info->trailVerts.size/2))*2;
			vert=info->trailVerts.getItem(vert1Index);

			if(POLISHERINFO::vertFlag(vert) == FPTRAILVERT_NORMAL)// PP: don't place twinkles in degenerate or edge quads
			{
				FPTRAILSTAR*	star;
				VEC				starPos;
				int				temp;

				star=info->trailStars.getNew();

				BDVERTEX_GETVEC(vert, vert1Pos);

				temp=(info->trailVerts.size-1)-vert1Index;

				temp=bmRand()%MIN(3, temp);
				vert2Index=vert1Index+temp+1;

				BDVERTEX_GETVEC(info->trailVerts.getItem(vert2Index), vert2Pos);
				starPos=vert1Pos.lerp(vert2Pos, bmRandf());

				star->init(starPos.v);
			}
			
			info->starTimer=0.0f;// PP: if quad was degenerate, miss rather than try again
		}
	}

	if(info->visible())
	{
		// PP: update stars...

		LA_FOREACH(info->trailStars, FPTRAILSTAR, star)

			star->update();

			if(star->life < 0.0f)
			{
				info->trailStars.discardOldest();
			}

		LA_ENDFOREACH(info->trailStars)

		info->starTimer+=fTime;
		info->boundingBoxTimer+=fTime;
	}

	// PP: NOTE: can't prepareTrailPrims here cos reflections change between draws (between screens)

#ifdef FPTRAIL_TIMERS
	ENDTIMER("UpdateFloorPolisher");
#endif// PP: def FPTRAIL_TIMERS
}


/*	--------------------------------------------------------------------------------
	Function 	: UpdateFloorPolisherReflections
	Purpose 	: update reflections in a floor-polisher trail
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: must be called before each DRAW because the effect is obviously viewpoint-dependent
*/
void UpdateFloorPolisherReflections(ACTORINSTANCE *const polisher)
{

#define FPTRAIL_REFLECTION_DOME_RADIUS			METERTOUNIT(4.5f)
#define FPTRAIL_TEXTURE_SIZE					METERTOUNIT(6)		// PP: yadda

	// PP: number of times the reflection map tiles around a 90° arc
	static const float	fpTrailReflectionTiles	=(HALFPI*FPTRAIL_REFLECTION_DOME_RADIUS)/FPTRAIL_TEXTURE_SIZE;// PP: the number of radians in any given arc, times the radius of the arc, equals the length of the arc.  So THAT'S what radians are for!  And that's why they're called RADIANS!

	POLISHERINFO*		info=polisher->vehicleInfo->polisherInfo;		// PP: pointer straight to the polisher's info
						
	VEC					vertPos;
	VEC					viewPos=VEC(gameStatus.viewPoint);
	VEC					viewToVertDirNorm;		// PP: normalised direction vector from viewpoint to vert pos
	float				yAng;					// PP: y angle of viewpoint around vert
	float				dot;					// PP: dot-product of camera-to-vert and vert normal


	// PP: reflection map effect...

	/* PP: 
	======================================================================
	IF YOU CAN THINK OF ANY OPTIMISATIONS THAT COULD BE MADE TO THIS LOOP,
	PLEASE GO FOR IT!
	======================================================================
	*/

	float				uCentre;			// PP: a sheep showroom
	float				vCentre;
	float				u;					// PP: texture U coord
	float				v;					// PP: texture V coord
	float				tempFloat;

	LA_FOREACH(info->trailVerts, TBPrimVertex, vert)

		// PP: texture coords...

		BDVERTEX_GETVEC(vert, vertPos);

		// PP: calculate central tex coords...

		uCentre=vertPos[X]/FPTRAIL_TEXTURE_SIZE;
		vCentre=vertPos[Z]/FPTRAIL_TEXTURE_SIZE;

		viewToVertDirNorm=(vertPos-viewPos).normalise();

		// PP: get y angle of camera around vert
		yAng=viewToVertDirNorm.yAng();

		// PP: get dot-product of camera-to-vert and vert normal (assume vert normal 0,1,0)
		dot= (viewToVertDirNorm[Y]);// PP: -1..0

		tempFloat=(1+dot)*fpTrailReflectionTiles;

		u=uCentre+(tempFloat*bmSinApprox(yAng));
		v=vCentre+(tempFloat*bmCosApprox(yAng));

		BDVERTEX_SETU(vert, u);
		BDVERTEX_SETV(vert, v);

	LA_ENDFOREACH(info->trailVerts);
}


// PP: TEMP TEST
const COLOUR partCols[]={RED, GREEN, BLUE, YELLOW, MAGENTA};


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::drawTrailPart
	Purpose 	: draw one of the stretches of the trail
	Parameters 	: 0-based index of part to draw,
					current draw pass (FPTRAILPASS_...)
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::drawTrailPart(const uchar part, const FPTrailDrawPass drawPass)
{
	float			zBias;
	FPTRAILPART&	trailPart=this->trailParts[part];

	bool			gotEdge=false;
	TBPrimVertex	edgeVerts[4];
	uchar			edgeVertIndex=0;
	uint32			tempInt;

	// PP: ***************************
	// PP: ***** TRAIL EDGE DRAW *****
	// PP: ***************************

	if(drawPass == FPTRAILPASS_ZTEMPLATE)
	{
		bdSetZBias(FPTRAIL_ZTEMPLATE_ZBIAS+(part*FPTRAIL_INTER_PART_ZBIAS_STEP));

		// PP: set the z-template texture for any rounded edges
		bdSetTexture(0, POLISHERINFO::trailZTemplateTexture);
	}
	else if(drawPass == FPTRAILPASS_DARK)
	{
		bdSetZBias(FPTRAIL_ZMASK_ZBIAS+(part*FPTRAIL_INTER_PART_ZBIAS_STEP));

		// PP: set the z-mask texture for any rounded edges
		bdSetTexture(0, POLISHERINFO::trailZMaskTexture);
	}
	else// PP: light
	{
		// PP: no edges
		goto mainTrailPartDraw;
	}

	// PP: when debugging the trail, let's be seeing those rounded edges
#ifdef FPTRAIL_DEBUG
#define MAYBE_DISABLE_FRAMEWRITE
#else// PP: ifndef FPTRAIL_DEBUG
#define MAYBE_DISABLE_FRAMEWRITE		DISABLE_FRAMEWRITE
#endif// PP: ndef FPTRAIL_DEBUG

	// PP: prepare to draw an edge zmask/ztemplate
#define PREPARE_FOR_EDGE\
	if(drawPass != FPTRAILPASS_ZTEMPLATE)\
	{\
		gotEdge=true;\
		/* PP: only draw to the z-buffer*/\
		MAYBE_DISABLE_FRAMEWRITE;\
		/* PP: turn off filtering so's the zmask/ztemplate texture's hatching detail doesn't get buggered*/\
		SETFILTERING(POINT);\
	}

	// PP: if there's a trailing edge on this part
	if(trailPart.firstVert != trailPart.firstVertTruncated)
	{
		PREPARE_FOR_EDGE;

		for(int k=0; k<4; k++)
		{
			tempInt=(trailPart.firstVert+k);

			COPY_VERT_POS(&edgeVerts[k], &FPTrail_contigVerts[tempInt]);
			BDVERTEX_SETUV(&edgeVerts[k], float(k&1), float(k/2));
			BDVERTEX_SETA(&edgeVerts[k], 255);// PP: need some alpha to write to zbuffer

#ifdef FPTRAIL_DEBUG
			BDVERTEX_SETCOLOUR(&edgeVerts[k], MAGENTA);
#endif// PP: def FPTRAIL_DEBUG
		}

		// PP: TODO: *** FIND OUT WHASSUP WITH QUADLISTS ON PS2 ***
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, edgeVerts, 4, BVERTTYPE_SINGLE);
	}

	// PP: if there's a leading edge on this part
	if(trailPart.lastVert != trailPart.lastVertTruncated)
	{
		if(!gotEdge)
		{
			PREPARE_FOR_EDGE;
		}

		for(int k=0; k<4; k++)
		{
			// PP: tempInt=(trailPart.lastVert+1-k);
			tempInt=(trailPart.lastVert-k);

			COPY_VERT_POS(&edgeVerts[k], &FPTrail_contigVerts[tempInt]);
			BDVERTEX_SETUV(&edgeVerts[k], float(k&1), float(k/2));
			BDVERTEX_SETA(&edgeVerts[k], 255);// PP: need some alpha to write to zbuffer

#ifdef FPTRAIL_DEBUG
			BDVERTEX_SETCOLOUR(&edgeVerts[k], MAGENTA);
#endif// PP: def FPTRAIL_DEBUG
		}

		// PP: TODO: *** FIND OUT WHASSUP WITH QUADLISTS ON PS2 ***
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, edgeVerts, 4, BVERTTYPE_SINGLE);
	}

	if(gotEdge)
	{
		if(drawPass != FPTRAILPASS_ZTEMPLATE)
		{
#ifndef FPTRAIL_DEBUG
			REENABLE_FRAMEWRITE;
#endif// PP: ndef FPTRAIL_DEBUG

			RESTOREFILTERING;
		}
	}

mainTrailPartDraw:

	// PP: ***************************
	// PP: ***** MAIN TRAIL DRAW *****
	// PP: ***************************

	if(drawPass == FPTRAILPASS_ZTEMPLATE)
	{
		// PP: use no texture, in case for some crazy reason there's a speed increase to be gained by doing so
		NOTEXTURE;

		// PP: It is quite valid for the trail to have no verts in truncated form.
		// PP: However, it seems that sometimes firstVertTruncated is 
		// PP: greater than lastVertTruncated (numVertsTruncated will always be 0 in this case).
		// PP: I STRONGLY DOUBT THAT THIS IS ANYTHING TO WORRY ABOUT!
		// PP: So I can't be arsed preventing it.
		if(trailPart.numVertsTruncated > 2)
		{
			// PP: draw the trail part (truncated)
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,
							&FPTrail_contigVerts[trailPart.firstVertTruncated],
							trailPart.numVertsTruncated,
							BVERTTYPE_SINGLE);
		}
	}
	else
	{
		if(drawPass == FPTRAILPASS_DARK)
		{
			bdSetZBias(FPTRAIL_LOWER_LAYER_ZBIAS+(part*FPTRAIL_INTER_PART_ZBIAS_STEP));

			// PP: set the dust texture
			bdSetTexture(0,POLISHERINFO::trailDustTexture);
		}
		else
		{
			bdSetZBias(FPTRAIL_UPPER_LAYER_ZBIAS+(part*FPTRAIL_INTER_PART_ZBIAS_STEP));

			// PP: set the reflection texture
			bdSetTexture(0,POLISHERINFO::trailReflectionTexture);
		}

		// PP: draw the trail part (non-truncated)
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,
						&FPTrail_contigVerts[trailPart.firstVert],
						trailPart.numVerts,
						BVERTTYPE_SINGLE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::drawTrailZTemplate
	Purpose 	: draw the complete z-template of the trail
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::drawTrailZTemplate(void)
{
	// PP: only draw to the z-buffer
	DISABLE_FRAMEWRITE;

	// PP: turn off filtering so's the zmask/ztemplate texture's hatching detail doesn't get buggered
	SETFILTERING(POINT);

	for(int i=0; i<this->numTrailParts; i++)
	{
		this->drawTrailPart(i, FPTRAILPASS_ZTEMPLATE);
	}

	REENABLE_FRAMEWRITE;
	RESTOREFILTERING;
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::drawTrailParts
	Purpose 	: draw each of the 'parts' of the trail (both dark & light layers of each part)
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: a 'part' is an unbroken strech of trail
*/
void POLISHERINFO::drawTrailParts(void)
{
	for(int i=0; i<this->numTrailParts; i++)
	{
		// PP: **** REMOVE DUST ****

		// PP: This lower, 'darkening' layer removes light that we pretend was being diffusely reflected by dust.
		// PP: On platforms other than PS2, it also increases the saturation of the floor's colour, enhancing the impression of cleanliness.

		// PP: If we were to neglect this darkening layer, the effect would look exactly like a slug trail.
		// PP: It would look as if we had coated the floor with something glossy, rather than having actually *cleaned* it -
		// PP: all spit and no polish - and we couldn't have that now could we!

#if(BPLATFORM == XBOX)

		D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
		D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);

#elif(BPLATFORM == PC)

		bDisplayInfo.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		bDisplayInfo.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);

#elif(BPLATFORM == PS2)

		// PP: (removed rant)

		// PP: use subtractive on PS2 because the multiply in the blending equation can only take RGBs on one side and only ALPHAs on the other
		bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 0);

		// PP: The texture that I use on PS2 has a distinctly dusty colour to it.
		// PP: This is to give an impression of cleaning when the trail subtracts that colour off the floor.

#elif(BPLATFORM == GAMECUBE)

	//	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_SUBTRACTIVE, 0);

		GXSetBlendMode(GX_BM_BLEND,  GX_BL_DSTCLR, GX_BL_ONE, GX_LO_NOOP);

#else// PP: unsupported platform - EEK

#error Need to implement ((src*destCol)+(dest*destCol)) blending for this platform.
#error If that`s not possible, just follow the PS2`s lead and go for subtractive.

#endif// PP: platform


		this->drawTrailPart(i, FPTRAILPASS_DARK);

		// PP: **** ADD SPECULAR REFLECTIONS *****

		SETBLEND(ADDITIVE);

		// PP: IMPORTANT!!!!!! MUST SET TEXTURE BEFORE SETTING TEXTURE ADDRESS MODE!!!!!!
		WRAPTEXTURE;

		this->drawTrailPart(i, FPTRAILPASS_LIGHT);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: POLISHERINFO::prepareTrailPrims
	Purpose 	: prepare all the primitives that make up the 
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void POLISHERINFO::prepareTrailPrims(void)
{
	int				v;
	FPTRAILPART*	trailPart=this->trailParts;

	// PP: copy prim verts into a contiguous buffer for drawing
	this->trailVerts.copyToBuffer(FPTrail_contigVerts);

	// PP: **** VERTS GET HIGHER TOWARDS END OF LIST - this prevents Z fighting... **** 
	// PP: Can only Z-bias on a per-prim basis :o(
	// PP: It's the best of many evils

	float					vertHeightAdjust;

	// PP: don't let the trail get below the minGroundClearance (this is used to keep the trail above the floor poster)
	vertHeightAdjust=POLISHERINFO::minGroundClearance;

	for(v=this->trailVerts.size-2; v>=0; v-=2)
	{
		ADJUST_VERT_YPOS(&FPTrail_contigVerts[v], vertHeightAdjust);
		ADJUST_VERT_YPOS(&FPTrail_contigVerts[v+1], vertHeightAdjust);

		vertHeightAdjust+=FPTRAIL_Y_OFFSET_STEP;
	}

	this->numTrailParts=0;

	// PP: start at end of trail (contigVerts run from tail to head)
	// PP: so we're going to run back from head to tail
	v=this->trailVerts.size-2;
	trailPart->lastVert=trailPart->lastVertTruncated=this->trailVerts.size-1;

	while(POLISHERINFO::vertFlag(&FPTrail_contigVerts[v]) != FPTRAILVERT_NORMAL)
	{
		v-=2;
		trailPart->lastVertTruncated=v+1;
		trailPart->lastVert=trailPart->lastVertTruncated+2;
	}

	while(v>0)
	{
		// PP: NOTE: there is a case where the tail section is trailing-edge
		while(v>0)
		{
			if(POLISHERINFO::vertFlag(&FPTrail_contigVerts[v]) == FPTRAILVERT_TRAILINGEDGE)
			{
				trailPart->firstVert=v;
				trailPart->firstVertTruncated=v+2;

				trailPart->numVerts=(trailPart->lastVert-trailPart->firstVert)+1;
				trailPart->numVertsTruncated=(trailPart->lastVertTruncated-trailPart->firstVertTruncated)+1;

				break;
			}

			v-=2;
		}

		if(v)// PP: if we reached a trailing edge rather than the tail
		{
			if(this->numTrailParts == (FPTRAIL_MAX_PARTS-1))
			{
				break;
			}

			// PP: trailPart is already filled-in, progress to next trailPart
			this->numTrailParts++;// PP: another trailPart has been filled-in
			trailPart++;

			// PP: get the last vert and truncated last vert for the next trail part

#ifdef FPTRAIL_DEGENERATE_QUADS
			v-=8;
#else// PP: ifndef def FPTRAIL_DEGENERATE_QUADS
			v-=4;
#endif// PP: ndef def FPTRAIL_DEGENERATE_QUADS

			if(v>=0)
			{
				trailPart->lastVertTruncated=v+1;
				trailPart->lastVert=v+3;
			}
		}
		else// PP: if we reached the tail
		{
			if(this->numTrailParts == (FPTRAIL_MAX_PARTS-1))
			{
				break;
			}

			// PP: trailPart has not been filled-in yet, do it here.
			// PP: this is the last trailPart

			this->numTrailParts++;// PP: another trailPart has been filled-in

			trailPart->firstVert=0;
			trailPart->firstVertTruncated=0;

			trailPart->numVerts=(trailPart->lastVert-trailPart->firstVert)+1;
			trailPart->numVertsTruncated=(trailPart->lastVertTruncated-trailPart->firstVertTruncated)+1;
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: DrawFloorPolisher
	Purpose 	: draw anything to do with the floor polisher
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: 
*/
void DrawFloorPolisher(ACTORINSTANCE *const polisher)
{
#ifdef FPTRAIL_TIMERS
	STARTTIMER;
#endif// PP: def FPTRAIL_TIMERS

	POLISHERINFO*			info=polisher->vehicleInfo->polisherInfo;

	// PP: RETURN IF THERE ARE NO VERTS TO DRAW!
	if(info->trailVerts.size == 0)
	{
		return;
	}

	int32					starVertIndex=0;
	
	int						v;
	TBSavedRenderStates		savedRenderStates;

	// PP: Update floor polisher reflections, just like it says on the tin
	UpdateFloorPolisherReflections(polisher);

	// PP: moved this here from the update
	info->prepareTrailPrims();

	// PP: save current render states
	bdSaveRenderStates(&savedRenderStates);

	// PP: DRAW A SPARKLING TRAIL OF CLEANLINESS...

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);

	bdSetIdentityObjectMatrix();

	ENABLEBLEND(TRUE);
	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	bdSetZBias(0.0f);

	// PP: ~~~~~~~~

	// PP: **** DRAW THE TRAIL'S COMPLETE Z-TEMPLATE ****
	info->drawTrailZTemplate();

	// PP: **** DRAW EACH OF THE TRAIL'S SECTIONS ****
	info->drawTrailParts();

	// PP: DRAW STARS...

	// PP: put some z bias on to stop the bottom spikes of the stars clipping into the trail
	bdSetZBias(FPTRAIL_STAR_ZBIAS);

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);

	if(info->trailStars.size > 0)
	{
		LA_FOREACH(info->trailStars, FPTRAILSTAR, star)

			star->draw(global2DVertexBuffer, &starVertIndex);

		LA_ENDFOREACH(info->trailStars)

		bdSetTexture(0, FPTRAILSTAR::texture);

		// set sprite projection mode
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODESPRITE);

		// PP: correct ps2 splitscreen distortion
		setSpriteObjMat();

		if(starVertIndex) bdDrawPrimitive(BDPRIMTYPE_QUADLIST, global2DVertexBuffer, starVertIndex,BVERTTYPE_SINGLE2D);

		// go back to normal 3D mode
		bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	}

#ifdef FPTRAIL_DEBUG
	
	// PP: TEMP TEST - show trail edge offsets
	{
		bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_ALWAYS, FALSE);

		VEC	startVec=gameStatus.player1->actorInstance.position+VECM(0,0.01f,0);

		// PP: calculate X & Z offsets of newest section verts
		TBVector2D	XZoffset;
		XZoffset[X]=METERTOUNIT(0.5f)*bmCos(info->sectionYang);
		XZoffset[Y]=METERTOUNIT(0.5f)*bmSin(info->sectionYang);

		utilDrawLine(startVec, startVec+VEC(XZoffset[X],0,XZoffset[Y]), PINK);
		//utilDrawLine(startVec, startVec-VEC(XZoffset[X],0,XZoffset[Y]), LIGHTBLUE);
	}
	

	// PP: show trail bounding box

	info->trailBoundingBox.draw();

	utilDrawSphere(info->trailBoundingBox.pos());

#endif// PP: def FPTRAIL_DEBUG

	// PP: take z bias off again
	bdSetZBias(0.0f);

	// PP: restore render states
	bdRestoreRenderStates(&savedRenderStates);

#ifdef FPTRAIL_TIMERS
	ENDTIMER("DrawFloorPolisher");
#endif// PP: def FPTRAIL_TIMERS
}

/* --------------------------------------------------------------------------------
   Function : RespondToFloorPolisherCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToFloorPolisherCollision(ACTORINSTANCE *collider, ACTORINSTANCE *polisher)
{
	TBVector		relativePosition = {0.0f, 0.0f, 0.0f, 1.0f};
	TBVector		relativeScale = {1.0f, 1.0f, 1.0f, 1.0f};
	TBActorInstance	*actorInstance;

	if (collider->genericAI) 
	{
		AICollideWithSpecial(collider,polisher);
		return FALSE;
	}

	// JW: Removed from the below if statement

	if((~collider->flags&ACTORFLAG_DEBRISACTOR)&&
		(collider->actorStatus->currentState == STATE_JUMP)&&
		(collider->actorStatus->timeWithoutVehicleCollision > 0.3f))
	{
		// JW: **************************

		if (RequestStateChange(collider, STATE_VEHICLE))
		{
			if (UpdateCharacterState(&gameStatus.player[collider->playerNo], true))
			{
				polisher->vehicleInfo->state = VEHICLE_STATE_ON;
				polisher->vehicleInfo->currentRotation = 0.0f;
				polisher->vehicleInfo->currentRotationalVelocity = 0.0f;
				polisher->vehicleInfo->polisherInfo->rider=collider;		// PP: give the polisher a pointer to its rider actorinstance
				collider->actorStatus->vehicleInfo = polisher->vehicleInfo;

				//change field of view
//				if (collider->playerNo != -1)
//				{
//					SetFixedDistanceToLookAt(gameStatus.player[collider->playerNo].camera, METERTOUNIT(2.0f));
//					SetCameraSmooth(gameStatus.player[collider->playerNo].camera, 0.9f, 0.9f);
//				}

				//set orientation of collider to match rocketbike
				SetActorOrientation(collider, polisher->actorInstance.orientation);
				// CMD: also need to match Taz's position to that of the floor polisher
				SetActorPosition(collider, polisher->actorInstance.position);
				//set actor velocity
				SETVECTOR(collider->actorBehaviour->physics.velocity, 0.0f, 0.0f, 0.0f, 1.0f);

				polisher->vehicleInfo->handle = AttachObjectToCharacter(collider, polisher->actorName, "motion", relativePosition, relativeScale, bIdentityQuaternion, 0);
				actorInstance = ReturnAttachedObjectInstance(collider, polisher->vehicleInfo->handle);
				if(!actorInstance)
				{
					bkPrintf("*** ERROR ***, could not find polisher.obe\n");
					return FALSE;
				}
				DisableActorCartoonRenderMode(actorInstance);
				FlushAnimationQueue(actorInstance);
				PlayAnimationByName(actorInstance, "polish", 1.0f, 1, 0, 0.1f, NONE);
				FlushAnimationQueue(collider);
				PlayAnimationByName(collider, "polish", 1.0f, 1, 0, 0.1f, NONE);

				float spdPrcnt = (bmVectorLen(collider->actorBehaviour->physics.velocity) / FLOORPOLISHER_MAX_SPEED);

				int frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt) + LOWEST_FREQUENCY;

				//CallbackWhenSampleFinished(
				PlaySample("obj_mus_03a.wav", 255, 0, frequency, collider->actorInstance.position, 0);
				FloorPolisherSoundCallback(collider);

				RemoveActorFromCollisionCache(&collisionCache, polisher);
				polisher->flags |= ACTORFLAG_DONTDRAW;

//				if (collider->playerNo!=-1)
//				{
//					EnableCameraRotateBehind(gameStatus.player[collider->playerNo].camera, RAD(90.0f));
//				}

				//turn motion bone off
				collider->actorAnimation->useMotionBone = FALSE;
				SetActorMinFricAngle(collider->actorBehaviour, 1.0f);
				collider->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
				collider->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;

				polisher->vehicleInfo->polisherInfo->updateBoundingBox();

				polisher->vehicleInfo->timeSinceCameraColl = 0.0f;
			}
			else
			{
				ClearStateChangeRequest(collider);
			}
		}
	}

	return FALSE;
}

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToTrolleyControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToFloorPolisherControl(PADCONTROLSTATUS *controller, CAMERAINFO *cameraInfo, ACTORINSTANCE *actorInstance)
{
	TBMatrix		tempMatrix;
	TBVector		tempVector;
	TBVector		tempVelocity;
	TBVector		orientationVector;
	float			angleOfMovement, length;

	// CMD: turn motion bone off just in case
	actorInstance->actorAnimation->useMotionBone = FALSE;
	actorInstance->actorBehaviour->physics.flags |= PHYSFLAG_WALLGROUNDCOLLFIX;
	actorInstance->actorBehaviour->physics.flags &= ~PHYSFLAG_WALLVELCORRECTION;
	// CMD: zero time without vehicle collision
	actorInstance->actorStatus->timeWithoutVehicleCollision = 0.0f;

	angleOfMovement = ConvertFromCameraToGlobalRotation(cameraInfo,controller->direction1);

	tempVector[0] = controller->x1;
	tempVector[1] = 0.0f;
	tempVector[2] = -controller->y1;
	tempVector[3] = 1.0f;

	// CMD: only add speed if on ground
	if ((actorInstance->actorStatus->timeWithoutGroundCollision < 0.15f)||(actorInstance->actorStatus->timeWithoutWallCollision < 0.15f))
	{
		// CMD: copy current velocity to prevent shennanigans
		// CMD: at first glance this line seems redundant, however it is needed for those occasions when there is no pad input
		bmVectorCopy(tempVelocity, actorInstance->actorBehaviour->physics.velocity);

		if(bmVectorLen(tempVector))
		{
			// CMD: velocity
			bmVectorScaleToLength(tempVelocity, tempVector, (FLOORPOLISHER_SPEED_INC*fTime));
			// CMD: find orientation vector to apply velocity along
			bmQuatToMatrix(tempMatrix, actorInstance->actorInstance.orientation);
			SETVECTOR(orientationVector, 0.0f, 0.0f, -1.0f, 1.0f);
			bmMatMultiplyVector(tempMatrix, orientationVector);
			bmVectorScaleToLength(tempVelocity, orientationVector, tempVelocity[2]*1.5f);

			bmVectorAdd(tempVelocity, tempVelocity, actorInstance->actorBehaviour->physics.velocity);
			if(bmVectorLen(tempVelocity) > FLOORPOLISHER_MAX_SPEED)
			{
				bmVectorScaleToLength(tempVelocity, tempVelocity, FLOORPOLISHER_MAX_SPEED);
			}
			// CMD: orientation
			if(bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity += FLOORPOLISHER_ROT_INC*tempVector[0]/fps) > FLOORPOLISHER_MAX_ROT_ACCEL)
			{
				actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity = actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity/bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity)*FLOORPOLISHER_MAX_ROT_ACCEL;
			}
		}
		// CMD: allways subtract a little velocity
		length = bmVectorLen(tempVelocity);
		if((length -= METERTOUNIT(0.4f)*fTime) > 0.0f)
		{
			bmVectorScaleToLength(tempVelocity, tempVelocity, length);
		}
		else
		{
			SETVECTOR(tempVelocity, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		// CMD: how about a bit of rotational friction
		length = bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity);
		if((length -= RAD(60.0f)*fTime) > 0.0f)
		{
			// CMD: scale rotational velocity to length
			actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity = 
				actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity/bmFAbs(actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity)*length;
		}
		else
		{
			actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity = 0.0f;
		}
		
		SetActorVelocity(actorInstance->actorBehaviour, tempVelocity);
		// CMD: check for acceleration/brake input
	}

	if((actorInstance->actorStatus->vehicleInfo->currentRotation += (actorInstance->actorStatus->vehicleInfo->currentRotationalVelocity/fps)) > RAD(360.0f))
	{
		actorInstance->actorStatus->vehicleInfo->currentRotation -= RAD(360.0f);
	}
	if(actorInstance->actorStatus->vehicleInfo->currentRotation < RAD(-360.0f))
	{
		actorInstance->actorStatus->vehicleInfo->currentRotation += RAD(360.0f);
	}
	CharacterRotateToAngle(actorInstance, 0.0f, actorInstance->actorStatus->vehicleInfo->currentRotation, 0.0f, 3.0f);

	// CMD: choose animation
	ChooseCorrectTazOnFloorPolisherAnimations(actorInstance, controller);

	if (bsGetChannelStatus(actorInstance->actorStatus->vehicleInfo->soundHandle) == BSCHSTATUS_PLAYING)
	{
		float spdPrcnt = (bmVectorLen(actorInstance->actorBehaviour->physics.velocity) / FLOORPOLISHER_MAX_SPEED);

		int frequency = (int) ((STANDARD_FREQUENCY - LOWEST_FREQUENCY) * spdPrcnt) + LOWEST_FREQUENCY;
		int	volume = (int) ((255.0f - 160.0f) * spdPrcnt) + 160;

		if (frequency > STANDARD_FREQUENCY)
			frequency = STANDARD_FREQUENCY;

		bsSetChannelFrequency(actorInstance->actorStatus->vehicleInfo->soundHandle, frequency);

		// TP: This was asserting on xbox with a volume of 634, now thats loud
		volume = CONSTRAIN(volume, 0, 255);
		bsSetChannelVolume(actorInstance->actorStatus->vehicleInfo->soundHandle, volume);
	}
}

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnFloorPolisherAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnFloorPolisherAnimations(ACTORINSTANCE *actorInstance, PADCONTROLSTATUS *controller)
{
	TBActorInstance		*tbActorInstance;

	tbActorInstance = ReturnAttachedObjectInstance(actorInstance, actorInstance->actorStatus->vehicleInfo->handle);

	if (!tbActorInstance) return;
		
	if ((actorInstance->actorStatus->timeWithoutWallCollisionLastFrame > 0.2f)&&(actorInstance->actorBehaviour->physics.coll))
	{
		//Taz
		if(!CheckIfPlayingAnimation(actorInstance, "polishbounce"))
		{
			PlaySample("obj_mus_04a.wav",255,actorInstance->actorInstance.position);
			PlayAnimationByName(actorInstance, "polishbounce", 1.0f, 0, 0, 0.1f, REACT);
		}
		//Trolley
		if(!CheckIfPlayingAnimation(tbActorInstance, "bounce"))
		{
			PlayAnimationByName(tbActorInstance, "bounce", 1.0f, 0, 0, 0.1f, REACT);
		}
	}
/*	if(controller->idleTime > 5.0f)
	{
		//Taz
		if((!CheckIfPlayingAnimation(actorInstance, "trolleygotoidle"))&&(!CheckIfPlayingAnimation(actorInstance, "trolleyidle")))
		{
			PlayAnimationByName(actorInstance, "trolleygotoidle", 1.0f, 0, 0, 0.1f, IDLE);
			PlayAnimationByName(actorInstance, "trolleyidle", 1.0f, 1, 1, 0.1f, IDLE);
		}
		//Trolley
		if(!CheckIfPlayingAnimation(tbActorInstance, "static"))
		{
			PlayAnimationByName(tbActorInstance, "static", 1.0f, 1, 0, 0.1f, IDLE);
		}
	}
	else*/
	{
		//Taz
		if(!CheckIfPlayingAnimation(actorInstance, "polish"))
		{
			PlayAnimationByName(actorInstance, "polish", 1.0f, 0, 0, 0.1f, MOVE);
		}
		//Trolley
		if(tbActorInstance)
		{
			if(!CheckIfPlayingAnimation(tbActorInstance, "polish"))
			{
				PlayAnimationByName(tbActorInstance, "polish", 1.0f, 0, 0, 0.1f, MOVE);
			}
		}
	}
}

/* --------------------------------------------------------------------------------
   Function : FloorPolisherSoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void FloorPolisherSoundCallback(void *context)
{
	ACTORINSTANCE	*actorInstance;

	actorInstance = (ACTORINSTANCE*)context;

	if (actorInstance->actorStatus->vehicleInfo)
	{
		actorInstance->actorStatus->vehicleInfo->soundHandle = PlaySample("loop\\obj_mus_03b.wav", 255, actorInstance->actorInstance.position);
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FPTRAILSTAR methods

								Twinkling sparkles of cleanliness on a floor polisher trail
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FPTRAILSTAR::init
	Purpose 	: initialisation
	Parameters 	: position
	Returns 	: 
	Info 		: 
*/
void FPTRAILSTAR::init(TBVector posIn)
{
	bmVectorCopy(this->pos, posIn);
	this->life=FPTRAILSTAR_LIFETIME;
	this->size=0.0f;
	
	// PP: Cleanliness stars come in 2 fresh flavours...

	if(bmRand()&1)
	{
		this->col=COLOUR(128, 128, 128, 255);// PP: snowdrop
	}
	else
	{
		this->col=COLOUR(50, 128, 128, 255);// PP: and aqua
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: FPTRAILSTAR::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void FPTRAILSTAR::update(void)
{
	if(this->life < (FPTRAILSTAR_LIFETIME/2.0f))
	{
		this->size=(this->life/(FPTRAILSTAR_LIFETIME/2.0f))*FPTRAILSTAR_MAX_SIZE;
	}
	else
	{
		this->size=(1.0f-((this->life-(FPTRAILSTAR_LIFETIME/2.0f))/(FPTRAILSTAR_LIFETIME/2.0f)))*FPTRAILSTAR_MAX_SIZE;
	}

	this->life -= fTime;
}


/*	--------------------------------------------------------------------------------
	Function 	: FPTRAILSTAR::draw
	Purpose 	: draw
	Parameters 	: ptr to start of vertex buffer to which to draw the star, (in&out)ptr to current vert index (updated by this method)
	Returns 	: 
	Info 		: 
*/
void FPTRAILSTAR::draw(TBPrimVertex2D* const vertbuf, int32* const vertIndex)
{
	if(bdDrawFlatSprite(
		this->pos,
		this->size,
		this->size,
		0.0f,
		FPTRAILSTAR::texture,
		FALSE,
		FALSE,
		this->col.r,
		this->col.g,
		this->col.b,
		this->col.a,
		&vertbuf[*vertIndex],
		0.0f
	) != FAIL)
	{
		// PP: sprite has drawn; update vert index accordingly
		(*vertIndex)+=4;
	}
}