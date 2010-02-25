#include "Helpers.h"

#include "FunctionsData.h"
#include "FunctionsDialogs.h"
#include "FunctionsFileReg.h"
#include "FunctionsWindows.h"
#include "FunctionsSystem.h"

#include "CommandsDialogs.h"
#include "CommandsFileReg.h"

#ifdef DESKTOP
#include "vc6\resource.h"
#else
#ifdef SMARTPHONE
#include "smartphone\resource.h"
#else
#include "resource.h"
#endif
#endif

#include "DlgStatus.h"

#include "mortafx.h"
#include "inifile.h"

CStr ExecuteFileName;

// defined in Functions.cpp
extern function FunctionsArray[];
extern short FunctionsArraySize;
// defined in Commands.cpp
extern function CommandsArray[];
extern short CommandsArraySize;

//extern HWND StatusWindow;
// ZIP DLL pointers
HINSTANCE ZipDll = NULL;
BOOL  (_cdecl *UnzipToFile)(LPCTSTR,LPCTSTR,LPCTSTR);
BOOL  (_cdecl *UnzipAllToPath)(LPCTSTR,LPCTSTR);
BOOL  (_cdecl *UnzipPathToPath)(LPCTSTR,LPCTSTR,LPCTSTR);
void* (_cdecl *UnzipToMemory)(LPCTSTR,LPCTSTR,UINT*);
BOOL  (_cdecl *ZipFromFile)(LPCTSTR,LPCTSTR,LPCTSTR,int);
BOOL  (_cdecl *ZipFromMemory)(LPCTSTR,LPCTSTR,void *,DWORD,int);
BOOL  (_cdecl *ZipFromFilesToZipPath)(LPCTSTR,LPCTSTR,LPCTSTR,BOOL,int);

#ifndef DESKTOP
// Toolhelp DLL pointers
HINSTANCE ToolhelpDll = NULL;
HANDLE (_cdecl *CreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID) = NULL;
BOOL (_cdecl *CloseToolhelp32Snapshot)(HANDLE hSnapshot) = NULL;
BOOL (_cdecl *Process32First)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) = NULL;
BOOL (_cdecl *Process32Next)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe) = NULL;
BOOL (_cdecl *Module32First)(HANDLE hSnapshot, LPMODULEENTRY32 lpme) = NULL;
BOOL (_cdecl *Module32Next)(HANDLE hSnapshot, LPMODULEENTRY32 lpme) = NULL;
#endif

CStr Proxy = L"";
CStr AppPath = L"";

CStr InvalidParameterCount = L"Invalid parameter count for ";

void GetFunctionArraySize( function array[], short &size )
{
	// Get the size (find the end flag)
	for ( size = 0; array[size].name != NULL; size++ );
}

void *SearchFunctionPointer( function array[], short &size, LPCTSTR name )
{
	if ( size == 0 )
	{
		GetFunctionArraySize( array, size );
	}

	void   *pointer = NULL;

	int		start = 0, end = size-1, pos;
	int		cmp;
	BOOL	found = FALSE;

	if ( size <= 0 )
	{
		return NULL;
	}

	while ( end-start > 5 && start >= 0 && end < size && found == FALSE )
	{
		pos = int( (end-start)/2 )+start;
		cmp = wcscmp( name, array[pos].name );
		if ( cmp == 0 )
			found = TRUE;
		else
		{
			if ( cmp < 0 )
				end = pos-1;
			else
				start = pos+1;
		}
	}

	if ( !found )
	{
		if ( start < 0 ) start = 0;
		if ( end >= size ) end = size-1;
		pos = start;
		while ( wcscmp( name, array[pos].name ) > 0 && pos < end )
		{
			pos++;
		}
		if ( wcscmp( name, array[pos].name ) == 0 )
		{
			found = TRUE;
		}
		else
		{
			if ( pos == end )
			{
				if ( pos < size && wcscmp( name, array[pos].name ) > 0 )
					pos++;
				if ( pos > size ) pos = -1;
			}
			else
			{
				while ( pos > 0 && wcscmp( name, array[pos-1].name ) < 0 )
				{
					pos--;
				}
			}
		}
	}

	if ( found )
		return array[pos].ptr;
	else
		return NULL;
}

