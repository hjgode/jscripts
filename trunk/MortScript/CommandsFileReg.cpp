#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "variables.h"
#include "interpreter.h"
#include "IniFile.h"
#include <winioctl.h>

#ifndef SMARTPHONE
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#include "DlgDownload.h"
#else
extern HINSTANCE g_hInst;
#endif
extern HANDLE ScriptAborted;

#ifndef DESKTOP
const GUID IID_ConnPrv_IProxyExtension = 
 { 0xaf96b0bd, 0xa481, 0x482c, { 0xa0, 0x94, 0xa8, 0x44, 0x87, 0x67, 0xa0, 0xc0 } };
#include "connmgr.h"
#include <connmgr_proxy.h>
#include "ras.h"
#include "raserror.h"
#include "notify.h"
#endif


#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"

// CreateLink - uses the shell's IShellLink and IPersistFile interfaces 
//   to create and store a shortcut to the specified object. 
// Returns the result of calling the member functions of the interfaces. 
// lpszPathObj - address of a buffer containing the path of the object 
// lpszPathLink - address of a buffer containing the path where the 
//   shell link is to be stored 
// lpszDesc - address of a buffer containing the description of the 
//   shell link 
HRESULT CreateLink(LPCTSTR lpszPathObj, LPTSTR lpszPathLink, LPTSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    CoInitialize( NULL );

    hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<void**>(&psl) ); 
    if (SUCCEEDED(hres)) { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target, and add the 
        // description. 
        psl->SetPath(lpszPathObj); 

        psl->SetDescription(lpszDesc); 
 
       // Query IShellLink for the IPersistFile interface for saving the 
       // shortcut in persistent storage. 
        hres = psl->QueryInterface( IID_IPersistFile, reinterpret_cast<void**>(&ppf) ); 
 
        if (SUCCEEDED(hres)) { 
            //WORD wsz[MAX_PATH]; 
 
            // Ensure that the string is ANSI. 
#ifndef UNICODE
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
#endif

            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(lpszPathLink, TRUE); 
            ppf->Release(); 
        } 
        psl->Release(); 
    } 

    CoUninitialize();

    return hres; 
} 
#endif

#include "Interpreter.h"
#include "CommandsFileReg.h"

#ifdef DESKTOP
BOOL CmdChDir( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ChDir'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    if ( ( attribs == -1 || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0 ) )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Directory '%s' does not exist", (LPCTSTR)params.GetAt(0) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }
	else
		_wchdir( (LPCTSTR)params.GetAt(0) );

    return rc;
}
#endif

