#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "winuser.h"

#include "inifile.h"
#include "interpreter.h"

#include "deelx.h"

#ifdef SMARTPHONE
#include <bthutil.h>
#include "FctFlight.h"
#endif

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

extern CInterpreter *CurrentInterpreter; //jwz::add


#ifndef PNA
extern CStr Proxy;
#include "wininet.h"
#endif

#ifdef SMARTPHONE
	#define LoadStdCursor(x) LoadCursor(NULL,x)
#else
	#define LoadStdCursor(x) theApp.LoadStandardCursor(x)
#endif

#include "deelx.h" //RegExLib

#if !defined( PNA ) && !defined( DESKTOP )
#include "ras.h"
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "ValueArray.h"
#include "FunctionsSystem.h"
#include "Interpreter.h"
#include "variables.h"

CValue  FctExternalPowered( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
#ifdef DESKTOP
    rc = 1L;
#else
    SYSTEM_POWER_STATUS_EX pwrStatus;
    GetSystemPowerStatusEx( &pwrStatus, TRUE );
    if ( pwrStatus.ACLineStatus == 1 || pwrStatus.BatteryFlag == 8 /*charging*/ || pwrStatus.BatteryFlag == 128 /* no battery */ || pwrStatus.BatteryLifePercent > 100 )
        rc = 1L;
    else
        rc = 0L;
#endif
    return rc;
}

CValue  FctBatteryPercentage( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
#ifdef DESKTOP
    rc = 100L;
#else
    SYSTEM_POWER_STATUS_EX pwrStatus;
    GetSystemPowerStatusEx( &pwrStatus, TRUE );
    if ( pwrStatus.BatteryLifePercent > 100 ) pwrStatus.BatteryLifePercent = 100;
    rc = (long)pwrStatus.BatteryLifePercent;
#endif
    return rc;
}

CValue  FctBackupBatteryPercentage( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
#ifdef DESKTOP
    rc = 100L;
#else
    SYSTEM_POWER_STATUS_EX pwrStatus;
    GetSystemPowerStatusEx( &pwrStatus, TRUE );
    if ( pwrStatus.BackupBatteryLifePercent > 100 ) pwrStatus.BackupBatteryLifePercent = 100;
    rc = (long)pwrStatus.BackupBatteryLifePercent;
#endif
    return rc;
}


CValue  FctActiveProcess( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() > 1 )
    {
        errorMessage = InvalidParameterCount + L"'ActiveProcess'";
        error = 9;
        return CValue();
    }

	//if ( LoadToolhelp() )
	//{
		HWND  hwnd = ::GetForegroundWindow();
		// Sometimes, after a window was closed, Windows isn't able to return the active window.
		// So wait until one becomes active...
		for ( int retry = 0; hwnd == NULL && retry < 10; retry++ )
		{
			Sleep(10);
			hwnd = ::GetForegroundWindow();
		}

		CStr procName;
		DWORD dwProcessID;

		::GetWindowThreadProcessId(hwnd,&dwProcessID);
		int e = GetLastError();
		if (dwProcessID)
		{
			procName = GetProcessExePath( dwProcessID );

			if ( params.GetSize() == 0 || (long)params[0] == 0 )
			{
				int pos = procName.ReverseFind( '\\' );
				if ( pos == -1 )
					rc = procName;
				else
					rc = procName.Mid( pos+1 );
			}
			else
			{
				rc = procName;
			}
		}
	//}
	//else
	//{
    //    errorMessage = L"'ActiveProcess' requires toolhelp.dll on your device";
    //    error = 9;
    //    return CValue();
	//}
    
    return rc;
}

CValue  FctWindowProcess( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() < 1 || params.GetSize() > 2 )
    {
        errorMessage = InvalidParameterCount + L"'WindowProcess'";
        error = 9;
        return CValue();
    }

	//if ( LoadToolhelp() )
	//{
		HWND  hwnd = FindWindowMy( params[0] ,0);
		CStr procName;
		DWORD dwProcessID;

		::GetWindowThreadProcessId(hwnd,&dwProcessID);
		if (dwProcessID)
		{
			procName = GetProcessExePath( dwProcessID );

			if ( params.GetSize() == 1 || (long)params[1] == 0 )
			{
				int pos = procName.ReverseFind( '\\' );
				if ( pos == -1 )
					rc = procName;
				else
					rc = procName.Mid( pos+1 );
			}
			else
			{
				rc = procName;
			}
		}
	//}
	//else
	//{
    //    errorMessage = L"'WindowProcess' requires toolhelp.dll on your device";
    //    error = 9;
    //    return CValue();
	//}
    
    return rc;
}

