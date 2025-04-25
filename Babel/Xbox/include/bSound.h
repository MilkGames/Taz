// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bSound.h
//   Purpose : include all Babel Sound module headers
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_H__
#define __BSOUND_H__

// ********************************************************************************
// constants and macros

// define this symbol if our platform supports the 3D sound interface
#define B3DSOUNDIFACE


// ********************************************************************************
// include all required headers

#include <dsound.h>


#include "bSound/xbARAM.h"
#include "bSound/xbWave.h"
#include "bSound/wave.h"
#include "bSound/xbAudioStream.h"
#include "bSound/xbAudioStreamADPCM.h"
#include "bSound/xbAudioStreamWMA.h"
#include "bSound/xbPlayList.h"
#include "bSound/xbDSPDefault.h"
#include "bSound/lipSync.h"


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bInitSound
	Purpose : initialise all sound functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/

int bInitSound();


/*	--------------------------------------------------------------------------------
	Function : bShutdownSound
	Purpose : shutdown the sound functions
	Parameters : 
	Returns : 
	Info : 
*/

void bShutdownSound();

#endif		// __BSOUND_H__
