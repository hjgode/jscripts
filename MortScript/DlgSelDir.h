#if !defined(AFX_DLGSELDIR_H__29C6A65C_7679_40A6_B455_5DEBD6731511__INCLUDED_)
#define AFX_DLGSELDIR_H__29C6A65C_7679_40A6_B455_5DEBD6731511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelDir.h : header file
//

#include "dirtreectrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDir dialog

class CDlgSelDir : public CDialog
{
// Construction
public:
	CDlgSelDir(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelDir)
	enum { IDD = IDD_SELDIR };
	CDirTreeCtrl	m_Tree;
	CString	m_Info;
	//}}AFX_DATA
    CString	m_Title;
    CString m_Selected;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelDir)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDlgSelDir)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELDIR_H__29C6A65C_7679_40A6_B455_5DEBD6731511__INCLUDED_)
