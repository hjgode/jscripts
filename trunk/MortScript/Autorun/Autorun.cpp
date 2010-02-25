// Autorun.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
#ifdef _WIN32_WCE
					LPTSTR    lpCmdLine,
#else
					LPSTR     lpCmdLine,
#endif
					int       nCmdShow)
{
	//::MessageBox( NULL, L"Looking for MortButtons window", L"Debug", MB_OK );
	TCHAR module[MAX_PATH];
	TCHAR exe[MAX_PATH];
	TCHAR param[MAX_PATH];
	DWORD attribs;

    if ( ::GetModuleFileName(hInstance,module,MAX_PATH) == 0 )
	{
#ifdef PNA
		MessageBox( NULL, L"Couldn't determine current path", L"Autorun", MB_OK );
		return 0;
#else
        SHGetAutoRunPath( module );
#endif
	}

    wcscpy( exe, module );
	int pos;
	for ( pos = wcslen(exe); pos > 0; pos-- )
	{
		if ( exe[pos] == L'\\' ) break;
	}
	exe[pos+1] = L'\0';
	wcscpy( param, exe );
	wcscat( exe,   L"MortScript.exe" );

#ifndef PNA
    if ( wcscmp( lpCmdLine, L"install" ) == 0 )
    {
        attribs = GetFileAttributes( exe );
        if ( attribs == -1 )
        {
			SHGetAutoRunPath( module );

			wcscpy( exe, module );
			int pos;
			for ( pos = wcslen(exe); pos > 0; pos-- )
			{
				if ( exe[pos] == L'\\' ) break;
			}
			exe[pos+1] = L'\0';
			wcscpy( param, exe );
			wcscat( exe,   L"MortScript.exe" );
	        attribs = GetFileAttributes( exe );
		}
        if ( attribs == -1 )
        {
            WIN32_FIND_DATA   fflash;
            HANDLE findflash = FindFirstFlashCard( &fflash );
            while ( INVALID_HANDLE_VALUE != findflash )
            {
                wsprintf( exe, L"\\%s\\2577\\MortScript.exe", fflash.cFileName );
                attribs = GetFileAttributes( exe );
                if ( attribs != -1 )
                {
                    break;
                }

                wsprintf( exe, L"\\%s\\1824\\MortScript.exe", fflash.cFileName );
                attribs = GetFileAttributes( exe );
                if ( attribs != -1 )
                {
                    break;
                }

                wsprintf( exe, L"\\%s\\0\\MortScript.exe", fflash.cFileName );
                attribs = GetFileAttributes( exe );
                if ( attribs != -1 )
                {
                    break;
                }

                if ( !FindNextFlashCard( findflash, &fflash ) )
                {
                    FindClose( findflash );
                    findflash = INVALID_HANDLE_VALUE;
                    // exe[0] = '\0';
                }
            }

            if ( INVALID_HANDLE_VALUE != findflash )
                FindClose( findflash );
        }
    }
#endif

//    TCHAR msg[2000];
//    swprintf( msg, L"exe: %s\nparam: %s\nautorun path: %s", exe, lpCmdLine, module );
//    MessageBox( NULL, msg, L"Debug", MB_OK );
//    swprintf( msg, L"exe: %s\nparam: %s\nautorun path: %s", exe, lpCmdLine, module );
//    MessageBox( NULL, msg, L"Debug", MB_OK );

#ifdef _WIN32_WCE
    if ( wcscmp( lpCmdLine, L"install" ) == 0 )
    {
        TCHAR uninst[MAX_PATH];
    	wcscpy( uninst, param );
        wcscat( uninst, L"Autoexit.mscr" );
        attribs = GetFileAttributes( uninst );
        if ( attribs == -1 )
        {
    	    wcscpy( uninst, param );
            wcscat( uninst, L"Autoexit.mortrun" );
            attribs = GetFileAttributes( uninst );
        }
        if ( attribs != -1 )
        {
            TCHAR target[MAX_PATH], target2[MAX_PATH];
            ::GetModuleFileName(NULL,target,MAX_PATH);
	        for ( pos = wcslen(target); pos > 0; pos-- )
	        {
		        if ( target[pos] == L'\\' ) break;
	        }
	        target[pos+1] = L'\0';
            wcscpy( target2, target );
	        wcscat( target, L"autoexit.mscr" );
            wcscat( target2, L"MortScript.exe" );

//    swprintf( msg, L"copy %s to %s", uninst, target );
//    MessageBox( NULL, msg, L"Debug", MB_OK );
            CopyFile( uninst, target, FALSE );
//    swprintf( msg, L"copy %s to %s", exe, target2 );
//    MessageBox( NULL, msg, L"Debug", MB_OK );
            CopyFile( exe, target2, FALSE );
        }

    	wcscpy( uninst, param );
        wcscat( uninst, L"Autorun.mscr" );
        attribs = GetFileAttributes( uninst );
        if ( attribs == -1 )
        {
    	    wcscpy( uninst, param );
            wcscat( uninst, L"Autorun.mortrun" );
        }
    	wcscpy( param, uninst );

//        swprintf( msg, L"file: %s\nparam: %s", exe, param );
//        MessageBox( NULL, msg, L"Debug", MB_OK );
    }
    else
    {
        if ( wcscmp( lpCmdLine, L"uninstall" ) == 0 )
        {
        	wcscat( param, L"Autoexit.mscr" );
        }
        else
        {
#endif
        	wcsncpy( param, module, wcslen(module)-3 );
			param[wcslen(module)-3] = '\0';
            wcscat( param, L"mscr" );
            attribs = GetFileAttributes( param );
            if ( attribs == -1 )
            {
            	wcsncpy( param, module, wcslen(module)-3 );
				param[wcslen(module)-3] = '\0';
                wcscat( param, L"mortrun" );
            }
#ifdef _WIN32_WCE
        }
    }
#endif

    //swprintf( msg, L"exe: %s\nparam: %s", exe, param );

    attribs = GetFileAttributes( param );
    if ( attribs != -1 )
    {
		TCHAR quotedExe[MAX_PATH];
		TCHAR quotedParam[MAX_PATH*2+1];
		wsprintf( quotedExe,   L"\"%s\"", exe );
#ifndef _WIN32_WCE
		wsprintf( quotedParam, L"\"%s\" \"%s\"", exe, param );
#else
		wsprintf( quotedParam, L"\"%s\"", param );
#endif

	    attribs = GetFileAttributes( exe );
        if ( attribs != -1 ) // (wcscmp( lpCmdLine, L"install" ) == 0 || wcscmp( lpCmdLine, L"uninstall" ) == 0 )
        {
            PROCESS_INFORMATION inf;
		    memset(&inf, 0, sizeof(inf));
		    STARTUPINFO *startup = NULL;

#ifndef _WIN32_WCE
		    STARTUPINFO info;
		    memset(&info, 0, sizeof(info));
		    info.cb = sizeof(info);
		    info.wShowWindow = SW_SHOW;
		    info.dwFlags = STARTF_USESHOWWINDOW;
			startup = &info;
#endif


            BOOL rc = CreateProcess( exe, quotedParam, NULL, NULL, FALSE, 0, NULL, NULL, startup, &inf );
            if ( rc )
            {
                DWORD exitCode;
                while ( GetExitCodeProcess( inf.hProcess, &exitCode ) != FALSE && exitCode == STILL_ACTIVE )
                {
                    ::Sleep( 100 );
                }
                CloseHandle( inf.hProcess );
            }

#ifdef _WIN32_WCE
            if ( wcscmp( lpCmdLine, L"uninstall" ) == 0 )
            {
                Sleep( 1000 );
                DeleteFile( param );
            }
#endif
        }
        else
        {
        //swprintf( msg, L"param: %s", exe, quotedParam );
        //MessageBox( NULL, msg, L"Debug", MB_OK );
            SHELLEXECUTEINFO sei;
            sei.cbSize = sizeof(SHELLEXECUTEINFO);
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            sei.hwnd  = NULL;
            sei.lpVerb = L"Open";
            sei.lpDirectory = L"\\";
            sei.lpFile = (LPCTSTR)quotedParam;
            sei.lpParameters = NULL;
            sei.nShow = SW_SHOW;
            sei.hInstApp = NULL;

            ShellExecuteEx( &sei );
        }
    }

	return 0;
}

