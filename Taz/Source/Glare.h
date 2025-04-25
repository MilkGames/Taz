// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Glare.h
//   Purpose : Light glare effects
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_GLARE
#define __H_GLARE


#define GLARE_DEFAULT_COLOUR			COLOUR(255, 255, 255, 127)		// PP: default glare colour: semi transparent white - useles to man and beast and man-beast and Beast Man oh God I need some sleep
#define GLARE_DEFAULT_ANGLE_RANGE		0.4f							// PP: default angle range for glare
#define GLARE_RECT_WIDTH				(videoMode.xScreen/2)// PP: 500								// PP: HALF the width of the glare quad
#define GLARE_RECT_HEIGHT				(videoMode.yScreen/2)// PP: 500								// PP: HALF the height of the glare quad


/*	--------------------------------------------------------------------------------
	Function 	: Glare_drawDirectional
	Purpose 	: draw directional light glare, eg. from the sun
	Parameters 	: direction vector, (opt/default)straight-on glare colour,
					(opt/default)angle range of glare
	Returns 	: the angle difference between the opposite of the camera dir and the light dir
	Info 		: 
*/
float Glare_drawDirectional(const VEC& dir,
						   const COLOUR& col=GLARE_DEFAULT_COLOUR,
						   const float angleRange=GLARE_DEFAULT_ANGLE_RANGE);


#endif // PP: ndef __H_GLARE