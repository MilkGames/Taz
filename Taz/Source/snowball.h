#ifndef __H_SNOWBALL
#define __H_SNOWBALL


#define	WEAK_SNOWBALL_SPEED		(METERTOUNIT(4.0f))
#define	MEDIUM_SNOWBALL_SPEED	(METERTOUNIT(8.0f))
#define	STRONG_SNOWBALL_SPEED	(METERTOUNIT(16.0f))

enum
{
	SNOWBALL_WEAK,
	SNOWBALL_MEDIUM,
	SNOWBALL_STRONG,
};

/* --------------------------------------------------------------------------------
   Function : CreateSnowball
   Purpose : Creates a snowball, which takes the form of an extra actor
   Parameters : 
   Returns : 
   Info : 
*/

DEBRIS *CreateSnowball(ACTORINSTANCE *actorInstance,TBVector initialPosition, TBVector direction2, float	size, TBVector target, uint32 strength, char *actorName, float angOff=RAD(15.0f), float scale = 1.0f);

#endif