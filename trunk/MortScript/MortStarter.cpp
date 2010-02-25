// MortStarter.cpp : Defines the class behaviors for the application.
//

#ifdef DESKTOP
#include "..\..\vc6\mortrunner\stdafx.h"
#include "..\..\vc6\mortrunner\resource.h"
#include "DlgChoice.h"
#include "DlgDownload.h"
#include "DlgWait.h"
#include "DlgInput.h"
#include "Parser.h"
#define KEYEVENTF_SILENT 0



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

#ifdef POCKETPC
#include "stdafx.h"
#include "resource.h"
#include "DlgChoice.h"
#include "DlgDownload.h"
#include "DlgWait.h"
#include "DlgInput.h"
#include "Parser.h"
#include "vibrate.h"
#endif

#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone\mortafx.h"
#include <vibrate.h>
#endif

#ifndef DESKTOP
#include "keybd.h"

#ifndef _i386_
void PowerOffSystem()
{
    ::keybd_event(VK_OFF, 0, 0, 0);
    ::keybd_event(VK_OFF, 0, KEYEVENTF_KEYUP, 0);
}
#else
#define PowerOffSystem();
#endif
#endif

#include "MortStarter.h"
#include "IniFile.h"
#include "Tlhelp32.h"
#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ERROR_OFF      0
#define ERROR_CRITICAL 1
#define ERROR_SYNTAX   2
#define ERROR_ERROR    3
#define ERROR_WARN     4
#define ERROR_DEBUG    5

#define IOCTL_HAL_REBOOT CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
extern "C" __declspec(dllimport) void SetCleanRebootFlag(void);
extern "C" __declspec(dllimport) BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

// For keep alive (in coredll.dll)
#ifndef DESKTOP
void WINAPI SystemIdleTimerReset(void);
#endif

#ifndef SMARTPHONE
CMapStringToString  Variables;
#endif


