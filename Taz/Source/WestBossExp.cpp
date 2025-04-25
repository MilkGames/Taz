// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : WestBossExp.cpp
//   Purpose : Bomber-Man-style corridor explosions, only for the West Boss game
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"			// PP: global stuff
#include "LoopArray.h"		// PP: Looping array class - please use it!
#include "actors.h"			// PP: actor stuff
#include "animation.h"		// PP: animation stuff
#include "WestBoss.h"		// PP: Wile E West Boss - Yosemite Sam's ammo dump
#include "util.h"			// PP: general utility functions
#include "sfx.h"			// PP: sound effect rountines
#include "textures.h"		// PP: functions to assist in the management of textures
#include "quakecam.h"		// PP: camera shake - vital for explosion effects!
#include "display.h"		// PP: display stuff
#include "WestBossExp.h"	// PP: Bomber-Man-style corridor explosions, only for the West Boss game							
#include "Rumble.h"			// PP: pad vibrations
													
#define WBEGRID											(WESTBOSSGAME::grid)
#define WBEGRIDPOS										(WESTBOSSGAME::gridPoints)

// PP: maximum number of ball explosions that can co-exist at any time
#define MAX_WBEXPLOSIONS								4

// PP: list of explosions
static LOOPARRAY<WESTBOSSEXPLOSION, MAX_WBEXPLOSIONS>	WestBossExplosions;

// PP: texture coord distance between 'danger' chevrons: 2*(1/texture dimension)
#define WBE_MAX_CHEVRON_OFFSET							0.125f

// PP: scroll speed for 'danger' chevrons
#define WBE_CHEVRON_SPEED								0.255f

// PP: define this to get soft chevrons
//#define WBE_CHEVRON_FILTERING

// PP: texture coord offset used to scroll 'danger' chevrons
static float											WestBossExp_chevronOffset=WBE_MAX_CHEVRON_OFFSET;

// PP: chevron texture used to highlight 'danger' tiles that will be engulfed by an explosion branch
TBTexture*												WestBossExp_chevronTexture=NULL;	




