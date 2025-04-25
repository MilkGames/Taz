// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiScrollBar.h
//   Purpose : gui scrollbar class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUISCROLLBAR_H__
#define __FGUI_CBGUISCROLLBAR_H__


// ********************************************************************************
// Constants and Macros

#define FGUI_SCROLLBARFLAGVERTICAL		0x00000001u				// vertical scrollbar
#define FGUI_SCROLLBARFLAGHORIZONTAL	0x00000002u				// horizontal scrollbar


// ********************************************************************************
// Types, Structures and Classes

// gui window base class
typedef class CBGuiScrollBar : public CBGuiObject {
	public:
							CBGuiScrollBar();					// constructor
							CBGuiScrollBar(int iFlags);
		virtual 			~CBGuiScrollBar();					// destructor
		virtual void		SetPosition(int iX, int iY, int iWidth, int iHeight);	// set scrollbar's position and size
		virtual CBGuiObject	*HitTest(int iX, int iY);			// hit test object
		virtual int			CanDrag();							// returns 1
		virtual void		MoveObject(int iX, int iY, int iDx, int iDy);
		virtual void		Paint1();							// paint scrollbar (before children)
		virtual void		Paint2();							// paint scrollbar (after children)
		virtual void		Activate();							// scrollbar activated
		void				SetScrollBarFlag(int iFlags);
		void				ClearScrollBarFlag(int iFlags);

		int					m_iScrollBarFlags;					// scroll bar specific flags
		int					m_iSBx, m_iSBy, m_iSBw, m_iSBh;		// bar x, y, w, h
		float				m_fOffset;							// offset for slider (0 - 1)
		float				m_fClientSize;						// size of client area (i.e. window)

		float				m_fProportion;						// proportion of the window displayed

} CBGuiScrollBar;


#endif		//__FGUI_CBGUIBUTTON_H__
