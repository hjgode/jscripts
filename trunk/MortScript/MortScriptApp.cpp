// MortScriptApp.cpp : Defines the class behaviors for the application.
//

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#endif

#ifdef POCKETPC
#include "stdafx.h"
#include "resource.h"
#endif
#ifndef DESKTOP
#include "Kfuncs.h"
#endif

#include "Interpreter.h"
#include "MortScriptApp.h"
#include "DlgWait.h"
#include "ParseCmdLine.h"
#include "morttypes.h"
#include "variables.h"
#include "time.h"
#include "DlgStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HFONT ChoiceFont, StatusListFont;
extern CMortPtrArray				LocalVariablesStack;

HINSTANCE g_hInst = NULL;  // Local copy of hInstance
LPCTSTR MutexName;

HANDLE ScriptAborted;

typedef struct _FindAppT
{
	DWORD procId;
	HWND  hWnd;
} 
FindAppT, *pFindAppT;

BOOL CALLBACK FindApplicationWindowProc(HWND hwnd, LPARAM lParam)
{
	DWORD		dwProcessID;
	pFindAppT	pFindApp=(pFindAppT)lParam;

	::GetWindowThreadProcessId(hwnd,&dwProcessID);
	if (   dwProcessID == pFindApp->procId
		&& ::IsWindowVisible( hwnd )
		&& ( ::GetParent( hwnd ) == NULL || ::GetParent( hwnd ) == ::GetDesktopWindow() )
	   )
	{
		// TCHAR windowTitle[256];
		// int len = ::GetWindowText( hwnd, windowTitle, 255 );
		// CString msg;
		// msg.Format( L"Found window for id %08x: %08x - %s", pFindApp->procId, hwnd, windowTitle );
		// MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );

		pFindApp->hWnd=hwnd;
		return TRUE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CMortScriptApp object

CMortScriptApp theApp;

BEGIN_MESSAGE_MAP(CMortScriptApp, CWinApp)
	//{{AFX_MSG_MAP(CMortStarterApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMortScriptApp::CMortScriptApp()
	: CWinApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMortScriptApp initialization

BOOL CMortScriptApp::InitInstance()
{
    SetRegistryKey( L"Mort" );

	CParseCmdLine myCmdLine;
	myCmdLine.ParseCmdLine( m_lpCmdLine );

	TCHAR exe[MAX_PATH];
   ::GetModuleFileName(NULL,exe,MAX_PATH);

	ExecuteFileName = exe;

    CStr file = myCmdLine.Filename; // m_lpCmdLine;
    // file.TrimLeft( '\"' ); file.TrimRight( '\"' );

    if ( myCmdLine.RegOnly || file.IsEmpty() )
    {
        RegisterFileClass();
        RegisterFileType( L".jscr", L"JScripts" );
        RegisterFileType( L".mscr", L"JScripts" );
		if ( !myCmdLine.RegOnly )
		{
			MessageBox( NULL
					  , L".jscr and .mscr extensions registered.\nPlease run any .jscr/.mscr file or read the manual.\n\n"
						L"(c) Mirko Schenk 2005-2007"
					  , L"JScripts V" + CStr( VERSION_INFO )
					  , MB_OK|MB_SETFOREGROUND );
		}
    }
	else
	{
		if (   file.GetLength() >= 4
			/* file.GetLength() >= 8 && file.Right(8).CompareNoCase( L".mortrun" ) == 0
			|| file.GetLength() >= 5 && file.Right(5).CompareNoCase( L".mscr" ) == 0 */
		   )
		{
            CStr mutexName = file;
            mutexName.MakeLower();
#ifdef DESKTOP
			// Windows XP doesn't like some path characters in the mutex' name
			mutexName.Replace( ':', '_' );
			mutexName.Replace( '\\', '/' );
#endif
          MutexName = (LPCTSTR)mutexName;

	        HANDLE mutex = ::CreateMutex(NULL, FALSE, MutexName);

            if ( mutex!=NULL )
	        {
				int exists = ::GetLastError();
		        if ( exists == ERROR_ALREADY_EXISTS) 
		        {
					DWORD procId = GetRunningScriptProcId( file );

					if ( procId != NULL )
					{
						/*
						 CString msg;
						 msg.Format( L"Process ID: %08x", procId );
						 MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );
						*/

						FindAppT findApp;
						findApp.procId = procId;
						findApp.hWnd   = NULL;
						::EnumWindows( FindApplicationWindowProc, (LPARAM)&findApp );
						if ( findApp.hWnd != NULL )
						{
							// msg.Format( L"Set foreground window: %08x", findApp.hWnd );
							// MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );
							::SetForegroundWindow( findApp.hWnd );
						}
					}
					else
						exists = 0;

					//MessageBox( NULL, L"Process opened", L"Debug", MB_SETFOREGROUND );
					/*
						TCHAR procName[256];
				    	::GetModuleFileName((HMODULE)procId,procName,256);
						//MessageBox( NULL, procName, L"Debug", MB_SETFOREGROUND );
						if ( CString(procName).Right(14).CompareNoCase( L"MortScript.exe" ) == 0 )
						{
							int aw = MessageBox( NULL
											   , L"Script seems to be running. Cancel old script?"
											   , L"Script already running"
											   , MB_YESNO|MB_SETFOREGROUND
											   );
							if ( aw == IDYES )
							{
								RegWriteDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Abort", MutexName, 1 );
								DWORD exitCode = 0;
								SetCursor(LoadStandardCursor(IDC_WAIT));
								for ( int i=0; i<=10; i++ )
								{
									Sleep(1000);
									if ( GetExitCodeProcess( hProc, &exitCode ) == FALSE )
									{
										//MessageBox( NULL, L"GetExitCode failed", L"Debug", MB_SETFOREGROUND );
										exitCode = 0;
										break;
									}
									else
									{
										if ( exitCode != STILL_ACTIVE )
										{
											//MessageBox( NULL, L"No longer active", L"Debug", MB_SETFOREGROUND );
											break;
										}
									}
								}
								SetCursor(LoadStandardCursor(IDC_ARROW)); 
								if ( exitCode == STILL_ACTIVE )
								{
									int aw = MessageBox( NULL
													   , L"Script seems to be hanging or busy. Terminate old script?"
													   , L"Script still running"
													   , MB_YESNO|MB_SETFOREGROUND
													   );
									if ( aw == IDYES )
									{
										TerminateProcess( hProc, 0 );
									}
								}
							}
						}
						else
						{
							exists = 0;
						}

						CloseHandle( hProc );
					}
					else
					{
						exists = 0;
					}
					*/
                }

                if ( exists != ERROR_ALREADY_EXISTS )
                {
	                HKEY    key;
	                if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, 0, &key ) == ERROR_SUCCESS )
	                {
                        RegDeleteValue( key, MutexName );
		                RegCloseKey( key );
	                }

					DWORD currProcId = GetCurrentProcessId();
					//CString dbg;
					//dbg.Format(L"ProcId: %d", GetCurrentProcessId());
					//MessageBox( NULL, dbg, L"Debug", MB_SETFOREGROUND );

					// Remove old script entries with same process id
	                if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, 0, &key ) == ERROR_SUCCESS )
	                {
						int   idx;
						TCHAR valName[MAX_PATH];
						DWORD valSize = MAX_PATH, type;
						DWORD value, valueSize = sizeof(DWORD);

						CStrArray oldProcesses;
						for ( idx = 0; RegEnumValue( key, idx, valName, &valSize, NULL, &type, (BYTE*)&value, &valueSize ) == ERROR_SUCCESS; idx++ )
						{
							if ( type == REG_DWORD && value == currProcId )
							{
								oldProcesses.Add( valName );
							}
							valSize = MAX_PATH;
							valueSize = sizeof(DWORD);
						}

						for ( idx = 0; idx < oldProcesses.GetSize(); idx++ )
						{
							RegDeleteValue( key, oldProcesses.GetAt(idx) );
						}

		                RegCloseKey( key );
					}

					RegWriteDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", MutexName, currProcId );

					if ( myCmdLine.WaitForFile > 0 && (long)FileOrDirExists( file, 0 ) == 0 )
					{
						CDlgWait wait(NULL);
						wait.AllowOK = TRUE;
						wait.Countdown = myCmdLine.WaitForFile;
						wait.Expression = L"FileExists(\"" + file + L"\")";
						wait.Title = L"JScripts";
						wait.m_Label = L"Script " + file + " not found, waiting for existance (storage card not initialized?)";
						wait.DoModal();
					}

					if ( myCmdLine.WaitForFile <= 0 || (long)FileOrDirExists( file, 0 ) == 1 )
					{
						g_hInst = theApp.m_hInstance;

						SYSTEMTIME now;
						GetLocalTime( &now );
						long seed = SystemTimeToUnixTime( now );
						srand( seed );

						AppPath = m_pszHelpFilePath;
						AppPath = AppPath.Left( AppPath.ReverseFind('\\') );

						ScriptAborted = CreateEvent( 0, TRUE, FALSE, CStr(MutexName)+L"ScriptAborted" );
					    StatusDialog = new CDlgStatus( file );
						//status.DoModal();

						CInterpreter interpreter;
						//Debug(file);

						//jwz:modi
						interpreter.RunFile( file );
						interpreter.Parser();
						//jwz:modi end

						if ( StatusWindow != NULL )
						{
							StatusDialog->ScriptFinished = TRUE;
							SendMessage( StatusWindow, WM_EXIT_STATUS, 0, 0 );
							WaitForSingleObject( StatusDialogFinished, INFINITE );
							CloseHandle( StatusDialogFinished );
						}

						delete StatusDialog;

						UnloadToolhelp();

						CloseHandle( ScriptAborted );
						ReleaseMutex( mutex );

						Variables.RemoveAll();
						for ( int i = 0; i<LocalVariablesStack.GetSize(); i++ )
							delete (CMapStrToValue*)LocalVariablesStack.GetAt(i);

						POSITION pos = FileHandles.GetStartPosition();
						CStr key; void *value;
						while ( pos != 0 )
						{
							FileHandles.GetNextAssoc( pos, key, value );
							if ( value != NULL )
							{
								delete (CFileInfo*)value;
							}
						}
						FileHandles.RemoveAll();

						if ( ChoiceFont != NULL ) DeleteObject( ChoiceFont );
						if ( StatusListFont != NULL ) DeleteObject( StatusListFont );
					}

					if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, KEY_WRITE, &key ) == ERROR_SUCCESS )
	                {
                        RegDeleteValue( key, MutexName );
		                RegCloseKey( key );
	                }
                }
                CloseHandle( mutex );
            }
			else
			{
				int error = ::GetLastError();
				MessageBox( NULL
						  , L"Error creating mutex"
						  , L"JScripts"
						  , MB_OK|MB_SETFOREGROUND );
			}
		}
		else
		{
			MessageBox( NULL
					  , L"Invalid file type for MortScript"
					  , L"JScripts"
					  , MB_OK|MB_SETFOREGROUND );
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


void CMortScriptApp::RegisterFileClass()
{
	HKEY    key, subKey;
	DWORD   disposition;
	CStr label,exe;

	if ( RegCreateKeyEx( HKEY_CLASSES_ROOT
					   , L"JScripts", 0
					   , L"JScripts", 0
#ifndef DESKTOP
             , 0
#else
             , KEY_WRITE
#endif
             , NULL, &key, &disposition ) == ERROR_SUCCESS )
	{
		label = L"JScripts";
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

			exe.Format( L"\"%s.exe\"",(LPCTSTR)label.Left( label.GetLength()-4 ) ); //Get current exe filename!
			exe = exe + " \"%1\"";

#ifndef DESKTOP
			label.Format( L"%s.exe,-%d", (LPCTSTR)label.Left( label.GetLength()-4 ), IDI_RUNFILE );
#else
			label.Format( L"%s.exe,1", (LPCTSTR)label.Left( label.GetLength()-4 ) );
#endif

			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(USHORT)*(CStr(label).GetLength()+1) );
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
			//label = L"\"" + GetRelativeToAppPath( L"MortScript.exe\" \"%1\"" ); //jwz:modi with up exe.Format(....

			//label = L"\"" + label.Left( label.GetLength()-3 ) + L"exe\" \"%1\"";
			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)exe/*jwz:modi org is:label*/
				, sizeof(TCHAR)*(CStr(exe).GetLength()+1) );
			RegCloseKey( subKey );
		}
		RegCloseKey( key );
	}
}

void CMortScriptApp::RegisterFileType( LPCTSTR type, LPCTSTR typeClass )
{
	HKEY    key;
	DWORD   disposition;
	CStr label;

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

CStr CMortScriptApp::GetRelativeToAppPath( LPCTSTR file )
{
	CStr path = m_pszHelpFilePath;
	//Debug(path);
	//exit(0);
	path = path.Left( path.ReverseFind('\\') + 1 ) + file;
	return path;
}
