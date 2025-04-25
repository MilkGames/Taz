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
#include "invisibleobjects.h"

/*	--------------------------------------------------------------------------------
	Function 	: CreateInvisibleObject
	Purpose 	: Create object but don't draw it
	Parameters 	: ACTORINSTANCE *
	Returns 	: 
	Info 		:
*/

void CreateInvisibleObject(ACTORINSTANCE *actor, TBVector position)
{
	SetActorPosition(actor, position);
}