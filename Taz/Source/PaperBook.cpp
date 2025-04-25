// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : PaperBook.cpp
//   Purpose : 'Paper' book type, ie. one that looks and acts like a real physical book
// Component : Taz
//		Info : Some things have just gotta be done!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"					// PP: global stuff
#include "maths.h"					// PP: maths stuff
#include "main.h"					// PP: main stuff
#include "control.h"				// PP: provide basic input and control functions
#include "display.h"				// PP: display stuff
#include "PaperBook.h"				// PP: 'Paper' book type, ie. one that looks and acts like a real physical book
#include "VideoFX.h"				// PP: Video effects - motion blur, defocus, whatever.  Top-of-the-Pops-TASTIC, Mate!
#include "textures.h"				// PP: texture stuff
#include "philscene.h"				// PP: phil stuff
#include "Cheats.h"					// PP: TEMP TEST
#include "Icon.h"					// PP: Icon effects used by the front end


TEXTSTYLE	TS_paperBookHeading;	// PP: text style for paper book headings
TEXTSTYLE	TS_paperBookBody;		// PP: text style for paper book body text


// PP: TEMP TEST!!!!!
//#undef BPLATFORM
//#define BPLATFORM	PS2


// PP: *********************************************
// PP: *********************************************
// PP: TODO: MAKE SURE PAPERBOOKS SUPPORT ODD NUMBERS OF PAGES!!!!!!!!!
// PP: *********************************************
// PP: *********************************************



//#error "FIX THE PAPERBOOKS!!"

// PP: ~~~~~~~~~~~~~~~~~~~~~~

// PP: REMOUT: something odd about that texture (on PC)
#define PAPERBOOK_SHADOWS

#if(BPLATFORM == PC)
// PP: #error "PP: Nige - give's a shout if those pages start disappearing again.  Cheers."
#endif// PP: pc

#define PAPERBOOK_READ_LINK_REST_POSITIONS

#ifdef PAPERBOOK_READ_LINK_REST_POSITIONS

// PP: array of link resting positions, written out by the game ifndef PAPERBOOK_READ_LINK_REST_POSITIONS
// PP: array dimensions are [link index][position x,y]
// PP: book is assumed to be symmetrical, so these positions just get mirrored for the opposite page
const float PAPERPAGEINFO::linkRestInfo[PAPERPAGE_NUM_LINKS][LRI_NUM_FIELDS]=
{
#if(BPLATFORM == PS2)
	#include "PS2_PaperPageLinkRestInfo.txt"
#else
	#include "PaperPageLinkRestInfo.txt"
#endif
};


#endif// PP: def PAPERBOOK_READ_LINK_REST_POSITIONS


// PP: ~~~~~~~~~~~~~~~~~~~~~~

// PP: define this to get outlines round the pages
//#define PAPERBOOK_OUTLINES

// PP: define this to let the topmost two leaves float down into position as the book is opened

#define PAPERPAGE_MIN_SPINE_REST_ANGLE		RAD(60)// PP: ?		// PP: shallowest spine-hinge resting angle
#define PAPERPAGE_MAX_SPINE_REST_ANGLE		RAD(65)// PP: ?		// PP: steepest spine-hinge resting angle
#define PAPERPAGE_SPINE_REST_ANGLE_RANGE	(PAPERPAGE_MAX_SPINE_REST_ANGLE-PAPERPAGE_MIN_SPINE_REST_ANGLE)// PP: yadda
#define PAPERPAGE_MID_SPINE_REST_ANGLE		(PAPERPAGE_MIN_SPINE_REST_ANGLE+(0.5f*PAPERPAGE_SPINE_REST_ANGLE_RANGE))

// PP: MILESTONE FRIG?
#if(BPLATFORM == PS2)
#define PAPERPAGE_TOTAL_WIDTH				METERTOUNIT(2.12f)		// PP: total width of a paper page - big so's to avoid clipping hoy
#define PAPERPAGE_HEIGHT					PAPERPAGE_TOTAL_WIDTH*1.335f// PP: height of a paper page
#define PAPERBOOK_DRAW_Z_OFFSET				(-0.0435f)				// PP: how far to slide the book along its spine (to fit it better in view)
#else// PP: not ps2
#define PAPERPAGE_TOTAL_WIDTH				METERTOUNIT(2.0f)		// PP: total width of a paper page - big so's to avoid clipping hoy
#define PAPERPAGE_HEIGHT					PAPERPAGE_TOTAL_WIDTH*1.25f// PP: height of a paper page
#define PAPERBOOK_DRAW_Z_OFFSET				(-0.05f)				// PP: how far to slide the book along its spine (to fit it better in view)
#endif// PP: bleh
#define PAPERBOOK_SKETCHPAD_DRAW_X_OFFSET	(-0.5f)				// PP: how far to slide the (sketchpad) book across its spine (to fit it better in view)
#define PAPERBOOK_DRAW_X_OFFSET				(0.0f)				// PP: how far to slide the book across its spine (to fit it better in view)

#define PAPERPAGE_TOTAL_BEND_RESISTANCE		10.0f// PP: ?		// PP: pageweights of resistance per radian of current bend - this gets distributed evenly across all the links in the page
#define PAPERPAGE_TOTAL_WEIGHT				1.0f				// PP: total weight of a paper page
#define PAPERPAGE_TURN_SPEED				1.7f				// PP: page turn speed, in rads/sec

#define PAPERPAGE_FLOATSPEED_OPEN			0.27f				// PP: speed value for paper floating when opening
#define PAPERPAGE_FLOATSPEED_TURN			0.671434f			// PP: speed value for paper floating when turning

#define PAPERBOOK_DRAW_POS					VECM(0,37,0)		// PP: position at which paperbooks get drawn

bool						PaperBook_debug=false;

// PP: ptr to the single paperbook currently being displayed, or NULL if none
static PAPERBOOK*			currentPaperBook					=NULL;

// PP: initialise some static PAPERPAGEINFO stuff...
const float					PAPERPAGEINFO::linkWidth			=PAPERPAGE_TOTAL_WIDTH/(PAPERPAGE_NUM_LINKS-1);
const float					PAPERPAGEINFO::linkBendResistance	=PAPERPAGE_TOTAL_BEND_RESISTANCE/(PAPERPAGE_NUM_LINKS-1);
const float					PAPERPAGEINFO::linkWeight			=PAPERPAGE_TOTAL_WEIGHT/(PAPERPAGE_NUM_LINKS-1);

#ifdef PAPERBOOK_OUTLINES

static TBPrimVertex			page1OutlineVerts[12];				// PP: array of verts forming the outlines on one of the two sides of any page
static TBPrimVertex			page2OutlineVerts[12];				// PP: array of verts forming the outlines on the other side of any page

#endif// PP: def PAPERBOOK_OUTLINES

static TBPrimVertex			page1ShadowVerts[4];				// PP: array of verts used for page shadows on one of the two sides of any page
static TBPrimVertex			page2ShadowVerts[4];				// PP: array of verts used for page shadows on the other side of any page

static TBTexture*			pageShadowTexture;					// PP: texture to use for page shadows

#if BPLATFORM==GAMECUBE
static TBRenderTarget*		bookRenderTarget[4];
static int					turnDir;
#endif


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::init
	Purpose 	: initialise the paper model for this page
	Parameters 	: new resting angle for the page's spine hinge, whether or not the leaf is to float into position rather than starting at rest
	Returns 	: 
	Info 		: sets up initial link positions, and a zero angle on all but the spine hinge, which rests
