#if !defined(AFX_DLGBIGMESSAGE_H__EC0D2CB2_93D9_4631_9A6C_70CD99DD09AE__INCLUDED_)
#define AFX_DLGBIGMESSAGE_H__EC0D2CB2_93D9_4631_9A6C_70CD99DD09AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBigMessage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBigMessage dialog

class CDlgBigMessage : public CDialog
{
// Construction
public:
	CDlgBigMessage(CWnd* pParent = NULL);   // standard constructor
	CString Title;

// Dialog Data
	//{{AFX_DATA(CDlgBigMessage)
	enum { IDD = IDD_BIGMESSAGE };
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBigMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDlgBigMessage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBIGMESSAGE_H__EC0D2CB2_93D9_4631_9A6C_70CD99DD09AE__INCLUDED_)
