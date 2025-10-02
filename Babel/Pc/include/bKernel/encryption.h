// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : encryption.h
//   Purpose : encryption functions
// Component : Generic Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BKERNEL_ENCRYPTION_H__
#define __BKERNEL_ENCRYPTION_H__

// ********************************************************************************
// Constants and Macros

#define MARKER_LENGTH		64							// length of magic marker in DWORD's
#define STRING_LENGTH		256							// length of signature string
#define YEAR_ENCODER		0x937f256c					// encoder value to encode expiry year number
#define MONTH_ENCODER		0x372cde35					// encoder value to encode expiry month number
#define DAY_ENCODER			0xd3a749b9					// encoder value to encode expiry day number
#define STRING_ENCODER		0x372353de					// random seed for string encoding
#define SIG_VALID			0xa8						// last byte of magic marker if file has been signed
#define SIG_INVALID			0xa7						// last byte of magic marker if file has not been signed


// ********************************************************************************
// Types and Structures

// information for a code signature
typedef struct {
	uchar			magicMarker[MARKER_LENGTH];			// magic sequence so signing program can find the data
	char			identString[STRING_LENGTH];			// identification string (encoded)
	int32			yearToExpire;						// year to stop running on ^ YEAR_ENCODER
	int32			monthToExpire;						// month to stop running on [1,12] ^ MONTH_ENCODER
	int32			dayToExpire;						// day to stop running on [1,31] ^ DAY_ENCODER
	uint32			doubleCheck;						// yearToExpire ^ monthToExpire ^ dayToExpire
} TCodeSignature;


// ********************************************************************************
// Prototypes

/*	--------------------------------------------------------------------------------
	Function : bCheckSignature
	Purpose : check the expiry date on the code signature
	Parameters : 
	Returns : 
	Info : if the check fails, this function never exits
*/

void bCheckSignature();


#endif // __BKERNEL_ENCRYPTION_H__