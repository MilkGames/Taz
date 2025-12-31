// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : gui.cpp
//   Purpose : Defines enums for GUI fill modes, frame styles, icons, and color IDs. Defines macros for standard GUI colors. Defines structs for TBGuiFontChar, TBGuiFont, TBGuiMenuItem, TBGuiMenuInfo, TBGuiElement, TBGuiEventInfo
// Component : Babel GUI
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>
#include "../../include/bGui/defFont.h"

// ********************************************************************************
// Globals

TBGuiInfo	bGuiInfo;

// ********************************************************************************
// Local Functions

void DrawPopupMenu(TBGuiElement *element, int xPos, int yPos)
{
    int32 itemCount;
    int32 i;
    int32 maxItemWidth;
    int32 menuWidth;
    int32 menuHeight;
    int32 x;
    int32 y;
    int32 hiliteY;
    int32 textY;

    itemCount = element->menu.noofItems;

    maxItemWidth = 0;
    for (i = 0; i < itemCount; i++)
    {
        if (maxItemWidth < (int32)element->menu.items[i].width)
        {
            maxItemWidth = (int32)element->menu.items[i].width;
        }
    }

    menuWidth = maxItemWidth + 0x1c;
    menuHeight = itemCount * 0x0e + 4;

    bGuiInfo.textFore[0] = bGuiInfo.menuFore[0];
    bGuiInfo.textFore[1] = bGuiInfo.menuFore[1];
    bGuiInfo.textFore[2] = bGuiInfo.menuFore[2];
    bGuiInfo.fillMode = BGUIFILLMODE_TRANSPARENT;

    x = xPos;
    if (bDisplayInfo.xRes <= (menuWidth + xPos))
    {
        x = bDisplayInfo.xRes - menuWidth;
    }

    y = yPos;
    if (bDisplayInfo.yRes <= (menuHeight + yPos))
    {
        y = bDisplayInfo.yRes - menuHeight;
    }

    bGuiFillRect(x, y, menuWidth, menuHeight,
                 bGuiInfo.menuBack[0], bGuiInfo.menuBack[1], bGuiInfo.menuBack[2]);

    bGuiDrawFrame(x, y, menuWidth, menuHeight, BGUIFRAME_RAISED,
                  bGuiInfo.menuBack[0], bGuiInfo.menuBack[1], bGuiInfo.menuBack[2]);

    hiliteY = y + 2;
    textY = y + 3;

    for (i = 0; i < itemCount; i++)
    {
        TBGuiMenuItem *item;
        ushort flags;

        item = &element->menu.items[i];
        flags = item->flags;

        if ((flags & BGUIMENUITEMFLAG_SEPARATOR) != 0)
        {
            bGuiDrawHBar(x + 3, textY + 6, maxItemWidth + 0x16,
                         bGuiInfo.menuBack[0], bGuiInfo.menuBack[1], bGuiInfo.menuBack[2]);
        }
        else
        {
            if (element->menu.focusItem == i)
            {
                if ((flags & BGUIMENUITEMFLAG_DISABLED) != 0)
                {
                    bGuiInfo.textFore[0] = bGuiInfo.highFore[0] >> 1;
                    bGuiInfo.textFore[1] = bGuiInfo.highFore[1] >> 1;
                    bGuiInfo.textFore[2] = bGuiInfo.highFore[2] >> 1;
                }
                else
                {
                    bGuiInfo.textFore[0] = bGuiInfo.highFore[0];
                    bGuiInfo.textFore[1] = bGuiInfo.highFore[1];
                    bGuiInfo.textFore[2] = bGuiInfo.highFore[2];
                }

                bGuiFillRect(x + 1, hiliteY, maxItemWidth + 0x1a, 0x0f,
                             bGuiInfo.highBack[0], bGuiInfo.highBack[1], bGuiInfo.highBack[2]);

                bGuiPrintText((TBGuiFont *)0, x + 0x12, textY, item->text);

                if ((flags & BGUIMENUITEMFLAG_TICKED) != 0)
                {
                    bGuiPrintIcon(BGUIICON_TICK, x + 5, textY);
                }

                if (item->subMenu != (TBGuiElement *)0)
                {
                    bGuiPrintIcon(BGUIICON_SUBMENU, x + maxItemWidth + 0x12, textY);
                }

                bGuiInfo.textFore[0] = bGuiInfo.menuFore[0];
                bGuiInfo.textFore[1] = bGuiInfo.menuFore[1];
                bGuiInfo.textFore[2] = bGuiInfo.menuFore[2];

                if ((item->subMenu != (TBGuiElement *)0) && (bGuiInfo.focusElement != element))
                {
                    DrawPopupMenu(item->subMenu, x + maxItemWidth + 0x1a, hiliteY);
                }
            }
            else
            {
                if ((flags & BGUIMENUITEMFLAG_DISABLED) != 0)
                {
                    bGuiInfo.textFore[0] = bGuiInfo.highFore[0] >> 1;
                    bGuiInfo.textFore[1] = bGuiInfo.highFore[1] >> 1;
                    bGuiInfo.textFore[2] = bGuiInfo.highFore[2] >> 1;
                }

                bGuiPrintText((TBGuiFont *)0, x + 0x12, textY, item->text);

                if ((flags & BGUIMENUITEMFLAG_TICKED) != 0)
                {
                    bGuiPrintIcon(BGUIICON_TICK, x + 5, textY);
                }

                if (item->subMenu != (TBGuiElement *)0)
                {
                    bGuiPrintIcon(BGUIICON_SUBMENU, x + maxItemWidth + 0x12, textY);
                }

                if ((flags & BGUIMENUITEMFLAG_DISABLED) != 0)
                {
                    bGuiInfo.textFore[0] = bGuiInfo.menuFore[0];
                    bGuiInfo.textFore[1] = bGuiInfo.menuFore[1];
                    bGuiInfo.textFore[2] = bGuiInfo.menuFore[2];
                }
            }
        }

        hiliteY += 0x0e;
        textY += 0x0e;
    }
}

