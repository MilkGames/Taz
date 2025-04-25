// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Shockwave.h
//   Purpose : Shockwave effect
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __H_SHOCKWAVE
#define __H_SHOCKWAVE


#include "Colour.h"					// PP: handy colour class


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														SHOCKWAVERING

								These are the individual rings that make up the shockwave effect
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_SHOCKWAVERING
{
	float			radius;
	float			thickness;
	float			speed;
	bool			started;
	bool			finished;
	float			opacity;
	COLOUR			colour;


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVERING::init
	   Purpose : initialise the shockwave ring
	   Parameters : 
	   Returns : 
	   Info : 
	*/
	void init(void);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVERING::init
	   Purpose : initialise the shockwave ring
	   Parameters : speed
	   Returns : 
	   Info : 
	*/
	void init(const float speedIn);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVERING::TAG_SHOCKWAVERING
	   Purpose : default constructor, does universal initialisation
	   Parameters : 
	   Returns : new shockwave ring instance
	   Info : 
	*/
	TAG_SHOCKWAVERING(void);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVERING::init
	   Purpose : initialise the shockwave ring
	   Parameters : speed to start at, colour
	   Returns : 
	   Info : 
	*/
	void init(const float speedIn, const COLOUR& colourIn);


}SHOCKWAVERING, _SHOCKWAVERING BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
															SHOCKWAVE

								Shockwave effect - one or more expanding, fading, concentric rings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


typedef struct TAG_SHOCKWAVE
{

private:

	// PP: data that applies to all the rings in this shockwave...

	TBVector		pos;			// PP: centre position for the concentric rings
	float			maxRadius;		// PP: radius at which rings completely expire

	float			startSpeed;		// PP: speed rings start at
	float			endSpeed;		// PP: speed rings end at

	float			startWidth;		// PP: width of rings as they start
	float			endWidth;		// PP: width of rings as they finish

	float			timer;			// PP: timer
	float			interval;		// PP: interval between activating rings

	int				numRings;		// PP: number of rings in array
	SHOCKWAVERING*	rings;			// PP: array of rings

	int				randomSeed;		// PP: seed for random number generation - this gets updated every frame unless we're paused

public:

	bool			active;			// PP: is the shockwave active (if not it should be removed from the list)


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVE::TAG_SHOCKWAVE
	   Purpose : create the shockwave effect
	   Parameters : lots
	   Returns : new shockwave instance
	   Info : 
	*/
	TAG_SHOCKWAVE(const TBVector posIn,
					  const float maxRadiusIn,
					  const float startSpeedIn,
					  const float endSpeedIn,
					  const int numRingsIn,
					  const float intervalIn,
					  const float startWidthIn,
					  const float endWidthIn,
					  const COLOUR* const colourIn);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVE::update
	   Purpose : update the shockwave effect
	   Parameters : 
	   Returns : 
	   Info : 
	*/
	void update(void);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVE::draw
	   Purpose : draw the shockwave effect
	   Parameters : 
	   Returns : 
	   Info : 
	*/
	void draw(void);


	/* --------------------------------------------------------------------------------
	   Function : SHOCKWAVE::~TAG_SHOCKWAVE
	   Purpose : destroy the shockwave effect
	   Parameters : 
	   Returns : 
	   Info : 
	*/
	~TAG_SHOCKWAVE(void);


}SHOCKWAVE, _SHOCKWAVE BALIGN16;


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
					  const COLOUR* const colours);


/* --------------------------------------------------------------------------------
   Function		: Shockwave_shutdown
   Purpose		: shutdown the shockwave list
   Parameters	: 
   Returns		: 
   Info			: shockwaves get freed and removed from the list when they expire anyway;
					this function only does something if the game is quit while a shockwave is in progress
*/
void Shockwave_shutdown(void);


/* --------------------------------------------------------------------------------
   Function		: Shockwave_update
   Purpose		: update all shockwaves
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Shockwave_update(void);


/* --------------------------------------------------------------------------------
   Function		: Shockwave_draw
   Purpose		: draw all shockwaves
   Parameters	: 
   Returns		: 
   Info			: 
*/
void Shockwave_draw(void);


#endif // PP: ndef __H_SHOCKWAVE