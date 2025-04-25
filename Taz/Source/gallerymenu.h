#ifndef __H_GALLERYMENU
#define __H_GALLERYMENU

enum EGallerySubStates
{
	EX_GALLERYS,
	EX_WORKTOP,
};

enum EGalleryLoadStates
{
	EG_NONE,
	EG_LOAD,
};

extern char *galleryIconNames[];

/*	--------------------------------------------------------------------------------
	Function 	: AddGalleryIcon
	Purpose 	: 
	Parameters 	: Adds Icons to the gallery page
	Returns 	: 
	Info 		:
*/

bool AddGalleryIcon(struct TAG_GALLERYPAGE *ptr ,int iconNo, bool openFlag1, bool openFlag2, bool openFlag3, bool newLine);

/*	--------------------------------------------------------------------------------
	Function 	: AddGalleryChapter
	Purpose 	: Add a gallery chapter to the Gallery Paperbook (Puts all graphics defined in galleryImages and text on the page)
	Parameters 	: ptr (Pointer to the Gallery Page), galleryTitle (Title of gallery), galleryNo (Number of gallery)
	Returns 	: 
	Info 		:
*/

void AddGalleryChapter(struct TAG_GALLERYPAGE *ptr, char* galleryTitle, int galleryNo);

/*	--------------------------------------------------------------------------------
	Function 	: CreateGalleryMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void CreateGalleryMenuPage(struct TAG_FRONTENDMENU *menu);

/*	--------------------------------------------------------------------------------
	Function 	: UpdateGalleryMenuPage
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void UpdateGalleryMenuPage(struct TAG_FRONTENDMENU *menu, struct TAG_PADCONTROLSTATUS *controller);

/*	--------------------------------------------------------------------------------
	Function 	: galleryBookCloseCallback
	Purpose 	: receives notification that the gallery paper-book has closed
	Parameters 	: ptr to paperbook that has closed
	Returns 	: 
	Info 		: // PP: must match the PaperBookCloseCallback typedef (PaperBook.h)
*/

void galleryBookCloseCallback(struct TAG_PAPERBOOK *const paperBook);

/*	--------------------------------------------------------------------------------
	Function 	: galleryFirstPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on the first page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/

void galleryFirstPageCallback(void);

/*	--------------------------------------------------------------------------------
	Function 	: galleryCentralPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on a central page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void galleryCentralPageCallback(void);

/*	--------------------------------------------------------------------------------
	Function 	: galleryLastPageCallback
	Purpose 	: receives notification that the gallery paper-book has turned/opened on the last page
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void galleryLastPageCallback(void);

#endif