// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bSound.cpp
//   Purpose : Includes Babel Sound module headers. Defines B3DSOUNDIFACE and declares bInitSound and bShutdownSound
// Component : Babel Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bInitSound
	Purpose : initialise all sound functions
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bInitSound()
{
	if (!bInitSpecificWave()) {
		return FAIL;
	}

	if (!bInitGenericWave()) {
		bShutdownSpecificWave();
		return FAIL;
	}

	if (!bInitSpecificMusic()) {
		bShutdownGenericWave();
		bShutdownSpecificWave();
		return FAIL;
	}
	return OK;
}

/*	--------------------------------------------------------------------------------
	Function : bShutdownSound
	Purpose : shutdown the sound functions
	Parameters : 
	Returns : 
	Info : 
*/
void bShutdownSound()
{
	bShutdownGenericWave();
	bShutdownSpecificWave();
}
