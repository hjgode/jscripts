#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "variables.h"
#include "interpreter.h"

#ifndef SMARTPHONE
#include "dlgwait.h"
#include "dlginput.h"
#include "dlgseldir.h"
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#include "dlgchoice.h"
#else
#include "smartphone/dlgwait.h"
#include "smartphone/dlginput.h"
#include "smartphone/dlgchoice.h"
#include "smartphone/dlgseldir.h"
#include "smartphone/dlgselfile.h"
extern HINSTANCE g_hInst;
#endif

#ifdef POCKETPC
#include "dlgselfile.h"
#else
#include "commdlg.h"
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

extern CInterpreter *CurrentInterpreter;

#include "ValueArray.h"
#include "FunctionsData.h"
#include "Interpreter.h"


/*
CValue  FctQuestion( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 1 || params.GetSize() > 3 )
    {
        errorMessage = InvalidParameterCount + L"'Question'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = 0L;
    
	CStr title = L"JScripts";
    if ( params.GetSize() >= 2 )
        title = (CStr)params[1];

    int type = MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND;
    if ( params.GetSize() >= 3 )
	{
		if ( ((CStr)params[2]).CompareNoCase( L"YesNoCancel" ) == 0 )
	        type = MB_YESNOCANCEL|MB_ICONQUESTION|MB_SETFOREGROUND;
		else if ( ((CStr)params[2]).CompareNoCase( L"OkCancel" ) == 0 )
	        type = MB_OKCANCEL|MB_ICONQUESTION|MB_SETFOREGROUND;
		else if ( ((CStr)params[2]).CompareNoCase( L"RetryCancel" ) == 0 )
	        type = MB_RETRYCANCEL|MB_ICONQUESTION|MB_SETFOREGROUND;
		else if ( ((CStr)params[2]).CompareNoCase( L"YesNo" ) != 0 )
		{
			errorMessage = L"Invalid query type '" + (CStr)params[2] + L"' for Question";
			error = 9;
			return CValue();
		}
	}

    int aw = ::MessageBox( NULL, (LPCTSTR)params[0], title, type );
    if ( aw == IDYES || aw == IDOK || aw == IDRETRY )
        rc = 1L;
    if ( aw == IDNO )
        rc = 0L;
    if ( aw == IDCANCEL )
        rc = 2L;

    return rc;
}
*/
//BOOL CmdSleepMessage( CInterpreter &interpreter, CStr &param )
CValue  FctQuestion( CValueArray &params, int &error, CStr &errorMessage )
{
    CDlgWait dlg(NULL);

    if ( params.GetSize() < 1 || params.GetSize() > 7 )
    {
        errorMessage = InvalidParameterCount + L"'Question'";
        error = 9;
        return CValue();
    }
	BOOL show = TRUE;
	//int ParamCountCtrl=0;
	int ParamIdx = 0;
	if ((params[ParamIdx]).IsValidNumber())
		dlg.Countdown = (long)params[ParamIdx++];
	dlg.m_Label   = (CStr)params[ParamIdx++];

	if (params.GetSize()>=ParamIdx+2 && (params[ParamIdx]).IsValidNumber() && (params[ParamIdx+1]).IsValidNumber()){
		dlg.wWidth = (long)params[ParamIdx++];
		dlg.wHeight = (long)params[ParamIdx++];
	}

	if (params.GetSize()>=ParamIdx+1){
		dlg.Title       = (CStr)params[ParamIdx++];
		dlg.Title.TrimLeft();
	}

	if (params.GetSize()>=ParamIdx+1)
	    dlg.AllowOK     = (long)params[ParamIdx++];

	if (params.GetSize()>=ParamIdx+1)
	{
		show = (long)params[ParamIdx++];

	}
    if ( show ) dlg.DoModal();

    return dlg.Result;
}

CValue  FctChoice( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 5 )
    {
        errorMessage = InvalidParameterCount + L"'Choice'";
        error = 9;
        return CValue();
    }

    CDlgChoice choiceDlg;
    choiceDlg.m_Title   = (CStr)params[0];
    choiceDlg.m_Info    = (CStr)params[1];
    choiceDlg.m_Default = (long)params[2]-1;
    choiceDlg.m_Timeout = (long)params[3];

    CValue *mapValue = NULL;

    if ( params.GetSize() == 5 )
    {
        switch( params[4].GetType() )
        {
        case VALUE_STRING:
            mapValue = GetVariable( params[4], FALSE );
            break;

        case VALUE_MAP:
            mapValue = &params[4];
            break;
        }
    }

    if ( mapValue != NULL && mapValue->GetType() == VALUE_MAP )
    {
        CMapStrToValue *map = mapValue->GetMap();
        CStr elem; CValue *cont;

        for ( int i=1; i<50; i++ )
        {
            elem.Format( L"%d", i );
            map->Lookup( elem, cont );
            if ( cont != NULL && !cont->IsNull() )
                choiceDlg.m_Strings.Add( *cont );
            else
                break;
        }
    }
    else
    {
        for ( int i=4; i<params.GetSize(); i++ )
        {
            choiceDlg.m_Strings.Add( (LPCTSTR)params[i] );
        }
    }

    choiceDlg.DoModal();

    CValue rc;
    rc = (long)choiceDlg.m_Selected;

    return rc;
}

