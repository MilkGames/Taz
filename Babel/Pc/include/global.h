// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2025 MilkGames
//
//      File : global.h
//   Purpose : define some globals for stuff we need
// Component : PC Babel
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define NORMALIZEFPU          // restores default rounding/precision (temp)

// there are a lot of problems or things in Babel that I wanna fix or add as I explore
// sometimes you may very much need exactly the original library code so
// by removing this nasty thing you can explore everything in Ghidra
// with about the same code as in assembler, good luck with that btw
#define AUTHENTIC

#ifdef AUTHENTIC
	#define PRINT_DISCLAIMER    // print a really long legal + MilkGames disclaimer
	#define PRINT_REALBUILDDATE // print real Babel build date
	#define FIX_PLATFORMINFO    // fix PlatformInfo in pcDebug.cpp
	//#define PRINT_CRCTABLE      // print Babel CRC table, could be useful sometimes
	#define PRINT_COMMANDLINE	// print command line arguments
	#define FORCE_WINDOWED		// force windowed mode
#endif

#endif		// __BABEL_H__