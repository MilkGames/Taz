// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiMenuItem.h
//   Purpose : gui menu item class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUIMENUITEM_H__
#define __FGUI_CBGUIMENUITEM_H__


// ********************************************************************************
// Constants and Macros

#define FGUI_MENUITEMTEXTLEN			64

// menu item flags
#define FGUI_MENUITEMFLAGSEPARATOR		0x00000001u							// separator
#define FGUI_MENUITEMFLAGDISABLED		0x00000002u							// disabled
#define FGUI_MENUITEMFLAGCHILDMENU		0x00000004u							// item has a child menu


// ********************************************************************************
// Types, Structures and Classes

class CBGuiMenu;

// gui menu item class
typedef class CBGuiMenuItem : public CBGuiObject {
	public:
							CBGuiMenuItem();						// constructor
							CBGuiMenuItem(int iFlags);
		virtual 			~CBGuiMenuItem();						// destructor

		virtual void		SetPosition(int iX, int iY, int iWidth, int iHeight);	// set object's position and size

		virtual void		Paint1();								// paint menu (before children)
		virtual void		Paint2();								// paint menu (after children)

		void				AddMenuItem(CBGuiMenuItem *pItem);
		void				AddMenu(CBGuiMenu *pMenu);

		void				SetMenuItemFlag(int iFlag);
		void				ClearMenuItemFlag(int iFlag);

		int					SetMenuItemText(ushort *pText);			// set the menu item text

		virtual void		BringToTop();							// bring this object to the top, along with any parents
		virtual void		Activate();
		virtual void		Deactivate();
		void				Selected();

		ushort				m_pMenuItemText[FGUI_MENUITEMTEXTLEN];	// window text
		int					m_iMenuItemFlags;						// menu item specific flags
		CBGuiMenuItem		*m_pNextItem;							// pointer to next menu item
		CBGuiMenu			*m_pParentMenu;
		CBGuiMenu			*m_pChildMenu;

		int					m_iMenuSelected;						// menu item is selected

} CBGuiMenuItem;


#endif		//__FGUI_CBGUIMENUITEM_H__
