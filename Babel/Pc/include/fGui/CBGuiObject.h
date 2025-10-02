// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : CBGuiObject.h
//   Purpose : gui object base class
// Component : BFC Gui
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __FGUI_CBGUIOBJECT_H__
#define __FGUI_CBGUIOBJECT_H__


// ********************************************************************************
// Constants and Macros

// object flags
#define FGUI_OBJECTFLAGVISIBLE	0x00000001u									//  object is visible (default)


// ********************************************************************************
// Types, Structures and Classes

// gui object base class
typedef class CBGuiObject {
	public:
							CBGuiObject();									// constructor
		virtual 			~CBGuiObject();									// destructor

		int					AddChild(CBGuiObject *pChild);					// add child to object
		int					RemoveChild(CBGuiObject *pChild);				// remove child from object

		virtual void		SetPosition(int iX, int iY, int iWidth, int iHeight);	// set object's position and size
		virtual void		MoveObject(int iX, int iY, int iDx, int iDy);
		virtual void		CalcClip();										// calculate screen clipping co-ords

		virtual void		Paint1() = NULL;								// paint object (before children)
		virtual void		Paint2() = NULL;								// paint object (after children)
		void				PaintAll();										// paint self and children

		void				SetObjectFlag(int flag);						// set object flag
		void				ClearObjectFlag(int flag);						// clear object flag

		virtual CBGuiObject	*HitTest(int iX, int iY);						// hit test object

		virtual int			CanDrag();										// this object can be dragged
		virtual void		BringToTop();									// bring this object to the top, along with any parents

		virtual void		Deactivate();									// deactivate this object
		virtual void		Activate();										// activate this object

		void				SetCallback(TBGuiCallback pFunc);

		void				SetId(int iId);

		CBGuiObject			*m_pParent;										// pointer to parent object
		CBGuiObject			*m_pChild;										// pointer to first child

		CBGuiObject			*m_pPrev;										// pointer to previous object
		CBGuiObject			*m_pNext;										// pointer to next object

		TBGuiCallback		m_pCallback;									// callback function

		int					m_iId;											// ID for this object

		int					m_iLocalX;										// object's local X position
		int					m_iLocalY;										// object's local Y position
		int					m_iGlobalX;										// object's global X position
		int					m_iGlobalY;										// object's global Y position
		int					m_iChildOffsetX;								// children's X offset
		int					m_iChildOffsetY;								// children's Y offset
		int					m_iWidth;										// object's pixel width
		int					m_iHeight;										// object's pixel height

		int					m_iClipX1;										// left clip co-ord
		int					m_iClipX2;										// right clip co-ord
		int					m_iClipY1;										// top clip co-ord
		int					m_iClipY2;										// bottom clip co-ord

		int					m_iChildClipX1;									// child left clip co-ord
		int					m_iChildClipX2;
		int					m_iChildClipY1;
		int					m_iChildClipY2;

		int					m_iObjectFlags;									// object flags
		int					m_iActivated;									// this object is activated
		
} CBGuiObject;


#endif		//__FGUI_CBGUIOBJECT_H__
