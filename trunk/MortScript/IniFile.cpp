#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "Afxcoll.h"
#include "wininet.h"
#endif

#ifdef POCKETPC
#include "stdafx.h"
#include "Afxcoll.h"
#include <todaycmn.h>
#endif

#ifdef SMARTPHONE
#include "windows.h"
#include "winbase.h"
#ifndef PNA
#include "wininet.h"
#endif
#include "smartphone\mortafx.h"
#endif

#include "morttypes.h"
#include "interpreter.h"
#include "IniFile.h"

time_t SystemTimeToUnixTime( const SYSTEMTIME &st );

extern CMapStrToValue Variables;
extern CInterpreter			*CurrentInterpreter;

HANDLE *comPort;
char   *comTargetBuffer;
UINT    comTargetSize;
UINT    comReadSize;
SYSTEMTIME endTime;

DWORD _stdcall PortReadThread(LPVOID lpvoid)
{
	BYTE readByte;
	DWORD dwCommModemStatus, dwBytesTransferred;
	
	comReadSize = 0;
	
	// Specify a set of events to be monitored for the port.
	SetCommMask( *comPort, EV_RXCHAR | EV_CTS | EV_DSR | EV_RLSD | EV_RING );
	
	while ( INVALID_HANDLE_VALUE != *comPort && comReadSize < comTargetSize-1 ) 
	{
		// Wait for an event to occur for the port.
		WaitCommEvent( *comPort, &dwCommModemStatus, 0 );
		
		// Re-specify the set of events to be monitored for the port.
		SetCommMask( *comPort, EV_RXCHAR | EV_CTS | EV_DSR | EV_RING );
		
		if ( dwCommModemStatus & EV_RXCHAR ) 
		{
			// Loop for waiting for the data.
			do 
			{
				// Read the data from the serial port.
				ReadFile( *comPort, &readByte, 1, &dwBytesTransferred, 0 );
				
				// Add to string
				if (dwBytesTransferred == 1)
				{
					comTargetBuffer[comReadSize] = readByte;
					comReadSize++;
				}
				
			} 
			while ( dwBytesTransferred == 1 && comReadSize < comTargetSize-1 );
		}
		
		// Retrieve modem control-register values.
		// GetCommModemStatus( *comPort, &dwCommModemStatus );
	}
	
	return 0;
}


CIniFile::CIniFile()
{
	Sections.RemoveAll();
}

CIniFile::~CIniFile()
{
    int pos;
    for( pos = Sections.GetStartPosition(); pos != NULL; )
    {
        CStr section;
        CMapStrToString *values;
        Sections.GetNextAssoc( pos, section, (void*&)values );
        delete values;
    }
}

extern CStr Proxy;

// -------------------------------------------------------------------
//                          öffentliche Funktionen
// -------------------------------------------------------------------

int
CIniFile::Split( const CStr &source, LPCTSTR sep, CStrArray &dest, BOOL trim /* = TRUE */ )
{
	int pos = source.Find( sep );
	int startPos = 0;
	CStr elem;
	int sepLen = wcslen( sep );

	dest.RemoveAll();
	while( pos != -1 )
	{
		elem = source.Mid( startPos, pos-startPos );
        if ( trim ) { elem.TrimLeft(); elem.TrimRight(); }
		dest.Add( elem );

		startPos = pos+sepLen;
		pos      = source.Find( sep, startPos );
	}
	elem = source.Mid( startPos );
    if ( trim ) { elem.TrimLeft(); elem.TrimRight(); }
	dest.Add( elem );

	return dest.GetSize();
}

