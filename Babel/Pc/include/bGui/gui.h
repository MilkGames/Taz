// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : gui.h
//   Purpose : gui functions
// Component : Generic Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BGUI_GUI_H__
#define __BGUI_GUI_H__

// ********************************************************************************
// Constants and Macros

// fill modes
enum EBGuiFillMode {
	BGUIFILLMODE_OPAQUE,
	BGUIFILLMODE_TRANSPARENT,
};


// GUI frame styles
enum EBGuiFrameStyle {
	BGUIFRAME_RAISED,
	BGUIFRAME_SUNKEN,
	BGUIFRAME_FLAT,
	BGUIFRAME_RAISEDX2,
	BGUIFRAME_SUNKENX2,
};


// GUI icons (8x12)
enum EBGuiIcons {
	BGUIICON_SUBMENU,
	BGUIICON_TICK,

	BGUIICON_MAX,
};


// standard colour macros
#define BGUICOLOUR_TEXTFORE  bGuiInfo.textFore[0],bGuiInfo.textFore[1],bGuiInfo.textFore[2]
#define BGUICOLOUR_TEXTBACK  bGuiInfo.textBack[0],bGuiInfo.textBack[1],bGuiInfo.textBack[2]
#define BGUICOLOUR_MENUFORE  bGuiInfo.menuFore[0],bGuiInfo.menuFore[1],bGuiInfo.menuFore[2]
#define BGUICOLOUR_MENUBACK  bGuiInfo.menuBack[0],bGuiInfo.menuBack[1],bGuiInfo.menuBack[2]
#define BGUICOLOUR_DLGFORE  bGuiInfo.dlgFore[0],bGuiInfo.dlgFore[1],bGuiInfo.dlgFore[2]
#define BGUICOLOUR_DLGBACK  bGuiInfo.dlgBack[0],bGuiInfo.dlgBack[1],bGuiInfo.dlgBack[2]
#define BGUICOLOUR_HIGHFORE  bGuiInfo.highFore[0],bGuiInfo.highFore[1],bGuiInfo.highFore[2]
#define BGUICOLOUR_HIGHBACK  bGuiInfo.highBack[0],bGuiInfo.highBack[1],bGuiInfo.highBack[2]
#define BGUICOLOUR_DIMFORE  bGuiInfo.highFore[0]>>1,bGuiInfo.highFore[1]>>1,bGuiInfo.highFore[2]>>1
#define BGUICOLOUR_NOFOCUS	191,191,191


// colour IDs for setting
typedef enum EBGuiColourID {
	EGUICOLOUR_TEXTFORE,
	EGUICOLOUR_TEXTBACK,
	EGUICOLOUR_MENUFORE,
	EGUICOLOUR_MENUBACK,
	EGUICOLOUR_DLGFORE,
	EGUICOLOUR_DLGBACK,
	EGUICOLOUR_HIGHFORE,
	EGUICOLOUR_HIGHBACK,
};


// element types
enum EBGuiElement {
	BGUIELEMENT_MENU,
	BGUIELEMENT_DIALOGUE
};


// max items on a menu
#define BGUIMENU_MAXITEMS		32


// element flags
#define BGUIELEMENTFLAG_MODAL		0x00000001		// this element is modal
#define BGUIELEMENTFLAG_VISIBLE		0x00000002		// this element is visible
#define BGUIELEMENTFLAG_HASFOCUS	0x00000004		// this element has focus (or a child has)


// menu flags
#define BGUIMENUFLAG_POPUP			0x00000001		// this is a popup menu (rather than a menu-bar)


// menu item flags
#define BGUIMENUITEMFLAG_SEPARATOR	0x0001			// item is a separator
#define BGUIMENUITEMFLAG_DISABLED	0x0002			// item is disabled (cannot be selected)
#define BGUIMENUITEMFLAG_TICKED		0x0004			// item is ticked


// GUI event IDs
enum {
	EBGUIEVENT_MENUSELECT,							// a menu item has been selected
};


// ********************************************************************************
// Types, Structures and Classes

// a GUI font character
typedef struct _TBGuiFontChar {
	int32				width;								// character width
	char				*data;								// char data
} TBGuiFontChar;


// a GUI font
typedef struct _TBGuiFont {
	int32				height;								// font height
	TBGuiFontChar		chars[95];							// character data
} TBGuiFont;


// a single menu bar item
typedef struct _TBGuiMenuItem {
	char					text[30];						// text for this menu item
	ushort					flags;							// menu item flags (see BGUIMENUITEMFLAG_)
	struct _TBGuiElement	*subMenu;						// ptr to submenu (NULL for no submenu)
	ushort					id;								// menu item ID
	ushort					xPos, yPos;						// position & dimensions
	ushort					width;							// width in pixels
} TBGuiMenuItem;


