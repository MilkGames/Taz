// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : buildNumber.h
//   Purpose : specifies the version of babel CURRENTLY BEING WORKED ON
//			   (this number is incremented immediately after a build ships)
// Component : Generic Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BUILDNUMBER_H__
#define __BUILDNUMBER_H__

// Build number and version string
#define BBUILDNUMBER		64
#define BBUILDNUMBER_POINT	3
#define BVERSION			"64.03"


#define BVERSION_CHECK(_major, _minor) ((BBUILDNUMBER>(_major)) || ((BBUILDNUMBER==(_major)) && (BBUILDNUMBER_POINT>=(_minor))))


#endif		// __BUILDNUMBER_H__