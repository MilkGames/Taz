// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : ShadowExtra.h
//   Purpose : Useful shadow stuff that needs to be included when shadow.h can't be
//				included, e.g. in actors.h.  If you can think of a nicer solution
//				please let me know // PP: 
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_SHADOWEXTRA
#define __H_SHADOWEXTRA


// PP: Shadow types
// PP: NOTE: currently, shadows of all types turn into proper shadows when VFX_SHADOWMAP is activated
typedef enum _ShadowType
{
	SHADOWTYPE_DOWNWARD,			// PP: the normal downward spot shadow we've been using all along
	SHADOWTYPE_ZOOBOSS,				// PP: a horizontal spot shadow used for the Zoo boss sunset

	NUM_SHADOW_TYPES				// PP: number of shadow types

}ShadowType;

// PP: default shadow type to use - this is the normal downward shadow we've been using all along
#define SHADOW_DEFAULT_TYPE			SHADOWTYPE_DOWNWARD


#endif // PP: ndef __H_SHADOWEXTRA