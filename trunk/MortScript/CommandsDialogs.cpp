#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "variables.h"
#include "interpreter.h"
#include "wingdi.h"

#ifndef SMARTPHONE
#include "dlginput.h"
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#include "DlgBigMessage.h"
#include "DlgWait.h"
#else
#include "smartphone\DlgInput.h"
#include "smartphone\DlgBigMessage.h"
#include "smartphone\DlgWait.h"
extern HINSTANCE g_hInst;
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "Interpreter.h"
#include "CommandsDialogs.h"

int   ChoiceItemHeight = 0;
HFONT ChoiceFont = NULL;

HFONT MsgFont = NULL;

BOOL CmdMessage( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
	interpreter.Split( param, L',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Message'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }
	CStr tStr=params.GetAt(0);
    MessageBox( GetMsgParent(), (LPCTSTR)params.GetAt(0)
              , params.GetSize() == 2 ? (LPCTSTR)params.GetAt(1) : L"JScripts"
              , MB_OK|MB_SETFOREGROUND|MB_SETFOREGROUND );
	
    return rc;
}

BOOL CmdBigMessage( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, L',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'BigMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CDlgBigMessage dlg;
	dlg.m_Text = (CStr)params.GetAt(0);
	if ( params.GetSize() == 2 )
		dlg.Title = (CStr)params.GetAt(1);
	else
		dlg.Title = L"JScripts";

    dlg.DoModal();

    return rc;
}

BOOL CmdInput( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params, 1, 1 );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Input'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CDlgInput dlg( GetMsgParent() );
    dlg.Numeric = (long)params[1];
    dlg.m_Label = (CStr)params[2];
    if ( params.GetSize() == 4 )
        dlg.Title = (CStr)params[3];
    dlg.DoModal();

    return interpreter.SetVariable( params[0], dlg.m_Edit );
}

BOOL CmdSleepMessage( CInterpreter &interpreter, CStr &param )
{
    CDlgWait dlg( GetMsgParent() );
    CValueArray params;
    interpreter.Split( param, L',', params, 1, (1<<4) );
    if ( params.GetSize() < 2 || params.GetSize() > 7 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SleepMessage'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	BOOL show = TRUE;
	int ParamCountCtrl=2;
	int ParamIdx = 2;
    dlg.Countdown = (long)params[0];
    dlg.m_Label   = (CStr)params[1];

    if ( params.GetSize() >= 4 && params[2].IsValidNumber() && params[3].IsValidNumber() ){
		dlg.wWidth = (long)params[2];
		dlg.wHeight = (long)params[3];
		ParamCountCtrl += 2;
		ParamIdx += 2;
	}

    if ( params.GetSize() >= ParamCountCtrl+1 ){
		dlg.Title       = (CStr)params[ParamIdx];
		dlg.Title.TrimLeft();dlg.Title.TrimRight();
		if (dlg.Title.StartsWith(L"\"") && dlg.Title.EndsWith(L"\""))
			dlg.Title = dlg.Title.Mid(1,dlg.Title.GetLength()-2);
	}

	//dlg.Title.TrimLeft('"');
	//dlg.Title.TrimRight('"');

    if ( params.GetSize() >= ParamCountCtrl+2 )
	    dlg.AllowOK     = (long)params[ParamIdx+1];

    if ( params.GetSize() == ParamCountCtrl+3 )
	{
		dlg.Expression = (CStr)params[ParamIdx+2];

		show = ! (long)interpreter.EvaluateExpression( (CStr)dlg.Expression, true );
        if ( interpreter.GetError() != 0 )
        {
            if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), interpreter.GetErrorMessage() + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            return FALSE;
        }
	}
    if ( show ) dlg.DoModal();

    return TRUE;
}



BOOL CmdSetChoiceEntryFormat( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetChoiceEntryFormat'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HDC dc = GetDC( ::GetDesktopWindow() );
	int res = GetDeviceCaps( dc, LOGPIXELSY );
	ReleaseDC( ::GetDesktopWindow(), dc );

	ChoiceItemHeight = (long)params[0];
#ifndef DESKTOP
	if ( res == 192 )
		ChoiceItemHeight *= 2;
#endif

	if ( params.GetSize() > 1 )
	{
		if ( ChoiceFont != NULL )
			DeleteObject( ChoiceFont );

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

		ChoiceFont = ::CreateFontIndirect( &lf );
		/*
		size * GetDeviceCaps( dc, LOGPIXELSY) / 72, // nHeight
								 0,                         // nWidth
								 0,                         // nEscapement
								 0,                         // nOrientation
								 FW_NORMAL,					// nWeight
								 0,							// bItalic
								 FALSE,                     // bUnderline
								 0,                         // cStrikeOut
								 OEM_CHARSET,               // nCharSet
								 OUT_DEFAULT_PRECIS,        // nOutPrecision
								 CLIP_DEFAULT_PRECIS,       // nClipPrecision
								 DEFAULT_QUALITY,           // nQuality
								 DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
								 fontName );
		*/
	}

	return TRUE;
}

BOOL CmdSetMessageFont( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params );
    if ( params.GetSize() < 1 || params.GetSize() > 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetMessageFont'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	if ( MsgFont != NULL )
		DeleteObject( MsgFont );

	LPCTSTR fontName;
	int size = (long)params[0];

	if ( size < 4 ) size = 10;
	if ( params.GetSize() > 1 )
		fontName = params[2];
	else
#ifdef DESKTOP
		fontName = L"System";
#else
		fontName = L"Tahoma";
#endif


	HDC dc = GetDC( ::GetDesktopWindow() );
	LOGFONT lf;
	lf.lfHeight = size * GetDeviceCaps( dc, LOGPIXELSY) / 72;
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

	MsgFont = ::CreateFontIndirect( &lf );

	ReleaseDC( ::GetDesktopWindow(), dc );

	return TRUE;
}
