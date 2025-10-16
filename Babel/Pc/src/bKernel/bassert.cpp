// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : bassert.cpp
//   Purpose : Defines assertions for the Babel Kernel
// Component : Babel Kernel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/*	--------------------------------------------------------------------------------
	Function : bkAssert
	Purpose : assert function only used in debug mode (_DEBUG flag set)
	Parameters : error number, code to assert is true, output text
	Returns : 
	Info : Use the BASSERT macro so this is #defined out in release mode
	Records and displays message if any assertion checks fail
*/
void bkAssert(int errorno, int test, char *format, ...)
{
        bkPrintf("*** WARNING *** bkAssert was called but it wasn't implemented! REPORT IMMEDIATELY! *** WARNING ***\n");
    return;
}