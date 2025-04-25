#ifndef	__H_FLOORPOLISHER
#define	__H_FLOORPOLISHER


#include "LoopArray.h"							// PP: Looping array class - please use it!
#include "Colour.h"								// PP: handy colour struct
#include "MonitorFX.h"							// PP: for windows colours: Computer monitor effects


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FPTRAILSTAR

								Twinkling sparkles of cleanliness on a floor polisher trail
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define FPTRAILSTAR_LIFETIME		0.5f								// PP: lifetime of a trail star
#define FPTRAILSTAR_MAX_SIZE		14.0f								// PP: maximum size of a trail star
#define FPTRAILSTAR_TEXTURE			"polishertrailstar.bmp"				// PP: name of texture for trail stars

struct FPTRAILSTAR
{
	TBVector				pos;
	COLOUR					col;
	float					life;			// PP: remaining life in seconds
	float					size;			// PP: sprite size

	static	TBTexture*		texture;


	/*	--------------------------------------------------------------------------------
		Function 	: FPTRAILSTAR::init
		Purpose 	: initialisation
		Parameters 	: position
		Returns 	: 
		Info 		: 
	*/
	void init(TBVector posIn);


	/*	--------------------------------------------------------------------------------
		Function 	: FPTRAILSTAR::update
		Purpose 	: update
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FPTRAILSTAR::draw
		Purpose 	: draw
		Parameters 	: ptr to start of vertex buffer to which to draw the star, (in&out)ptr to current vert index (updated by this method)
		Returns 	: true if the star drew, else false
		Info 		: 
	*/
	void draw(TBPrimVertex2D* const vertbuf, int32* const vertIndex);

};


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														POLISHERINFO

								Info about a floor polisher vehicle - so far just trail data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define FPTRAIL_NUM_VERTS				((FPTRAIL_NUM_SECTIONS*2)+2)			// PP: number of verts in a trail
																				
#define FPTRAIL_MAX_STARS				(FPTRAIL_NUM_VERTS/3)					// PP: number of stars in a trail

#define	FPTRAIL_NUM_SECTIONS			80										// PP: number of sections in a trail

#define FPTRAIL_MAX_PARTS				10										// PP: max number of different stretches of trail

#if(BPLATFORM == PS2)
#define FPTRAIL_DUST_TEXTURE			"polishertraildustsub.bmp"				// PP: name of texture for trail dust (subtracted because PS2 is shite)
#else// PP: if (BPLATFORM == PS2) else
#define FPTRAIL_DUST_TEXTURE			"polishertraildust.bmp"					// PP: name of texture for trail dust (multiplied in a fancy way with the frame buffer, can't explain, too tired)
#endif// PP: (BPLATFORM == PS2) else

#define FPTRAIL_REFLECTION_TEXTURE		"polishertrail.bmp"						// PP: name of texture for trail reflection (added)

#define FPTRAIL_ZMASK_TEXTURE			"polishertrailzmask.bmp"				// PP: name of texture for z-masking trail ends
#define FPTRAIL_ZTEMPLATE_TEXTURE		"polishertrailztemplate.bmp"			// PP: name of texture for z-templating trail ends

// PP: **** VERTS GET HIGHER TOWARDS END OF LIST - this prevents Z fighting... **** 
// PP: Can only Z-bias on a per-prim basis :o(
// PP: It's the best of many evils
// PP: keep this as low as possible.  It may well have to change between platforms because of differing zbuffer resolutions
// PP: For the moment, I'll change it depending on platform, - but DON'T FORGET ABOUT THIS
extern float yOffsetStepTwico;
#if(BPLATFORM == PS2)
#define FPTRAIL_Y_OFFSET_STEP			yOffsetStepTwico
#else// PP: if(BPLATFORM == PS2) else
#define FPTRAIL_Y_OFFSET_STEP			yOffsetStepTwico
#endif// PP: (BPLATFORM == PS2) else


// PP: visibility statuses for a POLISHERINFO
enum FPTrailVisibility
{
	FPTRAILVIS_NO			=0,											// PP: not visible on any screen
	FPTRAILVIS_SCREEN1		=1<<0,										// PP: visible on screen 1
	FPTRAILVIS_SCREEN2		=1<<1,										// PP: visible on screen 2
	FPTRAILVIS_ANY			=(FPTRAILVIS_SCREEN1|FPTRAILVIS_SCREEN2),	// PP: visible on either / both screens
	FPTRAILVIS_ALL			=FPTRAILVIS_ANY								// PP: visible on either / both screens
};