functionPointer GetFunction( LPCTSTR name )
{
	return (functionPointer)SearchFunctionPointer( FunctionsArray, FunctionsArraySize, name );
}

commandPointer GetCommand( LPCTSTR name )
{
	return (commandPointer)SearchFunctionPointer( CommandsArray, CommandsArraySize, name );
}

DWORD GetRunningScriptProcId( CStr &scriptName, CStr *mutexName )
{
    CStr mutex = scriptName;
    BOOL    compPath = ( scriptName.Find('\\') != -1 );
    BOOL    result = FALSE;
    HKEY    key;
    DWORD   procId = NULL;

    mutex.MakeLower();
    if ( compPath )
    {
#ifdef DESKTOP
        // Windows XP doesn't like some path characters in the mutex' name
        mutex.Replace( ':', '_' );
        mutex.Replace( '\\', '/' );
#endif
    }

    DWORD res = RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, REG_ACCESS_READ, &key );

    if ( res == ERROR_SUCCESS )
    {
        DWORD type, nameLength = MAX_PATH, valLength = MAX_PATH;
        TCHAR valName[MAX_PATH], value[MAX_PATH];
        for ( int i=0; RegEnumValue( key, i, (TCHAR*)&valName, &nameLength, NULL, &type, (BYTE*)&value, &valLength ) == ERROR_SUCCESS; i++ )
        {
            if ( compPath )
            {
                if ( mutex.Compare( valName ) == 0 )
                {
                    procId = *((DWORD*)value);
                }
            }
            else
            {
                CStr valEnd = CStr( valName ).Right( mutex.GetLength() + 1 );
                if (    ( valEnd[0] == '\\' || valEnd[0] == '/' )
                     && ( mutex.Compare( valEnd.Mid(1) ) == 0 )
                   )
                {
                    procId = *((DWORD*)value);
                }
            }

            if ( procId != NULL )
            {
				CStr procName = GetProcessExePath( procId );
				//MessageBox( NULL, procName, ExecuteFileName, MB_SETFOREGROUND );
				if ( procName.CompareNoCase( ExecuteFileName ) != 0 )
				{
					procId = NULL;
				}

                if ( procId != NULL )
                {
                    if ( mutexName != NULL )
                        *mutexName = valName;
                    break;
                }
            }
            nameLength = MAX_PATH, valLength = MAX_PATH;
        }
        RegCloseKey( key );
    }

    int e = GetLastError();

    return procId;
}

CStr GetProcessExePath( DWORD processId, LPTSTR exeName )
{
	CStr procName;

	// On desktop, security features avoid getting the program name with GetModuleFileName (it always
	// would return MortScript's path), so we have to iterate through all modules.

	// With Windows Mobile, on the other hand, ToolHelp doesn't retrieve the module file name, so
	// we can't use the same there and are forced to use GetModuleFileName which shouldn't work,
	// but does...

#ifdef DESKTOP
	BOOL tmpExe = FALSE;
	if ( exeName == NULL )
	{
		tmpExe = TRUE;
		exeName = new TCHAR[256];
		exeName[0] = '\0';

	    HANDLE  procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		if ( procSnap == NULL || (int)procSnap == -1 )
		{
			return procName;
		}

	    PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && (int)procSnap != -1 && Process32First( procSnap, &procEntry ) )
		{
			do
			{
				if ( procEntry.th32ProcessID == processId )
				{
					wcscpy( exeName, procEntry.szExeFile );
					MessageBox( NULL, exeName, L"exe", MB_SETFOREGROUND );
					break;
				}
				procEntry.dwSize = sizeof( procEntry );
			}
			while ( Process32Next( procSnap, &procEntry ) );

//#ifndef DESKTOP
//			CloseToolhelp32Snapshot( procSnap );
//#else
			CloseHandle( procSnap );
//#endif
		}
	}

	if ( exeName[0] == '\0' )
	{
		if ( tmpExe ) delete[] exeName;
		return procName;
	}

	procName = exeName;

	MODULEENTRY32 modEntry;
	modEntry.dwSize	= sizeof( modEntry );

	HANDLE modSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, processId ); 
	Module32First( modSnap, &modEntry );

	CStr msg;
	msg = exeName;
	do
	{
		msg += L"\n";
		msg += modEntry.szExePath;

		//if ( tmpExe )
		//	MessageBox( NULL, modEntry.szExePath, L"module", MB_SETFOREGROUND );
		int procLen = wcslen( exeName );
		int modLen  = wcslen( modEntry.szExePath );
		if ( modLen > procLen && wcsicmp( exeName, modEntry.szExePath+modLen-procLen ) == 0 )
		{
			msg += L"<<";
			procName = modEntry.szExePath;
			break;
		}
		modEntry.dwSize	= sizeof( modEntry );
	}
	while ( Module32Next( modSnap, &modEntry ) );

	//MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );

