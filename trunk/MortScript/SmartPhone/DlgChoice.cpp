#include "windows.h"
#include <windowsx.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "resource.h"
#include "../helpers.h"
#include "DlgChoice.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance

extern int   ChoiceItemHeight;
extern HFONT ChoiceFont;

CDlgChoice* CDlgChoice::CurrentObject = NULL;

BOOL CALLBACK choiceDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND listbox = GetDlgItem( hwndDlg, IDC_LIST );
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL );
    HWND count   = GetDlgItem( hwndDlg, IDC_COUNTDOWN );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );
	HWND cancel  = GetDlgItem( hwndDlg, IDCANCEL );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
            for ( int i=0; i < CDlgChoice::CurrentObject->m_Strings.GetSize(); i++ )
            {
                SendMessage (listbox, LB_ADDSTRING, 0,  (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Strings.GetAt(i) ); 
            }
            SendMessage (listbox, LB_SETCURSEL, CDlgChoice::CurrentObject->m_Default, 0 ); 
            //SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Info ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Title ); 

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
			if ( ChoiceFont != NULL )
				SendMessage( listbox, WM_SETFONT, (WPARAM)ChoiceFont, MAKELPARAM(1,0) );

			if ( ChoiceItemHeight != 0 )
				SendMessage( listbox, LB_SETITEMHEIGHT, 0, MAKELPARAM(ChoiceItemHeight,0) );

	        if ( CDlgChoice::CurrentObject->m_Timeout > 0 )
	        {
		        CDlgChoice::CurrentObject->countdown = CDlgChoice::CurrentObject->m_Timeout;
		        CDlgChoice::CurrentObject->m_Countdown.Format( L"%d", CDlgChoice::CurrentObject->countdown );
                SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Countdown ); 
                SetTimer( hwndDlg, 1, 1000, NULL );
	        }
	        else
	        {
		        ShowWindow( count, SW_HIDE );
	        }

			SetForegroundWindow( hwndDlg );
            SetFocus( listbox );

            return TRUE;
        }

    case WM_COMMAND:
        if ( wParam == IDOK )
        {
            CDlgChoice::CurrentObject->m_Selected = SendMessage (listbox, LB_GETCURSEL, 0, 0 ) + 1; 
            EndDialog( hwndDlg, IDOK );
            return TRUE;
        }
        if ( wParam == IDCANCEL )
        {
            CDlgChoice::CurrentObject->m_Selected = 0; 
            EndDialog( hwndDlg, IDCANCEL );
            return TRUE;
        }
        if ( HIWORD(wParam) == LBN_SELCHANGE )
        {
	        if ( CDlgChoice::CurrentObject->m_Timeout > 0 )
	        {
		        KillTimer( hwndDlg, 1 );
		        CDlgChoice::CurrentObject->countdown = CDlgChoice::CurrentObject->m_Timeout;
		        CDlgChoice::CurrentObject->m_Countdown.Format( L"%d", CDlgChoice::CurrentObject->countdown );
                SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Countdown ); 
                SetTimer( hwndDlg, 1, 1000, NULL );
	        }
            return TRUE;
        }
        if ( HIWORD(wParam) == LBN_DBLCLK )
        {
            CDlgChoice::CurrentObject->m_Selected = SendMessage (listbox, LB_GETCURSEL, 0, 0 ) + 1; 
            if ( CDlgChoice::CurrentObject->m_Selected > 0 ) EndDialog( hwndDlg, IDOK );
            return TRUE;
        }

    case WM_TIMER:
    	if ( wParam == 1 )
	    {
		    CDlgChoice::CurrentObject->countdown--;
		    if ( CDlgChoice::CurrentObject->countdown == 0 )
		    {
			    KillTimer( hwndDlg, 1 );
                CDlgChoice::CurrentObject->m_Selected = SendMessage (listbox, LB_GETCURSEL, 0, 0 ) + 1; 
                EndDialog( hwndDlg, IDOK );
		    }
		    else
		    {
			    CDlgChoice::CurrentObject->m_Countdown.Format( L"%d", CDlgChoice::CurrentObject->countdown );
			    SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgChoice::CurrentObject->m_Countdown ); 
		    }
	    }
		return TRUE;
		break;

	case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;
			if ( nmhdr->code == NM_RETURN )
				PostMessage( hwndDlg, WM_COMMAND, IDOK, NULL );
		}
        return TRUE;
		break;

	case WM_SIZE:
		{
			if ( wParam == SIZE_RESTORED )
			{
				int   cx = LOWORD( lParam );
				int   cy = HIWORD( lParam );
				// Minimum smartphone screen size - cause some devices deliver 0x0... ^^
				if ( cx < 176 ) cx = 176;
				if ( cy < 168 ) cy = 168;

				RECT  rect, rect2;
				int   width, height, infoBottom;
				int   res = (cx>320)?2:1;

#ifdef PNA
				cy = cy - 26 * res;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left = 4;
				rect.right = cx-4;
				rect.top = 4;

				if ( CDlgChoice::CurrentObject->m_Info.IsEmpty() && CDlgChoice::CurrentObject->m_Timeout <= 0 )
				{
					ShowWindow( text, SW_HIDE );
					ShowWindow( count, SW_HIDE );
					infoBottom = 0;
				}
				else
				{
					HDC dc = ::GetWindowDC( text );
					CStrArray lines;
					if ( CDlgChoice::CurrentObject->m_Info.IsEmpty() )
					{
						lines.Add( L"" );
						SetWindowText( text, L"" );
					}
					else
					{
						SplitLines( dc, (LPCTSTR)CDlgChoice::CurrentObject->m_Info, lines, rect.right-rect.left );
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
					infoBottom = rect.bottom = rect.top + size.cy * ( lines.GetSize() + ((CDlgChoice::CurrentObject->m_Timeout>0)?1:0) );
					::ReleaseDC( text, dc );
					MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
					//GetDlgItem( IDC_INFO )->Invalidate();

					GetWindowRect( count, &rect );
					ScreenToClient( hwndDlg, &rect );
					width = rect.right - rect.left;
					height = rect.bottom - rect.top;
					rect.right = cx-4;
					rect.left  = rect.right - width;
					rect.bottom = infoBottom;
					rect.top = infoBottom - height;
					MoveWindow( count, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				}

				rect2.top = infoBottom + 4;
				rect2.left = 4;
				rect2.right = rect.right;
				rect2.bottom = cy - 4;
				MoveWindow( listbox, rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, TRUE );

#ifdef PNA
				rect.top = cy;
				rect.left = 4;
				rect.right = (cx/2) - 2;
				rect.bottom = cy + 24*res;
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

				rect.left = (cx/2) + 2;
				rect.right = cx - 4;
				MoveWindow( cancel, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#endif
			}
		}
		return TRUE;
		break;

    default:
        return FALSE;
    }
}


CDlgChoice::CDlgChoice()
{
	m_Info = _T("");
	m_Countdown = _T("");
	m_Timeout = -1;
	m_Default = 0;
}

int CDlgChoice::DoModal()
{
    CurrentObject = this;
    int choice = DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), NULL, choiceDialogProc );
	// int e = GetLastError();
	// CString errMsg;
	// errMsg.Format( L"Fehler %d, IDD: %d, Inst.: %d", e, IDD, g_hInst );
	// MessageBox( NULL, errMsg, L"Debug", MB_OK );
	return choice;
}

// end SPDlgTest.cpp
