#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
//#include "Tlhelp32.h"
//#include "resource.h"
#include "variables.h"
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

extern CInterpreter *CurrentInterpreter;

#include "Helpers.h"
#include "FunctionsFileReg.h"
#include "Interpreter.h"
#include "shlobj.h"

CValue  FctFileExists( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FileExists'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = (long)FileOrDirExists( params[0], 0 );
    return rc;
}

CValue  FctDirExists( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'DirExists'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = (long)FileOrDirExists( params[0], 1 );
    return rc;
}

CValue  FctMortScriptVersion( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 0 )
    {
        errorMessage = InvalidParameterCount + L"'MortScriptVersion'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = VERSION;

    return rc;
}


CValue  FctFileVersion( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FileVersion'";
        error = 9;
        return CValue();
    }

    DWORD size, dummy;
    size = GetFileVersionInfoSize( (LPTSTR)(LPCTSTR)params[0], &dummy );
    if ( size == 0 )
    {
        errorMessage.Format( L"Couldn't get version info for '%s'", (LPCTSTR)params.GetAt(0) );
        error = 9;
        return CValue();
    }

    void *verData = malloc( size );
    GetFileVersionInfo( (LPTSTR)(LPCTSTR)params[0], NULL, size, verData );
    void *fileInfo;
    UINT size2;
    VerQueryValue( verData, L"\\", &fileInfo, &size2 );
    VS_FIXEDFILEINFO *ffi = (VS_FIXEDFILEINFO*)fileInfo;
    DWORD ms = ffi->dwFileVersionMS;
    DWORD ls = ffi->dwFileVersionLS;

    CStr ver;
    ver.Format( L"%d.%d.%d.%d", HIWORD(ms), LOWORD(ms), HIWORD(ls), LOWORD(ls) );

    CValue rc;
    rc = ver;

	free( verData );

    return rc;
}

CValue  FctFileAttribute( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'FileAttribute'";
        error = 9;
        return CValue();
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    DWORD compAttr = 0;
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"directory" ) == 0 )
        compAttr = FILE_ATTRIBUTE_DIRECTORY;
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"hidden" ) == 0 )
        compAttr = FILE_ATTRIBUTE_HIDDEN;
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"readonly" ) == 0 )
        compAttr = FILE_ATTRIBUTE_READONLY;
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"system" ) == 0 )
        compAttr = FILE_ATTRIBUTE_SYSTEM;
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"archive" ) == 0 )
        compAttr = FILE_ATTRIBUTE_ARCHIVE;
#ifndef DESKTOP
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"romfile" ) == 0 )
        compAttr = FILE_ATTRIBUTE_INROM;
#endif
    if ( ((CStr)(params.GetAt(1))).CompareNoCase( L"compressed" ) == 0 )
        compAttr = FILE_ATTRIBUTE_COMPRESSED;
//#endif

    if ( compAttr == 0 )
    {
        errorMessage = L"Invalid attribute '" + ((CStr)params.GetAt(1)) + L"' for 'FileAttribute'";
        error = 9;
        return CValue();
    }

    CValue rc;
    if ( attribs != (DWORD)-1 && (attribs & compAttr) == compAttr )
        rc = 1L;
    else
        rc = 0L;
    return rc;
}

CValue  FctFileCreateTime( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FileCreateTime'";
        error = 9;
        return CValue();
    }

    HANDLE hFile = CreateFile( params[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    CValue time;
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        time = 0L;
        return time;
    }

	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation( &tzi );

    FILETIME filetime;
    BOOL rc = GetFileTime( hFile, &filetime, NULL, NULL );
    time = (long)(FileTimeToUnixTime( filetime ) - tzi.Bias * 60);

    CloseHandle( hFile );

    return time;
}