// menu bar element info
typedef struct _TBGuiMenuInfo {
	int						noofItems;						// #items in this menu
	uint32					flags;							// flags (see BGUIMENUFLAG_)
	TBGuiMenuItem			items[BGUIMENU_MAXITEMS];		// items
	int32					focusItem;						// index of item with focus or -1 for none
} TBGuiMenuInfo;


// a user interface element
typedef struct _TBGuiElement {
	EBGuiElement		type;								// type of element
	uint32				flags;								// element flags (see BGUIELEMENTFLAG_)
	int					id;									// resource ID
	union {
		TBGuiMenuInfo	menu;								// info for menus
	};

	struct _TBGuiElement	*next, *prev;					// list links
	struct _TBGuiElement	*parent;						// parent element
} TBGuiElement;


// GUI event info passed to callback
typedef struct _TBGuiEventInfo {
	int					reason;								// reason for the event
	int					resourceID;							// resource element ID
} TBGuiEventInfo;


// GUI event callback prototype
typedef int (*TBGuiEventCallback)(TBGuiEventInfo *cbInfo);


// GUI info container
typedef struct _TBGuiInfo {
	int					textFore[3];						// text foreground colour
	int					textBack[3];						// text background colour
	EBGuiFillMode		fillMode;							// current fill mode
	int					menuFore[3];						// menu foreground
	int					menuBack[3];						// menu background
	int					dlgFore[3];							// dialogue foreground
	int					dlgBack[3];							// dialogue background
	int					highFore[3];						// highlight foreground
	int					highBack[3];						// highlight background

	TBIChannelMap		*inputMap;							// GUI input map
	TBIInputChannel		*chLeft, *chRight, *chUp, *chDown;	// directional navigation channels
	TBIInputChannel		*chSelect, *chDeselect;				// selection channels
	TBIInputChannel		*chNext, *chPrior;					// dialog navigation channels
	TBIInputChannel		*chExit;							// exit GUI channel

	TBGuiElement		elements;							// dummy head of element list
	TBGuiElement		*menuBar;							// menu bar element
	TBGuiElement		*focusElement;						// element that has focus

	int					hasFocus;							// set when the GUI has modal focus
	int					noofVisibleElements;				// #visible elements
	TBGuiEventCallback	eventCallback;						// current event callback function
} TBGuiInfo;


// ********************************************************************************
// Globals

// gui font characters
extern TBGuiFont	bDefGuiFont;
extern TBGuiInfo	bGuiInfo;
extern char			*bGuiIcons[BGUIICON_MAX];


// ********************************************************************************
// Prototypes

// ------------------
// internal functions
// ------------------

/* --------------------------------------------------------------------------------
   Function : bGuiTextExtent
   Purpose : calculate how wide a string will be printed in a certain font
   Parameters : font, text
   Returns : width in pixels
   Info : 
*/

int bGuiTextExtent(TBGuiFont *font, char *text);


/* --------------------------------------------------------------------------------
   Function : bGuiDeltaRGB
   Purpose : modify an RGB colour
   Parameters : source colour, dest colour ptrs, mod amount (+ve/-ve)
   Returns : 
   Info : 
*/

void bGuiDeltaRGB(int r, int g, int b, int *rp, int *gp, int *bp, int delta);


/* --------------------------------------------------------------------------------
   Function : bGuiCentreText
   Purpose : print some centred text
   Parameters : font, xmin, xmax, y, text
   Returns : 
   Info : 
*/

void bGuiCentreText(TBGuiFont *font, int xMin, int xMax, int y, char *text);


/* --------------------------------------------------------------------------------
   Function : bGuiRightText
   Purpose : print some right-justified text
   Parameters : font, xmax, y, text
   Returns : 
   Info : 
*/

void bGuiRightText(TBGuiFont *font, int xMax, int y, char *text);



// -------------
// API functions
// -------------

/* --------------------------------------------------------------------------------
   Function : bgInitGui
   Purpose : initialise the gui module
   Parameters : 
   Returns : OK/FAIL
   Info : 
*/

int bgInitGui();


/* --------------------------------------------------------------------------------
   Function : bgShutdownGui
   Purpose : shutdown the gui module
   Parameters : 
   Returns : 
   Info : 
*/

void bgShutdownGui();


/* --------------------------------------------------------------------------------
   Function : bgPoll
   Purpose : poll the GUI
   Parameters : 
   Returns : 
   Info : 
*/

void bgPoll();


