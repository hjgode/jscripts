#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
//#include "Tlhelp32.h"
//#include "resource.h"
#include "inifile.h"
#include "interpreter.h"
BOOL CALLBACK SearchWindowText( HWND hwnd, LPARAM lParam );
time_t SystemTimeToUnixTime( const SYSTEMTIME &st );
void   UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst);
time_t FileTimeToUnixTime( const FILETIME &ft );

#ifndef SMARTPHONE
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#else
extern HINSTANCE g_hInst;
#endif

#ifndef PNA
extern CStr Proxy;
#include "wininet.h"
#endif

#if !defined( PNA ) && !defined( DESKTOP )
#include "ras.h"
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "ValueArray.h"
#include "FunctionsData.h"
#include "Interpreter.h"

CValue  FctWndExists( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2)
    {
        errorMessage = InvalidParameterCount + L"'WndExists'";
        error = 9;
        return CValue();
    }

    CStr wnd = params[0];
	HWND foundWnd;
	if (params.GetSize()==2){
		CStr flag = params[1];
		if (flag.CompareNoCase(L"true")==0 || flag.CompareNoCase(L"1")==0 )
			foundWnd = FindWindowMy( wnd , 1);
		else
			foundWnd = FindWindowMy( wnd , 0);
	}else
		foundWnd = FindWindowMy( wnd , 0);
    // CStr msg;
    // msg.Format( L"Wnd: %s, hWnd: %x", wnd, sw.FoundWnd );
    // MessageBox( NULL, msg, L"Debug", MB_OK|MB_SETFOREGROUND );
    CValue rc;
    if ( foundWnd != NULL )
        rc = 1L;
    else
        rc = 0L;

    return rc;
}

CValue  FctActiveWindow( CValueArray &params, int &error, CStr &errorMessage )
{
    HWND hwnd = ::GetForegroundWindow();
	// Sometimes, after a window was closed, Windows isn't able to return the active window.
	// So wait until one becomes active...
	for ( int retry = 0; hwnd == NULL && retry < 10; retry++ )
	{
		Sleep(10);
		hwnd = ::GetForegroundWindow();
	}

    TCHAR windowTitle[256];
    ::GetWindowText( hwnd, windowTitle, 256 );

    CValue rc;
    rc = windowTitle;

    return rc;
}

CValue  FctWndActive( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'WndActive'";
        error = 9;
        return CValue();
    }

    HWND hwnd = ::GetForegroundWindow();
    TCHAR windowTitle[256];
    int len = ::GetWindowText( hwnd, windowTitle, 256 );
	while ( len == 0 && ::GetParent( hwnd ) != NULL )
	{
		hwnd = ::GetParent( hwnd );
	    len  = ::GetWindowText( hwnd, windowTitle, 256 );
	}

    CValue rc;

    if ( CStr(windowTitle).FindNoCase( (CStr)params[0] ) != -1 )
        rc = 1L;
    else
        rc = 0L;
    return rc;
}

CValue  FctWindowText( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'WindowText'";
        error = 9;
        return CValue();
    }

    CValue rc;

    POINT pt;
    pt.x = (long)params[0];
    pt.y = (long)params[1];
    HWND wnd = ::WindowFromPoint( pt );
    if ( wnd != NULL )
    {
        TCHAR value[MAX_PATH];
        ::GetWindowText( wnd, value, MAX_PATH );
        rc = value;
    }

    return rc;
}

CValue  FctWndLeft( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'WndLeft'";
        error = 9;
        return CValue();
    }

    CStr wnd = params[0];
	HWND foundWnd = FindWindowMy( wnd ,0);
	/*
    wcscpy( sw.SearchText, (LPCTSTR)wnd );
	sw.SearchText[255] = '\0';
    sw.FoundWnd   = NULL;
	sw.Type = 0;
    ::EnumWindows( SearchWindowText, (LPARAM)&sw );
	*/

    CValue rc;
    if ( foundWnd != NULL )
    {
        RECT rect;
        if ( ::GetWindowRect( foundWnd, &rect ) )
            rc = (long)rect.left;
    }

    return rc;
}

CValue  FctWndRight( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'WndRight'";
        error = 9;
        return CValue();
    }

    CStr wnd = params[0];
	HWND foundWnd = FindWindowMy( wnd ,0);
	/*
    wcscpy( sw.SearchText, (LPCTSTR)wnd );
	sw.SearchText[255] = '\0';
    sw.FoundWnd   = NULL;
	sw.Type = 0;
    ::EnumWindows( SearchWindowText, (LPARAM)&sw );
	*/

    CValue rc;
    if ( foundWnd != NULL )
    {
        RECT rect;
        if ( ::GetWindowRect( foundWnd, &rect ) )
            rc = (long)rect.right;
    }

    return rc;
}

