// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Slider.cpp
//   Purpose : Horizontal slider page-item used for controlling values, eg. music/sound fx volume.
// Component : Taz
//		Info : Named after the Windows controls that do the same thing
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"				// PP: global stuff
#include "maths.h"				// PP: maths stuff
#include "main.h"				// PP: main stuff
#include "control.h"			// PP: provide basic input and control functions
#include "Slider.h"				// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.
#include "PageItemFX.h"			// PP: Special effects for page items

#define FESLIDER_MAX_STEP		(0.5f*fTime)		// PP: distance by which a slider will be moved if the stick is pushed fully to the right and the framerate is 60fps

/*	--------------------------------------------------------------------------------
	Function 	: SLIDER::update
	Purpose 	: update the slider
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::update
*/
void SLIDER::update(void)
{
	RECTANGLE	*rect;
	float		calcVolume;

	// PP: base-class update (includes effects update)
	PAGEITEM::update();

	// PP: Prevent the slider from being adjusted while it's entering or exiting.
	// PP: TODO: a nicer way of checking this please!
	// PP: if((this->effect == NULL) || (this->effect->mode == PIEMODE_ON))// PP: TEMP I hope
	if((~this->pageItemFlags) & PIFLAG_IGNOREINPUT)
	{
		if (this->selected)
		{
			// NH: Calculate new slider position when using mouse
			if (mouse.active && controller1.mb1Channel->value)
			{
				rect = &this->screenRect;

				if (rect->right != rect->left)
				{
					calcVolume = (mouse.mousePos[X] - rect->left) / (rect->right - rect->left);
					// NH: Allow user to click just off the ends of the slider bars to get Max/Min values
					if (calcVolume >= -0.1 && calcVolume <=1.1 && mouse.mousePos[Y] <= rect->top && mouse.mousePos[Y] >= rect->bottom)
					{
						// NH: Set value of slider but also make sure the final values are between 0 and 1
						this->value = CONSTRAIN(calcVolume, 0.0f, 1.0f);
					}
				}
			}
			else if(controller1.l3DebounceChannel->value != 0)
			{
				/* PP: if the left stick is clicked, toggle the volume between none and the default level... */
				
				if(this->value > 0.0f)
				{
					this->value=0.0f;
				}
				else
				{
					this->value=1.0f;
				}
			}
			else
			{
				/* PP: control volume by pushing stick left and right (analogue control) */
				this->value = CONSTRAIN(this->value+(controller1.x1*FESLIDER_MAX_STEP), 0.0f, 1.0f);
				/* PP: control volume by pushing left and right on the d-pad (analogue control) */
				this->value =  CONSTRAIN(this->value+((controller1.dPadXChannel->value/BIMAX)*FESLIDER_MAX_STEP), 0.0f, 1.0f);
#if BPLATFORM == PC
				// NH: control volume by pushing left and right on the keyboard
				this->value =  CONSTRAIN(this->value+((controller1.menuXChannel->value/BIMAX)*FESLIDER_MAX_STEP), 0.0f, 1.0f);
#endif // NH: BPLATFORM == PC
			}
		}
	}

	// PP: call the update callback, whether the value has changed or not
	// NH: No. Not when fading in or out
	// PP: REMOUT: got a better check now (PIFLAG_IGNOREINPUT)		if((this->effect == NULL) || (this->effect->mode != PIEMODE_ENTER && this->effect->mode != PIEMODE_EXIT))
	if((~this->pageItemFlags) & PIFLAG_IGNOREINPUT)
	{
		if(this->updateFunc != NULL)
		{
			this->updateFunc(this->value, this);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: SLIDER::mainDraw
	Purpose 	: the core of the draw method - actually draw the slider
	Parameters 	: 
	Returns 	: 
	Info 		: this overrides PAGEITEM::mainDraw
*/

//#define SLIDER_WIDTH			(2.0f*200.0f)							// PP: width of slider
#define SLIDER_R0				this->colour1.r							// PP: slider background red
#define SLIDER_G0				this->colour1.g							// PP: slider background green
#define SLIDER_B0				this->colour1.b							// PP: slider background blue
#define SLIDER_A0				this->colour1.a							// PP: slider background alpha
#define SLIDER_R1				this->colour2.r							// PP: slider left red
#define SLIDER_G1				this->colour2.g							// PP: slider left green
#define SLIDER_B1				this->colour2.b							// PP: slider left blue
#define SLIDER_A1				this->colour2.a							// PP: slider left alpha
#define SLIDER_R2				SLIDER_R1								// PP: slider right red
#define SLIDER_G2				SLIDER_G1								// PP: slider right green
#define SLIDER_B2				SLIDER_B1								// PP: slider right blue
#define SLIDER_A2				SLIDER_A1								// PP: slider right alpha
#define SLIDER_NUM_BLOCKS		7										// PP: number of blocks in the slider
#define SLIDER_OUTLINE_WIDTH				(SLIDER_WIDTH/80.0f)		// PP: slider outline width
#define SLIDER_OUTLINE_WIDTH_HORIZONTAL		(SLIDER_OUTLINE_WIDTH*2.0f)	// PP: thickened outline width for horizontal-ish lines

#define SLIDER_KNOB_HEIGHT					(SLIDER_HEIGHT*1.10f)		// PP: inner height of slider knob
#define SLIDER_KNOB_WIDTH					(SLIDER_KNOB_HEIGHT*0.2f)	// PP: width of slider knob

#define SLIDER_LEFT				(/*this->rect.midX()*/-(SLIDER_WIDTH*0.5f))
#define SLIDER_RIGHT			(/*this->rect.midX()+*/(SLIDER_WIDTH*0.5f))
#define SLIDER_TOP				(/*this->rect.midY()+*/(SLIDER_HEIGHT*0.5f))
#define SLIDER_BOTTOM			(/*this->rect.midY()*/-(SLIDER_HEIGHT*0.5f))

// PP: calculate the colour corresponding to the horizontal position 'penX'
#define SLIDER_UPDATE_COLOURS	r=SLIDER_R0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_R1-SLIDER_R0)); \
								g=SLIDER_G0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_G1-SLIDER_G0)); \
								b=SLIDER_B0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_B1-SLIDER_B0)); \
								a=(int)(this->opacity*(SLIDER_A0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_A1-SLIDER_A0))));

