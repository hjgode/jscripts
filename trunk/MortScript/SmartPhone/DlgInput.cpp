#include "windows.h"
#include <windowsx.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "../helpers.h"
#include "resource.h"
#include "DlgInput.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance
extern HFONT MsgFont;

CDlgInput* CDlgInput::CurrentObject = NULL;

BOOL CALLBACK InputDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND m_EditCtrl = GetDlgItem( hwndDlg, IDC_EDIT );
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );
	HWND cancel  = GetDlgItem( hwndDlg, IDCANCEL );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
			if ( ! CDlgInput::CurrentObject->Multiline )
			{
				SetWindowLong( m_EditCtrl, GWL_STYLE, GetWindowLong( m_EditCtrl, GWL_STYLE ) & !(ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL) );
				//SetWindowLong( m_EditCtrl, GWL_STYLE, GetWindowLong( m_EditCtrl, GWL_STYLE ) | ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL );
				//SendMessage( m_EditCtrl, EM_SETEXTENDEDSTYLE,0, (LPARAM)ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL ); 
			}

#ifdef PNA
			RECT rect;
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
			MoveWindow( hwndDlg, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#else
            SHMENUBARINFO info;
            info.cbSize = sizeof(info);
            info.hwndParent = hwndDlg;
			info.dwFlags = 0; 
            info.nToolBarId = IDR_OKCANCEL;
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
			ShowWindow( cancel, SW_HIDE );
#endif

			//MoveWindow( hwndDlg, 0, 0, GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ), TRUE );
			if ( MsgFont != NULL )
				SendMessage( text, WM_SETFONT, (WPARAM)MsgFont, MAKELPARAM(1,0) );

            SendMessage (m_EditCtrl, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgInput::CurrentObject->m_Edit ); 
            //SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgInput::CurrentObject->m_Label ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgInput::CurrentObject->Title ); 
			if ( CDlgInput::CurrentObject->Numeric )
				SendMessage( m_EditCtrl, EM_SETINPUTMODE ,0, (LPARAM)EIM_NUMBERS ); 

			SetForegroundWindow( hwndDlg );
            SetFocus( m_EditCtrl );
#ifndef PNA
		    SHSipPreference( hwndDlg, SIP_UP );
#endif

            return TRUE;
        }

    case WM_COMMAND:
        if ( wParam == IDOK )
        {
			LPTSTR buffer = CDlgInput::CurrentObject->m_Edit.GetBuffer( 512 );
            SendMessage( m_EditCtrl, WM_GETTEXT, 512, (LPARAM)(LPCTSTR)buffer ); 
			CDlgInput::CurrentObject->m_Edit.ReleaseBuffer();
            EndDialog( hwndDlg, IDOK );
            return TRUE;
        }
        if ( wParam == IDCANCEL )
        {
			CDlgInput::CurrentObject->m_Edit.Empty();
            EndDialog( hwndDlg, IDCANCEL );
            return TRUE;
        }

	case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;
			if ( nmhdr->code == NM_RETURN )
				PostMessage( hwndDlg, WM_COMMAND, IDOK, NULL );
		}
        return TRUE;

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
				int   height, infoBottom;//,left, width;
				int   res = (cx>320)?2:1;

#ifdef PNA
				cy = cy - 26 * res;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left = 4;
				rect.right = cx-4;

				HDC dc = ::GetWindowDC( text );
				CStrArray lines;
				if ( CDlgInput::CurrentObject->m_Label.IsEmpty() )
				{
					lines.Add( L"" );
					SetWindowText( text, L"" );
				}
				else
				{
					SplitLines( dc, (LPCTSTR)CDlgInput::CurrentObject->m_Label, lines, rect.right-rect.left );
					CStr infoWrapped;
					for ( int i=0; i<lines.GetSize(); i++ )
					{
						infoWrapped += lines[i];
						if ( i<lines.GetSize()-1 ) infoWrapped += L"\n";
					}
					SetWindowText( text, infoWrapped );
					//UpdateData(FALSE);
				}
				SIZE size;
				GetTextExtentPoint( dc, L"W", 1, &size );
				infoBottom = rect.bottom = rect.top + size.cy * lines.GetSize();
				::ReleaseDC( text, dc );
				MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				//GetDlgItem( IDC_INFO )->Invalidate();


				GetWindowRect( m_EditCtrl, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left = 4;
				rect.right = cx-4;
				height = rect.bottom-rect.top;
				rect.top = infoBottom + 4;
				rect.bottom = cy-4;
				if ( !CDlgInput::CurrentObject->Multiline )
				{
					SetWindowLong( m_EditCtrl, GWL_STYLE, GetWindowLong( m_EditCtrl, GWL_STYLE ) & !(ES_MULTILINE|ES_WANTRETURN|WS_VSCROLL) );
					rect.bottom = rect.top + height;
				}
				height = rect.top;
				SetWindowPos( m_EditCtrl, HWND_TOP, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW );

#ifdef PNA
				rect.top = cy;
				rect.left = 2*res;
				rect.right = (cx/2) - 2*res;
				rect.bottom = cy + 24*res;
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

				rect.left = (cx/2) + 2*res;
				rect.right = cx - 2*res;
				MoveWindow( cancel, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#endif
			}
		}
        return TRUE;

    default:
        return FALSE;
    }
}


CDlgInput::CDlgInput(HWND parent)
{
	m_Parent = parent;
	m_Label = _T("");
	m_Edit = _T("");
	Multiline = FALSE;
}

int CDlgInput::DoModal()
{
    CurrentObject = this;
    return DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), m_Parent, InputDialogProc );
}

// end SPDlgTest.cpp