//#ifndef SMARTPHONE
CValue  FctInput( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 1 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'Input'";
        error = 9;
        return CValue();
    }

    CDlgInput dlg( GetMsgParent() );
    dlg.Multiline = (params.GetSize()>=4) ? (long)params[3] : 0;
    dlg.Numeric = (params.GetSize()>=3) ? (long)params[2] : 0;
    dlg.m_Label = (LPCTSTR)params[0];
    dlg.Title   = (params.GetSize()>=2) ? (LPCTSTR)params[1] : L"JScripts";
    dlg.m_Edit  = (params.GetSize()>=5) ? (LPCTSTR)params[4] : L"";
    dlg.DoModal();

    CValue rc;
    rc = dlg.m_Edit;
    return rc;
}

CValue  FctSelectDirectory( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 2 || params.GetSize() > 3 )
    {
        errorMessage = InvalidParameterCount + L"'SelectDirectory'";
        error = 9;
        return CValue();
    }

    CDlgSelDir dlg;
    dlg.m_Title   = (params.GetSize()>=1) ? (LPCTSTR)params[0] : L"JScripts";
    dlg.m_Info  = (params.GetSize()>=2) ? (LPCTSTR)params[1] : L"";
    dlg.m_Selected = (params.GetSize()>=3) ? (LPCTSTR)params[2] : L"";
    dlg.DoModal();

    CValue rc;
    rc = dlg.m_Selected;
    return rc;
}
//#endif

#ifndef DESKTOP
CValue  FctSelectFile( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 2 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'SelectFile'";
        error = 9;
        return CValue();
    }

    CDlgSelFile dlg;
    dlg.m_Title    = (params.GetSize()>=1) ? (LPCTSTR)params[0] : L"JScripts";
    dlg.Save       = (params.GetSize()>=2) ? (long)(params[1]) : 0;
    dlg.m_Filter   = (params.GetSize()>=3) ? (LPCTSTR)params[2] : L"*.*";
    dlg.m_Info     = (params.GetSize()>=4) ? (LPCTSTR)params[3] : L"";
    dlg.m_Selected = (params.GetSize()>=5) ? (LPCTSTR)params[4] : L"";
    dlg.DoModal();

    CValue rc;
    rc = dlg.m_Selected;
    return rc;
}
#else
CValue  FctSelectFile( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 2 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'SelectFile'";
        error = 9;
        return CValue();
    }

    LPTSTR filter = NULL;
    if ( params.GetSize() >= 3 )
    {
        int fltLen = ((CStr)params[2]).GetLength();
        int infoLen = fltLen;
        if ( params.GetSize() >= 4 )
            infoLen = ((CStr)params[3]).GetLength();
        filter = new TCHAR[fltLen + infoLen + 3];
        if ( params.GetSize() >= 4 )
            wcscpy( filter, (LPCTSTR)params[3] );
        else
            wcscpy( filter, (LPCTSTR)params[2] );
        filter[infoLen] = 0;
        wcscpy( filter+infoLen+1, (LPCTSTR)params[2] );
        filter[fltLen+infoLen+1] = 0;
        filter[fltLen+infoLen+2] = 0;
    }

    LPTSTR filename = new TCHAR[MAX_PATH];
    if ( params.GetSize() >= 5 )
        wcscpy( filename, (LPCTSTR)params[4] );
    else
        filename[0] = 0;

    BOOL  save       = (params.GetSize()>=2) ? (long)(params[1]) : 0;
    DWORD flags = save ? OFN_HIDEREADONLY|OFN_PATHMUSTEXIST : OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

    OPENFILENAME ofn;
    ofn.lStructSize     = sizeof(OPENFILENAME);
#ifdef DESKTOP
    ofn.hwndOwner       = NULL;
#else
    HWND topWnd = ::CreateWindow( L"FileSelection", (params.GetSize()>=1) ? (LPCTSTR)params[0] : L"JScripts", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, g_hInst, NULL);
    ::ShowWindow( topWnd, SW_SHOW );
    ::SetForegroundWindow( topWnd );
    ofn.hwndOwner       = topWnd;
#endif
    ofn.hInstance       = NULL;
    ofn.lpstrFilter     = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter  = 0;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle      = (params.GetSize()>=1) ? (LPCTSTR)params[0] : L"JScripts";
    ofn.Flags           = flags;
    ofn.nFileOffset     = 0;
    ofn.nFileExtension  = 0;
    ofn.lpstrDefExt     = 0;
    ofn.lCustData       = 0;
    ofn.lpfnHook        = NULL;
    ofn.lpTemplateName  = NULL;


    CValue selFile;
    BOOL rc;
    if ( save )
        rc = GetSaveFileName( &ofn );
    else
        rc = GetOpenFileName( &ofn );

    if ( rc == FALSE )
        selFile.Clear();
    else
        selFile = filename;

#ifndef DESKTOP
    DestroyWindow( topWnd );
#endif

    delete[] filter;
    delete[] filename;

    return selFile;
}

#endif