/* --------------------------------------------------------------------------------
   Function : bgEnterGui
   Purpose : enter GUI control
   Parameters : event callback function
   Returns : 
   Info : 
*/

void bgEnterGui(TBGuiEventCallback callback);


/* --------------------------------------------------------------------------------
   Function : bgExitGui
   Purpose : exit GUI control
   Parameters : 
   Returns : 
   Info : 
*/

void bgExitGui();


/* --------------------------------------------------------------------------------
   Function : bgCreateMenu
   Purpose : create a new menu
   Parameters : flags (BGUIMENUFLAG_POPUP for popup or zero for bar)
   Returns : ptr to new menu element
   Info : 
*/

TBGuiElement *bgCreateMenu(uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bgDeleteElement
   Purpose : delete a GUI element
   Parameters : element ptr
   Returns : 
   Info : 
*/

void bgDeleteElement(TBGuiElement *elementPtr);


/* --------------------------------------------------------------------------------
   Function : bgSetMainMenu
   Purpose : set the primary menu bar menu
   Parameters : menu element ptr
   Returns : 
   Info : 
*/

void bgSetMainMenu(TBGuiElement *menuPtr);


/* --------------------------------------------------------------------------------
   Function : bgAddMenuItem
   Purpose : add a new item to a menu
   Parameters : menu element ptr, text, id, subMenu, flags
   Returns : ptr to new menu item or NULL for failure
   Info : 
*/

TBGuiMenuItem *bgAddMenuItem(TBGuiElement *menuPtr, char *text, int32 id, TBGuiElement *subMenu, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bgInsertMenuItem
   Purpose : insert a new item in the middle of a menu
   Parameters : menu element ptr, item to insert after (NULL for at top), text, id, subMenu, flags
   Returns : ptr to new menu item or NULL for failure
   Info : 
*/

TBGuiMenuItem *bgInsertMenuItem(TBGuiElement *menuPtr, TBGuiMenuItem *afterItem, char *text, int32 id, TBGuiElement *subMenu, uint32 flags);


/* --------------------------------------------------------------------------------
   Function : bgLoadElementFile
   Purpose : load a resource element file
   Parameters : package index, filename
   Returns : OK/FAIL
   Info : 
*/

int bgLoadElementFile(TBPackageIndex *pak, char *filename);


/* --------------------------------------------------------------------------------
   Function : bgFindElement
   Purpose : find an element from it's type and ID
   Parameters : type (see BGUIELEMENT_), id
   Returns : 
   Info : 
*/

TBGuiElement *bgFindElement(int type, int id);


/* --------------------------------------------------------------------------------
   Function : bgCountMenuItems
   Purpose : count the number of items on a menu
   Parameters : menu element ptr
   Returns : #items
   Info : 
*/

int bgCountMenuItems(TBGuiElement *menuPtr);


/* --------------------------------------------------------------------------------
   Function : bgDeleteMenuItem
   Purpose : delete a menu item
   Parameters : menu element ptr, item ptr
   Returns : 
   Info : 
*/

void bgDeleteMenuItem(TBGuiElement *menuPtr, TBGuiMenuItem *item);


/* --------------------------------------------------------------------------------
   Function : bgFindMenuItemByIndex
   Purpose : find a menu item identified by it's zero-based index in the menu
   Parameters : menu element ptr, item index
   Returns : ptr to menu item or NULL for item not found
   Info : 
*/

TBGuiMenuItem *bgFindMenuItemByIndex(TBGuiElement *menuPtr, int itemIndex);


/* --------------------------------------------------------------------------------
   Function : bgFindMenuItemById
   Purpose : find a menu item identified by it's numeric identifier
   Parameters : menu element ptr, item ID
   Returns : ptr to menu item or NULL for item not found
   Info : 
*/

TBGuiMenuItem *bgFindMenuItemById(TBGuiElement *menuPtr, int itemId);


/* --------------------------------------------------------------------------------
   Function : bgSetMenuItem
   Purpose : set the data for a menu item
   Parameters : menu element ptr, menu item ptr, text, id, submenu, flags
   Returns : 
   Info : 
*/

void bgSetMenuItem(TBGuiMenuItem *itemPtr, char *text, int32 id, TBGuiElement *subMenu, uint32 flags, uint32 flagMask);


/* --------------------------------------------------------------------------------
   Function : bgSetColour
   Purpose : set the colour of a GUI interface element
   Parameters : colour ID, red, green, blue (all 0..255)
   Returns : 
   Info : 
*/

void bgSetColour(EBGuiColourID colour, int r, int g, int b);

#endif		// __BGUI_GUI_H__