//#ifndef DESKTOP
//	CloseToolhelp32Snapshot( modSnap );
//#else
	CloseHandle( modSnap );
//#endif

	if ( tmpExe ) delete[] exeName;
#else
	HANDLE procHandle = OpenProcess( 0, FALSE, processId );
	if ( procHandle != NULL )
	{
		TCHAR buffer[MAX_PATH];
		::GetModuleFileName( (HMODULE)procHandle, buffer, MAX_PATH );
		procName = buffer;
		CloseHandle( procHandle );
	}
#endif

	return procName;
}


void GetExeForDoc( const CStr &doc, CStr &exe )
{
	CStr ext = doc.Mid( doc.ReverseFind( '.' ) );

	CStr appID = RegRead( HKEY_CLASSES_ROOT, ext, NULL );
	exe = RegRead( HKEY_CLASSES_ROOT, appID + L"\\Shell\\Open\\Command", NULL );
}


void GetExe( const CStr &appAndParam, CStr &exe, CStr &param )
{
	if ( appAndParam.GetAt(0) == '\"' )
	{
		int end = appAndParam.Find( '\"', 1 );
		exe   = appAndParam.Mid( 1, end-1 );
		if ( end < appAndParam.GetLength() - 1 )
		{
			param = appAndParam.Mid( end+2 );
		}
		else
		{
			param = L"";
		}
	}
	else
	{
		int end = appAndParam.Find( ' ' );
		if ( end != -1 )
		{
			exe   = appAndParam.Left( end );
			param = appAndParam.Mid( end+1 );
		}
		else
		{
			exe = appAndParam;
			param = L"";
		}
	}
}

void GetRunData( CStr &exe, CStr &param )
{
	// "Normal" links...
	if ( exe.Right(4).CompareNoCase( L".exe" ) != 0 )
	{
		CStr docRun, docExe;

		// No exe - that means, it's a document!
		GetExeForDoc( exe, docRun );
		if ( docRun.IsEmpty() )
		{
			exe = L"";
			param = L"";
		}
		else
		{
			GetExe( docRun, docExe, param );
			param.Replace( L"%1", exe );
			exe = docExe;
		}
	}
}


