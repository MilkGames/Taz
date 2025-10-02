// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : gui.cpp
//   Purpose : Defines enums for GUI fill modes, frame styles, icons, and color IDs. Defines macros for standard GUI colors. Defines structs for TBGuiFontChar, TBGuiFont, TBGuiMenuItem, TBGuiMenuInfo, TBGuiElement, TBGuiEventInfo
// Component : Babel GUI
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bGuiTextExtent
	Purpose : get GUI text extent
	Parameters : font, text
	Returns : extent
	Info : 
*/
int bGuiTextExtent(TBGuiFont *font, char *text)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiDeltaRGB
	Purpose : get GUI delta RGB
	Parameters : r, g, b, rp, gp, bp, delta
	Returns : 
	Info : 
*/
void bGuiDeltaRGB(int r, int g, int b, int *rp, int *gp, int *bp, int delta)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiCentreText
	Purpose : centre GUI text
	Parameters : font, xMin, xMax, y, text
	Returns : 
	Info : 
*/
void bGuiCentreText(TBGuiFont *font, int xMin, int xMax, int y, char *text)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bGuiRightText
	Purpose : right align GUI text
	Parameters : font, xMax, y, text
	Returns : 
	Info : 
*/
void bGuiRightText(TBGuiFont *font, int xMax, int y, char *text)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgInitGui
	Purpose : initialise GUI
	Parameters : 
	Returns : OK/FAIL
	Info : 
*/
int bgInitGui()
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bgShutdownGui
	Purpose : shutdown GUI
	Parameters : 
	Returns : 
	Info : 
*/
void bgShutdownGui()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgPoll
	Purpose : poll GUI
	Parameters : 
	Returns : 
	Info : 
*/
void bgPoll()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgEnterGui
	Purpose : enter GUI
	Parameters : callback
	Returns : 
	Info : 
*/
void bgEnterGui(TBGuiEventCallback callback)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgExitGui
	Purpose : exit GUI
	Parameters : 
	Returns : 
	Info : 
*/
void bgExitGui()
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgCreateMenu
	Purpose : create menu
	Parameters : flags
	Returns : menu element
	Info : 
*/
TBGuiElement *bgCreateMenu(uint32 flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgDeleteElement
	Purpose : delete element
	Parameters : elementPtr
	Returns : 
	Info : 
*/
void bgDeleteElement(TBGuiElement *elementPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgSetMainMenu
	Purpose : set main menu
	Parameters : menuPtr
	Returns : 
	Info : 
*/
void bgSetMainMenu(TBGuiElement *menuPtr)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgAddMenuItem
	Purpose : add menu item
	Parameters : menuPtr, text, id, subMenu, flags
	Returns : menu item
	Info : 
*/
TBGuiMenuItem *bgAddMenuItem(TBGuiElement *menuPtr, char *text, int32 id, TBGuiElement *subMenu, uint32 flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgInsertMenuItem
	Purpose : insert menu item
	Parameters : menuPtr, afterItem, text, id, subMenu, flags
	Returns : menu item
	Info : 
*/
TBGuiMenuItem *bgInsertMenuItem(TBGuiElement *menuPtr, TBGuiMenuItem *afterItem, char *text, int32 id, TBGuiElement *subMenu, uint32 flags)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgLoadElementFile
	Purpose : load element file
	Parameters : pak, filename
	Returns : OK/FAIL
	Info : 
*/
int bgLoadElementFile(TBPackageIndex *pak, char *filename)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bgFindElement
	Purpose : find element
	Parameters : type, id
	Returns : element
	Info : 
*/
TBGuiElement *bgFindElement(int type, int id)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgCountMenuItems
	Purpose : count menu items
	Parameters : menuPtr
	Returns : count
	Info : 
*/
int bgCountMenuItems(TBGuiElement *menuPtr)
{
    return 0;
}

/*	--------------------------------------------------------------------------------
	Function : bgDeleteMenuItem
	Purpose : delete menu item
	Parameters : menuPtr, item
	Returns : 
	Info : 
*/
void bgDeleteMenuItem(TBGuiElement *menuPtr, TBGuiMenuItem *item)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgFindMenuItemByIndex
	Purpose : find menu item by index
	Parameters : menuPtr, itemIndex
	Returns : menu item
	Info : 
*/
TBGuiMenuItem *bgFindMenuItemByIndex(TBGuiElement *menuPtr, int itemIndex)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgFindMenuItemById
	Purpose : find menu item by ID
	Parameters : menuPtr, itemId
	Returns : menu item
	Info : 
*/
TBGuiMenuItem *bgFindMenuItemById(TBGuiElement *menuPtr, int itemId)
{
    return NULL;
}

/*	--------------------------------------------------------------------------------
	Function : bgSetMenuItem
	Purpose : set menu item
	Parameters : itemPtr, text, id, subMenu, flags, flagMask
	Returns : 
	Info : 
*/
void bgSetMenuItem(TBGuiMenuItem *itemPtr, char *text, int32 id, TBGuiElement *subMenu, uint32 flags, uint32 flagMask)
{
    return;
}

/*	--------------------------------------------------------------------------------
	Function : bgSetColour
	Purpose : set colour
	Parameters : colour, r, g, b
	Returns : 
	Info : 
*/
void bgSetColour(EBGuiColourID colour, int r, int g, int b)
{
    return;
}
