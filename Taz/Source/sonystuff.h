#ifndef __H_SONYSTUFF
#define __H_SONYSTUFF

#if BPLATFORM == PS2
int vswprintf(ushort *target, const ushort *format, va_list argp);
#endif

#endif