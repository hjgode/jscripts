#if !defined(AFX_DLGINPUT_H__29A72649_4567_43BF_A5EE_A4C0BA84E16E__INCLUDED_)
#define AFX_DLGINPUT_H__29A72649_4567_43BF_A5EE_A4C0BA84E16E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgInput dialog

class CDlgInput : public CDialog
{
// Construction
public:
	CDlgInput(HWND pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInput)
	enum { IDD = IDD_INPUT };
	CString	m_Edit;
	CString	m_Label;
	//}}AFX_DATA
    CString Title;
    BOOL    Numeric;
	BOOL    Multiline;
	int		PressedKey;
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInput)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInput)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUT_H__29A72649_4567_43BF_A5EE_A4C0BA84E16E__INCLUDED_)
