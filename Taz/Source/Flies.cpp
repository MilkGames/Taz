// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Flies.cpp
//   Purpose : Midges and the like
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"									// PP: Global stuff
#include "main.h"									// PP: Main stuff
#include "maths.h"									// PP: Maths stuff
#include "LinkedList.h"								// PP: Linked List class - please use it!

#include "actors.h"									// PP: actor stuff for...
#include "camera.h"									// PP: camera stuff
													
#include "Flies.h"									// PP: Midges and the like

													
static LINKEDLIST<FLYGROUP>		flyGroups("flies");	// PP: linked list of fly groups

// PP: statistics specific to each fly type
typedef struct FLYSTATS
{
	float		maxSpeed;							// PP: maximum speed of any fly in the group
	float		range;								// PP: average range of flies in the group (distance units in any direction)
	float		rangeDeviation;						// PP: maximum deviation fraction from group's average range (0..1)
	float		updateFraction;						// PP: fraction of flies in whose velocity is to be updated each frame, excluding those who have reached their range (0..1)
	float		verticalScale;						// PP: vertical scale factor for the group (I've somehow got the feeling that midge swarms for example are a bit wider than they are tall, something to do with their vision no doubt)

}FLYSTATS, _FLYSTATS BALIGN16;

// PP: Array of stats for the different types of fly...
// PP: 															 maxSpeed			range				rangeDeviation	updateFraction	verticalScale
static FLYSTATS					FlyStats[NUM_FLY_TYPES]={/*		 ¯¯¯¯¯¯¯¯			¯¯¯¯¯				¯¯¯¯¯¯¯¯¯¯¯¯¯¯	¯¯¯¯¯¯¯¯¯¯¯¯¯¯	¯¯¯¯¯¯¯¯¯¯¯¯¯*/
												/*MIDGES*/		{METERTOUNIT(0.8f),	METERTOUNIT(1.2f),	0.5f,			0.1f,			0.65f}
														};


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													FLYGROUP methods

										A group of flies, or a swarm if you prefer.
				All flies in a single group share type and colour, and all buzz around the group's centre position.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: FLYGROUP::TAG_FLYGROUP
	Purpose 	: constructor: create a fly group
	Parameters 	: position of group, (opt/grey)colour, (opt/30)number of flies, (opt/midge)fly type (FLYTYPE_)
	Returns 	: new fly group instance
	Info 		: 