BOOL CmdMkDir( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'MkDir'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    if (   ( attribs == -1 || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0 )
        && CreateDirectory( params.GetAt(0), NULL ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't create directory '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdRmDir( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RmDir'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    if (   attribs != -1 && (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0
        && RemoveDirectory( params.GetAt(0) ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't remove directory '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdDeleteIntern( CInterpreter &interpreter, CStr &param, BOOL recursive )
{
    BOOL rc = TRUE;
    CValueArray params;
	CStr			path;
	WIN32_FIND_DATA		findFileData;
    CStrArray    directories;

    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Delete'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    path = ((CStr)params[0]).Left(((CStr)params[0]).ReverseFind('\\')+1);
	//MessageBox( GetMsgParent(), path, L"Path", MB_SETFOREGROUND );
	HANDLE ffh = FindFirstFile( params[0], &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
        do
        {
            CStr file = path+findFileData.cFileName;
            if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
            {
                if ( DeleteFile( file ) == 0 )
                {
                    if ( interpreter.ErrorLevel >= ERROR_WARN )
                    {
                        CStr msg;
                        msg.Format( L"Couldn't delete '%s'", (LPCTSTR)file );
                        MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    }
                }
            }
            else
            {
                if ( recursive && wcscmp( findFileData.cFileName, L"." ) != 0 && wcscmp( findFileData.cFileName, L".." ) != 0 )
                {
                    directories.Add( file );
					// MessageBox( GetMsgParent(), L"Added directory " + file, L"Debug", MB_SETFOREGROUND );
                }
            }
        }
		while ( FindNextFile( ffh, &findFileData ) == TRUE );

		FindClose( ffh );
	}

    if ( recursive )
    {
		BOOL noFilter = FALSE;
        CStr filter, pathAndFilter, dir;
        dir    = ((CStr)params.GetAt(0)).Left( ((CStr)params.GetAt(0)).ReverseFind( '\\' ) );
        filter = ((CStr)params.GetAt(0)).Mid( ((CStr)params.GetAt(0)).ReverseFind( '\\' ) );

        for ( int i=0; i<directories.GetSize(); i++ )
        {
            pathAndFilter = L"\"" + directories.GetAt(i) + filter + L"\"";

			// Path without file filter was given?
			if ( directories.GetAt(i).CompareNoCase( (CStr)params.GetAt(0) ) == 0 )
			{
				noFilter = TRUE;
	            pathAndFilter = L"\"" + directories.GetAt(i) + L"\\*.*\"";
				// MessageBox( GetMsgParent(), pathAndFilter, L"Debug", MB_SETFOREGROUND );
			}

            CmdDeleteIntern( interpreter, pathAndFilter, TRUE );
        }

		if ( ! noFilter )
	        RemoveDirectory( dir );
    }

    return rc;
}

BOOL CmdDelete( CInterpreter &interpreter, CStr &param )
{
	return CmdDeleteIntern( interpreter, param, FALSE );
}

BOOL CmdDelTree( CInterpreter &interpreter, CStr &param )
{
	return CmdDeleteIntern( interpreter, param, TRUE );
}

BOOL CmdSetProxy( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetProxy'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    Proxy = (CStr)params[0];

    return TRUE;
}

#ifndef SMARTPHONE
BOOL CmdDownload( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Download'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CDlgDownload dlg;
    dlg.Source = (LPCTSTR)params[0];
    dlg.Target = (LPCTSTR)params[1];
    dlg.DoModal();

	if ( dlg.Result != 0 )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            switch( dlg.Result )
            {
            case -1:
                msg.Format( L"Couldn't open connection to internet" );
                break;

            case -2:
            case 404:
                msg.Format( L"'%s' couldn't be opened", (LPCTSTR)params[0] );
                break;

            case -3:
                msg.Format( L"'%s' is too big", (LPCTSTR)params[0] );
                break;

            case -4:
                msg.Format( L"Not enough memory to read file '%s'", (LPCTSTR)params[0] );
                break;

            case -5:
                msg.Format( L"Error reading '%s'", (LPCTSTR)params[0] );
                break;

            case -6:
                msg.Format( L"'%s' couldn't be opened", (LPCTSTR)params[1] );
                break;

            case -7:
                msg.Format( L"Download was cancelled" );
                DeleteFile( params[1] );
                break;

            default:
                msg.Format( L"'%s' couldn't be opened (http error %d)", (LPCTSTR)params[0], dlg.Result );
            }

            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}
#endif

BOOL CmdCopy( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Copy'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL failIfExists = ( params.GetSize() == 2 ) ? TRUE : ( (long)params.GetAt(2) == 0 );

	if ( CopyFile( params.GetAt(0), params.GetAt(1), failIfExists ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't copy '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdRename( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Rename'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }


    if ( params.GetSize() == 3 && (long)params.GetAt(2) )
    {
#ifndef SMARTPHONE
        CFileStatus srcStatus, destStatus;
        if (    CFile::GetStatus( params.GetAt(0), srcStatus )
             && CFile::GetStatus( params.GetAt(1), destStatus )
             && srcStatus.m_attribute != -1 // does exist
             && (srcStatus.m_attribute & CFile::readOnly  ) == 0
             //&& (srcStatus.m_attribute & CFile::system    ) == 0
             && destStatus.m_attribute != -1 // does exist
             && (destStatus.m_attribute & CFile::readOnly  ) == 0
             //&& (destStatus.m_attribute & CFile::system    ) == 0
             && (destStatus.m_attribute & CFile::directory ) == 0
           )
        {
#endif
            DeleteFile( params.GetAt(1) );
#ifndef SMARTPHONE
        }
#endif
    }

	if ( MoveFile( params.GetAt(0), params.GetAt(1) ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't rename/move '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdRecursiveCopyMove( CInterpreter &interpreter, CStr &path, CStr &filter, CStr &target, BOOL failIfExists, BOOL rec, BOOL move )
{
	BOOL rc = TRUE;

	CStr search = path + filter;

	CStrArray subDirs;
	WIN32_FIND_DATA findFileData;
	//MessageBox( GetMsgParent(), search, L"Debug", MB_OK );
	HANDLE ffh = FindFirstFile( search, &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
		DWORD attribs = GetFileAttributes( target );
		if (   ( attribs == -1 || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			&& CreateDirectory( target, NULL ) == FALSE )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
			{
				CStr msg;
				msg.Format( L"Couldn't create directory '%s'", (LPCTSTR)target );
				MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
				rc = FALSE;
			}
		}

		if ( rc )
		{
			do
			{
				if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					if ( rec && wcscmp( findFileData.cFileName, L"." ) != 0 && wcscmp( findFileData.cFileName, L".." ) != 0)
						subDirs.Add( findFileData.cFileName );
				}
				else
				{
					CStr source = path+findFileData.cFileName;
					CStr targetFile = target;
					if ( targetFile.Right(1) != L"\\" ) targetFile += L"\\";
					targetFile += findFileData.cFileName;
					if ( move && !failIfExists )
					{
						if (   ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) == 0
							&& ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) == 0
						   )
						{
							DeleteFile( targetFile );
						}
					}
					BOOL success;
					if ( move )
						success = MoveFile( source, targetFile );
					else
						success = CopyFile( source, targetFile, failIfExists );
					if ( success == FALSE )
					{
						if ( interpreter.ErrorLevel >= ERROR_WARN )
						{
							CStr msg;
							msg.Format( L"Couldn't %s '%s' to '%s'", (move)?L"move":L"copy", (LPCTSTR)source, (LPCTSTR)targetFile );
							MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
						}
					}
				}
			}
			while ( FindNextFile( ffh, &findFileData ) == TRUE && WaitForSingleObject( ScriptAborted,0 ) != WAIT_OBJECT_0 );
		}

		FindClose( ffh );
	}

	if ( rc && subDirs.GetSize() > 0 )
	{
		CStr subSource, subTarget;
		for ( int i=0; rc && i<subDirs.GetSize(); i++ )
		{
			subSource = path + subDirs.GetAt(i) + L"\\";
			subTarget = target;
			if ( subTarget.Right(1) != L"\\" ) subTarget += L"\\";
			subTarget += subDirs.GetAt(i);

			rc = CmdRecursiveCopyMove( interpreter, subSource, filter, subTarget, failIfExists, rec, move );
		}
	}

	return rc;
}

BOOL CmdXCopy( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray		params;
	CStr		path, filter;

    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'XCopy'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL failIfExists = ( params.GetSize() == 2 ) ? TRUE : ( (long)params.GetAt(2) == 0 );

	int lastBackSlash = ((CStr)params[0]).ReverseFind('\\');
    path   = ((CStr)params[0]).Left(lastBackSlash+1);
    filter = ((CStr)params[0]).Mid(lastBackSlash+1);
	BOOL rec = FALSE;
	if ( params.GetSize() >= 4 ) rec = (long)params.GetAt(3);

	CmdRecursiveCopyMove( interpreter, path, filter, params[1], failIfExists, rec, FALSE );

    return rc;
}

BOOL CmdMove( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray     params;
	CStr		path, filter;

    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Move'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL failIfExists = ( params.GetSize() == 2 ) ? TRUE : ( (long)params.GetAt(2) == 0 );

	int lastBackSlash = ((CStr)params[0]).ReverseFind('\\');
    path   = ((CStr)params[0]).Left(lastBackSlash+1);
    filter = ((CStr)params[0]).Mid(lastBackSlash+1);
	BOOL rec = FALSE;
	if ( params.GetSize() >= 4 ) rec = (long)params.GetAt(3);

	CmdRecursiveCopyMove( interpreter, path, filter, params[1], failIfExists, rec, TRUE );

    return rc;
}

BOOL CmdCreateShortcut( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
	interpreter.Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'CreateShortcut'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }


    if ( params.GetSize() == 3 && (long)params.GetAt(2) )
    {
#ifndef SMARTPHONE
        CFileStatus status;
        if (    CFile::GetStatus( params.GetAt(1), status )
             && status.m_attribute != -1 // does exist
             && (status.m_attribute & CFile::readOnly  ) == 0
             && (status.m_attribute & CFile::system    ) == 0
             && (status.m_attribute & CFile::directory ) == 0
           )
        {
#endif
        DeleteFile( params.GetAt(0) );
#ifndef SMARTPHONE
        }
#endif
    }

#ifndef DESKTOP
	CStr tStr=params.GetAt(1);
	tStr.Format(L"%u#%s",tStr.GetLength(),(LPCTSTR)tStr);

	if ( CIniFile::WriteFile( (LPCTSTR)params.GetAt(0), tStr) == FALSE ) 
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't create shortcut '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }
#else
    if ( CreateLink( (LPCTSTR)params.GetAt(1), (LPTSTR)(LPCTSTR)params.GetAt(0), L"") != 0 )
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't create shortcut '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }
#endif

    return rc;
}

BOOL CmdGetMortScriptVersion( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params, 1, 0x0f ); // 1111
    if ( params.GetSize() != 1 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetMortScriptVersion'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( params.GetSize() == 1 )
    {
        CStr ver;
		ver = VERSION;
	    rc = interpreter.SetVariable( params[0], ver );
    }
    else
    {
		CStrArray parts;
		CIniFile::Split( CStr(VERSION), L".", parts );

	    rc = interpreter.SetVariable( params[0], parts[0] );
	    rc = interpreter.SetVariable( params[1], parts[1] );
	    rc = interpreter.SetVariable( params[2], parts[2] );
	    rc = interpreter.SetVariable( params[3], parts[3] );
    }

    return rc;
}


BOOL CmdGetVersion( CInterpreter &interpreter, CStr &param )
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

    DWORD size, dummy;
    size = GetFileVersionInfoSize( (LPTSTR)(LPCTSTR)params[0], &dummy );
    if ( size == 0 )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't get version info for '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    void *verData = malloc( size );
    GetFileVersionInfo( (LPTSTR)(LPCTSTR)params[0], NULL, size, verData );
    void *fileInfo;
    UINT size2;
    VerQueryValue( verData, L"\\", &fileInfo, &size2 );
    VS_FIXEDFILEINFO *ffi = (VS_FIXEDFILEINFO*)fileInfo;
    DWORD ms = ffi->dwFileVersionMS;
    DWORD ls = ffi->dwFileVersionLS;

    if ( params.GetSize() == 2 )
    {
        CStr ver;
        ver.Format( L"%d.%d.%d.%d", HIWORD(ms), LOWORD(ms), HIWORD(ls), LOWORD(ls) );
	    rc = interpreter.SetVariable( params[1], ver );
    }
    else
    {
        CValue ver;
        ver = (long)HIWORD(ms);
	    rc = interpreter.SetVariable( params[1], ver );

        ver = (long)LOWORD(ms);
	    rc &= interpreter.SetVariable( params[2], ver );

        ver = (long)HIWORD(ls);
	    rc &= interpreter.SetVariable( params[3], ver );

        ver = (long)LOWORD(ls);
	    rc &= interpreter.SetVariable( params[4], ver );
    }

    return rc;
}

BOOL CmdSetFileAttribs( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetFileAttribs'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	DWORD attribs = GetFileAttributes( params.GetAt(0) );
	if ( attribs != 0xFFFFFFFF )
	{
		if ( (long)params[1] == 1 )
			attribs |= FILE_ATTRIBUTE_READONLY;

		if ( (long)params[1] == 0 )
			attribs &= ~FILE_ATTRIBUTE_READONLY;

		if ( params.GetSize() >= 3 && (long)params[2] == 1 )
			attribs |= FILE_ATTRIBUTE_HIDDEN;

		if ( params.GetSize() >= 3 && (long)params[2] == 0 )
			attribs &= ~FILE_ATTRIBUTE_HIDDEN;

		if ( params.GetSize() >= 4 && (long)params[3] == 1 )
			attribs |= FILE_ATTRIBUTE_ARCHIVE;

		if ( params.GetSize() >= 4 && (long)params[3] == 0 )
			attribs &= ~FILE_ATTRIBUTE_ARCHIVE;

		SetFileAttributes( params.GetAt(0), attribs );
	}

	return TRUE;
}

BOOL CmdSetFileAttribute( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetFileAttribute'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	DWORD attribs = GetFileAttributes( params.GetAt(0) );
	if ( attribs != 0xFFFFFFFF )
	{
		int compAttr = 0;
		//if ( params.GetAt(1).CompareNoCase( L"directory" ) == 0 )
		//	compAttr = FILE_ATTRIBUTE_DIRECTORY;
		if ( ((CStr)params.GetAt(1)).CompareNoCase( L"hidden" ) == 0 )
			compAttr = FILE_ATTRIBUTE_HIDDEN;
		if ( ((CStr)params.GetAt(1)).CompareNoCase( L"readonly" ) == 0 )
			compAttr = FILE_ATTRIBUTE_READONLY;
		if ( ((CStr)params.GetAt(1)).CompareNoCase( L"system" ) == 0 )
			compAttr = FILE_ATTRIBUTE_SYSTEM;
		if ( ((CStr)params.GetAt(1)).CompareNoCase( L"archive" ) == 0 )
			compAttr = FILE_ATTRIBUTE_ARCHIVE;

		if ( compAttr == 0 )
		{
	        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
	            MessageBox( GetMsgParent(), L"Invalid attribute '" + (CStr)params.GetAt(1) + L"' for 'SetFileAttribute'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}

		if ( (long)params[2] != 0 )
			attribs |= compAttr;
		else
			attribs &= ~compAttr;

		SetFileAttributes( (LPCTSTR)params.GetAt(0), attribs );
	}

	return TRUE;
}

BOOL CmdZipFile( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ZipFile'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    int compLevel = 8;
    if ( params.GetSize() == 4 ) compLevel = (long)params.GetAt(3);

    if ( !OpenZipDll() )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Error opening ZIP library", L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // 0=source, 1=zip, 2=file in zip
	if ( ZipFromFile( params.GetAt(1), params.GetAt(2), params.GetAt(0), compLevel ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't zip '%s' to '%s' in '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(2), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdZipFiles( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 5 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ZipFiles'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	int recursive = 0;
    int compLevel = 8;
    CStr zipPath = L"";
    if ( params.GetSize() >= 3 ) recursive = (long)params.GetAt(2);
    if ( params.GetSize() >= 4 ) zipPath = (CStr)params.GetAt(3);
    if ( params.GetSize() == 5 ) compLevel = (long)params.GetAt(4);

    if ( !OpenZipDll() )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Error opening ZIP library", L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // 0=source, 1=zip, 2=recursive, 3=rate
	if ( ZipFromFilesToZipPath( params.GetAt(1), params.GetAt(0), zipPath, recursive, compLevel ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't zip '%s' to '%s' in '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(2), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdUnzipFile( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'UnzipFile'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( !OpenZipDll() )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Error opening ZIP library", L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // 0=zip, 1=file in zip, 2=target
	if ( UnzipToFile( params.GetAt(0), params.GetAt(1), params.GetAt(2) ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't unzip '%s' in '%s' to '%s'", (LPCTSTR)params.GetAt(1), (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(2) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdUnzipAll( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'UnzipAll'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }


    if ( !OpenZipDll() )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Error opening ZIP library", L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // 0=source, 1=zip, 2=file in zip
	if ( UnzipAllToPath( params.GetAt(0), params.GetAt(1) ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't unzip '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CmdUnzipPath( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, ',', params );
    if ( params.GetSize() != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'UnzipPath'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }


    if ( !OpenZipDll() )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Error opening ZIP library", L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // 0=source, 1=zip, 2=file in zip
	if ( UnzipPathToPath( params.GetAt(0), params.GetAt(1), params.GetAt(2) ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't unzip path '%s' in '%s' to '%s'", (LPCTSTR)params.GetAt(1), (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(2) );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}


BOOL CmdRegWriteString( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegWriteString'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    if ( RegWrite( root, params[1], params[2], params[3] ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdRegWriteDWord( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegWriteDWord'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    if ( RegWriteDW( root, params[1], params[2], (long)params[3] ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CmdRegWriteBinary( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegWriteBinary'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    UINT    length = ((CStr)params[3]).GetLength() / 2;
    BYTE    *data = new BYTE[length];

    for ( int i=0; (UINT)i<length; i++ )
    {
        if ( swscanf( ((CStr)params[3]).Mid(i*2,2), L"%X", &(data[i]) ) != 1 )
        {
            if ( interpreter.ErrorLevel >= ERROR_ERROR )
            {
                MessageBox( GetMsgParent(), L"Invalid data for RegWriteBinary" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            rc = FALSE;
        }
    }

    if ( rc )
    {
        BOOL    result = FALSE;
	    HKEY    key;
	    DWORD   type;

	    if ( RegCreateKeyEx( root, params[1], 0, 0, 0
#ifndef DESKTOP
                          , 0
#else
                          , KEY_WRITE
#endif
                          , NULL, &key, &type ) == ERROR_SUCCESS )
	    {
		    if ( RegSetValueEx( key, params[2], NULL, REG_BINARY, (BYTE*)data, length ) != ERROR_SUCCESS )
		    {
                if ( interpreter.ErrorLevel >= ERROR_ERROR )
                {
                    CStr msg;
                    msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
                    MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    rc = FALSE;
                }
		    }
		    RegCloseKey( key );
	    }
    }

    delete[] data;

    return rc;
}

BOOL CmdRegWriteMultiString( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegWriteString'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

	LPTSTR data = NULL;
	int    dataLength;
	if ( params[3].GetType() == VALUE_MAP )
	{
		CMapStrToValue *map = params[3].GetMap();
		CStr elem;
		CValue      *cont;
        for ( int i=1; i<50; i++ )
        {
            elem.Format( L"%d", i );
            map->Lookup( elem, cont );
            if ( cont != NULL && !cont->IsNull() )
			{
				if ( data == NULL )
				{
					dataLength = ((CStr)*cont).GetLength() + 2;
					data = (LPTSTR)malloc( dataLength * sizeof(TCHAR) );
					wcscpy( data, (LPCTSTR)*cont );
				}
				else
				{
					int start = dataLength-1;
					dataLength += ((CStr)*cont).GetLength() + 1;
					data = (LPTSTR)realloc( data, dataLength * sizeof(TCHAR) );
					wcscpy( data+start, (LPCTSTR)*cont );
				}
			}
            else
                break;
        }
		if ( data != NULL )
		{
			data[dataLength-1] = '\0';
		}
		else
		{
			dataLength=2;
			data = (LPTSTR)calloc( 2, sizeof(TCHAR) );
		}
	}
	else
	{
		dataLength = ((CStr)params[3]).GetLength() + 2;
		data = (LPTSTR)malloc( dataLength * sizeof(TCHAR) );
		wcscpy( data, (LPCTSTR)*params[3] );
		data[dataLength-1] = '\0';
	}

    BOOL    result = FALSE;
	HKEY    key;
	DWORD   type;

	if ( RegCreateKeyEx( root, params[1], 0, 0, 0
#ifndef DESKTOP
                       , 0
#else
                       , KEY_WRITE
#endif
                       , NULL, &key, &type ) == ERROR_SUCCESS )
	{
		if ( RegSetValueEx( key, params[2], NULL, REG_MULTI_SZ, (BYTE*)data, dataLength*sizeof(TCHAR) ) != ERROR_SUCCESS )
		{
            if ( interpreter.ErrorLevel >= ERROR_ERROR )
            {
                CStr msg;
                msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
                MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                rc = FALSE;
            }
		}
		RegCloseKey( key );
    }
	delete data;

    return rc;
}

BOOL CmdRegReadString( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegReadString'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CStr value = RegRead( root, params[1], params[2] );

    return interpreter.SetVariable( params[3], value );
}

BOOL CmdRegReadDWord( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegReadDWord'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CValue value;
    value = (long)RegReadDW( root, params[1], params[2] );

    return interpreter.SetVariable( params[3], value );
}

BOOL CmdRegReadBinary( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegReadBinary'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

	BOOL    result = FALSE;
	HKEY    key;
	CStr label;

	DWORD type, length;
	BYTE cont[16384];
	if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
	{
		length = 16384;
		if ( RegQueryValueEx( key, params[2], NULL, &type, (BYTE*)cont, &length ) != ERROR_SUCCESS )
		{
			length = 0;
		}
		RegCloseKey( key );
	}

    CStr value, byte;
    for ( int i=0; (DWORD)i < length; i++ )
    {
        byte.Format( L"%02X", cont[i] );
        value += byte;
    }

    return interpreter.SetVariable( params[3], value );
}

BOOL CmdRegDelete( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegDelete'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

	HKEY    key;
    rc = FALSE;
	if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS )
	{
        if ( RegDeleteValue( key, params[2] ) == ERROR_SUCCESS )
            rc = TRUE;
		RegCloseKey( key );
	}
    if ( rc == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't remove value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }


    return rc;
}

void CmdRegDeleteSubKey( HKEY key, LPCTSTR subkeyName, BOOL values, BOOL recursive )
{
    HKEY subkey;

    int idx;

	if ( RegOpenKeyEx( key, subkeyName, 0, REG_ACCESS_WRITE, &subkey ) == ERROR_SUCCESS )
    {
        TCHAR subName[MAX_PATH];
        DWORD subSize = MAX_PATH, type;
        CStrArray subKeys;

        if ( values == 1 )
        {
            for ( idx = 0; RegEnumValue( subkey, idx, subName, &subSize, NULL, &type, NULL, NULL ) == ERROR_SUCCESS; idx++ )
            {
                RegDeleteValue( subkey, subName );
                subSize = MAX_PATH;
            }
        }

        if ( recursive )
        {
            subKeys.RemoveAll();

            for ( idx = 0; RegEnumKeyEx( subkey, idx, subName, &subSize, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS; idx++ )
            {
                subKeys.Add( subName );
                subSize = MAX_PATH;
            }

            for ( int i=0; i<subKeys.GetSize(); i++ )
            {
                CmdRegDeleteSubKey( subkey, subKeys[i], values, recursive );
                ::RegDeleteKey( subkey, subKeys[i] );
            }
        }

		RegCloseKey( subkey );
	}
}

BOOL CmdRegDeleteKey( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RegDeleteKey'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ((CStr)params[1]).GetLength() < 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"RegDeleteKey: Registry key mustn't be empty!" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"Invalid root entry" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    rc = FALSE;
    if ( (long)params[2] == 1 || (long)params[3] == 1 )
    {
        CmdRegDeleteSubKey( root, params[1], (long)params[2], (long)params[3] );
    }

    if ( ::RegDeleteKey( root, params[1] ) == ERROR_SUCCESS )
        rc = TRUE;

    if ( rc == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_WARN )
        {
            CStr msg;
            msg.Format( L"Couldn't remove registry key %s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
		rc = TRUE;
    }


    return rc;
}

BOOL CmdIniRead( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'IniRead'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CIniFile ini;
    if ( ini.Read( params[0] ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            CStr msg;
            msg.Format( L"File '%s' not found", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CStr value = ini.GetString( params[1], params[2] );

    return interpreter.SetVariable( params[3], value );
}

BOOL CmdIniWrite( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'IniWrite'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStr content, newContent, newLine;
	BOOL foundSection=FALSE, foundValue=FALSE;
    if ( CIniFile::ReadFile( params[0], content ) < 0 )
		content.Empty();

	if ( ((CStr)params[1]).GetLength() == 0 ) // values outside of a section
		foundSection = TRUE;

	while ( content.GetLength() > 0 )
	{
		CStr line;
		int pos = content.Find( _T("\n") );
		if ( pos != -1 )
		{
			line    = content.Left( pos );
			content = content.Mid( pos+1 );
		}
		else
		{
			line = content;
			content = _T("");
		}
		line.TrimRight();

		if ( line.GetLength() > 0 && line.GetAt(0) != '#' && line.GetAt(0) != ';' )
		{
			if ( line.GetAt(0) == '[' && line.Right(1) == L"]" )
			{
				if ( foundSection )
				{
					newLine.Format( L"%s=%s\r\n", (LPCTSTR)params[2], (LPCTSTR)params[3] );
					newContent += newLine;
					foundValue = TRUE;
					newContent += line;
					newContent += L"\r\n";
					break;
				}

				CStr sectionName = line.Mid( 1, line.GetLength()-2 );
				if ( sectionName.CompareNoCase( params[1] ) == 0 )
					foundSection = TRUE;
			}
			else
			{
				if ( foundSection )
				{
					int eqPos = line.Find( '=' );
					if ( eqPos != -1 )
					{
						CStr key   = line.Left( eqPos );
						key.TrimLeft(); key.TrimRight();
						if ( key.CompareNoCase( params[2] ) == 0 )
						{
							newLine.Format( L"%s=%s\r\n", (LPCTSTR)params[2], (LPCTSTR)params[3] );
							newContent += newLine;
							foundValue = TRUE;
							break;
						}
					}
				}
			}
		}

		newContent += line;
		newContent += L"\r\n";
	}

	if ( foundSection && !foundValue )
	{
		newLine.Format( L"%s=%s\r\n", (LPCTSTR)params[2], (LPCTSTR)params[3] );
		newContent += newLine;
	}
	newContent += content;
	if ( !foundSection )
	{
		newLine.Format( L"\r\n[%s]\r\n", (LPCTSTR)params[1] );
		newContent += newLine;
		newLine.Format( L"%s=%s\r\n", (LPCTSTR)params[2], (LPCTSTR)params[3] );
		newContent += newLine;
	}

	CIniFile::WriteFile( params[0], newContent, FALSE );

    return TRUE;
}


BOOL CmdReadFile( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, ',', params, 1, 2 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ReadFile'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStr content;
    int err;
#ifndef PNA
    if ( ((CStr)params[0]).Left(5) == L"http:" || ((CStr)params[0]).Left(4) == L"ftp:" )
    {
        err = CIniFile::ReadWebFile( params[0], content );
    }
    else
    {
#endif
        err = CIniFile::ReadFile( params[0], content );
#ifndef PNA
    }
#endif

    if ( err != 0 )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            switch( err )
            {
            case -1:
                msg.Format( L"Couldn't open connection to internet" );
                break;

            case -2:
            case 404:
                msg.Format( L"'%s' couldn't be opened", (LPCTSTR)params[0] );
                break;

            case -3:
                msg.Format( L"'%s' is too big", (LPCTSTR)params[0] );
                break;

            case -4:
                msg.Format( L"Not enough memory to read file '%s'", (LPCTSTR)params[0] );
                break;

            case -5:
                msg.Format( L"Error reading '%s'", (LPCTSTR)params[0] );
                break;

            default:
                msg.Format( L"'%s' couldn't be opened (http error %d)", (LPCTSTR)params[0], err );
            }

            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    return interpreter.SetVariable( params[1], content );
}

BOOL CmdWriteFile( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
	interpreter.Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'WriteFile'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	BOOL append = FALSE;
	int cp = CP_ACP;

	if ( params.GetSize() >= 3 && (long)params.GetAt(2) != 0 )
		append = TRUE;
	if ( params.GetSize() >= 4 )
		cp = GetCodePage( params.GetAt(3) );

    if ( CIniFile::WriteFile( params[0], params[1], append, cp ) == FALSE )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            CStr msg;
            msg.Format( L"Couldn't write file '%s'", (LPCTSTR)params[0] );
            MessageBox( GetMsgParent(), msg + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    return rc;
}

BOOL CmdSetComInfo( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
	interpreter.Split( param, ',', params );
    if ( params.GetSize() < 3 || params.GetSize() > 7 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr data;
	if ( ((CStr)params[0]).Right(1) != L":" )
	{
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Invalid port name for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
	}
	((CStr)params[0]).MakeUpper();

	int timeout = (long)params[1];

	int rate = (long)params[2];
	if ( rate == 0 )
	{
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Invalid baud rate for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
	}

	int parity = 0;
	if ( params.GetSize() > 3 )
	{
		parity = -1;
		if ( ((CStr)params[3]).CompareNoCase( L"None" ) == 0 )  parity = 0;
		if ( ((CStr)params[3]).CompareNoCase( L"Odd" ) == 0 )   parity = 1;
		if ( ((CStr)params[3]).CompareNoCase( L"Even" ) == 0 )  parity = 2;
		if ( ((CStr)params[3]).CompareNoCase( L"Mark" ) == 0 )  parity = 3;
		if ( ((CStr)params[3]).CompareNoCase( L"Space" ) == 0 ) parity = 4;
		if ( parity == -1 )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
				MessageBox( GetMsgParent(), L"Invalid parity for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

	int bits = 8;
	if ( params.GetSize() > 4 )
	{
		bits = (long)params[4];
		if ( bits < 4 || bits > 8 )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
				MessageBox( GetMsgParent(), L"Invalid bits count for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

	int stopBits = 0;
	if ( params.GetSize() > 5 )
	{
		stopBits = -1;
		if ( ((CStr)params[5]).CompareNoCase( L"1" ) == 0 )		stopBits = 0;
		if ( ((CStr)params[5]).CompareNoCase( L"1.5" ) == 0 )	stopBits = 1;
		if ( ((CStr)params[5]).CompareNoCase( L"2" ) == 0 )		stopBits = 2;
		if ( stopBits == -1 )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
				MessageBox( GetMsgParent(), L"Invalid stop bits count for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

	int flow = 1;
	if ( params.GetSize() > 6 )
	{
		flow = -1;
		if ( ((CStr)params[6]).CompareNoCase( L"None" ) == 0 )		flow = 0;
		if ( ((CStr)params[6]).CompareNoCase( L"RTS/CTS" ) == 0 )	flow = 1;
		if ( ((CStr)params[6]).CompareNoCase( L"XON/XOFF" ) == 0 )	flow = 2;
		if ( flow == -1 )
		{
			if ( interpreter.ErrorLevel >= ERROR_ERROR )
				MessageBox( GetMsgParent(), L"Invalid flow control for 'SetComInfo'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

	data.Format( L"%d\n%d\n%d\n%d\n%d\n%d", timeout, rate, parity, bits, stopBits, flow );
	CStr port = params[0];
	port.MakeUpper();
	Variables.SetAt( L"[Port_" + port + L"]", data );

    return TRUE;
}

