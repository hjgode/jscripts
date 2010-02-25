#if !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
#define AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChoice.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice dialog

class CDlgChoice : public CDialog
{
// Construction
public:
	CDlgChoice(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChoice)
	enum { IDD = IDD_CHOICE };
	CListBox	m_Entries;
	CString	m_Countdown;
	//}}AFX_DATA
    CStringArray m_Strings;
    CString	m_Title;
	CString	m_Info;
    int m_Selected;
	int m_Default, m_Timeout, countdown;
	int PressedKey;
	int SelType;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChoice)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDlgChoice)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkEntries();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeEntries();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