CValue  FctFileModifyTime( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FileModifyTime'";
        error = 9;
        return CValue();
    }

    HANDLE hFile = CreateFile( params[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    CValue time;
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        time = 0L;
        return time;
    }

	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation( &tzi );

    FILETIME filetime;
    BOOL rc = GetFileTime( hFile, NULL, NULL, &filetime );
    time = (long)(FileTimeToUnixTime( filetime ) - tzi.Bias * 60);

    CloseHandle( hFile );

    return time;
}

CValue  FctFileSize( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'FileSize'";
        error = 9;
        return CValue();
    }

    HANDLE hFile = CreateFile( params[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    CValue sizeVal;
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        sizeVal = 0L;
        return sizeVal;
    }

    DWORD hi;
    DWORD size = GetFileSize( hFile, &hi );
    CloseHandle( hFile );

	if ( params.GetSize() > 1 )
	{
		for( int i = 0; i < (long)params[1]; i++ )
		{
			DWORD hiBits = hi << 22; // 32 Bit - 10 shifted
			size = size >> 10;
			size |= hiBits;
			hi = hi >> 10;
		}
	}

    if ( hi != 0 || (long)size < 0 ) size = 2147483647L;
    sizeVal = (long)size;

    return sizeVal;
}

CValue  FctDirContents( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'DirContents'";
        error = 9;
        return CValue();
    }

	short type = 3;
	if ( params.GetSize() > 1 )
	{
		type = (short)(long)params[1];
	}
	if ( type < 1 || type > 3 ) type = 3;

	CValue rc;
	CMapStrToValue *map = rc.GetMap();
	int pos = 1;
	CStr entry;
	
    CStr path = ((CStr)params[0]).Left(((CStr)params[0]).ReverseFind('\\')+1);
    WIN32_FIND_DATA findFileData;
    HANDLE ffh = FindFirstFile( params[0], &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
        do
        {
            CStr file = path+findFileData.cFileName;
            if (   findFileData.cFileName[0] != '.'
				&& (   type == 3
				    || ( type == 1 && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
					|| ( type == 2 && (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
				   )
			   )
            {
				entry.Format( L"%d", pos );
				map->SetAt( entry, findFileData.cFileName );
                pos++;
            }
        }
		while ( FindNextFile( ffh, &findFileData ) == TRUE );

		FindClose( ffh );
	}

	return rc;
}

CValue  FctSystemPath( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'SystemPath'";
        error = 9;
        return CValue();
    }

    CStr path;
    LPTSTR  pathBuffer = path.GetBuffer( MAX_PATH );
    if ( ((CStr)params[0]).CompareNoCase( L"ProgramsMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PROGRAMS, FALSE );
    }
//#if defined( PNA ) || !defined( SMARTPHONE )
    else if ( ((CStr)params[0]).CompareNoCase( L"StartMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_STARTMENU, FALSE );
    }
//#endif
    else if ( ((CStr)params[0]).CompareNoCase( L"Startup" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_STARTUP, FALSE );
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"Documents" ) == 0 )
    {
#ifdef POCKETPC
        // Works only in WM2003 and later
        OSVERSIONINFO ver;
        GetVersionEx( &ver );
        if ( ( ver.dwMajorVersion > 4 || (ver.dwMajorVersion == 4 && ver.dwMinorVersion >= 20) ) )
        {
            SHGetDocumentsFolder( L"\\", pathBuffer );
        }
        else
        {
            errorMessage = L"GetSystemPath: Document path requires WM2003";
            error = 9;
            return CValue();
        }
#else
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PERSONAL, FALSE );
#endif
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ProgramFiles" ) == 0 )
    {
        // Works only in WM2003 and later
#ifndef DESKTOP
        OSVERSIONINFO ver;
        GetVersionEx( &ver );
        if ( ( ver.dwMajorVersion > 4 || (ver.dwMajorVersion == 4 && ver.dwMinorVersion >= 20) ) )
        {
            SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PROGRAM_FILES, FALSE );
        }
        else
        {
            errorMessage = L"GetSystemPath: Program files path requires WM2003";
            error = 9;
            return CValue();
        }
#else
        if ( SHGetSpecialFolderPath( NULL, pathBuffer, 0x0026, FALSE ) == FALSE )
        {
            errorMessage = L"GetSystemPath: Retrieving program files path not supported by your system";
            error = 9;
            return CValue();
        }
#endif
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"AppData" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_APPDATA, FALSE );
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"JScripts" ) == 0 || ((CStr)params[0]).CompareNoCase( L"ScriptPath" ) == 0)
    {
        int len = CurrentInterpreter->ScriptFile.ReverseFind('\\');
		if ( len == -1 )
			len = 0;
        else
			wcsncpy( pathBuffer, (LPCTSTR)(CurrentInterpreter->ScriptFile), len );
        pathBuffer[len] = '\0';
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptName" ) == 0 )
    {
        int len = CurrentInterpreter->ScriptFile.ReverseFind('\\');
        int dot = CurrentInterpreter->ScriptFile.ReverseFind('.');
        wcsncpy( pathBuffer, (LPCTSTR)CurrentInterpreter->ScriptFile+len+1, dot-len-1 );
        pathBuffer[dot-len-1] = '\0';
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptExt" ) == 0 )
    {
        int dot = CurrentInterpreter->ScriptFile.ReverseFind('.');
		if ( dot == -1 )
			pathBuffer[0] = '\0';
		else
	        wcscpy( pathBuffer, (LPCTSTR)CurrentInterpreter->ScriptFile.Mid(dot) );
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptExe" ) == 0 )
    {
        wcscpy( pathBuffer, (LPCTSTR)AppPath );
    }
    else
    {
        errorMessage = L"Invalid path type for SystemPath";
        error = 9;
        return CValue();
    }
    path.ReleaseBuffer();

    CValue rc;
    rc = path;

    return rc;
}

CValue  FctFreeDiskSpace( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'FreeDiskSpace'";
        error = 9;
        return CValue();
    }

    ULARGE_INTEGER   freeBytesAvailableToCaller;
    ULARGE_INTEGER   totalNumberOfBytes;
    ULARGE_INTEGER   totalNumberOfFreeBytes;
    BOOL fseRc = GetDiskFreeSpaceEx( (LPCTSTR)params[0]
                                   , &freeBytesAvailableToCaller
                                   , &totalNumberOfBytes
                                   , &totalNumberOfFreeBytes );

    if ( fseRc == 0 )
    {
        errorMessage = L"Unable to receive free disk space for " + (CStr)params[0];
        error = 9;
        return CValue();
    }

	if ( params.GetSize() > 1 )
	{
		for( int i = 0; i < (long)params[1]; i++ )
		{
			DWORD hiBits = freeBytesAvailableToCaller.HighPart << 22; // 32 Bit - 10 shifted
			freeBytesAvailableToCaller.LowPart = freeBytesAvailableToCaller.LowPart >> 10;
			freeBytesAvailableToCaller.LowPart |= hiBits;
			freeBytesAvailableToCaller.HighPart = freeBytesAvailableToCaller.HighPart >> 10;
		}
	}

    CValue rc;
    if ( freeBytesAvailableToCaller.HighPart > 0 || (long)freeBytesAvailableToCaller.LowPart < 0 )
        rc = 2147483647L;
    else
        rc = (long)freeBytesAvailableToCaller.LowPart;

    return rc;
}

CValue  FctTotalDiskSpace( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'TotalDiskSpace'";
        error = 9;
        return CValue();
    }

    ULARGE_INTEGER   freeBytesAvailableToCaller;
    ULARGE_INTEGER   totalNumberOfBytes;
    ULARGE_INTEGER   totalNumberOfFreeBytes;
    BOOL fseRc = GetDiskFreeSpaceEx( (LPCTSTR)params[0]
                                   , &freeBytesAvailableToCaller
                                   , &totalNumberOfBytes
                                   , &totalNumberOfFreeBytes );

    if ( fseRc == 0 )
    {
        errorMessage = L"Unable to receive disk space for " + (CStr)params[0];
        error = 9;
        return CValue();
    }

	if ( params.GetSize() > 1 )
	{
		for( int i = 0; i < (long)params[1]; i++ )
		{
			DWORD hiBits = totalNumberOfBytes.HighPart << 22; // 32 Bit - 10 shifted
			totalNumberOfBytes.LowPart = totalNumberOfBytes.LowPart >> 10;
			totalNumberOfBytes.LowPart |= hiBits;
			totalNumberOfBytes.HighPart = totalNumberOfBytes.HighPart >> 10;
		}
	}

    CValue rc;
    if ( totalNumberOfBytes.HighPart > 0 || (long)totalNumberOfBytes.LowPart < 0 )
        rc = 2147483647L;
    else
        rc = (long)totalNumberOfBytes.LowPart;

    return rc;
}


