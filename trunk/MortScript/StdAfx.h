// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E5E6C6D4_FD9B_4C45_8389_3715D797204E__INCLUDED_)
#define AFX_STDAFX_H__E5E6C6D4_FD9B_4C45_8389_3715D797204E__INCLUDED_

#define TH32CS_SNAPNOHEAPS 0x40000000

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#if defined(_AFXDLL)
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef DESKTOP
#define REG_ACCESS_READ  KEY_READ
#define REG_ACCESS_WRITE KEY_ALL_ACCESS
#else
#define REG_ACCESS_READ  0
#define REG_ACCESS_WRITE 0
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E5E6C6D4_FD9B_4C45_8389_3715D797204E__INCLUDED_)
