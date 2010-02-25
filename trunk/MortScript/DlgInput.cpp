// DlgInput.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HFONT MsgFont;

/////////////////////////////////////////////////////////////////////////////
// CDlgInput dialog


CDlgInput::CDlgInput(HWND pParent /*=NULL*/)
	: CDialog(CDlgInput::IDD, CWnd::FromHandle(pParent) )
{
	//{{AFX_DATA_INIT(CDlgInput)
	m_Edit = _T("");
	//}}AFX_DATA_INIT
	m_Label = _T("");
    Title = L"";
	Multiline = 0;
	PressedKey = 0;
    //m_bFullScreen = FALSE;
}


void CDlgInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInput)
	DDX_Text(pDX, IDC_EDIT, m_Edit);
	//}}AFX_DATA_MAP
	//DDX_Text(pDX, IDC_LABEL, m_Label);
}


BEGIN_MESSAGE_MAP(CDlgInput, CDialog)
	//{{AFX_MSG_MAP(CDlgInput)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInput message handlers

BOOL CDlgInput::OnInitDialog() 
{
	CDialog::OnInitDialog();

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
	
	if ( Title.IsEmpty() ) SetWindowText( L"JScripts" );
                      else SetWindowText( Title );
	if ( Numeric ) ((CEdit*)GetDlgItem(IDC_EDIT))->ModifyStyle( 0, ES_NUMBER );
	if ( Multiline )
	{
		RECT pos;
		int height;

		GetDlgItem(IDC_EDIT)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		height = pos.bottom-pos.top;
		pos.bottom += height*4;
		GetDlgItem(IDC_EDIT)->MoveWindow( &pos );

#ifdef DESKTOP
		GetWindowRect( &pos );
		pos.bottom += height*4;
		MoveWindow( &pos );
#endif

		/*
		GetDlgItem(IDOK)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		pos.top += height*4;
		pos.bottom += height*4;
		GetDlgItem(IDOK)->MoveWindow( &pos );

		GetDlgItem(IDCANCEL)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		pos.top += height*4;
		pos.bottom += height*4;
		GetDlgItem(IDCANCEL)->MoveWindow( &pos );
		*/
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EDIT))->ModifyStyle( ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL, 0 );
		//((CEdit*)GetDlgItem(IDC_EDIT))->ModifyStyleEx( 0, ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL );
	}
#ifndef DESKTOP
    UpdateData(FALSE);
    SHSipPreference( m_hWnd, SIP_UP );
