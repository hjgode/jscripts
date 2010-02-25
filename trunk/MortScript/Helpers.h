#ifndef HELPERS_H
#define HELPERS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "morttypes.h"
#include "interpreter.h"
#include <time.h>

#define VERSION L"4.1.2.0"
#define VERSION_INFO L"4.12b0"

extern CStr ExecuteFileName;

// In desktop Windows, dynamic loading doesn't work because toolhelp.dll is an old 16 bit ("NE format") dll...
#ifdef DESKTOP
	#include "Tlhelp32.h"
#else
	// Toolhelp.dll
	#define TH32CS_SNAPHEAPLIST 0x00000001
	#define TH32CS_SNAPPROCESS  0x00000002
	#define TH32CS_SNAPTHREAD   0x00000004
	#define TH32CS_SNAPMODULE   0x00000008
	#define TH32CS_SNAPALL	    (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
	#define TH32CS_GETALLMODS	0x80000000

	extern "C"
	{
		typedef struct tagPROCESSENTRY32 {
			DWORD   dwSize;
			DWORD   cntUsage;
			DWORD   th32ProcessID;
			DWORD   th32DefaultHeapID;
			DWORD   th32ModuleID;
			DWORD   cntThreads;
			DWORD   th32ParentProcessID;
			LONG    pcPriClassBase;
			DWORD   dwFlags;
			TCHAR   szExeFile[MAX_PATH];
			DWORD	th32MemoryBase;
			DWORD	th32AccessKey;
		} PROCESSENTRY32, *PPROCESSENTRY32, *LPPROCESSENTRY32;

		typedef struct tagMODULEENTRY32 {
			DWORD   dwSize;
			DWORD   th32ModuleID;
			DWORD   th32ProcessID;
			DWORD   GlblcntUsage;
			DWORD   ProccntUsage;
			BYTE   *modBaseAddr;
			DWORD   modBaseSize;
			HMODULE hModule;
			TCHAR   szModule[MAX_PATH];
			TCHAR   szExePath[MAX_PATH];
			DWORD	dwFlags;
		} MODULEENTRY32, *PMODULEENTRY32, *LPMODULEENTRY32;
	}

	extern HINSTANCE ToolhelpDll;
	extern HANDLE (_cdecl *CreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);
	extern BOOL (_cdecl *CloseToolhelp32Snapshot)(HANDLE hSnapshot);
	extern BOOL (_cdecl *Process32First)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
	extern BOOL (_cdecl *Process32Next)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
	extern BOOL (_cdecl *Module32First)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	extern BOOL (_cdecl *Module32Next)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
#endif

BOOL LoadToolhelp();
void UnloadToolhelp();
CStr GetProcessExePath( DWORD processId, LPTSTR exeName=NULL );

#ifdef DESKTOP
#define REG_ACCESS_READ  KEY_READ
#define REG_ACCESS_WRITE KEY_ALL_ACCESS
#else
#define REG_ACCESS_READ  0
#define REG_ACCESS_WRITE 0
#endif

typedef struct _function
{
	TCHAR *name;
    void  *ptr;
} function;

typedef CValue(*functionPointer)(CValueArray&,int&,CStr&);
typedef BYTE(*commandPointer)(CInterpreter&,CStr&);

class SearchWindow
{
public:
	HWND    FoundWnd;
	TCHAR   SearchText[256];
	int     Type;
};


void   *SearchFunctionPointer( function array[], LPCTSTR name );
functionPointer GetFunction( LPCTSTR name );
commandPointer  GetCommand( LPCTSTR name );

void	GetExeForDoc( const CStr &doc, CStr &exe );
void	GetExe( const CStr &appAndParam, CStr &exe, CStr &param );
void	GetRunData( CStr &exe, CStr &param );

BOOL	FileOrDirExists( CStr &file, BOOL dir );
HWND	FindWindowMy( CStr &window , int Type);
DWORD	GetRunningScriptProcId( CStr &scriptName, CStr *mutexName = NULL );

CValue	GetWindowFromProcess(DWORD dwProcId);

CStr RegRead( HKEY topkey, LPCTSTR path, LPCTSTR value );
DWORD	RegReadDW( HKEY topkey, LPCTSTR path, LPCTSTR value );
BOOL	RegWrite( HKEY topkey, LPCTSTR path, LPCTSTR valName, LPCTSTR string );

int		GetCodePage( CStr &encoding );

// Zip library
extern BOOL  (_cdecl *UnzipToFile)(LPCTSTR,LPCTSTR,LPCTSTR);
extern BOOL  (_cdecl *UnzipAllToPath)(LPCTSTR,LPCTSTR);
extern BOOL  (_cdecl *UnzipPathToPath)(LPCTSTR,LPCTSTR,LPCTSTR);
extern void* (_cdecl *UnzipToMemory)(LPCTSTR,LPCTSTR,UINT*);
extern BOOL  (_cdecl *ZipFromFile)(LPCTSTR,LPCTSTR,LPCTSTR,int);
extern BOOL  (_cdecl *ZipFromMemory)(LPCTSTR,LPCTSTR,void *,DWORD,int);
extern BOOL  (_cdecl *ZipFromFilesToZipPath)(LPCTSTR,LPCTSTR,LPCTSTR,BOOL,int);
BOOL OpenZipDll();
void CloseZipDll();

extern CStr InvalidParameterCount;

extern CStr Proxy;
extern CStr AppPath;

HWND GetMsgParent();

void UnixTimeToFileTime(time_t t, LPFILETIME pft);
void UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst);
time_t FileTimeToUnixTime( const FILETIME &ft );
time_t SystemTimeToUnixTime( const SYSTEMTIME &st );

void ScreenToClient( HWND wnd, RECT *rect );

void SplitLines( HDC dc, LPCTSTR text, CStrArray &lines, int width );

#endif