CValue  FctProcExists( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ProcExists'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = 0L;

	if ( LoadToolhelp() )
	{
		CStr search = params[0];
		BOOL fullPath = FALSE;
		if ( search.Find('\\') != -1 )
			fullPath = TRUE;

	    HANDLE  procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		if ( procSnap == NULL || (int)procSnap == -1 )
		{
			error = GetLastError();
			errorMessage.Format( L"ProcExists failed, error code %d", error );
			error = 9;
		}
	    PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && (int)procSnap != -1 && Process32First( procSnap, &procEntry ) )
		{
			CStr procName;
			do
			{
				if ( fullPath )
				{
					procName = GetProcessExePath( (DWORD)procEntry.th32ProcessID, procEntry.szExeFile );
				}
				else
				{
					procName = procEntry.szExeFile;
				}


				//Debug(procName,(long)procEntry.th32ProcessID);

				if ( procName.CompareNoCase( search ) == 0 )
				{
					rc = 1L;

					break;
				}
				procEntry.dwSize = sizeof( procEntry );
			}
			while ( Process32Next( procSnap, &procEntry ) );
		}

		if ( procSnap != NULL && (int)procSnap != -1 )
#ifndef DESKTOP
	      CloseToolhelp32Snapshot( procSnap );
#else
		  CloseHandle( procSnap );
#endif
	}
	else
	{
        errorMessage = L"'ProcExists' requires toolhelp.dll on your device";
        error = 9;
        return CValue();
	}

    return rc;
}

//jwz::add
CValue  FctGetWindowFromProc( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'GetWindowFromProc'";
        error = 9;
        return CValue();
    }

    CValue rc=L"";

	if ( LoadToolhelp() )
	{
		CStr search = params[0];
		BOOL fullPath = FALSE;
		if ( search.Find('\\') != -1 )
			fullPath = TRUE;

	    HANDLE  procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		if ( procSnap == NULL || (int)procSnap == -1 )
		{
			error = GetLastError();
			errorMessage.Format( L"GetWindowFromProc failed, error code %d", error );
			error = 9;
		}
	    PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && (int)procSnap != -1 && Process32First( procSnap, &procEntry ) )
		{
			CStr procName;
			do
			{
				if ( fullPath )
				{
					procName = GetProcessExePath( (DWORD)procEntry.th32ProcessID, procEntry.szExeFile );
				}
				else
				{
					procName = procEntry.szExeFile;
				}

				if ( procName.CompareNoCase( search ) == 0 && (DWORD)procEntry.th32ParentProcessID == 0)
				{
					rc = GetWindowFromProcess((DWORD)procEntry.th32ProcessID);

					break;
				}
				procEntry.dwSize = sizeof( procEntry );
			}
			while ( Process32Next( procSnap, &procEntry ) );
		}

		if ( procSnap != NULL && (int)procSnap != -1 )
#ifndef DESKTOP
	      CloseToolhelp32Snapshot( procSnap );
#else
		  CloseHandle( procSnap );
#endif
	}
	else
	{
        errorMessage = L"'GetWindowFromProc' requires toolhelp.dll on your device";
        error = 9;
        return CValue();
	}

    return rc;
}
//jwz::add end

CValue  FctScriptProcExists( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ScriptProcExists'";
        error = 9;
        return CValue();
    }

    CValue rc;
    if ( GetRunningScriptProcId( params[0] ) == 0 )
        rc = 0L;
    else
        rc = 1L;
    return rc;
}

CValue  FctClipText( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    OpenClipboard(NULL); 
    HANDLE clip=GetClipboardData(CF_UNICODETEXT);
    if ( clip != NULL )
    {
        CStr str;
        str.Format( L"%s", clip );
        rc = str;
    }
    CloseClipboard();

    return rc;
}

CValue  FctVolume( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

	DWORD volume;
	waveOutGetVolume( 0, &volume );

	BYTE  *volBytes = (BYTE*)&volume; // direct access to bytes
    
	rc = (long)( (volBytes[1]+volBytes[3]) / 2 ); // Average of left and right channel

    return rc;
}