int
CIniFile::ReadFile( LPCTSTR filename, CStr &content, int size, int cp )
{
    HANDLE file   = NULL;
    char  *buffer = NULL;
    char  *tmpStr = NULL;
    ULONG  fileSize, readSize;
    ULONG  bufSize;
    BOOL   rc     = 0, com;
	int    timeout;

	if ( wcsnicmp( filename, L"COM", 3 ) == 0 && filename[wcslen(filename)-1] == ':' )
	{
		file = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
		if ( file != INVALID_HANDLE_VALUE )
		{
			timeout = SetComPort( filename, file );
		}
		com = TRUE;
	}
	else
	{
		CStr fileWithPath;
		fileWithPath = filename;

		if ( CurrentInterpreter != NULL && ( fileWithPath.GetLength() < 2 || ( fileWithPath.GetAt(0) != '\\' && fileWithPath.GetAt(1) != ':' ) ) )
		{
			int len = CurrentInterpreter->ScriptFile.ReverseFind('\\');
			if ( len == -1 )
				fileWithPath = L"\\" + fileWithPath;
			else
				fileWithPath = CurrentInterpreter->ScriptFile.Left( len+1 ) + fileWithPath;
		}
	    file = CreateFile( fileWithPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		com = FALSE;
	}

    if ( file == INVALID_HANDLE_VALUE )
    {
        int error = GetLastError();
        rc = -2;
    }

    if ( rc == 0 )
    {
		if ( size <= 0 )
		{
			fileSize = GetFileSize( file, NULL );
		}
		else
		{
			fileSize = size;
		}

	    if ( fileSize > 1024 * 1024 || fileSize == -1 )
	    {
		    // Too big
		    rc = -3;
	    }
    }

    if ( rc == 0 )
    {
        bufSize  = (fileSize+1)*sizeof(TCHAR);
        buffer   = (char*)content.GetBufferSetLength( bufSize );
        if ( buffer == NULL ) rc = -4;
    }

    if ( rc == 0 )
    {
        tmpStr = new char[fileSize+2]; // if it's unicode, two binary zeroes are required at the end
		if ( tmpStr == NULL ) rc = -4;
    }
     
    if ( rc == 0 )
    {
		if ( ! com )
		{
			int stat = ::ReadFile( file, (void*)tmpStr, fileSize, &readSize, NULL );
			if ( stat == 0 )
			{
				int error = GetLastError();
				rc = -5;
			}
		}
		else
		{
			comPort = &file;
			comTargetSize = fileSize+1;
			comTargetBuffer = tmpStr;

			DWORD dwThreadID;
			HANDLE readThread = CreateThread( NULL, 0, PortReadThread, 0, 0, &dwThreadID );
			if ( readThread != NULL )
			{
				WaitForSingleObject( readThread, timeout );
				TerminateThread( readThread, 0 );
				CloseHandle( readThread );
			    SetCommMask( file, 0 );
				readSize = comReadSize;
			}
			else
			{
				rc = -6;
				readSize = 0;
			}
		}
    }

    if ( rc == 0 )
    {
		if ( cp == CP_ACP )
		{
			if ( readSize >= 3 && memcmp( tmpStr, (void*)"\xEF\xBB\xBF", 3 ) == 0 )
			{
				cp = CP_UTF8_PREFIX;
			}
			if ( readSize >= 2 && memcmp( tmpStr, (void*)"\xFF\xFE", 2 ) == 0 )
			{
				cp = CP_UNICODE_PREFIX;
			}
		}

		if ( cp != CP_UNICODE && cp != CP_UNICODE_PREFIX && cp != -1)
		{
			int offset = 0;

			// Strip first 3 bytes for prefixed UFT8
			if ( cp == CP_UTF8_PREFIX )
			{
				if ( memcmp( tmpStr, (void*)"\xEF\xBB\xBF", 3 ) == 0 )
				{
					offset = 3;
				}
				cp = CP_UTF8;
			}

			// set binary zero at end
			tmpStr[readSize] = '\0';
			MultiByteToWideChar( cp, 0, tmpStr+offset, readSize-offset+1, (LPTSTR)buffer, bufSize );
		}
		else if (cp != -1)
		{
			int offset = 0;
			// Strip first 2 bytes for prefixed UFT8
			if ( cp == CP_UNICODE_PREFIX )
			{
				if ( memcmp( tmpStr, (void*)"\xFF\xFE", 2 ) == 0 )
				{
					offset = 2;
				}
				cp = CP_UNICODE;
			}

			// for unicode, two binary zeroes are required at the end
			tmpStr[readSize] = '\0';
			tmpStr[readSize+1] = '\0';
			wcscpy( (TCHAR*)buffer, (TCHAR*)(tmpStr+offset) );
		}
    }

    if ( file   != NULL ) CloseHandle( file );
    if ( buffer != NULL ) content.ReleaseBuffer();
    if ( tmpStr != NULL ) delete[] tmpStr;

    return rc;
}


#ifndef PNA

// > 0: http error
// -1: Keine Internet-Verbindung
// -2: URL konnte nicht geöffnet werden
// -3: Zu gro?
// -4: Kein Speicher
// -5: Fehler beim Lesen
int
CIniFile::ReadWebFile( LPCTSTR url, CStr &content, int size, int cp )
{
    HINTERNET web = NULL, file = NULL;
    char  *buffer = NULL;
    char  *tmpStr = NULL;
    ULONG  blockSize, fileSize, readSize;
    ULONG  bufSize;
    BOOL   rc     = 0;

    if ( Proxy.IsEmpty() )
    {
#ifdef DESKTOP
		web  = InternetOpen( L"MortScript", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
#else
        web  = InternetOpen( L"MortScript", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
#endif
    }
    else
    {
		web  = InternetOpen( L"MortScript", INTERNET_OPEN_TYPE_PROXY, (LPCTSTR)Proxy, NULL, 0 );
    }

    if ( web == NULL )
	{
        int error = GetLastError();
		rc = -1;
	}

    if ( rc == 0 )
    {
	    file = InternetOpenUrl( web, url, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	    if ( file == NULL )
	    {
            int error = GetLastError();
		    rc = -2;
	    }
    }

    if ( rc == 0 )
    {
	    if ( _tcsncmp( url, L"http", 4 ) == 0 )
	    {
		    // No http error?
		    TCHAR status[MAX_PATH];
            DWORD length=MAX_PATH;
		    if ( HttpQueryInfo( file, HTTP_QUERY_STATUS_CODE, status, &length, NULL ) )
		    {
			    if ( _ttol( status ) != 200 )
			    {
				    rc = _ttol( status );
			    }
		    }
	    }
    }

    if ( rc == 0 )
    {
		fileSize = 0;

        BOOL more = InternetQueryDataAvailable( file, &blockSize, 0, 0 );
        tmpStr = (char*)malloc(blockSize+1);

		while ( rc == 0 && more && blockSize > 0 && tmpStr != NULL )
		{
			int stat = InternetReadFile( file, (void*)(tmpStr+fileSize), blockSize, &readSize );
			if ( stat == 0 )
			{
				int error = GetLastError();
				rc = -5;
			}
			else
			{
				tmpStr[fileSize+readSize] = '\0';
				fileSize += readSize;
				more = InternetQueryDataAvailable( file, &blockSize, 0, 0 );
				if ( more && blockSize > 0 )
				{
					if ( fileSize + blockSize > 256 * 1024 )
					{
						// Too big
						rc = -3;
					}
					else
					{
						void *movedStr = realloc( tmpStr, fileSize+blockSize+1 );
						if ( movedStr == NULL )
						{
							more = FALSE;
							rc = -4;
						}
						else
							tmpStr = (char*)movedStr;
					}
				}
			}
		}
    }

    if ( rc == 0 )
    {
        bufSize  = (fileSize+1)*sizeof(TCHAR);
        buffer   = (char*)content.GetBufferSetLength( fileSize+1 );
        if ( buffer == NULL )
	    {
		    rc = -4;
        }
    }

    if ( rc == 0 )
    {
    	tmpStr[fileSize] = '\0';

        if ( cp == CP_ACP ) // Default: Device's encoding
		{
			// Copy first few lines
			char scanText[256];
			strncpy( scanText, tmpStr, 255 );
			scanText[255] = '\0';
			_strlwr( scanText );

			// Unicode?
			if (   strstr( scanText, "utf8" )
				|| strstr( scanText, "utf-8" )
			   )
			{
				cp = CP_UTF8;
			}
			if ( strstr( scanText, "iso-8835-1" ) )
			{
				cp = 1252;
			}
		}

		if ( cp != CP_UNICODE )
			MultiByteToWideChar( cp, 0, tmpStr, fileSize+1, (LPTSTR)buffer, bufSize );
		else
			strcpy( buffer, tmpStr );
    }

    if ( file   != NULL ) InternetCloseHandle( file );
    if ( web    != NULL ) InternetCloseHandle( web );
    if ( buffer != NULL ) content.ReleaseBuffer();
    if ( tmpStr != NULL ) free(tmpStr);

    return rc;
}

#endif

int
CIniFile::SetComPort( LPCTSTR filename, HANDLE file )
{
	CStr port = filename;
	CValue *comData;
	int timeout, rate, bits, parity, stopBit, flow;
	port.MakeUpper();
	if ( Variables.Lookup( L"[Port_" + port + L"]", comData ) )
	{
		CStrArray data;
		CStr dataString;
		dataString = (CStr)*comData;

		Split( dataString, L"\n", data );
		timeout = _wtol( data[0] );
		rate = _wtol( data[1] );
		parity = _wtol( data[2] );
		bits = _wtol( data[3] );
		stopBit = _wtol( data[4] );
		flow = _wtol( data[5] );
		//MessageBox( NULL, comData, L"Debug", MB_OK );
	}
	else
	{
		timeout = 10000;
		rate = 4800;
		parity = NOPARITY;
		bits = 8;
		stopBit = ONESTOPBIT;
		flow = 0;
	}

	DCB PortDCB;
	ZeroMemory (&PortDCB, sizeof(PortDCB));
	PortDCB.DCBlength = sizeof(DCB);
	GetCommState( file, &PortDCB );

	// Change the DCB structure settings.
	PortDCB.BaudRate = rate;              // Current baud 
	PortDCB.fBinary = TRUE;               // ASCII mode
	PortDCB.fParity = TRUE;               // Enable parity checking 
	PortDCB.fOutxCtsFlow = (flow==1);     // CTS output flow control 
	PortDCB.fOutxDsrFlow = FALSE;         // DSR output flow control 
	PortDCB.fDtrControl = DTR_CONTROL_ENABLE; 
	// DTR flow control type 
	PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
	PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
	PortDCB.fOutX = (flow==2);            // XON/XOFF out flow control 
	PortDCB.fInX = (flow==2);             // XON/XOFF in flow control 
	PortDCB.fErrorChar = FALSE;           // Disable error replacement 
	PortDCB.fNull = TRUE;                 // Enable null stripping 
	PortDCB.fRtsControl = (flow==1) ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_ENABLE; 
	// RTS flow control 
	PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on 
	// error
	PortDCB.ByteSize = bits;              // Number of bits/byte, 4-8 
	PortDCB.Parity = parity;              // 0-4=no,odd,even,mark,space 
	PortDCB.StopBits = stopBit;           // 0,1,2 = 1, 1.5, 2 

	//CStr msg;
	//msg.Format( L"BaudRate: %d\nByteSize: %d\nStopBits: %d\nParity: %d\nfOutX: %d\nfInX: %d\nfOutxCtsFlow: %d\nfRtsControl: %d\nfDsrSensitivity: %d", 
	//			     PortDCB.BaudRate,
	//				 PortDCB.ByteSize,
	//				 PortDCB.StopBits,
	//				 PortDCB.Parity,
	//				 PortDCB.fOutX,
	//				 PortDCB.fInX,
 	//				 PortDCB.fOutxCtsFlow,
	//				 PortDCB.fRtsControl,
	//				 PortDCB.fDsrSensitivity );
	//MessageBox( NULL, L"Port-Info: " + msg, L"Port-Info", MB_OK|MB_SETFOREGROUND );

	// Configure the port according to the specifications of the DCB 
	// structure.
	SetCommState( file, &PortDCB );

	// Retrieve the time-out parameters for all read and write operations
	// on the port. 
	COMMTIMEOUTS CommTimeouts;
	GetCommTimeouts( file, &CommTimeouts);

	// Change the COMMTIMEOUTS structure settings.
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;
	CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;  
	CommTimeouts.ReadTotalTimeoutConstant = timeout;
	CommTimeouts.WriteTotalTimeoutMultiplier = 10;
	CommTimeouts.WriteTotalTimeoutConstant = 1000;
	
	// Set the time-out parameters for all read and write operations
	// on the port. 
	SetCommTimeouts( file, &CommTimeouts);

	EscapeCommFunction( file, SETDTR );
	EscapeCommFunction( file, SETRTS );

	return timeout;
}

BOOL
CIniFile::WriteFile( LPCTSTR filename, CStr &content, BOOL append, UINT cp )
{
    HANDLE  file;
    char   *buffer = NULL;
    char   *tmpStr = NULL;
    ULONG   iLen, feLen;
    BOOL    rc = TRUE, comPort = FALSE;

	if ( filename[wcslen(filename)-1] == ':' )
	{
		file = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
		if ( file != INVALID_HANDLE_VALUE )
			SetComPort( filename, file );
		comPort = TRUE;
	}
	else
	{
		CStr fileWithPath;
		fileWithPath = filename;

		if ( CurrentInterpreter != NULL && ( fileWithPath.GetLength() < 2 || ( fileWithPath.GetAt(0) != '\\' && fileWithPath.GetAt(1) != ':' ) ) )
		{
			int len = CurrentInterpreter->ScriptFile.ReverseFind('\\');
			if ( len == -1 )
				fileWithPath = L"\\" + fileWithPath;
			else
				fileWithPath = CurrentInterpreter->ScriptFile.Left( len+1 ) + fileWithPath;
		}

		if ( append )
		{
			file = CreateFile( fileWithPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,   FILE_ATTRIBUTE_NORMAL, NULL );
			if ( SetFilePointer( file, 0, NULL, FILE_END ) == 0 )
				append = FALSE;
		}
		else
			file = CreateFile( fileWithPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	}

    if ( file == INVALID_HANDLE_VALUE ) rc = FALSE;

    if ( rc == TRUE )
    {
        iLen   = content.GetLength();
        if ( cp != CP_UNICODE )
        {
	        tmpStr = new char[iLen*2+1]; // special characters might use 2 bytes!
		    if ( tmpStr == NULL ) rc = FALSE;
		}
		else
			tmpStr = NULL;
    }

    if ( rc == TRUE )
    {
        ULONG written;
        int   stat;


        if ( cp != CP_UNICODE && cp != CP_UNICODE_PREFIX )
        {
			if ( cp == CP_UTF8_PREFIX  )
			{
				if ( ! comPort && ! append )
					::WriteFile( file, (void*)"\xEF\xBB\xBF", 3, &written, NULL ); // omit the \0!
				cp = CP_UTF8;
			}
            feLen = WideCharToMultiByte( cp, 0, (LPCTSTR)content, -1, tmpStr, iLen*2+1, NULL, NULL );
			if ( !comPort )
	            stat  = ::WriteFile( file, (void*)tmpStr, feLen-1, &written, NULL ); // omit the \0!
			else
			{
				stat = 1, written = 1;
				for ( ULONG i=0; i<feLen-1 && stat != 0 && written == 1; i++ )
				{
		            stat  = ::WriteFile( file, (void*)&(tmpStr[i]), 1, &written, NULL ); // omit the \0!
				}
			}
        }
        else
        {
			if ( cp == CP_UNICODE_PREFIX  )
			{
				if ( ! comPort && ! append )
					::WriteFile( file, (void*)"\xFF\xFE", 2, &written, NULL ); // omit the \0!
			}
            stat = ::WriteFile( file, (void*)(LPCTSTR)content, (content.GetLength())*2, &written, NULL ); // omit the \0!
        }

        if ( stat == 0 ) rc = FALSE;
    }

    if ( file   != NULL ) CloseHandle( file );
    if ( buffer != NULL ) content.ReleaseBuffer();
    if ( tmpStr != NULL ) delete[] tmpStr;

    return rc;
}

void
CIniFile::Parse( LPCTSTR cont )
{
	CStr content = cont;
	CStr sectionName, key, value;
    CMapStrToString *section = NULL;
	BOOL hasEmptySection = FALSE;

	Sections.RemoveAll();

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
		line.TrimLeft(); line.TrimRight();

		if ( line.GetLength() > 0 && line.GetAt(0) != '#' && line.GetAt(0) != ';' )
		{
			if ( line.GetAt(0) == '[' && line.Right(1) == L"]" )
			{
				sectionName = line.Mid( 1, line.GetLength()-2 );
                sectionName.MakeLower();
                section = new CMapStrToString();
                Sections.SetAt( sectionName, section );
			}
			else
			{
				int eqPos = line.Find( '=' );
				if ( eqPos != -1 )
				{
					key   = line.Left( eqPos );
					key.TrimLeft(); key.TrimRight();
					value = line.Mid( eqPos+1 );
					value.TrimLeft(); value.TrimRight();
					if ( value.Left(1) == L"\"" && value.Right(1) == L"\"" )
					{
						value = value.Mid( 1, value.GetLength()-2 );
					}
					key.MakeLower();
					if ( section == NULL && hasEmptySection == FALSE )
					{
						section = new CMapStrToString();
						Sections.SetAt( L"", section );
						hasEmptySection = TRUE;
					}
					section->SetAt( key, value );
				}
			}
		}
	}
}

BOOL
CIniFile::Read( LPCTSTR filename )
{
    CStr content;

    int err;
#ifndef PNA
    if (   _tcsncmp( filename, L"http:", 5 ) == 0
        || _tcsncmp( filename, L"https:", 6 ) == 0
        || _tcsncmp( filename, L"ftp:",  4 ) == 0
       )
    {
        err = CIniFile::ReadWebFile( filename, content );
    }
    else
    {
#endif
        err = CIniFile::ReadFile( filename, content );
#ifndef PNA
    }
#endif

    if ( err == 0 )
    {
		Parse( content );
    	return TRUE;
	}
    else
    {
    	return FALSE;
    }
}


BOOL
CIniFile::ExistsSection( LPCTSTR section )
{
    BOOL rc = FALSE;

    CStr cmp = section;
    cmp.MakeLower();
    void *dummy;
    rc = Sections.Lookup( section, dummy );

    return rc;
}


/*
COLORREF
CIniFile::GetColor( LPCTSTR section, LPCTSTR element, COLORREF def, LPCTSTR defElement )
{
	CRect    rect;
	COLORREF col = def;

	CStr  elemKey = CStr(section) + "\\" + element;
	elemKey.MakeUpper();

	CStr  string = GetString( section, element, L"", defElement );
	if ( ! string.IsEmpty() )
	{
		col = StringToColor( string );
	}

	return col;
}

COLORREF
CIniFile::StringToColor( const CStr &string )
{
	COLORREF col = 0;

	if ( string.CompareNoCase( L"transparent" ) == 0 )
	{
		col = CLR_NONE;
	}
	else if ( string.CompareNoCase( L"today" ) == 0 )
	{
        HWND todayWnd = FindWindow( NULL, L"Desktop" );
        col = ::SendMessage(todayWnd, TODAYM_GETCOLOR, (WPARAM) TODAYCOLOR_TEXT, NULL) ;
	}
    else if ( string.Find(',') == -1 )
	{
		LPTSTR end;
		long color = wcstol( string, &end, 16 );
		// COLORREF: 0x00bbggrr
		// color:    0x00rrggbb
		col = RGB( GetBValue(color), GetGValue(color), GetRValue(color) );
	}
	else
	{
		int comma1 = string.Find(',');
		int comma2 = string.Find(',', comma1+1);
		if ( comma2 != -1 )
		{
			int red   = _wtol( string.Left( comma1 ) );
			int green = _wtol( string.Mid( comma1+1, comma2-comma1 ) );
			int blue  = _wtol( string.Mid( comma2+1 ) );
			col = RGB( red, green, blue );
		}
	}

	return col;
}

BOOL
CIniFile::GetBoolean( LPCTSTR section, LPCTSTR element, BOOL     def, LPCTSTR defElement )
{
	BOOL     ret = def;

	CStr  elemKey = CStr(section) + "\\" + element;
	elemKey.MakeUpper();

    CStr  string = GetString( section, element, L"", defElement );
	if ( ! string.IsEmpty() )
	{
		if (   string == L"1"
			|| string.CompareNoCase( L"TRUE" ) == 0
			|| string.CompareNoCase( L"ON" )   == 0
		   )
		{
			ret = TRUE;
		}
		else
		{
			ret = FALSE;
		}
	}

	return ret;
}

RECT
CIniFile::GetPosition( LPCTSTR section, LPCTSTR element, RECT *def, LPCTSTR defElement )
{
	CRect    rect;

	CStr  elemKey = CStr(section) + "\\" + element;
	elemKey.MakeUpper();

	rect.SetRectEmpty();
	if ( def != NULL ) rect.CopyRect( def );

	CStr  string = GetString( section, element, L"", defElement );
	if ( ! string.IsEmpty() )
	{
		rect.CopyRect( &StringToPos( string ) );
	}

	return (RECT)rect;
}

RECT
CIniFile::StringToPos( const CStr &string )
{
	CRect rect;

	rect.SetRectEmpty();

	int comma[3], prevComma = 0;
	for ( int i = 0; i < 3 && prevComma != -1; i++ )
	{
		prevComma = string.Find( ',', prevComma+1 );
		comma[i] = prevComma;
	}

	if ( i == 3 )
	{
		rect.left   = _wtol( string.Left( comma[0] ) );
		rect.top    = _wtol( string.Mid( comma[0]+1, comma[1]-comma[0] ) );
		rect.right  = rect.left + _wtol( string.Mid( comma[1]+1, comma[2]-comma[1] ) );
		rect.bottom = rect.top  + _wtol( string.Mid( comma[2]+1 ) );
	}

	return (RECT)rect;
}

void 
CIniFile::GetPoint( LPCTSTR section, LPCTSTR element, CPoint &point )
{
	CStr  elemKey = CStr(section) + "\\" + element;
	elemKey.MakeUpper();

    point.x = point.y = 0;

    CStr  string = GetString( section, element, L"" );
	if ( ! string.IsEmpty() )
	{
        CStrArray values;
        if ( Split( string, ',', values ) == 2 )
        {
            point.x = _ttoi( values[0] );
            point.y = _ttoi( values[1] );
        }
	}
}

long
CIniFile::GetInt( LPCTSTR section, LPCTSTR element, long def, LPCTSTR defElement )
{
	long     val = def;

	CStr  elemKey = CStr(section) + "\\" + element;
	elemKey.MakeUpper();

	CStr  string = GetString( section, element, L"", defElement );
	if ( ! string.IsEmpty() )
	{
		val = _wtol( string );
	}

	return val;
}

CFont*
CIniFile::GetFont( LPCTSTR section, LPCTSTR element, HDC dc, LPCTSTR defFont, BOOL defBold, BOOL defItalics, int defSize, short factor )
{
	CFont   *font    = new CFont();
	CStr  string  = GetString( section, element );
	CStr  fontName = defFont;
	int      size     = defSize;
	BOOL     bold = defBold, italic = defItalics;

	if ( ! string.IsEmpty() )
	{
		int comma[4], prevComma = 0;
		for ( int i = 0; i < 4 && prevComma != -1; i++ )
		{
			prevComma = string.Find( ',', prevComma+1 );
			comma[i] = prevComma;
		}

		if ( defFont != NULL && i == 1 )
		{
			size = _wtol( string );
		}

		if ( defFont != NULL && i == 3 )
		{
			size   = _wtol( string.Left( comma[0] ) );
			bold   = _wtol( string.Mid( comma[0]+1, comma[1]-comma[0] ) );
			italic = _wtol( string.Mid( comma[1]+1 ) );
		}

		if ( i == 4 )
		{
			fontName = string.Left( comma[0] );
			fontName.TrimLeft(); fontName.TrimRight();
			size   = _wtol( string.Mid( comma[0]+1, comma[1]-comma[0] ) );
			bold   = _wtol( string.Mid( comma[1]+1, comma[2]-comma[1] ) );
			italic = _wtol( string.Mid( comma[2]+1 ) );
		}
	}

    font->CreateFont( size * GetDeviceCaps( dc, LOGPIXELSY) / factor, // nHeight
					  0,                         // nWidth
					  0,                         // nEscapement
					  0,                         // nOrientation
					  bold ? FW_BOLD : FW_NORMAL, // nWeight
					  italic,                     // bItalic
					  FALSE,                     // bUnderline
					  0,                         // cStrikeOut
					  OEM_CHARSET,               // nCharSet
					  OUT_DEFAULT_PRECIS,        // nOutPrecision
					  CLIP_DEFAULT_PRECIS,       // nClipPrecision
					  DEFAULT_QUALITY,           // nQuality
					  DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
					  fontName );

	return font;
}

  */


CStr
CIniFile::GetString( LPCTSTR section, LPCTSTR element, LPCTSTR  def, LPCTSTR defElement )
{
	CStr  sectionCmp = section, elemCmp = element, defElemCmp = defElement;
	CStr  string;
	BOOL     found = FALSE;
    CMapStrToString *elements;

    sectionCmp.MakeLower();
    elemCmp.MakeLower();
    defElemCmp.MakeLower();

    BOOL secFound = Sections.Lookup( sectionCmp, (void*&)elements );
    if ( secFound && elements->Lookup( elemCmp, string ) )
	{
		found = TRUE;
	}

	if ( found == FALSE && secFound && !defElemCmp.IsEmpty() )
	{
		if ( elements->Lookup( defElemCmp, string ) )
		{
			found = TRUE;
		}
	}

	if ( found == FALSE )
	{
        secFound = Sections.Lookup( L"DEFAULT", (void*&)elements );
		if ( secFound && elements->Lookup( elemCmp, string ) )
		{
			found = TRUE;
		}
	}

	if ( found == FALSE && secFound && !defElemCmp.IsEmpty() )
	{
		if ( elements->Lookup( defElemCmp, string ) )
		{
			found = TRUE;
		}
	}

	if ( found == FALSE )
	{
		string = def;
	}

	return string;
}
