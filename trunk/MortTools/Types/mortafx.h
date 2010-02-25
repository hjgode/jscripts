#ifndef MORTAFX

/*
#define TCHAR   unsigned short
#define LPCTSTR const unsigned short*
#define LPTSTR  unsigned short*
#define BOOL    int
#define UINT    unsigned int
#define ULONG   unsigned long
#define TRUE    1
#define FALSE   0
#define NULL    0
*/
#ifndef POSITION
#define POSITION int
#endif

#ifndef TH32CS_SNAPNOHEAPS
#define TH32CS_SNAPNOHEAPS 0x40000000
#endif

#include "windows.h"
#include <windowsx.h>
#include "string.h"
#include "math.h"

#ifndef ASSERT
#define ASSERT(x)
#endif

#endif