CValue  FctWndTop( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'WndTop'";
        error = 9;
        return CValue();
    }

    CStr wnd = params[0];
	HWND foundWnd = FindWindowMy( wnd ,0);
	/*
    wcscpy( sw.SearchText, (LPCTSTR)wnd );
	sw.SearchText[255] = '\0';
    sw.FoundWnd   = NULL;
	sw.Type = 0;
    ::EnumWindows( SearchWindowText, (LPARAM)&sw );
	*/

    CValue rc;
    if ( foundWnd != NULL )
    {
        RECT rect;
        if ( ::GetWindowRect( foundWnd, &rect ) )
            rc = (long)rect.top;
    }

    return rc;
}

CValue  FctWndBottom( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'WndBottom'";
        error = 9;
        return CValue();
    }

    CStr wnd = params[0];
	HWND foundWnd = FindWindowMy( wnd ,0);
	/*
    wcscpy( sw.SearchText, (LPCTSTR)wnd );
	sw.SearchText[255] = '\0';
    sw.FoundWnd   = NULL;
	sw.Type = 0;
    ::EnumWindows( SearchWindowText, (LPARAM)&sw );
	*/

    CValue rc;
    if ( foundWnd != NULL )
    {
        RECT rect;
        if ( ::GetWindowRect( foundWnd, &rect ) )
            rc = (long)rect.bottom;
    }

    return rc;
}

CValue  FctColorAt( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'WindowText'";
        error = 9;
        return CValue();
    }

    CValue rc;

    HDC hScreenDC  = ::GetWindowDC(0);
    COLORREF col = ::GetPixel( hScreenDC, (long)params[0], (long)params[1] );
    ::ReleaseDC(0, hScreenDC); 

    rc = (long)col;

    return rc;
}

CValue  FctScreenToChars( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 5 && params.GetSize() > 8 )
    {
        errorMessage = InvalidParameterCount + L"'ScreenToChars'";
        error = 9;
        return CValue();
    }

    CValue rc;

	CMapStrToValue *lines = rc.GetMap();
	int currCol,  startCol  = (long)params[0], width  = (long)params[2];
	int currLine, startLine = (long)params[1], height = (long)params[3];
	DWORD color = (long)params[4];
	bool colorAsBg = TRUE;
	if ( params.GetSize() >= 6 ) colorAsBg = (long)params[5] ? true : false;
	CStr fgChar, bgChar;
	if ( params.GetSize() >= 7 ) fgChar = (CStr)params[6];
	if ( params.GetSize() >= 8 ) bgChar = (CStr)params[7];
	if ( fgChar.GetLength() == 0 ) fgChar = L"#";
	if ( bgChar.GetLength() == 0 ) bgChar = L"_";

	CStr clStr, lineStr;

    HDC hScreenDC  = ::GetWindowDC(0);
	for ( currLine = startLine; currLine < startLine+height; currLine++ )
	{
		clStr.Format( L"%d", currLine-startLine+1 );
		lineStr.Empty();
		for ( currCol = startCol; currCol < startCol+width; currCol++ )
		{
			COLORREF col = ::GetPixel( hScreenDC, currCol, currLine );
			if ( ( colorAsBg && col != color ) || ( !colorAsBg && col == color ) )
				lineStr += fgChar;
			else
				lineStr += bgChar;
		}
		lines->SetAt( clStr, lineStr );
	}
    ::ReleaseDC(0, hScreenDC); 

    return rc;
}


CValue  FctScreen( CValueArray &params, int &error, CStr &errorMessage )
{
    int condRes = FALSE;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Screen'";
        error = 9;
        return CValue();
    }

    if ( ((CStr)params[0]).CompareNoCase( L"landscape" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CXSCREEN ) > GetSystemMetrics( SM_CYSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"portrait" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > GetSystemMetrics( SM_CXSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"vga" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > 320 && GetSystemMetrics( SM_CXSCREEN ) > 320 );
    else if ( ((CStr)params[0]).CompareNoCase( L"qvga" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) <= 320 && GetSystemMetrics( SM_CXSCREEN ) <= 320 );
    else if ( ((CStr)params[0]).CompareNoCase( L"square" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) == GetSystemMetrics( SM_CXSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"width" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CXSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"height" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) );
    else
    {
        
        errorMessage = L"Invalid screen condition";
        error = 9;
        return CValue();
    }

    CValue rStr;
    rStr = (long)condRes;
    return rStr;
}

CValue  FctSendMessage( CValueArray &params, int &error, CStr &errorMessage )
{
    long result;

    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        errorMessage = InvalidParameterCount + L"'SendMessage'";
        error = 9;
        return CValue();
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
            errorMessage.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            error = 9;
		}
		message = (long)params[1];
		wparam  = (long)params[2];
		lparam  = (long)params[3];
    }

    CValue rVal;
	if ( error == 0 )
	{
	    result = ::SendMessage( wnd, message, wparam, lparam );
	    rVal = result;
	}

    return rVal;
}