// PP: the draw passes involved in rendering the polisher trail (in order)
enum FPTrailDrawPass
{
	FPTRAILPASS_ZTEMPLATE,						// PP: z-template pass of the entire trail
	FPTRAILPASS_DARK,							// PP: darker lower layer
	FPTRAILPASS_LIGHT							// PP: lighter upper layer (specular reflections)
};


// PP: floor polisher type-specific data struct - instantiated by POLISHER type VEHICLEINFO instances
struct POLISHERINFO
{
	// PP: some happy fluffy friendly functions - THE REST OF YA CAN PISS OFF!!  YA HEAR ME?!!  I'M NOT DRUNK - YOU'RE JUST STUPID!!  EH?!  YA HEAR ME?!!  "MISTER STUPID-HEAD"?!							J'YOU WANNA FIGHT?!!  J'YOU WANNA FOOKIN FIGHT MATE?!!  COME ON THEN!!

	/* --------------------------------------------------------------------------------
	   Function		: InitFloorPolishers
	   Purpose		: initialise stuff for any floor polishers
	   Parameters	: 
	   Returns		: 
	   Info			: // PP: 
	*/
	friend void InitFloorPolishers(void);


	/*	--------------------------------------------------------------------------------
		Function 	: UpdateFloorPolisher
		Purpose 	: update anything to do with the floor polisher
		Parameters 	: floor polisher ACTORINSTANCE
		Returns 	: 
		Info 		: // PP: 
	*/
	friend void UpdateFloorPolisher(ACTORINSTANCE *const polisher);


	/*	--------------------------------------------------------------------------------
		Function 	: UpdateFloorPolisherReflections
		Purpose 	: update reflections in a floor-polisher trail
		Parameters 	: floor polisher ACTORINSTANCE
		Returns 	: 
		Info 		: // PP: must be called before each DRAW because the effect is obviously viewpoint-dependent
	*/
	friend void UpdateFloorPolisherReflections(ACTORINSTANCE *const polisher);


	/*	--------------------------------------------------------------------------------
		Function 	: DrawFloorPolisher
		Purpose 	: draw anything to do with the floor polisher
		Parameters 	: floor polisher ACTORINSTANCE
		Returns 	: 
		Info 		: // PP: 
	*/
	friend void DrawFloorPolisher(ACTORINSTANCE *const polisher);


	/* --------------------------------------------------------------------------------
	   Function		: ShutdownFloorPolishers
	   Purpose		: Shutdown stuff for any floor polishers
	   Parameters	: 
	   Returns		: 
	   Info			: // PP: 
	*/
	friend void ShutdownFloorPolishers(void);


private:

	static COLOUR									colour;							// PP: start colour for all polisher trails - I can imagine this getting un-static'd if ever there were to be a 2-player floor polisher race
	static TBTexture*								trailDustTexture;				// PP: solid dust texture for all floor polisher trails (subtracted)
	static TBTexture*								trailReflectionTexture;			// PP: reflection-map texture for all floor polisher trails (added)
	static TBTexture*								trailZMaskTexture;				// PP: texture for z-masking trail ends
	static TBTexture*								trailZTemplateTexture;			// PP: texture for z-templating trail ends
	static TBActorInstance*							worldZMask;						// PP:  actor instance used to z-mask floor-polisher trails in the current level
	static float									minGroundClearance;				// PP: the minimum ground clearance for any trail in this level

	BOUNDINGBOX										trailBoundingBox;				// PP: bounding box of trail effect
	LOOPARRAY<TBPrimVertex, FPTRAIL_NUM_VERTS>		trailVerts;						// PP: looping array of trail verts
	LOOPARRAY<FPTRAILSTAR, FPTRAIL_MAX_STARS>		trailStars;						// PP: looping array of trail stars
	uchar											visibility;						// PP: trail's current visibility status, updated by POLISHERINFO::visible
	float											sectionProgress;				// PP: progress through the current trail section.  When this reaches 1, we should lay a new section and set this to 0.
	float											starTimer;						// PP: timer used for placing twinkles
	float											boundingBoxTimer;				// PP: timer used to update bounding box
	VEC												lastSectionPos;					// PP: position of polisher when last trail section was added// PP: misleadingly named now, since it gets moved along with the polisher each frame that it's on the ground
	VEC												sectionBeforeLastPos;			// PP: position of polisher when the trail section before last was added
	VEC												lastUpdatePos;					// PP: position of polisher last 
	float											sectionYang;					// PP: y angle of section to be added / section last added
	float											oldSectionYang;					// PP: sectionYang from last time a section was added
	TBVector2D										oldXZOffset;					// PP: x&z offsets of one of the verts of the last section added
	bool											lastOnGround;					// PP: was the polisher on the ground last update?