BOOL FileOrDirExists( CStr &file, BOOL dir )
{
    BOOL rc = 0;

    DWORD attribs = GetFileAttributes( file );
    if ( attribs != -1 )
    {
        if ( (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0 )
        {
            if ( dir ) rc = 1;
        }
        else
        {
            if ( !dir ) rc = 1;
        }
    }

    return rc;
}

SearchWindow sw;
TCHAR windowTitle[256];

//jwz::modi for FindWindow for no case sensitive.
BOOL CALLBACK SearchWindowText( HWND hwnd, LPARAM lParam )
{
	// SearchWindow *sw = (SearchWindow*)lParam;

	int NoCase = sw.Type;
	sw.Type = 0;
	int len = ::GetWindowText( hwnd, windowTitle, 255 );
    
	if ( len > 0 )
	{
		windowTitle[255] = '\0';
		TCHAR *wndTitleTmpUpCase = wcsupr(wcsdup(windowTitle));
		TCHAR * SrchTextUpCase = wcsupr(wcsdup(sw.SearchText));

		TCHAR *pos = (NoCase == 0) ? wcsstr( windowTitle, sw.SearchText ) : wcsstr(wndTitleTmpUpCase,SrchTextUpCase);
		if ( pos != NULL )
		{
			int type = 0;
			if ( ::IsWindowVisible( hwnd ) )
			{
				type = 1;
			}
			if ( pos == windowTitle )
			{
				type |= 2;
				if (((NoCase == 0) ? wcsstr( sw.SearchText, windowTitle ) : wcsstr(SrchTextUpCase, wndTitleTmpUpCase)) == 0 )
				{
					type |= 4;
				}
			}
			HWND parent = ::GetParent( hwnd );
			if ( parent == NULL || parent == ::GetDesktopWindow() )
			{
				type |= 8;
			}

			if ( type > sw.Type )
			{
				sw.FoundWnd = hwnd;
				sw.Type = type;
			}

			if ( type == 15 )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

HWND FindWindowMy( CStr &window , int Type)
{
	HWND wnd = NULL;

	wcscpy( sw.SearchText, (LPCTSTR)window );
	sw.SearchText[255] = '\0';
	sw.FoundWnd   = NULL;
	sw.Type       = Type;
	BOOL rc = ::EnumWindows( SearchWindowText, (LPARAM)&sw );
	if ( sw.FoundWnd != NULL )
		wnd = sw.FoundWnd;

	return wnd;
}
//jwz::modi end

HKEY GetTopKey( CStr &topkey )
{
    HKEY root = NULL;

    if ( topkey.CompareNoCase( L"HKCU" ) == 0 ) root = HKEY_CURRENT_USER;
    if ( topkey.CompareNoCase( L"HKLM" ) == 0 ) root = HKEY_LOCAL_MACHINE;
    if ( topkey.CompareNoCase( L"HKCR" ) == 0 ) root = HKEY_CLASSES_ROOT;
    if ( topkey.CompareNoCase( L"HKUS" ) == 0 ) root = HKEY_USERS;

    return root;
}

CStr RegRead( HKEY topkey, LPCTSTR path, LPCTSTR value )
{
	BOOL    result = FALSE;
	HKEY    key;
	CStr label;

	if ( RegOpenKeyEx( topkey, path, 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
	{
		DWORD type, length;
		TCHAR cont[MAX_PATH];
		length = MAX_PATH;
		if ( RegQueryValueEx( key, value, NULL, &type, (BYTE*)cont, &length ) == ERROR_SUCCESS )
		{
			label = cont;
		}
		RegCloseKey( key );
	}

	return label;
}

DWORD RegReadDW( HKEY topkey, LPCTSTR path, LPCTSTR value )
{
	BOOL    result = FALSE;
	HKEY    key;
	DWORD   content = 0;

	//MessageBox(NULL,path,value,MB_SETFOREGROUND);
	if ( RegOpenKeyEx( topkey, path, 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
	{
		DWORD type, length = sizeof(DWORD);
		DWORD cont;
		length = MAX_PATH;
		if ( RegQueryValueEx( key, value, NULL, &type, (BYTE*)&cont, &length ) == ERROR_SUCCESS )
		{
			content = cont;
		}
		RegCloseKey( key );
	}

	return content;
}

//  RegWriteDW
// ------------
//  Writes a registry entry (DWORD)
//
BOOL RegWriteDW( HKEY topkey, LPCTSTR path, LPCTSTR valName, DWORD value )
{
	BOOL    result = FALSE;
	HKEY    key;
	DWORD   type;

	if ( RegCreateKeyEx( topkey, path, 0, 0, 0
#ifndef DESKTOP
                     , 0
#else
                     , KEY_WRITE
#endif
                     , NULL, &key, &type ) == ERROR_SUCCESS )
	{
		if ( RegSetValueEx( key, valName, NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) ) == ERROR_SUCCESS )
		{
			result = TRUE;
		}
		RegCloseKey( key );
	}

	return result;
}

//  RegWrite
// ------------
//  Writes a registry entry (string)
//
BOOL RegWrite( HKEY topkey, LPCTSTR path, LPCTSTR valName, LPCTSTR string )
{
	BOOL    result = FALSE;
	HKEY    key;
	DWORD   type;

	if ( RegCreateKeyEx( topkey, path, 0, 0, 0
#ifndef DESKTOP
                      , 0
#else
                      , KEY_WRITE
#endif
                      , NULL, &key, &type ) == ERROR_SUCCESS )
	{
		if ( RegSetValueEx( key, valName, NULL, REG_SZ, (BYTE*)string, (wcslen(string)+1)*sizeof(TCHAR) ) == ERROR_SUCCESS )
		{
			result = TRUE;
		}
		RegCloseKey( key );
	}

	return result;
}

int GetCodePage( CStr &encoding )
{
    int cp = CP_ACP;
    if ( _wtol( encoding ) != 0 )
        cp = _wtol( encoding );
    if ( encoding.CompareNoCase( L"latin1" ) == 0 )
        cp = 1252;
    if ( encoding.CompareNoCase( L"jis" ) == 0 )
        cp = 932;
    if ( encoding.CompareNoCase( L"wansung" ) == 0 )
        cp = 949;
    if ( encoding.CompareNoCase( L"johab" ) == 0 )
        cp = 1361;
    if ( encoding.CompareNoCase( L"chinesesimp" ) == 0 )
        cp = 936;
    if ( encoding.CompareNoCase( L"chinesetrad" ) == 0 )
        cp = 950;
    if ( encoding.CompareNoCase( L"hebrew" ) == 0 )
        cp = 1255;
    if ( encoding.CompareNoCase( L"arabic" ) == 0 )
        cp = 1256;
    if ( encoding.CompareNoCase( L"greek" ) == 0 )
        cp = 1253;
    if ( encoding.CompareNoCase( L"turkish" ) == 0 )
        cp = 1254;
    if ( encoding.CompareNoCase( L"baltic" ) == 0 )
        cp = 1257;
    if ( encoding.CompareNoCase( L"latin2" ) == 0 )
        cp = 1250;
    if ( encoding.CompareNoCase( L"cyrillic" ) == 0 )
        cp = 1251;
    if ( encoding.CompareNoCase( L"thai" ) == 0 )
        cp = 874;
    if ( encoding.CompareNoCase( L"utf8" ) == 0 || encoding.CompareNoCase( L"utf-8" ) == 0 )
        cp = CP_UTF8;
    if ( encoding.CompareNoCase( L"utf16" ) == 0 || encoding.CompareNoCase( L"unicode" ) == 0 )
        cp = CP_UNICODE;
    if ( encoding.CompareNoCase( L"utf8-prefix" ) == 0 )
        cp = CP_UTF8_PREFIX;
    if ( encoding.CompareNoCase( L"utf16-prefix" ) == 0 || encoding.CompareNoCase( L"unicode-prefix" ) == 0 )
        cp = CP_UNICODE_PREFIX;
	if ( encoding.CompareNoCase( L"binary" ) == 0 )
		cp = -1;
    return cp;
}

BOOL OpenZipDll()
{
    if ( ZipDll == NULL )
    {
        ZipDll = LoadLibrary( L"zip.dll" );
	    if ( ZipDll == NULL ){
		    return FALSE;
	    }

#ifdef DESKTOP
        UnzipToFile   = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, "UnzipToFile" );
        UnzipAllToPath = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, "UnzipAllToPath" );
        UnzipPathToPath = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, "UnzipPathToPath" );
        UnzipToMemory = (void* (_cdecl *)(LPCTSTR,LPCTSTR,UINT*))GetProcAddress( ZipDll, "UnzipToMemory" );
        ZipFromFile   = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR,int))GetProcAddress( ZipDll, "ZipFromFile" );
        ZipFromMemory = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,void*,DWORD,int))GetProcAddress( ZipDll, "ZipFromMemory" );
		ZipFromFilesToZipPath  =	(BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR,BOOL,int))GetProcAddress( ZipDll, "ZipFromFilesToZipPath" );
#else
        UnzipToFile   = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, _T("UnzipToFile") );
        UnzipAllToPath = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, _T("UnzipAllToPath") );
        UnzipPathToPath = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR))GetProcAddress( ZipDll, _T("UnzipPathToPath") );
        UnzipToMemory = (void* (_cdecl *)(LPCTSTR,LPCTSTR,UINT*))GetProcAddress( ZipDll, _T("UnzipToMemory") );
        ZipFromFile   = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR,int))GetProcAddress( ZipDll, _T("ZipFromFile") );
        ZipFromMemory = (BOOL (_cdecl *)(LPCTSTR,LPCTSTR,void*,DWORD,int))GetProcAddress( ZipDll, _T("ZipFromMemory") );
		ZipFromFilesToZipPath  =	(BOOL (_cdecl *)(LPCTSTR,LPCTSTR,LPCTSTR,BOOL,int))GetProcAddress( ZipDll, _T("ZipFromFilesToZipPath") );
