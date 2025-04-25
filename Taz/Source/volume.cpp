// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : volume.cpp
//   Purpose : functions to control the sfx and music volume sliders
// Component : 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "volume.h"
#include "pause.h"
#include "music.h"			// PP: music	

VOLUMESTUFF	volumeStuff;

static unsigned char		lastTempVolume;// PP: REMOUT: luckily for me this doesn't seem to be needed		=DEFAULT_SFX_VOLUME;

/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_musicSliderUpdateFunc
	Purpose 	: callback function for the music volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void FrontEnd_musicSliderUpdateFunc(const float sliderValue, SLIDER* const slider)
{
	gameStatus.globalSettings.musicVolume=(int)(sliderValue*MAX_MUSIC_VOLUME);

#ifndef STREAM_VOLUME_WORKAROUND
	bsSetGlobalAudioStreamVolume(gameStatus.globalSettings.musicVolume);
#endif// PP: ndef STREAM_VOLUME_WORKAROUND
}


/*	--------------------------------------------------------------------------------
	Function 	: FrontEnd_SFXSliderUpdateFunc
	Purpose 	: callback function for the sound effects volume slider
	Parameters 	: the value represented by the slider (0..1), ptr to slider
	Returns 	: 
	Info 		: called on every update of the slider, whether the value has changed or not
*/
void FrontEnd_SFXSliderUpdateFunc(const float sliderValue, SLIDER* const slider)
{
	// NH: whether or not an explosion has been done since the volume stopped being adjusted
	bool					doExplosion;
	static bool				compare;
	int						freq;

	gameStatus.globalSettings.sfxVolume=(int)(sliderValue*MAX_SFX_VOLUME);

	// PP: *** UNLESS WE'RE IN THE FRONTEND, frontEndMenu.audioPage WILL BE NULL!!!! ***
	if(map.sceneNumber == SCENE_FRONTEND)// PP: TEMP, TODO: fix this stuff
	{
		if((gameStatus.globalSettings.sfxVolume == lastTempVolume) && (!controller1.mb1Channel->value))
		{
			// PP: slider position has changed since last update

			if (!volumeStuff.explosionDone)
			{
				// NH: apply the old sfx volume
				bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);	
				
				if (!pauseMenuActive)
				{
					if (volumeStuff.ShovePlungerDown != compare)
					{
						// PP: play the test sample (explosion sound)
						freq=22000+(int)((1.0f-sliderValue)*44000);
						PlaySample(FRONTEND_SFX_VOLUME_SAMPLE, 255, 0, freq, (float*)NULL, BSPLAYFLAG_EXCLUSIVE);// PP: I don't think the 'exclusive' flag is working
						bkPrintf("freq: %d\n", freq);

						// PP: TEMP! - position the explosion at the detonator (hmm does that pass as Road-Runner-style slapstick humour?!)
			//			bmVectorCopy(explodePos, FrontEnd_items[FEITEM_DYNAMITE].object->actorInstance.position);

						// PP: make an explosion whose intensity matches the volume level
			//			StartCartoonExplosion(explodePos, 0.1f, sliderValue*200.0f, 10+(int)((1.0f-sliderValue)*100));
			
						// PP: there is a rule ... EXPLOSIONS SHAKE THE GODDAM CAMERA!!!!!!!!!!!!!    KABOOOOOM!!!!!!!(shake, bounce, rattle...)   yes
						camera[0].quakeOffsetLevel = sliderValue*0.1f;
						camera[0].tiltOffsetLevel = sliderValue*RAD(5.0f);
						camera[0].flags |= CAMERA_QUAKEFADE;
						
						// NH: Set plunger compare state back to false
						compare = false;
						// NH: Explosion has been done
						volumeStuff.explosionDone = true;
					}		
					else
					{
						// NH: Set the ShovePlungerDown flag so the plunger gets pushed
						volumeStuff.ShovePlungerDown = compare = true;
					}
				}
				else
				{
					// PP: play the test sample (explosion sound)
					freq=22000+(int)((1.0f-sliderValue)*44000);
					PlaySample(FRONTEND_SFX_VOLUME_SAMPLE, 255, 0, freq, (float*)NULL, BSPLAYFLAG_EXCLUSIVE);// PP: I don't think the 'exclusive' flag is working
					bkPrintf("freq: %d\n", freq);
					//PlaySample(PAUSE_SFX_VOLUME_SAMPLE, 255, 0, 0, (float*)NULL, BSPLAYFLAG_EXCLUSIVE);
					
					// NH: Explosion has been done
					volumeStuff.explosionDone = true;
				}
			}
		}
		else
		{
			volumeStuff.explosionDone = false;	
		}
	}
	else// PP: not in frontend
	{
		// NH: apply the old sfx volume
		bsSetGlobalSampleVolume(gameStatus.globalSettings.sfxVolume);

		// PP: TODO: FIX THIS STUFF AND GET THE TEST SAMPLE BACK IN
	}

	// PP: save the current temp volume level for comparison next update
	lastTempVolume=gameStatus.globalSettings.sfxVolume;
}