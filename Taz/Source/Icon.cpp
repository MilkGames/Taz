// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Icon.cpp
//   Purpose : Icon effects used by the front end
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"
#include "maths.h"
#include "main.h"
#include "actors.h"				// PP: needed by camera.h
#include "Camera.h"				// PP: for camera shaking
#include "SFX.h"				// PP: sound effects
#include "Icon.h"				// PP: icon effects
#include "control.h"			// PP: pad control
#include "util.h"				// PP: Utility functions
#include "mouse.h"				// NH: Mouse functions
#include "PageItemFX.h"			// PP: Special effects for page items


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													NORMAL ICON STUFF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: ICON::chooseEffect
	Purpose 	: choose and initialise a PAGEITEMEFFECT for this textbox
	Parameters 	: mode of effect (PIEMODE_...), whether or not to recur the call down the contents heirarchy
	Returns 	: 
	Info 		: this overrides PAGEITEM::chooseEffect
*/
void ICON::chooseEffect(const PIEMode mode, const bool recur)
{
	// PP: choose an effect for this icon...
	// PP: (See macro definition in PageItemFX.h)
	PAGEITEM_CHOOSE_EFFECT(icon);
}


/*	--------------------------------------------------------------------------------
	Function 	: ICON::update
	Purpose 	: update the icon
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::update
*/
void ICON::update(void)
{
	// PP: IMPORTANT: base-class update (includes effects update)
	PAGEITEM::update();

	// JW: Not sure if this is the best place to put this
	if (this->texNameCRC && !this->tex)
		this->tex=bkLoadTextureByCRC(NULL, this->texNameCRC, 0);

	if(this->tilt)
	{
		// PP: increment global counter for icon tilting only
		this->tiltCounter+=rawFTime;
		// PP: calculate icon tilt phase
		this->tiltPhase=(fmodf(this->tiltCounter, ICON_TILT_PERIOD)/ICON_TILT_PERIOD)*TWOPI;
		// PP: calculate icon tilt angle
		if (this->lag!=1.0f)
		{
			// TP: Apply smooth factor
			// PP: bmSinApprox seems fine for this
			this->angle[Z] += ((bmSinApprox(this->tiltPhase)*this->maxTilt)-this->angle[Z]) * this->lag;
		}
		else
		{
			// PP: bmSinApprox seems fine for this
			this->angle[Z]=bmSinApprox(this->tiltPhase)*this->maxTilt;
		}
		// TP: Apply the max tilt decay factor
		if (this->maxTiltDecay!=0.0f)
		{
			this->maxTilt -= (this->maxTilt * this->maxTiltDecay) * rawFTime;
			if (this->maxTilt < 0.01f) 
			{
				// TP: zero really small tilts to stop jitter
				this->maxTilt = 0.0f;
			}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: ICON::mainDraw
	Purpose 	: the core of the draw method - actually draw the icon
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/
void ICON::mainDraw(void)
{
	TBVector	pos;

	//if(this->getDrawScale() == 0.0f)
/*		{
		pos[X]=0.0f;
		pos[Y]=0.0f;
	}
	else
	{
		pos[X]=(this->rect.midX()+)/this->getDrawScale();
		pos[Y]=(this->rect.midY()+this->offset[Y])/this->getDrawScale();
	}*/
	pos[X]=this->offset[X];
	pos[Y]=this->offset[Y];
	pos[Z]=0.0f;
	pos[W]=1.0f;

	COLOUR	tempCol;

	// PP: ****** VERY TEMP PLACEHOLDER SELECTION EFFECT!!! ******

	if(this->selected)
	{
	//	if(bmFMod(gameStatus.appTime, 0.4f)>0.2)
	//	{
			tempCol=YELLOW*this->opacity;

			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 0);
			bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_ADDITIVE, 1);

//			#define TEMPFACTOR	1.2f
//			utilDraw2DSprite(this->tex, pos, ICON_WIDTH*TEMPFACTOR*bmCos(this->angle[Y]), ICON_HEIGHT*TEMPFACTOR, this->angle[Z], tempCol);
			float selectScale;
			float selectScaleTo = 1.5f;
			for (selectScale = 1.0f; selectScale<selectScaleTo; selectScale+=0.075f)
			{
				tempCol.a = 20; 
				utilDraw2DSprite(this->tex, pos, ICON_WIDTH*selectScale*bmCosApprox(this->angle[Y]), ICON_HEIGHT*selectScale, this->angle[Z], tempCol);
			}
			
	//	}
	}
	// PP: ~~~~~~~~~~

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);

/*	if(this->selected)
	{
		tempCol=COLOUR(255, 128, 0);
	}
	else*/
	{
		// PP: fix off-whites on ps2
#if(BPLATFORM==PS2)
#define ICON_MAX_RGB	140
#else
#define ICON_MAX_RGB	128
#endif
		tempCol=COLOUR(ICON_MAX_RGB, ICON_MAX_RGB, ICON_MAX_RGB, (unsigned char)(this->opacity*128));// PP: these were 127, that's why we were seeing off-whites on ps2 icons
	}

	if(this->pageItemFlags & PIFLAG_DISABLED)
		tempCol=COLOUR(50, 50, 50, 80);

	utilDraw2DSprite(this->tex, pos, ICON_WIDTH*bmCosApprox(this->angle[Y]), ICON_HEIGHT, this->angle[Z], tempCol);
}


