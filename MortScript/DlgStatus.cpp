#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#else
#include "windows.h"
#include <windowsx.h>
#include "mortafx.h"
#ifdef SMARTPHONE
#include "smartphone\resource.h"
#else
#include "resource.h"
#endif
#endif

#if ( !defined PNA && !defined DESKTOP )
#include <aygshell.h>
#endif

#include "morttypes.h"
#include "helpers.h"

#include "DlgStatus.h"

extern HINSTANCE g_hInst;
extern LPCTSTR MutexName;
extern HANDLE ScriptAborted;

extern int   StatusListItemHeight;
extern HFONT StatusListFont;

HWND		StatusWindow;
CDlgStatus *StatusDialog;


HANDLE StatusDialogThread = NULL;
HANDLE StatusDialogFinished = NULL;
DWORD WINAPI StatusDialogThreadFunction( LPVOID lpParameter )
{
	int rc = StatusDialog->DoModal();
	SetEvent( StatusDialogFinished );
	return rc;

	//CInterpreter interpreter;
	//interpreter.RunFile( CDlgStatus::CurrentObject->ScriptFile );

	//PostMessage( StatusWindow, WM_EXIT_STATUS, NULL, NULL );
	//return IDOK;
}


BOOL CALLBACK statusDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND listbox = GetDlgItem( hwndDlg, IDC_ENTRIES );
    HWND text    = GetDlgItem( hwndDlg, IDC_INFO );
	HWND message = GetDlgItem( hwndDlg, IDC_STATUS_MSG );
    //HWND count   = GetDlgItem( hwndDlg, IDC_COUNTDOWN );
    //HWND ok      = GetDlgItem( hwndDlg, IDOK );
	HWND cancel  = GetDlgItem( hwndDlg, IDCANCEL );

    switch ( uMsg )
    {
	case WM_CREATE:
		ShowWindow( hwndDlg, SW_HIDE );
		return 0;

    case WM_INITDIALOG:
        {
			ShowWindow( hwndDlg, SW_HIDE );

            for ( int i=0; i < StatusDialog->m_Strings.GetSize(); i++ )
            {
                SendMessage ( listbox, LB_ADDSTRING, 0,  (LPARAM)(LPCTSTR)StatusDialog->m_Strings.GetAt(i) ); 
            }
            //SendMessage (listbox, LB_SETCURSEL, CDlgStatus::CurrentObject->m_Default, 0 ); 
            SendMessage( text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)StatusDialog->m_Info ); 
            SendMessage( hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)StatusDialog->m_Title ); 
			if ( StatusDialog->m_Strings.GetSize() == 0 )
				SendMessage( message, WM_SETTEXT, 0, (LPARAM)L"" ); 
			else
			{
				int lastidx = StatusDialog->m_Strings.GetSize()-1;
				SendMessage( message, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)StatusDialog->m_Strings.GetAt( lastidx ) ); 
				SendMessage( listbox, LB_SETCURSEL, lastidx, 0 );
			}

#ifdef PNA
			RECT rect;
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
			MoveWindow( hwndDlg, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#else
			/*
            SHMENUBARINFO info;
            info.cbSize = sizeof(info);
            info.hwndParent = hwndDlg;
			info.dwFlags = 0; 
            info.nToolBarId = IDR_OKCANCEL;
            info.hInstRes = g_hInst;
            info.nBmpId = 0;
            info.cBmpImages = 0;
            SHCreateMenuBar(&info); 
			*/

#if ( !defined PNA && !defined DESKTOP )
			SHINITDLGINFO shidi;
			memset(&shidi, 0, sizeof(SHINITDLGINFO));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);
#endif

			//ShowWindow( ok, SW_HIDE );
			ShowWindow( cancel, SW_HIDE );
#endif
			if ( StatusListFont != NULL )
				SendMessage( listbox, WM_SETFONT, (WPARAM)StatusListFont, MAKELPARAM(1,0) );

			if ( StatusListItemHeight != 0 )
				SendMessage( listbox, LB_SETITEMHEIGHT, 0, MAKELPARAM(StatusListItemHeight,0) );

			//SetForegroundWindow( hwndDlg );
            SetFocus( listbox );

#ifdef DESKTOP
			RECT clientRect;
			GetClientRect( hwndDlg, &clientRect );
			int x = ( GetSystemMetrics( SM_CXSCREEN ) - (clientRect.right-clientRect.left) ) / 2;
			int y = ( GetSystemMetrics( SM_CYSCREEN ) - (clientRect.bottom-clientRect.top) ) / 2;

			MoveWindow( hwndDlg, x, y, clientRect.right-clientRect.left, clientRect.bottom-clientRect.top, FALSE );
