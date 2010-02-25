// DlgWait.cpp : implementation file
//

#include "stdafx.h"
#include "MortScriptApp.h"
#include "DlgWait.h"
#include "Interpreter.h"
#include "morttypes.h"

extern CMapStrToValue  Variables;
extern HFONT MsgFont;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog


CDlgWait::CDlgWait(HWND pParent /*=NULL*/)
	: CDialog(CDlgWait::IDD, CWnd::FromHandle(pParent) )
{
	//{{AFX_DATA_INIT(CDlgWait)
	m_Countdown = _T("");
	m_Label = _T("");
	//}}AFX_DATA_INIT
    Countdown = 10;
    Title = L"";
    AllowOK = FALSE;
	Expression = L"";
	Result = 0L;
#ifndef DESKTOP
    m_bFullScreen = FALSE;
#endif
}


void CDlgWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWait)
	DDX_Text(pDX, IDC_COUNTDOWN, m_Countdown);
	DDX_Text(pDX, IDC_LABEL, m_Label);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWait, CDialog)
	//{{AFX_MSG_MAP(CDlgWait)
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWait message handlers

BOOL CDlgWait::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if ( Title.IsEmpty() ) SetWindowText( L"JScripts" );
                      else SetWindowText( Title );
    m_Countdown.Format( L"%d", Countdown );
    UpdateData(FALSE);

    if ( AllowOK )
    {
		GetDlgItem( IDOK )->ShowWindow( SW_SHOW );
		if (AllowOK==2)GetDlgItem( IDCANCEL )->ShowWindow( SW_SHOW );
		GetDlgItem( IDC_COUNTDOWN )->ShowWindow( SW_HIDE );
		if ( Countdown > 0 ){
			if (AllowOK == 2){
				GetDlgItem( IDCANCEL )->SetWindowText( m_Countdown );
				GetDlgItem( IDOK )->SetWindowText( L"OK" );
			}else
				GetDlgItem( IDOK )->SetWindowText( m_Countdown );
		}else{
			if (AllowOK == 2)GetDlgItem( IDCANCEL )->SetWindowText( L"Cancel" );
			GetDlgItem( IDOK )->SetWindowText( L"OK" );
		}

#ifndef DESKTOP
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
    }

	if ( MsgFont != NULL )
		GetDlgItem(IDC_LABEL)->SetFont( CFont::FromHandle( MsgFont ) );

	if ( Countdown > 0 )
	    SetTimer( 1, 1000, NULL );
    SetForegroundWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWait::OnTimer(UINT nIDEvent) 
{
    if ( nIDEvent == 1 )
    {
        Countdown--;
        if ( Countdown > 0 )
        {
			if ( ! Expression.IsEmpty() )
			{
				CInterpreter parser;
				if ( _wtol( parser.EvaluateExpression((CStr)Expression,true) ) )
					OnCancel();
			}

            m_Countdown.Format( L"%d", Countdown );
            UpdateData(FALSE);
            if ( AllowOK!=2 )
            {
				GetDlgItem( IDOK )->SetWindowText( m_Countdown );
            }else{
				GetDlgItem( IDCANCEL )->SetWindowText( m_Countdown );
            }
        }
        else
        {
            OnCancel();
        }
    }
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgWait::OnOK() 
{
    if ( AllowOK ){
		Result = 1L;
	    CDialog::OnOK();
	}
}

void CDlgWait::OnCancel() 
{
    if ( AllowOK==2 )
	    CDialog::OnCancel();
}
void CDlgWait::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}