*/
FLYGROUP::TAG_FLYGROUP(TBVector posIn, const COLOUR colIn, const int numFliesIn, const FlyType typeIn)
{
	bmVectorCopy(this->pos, posIn);
	this->col				=colIn;
	this->numFlies			=numFliesIn;
	this->type				=typeIn;

	// PP: malloc the array of flies
	this->flies=new FLY[this->numFlies];
	ASSERT(this->flies);

	// PP: malloc the array of verts used to draw the flies
	this->flyVerts=new TBPrimVertex[this->numFlies];
	ASSERT(this->flyVerts);

	// PP: initialise flies and fly verts...
	FLY*			fly;
	float			groupRange		=FlyStats[this->type].range;
	float			maxSpeed		=FlyStats[this->type].maxSpeed;
	float			rangeDeviation	=FlyStats[this->type].rangeDeviation;
	float			flyRange;

	for(int f=0; f<this->numFlies; f++)
	{
		fly= &this->flies[f];

		// PP: RANGE ~~~~~~~~~~~~~~~~~~~~~~~

		// PP: the range of the fly group, plus or minus a random deviation
		flyRange=groupRange*((1.0f-rangeDeviation)+((bmRand()/RANDMAX)*rangeDeviation));
		
		// PP: square the range to eliminate costly square roots in the update of each fly
		fly->squaredRange=SQR(flyRange);

		// PP: POSITION ~~~~~~~~~~~~~~~~~~~~

		// PP: random offset -1..1
		SETVECTOR(fly->pos, ((bmRand()/RANDMAX)*2.0f)-1.0f, ((bmRand()/RANDMAX)*2.0f)-1.0f, ((bmRand()/RANDMAX)*2.0f)-1.0f, 0.0f);

		// PP: times fly's range
		bmVectorCMul(fly->pos, fly->pos, flyRange);

		// PP: NB that group's position and vertical scale are only taken into account when setting coordinates for the verts representing the flies

		// PP: VERTEX ~~~~~~~~~~~~~~~~~~~~~~

		// PP: a vert at each fly's position
		BDVERTEX_SETPOS(&this->flyVerts[f], fly->pos);

		// PP: and with the group's colour
		BDVERTEX_SETCOLOUR(&this->flyVerts[f], this->col);

		//this->flyVerts[(f*2)+1]=this->flyVerts[f*2];
		
		// PP: VELOCITY ~~~~~~~~~~~~~~~~~~~~

		// PP: random
		SETVECTOR(fly->velocity, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, 0.0f);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: FLYGROUP::~TAG_FLYGROUP
	Purpose 	: destroy a fly group
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
FLYGROUP::~TAG_FLYGROUP(void)
{
	delete (this->flies);
	delete (this->flyVerts);
}


/*	--------------------------------------------------------------------------------
	Function 	: FLYGROUP::draw
	Purpose 	: draw the fly group
	Parameters 	: ptr to camera with which to draw the flies (affects opacity)
	Returns 	: 
	Info 		: 
*/
void FLYGROUP::draw(const CAMERAINFO* const camera)
{
	// PP: As the flies get further away from the camera, the pixels representing them get bigger
	// PP: Reduce their opacity to compensate...

	float			dist	=bmVectorDistance(camera->pos, this->pos);
	unsigned char	alpha	=(unsigned char)(this->col.a * MAX(FLIES_MIN_OPACITY_FACTOR, (1.0f-(dist/FLIES_VISIBILITY_RANGE))));

	for(int f=0; f<this->numFlies; f++)
	{
		BDVERTEX_SETA(&this->flyVerts[f], alpha);
	}
	
	// PP: draw the flies
	ASSERT(this->numFlies);
	bdDrawPrimitive(BDPRIMTYPE_POINTLIST, this->flyVerts, this->numFlies, BVERTTYPE_SINGLE);
}


/*	--------------------------------------------------------------------------------
	Function 	: FLYGROUP::update
	Purpose 	: update the fly group
	Parameters 	: 
	Returns 	: 
	Info 		: Pantaloons!  Pantaloons!  Five dorrah!
*/
void FLYGROUP::update(void)
{
	float			squaredDist;
	TBVector		tempVec;
	float			maxSpeed			=FlyStats[this->type].maxSpeed;
	float			verticalScale		=FlyStats[this->type].verticalScale;
	int				updateInterval		=(int)(FlyStats[this->type].updateFraction * this->numFlies);
	int				flyToUpdate			=bmRand()%updateInterval;
	FLY*			fly;
	
	for(int f=0; f<this->numFlies; f++)
	{
		fly= &this->flies[f];

		// PP: if the fly has rached its maximum range...
		squaredDist=bmVectorSquaredLen(fly->pos);
		if(squaredDist > fly->squaredRange)
		{
			// PP: restrain the fly to its maximum range...
			bmVectorCMul(fly->pos, fly->pos, (fly->squaredRange/squaredDist));

			// PP: and give the fly a random velocity - within a few updates it will be heading back into the group to join the PAR-TAY!
			SETVECTOR(fly->velocity, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, 0.0f);
		}
		else
		{
			// PP: if this fly is special...
			if((f%updateInterval) == flyToUpdate)
			{
				// PP: give it a random velocity
				SETVECTOR(fly->velocity, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, (((bmRand()/RANDMAX)*2.0f)-1.0f)*maxSpeed, 0.0f);
			}
		}

		// PP: apply fly's velocity
		bmVectorCMul(tempVec, fly->velocity, fTime);
		bmVectorAdd(fly->pos, fly->pos, tempVec);

		// PP: set vertex position for this fly - NB it's only at this late stage that we take into account group position and group's vertical scale
		BDVERTEX_SETXYZW(&this->flyVerts[f], this->pos[X]+fly->pos[X], this->pos[Y]+(verticalScale*fly->pos[Y]), this->pos[Z]+fly->pos[Z], 0.0f);
		//this->flyVerts[(f*2)+1]=this->flyVerts[f*2];
// PP: TEST		BDVERTEX_SETXYZW(&this->flyVerts[(f*2)+1], this->pos[X]+fly->pos[X]+100.0f, this->pos[Y]+(verticalScale*fly->pos[Y])+100.0f, this->pos[Z]+fly->pos[Z]+100.0f, 0.0f);
	}
}


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
void Flies_addGroup(TBVector pos, const COLOUR col, const int numFlies, const FlyType type)
{
	FLYGROUP*		group;

	// PP: create the new group
	group=new FLYGROUP(pos, col, numFlies, type);
	ASSERT(group);

	// PP: add the group to the list
	flyGroups.append(group);
}


/*	--------------------------------------------------------------------------------
	Function 	: Flies_draw
	Purpose 	: draw all flies
	Parameters 	: ptr to camera with which to draw the flies (affects opacity)
	Returns 	: 
	Info 		: 
*/
void Flies_draw(const CAMERAINFO* const camera)
{
	if(flyGroups.getSize() == 0) return;

	FLYGROUP*		group;

	bdSetProjectionMode(BDISPLAY_PROJECTIONMODE3D);
	bdSetIdentityObjectMatrix();

	bdSetRenderState(BDRENDERSTATE_ZWRITE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetTexture(0,NULL);

	// PP: draw each group in the list...
	for(int g=flyGroups.getSize()-1; g>=0; g--)
	{
		group=flyGroups.get(g);

		group->draw(camera);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: Flies_update
	Purpose 	: update all flies
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void Flies_update(void)
{
	FLYGROUP*		group;

	// PP: update each group in the list...
	for(int g=flyGroups.getSize()-1; g>=0; g--)
	{
		group=flyGroups.get(g);

		group->update();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: Flies_shutdown
	Purpose 	: delete all flies
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void Flies_shutdown(void)
{
	flyGroups.kill();
}