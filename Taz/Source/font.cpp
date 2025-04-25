// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : font.cpp
//   Purpose : general text printing
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include "taz.h"

#include "global.h"
#include "font.h"
#include "files.h"
#include "main.h"
#include "mouse.h"

#if(BPLATFORM == PS2)
#include <libsceedemo.h>		// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#else// PP: not ps2
#include "libsceedemo.h"		// PP: SCEE Demo Disk SDK Version 1.0 Pre-release 1
#endif// PP: not ps2

/* --------------------------------------------------------------------------------
   Function : LoadFont
   Purpose : load a font
   Parameters :  font name, pak file name
   Returns : TBFont
   Info : 
*/
TBFont *LoadFont(char *fontName,char *pakName)
{
	TBPackageIndex	*pakIndex;
	TBFont			*font;

	bdSetRenderState(BDRENDERSTATE_TEXTUREENABLE, 1, 0);
	bdSetRenderState(BDRENDERSTATE_TEXTUREENABLE, 1, 1);

	if (pakIndex = FindPackage(pakName))
	{
		font = bkLoadFont(pakIndex,fontName,0);
		return font;
	}
	else
	{
/*		if ((pakIndex = bkOpenPackage(pakName)))
		{
			font = bkLoadFont(pakIndex,fontName,0);
			bkClosePackage(pakIndex);
			return font;
		}*/
		bkPrintf("*** ERROR *** Package %s not loaded\n",pakName);
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : FreeFont
   Purpose : free a font
   Parameters : TBFont
   Returns : 
   Info : 
*/
void FreeFont(TBFont *font)
{
	bkDeleteFont(font);
}

/* --------------------------------------------------------------------------------
   Function : LoadStringTable
   Purpose : load a string table
   Parameters : table name, package name
   Returns : TBStringTable
   Info : 
*/
TBStringTable *LoadStringTable(char *tableName, char *pakName)
{
	TBPackageIndex	*pakIndex;
	TBStringTable	*string;

#if(BPLATFORM == PS2)
#ifdef CONSUMERDEMO
	#ifndef USE_HOST0
		#ifndef STANDALONE
			char			tempChar[256];

			sprintf(tempChar, "tazdemo\\%s", pakName);

			pakName = tempChar;
		#endif
	#endif
#endif// PP: demo
#endif// PP: ps2

	if (pakIndex = FindPackage(pakName))
	{
		string = bkLoadStringTable(pakIndex,tableName,0);
		return string;
	}
	else
	{
		if ((pakIndex = bkOpenPackage(pakName)))
		{
			string = bkLoadStringTable(pakIndex,tableName,0);
			bkClosePackage(pakIndex);
			return string;
		}
		bkPrintf("*** ERROR *** Package %s not loaded\n",pakName);
	}
	return NULL;
}

/* --------------------------------------------------------------------------------
   Function : FreeStringTable
   Purpose : free a string table
   Parameters : TBStringTable
   Returns : 
   Info : 
*/
void FreeStringTable(TBStringTable *table)
{
	bkDeleteAllStringTables();
}


/*  --------------------------------------------------------------------------------
	Function	: ChooseCurrentLanguae
	Purpose		: Fill in gameStatus.currentLanguage with the chosen boot langauge
	Parameters	: 
	Returns		: 
	Info		: 
*/

void ChooseCurrentLanguage()
{
#if BPLATFORM != PC // NH: // PP: ps2, gamecube

#if(BPLATFORM == XBOX)

	// PP: use current language setting from Xbox dashboard...

	switch(XGetLanguage())
	{
	case XC_LANGUAGE_ENGLISH:
		{
			gameStatus.currentLanguage = BLANGUAGEID_UK;
		}
		break;

	case XC_LANGUAGE_JAPANESE:
		{

			// PP: switch to English instead

			gameStatus.currentLanguage = BLANGUAGEID_UK;
		}
		break;

	case XC_LANGUAGE_GERMAN:
		{
			gameStatus.currentLanguage = BLANGUAGEID_D;
		}
		break;

	case XC_LANGUAGE_FRENCH:
		{
			gameStatus.currentLanguage = BLANGUAGEID_F;
		}
		break;

	case XC_LANGUAGE_SPANISH:
		{
			gameStatus.currentLanguage = BLANGUAGEID_E;
		}
		break;

	case XC_LANGUAGE_ITALIAN:
		{
			gameStatus.currentLanguage = BLANGUAGEID_IT;
		}
		break;

	default:
		// TP: If unknown language then select uk
		gameStatus.currentLanguage = BLANGUAGEID_UK;
		break;
	}

#else// PP: !xbox

	#ifdef CONSUMERDEMO
		switch(language)
		{
			case SCEE_DEMO_LANGUAGE_ENGLISH:	gameStatus.currentLanguage = BLANGUAGEID_UK;	break;
			case SCEE_DEMO_LANGUAGE_FRENCH:		gameStatus.currentLanguage = BLANGUAGEID_F;		break;
			case SCEE_DEMO_LANGUAGE_GERMAN:		gameStatus.currentLanguage = BLANGUAGEID_D;		break;
			case SCEE_DEMO_LANGUAGE_SPANISH:	gameStatus.currentLanguage = BLANGUAGEID_E;		break;
			case SCEE_DEMO_LANGUAGE_ITALIAN:	gameStatus.currentLanguage = BLANGUAGEID_IT;	break;
		}
	#else

		// JW: Get the language from the console

		switch(bkGetSystemLanguage())
		{
		case BLANGUAGEID_F:
		case BLANGUAGEID_D:
		case BLANGUAGEID_E:
		case BLANGUAGEID_IT:	GAMESTATUS::currentLanguage = bkGetSystemLanguage();	break;
	
		default:				// PP: use English by default
		case BLANGUAGEID_US:	GAMESTATUS::currentLanguage = BLANGUAGEID_UK;			break;
		}

	#endif

#endif// PP: !xbox

#else// PP:  BPLATFORM == PC

	// NH: Load PC config File
	ProcessConfigFile();

	// TP: just having a tidy up in main
	LoadTazDataPC();
	// NH: Setup the mouse ready for use
	InitMouse(&mouse);

#endif // NH: BPLATFORM
}