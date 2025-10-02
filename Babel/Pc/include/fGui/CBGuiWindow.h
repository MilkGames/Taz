// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiWindow.h
//   Purpose : gui window base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUIWINDOW_H__
#define __FGUI_CBGUIWINDOW_H__


// ********************************************************************************
// Constants and Macros

#define FGUI_WINDOWTEXTLEN		64

#define FGUI_WINDOWFLAGTITLEBAR 0x00000001u						// window has a title bar (default)
#define FGUI_WINDOWFLAGSYSTEM	0x00000002u						// window is the system window
#define FGUI_WINDOWFLAGRESIZE	0x00000004u						// window is resizeable



// ********************************************************************************
// Types, Structures and Classes

// gui window base class
typedef class CBGuiWindow : public CBGuiObject {
	public:
							CBGuiWindow();						// constructor
							CBGuiWindow(int iFlags);
		virtual 			~CBGuiWindow();						// destructor

		virtual void		CalcClip();							// calculate screen clipping co-ords
		virtual void		SetPosition(int iX, int iY, int iWidth, int iHeight);	// set window's position and size
		virtual void		Paint1();							// paint window (before children)
		virtual void		Paint2();							// paint window (after children)

		virtual CBGuiObject	*HitTest(int iX, int iY);			// hit test object

		void				SetWindowFlag(int iFlag);			// set window flag
		void				ClearWindowFlag(int iFlag);			// clear window flag

		int					SetWindowText(ushort *pText);		// set the window title
		
		virtual int			CanDrag();							// returns 1

		virtual void		MoveObject(int iX, int iY, int iDx, int iDy);

		virtual void		Activate();							// activate this window

		void				GetChildExtents();					// get extent of children

		void				SetHorizScrollBar(float xProp, float xPos, float size);
		void				SetVertScrollBar(float yProp, float yPos, float size);

		ushort				m_pWindowText[FGUI_WINDOWTEXTLEN];	// window text
		int					m_iWindowFlags;						// window specific flags

		int					m_iMinChildX, m_iMaxChildX;
		int					m_iMinChildY, m_iMaxChildY;
		
		CBGuiScrollBar		*m_pHorizScroll;
		CBGuiScrollBar		*m_pVertScroll;


} CBGuiWindow;


#endif		//__FGUI_CBGUIWINDOW_H__
