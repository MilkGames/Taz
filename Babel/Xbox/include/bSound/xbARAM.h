// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbARAM.h
//   Purpose : Xbox specific ARAM managment routines
// Component : Xbox Sound
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_XBARAM_H__
#define __BSOUND_XBARAM_H__

// ********************************************************************************
// Constants and Macros

#define ARAM_MALLOC(SIZE)					0
#define ARAM_FREE(PTR)						

// ********************************************************************************
// Types, Structures and Classes

// ARAM block header - 32 bytes
typedef struct _TARAMBlock {
} TARAMBlock;

// ********************************************************************************
// Globals


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bkARAMHeapFreeSpace
	Purpose : Reports the space that is left to fill in the ARAM heap.
	Parameters : ptr to fill with largest free block or NULL
	Returns : bytes free
	Info : 
*/

inline int bkARAMHeapFreeSpace(int *largestFreeBlock = NULL) {return 0;}


/* --------------------------------------------------------------------------------
   Function : bkARAMHeapShow
   Purpose : send a list of ARAM heap blocks to debug output
   Parameters : millisecond pause between prints
   Returns : 
   Info : 
*/

inline void bkARAMHeapShow(int pause = 0) {};


/*	--------------------------------------------------------------------------------
	Function : bkARAMUpload
	Purpose : Upload from normal memory to ARAM memory
	Parameters : pointer to main memory buffer, pointer to ARAM buffer, completion event
			  offset into ARAM block to upload TO, size to upload (set to zero for all)
			  related package index (to update counter and be free on completion)
			  priority or ARAM transfer
	Returns : OK/FAIL
	Info : size taken from ARAM block information
*/

inline int bkARAMUpload(void *buffer, TARAMBlock *block, char *event, int offset=0, int size=0,
						 TBPackageIndex *index=0, int priority=0) {return FAIL;}


/*	--------------------------------------------------------------------------------
	Function : bkARAMDownload
	Purpose : Download from ARAM memory to normal memory
	Parameters : pointer to main memory buffer, pointer to ARAM buffer, completion event
				 offset into ARAM block to download FROM, size to download (set to zero for all)
	Returns : OK/FAIL
	Info : offset must be multiple of 32, size must be multiple of 32
*/

inline int bkARAMDownload(void *buffer, TARAMBlock *block, char *event, int offset=0, int size=0)  {return FAIL;}


/*	--------------------------------------------------------------------------------
	Function : bkARAMSwap
	Purpose : Swap normal memory and ARAM memory
	Parameters : pointer to main memory buffer, pointer to ARAM buffer, completion event
	Returns : OK/FAIL
	Info : size taken from ARAM block information
*/

inline int bkARAMSwap(void *buffer, TARAMBlock *block, char *event) {return FAIL;}

#endif		// __BSOUND_XBARAM_H__