	// PP: a single unbroken stretch of trail
	struct FPTRAILPART
	{
		uint32		firstVert;						// PP: index of first vert in the part (including the rounded end if there is one)
		uint32		firstVertTruncated;				// PP: index of first vert in the part (after the rounded end if there is one)

		uint32		lastVert;						// PP: index of last vert in the part (including the rounded end if there is one)
		uint32		lastVertTruncated;				// PP: index of last vert in the part (before the rounded end if there is one)

		uint32		numVerts;						// PP: number of verts in the part (including the rounded end if there is one
		uint32		numVertsTruncated;				// PP: number of verts in the part (not including the rounded ends if there are some)
	}												trailParts[FPTRAIL_MAX_PARTS];	// PP: the different stretches of trail

	uchar											numTrailParts;					// PP: number of different stretches of trail


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::prepareTrailPrims
		Purpose 	: prepare all the primitives that make up the 
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void prepareTrailPrims(void);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::drawTrailPart
		Purpose 	: draw one of the stretches of the trail
		Parameters 	: 0-based index of part to draw,
						current draw pass (FPTRAILPASS_...)
		Returns 	: 
		Info 		: 
	*/
	void drawTrailPart(const uchar part, const FPTrailDrawPass drawPass);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::drawTrailZTemplate
		Purpose 	: draw the complete z-template of the trail
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void drawTrailZTemplate(void);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::drawTrailParts
		Purpose 	: draw each of the 'parts' of the trail (both dark & light layers of each part)
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: a 'part' is an unbroken strech of trail
	*/
	void drawTrailParts(void);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::updateTrailTail
		Purpose 	: make the polisher trail 'tail-off' by getting thinner at the trailing end
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateTrailTail(void);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::getLength
		Purpose 	: get the total length of the polisher trail
		Parameters 	: 
		Returns 	: the total length of the polisher trail
		Info 		: 
	*/
	float getLength(void) const;


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::drawTrailVerts
		Purpose 	: draw main floor-polisher trail verts
		Parameters 	: ptr to contiguous array of verts to draw, base z-bias value to use,
						flag indicating that this is the preliminary z-only draw pass
		Returns 	: 
		Info 		: // PP: use this draw each of the two trail layers
	*/
	void drawTrailVerts(const float zBias, const bool zPass);


public:

