// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : BookList.cpp
//   Purpose : List of books - handles drawing, update etc.
// Component : Taz
//		Info : // PP: The purpose of this is to get some control over our books,
//					eg. making sure that they close gracefully, with as little work as possible.
//					Also, the list should make it RIDICULOUSLY simple to use books in the future.
//					And the bulk drawing should increase efficiency too.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"				// PP: global stuff
#include "main.h"				// PP: main stuff
#include "TextFX.h"				// PP: Text effects, eg. button glyphs, coloured text
#include "LinkedList.h"			// PP: Linked List class - please use it!
#include "Stack.h"				// PP: Stack class - please use it!

#include "BookList.h"			// PP: List of books - handles drawing, update etc.

#include "display.h"			// PP: TEMP

#include "MemCard.h"			// PP: BODGE; REMOVE AFTER TAZ

// PP: define BOOKLIST_DEBUG_SIZE to get a per-frame print of the size of the booklist
// PP: #define BOOKLIST_DEBUG_SIZE

static LINKEDLIST<BOOK>								bookList;											// PP: the Book List
STACK<BOOK, BOOKLIST_EXCLUSIVE_BOOK_STACK_DEPTH>	BookList_exclusiveBooks("BookList_exclusiveBooks");	// PP: the Exclusive Books stack (the topmost book in this stack will be the only one open)