// PP: TEMP!!!
#define SLIDER_SELECTED_COLOUR	RED

void SLIDER::mainDraw(void)
{
	TBPrimVertex	vert[4];		// PP: vertex array

	float			penX, penY;		// PP: x and y position variables for drawing
	
	// PP: width of each block (except the first, triangular, one)
	float			blockWidth=((SLIDER_WIDTH-SLIDER_OUTLINE_WIDTH)/SLIDER_NUM_BLOCKS)-SLIDER_OUTLINE_WIDTH;

	int				r, g, b, a;		// PP: vertex colour variables
	int				v;				// PP: vertex index

	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 1);
	bdSetRenderState(BDRENDERSTATE_ALPHABLENDMODE, BDALPHABLENDMODE_BLEND, 0);
	bdSetRenderState(BDRENDERSTATE_ALPHAENABLE, TRUE, FALSE);
	bdSetTexture(0, NULL);			// PP: disable texturing

	// PP: draw the background triangle...

	if((this->opacity > 1.0f)||(this->opacity < 0.0f))
	{
		int x=0;
	}

	BDVERTEX_SETXYZW(&vert[0],SLIDER_LEFT,SLIDER_BOTTOM,0.0f,1.0f);

	// PP: TEMP TEMP TEMP !!!!
	COLOUR	selCol;
	
	selCol=SLIDER_SELECTED_COLOUR;
	selCol.a= uchar(this->opacity*255.0f);

	// PP: TEMP!
	if(this->selected)
	{
		BDVERTEX_SETCOLOUR(&vert[0], selCol);
	}
	else
	{
		BDVERTEX_SETRGBA(&vert[0],0,0,0,(int)(this->opacity*255));
	}

	BDVERTEX_SETXYZW(&vert[1],SLIDER_RIGHT,SLIDER_TOP,0.0f,1.0f);

	// PP: TEMP!
	if(this->selected)
	{
		BDVERTEX_SETCOLOUR(&vert[1], selCol);
	}
	else
	{
		BDVERTEX_SETRGBA(&vert[1],0,0,0,(int)(this->opacity*255));
	}

	BDVERTEX_SETXYZW(&vert[2],SLIDER_RIGHT,SLIDER_BOTTOM,0.0f,1.0f);

	// PP: TEMP!
	if(this->selected)
	{
		BDVERTEX_SETCOLOUR(&vert[2], selCol);
	}
	else
	{
		BDVERTEX_SETRGBA(&vert[2],0,0,0,(int)(this->opacity*255));
	}

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLELIST,vert,3,BVERTTYPE_SINGLE);

	// PP: draw the blocks...

	penX=SLIDER_LEFT+((2*SLIDER_OUTLINE_WIDTH_HORIZONTAL)/SLIDER_HEIGHT)*SLIDER_WIDTH;

	for(int block=0;block<SLIDER_NUM_BLOCKS;block++)
	{
		v=0;

		//	SLIDER_UPDATE_COLOURS;

		r=SLIDER_R0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_R1-SLIDER_R0));
		g=SLIDER_G0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_G1-SLIDER_G0));
		b=SLIDER_B0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_B1-SLIDER_B0));
		a=(int)(this->opacity*(SLIDER_A0+(int)(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*(SLIDER_A1-SLIDER_A0))));

		if(block == 0)	// PP: first block (triangular) is best treated as a special case...
		{
			penY=SLIDER_BOTTOM+(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*SLIDER_HEIGHT)/2.0f;
			BDVERTEX_SETXYZW(&vert[v],penX,penY,0.0f,1.0f);
			BDVERTEX_SETRGBA(&vert[v],r,g,b,a);

			penX=SLIDER_LEFT+SLIDER_OUTLINE_WIDTH+blockWidth;
		}
		else
		{
			penY=SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL;
			BDVERTEX_SETXYZW(&vert[v],penX,penY,0.0f,1.0f);
			BDVERTEX_SETRGBA(&vert[v],r,g,b,a);

			v++;

			penY=SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL+(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*SLIDER_HEIGHT)-(2*SLIDER_OUTLINE_WIDTH_HORIZONTAL);
			penY=MAX(penY, SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL);
			BDVERTEX_SETXYZW(&vert[v],penX,penY,0.0f,1.0f);
			BDVERTEX_SETRGBA(&vert[v],r,g,b,a);
			
			penX+=blockWidth;
		}
		
		v++;

		SLIDER_UPDATE_COLOURS;

		penY=SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH;
		penY=MAX(penY, SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL);
		BDVERTEX_SETXYZW(&vert[v],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[v],r,g,b,a);

		v++;

		penY=SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL+(((penX-SLIDER_LEFT)/(float)SLIDER_WIDTH)*SLIDER_HEIGHT)-(2*SLIDER_OUTLINE_WIDTH_HORIZONTAL);
		penY=MAX(penY, SLIDER_BOTTOM+SLIDER_OUTLINE_WIDTH_HORIZONTAL);
		BDVERTEX_SETXYZW(&vert[v],penX,penY,0.0f,1.0f);
		BDVERTEX_SETRGBA(&vert[v],r,g,b,a);

		penX+=SLIDER_OUTLINE_WIDTH;

		// PP: TEMP?: don't draw the first notch - it's more trouble than it;s worth
		// PP: REMOUT: oh ok one more chance then			if(block > 1)
		{
			ASSERT(v+1);
			bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,v+1,BVERTTYPE_SINGLE);	// PP: draw
		}
	}

	// PP: draw the sliding knob (just a textured Gourauded quad)...

	// PP: And by 'sliding knob' you mean...?

	penX=SLIDER_LEFT+(this->value*(SLIDER_WIDTH-SLIDER_KNOB_WIDTH));
	penY=SLIDER_BOTTOM+(SLIDER_HEIGHT/2.0f)+(SLIDER_KNOB_HEIGHT/2.0f);
	BDVERTEX_SETXYZW(&vert[0],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[0],127,127,127,(int)(this->opacity*128));
	BDVERTEX_SETUV(&vert[0],0.0f,0.0f);
	
	penX+=SLIDER_KNOB_WIDTH;
	BDVERTEX_SETXYZW(&vert[1],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[1],127,127,127,(int)(this->opacity*128));
	BDVERTEX_SETUV(&vert[1],1.0f,0.0f);

	penX-=SLIDER_KNOB_WIDTH;
	penY-=SLIDER_KNOB_HEIGHT;
	BDVERTEX_SETXYZW(&vert[2],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[2],64,64,64,(int)(this->opacity*128));
	BDVERTEX_SETUV(&vert[2],0.0f,1.0f);

	penX+=SLIDER_KNOB_WIDTH;
	BDVERTEX_SETXYZW(&vert[3],penX,penY,0.0f,1.0f);
	BDVERTEX_SETRGBA(&vert[3],64,64,64,(int)(this->opacity*128));
	BDVERTEX_SETUV(&vert[3],1.0f,1.0f);

	bdSetTexture(0,bkLoadTexture(NULL,"sldrknob.bmp",0));				// PP: apply the slider knob texture

	bdDrawPrimitive(BDPRIMTYPE_TRIANGLESTRIP,vert,4,BVERTTYPE_SINGLE);	// PP: draw
}

/*	--------------------------------------------------------------------------------
	Function 	: SLIDER::getRect
	Purpose 	: get the rect of the slider
	Parameters 	: 
	Returns 	: The rect used by the slider
	Info 		: 
*/
RECTANGLE* SLIDER::getRect(void)
{
	return &this->optRect;
}