/*	--------------------------------------------------------------------------------
	Function 	: ICON::init
	Purpose 	: initialise the icon
	Parameters 	: (opt/NULL=placeholder)texture name, (opt/NULL=placeholder)name for icon
	Returns 	: 
	Info 		: this overrides PAGEITEM::init// PP: I wish Tech Team would use const now & again!
*/
void ICON::init(char* texName, const char* const itemName)
{
	PAGEITEM::init();

	if(texName == NULL)
	{
		this->tex=NULL;

		this->texNameCRC = 0;
	}
	else
	{
		this->tex=bkLoadTexture(NULL, texName, 0);

#ifndef INFOGRAMES
		if(this->texNameCRC != 0)
		{
			ASSERTM(0, "AW THIS IS BAD!\n");
		}
#endif// PP: def INFOGRAMES

		this->texNameCRC = bkCRC32((uchar*)texName, strlen(texName), 0);
	}

	if(itemName == NULL)
	{
		// PP: no problem, PAGEITEM constructor has already given it a default name
	}
	else
	{
		strcpy(this->name, itemName);
	}

	this->tiltPhase=0.0f;
	this->tiltCounter=0.0f;
	this->maxTilt=0.2f;
	this->maxTiltDecay=0.0f;
	this->lag = 1.0f;
	this->tilt=true;

	SETVECTOR(this->offset, 0.0f, 0.0f, 0.0f, 1.0f);
	SETVECTOR(this->angle, 0.0f, 0.0f, 0.0f, 0.0f);			// PP: no need for W; this ain't no quaternion, Man.  Four-dimensional spheres?  That's, like, waay too trippy, Dude.
}


/*	--------------------------------------------------------------------------------
	Function 	: ICON::setSelectable
	Purpose 	: enable/disable selectability for individual icons
	Parameters 	: true to enable / false to disable, (opt/false)selectable by mouse only (if parm1==true)
	Returns 	: this
	Info 		: 
*/
TAG_ICON* ICON::setSelectable(const bool on, const bool mouseOnly)
{
	if(on)
	{
		this->pageItemFlags |= PIFLAG_SELECTABLE;

		if(mouseOnly)
		{
			this->pageItemFlags |= PIFLAG_MOUSEONLY;
		}
		else
		{
			this->pageItemFlags &= ~PIFLAG_MOUSEONLY;
		}
	}
	else
	{
		this->pageItemFlags &= ~PIFLAG_SELECTABLE;
	}

	return this;
}