/*	--------------------------------------------------------------------------------
	Function 	: BookList_add
	Purpose 	: add a book to the list
	Parameters 	: ptr to book to add to the list
	Returns 	: 
	Info 		: to be called by book constructors
*/
void BookList_add(BOOK* const book)
{
	bookList.append(book);
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_remove
	Purpose 	: remove a book from the list
	Parameters 	: ptr to book to remove from the list
	Returns 	: 
	Info 		: TEMP!!!! for debugging
*/
void BookList_remove(BOOK* const book)
{
	// PP: books holding "controller missing" messages (one per player)
	extern BOOK	missingControllerBooks[];

	// PP: TEMP TEST
	if(book == &missingControllerBooks[0])
	{
		int g=0;
	}

	if(book->onBookList)
	{
		bookList.remove(book);
		book->onBookList=false;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAndFreeButDontNull
	Purpose 	: specify that a book should be closed and then, when it has finished closing, be deleted
	Parameters 	: ptr to book to close and free
	Returns 	: 
	Info 		: PLEASE DON'T CALL THIS - PLEASE CALL BookList_closeAndFree!
*/
void BookList_closeAndFreeButDontNull(BOOK* const book)
{
	if (!book->onBookList)
	{
		delete book;
	}
	else
	{
		book->flags |= BKFLAG_FREEWHENCLOSED;

		book->close();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAndShutdown
	Purpose 	: specify that a book should be closed and then, when it has finished closing, be shut-down
	Parameters 	: ref to book to close and shut-down
	Returns 	: 
	Info 		: 
*/
void BookList_closeAndShutdown(BOOK& book)
{
	if (!book.onBookList)
	{
		book.shutdown();
	}
	else
	{
		book.flags |= BKFLAG_SHUTDOWNWHENCLOSED;

		book.close();
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_update
	Purpose 	: update the book list and the books within it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BookList_update(void)
{
	BOOK*	book;

#ifdef BOOKLIST_DEBUG_SIZE
	bkPrintf("booklist size: %d\n", bookList.getSize());
#endif// PP: def BOOKLIST_DEBUG_SIZE

	// PP: NASTY MASTER BODGE: REMOVE AFTER TAZ: fake exclusive-book status for the memory card book
	if(MemCard_book.flags & BKFLAG_OPEN)
	{
		MemCard_book.privateUpdate();
	}
	else
	{
		for(int i=bookList.getSize()-1; i>=0; i--)
		{
			book=bookList.get(i);

			ASSERT(!(((~book->flags) & BKFLAG_OPEN)&&(book->flags & (BKFLAG_FREEWHENCLOSED|BKFLAG_SHUTDOWNWHENCLOSED))));

			book->privateUpdate();
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAll
	Purpose 	: close all the books on the Book List
	Parameters 	: (opt/false) close books instantly (without exit effects)
	Returns 	: 
	Info 		: 
*/
void BookList_closeAll(const bool instant)
{
	BOOK*	book;

	for(int i=bookList.getSize()-1; i>=0; i--)
	{
		book=bookList.get(i);

		book->close(0.0f, instant);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_finishCloses
	Purpose 	: force any closing books on the Book List to finish closing immediately, 
					and either free or shutdown if their appropriate flags are set.
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: Called by FreeScene
*/
void BookList_finishCloses(void)
{
	BOOK*	book;

	for(int i=bookList.getSize()-1; i>=0; i--)
	{
		book=bookList.get(i);

		// PP: this will override a slow close in progress
		book->close(0.0f, true);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_draw
	Purpose 	: draw all the books in the book list
	Parameters 	: (opt/false) also draw debug stuff for the books
	Returns 	: 
	Info 		: 
*/
void BookList_draw(const bool debug)
{
	BOOK*	book;

	SAVESTATES;

	SETPROJECTION(2D);
	ENABLEBLEND(TRUE);
	SETBLEND(BLEND);
	SETZWRITE(FALSE);
	SETZTEST(ALWAYS);

	bdSetIdentityObjectMatrix();

	// PP: Not sure where is the best place to put this.
	// PP: Sometimes the case arises that a wacky texture matrix gets left on.
	// PP: Seems to happen when drawing sprites and shadow maps (but not some other mystery ingredient) in the same frame
#if (BPLATFORM!=PS2 && BPLATFORM!=GAMECUBE)
	bdDisableTextureMatrix(0);
#endif

	// PP: NASTY MASTER BODGE: REMOVE AFTER TAZ: fake exclusive-book status for the memory card book
	if(MemCard_book.flags & BKFLAG_OPEN)
	{
		MemCard_book.privateDraw();
	}
	else
	{
		for(int i=bookList.getSize()-1; i>=0; i--)
		{
			book=bookList.get(i);

			// NH: Only draw the book if autoDraw has been set
			if (book->getAutoDraw())
			{
				book->privateDraw();
			
	#ifdef _TAZDEBUG

				if(debug)
				{
					if(book->flags & BKFLAG_OPEN)
					{
						bdSetIdentityObjectMatrix();
						book->drawScreenRects();
						book->drawDebug();
					}
				}
			
	#endif// PP: def _TAZDEBUG
			}
		}
	}

	SETPROJECTION(3D);

	RESTORESTATES;
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_getExclusiveBook
	Purpose 	: get a ptr to the exclusive book
	Parameters 	: a ptr to the exclusive book
	Returns 	: 
	Info 		: 
*/
BOOK* BookList_getExclusiveBook(void)
{
	extern STACK<BOOK, BOOKLIST_EXCLUSIVE_BOOK_STACK_DEPTH>		BookList_exclusiveBooks;

	return BookList_exclusiveBooks.getTopItem();
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_pushExclusiveBook
	Purpose 	: push an exclusive book onto the top of the exclusive book stack
	Parameters 	: new exclusive book
	Returns 	: 
	Info 		: 
*/
void BookList_pushExclusiveBook(BOOK* const book)
{
	BOOK*	xBook;
	BOOK*	tempBook;

	xBook=BookList_getExclusiveBook();

	if(xBook == NULL)
	{
		// PP: there is currently no exclusive book.
		// PP: all open books must be closed...

		for(int b=bookList.getSize()-1; b>=0; b--)
		{
			tempBook=bookList.get(b);

			// PP: we don't wanna be closing our new exclusive book!
			if(tempBook == book)
			{
				continue;
			}

			// PP: nor do we want to be closing any co-exclusive books
			if((~tempBook->flags) & BKFLAG_COEXCLUSIVE)
			{
				continue;
			}

			// PP: if the book is open and not closing,
			// PP: flag it as 'open-when-no-exclusive'
			if(tempBook->flags & BKFLAG_OPEN)
			{
				if(~(tempBook->flags) & BKFLAG_CLOSING)
				{
					tempBook->close();
					tempBook->flags |= BKFLAG_OPENWHENNOEXCLUSIVE;
				}
			}
		}
	}
	else
	{
		// PP: there is already an exclusive book...

		// PP: clear the exclusive flag of the current exclusive book
		xBook->flags &= ~BKFLAG_EXCLUSIVE;

		if((~xBook->flags) & BKFLAG_COEXCLUSIVE)
		{
			// PP: if the xBook isn't closing,
			// PP: flag it as 'open-when-no-exclusive'.
			// PP: the 'open-when-no-exclusive' flag will get cleared if the book is closed while non-exclusive
			if(~(xBook->flags) & BKFLAG_CLOSING)
			{
				xBook->close();
				xBook->flags |= BKFLAG_OPENWHENNOEXCLUSIVE;
			}
		}
	}
	
	// PP: flag the new exclusive book as such
	book->flags |= BKFLAG_EXCLUSIVE;

	// PP: push it onto the top of the stack
	BookList_exclusiveBooks.push(book);
}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_popExclusiveBook
	Purpose 	: pop an exclusive book off of the top of the exclusive book stack
	Parameters 	: ptr to the exclusive book
	Returns 	: 
	Info 		: 
*/
void BookList_popExclusiveBook(BOOK* const book)
{
	BOOK*	xBook;
	BOOK*	tempBook;

	xBook=BookList_getExclusiveBook();

	// PP: check we're not mistaken about the identity of the exclusive book
	ASSERT(book == xBook);

	// PP: AWAY WITH YOU! PEASANT!...

	xBook->flags &= ~BKFLAG_EXCLUSIVE;

	// PP: NOTE: THIS FUNCTION WILL ONLY BE CALLED BY THE EXCLUSIVE BOOK CLOSING, SO DON'T WORRY ABOUT CLOSING THE EXCLUSIVE BOOK; IT'S GONE

	do
	{
		// PP: NOTE: the only reason a book popped off the xbook stack would have BKFLAG_OPENWHENNOEXCLUSIVE cleared
		// PP: is that it was closed while somewhere below the top of the stack - so it's unsuitable to become the new xbook - no need to do anything with it now

		// PP: pop the exclusive book off the stack
		BookList_exclusiveBooks.pop();

		xBook=BookList_getExclusiveBook();
	}
	while((xBook != NULL) && ((~xBook->flags) & BKFLAG_OPENWHENNOEXCLUSIVE));



	if(xBook == NULL)
	{
		// PP: there is now no exclusive book.
		// PP: all 'open-when-no-exclusive books' must be opened...

		for(int b=bookList.getSize()-1; b>=0; b--)
		{
			tempBook=bookList.get(b);

			if(tempBook->flags & BKFLAG_OPENWHENNOEXCLUSIVE)
			{
				tempBook->open();

				tempBook->flags |= BKFLAG_OPENWHENNOEXCLUSIVE;
			}
		}
	}
	else
	{// PP: flag exclusive book's successor as being the new exclusive book
		xBook->flags &= ~BKFLAG_OPENWHENNOEXCLUSIVE;
		xBook->flags |= BKFLAG_EXCLUSIVE;

		// PP: ***************************
		// PP: NOTE: to start with at least, we'll be unable to change the pages of books while they are hidden by an exclusive book
		// PP: ***************************

		xBook->open();
	}
}
