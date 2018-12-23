/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2012     */
/*------------------------------------------------------------------------*/

#ifndef _STRFUNC
#define _STRFUNC

#include <stdint.h>

#define _USE_XFUNC_OUT	1	/* 1: Use output functions */
#define	_CR_CRLF		1	/* 1: Convert \n ==> \r\n in the output char */
#define	_USE_LONGLONG	0	/* 1: Enable long long integer in type "ll". */
#define	_LONGLONG_t		long long	/* Platform dependent long long integer type */

#define _USE_XFUNC_IN	0	/* 1: Use input function */
#define	_LINE_ECHO		0	/* 1: Echo back input chars in xgets function */


#if _USE_XFUNC_OUT
#define xdev_out(func) xfunc_out = (void(*)(uint8_t))(func)
extern void (*xfunc_out)(uint8_t);
void xputc (int8_t c);
void xputs (const char* str);
void xfputs (void (*func)(uint8_t), const char* str);
void xprintf (const char* fmt, ...);
void xsprintf (char* buff, const char* fmt, ...);
void xfprintf (void (*func)(uint8_t), const char*	fmt, ...);
void put_dump (const void* buff, unsigned long addr, int32_t len, int32_t width);
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif

#if _USE_XFUNC_IN
#define xdev_in(func) xfunc_in = (uint8_t(*)(void))(func)
extern uint8_t (*xfunc_in)(void);
int32_t xgets (char* buff, int32_t len);
int32_t xfgets (uint8_t (*func)(void), char* buff, int32_t len);
int32_t xatoi (char** str, long* res);
#endif

#endif
