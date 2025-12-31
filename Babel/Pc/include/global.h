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
// by adding this nasty thing you can explore everything in Ghidra
// with about the same code as in assembler, good luck with that btw
// #define AUTHENTIC

// I need some stuff in Babel and Taz, so by adding this I change something in those
#define MILKGAMES

// if AUTHENTIC is not defined or MILKGAMES was - enable Babel modifications
#if !defined(AUTHENTIC) || defined(MILKGAMES) // || defined(ADD_YOUR_NAME_HERE)
	#define ENABLE_MODIFICATIONS
#endif

// or you can enable modifications here
// #define ENABLE_MODIFICATIONS

#ifdef ENABLE_MODIFICATIONS
	#define FILTER_STUB_SPAM	// removes stub functions spam
	#define PRINT_DISCLAIMER    // print a really long legal + MilkGames disclaimer
	#define PRINT_REALBUILDDATE // print real Babel build date
	#define PRINT_CREDITS		// print credits
	#define FIX_PLATFORMINFO    // fix PlatformInfo in pcDebug.cpp
	#define SHORT_PLATFORMINFO  // hides computer name and user name in PlatformInfo
	//#define PRINT_CRCTABLE      // print Babel CRC table, could be useful sometimes
	#define PRINT_COMMANDLINE	// print command line arguments
	#define FORCE_WINDOWED		// force windowed mode
	#define SKIP_IDEVICES_IF_INACTIVE // skip reading input devices if the MainWindow is not active
#endif

#endif		// __BABEL_H__