CValue  FctTimeStamp( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    SYSTEMTIME now;
    GetLocalTime( &now );
    rc = (long)SystemTimeToUnixTime( now );

    return rc;
}

CValue  FctTimeStampUTC( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    SYSTEMTIME now;
    GetSystemTime( &now );
    rc = (long)SystemTimeToUnixTime( now );

    return rc;
}

CValue FctTimeZoneBias( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;
	TIME_ZONE_INFORMATION tzi;
	int type = GetTimeZoneInformation( &tzi );
	if ( type == TIME_ZONE_ID_DAYLIGHT )
		rc = -tzi.Bias-tzi.DaylightBias;
	else
		rc = -tzi.Bias-tzi.StandardBias;

	return rc;
}

CValue FctTimeZoneName( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;

	TIME_ZONE_INFORMATION tzi;
	int type = GetTimeZoneInformation( &tzi );
	if ( type == TIME_ZONE_ID_DAYLIGHT )
		rc = tzi.DaylightName;
	else
		rc = tzi.StandardName;

	return rc;
}

CValue FctTimeZoneDST( CValueArray &params, int &error, CStr &errorMessage )
{
	TIME_ZONE_INFORMATION tzi;
	int type = GetTimeZoneInformation( &tzi );

	return CValue( (long)(type == TIME_ZONE_ID_DAYLIGHT) );
}


CValue  FctFormatTime( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'FormatTime'";
        error = 9;
        return CValue();
    }

    ULONG time;
    SYSTEMTIME now;
    if ( params.GetSize() == 2 )
    {
        time = (long)params[1];
        UnixTimeToSystemTime( time, &now );
    }
    else
    {
        GetLocalTime( &now );
        time = SystemTimeToUnixTime( now );
    }

    CStr hour24, hour12, ampm, minute, second, day, month, year, wday, unix, dayShort, dayLong, monthShort, monthLong;

    LCTYPE abbrevDays[] = { LOCALE_SABBREVDAYNAME7, LOCALE_SABBREVDAYNAME1, LOCALE_SABBREVDAYNAME2, LOCALE_SABBREVDAYNAME3,
							LOCALE_SABBREVDAYNAME4, LOCALE_SABBREVDAYNAME5, LOCALE_SABBREVDAYNAME6 };
    LCTYPE longDays[]   = { LOCALE_SDAYNAME7, LOCALE_SDAYNAME1, LOCALE_SDAYNAME2, LOCALE_SDAYNAME3,
							LOCALE_SDAYNAME4, LOCALE_SDAYNAME5, LOCALE_SDAYNAME6,  };

	TCHAR output[256];
	GetLocaleInfo( LOCALE_USER_DEFAULT, longDays[now.wDayOfWeek],   output, 256 );
	dayLong = output;
	GetLocaleInfo( LOCALE_USER_DEFAULT, abbrevDays[now.wDayOfWeek], output, 256 );
	dayShort  = output;
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME1 + now.wMonth - 1, output, 256 );
	monthLong = output;
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME1 + now.wMonth - 1, output, 256 );
	monthShort  = output;


    CStr result = params.GetAt(0);
    hour24.Format( L"%02d", now.wHour );
    int hour = now.wHour % 12;
    if ( hour == 0 ) hour = 12;
    hour12.Format( L"%02d", hour );
    minute.Format( L"%02d", now.wMinute );
    second.Format( L"%02d", now.wSecond );
    ampm = (now.wHour < 12) ? L"AM" : L"PM";
    day.Format( L"%02d", now.wDay );
    month.Format( L"%02d", now.wMonth );
    year.Format( L"%02d", now.wYear );
    wday.Format( L"%d", now.wDayOfWeek );
    unix.Format( L"%d", time );

    result.Replace( L"H", hour24 );
    result.Replace( L"h", hour12 );
    result.Replace( L"i", minute );
    result.Replace( L"s", second );
    result.Replace( L"d", day );
    result.Replace( L"m", month );
    result.Replace( L"Y", year );
    year = year.Mid(2);
    result.Replace( L"y", year );
    result.Replace( L"w", wday );
    result.Replace( L"u", unix );
    result.Replace( L"A", ampm );
    ampm.MakeLower();
    result.Replace( L"a", ampm );

    result.Replace( L"{MM}", monthLong );
    result.Replace( L"{M}",  monthShort );
    result.Replace( L"{WW}", dayLong );
    result.Replace( L"{W}",  dayShort );

    CValue rc;
    rc = result;

    return rc;
}

