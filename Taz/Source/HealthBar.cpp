// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : HealthBar.h
//   Purpose : 'Health Bar' display for player characters, where it is needed
//				(eg. when the character is perhaps too far away for the head-circling drumsticks to be of much use)
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "maths.h"				// PP: maths stuff
#include "main.h"				// PP: main stuff
#include "HealthBar.h"			// PP: 'Health Bar' display for player characters, where it is needed


// PP: initialise static health bar texture
TBTexture* HEALTHBAR::emptyTexture=NULL;
TBTexture* HEALTHBAR::fullTexture=NULL;


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::init
	Purpose 	: initialise the health bar
	Parameters 	: max health, (opt/NULL) name for the health bar
	Returns 	: 
	Info 		: this overrides PAGEITEM::init
*/
void HEALTHBAR::init(const int maxHealthIn, const char* const nameIn)
{
	PAGEITEM::init(nameIn);

	// PP: REMOUT: TEST			if(HEALTHBAR::texture == NULL)
	{
		HEALTHBAR::emptyTexture=bkLoadTexture(NULL,HEALTHBAR_DRUMSTICK_EMPTY_TEXTURE,0);
		HEALTHBAR::fullTexture=bkLoadTexture(NULL,HEALTHBAR_DRUMSTICK_FULL_TEXTURE,0);
	}

	this->maxHealth=maxHealthIn;

	this->setHealth(this->maxHealth, true);
}


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::setHealth
	Purpose 	: set the health to be represented by this health bar
	Parameters 	: the health to be represented by this health bar
	Returns 	: 
	Info 		: 
*/
void HEALTHBAR::setHealth(const int healthIn, const int instant)
{
	// PP: set the health bar's health
	this->health=healthIn;

	for(int b=0; b<this->maxHealth; b++)
	{
		// PP: set each drumstick to either on or off depending on the health
		this->drumsticks[b].on=(b<this->health);

		// PP: prevent fading if the 'instant' flag is high
		if(instant)
		{
			if(this->drumsticks[b].on)
			{
				this->drumsticks[b].life=1.0f;
			}
			else
			{
				this->drumsticks[b].life=0.0f;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::decreaseHealth
	Purpose 	: take 1 away from the health
	Parameters 	: 
	Returns 	: new health
	Info 		: 
*/
int HEALTHBAR::decreaseHealth(void)
{
	this->setHealth(this->health-1);

	return this->getHealth();
}


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::updateBrickAlphas
	Purpose 	: update the alpha values of the drumsticks to show their life values,
					taking into account the current opacity of the health bar page item
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
inline void HEALTHBAR::updateDrumstickAlphas(void)
{
	for(int b=0; b<this->maxHealth; b++)
	{
		this->drumsticks[b].alpha=(unsigned char)(MAX_ALPHA_TEX*this->opacity*this->drumsticks[b].life);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::update
	Purpose 	: update the health bar
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void HEALTHBAR::update(void)
{
	int		b;				// PP: loop counter

	PAGEITEM::update();

	for(b=0; b<this->maxHealth; b++)
	{
		if(this->drumsticks[b].on)
		{
			// PP: handle fading in...

			if(this->drumsticks[b].life < 1.0f)
			{
				// PP: fade up this drumstick only
				this->drumsticks[b].life=MIN(this->drumsticks[b].life+((1/(float)fps)/(float)HEALTHBAR_DURATION_FADEIN), 1.0f);
				break;
			}
		}
		else// PP: drumstick is off
		{
			// PP: handle fading out...

			if(this->drumsticks[b].life > 0.0f)
			{
				// PP: fade down any drumsticks that are off
				this->drumsticks[b].life=MAX(this->drumsticks[b].life-((1/(float)fps)/(float)HEALTHBAR_DURATION_FADEOUT), 0.0f);
			}
		}
	}

	// PP: show each drumstick's life through its opacity
	this->updateDrumstickAlphas();
}


/*	--------------------------------------------------------------------------------
	Function 	: HEALTHBAR::mainDraw
	Purpose 	: the core of the draw method - actually draw the health bar
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void HEALTHBAR::mainDraw(void)
{
	float				penX;
	float				penY;
	float				penXoffset;
	float				penYoffset;
	TBPrimVertex		verts[4];
	float				u, v;


	penX=(-this->maxHealth/2.0f)*HEALTHBAR_DRUMSTICK_WIDTH*HEALTHBAR_DRUMSTICK_X_SPACE;
	penY=(-HEALTHBAR_DRUMSTICK_HEIGHT/2.0f);

	for(int b=0; b<this->maxHealth; b++)
	{
		// PP: draw bone...

		bdSetTexture(0,HEALTHBAR::emptyTexture);

		if(this->drumsticks[b].life<1.0f)
		{
			// PP: GET THE POSITION OF THE BOTTOM-LEFT CORNER OF THE DRUMSTICKS...
			// PP: ~~~~~~~~~~~

			// PP: ~~~~~~~~~~~

			// PP: PLOT THE OTHER POINTS RELATIVE TO THE BOTTOM-LEFT CORNER

			for(int vert=0; vert<4; vert++)
			{
				u=(float)(vert&1);
				v=(float)((vert/2)^1);

				penXoffset=(vert&1)*HEALTHBAR_DRUMSTICK_WIDTH;
				penYoffset=(vert/2)*HEALTHBAR_DRUMSTICK_HEIGHT;

				BDVERTEX_SETXYZW(&verts[vert],penX+penXoffset,penY+penYoffset,0.0f,1.0f);
				BDVERTEX_SETRGBA(&verts[vert],128,128,128,MAX_ALPHA_TEX-this->drumsticks[b].alpha);
				BDVERTEX_SETUV(&verts[vert],u,v);
			}

			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);
		}

		// PP: draw drumstick...

		bdSetTexture(0,HEALTHBAR::fullTexture);

		if(this->drumsticks[b].life>0.0f)
		{
			// PP: GET THE POSITION OF THE BOTTOM-LEFT CORNER OF THE DRUMSTICKS...
			// PP: ~~~~~~~~~~~

			// PP: ~~~~~~~~~~~

			// PP: PLOT THE OTHER POINTS RELATIVE TO THE BOTTOM-LEFT CORNER

			for(int vert=0; vert<4; vert++)
			{
				u=(float)(vert&1);
				v=(float)((vert/2)^1);

				penXoffset=(vert&1)*HEALTHBAR_DRUMSTICK_WIDTH;
				penYoffset=(vert/2)*HEALTHBAR_DRUMSTICK_HEIGHT;

				BDVERTEX_SETXYZW(&verts[vert],penX+penXoffset,penY+penYoffset,0.0f,1.0f);
				BDVERTEX_SETRGBA(&verts[vert],128,128,128,this->drumsticks[b].alpha);
				BDVERTEX_SETUV(&verts[vert],u,v);
			}

			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,verts,4,BVERTTYPE_SINGLE);
		}
		
		penX+=HEALTHBAR_DRUMSTICK_WIDTH*HEALTHBAR_DRUMSTICK_X_SPACE;
	}
}