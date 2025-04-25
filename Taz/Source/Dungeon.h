// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz: Wanted, © 2001 Blitz Games Ltd.
//
//      File : Dungeon.h
//   Purpose : Pieces of code so ugly that they have to be kept out of public view
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __H_DUNGEON
#define __H_DUNGEON


// PP: ____________________________
// PP: "The World's Ugliest Macro"™		If you know a nice way of doing this then please let me know
// PP: ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// PP: BITS_REQUIRED evaluates to the number of bits required to hold any positive number lower than the one you specify

#define BITS_REQUIRED(_num)				((_num)>(1<<1)? \
										((_num)>(1<<2)? \
										((_num)>(1<<3)? \
										((_num)>(1<<4)? \
										((_num)>(1<<5)? \
										((_num)>(1<<6)? \
										((_num)>(1<<7)? \
										((_num)>(1<<8)? \
										((_num)>(1<<9)? \
										((_num)>(1<<10)? \
										((_num)>(1<<11)? \
										((_num)>(1<<12)? \
										((_num)>(1<<13)? \
										((_num)>(1<<14)? \
										((_num)>(1<<15)? \
										((_num)>(1<<16)? \
										((_num)>(1<<17)? \
										((_num)>(1<<18)? \
										((_num)>(1<<19)? \
										((_num)>(1<<20)? \
										((_num)>(1<<21)? \
										((_num)>(1<<22)? \
										((_num)>(1<<23)? \
										((_num)>(1<<24)? \
										((_num)>(1<<25)? \
										((_num)>(1<<26)? \
										((_num)>(1<<27)? \
										((_num)>(1<<28)? \
										((_num)>(1<<29)? \
										((_num)>(1<<30)? \
										((_num)>(1<<31)?32:31) \
										:30) \
										:29) \
										:28) \
										:27) \
										:26) \
										:25) \
										:24) \
										:23) \
										:22) \
										:21) \
										:20) \
										:19) \
										:18) \
										:17) \
										:16) \
										:15) \
										:14) \
										:13) \
										:12) \
										:11) \
										:10) \
										:9) \
										:8) \
										:7) \
										:6) \
										:5) \
										:4) \
										:3) \
										:2) \
										:1)


// PP: ENUMOPS decalares overloads, for the specified enum type, of all the operators you're likely to want - just saves a load of casting.
// PP: I put this here because it's too big to put in, say, 'main.h' without causing a real clutter - that's what 'Dungeon.h' is for really.
#define ENUMOPS(_enumType) \
;\
/* PP: ~~~~ ADDITION ~~~~ */ \
\
/* PP: _enumType plus int, with assignment */ \
inline _enumType operator += (_enumType& enumVal, const int intVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)+intVal)); \
} \
\
/* PP: _enumType plus _enumType, with assignment */ \
inline _enumType operator += (_enumType& enumVal, const _enumType enumVal2) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)+(int)enumVal2)); \
} \
\
/* PP: _enumType pre-increment */ \
inline _enumType operator ++ (_enumType& enumVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)+1)); \
} \
\
/* PP: _enumType post-increment */ \
inline _enumType operator ++ (_enumType& enumVal, int) \
{ \
	_enumType	rtnEnumVal=enumVal; \
\
	enumVal=(_enumType)(((int)enumVal)+1); \
\
	return rtnEnumVal; \
} \
\
/* PP: ~~~~ SUBTRACTION ~~~~ */ \
\
/* PP: _enumType minus int, with assignment */ \
inline _enumType operator -= (_enumType& enumVal, const int intVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)-intVal)); \
} \
\
/* PP: _enumType minus _enumType, with assignment */ \
inline _enumType operator -= (_enumType& enumVal, const _enumType enumVal2) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)-(int)enumVal2)); \
} \
\
/* PP: _enumType pre-decrement */ \
inline _enumType operator -- (_enumType& enumVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal))); \
} \
\
/* PP: _enumType post-decrement */ \
inline _enumType operator -- (_enumType& enumVal, int) \
{ \
	_enumType	rtnEnumVal=enumVal; \
\
	enumVal=(_enumType)(((int)enumVal)); \
\
	return rtnEnumVal; \
} \
\
/* PP: ~~~~ MULTIPLICATION ~~~~ */ \
 \
/* PP: _enumType multiply by int, with assignment */ \
inline _enumType operator *= (_enumType& enumVal, const int intVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)*intVal)); \
} \
\
/* PP: _enumType multiply by _enumType, with assignment */ \
inline _enumType operator *= (_enumType& enumVal, const _enumType enumVal2) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)*(int)enumVal2)); \
} \
\
/* PP: ~~~~ DIVISION ~~~~ */ \
\
/* PP: _enumType divide by int, with assignment */ \
inline _enumType operator /= (_enumType& enumVal, const int intVal) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)/intVal)); \
} \
\
/* PP: _enumType divide by _enumType, with assignment */ \
inline _enumType operator /= (_enumType& enumVal, const _enumType enumVal2) \
{ \
	return (enumVal=(_enumType)(((int)enumVal)/(int)enumVal2)); \
}


#endif // PP: ndef __H_DUNGEON