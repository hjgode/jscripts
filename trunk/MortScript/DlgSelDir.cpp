// DlgSelDir.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgSelDir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDir dialog


CDlgSelDir::CDlgSelDir(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelDir::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelDir)
	m_Info = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSelDir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelDir)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
	//DDX_Text(pDX, IDC_INFO, m_Info);
}


BEGIN_MESSAGE_MAP(CDlgSelDir, CDialog)
	//{{AFX_MSG_MAP(CDlgSelDir)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelDir message handlers

BOOL CDlgSelDir::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    SetWindowText( m_Title );

#ifndef DESKTOP
	m_wndCommandBar.m_bShowSharedNewButton = FALSE;
    if(    !m_wndCommandBar.Create(this)
	    || !m_wndCommandBar.AddAdornments()
      )
    { 
        TRACE0("Failed to create CommandBar\n"); 
    }
    else
    {
        CString ok     = L"OK"; //GetTranslation( okLabel );
        CString cancel = L"Cancel"; //GetTranslation( IDS_CANCEL );

        TCHAR texts[MAX_PATH];
        wcscpy( texts, ok );
        texts[ok.GetLength()] = '\0';
        wcscpy( texts+ok.GetLength()+1, cancel );
        texts[ok.GetLength()+cancel.GetLength()+1] = '\0';
        texts[ok.GetLength()+cancel.GetLength()+2] = '\0';
        int idx = m_wndCommandBar.SendMessage(TB_ADDSTRING, 0, (LPARAM)texts );
        
        TBBUTTON tbbi[2];
        tbbi[0].idCommand = IDOK;
        tbbi[0].fsState = TBSTATE_ENABLED;
        tbbi[0].fsStyle = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
        tbbi[0].dwData  = 0;
        tbbi[0].iBitmap = -2;
        tbbi[0].iString = idx;

        tbbi[1].idCommand = IDCANCEL;
        tbbi[1].fsState = TBSTATE_ENABLED;
        tbbi[1].fsStyle = (BYTE)TBSTYLE_FLAT|TBSTYLE_AUTOSIZE;
        tbbi[1].dwData  = 0;
        tbbi[1].iBitmap = -2;
        tbbi[1].iString = idx+1;

        m_wndCommandBar.SendMessage(TB_ADDBUTTONS, 2, (LPARAM)&tbbi);
    }
#endif

	m_Tree.StartPath = m_Selected;
	m_Tree.Initialize();

	UpdateData( FALSE );

#ifdef DESKTOP
	// Enforce WM_SIZE
	RECT rect;
	GetWindowRect( &rect );
	rect.bottom++;
	rect.right++;
	MoveWindow( &rect );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelDir::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if ( ! ::IsWindow( m_hWnd ) || GetDlgItem( IDC_INFO ) == NULL )
		return;

    CRect rect, rect2;
    int   height, infoBottom;
    int   res = (cx>320)?2:1;

	GetDlgItem( IDC_INFO )->GetWindowRect( rect );
	ScreenToClient( rect );
	rect.left = 4;
	rect.right = cx-4;
	rect.top = 4;

	if ( m_Info.IsEmpty() )
	{
		GetDlgItem( IDC_INFO )->ShowWindow( SW_HIDE );
		infoBottom = 0;
	}
	else
	{
		HDC dc = ::GetWindowDC( GetDlgItem(IDC_INFO)->m_hWnd );
		CStrArray lines;
		SplitLines( dc, (LPCTSTR)m_Info, lines, rect.Width() );
		CStr infoWrapped;
		for ( int i=0; i<lines.GetSize(); i++ )
		{
			infoWrapped += lines[i];
			if ( i<lines.GetSize()-1 ) infoWrapped += L"\n";
		}
		GetDlgItem( IDC_INFO )->SetWindowText(infoWrapped);

		SIZE size;
		GetTextExtentPoint( dc, L"W", 1, &size );
		infoBottom = rect.bottom = rect.top + size.cy * lines.GetSize();
		::ReleaseDC( GetDlgItem(IDC_INFO)->m_hWnd, dc );
		GetDlgItem( IDC_INFO )->MoveWindow( rect );
		GetDlgItem( IDC_INFO )->Invalidate();
	}

    rect2.top = infoBottom + 4;
    rect2.left = 4;
    rect2.right = cx-4;

#ifndef DESKTOP
    OSVERSIONINFO ver;
    GetVersionEx( &ver );
    if ( ver.dwMajorVersion < 5 )
    {
#endif
	GetDlgItem( IDOK )->GetWindowRect( rect );
	ScreenToClient( rect );
    height = rect.Height();
	rect.left = 4;
	rect.right = cx/2 - 2;
    rect.bottom = cy - 4;
    rect.top = rect.bottom - height;
	GetDlgItem( IDOK )->MoveWindow( rect );

    rect.left = rect.right + 4;
    rect.right = cx-4;
    GetDlgItem( IDCANCEL )->MoveWindow( rect );
#ifndef DESKTOP
	}
	else
	{
		GetDlgItem( IDOK )->ShowWindow( SW_HIDE );
		GetDlgItem( IDCANCEL )->ShowWindow( SW_HIDE );
		rect.top = cy;
	}
#endif

    rect2.bottom = rect.top - 4;
	GetDlgItem( IDC_TREE )->MoveWindow( rect2 );
}

void CDlgSelDir::OnOK() 
{
	HTREEITEM sel = m_Tree.GetSelectedItem();
	m_Selected.Empty();
	if ( sel != NULL )
	{
		m_Selected = m_Tree.GetItemPath( sel );
	}
	CDialog::OnOK();
}

void CDlgSelDir::OnCancel() 
{
	m_Selected.Empty();
	CDialog::OnCancel();
}
