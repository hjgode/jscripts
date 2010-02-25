#if !defined(AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)
#define AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWait.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog

class CDlgWait : public CDialog
{
// Construction
public:
	CDlgWait(HWND pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWait)
	enum { IDD = IDD_WAIT };
	CString	m_Countdown;
	CString	m_Label;
	//}}AFX_DATA
    int Countdown;
    CString Title;
    BOOL AllowOK;
	CString Expression;
	int wWidth;
	int wHeight;
	CValue Result;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWait)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDlgWait)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)