#endif
	if ( MsgFont != NULL )
		GetDlgItem(IDC_LABEL)->SetFont( CFont::FromHandle( MsgFont ) );

	((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(0,0,FALSE);

	CRect wndPos;
	GetClientRect( wndPos );
	::PostMessage( m_hWnd, WM_SIZE, SIZE_RESTORED, MAKEWPARAM( wndPos.Width(), wndPos.Height() ) ); // initialize control positions
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInput::OnOK() 
{
#ifndef DESKTOP
    SHSipPreference( m_hWnd, SIP_DOWN );
#endif
    UpdateData(TRUE);
	
	CDialog::OnOK();
}

void CDlgInput::OnCancel() 
{
#ifndef DESKTOP
    SHSipPreference( m_hWnd, SIP_DOWN );
#endif
    m_Edit.Empty();
	
	CDialog::OnCancel();
}

BOOL CDlgInput::PreTranslateMessage(MSG* pMsg) 
{
#ifndef DESKTOP
	if ( pMsg->message == WM_KEYDOWN )
		PressedKey = pMsg->wParam;

    if ( pMsg->message == WM_KEYUP && pMsg->wParam == PressedKey && pMsg->wParam == 13 )
    {
		if ( !Multiline || GetDlgItem( IDC_EDIT ) != CWnd::GetFocus() )
		{
			CDialog::PreTranslateMessage(pMsg);
			OnOK();
		}
        return 1;
    }

    SHInputDialog (m_hWnd, pMsg->message, pMsg->wParam);
#endif
    
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgInput::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if ( GetDlgItem(IDC_LABEL) == NULL || ! ::IsWindow( GetDlgItem(IDC_LABEL)->m_hWnd ) ) return;

	CRect pos;
    int   infoBottom;

	GetDlgItem( IDC_LABEL )->GetWindowRect( &pos );
	ScreenToClient( pos );
	if ( m_Label.IsEmpty() )
	{
		GetDlgItem( IDC_LABEL )->ShowWindow( SW_HIDE );
		infoBottom = 0;
	}
	else
	{
		HDC dc = ::GetWindowDC( GetDlgItem(IDC_LABEL)->m_hWnd );
		CStrArray lines;
		if ( m_Label.IsEmpty() )
		{
			lines.Add( L"" );
			GetDlgItem( IDC_LABEL )->SetWindowText(L"");
		}
		else
		{
			SplitLines( dc, (LPCTSTR)m_Label, lines, pos.Width() );
			CString infoWrapped;
			for ( int i=0; i<lines.GetSize(); i++ )
			{
				infoWrapped += lines[i];
				if ( i<lines.GetSize()-1 ) infoWrapped += "\n";
			}
			GetDlgItem( IDC_LABEL )->SetWindowText(infoWrapped);
			//UpdateData(FALSE);
		}
		SIZE size;
		GetTextExtentPoint( dc, L"W", 1, &size );
		pos.top   = 4;
		pos.left  = 4;
		pos.right = cx - 8;
		infoBottom = pos.bottom = pos.top + size.cy * lines.GetSize();
		::ReleaseDC( GetDlgItem(IDC_LABEL)->m_hWnd, dc );
		GetDlgItem( IDC_LABEL )->MoveWindow( pos );
		GetDlgItem( IDC_LABEL )->Invalidate();
	}


	//GetDlgItem(IDC_LABEL)->GetWindowRect( &pos );
	//ScreenToClient( &pos );
	//GetDlgItem(IDC_LABEL)->MoveWindow( &pos );

	if ( Multiline )
	{
		int height;

		GetDlgItem(IDOK)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		height = pos.bottom-pos.top;
		pos.top = cy - height - 4;
		pos.bottom = cy - 4;
		pos.left  = 4;
		pos.right = cx/2 - 4;
		GetDlgItem(IDOK)->MoveWindow( &pos );

		GetDlgItem(IDCANCEL)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		pos.top = cy - height - 4;
		pos.bottom = cy - 4;
		pos.left = cx/2 + 4;
		pos.right = cx - 8;
		GetDlgItem(IDCANCEL)->MoveWindow( &pos );

		GetDlgItem(IDC_EDIT)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		pos.top   = infoBottom + 4;
		pos.bottom = cy - height - 8;
		pos.left  = 4;
		pos.right = cx - 8;
		GetDlgItem(IDC_EDIT)->MoveWindow( &pos );
	}
	else
	{
		int height;

		GetDlgItem(IDC_EDIT)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		height = pos.Height();
		pos.top   = infoBottom + 4;
		pos.bottom = pos.top + height;
		pos.left  = 4;
		pos.right = cx - 8;
		GetDlgItem(IDC_EDIT)->MoveWindow( &pos );
		infoBottom = pos.bottom;

		((CEdit*)GetDlgItem(IDC_EDIT))->ModifyStyle( ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL, 0 );
#ifndef DESKTOP
		OSVERSIONINFO ver;
		GetVersionEx( &ver );
		if ( ver.dwMajorVersion < 5 )
		{
#endif
		GetDlgItem(IDOK)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		height = pos.bottom-pos.top;
		pos.top   = infoBottom + 4;
		pos.bottom = pos.top + height;
		pos.left  = 4;
		pos.right = cx/2 - 4;
		GetDlgItem(IDOK)->MoveWindow( &pos );

		GetDlgItem(IDCANCEL)->GetWindowRect( &pos );
		ScreenToClient( &pos );
		pos.top   = infoBottom + 4;
		pos.bottom = pos.top + height;
		pos.left = cx/2 + 4;
		pos.right = cx - 8;
		GetDlgItem(IDCANCEL)->MoveWindow( &pos );
#ifndef DESKTOP
		}
		else
		{
			GetDlgItem( IDOK )->ShowWindow( SW_HIDE );
			GetDlgItem( IDCANCEL )->ShowWindow( SW_HIDE );
		}
#endif


		//((CEdit*)GetDlgItem(IDC_EDIT))->ModifyStyleEx( 0, ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL );

#ifdef DESKTOP
		CRect client;
		GetWindowRect( &pos );
		GetClientRect( &client );
		if ( infoBottom + height + 8 != client.Height() )
		{
			pos.bottom = pos.top + pos.Height() - client.Height() + infoBottom + height + 8;
			MoveWindow( &pos );
		}
#endif
	}
	
	((CEdit*)GetDlgItem(IDC_EDIT))->SetSel(0,0,FALSE);	
}
