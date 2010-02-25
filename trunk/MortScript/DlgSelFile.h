#if !defined(AFX_DLGSELFILE_H__94200D9B_4D11_4FAD_8D1C_70B9713C20AA__INCLUDED_)
#define AFX_DLGSELFILE_H__94200D9B_4D11_4FAD_8D1C_70B9713C20AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelFile.h : header file
//

#include "DirTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSelFile dialog

class CDlgSelFile : public CDialog
{
// Construction
public:
	CDlgSelFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelFile)
	enum { IDD = IDD_SELFILE };
	CListBox	m_Files;
	CDirTreeCtrl	m_Tree;
	CString	m_File;
	CString	m_Info;
	//}}AFX_DATA

    CString	m_Title;
    CString m_Selected;
	CString m_Filter;
	CString Dir;
	BOOL	Save;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifndef DESKTOP
	CCeCommandBar m_wndCommandBar;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDlgSelFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeFiles();
	afx_msg void OnDblclkFiles();
	//}}AFX_MSG
	void ShowFiles();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELFILE_H__94200D9B_4D11_4FAD_8D1C_70B9713C20AA__INCLUDED_)
