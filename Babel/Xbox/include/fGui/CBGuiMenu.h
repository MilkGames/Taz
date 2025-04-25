// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiMenu.h
//   Purpose : gui menu base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUIMENU_H__
#define __FGUI_CBGUIMENU_H__


// ********************************************************************************
// Constants and Macros

// menu flags
#define FGUI_MENUFLAGPOPUP		0x00000001u									//  menu is a popup menu


// ********************************************************************************
// Types, Structures and Classes

// gui menu base class
typedef class CBGuiMenu : public CBGuiObject {
	public:
							CBGuiMenu();						// constructor
							CBGuiMenu(int iFlags);
		virtual 			~CBGuiMenu();						// destructor

		virtual void		SetPosition(int iX, int iY, int iWidth, int iHeight);	// set object's position and size

		virtual void		Paint1();							// paint menu (before children)
		virtual void		Paint2();							// paint menu (after children)

		virtual CBGuiObject	*HitTest(int iX, int iY);						// hit test object

		void				SetMenuFlag(int iFlag);				// set menu flag
		void				ClearMenuFlag(int iFlag);			// clear menu flag

		void				AddMenuItem(CBGuiMenuItem *pItem);	// add menu item to menu

		virtual void		Activate();
		virtual void		Deactivate();
		void				Selected();

		int					m_iMenuFlags;						// menu specific flags

		CBGuiMenuItem		*m_pParentMenuItem;

		CBGuiMenuItem		*m_pMenuItems;						// menu items
		int					m_iMenuSelected;					// menu is selected

} CBGuiMenu;


#endif		//__FGUI_CBGUIMENU_H__
