// mpconfig.h : main header file for the MPCONFIG DLL
//

#if !defined(AFX_MPCONFIG_H__1AFC76DA_F750_4CD4_B212_BEB993B180BE__INCLUDED_)
#define AFX_MPCONFIG_H__1AFC76DA_F750_4CD4_B212_BEB993B180BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMpconfigApp
// See mpconfig.cpp for the implementation of this class
//

class CMpconfigApp : public CWinApp
{
public:
	CMpconfigApp();

    CString AppPath;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMpconfigApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMpconfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPCONFIG_H__1AFC76DA_F750_4CD4_B212_BEB993B180BE__INCLUDED_)
