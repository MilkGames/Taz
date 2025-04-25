// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : BookList.h
//   Purpose : List of books - handles drawing, update etc.
// Component : Taz
//		Info : // PP: The purpose of this is to get some control over our books,
//					eg. making sure that they close gracefully, with as little work as possible.
//					Also, the list should make it RIDICULOUSLY simple to use books in the future.
//					And the bulk drawing should increase efficiency too.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __H_BOOKLIST
#define __H_BOOKLIST


// PP: the depth of the Book List's 'Exclusive Books' list.
// PP: These guys get complimentary drinks brought to them, have a power socket for their laptops and mobiles - AND they get extra leg room!
// PP: Only kidding.  They're just a bunch of pointers.
// PP: Dull, ordinary, pointers.
// PP: Pointers with brief cases!  And complimentary coffee!
// PP: Nah, seriously, they're just pointers.
// PP: Power-Exec' Management Pointers!
#define BOOKLIST_EXCLUSIVE_BOOK_STACK_DEPTH		3


/*	--------------------------------------------------------------------------------
	Function 	: BookList_add
	Purpose 	: add a book to the list
	Parameters 	: ptr to book to add to the list
	Returns 	: 
	Info 		: to be called by book constructors
*/
void BookList_add(BOOK* const book);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_remove
	Purpose 	: remove a book from the list
	Parameters 	: ptr to book to remove from the list
	Returns 	: 
	Info 		: 
*/
void BookList_remove(BOOK* const book);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAndFreeButDontNull
	Purpose 	: specify that a book should be closed and then, when it has finished closing, be deleted
	Parameters 	: ptr to book to close and free
	Returns 	: 
	Info 		: PLEASE DON'T CALL THIS - PLEASE CALL BookList_closeAndFree!
*/
void BookList_closeAndFreeButDontNull(BOOK* const book);


/*	--------------------------------------------------------------------------------
	Macro	 	: BookList_closeAndFree
	Purpose 	: specify that a book should be closed and then, when it has finished closing, be deleted
	Parameters 	: l-value ptr (don't cast) to book to close and free
	Value	 	: 
	Info 		: This ensures a graceful close by books that would otherwise snap out of view.
					Sets yer ptr to NULL so's you know it's gone and will have to be re-malloced if you want to open it again.
*/
#define BookList_closeAndFree(_book) \
		if((_book) != NULL) \
		{ \
			BookList_closeAndFreeButDontNull((BOOK*)(_book)); \
			(_book)=NULL; \
		}


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAndShutdown
	Purpose 	: specify that a book should be closed and then, when it has finished closing, be shut-down
	Parameters 	: ref to book to close and shut-down
	Returns 	: 
	Info 		: 
*/
void BookList_closeAndShutdown(BOOK& book);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_update
	Purpose 	: update the book list and the books within it
	Parameters 	: 
	Returns 	: 
	Info 		: 
*/
void BookList_update(void);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_closeAll
	Purpose 	: close all the books on the Book List
	Parameters 	: (opt/false) close books instantly (without exit effects)
	Returns 	: 
	Info 		: 
*/
void BookList_closeAll(const bool instant=false);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_finishCloses
	Purpose 	: force any closing books on the Book List to finish closing immediately, 
					and either free or shutdown if their appropriate flags are set.
	Parameters 	: 
	Returns 	: 
	Info 		: // PP: Called by FreeScene
*/
void BookList_finishCloses(void);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_draw
	Purpose 	: draw all the books in the book list
	Parameters 	: (opt/false) also draw debug stuff for the books
	Returns 	: 
	Info 		: 
*/
void BookList_draw(const bool debug=false);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_getExclusiveBook
	Purpose 	: get a ptr to the exclusive book
	Parameters 	: a ptr to the exclusive book
	Returns 	: 
	Info 		: 
*/
BOOK* BookList_getExclusiveBook(void);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_pushExclusiveBook
	Purpose 	: push an exclusive book onto the top of the exclusive book stack
	Parameters 	: new exclusive book
	Returns 	: 
	Info 		: 
*/
void BookList_pushExclusiveBook(BOOK* const book);


/*	--------------------------------------------------------------------------------
	Function 	: BookList_popExclusiveBook
	Purpose 	: pop an exclusive book off of the top of the exclusive book stack
	Parameters 	: ptr to the exclusive book
	Returns 	: 
	Info 		: 
*/
void BookList_popExclusiveBook(BOOK* const book);


#endif// PP: ndef __H_BOOKLIST