void __cdecl DrawMenuBar(TBGuiElement *element)
{
    int32 itemIndex;
    int32 xPos;
    uchar *item;
    ushort itemWidth;
    int32 r;
    int32 g;
    int32 b;

    if ((((uchar *)element)[0x04] & 0x04) != 0) {
        r = (int32)bGuiInfo.menuBack[0];
        g = (int32)bGuiInfo.menuBack[1];
        b = (int32)bGuiInfo.menuBack[2];
    } else {
        r = 0xbf;
        g = 0xbf;
        b = 0xbf;
    }

    bGuiFillRect(0, 0x1e, bDisplayInfo.xRes, 0x10, r, g, b);

    bGuiInfo.fillMode = BGUIFILLMODE_TRANSPARENT;
    bGuiInfo.textFore[0] = bGuiInfo.menuFore[0];
    bGuiInfo.textFore[1] = bGuiInfo.menuFore[1];
    bGuiInfo.textFore[2] = bGuiInfo.menuFore[2];

    itemIndex = 0;
    xPos = 5;
    item = (uchar *)element + 0x14;

    while (itemIndex < *(int32 *)((uchar *)element + 0x0c)) {
        itemWidth = *(ushort *)(item + 0x2a);

        if ((int32)bDisplayInfo.xRes < (int32)itemWidth + xPos) {
            break;
        }

        bGuiPrintText((_TBGuiFont *)0, xPos, 0x1f, (char *)item);

        if ((((uchar *)element)[0x04] & 0x04) == 0) {
            xPos = (int32)itemWidth + xPos + 8;
        } else {
            if (*(int32 *)((uchar *)element + 0x594) == itemIndex) {
                bGuiDrawFrame(
                    xPos - 3,
                    0x1f,
                    (int32)itemWidth + 6,
                    0x0e,
                    (EBGuiFrameStyle)(bGuiInfo.focusElement != element),
                    bGuiInfo.menuBack[0],
                    bGuiInfo.menuBack[1],
                    bGuiInfo.menuBack[2]
                );

                if ((bGuiInfo.focusElement != element) && (*(void **)(item + 0x20) != (void *)0)) {
                    DrawPopupMenu(*(_TBGuiElement **)(item + 0x20), xPos - 4, 0x2d);
                }
            }

            xPos = (int32)itemWidth + xPos + 8;
        }

        item += 0x2c;
        itemIndex += 1;
    }
}

