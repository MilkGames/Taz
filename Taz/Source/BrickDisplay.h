// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : BrickDisplay.h
//   Purpose : 'Brick Wall' score display for the Zoo boss game
//				// PP: ********* NOW CANNIBALISED FOR WEST BOSS CRATE DISPLAY ONLY *********
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BRICKDISPLAY_H__
#define __BRICKDISPLAY_H__


#include "TextFX.h"																// PP: Text effects, eg. button glyphs, coloured text
																			
																			
#define BRICKDISPLAY_NUM_BRICKS				4									// PP: maximum number of score points representable by a brick display
#define BRICKDISPLAY_INITIAL_SCORE			BRICKDISPLAY_NUM_BRICKS				// PP: score represented by the brick display when it starts
																			
#define BRICKDISPLAY_DURATION_FADEIN		0.5f								// PP: time taken for a brick on the display to fade 
#define BRICKDISPLAY_DURATION_FADEOUT		BRICKDISPLAY_DURATION_FADEIN		// PP: time taken for a brick on the display to fade
																			
#define BRICKDISPLAY_BRICK_TEXTURE			"extras\\supercrateicon.bmp"		// PP: texture for drawing bricks
																			
#define BRICKDISPLAY_BRICKS_PER_ROW			2									// PP: number of bricks in each row
											
#define BRICKDISPLAY_BRICK_HEIGHT			0.1f// PP: REMOUT: must now be expressed as a fraction of the screen height			45.0f								// PP: height of each brick in the brick display
#define BRICKDISPLAY_BRICK_WIDTH			BRICKDISPLAY_BRICK_HEIGHT			// PP: width of each brick in the brick display
#define BRICKDISPLAY_BRICK_X_SPACE			0.85f								// PP: space between left hand side of one brick and left hand side of the next, as a fraction of the full brick width (brick sprites overlap for isometric effect)
#define BRICKDISPLAY_BRICK_Y_SPACE			0.71f								// PP: as above but for the Y axis


typedef struct TAG_BRICKDISPLAY		:	public PAGEITEM
{

private:

	struct
	{
		bool			on;						// PP: is the brick on (includes fading in but not fading out)
		float			life;					// PP: how 'alive' the brick is
		unsigned char	alpha;					// PP: final alpha of the brick
												
	}bricks[BRICKDISPLAY_NUM_BRICKS];			
												
	int					score;					// PP: score represented by this brick display


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::updateBrickAlphas
		Purpose 	: update the alpha values of the bricks to show their life values,
						taking into account the current opacity of the brick display page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void updateBrickAlphas(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::mainDraw
		Purpose 	: the core of the draw method - actually draw the brick display
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


	static TBTexture*	texture;				// PP: texture for bricks

public:


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::TAG_BRICKDISPLAY
		Purpose 	: [default] constructor; creates a new brick display
		Parameters 	: (opt/NULL)name for brick display
		Returns 	: new BRICKDISPLAY instance
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_BRICKDISPLAY(const char* const nameIn=NULL)
	{
		TAG_BRICKDISPLAY::init(nameIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::getRequiredHeight
		Purpose 	: get the minimum required height for a BRICKDISPLAY
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	inline float getRequiredHeight(void)
	{
		return this->getAlignScale()*(ROUNDUP(BRICKDISPLAY_NUM_BRICKS/2.0f)*BRICKDISPLAY_BRICK_HEIGHT);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::getRequiredWidth
		Purpose 	: get the minimum required width for a BRICKDISPLAY
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	inline float getRequiredWidth(void)
	{
		// PP: 2.5 because of the way the bricks are stacked - this IS NOT A BODGE DAMN YOU!
		return this->getAlignScale()*(2.5f*BRICKDISPLAY_BRICK_WIDTH);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::init
		Purpose 	: initialise the brick display
		Parameters 	: (opt/NULL)name for the brick display
		Returns 	: 
		Info 		: this overrides PAGEITEM::init
	*/
	void init(const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::setScore
		Purpose 	: set the score to be represented by this brick display
		Parameters 	: the score to be represented by this brick display
		Returns 	: 
		Info 		: 
	*/
	void setScore(const int scoreIn, const int instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::decreaseScore
		Purpose 	: take 1 away from the score
		Parameters 	: 
		Returns 	: new score
		Info 		: 
	*/
	int decreaseScore(void);


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::getScore
		Purpose 	: get the score represented by this brick display
		Parameters 	: 
		Returns 	: the score represented by this brick display
		Info 		: 
	*/
	inline int getScore(void) const
	{
		return this->score;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: BRICKDISPLAY::update
		Purpose 	: update the brick display
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}BRICKDISPLAY, _BRICKDISPLAY BALIGN16;


#endif // PP: __BRICKDISPLAY_H__
