// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Table.cpp
//   Purpose : Table page-item type - lets you line up items into a grid arrangement
// Component : Taz
//		Info : subclass of PAGE
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
//#include "control.h"			// PP: provide basic input and control functions
#include "Table.h"				// PP: Table page-item type - lets you line up items into a grid arrangement


/*inline PAGE* TABLE::getColumn(const int index) const
{
	return (PAGE*)(this->getItem(index));
}*/


/*	--------------------------------------------------------------------------------
	Function 	: TABLE::init
	Purpose 	: initialise the page
	Parameters 	: (opt/NULL)name, (opt/NULL)update function, (opt/NULL)open function, (opt/NULL)close function, (opt/NULL)has-closed function
	Returns 	: 
	Info 		: This overrides PAGE::init
*/
void TABLE::init(const char* const nameIn, PageFunc const updateFuncIn, PageFunc const openFuncIn, PageFunc const closeFuncIn, PageFunc const hasOpenedFuncIn, PageFunc const hasClosedFuncIn)
{
	PAGE::init(nameIn, updateFuncIn, openFuncIn, closeFuncIn, hasOpenedFuncIn, hasClosedFuncIn);// PP: does this stuff still get used?

	this->numColumns=0;
	this->explicitNumColumns=false;// PP: the number of columns has not been explicitly set

	// PP: set default navigation wrapping modes for a table
	this->setNavigationWrap(TABLE_DEFAULT_NAVIGATION_WRAPPING);
}


/*	--------------------------------------------------------------------------------
	Function 	: TABLE::getItemsUnsuitabilityAsNavigationTarget
	Purpose 	: determine how unsuitable an item is as a navigation target for a page
	Parameters 	: item to assess, ptr to current item, position of currently selected item, angle of navigation in radians
	Returns 	: an unsuitability value, or sub-zero for completely unsuitable
	Info 		: // PP: this overrides PAGE::getItemsUnsuitabilityAsNavigationTarget
*/
float TABLE::getItemsUnsuitabilityAsNavigationTarget(PAGEITEM* const item, const TBVector2D currentPos, const float navAng)
{
	// PP: TEMP TEST
	//return 0.1f;

	return PAGE::getItemsUnsuitabilityAsNavigationTarget(item, currentPos, navAng);
}


/*	--------------------------------------------------------------------------------
	Function 	: TABLE::setRect
	Purpose 	: set the Rectangle for the table (and update its contents accordingly)
	Parameters 	: rectangle
	Returns 	: (PAGEITEM*)this
	Info 		: this overrides PAGE::setRect
*/
PAGEITEM* TABLE::setRect(const RECTANGLE& rectIn)
{
	PAGE::setRect(rectIn);

	// PP: moved stuff out into align()

	return (PAGEITEM*)this;
}