	ACTORINSTANCE*									rider;						// PP: ACTORINSTANCE riding the vehicle

	
	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::getTrailZMask
		Purpose 	: get a ptr to the actor instance to use to z-mask floor polisher trails in the current level
		Parameters 	: 
		Returns 	: a ptr to the actor instance to use to z-mask floor polisher trails in the current level
		Info 		: 
	*/
	static inline TBActorInstance* getTrailZMask(void)
	{
		return POLISHERINFO::worldZMask;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::getMaxTrailYOffset
		Purpose 	: get the maximum Y offset of any floor polisher trail from the floor it's polishing
		Parameters 	: 
		Returns 	: the maximum Y offset of a floor polisher trail from the floor it's polishing
		Info 		: polisher trails have to be y-offset otherwise they'll z-fight when they cross over themselves.
						Unless you have any better ideas??
	*/
	static inline float getMaxTrailYOffset(void)
	{
		return FPTRAIL_Y_OFFSET_STEP*FPTRAIL_NUM_SECTIONS;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::init
		Purpose 	: initialisation
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void init(void);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::visible
		Purpose 	: determine whether or not a polisher trail is visible
		Parameters 	: (opt/false) re-test visibility (otherwise just use last result),
						(opt/true) whether visibility on ANY screen counts
		Returns 	: whether or not the polisher trail is visible
		Info 		: 
	*/
	bool visible(const bool test=false, const bool anyScreen=true);


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::shouldUpdate
		Purpose 	: should we update the polisher trail now?
		Parameters 	: 
		Returns 	: true if we should update the polisher trail now, else false
		Info 		: 
	*/
	inline bool shouldUpdate(void)
	{
		return ( (this->trailVerts.size==0)
				||this->visible() );// PP: should also update the trail if it is visible on any screen
	}


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::shouldDraw
		Purpose 	: should we draw the polisher trail now?
		Parameters 	: 
		Returns 	: true if we should draw the polisher trail now, else false
		Info 		: 
	*/
	inline bool shouldDraw(void)
	{
		// PP: we should only draw the trail if, having tested its visibility anew on the current screen,
		// PP:	we find that it is visibile on the current screen
		return this->visible(true, false);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::POLISHERINFO
		Purpose 	: default constructor: calls init
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	POLISHERINFO(void)
	{
		this->init();
	}


	// PP: trail vert flags - would normally enum but I want them as floats for speed (does that seem reasonable to you?)
#define FPTRAILVERT_NORMAL			0.0f
#define FPTRAILVERT_LEADINGEDGE		1.0f
#define	FPTRAILVERT_TRAILINGEDGE	2.0f
#define FPTRAILVERT_DEGENERATE		3.0f


	/*	--------------------------------------------------------------------------------
		Function 	: flagVert
		Purpose 	: flag a polisher trail vertex
		Parameters 	: ptr to vert to flag, flag to set (FPTRAILVERT_...)
		Returns 	: 
		Info 		: // PP: X component of vert's normal holds the flag
	*/
	static inline void flagVert(TBPrimVertex* const vert, const float flag)
	{
		BDVERTEX_SETNORMALX(vert, flag);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: vertFlag
		Purpose 	: get the flag of a trail vertex
		Parameters 	: ptr to vert to check
		Returns 	: the flag of the trail vertex (FPTRAILVERT_...)
		Info 		: // PP: X component of vert's normal holds the flag
	*/
	static inline float vertFlag(const TBPrimVertex* const vert)
	{
		float	nx;
		
		BDVERTEX_GETNORMALX(vert, nx);

		return nx;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: POLISHERINFO::updateBoundingBox
		Purpose 	: update the bounding box of the polisher trail effect
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void updateBoundingBox(void);


}BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENEREAL FLOOR-POLISHER FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#define FLOORPOLISHER_SPEED_INC	(METERTOUNIT(4.0f))
#define	FLOORPOLISHER_MAX_SPEED	(METERTOUNIT(6.0f))
#define FLOORPOLISHER_MAX_ROT_ACCEL	(RAD(120.0f))
#define FLOORPOLISHER_ROT_INC	(RAD(180.f))
#define FLOORPOLISHER_RADIUS	(METERTOUNIT(0.35f))

#if BPLATFORM == PS2
#define POLISHER_BUF_RATE		(512.0f)
#else
#define POLISHER_BUF_RATE		(512.0f*256.0f)
#endif



/* --------------------------------------------------------------------------------
   Function		: InitFloorPolishers
   Purpose		: initialise stuff for any floor polishers
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void InitFloorPolishers(void);


/* --------------------------------------------------------------------------------
   Function		: ShutdownFloorPolishers
   Purpose		: Shutdown stuff for any floor polishers
   Parameters	: 
   Returns		: 
   Info			: // PP: 
*/
void ShutdownFloorPolishers(void);


/* --------------------------------------------------------------------------------
   Function : CreateFloorPolisher
   Purpose : Creates and initialises actorInstances that act as floor polishers
   Parameters : 
   Returns : 
   Info : 
*/

void CreateFloorPolisher(struct TAG_ACTORINSTANCE *polisher);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateFloorPolisher
	Purpose 	: update anything to do with the floor polisher
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: 
*/
void UpdateFloorPolisher(ACTORINSTANCE *const polisher);


/*	--------------------------------------------------------------------------------
	Function 	: UpdateFloorPolisherReflections
	Purpose 	: update reflections in a floor-polisher trail
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: must be called before each DRAW because the effect is obviously viewpoint-dependent
*/
void UpdateFloorPolisherReflections(ACTORINSTANCE *const polisher);


/*	--------------------------------------------------------------------------------
	Function 	: DrawFloorPolisher
	Purpose 	: draw anything to do with the floor polisher
	Parameters 	: floor polisher ACTORINSTANCE
	Returns 	: 
	Info 		: // PP: 
*/
void DrawFloorPolisher(ACTORINSTANCE *const polisher);

/* --------------------------------------------------------------------------------
   Function : RespondToTrolleyCollision
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

int RespondToFloorPolisherCollision(struct TAG_ACTORINSTANCE *collider, struct TAG_ACTORINSTANCE *polisher);

/* --------------------------------------------------------------------------------
   Function : CharacterRespondToTrolleyControl
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void CharacterRespondToFloorPolisherControl(struct TAG_PADCONTROLSTATUS *controller, struct TAG_CAMERAINFO *cameraInfo, struct TAG_ACTORINSTANCE *actorInstance);

/* --------------------------------------------------------------------------------
   Function : ChooseCorrectTazOnFloorPolisherAnimations
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void ChooseCorrectTazOnFloorPolisherAnimations(struct TAG_ACTORINSTANCE *actorInstance, struct TAG_PADCONTROLSTATUS *controller);

/* --------------------------------------------------------------------------------
   Function : FloorPolisherSoundCallback
   Purpose : 
   Parameters : 
   Returns :
   Info : 
*/

void FloorPolisherSoundCallback(void *context);

#endif