CValue  FctMortScriptType( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;
#ifdef POCKETPC
    rc = L"PPC";
#endif
#ifdef SMARTPHONE
#ifdef PNA
    rc = L"PNA";
#else
    rc = L"SP";
#endif
#endif
#ifdef DESKTOP
    rc = L"PC";
#endif

    return rc;
}

CValue FctScreenWidth( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;
	rc = (long)GetSystemMetrics( SM_CXSCREEN );
	return rc;
}

CValue FctScreenHeight( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;
	rc = (long)GetSystemMetrics( SM_CYSCREEN );
	return rc;
}

CValue FctCurrentCursor( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;

	HWND wnd;
    if ( params.GetSize() > 1 )
    {
        errorMessage = InvalidParameterCount + L"'CurrentCursor'";
        error = 9;
        return CValue();
    }
    else if ( params.GetSize() == 1 )
    {
		CStr wndName = params[0];
		wnd = FindWindowMy( wndName ,0);
	}
	else
	{
		wnd = ::GetForegroundWindow();
	}

	if ( wnd != NULL )
	{
		DWORD actCursor = GetClassLong(wnd,GCL_HCURSOR);

		HCURSOR compare;
#ifdef DESKTOP
		LPCTSTR	curList[] = { IDC_ARROW, IDC_WAIT, IDC_APPSTARTING, IDC_CROSS, IDC_HELP, IDC_UPARROW, NULL };
		LPCTSTR curName[] = { L"arrow",  L"wait",  L"appstart",		L"cross",  L"help",  L"uparrow" };
#else
		LPCTSTR curList[] = { IDC_ARROW, IDC_WAIT, IDC_APPSTARTING, IDC_CROSS, IDC_HELP, IDC_UPARROW, NULL };
		LPCTSTR curName[] = { L"arrow",  L"wait",  L"appstart",		L"cross",  L"help",  L"uparrow" };
#endif

		for ( int i=0; curList[i] != NULL; i++ )
		{
			compare = LoadStdCursor( curList[i] );
			if ( compare != NULL )
			{
				if ( actCursor == (DWORD)compare )
				{
					rc = curName[i];
				}
			}
		}
		if ( rc.IsNull() )
			rc = L"other";
	}
	
	return rc;
}

#if !defined( PNA ) && !defined( DESKTOP )
CValue  FctConnected( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue result;
    result = 0L;

    RASCONN rsconn[10];
    DWORD dwcb, dwConnections;
    RASCONNSTATUS rasStatus;

    // Enumerate active connections
    dwcb = sizeof(rsconn);

    rsconn[0].dwSize = sizeof(RASCONN);

    if ( RasEnumConnections(rsconn, &dwcb, &dwConnections) == 0 )
    {
        if ( dwConnections != 0 && rsconn[0].hrasconn != NULL )
        {
            // Get first RAS connection status
            rasStatus.dwSize = sizeof( rasStatus );

            if ( RasGetConnectStatus( rsconn[0].hrasconn, &rasStatus ) == 0 )
            {
                if ( rasStatus.rasconnstate == RASCS_Connected )
                {
                    result = 1L;
                }
            }
        }
    }

    return result;
}
#endif

#ifndef PNA
CValue  FctInternetConnected( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue result;
    result = 0L;

    HINTERNET web = NULL;
    if ( Proxy.IsEmpty() )
    {
#ifdef DESKTOP
        web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
#else
        web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
#endif
    }
    else
    {
        web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_PROXY, (LPCTSTR)Proxy, NULL, 0 );
    }

    if ( web != NULL )
    {
        if ( params.GetSize() > 0 )
        {
            HINTERNET file = InternetOpenUrl( web, params[0], NULL, 0, INTERNET_FLAG_RELOAD, 0 );
            if ( file != NULL )
            {
                result = 1L;
                InternetCloseHandle( file );
            }
        }
        else
        {
            result = 1L;
        }
        InternetCloseHandle( web );
    }

    return result;
}
#endif


