// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : encryption.cpp
//   Purpose : Encryption support
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

static unsigned long bSigYearEnc  = 0;
static unsigned long bSigMonthEnc = 0;
static unsigned long bSigDayEnc   = 0;
static unsigned long bSigXorCheck = 0;

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bCheckSignature
	Purpose : check the expiry date on the code signature
	Parameters : 
	Returns : 
	Info : if the check fails, this function never exits
*/
void bCheckSignature()
{
    // Disabled if no year value is present
    if (bSigYearEnc == 0)
        return;

    // Integrity check (same as decompile): year^month^day must match stored check.
    if (bSigXorCheck == (bSigYearEnc ^ bSigMonthEnc ^ bSigDayEnc))
    {
        _TBClock now;
        bkReadClock(&now);

        // Decode expiry date. Year is stored as (2000 + Y) XOR encoder.
        const int expiryYear  = (int)((bSigYearEnc  ^ YEAR_ENCODER)  - 2000u);
        const int expiryMonth = (int) (bSigMonthEnc ^ MONTH_ENCODER);
        const int expiryDay   = (int) (bSigDayEnc   ^ DAY_ENCODER);

        // If now < expiry -> OK (return). Otherwise fall through to "block".
        if ((int)now.year < expiryYear) return;
        if ((int)now.year == expiryYear) {
            if ((int)now.month < expiryMonth) return;
            if ((int)now.month == expiryMonth && (int)now.day < expiryDay) return;
        }
    }

    // Block path: show two white frames
    bdFlip(0, 0xFF, 0xFF, 0xFF, 1);
    bdFlip(0, 0xFF, 0xFF, 0xFF, 1);

    // Wait until elapsed >= bTimerFrequency * 4  (i.e., ~4 seconds).
    const TBTimerValue t0 = bkTimerRead();
    const TBTimerValue fourSec = (TBTimerValue)((unsigned __int64)bTimerFrequency * 4u);
    while (bkTimerDelta(t0, bkTimerRead()) < fourSec) {
        // busy-wait to match original behavior
    }

    exit(1); // terminate process
}