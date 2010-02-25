// DlgBigMessage.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgBigMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HFONT MsgFont;

/////////////////////////////////////////////////////////////////////////////
// CDlgBigMessage dialog


CDlgBigMessage::CDlgBigMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBigMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBigMessage)
	m_Text = _T("");
	//}}AFX_DATA_INIT
}


void CDlgBigMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBigMessage)
	//DDX_Text(pDX, IDC_LABEL_SCROLL, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBigMessage, CDialog)
	//{{AFX_MSG_MAP(CDlgBigMessage)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBigMessage message handlers

BOOL CDlgBigMessage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText( Title );
	((CEdit*)GetDlgItem(IDC_LABEL_SCROLL))->SetWindowText( m_Text );
	((CEdit*)GetDlgItem(IDC_LABEL_SCROLL))->SetSel(0,0,FALSE);

#ifndef DESKTOP
	m_wndCommandBar.m_bShowSharedNewButton = FALSE;
    m_wndCommandBar.Create(this);

    CString ok     = L"OK"; //GetTranslation( okLabel );
    TCHAR texts[MAX_PATH];
    wcscpy( texts, ok );
    texts[ok.GetLength()] = '\0';
    texts[ok.GetLength()+1] = '\0';
    int idx = m_wndCommandBar.SendMessage(TB_ADDSTRING, 0, (LPARAM)texts );

    TBBUTTON tbbi[1];
    tbbi[0].idCommand = IDOK;
    tbbi[0].fsState = TBSTATE_ENABLED;
    tbbi[0].fsStyle = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
    tbbi[0].dwData  = 0;
    tbbi[0].iBitmap = -2;
    tbbi[0].iString = idx;

    m_wndCommandBar.SendMessage(TB_ADDBUTTONS, 1, (LPARAM)&tbbi);
#endif

	if ( MsgFont != NULL )
		GetDlgItem(IDC_LABEL_SCROLL)->SetFont( CFont::FromHandle( MsgFont ) );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBigMessage::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
    int   res = (cx>320)?2:1;

	RECT rect;
	if ( GetDlgItem( IDOK ) != NULL && IsWindow(GetDlgItem( IDOK )->m_hWnd) )
	{
		GetDlgItem( IDOK )->GetWindowRect( &rect );
		ScreenToClient( &rect );
		int okheight = rect.bottom-rect.top;
		int okwidth = rect.right-rect.left;
		rect.bottom = cy-res*2;
		rect.top    = cy-res*2-okheight;
		rect.left   = (cx-okwidth)/2;
		rect.right  = cx-rect.left;
		GetDlgItem( IDOK )->MoveWindow( &rect );

#ifndef DESKTOP
		GetDlgItem( IDC_LABEL_SCROLL )->SetWindowPos( &wndTop, res*2, res*2, cx-res*4, cy-res*4, SWP_SHOWWINDOW );
#else
		GetDlgItem( IDC_LABEL_SCROLL )->MoveWindow( res*2, res*2, cx-res*4, cy-res*8-okheight );
#endif
		((CEdit*)GetDlgItem(IDC_LABEL_SCROLL))->SetSel(0,0,FALSE);
	}
}

HBRUSH CDlgBigMessage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if( pWnd->GetDlgCtrlID() == IDC_LABEL_SCROLL )
	{
        pDC->SetTextColor( 0x000000 );
        pDC->SetBkMode(TRANSPARENT);
#ifndef DESKTOP
        hbr=GetSysColorBrush(COLOR_STATIC); 
#else
        hbr=GetSysColorBrush(COLOR_BTNFACE);
#endif
    }
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
