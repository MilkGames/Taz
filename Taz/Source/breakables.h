#ifndef __H_BREAKABLES
#define __H_BREAKABLES

#include "destructibles.h"

enum EBreakableTypes
{
	DESTRUCTIBLETYPE_ROCK,
	DESTRUCTIBLETYPE_TREE,
	ENDOFDESTRUCTIBLETYPES,
};

/* --------------------------------------------------------------------------------
   Function : DestroyObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyObject(ACTORINSTANCE *objectInstance);

/* --------------------------------------------------------------------------------
   Function : BreakObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void BreakObject(ACTORINSTANCE *objectInstance);

/* --------------------------------------------------------------------------------
   Function : BreakTazPickup
   Purpose : Function to be called when a pickup is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void BreakTazPickup(TBVector pos,char *name);

/* --------------------------------------------------------------------------------
   Function : DestroyObject
   Purpose : Function to be called when a object is to be destroyed
   Parameters : ACTORINSTANCE
   Returns : 
   Info : 
*/
void DestroyObject(ACTORINSTANCE *objectInstance, DESTRUCTIBLEDEBRISINFO *debris, int destroyOriginal);


#endif