*/
void PAPERPAGEINFO::init(const float spineRestAngleIn, bool floating)
{
	this->spineRestAngle=this->oldSpineRestAngle=spineRestAngleIn;

#ifndef PAPERBOOK_READ_LINK_REST_POSITIONS
	floating=true;
#endif// PP: ndef PAPERBOOK_READ_LINK_REST_POSITIONS

	this->floatSpeed=PAPERPAGE_FLOATSPEED_OPEN;

	if(floating)
	{
		this->spineRestAngle=this->oldSpineRestAngle=spineRestAngleIn;

		this->links[0].pos=ZVEC;								// PP: all spine hinges are at 0,0
		this->links[0].lastPos=ZVEC;
		this->links[0].relativeAng=0.0f;
		this->links[0].realAng=this->spineRestAngle;			// PP: spine hinge starts at rest

		this->floatPlease();					// PP: need to give all the pages the chance to fall into position

		for(int i=0; i<PAPERPAGE_NUM_LINKS; i++)
		{
			this->links[i].relativeAng=0.0f;
			this->links[i].lastPos=VEC(-FLOATMAX, -FLOATMAX, -FLOATMAX);// PP: vandalise lastPos of each link so's to avoid problems
		}

		// PP: update ready for first draw
		//this->update();
	}
#ifdef PAPERBOOK_READ_LINK_REST_POSITIONS
	else
	{
		// PP: read in resting link info...

		float	sign;

		sign=((this->spineRestAngle > 0.0f)?1.0f:(-1.0f));

		this->spineRestAngle=this->oldSpineRestAngle=(sign*PAPERPAGEINFO::linkRestInfo[0][LRI_ANG]);
		
		for(int i=0; i<PAPERPAGE_NUM_LINKS; i++)
		{
			this->links[i].pos=VEC(sign*PAPERPAGEINFO::linkRestInfo[i][LRI_POSX],
									PAPERPAGEINFO::linkRestInfo[i][LRI_POSY]);

			this->links[i].relativeAng=sign*PAPERPAGEINFO::linkRestInfo[i][LRI_RELANG];
			this->links[i].realAng=sign*PAPERPAGEINFO::linkRestInfo[i][LRI_ANG];

#if(BPLATFORM == PS2)// PP: PLATFORM: don't ask
			this->links[i].normal=VEC(sign*PAPERPAGEINFO::linkRestInfo[i][LRI_NORMX],
										sign*PAPERPAGEINFO::linkRestInfo[i][LRI_NORMY]);
#else//not ps2
			this->links[i].normal=VEC(-sign*PAPERPAGEINFO::linkRestInfo[i][LRI_NORMX],
										-sign*PAPERPAGEINFO::linkRestInfo[i][LRI_NORMY]);
#endif//not ps2

			this->links[i].lastPos=this->links[i].pos;
		}

		this->rest();
	}
#endif// PP: def PAPERBOOK_READ_LINK_REST_POSITIONS

	// PP: initialise shadow data...

	this->shadowDistance=0.0f;
	this->leftShadowCastDistance=0.0f;
	this->rightShadowCastDistance=0.0f;

}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::rest
	Purpose 	: go into a resting state
	Parameters 	: 
	Returns 	: 
	Info 		: ifndef PAPERBOOK_READ_LINK_POSITIONS, this writes out link positions