CValue  FctFreeMemory( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue result;
    MEMORYSTATUS mems;
    mems.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&mems);

	int shift = 1;
	if ( params.GetSize() >= 1 )
	{
		shift = (long)params[0];
	}
	for( int i = 0; i < shift; i++ )
	{
		mems.dwAvailPhys = mems.dwAvailPhys >> 10;
	}
    if ( mems.dwAvailPhys > 0x0fffffff ) mems.dwAvailPhys = 0x0fffffff;

    result = (long)(mems.dwAvailPhys);
    return result;
}

CValue  FctTotalMemory( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue result;
    MEMORYSTATUS mems;
    mems.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&mems);

	int shift = 1;
	if ( params.GetSize() >= 1 )
	{
		shift = (long)params[0];
	}
	for( int i = 0; i < shift; i++ )
	{
		mems.dwTotalPhys = mems.dwTotalPhys >> 10;
	}
    if ( mems.dwTotalPhys > 0x0fffffff ) mems.dwTotalPhys = 0x0fffffff;
    
	result = (long)(mems.dwTotalPhys);
    return result;
}

CValue  FctSystemVersion( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() > 1 )
    {
        errorMessage = InvalidParameterCount + L"'FileModified'";
        error = 9;
        return CValue();
    }

    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof(ver);
    GetVersionEx( &ver );
    CValue retVal;

    if ( params.GetSize() > 0 )
    {
        if ( ((CStr)params[0]).CompareNoCase( L"major" ) == 0 )
            retVal = (long)ver.dwMajorVersion;
        if ( ((CStr)params[0]).CompareNoCase( L"minor" ) == 0 )
            retVal = (long)ver.dwMinorVersion;
        if ( ((CStr)params[0]).CompareNoCase( L"build" ) == 0 )
            retVal = (long)ver.dwBuildNumber;
        if ( ((CStr)params[0]).CompareNoCase( L"platform" ) == 0 )
        {
            switch ( ver.dwPlatformId )
            {
            case VER_PLATFORM_WIN32s:
                retVal = L"Win32s"; break;
            case VER_PLATFORM_WIN32_WINDOWS:
                retVal = L"Win95"; break;
            case VER_PLATFORM_WIN32_NT:
                retVal = L"WinNT"; break;
#ifndef DESKTOP
            case VER_PLATFORM_WIN32_CE:
                retVal = L"WinCE"; break;
#endif
            default:
                retVal = L"unknown";
            }
        }
    }

    if ( params.GetSize() == 0 || retVal.IsNull() )
    {
        CStr retStr;
        retStr.Format( L"%d.%d.%d", ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber );
        retVal = retStr;
    }

    return retVal;
}

CValue  FctSupportsProcHandling( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue rc;

	if ( LoadToolhelp() )
		rc = 1L;
	else
		rc = 0L;

	return rc;
}

CValue  FctRunWait( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 1 || params.GetSize() > 2 )
    {
        errorMessage = InvalidParameterCount + L"'RunWait'";
        error = 9;
        return CValue();
    }

	CValue rc;

    PROCESS_INFORMATION inf;
    CStr exe = params[0];
    CStr param;
    if ( params.GetSize() == 2 ) param = (CStr)params[1];
    GetRunData( exe, param );
    if ( ! exe.IsEmpty() )
	{
		STARTUPINFO info;
		memset(&info, 0, sizeof(info));
		info.cb = sizeof(info);
		info.wShowWindow = SW_SHOW;
#ifdef DESKTOP
		info.dwFlags = STARTF_USESHOWWINDOW;
		memset(&inf, 0, sizeof(inf));
		if ( ! param.IsEmpty() )
			param = L"\"" + exe + L"\" " + param;
#endif

		BOOL created = CreateProcess( (LPCTSTR)exe, (param.IsEmpty()) ? NULL : (LPTSTR)(LPCTSTR)param, NULL, NULL, FALSE, 0, NULL, NULL, &info, &inf );

        if ( created )
        {
            DWORD exitCode;
            while ( GetExitCodeProcess( inf.hProcess, &exitCode ) != FALSE && exitCode == STILL_ACTIVE )
            {
                ::Sleep( 100 );
            }
			rc = (long)exitCode;
            CloseHandle( inf.hProcess );
        }
		else
			rc = -1L;
    }
	else
		rc = -1L;

	return rc;
}