#endif
			//ShowWindow( hwndDlg, SW_HIDE );

			SendMessage( hwndDlg, WM_UPDATE_DISPLAY, 0, 0 );

			StatusWindow = hwndDlg;
			// ScriptExecutionThread = CreateThread( NULL, 0, ScriptExecutionThreadFunction, NULL, 0, NULL );
        }
		return TRUE;
		break;

    case WM_COMMAND:
        if ( wParam == IDCANCEL )
        {
			if ( ! StatusDialog->ScriptFinished )
			{
				RegWriteDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Abort", MutexName, 1 );
				SetEvent( ScriptAborted );
				//WaitForSingleObject( ScriptFinished, 2000 );

				/*
				DWORD exitCode = 0;
				for ( int i=0; i<2000 && GetExitCodeThread( ScriptExecutionThread, &exitCode ) && exitCode == STILL_ACTIVE; i+=100 )
					Sleep( 100 );
				
				if ( exitCode == STILL_ACTIVE )
				{
					TerminateThread( AfxGetThr, IDCANCEL );

					HKEY    key;
					if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Abort", 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS )
					{
						RegDeleteValue( key, MutexName );
						RegCloseKey( key );
					}
					if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS )
					{
						RegDeleteValue( key, MutexName );
						RegCloseKey( key );
					}
				}

				CloseHandle( ScriptExecutionThread );
				ScriptExecutionThread = NULL;

				EndDialog( hwndDlg, IDCANCEL );
				*/
			}
			else
				EndDialog( hwndDlg, IDOK );
        }
        return TRUE;
		break;

	case WM_EXIT_STATUS:
		if ( StatusDialog->ScriptFinished )
		{
			//CloseHandle( ScriptExecutionThread );
			//ScriptExecutionThread = NULL;
			if ( StatusDialog->KeepOpen && StatusDialog->DisplayStyle != 0 )
			{
				StatusDialog->ShowCancel = TRUE;
	            SendMessage( cancel, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)L"OK" ); 

				RECT clientRect;
				GetClientRect( hwndDlg, &clientRect );
				SendMessage( hwndDlg, WM_SIZE, SIZE_RESTORED, MAKELPARAM( clientRect.right-clientRect.left, clientRect.bottom-clientRect.top ) );
			}
			else
			{
				EndDialog( hwndDlg, IDOK );
			}
		}
		else
		{
			EndDialog( hwndDlg, IDCANCEL );
		}
		return TRUE;
		break;

	case WM_CLOSE:
		if ( StatusDialog->ScriptFinished )
		{
			EndDialog( hwndDlg, IDOK );
		}
		else
		{
			ShowWindow( hwndDlg, SW_MINIMIZE );
		}
		return TRUE;
		break;

	case WM_UPDATE_DISPLAY:
		{
			if ( StatusDialog->DisplayStyle == 0 )
				ShowWindow( hwndDlg, SW_HIDE );
			else
			{
	            SendMessage( hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)StatusDialog->m_Title ); 
	            SendMessage( text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)StatusDialog->m_Info ); 

				RECT clientRect;
				GetClientRect( hwndDlg, &clientRect );
				if ( StatusDialog->DisplayStyle == 1 )
				{
					ShowWindow( message, SW_HIDE );
					ShowWindow( listbox, SW_SHOW );
				}
				else
				{
					ShowWindow( message, SW_SHOW );
					ShowWindow( listbox, SW_HIDE );
				}

				SendMessage( hwndDlg, WM_SIZE, SIZE_RESTORED, MAKELPARAM( clientRect.right-clientRect.left, clientRect.bottom-clientRect.top ) );
				if ( ! IsWindowVisible( hwndDlg ) )
				{
					ShowWindow( hwndDlg, SW_SHOW );
					SetForegroundWindow( hwndDlg );
				}

				InvalidateRect( hwndDlg, NULL, TRUE );
			}
			return TRUE;
		}
		break;

	case WM_ADD_STATUS_MSG:
		{
            SendMessage( message, WM_SETTEXT, 0, lParam ); 

			int sel = SendMessage( listbox, LB_ADDSTRING, 0, lParam );
			if ( sel >= StatusDialog->MaxEntries )
			{
				SendMessage( listbox, LB_DELETESTRING, 0, 0 );
				sel--;
			}

			SendMessage( listbox, LB_SETCURSEL, sel, 0 );

			//if ( StatusDialog->DisplayStyle == 0 )
			//	ShowWindow( hwndDlg, SW_HIDE );

			return TRUE;
		}
		break;

	case WM_DEL_STATUS_MSG:
		{
			int idx = SendMessage( listbox, LB_GETCOUNT, 0,0 ) - 1;

			if ( idx >= 0 )
			{
				SendMessage( listbox, LB_DELETESTRING, idx, 0 );
				idx--;
			}

			if ( idx >= 0 )
			{
				int len = SendMessage( listbox, LB_GETTEXTLEN, idx, 0 );
				TCHAR *text = new TCHAR[len+1];
				SendMessage( listbox, LB_GETTEXT, idx, (LPARAM)text );

				SendMessage( message, WM_SETTEXT, 0, (LPARAM)text ); 

				delete[] text;

				SendMessage( listbox, LB_SETCURSEL, idx, 0 );
			}

			return TRUE;
		}
		break;

	case WM_SET_STATUS_LINES:
		{
			StatusDialog->MaxEntries = lParam;
			if ( StatusDialog->MaxEntries == 0 )
				StatusDialog->MaxEntries = 10;

			while ( SendMessage( listbox, LB_GETCOUNT, 0,0 ) > StatusDialog->MaxEntries )
			{
				SendMessage( listbox, LB_DELETESTRING, 0, 0 );
			}
			int sel = SendMessage( listbox, LB_GETCOUNT, 0,0 ) - 1;
			SendMessage( listbox, LB_SETCURSEL, sel, 0 );
			return TRUE;
		}
		break;

	case WM_CLEAR_STATUS_MSG:
		{
            SendMessage( message, WM_SETTEXT, 0, (LPARAM)L"" ); 
			while ( SendMessage( listbox, LB_GETCOUNT, 0,0 ) > 0 )
			{
				SendMessage( listbox, LB_DELETESTRING, 0, 0 );
			}
			return TRUE;
		}
		break;

	/*
	case WM_FETCH_STATUS_LINES:
		{
			StatusDialog->m_Strings.RemoveAll();

			int count = SendMessage( listbox, LB_GETCOUNT, 0,0 );
			for ( int i = 0; i < count; i++ )
			{
				int len = SendMessage( listbox, LB_GETTEXTLEN, i, 0 );
				TCHAR *text = new TCHAR[len+1];
				SendMessage( listbox, LB_GETTEXT, i, (LPARAM)text );
				StatusDialog->m_Strings.Add( text );
				delete[] text;
			}
			return TRUE;
		}
		break;
	*/

	case WM_MOVE:
		{
			if ( StatusDialog->DisplayStyle == 0 )
				ShowWindow( hwndDlg, SW_HIDE );
			return TRUE;
		}

	case WM_SIZE:
		{
			if ( StatusDialog->DisplayStyle == 0 )
				ShowWindow( hwndDlg, SW_HIDE );

			if ( wParam == SIZE_RESTORED )
			{
				int   cx = LOWORD( lParam );
				int   cy = HIWORD( lParam );
				// Minimum smartphone screen size - cause some devices deliver 0x0... ^^
				if ( cx < 176 ) cx = 176;
				if ( cy < 168 ) cy = 168;

				RECT  rect, rect2;
				//int   width; //, height;//jwz::
#ifdef DESKTOP
				int   res = 1;
#else
				int   res = (cx>320)?2:1;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.top = 4;
				rect.left = 4;
				rect.right = cx-4;
				MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				if ( ! StatusDialog->m_Info.IsEmpty() )
				{
					HDC dc = ::GetWindowDC( text );
					CStrArray lines;

					SplitLines( dc, (LPCTSTR)StatusDialog->m_Info, lines, rect.right-rect.left );
					CStr infoWrapped;
					for ( int i=0; i<lines.GetSize(); i++ )
					{
						infoWrapped += lines[i];
						if ( i<lines.GetSize()-1 ) infoWrapped += L"\n";
					}
					SetWindowText( text, infoWrapped );

					SIZE size;
					GetTextExtentPoint( dc, L"W", 1, &size );
					rect.bottom = rect.top + size.cy * lines.GetSize();
					::ReleaseDC( text, dc );
					MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

					ShowWindow( text, SW_SHOW );
					rect2.top = rect.bottom + 4;
				}
				else
				{
					ShowWindow( text, SW_HIDE );
					rect2.top = rect.top;
				}

				rect2.left = 4;
				rect2.right = cx - 4;
				if ( StatusDialog->ShowCancel )
				{
					rect2.bottom = cy - 28*res;
				}
				else
				{
					rect2.bottom = cy - 4;
				}
				MoveWindow( listbox, rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, TRUE );
				MoveWindow( message, rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, TRUE );

				if ( StatusDialog->ShowCancel )
				{
					rect.top = cy - 26*res;;
					rect.left = 4;
					rect.bottom = cy - 4;
					rect.right = cx - 4;
					MoveWindow( cancel, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
					ShowWindow( cancel, SW_SHOW );
				}
				else
				{
					ShowWindow( cancel, SW_HIDE );
				}
			}
		}
		return TRUE;
		break;

    default:
		// Sometimes, Windows just loves to make the window visible for no reason, so hide it again... ^^
		if ( hwndDlg != NULL && IsWindowVisible( hwndDlg ) && StatusDialog->DisplayStyle == 0 )
			ShowWindow( hwndDlg, SW_HIDE );
        return FALSE;
    }
}


