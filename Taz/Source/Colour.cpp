// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Colour.h
//   Purpose : handy colour struct
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "Colour.h"				// PP: handy colour struct

/*	--------------------------------------------------------------------------------
	Function 	: COLOUR::TAG_COLOUR
	Purpose 	: constructor - creates a colour by interpolating between two others
	Parameters 	: colour1, colour2, (opt/0.5f)mix fraction ('interpolant'?  is that what it's called?),
					(opt/false)whether or not to fade via the sum of the two colours
	Returns 	: new COLOUR instance
	Info 		: 
*/
COLOUR::TAG_COLOUR(const struct TAG_COLOUR& colour1, const struct TAG_COLOUR& colour2, const float mix, const bool sum)
{
	if(sum)
	{
		if(mix < 0.5f)
		{
			*this=(1.0f-(mix/0.5f))*colour1 + (mix/0.5f)*(colour1+colour2);
		}
		else
		{
			*this=(1.0f-((mix-0.5f)/0.5f))*(colour1+colour2) + ((mix-0.5f)/0.5f)*colour2;
		}
	}
	else
	{
		*this=(1.0f-mix)*colour1 + (mix*colour2);
	}
}