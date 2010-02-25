#if !defined(AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)
#define AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog

class CDlgWait
{
// Construction
public:
	CDlgWait(HWND parent);   // standard constructor

// Dialog Data
    static CDlgWait* CurrentObject;
	enum { IDD = IDD_WAIT };
	HWND	    m_Parent;
	CStr	m_Countdown;
	CStr	m_Label;
    int Countdown;
    CStr Title;
    BOOL AllowOK;
	CStr Expression;
	int wWidth;
	int wHeight;
	CValue Result;

// Implementation
public:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)
