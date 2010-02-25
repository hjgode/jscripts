#include "windows.h"
#include <windowsx.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "resource.h"
#include "../helpers.h"
#include "../interpreter.h"
#include "DlgBigMessage.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance
extern HFONT MsgFont;

CDlgBigMessage* CDlgBigMessage::CurrentObject = NULL;

BOOL CALLBACK bigMsgDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL_SCROLL );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
            SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgBigMessage::CurrentObject->m_Text ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgBigMessage::CurrentObject->Title ); 

#ifdef PNA
			RECT rect;
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
			MoveWindow( hwndDlg, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#else
            SHMENUBARINFO info;
            info.cbSize = sizeof(info);
            info.hwndParent = hwndDlg;
			info.dwFlags = 0; 
            info.nToolBarId = IDR_OK;
            info.hInstRes = g_hInst;
            info.nBmpId = 0;
            info.cBmpImages = 0;
            SHCreateMenuBar(&info); 

			SHINITDLGINFO shidi;
			memset(&shidi, 0, sizeof(SHINITDLGINFO));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);

			ShowWindow( ok, SW_HIDE );
#endif
			if ( MsgFont != NULL )
				SendMessage( text, WM_SETFONT, (WPARAM)MsgFont, MAKELPARAM(1,0) );

			SetForegroundWindow( hwndDlg );

            return TRUE;
        }

    case WM_COMMAND:
        if ( wParam == IDOK )
        {
            EndDialog( hwndDlg, IDOK );
            return TRUE;
        }

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
		{
			HWND hwndEdit = (HWND)lParam;
			HDC  hdcEdit  = (HDC)wParam; 

			SetTextColor( hdcEdit, 0x00000000 );
			SetBkMode(hdcEdit, TRANSPARENT);

			HBRUSH	hbr;
			hbr = GetSysColorBrush( COLOR_WINDOW ); 
			
			return (int)hbr;
		}

    case WM_SIZE:
		{
			if ( wParam == SIZE_RESTORED )
			{
				int   cx = LOWORD( lParam );
				int   cy = HIWORD( lParam );
				// Minimum smartphone screen size - cause some devices deliver 0x0... ^^
				if ( cx < 176 ) cx = 176;
				if ( cy < 168 ) cy = 168;

				RECT  rect;
				//int   left, width, height;
				int   res = (cx>320)?2:1;

#ifdef PNA
				int width,height;
				GetWindowRect( ok, &rect );
				ScreenToClient( hwndDlg, &rect );
				width  = rect.right-rect.left;
				height = rect.bottom-rect.top;
				rect.left  = (cx-width)/2;
				rect.right = cx/2 + width/2;
				rect.bottom = cy-4*res;
				rect.top    = rect.bottom - height;
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				cy -= height+2*res;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left  = 2*res;
				rect.right = cx-2*res;
				rect.bottom = cy-8*res;
				MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
			}
		}
		return TRUE;

    default:
        return FALSE;
    }
}


CDlgBigMessage::CDlgBigMessage()
{
	m_Text = _T("");
    Title = L"";

}

int CDlgBigMessage::DoModal()
{
    CurrentObject = this;
    return DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), NULL, bigMsgDialogProc );
}

// end SPDlgTest.cpp