#endif

	    if ( UnzipToFile == NULL || UnzipToMemory == NULL || ZipFromFile == NULL || ZipFromMemory == NULL || ZipFromFilesToZipPath == NULL)
	    {
		    FreeLibrary( ZipDll );
            ZipDll = NULL;
		    return FALSE;
	    }
    }

    return TRUE;
}

void CloseZipDll()
{
    if ( ZipDll != NULL )
    {
		FreeLibrary( ZipDll );
        ZipDll = NULL;
    }
}

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
    LONGLONG ll;
    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = (DWORD)(ll >> 32);
}

void UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst)
{
    FILETIME ft;
    UnixTimeToFileTime(t, &ft);
    FileTimeToSystemTime(&ft, pst);
}

time_t FileTimeToUnixTime( const FILETIME &ft )
{
    LONGLONG t;
    t = (LONGLONG)ft.dwLowDateTime + ((LONGLONG)ft.dwHighDateTime<<32);
    t -= (LONGLONG)10000000*134774*24*60*60;
    return (time_t)(t/10000000);
}

time_t SystemTimeToUnixTime( const SYSTEMTIME &st )
{
    FILETIME ft;
    SystemTimeToFileTime( &st, &ft );
    return FileTimeToUnixTime( ft );
}

BOOL LoadToolhelp()
{
#ifndef DESKTOP
    if ( ToolhelpDll == NULL )
    {
        ToolhelpDll = LoadLibrary( L"toolhelp.dll" );
	    if ( ToolhelpDll == NULL )
	    {
			int e = GetLastError();
		    return FALSE;
	    }

        CreateToolhelp32Snapshot = (HANDLE (_cdecl*)(DWORD, DWORD))GetProcAddress( ToolhelpDll, _T("CreateToolhelp32Snapshot") );
		CloseToolhelp32Snapshot  = (BOOL (_cdecl*)(HANDLE))GetProcAddress( ToolhelpDll, _T("CloseToolhelp32Snapshot") );
		Process32First = (BOOL (_cdecl*)(HANDLE, LPPROCESSENTRY32))GetProcAddress( ToolhelpDll, _T("Process32First") );
		Process32Next  = (BOOL (_cdecl*)(HANDLE, LPPROCESSENTRY32))GetProcAddress( ToolhelpDll, _T("Process32Next") );
		Module32First = (BOOL (_cdecl*)(HANDLE, LPMODULEENTRY32))GetProcAddress( ToolhelpDll, _T("Module32First") );
		Module32Next  = (BOOL (_cdecl*)(HANDLE, LPMODULEENTRY32))GetProcAddress( ToolhelpDll, _T("Module32Next") );

		if ( CreateToolhelp32Snapshot == NULL || Process32First == NULL || Process32Next == NULL )
		{
			UnloadToolhelp();
			return FALSE;
		}
	}
#endif

	return TRUE;
}	

