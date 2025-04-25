#ifndef __H_VOLUME
#define __H_VOLUME

#include "global.h"
#include "main.h"

#include "control.h"
#include "slider.h"				// PP: Horizontal slider page-item used for controlling values, eg. music/sound fx volume.

// PP: moved from frontend.h
#define FRONTEND_SFX_VOLUME_SAMPLE	"explosion.wav"			// PP: sample used for testing sound effects
#define PAUSE_SFX_VOLUME_SAMPLE		"burp1.wav"	// NH: sample used for testing sound effects during pause 

typedef struct TAG_VOLUMESTUFF
{
	uint32				subState;
	bool				ShovePlungerDown;
	bool				explosionDone;
}VOLUMESTUFF, _VOLUMESTUFF BALIGN16;

extern VOLUMESTUFF volumeStuff;

/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_musicSliderFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void FrontEnd_musicSliderUpdateFunc(const float sliderValue, SLIDER* const slider);


/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_SFXSliderFunc
	Purpose 	: callback function for the sound effects volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void FrontEnd_SFXSliderUpdateFunc(const float sliderValue, SLIDER* const slider);

#endif