*/
void PAPERPAGEINFO::rest(void)
{
	if(this->spineRestAngle < 0.0f)
	{
		this->state = PAPERPAGESTATE_RESTINGLEFT;
#if BPLATFORM==GAMECUBE
		turnDir = 0;
#endif
	}
	else
	{
		this->state = PAPERPAGESTATE_RESTINGRIGHT;
#if BPLATFORM==GAMECUBE
		turnDir = 0;
#endif
	}

	// PP: if resting, the page is higher than no other page
	this->height=0.0f;

#ifndef PAPERBOOK_READ_LINK_REST_POSITIONS

	// PP: write out link resting positions...

	TBHostFileHandle		fpPtr;
	char					string[256];
	float					sign;

	sign=((this->links[PAPERPAGE_NUM_LINKS-1].pos[X]>0.0f)?1.0f:(-1.0f));

	if(bkHostCreateFile("PS2_PaperPageLinkRestInfo.txt", &fpPtr) == OK)
	{
		for(int i=0; i<PAPERPAGE_NUM_LINKS; i++)
		{
			sprintf(string, "%ff,%ff,%ff,%ff,%ff,%ff,",
					sign*this->links[i].pos[X],
					this->links[i].pos[Y],
					sign*this->links[i].relativeAng,
					sign*this->links[i].realAng, 
					sign*this->links[i].normal[X],
					this->links[i].normal[Y]);

			bkHostWriteToFile(fpPtr, string, strlen(string));
		}

		bkHostCloseFile(fpPtr);
	}
	else
	{
		bkPrintf("problem!\n");
	}

#endif// PP: PAPERBOOK_READ_LINK_POSITIONS
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::turn
	Purpose 	: turn the page
	Parameters 	: new resting angle for the page's spine hinge
	Returns 	: 
	Info 		: 
*/
void PAPERPAGEINFO::turn(const float spineRestAngleIn)
{
	if(spineRestAngleIn == this->spineRestAngle)
	{
		bkPrintf("OI! silly turn!\n");
		return;
	}

	this->floatSpeed=PAPERPAGE_FLOATSPEED_TURN;

	if(spineRestAngleIn > this->spineRestAngle)
	{
		// PP: turning the page to move backwards through the book: page turns to right
		this->state=PAPERPAGESTATE_TURNINGRIGHT;
		// IH: started turning
#if BPLATFORM==GAMECUBE
		turnDir = 1;
#endif
	}
	else
	{
		// PP: turning the page to move forwards through the book: page turns to left
		this->state=PAPERPAGESTATE_TURNINGLEFT;
		// IH: started turning
#if BPLATFORM==GAMECUBE
		turnDir = -1;
#endif

	}

	this->oldSpineRestAngle=this->spineRestAngle;
	this->spineRestAngle=spineRestAngleIn;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::draw
	Purpose 	: draw the page
	Parameters 	: which side to draw (0/1), is the page upright (portrait)?,
					number of floating leaves above the right-hand leaf stack
	Returns 	: 
	Info 		: 
*/

#define PAPERPAGE_HIGHLIGHT_COLOUR		COLOUR(128, 127, 118)	// PP: brightest colour for the paper// PP: old-paper-off-white
#define PAPERPAGE_LOWLIGHT_COLOUR		COLOUR(70, 62, 55)		// PP: darkest colour for the paper// PP: brownish grey

// PP: maximum z-bias with which to draw paper pages - this is divided between the pages to z-space them (they're really VERY close together y'know)
#if(BPLATFORM == PS2)
#define PAPERPAGE_ZBIAS				(-100000.0f)			// PP: ????? I have no idea what sort of value to use
#else// PP: not ps2
#define PAPERPAGE_ZBIAS				(-1000.0f)// PP: (-12.0f)
#endif// PP: ps2/not ps2

void PAPERPAGEINFO::draw(const uchar side, const bool uprightPage, const uint32 numRightFloatingLeaves) const
{
	TBPrimVertex				verts[PAPERPAGE_NUM_LINKS*2];
	COLOUR						colour;
	const PAPERPAGELINK*		link;
	float						tempFloat;
	int							vertIndex=0;
	VEC							vertPos;
	unsigned char				alpha;
	float						u,v;		// PP: texture coords for verts
	float						*pu,*pv;	// PP: pointers to texture coords for verts (may be swapped according to page orientation)

	// PP: Z-BIAS TOWARDS THE VIEWER IF THE PAGE IS NOT RESTING
	if(this->resting())
	{
		bdSetZBias(0.0f);
	}
	else
	{
		if(this->floatingRight())
		{
			if(numRightFloatingLeaves > 0)
			{
				// PP: divide max z bias by number of RH floating leaves
				bdSetZBias(PAPERPAGE_ZBIAS*((this->heightIndex+1)/float(numRightFloatingLeaves)));
			}
			else
			{
				// PP: something's wrong!	-	but can I be bothered looking into it?			NO.

				// PP: might as well use max z bias
				bdSetZBias(PAPERPAGE_ZBIAS);
			}
		}
		else
		{
			// PP: turning left or floating left
			// PP: might as well use max z bias
			bdSetZBias(PAPERPAGE_ZBIAS);
		}
	}

	// PP: REMOUT: ?	if(uprightPage)
	{
		pu=(&u);
		pv=(&v);
	}
/* PP: REMOUT: ?	else
	{
		pu=(&v);
		pv=(&u);
	}*/
	
	for(int i=0; i<PAPERPAGE_NUM_LINKS; i++)
	{
		link= &this->links[i];

		tempFloat=bmFAbs(link->normal[Y]);

		(*pv)=i/float(PAPERPAGE_NUM_LINKS-1);

		if(side)
		{
			alpha=(link->normal[Y]>0.0f)?128:0;
		}
		else
		{
			alpha=(link->normal[Y]<0.0f)?128:0;
		}

		// PP: TEMP FRIG// PP: TODO: think about what's really going on here
		if((side != 0) == uprightPage)
		{
			(*pv)=(1.0f-(*pv));
		}

#ifdef PAPERBOOK_READ_LINK_REST_POSITIONS
		// PP: constant hinge colour
		if(i == 0)
		{
			// PP: This helps the page join look more natural as the page starts to turn
			colour=COLOUR(PAPERPAGE_LOWLIGHT_COLOUR, PAPERPAGE_HIGHLIGHT_COLOUR, bmFAbs(PAPERPAGEINFO::linkRestInfo[0][LRI_NORMY]));
			colour.a=alpha;
		}
		else
#endif// PP: def PAPERBOOK_READ_LINK_REST_POSITIONS
		{
			colour=COLOUR(PAPERPAGE_LOWLIGHT_COLOUR, PAPERPAGE_HIGHLIGHT_COLOUR, tempFloat);
			colour.a=alpha;
		}

		//vertPos=PAPERBOOK_DRAW_POS+(this->links[i].pos.setZ(PAPERPAGE_HEIGHT*(0.5f+PAPERBOOK_DRAW_Z_OFFSET)))+(this->links[i].pos.setX(PAPERPAGE_TOTAL_WIDTH*(0.5f+PAPERBOOK_DRAW_X_OFFSET)));

		vertPos=this->links[i].pos;

		BDVERTEX_SETVEC(&verts[vertIndex], vertPos);
		BDVERTEX_SETCOLOUR(&verts[vertIndex], colour);

		// PP: CRAPPY BODGE.  I JUST DON'T CARE ANYMORE, YA HEAR ME!  ALL I WaNT IS TO DO IS GO HOME AND *SLEEP*
		if(uprightPage)
		{
			(*pu)=0.0f;
		}
		else
		{
			(*pu)=1.0f;
		}

		BDVERTEX_SETUV(&verts[vertIndex], u, v);

		vertIndex++;

		// PP: forgotten why this is done
		vertPos[Z]-=PAPERPAGE_HEIGHT;

		BDVERTEX_SETVEC(&verts[vertIndex], vertPos);
		BDVERTEX_SETCOLOUR(&verts[vertIndex], colour);

		// PP: CRAPPY BODGE.  I JUST DON'T CARE ANYMORE, YA HEAR ME!  ALL I WaNT IS TO DO IS GO HOME AND *SLEEP*
		if(uprightPage)
		{
			(*pu)=1.0f;
		}
		else
		{
			(*pu)=0.0f;
		}

		BDVERTEX_SETUV(&verts[vertIndex], u, v);

		vertIndex++;
	}

	if(vertIndex > 2) bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, verts, vertIndex, BVERTTYPE_SINGLE);

	bdSetZBias(0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::drawDebug
	Purpose 	: debug-draw the page
	Parameters 	: colour in which to debug-draw the page, (opt/false)whether or not to debug-draw the contents of the page (no need to draw them if they're hidden)
	Returns 	: 
	Info 		: // PP: drawContents not used yet
*/
void PAPERPAGEINFO::drawDebug(const COLOUR& colourIn, const bool drawContents) const
{
	// PP: TEMP - just draw the cross-section in 2D and whatever debug stuff I wanna see...

	TBPrimVertex	verts[2];
	COLOUR			colour;

	for(int i=1; i<PAPERPAGE_NUM_LINKS; i++)
	{
		colour=((i&1)?colourIn:(colourIn*0.7f));
		colour=((i&1)?BLUE:GREEN);

		BDVERTEX_SETVEC(&verts[0], this->links[i-1].pos);
		BDVERTEX_SETVEC(&verts[1], this->links[i].pos);

		BDVERTEX_SETCOLOUR(&verts[0], colour);
		BDVERTEX_SETCOLOUR(&verts[1], colour);

		// PP: to hell with efficiency this is a debug func!
		bdDrawPrimitive(BDPRIMTYPE_LINELIST, verts, 2, BVERTTYPE_SINGLE);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERPAGEINFO::update
	Purpose 	: update the paper model for this page
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: page contents get updated completely independently of the paper model, so that's cool
*/

// PP: define this to freeze the paper model
//#define PAPERPAGE_FROZEN_PAPER// PP: TEMP, for initial test only

void PAPERPAGEINFO::update(void)
{
	int					i;					// PP: loop counter
	PAPERPAGELINK*		link;
	PAPERPAGELINK*		prevLink;

	// PP: dokey.

	// PP: firstly, update any turning action...

	if(this->resting())
	{
		return;// PP: no physical update to do - all forces are balanced
	}
	else if(this->turningRight())
	{
		this->links[0].realAng+=(fTime*PAPERPAGE_TURN_SPEED);

		if(this->links[0].realAng > this->spineRestAngle)
		{
			this->links[0].realAng=this->spineRestAngle;
			this->floatPlease();
		}
	}
	else if(this->turningLeft())
	{
		this->links[0].realAng-=(fTime*PAPERPAGE_TURN_SPEED);

		if(this->links[0].realAng < this->spineRestAngle)
		{
			this->links[0].realAng=this->spineRestAngle;
			this->floatPlease();
		}
	}

	// PP: for each link...

	bool	motion=false;// PP: assume no motion till we find some
	VEC		posCopy;

	// PP: minimum squared motion of links, below which the paper will start to freeze
#define PAPERPAGE_MIN_SQUARED_LINK_MOTION	(4.0f * fTime)

	float	minX=FLOATMAX;
	float	maxX=FLOATMIN;

	this->height=0.0f;

	float	sign;

	if(this->links[PAPERPAGE_NUM_LINKS-1].pos[X] > 0.0f)
	{
		sign=1.0f;
	}
	else
	{
		sign=(-1.0f);
	}

	for(i=1; i<PAPERPAGE_NUM_LINKS; i++)
	{
		link= &this->links[i];
		prevLink= &this->links[i-1];

		// PP: update real angle
		link->realAng=prevLink->realAng+link->relativeAng;

		// PP: update position
		link->pos=prevLink->pos+(VEC(0.0f, linkWidth).rotateZ(link->realAng));

		// PP: record rightmost & leftmost link X's, for shadow casting...

		if(link->pos[X] > maxX)
		{
			maxX=link->pos[X];
		}

		if(link->pos[X] < minX)
		{
			minX=link->pos[X];
		}

		this->height += link->pos[Y];

		// PP: check for movement
		// PP: motion |= (link->pos != link->lastPos);
		motion |= (squaredDistance(link->pos, link->lastPos) > PAPERPAGE_MIN_SQUARED_LINK_MOTION);

		// PP: update normal
		link->normal=VEC(0.0f, 1.0f).rotateZ(((link->realAng+prevLink->realAng)/2.0f)-HALFPI);

		link->lastPos=link->pos;
	}

	this->height;

#ifdef PAPERBOOK_READ_LINK_REST_POSITIONS
	this->rightShadowCastDistance=maxX/PAPERPAGEINFO::linkRestInfo[PAPERPAGE_NUM_LINKS-1][LRI_POSX];
	this->leftShadowCastDistance=minX/(-PAPERPAGEINFO::linkRestInfo[PAPERPAGE_NUM_LINKS-1][LRI_POSX]);
#endif//def PAPERBOOK_READ_LINK_REST_POSITIONS

	if(this->floating())
	{
		if(!motion)
		{
			this->rest();
			return;
		}
	}

	// PP: must also update normal of spine hinge
	this->links[0].normal=VEC(0.0f, 1.0f).rotateZ(this->links[0].realAng-HALFPI);

#ifndef PAPERPAGE_FROZEN_PAPER

	// PP: start at the outside edge of the page, apply cumulating weight to each link as we move inwards...

	float	weight=PAPERPAGEINFO::linkWeight;	// PP: downwards weight
	float	bendForce;							// PP: bending force on link, ie. weight acting perpendicular to link
	float	bendResistance;						// PP: current bending resistance of link
	float	bendResultant;						// PP: resultant unbalanced bending force

	for(i=PAPERPAGE_NUM_LINKS-2; i>=0; i--)
	{
		link= &this->links[i];

		// PP: calculate the component of the weight that acts perpendicular to the link (bending force)
		bendForce=weight*bmSinApprox(link->realAng);// PP: weight acts straight down the Y axis

		// PP: how far does that force bend the link? ...
		
		// PP: from the current relative angle of the link, determine its current bending resistance
#define FRIG		20.5f			// PP: controls floppiness of paper - don't touch this!
		bendResistance=link->relativeAng*(PAPERPAGEINFO::linkBendResistance)*FRIG;

		// PP: get resultant unbalanced bending force
		bendResultant=bendForce-bendResistance;

		// PP: bend the link accordingly - that balances the forces
		link->relativeAng += (bendResultant*fTime*this->floatSpeed);

		weight += PAPERPAGEINFO::linkWeight;					// PP: add the weight of this link (ie. the weight of the paper on the spine side of it) onto the total
	}

#endif// PP: ndef PAPERPAGE_FROZEN_PAPER

}


// PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CAMERAINFO		PaperBook_camera;

// PP: initialise some static PAPERBOOK stuff...
bool PAPERBOOK::cameraState=false;


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::init
	Purpose 	: one-time init / reset for the book
	Parameters 	: (opt/false)flag to start the book in the open state
	Returns 	: 
	Info 		: this overrides BOOK::init
*/

// PP: moveme?
#define PAPERBOOK_MARGIN	0.07f

void PAPERBOOK::init(const bool open)
{
	BOOK::init(open);

	// PP: clear top-page ptrs
	this->leftTopPage=NULL;
	this->rightTopPage=NULL;

	// PP: set default flags
	this->paperBookFlags=DEFAULT_PBFLAGS;

	// PP: TODO: make a macro for this - any book drawing to an ORT for 3D effect will need it
	this->flags &= ~(BKFLAG_SCREENADJUST|BKFLAG_STEREOADJUST|BKFLAG_BOOKLIST);

	// PP: set default background, why not
	this->setBackground(PAPERBOOK_DEFAULT_BACKGROUND);

	// PP: initialise close callback (none)
	this->closeCallback=NULL;

	// NH: initialise PageCallbacks (none)
	this->firstPageCallback=NULL;
	this->lastPageCallback=NULL;
	this->centralPageCallback=NULL;

	// PP: No page item effects within books ta!
	this->setEffectChooser(NULL);

	// PP: set default book type / page orientation (the 'normal' books like in the tutorial)
	this->setOrientation(PBO_PORTRAIT);

	// PP: set default close button(s)
	this->setCloseButton(PAPERBOOK_DEFAULT_CLOSE_BUTTON);

	// PP: initialise background splashscreen
	// PP: is there a better way?
	ZERO(this->background);

#if BPLATFORM==GAMECUBE
	// IH: Don't create render targets, do them on the fly as and when we need them
	turnDir = 0;
#endif
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::addPage
	Purpose 	: add a page to the book
	Parameters 	: (opt/NULL)custom update function ptr, (opt/NULL) name of page,
					(opt/NULL)custom open function ptr, (opt/NULL)custom close function ptr
	Returns 	: ptr to page
	Info 		: this overrides BOOK::addPage to add the accompanying paper info for the page
*/
PAGE* PAPERBOOK::addPage(PageFunc const update, const char* const pageName, PageFunc const open, PageFunc const close)
{
	PAGE*	rtn;

	rtn=BOOK::addPage(update, pageName, open, close);

	// PP: ~~~~

	// PP: only add a leaf every 2nd page, yeah? cool.
	if((this->getNumPages() == 1) || ((this->getNumPages()&1) == 0))
	{
		// PP: add a paper page info...

		this->paperPages.append(new PAPERPAGEINFO);// PP: TODO: Append NULL here; malloc and free the leaves on a basis of visibility, in PAPERBOOK::update.
	}

	// PP: ~~~~

	return rtn;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::mainDraw
	Purpose 	: the core of the draw method - actually draw the book
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void PAPERBOOK::mainDraw(void)
{
	// PP: draw the paperbook's background
#if BPLATFORM!=GAMECUBE
	DrawSplashScreen(&this->background);
#endif

	SETPROJECTION(3D);

	// PP: send the camera
	SendCamera(PAPERBOOK::getCamera());

	SETZWRITE(TRUE);
	SETZTEST(LESSTHAN);
	CLAMPTEXTURE;					// PP: gotter remember to do this whenever texturing with a render target!!!

	ENABLEBLEND(TRUE);

	// PP: move the book to the position we want...
	TBMatrix	transMat;

	// NH: Book type specific translations
	if(this->orientation == PBO_LANDSCAPE)
	{
		bmMatTranslate(transMat, PAPERBOOK_DRAW_POS[X]+(PAPERPAGE_TOTAL_WIDTH*PAPERBOOK_SKETCHPAD_DRAW_X_OFFSET), PAPERBOOK_DRAW_POS[Y], PAPERBOOK_DRAW_POS[Z]+(PAPERPAGE_HEIGHT*(0.5f)));
	}
	else
	{
		bmMatTranslate(transMat, PAPERBOOK_DRAW_POS[X]+(PAPERPAGE_TOTAL_WIDTH*PAPERBOOK_DRAW_X_OFFSET), PAPERBOOK_DRAW_POS[Y], PAPERBOOK_DRAW_POS[Z]+(PAPERPAGE_HEIGHT*(0.5f+PAPERBOOK_DRAW_Z_OFFSET)));
	}

	int					p;			// PP: loop counter
	int					b;
	PAGE*				page1;
	PAGE*				page2;
	PAGE*				page;
	PAPERPAGEINFO*		paperPage;
	bool				pageCurrent;

#if BPLATFORM==GAMECUBE
	int					currentTarget = 0;
	RECTANGLE			tempRect;
	
	// IH: TODO: Read in screen width and height, and half them		// PP: videoMode.xScreen & .yScreen should do the trick // IH: Thanks
	tempRect.setWidth(videoMode.xScreen/2);
	tempRect.setHeight(videoMode.yScreen/2);
#endif

	// PP: ***** PS2 BABEL WORKAROUND *****
#if(BPLATFORM == PS2)
	BOOK::PS2BabelWorkaround_setRenderTargetDimensions(VFX_renderTargetInfo.width, VFX_renderTargetInfo.height);
#endif// PP: sodding ps2
	// PP: ********************************

	for(p=this->getNumPaperPages()-1; p>=0; p--)
	{
		paperPage=this->getPaperPage(p);

		page2=this->getPage(p*2);

		if(p==0)
		{
			page1=NULL;
		}
		else
		{
			page1=this->getPage((p*2)-1);
		}

		for(b=0; b<2; b++)
		{
			page=(b?page1:page2);

			// PP: TEMP TEST: don't even draw paper if contents aren't visible
			if(paperPage->resting() && (page != this->leftTopPage) && (page != this->rightTopPage))
			{
				continue;
			}

			// PP: only draw visible pages
			if((page == NULL) || (VFX_renderTarget == NULL))
			{
				// PP: set no texture, so's null pages show up grey
				NOTEXTURE;
			}
			else
			{
				if(this->iconOnly())
				{
					// PP: NEW: if this is an 'icon only' (ie. gallery) paperbook, simply use the picture on the page as the texture for the paper.
					// PP: If the page is blank though, use a blank white texture instead.

					PAGEITEM*	item=page->getFirstItem();

					if(item == NULL)
					{
						// PP: blank page, nowt wrong wi' that
						bdSetTexture(0, textureWhite);// PP: heheh I knew that'd come in handy!
					}
					else
					{
						bdSetTexture(0, ((ICON*)item)->tex);// PP: see?  you must ONLY put icons in icon-only paperbooks!
					}
				}
				else// PP: NOTE: shadows not possible in icon-only books, sorry
				{
	#ifdef PHIL
	#ifdef _TAZDEBUG
					// PP: TEMP - highlight current page
					if(PaperBook_debug && (page == this->currentPage) && (bmFMod(gameStatus.appTime, 0.3f)>0.15f))
					{
						pushRenderTarget(VFX_renderTarget, 200,255,200,255, 1.0f, BSETRENDERTARGET_CLEARRGB);
					}
					else
	#endif// PP: def _TAZDEBUG
	#endif// PP: def PHIL
					{
	#if BPLATFORM!=GAMECUBE
						pushRenderTarget(VFX_renderTarget, 255,255,255,255, 1.0f, BSETRENDERTARGET_CLEARRGB);
	#else
						if( currentTarget < 4 && !bookRenderTarget[currentTarget] )
							bookRenderTarget[currentTarget] = bdCreateRenderTarget(videoMode.xScreen/2, videoMode.yScreen/2, 16, 16, BCREATERENDERTARGET_PRIVATEZ);
						// IH: render to it's only separate render target
						pushRenderTarget(bookRenderTarget[currentTarget], 255,255,255,255, 1.0f, BSETRENDERTARGET_CLEARRGB);
	#endif
					}

					// PP: NEW TEST
					bdSetIdentityObjectMatrix();

					// PP: render the page onto the paper...

					SETPROJECTION(2D);
					SETZWRITE(FALSE);
					SETZTEST(ALWAYS);

					// IH: There seems to be an error with stacking up render targets...
					// IH: ...so clear the background by hand
	#if BPLATFORM==GAMECUBE
					tempRect.draw(WHITE, SOLIDFILL);
	#endif

					BOOK::startDraw();
					page->draw();
					BOOK::endDraw();

	#ifdef PAPERBOOK_OUTLINES

					// PP: Draw page outlines
					NOTEXTURE;
					bdDrawPrimitive(BDPRIMTYPE_QUADLIST, b?page1OutlineVerts:page2OutlineVerts, 12, BVERTTYPE_SINGLE);

	#endif// PP: def PAPERBOOK_OUTLINES

	#ifdef PAPERBOOK_SHADOWS
					// PP: Draw shadow
					if(paperPage->shadowDistance > 0.0f)
					{
						TBPrimVertex*	shadowVerts;
						float			innerU, outerU;	
						COLOUR			shadowColour;
						float			shadowDarkness;
						float			sharpness;

						// PP: get current shadow colour (it gets darker as it goes across the page see)

	#define PAPERBOOK_SHADOW_R				35// PP: 30
	#define PAPERBOOK_SHADOW_G				39// PP: 33
	#define PAPERBOOK_SHADOW_B				43// PP: 36
	#define PAPERBOOK_SHADOW_A				128// PP: don't multiply

	#define PAPERBOOK_SHADOW_MIN_DARKNESS	0.2f

						shadowDarkness=PAPERBOOK_SHADOW_MIN_DARKNESS+(paperPage->shadowDistance*(1.0f-PAPERBOOK_SHADOW_MIN_DARKNESS));

						shadowColour=COLOUR(uchar(PAPERBOOK_SHADOW_R*shadowDarkness),
											uchar(PAPERBOOK_SHADOW_G*shadowDarkness),
											uchar(PAPERBOOK_SHADOW_B*shadowDarkness),
											PAPERBOOK_SHADOW_A);

						shadowVerts=(b?page2ShadowVerts:page1ShadowVerts);

						SETBLEND(SUBTRACTIVE);

	#define PBSHADOW_MAX_SHARPNESS		2.0f
	#define PBSHADOW_MIN_SHARPNESS		0.2f
						
						sharpness=paperPage->shadowDistance*(PBSHADOW_MAX_SHARPNESS-PBSHADOW_MIN_SHARPNESS);

						innerU=(0.5f-(sharpness*0.5f))-((paperPage->shadowDistance-0.5f)*sharpness);
						outerU=(0.5f+(sharpness*0.5f))-((paperPage->shadowDistance-0.5f)*sharpness);

						if(this->orientation == PBO_LANDSCAPE)
						{
							BDVERTEX_SETU(&shadowVerts[1], outerU);
							BDVERTEX_SETU(&shadowVerts[3], outerU);

							BDVERTEX_SETU(&shadowVerts[0], innerU);
							BDVERTEX_SETU(&shadowVerts[2], innerU);
						}
						else
						{
							BDVERTEX_SETU(&shadowVerts[0], outerU);
							BDVERTEX_SETU(&shadowVerts[1], outerU);

							BDVERTEX_SETU(&shadowVerts[2], innerU);
							BDVERTEX_SETU(&shadowVerts[3], innerU);
						}

						BDVERTEX_SETCOLOUR(&shadowVerts[0], shadowColour);
						BDVERTEX_SETCOLOUR(&shadowVerts[1], shadowColour);
						BDVERTEX_SETCOLOUR(&shadowVerts[2], shadowColour);
						BDVERTEX_SETCOLOUR(&shadowVerts[3], shadowColour);

						bdSetTexture(0, pageShadowTexture);

						// PP: TEMP  TEST - dunno where this is getting switched off
						SETFRAMEWRITE(rgbFramewriteChannels);

						bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, shadowVerts, 4, BVERTTYPE_SINGLE);

						SETBLEND(BLEND);
					}
	#endif // PP: def PAPERBOOK_SHADOWS

	#if BPLATFORM==GAMECUBE
					bdSetRenderTargetAsTexture(bookRenderTarget[currentTarget++], 0);
	#else
					bdSetRenderTargetAsTexture(VFX_renderTarget, 0);
	#endif
					
					popRenderTarget();

					// IH: Sorry for messin up your code Phil!
	#if BPLATFORM!=GAMECUBE

					SETPROJECTION(3D);
					SETZWRITE(TRUE);
					SETZTEST(LESSTHAN);
	#endif	
				}

			}// PP: end if(page & ORT exist)

			// IH: Don't actually draw any of the pages yet
#if BPLATFORM!=GAMECUBE
			bdSetObjectMatrix(transMat);
			// PP: draw the page onto the appropriate side of the leaf
			paperPage->draw(b, (this->orientation == PBO_PORTRAIT), this->numRightFloatingLeaves);
#endif
		}
	}

#if BPLATFORM==GAMECUBE
	// IH: Draw the background and then the pages
	DrawSplashScreen(&this->background);
	currentTarget = 0;
	for(p=this->getNumPaperPages()-1; p>=0; p--)
	{
		paperPage=this->getPaperPage(p);

		page2=this->getPage(p*2);

		if(p==0)
		{
			page1=NULL;
		}
		else
		{
			page1=this->getPage((p*2)-1);
		}

		for(b=0; b<2; b++)
		{
			page=(b?page1:page2);

			// PP: don't even draw paper if contents aren't visible
			if(paperPage->resting() && (page != this->leftTopPage) && (page != this->rightTopPage))
			{
				continue;
			}

			// PP: only draw visible pages
			if((page == NULL) || (VFX_renderTarget == NULL))
			{
				// PP: set no texture, so's null pages show up grey
				NOTEXTURE;
			}
			else
			{
				// PP: NEW: if this is an 'icon only' (ie. gallery) paperbook, simply use the picture on the page as the texture for the paper.
				// PP: If the page is blank though, use a blank white texture instead.
				if(this->iconOnly())
				{
					PAGEITEM*	item=page->getFirstItem();

					if(item == NULL)
					{
						// PP: blank page, nowt wrong wi' that
						bdSetTexture(0, textureWhite);// PP: heheh I knew that'd come in handy!
					}
					else
					{
						bdSetTexture(0, ((ICON*)item)->tex);// PP: see?  you must ONLY put icons in icon-only paperbooks!
					}
				}
				else
				{
					bdSetRenderTargetAsTexture(bookRenderTarget[currentTarget++], 0);
				}
			}

			SETPROJECTION(3D);
			SETZWRITE(TRUE);
			SETZTEST(LESSTHAN);

			bdSetObjectMatrix(transMat);

			// PP: draw the page onto the appropriate side of the leaf
			paperPage->draw(b, (this->orientation == PBO_PORTRAIT), this->numRightFloatingLeaves);
		}
	}
#endif

	// PP: remove z-bias
	bdSetZBias(0.0f);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::mainDrawDebug
	Purpose 	: the core of the debug-draw method - actually debug-draw the paper book
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOOK::mainDrawDebug
*/

#define PAPERBOOK_DRAW_DEBUG_SCALE			0.25f				// PP: factor by which to scale link positions when drawing a debug 2D cross section

void PAPERBOOK::mainDrawDebug(void)
{
	if(!(this->flags & (BKFLAG_OPEN/*|BKFLAG_OPENING|*/|BKFLAG_CLOSING)))
	{
		return;
	}
	// PP: TODO: draw both the left and right pages
	// PP: TODO: override default transitional effects in a nice way

	// PP: for now I'll just this opportunity to test the paper model

	// PP: TEMP: draw the paper cross-section in 2D - saves setting up any special camera just yet...

	SAVESTATES;

	SETPROJECTION(2D);
	NOTEXTURE;
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);

	TBMatrix	scaleMat;

	bmMatScale(scaleMat, PAPERBOOK_DRAW_DEBUG_SCALE, PAPERBOOK_DRAW_DEBUG_SCALE, 1.0f);
	scaleMat[3][1]=(-150.0f);// PP: heheh

	bdSetObjectMatrix(scaleMat);

	int		p;		// PP: loop counter

	for(p=this->getNumPaperPages()-1; p>=0; p--)
	{
		this->paperPages[p].drawDebug(GREEN);
	}

	// PP: just in case
	SETPROJECTION(3D);

	RESTORESTATES;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::update
	Purpose 	: update the book and its contents
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOOK::update
					// PP: NOTE!: Paper books don't call update() on their pages!
					// PP: (they tend to hold simple content and therefore don't need to)
*/
void PAPERBOOK::update(void)
{	
	int				p;				// PP: loop counter
	PAPERPAGEINFO*	leaf;
	PAPERPAGEINFO*	prevLeaf;
	PAPERPAGEINFO*	nextLeaf;
	int				pageIndex;

	// PP: TEMP TEST!!!!!!!!
/*	if(controller1.triangleDebounceChannel->value)
	{
		// PP: TEMP TEST
		this->setRect(this->bookRect);

		this->realign();

		for(int i=this->getNumPages()-1; i>=0; i--)
		{
		//	this->getPage(i)->align();

			this->getPage(i)->update();
		}
	}*/

	// PP: don't trigger close if this is the first update after being opened (debounce button values won't have had a chance to reset)
	if((~this->flags) & BKFLAG_FIRSTUPDATE)
	{
		// PP: check for button press to close book...

		if(this->closeButton & PBCLOSEBUTTON_FORWARD)
		{
			// PP: REMOUT: can't accept OKBUTTOn cos it says press start:		 || OKBUTTON
			if(controller1.startDebounceChannel->value)
			{
				this->close();	
				return;
			}
		}

		if(this->closeButton & PBCLOSEBUTTON_BACK)
		{
			if(CANCELBUTTON)
			{
				this->close();
				return;
			}
		}
	}

//	STARTTIMER;

	if(!(this->flags & (BKFLAG_OPEN/*|BKFLAG_OPENING*/|BKFLAG_CLOSING)))
	{
		return;
	}

	// PP: respond to a change in language (duplicated from BOOK::update)
	if(this->language != GAMESTATUS::currentLanguage)
	{
		this->setLanguage(GAMESTATUS::currentLanguage);

		this->realign();
	}

	// PP: update paper physics and shadows...

	p=this->getNumPaperPages()-1;

	leaf=this->getPaperPage(p);

	for(; p>=0; p--, leaf=prevLeaf)
	{
		leaf->update();

		if(p>0)
		{
			prevLeaf=this->getPaperPage(p-1);
		}

		if(!leaf->resting())
		{
			// PP: CALCULATE SHADOW DISTANCES...

			if(p>0)
			{
				if(leaf->height >= prevLeaf->height)
				{
					prevLeaf->shadowDistance=leaf->leftShadowCastDistance;
				}
			}

			if(p<(this->getNumPaperPages()-1))
			{
				nextLeaf=this->getPaperPage(p+1);
				
				if(leaf->height >= nextLeaf->height)
				{
					nextLeaf->shadowDistance=leaf->rightShadowCastDistance;
				}
			}
		}
	}

	// PP: find topmost pages and record their indices (these pages will be the only resting pages to get drawn)...

	this->leftTopPage=NULL;
	this->rightTopPage=NULL;

	this->numRightFloatingLeaves=0;

	if(this->getNumPages() != 0)
	{
		// PP: LEFT STACK...

		pageIndex=0;

		uint32		leafIndex;
		int32		leftTopLeaf= -1;
		int32		rightTopLeaf= -1;

		while((leaf=this->getPaperPage(leafIndex=PAPERBOOK::getPaperPageIndex(pageIndex)))->restingLeft())
		{
			this->leftTopPage=this->getPage(pageIndex);
			leftTopLeaf=leafIndex;

			pageIndex+=2;

			if(pageIndex == this->getNumPages()) break;
		}

		// PP: RIGHT STACK...

		if(this->getNumPages() > 1)
		{
			pageIndex=(this->getNumPages()-1);

			// PP: must be a right hand (odd index) page
			if((pageIndex&1)==0)
			{
				pageIndex -= 1;
			}

			while((leaf=this->getPaperPage(leafIndex=PAPERBOOK::getPaperPageIndex(pageIndex)))->restingRight())
			{
				this->rightTopPage=this->getPage(pageIndex);
				rightTopLeaf=leafIndex;

				pageIndex-=2;

				if(pageIndex < 0) break;
			}
			
			// PP: groovy - we have our left & right topmost resting leaf indices
			// PP: Now get the height order of the right-hand-side turners & floaters between them - this will be used to distance them with z-bias

			if(rightTopLeaf != (-1))// PP: not too sure how this occurs but it does hmmmm
			{
				if((rightTopLeaf-leftTopLeaf) > 1)
				{
					// PP: right floaters only!  Leaf draw order eliminates the need to height-order the left non-resters!

					leafIndex=rightTopLeaf-1;

					leaf=this->getPaperPage(leafIndex);

					this->numRightFloatingLeaves=0;

					while(leaf->floatingRight())
					{
						// PP: notify each leaf of its numerical position above the stack (used for z-bias separation)
						leaf->heightIndex=this->numRightFloatingLeaves++;

						leaf=this->getPaperPage(--leafIndex);
					}
				}
			}
		}
	}

	// PP: pages can be turned by pushing left or right... 
	// NH: or up and down, depemding on type of book!
	if(this->orientation == PBO_LANDSCAPE)
	{
		if(controller1.menuDownDebounceChannel->value)
		{
			// IH: Can only have one page turning at a time
			#if BPLATFORM==GAMECUBE
			if(turnDir < 0)
				return;
			#endif

			this->nextPage();

			bkPrintf("currentPageIndex %d\n", this->currentPageIndex);
		}

		if(controller1.menuUpDebounceChannel->value)
		{
			// IH: Can only have one page turning at a time
			#if BPLATFORM==GAMECUBE
			if(turnDir > 0)
				return;
			#endif

			this->prevPage();

			bkPrintf("currentPageIndex %d\n", this->currentPageIndex);
		}		
	}
	else
	{
		if(controller1.menuRightDebounceChannel->value)
		{
			// IH: Can only have one page turning at a time
			#if BPLATFORM==GAMECUBE
			if(turnDir < 0)
				return;
			#endif

			this->nextPage();

			bkPrintf("currentPageIndex %d\n", this->currentPageIndex);
		}

		if(controller1.menuLeftDebounceChannel->value)
		{
			// IH: Can only have one page turning at a time
			#if BPLATFORM==GAMECUBE
			if(turnDir > 0)
				return;
			#endif

			this->prevPage();

			bkPrintf("currentPageIndex %d\n", this->currentPageIndex);
		}
	}

	// PP: clear first-update flag
	this->flags &= ~BKFLAG_FIRSTUPDATE;

#ifndef INFOGRAMES
#ifdef PHIL
	// PP: TEMP! ;O)
/* PP: 	if(controller1.crossDebounceChannel->value != 0)
	{
		PaperBook_debug^=1;
	}*/
#endif// PP: def PHIL
#endif// PP: ndef INFOGRAMES

//	ENDTIMER("paperbook update");
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::open
	Purpose 	: open the book; make it visible
	Parameters 	: (opt/NULL=current) page to open on, (opt/false) whether or not to open instantly,
					(opt/true[?!])whether or not to do a very costly book realignment as we open
	Returns 	: 
	Info 		: this overrides BOOK::open to calculate the stacking of the paper pages (maybe move this process?)
*/
void PAPERBOOK::open(const char* const pageName, const bool instant, const int realign)
{
	uint32		leafIndex;

	BOOK::open(pageName, instant);

	// PP: new temp test!!!!
	/*for(int i=this->getNumPages()-1; i>=0; i--)
	{
		this->getPage(i)->align();
	}*/

	// PP: book a permanent offscreen render target with which to draw the book
	VFX_bookPermanentRenderTarget();

	// PP: load the background
	if(this->backgroundName[0] != '\0')
	{
		LoadSplashScreen(&this->background, this->backgroundName, "splshscr");
	}

	// PP: probably gonna move all this...

	float	spineHingeRestAngle;
	int		p;							// PP: loop counter
	

	this->spineHingeRestAngleStep=(PAPERPAGE_SPINE_REST_ANGLE_RANGE/(this->getNumPaperPages()-1));
		
	// PP: pages stacked on LH side...

	spineHingeRestAngle=((-HALFPI)+PAPERPAGE_MIN_SPINE_REST_ANGLE);

	bool	floating;

	for(p=0; p<=PAPERBOOK::getPaperPageIndex(this->currentPageIndex); p++)
	{
		floating=false;

#ifdef PAPERBOOK_FLOAT_OPEN
		if(p == PAPERBOOK::getPaperPageIndex(this->currentPageIndex))
		{
			floating=true;
		}	
#endif// PP: def PAPERBOOK_FLOAT_OPEN
		
		this->getPaperPage(p)->init(/*spineHingeRestAngle*/-(HALFPI-PAPERPAGE_MID_SPINE_REST_ANGLE), floating);

	//	bkPrintf("init paper page %d, hinge %5.2f\n", p, spineHingeRestAngle);

		spineHingeRestAngle += this->spineHingeRestAngleStep;

	}

	// PP: pages stacked on RH side...

	spineHingeRestAngle=(HALFPI-PAPERPAGE_MIN_SPINE_REST_ANGLE);

	for(p=this->getNumPaperPages()-1; p>PAPERBOOK::getPaperPageIndex(this->currentPageIndex); p--)
	{
		floating=false;

#ifdef PAPERBOOK_FLOAT_OPEN
		if(p == (PAPERBOOK::getPaperPageIndex(this->currentPageIndex)+1))
		{
			floating=true;
		}
#endif// PP: def PAPERBOOK_FLOAT_OPEN

		this->getPaperPage(p)->init(/*spineHingeRestAngle*/(HALFPI-PAPERPAGE_MID_SPINE_REST_ANGLE), floating);

	//	bkPrintf("init paper page %d, hinge %5.2f\n", p, spineHingeRestAngle);

		spineHingeRestAngle -= this->spineHingeRestAngleStep;
	}

	// PP: specify that the PAPERBOOK camera should be getting sent
//	PAPERBOOK::cameraState=true;

	currentPaperBook=this;

	// PP: override widescreen - gotter line up with a 4:3 splash screen
	overrideWidescreen();

	// PP: override stereoscopy - it just wouldn't work I tells ya!
	overrideStereoscopy(true);

	// PP: new test, was per frame...
	{
		// PP: position the camera...
		VEC	bookCamOffset;

		// NH: Book type specific cameras
		if (this->orientation == PBO_LANDSCAPE)
		{
			bookCamOffset = VEC(PAPERPAGE_TOTAL_WIDTH*0.2f, PAPERPAGE_TOTAL_WIDTH*1.06f, 0.0f)*2.9f;// NH: offset of camera from sketch book
		}
		else
		{
			bookCamOffset = VEC(0.0f, PAPERPAGE_TOTAL_WIDTH*1.35f, -PAPERPAGE_TOTAL_WIDTH*0.75f)*2.9f;// PP: offset of camera from book
		}

		setFOV(RAD(20));
		// PP: REMOUT: NEW TEST	applyFOV();

		// PP: set camera position
		SetCameraPosition(PAPERBOOK::getCamera(), PAPERBOOK_DRAW_POS+bookCamOffset);

		// PP: set camera lookat position
		SetCameraLookAtPosition(PAPERBOOK::getCamera(), PAPERBOOK_DRAW_POS.v);
	}

	this->getCurrentPage()->update();
	this->getPage(getCurrentPageIndex() + 1)->update();

	// PP: enter paper book game state
	SetGameState(&gameStatus, GAMESTATE_PAPERBOOK, GAMESUBSTATE_NORMAL);
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::close
	Purpose 	: close the book; make it invisible
	Parameters 	: (opt/0.0f) delay in seconds before closing, (opt/false) whether or not to close instantly
	Returns 	: 
	Info 		: this overrides BOOK::close to turn off the PAPERBOOK camera
*/
void PAPERBOOK::close(const float delay, const bool instant)
{
	BOOK::close(0.0f, true);// PP: ignore close parameters - no use here

	// PP: All this assumes only one paperbook will be open at a time - oh boo-hoo! ...

	// PP: release permanent render target
	VFX_releasePermanentRenderTarget();

	// PP: free background; we're finished with it
	FreeSplashScreen(&this->background);

	// PP: finish overriding widescreen
	overrideWidescreen(false);

	// PP: finish overrideing stereoscopy
	overrideStereoscopy(false);

	// PP: NEW TEST, was per frame
	{
		// PP: restore normal fov
		setFOV(NORMAL_INGAME_FOV);

	//	bdSetIdentityObjectMatrix();
	}

	TAG_PAPERBOOK::cameraState=false;

	// PP: call the close callback if one is set
	if(this->closeCallback != NULL)
	{
		this->closeCallback(this);
	}
	// IH: delete the render target when the book is closed
#if BPLATFORM==GAMECUBE
	for( int i = 0; i < 4; i++ )
	{
		if( bookRenderTarget[i] )
			bdDeleteRenderTarget(bookRenderTarget[i]);
		bookRenderTarget[i] = NULL;
	}
#endif

}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::turnPaperPageToRight
	Purpose 	: turn the paper page at the specified index to the right (to move backwards through the book)
	Parameters 	: index of page to turn to right
	Returns 	: 
	Info 		: 
*/
void PAPERBOOK::turnPaperPageToRight(const int index)
{

	// PP: because opening & turning float speeds differ, set all leaves to right of turning one to turning float speed, so's to avoid collision
	for(int i=this->getNumPaperPages()-1; i>index; i--)
	{
		this->getPaperPage(i)->floatSpeed=PAPERPAGE_FLOATSPEED_TURN;
	}
	this->getPaperPage(index)->turn((HALFPI-PAPERPAGE_MID_SPINE_REST_ANGLE));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::turnPaperPageToLeft
	Purpose 	: turn the paper page at the specified index to the left (to move forwards through the book)
	Parameters 	: index of page to turn to left
	Returns 	: 
	Info 		: 
*/
void PAPERBOOK::turnPaperPageToLeft(const int index)
{

	// PP: because opening & turning float speeds differ, set all leaves to right of turning one to turning float speed, so's to avoid collision
	for(int i=index-1; i>=0; i--)
	{
		this->getPaperPage(i)->floatSpeed=PAPERPAGE_FLOATSPEED_TURN;
	}
	this->getPaperPage(index)->turn((-(HALFPI-PAPERPAGE_MID_SPINE_REST_ANGLE)));
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::gotoPageNumer
	Purpose 	: go to the page at the specified index
	Parameters 	: page number to turn to, whether or not to change page instantly
	Returns 	: true if the page index is valid, else false
	Info 		: this overrides BOOK::gotoPageNumber; page numbers are zero-based
*/
bool PAPERBOOK::gotoPageNumber(const int index, const bool instant)
{
	int		oldPageIndex;

	oldPageIndex=this->currentPageIndex;

	bkPrintf("PAPERBOOK::gotoPageNumber: cur %d, new %d\n", this->currentPageIndex, index);

	if(!BOOK::gotoPageNumber(index, instant))
	{
		return false;
	}

	// PP: right, all the pages between current page and target page, flip right now!

	int		p;		// PP: loop counter
	int		firstPage, lastPage;

	if (this->restrictingChapter)
	{
		// NH: If first page run the firstPageCallback
		if (index == this->restrictingChapter->startPage)
		{
			if (this->firstPageCallback != NULL)
				this->firstPageCallback();
		}
		// NH: If last page run the lastPageCallback
		else if (index == this->restrictingChapter->endPage - 1)
		{
			if (this->lastPageCallback != NULL)
				this->lastPageCallback();
		}
		// NH: else run the standard callback
		else if (this->centralPageCallback != NULL)
		{
			this->centralPageCallback();
		}
	}
	else
	{
		// NH: If first page run the firstPageCallback
		if (index == this->getNumPages() - 2)
		{
			if (this->firstPageCallback != NULL)
				this->firstPageCallback();
		}
		// NH: If last page run the lastPageCallback
		else if (index>>1 == 0)
		{
			if (this->lastPageCallback != NULL)
				this->lastPageCallback();
		}
		// NH: else run the standard callback
		else if (this->centralPageCallback != NULL)
		{
			this->centralPageCallback();
		}
	}


	if(PAPERBOOK::getPaperPageIndex(index) > PAPERBOOK::getPaperPageIndex(oldPageIndex))
	{
		for(p=PAPERBOOK::getPaperPageIndex(oldPageIndex)+1; p<=PAPERBOOK::getPaperPageIndex(index); p++)
		{
			this->turnPaperPageToLeft(p);
		}

		if (bmRand()%2)
			PlaySample("pageturn1.wav", 255);
		else
			PlaySample("pageturn2.wav", 255);
	}
	else if(PAPERBOOK::getPaperPageIndex(index) < PAPERBOOK::getPaperPageIndex(oldPageIndex))
	{
		for(p=PAPERBOOK::getPaperPageIndex(oldPageIndex); p>PAPERBOOK::getPaperPageIndex(index); p--)
		{
			this->turnPaperPageToRight(p);
		}

		if (bmRand()%2)
			PlaySample("pageturn1.wav", 255);
		else
			PlaySample("pageturn2.wav", 255);
	}

	return true;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::nextPage
	Purpose 	: turn to the next leaf
	Parameters 	: (opt/false)change page instantly (currently ignored by this overload)
	Returns 	: ptr to current page (after the page change)
	Info 		: this overrides BOOK::nextPage, to skip a page if nec, to ensure a leaf turn
*/
PAGE* PAPERBOOK::nextPage(const bool instant)
{
	int		newPageIndex;
	int		lastLeftPage;

	lastLeftPage=(this->getNumPages()-1)&(~1);
	
	// PP: if the current page index is odd (right hand leaf), progress by 1 page, to cause a leaf turn
	if(this->currentPageIndex & 1)
	{
		// PP: note: no looping, it'd detract from the effect of the physical book representation
		newPageIndex=MIN(this->currentPageIndex+1, lastLeftPage);// PP: can only turn to an even-index page
	}
	else// PP: if the current page index is even (left hand leaf), progress by 2 pages, to cause a leaf turn
	{
		// PP: note: no looping, it'd detract from the effect of the physical book representation
		newPageIndex=MIN(this->currentPageIndex+2, lastLeftPage);// PP: can only turn to an even-index page
	}

	this->gotoPageNumber(newPageIndex, instant);

	return this->currentPage;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::prevPage
	Purpose 	: turn to the previous page leaf
	Parameters 	: (opt/false)change page instantly (currently ignored by this overload)
	Returns 	: ptr to current page (after the page change)
	Info 		: this overrides BOOK::nextPage, to skip a page if nec, to ensure a leaf turn
*/
PAGE* PAPERBOOK::prevPage(const bool instant)
{
	int newPageIndex;
	
	// PP: if the current page index is odd (right hand leaf), progress by -2 pages, to cause a leaf turn
	if(this->currentPageIndex & 1)
	{
		// PP: note: no looping, it'd detract from the effect of the physical book representation
		newPageIndex=MAX(this->currentPageIndex-2, 0);		
	}
	else// PP: if the current page index is even (left hand leaf), progress by -1 page, to cause a leaf turn
	{
		// PP: note: no looping, it'd detract from the effect of the physical book representation
		newPageIndex=MAX(this->currentPageIndex-2, 0);
	}

	this->gotoPageNumber(newPageIndex, instant);

	return this->currentPage;
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::shutdown
	Purpose 	: shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides BOOK::shutdown
*/
void PAPERBOOK::shutdown(void)
{
	// PP: base-class shutdown
	BOOK::shutdown();

	// PP: if Babel has shut down, we don't want to be trying to free anything - it's already gone
	if(BabelHasShutdown)
	{
		return;
	}

	// TP: had to add "this->" to get this to complile on PS2
	//this->BOOK::~TAG_BOOK();

	// PP: also kill paper-page list
	this->paperPages.kill();

	// PP: clear top-page ptrs, why not
	this->leftTopPage=NULL;
	this->rightTopPage=NULL;

	// PP: clear background name
	this->backgroundName[0]='\0';

	// PP: other bits of shutdown (eg. freeing splash screen) will have been done in 'close'
}


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::pauseUpdateFunc
	Purpose 	: custom pause update function used while a paperbook is open
	Parameters 	: ptr to paperbook that is open
	Returns 	: 
	Info 		: must match the PauseUpdateFunc typedef (main.h)
*/
/*void PAPERBOOK::pauseUpdateFunc(const int32 parameter)
{
	PAPERBOOK*	book=(PAPERBOOK*)parameter;

	book->update();

	book->draw();
}*/


/*	--------------------------------------------------------------------------------
	Function 	: PAPERBOOK::setOrientation
	Purpose 	: set the orientation of the book
	Parameters 	: orientation for the book (PBO_PORTRAIT/PBO_LANDSCAPE)
	Returns 	: 
	Info 		: 
*/
void PAPERBOOK::setOrientation(const PBOrientation orientationIn)
{
	float xMargin;
	float yMargin=PAPERBOOK_MARGIN;

	this->orientation = orientationIn;

	// NH: Setup the book type specific details
	if (this->orientation == PBO_LANDSCAPE)
	{
		// PP: set the aspect ratio to match that of the paper pages in the book
		this->setAspectRatio(PAPERPAGE_HEIGHT, PAPERPAGE_TOTAL_WIDTH);	
		
		// PP: put some page margins on...
		xMargin=PAPERBOOK_MARGIN/this->aspectRatio;

		this->setBookRect(RECTANGLE(-(0.5f-yMargin), 0.5f-yMargin, -(0.5f-xMargin), 0.5f-xMargin));

		// NH: pages are wider than they are tall
		this->flags &= ~BKFLAG_TURNEDONSIDE;
	}
	else
	{
		// PP: set the aspect ratio to match that of the paper pages in the book
		this->setAspectRatio(PAPERPAGE_TOTAL_WIDTH, PAPERPAGE_HEIGHT);	
		
		// PP: put some page margins on...
		xMargin=PAPERBOOK_MARGIN/this->aspectRatio;

		this->setBookRect(RECTANGLE(-(0.5f-xMargin), 0.5f-xMargin, -(0.5f-yMargin), 0.5f-yMargin));

		// PP: pages are taller than they are wide, but they draw to a screen-sized ORT -
		// PP:		be a dear and tip them on their sides to make the most of that larger X res would you?  cheers
		this->flags |= BKFLAG_TURNEDONSIDE;
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL PAPERBOOK FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_draw
	Purpose 	: draw the current paper book
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_draw(void)
{
	currentPaperBook->privateDraw();

	// PP: TEMP TEST
 	//currentPaperBook->mainDrawDebug();
}


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_update
	Purpose 	: update the current paper book
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_update(void)
{
#ifdef _TAZDEBUG
	ASSERTM((gameStatus.currentState == GAMESTATE_PAPERBOOK), "PP: Please clear the ML_PAPERBOOK flag when calling MainLoop, unless in GAMESTATE_PAPERBOOK\n");
#endif

	currentPaperBook->privateUpdate();
}


/*	--------------------------------------------------------------------------------
	Function 	: PaperBook_init
	Purpose 	: initalise stuff for paper books
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void PaperBook_init(void)
{
	int				v;

	// PP: INITIALISE PAPER BOOK TEXT STYLES...

	TS_paperBookHeading.fontSize=1.0f;
	TS_paperBookHeading.italic=false;
	TS_paperBookHeading.vGrad=false;
	TS_paperBookHeading.outline=false;
	TS_paperBookHeading.colour1=COLOUR(90, 0, 0, 128);

	TS_paperBookBody.fontSize=0.7f;
	TS_paperBookBody.italic=false;
	TS_paperBookBody.vGrad=false;
	TS_paperBookBody.outline=false;
	TS_paperBookBody.colour1=COLOUR(0, 0, 0, 128);

#ifdef PAPERBOOK_OUTLINES

	// PP: SET UP PAGE OUTLINES...

#define PAPERBOOK_OUTLINE_WIDTH		0.02f	// PP: fraction of viewport height
#define PAPERBOOK_OUTLINE_HEIGHT	0.015f	// PP: fraction of viewport width

	int				vi=0;
	int				vi2;


	// PP: SET UP OUTLINE VERTS...

	for(v=0; v<4; v++)
	{
		BDVERTEX_SETXYZW(&page1OutlineVerts[vi], (v&2)?(videoMode.xScreen/2.0f):((videoMode.xScreen*(1.0f-PAPERBOOK_OUTLINE_HEIGHT))/2.0f),
											(v&1)?(videoMode.yScreen/2.0f):(-videoMode.yScreen/2.0f), 0.0f, 1.0f);
		BDVERTEX_SETCOLOUR(&page1OutlineVerts[vi], BLACK);

		page2OutlineVerts[vi]=page1OutlineVerts[vi];

		vi++;
	}

	for(v=0; v<4; v++)
	{
		BDVERTEX_SETXYZW(&page1OutlineVerts[vi], (v&2)?(-videoMode.xScreen/2.0f):((-videoMode.xScreen*(1.0f-PAPERBOOK_OUTLINE_HEIGHT))/2.0f),
											(v&1)?(videoMode.yScreen/2.0f):(-videoMode.yScreen/2.0f), 0.0f, 1.0f);
		BDVERTEX_SETCOLOUR(&page1OutlineVerts[vi], BLACK);

		page2OutlineVerts[vi]=page1OutlineVerts[vi];

		vi++;
	}

	vi2=vi;

	for(v=0;v<4;v++)
	{
		BDVERTEX_SETXYZW(&page1OutlineVerts[vi], (v&1)?(videoMode.xScreen/2.0f):(-videoMode.xScreen/2.0f),
											(v&2)?(videoMode.yScreen/2.0f):((videoMode.yScreen*(1.0f-PAPERBOOK_OUTLINE_WIDTH))/2.0f), 0.0f, 1.0f);
		BDVERTEX_SETCOLOUR(&page1OutlineVerts[vi], BLACK);

		vi++;
	}

	for(v=0;v<4;v++)
	{
		BDVERTEX_SETXYZW(&page2OutlineVerts[vi2], (v&1)?(videoMode.xScreen/2.0f):(-videoMode.xScreen/2.0f),
											(v&2)?(-videoMode.yScreen/2.0f):((-videoMode.yScreen*(1.0f-PAPERBOOK_OUTLINE_WIDTH))/2.0f), 0.0f, 1.0f);
		BDVERTEX_SETCOLOUR(&page2OutlineVerts[vi2], BLACK);

		vi2++;
	}

#endif// PP: def PAPERBOOK_OUTLINES

	// PP: SET UP SHADOW VERTS...

	// NH: Removed the INT definition for v from this and the next FOR. PC can't handle multiple definitions of the same variable.
	for(v=0; v<4; v++)
	{
		BDVERTEX_SETXYZW(&page1ShadowVerts[v], (v&1)?(videoMode.xScreen/2.0f):(-videoMode.xScreen/2.0f), (v&2)?(videoMode.yScreen/2.0f):(-videoMode.yScreen/2.0f), 0.0f, 1.0f);
	}

	for(v=0; v<4; v++)
	{
		BDVERTEX_SETXYZW(&page2ShadowVerts[v], (v&1)?(videoMode.xScreen/2.0f):(-videoMode.xScreen/2.0f), (v&2)?(-videoMode.yScreen/2.0f):(videoMode.yScreen/2.0f), 0.0f, 1.0f);
	}
	
	// PP: LOAD SHADOW TEXTURE
	pageShadowTexture=bkLoadTexture(NULL,"paperbookshadow.bmp",0);
}
