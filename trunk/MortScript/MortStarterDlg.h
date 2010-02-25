// MortStarterDlg.h : header file
//

#if !defined(AFX_MORTSTARTERDLG_H__38B2A944_63D1_443F_9AC3_C61E7A242F31__INCLUDED_)
#define AFX_MORTSTARTERDLG_H__38B2A944_63D1_443F_9AC3_C61E7A242F31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMortStarterDlg dialog

class CMortStarterDlg : public CDialog
{
// Construction
public:
	CMortStarterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMortStarterDlg)
	enum { IDD = IDD_MORTSTARTER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMortStarterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMortStarterDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MORTSTARTERDLG_H__38B2A944_63D1_443F_9AC3_C61E7A242F31__INCLUDED_)