/*	--------------------------------------------------------------------------------
	Function 	: TABLE::align
	Purpose 	: calculate alignment for the table's contents
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: this overrides PAGE::align
					// PP: this gets recurred down the contents hierarchy
*/
void TABLE::align(void)
{
	// PP: base class align
	PAGE::align();

	if(this->getNumColumns() > 0)
	{
		PAGEROW*	row;
		PAGEITEM*	item;
		float		itemWidth;
		int			c,r;							// PP: loop counters
		float		totalRequiredWidth=0.0f;

		for(c=this->getNumColumns()-1; c>=0; c--)
		{
			this->columnWidths[c]=0.0f;

			for(r=this->getNumRows()-1; r>=0; r--)
			{
				// PP: get the required width of column #c in row #r

				row=this->getRow(r);

				item=row->getItem(c);

				if(item == NULL)
				{
					continue;
				}

				itemWidth=row->getColumnWidth(c);

				if(itemWidth > this->columnWidths[c])
				{
					this->columnWidths[c]=itemWidth;
				}
			}

			totalRequiredWidth+=columnWidths[c];
		}

		// PP: cool, we have a full set of minumum column widths now

		// PP: If vacuupacking is off, apply uniform x padding to the columns...

		if(this->flags & PAGEFLAG_HSLACK)// PP: if(!(this->flags&PAGEFLAG_VACUUMPACK))
		{
			float		padWidth;

			padWidth=(this->rect.width()-totalRequiredWidth)/this->getNumColumns();

			for(c=this->getNumColumns()-1; c>=0; c--)
			{
				this->columnWidths[c]+=padWidth;
			}
		}

		// PP: apply the column widths to each row...

		for(r=this->getNumRows()-1; r>=0; r--)
		{
			row=this->getRow(r);

			row->setColumnWidths(this->columnWidths);
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: PAGE::getNavigationCandidate
	Purpose 	: get a pointer to the item within this page which is
					the most suitable item to which to navigate, from a specified
					current position at a specified navigation angle
	Parameters 	: (in/out) ref to a NAVCANDIDATE_INFO struct (TextFX.h)
	Returns 	: a pointer to the item within this page which is the
					most suitable item to which to navigate
	Info 		: this overrides PAGEITEM::getNavigationCandidate
*/
PAGEITEM* TABLE::getNavigationCandidate(NAVCANDIDATE_INFO& info)
{
	if(info.currentItem->container == this)
	{
		int32		dir;
		PAGEITEM*	item;
		int32		rowIndex;
		int32		columnIndex;

		dir=bmFloatToInt(bmFloor((Aabs(info.navAng)+QUARTPI)/HALFPI));
		dir%=4;

		rowIndex=info.currentItem->rowIndex;
		columnIndex=info.currentItem->columnIndex;

		// PP: initially, assume that the candidate getting returned is NOT the result of a navigation wrap within this pageitem
		info.wrapOut=false;

		switch(dir)
		{
		case 0:// PP: UP
			{
				do
				{
					if(--rowIndex < 0)
					{
						if(this->flags & PAGEFLAG_NAVWRAP_NEWCOLUMN)
						{
							rowIndex=this->getNumRows()-1;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;

							if(columnIndex == 0)
							{
								// PP: wrap from the top item of the first column to the bottom item of the last column
								columnIndex=this->getNumColumns()-1;
							}
							else
							{
								// PP: wrap from the top of one column to the bottom of the one before
								columnIndex--;
							}
						}
						else if(this->flags&PAGEFLAG_NAVWRAP_Y)
						{
							// PP: wrap from one edge to the other
							rowIndex=this->getNumRows()-1;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
						}
						else
						{
							return NULL;
						}
					}

					item=this->getRow(rowIndex)->getItem(columnIndex);

				}while(item && !item->selectable());
			}
			break;

		case 1:// PP: RIGHT
			{
				do
				{
					if(++columnIndex > (this->getNumColumns()-1))
					{
						if(this->flags & PAGEFLAG_NAVWRAP_NEWLINE)
						{
							// PP: wrap from the end of one line to the start of the next
							columnIndex=0;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
							
							if(rowIndex == (this->getNumRows()-1))
							{
								// PP: go from last item of last row to first item of first row
								rowIndex=0;
							}
							else
							{
								rowIndex++;
							}
						}
						else if(this->flags&PAGEFLAG_NAVWRAP_X)
						{
							// PP: wrap from one edge to the other
							columnIndex=0;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
						}
						else
						{
							return NULL;
						}
					}

					item=this->getRow(rowIndex)->getItem(columnIndex);

				}while(item && !item->selectable());
			}
			break;

		case 2:// PP: DOWN
			{
				do
				{
					if(++rowIndex > (this->getNumRows()-1))
					{
						if(this->flags & PAGEFLAG_NAVWRAP_NEWCOLUMN)
						{
							rowIndex=0;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;

							if(columnIndex == (this->getNumColumns()-1))
							{
								// PP: wrap from the bottom item of the last column to the top item of the first column
								columnIndex=0;
							}
							else
							{
								// PP: wrap from the bottom of one column to the top of the next
								columnIndex++;
							}
						}
						else if(this->flags & PAGEFLAG_NAVWRAP_X)
						{
							// PP: wrap from one edge to the other
							rowIndex=0;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
						}
						else
						{
							return NULL;
						}
					}

					item=this->getRow(rowIndex)->getItem(columnIndex);

				}while(item && !item->selectable());
			}
			break;

		case 3:// PP: LEFT
			{
				do
				{
					if(--columnIndex < 0)
					{
						if(this->flags & PAGEFLAG_NAVWRAP_NEWLINE)
						{
							// PP: wrap from the beginning of one line to the end of the one before
							columnIndex=this->getNumColumns()-1;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
							
							if(rowIndex == 0)
							{
								// PP: go from first item of first row to last item of last row
								rowIndex=this->getNumRows()-1;
							}
							else
							{
								rowIndex--;
							}
						}
						else if(this->flags&PAGEFLAG_NAVWRAP_X)
						{
							// PP: wrap from one edge to the other
							columnIndex=this->getNumColumns()-1;

							// PP: the candidate getting returned is the result of a navigation wrap within this pageitem
							info.wrapOut=true;
						}
						else
						{
							return NULL;
						}
					}

					item=this->getRow(rowIndex)->getItem(columnIndex);

				}while(item && !item->selectable());
			}
			break;

			NODEFAULT;
		}

		if(item == NULL)
		{
			return NULL;
		}
		else
		{
			if(item->selectable())
			{
				return item;
			}
			else
			{
				return NULL;
			}
		}
	}
	else// PP: this is not the container of info.currentItem
	{
		return PAGE::getNavigationCandidate(info);
	}

	return NULL;
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
PAGEITEM* TABLE::insertItem(PAGEITEM* const item, const int newRow)
{
	PAGEITEM*	rtn;
	int			columnIndex;
	int			reallyNewRow;		// PP: I can't think why I decided PAGE::insertItem should take an 'int' for newRow

	if(this->explicitNumColumns)
	{
		if(this->getNumRows() == 0)
		{
			reallyNewRow=true;// PP: or false, or whatever, doesn't actually matter
		}
		else
		{
			reallyNewRow=(this->getLastRow()->getNumItems() == this->numColumns);
		}
	}
	else
	{
		reallyNewRow=newRow;
	}

	rtn=PAGE::insertItem(item, reallyNewRow);

	if(!this->explicitNumColumns)
	{
		if(!newRow)
		{
			this->numColumns=MAX(this->numColumns, this->getLastRow()->getNumItems());
		}
	}

	return rtn;
}
