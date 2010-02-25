// DlgSelFile.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgSelFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelFile dialog


CDlgSelFile::CDlgSelFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelFile::IDD, pParent)
{
	Save = TRUE;
	//{{AFX_DATA_INIT(CDlgSelFile)
	m_File = _T("");
	m_Info = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSelFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelFile)
	DDX_Control(pDX, IDC_FILES, m_Files);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Text(pDX, IDC_FILE, m_File);
	//}}AFX_DATA_MAP
	//DDX_Text(pDX, IDC_INFO, m_Info);
}


BEGIN_MESSAGE_MAP(CDlgSelFile, CDialog)
	//{{AFX_MSG_MAP(CDlgSelFile)
	ON_WM_SIZE()
	ON_COMMAND(IDOK, OnOk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_LBN_SELCHANGE(IDC_FILES, OnSelchangeFiles)
	ON_LBN_DBLCLK(IDC_FILES, OnDblclkFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelFile message handlers

BOOL CDlgSelFile::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if ( m_Filter.IsEmpty() ) m_Filter = L"*.*";

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

    Dir = m_Selected.Left(m_Selected.ReverseFind('\\'));
	m_Tree.StartPath = Dir;
    m_Tree.Initialize();
    UpdateData(FALSE);

	if ( !Save ) GetDlgItem( IDC_FILE )->ShowWindow( SW_HIDE );

	ShowFiles();
	m_File = m_Selected.Mid(m_Selected.ReverseFind('\\')+1);
	UpdateData(FALSE);
	if ( ! m_File.IsEmpty() )
		m_Files.SelectString( -1,m_File );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelFile::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    int scrWidth   = GetSystemMetrics(SM_CXSCREEN);
    int scrHeight  = GetSystemMetrics(SM_CYSCREEN);

    int res;
    if ( scrWidth > 320 && scrHeight > 320 ) res = 2;
                                    else     res = 1;

	int height, infoBottom;
    CRect rect;
	//if ( cy < 188 ) cy = 188;
#ifndef DESKTOP
    OSVERSIONINFO ver;
    GetVersionEx( &ver );
    if ( ver.dwMajorVersion < 5 )
    {
#endif
	GetDlgItem( IDOK )->GetWindowRect( rect );
	ScreenToClient( rect );
    height = rect.Height();
	rect.right = cx/2 - 2;
    rect.bottom = cy - 4;
    rect.top = rect.bottom - height;
	GetDlgItem( IDOK )->MoveWindow( rect );

    rect.left = rect.right + 4;
    rect.right = cx-4;
    GetDlgItem( IDCANCEL )->MoveWindow( rect );

	cy = rect.top;
#ifndef DESKTOP
	}
	else
	{
		GetDlgItem( IDOK )->ShowWindow( SW_HIDE );
		GetDlgItem( IDCANCEL )->ShowWindow( SW_HIDE );
		rect.top = cy;
	}
#endif

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

	int top = infoBottom;

    if ( scrWidth > scrHeight )
    {
    	GetDlgItem( IDC_TREE )->GetWindowRect( rect );
        rect.left   = 4;
        rect.right  = cx / 2 - 2;
		rect.top    = top + 4;
        rect.bottom = cy - 4;
        GetDlgItem( IDC_TREE )->MoveWindow( rect );

    	GetDlgItem( IDC_FILE )->GetWindowRect( rect );
        ScreenToClient( rect );
		height  = rect.Height();
		rect.top    = top + 4;
		rect.bottom = top + 4+height;
        rect.left   = cx/2+2;
        rect.right  = cx-4;
        GetDlgItem( IDC_FILE )->MoveWindow( rect );

		if ( Save )
			rect.top = rect.bottom+4;
		else
			rect.top = top + 4;

        rect.left = cx/2+2;
        rect.right = cx-4;
		rect.bottom = cy - 4;
        GetDlgItem( IDC_FILES )->MoveWindow( rect );
    }
    else
    {
        // Portrait/Square: dirs on top, files on bottom
    	GetDlgItem( IDC_TREE )->GetWindowRect( rect );
        ScreenToClient( rect );
        rect.left   = 4;
        rect.right  = cx - 4;
        rect.top    = top + 4;
		rect.bottom = cy/2 - 4;
        GetDlgItem( IDC_TREE )->MoveWindow( rect );

    	GetDlgItem( IDC_FILE )->GetWindowRect( rect );
        ScreenToClient( rect );
		height  = rect.Height();
        rect.left   = 4;
        rect.right  = cx - 4;
        rect.top    = cy/2 + 4;
        rect.bottom = rect.top + height;
        GetDlgItem( IDC_FILE )->MoveWindow( rect );

		if ( Save )
			rect.top = rect.bottom+1;
		else
			rect.top = cy/2 + 4;
        rect.bottom = cy - 4;
        GetDlgItem( IDC_FILES )->MoveWindow( rect );
    }
}

void CDlgSelFile::OnOk() 
{
	UpdateData(TRUE);
	if ( m_Tree.GetSelectedItem() != NULL && ! m_File.IsEmpty() )
	{
		m_Selected = Dir;
		if ( m_Selected.Right(1) != L"\\" ) m_Selected += L"\\";
		m_Selected += m_File;
		CDialog::OnOK();
	}
	else
		MessageBox( L"No file selected", m_Title, MB_ICONWARNING|MB_SETFOREGROUND );
}

void CDlgSelFile::OnCancel() 
{
	m_Selected = L"";
	
	CDialog::OnCancel();
}

void CDlgSelFile::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    UpdateData(TRUE);
    Dir = m_Tree.GetItemPath();
    if ( Dir.IsEmpty() ) m_Files.ResetContent();
                    else ShowFiles();
    UpdateData(FALSE);
	
	*pResult = 0;
}

void CDlgSelFile::ShowFiles()
{
    int type = 1;
    CStringArray titles;

    UpdateData(TRUE);
    Dir = m_Tree.GetItemPath();
    m_Files.ResetContent();

	WIN32_FIND_DATA  findFileData;
	CString searchString = Dir;
	if ( searchString.Right(1) != L"\\" ) searchString += L"\\";
	searchString += m_Filter;

	HANDLE ffh = FindFirstFile( searchString, &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
		while ( TRUE )
		{
			if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			{
				m_Files.AddString( findFileData.cFileName );
			}
			if ( FindNextFile( ffh, &findFileData ) == 0 ) break;
		}
		FindClose( ffh );
	}
}


void CDlgSelFile::OnSelchangeFiles() 
{
    UpdateData(TRUE);
    int idx = m_Files.GetCurSel();
    m_Files.GetText( idx, m_File );
    UpdateData(FALSE);
}

void CDlgSelFile::OnDblclkFiles() 
{
	PostMessage( WM_COMMAND, IDOK, 0 );
	//OnSelchangeFiles();
	//OnOK();
}
