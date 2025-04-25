// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : BrickDisplay.cpp
//   Purpose : 'Brick Wall' score display for the Zoo boss game
//				// PP: ********* NOW CANNIBALISED FOR WEST BOSS CRATE DISPLAY ONLY *********
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "maths.h"				// PP: maths stuff
#include "main.h"				// PP: main stuff

#include "BrickDisplay.h"		// PP: 'Brick Wall' score display for the Zoo boss game


// PP: initialise static brick display texture
TBTexture* BRICKDISPLAY::texture=NULL;


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::init
	Purpose 	: initialise the brick display
	Parameters 	: (opt/NULL) name for the brick display
	Returns 	: 
	Info 		: this overrides PAGEITEM::init
*/
void BRICKDISPLAY::init(const char* const nameIn)
{
	PAGEITEM::init(nameIn);

	// PP: REMOUT: TEST			if(BRICKDISPLAY::texture == NULL)
	{
		BRICKDISPLAY::texture=bkLoadTexture(NULL,BRICKDISPLAY_BRICK_TEXTURE,0);
	}

	this->setScore(BRICKDISPLAY_INITIAL_SCORE, true);
}


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::setScore
	Purpose 	: set the score to be represented by this brick display
	Parameters 	: the score to be represented by this brick display
	Returns 	: 
	Info 		: 
*/
void BRICKDISPLAY::setScore(const int scoreIn, const int instant)
{
	// PP: set the brick display's score
	this->score=scoreIn;

	for(int b=0; b<BRICKDISPLAY_NUM_BRICKS; b++)
	{
		// PP: set each brick to either on or off depending on the score
		this->bricks[b].on=(b<this->score);

		// PP: prevent fading if the 'instant' flag is high
		if(instant)
		{
			if(this->bricks[b].on)
			{
				this->bricks[b].life=1.0f;
			}
			else
			{
				this->bricks[b].life=0.0f;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::decreaseScore
	Purpose 	: take 1 away from the score
	Parameters 	: 
	Returns 	: new score
	Info 		: 
*/
int BRICKDISPLAY::decreaseScore(void)
{
	this->setScore(this->score-1);

	return this->getScore();
}


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::updateBrickAlphas
	Purpose 	: update the alpha values of the bricks to show their life values,
					taking into account the current opacity of the brick display page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
inline void BRICKDISPLAY::updateBrickAlphas(void)
{
	for(int b=0; b<BRICKDISPLAY_NUM_BRICKS; b++)
	{
		this->bricks[b].alpha=(unsigned char)(MAX_ALPHA_TEX*this->opacity*this->bricks[b].life);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::update
	Purpose 	: update the brick display
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BRICKDISPLAY::update(void)
{
	int		b;				// PP: loop counter

	PAGEITEM::update();

	for(b=0; b<BRICKDISPLAY_NUM_BRICKS; b++)
	{
		if(this->bricks[b].on)
		{
			// PP: handle fading in...

			if(this->bricks[b].life < 1.0f)
			{
				// PP: fade up this brick only
				this->bricks[b].life=MIN(this->bricks[b].life+((1/(float)fps)/(float)BRICKDISPLAY_DURATION_FADEIN), 1.0f);
				break;
			}
		}
		else// PP: brick is off
		{
			// PP: handle fading out...

			if(this->bricks[b].life > 0.0f)
			{
				// PP: fade down any bricks that are off
				this->bricks[b].life=MAX(this->bricks[b].life-((1/(float)fps)/(float)BRICKDISPLAY_DURATION_FADEOUT), 0.0f);
			}
		}
	}

	// PP: show each brick's life through its opacity
	this->updateBrickAlphas();
}


/*	--------------------------------------------------------------------------------
	Function 	: BRICKDISPLAY::mainDraw
	Purpose 	: the core of the draw method - actually draw the brick display
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void BRICKDISPLAY::mainDraw(void)
{
	float				penX;
	float				penY;
	float				penXoffset;
	float				penYoffset;
	TBPrimVertex		verts[4];
	float				u, v;

	bdSetTexture(0,BRICKDISPLAY::texture);

	for(int b=0; b<BRICKDISPLAY_NUM_BRICKS; b++)
	{
		if(this->bricks[b].life>0.0f)
		{
			// PP: GET THE POSITION OF THE BOTTOM-LEFT CORNER OF THE BRICK...
			// PP: ~~~~~~~~~~~

			// PP: is the brick on the left, or the right?
			penX=((b&1)?0:-1)*BRICKDISPLAY_BRICK_WIDTH*BRICKDISPLAY_BRICK_X_SPACE;

			// PP: is the brick on a row staggered to the left, or the right?
			penX+=((float)(((b/2)&1)?1:(-1)))*0.25f*BRICKDISPLAY_BRICK_WIDTH*BRICKDISPLAY_BRICK_X_SPACE;

			// PP: start at the base of the 'wall'
			penY= -ROUNDUP(BRICKDISPLAY_NUM_BRICKS/2.0f)*0.5f*BRICKDISPLAY_BRICK_HEIGHT*BRICKDISPLAY_BRICK_Y_SPACE;

			// PP: move up according to what row the brick is on
			penY+=(b/2)*BRICKDISPLAY_BRICK_HEIGHT*BRICKDISPLAY_BRICK_Y_SPACE;

			// PP: ~~~~~~~~~~~

			// PP: PLOT THE OTHER POINTS RELATIVE TO THE BOTTOM-LEFT CORNER

			for(int vert=0; vert<4; vert++)
			{
				u=(float)(vert&1);
				v=(float)((vert/2)^1);

				penXoffset=(vert&1)*BRICKDISPLAY_BRICK_WIDTH;
				penYoffset=(vert/2)*BRICKDISPLAY_BRICK_HEIGHT;

				BDVERTEX_SETXYZW(&verts[vert],penX+penXoffset,penY+penYoffset,0.0f,1.0f);
				BDVERTEX_SETRGBA(&verts[vert],128,128,128,this->bricks[b].alpha);
				BDVERTEX_SETUV(&verts[vert],u,v);
			}

			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);
		}
	}
}