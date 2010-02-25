#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "interpreter.h"

#ifndef SMARTPHONE
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#else
extern HINSTANCE g_hInst;
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "CommandsWindows.h"


BOOL CmdShow( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Show'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindowMy( params.GetAt(0) , 0);

	if ( wnd != NULL )
	{
#ifdef DESKTOP
        ::ShowWindow( wnd, SW_SHOW );
#endif
		::SetForegroundWindow(wnd);
	}
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdMinimize( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Minimize'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindowMy( params.GetAt(0) , 0);

	if ( wnd != NULL )
	{
		::ShowWindow( wnd, SW_MINIMIZE );
	}
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdClose( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Close'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindowMy( params.GetAt(0) , 0);

	if ( wnd != NULL )
	{
		if ( wnd != ::GetDesktopWindow() )
		{
			DWORD	dwProcessID;
			CStr winProcName;
			LPTSTR  wpnBuffer = winProcName.GetBuffer( MAX_PATH );
			if ( ::GetWindowThreadProcessId( wnd, &dwProcessID ) )
			{
    			::GetModuleFileName( (HMODULE)dwProcessID, wpnBuffer, MAX_PATH );
			}
			winProcName.ReleaseBuffer();

			if ( winProcName.GetLength() >= 11 && winProcName.Right(11).CompareNoCase( L"shell32.exe" ) != 0 )
			{
	    		::PostMessage( wnd, WM_CLOSE, 0,0 );
			}
			else 
			{
				::SendMessage( wnd, WM_CLOSE, 0, 0 );
			}
		}
	}
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdGetColorAt( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;

    if ( interpreter.Split( param, ',', params, 1, 1<<2 ) != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetColorAt'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HDC hScreenDC  = ::GetWindowDC(0);
    COLORREF col = ::GetPixel( hScreenDC, (long)params[0], (long)params[1] );
    ::ReleaseDC(0, hScreenDC); 

    CValue value;
    value = (long)col;
    return interpreter.SetVariable( params[2], value );
}

BOOL CmdGetWindowPos( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params, 1, 0x1e ); // 0001 1110
    if ( params.GetSize() != 2 && params.GetSize() != 5 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetVersion'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindowMy( params.GetAt(0) ,0);
	RECT rect;

	if ( wnd != NULL && ::GetWindowRect( wnd, &rect ) )
	{
	    rc  = interpreter.SetVariable( params[1], CValue( (long)rect.left ) );
	    rc &= interpreter.SetVariable( params[2], CValue( (long)rect.top ) );
	    rc &= interpreter.SetVariable( params[3], CValue( (long)rect.right ) );
	    rc &= interpreter.SetVariable( params[4], CValue( (long)rect.bottom ) );
    }
	else
	{
	    rc  = interpreter.SetVariable( params[1], 0L );
	    rc &= interpreter.SetVariable( params[2], 0L );
	    rc &= interpreter.SetVariable( params[3], 0L );
	    rc &= interpreter.SetVariable( params[4], 0L );
	}

    return rc;
}


BOOL CmdGetWindowText( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;

    if ( interpreter.Split( param, ',', params, 1, 1<<2 ) != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetWindowText'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;
    pt.x = (long)params[0];
    pt.y = (long)params[1];
    HWND wnd = ::WindowFromPoint( pt );
    TCHAR value[MAX_PATH];
    ::GetWindowText( wnd, value, MAX_PATH );

    return interpreter.SetVariable( params[2], value );
}

BOOL CmdWaitFor( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'WaitFor'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	ULONG   wait   = (long)params.GetAt(1) * 2;
	CStr window = (LPCTSTR)params.GetAt(0);

	ULONG i;
	HWND wnd = NULL;
	for ( i=0; wnd == NULL && i <= wait; i++ )
	{
		if ( i > 0 ) ::Sleep( 500 );

		wnd = FindWindowMy( window ,0);
	}

    if ( wnd == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdWaitForActive( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'WaitForActive'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	ULONG   wait   = (long)params.GetAt(1) * 2;
	CStr window = (LPCTSTR)params.GetAt(0);

	ULONG i;
	HWND wnd = NULL;
	for ( i=0; wnd == NULL && i <= wait; i++ )
	{
		if ( i > 0 ) ::Sleep( 500 );

		HWND hwnd = ::GetForegroundWindow();
		TCHAR windowTitle[256];
		::GetWindowText( hwnd, windowTitle, 256 );
		if ( CStr(windowTitle).Find( window ) != -1 )
		{
			wnd = hwnd;
		}
	}

    if ( wnd == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdGetActiveWindow( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetActiveWindow'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND hwnd = ::GetForegroundWindow();
	TCHAR windowTitle[256];
	::GetWindowText( hwnd, windowTitle, 256 );

    return interpreter.SetVariable( params[0], windowTitle );
}


BOOL CmdSendCommand( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SendCommand'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HWND wnd;
	long message;
    if ( params.GetSize() == 1 )
    {
        wnd = ::GetForegroundWindow();
		message = (long)params[0];
    }
    else
    {
	    wnd = FindWindowMy( params[0] ,0);
		if ( wnd == NULL )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
            {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
			}
			else
			{
                return TRUE;
            }
		}
		message = (long)params[1];
    }

    ::PostMessage( wnd, WM_COMMAND, message, 0 );

    return TRUE;
}

BOOL CmdSendMessage( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SendMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HWND wnd;
	long message, lparam, wparam;
    if ( params.GetSize() == 3 )
    {
        wnd = ::GetForegroundWindow();
		message = (long)params[0];
		wparam  = (long)params[1];
		lparam  = (long)params[2];
    }
    else
    {
	    wnd = FindWindowMy( params[0] ,0);
		if ( wnd == NULL )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
            {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
			}
			else
			{
                return TRUE;
            }
		}
		message = (long)params[1];
		wparam  = (long)params[2];
		lparam  = (long)params[3];
    }

    ::SendMessage( wnd, message, wparam, lparam );

    return TRUE;
}


BOOL CmdPostMessage( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'PostMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HWND wnd;
	long message, lparam, wparam;
    if ( params.GetSize() == 3 )
    {
        wnd = ::GetForegroundWindow();
		message = (long)params[0];
		wparam  = (long)params[1];
		lparam  = (long)params[2];
    }
    else
    {
	    wnd = FindWindowMy( params[0] , 0);
		if ( wnd == NULL )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
            {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
			}
			else
			{
                return TRUE;
            }
		}
		message = (long)params[1];
		wparam  = (long)params[2];
		lparam  = (long)params[3];
    }

    ::PostMessage( wnd, message, wparam, lparam );

    return TRUE;
}