//jwz:add regular expression for string
CValue
FctRegEx( CValueArray &params, int &error, CStr &errorMessage ){

    if ( params.GetSize() < 2 || params.GetSize() > 4 )
    {
        errorMessage = InvalidParameterCount + L"'RegEx'";
        error = 9;
        return CValue();
    }
	CStr text = params[0];
	CStr pattern = params[1];
	
	long RegFlag = 0,lStart=0;
	if (params.GetSize()>=3) RegFlag = params[2];
	if (params.GetSize()>=4) lStart = params[3];

	static CRegexpT <WCHAR> regexp(pattern,RegFlag); //RegEx Compile

	MatchResult result = regexp.Match(text,lStart); //RegEx Match

	//Save result as Array!
	CValue res;
	CMapStrToValue *map = res.GetMap();

	CStr tStr,tVar;

	long MaxIndex = result.MaxGroupNumber()+1;
	CStrArray elements;

	if (result.IsMatched()){
		//MessageBox(NULL,index,L"GroupNumber",MB_SETFOREGROUND);

		tStr = text.Mid(result.GetStart() , result.GetEnd() - result.GetStart());
		tVar = L"$&";
		SetVariable( tVar, tStr );
		tVar = L"$`";
		SetVariable( tVar, text.Left(result.GetStart()));
		tVar = L"$'";
		SetVariable( tVar, text.Mid(result.GetEnd()));
		
		tVar = L"$0";
		tStr.Format(L"%d",result.MaxGroupNumber());

		SetVariable( tVar, tStr );

		tStr.Format( L"%d", MaxIndex );
		elements.Add(tStr);

		for (int i=0;i<=result.MaxGroupNumber();i++){

			tVar.Format( L"$%d",i+1 );
			tStr = text.Mid(result.GetGroupStart(i) , result.GetGroupEnd(i) - result.GetGroupStart(i));
			SetVariable( tVar, text.Mid(result.GetGroupStart(i+1) , result.GetGroupEnd(i+1) - result.GetGroupStart(i+1)));
			elements.Add(tStr);
			if ( tStr.GetLength() > 0 ){
				tVar = L"$+";
				tStr.Format(L"%d",i);
				SetVariable( tVar, tStr );
			}
		}
	}else{
		tVar = L"$0";
		SetVariable( tVar, L"0" );

		tStr = L"0";
		elements.Add(tStr);
	}

	//CStr index;
	for (int i=0; i<elements.GetSize(); i++ )
	{
		tStr.Format( L"%d", i );
		map->SetAt( tStr, elements[i] );
	}

	return res;
}

CValue
FctRegExReplace( CValueArray &params, int &error, CStr &errorMessage ){

    if ( params.GetSize() < 3 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'RegExReplace'";
        error = 9;
        return CValue();
    }
	CStr text = params[0];
	CStr pattern = params[1];
	CStr rplStr = params[2];
	long regStart= -1;
	long regTimes= -1;
	if (params.GetSize()>=4 ) regStart = (long) params[3];
	if (params.GetSize()>=5 ) regTimes = (long) params[4];
	CStr tStr;
	tStr.Format(L"%d-%d",regStart,regTimes);
	
	static CRegexpT <WCHAR> regexp(pattern,regStart); //RegEx Compile

	CStr result = regexp.Replace(text,rplStr,regStart,regTimes); //RegEx Match

	//Save result as Array!
	return (CValue)result;
}

#ifdef SMARTPHONE
//Get Bluetooth state,
//0-Poweroff
//1-Connectable
//2-Discoverable
CValue
FctGetBTState( CValueArray &params, int &error, CStr &errorMessage ){

    if ( params.GetSize() !=0 )
    {
        errorMessage = InvalidParameterCount + L"'Get Bluetooth State'";
        error = 9;
        return CValue();
    }
	DWORD BthMode;
	CValue result=-1L;
	if (BthGetMode(&BthMode)==ERROR_SUCCESS){
		result = (long)BthMode;
	}

	return result;
}

CValue
FctGetRadioMode( CValueArray &params, int &error, CStr &errorMessage ){

    if ( params.GetSize() !=0 )
    {
        errorMessage = InvalidParameterCount + L"'Get Bluetooth State'";
        error = 9;
        return CValue();
    }
	CValue rc = L"On";
	if (Flight(3)!=5) rc = L"Off";
	return rc;
}

#endif
//jwz:add end.