void UnloadToolhelp()
{
#ifndef DESKTOP
    if ( ToolhelpDll != NULL )
    {
		FreeLibrary( ToolhelpDll );
		ZipDll = NULL;
        CreateToolhelp32Snapshot = NULL;
		CloseToolhelp32Snapshot  = NULL;
		Process32First = NULL;
		Process32Next  = NULL;
		Module32First = NULL;
		Module32Next  = NULL;
    }
#endif
}

void ScreenToClient( HWND wnd, RECT *rect )
{
	POINT pt;
	pt.x = rect->left;
	pt.y = rect->top;
	ScreenToClient( wnd, &pt );
	rect->left = pt.x;
	rect->top  = pt.y;

	pt.x = rect->right;
	pt.y = rect->bottom;
	ScreenToClient( wnd, &pt );
	rect->right  = pt.x;
	rect->bottom = pt.y;
}

HWND GetMsgParent()
{
	if ( StatusDialog->DisplayStyle == 0 )
		return NULL;
	else
		return StatusWindow;
}

void SplitLines( HDC dc, LPCTSTR text, CStrArray &lines, int width )
{
	int i, pos;
	CStr subText, rest, txt;
	txt = text;

	lines.RemoveAll();
	CIniFile::Split( txt, L"\n", lines );

	SIZE size;
	GetTextExtentPoint( dc, L"W", 1, &size );
    int minWidth = size.cx;

	for ( i = 0; i < lines.GetSize(); i++ )
	{
		subText = lines[i];
		subText.TrimRight(); subText.TrimLeft();
		if ( !subText.IsEmpty() && width > minWidth )
		{
			GetTextExtentPoint( dc, subText, subText.GetLength(), &size );
			if ( size.cx > width )
			{
				pos = -1;
				int idx, maxLen = subText.GetLength()-1;
				// Calculate max displayable length
				GetTextExtentExPoint( dc, subText, subText.GetLength(),
									  width, &maxLen,
									  NULL, &size );
				idx = maxLen;
				while ( TRUE )
				{
					// look for last useful break
					for ( ; idx >= 0; idx-- )
					{
						if ( subText.GetAt(idx) == ' ' || subText.GetAt(idx) == '/' || subText.GetAt(idx) == '-' )
						{
							pos = idx;
							break;
						}
					}
					if ( pos <= 2 || idx <= 2 ) { pos = -1; break; }
					if ( subText.GetAt(pos) == ' '  )
    					subText = subText.Left( pos );
					else
						subText = subText.Left( pos+1 );

					GetTextExtentPoint( dc, subText, subText.GetLength(), &size );
					if ( size.cx <= width )
						break;
					else
						idx--;
				}
				if ( pos != -1 )
				{
					rest = lines[i].Mid( pos+1 );
					rest.TrimLeft(); rest.TrimRight();
					if ( rest.IsEmpty() == FALSE )
    					lines.InsertAt( i+1, rest );
					lines.SetAt( i, subText );
				}
				else
				{
					subText = lines[i].Left( maxLen+1 );
					while ( subText.GetLength() > 2 )
					{
						subText = subText.Left( subText.GetLength()-1 );
						GetTextExtentPoint( dc, subText, subText.GetLength(), &size );
						if ( size.cx <= width ) break;
					}
					rest = lines[i].Mid( subText.GetLength() );
					rest.TrimLeft(); rest.TrimRight();
					if ( rest.IsEmpty() == FALSE )
    					lines.InsertAt( i+1, rest );
					lines.SetAt( i, subText );
				}
			}
			else
			{
				lines.SetAt( i, subText );
			}
		}
        else
        {
            // assign trimmed text
			lines.SetAt( i, subText );
        }
	}
}