/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WESTBOSSEXPBRANCH methods

								The branches (one per direction) that form a west boss explosion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::init
	Purpose 	: initialisation
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPBRANCH::init(void)
{
	this->state=WBEBSTATE_NOTUSED;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::init
	Purpose 	: initialisation
	Parameters 	: start pos, start grid X, start grid Y, velocity, range in grid squares, direction, final x&y coords, blocking x & y grid coords
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPBRANCH::init(TBVector posIn, const int startXin, const int startYin, TBVector velIn, const int rangeIn, const WBE_Direction dir, const int finalXin, const int finalYin, const int extendedFinalXin, const int extendedFinalYin)
{
	bmVectorCopy(this->vel, velIn);
	bmVectorCopy(this->pos, posIn);
	this->range=rangeIn;
	this->direction=dir;
//	this->distTraveled=0.0f;

//	this->started=false;// PP: this gives the first ball a chance to get properly init'd

	this->startX=startXin;
	this->startY=startYin;

	this->finalX=finalXin;
	this->finalY=finalYin;

	this->extendedFinalX=extendedFinalXin;
	this->extendedFinalY=extendedFinalYin;

	this->state=WBEBSTATE_PREPARED;

	// PP: update actual range
	this->updateActualRange();
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::start
	Purpose 	: start the explosion branch
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
	
#define WBE_FIREBALL_SCALE		1.0f// PP: FB_DEFAULT_SCALE
#define WBE_FIREBALL_HEAT		1.0f// PP: 1.0f

void WESTBOSSEXPBRANCH::start(void)
{
	Fireball_create(this->pos, this->vel, (this->actualRange-0.5f)*WB_SQUARE_WIDTH, WBE_FIREBALL_SCALE, WBE_FIREBALL_HEAT, WESTBOSSEXPBRANCH::fireballUpdateCallback, WBE_FIREBALLBRANCHINFO(this));

	// PP: fireball will take over from here on
	this->state=WBEBSTATE_FINISHED;
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::updateActualRange
	Purpose 	: update the actual range of the branch: find out how far it can go without hitting something
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPBRANCH::updateActualRange(void)
{
	int		dirX;
	int		dirY;

	switch(this->direction)
	{
	case WBEDIRECTION_UP:
		{
			dirX=0;
			dirY=1;
		}
		break;

	case WBEDIRECTION_DOWN:
		{
			dirX=0;
			dirY=(-1);
		}
		break;

	case WBEDIRECTION_LEFT:
		{
			dirX=(-1);
			dirY=0;
		}
		break;

	case WBEDIRECTION_RIGHT:
		{
			dirX=1;
			dirY=0;
		}
		break;
	}

	TBVector	centrePos;
	int			gridX;
	int			gridY;
	TBVector	distVec;
	float		dist;

	this->actualRange=0;

	for(int square=0; square<this->range; square++)
	{
		gridX=this->startX+(square*dirX);
		gridY=this->startY+(square*dirY);

		// PP: NEW: well, is that position even a ground square?!
		if(WBEGRID[gridY][gridX] != WBGRID_GROUND) goto done;

		SETVECTOR(centrePos, WestBoss_game.gridPoints[gridY][gridX][X], WestBoss_game.gridPoints[gridY][gridX][Y], WestBoss_game.gridPoints[gridY][gridX][Z], 1.0f);

		// PP: find out if that centre pos is on top of anything...

		// PP: EXPLODE CRATES ON CONTACT...
/*
		for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
		{
			if(wbcrate[i].notUsed || wbcrate[i].exploded || wbcrate[i].finished) continue;

			bmVectorSub(distVec, wbcrate[i].pos(), centrePos);
			dist=bmVectorLen(distVec);

			if(dist < WBEB_CRATEEXPLODE_RANGE)
			{
				goto done;// PP: hit something
			}
		}

		for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
		{
			if(wbstnt[i].notUsed || wbstnt[i].exploded || wbstnt[i].finished) continue;

			bmVectorSub(distVec, wbstnt[i].pos(), centrePos);
			dist=bmVectorLen(distVec);

			if(dist < WBEB_CRATEEXPLODE_RANGE)
			{
				goto done;// PP: hit something
			}
		}
*/
		// PP: EXPLODE DYNAMITE ON CONTACT...

#define WBEB_DYNAMITEEXPLODE_RANGE		METERTOUNIT(1.5f)		// PP: max distance at which an explosion branch will explode dynamite

		LA_FOREACH(WestBoss_game.dynamiteList, WBDYNAMITE, dynamite)

			if(dynamite->state == WBDYNAMITESTATE_LANDED)
			{		
				bmVectorSub(distVec, dynamite->pos(), centrePos);
				dist=bmVectorLen(distVec);

				if(dist < WBEB_DYNAMITEEXPLODE_RANGE)
				{
					goto done;// PP: hit something			
				}
			}
		
		LA_ENDFOREACH(WestBoss_game.dynamiteList)

		this->actualRange++;
	}

done:;

}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::update
	Purpose 	: update the explosion branch
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPBRANCH::update(void)
{
/*	if(WBE_FIREBALLBRANCHINFO(this).reaches(wbtaz.gridX, wbtaz.gridY))
	{
		bkPrintf("********   REaCHING   *********\n");
	}*/
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBEXPBRANCH::fireballUpdateCallback
	Purpose 	: custom update callback for FIREBALLs used by WESTBOSSEXPLOSIONs
	Parameters	: ptr to fireball, ptr to branch to which fireball belongs
	Returns		: 
	Info		: must match the FB_UpdateCallback typedef (Fireball.h)
*/

#define WBE_Y_OFFSET					METERTOUNIT(1.5f)

void WESTBOSSEXPBRANCH::fireballUpdateCallback(FIREBALL* const fireball, const long32 context)
{
	WBE_FIREBALLBRANCHINFO		branchInfo(context);
	int							gridX, gridY;

	if(fireball->state == FBSTATE_EMITTING)
	{	
		gridX=WESTBOSSGAME::getGridX(fireball->pos[X]);
		gridY=WESTBOSSGAME::getGridY(fireball->pos[Z]);

		if(WBEGRID[gridY][gridX] != WBGRID_GROUND)
		{
			// PP: hit sumfin - stop emitting
			fireball->stop();
		}
		else
		{
			WestBoss_game.sitPointOnGrid(fireball->pos, WBE_Y_OFFSET);
			
			// PP: check we haven't hit something...

			// PP: EXPLODE CRATES ON CONTACT...

			int i;
			//TBVector2D	distVec2D;
			TBVector2D	aPos2D;
			TBVector2D	bPos2D;
			float		dist;

			for(i=0; i<WB_NUM_EXPLOSIVE_CRATES; i++)
			{
				if(wbcrate[i].notUsed || wbcrate[i].exploded || wbcrate[i].finished) continue;

				aPos2D[X]=fireball->pos[X];
				aPos2D[Y]=fireball->pos[Z];

				bPos2D[X]=wbcrate[i].pos()[X];
				bPos2D[Y]=wbcrate[i].pos()[Z];

				dist=bmVector2DDistance(aPos2D, bPos2D);

				if(dist < WBEB_CRATEEXPLODE_RANGE)
				{
					wbcrate[i].kaboom(branchInfo.getYang());

					wbsam.sulk(false);

					// PP: hit sumfin - stop emitting
					fireball->stop();
				}
			}

			for(i=0; i<WB_NUM_SUPER_TNT_CRATES; i++)
			{
				if(wbstnt[i].notUsed || wbstnt[i].exploded || wbstnt[i].finished) continue;

				aPos2D[X]=fireball->pos[X];
				aPos2D[Y]=fireball->pos[Z];

				bPos2D[X]=wbstnt[i].pos()[X];
				bPos2D[Y]=wbstnt[i].pos()[Z];

				dist=bmVector2DDistance(aPos2D, bPos2D);

				if(dist < WBEB_CRATEEXPLODE_RANGE)
				{
					wbstnt[i].kaboom(branchInfo.getYang());

					wbsam.sulk(true);

					// PP: hit sumfin - stop emitting
					fireball->stop();
				}
			}


			// PP: EXPLODE DYNAMITE ON CONTACT...

#define WBEB_DYNAMITEEXPLODE_RANGE		METERTOUNIT(1.5f)		// PP: max distance at which an explosion branch will explode dynamite

			LA_FOREACH(WestBoss_game.dynamiteList, WBDYNAMITE, dynamite)

				if(dynamite->state == WBDYNAMITESTATE_LANDED)
				{		
					aPos2D[X]=fireball->pos[X];
					aPos2D[Y]=fireball->pos[Z];

					bPos2D[X]=dynamite->pos()[X];
					bPos2D[Y]=dynamite->pos()[Z];

					dist=bmVector2DDistance(aPos2D, bPos2D);

					if(dist < WBEB_DYNAMITEEXPLODE_RANGE)
					{
						dynamite->kaboom();

						// PP: make sure the explosion doesn't go any farther
		//				this->state=WBEBSTATE_FADING;				
					}
				}
			
			LA_ENDFOREACH(WestBoss_game.dynamiteList)


			// PP: HURT/PROPEL TAZ ON IMPACT...

			// PP: if taz is on a danger square
			if(branchInfo.reaches(wbtaz.gridX, wbtaz.gridY))
			{

#define WBEB_TAZHURT_RANGE		(WB_SQUARE_WIDTH)	// PP: max distance at which an explosion branch will explode dynamite// PP: there might be a problem if you make this much lower, but you wouldn't do that, would you?

				aPos2D[X]=fireball->pos[X];
				aPos2D[Y]=fireball->pos[Z];

				bPos2D[X]=wbtaz.pos()[X];
				bPos2D[Y]=wbtaz.pos()[Z];

				dist=bmVector2DDistance(aPos2D, bPos2D);

				if(dist < WBEB_TAZHURT_RANGE)
				{
					// PP: temp test
					//if(normalise((wbtaz.pos()-VEC(fireball->pos)).setY(0)).dotProduct(normalise(VEC(fireball->vel).setY(0))) > 0.5f)

					// PP: Hmm, this is a wee bit dodge
					//if(FindShortestAngle(VEC(fireball->pos).yAng(wbtaz.pos()), toYAng(branchInfo.getDir())) < QUARTPI)
					{
						wbtaz.getHitByExplosion(&branchInfo);
					}
				}
			}
		}// PP: end if ground square
	}// PP: end 'if emiiting'
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPBRANCH::draw
	Purpose 	: draw a west boss explosion branch
	Parameters	: x & y grid pos, range
	Returns		: ptr to the newly created WestBoss explosion
	Info		: 
*/

// PP: texture used to draw 'danger tile' effect
#define WBE_CHEVRON_TEXTURE			"extras\\dangerchevrons.bmp"
#define WBE_CHEVRON_UVDIST			0.125f
#define WBE_CHEVRON_COLOUR			COLOUR(128, 0, 0, WBE_CHEVRON_ALPHA_NORMAL)

#ifdef WBE_CHEVRON_FILTERING
#define WBE_CHEVRON_ALPHA_NORMAL	30
#define WBE_CHEVRON_ALPHA_FEINT		WBE_CHEVRON_ALPHA_NORMAL	
#else// PP: ifdef WBE_CHEVRON_FILTERING else
#define WBE_CHEVRON_ALPHA_NORMAL	20
#define WBE_CHEVRON_ALPHA_FEINT		8
#endif// PP: def WBE_CHEVRON_FILTERING

#define WBE_CHEVRON_WIDTH			0.5f

void WESTBOSSEXPBRANCH::draw(void)
{
	// PP: only gonna draw if in 'prepared' state
	if(this->state != WBEBSTATE_PREPARED) return;

	// PP: test: only draw explosion range for player 1's screen
	if(gameStatus.multiplayer.currentScreen != 0) return;

	// PP: if the branch has a range < 2
	if(this->actualRange<2) return;

	// PP: highlight squares that will be engulfed by this branch...

	int		dirX;
	int		dirY;

	switch(this->direction)
	{
	case WBEDIRECTION_UP:
		{
			dirX=0;
			dirY=1;
		}
		break;

	case WBEDIRECTION_DOWN:
		{
			dirX=0;
			dirY=(-1);
		}
		break;

	case WBEDIRECTION_LEFT:
		{
			dirX=(-1);
			dirY=0;
		}
		break;

	case WBEDIRECTION_RIGHT:
		{
			dirX=1;
			dirY=0;
		}
		break;
	}

	int				gridX;
	int				gridY;
	VEC				vertPos1, vertPos2;
	TBPrimVertex	verts[4];
	float			xShift, zShift, shift;
	char			axis;

	// PP: set up first two vert positions
	vertPos1=this->pos+VEC(dirY*WBE_CHEVRON_WIDTH*WB_SQUARE_WIDTH, 0.0f, dirX*WBE_CHEVRON_WIDTH*WB_SQUARE_HEIGHT);
	vertPos2=this->pos-(vertPos1-this->pos);

	for(int v=0; v<4; v++)
	{
		// PP: set danger colour - aye, rouge m'n
		BDVERTEX_SETCOLOUR(&verts[v], WBE_CHEVRON_COLOUR);
	}

	xShift=dirX*WB_SQUARE_WIDTH;
	zShift=dirY*WB_SQUARE_HEIGHT;

	if(xShift)
	{
		shift=(0.5f*xShift);
		axis=X;
	}
	else
	{
		shift=(0.5f*zShift);
		axis=Z;
	}

	vertPos1[axis]+=shift;
	vertPos2[axis]+=shift;

	// PP: draw the inner tri...

	BDVERTEX_SETVEC(&verts[1], this->pos);
	// PP: set texture coords
	BDVERTEX_SETUV(&verts[1], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f));
	BDVERTEX_SETA(&verts[1], 0);

	BDVERTEX_SETVEC(&verts[2], vertPos1);
	BDVERTEX_SETVEC(&verts[3], vertPos2);
	BDVERTEX_SETUV(&verts[2], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST*0.5f)));
	BDVERTEX_SETUV(&verts[3], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST*0.5f)), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f));

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,&verts[1],3,BVERTTYPE_SINGLE);

	BDVERTEX_SETA(&verts[1], WBE_CHEVRON_ALPHA_NORMAL);

	//*/

	// PP: draw the first, half-size, quad...

	BDVERTEX_SETUV(&verts[0], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST*0.5f)));
	BDVERTEX_SETUV(&verts[1], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST*0.5f)), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST*0.5f));
	BDVERTEX_SETUV(&verts[2], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+WBE_CHEVRON_UVDIST));
	BDVERTEX_SETUV(&verts[3], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST+WBE_CHEVRON_UVDIST), WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST));

	// PP: set the first two verts of this quad to the positions wot we got...

	BDVERTEX_SETVEC(&verts[0], vertPos1);
	BDVERTEX_SETVEC(&verts[1], vertPos2);

	// PP: shift the two vert positions along by a half square in the correct direction...

	vertPos1[axis]+=shift;
	vertPos2[axis]+=shift;

	// PP: set the next two verts of this quad to the new positions wot we got...

	BDVERTEX_SETVEC(&verts[2], vertPos1);
	BDVERTEX_SETVEC(&verts[3], vertPos2);

	if(this->actualRange<3)
	{
		BDVERTEX_SETA(&verts[2], WBE_CHEVRON_ALPHA_FEINT);
		BDVERTEX_SETA(&verts[3], WBE_CHEVRON_ALPHA_FEINT);
	}

	// PP: draw the quad
	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);
