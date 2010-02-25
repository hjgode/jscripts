// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F035D839_B9E6_487B_A4D8_5C21F460A79B__INCLUDED_)
#define AFX_STDAFX_H__F035D839_B9E6_487B_A4D8_5C21F460A79B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _WIN32_WCE
#if (_WIN32_WCE <= 211)
#error This project can not be built for H/PC Pro 2.11 or earlier platforms.
#endif
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#ifdef _WIN32_WCE
#include "projects.h"
#include <aygshell.h>
#else
#include <shellapi.h>
#endif

// Local Header Files

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F035D839_B9E6_487B_A4D8_5C21F460A79B__INCLUDED_)