//jwz::add for GetWindowTitleFromProcName
//GetProcMainHWND begin
typedef struct tagWNDINFO
{
	DWORD dwProcId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

CStr myWindowsText;
 
BOOL CALLBACK MyEnumProc(HWND hWnd,LPARAM lParam)
{
    DWORD dwProcId;
    LPWNDINFO pInfo = (LPWNDINFO)lParam;
    
    GetWindowThreadProcessId(hWnd, &dwProcId);

    if(dwProcId == pInfo->dwProcId 
		&& ::IsWindowVisible( hWnd )
		&& ( ::GetParent( hWnd ) == NULL || ::GetParent( hWnd ) == ::GetDesktopWindow() )
		)
    {
		int len = ::GetWindowText( hWnd, windowTitle, 255 );
		pInfo->hWnd = hWnd;
		CStr tStr = windowTitle;
		tStr.TrimRight();
		if (tStr.GetLength()>0)
			myWindowsText = tStr;
		return false;
    }
    
    return TRUE;
}

CValue GetWindowFromProcess(DWORD dwProcId)
{
    WNDINFO wi;
	CValue rc=L" ";

    wi.dwProcId = dwProcId;
    wi.hWnd = NULL;

	myWindowsText = L"";
    EnumWindows(MyEnumProc,(LPARAM)&wi);

	rc = myWindowsText;
	return rc;	

} 
//GetProcMainHWND End
//jwz::add end
