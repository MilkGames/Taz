// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Shockwave.cpp
//   Purpose : Shockwave effect
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"					// PP: global stuff
#include "LinkedList.h"				// PP: Linked List class - please use it!

#include "Shockwave.h"				// PP: Shockwave effect

static LINKEDLIST<SHOCKWAVE>	shockwaves;		// PP: list of shockwaves


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SHOCKWAVE methods

								Shockwave effect - one or more expanding, fading, concentric rings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVE::TAG_SHOCKWAVE
   Purpose : create the shockwave effect
   Parameters : lots
				centre pos of shockwave,
				radius at which rings expire (at which point they'll have faded to fully transparent),
				initial expansion rate of rings,
				ultimate expansion rate of rings,
				number of rings in the shockwave,
				time interval between appearance of rings,
				initial thickness of rings,
				ultimate thickness of rings,
				colour of rings (opacity fades from that specified therein to none at all)
   Returns : new shockwave effect instance
   Info : 
*/
SHOCKWAVE::TAG_SHOCKWAVE(const TBVector posIn,
							  const float maxRadiusIn,
							  const float startSpeedIn,
							  const float endSpeedIn,
							  const int numRingsIn,
							  const float intervalIn,
							  const float startWidthIn,
							  const float endWidthIn,
							  const COLOUR* const coloursIn)
{
	bmVectorCopy(this->pos, posIn);

	this->maxRadius=maxRadiusIn;

	this->startSpeed=startSpeedIn;
	this->endSpeed=endSpeedIn;

	this->numRings=numRingsIn;
		
	this->interval=intervalIn;

	this->startWidth=startWidthIn;
	this->endWidth=endWidthIn;

	this->timer=0.0f;

	// PP: create all the rings now...
	this->rings=new SHOCKWAVERING[this->numRings];

	for(int r=0; r<this->numRings; r++)
	{
		this->rings[r].init(this->startSpeed, coloursIn[r]);
	}

	// PP: activate!
	this->active=true;

	// PP: TEMP?
	// PP: update just to be safe
	this->update();
}

#define SHOCKWAVE_MIN_SPEED		0.1f

/* --------------------------------------------------------------------------------
   Function : SHOCKWAVE::update
   Purpose : update the shockwave effect
   Parameters : 
   Returns : 
   Info : 
*/
#define SHOCKWAVE_OPACITY_CUTOFF		0.15f		// PP: bit dirty?

void SHOCKWAVE::update(void)
{
	SHOCKWAVERING*		ring;
	int					numRingsFinished=0;

	// PP: update timer
	this->timer += (1/fps);

	// PP: update random seed - this won't happen if we're in pause mode; thus we can freeze the shock rings' random rotation
	this->randomSeed= *(int*)(&(gameStatus.gameTime));

	for(int r=0; r<this->numRings; r++)
	{
		ring= &(this->rings[r]);

		if(ring->finished)
		{
			numRingsFinished++;
			continue;
		}

		if(this->timer >= (r*this->interval))
		{
			if(!ring->started)
			{
				ring->started=true;
				//bkPrintf("starting ring %d\n", r);
			}

			// PP: update radius according to speed
			ring->radius += MAX(ring->speed, SHOCKWAVE_MIN_SPEED);

			// PP: update opacity according to radius
			ring->opacity= (1.0f-(ring->radius/this->maxRadius));

			// PP: update speed according to radius
			ring->speed= this->startSpeed+((ring->radius/this->maxRadius)*(this->endSpeed-this->startSpeed));

			// PP: update thickness according to radius
			ring->thickness= this->startWidth+((ring->radius/this->maxRadius)*(this->endWidth-this->startWidth));

			// PP: check for expiry
			if(ring->opacity <= SHOCKWAVE_OPACITY_CUTOFF)
			{
				ring->finished=true;
			}
		}
	}

	if(numRingsFinished == this->numRings)
	{
		this->active=false;
	}
}


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVE::draw
   Purpose : draw the shockwave effect
   Parameters : 
   Returns : 
   Info : 
*/

#define SHOCKWAVE_NUM_SECTORS		32				// PP: number of sectors in each ring
#define SHOCKWAVE_NUM_STRIPS		6				// PP: number of times the shockwave texture repeats horizontally (around the loop)

void SHOCKWAVE::draw(void)
{
	float			theta;																	// PP: angle at point being set up
	float			angleStep=(TWOPI/(float)SHOCKWAVE_NUM_SECTORS);							// PP: angles covered by each segment of the ring
	float			u;																		// PP: texture u coordinate at point being set up
	float			v;																		// PP: texture v coordinate at point being set up
	int				vert;																	// PP: vert counter
	float			uStep=(SHOCKWAVE_NUM_STRIPS/(float)SHOCKWAVE_NUM_SECTORS);				// PP: increment for texture U coord
	float			radius;																	// PP: radius at point on ring being positioned
	SHOCKWAVERING*	ring;																	// PP: pointer to ring being drawn
	TBPrimVertex	verts[(SHOCKWAVE_NUM_SECTORS*2)+2];										// PP: got to be trapezoids rather than triangles, otherwise the textures won't meet up cleanly

	// PP: seed the random number code
// PP: REMOUT: no	bmSRand(this->randomSeed);

	for(int r=this->numRings-1; r>=0; r--)
	{
		ring= &(this->rings[r]);

		if((!ring->started) || ring->finished)
		{
			continue;
		}

		vert=0;
		theta= (bmRand()/RANDMAX)*TWOPI;					// PP: randomize ring angle so's to make the effect look more chaotic & natural
		u=0.0f;

		for(int s=0; s<SHOCKWAVE_NUM_SECTORS; s++)
		{
			// PP: NOTE: I currently assume that these shockwaves will always sit on flat ground

			radius=ring->radius-(ring->thickness*0.5f);
			v=1.0f;
			BDVERTEX_SETXYZW(&verts[vert], this->pos[X]+(radius*bmSin(theta)), this->pos[Y], this->pos[Z]+(radius*bmCos(theta)), 1.0f);
			BDVERTEX_SETRGBA(&verts[vert], (int)(ring->colour.r*ring->opacity), (int)(ring->colour.g*ring->opacity), (int)(ring->colour.b*ring->opacity), 128);
			BDVERTEX_SETUV(&verts[vert], u, v);
			
			vert++;

			radius+=ring->thickness;
			v=0.0f;
			BDVERTEX_SETXYZW(&verts[vert], this->pos[X]+(radius*bmSin(theta)), this->pos[Y], this->pos[Z]+(radius*bmCos(theta)), 1.0f);
			BDVERTEX_SETRGBA(&verts[vert], (int)(ring->colour.r*ring->opacity), (int)(ring->colour.g*ring->opacity), (int)(ring->colour.b*ring->opacity), 128);
			BDVERTEX_SETUV(&verts[vert], u, v);

			theta+=angleStep;
			
			u+=uStep;
			vert++;
		}

		verts[vert]=verts[0];
		v=1.0f;
		BDVERTEX_SETUV(&verts[vert], u, v);

		vert++;

		verts[vert]=verts[1];
		v=0.0f;
		BDVERTEX_SETUV(&verts[vert], u, v);

		ASSERT(vert+1);
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP, &verts, vert+1, BVERTTYPE_SINGLE);
	}

}


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVE::~TAG_SHOCKWAVE
   Purpose : destroy the shockwave effect
   Parameters : 
   Returns : 
   Info : 
