// SetupDll.cpp : Defines the initialization routines for the DLL.
//

#include "windows.h"
#include <windowsx.h>
#include <aygshell.h>
#include "ce_setup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

codeINSTALL_INIT
Install_Init( HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled, LPCTSTR pszInstallDir )
{
    return codeINSTALL_INIT_CONTINUE;
}

codeINSTALL_EXIT
Install_Exit( HWND hwndParent, LPCTSTR pszInstallDir, WORD cFailedDirs, WORD cFailedFiles, WORD cFailedRegKeys, WORD cFailedRegVals, WORD cFailedShortcuts )
{
	TCHAR exe[MAX_PATH];
	TCHAR script[MAX_PATH];

	exe[0] = '\0';
	script[0] = '\0';

	/*
	if ( pszInstallDir == NULL )
	{
		MessageBox( hwndParent, L"pszInstallDir is empty", L"Debug", MB_OK );
	}
	else
	{
		MessageBox( hwndParent, pszInstallDir, L"Debug pszInstallDir", MB_OK );
	}
	*/

	HKEY key;
	if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Mort\\MortScript\\setup.dll", 0, 0, &key ) == ERROR_SUCCESS )
	{
		DWORD type, length;
		TCHAR cont[MAX_PATH];
		length = MAX_PATH;
		if ( RegQueryValueEx( key, L"Path", NULL, &type, (BYTE*)cont, &length ) == ERROR_SUCCESS )
		{
			wcscpy( script, cont );
			wcscat( script, L"\\install.mscr" );
			DWORD attribs = GetFileAttributes( script );
			if ( attribs != -1 )
			{
				// MessageBox( hwndParent, script, L"Found", MB_OK );
				wcscpy( exe, cont );
				wcscat( exe, L"\\MortScript.exe" );
			}
			else
			{
				MessageBox( hwndParent, script, L"Not found", MB_OK );
			}
			RegDeleteValue( key, L"Path" );
		}
		//else
		//{
		//	MessageBox( hwndParent, L"No Path value in setup.dll key", L"Debug", MB_OK );
		//}
		RegCloseKey( key );
	}

	if ( exe[0] == '\0' && pszInstallDir != NULL )
	{
	    wcscpy( exe, pszInstallDir );
		wcscat( exe, L"\\MortScript.exe" );
		wcscpy( script, pszInstallDir );
		wcscat( script, L"\\install.mscr" );
	}

	if ( exe[0] != '\0' )
	{
		DWORD attribs = GetFileAttributes( script );
		if ( attribs != -1 )
		{
			TCHAR quScript[MAX_PATH+2];
			wcscpy( quScript, L"\"" );
			wcscat( quScript, script );
			wcscat( quScript, L"\"" );

			PROCESS_INFORMATION inf;
			BOOL rc = CreateProcess( (LPCTSTR)exe, (LPCTSTR)quScript, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &inf );
			if ( rc )
			{
				DWORD exitCode;
				while ( GetExitCodeProcess( inf.hProcess, &exitCode ) != FALSE && exitCode == STILL_ACTIVE )
				{
					::Sleep( 100 );
				}
				CloseHandle( inf.hProcess );
			}
		}
	}

    return codeINSTALL_EXIT_DONE;
}

codeUNINSTALL_INIT
Uninstall_Init( HWND hwndParent, LPCTSTR pszInstallDir )
{
	TCHAR exe[MAX_PATH];
	TCHAR script[MAX_PATH];

	if ( pszInstallDir != NULL && wcslen( pszInstallDir ) > 0 )
	{
		wcscpy( exe, pszInstallDir );
		wcscat( exe, L"\\MortScript.exe" );
		wcscpy( script, pszInstallDir );
		wcscat( script, L"\\uninstall.mscr" );

		DWORD attribs = GetFileAttributes( script );
		if ( attribs != -1 )
		{
			TCHAR quScript[MAX_PATH+2];
			wcscpy( quScript, L"\"" );
			wcscat( quScript, script );
			wcscat( quScript, L"\"" );
        
			PROCESS_INFORMATION inf;
			BOOL rc = CreateProcess( (LPCTSTR)exe, (LPCTSTR)quScript, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &inf );
			if ( rc )
			{
				DWORD exitCode;
				while ( GetExitCodeProcess( inf.hProcess, &exitCode ) != FALSE && exitCode == STILL_ACTIVE )
				{
					::Sleep( 100 );
				}
				CloseHandle( inf.hProcess );
			}
		}
	}

    return codeUNINSTALL_INIT_CONTINUE;
}

codeUNINSTALL_EXIT
Uninstall_Exit( HWND hwndParent )
{
    return codeUNINSTALL_EXIT_DONE;
}
