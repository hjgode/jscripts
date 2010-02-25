#if !defined(AFX_DLGDOWNLOAD_H__4C4CE5B7_A988_4EFB_B06B_A334EBBBA958__INCLUDED_)
#define AFX_DLGDOWNLOAD_H__4C4CE5B7_A988_4EFB_B06B_A334EBBBA958__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDownload.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgDownload dialog

class CDlgDownload : public CDialog
{
// Construction
public:
	CDlgDownload(CWnd* pParent = NULL);   // standard constructor

    CString Source, Target, Status;
    DWORD   Progress, Total;
    int     Result;
    
// Dialog Data
	//{{AFX_DATA(CDlgDownload)
	enum { IDD = IDD_DOWNLOAD };
	CProgressCtrl	m_Progress;
	CString	m_Info;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDownload)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    HANDLE  ThreadHandle;

	// Generated message map functions
	//{{AFX_MSG(CDlgDownload)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnInfo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDOWNLOAD_H__4C4CE5B7_A988_4EFB_B06B_A334EBBBA958__INCLUDED_)
