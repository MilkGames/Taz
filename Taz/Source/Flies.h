// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Flies.h
//   Purpose : Midges and the like
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_FLIES
#define __H_FLIES

#include "Colour.h"					// PP: handy colour struct

typedef enum _FlyType
{
	FLYTYPE_MIDGE,

	NUM_FLY_TYPES					// PP: number of fly types

}FlyType;

#define FLIES_DEFAULT_TYPE			FLYTYPE_MIDGE					// PP: default fly type
#define FLIES_DEFAULT_NUMBER		100								// PP: default number of flies in a group
#define FLIES_DEFAULT_COLOUR		COLOUR(128, 128, 128, 64)		// PP: default fly colour

#define FLIES_VISIBILITY_RANGE		METERTOUNIT(14)					// PP: logical visibility range (flies get feinter the farther away from them you are)
#define FLIES_MIN_OPACITY_FACTOR	0.2f							// PP: minimum opacity - used to stop flies fading out completely which looks wrong


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
														FLYGROUP

										A group of flies, or a swarm if you prefer.
				All flies in a single group share type and colour, and all buzz around the group's centre position.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// PP: individual fly
typedef struct TAG_FLY						
{		
	TBVector			pos;					// PP: 3D position relative to centre of group
	TBVector			velocity;				// PP: fancy word wot means how fast it is goin
	float				squaredRange;			// PP: square of this fly's range from the centre of the group
													// PP: Each fly is a bit different, to prevent a sphere becoming apparent.
													// PP: Range is squared to eliminate costly square roots in the update of each fly.

}FLY, _FLY BALIGN16;


// PP: group of flies - I suppose it should be called a swarm but that might give the wrong impression
typedef struct TAG_FLYGROUP
{
	FlyType					type;				// PP: type of flies in this group
	COLOUR					col;				// PP: colour of flies in this group
	TBVector				pos;				// PP: 3D centre pos of group
	int						numFlies;			// PP: number of flies in the group
	TBPrimVertex*			flyVerts;			// PP: 1 vert per fly
	FLY*					flies;				// PP: array of flies

	/*	--------------------------------------------------------------------------------
		Function 	: FLYGROUP::TAG_FLYGROUP
		Purpose 	: constructor: create a fly group
		Parameters 	: position of group, (opt/grey)colour, (opt/30)number of flies, (opt/midge)fly type (FLYTYPE_)
		Returns 	: new fly group instance
		Info 		: 
	*/
	TAG_FLYGROUP(TBVector posIn, const COLOUR colIn=FLIES_DEFAULT_COLOUR, const int numFliesIn=FLIES_DEFAULT_NUMBER, const FlyType typeIn=FLIES_DEFAULT_TYPE);


	/*	--------------------------------------------------------------------------------
		Function 	: FLYGROUP::~TAG_FLYGROUP
		Purpose 	: destroy a fly group
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	~TAG_FLYGROUP(void);


	/*	--------------------------------------------------------------------------------
		Function 	: FLYGROUP::draw
		Purpose 	: draw the fly group
		Parameters 	: ptr to camera with which to draw the flies (affects opacity)
		Returns 	: 
		Info 		: 
	*/
	void draw(const struct TAG_CAMERAINFO* const camera);


	/*	--------------------------------------------------------------------------------
		Function 	: FLYGROUP::update
		Purpose 	: update the fly group
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);
}FLYGROUP, _FLYGROUP BALIGN16;


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													GENERAL FLY FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: Flies_addGroup
	Purpose 	: add a group of flies to the list
	Parameters 	: position of group, colour, (opt/default)number of flies, (opt/default)fly type (FLYTYPE_)
	Returns 	: 
	Info 		: 
*/
void Flies_addGroup(TBVector pos, const COLOUR col=FLIES_DEFAULT_COLOUR, const int numFlies=FLIES_DEFAULT_NUMBER, const FlyType type=FLIES_DEFAULT_TYPE);


/*	--------------------------------------------------------------------------------
	Function 	: Flies_draw
	Purpose 	: draw all flies
	Parameters 	: ptr to camera with which to draw the flies (affects opacity)
	Returns 	: 
	Info 		: 
*/
void Flies_draw(const struct TAG_CAMERAINFO* const camera);


/*	--------------------------------------------------------------------------------
	Function 	: Flies_update
	Purpose 	: update all flies
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void Flies_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: Flies_shutdown
	Purpose 	: delete all flies
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void Flies_shutdown(void);


#endif // PP: ndef __H_FLIES