/*
	if(xShift)
	{
		vertPos1[X]+=shift;
		vertPos2[X]+=shift;
	}
	else
	{
		vertPos1[Z]+=shift;
		vertPos2[Z]+=shift;
	}
*/
	// PP: set texture coords
	BDVERTEX_SETUV(&verts[0], WestBossExp_chevronOffset, WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST));
	BDVERTEX_SETUV(&verts[1], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST), WestBossExp_chevronOffset);

	for(int square=1; square<(this->actualRange-1); square++)
	{
		gridX=this->startX+(square*dirX);
		gridY=this->startY+(square*dirY);

		// PP: set the first two verts of this quad to the positions wot we got...

		BDVERTEX_SETVEC(&verts[0], vertPos1);
		BDVERTEX_SETVEC(&verts[1], vertPos2);

		// PP: shift the two vert positions along by 1 square in the correct direction...

		if(xShift)
		{
			shift=xShift;
			axis=X;
		}
		else
		{
			shift=zShift;
			axis=Z;
		}

		vertPos1[axis]+=shift;
		vertPos2[axis]+=shift;

		// PP: ...and set their Y pos to match ground
		vertPos1[Y]=vertPos2[Y]=WestBoss_game.getHeightOnGrid(gridX+0.5f, gridY+0.5f);

		// PP: set the next two verts of this quad to the new positions wot we got...

		BDVERTEX_SETVEC(&verts[2], vertPos1);
		BDVERTEX_SETVEC(&verts[3], vertPos2);

		if(square == this->actualRange-2)
		{
			BDVERTEX_SETA(&verts[2], WBE_CHEVRON_ALPHA_FEINT);
			BDVERTEX_SETA(&verts[3], WBE_CHEVRON_ALPHA_FEINT);
		}
		
		// PP: draw the quad - for good texturing reasons we gotta draw quads one at a time - sorry!
		bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);
	}

	// PP: draw the end tri...

	VEC		tempVec;

	tempVec=VEC(this->pos[X]+(dirX*(this->actualRange-0.5f)*WB_SQUARE_WIDTH), vertPos1[Y], this->pos[Z]+(dirY*(this->actualRange-0.5f)*WB_SQUARE_HEIGHT));

	BDVERTEX_SETVEC(&verts[1], tempVec);
	// PP: set texture coords
	BDVERTEX_SETUV(&verts[1], WestBossExp_chevronOffset+((WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST/**0.5f*/))*0.5f), WestBossExp_chevronOffset+((WBE_CHEVRON_UVDIST+(WBE_CHEVRON_UVDIST/**0.5f*/))*0.5f));
	BDVERTEX_SETA(&verts[1], WBE_CHEVRON_ALPHA_FEINT);


	BDVERTEX_SETUV(&verts[2], WestBossExp_chevronOffset, WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST));
	BDVERTEX_SETUV(&verts[3], WestBossExp_chevronOffset+(WBE_CHEVRON_UVDIST), WestBossExp_chevronOffset);

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,&verts[1],3,BVERTTYPE_SINGLE);

