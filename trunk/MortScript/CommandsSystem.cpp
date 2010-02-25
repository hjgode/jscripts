#ifdef SMARTPHONE
	#include <windows.h>
	#include "smartphone/mortafx.h"
	extern HINSTANCE g_hInst;
	#ifndef PNA
		#include <vibrate.h>
	#endif
	#define LoadStdCursor(x) LoadCursor(NULL,x)
	#include "smartphone\DlgChoice.h"
	#include <pm.h>
	//jwz:add
	#include <bthutil.h>
	#include "FctFlight.h"
	//jwz:add end
#endif

#ifndef SMARTPHONE
	#include "stdafx.h"
	#include "mortscriptapp.h"
	extern CMortScriptApp theApp;
	#include "DlgChoice.h"
#endif

#include <string.h>
#include "interpreter.h"

#ifdef DESKTOP
	#include "vc6\stdafx.h"
	#include "vc6\resource.h"
	#include "direct.h"
	#define LoadStdCursor(x) theApp.LoadStandardCursor(x)
#else
	const GUID IID_ConnPrv_IProxyExtension = 
	 { 0xaf96b0bd, 0xa481, 0x482c, { 0xa0, 0x94, 0xa8, 0x44, 0x87, 0x67, 0xa0, 0xc0 } };
	#include "connmgr.h"
	#include <connmgr_proxy.h>
	#include "ras.h"
	#include "raserror.h"
	#include "notify.h"

	#include <winioctl.h>
	#ifndef POWER_STATE_ON
		#define POWER_STATE_ON		(DWORD)(0x00010000)  // on state
		#define POWER_STATE_IDLE	(DWORD)(0x00100000)  // idle state
		#define POWER_STATE_RESET	(DWORD)(0x00800000)  // soft reset
		#define POWER_FORCE			(DWORD)(0x00001000)
	#endif

	#define IOCTL_HAL_REBOOT CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
	extern "C" __declspec(dllimport) void SetCleanRebootFlag(void);
	extern "C" __declspec(dllimport) BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
	void WINAPI SystemIdleTimerReset(void);
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

#ifdef POCKETPC
	#include "vibrate.h"
	#define LoadStdCursor(x) theApp.LoadStandardCursor(x)
#endif


#include "variables.h"
#include "inifile.h"
#include "Interpreter.h"
#include "CommandsSystem.h"

// defined in Interpreter.cpp
extern CMortPtrArray				SubResultStack;
extern CMortPtrArray				LocalVariablesStack;

extern HANDLE ScriptAborted;

