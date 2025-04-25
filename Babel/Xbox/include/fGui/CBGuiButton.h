// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiButton.h
//   Purpose : gui button base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUIBUTTON_H__
#define __FGUI_CBGUIBUTTON_H__


// ********************************************************************************
// Constants and Macros

#define FGUI_BUTTONTEXTLEN		64

// ********************************************************************************
// Types, Structures and Classes

// gui window base class
typedef class CBGuiButton : public CBGuiObject {
	public:
							CBGuiButton();						// constructor
							CBGuiButton(int iFlags);
		virtual 			~CBGuiButton();						// destructor

		virtual void		Paint1();							// paint button (before children)
		virtual void		Paint2();							// paint button (after children)

		void				SetButtonFlag(int iFlag);
		void				ClearButtonFlag(int iFlag);

		virtual void		Activate();

		int					SetButtonText(ushort *pText);		// set the button caption

		ushort				m_pButtonText[FGUI_BUTTONTEXTLEN];	// window text
		int					m_iButtonFlags;						// button specific flags

} CBGuiButton;


#endif		//__FGUI_CBGUIBUTTON_H__