/*
	// PP: TEMP TEST: highlight finalx&y
	SETVECTOR(centrePos, WestBoss_game.gridPoints[this->finalY][this->finalX][X], WestBoss_game.gridPoints[this->finalY][this->finalX][Y]+METERTOUNIT(1), WestBoss_game.gridPoints[this->finalY][this->finalX][Z], 1.0f);

	utilDrawSphere(centrePos);
*/
}



/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													WESTBOSSEXPLOSION methods

										Bomber-Man explosions for the West Boss game only
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPLOSION::update
	Purpose 	: update
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPLOSION::update(void)
{
	this->finished=true;

	for(int b=0; b<4; b++)
	{
		if(this->branches[b].state != WBEBSTATE_NOTUSED)
		{
			if(this->branches[b].state != WBEBSTATE_FINISHED)
			{
				this->branches[b].update();
				this->finished=false;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPLOSION::init
	Purpose 	: initialisation
	Parameters 	: x grid pos, y grid pos, range
	Returns 	: this
	Info 		: 
*/
#define BE_BRANCH_RANGE		METERTOUNIT(6.0f)

// PP: TEMP
#define MAX_SQUARES		4

WESTBOSSEXPLOSION* WESTBOSSEXPLOSION::init(const int xIn, const int yIn, const int rangeIn)
{
	int x,y;		// PP: loop counters
	TBVector	vel;
	int			maxNumSquares;
	int			finalX, finalY, extendedFinalX, extendedFinalY;		// PP: grid coords of blocking square of each branch

	this->startX=xIn;
	this->startY=yIn;

	this->range=rangeIn;

	// PP: init balls
	TBVector2D	gridPos;

	gridPos[X]=this->startX+0.5f;
	gridPos[Y]=this->startY+0.5f;

	WestBoss_game.evaluateGridPos(gridPos, this->pos);

	y=this->startY;

	// PP: get left range

	maxNumSquares=0;

	finalY=extendedFinalY=this->startY;

	for(x=this->startX; x>=0; x--)
	{
		if(WBEGRID[y][x] == WBGRID_GROUND)
		{
			maxNumSquares++;

		//	if(maxNumSquares == WBE_MAX_RANGE) break;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares > 1)
	{
		// PP: start branch
		SETVECTOR(vel, -WBE_BRANCH_SPEED, 0.0f, 0.0f, 0.0f);
		finalX=this->startX-(MIN(MAX_SQUARES-1, maxNumSquares-1));
		extendedFinalX=this->startX-(maxNumSquares-1);
		this->branches[WBEDIRECTION_LEFT].init(pos, this->startX, this->startY, vel, /*maxNumSquares*/MAX_SQUARES, WBEDIRECTION_LEFT, finalX, finalY, extendedFinalX, extendedFinalY);

		this->numBranches++;
	}
	else
	{
		this->branches[WBEDIRECTION_LEFT].init();
	}

	// PP: get right range

	maxNumSquares=0;

	for(x=this->startX; x<WB_SQUARES_WIDE; x++)
	{
		if(WBEGRID[y][x] == WBGRID_GROUND)
		{
			maxNumSquares++;

		//	if(maxNumSquares == WBE_MAX_RANGE) break;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares > 1)
	{
		// PP: start branch
		SETVECTOR(vel, WBE_BRANCH_SPEED, 0.0f, 0.0f, 0.0f);
		finalX=this->startX+(MIN(MAX_SQUARES-1, maxNumSquares-1));
		extendedFinalX=this->startX+(maxNumSquares-1);
		this->branches[WBEDIRECTION_RIGHT].init(pos, this->startX, this->startY, vel, /*maxNumSquares*/MAX_SQUARES, WBEDIRECTION_RIGHT, finalX, finalY, extendedFinalX, extendedFinalY);

		this->numBranches++;
	}
	else
	{
		this->branches[WBEDIRECTION_RIGHT].init();
	}

	x=this->startX;

	// PP: get down range

	maxNumSquares=0;

	finalX=extendedFinalX=this->startX;

	for(y=this->startY; y>=0; y--)
	{
		if(WBEGRID[y][x] == WBGRID_GROUND)
		{
			maxNumSquares++;

		//	if(maxNumSquares == WBE_MAX_RANGE) break;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares > 1)
	{
		// PP: start branch
		SETVECTOR(vel, 0.0f, 0.0f, -WBE_BRANCH_SPEED, 0.0f);
		finalY=this->startY-(MIN(MAX_SQUARES-1, maxNumSquares-1));
		extendedFinalY=this->startY-(maxNumSquares-1);
		this->branches[WBEDIRECTION_DOWN].init(pos, this->startX, this->startY, vel, /*maxNumSquares*/MAX_SQUARES, WBEDIRECTION_DOWN, finalX, finalY, extendedFinalX, extendedFinalY);

		this->numBranches++;
	}
	else
	{
		this->branches[WBEDIRECTION_DOWN].init();
	}

	// PP: get up range

	maxNumSquares=0;

	for(y=this->startY; y<WB_SQUARES_HIGH; y++)
	{
		if(WBEGRID[y][x] == WBGRID_GROUND)
		{
			maxNumSquares++;

		//	if(maxNumSquares == WBE_MAX_RANGE) break;
		}
		else
		{
			// PP: oops, hit a wall
			break;
		}
	}

	if(maxNumSquares > 1)
	{
		// PP: start branch
		SETVECTOR(vel, 0.0f, 0.0f, WBE_BRANCH_SPEED, 0.0f);
		finalY=this->startY+(MIN(MAX_SQUARES-1, maxNumSquares-1));
		extendedFinalY=this->startY+(maxNumSquares-1);
		this->branches[WBEDIRECTION_UP].init(pos, this->startX, this->startY, vel, /*maxNumSquares*/MAX_SQUARES, WBEDIRECTION_UP, finalX, finalY, extendedFinalX, extendedFinalY);

		this->numBranches++;
	}
	else
	{
		this->branches[WBEDIRECTION_UP].init();
	}

	// PP: this explosion is not finished yet
	this->finished=false;

	// PP: return this
	return this;
	// PP: hmm that didn't really need a comment did it!
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPLOSION::start
	Purpose 	: start the explosion
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPLOSION::start(void)
{
	// PP: start all branches that have a range > 0...

	for(int dir=0; dir<4; dir++)
	{
		if(this->branches[dir].state != WBEBSTATE_NOTUSED)
		{
			this->branches[dir].start();
		}
	}

	// PP: start effects to support an explosion - boom noise, camera shake, pad rumble
	Fireball_explosionSupportEffects(this->pos);

	// PP: If there's more than one branch...
// PP: REMOUT: TEST	if(numBranches > 1)
	{
		// PP: if Taz is right on the explosion...
		if((this->startX == wbtaz.gridX)&&(this->startY == wbtaz.gridY))
		{
			// PP: blow taz straight up in the air (stop giggling Tony)
			wbtaz.getHitByExplosion(NULL);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPLOSION::shutdown
	Purpose 	: shutdown
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPLOSION::shutdown(void)
{
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXPLOSION::draw
	Purpose 	: draw
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WESTBOSSEXPLOSION::draw(void)
{
	for(int b=0; b<4; b++)
	{
		if(this->branches[b].state != WBEBSTATE_NOTUSED)
		{
			if(this->branches[b].state != WBEBSTATE_FINISHED)
			{
				this->branches[b].draw();
			}
		}
	}
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												WBE_FIREBALLBRANCHINFO methods

						32-bit struct used to pass westboss explosion branch details on to their fireballs.
								Safer & more efficient than using a pointer to the original branch.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WBE_FIREBALLBRANCHINFO::reaches
	Purpose 	: find out if the branch reaches a specified grid square
	Parameters 	: grid X, grid Y
	Returns 	: true if the branch reaches the grid square, false otherwise
	Info 		: 
*/
bool WBE_FIREBALLBRANCHINFO::reaches(const int gridX, const int gridY)
{
	if(gridX == signed(this->startX))
	{
		if(gridY > signed(this->startY))
		{
			return (gridY <= signed(this->targetY));
		}
		else if(gridY < signed(this->startY))
		{
			return (gridY >= signed(this->targetY));
		}
		else// PP: equal x and y
		{
			return true;
		}
	}
	else if(gridY == signed(this->startY))
	{
		if(gridX > signed(this->startX))
		{
			return (gridX <= signed(this->targetX));
		}
		else if(gridX < signed(this->startX))
		{
			return (gridX >= signed(this->targetX));
		}
		else// PP: equal x and y
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: WBE_FIREBALLBRANCHINFO::makeImpossible
	Purpose 	: fill the branch info struct with impossible values
					(so it'll never compare positively with any real one)
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void WBE_FIREBALLBRANCHINFO::makeImpossible(void)
{
	this->startX
		=this->startY
		=this->targetX
		=this->targetY
		=this->extendedTargetX
		=this->extendedTargetY
		=(1<<4);				// PP: set all 5 bits of each high
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
												GENERAL WEST BOSS EXPLOSION FUNCTIONS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: WestBossExp_init
	Purpose 	: initialise west boss explosions
	Parameters	: 
	Returns		: 
	Info		: 
*/
void WestBossExp_init(void)
{
	// PP: load 'danger' chevron texture
	WestBossExp_chevronTexture=bkLoadTexture(NULL, WBE_CHEVRON_TEXTURE, 0);

	// PP: clear the list - DUH!
	WestBossExplosions.init();
}


/*	--------------------------------------------------------------------------------
	Function 	: WESTBOSSEXP_create
	Purpose 	: create a WestBoss explosion
	Parameters	: x & y grid pos, range
	Returns		: ptr to the newly created WestBoss explosion
	Info		: 
*/
WESTBOSSEXPLOSION* WestBossExp_create(const int x, const int y, const int range)
{
	return WestBossExplosions.getNew()->init(x, y, range);
}


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_shutdown
   Purpose		: shutdown the explosion list
   Parameters	: 
   Returns		: 
   Info			: explosions get freed and removed from the list when they expire anyway;
					this function only does something if the game is quit while an explosion is in progress
*/
void WestBossExp_shutdown(void)
{
	LA_FOREACHDO(WestBossExplosions, shutdown());
}


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_update
   Purpose		: update all bomber-man explosions
   Parameters	: 
   Returns		: 
   Info			: 
*/
void WestBossExp_update(void)
{
	LA_FOREACH(WestBossExplosions, WESTBOSSEXPLOSION, explosion)

		explosion->update();

		if(explosion->finished)
		{
			WestBossExplosions.discardOldest();
		}

	LA_ENDFOREACH(WestBossExplosions)

	// PP: shift danger chevrons along...

	WestBossExp_chevronOffset-=(WBE_CHEVRON_SPEED*rawFTime);
	if(WestBossExp_chevronOffset<0.0f)
	{
		WestBossExp_chevronOffset+=WBE_MAX_CHEVRON_OFFSET;
	}
}


/* --------------------------------------------------------------------------------
   Function		: WestBossExp_draw
   Purpose		: draw all west boss explosions
   Parameters	: 
   Returns		: 
   Info			: only need to draw danger tiles - actual fireball gets drawn elsewhere
*/
void WestBossExp_draw(void)
{
	if(WestBossExplosions.size == 0)
	{
		return;
	}

	// PP: at the moment there's just the danger chevron effect, so set up renderstates & what-not for those here

	bdSetRenderState(BDRENDERSTATE_ZWRITE, FALSE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ZTEST, BDZTESTMODE_LESSTHAN, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

#ifdef WBE_CHEVRON_FILTERING
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_LINEAR, 0);
#else// PP: ifdef WBE_CHEVRON_FILTERING else
	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, BDTEXTUREFILTER_POINT, 0);
#endif// PP: def WBE_CHEVRON_FILTERING

	bdSetTexture(0, WestBossExp_chevronTexture);

#if(BPLATFORM == PS2)
	bdSetZBias(-50000.0f);// PP: God I hate this!
#else
	bdSetZBias(-7.5f);
#endif

	bdSetIdentityObjectMatrix();

	// PP: draw each explosion - NOTE THAT THIS JUST DRAWS DANGER CHEVRONS - FIREBALLS GET DRAWN SOMEWHERE TOTALLY DIFFERENT
	LA_FOREACHDO(WestBossExplosions, draw());

	bdSetRenderState(BDRENDERSTATE_TEXTUREFILTER, gameStatus.gfxSettings.filterMode, 0);
	bdSetZBias(0.0f);
}