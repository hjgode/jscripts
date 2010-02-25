// MortStarter.h : main header file for the MORTSTARTER application
//

#if !defined(AFX_MORTSTARTER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
#define AFX_MORTSTARTER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef POCKETPC
#include "resource.h"		// main symbols
#endif

#ifdef DESKTOP
#include "vc6\resource.h"
#endif

#define CT_IF      1
#define CT_WHILE   2
#define CT_SWITCH  3
#define CT_CHOICE  4
#define CT_FOR     5
#define CT_FOREACH 6

/////////////////////////////////////////////////////////////////////////////
// CMortStarterApp:
// See MortStarter.cpp for the implementation of this class
//
class SearchWindow
{
public:
	HWND    FoundWnd;
	LPCTSTR SearchText;
};

class CMortStarterApp
#ifndef SMARTPHONE
 : public CWinApp
#endif
{
public:
	CMortStarterApp();
	void	RunFile( LPCTSTR filename );
    int     Split( const CString &source, TCHAR sep, CStringArray &dest, BOOL trim = TRUE );

protected:
    BYTE       ErrorLevel;

#ifndef SMARTPHONE
	CMapStringToPtr     Subs;
#endif

    BOOL       CondState;
    CUIntArray CondStack;
    
    CUIntArray RepeatCount;
    CUIntArray RepeatPos;
    
    //CUIntArray WhileStack;
    CUIntArray WhilePos;
    
    CUIntArray ChoiceStack;
    
    CUIntArray SubStack;

    CString ScriptFile;
    CString ErrLine;

#ifndef SMARTPHONE
    //CUIntArray   ForEachStack;
    CStringArray ForEachVariables;
    CPtrArray    ForEachData;
    CUIntArray   ForEachType;
    CUIntArray   ForEachPos;
#endif

#ifndef SMARTPHONE
	void	RegisterFileClass();
	void	RegisterFileType( LPCTSTR type, LPCTSTR typeClass );
	CString GetRelativeToAppPath( LPCTSTR file );
#endif
    HKEY    GetTopKey( CString &topkey );
	CString RegRead( HKEY topkey, LPCTSTR path, LPCTSTR value );
    DWORD   RegReadDW( HKEY topkey, LPCTSTR path, LPCTSTR value );
	
    void	GetExeForDoc( const CString &doc, CString &exe );
	void	GetExe( const CString &appAndParam, CString &exe, CString &param );
	void	GetRunData( CString &exe, CString &param );
	BOOL	RunApp( CString &param, BOOL wait = FALSE );
#ifdef POCKETPC
#ifndef PNA
    BOOL	New( CString &param );
#endif
#endif
    BOOL	Kill( CString &param );

    HWND	FindWindow( CString &window );
	BOOL	Show( CString &window );
	BOOL	Minimize( CString &window );
	BOOL	Close( CString &window );
	
	BOOL	WaitFor( CString &param );
	BOOL	WaitForActive( CString &param );
    BOOL	Sleep( CString &param );
    BOOL	SleepMessage( CString &param );

#ifndef SMARTPHONE
    BOOL    GetTime( CString &param );
#endif
    
    BOOL	SendKey( CString &window, USHORT ch, BOOL shift=FALSE, BOOL ctrl=FALSE, BOOL alt=FALSE );
	BOOL	SendChar( CString &window, USHORT ch );
    BOOL    SendSpecialKey( CString &cmd, CString &param );
    BOOL    SendCtrlKey( CString &param );
    BOOL    SendKeys( CString &param );
	
#ifndef SMARTPHONE
    BOOL    GetColorAt( CString &param );
    BOOL    GetRGB( CString &param );
    BOOL    GetWindowText( CString &param );
#endif

    BOOL    MkDir( CString &param );
    BOOL    RmDir( CString &param );
	BOOL	Delete( CString &param, BOOL recursive = FALSE );
    BOOL    SetProxy( CString &param );
    BOOL    Download( CString &param );
	BOOL	Copy( CString &param );
    BOOL	XCopy( CString &param );
	BOOL	Rename( CString &param );
    BOOL	Move( CString &param );
    BOOL	CreateShortcut( CString &param );
#ifndef SMARTPHONE
    BOOL    GetVersion( CString &param );
#endif
	
    DWORD MouseEvent( BYTE button, BOOL release );
    BOOL	MouseClick( CString &param, BOOL dbl=FALSE, BYTE button=0 );
	BOOL	MouseDown( CString &param, BYTE button=0 );
	BOOL	MouseUp( CString &param, BYTE button=0 );
	
    BOOL	RegWriteString( CString &param );
	BOOL	RegWriteDWord( CString &param );
#ifndef SMARTPHONE
    BOOL	RegReadString( CString &param );
	BOOL	RegReadDWord( CString &param );
#endif
    BOOL    RegDelete( CString &param );
    BOOL    RegDeleteKey( CString &param );
    void    RegDeleteSubKey( HKEY key, LPCTSTR subkeyName, BOOL values, BOOL recursive );


#ifndef SMARTPHONE
    BOOL	IniRead( CString &param );
    BOOL    ReadFile( CString &param );
#endif
    BOOL    WriteFile( CString &param );

#ifndef DESKTOP
    BOOL	Rotate( CString &param );
    BOOL	SetVolume( CString &param );
	BOOL	Vibrate( CString &param );
#endif
    BOOL	PlaySnd( CString &param );
#ifdef POCKETPC
    BOOL	SetBacklight( CString &param );
#endif

    BOOL    Message( CString &param );
#ifndef SMARTPHONE
    BOOL    Input( CString &param );
#endif

    BOOL    CheckCondition( CString &param, BOOL &condRes );
    BOOL    CheckWndExists( CString &element, BOOL &condRes );
    BOOL    CheckWndActive( CString &element, BOOL &condRes );
    BOOL    CheckProcExists( CString &element, BOOL &condRes );
    BOOL    CheckFileExists( CString &element, BOOL dir, BOOL &condRes );
    BOOL    CheckQuestion( CString &element, BOOL &condRes );
    BOOL    CheckScreen( CString &element, BOOL &condRes );
    BOOL    CheckRegKeyExists( CString &element, BOOL &condRes );
    BOOL    CheckRegKeyEqualsString( CString &element, BOOL &condRes );
    BOOL    CheckRegKeyEqualsDWord( CString &element, BOOL &condRes );
#ifndef SMARTPHONE
    BOOL    CheckEquals( CString &element, BOOL &condRes );
    BOOL    CheckExpression( CString &element, BOOL &condRes );

    BOOL    Set( CString &element );
    BOOL    GetActiveWindow( CString &element );
    BOOL    GetSystemPath( CString &element );
    BOOL	GetFilePath( CString &param );
    BOOL	GetFileBase( CString &param );
    BOOL	GetFileExt( CString &param );
    BOOL    Explode( CString &element );
	BOOL	GetPart( CString &param );
	BOOL	SubStr( CString &param );
	BOOL	Find( CString &param );
	BOOL	ReverseFind( CString &param );
	BOOL	MakeUpper( CString &param );
	BOOL	MakeLower( CString &param );
    BOOL	GetMortScriptType( CString &param );
#endif
    
    void    PushCondStack( short type, short state );
    void    PopCondStack( short &type, short &state, BOOL remove=TRUE );

    BOOL    If( CString &param, long pos );
    BOOL    Else();
    BOOL    EndIf();
    
    BOOL    Switch( CString &param, long pos );
#ifndef SMARTPHONE
    BOOL    Choice( CString &param, long pos );
    BOOL    ChoiceDefault( CString &param, long pos );
#endif
    BOOL    Case( CString &param );
    BOOL    EndChoice();
    
    BOOL    While( CString &param, long pos );
    BOOL    EndWhile( CString &param, long &pos );

    BOOL    Repeat( CString &param, long pos );
    BOOL    EndRepeat( CString &param, long &pos );

#ifndef SMARTPHONE
    BOOL    ForEach( CString &param, long pos );
    BOOL    EndForEach( CString &param, long &pos );
#endif

#ifndef SMARTPHONE
    BOOL    CallScript( CString &param );
    BOOL    Call( CString &param, long curr, long &pos );
    BOOL    EndSub( long &pos );
#endif

    BOOL    SetErrorLevel( CString &param );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMortStarterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

#ifndef SMARTPHONE
	//{{AFX_MSG(CMortStarterApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#endif
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MORTSTARTER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