#ifndef DESKTOP
#ifndef PNA
BOOL CmdConnect( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    interpreter.Split( param, L',', params );
    if ( params.GetSize() > 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Connect'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( interpreter.Connection != NULL )
	{
		CloseHandle(interpreter.Connection);
		interpreter.Connection = NULL;
	}

    GUID *guid = NULL;

    // Selection dialog
    if ( params.GetSize() == 2 )
    {
        CONNMGR_DESTINATION_INFO destInfo;
        CDlgChoice choiceDlg;
        choiceDlg.m_Title = (CStr)params[0];
        choiceDlg.m_Info  = (CStr)params[1];
        for ( int i=0; ConnMgrEnumDestinations(i,&destInfo) == S_OK; i++ )
        {
            choiceDlg.m_Strings.Add( destInfo.szDescription );
        }
        choiceDlg.DoModal();
        if ( choiceDlg.m_Selected > 0 )
        {
            ConnMgrEnumDestinations( choiceDlg.m_Selected, &destInfo );
            guid = &destInfo.guid;
        }
    }

    // Given interpreter.Connection name
    if ( params.GetSize() == 1 )
    {
        CONNMGR_DESTINATION_INFO destInfo;
        for ( int i=0; ConnMgrEnumDestinations(i,&destInfo) == S_OK; i++ )
        {
            if ( ((CStr)params.GetAt(0)).CompareNoCase( destInfo.szDescription ) == 0 )
            {
                guid = &destInfo.guid;
                break;
            }
        }
    }

    // Try to find default
    if ( guid == NULL )
    {
        ConnMgrMapURL( L"http://www.microsoft.com", guid, NULL );
    }

    if ( guid != NULL )
    {
        CONNMGR_CONNECTIONINFO connInfo;
        connInfo.cbSize   = sizeof(connInfo);
        connInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
        connInfo.dwFlags  = CONNMGR_FLAG_PROXY_HTTP;
        connInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
        connInfo.bExclusive = FALSE;
        connInfo.bDisabled  = FALSE;
        connInfo.guidDestNet = *guid;
        connInfo.hWnd       = NULL;

        DWORD status;
        if ( ConnMgrEstablishConnectionSync( &connInfo, &interpreter.Connection, 30000, &status ) != S_OK )
        {
            if ( interpreter.ErrorLevel >= ERROR_ERROR )
                MessageBox( GetMsgParent(), L"Couldn't establish internet interpreter.Connection" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
        else
        {
            PROXY_CONFIG  proxyInfo;

            ZeroMemory(&proxyInfo, sizeof(proxyInfo));
            proxyInfo.dwType = CONNMGR_FLAG_PROXY_HTTP;
            if (SUCCEEDED(ConnMgrProviderMessage(   interpreter.Connection,
                                                    &IID_ConnPrv_IProxyExtension,
                                                    NULL,
                                                    0,
                                                    0,
                                                    (PBYTE)&proxyInfo,
                                                    sizeof(proxyInfo))))
            {
                if (proxyInfo.dwType == CONNMGR_FLAG_PROXY_HTTP)
                {
                   Proxy = proxyInfo.szProxyServer;
                   // SECURITY: Zero out the username/password from memory.
                   ZeroMemory(&(proxyInfo.szUsername), sizeof(proxyInfo.szUsername));
                   ZeroMemory(&(proxyInfo.szPassword), sizeof(proxyInfo.szPassword));
                }
            }
        }
    }
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
            MessageBox( GetMsgParent(), L"Couldn't establish internet interpreter.Connection" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        rc = FALSE;
    }

    return TRUE;
}

BOOL CmdDisconnect( CInterpreter &interpreter, CStr &param )
{
    CmdCloseConnection( interpreter, param );

	LPRASCONN   rasConn = NULL;
	DWORD		error = 0;
	DWORD		count = 1, connectionCount = 0;
	DWORD		size;

	do
	{
		if ( rasConn != NULL )
			HeapFree(GetProcessHeap(), 0, rasConn);
		size = sizeof(RASCONN) * count;
		rasConn = (LPRASCONN)HeapAlloc( GetProcessHeap(), 0, size );
		rasConn->dwSize = sizeof(RASCONN);
		error = RasEnumConnections( rasConn, &size, &connectionCount );
	}
	while ( error == ERROR_BUFFER_TOO_SMALL && rasConn != NULL );

	if ( rasConn != NULL && error == ERROR_SUCCESS )
	{
		for(DWORD i=0;i<connectionCount;i++)
		{
			RasHangUp( rasConn->hrasconn );
			rasConn++;
		}
	}

	if ( rasConn != NULL )
		HeapFree(GetProcessHeap(), 0, rasConn);

	return TRUE;
}

BOOL CmdCloseConnection( CInterpreter &interpreter, CStr &param )
{
    if ( interpreter.Connection != NULL )
	{
        ConnMgrReleaseConnection( interpreter.Connection, FALSE );
		CloseHandle(interpreter.Connection);
		interpreter.Connection = NULL;
	}
    return TRUE;
}

#endif // PNA

BOOL CmdRotate( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Rotate'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
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
		switch ( (long)params.GetAt(0) )
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

BOOL CmdRedrawToday( CInterpreter &interpreter, CStr &param )
{
	::PostMessage( HWND_BROADCAST, WM_SETTINGCHANGE, SPI_SETDESKWALLPAPER, 0 );
    ::SendMessage( ::GetDesktopWindow(), WM_WININICHANGE, 0xF2, 0 ); 
    return TRUE;
}


BOOL CmdIdleTimerReset( CInterpreter &interpreter, CStr &param )
{
	SystemIdleTimerReset();
	return TRUE;
}

BOOL CmdPowerOff( CInterpreter &interpreter, CStr &param )
{
    PowerOffSystem();
	::Sleep(1500); 
	return TRUE;
}

BOOL CmdReset( CInterpreter &interpreter, CStr &param )
{
	KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
	return TRUE;
}

#endif // ! Desktop

BOOL CmdSetVolume( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetVolume'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    long vol = (long)params[0];
    if ( vol >= 0 && vol <= 255 )
    {
        ULONG volume;
        BYTE  *volBytes = (BYTE*)&volume; // direct access to bytes
        volBytes[0] = 0;
        volBytes[1] = (BYTE)vol; // left volume
        volBytes[2] = 0;
        volBytes[3] = (BYTE)vol; // right volume

        waveOutSetVolume( 0, volume );

#ifdef POCKETPC
		RegWriteDW( HKEY_CURRENT_USER, L"ControlPanel\\Volume", L"Volume", volume );
#endif
    }

    return rc;
}

BOOL CmdVibrate( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Vibrate'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

#ifdef DESKTOP
    ::Beep( 440, (long)params[0] );
#endif

#if ( ! defined PNA && ! defined DESKTOP )
#ifdef SMARTPHONE
	::Vibrate(0,NULL,TRUE,INFINITE);
#else
	::Vibrate();
#endif
  ::Sleep( (long)params[0] );
	::VibrateStop();
#endif

    return TRUE;
}

BOOL CmdPlaySnd( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'PlaySound'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	PlaySound( params[0], NULL, SND_FILENAME );

	return TRUE;
}

#ifdef POCKETPC
// GDI Escapes for ExtEscape()
#define QUERYESCSUPPORT    8
 
// The following are unique to CE
#define GETVFRAMEPHYSICAL   6144
#define GETVFRAMELEN        6145
#define DBGDRIVERSTAT       6146
#define SETPOWERMANAGEMENT  6147
#define GETPOWERMANAGEMENT  6148
 
typedef enum _VIDEO_POWER_STATE {
    VideoPowerOn = 1,
    VideoPowerStandBy,
    VideoPowerSuspend,
    VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;
 

typedef struct _VIDEO_POWER_MANAGEMENT {
    ULONG Length;
    ULONG DPMSVersion;
    ULONG PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;


BOOL CmdSetBacklight( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetBacklight'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD bright   = (long)params[0];
    DWORD acBright = (long)params[1];

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

    HDC gdc;
    int iESC=SETPOWERMANAGEMENT;

    gdc = ::GetDC( NULL );

    VIDEO_POWER_MANAGEMENT vpm;
    vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
    vpm.PowerState = VideoPowerOn;
    ExtEscape(gdc, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL );

    ::ReleaseDC( NULL, gdc );

    return rc;
}


BOOL CmdToggleDisplay( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ToggleDisplay'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    HDC gdc;
    int iESC=SETPOWERMANAGEMENT;

    gdc = ::GetDC( NULL );

    VIDEO_POWER_MANAGEMENT vpm;
    vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);

    if ( (long)params[0] == 0 )
		vpm.PowerState = VideoPowerOff;
	else
	    vpm.PowerState = VideoPowerOn;
    ExtEscape(gdc, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL );

    ::ReleaseDC( NULL, gdc );

    if ( (long)params[0] )
	{
		typedef DWORD (*SetSystemPowerStateFunction)(LPCWSTR pwsSystemState, DWORD StateFlags, DWORD Options);
		HMODULE hModule = ::LoadLibrary(TEXT("Coredll.dll"));
   
		SetSystemPowerStateFunction f = (SetSystemPowerStateFunction)::GetProcAddress( hModule, TEXT("SetSystemPowerState") );
   
		if ( f != NULL )
		{
			f(NULL,POWER_STATE_ON,0);
		}

		::FreeLibrary(hModule);
	}

	return TRUE;
}

CMapStrToPtr InputMethods;

int SetInputEnumImProc( IMENUMINFO* pIMInfo )
{
   CLSID *tmpId = new CLSID();
   memcpy( tmpId, &pIMInfo->clsid, sizeof(CLSID) );
   InputMethods.SetAt( pIMInfo->szName, tmpId );
   return TRUE;
}

BOOL CmdSetInput( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetInput'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CLSID imSip;
	CStr clsidStr;
	if ( ((LPCTSTR)params[0])[0] != '{' )
	{
		InputMethods.RemoveAll();
		SipEnumIM( SetInputEnumImProc );
		void *clsid;
		int found = InputMethods.Lookup( (LPCTSTR)params[0], clsid );
		// CStr dbg;
		// dbg.Format( L"%d %d", found, clsid );
		// MessageBox( GetMsgParent(), dbg, L"Debug", MB_OK );
		if ( found ) imSip = *((CLSID*)clsid);
		
		CStr dummy;
		void *remClsId;
		int pos = InputMethods.GetStartPosition();
		while ( pos != NULL )
		{
			InputMethods.GetNextAssoc( pos, dummy, remClsId );
			delete (CLSID*)remClsId;
		}
		InputMethods.RemoveAll();
		
		if ( ! found )
		{
			
			if ( interpreter.ErrorLevel >= ERROR_WARN )
				MessageBox( GetMsgParent(), L"Unknown input type for 'SetInput'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return TRUE;
		}
	}
	else
	{
		clsidStr = (CStr)params[0];
		int res = CLSIDFromString( (LPTSTR)(LPCTSTR)clsidStr, &imSip );
		if ( res != NOERROR )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Invalid CLSID for 'SetInput'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return TRUE;
		}
	}

	::SipSetCurrentIM( &imSip );

	return rc;
}


BOOL CmdShowInput( CInterpreter &interpreter, CStr &param )
{
	SipShowIM( SIPF_ON );
	return TRUE;
}

BOOL CmdHideInput( CInterpreter &interpreter, CStr &param )
{
	SipShowIM( SIPF_OFF );
	return TRUE;
}


BOOL CmdHardReset( CInterpreter &interpreter, CStr &param )
{
    CValue *hr;
    if ( Variables.Lookup( L"HARDRESET", hr ) == TRUE && (long)*hr == 1 )
    {
        SetCleanRebootFlag();
        KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);
        return FALSE;
    }
	return TRUE;
}

#endif // PPC

BOOL CmdGetClipText( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetClipText'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStr val;
    OpenClipboard(NULL); 
    HANDLE clip=GetClipboardData(CF_UNICODETEXT);
    if ( clip != NULL )
    {
        val.Format( L"%s", clip );
    }
    CloseClipboard();
    
    return interpreter.SetVariable( params[0], val );
}

BOOL CmdSetClipText( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SetClipText'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    int size = (((CStr)params.GetAt(0)).GetLength()+1) * sizeof(TCHAR);
    HLOCAL memUc = LocalAlloc( LMEM_ZEROINIT, size );
    HLOCAL memAsc = LocalAlloc( LMEM_ZEROINIT, size );
    memcpy( (void*)memUc, (LPCTSTR)params.GetAt(0), size );
    WideCharToMultiByte( CP_ACP, 0, (LPCTSTR)params.GetAt(0), -1, (char*)memAsc, size, NULL, NULL );

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData( CF_UNICODETEXT, memUc );
    SetClipboardData( CF_TEXT, memAsc );
    CloseClipboard();

    return rc;
}

BOOL CmdSleep( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Sleep'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	if ( GetMsgParent() != NULL )
		WaitForSingleObject( ScriptAborted, (long)params[0] );
	else
		::Sleep( (long)params[0] );

    return TRUE;
}

BOOL CmdGetTime( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;

    interpreter.Split( param, L',', params, 1, 0x3f ); // 0011 1111

    if ( params.GetSize() < 1 || ( params.GetSize() > 3 && params.GetSize() != 6 ) )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetTime'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    SYSTEMTIME now;
    GetLocalTime( &now );
    //CTime   now = CTime::GetCurrentTime();
    
    if ( params.GetSize() == 1 )
    {
        CValue val;
        val = (long)SystemTimeToUnixTime( now );
	    rc = interpreter.SetVariable( params[0], val );
    }

    if ( params.GetSize() == 2 )
    {
        CStr hour24, hour12, ampm, minute, second, day, month, year, wday;
        
        CStr time = params.GetAt(1);
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

	    rc = interpreter.SetVariable( params[0], time );
    }

    if ( params.GetSize() == 3 || params.GetSize() == 6 )
    {
        CStr val;
        val.Format( L"%02d", (long)now.wHour );
	    rc = interpreter.SetVariable( params[0], val );

        val.Format( L"%02d", (long)now.wMinute );
	    rc &= interpreter.SetVariable( params[1], val );

        val.Format( L"%02d", (long)now.wSecond );
	    rc &= interpreter.SetVariable( params[2], val );
    }

    if ( params.GetSize() == 6 )
    {
        CStr val;
        val.Format( L"%02d", (long)now.wDay );
	    rc &= interpreter.SetVariable( params[3], val );

        val.Format( L"%02d", (long)now.wMonth );
	    rc &= interpreter.SetVariable( params[4], val );

        val.Format( L"%02d", (long)now.wYear );
	    rc &= interpreter.SetVariable( params[5], val );
    }

    return rc;
}

BOOL CmdSetTime( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;

    interpreter.Split( param, L',', params );

    if (   params.GetSize() != 1
		&& params.GetSize() != 3
		&& params.GetSize() != 6 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetTime'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    SYSTEMTIME now;
    GetLocalTime( &now );
    
    if ( params.GetSize() == 1 )
    {
        UnixTimeToSystemTime( (long)params[0], &now );
    }

    if ( params.GetSize() == 3 || params.GetSize() == 6 )
    {
		now.wHour   =(unsigned short)(long)params[0];
		now.wMinute = (unsigned short)(long)params[1];
		now.wSecond = (unsigned short)(long)params[2];
    }

    if ( params.GetSize() == 6 )
    {
		now.wDay   = (unsigned short)(long)params[3];
		now.wMonth = (unsigned short)(long)params[4];
		now.wYear  = (unsigned short)(long)params[5];
    }

	SetLocalTime( &now );

    return rc;
}

BOOL CmdGetActiveProcess( CInterpreter &interpreter, CStr &param )
{
	BOOL rc = TRUE;
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetActiveWindow'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	HWND hwnd = ::GetForegroundWindow();
	TCHAR procName[256];
	DWORD dwProcessID;

	::GetWindowThreadProcessId(hwnd,&dwProcessID);
	if (!dwProcessID) 
    {
	    rc = interpreter.SetVariable( params[3], CValue() );
    }
    else
    {
    	::GetModuleFileName((HMODULE)dwProcessID,procName,256);
        LPTSTR bs = wcsrchr( procName, '\\' );
        if ( bs == NULL )
		    rc = interpreter.SetVariable( params[0], procName );
        else
			rc = interpreter.SetVariable( params[0], bs+1 );
    }

    return rc;
}

BOOL CmdGetSystemPath( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 2 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetSystemPath'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStr path;
    LPTSTR  pathBuffer = path.GetBuffer( MAX_PATH );
    if ( ((CStr)params[0]).CompareNoCase( L"ProgramsMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_PROGRAMS, FALSE );
    }
#ifndef SMARTPHONE
    else if ( ((CStr)params[0]).CompareNoCase( L"StartMenu" ) == 0 )
    {
        SHGetSpecialFolderPath( NULL, pathBuffer, CSIDL_STARTMENU, FALSE );
    }
#endif
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
            if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( GetMsgParent(), L"GetSystemPath: Document path requires WM2003" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
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
            if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( GetMsgParent(), L"GetSystemPath: Program files path requires WM2003" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
        }
#else
        if ( SHGetSpecialFolderPath( NULL, pathBuffer, 0x0026, FALSE ) == FALSE )
        {
            if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            {
                MessageBox( GetMsgParent(), L"GetSystemPath: Retrieving program files path not supported by your system" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            }
        }
#endif
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"JScripts" ) == 0 || ((CStr)params[0]).CompareNoCase( L"ScriptPath" ) == 0)
    {
        int len = interpreter.ScriptFile.ReverseFind('\\');
        wcsncpy( pathBuffer, (LPCTSTR)interpreter.ScriptFile, len );
        pathBuffer[len] = '\0';
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptName" ) == 0 )
    {
        int len = interpreter.ScriptFile.ReverseFind('\\');
        int dot = interpreter.ScriptFile.ReverseFind('.');
        wcsncpy( pathBuffer, (LPCTSTR)interpreter.ScriptFile+len+1, dot-len-1 );
        pathBuffer[dot-len-1] = '\0';
    }
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptExt" ) == 0 )
    {
        int dot = interpreter.ScriptFile.ReverseFind('.');
        wcscpy( pathBuffer, (LPCTSTR)interpreter.ScriptFile.Mid(dot) );
    }
#ifndef SMARTPHONE
    else if ( ((CStr)params[0]).CompareNoCase( L"ScriptExe" ) == 0 )
    {
        int len = wcsrchr( theApp.m_pszHelpFilePath, '\\' ) - theApp.m_pszHelpFilePath;
        wcsncpy( pathBuffer, theApp.m_pszHelpFilePath, len );
        pathBuffer[len] = '\0';
    }
#endif
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
        {
            MessageBox( GetMsgParent(), L"GetSystemPath: Invalid path type" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        }
        return FALSE;
    }
    path.ReleaseBuffer();

    return interpreter.SetVariable( params[1], path );
}

BOOL CmdGetMortScriptType( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetMortScriptType'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

#ifdef POCKETPC
	return interpreter.SetVariable( params[0], L"PPC" );
#endif
#ifdef SMARTPHONE
#ifdef PNA
	return interpreter.SetVariable( params[0], L"PNA" );
#else
	return interpreter.SetVariable( params[0], L"SP" );
#endif
#endif
#ifdef DESKTOP
	return interpreter.SetVariable( params[0], L"PC" );
#endif
}

BOOL CmdShowWaitCursor( CInterpreter &interpreter, CStr &param )
{
	SetCursor(LoadStdCursor(IDC_WAIT));
	return TRUE;
}

BOOL CmdHideWaitCursor( CInterpreter &interpreter, CStr &param )
{
    SetCursor(LoadStdCursor(IDC_ARROW)); 
	return TRUE;
}


BOOL CmdKill( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Kill'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

//#ifndef PNA
	if ( LoadToolhelp() )
	{
		CStr search = params[0];
		BOOL fullPath = FALSE;
		if ( search.Find('\\') != -1 )
			fullPath = TRUE;

		HANDLE         procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && Process32First( procSnap, &procEntry ) )
		{
			CStr procName;
			do
			{
				if ( fullPath )
				{
			        procName = GetProcessExePath( procEntry.th32ProcessID, procEntry.szExeFile );
				}
				else
				{
					procName = procEntry.szExeFile;
				}

				if ( procName.CompareNoCase( search ) == 0 )
				{
					HANDLE hProc = ::OpenProcess( PROCESS_TERMINATE, FALSE, procEntry.th32ProcessID ); 
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
	}
	else
	{
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'Kill' requires toolhelp.dll on your device" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
	}
//#else
//	// Run external tool for PNAs
//	CStr runParams;
//	runParams = L"\"" + AppPath + L"\\killproc.exe\", \"" + (CStr)params[0] + L"\"";
//	RunApp( runParams, TRUE );
//#endif

    return rc;
}

BOOL CmdKillScript( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

    CValueArray params;
    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'KillScript'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr mutex;
	DWORD procId = GetRunningScriptProcId( params[0], &mutex );

	if ( procId != NULL )
	{
		RegWriteDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", mutex, 1 );
		DWORD exitCode = 0;
		//for ( int i=0; i<=6; i++ )
		//{
			HANDLE hProc = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, procId );
			if ( hProc != NULL )
			{
				for ( int i=0; i<6; i++ )
				{
					if ( i != 0 ) ::Sleep( 500 );
					if ( GetExitCodeProcess( hProc, &exitCode ) )
					{
						if ( exitCode != STILL_ACTIVE )
						{
							break;
						}
					}
				}

				if ( exitCode == STILL_ACTIVE )
				{
					TerminateProcess( hProc, 0 );
	                HKEY    key;
	                if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS )
	                {
                        RegDeleteValue( key, mutex );
		                RegCloseKey( key );
	                }
	                if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS )
	                {
                        RegDeleteValue( key, mutex );
		                RegCloseKey( key );
	                }
				}

				CloseHandle( hProc );
			}
		//}
	}

    return rc;
}

BOOL RunApp( CInterpreter &interpreter, CStr &param, BOOL wait )
{
    CValueArray params;
    BOOL rc = TRUE;

    int parCnt = interpreter.Split( param, L',', params );
    if ( parCnt < 1  || parCnt > 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Run'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ! wait && ((CStr)params[0]).Right(4).CompareNoCase(L".lnk") == 0 )
    {
        SHELLEXECUTEINFO sei;
        sei.cbSize = sizeof(SHELLEXECUTEINFO);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd  = NULL;
        sei.lpVerb = L"Open";
        sei.lpFile = (LPCTSTR)params[0];
        sei.lpDirectory = L"\\";
        sei.lpParameters = NULL;
        if ( parCnt == 2 )sei.lpParameters = (LPCTSTR)params[1];
		sei.nShow = SW_SHOW;
        sei.hInstApp = NULL;

        rc = ShellExecuteEx( &sei ) != 0;

        if ( rc ) CloseHandle( sei.hProcess );
    }
    else
    {
        PROCESS_INFORMATION inf;
        CStr exe = params[0];
        CStr param;
        if ( parCnt == 2 ) param = (CStr)params[1];
		//Debug(exe,param);
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

			rc = CreateProcess( (LPCTSTR)exe, (param.IsEmpty()) ? NULL : (LPTSTR)(LPCTSTR)param, NULL, NULL, FALSE, 0, NULL, NULL, &info, &inf );

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
			/*
			else
			{
				int e = ::GetLastError();
				CStr msg;
				msg.Format( L"%d", e );
				MessageBox( GetMsgParent(), msg, L"Debug", MB_OK );
			}
			*/
        }
    }

    return TRUE;
}

BOOL CmdRun( CInterpreter &interpreter, CStr &param )
{
	return RunApp( interpreter, param, FALSE );
}

BOOL CmdRunWait( CInterpreter &interpreter, CStr &param )
{
	return RunApp( interpreter, param, TRUE );
}

#ifdef POCKETPC
#ifndef PNA
BOOL CmdNew( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    BOOL rc = TRUE;

    if ( interpreter.Split( param, L',', params ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'New'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
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
   	    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Shell\\Extensions\\NewMenu", 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS )
	    {
            TCHAR subKeyName[50];
            DWORD len = 50;
            for ( int i=0; RegEnumKeyEx( key, i, subKeyName, &len, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS; i++ )
            {
   	            if ( RegOpenKeyEx( key, subKeyName, 0, REG_ACCESS_READ, &newItemKey ) == ERROR_SUCCESS )
	            {
		            DWORD type, length;
		            TCHAR cont[MAX_PATH];
		            length = MAX_PATH;
		            if ( RegQueryValueEx( newItemKey, NULL, NULL, &type, (BYTE*)cont, &length ) == ERROR_SUCCESS )
		            {
                        if ( (CStr)params[0] == cont )
                        {
                            // Found! Now covert the key name to clsid and create the item
                            found = TRUE;
                            CLSID clsid;
                            CLSIDFromString( subKeyName, &clsid );
                            if ( SHCreateNewItem( NULL, clsid ) != NOERROR )
                            {
                                if ( interpreter.ErrorLevel >= ERROR_ERROR )
                                {
                                    MessageBox( GetMsgParent(), L"New document could not be created" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
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
                if ( interpreter.ErrorLevel >= ERROR_ERROR )
                {
                    MessageBox( GetMsgParent(), L"New document could not be created" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
                    rc = FALSE;
                }
            }
	    }
    }
    else
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            MessageBox( GetMsgParent(), L"'New' requires at least WM2003" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            rc = FALSE;
        }
    }

    return rc;
}
#endif
#endif

#ifndef DESKTOP
BOOL CmdRunOnPowerOn( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params ) ;
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RunOnPowerOn'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CE_NOTIFICATION_TRIGGER nt;

	// Fill in CE_NOTIFICATION_TRIGGER structure
	memset( &nt, 0, sizeof(CE_NOTIFICATION_TRIGGER) );
	nt.dwSize  = sizeof(CE_NOTIFICATION_TRIGGER);
	nt.dwType  = CNT_EVENT;
	nt.dwEvent = NOTIFICATION_EVENT_WAKEUP;
	nt.lpszApplication = (TCHAR*)(LPCTSTR)params.GetAt(0);
    if ( params.GetSize() > 1 )
    	nt.lpszArguments   = (TCHAR*)(LPCTSTR)params.GetAt(1);
    else
        nt.lpszArguments   = NULL;

	// Call the function to register the notification
	if ( CeSetUserNotificationEx (0, &nt, NULL) == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            MessageBox( GetMsgParent(), L"Couldn't set notification" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CmdRunAt( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params ) ;
    if ( params.GetSize() != 2 && params.GetSize() != 3 && params.GetSize() != 6 && params.GetSize() != 7 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RunAt'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CE_NOTIFICATION_TRIGGER nt;
    SYSTEMTIME alarmTime;
    int appidx;

    if ( params.GetSize() <= 3 )
    {
        appidx = 1;
        UnixTimeToSystemTime( (long)params.GetAt(0), &alarmTime );
    }
    else
    {
        appidx = 5;
        alarmTime.wYear   = (USHORT)(long)params.GetAt(0);
        alarmTime.wMonth  = (USHORT)(long)params.GetAt(1);
        alarmTime.wDay    = (USHORT)(long)params.GetAt(2);
        alarmTime.wHour   = (USHORT)(long)params.GetAt(3);
        alarmTime.wMinute = (USHORT)(long)params.GetAt(4);
        alarmTime.wSecond = 0;
		alarmTime.wMilliseconds = 0;
		alarmTime.wDayOfWeek = 0;
    }

	// Fill in CE_NOTIFICATION_TRIGGER structure
	memset( &nt, 0, sizeof(CE_NOTIFICATION_TRIGGER) );
	nt.dwSize  = sizeof(CE_NOTIFICATION_TRIGGER);
    nt.dwType  = CNT_TIME;
    nt.stStartTime     = alarmTime;
    nt.stEndTime       = alarmTime;
	nt.lpszApplication = (TCHAR*)(LPCTSTR)params.GetAt(appidx);
    if ( params.GetSize() > appidx+1 )
    	nt.lpszArguments   = (TCHAR*)(LPCTSTR)params.GetAt(appidx+1);
    else
        nt.lpszArguments   = NULL;

	// Call the function to register the notification
	if ( CeSetUserNotificationEx (0, &nt, NULL) == NULL )
    {
        if ( interpreter.ErrorLevel >= ERROR_ERROR )
        {
            MessageBox( GetMsgParent(), L"Couldn't set notification" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CmdRemoveNotifications( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params ) ;
    if ( params.GetSize() != 1 && params.GetSize() != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RemoveNotifications'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    ULONG handleCount;
    CeGetUserNotificationHandles (NULL, 0, &handleCount);
    if ( handleCount > 0 )
    {
        HANDLE *handles = new HANDLE[handleCount];
        BOOL  rc = CeGetUserNotificationHandles (handles, handleCount, &handleCount);
        if ( rc == TRUE )
        {

            for ( UINT i=0; i < handleCount; i++ )
            {
                CE_NOTIFICATION_INFO_HEADER *info;
                DWORD                        size, dwMaxNotifSize;

                CeGetUserNotification(handles[i], 0, &dwMaxNotifSize , NULL);
                if ( dwMaxNotifSize > 0 )
                {
                    BYTE *pNotifBuffer = new BYTE[dwMaxNotifSize];
                    if ( CeGetUserNotification(handles[i], dwMaxNotifSize, &size, pNotifBuffer) )
                    {
                        info = (CE_NOTIFICATION_INFO_HEADER*)pNotifBuffer;
                        CStr app = info->pcent->lpszApplication;
                        CStr par = info->pcent->lpszArguments;
                        if (   app.CompareNoCase( params.GetAt(0) ) == 0
                            && ( params.GetSize() == 1 || par.CompareNoCase( params.GetAt(1) ) == 0 )
                           )
                        {
                            CeClearUserNotification(handles[i]);
                        }
                    }
                    delete[] pNotifBuffer;
                }
            }
        }
        delete[] handles;
    }

    return TRUE;
}

#ifdef SMARTPHONE
BOOL CmdSetBTState( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params ) ;
    if ( params.GetSize() != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Set Bluetooth State'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	if ( (long)params[0]==2){
		BthSetMode(BTH_DISCOVERABLE);
	}else if ( (long)params[0]==1){
		BthSetMode(BTH_CONNECTABLE);
	}else if ( (long)params[0]==0 ){
		BthSetMode(BTH_POWER_OFF);
	}

    return true;
}

BOOL CmdSetRadioMode( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    interpreter.Split( param, L',', params ) ;
    if ( params.GetSize() > 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Set Bluetooth State'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	if ((long)params[0]==5 || ((CStr)params[0]).CompareNoCase(L"On")==0){
		Flight(0);
	}else if ((long)params[0]==1 || ((CStr)params[0]).CompareNoCase(L"Off")==0){
		Flight(1);
	}else{
		Flight(2);
	}

    return true;
}
#endif

#endif