CValue  FctReadFile( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
    CStr content;

    if ( params.GetSize() < 1 && params.GetSize() > 3 )
    {
        errorMessage = InvalidParameterCount + L"'ReadFile'";
        error = 9;
        return CValue();
    }

    int size = -1;
    if ( params.GetSize() == 2 )
    {
        size = (long)params[1];
    }

	int cp = CP_ACP;
	if ( params.GetSize() == 3 )
		cp = GetCodePage( params.GetAt(2) );

    int err;
#ifndef PNA
    if ( ((CStr)params[0]).Left(5) == L"http:" || ((CStr)params[0]).Left(4) == L"ftp:" )
    {
        err = CIniFile::ReadWebFile( params[0], content, size, cp );
    }
    else
    {
#endif
        err = CIniFile::ReadFile( params[0], content, size, cp );
#ifndef PNA
    }
#endif

    if ( err != 0 )
    {
        CStr msg;
        switch( err )
        {
        case -1:
            errorMessage.Format( L"Couldn't open connection to internet" );
            break;

        case -2:
        case 404:
            errorMessage.Format( L"'%s' couldn't be opened", (LPCTSTR)params[0] );
            break;

        case -3:
            errorMessage.Format( L"'%s' is too big", (LPCTSTR)params[0] );
            break;

        case -4:
            errorMessage.Format( L"Not enough memory to read file '%s'", (LPCTSTR)params[0] );
            break;

        case -5:
            errorMessage.Format( L"Error reading '%s'", (LPCTSTR)params[0] );
            break;

        default:
            errorMessage.Format( L"'%s' couldn't be opened (http error %d)", (LPCTSTR)params[0], err );
        }
        error = 9;
        return CValue();
    }

    rc = content;

    return rc;
}

