// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : HealthBar.h
//   Purpose : 'Health Bar' display for player characters, where it is needed
//				(eg. when the character is perhaps too far away for the head-circling drumsticks to be of much use)
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __HEALTHBAR_H__
#define __HEALTHBAR_H__


#include "TextFX.h"																// PP: Text effects, eg. button glyphs, coloured text
#include "health.h"																// PP: handles anything to do with a character and its health
																			
																			
#define HEALTHBAR_MAX_DRUMSTICKS			int(TAZHEALTH_MAX)									// PP: maximum number of health points representable by a health bar
//#define HEALTHBAR_INITIAL_HEALTH			HEALTHBAR_NUM_DRUMSTICKS				// PP: health represented by the health bar when it starts
																			
#define HEALTHBAR_DURATION_FADEIN		0.5f								// PP: time taken for a drumstick on the display to fade 
#define HEALTHBAR_DURATION_FADEOUT		HEALTHBAR_DURATION_FADEIN		// PP: time taken for a drumstick on the display to fade
																			
#define HEALTHBAR_DRUMSTICK_EMPTY_TEXTURE			"extras\\drumstickbone.bmp"		// PP: texture for drawing drumsticks
#define HEALTHBAR_DRUMSTICK_FULL_TEXTURE			"extras\\drumstick.bmp"		// PP: texture for drawing drumsticks
																			
//#define HEALTHBAR_DRUMSTICKS_PER_ROW			2									// PP: number of drumsticks in each row
											
#define HEALTHBAR_DRUMSTICK_HEIGHT			0.1f// PP: REMOUT: must now be expressed as a fraction of the screen height			45.0f								// PP: height of each drumstick in the health bar
#define HEALTHBAR_DRUMSTICK_WIDTH			HEALTHBAR_DRUMSTICK_HEIGHT			// PP: width of each drumstick in the health bar
#define HEALTHBAR_DRUMSTICK_X_SPACE			0.5f								// PP: space between left hand side of one drumstick and left hand side of the next, as a fraction of the full drumstick width (drumstick sprites overlap for isometric effect)
//#define HEALTHBAR_DRUMSTICK_Y_SPACE			0.71f								// PP: as above but for the Y axis


typedef struct TAG_HEALTHBAR		:	public PAGEITEM
{

private:

	struct
	{
		bool			on;						// PP: is the drumstick on (includes fading in but not fading out)
		float			life;					// PP: how 'alive' the drumstick is
		unsigned char	alpha;					// PP: final alpha of the drumstick
												
	}drumsticks[HEALTHBAR_MAX_DRUMSTICKS];
	
	int					health;					// PP: health value represented by this health bar
												
	int					maxHealth;				// PP: maximum health value representable by this health bar


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::updateDrumstickAlphas
		Purpose 	: update the alpha values of the drumsticks to show their life values,
						taking into account the current opacity of the health bar page item
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	inline void updateDrumstickAlphas(void);

	static TBTexture*	emptyTexture;			// PP: texture for drumsticks
	static TBTexture*	fullTexture;			// PP: texture for drumsticks

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::mainDraw
		Purpose 	: the core of the draw method - actually draw the health bar
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGEITEM::mainDraw
	*/
	virtual void mainDraw(void);


public:


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::TAG_HEALTHBAR
		Purpose 	: [default] constructor; creates a new health bar
		Parameters 	: max health, (opt/NULL)name for health bar
		Returns 	: new HEALTHBAR instance
		Info 		: this overrides PAGEITEM::TAG_PAGEITEM
	*/
	TAG_HEALTHBAR(const int maxHealthIn, const char* const nameIn=NULL)
	{
		TAG_HEALTHBAR::init(maxHealthIn, nameIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::getRequiredHeight
		Purpose 	: get the minimum required height for a HEALTHBAR
		Parameters 	: 
		Returns 	: minimum required height for the item
		Info 		: this overrides PAGEITEM::getRequiredHeight
	*/
	inline float getRequiredHeight(void)
	{
		return this->getAlignScale()*HEALTHBAR_DRUMSTICK_HEIGHT;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::getRequiredWidth
		Purpose 	: get the minimum required width for a HEALTHBAR
		Parameters 	: 
		Returns 	: minimum required width for the item
		Info 		: this overrides PAGEITEM::getRequiredWidth
	*/
	inline float getRequiredWidth(void)
	{
		return this->getAlignScale()*(this->maxHealth*HEALTHBAR_DRUMSTICK_WIDTH*HEALTHBAR_DRUMSTICK_X_SPACE);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::init
		Purpose 	: initialise the health bar
		Parameters 	: max health, (opt/NULL)name for the health bar
		Returns 	: 
		Info 		: this overrides PAGEITEM::init
	*/
	void init(const int maxHealthIn, const char* const nameIn=NULL);


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::setHealth
		Purpose 	: set the health to be represented by this health bar
		Parameters 	: the health to be represented by this health bar
		Returns 	: 
		Info 		: 
	*/
	void setHealth(const int healthIn, const int instant=false);


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::decreaseHealth
		Purpose 	: take 1 away from the health
		Parameters 	: 
		Returns 	: new health
		Info 		: 
	*/
	int decreaseHealth(void);


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::getHealth
		Purpose 	: get the health represented by this health bar
		Parameters 	: 
		Returns 	: the health represented by this health bar
		Info 		: 
	*/
	inline int getHealth(void) const
	{
		return this->health;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: HEALTHBAR::update
		Purpose 	: update the health bar
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/
	void update(void);


}HEALTHBAR, _HEALTHBAR BALIGN16;


#endif // PP: __HEALTHBAR_H__