void HandleMenuInputs()
{
	TBGuiElement *focus = bGuiInfo.focusElement;
	TBGuiElement *subMenu;
	TBGuiElement *parent;
	int32 noofItems;
	int32 oldFocus;
	int32 tries;
	int32 selectValue;
	TBGuiEventInfo eventInfo;

	// ---------------------------------------------------------------------
	// Menu bar navigation (only when this is NOT a popup menu)
	// ---------------------------------------------------------------------
	if ((focus->menu.flags & BGUIMENUFLAG_POPUP) == 0) {

		if (bGuiInfo.chRight->value != 0) {
			noofItems = focus->menu.noofItems;
			if (noofItems != 0) {
				focus->menu.focusItem = (focus->menu.focusItem + 1) % noofItems;
			}
		}
		else if (bGuiInfo.chLeft->value != 0) {
			if (focus->menu.noofItems != 0) {
				focus->menu.focusItem--;
				if (focus->menu.focusItem < 0) {
					focus->menu.focusItem = focus->menu.noofItems - 1;
				}
			}
		}
		else {
			if ((bGuiInfo.chDown->value != 0) || (bGuiInfo.chSelect->value != 0)) {
				int32 idx = focus->menu.focusItem;

				if (idx != -1) {
					subMenu = focus->menu.items[idx].subMenu;
					if (subMenu != NULL) {
						bGuiInfo.focusElement = subMenu;
						subMenu->menu.focusItem = 0;

						biForceDebounce(bGuiInfo.chSelect);
						bGuiInfo.chDown->value = 0;
					}
				}
			}
		}

		focus = bGuiInfo.focusElement;
	}

	// ---------------------------------------------------------------------
	// Popup/menu navigation: DOWN (skip separators)
	// ---------------------------------------------------------------------
	if (bGuiInfo.chDown->value != 0) {
		noofItems = focus->menu.noofItems;
		if (noofItems != 0) {
			oldFocus = focus->menu.focusItem;
			tries = noofItems;

			while (1) {
				tries--;
				focus->menu.focusItem = (focus->menu.focusItem + 1) % noofItems;

				if ((focus->menu.items[focus->menu.focusItem].flags & BGUIMENUITEMFLAG_SEPARATOR) == 0) {
					break;
				}

				if (tries == 0) {
					tries = -1;
					break;
				}
			}

			if (tries < 0) {
				focus->menu.focusItem = oldFocus;
			}
		}
		return;
	}

	// ---------------------------------------------------------------------
	// Popup/menu navigation: UP (skip separators)
	// ---------------------------------------------------------------------
	if (bGuiInfo.chUp->value != 0) {
		noofItems = focus->menu.noofItems;
		if (noofItems != 0) {
			oldFocus = focus->menu.focusItem;
			tries = noofItems;

			while (1) {
				tries--;
				focus->menu.focusItem--;

				if (focus->menu.focusItem < 0) {
					focus->menu.focusItem = focus->menu.noofItems - 1;
				}

				if ((focus->menu.items[focus->menu.focusItem].flags & BGUIMENUITEMFLAG_SEPARATOR) == 0) {
					break;
				}

				if (tries == 0) {
					tries = -1;
					break;
				}
			}

			if (tries < 0) {
				focus->menu.focusItem = oldFocus;
			}
		}
		return;
	}

	// ---------------------------------------------------------------------
	// SELECT: enter submenu if present and not disabled
	// ---------------------------------------------------------------------
	selectValue = bGuiInfo.chSelect->value;

	if (selectValue != 0) {
		int32 idx = focus->menu.focusItem;

		if (idx != -1) {
			subMenu = focus->menu.items[idx].subMenu;
			if (subMenu != NULL) {
				if ((focus->menu.items[idx].flags & BGUIMENUITEMFLAG_DISABLED) == 0) {
					bGuiInfo.focusElement = subMenu;
					subMenu->menu.focusItem = 0;
					return;
				}
			}
		}
	}

	// ---------------------------------------------------------------------
	// RIGHT: popup-only navigation into submenu, or move along parent menu
	// ---------------------------------------------------------------------
	if (bGuiInfo.chRight->value != 0) {
		int32 idx = focus->menu.focusItem;

		if (idx != -1) {
			if ((focus->menu.flags & BGUIMENUFLAG_POPUP) == 0) {
				return;
			}

			subMenu = focus->menu.items[idx].subMenu;
			if (subMenu != NULL) {
				if ((focus->menu.items[idx].flags & BGUIMENUITEMFLAG_DISABLED) != 0) {
					return;
				}

				bGuiInfo.focusElement = subMenu;
				subMenu->menu.focusItem = 0;
				return;
			}

			parent = focus->parent;
			if (parent != NULL) {
				while ((parent->menu.flags & BGUIMENUFLAG_POPUP) != 0) {
					parent = parent->parent;
				}

				noofItems = parent->menu.noofItems;
				if (noofItems != 0) {
					parent->menu.focusItem = (parent->menu.focusItem + 1) % noofItems;
				}

				subMenu = parent->menu.items[parent->menu.focusItem].subMenu;
				if (subMenu != NULL) {
					bGuiInfo.focusElement = subMenu;
					subMenu->menu.focusItem = 0;
					return;
				}

				bGuiInfo.focusElement = parent;
				return;
			}
		}
	}

	// ---------------------------------------------------------------------
	// DESELECT: go to parent if any
	// ---------------------------------------------------------------------
	if (bGuiInfo.chDeselect->value != 0) {
		if (focus->parent != NULL) {
			bGuiInfo.focusElement = focus->parent;
		}
		return;
	}

	// ---------------------------------------------------------------------
	// LEFT: go to parent popup, or move left in parent menu and enter its submenu
	// ---------------------------------------------------------------------
	if (bGuiInfo.chLeft->value != 0) {
		parent = focus->parent;

		if (parent != NULL) {
			if ((parent->menu.flags & BGUIMENUFLAG_POPUP) != 0) {
				bGuiInfo.focusElement = parent;
				return;
			}

			parent->menu.focusItem--;
			if (parent->menu.focusItem < 0) {
				parent->menu.focusItem = parent->menu.noofItems - 1;
			}

			subMenu = parent->menu.items[parent->menu.focusItem].subMenu;
			if (subMenu == NULL) {
				bGuiInfo.focusElement = parent;
				return;
			}

			bGuiInfo.focusElement = subMenu;
			subMenu->menu.focusItem = 0;
			return;
		}
		// fall through to callback block
	}

	// ---------------------------------------------------------------------
	// SELECT callback: only if item has no submenu and not disabled
	// ---------------------------------------------------------------------
	if (selectValue != 0) {
		noofItems = focus->menu.noofItems;

		if (noofItems > 0) {
			int32 idx = focus->menu.focusItem;

			if (idx != -1) {
				if (focus->menu.items[idx].subMenu == NULL) {
					if ((focus->menu.items[idx].flags & BGUIMENUITEMFLAG_DISABLED) == 0) {
						eventInfo.reason = 0;
						eventInfo.resourceID = (int)focus->menu.items[idx].id;

						if (bGuiInfo.eventCallback(&eventInfo) != 0) {
							while ((bGuiInfo.chSelect->value != 0) || (bGuiInfo.chSelect->duration != 0)) {
								biReadDevices();
							}
							bGuiInfo.chExit->value = 1;
						}
					}
				}
			}
		}
	}
}

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
        bkPrintf("*** WARNING *** bGuiTextExtent was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	int32 rr = r + delta;
	int32 gg = g + delta;
	int32 bb = b + delta;

	if (rr < 0) {
		rr = 0;
	} else if (rr > 0xff) {
		rr = 0xff;
	}

	if (gg < 0) {
		gg = 0;
	} else if (gg > 0xff) {
		gg = 0xff;
	}

	if (bb < 0) {
		*rp = rr;
		*gp = gg;
		*bp = 0;
		return;
	}

	if (bb > 0xff) {
		bb = 0xff;
	}

	*rp = rr;
	*gp = gg;
	*bp = bb;
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
        bkPrintf("*** WARNING *** bGuiCentreText was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bGuiRightText was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	memset(&bGuiInfo, 0, sizeof(TBGuiInfo));

	bGuiInfo.textFore[0] = 255;
	bGuiInfo.textFore[1] = 255;
	bGuiInfo.textFore[2] = 255;

	bGuiInfo.textBack[0] = 0;
	bGuiInfo.textBack[1] = 0;
	bGuiInfo.textBack[2] = 0;

	bGuiInfo.fillMode = BGUIFILLMODE_OPAQUE;

	bGuiInfo.menuFore[0] = 0;
	bGuiInfo.menuFore[1] = 0;
	bGuiInfo.menuFore[2] = 0;

	bGuiInfo.menuBack[0] = 180;
	bGuiInfo.menuBack[1] = 140;
	bGuiInfo.menuBack[2] = 100;

	bGuiInfo.dlgFore[0] = 0;
	bGuiInfo.dlgFore[1] = 0;
	bGuiInfo.dlgFore[2] = 0;

	bGuiInfo.dlgBack[0] = 115;
	bGuiInfo.dlgBack[1] = 189;
	bGuiInfo.dlgBack[2] = 183;

	bGuiInfo.highFore[0] = 255;
	bGuiInfo.highFore[1] = 255;
	bGuiInfo.highFore[2] = 255;

	bGuiInfo.highBack[0] = 0;
	bGuiInfo.highBack[1] = 0;
	bGuiInfo.highBack[2] = 127;

	bGuiInfo.inputMap = biCreateMap(9);

	bGuiInfo.chLeft = biCreateChannel(bGuiInfo.inputMap,"Left",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chRight = biCreateChannel(bGuiInfo.inputMap,"Right",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chUp = biCreateChannel(bGuiInfo.inputMap,"Up",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chDown = biCreateChannel(bGuiInfo.inputMap,"Down",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chSelect = biCreateChannel(bGuiInfo.inputMap,"Select",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chDeselect = biCreateChannel(bGuiInfo.inputMap,"Deselect",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_DEBOUNCE,0);
	bGuiInfo.chPrior = biCreateChannel(bGuiInfo.inputMap,"Prior",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chNext = biCreateChannel(bGuiInfo.inputMap,"Next",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_AUTOREPEAT,0);
	bGuiInfo.chExit = biCreateChannel(bGuiInfo.inputMap,"Exit",BICHANNELTYPE_DIGITAL,BICHANNELFLAG_DEBOUNCE,0);

	bGuiInfo.elements.prev = &bGuiInfo.elements;
	bGuiInfo.elements.next = &bGuiInfo.elements;

	bGuiInfo.menuBar = NULL;
	bGuiInfo.focusElement = NULL;

	bInitGuiHAL();

	bGuiInfo.hasFocus = 0;
	bGuiInfo.noofVisibleElements = 0;
	return OK;
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
        bkPrintf("*** WARNING *** bgShutdownGui was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    if (bGuiInfo.hasFocus == 0) {
        if (bGuiInfo.noofVisibleElements == 0) {
            return;
        }
    } else {
        if ((bGuiInfo.focusElement != (_TBGuiElement *)0) && (*(int32 *)bGuiInfo.focusElement == 0)) {
            HandleMenuInputs();
        }
    }

    bGuiLockScreen();

    if ((bGuiInfo.hasFocus != 0) && (bGuiInfo.menuBar != (_TBGuiElement *)0)) {
        DrawMenuBar(bGuiInfo.menuBar);
    }

    bGuiUnlockScreen();

    if (*(int32 *)((uchar *)bGuiInfo.chExit + 0x14) != 0) {
        bExclusiveMap = NULL;
        bGuiInfo.hasFocus = 0;
    }
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
	if ((bGuiInfo.elements.next != &bGuiInfo.elements) && (bGuiInfo.hasFocus == 0)) {
		bGuiInfo.eventCallback = callback;
		bExclusiveMap = bGuiInfo.inputMap;
		bGuiInfo.hasFocus = 1;
		biForceDebounce(bGuiInfo.chExit);
		if (bGuiInfo.menuBar != (_TBGuiElement *)0x0) {
			bGuiInfo.focusElement = bGuiInfo.menuBar;
			bGuiInfo.menuBar->flags = bGuiInfo.menuBar->flags | BGUIELEMENTFLAG_HASFOCUS;
			if (bGuiInfo.menuBar->menu.noofItems != 0) {
				bGuiInfo.menuBar->menu.focusItem = 0;
			}
		}
	}
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
        bkPrintf("*** WARNING *** bgExitGui was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	TBGuiElement *guiElement;

	guiElement = (TBGuiElement *) MALLOCEX(sizeof(TBGuiElement), (uint32)"GUI Menu");
	guiElement->prev = bGuiInfo.elements.prev;
	guiElement->next = &bGuiInfo.elements;
	bGuiInfo.elements.prev->next = guiElement;
	bGuiInfo.elements.prev = guiElement;

	guiElement->type = BGUIELEMENT_MENU;
	guiElement->flags = 0;
	guiElement->menu.flags = flags & BGUIMENUFLAG_POPUP;
	guiElement->menu.noofItems = 0;
	guiElement->parent = NULL;
	guiElement->menu.focusItem = -1;

	return guiElement;
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
        bkPrintf("*** WARNING *** bgDeleteElement was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	if ((menuPtr->menu.flags & BGUIMENUFLAG_POPUP) != 0) {
		return;
	}

	if (menuPtr->type != BGUIELEMENT_MENU) {
		return;
	}

	bGuiInfo.menuBar = menuPtr;
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
	int32 noofItems;
	TBGuiMenuItem *menuItem;

	noofItems = (int32)menuPtr->menu.noofItems;
	if (noofItems == BGUIMENU_MAXITEMS) {
		return NULL;
	}

	menuItem = &menuPtr->menu.items[noofItems];

	// inline strcpy (don't wanna test it rn)
	if (text == NULL) {
		menuItem->text[0] = '\0';
	} else {
		char *src = text;
		char *dst = menuItem->text;
		int32 delta = (int32)(dst - src);
		char c;

		do {
			c = *src;
			src[delta] = c;
			src++;
		} while (c != '\0');
	}

	// store low 16 bits only
	menuItem->flags = (ushort)flags;
	menuItem->id    = (ushort)id;

	menuItem->subMenu = subMenu;
	if (subMenu != NULL) {
		subMenu->parent = menuPtr; // offset 0x5A0 in this layout
	}

	// len = strlen(menuItem->text) (disasm uses SCASB)
	{
		const char *p = menuItem->text;
		int32 len = 0;

		while (*p != '\0') {
			p++;
			len++;
		}

		// widthTableBase == (uint8*)bDefGuiFont.chars - 0x100  (=> 0x0065927C)
		// widthSum in ESI, stored to menuItem->width as word (SI)
		{
			const uchar *widthTableBase = (const uchar *)bDefGuiFont.chars - 0x100;
			int32 widthSum = 0;
			int32 i;

			for (i = 0; i < len; i++) {
				int32 ch = (int32)menuItem->text[i]; // MOVSX
				widthSum += *(const int32 *)(widthTableBase + (ch * 8));
			}

			menuItem->width = (ushort)widthSum;
		}
	}

	// ++noofItems
	menuPtr->menu.noofItems++;
	return menuItem;
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
        bkPrintf("*** WARNING *** bgInsertMenuItem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bgLoadElementFile was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bgFindElement was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bgCountMenuItems was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
        bkPrintf("*** WARNING *** bgDeleteMenuItem was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
    if ((itemIndex >= 0) && (itemIndex < menuPtr->menu.noofItems)) {
        return &menuPtr->menu.items[itemIndex];
    }

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
        bkPrintf("*** WARNING *** bgFindMenuItemById was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
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
	if (text != NULL) {
		// inline strcpy via delta trick (matches: EDX=itemPtr; EAX=text; SUB EDX,ESI; loop writes [EDX+EAX])
		{
			char *src = text;
			int32 delta = (int32)((char *)itemPtr - src);
			char c;

			do {
				c = *src;
				src[delta] = c;
				src++;
			} while (c != '\0');
		}

		// len via scan (SCASB pattern); uses source pointer (ESI) in disasm
		{
			int32 len = 0;
			while (text[len] != '\0') {
				len++;
			}

			// width table base: (uchar*)bDefGuiFont.chars - 0x100 == &chars[0] - 0x20*8
			{
				const uchar *widthTableBase = (const uchar *)bDefGuiFont.chars - 0x100;
				int32 widthSum = 0;
				int32 i;

				for (i = 0; i < len; i++) {
					int32 ch = (int32)(signed char)text[i]; // MOVSX
					widthSum += *(const int32 *)(widthTableBase + (ch * 8)); // TBGuiFontChar.width at +0
				}

				itemPtr->width = (ushort)widthSum; // MOV word ptr [EBX+0x2A], DX
			}
		}
	}

	if (id != 0) {
		itemPtr->id = (ushort)id;
	}

	if (subMenu != NULL) {
		itemPtr->subMenu = subMenu;
	}

	// flags = (~mask & old) | (flags & mask)  (word-sized store)
	{
		ushort mask16 = (ushort)flagMask;
		ushort old16 = itemPtr->flags;
		ushort new16 = (ushort)((old16 & (ushort)(~mask16)) | ((ushort)flags & mask16));
		itemPtr->flags = new16;
	}
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
	TBGuiInfo *guiInfo;

	switch(colour) {
	case EGUICOLOUR_TEXTBACK:
		guiInfo = (TBGuiInfo *)bGuiInfo.textBack;
		break;
	case EGUICOLOUR_MENUFORE:
		guiInfo = (TBGuiInfo *)bGuiInfo.menuFore;
		break;
	case EGUICOLOUR_MENUBACK:
		guiInfo = (TBGuiInfo *)bGuiInfo.menuBack;
		break;
	case EGUICOLOUR_DLGFORE:
		guiInfo = (TBGuiInfo *)bGuiInfo.dlgFore;
		break;
	case EGUICOLOUR_DLGBACK:
		guiInfo = (TBGuiInfo *)bGuiInfo.dlgBack;
		break;
	case EGUICOLOUR_HIGHFORE:
		guiInfo = (TBGuiInfo *)bGuiInfo.highFore;
		break;
	case EGUICOLOUR_HIGHBACK:
		guiInfo = (TBGuiInfo *)bGuiInfo.highBack;
		break;
	default:
		guiInfo = &bGuiInfo;
	}

	guiInfo->textFore[0] = r;
	guiInfo->textFore[1] = g;
	guiInfo->textFore[2] = b;
}
