#if !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
#define AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_

// DlgChoice.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice dialog

class CDlgChoice
{
// Construction
public:
	CDlgChoice();   // standard constructor

// Data
    static CDlgChoice* CurrentObject;

	enum { IDD = IDD_CHOICE };
	HWND	m_Entries;
	CStr	m_Info;
	CStr	m_Countdown;
    CStrArray m_Strings;
    CStr	m_Title;
    int m_Selected;
	int m_Default, m_Timeout, countdown;

// Implementation
public:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