CValue  FctReadLine( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
    CStr content;

    if ( params.GetSize() < 1 && params.GetSize() > 2 )
    {
        errorMessage = InvalidParameterCount + L"'ReadLine'";
        error = 9;
        return CValue();
    }

	int cp = CP_ACP;
	if ( params.GetSize() == 2 )
		cp = GetCodePage( params.GetAt(1) );

	CStr fileWithPath;
	fileWithPath = (CStr)params[0];

	if ( CurrentInterpreter != NULL && ( fileWithPath.GetLength() < 2 || ( fileWithPath.GetAt(0) != '\\' && fileWithPath.GetAt(1) != ':' ) ) )
	{
		int len = CurrentInterpreter->ScriptFile.ReverseFind('\\');
		if ( len == -1 )
			fileWithPath = L"\\" + fileWithPath;
		else
			fileWithPath = CurrentInterpreter->ScriptFile.Left( len+1 ) + fileWithPath;
	}

	fileWithPath.MakeLower();
	CFileInfo *fi;
	//int remaining;//jwz::

	if ( FileHandles.Lookup( fileWithPath, (void*&)fi ) == FALSE )
	{
		HANDLE file = CreateFile( fileWithPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( file == INVALID_HANDLE_VALUE )
		{
			errorMessage.Format( L"'%s' couldn't be opened", (LPCTSTR)params[0] );
			error = 9;
			return CValue();
		}

		fi = new CFileInfo( file, cp );

		int stat = ::ReadFile( file, (void*)fi->data, 4094, &fi->dataSize, NULL );
		if ( stat == 0 )
		{
            errorMessage.Format( L"Error reading '%s'", (LPCTSTR)params[0] );
			delete fi;
			error = 9;
			return CValue();
		}
		fi->data[fi->dataSize] = '\0';
		fi->data[fi->dataSize+1] = '\0';

		FileHandles.SetAt( fileWithPath, fi );

		if ( fi->dataSize < 4094 )
		{
			CloseHandle( fi->handle );
			fi->handle = NULL;
			fi->eof = TRUE;
		}

		// Handle encoding prefixes
		if ( fi->dataSize >= 3 && memcmp( fi->data, (void*)"\xEF\xBB\xBF", 3 ) == 0 )
		{
			fi->encoding = CP_UTF8_PREFIX;
			memmove( fi->data, fi->data+3, fi->dataSize-1 ); // -3 bytes for prefix, +2 bytes for \0s
			fi->dataSize -= 3;
		}
		if ( fi->dataSize >= 2 && memcmp( fi->data, (void*)"\xFF\xFE", 2 ) == 0 )
		{
			fi->encoding = CP_UNICODE_PREFIX;
			memmove( fi->data, fi->data+2, fi->dataSize ); // -2 bytes for prefix, +2 bytes for \0s = no difference
			fi->dataSize -= 2;
		}
	}
	else
	{
		if ( fi->dataSize < 2048 && ! fi->eof )
		{
			int oldSize = fi->dataSize;
			int stat = ::ReadFile( fi->handle, (void*)(fi->data + oldSize), 4094-oldSize, &fi->dataSize, NULL );
			if ( stat == 0 )
			{
				errorMessage.Format( L"Error reading '%s'", (LPCTSTR)params[0] );
				delete fi;
				error = 9;
				return CValue();
			}

			if ( fi->dataSize < (unsigned long)4094-oldSize )
			{
				CloseHandle( fi->handle );
				fi->handle = NULL;
				fi->eof = TRUE;
			}

			fi->dataSize += oldSize;
			fi->data[fi->dataSize] = '\0';
			fi->data[fi->dataSize+1] = '\0';
		}
	}

	if ( fi->dataSize == 0 ) // Nothing in file or EOF exceeded
	{
		delete fi;
		FileHandles.RemoveKey( fileWithPath );
		return rc;
	}
	else
	{
		if ( fi->encoding == CP_UNICODE || fi->encoding == CP_UNICODE_PREFIX )
		{
			TCHAR *lf = wcschr( (TCHAR*)fi->data, '\n' );
			if ( lf == NULL ) // last line?
			{
				if ( fi->eof )
				{
					rc = (TCHAR*)fi->data;
					fi->dataSize = 0;
				}
				else
				{
					errorMessage.Format( L"Line too long in '%s'", (LPCTSTR)params[0] );
					delete fi;
					error = 9;
					return CValue();
				}
			}
			else
			{
				*lf = 0; // replace LF with string end
				if ( lf > (TCHAR*)fi->data && *(lf-1) == '\r' ) // CR before LF?
					*(lf-1) = 0; // replace CR with string end
				rc = (TCHAR*)fi->data;
				fi->dataSize -= (BYTE*)lf - fi->data + 2;
				memmove( fi->data, lf+1, fi->dataSize+2 ); // +2 bytes for \0s
			}
		}
		else
		{
			char *lf = strchr( (char*)fi->data, '\n' );
			TCHAR tmp[2048];
			if ( lf == NULL ) // last line
			{
				if ( fi->eof )
				{
					MultiByteToWideChar( fi->encoding, 0, (char*)fi->data, fi->dataSize, tmp, 2048 );
					rc = tmp;
					fi->dataSize = 0;
				}
				else
				{
					errorMessage.Format( L"Line too long in '%s'", (LPCTSTR)params[0] );
					delete fi;
					error = 9;
					return CValue();
				}
			}
			else
			{
				*lf = 0; // replace LF with string end
				if ( lf > (char*)fi->data && *(lf-1) == '\r' ) // CR before LF?
					*(lf-1) = 0; // replace CR with string end
				MultiByteToWideChar( fi->encoding, 0, (char*)fi->data, fi->dataSize, tmp, 2048 );
				rc = tmp;
				fi->dataSize -= (BYTE*)lf - fi->data + 1;
				memmove( fi->data, lf+1, fi->dataSize+2 ); // +2 bytes for \0s
			}
		}
	}

    return rc;
}

CValue  FctIniRead( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'IniRead'";
        error = 9;
        return CValue();
    }

    CIniFile ini;
    if ( ini.Read( params[0] ) )
    {
        rc = ini.GetString( params[1], params[2] );
    }

    return rc;
}