CDlgStatus::CDlgStatus( LPCTSTR file )
{
	m_Info = _T("");
	ScriptFile = file;
	ShowCancel = 0;
	DisplayStyle = 0;
	MaxEntries = 10;
	KeepOpen = true;
	ScriptFinished = false;
}

int CDlgStatus::DoModal()
{
    int rc = DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), NULL, statusDialogProc );
	return rc;
}

void CDlgStatus::SetStatusType( int type )
{
	DisplayStyle = type;
	if ( StatusWindow == NULL && type != 0 )
	{
		StatusDialogFinished = CreateEvent( 0, TRUE, FALSE, CStr(MutexName)+L"StatusDialogFinished" );
		StatusDialogThread = CreateThread( NULL, 0, StatusDialogThreadFunction, NULL, 0, NULL );
	}
	else if ( StatusWindow != NULL )
	{
		PostMessage( StatusWindow, WM_UPDATE_DISPLAY, 0, 0 );
	}
}

void CDlgStatus::AddStatusMsg( LPCTSTR message )
{
	m_Strings.Add( message );

	if ( StatusDialog->MaxEntries == 0 )
		StatusDialog->MaxEntries = 10;

	if ( m_Strings.GetSize() > StatusDialog->MaxEntries )
	{
		m_Strings.RemoveAt(0);
	}

	if ( StatusWindow != NULL )
	    SendMessage( StatusWindow, WM_ADD_STATUS_MSG, 0, (LPARAM)message ); 
}

