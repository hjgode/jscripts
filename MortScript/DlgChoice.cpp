// DlgChoice.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgChoice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice dialog

extern int   ChoiceItemHeight;
extern HFONT ChoiceFont;

CDlgChoice::CDlgChoice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChoice::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChoice)
	m_Countdown = _T("");
	//}}AFX_DATA_INIT
	m_Info = _T("");
	m_Timeout = -1;
	m_Default = 0;
	PressedKey = 0;
}


void CDlgChoice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChoice)
	DDX_Control(pDX, IDC_ENTRIES, m_Entries);
	DDX_Text(pDX, IDC_COUNTDOWN, m_Countdown);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChoice, CDialog)
	//{{AFX_MSG_MAP(CDlgChoice)
	ON_LBN_DBLCLK(IDC_ENTRIES, OnDblclkEntries)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_ENTRIES, OnSelchangeEntries)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice message handlers

BOOL CDlgChoice::OnInitDialog() 
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

    for ( int i=0; i<m_Strings.GetSize(); i++ )
    {
        m_Entries.AddString( m_Strings.GetAt(i) );
    }
    m_Entries.SetCurSel(m_Default);

	if ( m_Timeout > 0 )
	{
		countdown = m_Timeout;
		m_Countdown.Format( L"%d", countdown );
		SetTimer( 1, 1000, NULL );
	}
	else
	{
		GetDlgItem( IDC_COUNTDOWN )->ShowWindow( SW_HIDE );
	}

	if ( ChoiceFont != NULL )
		m_Entries.SetFont( CFont::FromHandle( ChoiceFont ) );

	if ( ChoiceItemHeight != 0 )
		m_Entries.SetItemHeight( 0, ChoiceItemHeight );

	UpdateData( FALSE );

	CRect wndPos;
	GetClientRect( wndPos );
	::PostMessage( m_hWnd, WM_SIZE, SIZE_RESTORED, MAKEWPARAM( wndPos.Width(), wndPos.Height() ) ); // initialize control positions
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgChoice::OnOK() 
{
	m_Selected = m_Entries.GetCurSel() + 1;
	
	CDialog::OnOK();
}

void CDlgChoice::OnCancel() 
{
	m_Selected = 0;
	
	CDialog::OnCancel();
}

void CDlgChoice::OnDblclkEntries() 
{
	m_Selected = m_Entries.GetCurSel() + 1;
	
    if ( m_Selected > 0 )
	    CDialog::OnOK();
}

void CDlgChoice::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == 1 )
	{
		countdown--;
		if ( countdown == 0 )
		{
			OnOK();
			KillTimer( nIDEvent );
		}
		else
		{
			m_Countdown.Format( L"%d", countdown );
			UpdateData(FALSE);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgChoice::OnSelchangeEntries() 
{
	if ( m_Timeout > 0 )
	{
		KillTimer( 1 );
		countdown = m_Timeout;
		m_Countdown.Format( L"%d", countdown );
		UpdateData(FALSE);
		SetTimer( 1, 1000, NULL );
	}
}

void CDlgChoice::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if ( ! ::IsWindow( m_hWnd ) || GetDlgItem( IDC_INFO ) == NULL )
		return;

    CRect rect, rect2;
    int   width, height, infoBottom;
    //int   res = (cx>320)?2:1;

	GetDlgItem( IDC_INFO )->GetWindowRect( rect );
	ScreenToClient( rect );
	rect.left = 4;
	rect.right = cx-4;
	rect.top = 4;

	if ( m_Info.IsEmpty() && m_Timeout <= 0 )
	{
		GetDlgItem( IDC_INFO )->ShowWindow( SW_HIDE );
		GetDlgItem( IDC_COUNTDOWN )->ShowWindow( SW_HIDE );
		infoBottom = 0;
	}
	else
	{
		HDC dc = ::GetWindowDC( GetDlgItem(IDC_INFO)->m_hWnd );
		CStrArray lines;
		if ( m_Info.IsEmpty() )
		{
			lines.Add( L"" );
			GetDlgItem( IDC_INFO )->SetWindowText(L"");
		}
		else
		{
			SplitLines( dc, (LPCTSTR)m_Info, lines, rect.Width() );
			CStr infoWrapped;
			for ( int i=0; i<lines.GetSize(); i++ )
			{
				infoWrapped += lines[i];
				if ( i<lines.GetSize()-1 ) infoWrapped += L"\n";
			}
			GetDlgItem( IDC_INFO )->SetWindowText(infoWrapped);
			//UpdateData(FALSE);
		}
		SIZE size;
		GetTextExtentPoint( dc, L"W", 1, &size );
		infoBottom = rect.bottom = rect.top + size.cy * ( lines.GetSize() + ((m_Timeout>0)?1:0) );
		::ReleaseDC( GetDlgItem(IDC_INFO)->m_hWnd, dc );
		GetDlgItem( IDC_INFO )->MoveWindow( rect );
		GetDlgItem( IDC_INFO )->Invalidate();

		GetDlgItem( IDC_COUNTDOWN )->GetWindowRect( rect );
		ScreenToClient( rect );
		width = rect.Width();
		height = rect.Height();
		rect.right = cx-4;
		rect.left  = rect.right - width;
		rect.bottom = infoBottom;
		rect.top = infoBottom - height;
		GetDlgItem( IDC_COUNTDOWN )->MoveWindow( rect );
	}

    rect2.top = infoBottom + 4;
    rect2.left = 4;
    rect2.right = rect.right;

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
	GetDlgItem( IDC_ENTRIES )->MoveWindow( rect2 );
}

BOOL CDlgChoice::PreTranslateMessage(MSG* pMsg) 
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == 38 || pMsg->wParam == 40 )
		{
			SelType = 0;
			if ( m_Entries.GetCount() > 1 )
			{
				if ( m_Entries.GetCurSel() == 0 ) SelType = 1;
				if ( m_Entries.GetCurSel() == m_Entries.GetCount()-1 ) SelType = 2;
			}
		}
		PressedKey = pMsg->wParam;
	}

    if ( pMsg->message == WM_KEYUP && pMsg->wParam == (DWORD)PressedKey )
    {
		CDialog::PreTranslateMessage(pMsg);
		if ( pMsg->wParam == 38 && SelType == 1 )
		{
			m_Entries.SetCurSel( m_Entries.GetCount()-1 );
			OnSelchangeEntries();
		}
		if ( pMsg->wParam == 40 && SelType == 2 )
		{
			m_Entries.SetCurSel( 0 );
			OnSelchangeEntries();
		}
		if ( pMsg->wParam == 13 )
			OnOK();
		if ( pMsg->wParam == 8 )
	        OnCancel();
        return 1;
    }
	
	return CDialog::PreTranslateMessage(pMsg);
}
