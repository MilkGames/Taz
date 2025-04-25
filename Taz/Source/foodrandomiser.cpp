// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : fallingobjects.cpp
//   Purpose : functions to calculate path of falling object
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "actors.h"
#include "collectibles.h"
#include "maths.h"
#include "foodrandomiser.h"


/* --------------------------------------------------------------------------------
   Function : CreateRandomFood
   Purpose : 
   Parameters : 
   Returns : 
   Info : 
*/

int CreateRandomFood(TBVector position, uint32 type)
{
	int		result;

	// TP: no randomizer anymore
	if (type==COLLECTIBLETYPE_STRANGEFOOD)
	{
		result = CreateCollectibleActor("dynamite.obe", position, 0, (int)(METERTOUNIT(0.7f)), COLLECTIBLETYPE_STRANGEFOOD);
	}
	else
	{
		// TP: no more standard food
		// TP: result = CreateCollectibleActor("clubsandwich.obe", position, 0, (int)(METERTOUNIT(0.7f)), COLLECTIBLETYPE_FOOD);
	}
	return result;
}