void CDlgStatus::DelStatusMsg()
{
	if ( m_Strings.GetSize() > 0 )
	{
		int idx = m_Strings.GetSize()-1;
		m_Strings.RemoveAt( idx );

		if ( StatusWindow != NULL )
		{
			SendMessage( StatusWindow, WM_DEL_STATUS_MSG, 0, 0 ); 
		}
	}
}

void CDlgStatus::AppendStatusMsg( LPCTSTR message )
{
	if ( StatusDialog->MaxEntries == 0 )
		StatusDialog->MaxEntries = 10;

	if ( m_Strings.GetSize() == 0 )
	{
		m_Strings.Add( message );
		if ( StatusWindow != NULL )
			SendMessage( StatusWindow, WM_ADD_STATUS_MSG, 0, (LPARAM)message ); 
	}
	else
	{
		CStr msg;
		int idx = m_Strings.GetSize()-1;
		msg = m_Strings.GetAt( idx );
		msg += message;
		m_Strings.RemoveAt( idx );
		m_Strings.Add( msg );

		if ( StatusWindow != NULL )
		{
			SendMessage( StatusWindow, WM_DEL_STATUS_MSG, 0, 0 ); 
			SendMessage( StatusWindow, WM_ADD_STATUS_MSG, 0, (LPARAM)(LPCTSTR)msg ); 
		}
	}
}

void CDlgStatus::SetStatusLines( int lines )
{
	StatusDialog->MaxEntries = lines;
	if ( StatusDialog->MaxEntries == 0 )
		StatusDialog->MaxEntries = 10;

	while ( m_Strings.GetSize() > StatusDialog->MaxEntries )
	{
		m_Strings.RemoveAt(0);
	}

	if ( StatusWindow != NULL )
		SendMessage( StatusWindow, WM_SET_STATUS_LINES, NULL, lines );
}

void CDlgStatus::ClearStatusLines()
{
	m_Strings.RemoveAll();

	if ( StatusWindow != NULL )
		SendMessage( StatusWindow, WM_CLEAR_STATUS_MSG, NULL, NULL );
}

// end SPDlgTest.cpp
