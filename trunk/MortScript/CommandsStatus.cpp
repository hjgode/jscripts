#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#else
#include "windows.h"
#include <windowsx.h>
#include "mortafx.h"
#include "resource.h"
#endif

#include "morttypes.h"
#include "helpers.h"
#include "interpreter.h"
#include "inifile.h"
#include "dlgstatus.h"

int   StatusListItemHeight = 0;
HFONT StatusListFont = NULL;
extern HWND StatusWindow;

BOOL CmdStatusInfo( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	StatusDialog->m_Title = (LPCTSTR)params[0];
	if ( params.GetSize() > 1 )
		StatusDialog->m_Info = (LPCTSTR)params[1];
	else
		StatusDialog->m_Info.Empty();

	if ( StatusWindow != NULL )
		SendMessage( StatusWindow, WM_UPDATE_DISPLAY, NULL, NULL );

	return TRUE;
}

BOOL CmdStatusType( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusType'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	if ( params.GetSize() > 1 )
		StatusDialog->KeepOpen = (long)params[1]?true:false;

	if ( params.GetSize() > 2 )
		StatusDialog->ShowCancel = (long)params[2]?true:false;

	//StatusDialog->DisplayStyle = (long)params[0];

	StatusDialog->SetStatusType( (long)params[0] );

	return TRUE;
}

BOOL CmdStatusHistorySize( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params );
    if ( params.GetSize() != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusHistorySize'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	StatusDialog->SetStatusLines( (long)params[0] );
	//SendMessage( GetMsgParent(), WM_SET_STATUS_LINES, NULL, (long)params[0] );

	return TRUE;
}

BOOL CmdStatusMessage( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	//if ( params.GetSize() > 1 )
	//	StatusDialog->DisplayStyle = (long)params[1];

	if ( params.GetSize() > 2 )
		StatusDialog->KeepOpen = (long)params[2]?true:false;

	if ( params.GetSize() > 3 )
		StatusDialog->ShowCancel = (long)params[3]?true:false;

	if ( params.GetSize() > 1 )
		StatusDialog->SetStatusType( (long)params[1] );

	//	SendMessage( GetMsgParent(), WM_UPDATE_DISPLAY, NULL, NULL );

	StatusDialog->AddStatusMsg( (LPCTSTR)params[0] );

	//SendMessage( GetMsgParent(), WM_ADD_STATUS_MSG, NULL, (LPARAM)(LPCTSTR)params[0] );

	return TRUE;
}

BOOL CmdStatusMessageAppend( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	StatusDialog->AppendStatusMsg( (LPCTSTR)params[0] );

	return TRUE;
}

BOOL CmdStatusRemoveLastMessage( CInterpreter &interpreter, CStr &param )
{
	StatusDialog->DelStatusMsg();

	return TRUE;
}


BOOL CmdStatusClear( CInterpreter &interpreter, CStr &param )
{
	StatusDialog->ClearStatusLines();
	return TRUE;
}

BOOL CmdStatusShow( CInterpreter &interpreter, CStr &param )
{
	if ( StatusWindow != NULL )
		SetForegroundWindow( StatusWindow );
	return TRUE;
}

BOOL CmdStatusListEntryFormat( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'StatusListEntryFormat'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HDC dc = GetDC( ::GetDesktopWindow() );
	int res = GetDeviceCaps( dc, LOGPIXELSY );
	ReleaseDC( ::GetDesktopWindow(), dc );

	StatusListItemHeight = (long)params[0];
#ifndef DESKTOP
	if ( res == 192 )
		StatusListItemHeight *= 2;
#endif

	if ( params.GetSize() > 1 )
	{
		if ( StatusListFont != NULL )
			DeleteObject( StatusListFont );

		LPCTSTR fontName;
		int size = (long)params[1];

		if ( size < 4 ) size = 10;
		if ( params.GetSize() > 2 )
			fontName = params[2];
		else
#ifdef DESKTOP
			fontName = L"System";
#else
			fontName = L"Tahoma";
#endif

		LOGFONT lf;
		lf.lfHeight = size * res / 72;
		lf.lfWidth  = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = OEM_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		wcsncpy( lf.lfFaceName, (LPCTSTR)fontName, 32 );

		StatusListFont = ::CreateFontIndirect( &lf );
	}

	if ( StatusListFont != NULL && StatusWindow != NULL )
		SendMessage( GetDlgItem( StatusWindow, IDC_ENTRIES ), WM_SETFONT, (WPARAM)StatusListFont, MAKELPARAM(1,0) );

	if ( StatusListItemHeight != 0 && StatusWindow != NULL )
		SendMessage( GetDlgItem( StatusWindow, IDC_ENTRIES ), LB_SETITEMHEIGHT, 0, MAKELPARAM(StatusListItemHeight,0) );

	return TRUE;
}

BOOL CmdWriteStatusHistory( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'WriteStatusHistory'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	BOOL append = FALSE;
	int cp = CP_ACP;

	if ( params.GetSize() == 2 && (long)params.GetAt(1) != 0 )
		append = TRUE;
	if ( params.GetSize() == 3 )
		cp = GetCodePage( params.GetAt(2) );

	//SendMessage( GetMsgParent(), WM_FETCH_STATUS_LINES, NULL, (LPARAM)(LPCTSTR)params[0] );
	
	CStr contents;
	for ( int i=0; i < StatusDialog->m_Strings.GetSize(); i++ )
	{
		contents += StatusDialog->m_Strings.GetAt(i);
		contents += L"\r\n";
	}
	
	CIniFile::WriteFile( params[0], contents, append, cp );

	return TRUE;
}

