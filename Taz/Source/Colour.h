// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Colour.h
//   Purpose : handy colour struct
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_COLOUR
#define __H_COLOUR

#include "maths.h"


// PP: ========== PROTOTYPES ==========

/*	--------------------------------------------------------------------------------
	Function 	: UtilGenerateRandomNumber
	Purpose 	: Generates a random number between given bounds quickly
	Parameters 	: upper, lower bounds
	Returns 	: the number that you're after
*/
float UtilGenerateRandomNumber(float upperLimit, float lowerLimit);// PP: util.cpp

// PP: ================================


// PP: you know it makes sense
typedef struct TAG_COLOUR
{
	unsigned char			r;
	unsigned char			g;
	unsigned char			b;
	unsigned char			a;


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::init
		Purpose 	: initialise a colour struct
		Parameters 	: red, green, blue, alpha (0..128? 255?) I'm really not sure any more!
		Returns 	: 
		Info 		: 
	*/
	inline void init(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		this->r=red;
		this->g=green;
		this->b=blue;
		this->a=alpha;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::TAG_COLOUR
		Purpose 	: default constructor: create a colour struct with default values
		Parameters 	: 
		Returns 	: new colour instance
		Info 		: 
	*/
	inline TAG_COLOUR(void)
	{
		this->init(255, 0, 255, 255);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::TAG_COLOUR
		Purpose 	: create a colour struct
		Parameters 	: red, green, blue, alpha (0..128? 255?) I'm really not sure any more!
		Returns 	: new colour instance
		Info 		: 
	*/
	inline TAG_COLOUR(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
	{
		this->init(red, green, blue, alpha);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::TAG_COLOUR
		Purpose 	: create a colour struct
		Parameters 	: red, green, blue (0..128? 255?) I'm really not sure any more!
		Returns 	: new colour instance
		Info 		: 
	*/
	inline TAG_COLOUR(const unsigned char red, const unsigned char green, const unsigned char blue)
	{
		this->init(red, green, blue, 255);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::TAG_COLOUR
		Purpose 	: constructor - creates a colour by interpolating between two others
		Parameters 	: colour1, colour2, (opt/0.5f)mix fraction ('interpolant'?  is that what it's called?),
						(opt/false)whether or not to fade via the sum of the two colours
		Returns 	: new COLOUR instance
		Info 		: 
	*/
	TAG_COLOUR(const struct TAG_COLOUR& colour1, const struct TAG_COLOUR& colour2, const float mix=0.5f, const bool sum=false);


	/*	--------------------------------------------------------------------------------
		Function 	: COLOUR::inverse
		Purpose 	: get the inverse of the colour
		Parameters 	: (opt/255)max value for all components
		Returns 	: the inverse of the colour
		Info 		: 
	*/
	TAG_COLOUR inverse(const unsigned char max=255)
	{
		return TAG_COLOUR(max, max, max, max)-(*this);
	}


	// PP: COLOUR binary addition - includes alpha
	friend struct TAG_COLOUR operator + (const struct TAG_COLOUR& col1, const struct TAG_COLOUR& col2);

	// PP: COLOUR binary subtraction - includes alpha
	friend struct TAG_COLOUR operator - (const struct TAG_COLOUR& col1, const struct TAG_COLOUR& col2);

	// PP: COLOUR subtraction of an integer from all components
	friend struct TAG_COLOUR operator - (const struct TAG_COLOUR& col1, const unsigned char sub);

	// PP: COLOUR multiplication with a constant - includes alpha
	friend struct TAG_COLOUR operator * (const struct TAG_COLOUR& col, const float factor);
	friend struct TAG_COLOUR operator * (const float factor, const struct TAG_COLOUR& col);

}COLOUR, _COLOUR BALIGN16;


/*	--------------------------------------------------------------------------------
	Function 	: randomColour
	Purpose 	: construct a random colour
	Parameters 	: 
	Returns 	: a random colour
	Info 		: // PP: rgb 0..255, alpha 255
*/
inline COLOUR randomColour(void)
{
	return COLOUR((unsigned char)UtilGenerateRandomNumber(255, 0), (unsigned char)UtilGenerateRandomNumber(255, 0), (unsigned char)UtilGenerateRandomNumber(255, 0));
}


// PP: COLOUR binary addition - includes alpha
inline COLOUR operator + (const struct TAG_COLOUR& col1, const struct TAG_COLOUR& col2)
{
	return COLOUR(	(unsigned char) MININT(255, col1.r + col2.r),
					(unsigned char) MININT(255, col1.g + col2.g),
					(unsigned char) MININT(255, col1.b + col2.b),
					(unsigned char) MININT(255, col1.a + col2.a));
}

// PP: COLOUR binary subtraction - includes alpha
inline COLOUR operator - (const struct TAG_COLOUR& col1, const struct TAG_COLOUR& col2)
{
	return COLOUR(	(unsigned char) MAXINT(0, col1.r - col2.r),
					(unsigned char) MAXINT(0, col1.g - col2.g),
					(unsigned char) MAXINT(0, col1.b - col2.b),
					(unsigned char) MAXINT(0, col1.a - col2.a));
}

// PP: COLOUR subtraction of an integer from all components
inline COLOUR operator - (const struct TAG_COLOUR& col1, const unsigned char sub)
{
	return COLOUR(	(unsigned char) MAXINT(0, col1.r - sub),
					(unsigned char) MAXINT(0, col1.g - sub),
					(unsigned char) MAXINT(0, col1.b - sub),
					(unsigned char) MAXINT(0, col1.a - sub));
}

// PP: COLOUR multiplication with a constant - includes alpha
inline COLOUR operator * (const struct TAG_COLOUR& col, const float val)
{
	return COLOUR(	(unsigned char)(col.r*val),
					(unsigned char)(col.g*val),
					(unsigned char)(col.b*val),
					(unsigned char)(col.a*val));
}

// PP: COLOUR multiplication with a constant - includes alpha
inline COLOUR operator * (const float val, const struct TAG_COLOUR& col)
{
	return col*val;
}

// PP: COLOUR division by a constant - includes alpha
inline COLOUR operator / (const struct TAG_COLOUR& col, const float val)
{
	return COLOUR(	(unsigned char)(col.r/val),
					(unsigned char)(col.g/val),
					(unsigned char)(col.b/val),
					(unsigned char)(col.a/val));
}

// PP: COLOUR division by a constant, with assignment - includes alpha
inline COLOUR& operator /= (struct TAG_COLOUR& col, const float val)
{
	return (col=col/val);
}

// PP: some pre-defined colour macros for the chronically lazy...

#define BLACK		COLOUR(0,0,0,255)
#define WHITE		COLOUR(255,255,255,255)
#define GREY		COLOUR(128,128,128,255)

#define RED			COLOUR(255,0,0,255)
#define PINK		COLOUR(255,64,64,255)
#define LIGHTRED	PINK
#define DARKRED		COLOUR(128,0,0,255)

#define GREEN		COLOUR(0,255,0,255)
#define LIGHTGREEN	COLOUR(64,255,64,255)
#define DARKGREEN	COLOUR(0,128,0,255)
#define AQUAGREEN	COLOUR(0,128,255,255)

#define BLUE		COLOUR(0,0,255,255)
#define LIGHTBLUE	COLOUR(64,64,255,255)
#define DARKBLUE	COLOUR(0,0,128,255)
#define AQUABLUE	COLOUR(0,255,128,255)

#define CYAN		COLOUR(0,255,255,255)
#define YELLOW		COLOUR(255,255,0,255)
#define MAGENTA		COLOUR(255,0,255,255)

#define BROWN		COLOUR(64,32,0,255)
#define ORANGE		COLOUR(255,128,0,255)


#endif // PP: ndef __H_COLOUR