BOOL CALLBACK SearchWindowText( HWND hwnd, LPARAM lParam )
{
	SearchWindow *sw = (SearchWindow*)lParam;
	TCHAR windowTitle[256];

	::GetWindowText( hwnd, windowTitle, 256 );
	if ( CString(windowTitle).Find( sw->SearchText ) != -1
		&& ::IsWindowVisible( hwnd )
		)
	{
		sw->FoundWnd = hwnd;
		return FALSE;
	}
	return TRUE;
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

int
CMortStarterApp::Split( const CString &source, TCHAR sep, CStringArray &dest, BOOL trim )
{
    int pos = 0;
    int startPos = 0;
    CString elem;

    dest.RemoveAll();
    while( pos != -1 && pos < source.GetLength() )
    {
        bool quoted = false;

        while (  startPos < source.GetLength()
              && (  source.GetAt(startPos) == ' '
                 || source.GetAt(startPos) == '\t'
                 )
              )
        {
            startPos++;
        }

        if ( source.GetAt(startPos) == '\"' )
        {
            startPos++;
            pos  = source.Find( '\"', startPos );

            while ( pos != -1 && pos < source.GetLength()-2 )
            {
                int nextQuote = source.Find( '\"', pos+1 );
                if ( nextQuote != pos+1 )
                    break;
                else
                    pos = source.Find( '\"', pos+2 );
            }
            int nextSep   = source.Find( sep,  pos+1 );

            if ( pos != -1 )
            {
                elem = source.Mid( startPos, pos-startPos );
                pos  = nextSep;
            }
            else
            {
                elem = source.Mid( startPos );
            }
            elem.Replace( L"\"\"", L"\"" );
            elem.Replace( L"^CR^", L"\n" );
            elem.Replace( L"^LF^", L"\r" );
            elem.Replace( L"^TAB^", L"\t" );

            quoted = true;
        }
#ifndef SMARTPHONE
        else if ( source.GetAt(startPos) == '{' )
        {
            startPos++;
            pos  = source.Find( '}', startPos );

            if ( pos != -1 )
            {
                CString expr = source.Mid( startPos, pos-startPos );
                CParser parser( expr, &Variables );
                elem = parser.EvaluateExpression();
                if ( parser.GetError() != 0 )
                {
                    if ( ErrorLevel >= ERROR_SYNTAX )
                        MessageBox( NULL, parser.GetErrorMessage() + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    dest.RemoveAll();
                    return -1;
                }
                pos  = source.Find( sep,  pos+1 );
            }
            else
            {
                elem = source.Mid( startPos );
                pos  = -1;
            }

            quoted = true;
        }
#endif
        else
        {
            pos  = source.Find( sep, startPos );
            if ( pos != -1 )
            {
                elem = source.Mid( startPos, pos-startPos );
            }
            else
            {
                elem = source.Mid( startPos );
            }
            if ( trim ) { elem.TrimLeft(); elem.TrimRight(); }
        }

#ifndef SMARTPHONE
        if (   quoted == false
            && elem.GetLength() > 2
            && elem.GetAt( 0 ) == '%'
            && elem.GetAt( elem.GetLength()-1 ) == '%'
           )
        {
            elem = elem.Mid( 1, elem.GetLength()-2 );
            elem.MakeUpper();
            CString value;
            if ( Variables.Lookup( elem, value ) )
            {
                dest.Add( value );
            }
            else
            {
                dest.Add( L"" );
            }
        }
        else
        {
#endif
            dest.Add( elem );
#ifndef SMARTPHONE
        }
#endif
        startPos = pos+1;
    }

    return dest.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
// CMortStarterApp

#ifndef SMARTPHONE
BEGIN_MESSAGE_MAP(CMortStarterApp, CWinApp)
	//{{AFX_MSG_MAP(CMortStarterApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif

/////////////////////////////////////////////////////////////////////////////
// CMortStarterApp construction

CMortStarterApp::CMortStarterApp()
#ifndef SMARTPHONE
	: CWinApp()
#endif
{
    ErrorLevel = ERROR_ERROR;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMortStarterApp object

CMortStarterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMortStarterApp initialization

BOOL CMortStarterApp::InitInstance()
{
#ifndef SMARTPHONE
    SetRegistryKey( L"Mort" );

    ScriptFile = m_lpCmdLine;
    ScriptFile.TrimLeft( '\"' ); ScriptFile.TrimRight( '\"' );
    if (   ScriptFile.GetLength() >= 8 && ScriptFile.Right(8).CompareNoCase( L".mortrun" ) == 0
        || ScriptFile.GetLength() >= 5 && ScriptFile.Right(5).CompareNoCase( L".mscr" ) == 0
       )
    {
		RunFile( ScriptFile );
    }

    if ( ScriptFile.IsEmpty() )
    {
        RegisterFileClass();
        RegisterFileType( L".mortrun", L"MortScript" );
        RegisterFileType( L".mscr", L"MortScript" );
        MessageBox( NULL
                  , L".mscr and .mortrun extensions registered.\nPlease run any .mscr/.mortrun file or read the manual.\n\n"
                    L"(c) Mirko Schenk 2005"
                  , L"MortScript V3.2b4"
                  , MB_OK|MB_SETFOREGROUND );
    }
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


#ifndef SMARTPHONE
void CMortStarterApp::RegisterFileClass()
{
	HKEY    key, subKey;
	DWORD   disposition;
	CString label;

	if ( RegCreateKeyEx( HKEY_CLASSES_ROOT
					   , L"MortScript", 0
					   , L"MortScript", 0
#ifndef DESKTOP
             , 0
#else
             , KEY_WRITE
#endif
             , NULL, &key, &disposition ) == ERROR_SUCCESS )
	{
		label = L"MortScript";
		RegSetValueEx( key, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(USHORT)*(label.GetLength()+1) );

		if ( RegCreateKeyEx( key
						   , L"DefaultIcon", 0
						   , L"DefaultIcon", 0
#ifndef DESKTOP
               , 0
#else
               , KEY_WRITE
#endif
               , NULL, &subKey, &disposition ) == ERROR_SUCCESS )
		{
			label = theApp.m_pszHelpFilePath;
			//label = label.Left( label.GetLength()-3 ) + L"exe,-142";
#ifndef DESKTOP
			label.Format( L"%s.exe,-%d", (LPCTSTR)label.Left( label.GetLength()-4 ), IDI_RUNFILE );
#else
			label.Format( L"%s.exe,1", (LPCTSTR)label.Left( label.GetLength()-4 ) );
#endif
			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(USHORT)*(CString(label).GetLength()+1) );
			RegCloseKey( subKey );
		}

		if ( RegCreateKeyEx( key
						   , L"Shell\\Open\\Command", 0
						   , L"Command", 0
#ifndef DESKTOP
               , 0
#else
               , KEY_WRITE
#endif
               , NULL, &subKey, &disposition ) == ERROR_SUCCESS )
		{
			label = L"\"" + GetRelativeToAppPath( L"MortScript.exe\" \"%1\"" );
			//label = L"\"" + label.Left( label.GetLength()-3 ) + L"exe\" \"%1\"";
			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(TCHAR)*(CString(label).GetLength()+1) );
			RegCloseKey( subKey );
		}
		RegCloseKey( key );
	}
}

void CMortStarterApp::RegisterFileType( LPCTSTR type, LPCTSTR typeClass )
{
	HKEY    key;
	DWORD   disposition;
	CString label;

	if ( RegCreateKeyEx( HKEY_CLASSES_ROOT
					   , (LPTSTR)type, 0
					   , (LPTSTR)type, 0
#ifndef DESKTOP
             , 0
#else
             , KEY_WRITE
#endif
             , NULL, &key, &disposition ) == ERROR_SUCCESS )
	{
		RegSetValueEx( key, NULL, NULL, REG_SZ, (BYTE*)typeClass, sizeof(TCHAR)*(_tcslen(typeClass)+1) );
		RegCloseKey( key );
	}
}

CString CMortStarterApp::GetRelativeToAppPath( LPCTSTR file )
{
	CString path = m_pszHelpFilePath;
	path = path.Left( path.ReverseFind('\\') + 1 ) + file;
	return path;
}
#endif


HKEY CMortStarterApp::GetTopKey( CString &topkey )
{
    HKEY root = NULL;

    if ( topkey.CompareNoCase( L"HKCU" ) == 0 ) root = HKEY_CURRENT_USER;
    if ( topkey.CompareNoCase( L"HKLM" ) == 0 ) root = HKEY_LOCAL_MACHINE;
    if ( topkey.CompareNoCase( L"HKCR" ) == 0 ) root = HKEY_CLASSES_ROOT;
    if ( topkey.CompareNoCase( L"HKUS" ) == 0 ) root = HKEY_USERS;

    return root;
}

CString CMortStarterApp::RegRead( HKEY topkey, LPCTSTR path, LPCTSTR value )
{
	BOOL    result = FALSE;
	HKEY    key;
	CString label;

	if ( RegOpenKeyEx( topkey, path, 0, 0, &key ) == ERROR_SUCCESS )
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

DWORD CMortStarterApp::RegReadDW( HKEY topkey, LPCTSTR path, LPCTSTR value )
{
	BOOL    result = FALSE;
	HKEY    key;
	DWORD   content = 0;

	if ( RegOpenKeyEx( topkey, path, 0, 0, &key ) == ERROR_SUCCESS )
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

void CMortStarterApp::GetExeForDoc( const CString &doc, CString &exe )
{
	CString ext = doc.Mid( doc.ReverseFind( '.' ) );

	CString appID = RegRead( HKEY_CLASSES_ROOT, ext, NULL );
	exe = RegRead( HKEY_CLASSES_ROOT, appID + L"\\Shell\\Open\\Command", NULL );
}


void CMortStarterApp::GetExe( const CString &appAndParam, CString &exe, CString &param )
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

void CMortStarterApp::GetRunData( CString &exe, CString &param )
{
	// "Normal" links...
	if ( exe.Right(4).CompareNoCase( L".exe" ) != 0 )
	{
		CString docRun, docExe;

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

BOOL CMortStarterApp::RunApp( CString &param, BOOL wait )
{
    CStringArray params;
    BOOL rc = TRUE;

    int parCnt = Split( param, ',', params );
    if ( parCnt != 1 && parCnt != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Run'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ! wait && params[0].Right(4).CompareNoCase(L".lnk") == 0 )
    {
        SHELLEXECUTEINFO sei;
        sei.cbSize = sizeof(SHELLEXECUTEINFO);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd  = NULL;
        sei.lpVerb = L"Open";
        sei.lpFile = (LPCTSTR)params[0];
        sei.lpDirectory = L"\\";
        sei.lpParameters = NULL;
        if ( parCnt == 2 )
            sei.lpParameters = (LPCTSTR)params[1];
        sei.nShow = SW_SHOW;
        sei.hInstApp = NULL;

        rc = ShellExecuteEx( &sei ) != 0;

        if ( rc ) CloseHandle( sei.hProcess );
    }
    else
    {
        PROCESS_INFORMATION inf;
        CString exe = params[0];
        CString param;
        if ( parCnt == 2 ) param = params[1];
        GetRunData( exe, param );
        if ( ! exe.IsEmpty() )
        {
            rc = CreateProcess( (LPCTSTR)exe, (param.IsEmpty()) ? NULL : (LPTSTR)(LPCTSTR)param, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &inf );

            if ( rc )
            {
                if ( wait )
                {
                    DWORD exitCode;
                    while ( GetExitCodeProcess( inf.hProcess, &exitCode ) != FALSE && exitCode == STILL_ACTIVE )
                    {
                        ::Sleep( 100 );
                    }
                }
                CloseHandle( inf.hProcess );
            }
        }
    }

    return TRUE;
}

#ifdef POCKETPC
#ifndef PNA
BOOL CMortStarterApp::New( CString &param )
{
    CStringArray params;
    BOOL rc = TRUE;

    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'New'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    // Works only in WM2003 and later
    OSVERSIONINFO ver;
    GetVersionEx( &ver );
    if ( ( ver.dwMajorVersion > 4 || (ver.dwMajorVersion == 4 && ver.dwMinorVersion >= 20) ) )
    {
        // Search "New" item in Registry
        BOOL found = FALSE;
	    HKEY    key, newItemKey;
   	    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"\\SOFTWARE\\Microsoft\\Shell\\Extensions\\NewMenu", 0, 0, &key ) == ERROR_SUCCESS )
	    {
            TCHAR subKeyName[50];
            DWORD len = 50;
            for ( int i=0; RegEnumKeyEx( key, i, subKeyName, &len, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS; i++ )
            {
   	            if ( RegOpenKeyEx( key, subKeyName, 0, 0, &newItemKey ) == ERROR_SUCCESS )
	            {
		            DWORD type, length;
		            TCHAR cont[MAX_PATH];
		            length = MAX_PATH;
		            if ( RegQueryValueEx( newItemKey, NULL, NULL, &type, (BYTE*)cont, &length ) == ERROR_SUCCESS )
		            {
                        if ( params[0] == cont )
                        {
                            // Found! Now covert the key name to clsid and create the item
                            found = TRUE;
                            CLSID clsid;
                            CLSIDFromString( subKeyName, &clsid );
                            if ( SHCreateNewItem( NULL, clsid ) != NOERROR )
                            {
                                if ( ErrorLevel >= ERROR_ERROR )
                                {
                                    MessageBox( NULL, L"New document could not be created" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                                    rc = FALSE;
                                }
                            }
                        }
                    }
                    RegCloseKey( newItemKey );
                }
                len = 50;
		    }
		    RegCloseKey( key );
            if ( found == FALSE )
            {
                if ( ErrorLevel >= ERROR_ERROR )
                {
                    MessageBox( NULL, L"New document could not be created" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    rc = FALSE;
                }
            }
	    }
    }
    else
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            MessageBox( NULL, L"'New' requires WM2003" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}
#endif
#endif

BOOL CMortStarterApp::Kill( CString &param )
{
    BOOL rc = TRUE;

    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Kill'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HANDLE         procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof( procEntry );

    if ( procSnap != NULL && Process32First( procSnap, &procEntry ) )
    {
        do
        {
            CString procName = procEntry.szExeFile;
            if ( procName.CompareNoCase( params[0] ) == 0 )
            {
                HANDLE hProc = ::OpenProcess( 0, FALSE, procEntry.th32ProcessID ); 
                if ( hProc != NULL )
                {
                    TerminateProcess( hProc, 0 );
                    CloseHandle( hProc );
                }
            }
			procEntry.dwSize = sizeof( procEntry );
        }
        while ( Process32Next( procSnap, &procEntry ) );
    }
    if ( procSnap != NULL )
#ifdef DESKTOP
      CloseHandle( procSnap );
#else
      CloseToolhelp32Snapshot( procSnap );
#endif

    return rc;
}

HWND CMortStarterApp::FindWindow( CString &window )
{
	HWND wnd; // = ::FindWindow( NULL, (LPCTSTR)window );
	//if ( wnd == NULL )
	//{
		SearchWindow sw;
		sw.SearchText = window;
		sw.FoundWnd   = NULL;
		if ( ::EnumWindows( SearchWindowText, (LPARAM)&sw ) )
			wnd = sw.FoundWnd;
	//}

	return wnd;
}

BOOL CMortStarterApp::Show( CString &param )
{
    BOOL rc = TRUE;

    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Show'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindow( params.GetAt(0) );

	if ( wnd != NULL )
	{
#ifdef DESKTOP
        ::ShowWindow( wnd, SW_SHOW );
#endif
		::SetForegroundWindow(wnd);
	}
    else
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::Minimize( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Minimize'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindow( params.GetAt(0) );

	if ( wnd != NULL )
	{
		::ShowWindow( wnd, SW_MINIMIZE );
	}
    else
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::SendKey( CString &window, USHORT ch, BOOL shift, BOOL ctrl, BOOL alt )
{
    BOOL rc = TRUE;
    if ( ! window.IsEmpty() )
    {
	    HWND wnd = FindWindow( window );
    	if ( wnd != NULL )
        {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        }
        else
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            else
                return TRUE;
        }
    }

    if ( shift ) keybd_event( VK_SHIFT,   NULL, KEYEVENTF_SILENT, NULL );
    if ( ctrl  ) keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT, NULL );
    if ( alt   ) keybd_event( VK_MENU,    NULL, KEYEVENTF_SILENT, NULL );
#ifdef DESKTOP
    ::Sleep( 100 );
#endif
	keybd_event( (BYTE)ch, NULL, KEYEVENTF_SILENT, NULL );
	keybd_event( (BYTE)ch, NULL, KEYEVENTF_SILENT|KEYEVENTF_KEYUP, NULL );
    if ( alt   ) keybd_event( VK_MENU,    NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL );
    if ( ctrl  ) keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 
    if ( shift ) keybd_event( VK_SHIFT,   NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 
	//::SendMessage( wnd, WM_KEYDOWN, ch, 1 );
	//::SendMessage( wnd, WM_KEYUP, ch, 3221225473 );

    return rc;
}

BOOL CMortStarterApp::SendChar( CString &window, USHORT ch )
{
    BOOL rc = TRUE;
    HWND wnd;
    if ( ! window.IsEmpty() )
    {
	    wnd = FindWindow( window );
    	if ( wnd != NULL )
        {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        }
        else
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            else
                return TRUE;
        }
    }
    else
    {
        wnd = ::GetForegroundWindow();
    }

    UINT st = 0, cb = ch;
#ifndef DESKTOP
    PostKeybdMessage( (HWND)-1, 0, KeyStateToggledFlag, 1, &st, &cb );
    PostKeybdMessage( (HWND)-1, 0, KeyStateToggledFlag, 1, &st, &cb );
#else
    SendKey( window, VkKeyScan( ch ), ((VkKeyScan( ch ) & 256) != 0), ((VkKeyScan( ch ) & 512) != 0), ((VkKeyScan( ch ) & 1024) != 0) );
#endif

    ::Sleep(100);

    return rc;
}

BOOL CMortStarterApp::SendSpecialKey( CString &cmd, CString &param )
{
    BOOL rc = 2;    // Unknown command
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 0 && params.GetSize() != 1 && params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            CString msg;
            msg.Format( L"Invalid parameter count for '%s'", (LPCTSTR)cmd );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CString wnd;
    if ( params.GetSize() == 1 || params.GetSize() >= 3 )
        wnd = params[0];

    BOOL ctrl=FALSE, shift=FALSE, alt=FALSE;
    if ( params.GetSize() == 3 )
    {
        ctrl  = _wtol( params[1] );
        shift = _wtol( params[2] );
    }
    if ( params.GetSize() == 4 )
    {
        alt = _wtol( params[2] );
    }
    else
    {
        alt = 0;
    }

	if ( cmd.CompareNoCase( L"SendCR" ) == 0 )
        rc = SendKey( wnd, VK_RETURN, shift, ctrl, alt );
	
    if ( cmd.CompareNoCase( L"SendTab" ) == 0 )
        rc = SendKey( wnd, VK_TAB, shift, ctrl, alt );
	
    if ( cmd.CompareNoCase( L"SendEsc" ) == 0 )
        rc = SendKey( wnd, VK_ESCAPE, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendSpace" ) == 0 )
        rc = SendKey( wnd, VK_SPACE, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendUp" ) == 0 )
        rc = SendKey( wnd, VK_UP, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendDown" ) == 0 )
        rc = SendKey( wnd, VK_DOWN, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendLeft" ) == 0 )
        rc = SendKey( wnd, VK_LEFT, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendRight" ) == 0 )
        rc = SendKey( wnd, VK_RIGHT, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendHome" ) == 0 )
        rc = SendKey( wnd, VK_HOME, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendEnd" ) == 0 )
        rc = SendKey( wnd, VK_END, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendDelete" ) == 0 )
        rc = SendKey( wnd, VK_DELETE, shift, ctrl, alt );

    if ( cmd.CompareNoCase( L"Snapshot" )  == 0 )
        rc = SendKey( wnd, VK_SNAPSHOT, shift, ctrl, alt );

    return rc;
}

BOOL CMortStarterApp::SendKeys( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SendKeys'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CString wnd, keys;
    if ( params.GetSize() == 1 )
    {
        wnd.Empty();
        keys = params[0];
    }
    else
    {
        wnd  = params[0];
        keys = params[1];
    }

    for ( int i=0; i<keys.GetLength(); i++ )
        rc = SendChar( wnd, keys.GetAt(i) );

    return rc;
}

BOOL CMortStarterApp::SendCtrlKey( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SendCtrlKey'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    int start = 0; CString window;
    if ( params.GetSize() == 2 )
    {
        window = params[0];
        start = 1;
    }

    if ( params[start].GetLength() != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid length for character in 'SendCtrlKey'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ! window.IsEmpty() )
    {
	    HWND wnd = FindWindow( window );
    	if ( wnd != NULL )
        {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        }
        else
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            else
                return TRUE;
        }
    }

#ifndef DESKTOP
    KEY_STATE_FLAGS st = KeyShiftAnyCtrlFlag | KeyStateDownFlag;
#endif
    UINT nChar = params[start].GetAt(0);
    if ( nChar >= 'A' && nChar <= 'Z' )
        nChar -= 'A'-1;
    else if ( nChar >= 'a' && nChar <= 'z' )
        nChar -= 'a'-1;
    else
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid character for 'SendCtrlKey'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( nChar != -1 )
    {
#ifndef DESKTOP
        keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT, NULL );
        PostKeybdMessage( (HWND)-1, nChar+64, KeyShiftAnyCtrlFlag | KeyStateDownFlag, 1, &st, &nChar );
        st = KeyShiftAnyCtrlFlag | KeyShiftNoCharacterFlag;
        PostKeybdMessage( (HWND)-1, nChar+64, KeyShiftAnyCtrlFlag | KeyShiftNoCharacterFlag, 1, &st, &nChar );
        keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 
#else
        CString dummy;
        SendKey( dummy, VkKeyScan(nChar+64), 0, 1 );
#endif
    }

    return rc;
}

BOOL CMortStarterApp::WaitFor( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'WaitFor'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	ULONG   wait   = _wtol( params.GetAt(1) ) * 2;
	CString window = params.GetAt(0);

	ULONG i;
	HWND wnd = NULL;
	for ( i=0; wnd == NULL && i <= wait; i++ )
	{
		if ( i > 0 ) ::Sleep( 500 );

		wnd = FindWindow( window );
	}

    if ( wnd == NULL )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::WaitForActive( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'WaitForActive'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	ULONG   wait   = _wtol( params.GetAt(1) ) * 2;
	CString window = params.GetAt(0);

	ULONG i;
	HWND wnd = NULL;
	for ( i=0; wnd == NULL && i <= wait; i++ )
	{
		if ( i > 0 ) ::Sleep( 500 );

		HWND hwnd = ::GetForegroundWindow();
		TCHAR windowTitle[256];
		::GetWindowText( hwnd, windowTitle, 256 );
		if ( CString(windowTitle).Find( window ) != -1 )
		{
			wnd = hwnd;
		}
	}

    if ( wnd == NULL )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::Close( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Show'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND wnd = FindWindow( params.GetAt(0) );

	if ( wnd != NULL )
	{
		SendMessage( wnd, WM_CLOSE, 0, 0 );
	}
    else
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::Sleep( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Sleep'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	::Sleep( _wtol( params[0] ) );

    return TRUE;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::SleepMessage( CString &param )
{
    CDlgWait dlg;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SleepMessage'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CString msg;
	msg.Format ( L"%s %d", (LPCTSTR)params[1], _wtol( params[0] ) );
	MessageBox( NULL, msg, L"Debug", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );

    dlg.Countdown = _wtol( params[0] );
    dlg.m_Label   = params[1];
    if ( params.GetSize() >= 3 )
        dlg.Title       = params[2];
    if ( params.GetSize() == 4 )
        dlg.AllowOK     = _wtol(params[3]);
    dlg.DoModal();

    return TRUE;
}


BOOL CMortStarterApp::GetTime( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;

    Split( param, ',', params );

    if ( params.GetSize() < 1 || ( params.GetSize() > 3 && params.GetSize() != 6 ) )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetTime'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CString varName = params.GetAt(0);
    varName.MakeUpper();
    CTime   now = CTime::GetCurrentTime();
    
    if ( params.GetSize() == 1 )
    {
        CString val;
        val.Format( L"%d", now.GetTime() );
        Variables.SetAt( varName, val );
    }

    if ( params.GetSize() == 2 )
    {
        CString hour24, hour12, ampm, minute, second, day, month, year, wday;
        
        CString time = params.GetAt(1);
        hour24.Format( L"%d", now.GetHour() );
        int hour = now.GetHour() % 12;
        if ( hour == 0 ) hour = 12;
        hour12.Format( L"%d", hour );
        minute.Format( L"%02d", now.GetMinute() );
        second.Format( L"%02d", now.GetSecond() );
        ampm = (now.GetHour() < 12) ? L"AM" : L"PM";
        day.Format( L"%02d", now.GetDay() );
        month.Format( L"%02d", now.GetMonth() );
        year.Format( L"%02d", now.GetYear() );
        wday.Format( L"%d", now.GetDayOfWeek() );

        time.Replace( L"H", hour24 );
        time.Replace( L"h", hour12 );
        time.Replace( L"i", minute );
        time.Replace( L"s", second );
        time.Replace( L"A", ampm );
        ampm.MakeLower();
        time.Replace( L"a", ampm );
        time.Replace( L"d", day );
        time.Replace( L"m", month );
        time.Replace( L"Y", year );
        year = year.Mid(2);
        time.Replace( L"y", year );
        time.Replace( L"w", wday );

        Variables.SetAt( varName, time );
    }

    if ( params.GetSize() == 3 || params.GetSize() == 6 )
    {
        CString val;
        val.Format( L"%d", now.GetHour() );
        Variables.SetAt( params.GetAt(0), val );

        val.Format( L"%d", now.GetMinute() );
        varName = params.GetAt(1);
        varName.MakeUpper();
        Variables.SetAt( varName, val );

        val.Format( L"%d", now.GetSecond() );
        varName = params.GetAt(2);
        varName.MakeUpper();
        Variables.SetAt( varName, val );
    }

    if ( params.GetSize() == 6 )
    {
        CString val;
        val.Format( L"%d", now.GetDay() );
        varName = params.GetAt(3);
        varName.MakeUpper();
        Variables.SetAt( varName, val );

        val.Format( L"%d", now.GetMonth() );
        varName = params.GetAt(4);
        varName.MakeUpper();
        Variables.SetAt( varName, val );

        val.Format( L"%d", now.GetYear() );
        varName = params.GetAt(5);
        varName.MakeUpper();
        Variables.SetAt( varName, val );
    }

    return rc;
}


BOOL CMortStarterApp::GetColorAt( CString &param )
{
    CStringArray params;

    if ( Split( param, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetColorAt'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HDC hScreenDC  = ::GetWindowDC(0);
    COLORREF col = ::GetPixel( hScreenDC, _wtol(params[0]), _wtol(params[1]) );
    ::ReleaseDC(0, hScreenDC); 

    CString value;
    value.Format( L"%d", col );
    params[2].MakeUpper();
    Variables.SetAt( params[2], value );

    return TRUE;
}


BOOL CMortStarterApp::GetRGB( CString &param )
{
    CStringArray params;

    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RGB'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CString value;
    value.Format( L"%d", RGB(_wtol(params[0]), _wtol(params[1]), _wtol(params[2])) );
    params[3].MakeUpper();
    Variables.SetAt( params[3], value );

    return TRUE;
}


BOOL CMortStarterApp::GetWindowText( CString &param )
{
    CStringArray params;

    if ( Split( param, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetWindowText'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;
    pt.x = _wtol(params[0]);
    pt.y = _wtol(params[1]);
    HWND wnd = ::WindowFromPoint( pt );
    TCHAR value[MAX_PATH];
    ::GetWindowText( wnd, value, MAX_PATH );

    params[2].MakeUpper();
    Variables.SetAt( params[2], value );

    return TRUE;
}
#endif

BOOL CMortStarterApp::MkDir( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MkDir'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    if (   ( attribs == -1 || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0 )
        && CreateDirectory( params.GetAt(0), NULL ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't create directory '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::RmDir( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RmDir'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params.GetAt(0) );
    if (   attribs != -1 && (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0
        && RemoveDirectory( params.GetAt(0) ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't remove directory '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CMortStarterApp::Delete( CString &param, BOOL recursive )
{
    BOOL rc = TRUE;
    CStringArray params;
	CString			path;
	WIN32_FIND_DATA findFileData;
    CStringArray    directories;

    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Delete'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    path = params[0].Left(params[0].ReverseFind('\\')+1);
	HANDLE ffh = FindFirstFile( params[0], &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
        do
        {
            CString file = path+findFileData.cFileName;
            if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
            {
                if ( DeleteFile( file ) == 0 )
                {
                    if ( ErrorLevel >= ERROR_WARN )
                    {
                        CString msg;
                        msg.Format( L"Couldn't delete '%s'", (LPCTSTR)file );
                        MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    }
                }
            }
            else
            {
                if ( recursive )
                {
                    directories.Add( file );
                }
            }
        }
		while ( FindNextFile( ffh, &findFileData ) == TRUE );

		FindClose( ffh );
	}

    if ( recursive )
    {
        CString filter, pathAndFilter, dir;
        dir    = params.GetAt(0).Left( params.GetAt(0).ReverseFind( '\\' ) );
        filter = params.GetAt(0).Mid( params.GetAt(0).ReverseFind( '\\' ) );

        for ( int i=0; i<directories.GetSize(); i++ )
        {
            pathAndFilter = directories.GetAt(i) + filter;
            Delete( pathAndFilter, TRUE );
        }

        RemoveDirectory( dir );
    }

    return rc;
}

CString Proxy = L"";

BOOL CMortStarterApp::SetProxy( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SetProxy'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    Proxy = params[0];

    return TRUE;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::Download( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Download'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CDlgDownload dlg;
    dlg.Source = params[0];
    dlg.Target = params[1];
    dlg.DoModal();

	if ( dlg.Result != 0 )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
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

            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}
#endif

BOOL CMortStarterApp::Copy( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Copy'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL failIfExists = ( params.GetSize() == 2 ) ? TRUE : ( _wtoi( params.GetAt(2) ) == 0 );

	if ( CopyFile( params.GetAt(0), params.GetAt(1), failIfExists ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't copy '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CMortStarterApp::XCopy( CString &param )
{
    BOOL rc = TRUE;
    CStringArray    params;
	CString			path;
	WIN32_FIND_DATA findFileData;

    Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'XCopy'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL failIfExists = ( params.GetSize() == 2 ) ? TRUE : ( _wtoi( params.GetAt(2) ) == 0 );

    path = params[0].Left(params[0].ReverseFind('\\')+1);
	HANDLE ffh = FindFirstFile( params[0], &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
        do
        {
            CString source = path+findFileData.cFileName;
            CString target = params[1]+L"\\"+findFileData.cFileName;
	        if ( CopyFile( source, target, failIfExists ) == FALSE )
            {
                if ( ErrorLevel >= ERROR_WARN )
                {
                    CString msg;
                    msg.Format( L"Couldn't copy '%s' to '%s'", (LPCTSTR)source, (LPCTSTR)target );
                    MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                }
            }
        }
		while ( FindNextFile( ffh, &findFileData ) == TRUE );

		FindClose( ffh );
	}

    return rc;
}

BOOL CMortStarterApp::Rename( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Rename'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }


    if ( params.GetSize() == 3 && _wtoi( params.GetAt(2) ) )
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
            DeleteFile( params.GetAt(1) );
#ifndef SMARTPHONE
        }
#endif
    }

	if ( MoveFile( params.GetAt(0), params.GetAt(1) ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't rename/move '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }

    return rc;
}

BOOL CMortStarterApp::Move( CString &param )
{
    BOOL rc = TRUE;
    CStringArray    params;
	CString			path;
	WIN32_FIND_DATA findFileData;

    Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Move'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    BOOL overwrite = ( params.GetSize() == 2 ) ? FALSE : ( _wtoi( params.GetAt(2) ) != 0 );

    path = params[0].Left(params[0].ReverseFind('\\')+1);
	HANDLE ffh = FindFirstFile( params[0], &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
        do
        {
            CString source = path+findFileData.cFileName;
            CString target = params[1]+L"\\"+findFileData.cFileName;
            if (   overwrite
                && ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0
                && ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) == 0
                && ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) == 0
               )
            {
                DeleteFile( target );
            }

	        if ( MoveFile( source, target ) == FALSE )
            {
                if ( ErrorLevel >= ERROR_WARN )
                {
                    CString msg;
                    msg.Format( L"Couldn't rename/move '%s' to '%s'", (LPCTSTR)source, (LPCTSTR)target );
                    MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                }
            }
        }
		while ( FindNextFile( ffh, &findFileData ) == TRUE );

		FindClose( ffh );
	}

    return rc;
}

BOOL CMortStarterApp::CreateShortcut( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'CreateShortcut'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

#ifndef DESKTOP
    if ( params.GetAt(1).GetAt(0) != '"' )
    {
        params[1] = L"\"" + params[1] + L"\"";
    }
	if ( SHCreateShortcut( (LPTSTR)(LPCTSTR)params.GetAt(0), (LPTSTR)(LPCTSTR)params.GetAt(1) ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't create shortcut '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }
#else
    if ( CreateLink(params.GetAt(1), (LPTSTR)(LPCTSTR)params.GetAt(0), L"") != 0 )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't create shortcut '%s' to '%s'", (LPCTSTR)params.GetAt(0), (LPCTSTR)params.GetAt(1) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }
#endif

    return rc;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::GetVersion( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 2 && params.GetSize() != 5 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetVersion'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD size, dummy;
    size = GetFileVersionInfoSize( (LPTSTR)(LPCTSTR)params[0], &dummy );
    if ( size == 0 )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't get version info for '%s'", (LPCTSTR)params.GetAt(0) );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
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
        CString ver;
        ver.Format( L"%d.%d.%d.%d", HIWORD(ms), LOWORD(ms), HIWORD(ls), LOWORD(ls) );
        CString var = params[1];
        var.MakeUpper();
        Variables.SetAt( var, ver );
    }
    else
    {
        CString ver;
        ver.Format( L"%d", HIWORD(ms) );
        CString var = params[1];
        var.MakeUpper();
        Variables.SetAt( var, ver );

        ver.Format( L"%d", LOWORD(ms) );
        var = params[2];
        var.MakeUpper();
        Variables.SetAt( var, ver );

        ver.Format( L"%d", HIWORD(ls) );
        var = params[3];
        var.MakeUpper();
        Variables.SetAt( var, ver );

        ver.Format( L"%d", LOWORD(ls) );
        var = params[4];
        var.MakeUpper();
        Variables.SetAt( var, ver );
    }

    return rc;
}
#endif

DWORD CMortStarterApp::MouseEvent( BYTE button, BOOL release )
{
    DWORD event;

    switch ( button )
    {
    case 1:
        event = release ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
        break;

    case 2:
        event = release ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN;
        break;

    default:
        event = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
        break;
    }

    return event;
}

BOOL CMortStarterApp::MouseClick( CString &param, BOOL dbl, BYTE button )
{
    BOOL rc = TRUE;
    CStringArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            if ( dbl )
                MessageBox( NULL, L"Invalid parameter count for 'MouseDblClick'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            else
                MessageBox( NULL, L"Invalid parameter count for 'MouseClick'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    POINT pt;

    if ( parNo == 3 )
    {
        RECT parrect;
    	HWND parwnd = FindWindow( params.GetAt(0) );
        if ( parwnd == NULL )
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x = _ttoi(params[1])+parrect.left;
        pt.y = _ttoi(params[2])+parrect.top;
    }
    else
    {
        pt.x = _ttoi(params[0]);
        pt.y = _ttoi(params[1]);
    }

#ifdef DESKTOP
    mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
#endif
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    if ( dbl )
    {
        mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
        mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    }
    /*
    wnd = WindowFromPoint( pt );
    ::GetWindowRect( wnd, &rect );
    ::PostMessage( wnd, WM_LBUTTONDOWN, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    if ( dbl )
        ::PostMessage( wnd, WM_LBUTTONDBLCLK, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    ::PostMessage( wnd, WM_LBUTTONUP,   0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    */

    return rc;
}

USHORT MouseX, MouseY;

BOOL CMortStarterApp::MouseDown( CString &param, BYTE button )
{
    BOOL rc = TRUE;
    CStringArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MouseDown'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;

    if ( parNo == 3 )
    {
        RECT parrect;
    	HWND parwnd = FindWindow( params.GetAt(0) );
        if ( parwnd == NULL )
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x = _ttoi(params[1])+parrect.left;
        pt.y = _ttoi(params[2])+parrect.top;
    }
    else
    {
        pt.x = _ttoi(params[0]);
        pt.y = _ttoi(params[1]);
    }

    MouseX = pt.x; MouseY = pt.y;
#ifdef DESKTOP
    mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
#endif
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    //wnd = WindowFromPoint( pt );
    //::GetWindowRect( wnd, &rect );
    //::PostMessage( wnd, WM_LBUTTONDOWN, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );

    return rc;
}

BOOL CMortStarterApp::MouseUp( CString &param, BYTE button )
{
    BOOL rc = TRUE;
    CStringArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MouseUp'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;
    pt.x = _ttoi(params[0]);
    pt.y = _ttoi(params[1]);

    if ( parNo == 3 )
    {
        RECT parrect;
    	HWND parwnd = FindWindow( params.GetAt(0) );
        if ( parwnd == NULL )
        {
            if ( ErrorLevel >= ERROR_ERROR )
            {
                CString msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x += _ttoi(params[1])+parrect.left;
        pt.y += _ttoi(params[2])+parrect.top;
    }
    else
    {
        pt.x = _ttoi(params[0]);
        pt.y = _ttoi(params[1]);
    }

    if ( MouseX != pt.x && MouseY != pt.y )
    {
      mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    }
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    //wnd = WindowFromPoint( pt );
    //::GetWindowRect( wnd, &rect );
    //::PostMessage( wnd, WM_LBUTTONUP,   0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );

    return rc;
}

BOOL CMortStarterApp::RegWriteString( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegWriteString'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    if ( RegWrite( root, params[1], params[2], params[3] ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

BOOL CMortStarterApp::RegWriteDWord( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegWriteDWord'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    if ( RegWriteDW( root, params[1], params[2], _ttoi( params[3] ) ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't write value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::RegReadString( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegReadString'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CString value = RegRead( root, params[1], params[2] );

    params[3].MakeUpper();
    Variables.SetAt( params[3], value );

    return rc;
}

BOOL CMortStarterApp::RegReadDWord( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegReadDWord'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CString value;
    value.Format( L"%d", RegReadDW( root, params[1], params[2] ) );
    value.TrimLeft();

    params[3].MakeUpper();
    Variables.SetAt( params[3], value );

    return rc;
}
#endif

BOOL CMortStarterApp::RegDelete( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegDelete'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

	HKEY    key;
    rc = FALSE;
	if ( RegOpenKeyEx( root, params[1], 0, 0, &key ) == ERROR_SUCCESS )
	{
        if ( RegDeleteValue( key, params[2] ) == ERROR_SUCCESS )
            rc = TRUE;
		RegCloseKey( key );
	}
    if ( rc == FALSE )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't remove value in %s\\%s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1], (LPCTSTR)params[2] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }


    return rc;
}

void CMortStarterApp::RegDeleteSubKey( HKEY key, LPCTSTR subkeyName, BOOL values, BOOL recursive )
{
    HKEY subkey;

    int idx;

	if ( RegOpenKeyEx( key, subkeyName, 0, 0, &subkey ) == ERROR_SUCCESS )
    {
        TCHAR subName[MAX_PATH];
        DWORD subSize = MAX_PATH, type;
        CStringArray subKeys;

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
                RegDeleteSubKey( subkey, subKeys[i], values, recursive );
                ::RegDeleteKey( subkey, subKeys[i] );
            }
        }

		RegCloseKey( subkey );
	}
}

BOOL CMortStarterApp::RegDeleteKey( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'RegDeleteKey'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( params[1].GetLength() < 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"RegDeleteKey: Registry key mustn't be empty!" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    rc = FALSE;
    if ( _wtol( params[2] ) == 1 || _wtol( params[3] ) == 1 )
    {
        RegDeleteSubKey( root, params[1], _wtol( params[2] ), _wtol( params[3] ) );
    }

    if ( ::RegDeleteKey( root, params[1] ) )
        rc = TRUE;

    if ( rc == FALSE )
    {
        if ( ErrorLevel >= ERROR_WARN )
        {
            CString msg;
            msg.Format( L"Couldn't remove registry key %s\\%s", (LPCTSTR)params[0], (LPCTSTR)params[1] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
    }


    return rc;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::IniRead( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'IniRead'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CIniFile ini;
    if ( ini.Read( params[0] ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            CString msg;
            msg.Format( L"File '%s' not found", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CString value = ini.GetString( params[1], params[2] );

    params[3].MakeUpper();
    Variables.SetAt( params[3], value );

    return rc;
}

BOOL CMortStarterApp::ReadFile( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'ReadFile'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CString content;
    int err;
    if ( params[0].Left(5) == L"http:" || params[0].Left(4) == L"ftp:" )
    {
        err = CIniFile::ReadWebFile( params[0], content );
    }
    else
    {
        err = CIniFile::ReadFile( params[0], content );
    }

    if ( err != 0 )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
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

            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    params[1].MakeUpper();
    Variables.SetAt( params[1], content );

    return rc;
}
#endif

BOOL CMortStarterApp::WriteFile( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'WriteFile'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( CIniFile::WriteFile( params[0], params[1] ) == FALSE )
    {
        if ( ErrorLevel >= ERROR_ERROR )
        {
            CString msg;
            msg.Format( L"Couldn't write file '%s'", (LPCTSTR)params[0] );
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    return rc;
}

#ifndef DESKTOP
BOOL CMortStarterApp::Rotate( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Rotate'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    OSVERSIONINFO ver;
    GetVersionEx( &ver );
    if ( ( ver.dwMajorVersion > 4 || (ver.dwMajorVersion == 4 && ver.dwMinorVersion >= 21) ) )
    {
        DEVMODE mode;
        mode.dmSize = sizeof(DEVMODE);
        mode.dmFields = DM_DISPLAYORIENTATION;
        ChangeDisplaySettingsEx(NULL, &mode, 0, CDS_TEST, NULL);
		switch ( _ttoi( (LPCTSTR)params.GetAt(0) ) )
		{
		case 90:
            mode.dmDisplayOrientation = DMDO_90;
			break;
		case 270:
            mode.dmDisplayOrientation = DMDO_270;
			break;
		case 180:
            mode.dmDisplayOrientation = DMDO_180;
			break;
        default:
            mode.dmDisplayOrientation = DMDO_0;
			break;
        }
        ChangeDisplaySettingsEx( NULL, &mode, NULL, 0, NULL );
    }

    return rc;
}
#endif

#ifndef DESKTOP
BOOL CMortStarterApp::SetVolume( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SetVolume'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    long vol = _wtol(params[0]);
    if ( vol >= 0 && vol <= 255 )
    {
        ULONG volume;
        BYTE  *volBytes = (BYTE*)&volume; // direct access to bytes
        volBytes[0] = 0;
        volBytes[1] = (BYTE)vol; // left volume
        volBytes[2] = 0;
        volBytes[3] = (BYTE)vol; // right volume

        waveOutSetVolume( 0, volume );
    }

    return rc;
}

BOOL CMortStarterApp::Vibrate( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Vibrate'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

#ifdef SMARTPHONE
	::Vibrate(0,NULL,TRUE,INFINITE);
#else
	::Vibrate();
#endif
    ::Sleep( _wtol(params[0]) );
	::VibrateStop();

    return TRUE;
}

#endif


BOOL CMortStarterApp::PlaySnd( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'PlaySound'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	PlaySound( params[0], NULL, SND_FILENAME );

	return TRUE;
}

#ifdef POCKETPC
BOOL CMortStarterApp::SetBacklight( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SetBacklight'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD bright   = _wtol(params[0]);
    DWORD acBright = _wtol(params[1]);

    HKEY hKey = NULL;
    DWORD dwDisposition = 0;
    if ( RegCreateKeyEx( HKEY_CURRENT_USER, _T("ControlPanel\\Backlight"),0, NULL,
                         0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition ) == ERROR_SUCCESS)
    {
        // Set backlight value for AC and Battery power
        if ( acBright >= 0 && acBright <= 100 )
            RegSetValueEx( hKey, _T("ACBrightNess"), 0, REG_DWORD, (PBYTE)&acBright, sizeof(REG_DWORD) );
        if ( bright >= 0 && bright <= 100 )
            RegSetValueEx( hKey, _T("BrightNess"),   0, REG_DWORD, (PBYTE)&bright,   sizeof(REG_DWORD) );

        // Signal display driver to update
        HANDLE hBackLightEvent = CreateEvent( NULL, FALSE, TRUE, TEXT("BackLightChangeEvent"));
        if (hBackLightEvent)
        {
            SetEvent(hBackLightEvent);
            CloseHandle(hBackLightEvent);
        }
    }

    return rc;
}
#endif

BOOL CMortStarterApp::Message( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Message'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    MessageBox( NULL, (LPCTSTR)params.GetAt(0)
              , params.GetSize() == 2 ? (LPCTSTR)params.GetAt(1) : L"MortScript"
              , MB_OK|MB_SETFOREGROUND|MB_SETFOREGROUND );

    return rc;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::Input( CString &param )
{
    CStringArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Input'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CDlgInput dlg;
    dlg.Numeric = _wtol(params[1]);
    dlg.m_Label = params[2];
    if ( params.GetSize() == 4 )
        dlg.Title = params[3];
    dlg.DoModal();

    params[0].MakeUpper();
    Variables.SetAt( params[0], dlg.m_Edit );

    return TRUE;
}
#endif

BOOL CMortStarterApp::CheckCondition( CString &param, BOOL &condRes )
{
    param.TrimLeft();
    param.TrimRight();

    int pos = param.FindOneOf(L" \t");
    if ( pos == -1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid condition" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }
    
    CString condType = param.Left(pos);
    condType.TrimLeft();
    
    BOOL not = FALSE;
    if ( condType.CompareNoCase( L"not" ) == 0 )
    {
        not = TRUE;
        int start = pos+1;
        while ( start < param.GetLength() && ( param[start] == ' ' || param[start] == '\t' ) ) start++;
        pos = start;
        while ( pos < param.GetLength() && ( param[pos] != ' ' && param[pos] != '\t' ) ) pos++;
        if ( pos == start || pos == param.GetLength() )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( NULL, L"Invalid condition" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            return FALSE;
        }
        
        condType = param.Mid( start, pos-start );
    }


    CString element = param.Mid(pos+1);
    element.TrimLeft(); element.TrimRight();
    //element.TrimLeft('\"'); element.TrimRight('\"');

    if ( condType.CompareNoCase( L"wndExists" ) == 0 )
        CheckWndExists( element, condRes );
    else if ( condType.CompareNoCase( L"wndActive" ) == 0 )
        CheckWndActive( element, condRes );
    else if ( condType.CompareNoCase( L"procExists" ) == 0 )
        CheckProcExists( element, condRes );
    else if (   condType.CompareNoCase( L"fileExists" ) == 0
        || condType.CompareNoCase( L"dirExists" ) == 0 )
        CheckFileExists( element, condType.CompareNoCase( L"dirExists" ) == 0, condRes );
    else if ( condType.CompareNoCase( L"question" ) == 0 )
        CheckQuestion( element, condRes );
#ifndef SMARTPHONE
    else if ( condType.CompareNoCase( L"screen" ) == 0 )
        CheckScreen( element, condRes );
#endif
    else if ( condType.CompareNoCase( L"regKeyExists" ) == 0 )
        CheckRegKeyExists( element, condRes );
    else if ( condType.CompareNoCase( L"regKeyEqualsString" ) == 0 )
        CheckRegKeyEqualsString( element, condRes );
    else if ( condType.CompareNoCase( L"regKeyEqualsDWord" ) == 0 )
        CheckRegKeyEqualsDWord( element, condRes );
#ifndef SMARTPHONE
    else if ( condType.CompareNoCase( L"equals" ) == 0 )
        CheckEquals( element, condRes );
    else if ( condType.CompareNoCase( L"expression" ) == 0 )
        CheckExpression( element, condRes );
    else if ( condType.GetAt(0) == '{' || condType.GetAt(0) == '(' )
    {
        if ( condType.GetLength() > 1 )
            element = condType.Mid(1) + element;
        element.TrimRight();
        if ( element.Right(1) == L")" || element.Right(1) == L"}" )
            element = element.Left( element.GetLength()-1 );
        CheckExpression( element, condRes );
    }
#endif
    else
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid condition" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( not ) condRes = !condRes;

    return TRUE;
}

BOOL CMortStarterApp::CheckWndExists( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If checkWndExists'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( FindWindow( params[0] ) != NULL )
        condRes = TRUE;

    return TRUE;
}

BOOL CMortStarterApp::CheckWndActive( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If checkWndActive'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND hwnd = ::GetForegroundWindow();
	TCHAR windowTitle[256];
	::GetWindowText( hwnd, windowTitle, 256 );
    if ( CString(windowTitle).Find( params[0] ) != -1 )
	{
		condRes = TRUE;
	}

    return TRUE;
}

BOOL CMortStarterApp::CheckProcExists( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If checkProcExists'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HANDLE         procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof( procEntry );

    if ( procSnap != NULL && Process32First( procSnap, &procEntry ) )
    {
        do
        {
            CString procName = procEntry.szExeFile;
            if ( procName.CompareNoCase( params[0] ) == 0 )
            {
                condRes = TRUE;
                break;
            }
			procEntry.dwSize = sizeof( procEntry );
        }
        while ( Process32Next( procSnap, &procEntry ) );
    }
    if ( procSnap != NULL )
#ifndef DESKTOP
      CloseToolhelp32Snapshot( procSnap );
#else
      CloseHandle( procSnap );
#endif

    return TRUE;
}

BOOL CMortStarterApp::CheckFileExists( CString &element, BOOL dir, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If checkFileExists'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params[0] );
    if ( attribs != -1 )
    {
        if ( (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0 )
        {
            if ( dir ) condRes = TRUE;
        }
        else
        {
            if ( !dir ) condRes = TRUE;
        }
    }

    return TRUE;
}

BOOL CMortStarterApp::CheckQuestion( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    Split( element, ',', params );
    if ( params.GetSize() == 2 )
    {
        if ( ::MessageBox( NULL, params[0], params[1], MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND ) == IDYES )
            condRes = TRUE;
    }
    if ( params.GetSize() == 1 )
    {
        if ( ::MessageBox( NULL, params[0], L"MortScript", MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND ) == IDYES )
            condRes = TRUE;
    }

    return TRUE;
}

BOOL CMortStarterApp::CheckScreen( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If checkScreen'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( params[0].CompareNoCase( L"landscape" ) )
        condRes = ( GetSystemMetrics( SM_CXSCREEN ) > GetSystemMetrics( SM_CYSCREEN ) );
    else if ( params[0].CompareNoCase( L"portrait" ) )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > GetSystemMetrics( SM_CXSCREEN ) );
    else if ( params[0].CompareNoCase( L"vga" ) )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > 320 && GetSystemMetrics( SM_CXSCREEN ) > 320 );
    else if ( params[0].CompareNoCase( L"qvga" ) )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) <= 320 && GetSystemMetrics( SM_CXSCREEN ) <= 320 );
    else
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid screen condition" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    return TRUE;
}

BOOL CMortStarterApp::CheckRegKeyExists( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    if ( Split( element, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If regKeyExists'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    HKEY key;
	if ( RegOpenKeyEx( root, params[1], 0, 0, &key ) == ERROR_SUCCESS )
	{
		DWORD type, length;
		TCHAR cont[MAX_PATH];
		length = MAX_PATH;
		int rc = RegQueryValueEx( key, params[2], NULL, &type, (BYTE*)cont, &length );
		if ( rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA )
		{
			condRes = TRUE;
		}
		RegCloseKey( key );
	}

    return condRes;
}

BOOL CMortStarterApp::CheckRegKeyEqualsString( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    if ( Split( element, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If regKeyEqualsString'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CString value = RegRead( root, params[1], params[2] );
    if ( value.Compare( params[3] ) == 0 )
    {
        condRes = TRUE;
    }

    return condRes;
}

BOOL CMortStarterApp::CheckRegKeyEqualsDWord( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    if ( Split( element, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If regKeyEqualsDWord'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"Invalid root entry" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }

    DWORD value = RegReadDW( root, params[1], params[2] );
    if ( value == (DWORD)_ttol( params[3] ) )
    {
        condRes = TRUE;
    }

    return condRes;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::CheckEquals( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CStringArray params;
    if ( Split( element, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'If equals'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( params[0].Compare( params[1] ) == 0 )
    {
        condRes = TRUE;
    }

    return condRes;
}

BOOL CMortStarterApp::CheckExpression( CString &element, BOOL &condRes )
{
    condRes = FALSE;

    CParser parser( element, &Variables );
    CString result = parser.EvaluateExpression();

    if ( parser.GetError() != 0 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, parser.GetErrorMessage() + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( _ttoi( result ) != 0 )
    {
        condRes = TRUE;
    }

    return condRes;
}
#endif

#ifndef SMARTPHONE
BOOL CMortStarterApp::Set( CString &param )
{
    BOOL rc = TRUE;

    int  pos = param.Find( ',' );

    if ( pos == -1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Set'" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CString varName = param.Left( pos );
    varName.TrimLeft();
    varName.TrimRight();

    if ( varName.GetLength() >= 3 && varName[0] == '%' && varName[varName.GetLength()-1] == '%' )
    {
        varName = varName.Mid( 1, varName.GetLength()-2 );
        varName.MakeUpper();
        CString value;
        if ( Variables.Lookup( varName, value ) )
        {
           varName = value;
        }
        else
        {
           varName.Empty();
        }
    }

    varName.MakeUpper();

    CString expression = param.Mid( pos+1 );
    CParser parser( expression, &Variables );
    CString result = parser.EvaluateExpression();

    if ( parser.GetError() != 0 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, parser.GetErrorMessage() + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    Variables.SetAt( varName, result );

    return rc;
}

BOOL CMortStarterApp::GetActiveWindow( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MakeUpper'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[0];
    var.MakeUpper();

	HWND hwnd = ::GetForegroundWindow();
	TCHAR windowTitle[256];
	::GetWindowText( hwnd, windowTitle, 256 );

	Variables.SetAt( var, windowTitle );

    return TRUE;
}

BOOL CMortStarterApp::GetSystemPath( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetSystemPath'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString path;
    LPTSTR  pathBuffer = path.GetBuffer( MAX_PATH );
    if ( params[0].CompareNoCase( L"ProgramsMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PROGRAMS, FALSE );
    }
    else if ( params[0].CompareNoCase( L"StartMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_STARTMENU, FALSE );
    }
    else if ( params[0].CompareNoCase( L"Startup" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_STARTUP, FALSE );
    }
    else if ( params[0].CompareNoCase( L"Documents" ) == 0 )
    {
#ifndef DESKTOP
        // Works only in WM2003 and later
        OSVERSIONINFO ver;
        GetVersionEx( &ver );
        if ( ( ver.dwMajorVersion > 4 || (ver.dwMajorVersion == 4 && ver.dwMinorVersion >= 20) ) )
        {
            SHGetDocumentsFolder( L"\\", pathBuffer );
        }
        else
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"GetSystemPath: Document path requires WM2003" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
        }
#else
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PERSONAL, FALSE );
#endif
    }
    else if ( params[0].CompareNoCase( L"ProgramFiles" ) == 0 )
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
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"GetSystemPath: Program files path requires WM2003" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
        }
#else
        if ( SHGetSpecialFolderPath( NULL, pathBuffer, 0x0026, FALSE ) == FALSE )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"GetSystemPath: Retrieving program files path not supported by your system" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
        }
#endif
    }
#ifndef SMARTPHONE
    else if ( params[0].CompareNoCase( L"Script" ) == 0 || params[0].CompareNoCase( L"ScriptPath" ) == 0)
    {
        int len = ScriptFile.ReverseFind('\\');
        wcsncpy( pathBuffer, (LPCTSTR)ScriptFile, len );
        pathBuffer[len] = '\0';
    }
    else if ( params[0].CompareNoCase( L"ScriptName" ) == 0 )
    {
        int len = ScriptFile.ReverseFind('\\');
        int dot = ScriptFile.ReverseFind('.');
        wcsncpy( pathBuffer, (LPCTSTR)ScriptFile+len+1, dot-len-1 );
        pathBuffer[dot-len-1] = '\0';
    }
    else if ( params[0].CompareNoCase( L"ScriptExt" ) == 0 )
    {
        int dot = ScriptFile.ReverseFind('.');
        wcscpy( pathBuffer, (LPCTSTR)ScriptFile.Mid(dot) );
    }
    else if ( params[0].CompareNoCase( L"ScriptExe" ) == 0 )
    {
        int len = wcsrchr( m_pszHelpFilePath, '\\' ) - m_pszHelpFilePath;
        wcsncpy( pathBuffer, m_pszHelpFilePath, len );
        pathBuffer[len] = '\0';
    }
#endif
    else
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( NULL, L"GetSystemPath: Invalid path type" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    path.ReleaseBuffer();

    CString varName = params[1];
    varName.MakeUpper();
    Variables.SetAt( varName, path );

    return TRUE;
}

BOOL CMortStarterApp::GetFilePath( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetFileExt'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

	CString path;
	int pos = params[0].ReverseFind('\\');
	if ( pos == -1 )
		path.Empty();
	else
		path = params[0].Left( pos );

    CString varName = params[1];
    varName.MakeUpper();
	Variables.SetAt( varName, path );

	return TRUE;
}

BOOL CMortStarterApp::GetFileBase( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetFileExt'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

	CString file;
	int pos1 = params[0].ReverseFind('\\');
	int pos2 = params[0].ReverseFind('.');
	if ( pos2 > pos1 )
		file = params[0].Mid( pos1+1, pos2-pos1-1 );
	else
		file = params[0].Mid( pos1+1 );

    CString varName = params[1];
    varName.MakeUpper();
	Variables.SetAt( varName, file );

	return TRUE;
}

BOOL CMortStarterApp::GetFileExt( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 2 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetFileExt'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

	CString ext;
	int pos = params[0].ReverseFind('.');
	if ( pos == -1 || pos < params[0].ReverseFind('\\') )
		ext.Empty();
	else
		ext = params[0].Mid( pos );

    CString varName = params[1];
    varName.MakeUpper();
	Variables.SetAt( varName, ext );

	return TRUE;
}

BOOL CMortStarterApp::Explode( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) < 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Split'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( params[1].GetLength() != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Split delimiter must be one character" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CStringArray elements;
    CIniFile::Split( params[0], params[1].GetAt(0), elements, _ttol( params[2] ) );
    
    for ( int i=0; i+3<params.GetSize(); i++ )
    {
        CString var = params[i+3];
        var.MakeUpper();
        if ( i < elements.GetSize() )
        {
            Variables.SetAt( var, elements[i] );
        }
        else
        {
            Variables.SetAt( var, L"" );
        }
    }

    return TRUE;
}

BOOL CMortStarterApp::GetPart( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 5 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetPart'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( params[1].GetLength() != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Split delimiter must be one character" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CStringArray elements;
    CIniFile::Split( params[0], params[1].GetAt(0), elements, _ttol( params[2] ) );
    
    CString var = params[4];
    var.MakeUpper();
    if ( _ttol( params[3] ) > 0 && _ttol( params[3] ) < elements.GetSize() )
    {
        Variables.SetAt( var, elements.GetAt( _ttol( params[3] )-1 ) );
    }
    else
    {
        Variables.SetAt( var, L"" );
    }

    return TRUE;
}

BOOL CMortStarterApp::SubStr( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 4 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'SubStr'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[3];
    var.MakeUpper();
	if ( _ttol( params[2] ) > 0 )
	{
		Variables.SetAt( var, params[0].Mid( _ttol( params[1]-1 ), _ttol( params[2] ) ) );
    }
    else
    {
        Variables.SetAt( var, params[0].Mid( _ttol( params[1]-1 ) ) );
    }

    return TRUE;
}

BOOL CMortStarterApp::Find( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Find'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( params[1].GetLength() < 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"String to find must be at least one character" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[2];
    var.MakeUpper();

	CString pos;
	pos.Format( L"%d", params[0].Find( params[1] )+1 );

	Variables.SetAt( var, pos );

    return TRUE;
}

BOOL CMortStarterApp::ReverseFind( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'ReverseFind'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( params[1].GetLength() != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"String to find must be one character" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[2];
    var.MakeUpper();

	CString pos;
	pos.Format( L"%d", params[0].ReverseFind( params[1].GetAt(0) )+1 );

	Variables.SetAt( var, pos );

    return TRUE;
}

BOOL CMortStarterApp::MakeUpper( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MakeUpper'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[0];
    var.MakeUpper();

	CString val;
	if ( ! Variables.Lookup( var, val ) )
		val.Empty();
	val.MakeUpper();

	Variables.SetAt( var, val );

    return TRUE;
}

BOOL CMortStarterApp::MakeLower( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'MakeUpper'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[0];
    var.MakeUpper();

	CString val;
	if ( ! Variables.Lookup( var, val ) )
		val.Empty();
	val.MakeLower();

	Variables.SetAt( var, val );

    return TRUE;
}

BOOL CMortStarterApp::GetMortScriptType( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'GetMortScriptType'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString var = params[0];
    var.MakeUpper();

#ifdef POCKETPC
	Variables.SetAt( var, L"PPC" );
#endif
#ifdef SMARTPHONE
	Variables.SetAt( var, L"SP" );
#endif
#ifdef DESKTOP
	Variables.SetAt( var, L"PC" );
#endif

    return TRUE;
}

#endif

void CMortStarterApp::PushCondStack( short type, short state )
{
    CondStack.Add( MAKELONG( state, type ) );
    CondState = state;
}

void CMortStarterApp::PopCondStack( short &type, short &state, BOOL remove )
{
    if ( CondStack.GetSize() == 0 )
    {
        type  = 0;
        state = 0;
    }
    else
    {
        int   idx   = CondStack.GetUpperBound();
        DWORD entry = CondStack.GetAt( idx );
        type  = HIWORD( entry );
        state = LOWORD( entry );
        if ( remove )
        {
            CondStack.RemoveAt( idx );
            if ( state != 2 )
            {
                CondState = 1;
            }
        }
    }
}

BOOL CMortStarterApp::If( CString &param, long pos )
{
    BOOL condRes;

    if ( CondState != 1 )
        condRes = 2;
    else
    {
        if ( CheckCondition( param, condRes ) == FALSE )
            return FALSE;
    }

    PushCondStack( CT_IF, condRes );

    return TRUE;
}

BOOL CMortStarterApp::Else()
{
    short type, state;
    PopCondStack( type, state );

    if ( type != CT_IF )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Else without If" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    // Additional level in FALSE block
    if ( state != 2 )
    {
        state = !state;
    }
    PushCondStack( CT_IF, state );

    return TRUE;
}

BOOL CMortStarterApp::EndIf()
{
    short type, state;
    PopCondStack( type, state );

    if ( type != CT_IF )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"EndIf without If" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    return TRUE;
}

BOOL CMortStarterApp::While( CString &param, long pos )
{
    BOOL condRes;

    if ( CondState != 1 )
        condRes = 2;
    else
    {
        if ( CheckCondition( param, condRes ) == FALSE )
            return FALSE;
    }

    WhilePos.Add( pos );

    PushCondStack( CT_WHILE, condRes );

    return TRUE;
}

BOOL CMortStarterApp::EndWhile( CString &param, long &pos )
{
    BOOL rc = TRUE;
    short type, state;
    PopCondStack( type, state );

    if ( type != CT_WHILE )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"'EndWhile' without 'While'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    int  stackPos = WhilePos.GetUpperBound();
    int  whilePos = WhilePos.GetAt(stackPos);
    WhilePos.RemoveAt(stackPos);

    if ( state == TRUE )
    {
        pos = whilePos;
    }

    return rc;
}

#ifndef SMARTPHONE
BOOL CMortStarterApp::ForEach( CString &param, long pos )
{
    BOOL condRes = TRUE;

    if ( CondState != 1 )
        condRes = 2;
    else
    {
        CString paramLc = param;
        paramLc.MakeLower();
        int inPos = paramLc.Find( L" in " );

        if ( inPos == -1 )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"ForEach without \"in\"" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            return FALSE;
        }

        CString variable = param.Left( inPos );
        variable.TrimLeft(); variable.TrimRight(); variable.MakeUpper();
        if ( variable.IsEmpty() )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"ForEach: Missing variable name(s)" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            return FALSE;
        }
        CStringArray variables;
        int varCount = Split( variable, ',', variables );

        CString tmptype = paramLc.Mid( inPos+4 );
        int lSpaces = tmptype.GetLength();
        tmptype.TrimLeft();
        lSpaces -= tmptype.GetLength();
        int typePos = tmptype.FindOneOf( L" \t" );
        if ( typePos == -1 )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( NULL, L"ForEach: invalid loop type" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            return FALSE;
        }

        CString stuff = param.Mid(inPos+typePos+lSpaces+4);
        stuff.TrimLeft(); stuff.TrimRight();
        CString type = tmptype.Left(typePos);
        int varCnt = 1;
        if ( type == L"values" )
        {
            if ( stuff.IsEmpty() )
            {
                MessageBox( NULL, L"ForEach: no value list found" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            if ( varCount != 1 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with values" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray *values = new CStringArray;
            int count = Split( stuff, ',', *values );
            if ( count > 0 )
            {
                Variables.SetAt( variables[0], values->GetAt(0) );
                values->RemoveAt( 0 );
                ForEachData.Add( (void*)values );
            }
            else
            {
                condRes = FALSE;
                ForEachData.Add( values );
            }
            ForEachVariables.Add( variable );
            ForEachType.Add( 1 );
        }
        else if ( type == L"split" )
        {
            if ( varCount != 1 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with split" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray params;
            if ( Split( stuff, ',', params ) != 3 )
            {
                MessageBox( NULL, L"Invalid parameter count for ForEach with split" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            if ( params[1].GetLength() != 1 )
            {
                if ( ErrorLevel >= ERROR_SYNTAX )
                    MessageBox( NULL, L"Split delimiter must be one character" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
                return FALSE;
            }

            condRes = FALSE;
            CStringArray *values = new CStringArray;
            CIniFile::Split( params[0], params[1].GetAt(0), *values, _ttol( params[2] ) );

            if ( values->GetSize() > 0 )
            {
                Variables.SetAt( variables[0], values->GetAt(0) );
                values->RemoveAt(0);
                condRes = TRUE;
            }
            else
            {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 2 );
        }
        else if ( type == L"inisections" )
        {
            if ( varCount != 1 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with iniSections" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray params;
            if ( Split( stuff, ',', params ) != 1 )
            {
                MessageBox( NULL, L"Invalid parameter count for ForEach with iniSections" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }

            CIniFile iniFile;
            CString data;
            condRes = FALSE;
            CStringArray *values = new CStringArray;
            if ( iniFile.Read( params[0] ) )
            {
                POSITION pos = iniFile.Sections.GetStartPosition();
                CString  section;
                void    *dummy;
                for ( int i=0; pos != NULL; i++ )
                {
                    iniFile.Sections.GetNextAssoc( pos, section, dummy );
                    if ( i == 0 )
                    {
                        Variables.SetAt( variables[0], section );
                        condRes = TRUE;
                    }
                    else
                    {
                        values->Add( section );
                    }
                }
            }
            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 3 );
        }
        else if ( type == L"inikeys" )
        {
            if ( varCount != 2 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with iniKeys" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray params;
            if ( Split( stuff, ',', params ) != 2 )
            {
                MessageBox( NULL, L"Invalid parameter count for ForEach with iniKeys" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }

            CIniFile iniFile;
            CString data;
            condRes = FALSE;
            CStringArray *values = new CStringArray;
            if ( iniFile.Read( params[0] ) )
            {
                CMapStringToString *elements;
                params[1].MakeLower();
                if ( iniFile.Sections.Lookup( params[1], (void*&)elements ) )
                {
                    POSITION pos = elements->GetStartPosition();
                    CString  key,value;
                    for ( int i=0; pos != NULL; i++ )
                    {
                        elements->GetNextAssoc( pos, key, value );
                        if ( i == 0 )
                        {
                            Variables.SetAt( variables[0], key );
                            Variables.SetAt( variables[1], value );
                            condRes = TRUE;
                        }
                        else
                        {
                            values->Add( key + L"=" + value );
                        }
                    }
                }
            }
            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 4 );
        }
        else if ( type == L"files" || type == L"directories" )
        {
            if ( varCount != 1 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with "+type + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray params;
            if ( Split( stuff, ',', params ) != 1 )
            {
                MessageBox( NULL, L"Invalid parameter count for ForEach with "+type + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }

            UINT fileType;
            if ( type == L"files" )
                fileType = 0;
            else
                fileType = FILE_ATTRIBUTE_DIRECTORY;

            condRes = FALSE;
            CStringArray *values = new CStringArray;

            CString path = params[0].Left(params[0].ReverseFind('\\')+1);
        	WIN32_FIND_DATA findFileData;
            HANDLE ffh = FindFirstFile( params[0], &findFileData );
	        if ( ffh != INVALID_HANDLE_VALUE )
	        {
                do
                {
                    CString file = path+findFileData.cFileName;
                    if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == fileType )
                    {
                        if ( condRes == FALSE ) // First entry
                        {
                            Variables.SetAt( variables[0], file );
                            condRes = TRUE;
                        }
                        else
                        {
                            values->Add( file );
                        }
                    }
                }
		        while ( FindNextFile( ffh, &findFileData ) == TRUE );

		        FindClose( ffh );
	        }
            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 5 );
        }
        else if ( type == L"charsof" )
        {
            if ( varCount != 1 )
            {
                MessageBox( NULL, L"Invalid variables count for ForEach with charsOf" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                return FALSE;
            }
            CStringArray params;
            if ( Split( stuff, ',', params ) != 1 )
            {
                MessageBox( NULL, L"Invalid parameter count for ForEach with charsOf" + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }

            condRes = FALSE;
            CStringArray *values = new CStringArray;

            TCHAR value[2];
            value[1] = '\0';
            for ( int i=0; i < params[0].GetLength(); i++ )
            {
                value[0] = params[0].GetAt(i);
                values->Add( value );
            }

            if ( values->GetSize() > 0 )
            {
                Variables.SetAt( variables[0], values->GetAt(0) );
                values->RemoveAt(0);
                condRes = TRUE;
            }
            else
            {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 6 );
        }
        else
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
            {
                CString msg;
                msg.Format( L"ForEach: invalid loop type" );
                MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
            return FALSE;
        }
    }

    PushCondStack( CT_FOREACH, condRes );
    ForEachPos.Add( pos );

    return TRUE;
}

BOOL CMortStarterApp::EndForEach( CString &param, long &pos )
{
    BOOL rc = TRUE;
    short type, cond;
    PopCondStack( type, cond, FALSE );
    if ( type == -1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"'EndForEach' without 'ForEach'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    int stackPos = ForEachPos.GetUpperBound();

    if ( cond != 2 )
    {
        CStringArray variables;
        Split( ForEachVariables.GetAt(stackPos), ',', variables );

        switch( ForEachType.GetAt(stackPos) )
        {
        // Data in CStringArray
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
            {
                CStringArray *data = (CStringArray*)ForEachData.GetAt(stackPos);
                if ( data == NULL || data->GetSize() == 0 )
                {
                    if ( data != NULL ) { delete data; data = NULL; }
                    cond = FALSE;
                }
                else
                {
                    if ( cond == 1 )
                    {
                        Variables.SetAt( variables[0], data->GetAt(0) );
                        data->RemoveAt(0);
                    }
                }
            }
            break;

        // INI values
        case 4:
            {
                CStringArray *data = (CStringArray*)ForEachData.GetAt(stackPos);
                if ( data == NULL || data->GetSize() == 0 )
                {
                    if ( data != NULL ) { delete data; data = NULL; }
                    cond = FALSE;
                }
                else
                {
                    if ( cond == 1 )
                    {
                        CString entry = data->GetAt(0);
                        int pos = entry.Find( '=' );
                        CString key   = entry.Left( pos );
                        CString value = entry.Mid( pos+1 );
                        Variables.SetAt( variables[0], key );
                        Variables.SetAt( variables[1], value );
                        data->RemoveAt(0);
                    }
                }
            }
            break;
        }
    }

    // No more continues...
    if ( cond == 0 )
    {
        ForEachData.RemoveAt(stackPos);
        ForEachVariables.RemoveAt(stackPos);
        ForEachType.RemoveAt(stackPos);
    }

    // Back to beginning...
    if ( cond == 1 )
    {
        pos = ForEachPos.GetAt(stackPos);
        if ( cond != 2 )
            CondState = TRUE;
    }
    else
    {
        // Remove position and state (also done for cond==2)
        ForEachPos.RemoveAt(stackPos);
        PopCondStack( type, cond );
    }

    return rc;
}
#endif

BOOL CMortStarterApp::Repeat( CString &param, long pos )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Repeat'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    RepeatCount.Add( _ttoi( params[0] ) );
    RepeatPos.Add( pos );

    return TRUE;
}

BOOL CMortStarterApp::EndRepeat( CString &param, long &pos )
{
    BOOL rc = TRUE;
    int stackPos = RepeatPos.GetUpperBound();
    if ( stackPos == -1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"'EndRepeat' without 'Repeat'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    RepeatCount[stackPos]--;
    if ( RepeatCount[stackPos] <= 0 )
    {
        RepeatCount.RemoveAt(stackPos);
        RepeatPos.RemoveAt(stackPos);
    }
    else
    {
        pos = RepeatPos.GetAt(stackPos);
    }

    return rc;
}


BOOL CMortStarterApp::Switch( CString &param, long pos )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Switch'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( CondState != 1 )
        PushCondStack( CT_SWITCH, 2 );
    else
    {
        ChoiceStack.Add( _wtol(params[0]) );
        PushCondStack( CT_SWITCH, 0 );
    }

    return TRUE;
}


#ifndef SMARTPHONE
BOOL CMortStarterApp::Choice( CString &param, long pos )
{
    CStringArray params;
    if ( Split( param, ',', params ) <= 3 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Choice'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( CondState != 1 )
        PushCondStack( CT_CHOICE, 2 );
    else
    {
        CDlgChoice choiceDlg;
        choiceDlg.m_Title = params[0];
        choiceDlg.m_Info  = params[1];
        for ( int i=2; i<params.GetSize(); i++ )
        {
            choiceDlg.m_Strings.Add( params[i] );
        }
        choiceDlg.DoModal();

        ChoiceStack.Add( choiceDlg.m_Selected );
        PushCondStack( CT_CHOICE, 0 );
    }

    return TRUE;
}

BOOL CMortStarterApp::ChoiceDefault( CString &param, long pos )
{
    CStringArray params;
    if ( Split( param, ',', params ) <= 5 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'ChoiceDefault'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    if ( CondState != 1 )
        PushCondStack( CT_CHOICE, 2 );
    else
    {
        CDlgChoice choiceDlg;
        choiceDlg.m_Title   = params[0];
        choiceDlg.m_Info    = params[1];
        choiceDlg.m_Default = _wtol(params[2])-1;
        choiceDlg.m_Timeout = _wtol(params[3]);
        for ( int i=4; i<params.GetSize(); i++ )
        {
            choiceDlg.m_Strings.Add( params[i] );
        }
        choiceDlg.DoModal();

        ChoiceStack.Add( choiceDlg.m_Selected );
        PushCondStack( CT_CHOICE, 0 );
    }

    return TRUE;
}
#endif

BOOL CMortStarterApp::Case( CString &param )
{
    short type, cond;
    PopCondStack( type, cond, FALSE );
    if ( type != CT_SWITCH && type != CT_CHOICE )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Case without Choice/Switch" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }
    if ( cond != 2 )
    {
        CStringArray params;
        if ( Split( param, ',', params ) < 1 )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( NULL, L"Invalid parameter count for 'Case'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
            return FALSE;
        }

        PopCondStack( type, cond );

        cond = FALSE;
        int value = ChoiceStack.GetAt( ChoiceStack.GetUpperBound() );
        for ( int i=0; i<params.GetSize(); i++ )
        {
            if ( _wtol( params[i] ) == value )
            {
                cond = TRUE;
                //CondState = TRUE;
            }
        }

        PushCondStack( type, cond );
    }

    return TRUE;
}


BOOL CMortStarterApp::EndChoice()
{
    short type, cond;
    PopCondStack( type, cond );
    if ( type != CT_SWITCH && type != CT_CHOICE )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"EndChoice/EndSwitch without Choice/Switch" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }
    if ( cond != 2 )
    {
        if ( ChoiceStack.GetSize() == 0 )
        {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( NULL, L"EndChoice without Choice" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
            return FALSE;
        }
        ChoiceStack.RemoveAt( ChoiceStack.GetUpperBound() );
    }

    return TRUE;
}


#ifndef SMARTPHONE
BOOL CMortStarterApp::CallScript( CString &param )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'CallScript'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

	CMortStarterApp subApp;
	subApp.RunFile( params[0] );

    return TRUE;
}


BOOL CMortStarterApp::Call( CString &param, long curr, long &pos )
{
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'Call'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    CString sub = params[0];
    sub.MakeUpper();
    void *newPos;
    if ( Subs.Lookup( sub, newPos ) == FALSE )
    {
        CString msg;
        msg.Format( L"Sub '%s' not found", (LPCTSTR)params[0] );
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    SubStack.Add( curr );
    pos = (long)newPos;

    return TRUE;
}


BOOL CMortStarterApp::EndSub( long &pos )
{
    int stackPos = SubStack.GetUpperBound();
    if ( stackPos > -1 )
    {
        pos = SubStack.GetAt( stackPos );
        SubStack.RemoveAt( stackPos );
        return TRUE;
    }
    else
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"EndSub without Sub" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }
}
#endif

BOOL CMortStarterApp::SetErrorLevel( CString &param )
{
    BOOL rc = TRUE;
    CStringArray params;
    if ( Split( param, ',', params ) != 1 )
    {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( NULL, L"Invalid parameter count for 'ErrorLevel'" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

	if ( params.GetAt(0).CompareNoCase( L"off" ) == 0 )
    {
        ErrorLevel = ERROR_OFF;
    }
	else if ( params.GetAt(0).CompareNoCase( L"critical" ) == 0 )
    {
        ErrorLevel = ERROR_CRITICAL;
    }
	else if ( params.GetAt(0).CompareNoCase( L"syntax" ) == 0 )
    {
        ErrorLevel = ERROR_SYNTAX;
    }
	else if ( params.GetAt(0).CompareNoCase( L"error" ) == 0 )
    {
        ErrorLevel = ERROR_ERROR;
    }
	else if ( params.GetAt(0).CompareNoCase( L"warn" ) == 0 )
    {
        ErrorLevel = ERROR_WARN;
    }
	else if ( params.GetAt(0).CompareNoCase( L"debug" ) == 0 )
    {
        ErrorLevel = ERROR_DEBUG;
    }
    else
    {
        MessageBox( NULL, L"Invalid ErrorLevel" + ErrLine, L"Error", MB_OK|MB_ICONERROR );
        return FALSE;
    }

    return rc;
}

void CMortStarterApp::RunFile( LPCTSTR filename )
{
	char   *buffer = NULL;

	CString cmd;
	CString param;

    long startPos = 0, pos = 0;
	long eqPos;
    long spacePos;
    long tabPos;
    long sepPos;
	CString line;

	CString content;
    if ( CIniFile::ReadFile( filename, content ) == 0 )
	{
		content += L"\n";
        BYTE cont = TRUE;
        CondState = TRUE;

        CString searchContent = content;
        searchContent.MakeUpper();

#ifndef SMARTPHONE
		while ( pos != -1 && pos < content.GetLength() && cont == TRUE )
		{
            pos = searchContent.Find( L"SUB", pos );
            if ( pos == -1 ) break;

            startPos = pos;

            int charPos;
            for ( charPos = pos-1
                ;    charPos > -1 
                  && (  searchContent[charPos] == ' '
                     || searchContent[charPos] == '\t'
                     )
                ; charPos--
                );

            if ( charPos == -1 || searchContent[charPos] == '\n' )
            {
    			pos = searchContent.Find( _T("\n"), startPos );
				if ( pos != -1 )
				{
					line = searchContent.Mid( startPos, pos-startPos );
                    pos++;
				}
				else
				{
					line = searchContent.Mid( startPos );
				}

                line.TrimLeft();
				line.TrimRight();
                /*
                while ( line.Right(1) == L"\\" )
                {
                    int oldPos = pos;
        			pos = searchContent.Find( _T("\n"), oldPos );
                    CString addLine = searchContent.Mid( oldPos, pos-oldPos );
                    addLine.TrimLeft(); addLine.TrimRight();
                    line += L" " + addLine;
                }
                */

				eqPos    = line.Find( '=' );
                spacePos = line.Find( ' ' );
                tabPos   = line.Find( '\t' );
            
                sepPos   = line.GetLength();
                if ( eqPos != -1    && eqPos    < sepPos ) sepPos = eqPos;
                if ( spacePos != -1 && spacePos < sepPos ) sepPos = spacePos;
                if ( tabPos != -1   && tabPos   < sepPos ) sepPos = tabPos;
                if ( sepPos != line.GetLength() )
                {
    				cmd   = line.Left(sepPos);
	    			param = line.Mid(sepPos+1);
                    if ( cmd == L"SUB" )
                    {
                        Subs.SetAt( param, (void*)pos );
                    }
                }
            }
            else
            {
                pos++;
            }
        }
#endif

        pos = 0; startPos = 0;
		while ( pos != -1 && pos < content.GetLength() && cont == TRUE )
		{
            cont = 2;

            UINT currStart = startPos;
			pos = content.Find( _T("\n"), startPos );
			if ( pos != -1 )
			{
				line    = content.Mid( startPos, pos-startPos );
			}
			else
			{
				line = content.Mid( startPos );
			}
            line.TrimLeft();
			line.TrimRight();
            while ( line.Right(1) == L"\\" && pos != -1 )
            {
                line = line.Left( line.GetLength() - 1 );
                line.TrimRight();
                int oldPos = pos+1;
        		pos = content.Find( _T("\n"), oldPos );
                if ( pos != -1 )
                {
                    CString addLine = content.Mid( oldPos, pos-oldPos );
                    addLine.TrimLeft(); addLine.TrimRight();
                    line += L" " + addLine;
                }
            }
            startPos = pos+1;

			if ( !line.IsEmpty() && line.GetAt(0) != '#' )
			{
                ErrLine = L"\n\nLine:\n" + line;

                if ( line.GetAt(0) == '%' )
                {
                    int endVar = line.Find('%',1);
                    if ( endVar != -1 )
                    {
                        int eq = line.Find('=',endVar+1);
                        if ( eq != -1 )
                        {
                            cmd   = L"Set";
                            param = line.Mid(1,endVar-1) + L"," + line.Mid( eq+1 );
                        }
                    }
                }
                else
                {
				    eqPos = line.Find( '=' );
                    spacePos = line.Find( ' ' );
                    tabPos = line.Find( '\t' );
                
                    sepPos = line.GetLength();
                    if ( eqPos != -1    && eqPos    < sepPos ) sepPos = eqPos;
                    if ( spacePos != -1 && spacePos < sepPos ) sepPos = spacePos;
                    if ( tabPos != -1   && tabPos   < sepPos ) sepPos = tabPos;
                    if ( sepPos != line.GetLength() )
                    {
    					cmd   = line.Left(sepPos);
	    				param = line.Mid(sepPos+1);
                    }
                    else
                    {
                        cmd   = line;
                        param.Empty();
                    }
                }

                cmd.TrimLeft();
                cmd.TrimRight();
                cmd.MakeLower();

                if ( CondState == TRUE )
                {
					if ( cmd == L"run" )	    	cont = RunApp( param );
                    if ( cmd == L"runwait" )		cont = RunApp( param, TRUE );
#ifdef POCKETPC
#ifndef PNA
					if ( cmd == L"new" )		    cont = New( param );
#endif
#endif
					if ( cmd == L"show" )   		cont = Show( param );
					if ( cmd == L"minimize" )		cont = Minimize( param );
					if ( cmd == L"sleep" )		    cont = Sleep( param );
#ifndef SMARTPHONE
					if ( cmd == L"sleepmessage" )   cont = SleepMessage( param );
                    if ( cmd == L"gettime" ) 		cont = GetTime( param );
#endif

					if (   cmd.Left(4) == L"send"
                        || cmd == L"snapshot"
                       )
					{
                        if ( cmd == L"sendkeys" )
                        {
                            cont = SendKeys( param );
                        }
                        else if ( cmd == L"sendctrlkey" )
                        {
                            cont = SendCtrlKey( param );
                        }
                        else
                        {
                            cont = SendSpecialKey( cmd, param );
                        }
                    }

                    if ( cmd.CompareNoCase( L"WaitFor" )  == 0 )		cont = WaitFor( param );
					if ( cmd.CompareNoCase( L"WaitForActive" ) == 0 )	cont = WaitForActive( param );
					if ( cmd.CompareNoCase( L"Close" ) == 0 )			cont = Close( param );
					if ( cmd.CompareNoCase( L"Kill" ) == 0 )			cont = Kill( param );
                    
                    if ( cmd.CompareNoCase( L"MkDir" ) == 0 )		    cont = MkDir( param );
                    if ( cmd.CompareNoCase( L"RmDir" ) == 0 )           cont = RmDir( param );
                    
#ifndef SMARTPHONE
                    if ( cmd == L"getcolorat" )     cont = GetColorAt( param );
                    if ( cmd == L"getrgb" )         cont = GetRGB( param );
                    if ( cmd == L"getwindowtext" )  cont = GetWindowText( param );
#endif

                    if ( cmd.CompareNoCase( L"Delete" )  == 0 )		    cont = Delete( param, FALSE );
                    if ( cmd.CompareNoCase( L"DelTree" ) == 0 )		    cont = Delete( param, TRUE );
#ifndef SMARTPHONE
                    if ( cmd == L"setproxy" )                           cont = SetProxy( param );
                    if ( cmd == L"download" )                           cont = Download( param );
#endif
                    if ( cmd.CompareNoCase( L"Copy" )  == 0 )           cont = Copy( param );
                    if ( cmd.CompareNoCase( L"XCopy" )  == 0 )          cont = XCopy( param );
                    if ( cmd.CompareNoCase( L"Rename" )  == 0 )         cont = Rename( param );
                    if ( cmd.CompareNoCase( L"Move" )  == 0 )           cont = Move( param );
                    if ( cmd.CompareNoCase( L"CreateShortcut" )  == 0 ) cont = CreateShortcut( param );
#ifndef SMARTPHONE
                    if ( cmd.CompareNoCase( L"GetVersion" )  == 0 )     cont = GetVersion( param );
#endif

                    
                    if ( cmd.CompareNoCase( L"MouseClick" )  == 0 )     cont = MouseClick( param );
                    if ( cmd.CompareNoCase( L"MouseDblClick" )  == 0 )  cont = MouseClick( param, TRUE );
                    if ( cmd.CompareNoCase( L"MouseDown" )  == 0 )      cont = MouseDown( param );
                    if ( cmd.CompareNoCase( L"MouseUp" )  == 0 )        cont = MouseUp( param );
#ifdef DESKTOP
                    if ( cmd.CompareNoCase( L"RightMouseClick" )  == 0 )     cont = MouseClick( param, FALSE, 1 );
                    if ( cmd.CompareNoCase( L"RightMouseDblClick" )  == 0 )  cont = MouseClick( param, TRUE, 1 );
                    if ( cmd.CompareNoCase( L"RightMouseDown" )  == 0 )      cont = MouseDown( param, 1 );
                    if ( cmd.CompareNoCase( L"RightMouseUp" )  == 0 )        cont = MouseUp( param, 1 );
                    if ( cmd.CompareNoCase( L"MiddleMouseClick" )  == 0 )     cont = MouseClick( param, FALSE, 2 );
                    if ( cmd.CompareNoCase( L"MiddleMouseDblClick" )  == 0 )  cont = MouseClick( param, TRUE, 2 );
                    if ( cmd.CompareNoCase( L"MiddleMouseDown" )  == 0 )      cont = MouseDown( param, 2 );
                    if ( cmd.CompareNoCase( L"MiddleMouseUp" )  == 0 )        cont = MouseUp( param, 2 );
#endif
                    
                    if ( cmd.CompareNoCase( L"RegWriteString" )  == 0 ) cont = RegWriteString( param );
                    if ( cmd.CompareNoCase( L"RegWriteDWord" )  == 0 )  cont = RegWriteDWord( param );
#ifndef SMARTPHONE
                    if ( cmd.CompareNoCase( L"RegReadString" )  == 0 )  cont = RegReadString( param );
                    if ( cmd.CompareNoCase( L"RegReadDWord" )  == 0 )   cont = RegReadDWord( param );
#endif
                    if ( cmd.CompareNoCase( L"RegDelete" )  == 0 )      cont = RegDelete( param );
                    if ( cmd.CompareNoCase( L"RegDeleteKey" )  == 0 )   cont = RegDeleteKey( param );
                    
#ifndef SMARTPHONE
                    if ( cmd.CompareNoCase( L"IniRead" )  == 0 )        cont = IniRead( param );
                    if ( cmd.CompareNoCase( L"ReadFile" )  == 0 )       cont = ReadFile( param );
#endif
                    if ( cmd.CompareNoCase( L"WriteFile" ) == 0 )       cont = WriteFile( param );

#ifndef DESKTOP
                    if ( cmd == L"rotate" )		        cont = Rotate( param );
                    if ( cmd == L"setvolume" )			cont = SetVolume( param );
					if ( cmd == L"vibrate" )			cont = Vibrate( param );
#endif
					if ( cmd == L"playsound" )			cont = PlaySnd( param );
#ifdef POCKETPC
                    if ( cmd.CompareNoCase( L"SetBacklight" )  == 0 )   cont = SetBacklight( param );
#endif
#ifndef SMARTPHONE
                    if ( cmd.CompareNoCase( L"Input" )  == 0 )          cont = Input( param );
#endif
                    if ( cmd.CompareNoCase( L"Message" )  == 0 )        cont = Message( param );

#ifndef SMARTPHONE
                    if ( cmd == L"set" )				cont = Set( param );
					if ( cmd == L"getactivewindow" )	cont = GetActiveWindow( param );
                    if ( cmd == L"getsystempath" )		cont = GetSystemPath( param );
                    if ( cmd == L"getfilepath" )		cont = GetFilePath( param );
                    if ( cmd == L"getfilebase" )		cont = GetFileBase( param );
                    if ( cmd == L"getfileext" )			cont = GetFileExt( param );

					if ( cmd == L"split" )				cont = Explode( param );
					if ( cmd == L"getpart" )			cont = GetPart( param );
					if ( cmd == L"substr" )				cont = SubStr( param );
					if ( cmd == L"find" )				cont = Find( param );
					if ( cmd == L"reversefind" )		cont = ReverseFind( param );
					if ( cmd == L"makeupper" )			cont = MakeUpper( param );
					if ( cmd == L"makelower" )			cont = MakeLower( param );
					if ( cmd == L"getmortscripttype" )	cont = GetMortScriptType( param );

                    if ( cmd == L"repeat" )				cont = Repeat( param, startPos );
                    if ( cmd == L"endrepeat" )			cont = EndRepeat( param, startPos );
                    if ( cmd == L"errorlevel" )			cont = SetErrorLevel( param );

                    if ( cmd == L"callscript" )			cont = CallScript( param );
                    if ( cmd == L"call" )				cont = Call( param, startPos, startPos );
#endif

                    if ( cmd.CompareNoCase( L"Exit" )  == 0 )           cont = FALSE;
#ifndef DESKTOP
                    if ( cmd.CompareNoCase( L"Reset" )  == 0 )
                    {
                        KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
                        cont = FALSE;
                    }
#endif
#ifdef POCKETPC
                    if ( cmd.CompareNoCase( L"HardReset" )  == 0 )
                    {
                        CString hr;
                        if ( Variables.Lookup( L"HARDRESET", hr ) == TRUE && hr == L"1" )
                        {
                            SetCleanRebootFlag();
                            KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
                            cont = FALSE;
                        }
                    }
#endif
#ifndef DESKTOP
                    if ( cmd.CompareNoCase( L"IdleTimerReset" )  == 0 )
                    {
                        SystemIdleTimerReset();
                        cont = TRUE;
                    }

					if ( cmd == L"poweroff" ) { PowerOffSystem(); ::Sleep(1500); cont = TRUE; }
#endif
                }
                else
                {
                    cont = TRUE;
                }
                
                if ( cmd.CompareNoCase( L"If" )  == 0 )                cont = If( param, startPos );
                if ( cmd.CompareNoCase( L"Else" )  == 0 )              cont = Else();
                if ( cmd.CompareNoCase( L"EndIf" )  == 0 )             cont = EndIf();
                
                if ( cmd.CompareNoCase( L"While" )  == 0 )             cont = While( param, currStart );
                if ( cmd.CompareNoCase( L"EndWhile" )  == 0 )          cont = EndWhile( param, startPos );

#ifndef SMARTPHONE
                if ( cmd.CompareNoCase( L"ForEach" )  == 0 )           cont = ForEach( param, startPos );
                if ( cmd.CompareNoCase( L"EndForEach" )  == 0 )        cont = EndForEach( param, startPos );
#endif

                if ( cmd == L"switch" )                                cont = Switch( param, currStart );
#ifndef SMARTPHONE
                if ( cmd == L"choice" )                                cont = Choice( param, currStart );
                if ( cmd == L"choicedefault" )                         cont = ChoiceDefault( param, currStart );
#endif
                if ( cmd == L"case" )                                  cont = Case( param );
                if ( cmd == L"endchoice" || cmd == L"endswitch" )      cont = EndChoice();

#ifndef SMARTPHONE
                if ( cmd.CompareNoCase( L"Sub" )  == 0 )               cont = FALSE;
                if ( cmd.CompareNoCase( L"EndSub" )  == 0 )            cont = EndSub( startPos );
#endif
			}
            else
            {
                cont = TRUE;
            }

            if ( cont == 2 )
            {
                if ( ErrorLevel >= ERROR_SYNTAX )
                {
                    CString msg;
                    msg.Format( L"Unknow command '%s'", (LPCTSTR)cmd );
                    MessageBox( NULL, msg + ErrLine, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                }
                cont = FALSE;
            }
		}
	}
}
