#ifndef MORTAFX

#ifdef DESKTOP
typedef unsigned short	TCHAR;
typedef const unsigned short* LPCTSTR;
typedef unsigned short* LPTSTR;
typedef int				BOOL;
typedef unsigned int	UINT;
typedef unsigned long	ULONG;
#define TRUE    1
#define FALSE   0
#define NULL    0
typedef POSITION int
#else
#define TCHAR   unsigned short
#define LPCTSTR const unsigned short*
#define LPTSTR  unsigned short*
#define BOOL    int
#define UINT    unsigned int
#define ULONG   unsigned long
#define TRUE    1
#define FALSE   0
#define NULL    0
#define POSITION int
#endif
//#include "cstring.h"
//#include "cstringarray.h"
//#include "cuintarray.h"
//#include "cptrarray.h"
//#include "cmapstringtostring.h"
//#include "cmapstringtoptr.h"

#endif