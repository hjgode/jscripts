#if !defined(AFX_DLGSELFILE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
#define AFX_DLGSELFILE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_

// DlgChoice.h : header file
//

#include "resource.h"
#include "morttypes.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice dialog

class CDlgSelFile
{
// Construction
public:
	CDlgSelFile();   // standard constructor

// Data
    static CDlgSelFile* CurrentObject;

	enum { IDD = IDD_SELFILE };
	HWND	m_Tree;
	HIMAGELIST m_ImageList;
	CStr	m_Info;
    CStr	m_Title;
    CStr	m_Selected;
	CStr m_Filter;
	CStr Dir;
	BOOL	Save;

// Implementation
public:
	HTREEITEM InsertItem( HTREEITEM hParent, LPCTSTR sText, int iImage=0, int children=-1, DWORD itemData=0 );
	BOOL	  PopulateTree( CStr path, HTREEITEM hParent, CStr selDir );
	CStr GetItemPath( HTREEITEM hItem );


	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