CValue  FctRegRead( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'RegRead'";
        error = 9;
        return CValue();
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        errorMessage = L"Invalid main key for 'RegKeyExists'";
        error = 9;
        return CValue();
    }

    BOOL    result = FALSE;
    HKEY    key;
    CStr label;

    DWORD type = REG_NONE, length;
    BYTE cont[16384];
    if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
    {
        length = 16384;
        if ( RegQueryValueEx( key, params[2], NULL, &type, (BYTE*)cont, &length ) != ERROR_SUCCESS )
        {
            length = 0;
			type = REG_NONE;
        }
        RegCloseKey( key );
    }

    switch ( type )
    {
    case REG_BINARY:
        {
            CStr content, byte;
            for ( DWORD i=0; i < length; i++ )
            {
                byte.Format( L"%02X", cont[i] );
                content += byte;
            }
            rc = content;
        }
        break;

    case REG_DWORD:
        rc = (long)*((DWORD*)cont);
        break;

	case REG_MULTI_SZ:
		{
			CMapStrToValue *map = rc.GetMap();
			int arrPos = 1, pos = 0, len;
			CStr key;
			while ( TRUE )
			{
				len = wcslen( ((LPCTSTR)cont)+pos );
				if ( len > 0 )
				{
					key.Format( L"%d", arrPos );
					map->SetAt( key, ((LPCTSTR)cont)+pos );
					pos += len+1;
					arrPos++;
				}
				else
					break;
			}
		}
		break;

    case REG_SZ:
    case REG_EXPAND_SZ:
        rc = (LPCTSTR)cont;
		break;

	case REG_NONE:
		rc.Clear();
		break;

	default:
		rc = L"Not supported registry type";
    }

    return rc;
}

CValue  FctRegKeyExists( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
    rc = 0L;

    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'RegKeyExists'";
        error = 9;
        return CValue();
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        errorMessage = L"Invalid main key for 'RegKeyExists'";
        error = 9;
        return CValue();
    }

    HKEY key;
    if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
    {
        rc = 1L;
        RegCloseKey( key );
    }

    return rc;
}

CValue  FctRegValueExists( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
    rc = 0L;

    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'RegValueExists'";
        error = 9;
        return CValue();
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        errorMessage = L"Invalid main key for 'RegValueExists'";
        error = 9;
        return CValue();
    }

    HKEY key;
    if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
    {
        DWORD type, length;
        TCHAR cont[MAX_PATH];
        length = MAX_PATH;
        int ret = RegQueryValueEx( key, params[2], NULL, &type, (BYTE*)cont, &length );
        if ( ret == ERROR_SUCCESS || ret == ERROR_MORE_DATA )
        {
            rc = 1L;
        }
        RegCloseKey( key );
    }

    return rc;
}