/* PP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
													ICONCHANGE page item type
														
				A kinda groovy way of toggling between two options, each represented by a circular icon type.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*	--------------------------------------------------------------------------------
	Function 	: ICONCHANGE::init
	Purpose 	: initialise the icon change struct
	Parameters 	: texture 1 name, texture 2 name,
						(opt/NULL)set function, (opt/NULL=default)name, (opt/ICFLAG_NONE) behaviour flags,
	Returns 	: 
	Info 		: 
*/
void ICONCHANGE::init(char* tex1in, char* tex2in, ICSetFunc const func, const char* const nameIn, const unsigned int flagsIn, const unsigned int masterStateIn)
{
	PAGEITEM::init(nameIn);

	this->icons[0].init(tex1in);
	this->icons[1].init(tex2in);

	this->setFunc=func;

	this->flags=flagsIn;

	this->counter1=0;
	this->counter2=0;

	this->state1=ICSTATE_NONE;
	this->state2=ICSTATE_NONE;

	this->masterState=masterStateIn;

	this->activeIcon= &(this->icons[this->masterState]);
	this->inactiveIcon= &(this->icons[this->masterState^1]);

	this->foremostIcon=this->activeIcon;
	this->backmostIcon=this->inactiveIcon;

	this->busy=false;

	this->update();
}


