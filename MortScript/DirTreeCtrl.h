#if !defined(AFX_DIRTREECTRL_H__9D52BE97_5B40_4B9C_B700_C435BC82898B__INCLUDED_)
#define AFX_DIRTREECTRL_H__9D52BE97_5B40_4B9C_B700_C435BC82898B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl window

class CDirTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDirTreeCtrl();
	virtual ~CDirTreeCtrl();

// Attributes
public:
	CImageList	m_ImageList;
    CString     StartPath;

// Operations
public:
    void        Initialize        ();
  	CString     GetItemPath       ( HTREEITEM hItem = NULL );

protected:
	BOOL	    PopulateTree      ( CString sPath, HTREEITEM hParent, CString newDir = "" );
    BOOL        EnumNetwork       ( HTREEITEM hParent );
	HTREEITEM   InsertItem        ( HTREEITEM          hParent
                                  , CString            sText
                                  , int iImage         = 0
                                  , int iImageSelected = -1
                                  , DWORD itemData     = 0
                                  );

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeCtrl)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeCtrl)
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREECTRL_H__9D52BE97_5B40_4B9C_B700_C435BC82898B__INCLUDED_)
