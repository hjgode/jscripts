// MortRunnerSP.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <windowsx.h>
#include <aygshell.h>
#include "..\interpreter.h"
#include "..\parsecmdline.h"
#include "mortafx.h"
#include "resource.h"

#include "dlgwait.h"
#include "..\variables.h"
#include "..\helpers.h"
#include "..\DlgStatus.h"

#include "..\deelx.h" //RegExLib

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

	if ( dwProcessID == pFindApp->procId )
	{
		pFindApp->hWnd=hwnd;
		return FALSE;
	}

	return TRUE;
}

void RegisterFileClass( LPCTSTR exe )
{
	HKEY    key, subKey;
	DWORD   disposition;
	CStr label;

	if ( RegCreateKeyEx( HKEY_CLASSES_ROOT
					   , L"JScripts", 0
					   , L"JScripts", 0, 0, NULL, &key, &disposition ) == ERROR_SUCCESS )
	{
		label = L"JScripts";
		RegSetValueEx( key, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(USHORT)*(label.GetLength()+1) );

		if ( RegCreateKeyEx( key
						   , L"DefaultIcon", 0
						   , L"DefaultIcon", 0, 0, NULL, &subKey, &disposition ) == ERROR_SUCCESS )
		{
			//label = label.Left( label.GetLength()-3 ) + L"exe,-142";
			label.Format( L"%s,-%d", exe, IDI_RUNFILE );
			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(USHORT)*(CStr(label).GetLength()+1) );
			RegCloseKey( subKey );
		}

		if ( RegCreateKeyEx( key
						   , L"Shell\\Open\\Command", 0
						   , L"Command", 0, 0, NULL, &subKey, &disposition ) == ERROR_SUCCESS )
		{
			label = L"\"" + CStr(exe) + L"\" \"%1\"";
			RegSetValueEx( subKey, NULL, NULL, REG_SZ, (BYTE*)(LPCTSTR)label, sizeof(TCHAR)*(CStr(label).GetLength()+1) );
			RegCloseKey( subKey );
		}
		RegCloseKey( key );
	}
}

void RegisterFileType( LPCTSTR type, LPCTSTR typeClass )
{
	HKEY    key;
	DWORD   disposition;
	CStr label;

	if ( RegCreateKeyEx( HKEY_CLASSES_ROOT
					   , (LPTSTR)type, 0
					   , (LPTSTR)type, 0, 0, NULL, &key, &disposition ) == ERROR_SUCCESS )
	{
		RegSetValueEx( key, NULL, NULL, REG_SZ, (BYTE*)typeClass, sizeof(TCHAR)*(_tcslen(typeClass)+1) );
		RegCloseKey( key );
	}
}


/*****************************************************************************

  WinMain

  ***************************************************************************/

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine,
    int nCmdShow
    )
{

	CParseCmdLine myCmdLine;

	myCmdLine.ParseCmdLine( lpCmdLine );//Command Line Parse

	bool isCmdRun = false;
	CStr file = L"";

	if (lpCmdLine[0] == L'/' && (lpCmdLine[1] == L'c' || lpCmdLine[1] == L'C'))
	{
		file = lpCmdLine;
		file = file.Mid(2);
		file.TrimLeft();
		file.Replace(L"\\n",L"\n");
		isCmdRun = true;
	}else
		file = myCmdLine.Filename; // m_lpCmdLine;

	TCHAR exe[MAX_PATH];
       ::GetModuleFileName(NULL,exe,MAX_PATH);

	ExecuteFileName = exe;
	
    if ( !isCmdRun && (myCmdLine.RegOnly || file.IsEmpty()))
    {
		RegisterFileClass(exe);
		RegisterFileType( L".mscr",    L"JScripts" );
		RegisterFileType( L".jscr",    L"JScripts" );

        	MessageBox( NULL
                  , L".jscr and .mscr extensions registered.\nPlease run any .mscr/.jscr file or read the manual.\n\n"
                    L"(c) Mirko Schenk 2005-2009"
				  , L"JScripts V" + CStr( VERSION_INFO )
                  , MB_OK|MB_SETFOREGROUND );
    }else{
		int pos;
		for ( pos = wcslen(exe); pos > 0; pos-- )
		{
			if ( exe[pos] == L'\\' ) break;
		}
		exe[pos] = L'\0';
		AppPath = exe;

		if ( isCmdRun
			|| file.GetLength() >= 5 && file.Right(5).CompareNoCase( L".mscr" ) == 0
			|| file.GetLength() >= 5 && file.Right(5).CompareNoCase( L".jscr" ) == 0
		   )
		{
            CStr mutexName = file;
            mutexName.MakeLower();
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
						FindAppT findApp;
						findApp.procId = procId;
						findApp.hWnd   = NULL;
						::EnumWindows( FindApplicationWindowProc, (LPARAM)&findApp );
						
						if ( findApp.hWnd != NULL ){
							::SetForegroundWindow( findApp.hWnd );
						}
					}
					else
						exists = 0;

                }

                if ( exists != ERROR_ALREADY_EXISTS )
                {
	                HKEY    key;
	                if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, 0, &key ) == ERROR_SUCCESS )
	                {
						RegDeleteValue( key, MutexName );
						RegCloseKey( key );
	                }

					//CStr dbg;
					//dbg.Format(L"ProcId: %d", GetCurrentProcessId());
					//MessageBox( NULL, dbg, L"Debug", MB_SETFOREGROUND );
					RegWriteDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", MutexName, GetCurrentProcessId() );

					if ( myCmdLine.WaitForFile > 0 && (long)FileOrDirExists( file, 0 ) == 0 )
					{
						CDlgWait wait( NULL );
						wait.AllowOK = TRUE;
						wait.Countdown = myCmdLine.WaitForFile;
						wait.Expression = L"FileExists(\"" + file + L"\")";
						wait.Title = L"JScripts";
						wait.m_Label = L"Script " + file + L" not found, waiting for existance (storage card not initialized?)";
						wait.DoModal();
					}

					if ( isCmdRun || myCmdLine.WaitForFile <= 0 || (long)FileOrDirExists( file, 0 ) == 1 )
					{
						g_hInst = hInstance;

						SYSTEMTIME now;
						GetLocalTime( &now );
						long seed = SystemTimeToUnixTime( now );
						srand( seed );

						ScriptAborted = CreateEvent( 0, TRUE, FALSE, CStr(MutexName)+L"ScriptAborted" );
					    StatusDialog = new CDlgStatus( file );

						CInterpreter interpreter;
						interpreter.RunFile( file , isCmdRun );
						interpreter.Parser();

						if ( StatusWindow != NULL )
						{
							StatusDialog->ScriptFinished = TRUE;
							SendMessage( StatusWindow, WM_EXIT_STATUS, 0, 0 );
							WaitForSingleObject( StatusDialogFinished, INFINITE );
							CloseHandle( StatusDialogFinished );
						}

						delete StatusDialog;

						UnloadToolhelp();

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

						if ( StatusListFont != NULL ) DeleteObject( StatusListFont );
						if ( ChoiceFont != NULL ) DeleteObject( ChoiceFont );
					}

					if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, KEY_WRITE, &key ) == ERROR_SUCCESS )
	                {
                        RegDeleteValue( key, MutexName );
		                RegCloseKey( key );
	                }
                }
                CloseHandle( mutex );
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

    return TRUE;
}

// end MortRunnerSP.cpp