*/
SHOCKWAVE::~TAG_SHOCKWAVE(void)
{
	delete(this->rings);
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													SHOCKWAVERING methods

								These are the individual rings that make up the shockwave effect
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVERING::init
   Purpose : initialise the shockwave ring
   Parameters : 
   Returns : 
   Info : 
*/
void SHOCKWAVERING::init(void)
{
	this->radius=0.0f;
	this->started=false;
	this->finished=false;
}


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVERING::init
   Purpose : initialise the shockwave ring
   Parameters : speed to start at, colour
   Returns : 
   Info : 
*/
void SHOCKWAVERING::init(const float speedIn, const COLOUR& colourIn)
{
	this->init();

	this->speed=speedIn;

	this->colour=colourIn;
}


/* --------------------------------------------------------------------------------
   Function : SHOCKWAVERING::TAG_SHOCKWAVERING
   Purpose : default constructor, calls init
   Parameters : 
   Returns : new shockwave ring instance
   Info : 
*/
SHOCKWAVERING::TAG_SHOCKWAVERING(void)
{
	this->init();
}





/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL SHOCKWAVE FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* --------------------------------------------------------------------------------
   Function		: Shockwave_create
   Purpose		: create a shockwave
   Parameters	: what for you not read um long, self-explanatory parameter list?
					centre pos of shockwave,
					radius at which rings expire (at which point they'll have faded to fully transparent),
					initial expansion rate of rings,
					ultimate expansion rate of rings,
					number of rings in the shockwave,
					time interval between appearance of rings,
					initial thickness of rings,
					ultimate thickness of rings,
					colours of rings
   Returns		: 
   Info			: 
*/
void Shockwave_create(const TBVector pos,
					  const float maxRadius,
					  const float startSpeed,
					  const float endSpeed,
					  const int numRings,
					  const float interval,
					  const float startWidth,
					  const float endWidth,
					  const COLOUR* const colours)
{
	shockwaves.append(new SHOCKWAVE(pos, maxRadius, startSpeed, endSpeed, numRings, interval, startWidth, endWidth, colours));
}


/* --------------------------------------------------------------------------------
   Function		: Shockwave_update
   Purpose		: update all shockwaves
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Shockwave_update(void)
{	
	SHOCKWAVE*		shock;

	for(int s=shockwaves.getSize()-1; s>=0; s--)
	{
		shock=shockwaves.get(s);

		shock->update();

		if(!shock->active)
		{
			delete(shock);
			shockwaves.remove(s);
		}
	}
}


/* --------------------------------------------------------------------------------
   Function		: Shockwave_draw
   Purpose		: draw all shockwaves
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Shockwave_draw(void)
{
	// PP: TEMP TEST
	return;

	if(shockwaves.getSize() < 1)// PP: if there are no shockwaves to draw
	{
		return;
	}

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();
	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSEQUAL, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);
	bdSetTexture(0,bkLoadTexture(NULL,"extras\\shockwaveadd3.bmp",0));
	bdSetRenderState(BDRENDERSTATE_TEXTUREWRAP, BDTEXTUREWRAP_WRAPU|BDTEXTUREWRAP_CLAMPV, FALSE);

	for(int s=shockwaves.getSize()-1; s>=0; s--)
	{
		shockwaves.get(s)->draw();
	}

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
}


/* --------------------------------------------------------------------------------
   Function		: Shockwave_shutdown
   Purpose		: shutdown the shockwave list
   Parameters	: 
   Returns		: 
   Info			: shockwaves get freed and removed from the list when they expire anyway;
					this function only does something if the game is quit while a shockwave is in progress
*/
void Shockwave_shutdown(void)
{
	shockwaves.shutdown();
}