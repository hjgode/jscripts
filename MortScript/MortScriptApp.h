// MortScriptApp.h : main header file for the MORTSTARTER application
//

#if !defined(AFX_MORTSCRIPTAPP_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
#define AFX_MORTSCRIPTAPP_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef POCKETPC
#include "resource.h"		// main symbols
#endif

#ifdef DESKTOP
#include "VC6\resource.h"
#endif

#include "morttypes.h"
#include "Helpers.h"

class CMortScriptApp
 : public CWinApp
{
public:
	CMortScriptApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMortScriptApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	void	RegisterFileClass();
	void	RegisterFileType( LPCTSTR type, LPCTSTR typeClass );
	CStr GetRelativeToAppPath( LPCTSTR file );

	//{{AFX_MSG(CMortScriptApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MORTSCRIPTAPP_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
