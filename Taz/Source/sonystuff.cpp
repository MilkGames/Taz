// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000/2001 Interactive Studios Ltd.
//
//      File : sony stuff.cpp
//   Purpose : provides ANSI functions Sony couldn't be arsed to implement themselves
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#include "global.h"

#if BPLATFORM == PS2

// Internal enum only
// CPW: "Borrowed" from the BABEL source!
typedef enum {
	FONT_SPRINTF_FORMAT_INVALID = 0,
	FONT_SPRINTF_FORMAT_DOUBLE = 1,
	FONT_SPRINTF_FORMAT_INT,
	FONT_SPRINTF_FORMAT_UINT,
	FONT_SPRINTF_FORMAT_LONG,
	FONT_SPRINTF_FORMAT_ULONG,
	FONT_SPRINTF_FORMAT_STRING8,
	FONT_SPRINTF_FORMAT_STRING16,
	FONT_SPRINTF_FORMAT_CHAR8,
	FONT_SPRINTF_FORMAT_CHAR16,
} BFONT_SPRINTF_FORMAT;

// CPW: "Borrowed" from those nice people in Team Tech
// CPW: Modified by Chris, (To take a va_list, & return num chars output)
int vswprintf(ushort *target, const ushort *format, va_list argp)
{
	// CPW: va_list		  argp;
	
	int           i,j;
	unsigned int  ui,uj;
	long          l,m;
	unsigned long ul,um;
	double		  f,g;
	int			  width,precision;
	int			  pfound; // precision specified
	int           digits;
	int           c; // counter
	char          pad;

	int			  plen,wlen;
	int			  digitLength,fullLength;

	ushort        *pT = target;

	char          *p8;
	ushort        *p16;


	// CPW: va_start(argp, format);
	while (*format)
	{
		if ((char)(*format) == '%')
		{
			format++;
			// parse width - for padding text size (if first char 0 pad with 0's)
			width = 0;
			pad = (*format=='0') ? '0' : ' ';
			while (*format>='0' && *format<='9')
			{
				width*=10;
				width+=((*format++)-'0');
			}
			// parse precision : number of floating point digits
			pfound = 0;
			if (*format=='.')
			{
				format++;
				precision = 0;
				while (*format>='0' && *format<='9')
				{
					precision*=10;
					precision+=((*format++)-'0');
				}
				pfound = 1;
			}

			int formatAt = FONT_SPRINTF_FORMAT_INVALID;
			switch ((char)(*format++))
			{
				case 'f':
					formatAt = FONT_SPRINTF_FORMAT_DOUBLE;
					break;
				case 'd':
				case 'i':
					formatAt = FONT_SPRINTF_FORMAT_INT;
					break;
				case 'u':
					formatAt = FONT_SPRINTF_FORMAT_UINT;
					break;
				case 'c':
					formatAt = FONT_SPRINTF_FORMAT_CHAR8;
					break;
				case 'C':
					formatAt = FONT_SPRINTF_FORMAT_CHAR16;
					break;
				case 's':
					formatAt = FONT_SPRINTF_FORMAT_STRING8;
					break;
				case 'S':
					formatAt = FONT_SPRINTF_FORMAT_STRING16;
					break;
				case 'l':
					switch (*format++)
					{
						case 'f':
							formatAt = FONT_SPRINTF_FORMAT_DOUBLE;
							break;
						case 'd':
						case 'i':
							formatAt = FONT_SPRINTF_FORMAT_LONG;
							break;
						case 'u':
							formatAt = FONT_SPRINTF_FORMAT_ULONG;
							break;
						case 'c':
							formatAt = FONT_SPRINTF_FORMAT_CHAR16;	 // l swaps style
							break;
						case 'C':
							formatAt = FONT_SPRINTF_FORMAT_CHAR16;
							break;
						case 's':
							formatAt = FONT_SPRINTF_FORMAT_STRING16; // l swaps style
							break;
						case 'S':
							formatAt = FONT_SPRINTF_FORMAT_STRING16;
							break;
					}
					break;
			}

			switch (formatAt)
			{
				case FONT_SPRINTF_FORMAT_CHAR8:
					i = va_arg( argp, int);
					*target++ = (ushort)i;
					break;
				case FONT_SPRINTF_FORMAT_CHAR16:
					i = va_arg( argp, int);
					*target++ = i;
					break;
				case FONT_SPRINTF_FORMAT_STRING8:
					p8 = va_arg( argp, char *);
					if (pfound)
					{
						while (*p8 && (precision--))
							*target++ = *p8++;
					}
					else
					{
						while (*p8)
							*target++ = *p8++;
					}
					break;
				case FONT_SPRINTF_FORMAT_STRING16:
					p16 = va_arg(argp, ushort *);
					if (pfound)
					{
						while (*p16 && (precision--))
							*target++ = *p16++;
					}
					else
					{
						while (*p16)
							*target++ = *p16++;
					}
					break;
				case FONT_SPRINTF_FORMAT_DOUBLE:
					if (!pfound) precision = 6; //default setting
					f = va_arg( argp, double);
					// Add minus if required
					if (f<0.0)
					{
						*target++='-';
						f=-f;
					}
					// Count digits (before decimal point)
					digits = 0;
					g = f;
					while (g>=1.0)
					{
						g/=10.0;
						digits++;
					}
					// Select number of floating point values
					digits+=(precision+1);	// +1 for '.'
					target+=(digits-1);
					i=0;
					f*=pow(10,precision);	// multiply to bring precision into quotiant
					// Fill out number from right to left
					while (f>=1.0f)
					{
						if (i==precision)
							*target--='.';
						else
						{
							*target-- = (ushort)('0'+(bmFMod(f,10.0)));
							f/=10.0;
						}
						i++;
					}
					target+=(digits+1);
					break;
				case FONT_SPRINTF_FORMAT_INT:
					if (!pfound) precision = 1; //default setting
					i = va_arg( argp, int);
					// Add minus if required
					if (i<0)
					{
						*target++='-';
						i=-i;
					}
					// Count digits
					digits = 0;
					j = i;
					while (j>0)
					{
						j/=10;
						digits++;
					}
					// Pad as required
					digitLength = MAX(precision,digits);
					fullLength  = MAX(digitLength,width);
					plen = digitLength - digits;
					wlen = fullLength  - (digits+plen);

					for (c=0;c<wlen;c++)
						*target++=(ushort)pad;
					for (c=0;c<plen;c++)
						*target++=(ushort)'0';
					// Fill out number from right to left
					target+=(digits-1);
					while (i>0)
					{
						*target-- = (ushort)('0'+(i%10));
						i/=10;
					}
					target+=(digits+1);
					break;
				case FONT_SPRINTF_FORMAT_UINT:
					if (!pfound) precision = 1; //default setting
					ui = va_arg( argp, unsigned int);
					// Count digits
					digits = 0;
					uj = ui;
					while (uj>0)
					{
						uj/=10;
						digits++;
					}
					// Pad as required
					digitLength = MAX(precision,digits);
					fullLength  = MAX(digitLength,width);
					plen = digitLength - digits;
					wlen = fullLength  - (digits+plen);

					for (c=0;c<wlen;c++)
						*target++=(ushort)pad;
					for (c=0;c<plen;c++)
						*target++=(ushort)'0';
					// Fill out number from right to left
					target+=(digits-1);
					while (ui>0)
					{
						*target-- = (ushort)('0'+(ui%10));
						ui/=10;
					}
					target+=(digits+1);
					break;
				case FONT_SPRINTF_FORMAT_LONG:
					if (!pfound) precision = 1; //default setting
					l = va_arg( argp, long);
					// Add minus if required
					if (l<0)
					{
						*target++='-';
						l=-l;
					}
					// Count digits
					digits = 0;
					m = l;
					while (m>0)
					{
						m/=10;
						digits++;
					}
					// Pad as required
					for (c=0;c<(width-digits);c++)
						*target++=(ushort)pad;
					// Fill out number from right to left
					target+=(digits-1);
					while (l>0)
					{
						*target-- = (ushort)('0'+(l%10));
						l/=10;
					}
					target+=(digits+1);
					break;
				case FONT_SPRINTF_FORMAT_ULONG:
					if (!pfound) precision = 1; //default setting
					ul = va_arg( argp, unsigned long);
					// Count digits
					digits = 0;
					um = ul;
					while (um>0)
					{
						um/=10;
						digits++;
					}
					// Pad as required
					for (c=0;c<(width-digits);c++)
						*target++=(ushort)pad;
					// Fill out number from right to left
					target+=(digits-1);
					while (ul>0)
					{
						*target-- = (ushort)('0'+(ul%10));
						ul/=10;
					}
					target+=(digits+1);
					break;
				case FONT_SPRINTF_FORMAT_INVALID:
				default:
					// Yes the %%%% is required - see bAssert for details
					BASSERT(BASSERT_FONT_SPRINTF_FORMATING_INVALID,
						BASSERT_ALWAYS,
						"bkStringSprintf16: Unsupported formatting command (%%%%l%c)",
						*(format-1));
					*target = 0;
					return -1;
			}
		}
		else
		{
			(*target++) = (*format++);
		}
	}
	*target = 0;

	// CPW: va_end(argp);

	return ((int)pT) - ((int)target);
}



#endif // CPW: BPLATFORM == PS2

