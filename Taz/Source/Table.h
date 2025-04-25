// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Table.h
//   Purpose : Table page-item type - lets you line up items into a grid arrangement
// Component : Taz
//		Info : subclass of PAGE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_TABLE
#define __H_TABLE

#include "Textfx.h"							// PP: Text effects, eg. button glyphs, coloured text


// PP: By default, TABLEs have both new-line and new-column navigation wrapping.
// PP: You can override this by calling 'setNavigationWrap', passing in the PAGEFLAG_NAVWRAP_ flags that you want.
#define TABLE_DEFAULT_NAVIGATION_WRAPPING			(PAGEFLAG_NAVWRAP_NEWLINE|PAGEFLAG_NAVWRAP_NEWCOLUMN)


typedef struct TAG_TABLE	:	public PAGE
{
private:

	LINKEDLIST<float>		columnWidths;			// PP: the widths of the columns in the table.  The size of this list is used to determine the number of columns in the table
	int						numColumns;				// PP: number of columns in the table
	bool					explicitNumColumns;		// PP: the number of columns has been explicitly set

	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::getNumColumns
		Purpose 	: get the number of columns in the table
		Parameters 	: 
		Returns 	: the number of columns in the table!
		Info 		: private only because I want to know about it if anything else wants to use it
	*/
	inline int getNumColumns(void) const
	{
		return this->numColumns;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::init
		Purpose 	: initialise the page
		Parameters 	: (opt/NULL)name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function, (opt/NULL)has-closed function
		Returns 	: 
		Info 		: This overrides PAGE::init
	*/
	void init(const char* const nameIn=NULL, PageFunc const updateFuncIn=NULL, PageFunc const openFuncIn=NULL, PageFunc const closeFuncIn=NULL, PageFunc const hasOpenedFuncIn=NULL, PageFunc const hasClosedFuncIn=NULL);

protected:


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::setRect
		Purpose 	: set the Rectangle for the table (and update its contents accordingly)
		Parameters 	: rectangle
		Returns 	: (PAGEITEM*)this
		Info 		: this overrides PAGE::setRect
	*/
	PAGEITEM* setRect(const RECTANGLE& rectIn);


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::align
		Purpose 	: calculate alignment for the table's contents
		Parameters 	: 
		Returns 	: 
		Info 		: // PP: this overrides PAGE::align
						// PP: this gets recurred down the contents hierarchy
	*/
	void align(void);


public:


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::getItemsUnsuitabilityAsNavigationTarget
		Purpose 	: determine how unsuitable an item is as a navigation target for a page
		Parameters 	: ptr item to assess, position of currently selected item, angle of navigation in radians
		Returns 	: an unsuitability value
		Info 		: // PP: this overrides PAGE::getItemsUnsuitabilityAsNavigationTarget
	*/
	float getItemsUnsuitabilityAsNavigationTarget(PAGEITEM* const item, const TBVector2D currentPos, const float navAng);


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::getNavigationCandidate
		Purpose 	: get a pointer to the item within this page which is
						the most suitable item to which to navigate, from a specified
						current position at a specified navigation angle
		Parameters 	: (in/out) ref to a NAVCANDIDATE_INFO structure (TextFX.h)
		Returns 	: a pointer to the item within this page which is the
						most suitable item to which to navigate
		Info 		: this overrides PAGE::getNavigationCandidate
	*/
	TAG_PAGEITEM* getNavigationCandidate(NAVCANDIDATE_INFO& info);


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::TAG_TABLE
		Purpose 	: constructor; initialises the page
		Parameters 	: 
		Returns 	: new TABLE instance
		Info 		: this used to have an (opt/null) name param - now handled by the next overload
	*/
	TAG_TABLE(void)
	{
		this->init();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::TAG_TABLE
		Purpose 	: constructor; initialises the table
		Parameters 	: name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function
		Returns 	: new TABLE instance
		Info 		: 
	*/
	TAG_TABLE(const char* const nameIn, PageFunc const update=NULL, PageFunc const open=NULL, PageFunc const close=NULL)
	{
		this->init(nameIn, update, open, close);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::TAG_TABLE
		Purpose 	: constructor; initialises the table
		Parameters 	: number of columns
		Returns 	: new TABLE instance
		Info 		: 
	*/
	TAG_TABLE(const uchar numColumnsIn)
	{
		this->init();
		this->setNumColumns(numColumnsIn);
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::setNumColumns
		Purpose 	: explicitly set the number of columns in the table
		Parameters 	: number of columns
		Returns 	: 
		Info 		: 
	*/
	void setNumColumns(const uchar numColumnsIn)
	{
		this->numColumns=numColumnsIn;
		this->explicitNumColumns=true;		
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::insertItem
		Purpose 	: insert an item onto the end of the TABLE
		Parameters 	: ptr to new item, (opt/true) whether or not to create a new row
		Returns 	: ptr to new item
		Info 		: This overrides PAGE::insertItem.
						// PP: NOTE: if you've explicitly set the number of columns for the table,
						the newRow param is ignored.
	*/
	PAGEITEM* insertItem(PAGEITEM* const item, const int newRow=true);


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::shutdown
		Purpose 	: free up space used by the TABLE and its contents
		Parameters 	: 
		Returns 	: 
		Info 		: this overrides PAGE::shutdown, to free-up TABLE-specific stuff
	*/
	void shutdown(void)
	{
		// PP: do normal page shutdown
		PAGE::shutdown();

		// PP: free-up column-width list
		this->columnWidths.kill();
	}


	/*	--------------------------------------------------------------------------------
		Function 	: TABLE::~TAG_TABLE
		Purpose 	: destructor; calls shutdown
		Parameters 	: 
		Returns 	: 
		Info 		: WHY ON EARTH DO I NEED THIS?  WHY THE HELL DOESN'T THE PAGE DESTRUCTOR CALL THE TABLE SHUTDOWN?  WHAT'S GOING ON????!!!
	*/
	~TAG_TABLE(void)
	{
		TAG_TABLE::shutdown();
	}


}TABLE, _TABLE BALIGN16;

#endif// PP: ndef __H_TABLE