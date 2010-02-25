#if !defined(AFX_DLGInput_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
#define AFX_DLGInput_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_

// DlgInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInput dialog

#include "resource.h"

class CDlgInput
{
// Construction
public:
	CDlgInput(HWND parent);   // standard constructor

// Data
    static CDlgInput* CurrentObject;

	enum { IDD = IDD_INPUT };
	HWND	m_Parent;
	HWND	m_EditCtrl;
	CStr	m_Label;
	CStr	m_Edit;
    CStr	Title;
    BOOL    Numeric, Multiline;

// Implementation
public:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGInput_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
