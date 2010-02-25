// killproc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tlhelp32.h"

#define TH32CS_SNAPNOHEAPS 0x40000000

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	if ( lpCmdLine != NULL && wcslen( lpCmdLine ) > 0 )
	{
		HANDLE         procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && Process32First( procSnap, &procEntry ) )
		{
			do
			{
				if ( wcsicmp( lpCmdLine, procEntry.szExeFile ) == 0 )
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
			CloseToolhelp32Snapshot( procSnap );
	}

	return 0;
}