/*	--------------------------------------------------------------------------------
	Function 	: ICONCHANGE::update
	Purpose 	: update the icon change
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void ICONCHANGE::update(void)
{
	int			side;

	if (this->selected)
	{
		// NH: Calculate new slider position when using mouse
		if (mouse.active && controller1.mb1DebounceChannel->value)
		{
			if(!this->busy)
			{
				this->toggle();
			}
		}
	}

	// PP: read pad controls and respond accordingly...
	if((this->flags & ICFLAG_PADCONTROL)&&(this->selected))
	{
		// PP: click stick to toggle icons
		if(controller1.l3DebounceChannel->value != 0)
		{
			if(!this->busy)
			{
				this->toggle();
			}
		}
		else
		{
			// PP: push stick left and right to toggle vibration
			if(controller1.x1 < 0)
			{
				if(!this->busy)
				{
					this->set(0);
				}
			}
			else if(controller1.x1 > 0)
			{
				if(!this->busy)
				{
					this->set(1);
				}
			}

			// PP: use left and right on d-pad to toggle vibration
			if(controller1.dPadXChannel->value < 0)
			{
				if(!this->busy)
				{
					this->set(0);
				}
			}
			else if(controller1.dPadXChannel->value > 0)
			{
				if(!this->busy)
				{
					this->set(1);
				}
			}
		}
	}

	side= this->masterState?1:-1;

	switch(this->state1)
	{
		case ICSTATE_MOVEBACK:
		{
		//	bkPrintf("MOVEBACK\n");

			if(this->moveBackSpeed < ICONCHANGE_MIN_MOVEBACK_SPEED)
			{
				// PP: move onto the next stage (ICSTATE_WHEELSPIN)
				this->state1=ICSTATE_WHEELSPIN;
				this->counter1=0;

				// PP: swap z order of icons
				this->foremostIcon=this->activeIcon;
				this->backmostIcon=this->inactiveIcon;

				this->activeIcon->setOpacity(this->getOpacity()*ICONCHANGE_ACTIVE_ALPHA);
			}
			else
			{
				this->moveBackSpeed*=ICONCHANGE_MOVEBACK_DECEL;
				this->moveBackOffset+=this->moveBackSpeed;

				this->activeIcon->setOpacity(this->getOpacity()*(ICONCHANGE_INACTIVE_ALPHA+((ICONCHANGE_ACTIVE_ALPHA-ICONCHANGE_INACTIVE_ALPHA))*(1.0f-((this->moveBackSpeed-ICONCHANGE_MIN_MOVEBACK_SPEED)/(ICONCHANGE_MOVEBACK_U-ICONCHANGE_MIN_MOVEBACK_SPEED)))));
			}

			this->activeIcon->angle[Z] = this->moveBackAng = side*this->moveBackOffset*ICONCHANGE_RADS_PER_DIST_UNIT;
			this->activeIcon->offset[X]=side*(ICONCHANGE_INITIAL_OFFSET+this->moveBackOffset);
		}
		break;

		case ICSTATE_WHEELSPIN:
		{
		//	bkPrintf("WHEELSPIN\n");

			this->counter1+=(1/fps);

			if(this->counter1 >= ICONCHANGE_DURATION_WHEELSPIN)
			{
				// PP: move onto the next stage (ICSTATE_CHARGE)
				this->state1=ICSTATE_CHARGE;
				//this->chargePos=(ICONCHANGE_INITIAL_OFFSET+this->moveBackOffset);
				this->counter1=0;
			}
			else
			{
				this->activeIcon->angle[Z]= ((1.0f-(this->counter1/(float)ICONCHANGE_DURATION_WHEELSPIN))*((side*ICONCHANGE_WHEELSPIN_TURNS*TWOPI)+this->moveBackAng))/*+moveBackAng*/;
			}

			this->activeIcon->setOpacity(this->opacity*ICONCHANGE_ACTIVE_ALPHA);
		}
		break;

		case ICSTATE_CHARGE:
		{
			//bkPrintf("CHARGE\n");

			if(this->counter1 >= ICONCHANGE_DURATION_CHARGE)
			{
				if(this->state2 == ICSTATE_NONE)
				{
					this->state2=ICSTATE_KNOCKED;
				}
				else if(this->state2 == ICSTATE_POSTKNOCKED)
				{
					this->state2=ICSTATE_NONE;
				}

				// PP: move onto the next stage (ICSTATE_POSTKNOCKED)
				this->state1=ICSTATE_POSTCHARGE;

				// PP: position and angle the icon properly
				this->activeIcon->offset[X]=0.0f;
				this->activeIcon->angle[Z]=0.0f;

				// PP: reset tilt phase so the transition from rolling to tilting goes supersmooth
				if(this->masterState)
				{
					this->activeIcon->setTiltPhase(0.0f);
				}
				else
				{
					this->activeIcon->setTiltPhase(PI);
				}

				this->activeIcon->setOpacity(this->opacity*ICONCHANGE_ACTIVE_ALPHA);

				this->activeIcon->tilt=true;
			}
			else
			{
				float temp=ICON_WIDTH;
				float chargePos=side*(ICONCHANGE_INITIAL_OFFSET+this->moveBackOffset)*(1.0f-(this->counter1/(float)ICONCHANGE_DURATION_CHARGE));

				if(bmFAbs(chargePos) <= bmFAbs(temp))
				{
					this->state2=ICSTATE_KNOCKED;

					if(this->flags & ICFLAG_CAMSHAKE)
					{
						// PP: shake the camera, just a smidgeon
						// TP: PROBLEM
						camera[0].quakeOffsetLevel = 0.005f;
						camera[0].tiltOffsetLevel = 0.0025f;
						camera[0].flags |= CAMERA_QUAKEFADE;
					}
				}

				this->counter1+=(1/fps);

				this->activeIcon->offset[X]=chargePos;
				this->activeIcon->angle[Z]= -side*(this->counter1/(float)ICONCHANGE_DURATION_CHARGE)*ICONCHANGE_CHARGE_TURNS*TWOPI;
			//	bkPrintf("offset %5.2f counter1 %5.2f duration %5.2f\n", this->activeIcon->offset[X], this->counter1, ICONCHANGE_DURATION_CHARGE);
			}

			this->activeIcon->setOpacity(this->getOpacity()*ICONCHANGE_ACTIVE_ALPHA);
		}
		break;

		case ICSTATE_NONE:
		case ICSTATE_POSTCHARGE:
		{
			this->activeIcon->setOpacity(this->getOpacity()*ICONCHANGE_ACTIVE_ALPHA);
			this->activeIcon->offset[X]=0.0f;
			this->foremostIcon=this->activeIcon;
			this->backmostIcon=this->inactiveIcon;
		}
		break;
	}

	if(this->state2 == ICSTATE_KNOCKED)
	{
		if(this->counter2 >= ICONCHANGE_DURATION_KNOCKED)
		{
			// PP: move onto the next stage (ICSTATE_POSTKNOCKED)
			this->state2=ICSTATE_POSTKNOCKED;

			// PP: position and angle the icon properly
			this->inactiveIcon->angle[Y]=0.0f;
			this->inactiveIcon->angle[Z]=0.0f;
			this->inactiveIcon->offset[X]= -side*ICONCHANGE_INITIAL_OFFSET;

			/* PP: REMOUT: hmm not sure yet till I get sounds & rumbling

			if(this->flags & ICFLAG_CAMSHAKE)
			{
				// PP: shake the camera, just a smidgeon
				cameraInfo.quakeOffsetLevel = 0.005f;
				cameraInfo.tiltOffsetLevel = 0.0025f;
				cameraInfo.flags |= CAMERA_QUAKEFADE;
			}
			*/
		}
		else
		{
			this->counter2+=(1/fps);

			this->inactiveIcon->angle[Y]= ((this->counter2/(float)ICONCHANGE_DURATION_KNOCKED)*ICONCHANGE_SPINOUT_TURNS*TWOPI);

			this->inactiveIcon->angle[Z]*= (1.0f-(this->counter2/(float)ICONCHANGE_DURATION_KNOCKED));

			this->inactiveIcon->tilt=false;

			this->inactiveIcon->offset[X]= -side*(this->counter2/(float)ICONCHANGE_DURATION_KNOCKED)*ICONCHANGE_INITIAL_OFFSET;

			this->inactiveIcon->setOpacity(this->getOpacity()*(ICONCHANGE_INACTIVE_ALPHA+((1.0f-(this->counter2/(float)ICONCHANGE_DURATION_KNOCKED))*(ICONCHANGE_ACTIVE_ALPHA-ICONCHANGE_INACTIVE_ALPHA))));
		}
	}
	else
	{
		if(this->state2 == ICSTATE_POSTKNOCKED)
		{
			this->inactiveIcon->offset[X]= -side*ICONCHANGE_INITIAL_OFFSET;
			this->inactiveIcon->setOpacity(this->getOpacity()*ICONCHANGE_INACTIVE_ALPHA);
		}
		else// PP: if(this->state2 == ICSTATE_NONE)
		{
			if(this->busy)
			{
				this->inactiveIcon->setOpacity(this->getOpacity()*ICONCHANGE_ACTIVE_ALPHA);
			}
			else
			{
				// PP: start conditions...
				this->inactiveIcon->tilt=false;
				this->inactiveIcon->offset[X]= -side*ICONCHANGE_INITIAL_OFFSET;
				this->inactiveIcon->setOpacity(this->getOpacity()*ICONCHANGE_INACTIVE_ALPHA);
			}
		}
	}

	// PP: if both icons have stopped moving
	if((this->state1 == ICSTATE_POSTCHARGE) && (this->state2 == ICSTATE_POSTKNOCKED))
	{
		// PP: reset the icon change instance...

		// PP: set both icons to initial state
		this->state1=this->state2=ICSTATE_NONE;

		//this->inactiveIcon->opacity=this->opacity*ICONCHANGE_INACTIVE_ALPHA;

		// PP: clear the 'busy' flag now we're finished
		this->busy=false;
	}

	this->icons[0].update();
	this->icons[1].update();
}


