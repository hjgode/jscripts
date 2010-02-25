#include "windows.h"
#include <windowsx.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "..\helpers.h"
#include "resource.h"
#include "../interpreter.h"
#include "DlgWait.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance
extern HFONT MsgFont;

CDlgWait* CDlgWait::CurrentObject = NULL;

BOOL CALLBACK waitDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL );
    HWND count   = GetDlgItem( hwndDlg, IDC_COUNTDOWN );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
            SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->m_Label ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->Title ); 

			RECT rect;
#ifdef PNA
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
			MoveWindow( hwndDlg, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
            if ( CDlgWait::CurrentObject->AllowOK )
            {
				ShowWindow( count, SW_HIDE );
			}
			else
			{
				ShowWindow( ok, SW_HIDE );
			}
#else
			int   cw = LOWORD( lParam );	//对话框宽
			int   ch = HIWORD( lParam );	//对话柜高

			SHINITDLGINFO shidi;	//初始化对话框信息
			memset(&shidi, 0, sizeof(SHINITDLGINFO));
			shidi.dwMask = SHIDIM_FLAGS;
			if (cw==0 || ch==0) 
				shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN ;
			else
				shidi.dwFlags = SHIDIF_SIPDOWN;//SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);

			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );

			if (cw==0) cw = rect.right;		//如果未定义宽和高，则使用默认的宽和高
			if (ch==0) ch = rect.bottom;

			rect.left=(rect.right - cw) /2;
			rect.top =(rect.bottom -ch) /2;
			
			MoveWindow( hwndDlg, rect.left, rect.top, cw,ch, TRUE );	//刷新窗口

            if ( CDlgWait::CurrentObject->AllowOK )	//如果OK键允许，则注册菜单
            {
                SHMENUBARINFO info;
                info.cbSize = sizeof(info);
                info.hwndParent = hwndDlg;
				info.dwFlags = 0; 
                info.nToolBarId = (CDlgWait::CurrentObject->AllowOK==2) ? IDR_OKCANCEL : IDR_OK;
                info.hInstRes = g_hInst;
                info.nBmpId = 0;
                info.cBmpImages = 0;
                SHCreateMenuBar(&info); 
            }

			ShowWindow( ok, SW_HIDE );	//显示窗口
#endif

			if ( MsgFont != NULL )
				SendMessage( text, WM_SETFONT, (WPARAM)MsgFont, MAKELPARAM(1,0) );

			if (CDlgWait::CurrentObject->Countdown>0){
				CDlgWait::CurrentObject->m_Countdown.Format( L"%d", CDlgWait::CurrentObject->Countdown );
				SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->m_Countdown ); 

#ifdef PNA
				SendMessage (ok, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->m_Countdown ); 
#endif
				SetTimer( hwndDlg, 1, 1000, NULL );
			}
			SetForegroundWindow( hwndDlg );

            return TRUE;
        }

    case WM_COMMAND:
        if ( CDlgWait::CurrentObject->AllowOK )
        {
            EndDialog( hwndDlg, IDOK );
			if (wParam == IDOK){
				CDlgWait::CurrentObject->Result = 1L;
				return TRUE;
			}else
				return FALSE;
        }
	
	case WM_KEYDOWN:
		//MessageBox(NULL,L"Key down",L"Key",MB_SETFOREGROUND);
		//switch (wParam)
		//{
		//	case 
		return TRUE;

    case WM_TIMER:
    	if ( wParam == 1 )
	    {
			BOOL exit = FALSE;
		    CDlgWait::CurrentObject->Countdown--;
		    if ( CDlgWait::CurrentObject->Countdown == 0 )
		    {
				exit = TRUE;
		    }
		    else
		    {
				if ( ! CDlgWait::CurrentObject->Expression.IsEmpty() )
				{
					CInterpreter parser;
					if ( _wtol( parser.EvaluateExpression( (CStr)CDlgWait::CurrentObject->Expression )))
					{
						exit = TRUE;
					}
				}
		    }
			if ( exit )
			{
			    KillTimer( hwndDlg, 1 );
                EndDialog( hwndDlg, IDOK );
			}
			else
			{
			    CDlgWait::CurrentObject->m_Countdown.Format( L"%d", CDlgWait::CurrentObject->Countdown );
			    SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->m_Countdown ); 
#ifdef PNA
				SendMessage (ok, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgWait::CurrentObject->m_Countdown ); 
#endif
			}
			return TRUE;
	    }


    case WM_SIZE:
		{
			if ( wParam == SIZE_RESTORED )
			{
				int   cx = LOWORD( lParam );
				int   cy = HIWORD( lParam );
				// Minimum smartphone screen size - cause some devices deliver 0x0... ^^
				//if ( cx < 176 ) cx = 176;
				//if ( cy < 168 ) cy = 168;

				RECT  rect;
				int   width, height;//left;//jwz::
				int   res = (cx>320)?2:1;

				GetWindowRect( count, &rect );
				ScreenToClient( hwndDlg, &rect );
				width  = rect.right-rect.left;
				height = rect.bottom-rect.top;
				rect.left  = (cx-width)/2;
				rect.right = cx/2 + width/2;
				rect.bottom = cy-4*res;
				rect.top    = rect.bottom - height;
				MoveWindow( count, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left  = 2*res;
				rect.right = cx-2*res;
				rect.bottom = cy-height-8*res;
				MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				
			}
		}
		return TRUE;

    default:
        return FALSE;
    }
}


CDlgWait::CDlgWait(HWND parent)
{
	m_Parent = parent;
	m_Countdown = _T("");
	m_Label = _T("");
    Countdown = 10;
    Title = L"";
    AllowOK = FALSE;
	wWidth = 0;
	wHeight = 0;
	Result = 0L;
}

int CDlgWait::DoModal()
{
    CurrentObject = this;
    return DialogBoxParam( g_hInst, MAKEINTRESOURCE( IDD ), m_Parent, waitDialogProc,MAKELPARAM(CurrentObject->wWidth ,CurrentObject->wHeight));
}

// end SPDlgTest.cpp