/*	--------------------------------------------------------------------------------
	Function 	: ICONCHANGE::set
	Purpose 	: set the master state of the icon change
	Parameters 	: (opt/false)flag to specify immediate change
	Returns 	: 
	Info 		: This sets masterState, not state.
*/
void ICONCHANGE::set(const unsigned int stateIn, const bool instant)
{
	if(this->setFunc != NULL)
	{
		if(this->setFunc)
		{
			this->setFunc(stateIn);
		}
	}

	// PP: we should only continue if
	// PP: a. the 'instant' flag is set - we may just want to formalise the state we're heading towards with the current transition
	// PP: b. the requested state is different from the current state
	if(!(instant||(stateIn!=this->masterState)))
	{
		return;
	}


	if(instant)
	{
		this->state1=ICSTATE_NONE;
		this->state2=ICSTATE_NONE;
		this->busy=false;
	}
	else
	{
		this->moveBackSpeed=ICONCHANGE_MOVEBACK_U;
		this->moveBackOffset=0.0f;
		this->state1=ICSTATE_MOVEBACK;
		this->state2=ICSTATE_NONE;
		this->busy=true;
	}

	this->masterState=stateIn;

	this->inactiveIcon= &(this->icons[this->masterState^1]);
	this->activeIcon= &(this->icons[this->masterState]);

	this->counter1=0;
	this->counter2=0;
}


/*	--------------------------------------------------------------------------------
	Function 	: ICONCHANGE::toggle
	Purpose 	: toggle the master state of the icon change
	Parameters 	: (opt/false)flag to specify immediate change
	Returns 	: 
	Info 		: 
*/
void ICONCHANGE::toggle(const bool instant)
{
	this->set(this->masterState^1, instant);
}
