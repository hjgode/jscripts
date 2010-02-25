// MortStarter.cpp : Defines the class behaviors for the application.
// ****************************************
// jwz:modi 
// modify RunFile() to File reader and syntax check and it's name RunFile()
// add Parse() function for main parser.
// add Eval() for parse current line.
// add EvalFunc() for parse a function.
// add some addition function for function call.
// add ExprParse's function for ++ --
// add C language like escape character \n \r \" .. and so on.
// add Some Regular Expression 's variable like $& $` $' $0 $1 ... $999 and so on.
// add a variable for last statement result, $_ so 1+2*5+4/6 will work fine.
// and lots of modify and source code.
// This comment just for Mort.
// ****************************************

//#include "..\log\log.h"

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "direct.h"
#define KEYEVENTF_SILENT 0
#endif

#ifdef POCKETPC
#include "stdafx.h"
#include "resource.h"
#endif

#ifndef SMARTPHONE
#include "MortScriptApp.h"
#include "DlgChoice.h"
#endif

#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone\resource.h"
#include "smartphone\mortafx.h"
#include "smartphone\DlgChoice.h"
#endif

#ifndef DESKTOP
#include "keybd.h"
#endif

#include "morttypes.h"
#include "variables.h"

#include <string.h>

#include "helpers.h"

#include "Interpreter.h"
#include "IniFile.h"
#include "math.h"
#include <Winuser.h>
#include "FunctionsFileReg.h"

extern LPCTSTR	MutexName;

CMortPtrArray	SubResultStack;
CMortPtrArray	LocalVariablesStack;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SMARTPHONE
extern CMortScriptApp theApp;
#endif

CMapStrToInt		KeyCodes;
BYTE                Cont = TRUE;
CInterpreter		*CurrentInterpreter;

#include "Helpers.h"

CValue * CInterpreter::GetVariable( CStr &var, BOOL create, int *position, BOOL local ){
	CValue * rc = ::GetVariable(var,create,position,local);
	_LastVariable = rc;
	return rc;
}

int
GetCharacterType( TCHAR character )
{
	int rc = CInterpreter::CHAR_OTHERS;
	

	switch ( character ) {
	case L' ':
	case L'\t':
		rc = CInterpreter::CHAR_WHITESPACE;
		break;
		
	case L'\r':
	case L'\n':
		rc = CInterpreter::CHAR_CR;
		break;
		
	case L'_':
		rc = CInterpreter::CHAR_UNDERSCORE;
		break;
		
	case L'$':
		rc = CInterpreter::CHAR_DOLLOR_VAR;
		break;
		
	case L'.':
		rc = CInterpreter::CHAR_DOT;
		break;
		
	case L'\\':
		rc = CInterpreter::CHAR_BACKSLASH;
		break;
		
	case L'\"':
		rc = CInterpreter::CHAR_STR_QUOTE;
		break;
	case L'\'':
		rc = CInterpreter::CHAR_QUOTE;
		break;
		
	case L'\0':
		rc = CInterpreter::CHAR_END;
		break;
	}
	
	if ( rc == CInterpreter::CHAR_OTHERS ) {
		if (    (    character >= L'a'
			&& character <= L'z' )
			|| (    character >= L'A'
			&& character <= L'Z' )
			)
		{
			rc = CInterpreter::CHAR_ALPHA;
		}
		
		if (    rc == CInterpreter::CHAR_OTHERS
			&& character >= L'0'
			&& character <= L'9' )
		{
			rc = CInterpreter::CHAR_DIGIT;
		}
	}
	
	return rc;
}

//jwz:add
//#define DEBUG
#ifdef DEBUG
void Debug(LPCTSTR msg){
	MessageBox(NULL,msg,L"Info",MB_SETFOREGROUND);
}

void Debug(long msg){
	CStr tStr;
	tStr.Format(L"%d",msg);
	MessageBox(NULL,tStr,L"Info",MB_SETFOREGROUND);
}

void Debug(long msg,long title){
	CStr tStr,Title;
	tStr.Format(L"%d",msg);
	Title.Format(L"%d",title);
	MessageBox(NULL,tStr,Title,MB_SETFOREGROUND);
}

void Debug(LPCTSTR msg,long title){
	CStr Title;
	Title.Format(L"%d",title);
	MessageBox(NULL,msg,Title,MB_SETFOREGROUND);
}

void Debug(long msg,LPCTSTR title){
	CStr tStr;
	tStr.Format(L"%d",msg);
	MessageBox(NULL,tStr,title,MB_SETFOREGROUND);
}

void Debug(LPCTSTR msg,LPCTSTR title){
	MessageBox(NULL,msg,title,MB_SETFOREGROUND);
}

#endif
//#undef DEBUG
//jwz:add end
//jwz:add for slash char processing
CStr SlashBackup( CStr line ){
	line.Replace( L"\\\\"	, L"^SLASH^"); 
	line.Replace( L"\\\""	, L"^QUOT^"	); 
	line.Replace( L"\\r"	, L"^CR^"	); 
	line.Replace( L"\\n"	, L"^LF^"	); 
	line.Replace( L"\\t"	, L"^TAB^"	); 
	line.Replace( L"\\a"	, L"^BELL^"	); 
	line.Replace( L"\\b"	, L"^BACK^"	); 
	line.Replace( L"\\f"	, L"^FEED^"	); 
	line.Replace( L"\\'"	, L"^SQUOT^"); 
	line.Replace( L"\\v"	, L"^VTAB^"	); 
	line.Replace( L"\\0"	, L"^NULL^"	); 

	return line;
}

CStr SlashChar(CStr str){
    str.Replace( L"\"\"", L"\"" );
    str.Replace( L"^CR^", L"\r" );
    str.Replace( L"^LF^", L"\n" );
    str.Replace( L"^NL^", L"\r\n" );
    str.Replace( L"^TAB^", L"\t" );
    str.Replace( L"^QUOT^", L"\"" );
    str.Replace( L"^SQUOT^", L"'" );
    str.Replace( L"^BELL^", L"\a" );
    str.Replace( L"^BACK^", L"\b" );
    str.Replace( L"^FEED^", L"\f" );
    str.Replace( L"^SQUOT^", L"\'" );
    str.Replace( L"^SLASH^", L"\\" );
    str.Replace( L"^VTAB^", L"\v" );
    str.Replace( L"^NULL^", L"\0" );

	return str;
}
//jwz:add end.
//============================
int
CInterpreter::Split( const CStr &source, TCHAR sep, CValueArray &dest, BOOL trim /*, CStrArray *variables*/, ULONG dontParse )
{
    int pos = 0;
    int startPos = 0;
	BOOL asExpr = FALSE;
    CStr var;
	CValue		elem;
	
	// Skip spaces
	while ( pos < source.GetLength() && GetCharacterType( source[pos] ) == CHAR_WHITESPACE ) pos++;

	if ( pos < source.GetLength() && source[pos] == L'(' ) {
		asExpr = TRUE;
		pos++;
	}

	startPos = pos;
    dest.RemoveAll();
    while( pos != -1 && pos < source.GetLength() && source[pos] != L')' ) {
        bool quoted = false;
		var.Empty();

		//skip whitespace
        while (  startPos < source.GetLength() && (  source.GetAt(startPos) == L' ' || source.GetAt(startPos) == L'\t' )) startPos++;

		bool fixed  = ( ( dontParse & 1<<dest.GetSize() ) != 0 );

        if ( source.GetAt(startPos) == L'{' || asExpr ) {
            if ( !asExpr ) startPos++;

            CStr expr = source.Mid( startPos );

            elem = EvaluateExpression(expr, FALSE);
			
            if ( GetError() != 0 ) {
                if ( ErrorLevel >= ERROR_SYNTAX )
                    MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                dest.RemoveAll();
                return -1;
            } else {
				pos = startPos + GetErrorPosition();
				if ( fixed )
					elem = expr.Left(GetErrorPosition());
				while ( pos < source.GetLength() && GetCharacterType( source[pos] ) == CHAR_WHITESPACE ) pos++;
				if ( !asExpr ) {
					if ( pos > source.GetLength() || source[pos] != L'}' ) {
						if ( ErrorLevel >= ERROR_SYNTAX )
							MessageBox( GetMsgParent(), L"Operator expected" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
						dest.RemoveAll();
						return -1;
					} else {
						pos++;
						while ( pos < source.GetLength() && GetCharacterType( source[pos] ) == CHAR_WHITESPACE ) pos++;
					}
				}

				if ( pos < source.GetLength() && source[pos] != L',' && ( !asExpr || source[pos] != L')' ) ) {
					if ( ErrorLevel >= ERROR_SYNTAX )
						MessageBox( GetMsgParent(), L"Comma expected" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
					dest.RemoveAll();
					return -1;
				}
            }

            quoted = true;
		} else if ( source.GetAt(startPos) == L'\'' ) {
			startPos++;
			elem = source[startPos++];
			if (source[startPos++] != L'\''){
				if ( ErrorLevel >= ERROR_SYNTAX )
					VarError = L"Char not closed (missing Single quote)";
					MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				dest.RemoveAll();
				break;
			}				
			pos = startPos;
        } else if ( source.GetAt(startPos) == L'\"' ) {
            startPos++;
            pos  = source.Find( L'\"', startPos );

            while ( pos != -1 && pos < source.GetLength()-2 ) {
                int nextQuote = source.Find( L'\"', pos+1 );
                if ( nextQuote != pos+1 )
                    break;
                else
                    pos = source.Find( L'\"', pos+2 );
            }
            int nextSep   = source.Find( sep,  pos+1 );

            if ( pos != -1 ) {
                elem = source.Mid( startPos, pos-startPos );
                pos  = nextSep;
            } else {
                elem = source.Mid( startPos );
            }

            quoted = true;
        } else if ( source.GetAt(startPos) == L'%' ) {
            elem = source.Mid( startPos+1 );
			CValue *val;
			int subPos=0;
			VarError.Empty();
			val = GetVariable( elem, FALSE, &subPos );
			if ( subPos == -1 ) {
				if ( ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				dest.RemoveAll();
				break;
			}
			pos = startPos + subPos + 1;
			ASSERT( source.Find('%', pos) != -1 );
			pos = source.Find(sep, pos);
            if ( val != NULL )
				elem = *val;
            else
				elem.Clear();
		} else {
            pos  = source.Find( sep, startPos );
            if ( pos != -1 ) {
                elem = source.Mid( startPos, pos-startPos );
            } else {
                elem = source.Mid( startPos );
            }
            if ( trim ) { ((CStr)elem).TrimLeft(); ((CStr)elem).TrimRight(); }
        }

		if (elem.GetType() == VALUE_STRING)
			elem = SlashChar(elem);
        dest.Add( elem );

        startPos = pos+1;
    }

	if ( asExpr && source[pos] != L')' ) {
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), L"Missing ')'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		dest.RemoveAll();
	}

	if ( pos != -1 ) // Anything after the parameters?
	{
		// Skip spaces
		if ( asExpr ) pos++; // skip closing parenthese
		while ( pos < source.GetLength() && GetCharacterType( source[pos] ) == CHAR_WHITESPACE ) pos++;
		if ( pos < source.GetLength() && source.GetAt(pos) != L'#' && source.GetAt(pos) != L'/' ){
			if ( ErrorLevel >= ERROR_WARN )
				MessageBox( GetMsgParent(), L"Unhandled text after parameter list" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		}
	}

    return dest.GetSize();
}

BOOL CInterpreter::SetVariable( CStr &varName, CValue &value )
{
	BOOL rc = ::SetVariable( varName, value );
	if ( rc == FALSE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
	}
	return rc;
}

BOOL CInterpreter::SetVariable( CStr &varName, LPCTSTR value )
{
	BOOL rc = ::SetVariable( varName, value );
	if ( rc == FALSE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
	}
	return rc;
}

BOOL CInterpreter::ClearVariable( CStr &varName )
{
	BOOL rc = ::ClearVariable( varName );
	if ( rc == FALSE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
	}
	return rc;
}

CStr CInterpreter::GetErrorLine()
{
	CStr file;
	CStr info;
	file = ScriptFile;

	//info.Format( L"%s %d\n%s", (LPCTSTR)file, ErrorPos, (LPCTSTR)ErrorLine );

	int line = 1;
	for ( long i = 0; i < ErrorPos; i++ ) {
		if ( Content.GetAt(i) == L'\n' ) {
			line++;
			if ( Content.Mid(i+1,7) == L"##FILE:" ) {
				line = 0;

				int endPos = Content.Find( '\n', i+1 );
				int len = endPos - (i+8);
				file = Content.Mid( i+8, len );
				file.TrimRight();
			}
		}
	}

	info.Format( L"\n\nLine %d (%s):\n%s", line, (LPCTSTR)file, (LPCTSTR)SlashChar(ErrorLine) );

	return info;
}

/////////////////////////////////////////////////////////////////////////////
// CInterpreter construction

CInterpreter::CInterpreter()
{
	Variables.SetAt( L"TRUE",   CValue(1L) );
	Variables.SetAt( L"FALSE",  CValue(0L) );
	Variables.SetAt( L"ON",     CValue(1L) );
	Variables.SetAt( L"OFF",    CValue(0L) );
	Variables.SetAt( L"YES",    CValue(1L) );
	Variables.SetAt( L"NO",     CValue(0L) );
	Variables.SetAt( L"CANCEL", CValue(2L) );
	Variables.SetAt( L"PI",		CValue(3.1415926535897932384626433832795) );
	Variables.SetAt( L"SQRT2",	CValue(1.4142135623730950488016887242097) );
	Variables.SetAt( L"PHI",	CValue(1.6180339887498948482045868343656) );
	Variables.SetAt( L"EULER",	CValue(2.7182818284590452353602874713527) );
	Variables.SetAt( L"HKCU",	CValue(L"HKCU") );
	Variables.SetAt( L"HKLM",	CValue(L"HKLM") );
	Variables.SetAt( L"HKCR",	CValue(L"HKCR") );
	Variables.SetAt( L"HKUS",	CValue(L"HKUS") );
	Variables.SetAt( L"BYTES",	CValue(0L) );
	Variables.SetAt( L"KB",		CValue(1L) );
	Variables.SetAt( L"MB",		CValue(2L) );
	Variables.SetAt( L"GB",		CValue(3L) );
	Variables.SetAt( L"ST_HIDDEN",  CValue(0L) );
	Variables.SetAt( L"ST_LIST",	CValue(1L) );
	Variables.SetAt( L"ST_MESSAGE",	CValue(2L) );
	Variables.SetAt( L"VAR_EMPTY",  CValue((long)VALUE_NULL) );
	Variables.SetAt( L"VAR_INT",    CValue((long)VALUE_LONG) );
	Variables.SetAt( L"VAR_FLOAT",  CValue((long)VALUE_DOUBLE) );
	Variables.SetAt( L"VAR_STRING", CValue((long)VALUE_STRING) );
	Variables.SetAt( L"VAR_ARRAY",  CValue((long)VALUE_MAP) );
	Variables.SetAt( L"VAR_FILE",   CValue((long)VALUE_PTR) );
	Variables.SetAt( L"VAR_WINDOW", CValue((long)VALUE_PTR+1) );

	KeyCodes.SetAt( L"alt",			VK_MENU );
	KeyCodes.SetAt( L"ctrl",		VK_CONTROL );
	KeyCodes.SetAt( L"shift",		VK_SHIFT );
	KeyCodes.SetAt( L"cr",			VK_RETURN );
	KeyCodes.SetAt( L"win",			91 );
	KeyCodes.SetAt( L"context",		93 );
    KeyCodes.SetAt( L"tab",			VK_TAB );
    KeyCodes.SetAt( L"esc",			VK_ESCAPE );
	KeyCodes.SetAt( L"space",		VK_SPACE );
	KeyCodes.SetAt( L"up",			VK_UP );
	KeyCodes.SetAt( L"down",		VK_DOWN );
	KeyCodes.SetAt( L"left",		VK_LEFT );
	KeyCodes.SetAt( L"right",		VK_RIGHT );
	KeyCodes.SetAt( L"home",		VK_HOME );
	KeyCodes.SetAt( L"end",			VK_END );
	KeyCodes.SetAt( L"pageup",		VK_PRIOR );
	KeyCodes.SetAt( L"pagedown",	VK_NEXT );
	KeyCodes.SetAt( L"delete",		VK_DELETE );
	KeyCodes.SetAt( L"backspace",	VK_BACK );
	KeyCodes.SetAt( L"insert",		VK_INSERT );
    KeyCodes.SetAt( L"snapshot",	VK_SNAPSHOT );
    KeyCodes.SetAt( L"f1",			VK_F1 );
    KeyCodes.SetAt( L"f2",			VK_F2 );
    KeyCodes.SetAt( L"f3",			VK_F3 );
    KeyCodes.SetAt( L"f4",			VK_F4 );
    KeyCodes.SetAt( L"f5",			VK_F5 );
    KeyCodes.SetAt( L"f6",			VK_F6 );
    KeyCodes.SetAt( L"f7",			VK_F7 );
    KeyCodes.SetAt( L"f8",			VK_F8 );
    KeyCodes.SetAt( L"f9",			VK_F9 );
    KeyCodes.SetAt( L"f10",			VK_F10 );
    KeyCodes.SetAt( L"f11",			VK_F11 );
    KeyCodes.SetAt( L"f12",			VK_F12 );
#ifndef DESKTOP
	KeyCodes.SetAt( L"leftsoft",	VK_TSOFT1 );
	KeyCodes.SetAt( L"rightsoft",	VK_TSOFT2 );
#endif

	ErrorLevel = ERROR_ERROR;
    Connection = NULL;
//jwz:add for last statement result
	CStr tVar = L"$_";
	_Statement_Result = GetVariable(tVar,true);
//jwz:add end

}

CInterpreter::~CInterpreter()
{
	int i;

	// Clean up temporary stack data (should only be required after an error)

	for ( i = 0; i <= ForLimit.GetUpperBound(); i++ )
		delete (CValue*)ForLimit.GetAt(i);

	for ( i = 0; i <= ForStep.GetUpperBound(); i++ )
		delete (CValue*)ForStep.GetAt(i);

	for ( i = 0; i <= ForEachType.GetUpperBound(); i++ ) {
        switch( ForEachType.GetAt( i ) ) {
        // Data in CValueArray
        case 1:
		case 4:
            delete (CValueArray*)ForEachData.GetAt(i);
			break;

        // Data in CStrArray
        case 2:
        case 3:
        case 5:
        case 6:
            delete (CStrArray*)ForEachData.GetAt(i);
			break;
        }
	}


    CloseZipDll();

#ifndef DESKTOP
    if ( Connection != NULL )
		CloseHandle(Connection);
#endif
}


BOOL CInterpreter::SendSpecial( CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
	Split( param, ',', params ) ;
    if ( params.GetSize() != 1 && params.GetSize() != 2 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SendSpecial'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

	long value;
	if ( params.GetAt(0).GetType() == VALUE_LONG ) {
		value = (long) params.GetAt(0);
	} else {
		CStr keyname = params.GetAt(0);
		keyname.MakeLower();
		if ( KeyCodes.Lookup( keyname, value ) == FALSE ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Invalid key code '"+(CStr)params.GetAt(0)+L"' for 'SendSpecial'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}
	}

	if ( params.GetSize() == 1 || ((CStr)params[1]).CompareNoCase( L"up" ) != 0 )
		keybd_event( (BYTE)value, NULL, KEYEVENTF_SILENT, NULL );
#ifdef DESKTOP
	if ( (BYTE)value == VK_MENU && ( params.GetSize() == 1 || !((CStr)params.GetAt(1)).CompareNoCase( L"up" ) ) )
		::Sleep( 100 );
#endif
	if ( params.GetSize() == 1 || ((CStr)params[1]).CompareNoCase( L"down" ) != 0 )
		keybd_event( (BYTE)value, NULL, KEYEVENTF_SILENT|KEYEVENTF_KEYUP, NULL );

	return rc;
}

BOOL CInterpreter::SendKey( CStr &window, USHORT ch, BOOL shift, BOOL ctrl, BOOL alt )
{
    BOOL rc = TRUE;
    if ( ! window.IsEmpty() ) {
	    HWND wnd = FindWindowMy( window , 0);
    	if ( wnd != NULL ) {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        } else {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            } else
                return TRUE;
        }
    }

    if ( shift ) keybd_event( VK_SHIFT,   NULL, KEYEVENTF_SILENT, NULL );
    if ( ctrl  ) keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT, NULL );
    if ( alt   ) keybd_event( VK_MENU,    NULL, KEYEVENTF_SILENT, NULL );
#ifdef DESKTOP
    ::Sleep( 100 );
#endif
	keybd_event( (BYTE)ch, NULL, KEYEVENTF_SILENT, NULL );
	keybd_event( (BYTE)ch, NULL, KEYEVENTF_SILENT|KEYEVENTF_KEYUP, NULL );
    if ( alt   ) keybd_event( VK_MENU,    NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL );
    if ( ctrl  ) keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 
    if ( shift ) keybd_event( VK_SHIFT,   NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 

    return rc;
}

BOOL CInterpreter::SendChar( CStr &window, USHORT ch )
{
    BOOL rc = TRUE;
    HWND wnd;
    if ( ! window.IsEmpty() ) {
	    wnd = FindWindowMy( window ,0);
    	if ( wnd != NULL ) {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        } else {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            } else
                return TRUE;
        }
    } else {
        wnd = ::GetForegroundWindow();
    }

    UINT st = 0, cb = ch;
#ifndef DESKTOP
	int retry;
    for ( retry = 0; retry < 100 && ! PostKeybdMessage( (HWND)-1, 0, KeyStateToggledFlag, 1, &st, &cb ); retry++ )
		Sleep(10);
    for ( retry = 0; retry < 100 && ! PostKeybdMessage( (HWND)-1, 0, KeyStateToggledFlag, 1, &st, &cb ); retry++ )
		Sleep(10);
    // PostKeybdMessage( (HWND)-1, 0, KeyStateToggledFlag, 1, &st, &cb );
#else
    SendKey( window, VkKeyScan( ch ), ((VkKeyScan( ch ) & 256) != 0), ((VkKeyScan( ch ) & 512) != 0), ((VkKeyScan( ch ) & 1024) != 0) );
#endif

	int sendCharDelay = 0;
	CValue *delay;
	if ( Variables.Lookup( L"SENDKEYDELAY", delay ) )
		sendCharDelay = (long)*delay;
	if ( sendCharDelay == 0 ) sendCharDelay = 20;

    ::Sleep( sendCharDelay );

    return rc;
}

BOOL CInterpreter::SendSpecialKey( CStr &cmd, CStr &param )
{
    BOOL rc = 2;    // Unknown command
    CValueArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 0 && params.GetSize() != 1 && params.GetSize() != 3 && params.GetSize() != 4 ) {
        if ( ErrorLevel >= ERROR_SYNTAX ) {
            CStr msg;
            msg.Format( InvalidParameterCount + L"'%s'", (LPCTSTR)cmd );
            MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CStr wnd;
    if ( params.GetSize() == 1 || params.GetSize() >= 3 )
        wnd = (CStr)params[0];

    BOOL ctrl=FALSE, shift=FALSE, alt=FALSE;
    if ( params.GetSize() == 3 ) {
        ctrl  = (long)params[1];
        shift = (long)params[2];
    } 
	
	if ( params.GetSize() == 4 ) {
        alt = (long)params[2];
    } else {
        alt = 0;
    }

	if ( cmd.CompareNoCase( L"SendCR" ) == 0 )
        rc = SendKey( wnd, VK_RETURN, shift, ctrl, alt );
	
	if ( cmd.CompareNoCase( L"SendWin" ) == 0 )
        rc = SendKey( wnd, 91, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendContext" ) == 0 )
        rc = SendKey( wnd, 93, shift, ctrl, alt );

    if ( cmd.CompareNoCase( L"SendTab" ) == 0 )
        rc = SendKey( wnd, VK_TAB, shift, ctrl, alt );
	
    if ( cmd.CompareNoCase( L"SendEsc" ) == 0 )
        rc = SendKey( wnd, VK_ESCAPE, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendSpace" ) == 0 )
        rc = SendKey( wnd, VK_SPACE, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendUp" ) == 0 )
        rc = SendKey( wnd, VK_UP, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendDown" ) == 0 )
        rc = SendKey( wnd, VK_DOWN, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendLeft" ) == 0 )
        rc = SendKey( wnd, VK_LEFT, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendRight" ) == 0 )
        rc = SendKey( wnd, VK_RIGHT, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendHome" ) == 0 )
        rc = SendKey( wnd, VK_HOME, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendEnd" ) == 0 )
        rc = SendKey( wnd, VK_END, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendPageUp" ) == 0 )
        rc = SendKey( wnd, VK_PRIOR, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendPageDown" ) == 0 )
        rc = SendKey( wnd, VK_NEXT, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendDelete" ) == 0 )
        rc = SendKey( wnd, VK_DELETE, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendBackspace" ) == 0 )
        rc = SendKey( wnd, VK_BACK, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendInsert" ) == 0 )
        rc = SendKey( wnd, VK_INSERT, shift, ctrl, alt );

#ifndef DESKTOP
	if ( cmd.CompareNoCase( L"SendLeftSoft" ) == 0 )
        rc = SendKey( wnd, VK_TSOFT1, shift, ctrl, alt );

	if ( cmd.CompareNoCase( L"SendRightSoft" ) == 0 )
        rc = SendKey( wnd, VK_TSOFT2, shift, ctrl, alt );
#endif

    if ( cmd.CompareNoCase( L"Snapshot" )  == 0 )
        rc = SendKey( wnd, VK_SNAPSHOT, shift, ctrl, alt );

    return rc;
}

BOOL CInterpreter::SendKeys( CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SendKeys'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    CStr wnd, keys;
    if ( params.GetSize() == 1 ) {
        wnd.Empty();
        keys = (CStr)params[0];
    } else {
        wnd  = (CStr)params[0];
        keys = (CStr)params[1];
    }

    for ( int i=0; i<keys.GetLength(); i++ )
        rc = SendChar( wnd, keys.GetAt(i) );

    return rc;
}


BOOL CInterpreter::SendCommand( CStr &cmd, CStr &param, DWORD message )
{
    BOOL rc = TRUE;
    CValueArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 0 && params.GetSize() != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'" + cmd + L"'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    HWND wnd;
    if ( params.GetSize() == 0 ) {
        wnd = ::GetForegroundWindow();
    } else {
	    wnd = FindWindowMy( params[0] ,0);
		if ( wnd == NULL ) {
			if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
			} else {
                return TRUE;
            }
		}
    }

    ::PostMessage( wnd, WM_COMMAND, message, 0 );

    return TRUE;
}

BOOL CInterpreter::SendCtrlKey( CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    Split( param, ',', params );
    if ( params.GetSize() != 1 && params.GetSize() != 2 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SendCtrlKey'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    int start = 0; CStr window;
    if ( params.GetSize() == 2 ) {
        window = (CStr)params[0];
        start = 1;
    }

    if ( ((CStr)params[start]).GetLength() != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Invalid length for character in 'SendCtrlKey'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ! window.IsEmpty() ) {
	    HWND wnd = FindWindowMy( window ,0);
    	if ( wnd != NULL ) {
#ifdef DESKTOP
            ::ShowWindow( wnd, SW_SHOW );
#endif
            ::SetForegroundWindow(wnd);
        } else {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)window );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            } else
                return TRUE;
        }
    }

#ifndef DESKTOP
    KEY_STATE_FLAGS st = KeyShiftAnyCtrlFlag | KeyStateDownFlag;
#endif
    UINT nChar = ((CStr)params[start]).GetAt(0);
    if ( nChar >= 'A' && nChar <= 'Z' )
        nChar -= 'A'-1;
    else if ( nChar >= 'a' && nChar <= 'z' )
        nChar -= 'a'-1;
    else {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Invalid character for 'SendCtrlKey'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    if ( nChar != -1 ) {
#ifndef DESKTOP
        keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT, NULL );
        PostKeybdMessage( (HWND)-1, nChar+64, KeyShiftAnyCtrlFlag | KeyStateDownFlag, 1, &st, &nChar );
        st = KeyShiftAnyCtrlFlag | KeyShiftNoCharacterFlag;
        PostKeybdMessage( (HWND)-1, nChar+64, KeyShiftAnyCtrlFlag | KeyShiftNoCharacterFlag, 1, &st, &nChar );
        keybd_event( VK_CONTROL, NULL, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, NULL ); 
#else
        CStr dummy;
        SendKey( dummy, VkKeyScan(nChar+64), 0, 1 );
#endif
    }

    return rc;
}


DWORD CInterpreter::MouseEvent( BYTE button, BOOL release )
{
    DWORD event;

    switch ( button ) {
    case 1:
        event = release ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
        break;

    case 2:
        event = release ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN;
        break;

    default:
        event = release ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
        break;
    }

    return event;
}

BOOL CInterpreter::MouseClick( CStr &param, BOOL dbl, BYTE button )
{
    BOOL rc = TRUE;
    CValueArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 ) {
        if ( ErrorLevel >= ERROR_SYNTAX ) {
            if ( dbl )
                MessageBox( GetMsgParent(), InvalidParameterCount + L"'MouseDblClick'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            else
                MessageBox( GetMsgParent(), InvalidParameterCount + L"'MouseClick'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        }
        return FALSE;
    }

    POINT pt;

    if ( parNo == 3 ) {
        RECT parrect;
    	HWND parwnd = FindWindowMy( params.GetAt(0) ,0);
        if ( parwnd == NULL ) {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x = (long)params[1]+parrect.left;
        pt.y = (long)params[2]+parrect.top;
    } else {
        pt.x = (long)params[0];
        pt.y = (long)params[1];
    }

#ifdef DESKTOP
    mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
#endif
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    if ( dbl ) {
        mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
        mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    }
    /*
    wnd = WindowFromPoint( pt );
    ::GetWindowRect( wnd, &rect );
    ::PostMessage( wnd, WM_LBUTTONDOWN, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    if ( dbl )
        ::PostMessage( wnd, WM_LBUTTONDBLCLK, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    ::PostMessage( wnd, WM_LBUTTONUP,   0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );
    */

    return rc;
}

USHORT MouseX, MouseY;

BOOL CInterpreter::MouseDown( CStr &param, BYTE button )
{
    BOOL rc = TRUE;
    CValueArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'MouseDown'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;

    if ( parNo == 3 ) {
        RECT parrect;
    	HWND parwnd = FindWindowMy( params.GetAt(0) ,0);
        if ( parwnd == NULL ) {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x = (long)params[1]+parrect.left;
        pt.y = (long)params[2]+parrect.top;
    } else {
        pt.x = (long)params[0];
        pt.y = (long)params[1];
    }

    MouseX = (USHORT)pt.x; MouseY = (USHORT)pt.y;
#ifdef DESKTOP
    mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
#endif
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, FALSE ), pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    //wnd = WindowFromPoint( pt );
    //::GetWindowRect( wnd, &rect );
    //::PostMessage( wnd, WM_LBUTTONDOWN, 0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );

    return rc;
}

BOOL CInterpreter::MouseUp( CStr &param, BYTE button )
{
    BOOL rc = TRUE;
    CValueArray params;
    int parNo = Split( param, ',', params );

    if ( parNo != 2 && parNo != 3 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'MouseUp'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    POINT pt;

    if ( parNo == 3 ) {
        RECT parrect;
    	HWND parwnd = FindWindowMy( params.GetAt(0) ,0);
        if ( parwnd == NULL ) {
            if ( ErrorLevel >= ERROR_ERROR ) {
                CStr msg;
                msg.Format( L"Window '%s' could not be found", (LPCTSTR)params[0] );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                rc = FALSE;
            }
            return rc;
        }

        ::GetWindowRect( parwnd, &parrect );
        pt.x = (long)params[1]+parrect.left;
        pt.y = (long)params[2]+parrect.top;
    } else {
        pt.x = (long)params[0];
        pt.y = (long)params[1];
    }

    if ( MouseX != pt.x || MouseY != pt.y ) {
      mouse_event( MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE, pt.x * 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    }
    mouse_event( MOUSEEVENTF_ABSOLUTE|MouseEvent( button, TRUE ), pt.x* 65536 / GetSystemMetrics( SM_CXSCREEN ), pt.y * 65536 / GetSystemMetrics( SM_CYSCREEN ), 0, 0 );
    //wnd = WindowFromPoint( pt );
    //::GetWindowRect( wnd, &rect );
    //::PostMessage( wnd, WM_LBUTTONUP,   0, MAKELPARAM( pt.x-rect.left, pt.y-rect.top ) );

    return rc;
}


//
// Old styled conditions ("If ..." instead of "If ( expression )")
//
BOOL CInterpreter::CheckCondition( CStr &param, BOOL &condRes )
{
    param.TrimLeft();
    param.TrimRight();

    int pos;
    CStr condType;
    CStr element;

    if ( param.GetAt(0) == L'{' || param.GetAt(0) == L'(' ) {
        pos = param.GetLength();
        condType = param;
    } else {
        pos = param.FindOneOf(L" \t");
        if ( pos == -1 ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), L"Invalid condition" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            return FALSE;
        }
        condType = param.Left(pos);
        condType.TrimLeft();
    }
    
    BOOL not = FALSE;
    if ( condType.CompareNoCase( L"not" ) == 0 ) {
        not = TRUE;
        int start = pos+1;
        while ( start < param.GetLength() && ( param[start] == L' ' || param[start] == L'\t' ) ) start++;
        pos = start;
        while ( pos < param.GetLength() && ( param[pos] != L' ' && param[pos] != L'\t' ) ) pos++;
        if ( pos == start || pos == param.GetLength() ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), L"Invalid condition" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            return FALSE;
        }
        
        condType = param.Mid( start, pos-start );
    }


    if ( param.GetAt(0) != L'{' && param.GetAt(0) != L'(' ) {
        element = param.Mid(pos+1);
        element.TrimLeft(); element.TrimRight();
    }
    //element.TrimLeft('\"'); element.TrimRight('\"');

	if ( condType.GetAt(0) == L'{' || condType.GetAt(0) == L'(' ) {
        if ( condType.GetLength() > 1 )
            element = condType.Mid(1) + element;
        element.TrimRight();
        if ( element.Right(1) == L")" || element.Right(1) == L"}" )
            element = element.Left( element.GetLength()-1 );

        CheckExpression( element, condRes );
    } else if ( condType.CompareNoCase( L"wndExists" ) == 0 )
        CheckWndExists( element, condRes );
    else if ( condType.CompareNoCase( L"wndActive" ) == 0 )
        CheckWndActive( element, condRes );
    else if ( condType.CompareNoCase( L"procExists" ) == 0 )
        CheckProcExists( element, condRes );
    else if (   condType.CompareNoCase( L"fileExists" ) == 0
             || condType.CompareNoCase( L"dirExists" ) == 0 )
        CheckFileExists( element, condType.CompareNoCase( L"dirExists" ) == 0, condRes );
    else if ( condType.CompareNoCase( L"question" ) == 0 )
        CheckQuestion( element, condRes );
    else if ( condType.CompareNoCase( L"screen" ) == 0 )
        CheckScreen( element, condRes );
    else if ( condType.CompareNoCase( L"regKeyExists" ) == 0 )
        CheckRegKeyExists( element, condRes );
    else if ( condType.CompareNoCase( L"regKeyEqualsString" ) == 0 )
        CheckRegKeyEqualsString( element, condRes );
    else if ( condType.CompareNoCase( L"regKeyEqualsDWord" ) == 0 )
        CheckRegKeyEqualsDWord( element, condRes );
    else if ( condType.CompareNoCase( L"equals" ) == 0 )
        CheckEquals( element, condRes );
    else if ( condType.CompareNoCase( L"expression" ) == 0 )
        CheckExpression( element, condRes );
    else {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Invalid condition" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    if ( not ) condRes = !condRes;

    return TRUE;
}

BOOL CInterpreter::CheckWndExists( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If checkWndExists'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    if ( FindWindowMy( params[0] ,0) != NULL )
        condRes = TRUE;

    return TRUE;
}

BOOL CInterpreter::CheckWndActive( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If checkWndActive'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

	HWND hwnd = ::GetForegroundWindow();
	TCHAR windowTitle[256];
	::GetWindowText( hwnd, windowTitle, 256 );
    if ( CStr(windowTitle).Find( (CStr)params[0] ) != -1 ) {
		condRes = TRUE;
	}

    return TRUE;
}

BOOL CInterpreter::CheckProcExists( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If checkProcExists'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

	if ( LoadToolhelp() ) {
		HANDLE         procSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS|TH32CS_SNAPNOHEAPS, 0 );
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof( procEntry );

		if ( procSnap != NULL && Process32First( procSnap, &procEntry ) ) {
			do {
				CStr procName = procEntry.szExeFile;
				if ( procName.CompareNoCase( params[0] ) == 0 ) {
					condRes = TRUE;
					break;
				}
				procEntry.dwSize = sizeof( procEntry );
			} while ( Process32Next( procSnap, &procEntry ) );
		}
		if ( procSnap != NULL )
#ifndef DESKTOP
	      CloseToolhelp32Snapshot( procSnap );
#else
		  CloseHandle( procSnap );
#endif
	} else {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'If checkProcExists' requires toolhelp.dll on your device" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
	}

    return TRUE;
}

BOOL CInterpreter::CheckFileExists( CStr &element, BOOL dir, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If checkFileExists'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    DWORD attribs = GetFileAttributes( params[0] );
    if ( attribs != -1 ) {
        if ( (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0 ) {
            if ( dir ) condRes = TRUE;
        } else {
            if ( !dir ) condRes = TRUE;
        }
    }

    return TRUE;
}

BOOL CInterpreter::CheckQuestion( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    Split( element, ',', params );
    if ( params.GetSize() == 2 ) {
        if ( ::MessageBox( GetMsgParent(), params[0], params[1], MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND ) == IDYES )
            condRes = TRUE;
    }
    if ( params.GetSize() == 1 ) {
        if ( ::MessageBox( GetMsgParent(), params[0], L"JScripts", MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND ) == IDYES )
            condRes = TRUE;
    }

    return TRUE;
}

BOOL CInterpreter::CheckScreen( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    int parCnt = Split( element, ',', params );
    if ( parCnt != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If checkScreen'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ((CStr)params[0]).CompareNoCase( L"landscape" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CXSCREEN ) > GetSystemMetrics( SM_CYSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"portrait" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > GetSystemMetrics( SM_CXSCREEN ) );
    else if ( ((CStr)params[0]).CompareNoCase( L"vga" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) > 320 && GetSystemMetrics( SM_CXSCREEN ) > 320 );
    else if ( ((CStr)params[0]).CompareNoCase( L"qvga" ) == 0 )
        condRes = ( GetSystemMetrics( SM_CYSCREEN ) <= 320 && GetSystemMetrics( SM_CXSCREEN ) <= 320 );
    else {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Invalid screen condition" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

    return TRUE;
}

BOOL CInterpreter::CheckRegKeyExists( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    if ( Split( element, ',', params ) != 3 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If regKeyExists'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL ) {
        if ( ErrorLevel >= ERROR_SYNTAX ) {
            MessageBox( GetMsgParent(), L"Invalid root entry" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        }
        return FALSE;
    }

    HKEY key;
	if ( RegOpenKeyEx( root, params[1], 0, REG_ACCESS_READ, &key ) == ERROR_SUCCESS ) {
		DWORD type, length;
		TCHAR cont[MAX_PATH];
		length = MAX_PATH;
		int rc = RegQueryValueEx( key, params[2], NULL, &type, (BYTE*)cont, &length );
		if ( rc == ERROR_SUCCESS || rc == ERROR_MORE_DATA ) {
			condRes = TRUE;
		}
		RegCloseKey( key );
	}

    return condRes;
}

BOOL CInterpreter::CheckRegKeyEqualsString( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    if ( Split( element, ',', params ) != 4 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If regKeyEqualsString'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL ) {
        if ( ErrorLevel >= ERROR_SYNTAX ) {
            MessageBox( GetMsgParent(), L"Invalid root entry" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        }
        return FALSE;
    }

    CStr value = RegRead( root, params[1], params[2] );
    if ( value.Compare( params[3] ) == 0 ) condRes = TRUE;

    return condRes;
}

BOOL CInterpreter::CheckRegKeyEqualsDWord( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    if ( Split( element, ',', params ) != 4 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If regKeyEqualsDWord'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    HKEY root = GetTopKey( params[0] );
    if ( root == NULL ) {
        if ( ErrorLevel >= ERROR_SYNTAX ) {
            MessageBox( GetMsgParent(), L"Invalid root entry" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        }
        return FALSE;
    }

    DWORD value = RegReadDW( root, params[1], params[2] );
    if ( value == (DWORD)(long)params[3] ) condRes = TRUE;

    return condRes;
}

BOOL CInterpreter::CheckEquals( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValueArray params;
    if ( Split( element, ',', params ) != 2 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'If equals'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    if ( ((CStr)params[0]).Compare( ((CStr)params[1]) ) == 0 ) condRes = TRUE;

    return condRes;
}

BOOL CInterpreter::CheckExpression( CStr &element, BOOL &condRes )
{
    condRes = FALSE;

    CValue result = EvaluateExpression( element );

    if ( GetError() != 0 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND);
        return FALSE;
    }

    if ( (long)result != 0 ) condRes = TRUE;

    return condRes;
}

//jwz:modi for ++ -- and some op= (&= |= ^= %= ..= <<= >>= \=)
BYTE CInterpreter::Assignment( CStr &line , int AssignPos )
{
	BYTE cont = 2;
	CStr var, expr;
	TCHAR assignment = 0;
	int IncDecType = 0;
	CValue *val = NULL;

    if ( line.GetAt(0) == L'%' ) {
		int pos;
		val = GetVariable( line.Mid(1), FALSE, &pos );
        if ( pos != -1 ) {
			var = line.Mid( 1, pos );
			expr = line.Mid(pos+1);
			expr.TrimLeft();
			if ( expr.IsEmpty() || expr[0] != L'%' ) {
				if ( ErrorLevel >= ERROR_SYNTAX ) {
					if ( ErrorLevel >= ERROR_SYNTAX )
						MessageBox( GetMsgParent(), L"Assinged variable not closed (missing '%')" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
					cont = FALSE;
				}
			} else {
				expr = expr.Mid(1);
				expr.TrimLeft();
			}
        }
    } else if ( line.GetAt(0) == L'[' ) {
		int subPos, dummy;
		expr = line.Mid( 1 );
		// expression.TrimLeft();
		CValue result = EvaluateExpression( expr, FALSE );

		subPos = GetErrorPosition();
		if ( GetError() != 0 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Error in variable reference: " + VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			cont = FALSE;
		}

		if ( Cont && expr[subPos] != L']' ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable reference not closed (missing ']')" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			cont = FALSE;
		}

		if ( Cont ) {
			var = (CStr)result;
			if ( var.IsEmpty() ) {
				if ( ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), L"Variable reference is empty" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				cont = FALSE;
			}
		}

		if ( cont ){
			val = GetVariable( var, TRUE, &dummy );

			expr = expr.Mid( subPos+1 );
			expr.TrimLeft();
        }
    } else {
		int eqPos;
		if (AssignPos != -1) 
			eqPos = AssignPos;
		else 
			eqPos = FindStrNotInQuote( line, L"=",true);

		if ( eqPos != -1) {
			int pos; 
			if (line.Left(2).Compare(L"++")==0){
				IncDecType = TOKEN_D_PLUS;
				line = line.Mid(2);
			}else if (line.Left(2).Compare(L"--")==0){
				IncDecType = TOKEN_D_MINUS;
				line = line.Mid(2);
			}
			val = GetVariable( line, TRUE, &pos );
			if (val == NULL) {
				MessageBox(GetMsgParent(),L"Variable invalid " + GetErrorLine(),L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND);
				exit(1);
			}
			eErrorPos += pos;
			var  = line.Left( pos );
			expr = line.Mid(pos);
			expr.TrimLeft();
		}
	}

	if ( cont == FALSE ) return FALSE;

	if ( val == NULL && !VarError.IsEmpty() ){
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		cont = FALSE;
	}else if ( expr.IsEmpty() || ( expr[0] != L'=' && expr[1] != L'=' && expr[2] != L'=')){
		if ( line.GetAt(0) == L'%' || line.GetAt(0) == L'[' ){
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Missing '=' after variable" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			cont = FALSE;
		}
	}else{
		if ( expr[0] != L'=' ){
			if ( expr[0] != L'/' 
				&& expr[0] != L'*' 
				&& expr[0] != L'+' 
				&& expr[0] != L'-' 
				&& expr[0] != L'&' 
				&& expr[0] != L'|' 
				&& expr[0] != L'^' 
				&& expr[0] != L'%' 
				&& (expr[0] != L'<' || expr[1] !=L'<')
				&& (expr[0] != L'>' || expr[1] !=L'>')
				&& expr[0] != L'\\' )
			{
				if ( ErrorLevel >= ERROR_SYNTAX ){
					CStr msg;
					msg.Format( L"Invalid assignment operator '%c%c'", expr[0], expr[1] );
					MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				}
				cont = FALSE;
			}else{
				assignment = expr[0];
				if (assignment!=L'<' && assignment != L'>' && assignment != L'.')	
					expr = expr.Mid(2); 
				else 
					expr = expr.Mid(3);
			}
		}else{
			assignment = L'=';
			eErrorPos ++;
			expr = expr.Mid(1);
		}

		expr.TrimLeft();
	}

    if ( assignment != 0 ){
		CValue oldVal;
		oldVal = *val;
	
		cont = SetVarToExpr( val, expr, true );

		if ( cont && assignment != L'=' ){
			if ( assignment == L'+' ){
				if ( val->IsDouble() || oldVal.IsDouble() ){
					*val = (double)*val + (double)oldVal;
				}else if (val->GetType() == VALUE_STRING || oldVal.GetType() == VALUE_STRING){
					*val = (CStr)oldVal + (CStr)*val;
				}else{
					*val = (long)*val + (long)oldVal;
				}
			}
			if ( assignment == L'-' ){
				if ( val->IsDouble() || oldVal.IsDouble() ){
					*val = (double)oldVal - (double)*val;
				}else{
					*val = (long)oldVal - (long)*val;
				}
			}
			if ( assignment == L'*' ){
				if ( val->IsDouble() || oldVal.IsDouble() ){
					*val = (double)*val * (double)oldVal;
				}else{
					*val = (long)*val * (long)oldVal;
				}
			}
			if ( assignment == L'/' ){
				if ( val->IsDouble() || oldVal.IsDouble() ){
					*val = (double)oldVal / (double)*val;
				}else{
					*val = (long)oldVal / (long)*val;
				}
			}

			if ( assignment == L'&' ){
				*val = (long)oldVal & (long)*val;
			}
			if ( assignment == L'%' ){
				*val = (long)oldVal % (long)*val;
			}
			if ( assignment == L'|' ){
				*val = (long)oldVal | (long)*val;
			}
			if ( assignment == L'^' ){
				*val = (long)oldVal ^ (long)*val;
			}
			if ( assignment == L'<' ){
				*val = (long)oldVal << (long)*val;
			}
			if ( assignment == L'>' ){
				*val = (long)oldVal >> (long)*val;
			}
			if ( assignment == L'\\' ){
				CStr left = oldVal;
				CStr right = *val;
				left.TrimRight(L'\\');
				right.TrimLeft(L'\\');
				*val = left + L"\\" + right;
			}
		}
    }
	// else "cont" remains 2 (for "unknown command")

	if (IncDecType == TOKEN_D_PLUS){
		*val = (long)*val + 1L;
	}else if (IncDecType == TOKEN_D_MINUS){
		*val = (long)((long)*val - 1);
	}
	*_Statement_Result = *val; //Save assignment result

	return cont;
}

BOOL CInterpreter::Set( CStr &param )
{
    BOOL rc = TRUE;

	param.TrimLeft(); param.TrimRight();
	if ( param[0] == L'(' && param[param.GetLength()-1] == L')' ) {
		param = param.Mid( 1, param.GetLength() - 2 );
		param.TrimLeft(); param.TrimRight();
	}
    long pos = param.Find( L',' );

    if ( pos == -1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Set'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
    }

	VarError.Empty();
    CStr varName;
	CValue *value;

	if ( param[0] == L'%' ) {
		int subPos;
		value = GetVariable( param.Mid(1), FALSE, &subPos );
		CStr rest = param.Mid(subPos+1);
		if ( rest.IsEmpty() || rest[0] != L'%' ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable not closed (missing '%')" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}
		if ( value == NULL )
			varName.Empty();
		else {
			varName = (LPCTSTR)*value;
			//varName.MakeUpper();
			int dummy;
			value = GetVariable( varName, TRUE, &dummy );
		}
		pos = param.Find( L',', subPos+2 );
	} else if ( param[0] == L'[' ) {
		int subPos, dummy;
		CStr expression = param.Mid( 1 );

		CValue result = EvaluateExpression( expression, FALSE );

		subPos = GetErrorPosition();
		if ( GetError() != 0 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Error in variable reference: " + VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}

		if ( expression[subPos] != L']' ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable reference not closed (missing ']')" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}

		varName = (CStr)result;
		if ( varName.IsEmpty() ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable reference is empty" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}
		value = GetVariable( varName, TRUE, &dummy );
		pos = param.Find( L',', subPos );
	} else {
		int subPos;
		value = GetVariable( param, TRUE, &subPos );
		varName = param.Left(subPos);
		if ( VarError.IsEmpty() ) {
			CStr rest = param.Mid(subPos); rest.TrimLeft();
			if ( rest.IsEmpty() || rest[0] != L',' ) {
				if ( ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), L"Invalid variable name" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				return FALSE;
			}
			pos = param.Find( L',', subPos );
		}
	}

	if ( ! VarError.IsEmpty() ) {
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		return FALSE;
	}

	if ( pos == -1 ) {
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), InvalidParameterCount + L"'Set'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		return FALSE;
	}

	if ( value != NULL ) {
		CStr expression = param.Mid( pos+1 );
		rc = SetVarToExpr( value, expression, FALSE );
	}

    return rc;
}

BOOL CInterpreter::SetVarToExpr( CValue *variable, CStr &expr, BOOL remAllowed )
{
	expr.TrimLeft();

	// Old style with {...}
	int eErrorPosSave = eErrorPos;
	if ( expr[0] == L'{' ) {
		expr = expr.Mid( 1, expr.ReverseFind('}')-2 );
	}

	CValue result = EvaluateExpression( expr );

	eErrorPos += eErrorPosSave;
	
	if ( GetError() != 0 ) {
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
		return FALSE;
	} if ( remAllowed ) {
		int pos = GetErrorPosition();
		if (   pos < expr.GetLength()
			&& (expr.GetAt(pos) != L'#' && (expr.GetAt(pos)!=L'/' && expr.GetAt(pos+1)!=L'/')))
		{
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Operator expected" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			return FALSE;
		}
	}

	variable->CopyFrom( result );

	return TRUE;
}
//jwz:modi for long source code, type as char state as long

void CInterpreter::PushCondStack( char type, long state )
{
	long l = state & 0x0FFFFFFF,t = type;
	t <<= 28;
	l |= t;
    CondStack.Add( l );
    CondState = state ? true:false;
}

void CInterpreter::PopCondStack( char &type, long &state, BOOL remove )
{
    if ( CondStack.GetSize() == 0 ) {
        type  = 0;
        state = 0;
    } else {
        int   idx   = CondStack.GetUpperBound();
        DWORD entry = CondStack.GetAt( idx );
        type  = (char)(entry>>28);
        state = entry & 0x0FFFFFFF;
        if ( remove ) {
            CondStack.RemoveAt( idx );
            if ( state != 2 ) {
                CondState = 1;
            }
        }
    }
}
//jwz:modi end

//jwz:modi for support single line if() stmt elseif() stmt else stmt
BOOL CInterpreter::If( CStr &param )
{
    BOOL condRes;
	//jwz:add for Singleline if
	BOOL IsSingleLine = false;
	param.TrimLeft();param.TrimRight();
	CStr tStr = param, tParam;
	if (IsSingleIf(tStr, tParam)){
		IsSingleLine = true;
	}
	param = tStr;
	//jwz:add-end
    if ( CondState != 1 )
        condRes = 2;
    else if ( CheckCondition( param, condRes ) == FALSE ) {
		return FALSE;
    }

    if (!IsSingleLine) {
		PushCondStack( CT_IF, condRes );
	}

	// Jump to ElseIf/Else/EndIf
	if ( condRes != 1 ){
		long endPos;
		if (IsSingleLine){
			if (IsSingleElse(tParam, tStr))
				if (condRes == 1) param = tParam; else param = tStr;
			else 
				param.Empty();
		}else if ( EndIfPositions.Lookup( PreviousPos, endPos ) ){ // Lookup (CurrentPos,endPos)
			CurrentPos = endPos;
			param.Empty();			//add empty param.
		}else{
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"if block statement can't find endif!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			exit(1);
		}
	}else if (IsSingleElse(tParam,tStr))
		if (condRes == 1) param = tParam; else param = tStr;
	else
		param = tParam;

    return TRUE;
}

BOOL CInterpreter::ElseIf( CStr &param )
{
    char type;	long state;
    PopCondStack( type, state );
	//jwz:add
	CStr tStr = param;
	param.TrimLeft();param.TrimRight();
	if (tStr.Right(4).CompareNoCase(L"then")==0){
		int lLen = tStr.GetLength()-4;
		tStr = tStr.Left(lLen);
		tStr.TrimLeft();tStr.TrimRight();
		param = L"("+tStr+L")";
	}
	param.TrimLeft();param.TrimRight();
	//jwz:add-end
    if ( type != CT_IF ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"ElseIf without If" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    switch( state ) {
	case 1:
		state = 3; // was fulfilled once
		break;

	case 0:
		{
			BOOL condRes;
			if ( CheckCondition( param, condRes ) == FALSE )
				return FALSE;
			state = condRes;
		}
		break;
    }

	PushCondStack( CT_IF, state );
	if ( state == 3 ) {
		CondState = 0;
	}

	// Jump to ElseIf/Else/EndIf
	if ( CondState != 1 ) {
		long endPos;
		if ( EndIfPositions.Lookup( PreviousPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}


BOOL CInterpreter::Else( )
{
    char type; long state;
    PopCondStack( type, state );

    if ( type != CT_IF ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Else without If" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    // Additional level in FALSE block
    if ( state < 2 ) {
        state = !state;
    }
	if ( state == 3 )
		state = 0;
    PushCondStack( CT_IF, state );

	// Jump to EndIf
	if ( CondState != 1 ) {
		long endPos;
		if ( EndIfPositions.Lookup( PreviousPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}
//jwz:modi end

BOOL CInterpreter::EndIf()
{
    char type; long state;
    PopCondStack( type, state );

    if ( type != CT_IF ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"EndIf without If" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    return TRUE;
}

BOOL CInterpreter::While( CStr &param )
{
    BOOL condRes;

    if ( CondState != 1 )
        condRes = 2;
    else {
        if ( CheckCondition( param, condRes ) == FALSE )
            return FALSE;
    }

    WhilePos.Add( PreviousPos );

    PushCondStack( CT_WHILE, condRes );

	// Jump to EndWhile
	if ( condRes != 1 ) {
		long endPos;
		if ( EndWhilePositions.Lookup( PreviousPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}

BOOL CInterpreter::EndWhile( CStr &param )
{
    BOOL rc = TRUE;
    char type; long state;
    PopCondStack( type, state );

    if ( type != CT_WHILE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'EndWhile' without 'While'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    int  stackPos = WhilePos.GetUpperBound();
    int  whilePos = WhilePos.GetAt(stackPos);
    WhilePos.RemoveAt(stackPos);

    if ( state == TRUE ) {
        CurrentPos = whilePos;
    }

    return rc;
}


BOOL CInterpreter::For( CStr &param)
{
    BOOL condRes = TRUE;

    if ( CondState != 1 )
        condRes = 2;
    else {
        CStr paramLc = param;
        paramLc.MakeLower();
        int eqPos = FindStrNotInQuote( paramLc, L"=",true);

        if ( eqPos == -1 ) {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                MessageBox( GetMsgParent(), L"For without \"=\"" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }

		// Get variable name
        CStr varName = param.Left( eqPos );
        varName.TrimLeft(); varName.TrimRight();
        if ( varName.IsEmpty() ) {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                MessageBox( GetMsgParent(), L"For: Missing variable name" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }

		// Get variable's CValue object
		VarError.Empty();
		CValue *variable = GetVariable( varName, TRUE );
		if ( ! VarError.IsEmpty() ) {
			if ( ErrorLevel >= ERROR_SYNTAX ) {
				MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
			}
			return FALSE;
		}

		// Get and parse start expression
        CStr tmpstr = param.Mid( eqPos+1 );

		CValue result = EvaluateExpression( tmpstr, FALSE );
        if ( GetError() != 0 ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            return FALSE;
        }

		// Set start value
		*variable = result;

		// Check "to"
		tmpstr = tmpstr.Mid( GetErrorPosition() );
		tmpstr.TrimLeft();

		if ( tmpstr.Mid( 0, 2 ).CompareNoCase( L"TO" ) != 0 && tmpstr.Mid(0,2).Compare(L"..")!=0 && tmpstr.Mid(0,1).Compare(L",")!=0) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), L"For without \"to\"" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            return FALSE;
		}

		// Get and parse end expression
		if (tmpstr[0] == L',') 
			tmpstr = tmpstr.Mid(1);
		else
			tmpstr = tmpstr.Mid(2);

		CValue limit = EvaluateExpression( tmpstr, FALSE );

        if ( GetError() != 0 ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            return FALSE;
        }

		CValue step;
		step = (long)( (double)*variable < (double)limit ) ? 1L : -1L;
		tmpstr = tmpstr.Mid( GetErrorPosition() );
		tmpstr.TrimLeft();

		// "Step" (or something wrong) after a valid end value?
		if ( tmpstr.GetLength() != 0 && tmpstr.GetAt(0) != L'#' ) {
			if ( tmpstr.Mid( 0, 4 ).CompareNoCase( L"STEP" ) != 0 && !tmpstr.StartsWith(L",")) {
	            if ( ErrorLevel >= ERROR_SYNTAX )
		            MessageBox( GetMsgParent(), L"Error in \"to\" expression or misspelled \"step\"" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				return FALSE;
			}

			// Get and parse step expression
			if (tmpstr[0]==L',')
				tmpstr = tmpstr.Mid(1);
			else
				tmpstr = tmpstr.Mid(4);

			step   = EvaluateExpression( tmpstr, FALSE );
			if ( GetError() != 0 ) {
				if ( ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), GetErrorMessage() + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				return FALSE;
			}

			if ( (double)step == 0 ) {
				if ( ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), L"For: Step is 0, no infinite loop allowed!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				return FALSE;
			}
		}

		// Create non-temporary objects and add them to the stack
		CValue *stackLimit = new CValue( limit );
		CValue *stackStep  = new CValue( step );
		ForVariable.Add( (UINT)variable );
		ForLimit.Add( (UINT)stackLimit );
		ForStep.Add( (UINT)stackStep  );

		if ( variable->IsDouble() || step.IsDouble() || limit.IsDouble() ) {
			long lVal1 = (long)( (double)*variable * pow( 10, 6 ) + .5 );
			long lVal2 = (long)( (double)limit    * pow( 10, 6 ) + .5 );
			if ( (double)step > 0 )
				condRes = ( lVal1 <= lVal2 );
			else
				condRes = ( lVal1 >= lVal2 );
		} else {
			long lVal1 = (long)*variable;
			long lVal2 = (long)limit;
			if ( (long)step > 0 )
				condRes = ( lVal1 <= lVal2 );
			else
				condRes = ( lVal1 >= lVal2 );
		}
	}

    PushCondStack( CT_FOR, condRes );
    ForPos.Add( CurrentPos );

	// Jump to Next
	if ( condRes != 1 ) {
		long endPos;
		if ( EndForPositions.Lookup( CurrentPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}

BOOL CInterpreter::Next( CStr &param )
{
    BOOL rc = TRUE;
    char type;	long cond;
    PopCondStack( type, cond, FALSE );
    if ( type != CT_FOR ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'Next' without 'For'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    int stackPos = ForPos.GetUpperBound();

    if ( cond == 1 ) {
        CValue *variable = (CValue*)ForVariable.GetAt(stackPos);
		CValue *step     = (CValue*)ForStep.GetAt(stackPos);
		CValue *limit    = (CValue*)ForLimit.GetAt(stackPos);

		if ( cond == 1 ) {
			if ( variable->IsDouble() || step->IsDouble() || limit->IsDouble() ) {
				*variable = (double)*variable + (double)*step;
				long lVal1 = (long)( (double)*variable * pow( 10, 6 ) + .5 );
				long lVal2 = (long)( (double)*limit    * pow( 10, 6 ) + .5 );
				if ( (double)*step > 0 )
					cond = ( lVal1 <= lVal2 );
				else
					cond = ( lVal1 >= lVal2 );
			} else {
				if (variable->GetType() == VALUE_CHAR){
					*variable = (TCHAR)((long)*variable + (long)*step);
				}else 
					*variable = (long)*variable + (long)*step;
				long lVal1 = (long)*variable;
				long lVal2 = (long)*limit;
				if ( (long)*step > 0 )
					cond = ( lVal1 <= lVal2 );
				else
					cond = ( lVal1 >= lVal2 );
			}
		}

		// No more continues...
		if ( cond == 0 ) {
			delete step;
			delete limit;

			ForVariable.RemoveAt(stackPos);
			ForStep.RemoveAt(stackPos);
			ForLimit.RemoveAt(stackPos);
		}
	}
	
    // Back to beginning...
    if ( cond == 1 ) {
        CurrentPos = ForPos.GetAt(stackPos);
        if ( cond != 2 )
            CondState = TRUE;
    } else {
        // Remove position and state (also done for cond==2)
        ForPos.RemoveAt(stackPos);
        PopCondStack( type, cond );
    }

    return rc;
}

//#ifndef SMARTPHONE
BOOL CInterpreter::ForEach( CStr &param)
{
    BOOL condRes = TRUE;

    if ( CondState != 1 )
        condRes = 2;
    else {
        CStr paramLc = param;
        paramLc.MakeLower();
        int inPos = paramLc.Find( L" in " );
		if (inPos == -1 ){
			inPos = paramLc.Find(L"\tin\t");
			if ( inPos == -1 ){
				inPos = paramLc.Find(L"\tin ");
				if (inPos == -1){
					inPos = paramLc.Find(L" in\t");
				}
			}
		}

        if ( inPos == -1 ) {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                MessageBox( GetMsgParent(), L"ForEach without \"in\"" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }

        CStr variable = param.Left( inPos );
        variable.TrimLeft(); variable.TrimRight();
        if ( variable.IsEmpty() ) {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                MessageBox( GetMsgParent(), L"ForEach: Missing variable name(s)" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }
        CStrArray variables;
        int varCount = CIniFile::Split( variable, L",", variables );
		variable.Empty();
		for ( int i=0; i<variables.GetSize(); i++ ) {
			VarError.Empty();
			GetVariable( variables[i], FALSE );
			if ( ! VarError.IsEmpty() ) {
				if ( ErrorLevel >= ERROR_SYNTAX ) {
					MessageBox( GetMsgParent(), VarError + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				}
				return FALSE;
			}
			variables.SetAt( i, variables[i] );
			variable += L"\n" + variables[i];
		}
		variable = variable.Mid(1);

        CStr tmptype = paramLc.Mid( inPos+4 );
        int lSpaces = tmptype.GetLength();
        tmptype.TrimLeft();
        lSpaces -= tmptype.GetLength();
        int typePos = tmptype.FindOneOf( L" \t(" );
        if ( typePos == -1 ) {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                MessageBox( GetMsgParent(), L"ForEach: invalid loop type" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }

        CStr stuff = param.Mid(inPos+typePos+lSpaces+4);
        stuff.TrimLeft(); stuff.TrimRight();
        CStr type = tmptype.Left(typePos);
        int varCnt = 1;
        if ( type == L"values" ) {
            if ( stuff.IsEmpty() ) {
                MessageBox( GetMsgParent(), L"ForEach: no value list found" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with values" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray *values = new CValueArray;
            int count = Split( stuff, L',', *values );
            if ( count > 0 ) {
                SetVariable( variables[0], values->GetAt(0) );
                values->RemoveAt( 0 );
                ForEachData.Add( (void*)values );
            } else {
                condRes = FALSE;
                ForEachData.Add( values );
            }
            ForEachVariables.Add( variable );
            ForEachType.Add( 1 );
        } else if ( type == L"split" ) {
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with split" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 3 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with split" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }

            condRes = FALSE;
            CStrArray *values = new CStrArray;
            CIniFile::Split( params[0], params[1], *values, (long)params[2] );

            if ( values->GetSize() > 0 ) {
                SetVariable( variables[0], values->GetAt(0) );
                values->RemoveAt(0);
                condRes = TRUE;
            } else {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 2 );
        } else if ( type == L"array" ) {
            if ( varCount != 1 && varCount != 2 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with array" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 1 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with array" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }

            condRes = FALSE;
            CValueArray *values = new CValueArray;
			CMapStrToValue *map = NULL;
			if ( params[0].GetType() != VALUE_MAP ) {
				CValue *val = GetVariable( params[0], FALSE );
				if ( val != NULL && val->GetType() == VALUE_MAP )
					map = val->GetMap();
			} else
				map = params[0].GetMap();

			if ( map != NULL && map->GetSize() > 0 ) {
				if ( varCount == 1 ) {
					CValue *val;
					map->Lookup( L"1", val );
					if ( val != NULL && !val->IsNull() ) {
						SetVariable( variables[0], *val );
						for ( int i = 2; TRUE; i++ ) {
							CStr elemVar;
							elemVar.Format( L"%d", i );
							map->Lookup( elemVar, val );
							if ( val != NULL && !val->IsNull() )
								values->Add( *val );
							else
								break;
						}
						condRes = TRUE;
					} else {
						condRes = FALSE;
					}
				} else {
					int mappos = map->GetStartPosition();
					CStr key; CValue value;
					while ( mappos != 0 ) {
						map->GetNextAssoc( mappos, key, value );
						if ( ! value.IsNull() ) {
							values->Add( CValue(key) );
							values->Add( value );
						}
					}
					if ( values->GetSize() > 0 ) {
						SetVariable( variables[0], values->GetAt(0) );
						SetVariable( variables[1], values->GetAt(1) );
						values->RemoveAt(0);
						values->RemoveAt(0);
						condRes = TRUE;
					} else
						condRes = FALSE;
				}
			} else {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( (varCount==1) ? 1 : 4 );
        } else if ( type == L"inisections" ) {
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with iniSections" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 1 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with iniSections" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }

            CIniFile iniFile;
            CStr data;
            condRes = FALSE;
            CStrArray *values = new CStrArray;
            if ( iniFile.Read( params[0] )) {
                int mappos = iniFile.Sections.GetStartPosition();
                CStr  section;
                void    *dummy;
                for ( int i=0; mappos != NULL; i++ ) {
                    iniFile.Sections.GetNextAssoc( mappos, section, dummy );
                    if ( i == 0 ) {
                        SetVariable( variables[0], section );
                        condRes = TRUE;
                    } else {
                        values->Add( section );
                    }
                }
            }
            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 3 );
        } else if ( type == L"inikeys" ) {
            if ( varCount != 2 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with iniKeys" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 2 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with iniKeys" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }

            CIniFile iniFile;
            CStr data;
            condRes = FALSE;
            CValueArray *values = new CValueArray;
            if ( iniFile.Read( params[0] ) ) {
                CMapStrToString *elements;
                CStr section = params[1];
				section.MakeLower();
                if ( iniFile.Sections.Lookup( section, (void*&)elements ) ) {
                    int mappos = elements->GetStartPosition();
                    CStr  key,value;
                    for ( int i=0; mappos != NULL; i++ ) {
                        elements->GetNextAssoc( mappos, key, value );
                        if ( i == 0 ) {
                            SetVariable( variables[0], key );
                            SetVariable( variables[1], value );
                            condRes = TRUE;
                        } else {
                            values->Add( CValue(key) );
							values->Add( CValue(value) );
                        }
                    }
                }
            }
            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 4 );
        } else if ( type == L"regsubkeys" ) {
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with regValues" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 2 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with regValues" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }

			HKEY root = GetTopKey( params[0] );
			if ( root == NULL ) {
				if ( ErrorLevel >= ERROR_SYNTAX ) {
					MessageBox( GetMsgParent(), L"Invalid root entry" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				}
				return FALSE;
			}

            condRes = FALSE;
            CStrArray *values = new CStrArray;
		    HKEY subkey;
			if ( RegOpenKeyEx( root, (LPCTSTR)params[1], 0, REG_ACCESS_READ, &subkey ) == ERROR_SUCCESS ) {
			    int   idx;
				TCHAR subName[MAX_PATH];
				DWORD subSize = MAX_PATH;

	            for ( idx = 0; RegEnumKeyEx( subkey, idx, subName, &subSize, NULL, NULL, NULL, NULL ) == ERROR_SUCCESS; idx++ ) {
                    if ( idx == 0 ) {
                        SetVariable( variables[0], subName );
                        condRes = TRUE;
                    } else {
                        values->Add( subName );
                    }
					subSize = MAX_PATH;
				}
			}

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 3 );
        } else if ( type == L"regvalues" ) {
            if ( varCount != 2 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with regValues" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 2 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with regValues" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }

			HKEY root = GetTopKey( params[0] );
			if ( root == NULL ) {
				if ( ErrorLevel >= ERROR_SYNTAX ) {
					MessageBox( GetMsgParent(), L"Invalid root entry" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
				}
				return FALSE;
			}

            condRes = FALSE;
            CValueArray *values = new CValueArray();
		    HKEY subkey;
			if ( RegOpenKeyEx( root, (LPCTSTR)params[1], 0, REG_ACCESS_READ, &subkey ) == ERROR_SUCCESS ) {
			    int   idx;
				TCHAR subName[MAX_PATH];
				DWORD subSize = MAX_PATH, type;

				for ( idx = 0; RegEnumValue( subkey, idx, subName, &subSize, NULL, &type, NULL, NULL ) == ERROR_SUCCESS; idx++ ) {
					DWORD type, length;
					BYTE cont[16384];

					length = 16384;
					if ( RegQueryValueEx( subkey, subName, NULL, &type, (BYTE*)cont, &length ) == ERROR_SUCCESS ) {
						CValue value;
						switch ( type )
						{
						case REG_BINARY:
							{
								CStr content, byte;
								for ( DWORD i=0; i < length; i++ ){
									byte.Format( L"%02X", cont[i] );
									content += byte;
								}
								value = content;
							}
							break;

						case REG_DWORD:
							value = (long)*((DWORD*)cont);
							break;

						default:
							value = (LPCTSTR)cont;
						}


						if ( idx == 0 ) {
							SetVariable( variables[0], subName );
							SetVariable( variables[1], value );
							condRes = TRUE;
						} else {
							values->Add( CValue(subName) );
							values->Add( value );
						}
					}

					subSize = MAX_PATH;
				}
			}

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 4 );
        } else if ( type == L"files" || type == L"directories" ) {
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with "+type + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 1 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with "+type + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }

			CValue list, listType;
			CValueArray dirParams;
			dirParams.Add( params[0] );
            if ( type == L"files" )
				listType = 1L;
            else
                listType = 2L;
			dirParams.Add( listType );

			int         error;
			CStr errorMessage;

            list = FctDirContents( dirParams, error, errorMessage );

            condRes = FALSE;
            CStrArray *values = new CStrArray;
			CStr path = params[0];
			path = path.Left( path.ReverseFind('\\')+1 );
			for ( int i = 1; TRUE; i++ ) {
				CStr elemVar;
				CValue *val;
				elemVar.Format( L"%d", i );
				list.GetMap()->Lookup( elemVar, val );
				if ( val != NULL && !val->IsNull() )
					values->Add( path + (CStr)*val );
				else
					break;
			}

            if ( values->GetSize() > 0 ) {
                SetVariable( variables[0], values->GetAt(0) );
                values->RemoveAt(0);
                condRes = TRUE;
            } else {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 2 );

        } else if ( type == L"charsof" ) {
            if ( varCount != 1 ) {
                MessageBox( GetMsgParent(), L"Invalid variables count for ForEach with charsOf" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
                return FALSE;
            }
            CValueArray params;
            if ( Split( stuff, L',', params ) != 1 ) {
                MessageBox( GetMsgParent(), InvalidParameterCount + L"ForEach with charsOf" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }

            condRes = FALSE;
            CStrArray *values = new CStrArray;

            TCHAR value[2];
            value[1] = L'\0';
            for ( int i=0; i < ((CStr)params[0]).GetLength(); i++ ) {
                value[0] = ((CStr)params[0]).GetAt(i);
                values->Add( value );
            }

            if ( values->GetSize() > 0 ) {
                SetVariable( variables[0], values->GetAt(0) );
                values->RemoveAt(0);
                condRes = TRUE;
            } else {
                condRes = FALSE;
            }

            ForEachData.Add( values );
            ForEachVariables.Add( variable );
            ForEachType.Add( 6 );
        } else {
            if ( ErrorLevel >= ERROR_SYNTAX ) {
                CStr msg;
                msg.Format( L"ForEach: invalid loop type" );
                MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
            }
            return FALSE;
        }
    }

    PushCondStack( CT_FOREACH, condRes );
    ForEachPos.Add( CurrentPos );

	// Jump to EndForEach
	if ( condRes != 1 ) {
		long endPos;
		if ( EndForEachPositions.Lookup( CurrentPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}

BOOL CInterpreter::EndForEach( CStr &param )
{
    BOOL rc = TRUE;
    char type; long cond;
    PopCondStack( type, cond, FALSE );
    if ( type != CT_FOREACH ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'EndForEach' without 'ForEach'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    int stackPos = ForEachPos.GetUpperBound();

    if ( cond != 2 ) {
        CStrArray variables;
		CIniFile::Split( ForEachVariables.GetAt(stackPos), L"\n", variables );

        switch( ForEachType.GetAt(stackPos) ) {
        // Data in CValueArray
        case 1:
            {
                CValueArray *data = (CValueArray*)ForEachData.GetAt(stackPos);
                if ( data == NULL || data->GetSize() == 0 ) {
                    if ( data != NULL ) { delete data; data = NULL; }
                    cond = FALSE;
                } else {
                    if ( cond == 1 ) {
                        SetVariable( variables[0], data->GetAt(0) );
                        data->RemoveAt(0);
                    }
                }
            }
            break;

        // Data in CStrArray
        case 2:
        case 3:
        case 5:
        case 6:
            {
                CStrArray *data = (CStrArray*)ForEachData.GetAt(stackPos);
                if ( data == NULL || data->GetSize() == 0 ) {
                    if ( data != NULL ) { delete data; data = NULL; }
                    cond = FALSE;
                } else {
                    if ( cond == 1 ) {
                        SetVariable( variables[0], data->GetAt(0) );
                        data->RemoveAt(0);
                    }
                }
            }
            break;

        // INI/array values
        case 4:
            {
                CValueArray *data = (CValueArray*)ForEachData.GetAt(stackPos);
                if ( data == NULL || data->GetSize() == 0 ) {
                    if ( data != NULL ) { delete data; data = NULL; }
                    cond = FALSE;
                } else {
                    if ( cond == 1 ) {
                        CValue key   = data->GetAt(0);
                        data->RemoveAt(0);
                        CValue value = data->GetAt(0);
                        data->RemoveAt(0);
                        SetVariable( variables[0], key );
                        SetVariable( variables[1], value );
                    }
                }
            }
            break;
        }
    }

    // No more continues...
    if ( cond == 0 ) {
        ForEachData.RemoveAt(stackPos);
        ForEachVariables.RemoveAt(stackPos);
        ForEachType.RemoveAt(stackPos);
    }

    // Back to beginning...
    if ( cond == 1 ) {
        CurrentPos = ForEachPos.GetAt(stackPos);
        if ( cond != 2 )
            CondState = TRUE;
    } else {
        // Remove position and state (also done for cond==2)
        ForEachPos.RemoveAt(stackPos);
        PopCondStack( type, cond );
    }

    return rc;
}
//#endif

//jwz:add for get parameter part abc(a,b,c) will return "(a,b,c)"? maybe.
CStr CInterpreter::GetParam(CStr &Func,TCHAR beg,TCHAR end, BOOL IncludeSep)
{
	int begCnt=0,endCnt=0,begPos=0,endPos=0,i=0;
	CStr tStr = Func;
	for (i=0;i<tStr.GetLength();i++){
		if (tStr[i]==beg){
			if (beg == end && begCnt!=0){
				endPos = i;
				endCnt++;
			}else if (begCnt==0){
				begPos = i;
				begCnt++;
			}else begCnt++;
		}else if (tStr[i]==end)
			endCnt++;

		if (begCnt==endCnt && begCnt!=0){
			Func = tStr.Mid(i+1);

			if (IncludeSep){
				return tStr.Mid(begPos,i-begPos+1);
			}else{
				return tStr.Mid(begPos+1,i-begPos-1);
			}
		}
	}
	return L"";
}

//this function will return the abc(a,b,c)'s  "(a,b,c)" and save "abc" to DefName.
CStr CInterpreter::GetParamDefName(CStr &line,CStr &DefName){

	int pos;
	if ((pos=line.Find(L'('))!=-1){
		DefName=line.Left(pos);
		return GetParam(line,L'(',L')',true);
	}else return L"";
}
//jwz:add end

//jwz:add for user define function/subroutine lookup.
BOOL CInterpreter::IsUserFunc(CStr &line){

	CStr cmd;
	int pos = FindStrNotInQuote(line,L"=",true);
	if (pos!=-1) line = line.Mid(pos+1);
	pos = line.Find(L'(');
	if (pos>0){
		cmd = line.Left(pos);
	}else cmd = line;

	cmd.MakeUpper();
	cmd.TrimLeft();cmd.TrimRight();

	void *newPos;
	if (Subs.Lookup( cmd, newPos ) ==TRUE) return true;//jwz::add

	return false;
}
//jwz:add end

//jwz:add for Fetch a script source line, and skip empty line.
CStr CInterpreter::GetLine(){

	CStr line,PrevLine=L"";
	long pos;
	
again:
	PreviousPos = CurrentPos;
	
	pos = Content.Find( _T("\n"), PreviousPos );
	if ( pos != -1 ) {
		line = Content.Mid( PreviousPos, pos-PreviousPos );
	} else {
		line = Content.Mid( PreviousPos );
	}
    line.TrimLeft();
	line.TrimRight();
    while ( line.Right(1) == L"\\" && pos != -1 ) {
        line = line.Left( line.GetLength() - 1 );
        line.TrimRight();
        int oldPos = pos+1;
        pos = Content.Find( _T("\n"), oldPos );
        if ( pos != -1 ) {
            CStr addLine = Content.Mid( oldPos, pos-oldPos );
            addLine.TrimLeft(); addLine.TrimRight();
            line += L" " + addLine;
        }
    }
	if ( pos == -1 )
		CurrentPos = -1;
	else
		CurrentPos = pos+1;

	line.TrimLeft();
	line.TrimRight();
	
	if (CurrentPos != -1 && line.IsEmpty()) {
		goto again;
	}

	//WriteToLog((LPTSTR)(LPCTSTR)line);
	return line;
}

CStr CInterpreter::FetchLine(){
	CStr line = GetLine();
	Comment(line);
	return line;
}

//jwz:add end

BOOL CInterpreter::Repeat( CStr &param)
{
	if ( CondState == 1 ) {
		CValueArray params;
		if ( Split( param, L',', params ) != 1 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), InvalidParameterCount + L"'Repeat'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			return FALSE;
		}

		RepeatCount.Add( (long)params[0] );
	} else {
		RepeatCount.Add( 0 );
	}
	RepeatPos.Add( CurrentPos );
	PushCondStack(CT_REPEAT,true);

	//jwz:add for save count variable to $_
	*_Statement_Result = (long)RepeatCount[RepeatCount.GetUpperBound()];
	//jwz:add end

    return TRUE;
}

BOOL CInterpreter::EndRepeat( CStr &param )
{
    BOOL rc = TRUE;

	char type; long state;
	PopCondStack(type, state);
	if (type != CT_REPEAT){
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'EndRepeat' without 'Repeat'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
	}

    int stackPos = RepeatPos.GetUpperBound();
    if ( stackPos == -1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"'EndRepeat' without 'Repeat'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

	if (state){
		RepeatCount[stackPos]--;
		//jwz:add for save repeat(6) count variable to $_
		*_Statement_Result = (long)RepeatCount[stackPos];
		//jwz:add end
		if ( RepeatCount[stackPos] <= 0 ) {
			RepeatCount.RemoveAt(stackPos);
			RepeatPos.RemoveAt(stackPos);
		} else {
			CurrentPos = RepeatPos.GetAt(stackPos);
			PushCondStack(type,state);
		}
	}else{
		RepeatCount.RemoveAt(stackPos);
		RepeatPos.RemoveAt(stackPos);
	}

    return rc;
}


BOOL CInterpreter::Switch( CStr &param )
{
    if ( CondState != 1 )
        PushCondStack( CT_SWITCH, 2 );
    else {
		CValueArray params;
		if ( Split( param, L',', params ) != 1 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), InvalidParameterCount + L"'Switch'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			return FALSE;
		}

        ChoiceStack.Add( params[0] );
        PushCondStack( CT_SWITCH, 0 );
    }
	SwitchPos.Add(PreviousPos);

    return TRUE;
}


BOOL CInterpreter::Choice( CStr &param )
{
    if ( CondState != 1 )
        PushCondStack( CT_CHOICE, 2 );
    else {
		CValueArray params;
		if ( Split( param, L',', params, TRUE ) < 3 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), InvalidParameterCount + L"'Choice'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			return FALSE;
		}

        CDlgChoice choiceDlg;
        choiceDlg.m_Title = (CStr)params[0];
        choiceDlg.m_Info  = (CStr)params[1];

		CValue *mapValue = NULL;
		if ( params.GetSize() == 3 ) {
			switch( params[2].GetType() ) {
			case VALUE_STRING:
				mapValue = GetVariable( params[2], FALSE );
				break;

			case VALUE_MAP:
				mapValue = &params[2];
				break;
			}
		}

		if ( mapValue != NULL && mapValue->GetType() == VALUE_MAP ) {
			CMapStrToValue *map = mapValue->GetMap();
			CStr elem; CValue *cont;

			for ( int i=1; i<50; i++ ) {
				elem.Format( L"%d", i );
				map->Lookup( elem, cont );
				if ( cont != NULL && !cont->IsNull() )
					choiceDlg.m_Strings.Add( *cont );
				else
					break;
			}
		} else {
			for ( int i=2; i<params.GetSize(); i++ ) {
				choiceDlg.m_Strings.Add( params[i] );
			}
		}

		if ( choiceDlg.m_Strings.GetSize() > 0 ) {
	        choiceDlg.DoModal();
	        ChoiceStack.Add( CValue((long)choiceDlg.m_Selected) );
		} else {
			ChoiceStack.Add( CValue(-1L) );
		}
        PushCondStack( CT_CHOICE, 0 );
    }

    return TRUE;
}

BOOL CInterpreter::ChoiceDefault( CStr &param )
{
    if ( CondState != 1 )
        PushCondStack( CT_CHOICE, 2 );
    else {
		CValueArray params;
		if ( Split( param, L',', params ) < 5 ) {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), InvalidParameterCount + L"'ChoiceDefault'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			return FALSE;
		}

        CDlgChoice choiceDlg;
        choiceDlg.m_Title   = (CStr)params[0];
        choiceDlg.m_Info    = (CStr)params[1];
        choiceDlg.m_Default = (long)params[2]-1;
        choiceDlg.m_Timeout = (long)params[3];

		CValue *mapValue = NULL;
		if ( params.GetSize() == 5 ) {
			switch( params[4].GetType() ) {
			case VALUE_STRING:
				mapValue = GetVariable( params[4], FALSE );
				break;

			case VALUE_MAP:
				mapValue = &params[4];
				break;
			}
		}

		if ( mapValue != NULL && mapValue->GetType() == VALUE_MAP ) {
			CMapStrToValue *map = mapValue->GetMap();
			CStr elem; CValue *cont;

			for ( int i=1; i<50; i++ ) {
				elem.Format( L"%d", i );
				map->Lookup( elem, cont );
				if ( cont != NULL && !cont->IsNull() )
					choiceDlg.m_Strings.Add( *cont );
				else
					break;
			}
		} else {
			for ( int i=4; i<params.GetSize(); i++ ) {
				choiceDlg.m_Strings.Add( params[i] );
			}
		}

        choiceDlg.DoModal();

        ChoiceStack.Add( CValue( (long)choiceDlg.m_Selected ) );
        PushCondStack( CT_CHOICE, 0 );
    }

    return TRUE;
}

//jwz:modi for support break, cond = 3 for break.
BOOL CInterpreter::Case( CStr &param )
{
    char type; long cond;
    PopCondStack( type, cond, FALSE );
    if ( type != CT_SWITCH && type != CT_CHOICE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Case without Choice/Switch" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    if ( cond < 2 ) {
        CValueArray params;
        if ( Split( param, L',', params ) < 1 ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), InvalidParameterCount + L"'Case'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
            return FALSE;
        }
		CondState = cond;


		if (!cond){
			PopCondStack( type, cond );

			cond = FALSE;
			CValue value = ChoiceStack.GetAt( ChoiceStack.GetSize()-1 );
			for ( int i=0; i<params.GetSize(); i++ ) {
				switch( params[i].GetType() ) {
				case VALUE_CHAR:
				case VALUE_LONG:
					cond = ( (long)params[i] == (long)value );
					break;
				case VALUE_DOUBLE:
					cond = ( (double)params[i] == (double)value );
					break;
				case VALUE_STRING:
					cond = ( (CStr)params[i] == (CStr)value );
					break;
				}
				if ( cond ) break;
			}

	        PushCondStack( type, cond );
		}
    }else if (cond == 3) CondState = 0;

	// Jump to next Case/EndSwitch/EndChoice
	if ( CondState != 1 ) {
		long endPos;
		if ( EndSwitchPositions.Lookup( PreviousPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}


BOOL CInterpreter::Default()
{
    char type; long cond;
    PopCondStack( type, cond, FALSE );
    if ( type != CT_SWITCH && type != CT_CHOICE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Default without Choice/Switch" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    if ( cond < 2 ) {
		CondState = cond;
		if (cond == 0){
			PopCondStack( type, cond );

			cond = TRUE;

			PushCondStack( type, cond );
		}
    }else if (cond == 3) CondState = 0;

	// Jump to next Case/EndSwitch/EndChoice
	if ( CondState != 1 ) {
		long endPos;
		if ( EndSwitchPositions.Lookup( PreviousPos, endPos ) )
			CurrentPos = endPos;
	}

    return TRUE;
}

BOOL CInterpreter::EndChoice()
{
    char type; long cond;
    PopCondStack( type, cond );
    if ( type != CT_SWITCH && type != CT_CHOICE ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"EndChoice/EndSwitch without Choice/Switch" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    if ( cond != 2 ) {
        if ( ChoiceStack.GetSize() == 0 ) {
            if ( ErrorLevel >= ERROR_SYNTAX )
                MessageBox( GetMsgParent(), L"EndChoice without Choice" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
            return FALSE;
        }
        ChoiceStack.RemoveAt( ChoiceStack.GetSize()-1 );
    }

    return TRUE;
}

BOOL CInterpreter::EndSub()
{
    int stackPos = SubStack.GetUpperBound();
    if ( stackPos > -1 ) {
        CurrentPos = SubStack.GetAt( stackPos );

        SubStack.RemoveAt( stackPos );
		
		int lvIdx = LocalVariablesStack.GetSize()-1;
		
		CValue *resultVar = (CValue*)SubResultStack.GetAt( lvIdx );

		if (resultVar != NULL) resultVar->CopyFrom(*_Statement_Result);

		SubResultStack.RemoveAt( lvIdx );

		delete LocalVariables;
		LocalVariables = NULL;
		LocalVariables = (CMapStrToValue*)LocalVariablesStack.GetAt( lvIdx-1 );
		LocalVariablesStack.RemoveAt( lvIdx );
        return TRUE;
    } else {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"EndSub without Sub" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }
}

//jwz:modi from sub?
BOOL CInterpreter::Def(CStr &param)
{
	CStr FuncName;
	GetParamDefName(param,FuncName); //jwz:add
	FuncName.MakeUpper();
	void *newPos;
	if (EndPosOfSubs.Lookup( FuncName, newPos ) == FALSE)
    {
        CStr msg;
        msg.Format( L"End of Sub/Def '%s' not found", (LPCTSTR)FuncName );
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
        return FALSE;
	}
	CurrentPos = (long)newPos;
	return TRUE;	
}
//jwz:modi end

//jwz:add for simple control loop structure terminal and make script file looks like ruby
BOOL CInterpreter::End(CStr &param)
{
	char type; long start;
    PopCondStack( type, start, FALSE );

	switch(type){
	case CT_IF:
		return EndIf();
		break;
	case CT_WHILE:
		return EndWhile( param);
		break;
	case CT_SWITCH:
	case CT_CHOICE:
		return EndChoice();
		break;
	case CT_FOR:
		return Next( param);
		break;
	case CT_REPEAT:
		return EndRepeat( param);
		break;
	case CT_FOREACH:
		return EndForEach( param);
		break;
	default:
		return EndSub();
	}
}
//jwz:add end

BOOL CInterpreter::SetErrorLevel( CStr &param )
{
    BOOL rc = TRUE;
    CValueArray params;
    if ( Split( param, L',', params ) != 1 ) {
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ErrorLevel'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

	CStr par = params[0];
	if ( par.CompareNoCase( L"off" ) == 0 ) {
        ErrorLevel = ERROR_OFF;
    } else if ( par.CompareNoCase( L"critical" ) == 0 ) {
        ErrorLevel = ERROR_CRITICAL;
    } else if ( par.CompareNoCase( L"syntax" ) == 0 ) {
        ErrorLevel = ERROR_SYNTAX;
    } else if ( par.CompareNoCase( L"error" ) == 0 ) {
        ErrorLevel = ERROR_ERROR;
    } else if ( par.CompareNoCase( L"warn" ) == 0 ) {
        ErrorLevel = ERROR_WARN;
    } else if ( par.CompareNoCase( L"debug" ) == 0 ) {
        ErrorLevel = ERROR_DEBUG;
    } else {
        MessageBox( GetMsgParent(), L"Invalid ErrorLevel" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
        return FALSE;
    }

    return rc;
}

//jwz:add
BOOL CInterpreter::Return( CStr &expr )//jwz:add return statement.
{
	char type; long state;

	if (!expr.IsEmpty()) {
		*_Statement_Result = EvaluateExpression( expr );
	}

	PopCondStack(type,state,false);
	while(type != CT_SUB && type != 0){
		PopCondStack(type, state);
		PopCondStack(type,state,false);
	}

	if (type == 0){
		if ( ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), L"Endsub/End can't found!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
		exit(1);
	}
	EndSub();

	return TRUE;
}

BOOL CInterpreter::IsSingleIf(CStr &line, CStr & ReminderStr){
	//a==1 then a=2 else a=3 || (a==1) a=2 else a=3
	//line = (a==1); ReminderStr = a=2 else a=3
	line.TrimLeft();line.TrimRight();
	CStr tStr = line;
	tStr.MakeUpper();
	int pos = tStr.Find(L"THEN");
	if ( pos != -1 ){
		if (pos + 4 != tStr.GetLength()){
			ReminderStr = line.Mid(pos + 4);
			line = line.Left(pos);
			line.TrimLeft();line.TrimRight();
			line = L"(" + line + L")";
			ReminderStr.TrimLeft();ReminderStr.TrimRight();
			return true;
		}else{
			line = line.Left(pos);
		}
	}else{
		if (line.StartsWith(L"IF"))
			line = line.Mid(2);
		line.TrimLeft();

		if (line.StartsWith(L"(")){
			pos = GetParamSize(line);
			ReminderStr = line.Mid(pos+1);
			line = line.Left(pos+1);
			line.TrimLeft();line.TrimRight();
			ReminderStr.TrimLeft();ReminderStr.TrimRight();
			if (!ReminderStr.IsEmpty()) 
				return true;
		}
	}
	line.TrimLeft();line.TrimRight();
	line = L"(" + line + L")";
	ReminderStr = L"";

	return false;
}

BOOL CInterpreter::IsSingleElse(CStr &line, CStr & ReminderStr){
	//if a==1 then a=2 else a=3
	line.TrimLeft();line.TrimRight();
	CStr tStr = line;
	tStr.MakeUpper();
	int pos = tStr.Find(L" ELSEIF");
	if ( pos != -1) {
		ReminderStr = line.Mid(pos + 5);
		line = line.Left(pos);
		ReminderStr.TrimLeft();ReminderStr.TrimRight();
		line.TrimLeft();line.TrimRight();
		return true;
	}else{
		pos = tStr.Find(L" ELSE ");
		if (pos != -1){
			ReminderStr = line.Mid(pos + 6);
			line = line.Left(pos);
			ReminderStr.TrimLeft();ReminderStr.TrimRight();
			line.TrimLeft();line.TrimRight();
			return true;
		}else{
			ReminderStr.Empty();
		}
	}
	return false;
}
//jwz:add end

void CInterpreter::RunFile( LPCTSTR filename , bool isCmdRun )
{
	char   *buffer = NULL;
	BOOL DefaultIsSet = FALSE;

	CStr cmd;
	CStr param;

	CStr function; //Use for EndPosOfSubs & Subs Stack!

    UINT startPos = 0, pos = 0;
	long sepPos; // , spacePos, tabPos, parPos
	CStr line;

    if ( isCmdRun || CIniFile::ReadFile( filename, Content ) == 0 ) {
		CurrentInterpreter = this;
		ScriptFile = filename;
		ScriptFile.Replace( L"/", L"\\" );

#ifndef DESKTOP
		if ( ScriptFile.GetLength() >= 1 && ScriptFile.GetAt(0) != L'\\' )
			ScriptFile = L"\\" + ScriptFile;
#else
		if ( ScriptFile.GetLength() >= 2 && ScriptFile.GetAt(1) != L':' ) {
			TCHAR currDir[MAX_PATH];
			GetCurrentDirectory( MAX_PATH, currDir );
			if ( ScriptFile.GetAt(0) == L'\\' ) {
				currDir[2] = L'\0';
				ScriptFile = currDir + ScriptFile;
			} else {
				ScriptFile = CStr(currDir) + L"\\" + ScriptFile;
			}
		}
#endif
		if (isCmdRun) Content = filename;

		Content += L"\n";

ReStart:
		CondState = TRUE;
		Subs.RemoveAll();
		EndPosOfSubs.RemoveAll();

		CondStack.RemoveAll();
    
		RepeatCount.RemoveAll();
		RepeatPos.RemoveAll();
    
		WhilePos.RemoveAll();
		ChoiceStack.RemoveAll();
    
		SubStack.RemoveAll();

		SwitchPos.RemoveAll();

		ForPos.RemoveAll();
		ForVariable.RemoveAll(); // Pointer to CValue
		ForLimit.RemoveAll();
		ForStep.RemoveAll();

		ForEachVariables.RemoveAll();
		ForEachData.RemoveAll();
		ForEachType.RemoveAll();
		ForEachPos.RemoveAll();
		
		EndRepeatPositions.RemoveAll();
		
		EndIfPositions.RemoveAll();
		EndForPositions.RemoveAll();
		EndForEachPositions.RemoveAll();
		EndWhilePositions.RemoveAll();
		EndSwitchPositions.RemoveAll();

		CurrentPos = 0;
		ContentLength = Content.GetLength();

		// Vor-Parsen f黵 Sprungadressen von SUB und Schleifen
		while ( CurrentPos != -1 && CurrentPos < ContentLength && Cont == TRUE ) {
			line = FetchLine();

			line.TrimLeft();line.TrimRight();

			if ( !line.IsEmpty() ) {
                sepPos = line.FindOneOf( L"( \t" );

                if ( sepPos != -1 ) {
    				cmd   = line.Left(sepPos);
					param = line.Mid( line[sepPos]==L'(' ? sepPos : sepPos+1 );
					param.TrimLeft();
				} else {
					cmd   = line;
					param.Empty();
				}

				if ( ! param.StartsWith( L"=" ) ) {
	                ErrorLine = line;
					ErrorPos  = PreviousPos;

					cmd.MakeUpper();
					if (cmd == L"#INCLUDE") {
						CValueArray params;
						if ( Split( param, L',', params, 1, 1 ) != 1 ) {
							if ( ErrorLevel >= ERROR_SYNTAX ) {
								MessageBox( GetMsgParent(), InvalidParameterCount + L"'Include'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
								Cont = 0;
							}
						} else {
							CStr incContent;
							CStr script = params[0];
							if (script.StartsWith(L"\"") && script.EndsWith(L"\"")){ //"1.mscr"->1.mscr
								script.TrimLeft(L"\"");
								script.TrimRight(L"\"");
							}
							
							if (  script.GetLength() < 2 || ( script.GetAt(0) != L'\\' && script.GetAt(1) != L':' ) ) {
								int len = ScriptFile.ReverseFind('\\');
								if ( len == -1 )
									script = L"\\" + script;
								else
									script = ScriptFile.Left( len+1 ) + script;
							}
							CStr searchString; int pos;
							searchString = script; searchString.MakeLower();
							if ( IncludedFiles.FindEntry( searchString, pos ) == FALSE ) {
								if ( pos == -1 )
									IncludedFiles.Add( searchString );
								else
									IncludedFiles.InsertAt( pos, searchString );

								if ( CIniFile::ReadFile( script, incContent ) >= 0 ) {
									Content.Replace(line,incContent);
									goto ReStart;
								} else {
									MessageBox( GetMsgParent(), L"File '" + script + L"' not found for 'Include'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
									Cont = 0;
								}
							}
						}
					}
					else if ( cmd == L"SUB" 
			   /*jwz:add*/ || cmd == L"DEF" /*jwz:addd-end*/ 
							)
					{
						PushCondStack( CT_SUB, PreviousPos );
						// Save SUB position
						GetParamDefName(param,function); //jwz:add (取得函数名)

						function.MakeUpper();
						void *dummy;
						if ( Subs.Lookup( function, dummy ) ) {
							MessageBox( GetMsgParent(), L"Sub " + param + L" was already defined" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
							Cont = 0;
						} else
							//Subs.SetAt( function, (void*)pos );//jwz:modi
							Subs.SetAt( function, (void*)PreviousPos );
					} else if ( cmd == L"IF" ) {
						CStr ReminderStr;
						if (!IsSingleIf(param,ReminderStr))
							PushCondStack( CT_IF, PreviousPos );
					} else if ( cmd == L"FOR" ) {
						PushCondStack( CT_FOR, CurrentPos );
					} else if ( cmd == L"REPEAT" ) {
						PushCondStack( CT_REPEAT, CurrentPos );
					} else if ( cmd == L"FOREACH" ) {
						PushCondStack( CT_FOREACH, CurrentPos );
					} else if ( cmd == L"WHILE" ) {
						PushCondStack( CT_WHILE, PreviousPos );
					} else if ( cmd == L"SWITCH" ) {
						PushCondStack( CT_SWITCH, PreviousPos );
					} else if ( cmd == L"CHOICE" || cmd == L"CHOICEDEFAULT" ) {
						PushCondStack( CT_CHOICE, PreviousPos );
					} else if (   cmd == L"NEXT"
						     || cmd == L"ENDWHILE"
							 || cmd == L"ENDFOR"
							 || cmd == L"ENDREPEAT"
							 || cmd == L"ENDFOREACH"
							 || cmd == L"ELSE"
							 || cmd == L"ELSEIF"
							 || cmd == L"ENDIF"
							 || cmd == L"CASE"
							 || cmd == L"DEFAULT"
							 || cmd == L"ENDSWITCH"
							 || cmd == L"ENDCHOICE"
							 || cmd == L"ENDSUB"
				 /*jwz:add*/ || cmd == L"END" /*jwz:addd-end*/
							)
					{
						char type; long start;
						PopCondStack( type, start, TRUE );
						CStr expected;
						if ( type != 0 ) {
							expected = L"Expected '";
							switch ( type ) {
							case CT_IF:
								expected += L"EndIf";
								break;
							case CT_FOR:
								expected += L"Next";
								break;
							case CT_REPEAT:
								expected += L"EndRepeat";
								break;
							case CT_FOREACH:
								expected += L"EndForEach";
								break;
							case CT_WHILE:
								expected += L"EndWhile";
								break;
							case CT_SWITCH:
								expected += L"EndSwitch";
								break;
							case CT_CHOICE:
								expected += L"EndChoice";
								break;
							case CT_SUB:
								expected += L"EndSub";
								break;
							}
							expected += L"' but found ";
						}
						//========jwz:add=============
						if ( cmd == L"END" ) {
							if (   type != CT_IF
								&& type != CT_FOR
								&& type != CT_REPEAT
								&& type != CT_FOREACH
								&& type != CT_WHILE
								&& type != CT_SWITCH
								&& type != CT_CHOICE
								&& type != CT_SUB
								)
							{
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'End' without One of 'If / For / Repeat / ForEach / While / Switch / Choice / Sub / Def'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'End'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							switch( type ) {
							case CT_IF:
								EndIfPositions.SetAt( start, PreviousPos ); //currStart
								break;
							case CT_FOR:
								EndForPositions.SetAt( start, PreviousPos );
								break;
							case CT_REPEAT:
								EndRepeatPositions.SetAt( start, PreviousPos );
								break;
							case CT_FOREACH:
								EndForEachPositions.SetAt( start, PreviousPos );
								break;
							case CT_WHILE:
								EndWhilePositions.SetAt( start, PreviousPos );
								break;
							case CT_SWITCH:
							case CT_CHOICE:
								EndSwitchPositions.SetAt( start, PreviousPos );
								break;
							case CT_SUB:
								EndPosOfSubs.SetAt( function, (void*)CurrentPos );
								break;
							}
						}
						//===============jwz:add:end=============

						if ( cmd == L"ENDIF" ) {
							if ( type != CT_IF ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndIf' without 'If'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndIf'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndIfPositions.SetAt( start, PreviousPos );
						} else if ( cmd == L"NEXT" ) {
							if ( type != CT_FOR ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'Next' without 'For'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'Next'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndForPositions.SetAt( start, PreviousPos );
						} else if ( cmd == L"ENDFOREACH"	) {
							if ( type != CT_FOREACH ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndForEach' without 'ForEach'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndForEach'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndForEachPositions.SetAt( start, PreviousPos );
						} else if ( cmd == L"ENDWHILE" ) {
							if ( type != CT_WHILE ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndWhile' without 'While'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndWhile'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndWhilePositions.SetAt( start, PreviousPos );
						} else if ( cmd == L"ENDSUB" ) {
							if ( type != CT_SUB ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndSub' without 'Sub'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndSub'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndPosOfSubs.SetAt( function, (void*)CurrentPos );
						} else if ( cmd == L"ELSE" 
							   || cmd == L"ELSEIF" 
							   || cmd == L"ENDIF" 
							   )
						{
							if ( type != CT_IF ) {
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), CStr(L"'") + cmd + CStr(L"' without 'If'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'") + cmd + L"'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndIfPositions.SetAt( start, PreviousPos );
							// Only EndIf really ends the structure, otherwise, we jump from If->ElseIf->Else->EndIf
							if ( cmd == L"ELSE" || cmd == L"ELSEIF" )
								PushCondStack( CT_IF, PreviousPos );
						} else if ( cmd == L"CASE" 
							|| cmd == L"ENDSWITCH" 
							|| cmd == L"DEFAULT" 
							|| cmd == L"ENDCHOICE"
							)
						{
							if ( cmd == L"ENDSWITCH" 
								&& type != CT_SWITCH )
							{
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndSwitch' without 'Switch'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndSwitch'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							if ( cmd == L"ENDCHOICE" 
								&& type != CT_CHOICE )
							{
								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'EndChoice' without 'Choice(Default)'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'EndChoice'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							if ( cmd == L"CASE" ){
								if ( type != CT_SWITCH 
								&& type != CT_CHOICE) 
								{
									if ( ErrorLevel >= ERROR_SYNTAX ) {
										if ( expected.IsEmpty() )
											MessageBox( GetMsgParent(), L"'Case' without 'Choice(Default)' or 'Switch'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
										else
											MessageBox( GetMsgParent(), expected + CStr(L"'Case'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									}
									Cont = FALSE;
									break;
								}else if (DefaultIsSet){
									if ( ErrorLevel >= ERROR_SYNTAX ) {
										if ( expected.IsEmpty() )
											MessageBox( GetMsgParent(), L"'Case' behind 'Default' is not allowed!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
										else
											MessageBox( GetMsgParent(), expected + CStr(L"'Case'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									}
									Cont = FALSE;
									break;
								}
							}

							if ( cmd == L"DEFAULT" 
								&& type != CT_SWITCH 
								&& type != CT_CHOICE ){

								if ( ErrorLevel >= ERROR_SYNTAX ) {
									if ( expected.IsEmpty() )
										MessageBox( GetMsgParent(), L"'Default' without 'Choice(Default)' or 'Switch'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
									else
										MessageBox( GetMsgParent(), expected + CStr(L"'Default'") + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
								}
								Cont = FALSE;
								break;
							}
							EndSwitchPositions.SetAt( start, PreviousPos );
							// Only EndSwitch/-Choice really ends the structure, otherwise, we jump from Case to Case until EndSwitch or EndCase is reached
							if ( cmd == L"CASE" || cmd == L"DEFAULT"){
								if (cmd == L"DEFAULT")DefaultIsSet = TRUE;
								PushCondStack( type, PreviousPos );
							}
						}
					}
				}
            }
        }

		if ( Cont ) {
			char type; long start;
			PopCondStack( type, start, FALSE );
			if ( type != 0 ) {
				if ( ErrorLevel >= ERROR_SYNTAX ) {
					pos = Content.Find( _T("\n"), start );
					if ( pos != -1 )
						line = Content.Mid( start, pos-start );
					else
						line = Content.Mid( start );

					while ( line.Right(1) == L"\\" && pos != -1 ) {
						line = line.Left( line.GetLength() - 1 );
						line.TrimRight();
						int oldPos = pos+1;
        				pos = Content.Find( _T("\n"), oldPos );
						if ( pos != -1 ) {
							CStr addLine = Content.Mid( oldPos, pos-oldPos );
							addLine.TrimLeft(); addLine.TrimRight();
							line += L" " + addLine;
						}
					}

	                ErrorLine = line;
					ErrorPos  = pos;

					switch ( type ) {
					case CT_IF:
						MessageBox( GetMsgParent(), L"'If' without 'EndIf'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_FOR:
						MessageBox( GetMsgParent(), L"'For' without 'Next'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_FOREACH:
						MessageBox( GetMsgParent(), L"'ForEach' without 'EndForEach'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_WHILE:
						MessageBox( GetMsgParent(), L"'While' without 'EndWhile'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_SWITCH:
						MessageBox( GetMsgParent(), L"'Switch' without 'EndSwitch'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_CHOICE:
						MessageBox( GetMsgParent(), L"'Choice' without 'EndChoice'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;

					case CT_SUB:
						MessageBox( GetMsgParent(), L"'Sub' without 'EndSub'" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
						break;
					}
				}

				Cont = FALSE;
			}
		}

	}
	else
	{
        CStr msg;
        msg.Format( L"Script file not found: '%s'", (LPCTSTR)filename );
        MessageBox( GetMsgParent(), msg, L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND );
	}
}

//jwz:add C language like statement break
BOOL CInterpreter::Break(){
	char type; long state;
	PopCondStack(type,state,false);
	if (type == CT_IF){
		PopCondStack(type,state);
		PopCondStack(type,state,false);
	}
	int stackPos;
	long currPos, endPos;

	switch(type){
	case CT_FOR: 
		PopCondStack(type,state);
		PushCondStack(type,false);
		stackPos = ForPos.GetUpperBound();
		currPos = ForPos.GetAt(stackPos);
		if ( EndForPositions.Lookup( currPos, endPos ) )
			CurrentPos = endPos;
		break;
	case CT_WHILE:
		PopCondStack(type,state);
		PushCondStack(type,false);
		stackPos = WhilePos.GetUpperBound();
		currPos = WhilePos.GetAt(stackPos);
		if (EndWhilePositions.Lookup(currPos, endPos))
			CurrentPos = endPos;
		break;
	case CT_FOREACH:
		PopCondStack(type,state);
		PushCondStack(type,false);
		stackPos = ForEachPos.GetUpperBound();
		currPos = ForEachPos.GetAt(stackPos);
		if (EndForEachPositions.Lookup(currPos, endPos))
			CurrentPos = endPos;
		break;
	case CT_REPEAT:
		PopCondStack(type,state);
		PushCondStack(type,false);
		stackPos = RepeatPos.GetUpperBound();
		currPos = RepeatPos.GetAt(stackPos);
		if (EndRepeatPositions.Lookup(currPos, endPos))
			CurrentPos = endPos;
		break;
	case CT_SWITCH:
	case CT_CHOICE:
		PopCondStack(type,state);
		PushCondStack(type,3);
		break;
	default:
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Break statement only use in Loop control structue/Switch/Choice!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
		return false;
	}
	return true;
}

//jwz:add C language like statement continue
BOOL CInterpreter::Continue(){
	char type; long state;
	PopCondStack(type,state,false);
	if (type == CT_IF){
		PopCondStack(type,state);
		PopCondStack(type,state,false);
	}
	int stackPos;
	long currPos,endPos;
	switch(type){
	case CT_FOR: 
		PopCondStack(type,state);
		PushCondStack(type,true);
		stackPos = ForPos.GetUpperBound();
		currPos = ForPos.GetAt(stackPos);
		if ( EndForPositions.Lookup( currPos, endPos ) )
			CurrentPos = endPos;
		break;
	case CT_REPEAT: 
		PopCondStack(type,state);
		PushCondStack(type,true);
		stackPos = RepeatPos.GetUpperBound();
		currPos = RepeatPos.GetAt(stackPos);
		if (EndRepeatPositions.Lookup(currPos,endPos))
			CurrentPos = endPos;
		break;
	case CT_FOREACH: 
		PopCondStack(type,state);
		PushCondStack(type,true);
		stackPos = ForEachPos.GetUpperBound();
		currPos = ForEachPos.GetAt(stackPos);
		if ( EndForEachPositions.Lookup( currPos, endPos ) )
			CurrentPos = endPos;
		break;
	case CT_WHILE:
		PopCondStack(type,state);
		PushCondStack(type,true);
		stackPos = WhilePos.GetUpperBound();
		currPos = WhilePos.GetAt(stackPos);
		if ( EndWhilePositions.Lookup( currPos, endPos ) )
			CurrentPos = endPos;
		break;
	default:
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Continue statement only use in Loop control structure!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
		return false;
	}
	return true;
}

//jwz:add modify from RunFile?
void CInterpreter::Parser( BOOL localVarsExist, CStr Statement )
{
 	if ( localVarsExist == FALSE ) {
		LocalVariables = NULL;
		LocalVariablesStack.Add( NULL );
		SubResultStack.Add( NULL );
	}

	CurrentPos = 0; PreviousPos = 0;
	CStr line, cmd, param;

	while ( CurrentPos != -1 && CurrentPos < ContentLength && Cont == TRUE ) {
        Cont = 2;

		line = SlashBackup(FetchLine());
		
		if ( !line.IsEmpty()) {
            ErrorLine = line;
			ErrorPos  = PreviousPos;
			Cont = Eval(line);
		} else { 
			// Empty lines and comments are OK (no "Unknown command") 
			Cont = TRUE; 
		}
		
		if ( Cont == 2 ) { 
			if ( ErrorLevel >= ERROR_SYNTAX ) { 
				CStr msg; 
				msg.Format( L"Unknown command '%s'", (LPCTSTR)cmd ); 
				MessageBox( GetMsgParent(), msg + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND |MB_SETFOREGROUND ); 
			} 
			Cont = FALSE; 
		} 
		
		if ( Cont == 1 ) { 
			if ( RegReadDW( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", MutexName ) == 1 ) { 
				if (::GetLastError() != ERROR_ALREADY_EXISTS) { 
					HKEY key; 
					if ( RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\JScripts\\Processes", 0, REG_ACCESS_WRITE, &key ) == ERROR_SUCCESS ) { 
						RegDeleteValue( key, MutexName ); 
						RegCloseKey( key ); 
					} 
					Cont = 0; 
				} 
			} 
		}
	}
}

//jwz:add for Get Parameter and return size. (a,(b,c))?
long CInterpreter::GetParamSize(CStr line, long begin){
	long pos = begin, BracketCnt = 0;
	if (line.IsEmpty()) line = Expression;
	for(int i=begin;i<line.GetLength();i++){
		if (line[i] == L'(') BracketCnt ++;
		if (line[i] == L')') {
			if (BracketCnt>1)
				BracketCnt --;
			else{
				pos = i;
				break;
			}
		}
	}
	return pos;
}
//jwz:add function for Parse a function call
CValue CInterpreter::EvalFunc(CStr FuncName, CValueArray &params){
	CValue rc;	CValueArray DefParams;
	CStr line, DefName, param, cmd,bExpression = Expression;
	long bPosition = Position,bCurrentPos = CurrentPos, bPreviousPos = PreviousPos, bExprLen = ExprLen ;

	FuncName.MakeUpper();
	void * newPos;
	if (Subs.Lookup(FuncName,newPos)){	//find function declaration
		CurrentPos = (long)newPos;
		line = FetchLine();
		line.TrimLeft();line.TrimRight();
		line = line.Mid(4);
		param = GetParam(line);
		int num = Split(param,L',',DefParams);
		LocalVariables = CreateAndInitLocalVariables();
		LocalVariablesStack.Add(LocalVariables);
		LocalVariables->SetAt(L"%AUTOLOCAL",CValue(1L));
		CStr tVar;
		CValue tVal;
		for (int i = 0; i< num; i++){
			tVar = (CStr)DefParams[i];
			tVal = (CValue)params[i];
			SetVariable(tVar, tVal);
		}

		PushCondStack(CT_SUB,PreviousPos);
		SubStack.Add(CurrentPos);
		while(CurrentPos<ContentLength){
			line = FetchLine();
			CondState = 1;
			if (line.CompareNoCase(L"end")==0 || line.CompareNoCase(L"endsub")==0 ) {
				char type; long state;
				PopCondStack(type,state,false);
				if (type == CT_SUB){
					Eval(line);
					rc = *_Statement_Result;
					break;
				}
			}else if ((line.Left(6)).CompareNoCase(L"return")==0){
				Eval(line);
				rc = *_Statement_Result;
				break;
			}
			Eval(line);
		}
		char type; long state;
		PopCondStack(type,state);
		PopCondStack(type,state,false);
		Expression = bExpression;

		Position = bPosition;
		PreviousPos = bPreviousPos;
		CurrentPos = bCurrentPos;
		ExprLen = bExprLen;
	}else{
        if ( ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"User define function can't found! please check it!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
		exit(1);
	}
	return rc;
}

int CInterpreter::FindStrNotInQuote(CStr &line, CStr sch, CStr NotFollowStr,bool isAssign){
	int length = line.GetLength();
	bool inQuote =false;
	for (int i=0;i<length-sch.GetLength();i++){
		if (!inQuote && line[i]==L'"'){
			if (isAssign) return -1;
			inQuote = true;
		}else if (inQuote && line[i]== L'"') 
			inQuote = false;
		else if (!inQuote && (line.Mid( i, _tcslen( sch ) ).CompareNoCase( sch ) == 0 )){
			if (NotFollowStr.IsEmpty())
				return i;
			else if (line.Mid(i,_tcslen(sch + NotFollowStr)).CompareNoCase(sch + NotFollowStr)!=0)
				return i;
		}
	}
	return -1;
}

int CInterpreter::FindStrNotInQuote(CStr &line, CStr sch,bool isAssign){
	return FindStrNotInQuote(line,sch,NULL,isAssign);
}

void CInterpreter::Comment(CStr & line){
	CStr Lside,Rside;
	long CommentPos;

	bool Flag = true;
	while(Flag){
		Flag = false;
		CommentPos = FindStrNotInQuote(line,L"//");
		if (CommentPos!=-1)	line = line.Left(CommentPos);
		CommentPos = FindStrNotInQuote(line,L"#");

		if (CommentPos!=-1 && line.FindNoCase(L"#include")==-1)	line = line.Left(CommentPos);
		CommentPos = FindStrNotInQuote(line,L";");
		if (CommentPos!=-1)	line = line.Left(CommentPos);

		CommentPos = FindStrNotInQuote(line,L"/*");
		if (CommentPos!=-1)	{
			Lside = line.Left(CommentPos);
			Rside = line.Mid(CommentPos+2);
			CommentPos = Rside.Find(L"*/");
			if (CommentPos != -1){
				line = Lside + Rside.Mid(CommentPos+2);
				Flag = true;
			}else{
				CStr tStr;
				while(1){
					tStr = GetLine();
					if (CurrentPos==-1)	break;
					Rside += tStr;
					CommentPos = Rside.Find(L"*/");
					if (CommentPos != -1){
						line = Lside + Rside.Mid(CommentPos+2);
						Flag = true;
						break;
					}
				}			
			}
		}
	}
}
//jwz:add end

//jwz:add modify from RunFile?
BOOL CInterpreter::Eval(CStr line){
	int sepPos;
	CStr cmd, param;
	BOOL Cont = 2;
	// Get first word of line (i.e., the command)
	eErrorPos = 0;
	sepPos = line.FindOneOf( L"( \t" );
	if ( sepPos != -1 )
	{
		cmd   = line.Left(sepPos);
		param = line.Mid( line[sepPos]=='(' ? sepPos : sepPos+1 );
		eErrorPos += sepPos;
	}
	else
	{
		// Command with no parameters
		cmd   = line;
		param.Empty();
	}

	// Trim whitespaces and make lower case to ease comparsion
	cmd.TrimLeft();cmd.TrimRight();
	cmd.MakeLower();
	param.TrimLeft();

	// Empty line? (How did that slip through?)
	if ( cmd.IsEmpty() ){
		Cont = TRUE;
	}

	if ( Cont == 2 && CondState == TRUE && line.GetAt(0) != L'%' && line.GetAt(0) != L'[' && param[0] == L'(') { 
		//cmd.MakeUpper();
		commandPointer cmdPtr = GetCommand( cmd ); 
		if ( cmdPtr != NULL ){
			Cont = cmdPtr( *this, param ); 
		} 
	} 

	if ( Cont == 2 && CondState == TRUE ){ 
		if ( cmd == L"#include" ) Cont = 1; 

		if ( cmd.Left(4) == L"send" 
			|| cmd == L"snapshot" 
			) 
		{ 
			if ( cmd == L"sendkeys" ) { 
				Cont = SendKeys( param ); 
			} else if ( cmd == L"sendspecial" ) { 
				Cont = SendSpecial( param ); 
			} else if ( cmd == L"sendctrlkey" ) { 
				Cont = SendCtrlKey( param ); 
			} else if ( cmd == L"sendok" ) { 
				Cont = SendCommand( cmd, param, 0x10000001 ); 
			} else if ( cmd == L"sendcancel" ) { 
				Cont = SendCommand( cmd, param, 0x10000002 ); 
			} else if ( cmd == L"sendyes" ) { 
				Cont = SendCommand( cmd, param, 0x10000006 ); 
			} else if ( cmd == L"sendno" ) { 
				Cont = SendCommand( cmd, param, 0x10000007 ); 
			} else { 
				Cont = SendSpecialKey( cmd, param ); 
			} 
		} 

		if ( cmd.CompareNoCase( L"MouseClick"			) == 0 )	Cont = MouseClick( param ); 
		if ( cmd.CompareNoCase( L"MouseDblClick"		) == 0 )	Cont = MouseClick( param, TRUE ); 
		if ( cmd.CompareNoCase( L"MouseDown"			) == 0 )	Cont = MouseDown( param ); 
		if ( cmd.CompareNoCase( L"MouseUp"				) == 0 )	Cont = MouseUp( param ); 
	#ifdef DESKTOP 
		if ( cmd.CompareNoCase( L"RightMouseClick"		) == 0 )	Cont = MouseClick( param, FALSE, 1 ); 
		if ( cmd.CompareNoCase( L"RightMouseDblClick"	) == 0 )	Cont = MouseClick( param, TRUE, 1 ); 
		if ( cmd.CompareNoCase( L"RightMouseDown"		) == 0 )	Cont = MouseDown( param, 1 ); 
		if ( cmd.CompareNoCase( L"RightMouseUp"			) == 0 )	Cont = MouseUp( param, 1 ); 
		if ( cmd.CompareNoCase( L"MiddleMouseClick"		) == 0 )	Cont = MouseClick( param, FALSE, 2 ); 
		if ( cmd.CompareNoCase( L"MiddleMouseDblClick"	) == 0 )	Cont = MouseClick( param, TRUE, 2 ); 
		if ( cmd.CompareNoCase( L"MiddleMouseDown"		) == 0 )	Cont = MouseDown( param, 2 ); 
		if ( cmd.CompareNoCase( L"MiddleMouseUp"		) == 0 )	Cont = MouseUp( param, 2 ); 
	#endif 

		if ( cmd == L"set"					)	Cont = Set( param ); 
		if ( cmd == L"repeat"				)	Cont = Repeat( param); 
		if ( cmd == L"endrepeat"			)	Cont = EndRepeat( param); 
		if ( cmd == L"errorlevel"			)	Cont = SetErrorLevel( param ); 

		if ( cmd.CompareNoCase(L"Exit") == 0)	Cont = FALSE; 
	}

	if ( Cont == 2 ) { 

		if ( cmd == L"if"								) {
			Cont = If( param ); 
			if (!param.IsEmpty()){
				Cont = Eval(param);
			}
		}
		if ( cmd == L"else"								) Cont = Else(); 
		if ( cmd == L"elseif"							) Cont = ElseIf( param ); 
		if ( cmd == L"endif"							) Cont = EndIf(); 

		if ( cmd == L"while"							) Cont = While( param); 
		if ( cmd == L"endwhile"							) Cont = EndWhile( param); 

		if ( cmd == L"for"								) Cont = For( param ); 
		if ( cmd == L"next"								) Cont = Next( param); 

		if ( cmd == L"foreach"							) Cont = ForEach( param); 
		if ( cmd == L"endforeach"						) Cont = EndForEach( param); 

		if ( cmd == L"switch"							) Cont = Switch( param); 
		if ( cmd == L"choice"							) Cont = Choice( param); 
		if ( cmd == L"choicedefault"					) Cont = ChoiceDefault( param); 
		if ( cmd == L"case"								) Cont = Case( param); 
		if ( cmd == L"default"							) Cont = Default(); 
		if ( cmd == L"endchoice" || cmd == L"endswitch" ) Cont = EndChoice(); 

		if ( cmd == L"sub"								) Cont = Def( param); 
		if ( cmd == L"endsub"							) Cont = EndSub(); 
		//jwz::add 
		if ( cmd == L"def"								) Cont = Def( param ); 
		if ( cmd == L"end"								) Cont = End( param); 

		if ( cmd == L"break"							) Cont = Break(); 
		if ( cmd == L"continue"							) Cont = Continue(); 
		//jwz::add end
		if ( cmd == L"return"							) Cont = Return( param ); 
	} 

	if ( Cont == 2 && CondState == 1 ){

		if ( FindStrNotInQuote(line,L"=",true)!=-1){
			Cont = Assignment( line ); 
		}else if (!line.IsEmpty()) {//no return keyword's return statements.
			*_Statement_Result = EvaluateExpression( line );
			Cont = TRUE;
		}	
	} 

	// Commands not parsed because they're skipped due to a false condition state 
	if ( Cont == 2 && CondState != TRUE ) { 
		Cont = TRUE; 
	} 

	return Cont;
}



//TokenArray add/modi more operator.
CInterpreter::token CInterpreter::TokenArray[] =
            { 
#ifndef JWZ
				{ L"NOT",		TOKEN_NOT,      TRUE  }//0
            ,	{ L"AND",		TOKEN_AND,      TRUE  }//1
            ,	{ L"OR",		TOKEN_OR,       TRUE  }//2
            ,	{ L"XOR",		TOKEN_XOR,      TRUE  }//3
            ,	{ L"LE",		TOKEN_LE_STR,   TRUE  }//4
            ,	{ L"GE",		TOKEN_GE_STR,   TRUE  }//5
            ,	{ L"NE",		TOKEN_NE_STR,   TRUE  }//6
            ,	{ L"LT",		TOKEN_LT_STR,   TRUE  }//7
            ,	{ L"GT",		TOKEN_GT_STR,   TRUE  }//8
            ,	{ L"EQ",		TOKEN_EQ_STR,   TRUE  }//9
            ,	{ L"MOD",		TOKEN_MODULO,   TRUE  }//0
            , 
#endif			
				{ L"&&",		TOKEN_LAND,     FALSE }//1 add
            ,	{ L"&",			TOKEN_AND,      FALSE }//2
            ,	{ L"||",		TOKEN_LOR,      FALSE }//3 add
            ,	{ L"|",			TOKEN_OR,       FALSE }//4
            ,	{ L"^",			TOKEN_XOR,      FALSE }//5 add
            ,	{ L"<=",		TOKEN_LE,       FALSE }//6
            ,	{ L">=",		TOKEN_GE,       FALSE }//7
            ,	{ L"<>",		TOKEN_NE,       FALSE }//8
            ,	{ L"!=",		TOKEN_NE,       FALSE }//9 add
            ,	{ L"<<",		TOKEN_LST,      FALSE }//0
            ,	{ L">>",		TOKEN_RST,      FALSE }//1
            ,	{ L"<",			TOKEN_LT,       FALSE }//2
            ,	{ L">",			TOKEN_GT,       FALSE }//3
            ,	{ L"==",		TOKEN_EQ,       FALSE }//4 add
            ,	{ L"=",			TOKEN_EQ,       FALSE }//5
            ,	{ L"++",		TOKEN_D_PLUS,   FALSE }//6 add
            ,	{ L"+",			TOKEN_PLUS,     FALSE }//7
            ,	{ L"--",		TOKEN_D_MINUS,  FALSE }//8 add
            ,	{ L"-",			TOKEN_MINUS,    FALSE }//9
            ,	{ L"**",		TOKEN_POWER,    FALSE }//0 add
            ,	{ L"*",			TOKEN_MULT,     FALSE }//1
            ,	{ L"/",			TOKEN_DIVIDE,   FALSE }//2
            ,	{ L"%",			TOKEN_MODULO,   FALSE }//3 MODULO add
            ,	{ L"\\",		TOKEN_PATHCAT,  FALSE }//4
            ,	{ L"(",			TOKEN_BR_OPEN,  FALSE }//5
            ,	{ L")",			TOKEN_BR_CLOSE, FALSE }//6
            ,	{ L",",			TOKEN_COMMA,    FALSE }//7
            ,	{ L"[",			TOKEN_REF_START,FALSE }//8
            ,	{ L"]",			TOKEN_REF_END,  FALSE }//9
            ,	{ L"?",			TOKEN_QUESTION, FALSE }//0
            ,	{ L"!",			TOKEN_NOT,      FALSE }//1
            ,	{ L"~",			TOKEN_LNOT,     FALSE }//2 add
            ,	{ L"::",		TOKEN_GLOBAL,   FALSE }//4 add global field.
            ,	{ L":",			TOKEN_COLON,    FALSE }//3
            ,	{ L"GETTYPE",	TOKEN_GETTYPE,	TRUE  }//5 add
            ,	{ L"TOUPCASE",	TOKEN_UCASE,	TRUE  }//6 add
            ,	{ L"TOLOWCASE",	TOKEN_LCASE,	TRUE  }//7 add			
			,	{ L"GETLENGTH",	TOKEN_GETSTRLEN,TRUE  }//8 add
            ,	{ L"TOCHAR",	TOKEN_TOCHAR,   TRUE  }//1 add
            ,	{ L"TOINTEGER",	TOKEN_TOLONG,   TRUE  }//2 add
            ,	{ L"TODOUBLE",	TOKEN_TODOUBLE, TRUE  }//5 add
            ,	{ L"TOSTRING",	TOKEN_TOSTRING, TRUE  }//6 add
            ,	{ L".",			TOKEN_DOT,      FALSE }//0
            ,	{ L"",			TOKEN_END,      FALSE }//1 "End of list" marker!
            };

//jwz:modi for == != < <= > >= compare
BOOL
CInterpreter::Compare( int      op
					, CValue  &par1
					, CValue  &par2
					)
{
  BOOL rc;

  switch( op )
  {
  case TOKEN_LT:
#ifndef JWZ
  case TOKEN_LT_STR:
#endif
	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_LT_STR
#endif
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) < 0 );
	}
	else
	    rc = ( (double)par1 < (double)par2 );
    break;
  case TOKEN_LE:
#ifndef JWZ
  case TOKEN_LE_STR:
#endif
	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_LE_STR
#endif		
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) <= 0 );
	}
	else
	    rc = ( (double)par1 <= (double)par2 );
    break;
  case TOKEN_GT:
#ifndef JWZ
  case TOKEN_GT_STR:
#endif
  	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_GT_STR
#endif
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) > 0 );
	}
	else
	    rc = ( (double)par1 > (double)par2 );
    break; 
  case TOKEN_GE:
#ifndef JWZ
  case TOKEN_GE_STR:
#endif
	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_GE_STR
#endif		
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) >= 0 );
	}
	else
	    rc = ( (double)par1 >= (double)par2 );
    break;
  case TOKEN_EQ:
#ifndef JWZ
  case TOKEN_EQ_STR:
#endif
	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_EQ_STR
#endif		
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) == 0 );
	}
	else
		rc = ( (double)par1 == (double)par2 );
    break;
  case TOKEN_NE:
#ifndef JWZ
  case TOKEN_NE_STR:
#endif
	if ( par1.GetType() == VALUE_STRING || par2.GetType() == VALUE_STRING 
#ifndef JWZ
		|| Token == TOKEN_NE_STR
#endif		
		)
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) != 0 );
	}else
	    rc = ( (double)par1 != (double)par2 );
    break;
  default:
    rc = 0;
    ASSERT( FALSE );
  }

  return rc;
}
//jwz:modi end


int
CInterpreter::GetCharacterType( TCHAR character )
{
  int rc = CHAR_OTHERS;

  switch ( character ) {
  case L' ':
  case L'\t':
    rc = CHAR_WHITESPACE;
    break;

  case L'\r':
  case L'\n':
    rc = CHAR_CR;
    break;

  case L'_':
    rc = CHAR_UNDERSCORE;
    break;

  case L'$':
    rc = CHAR_DOLLOR_VAR;
    break;

  case L'.':
    rc = CHAR_DOT;
    break;

  case L'\\':
    rc = CHAR_BACKSLASH;
    break;

  case L'\"':
	rc = CHAR_STR_QUOTE;
	break;
  case L'\'':
    rc = CHAR_QUOTE;
    break;

  //jwz:add but not used anymore
/*
  case L'+': case L'-': case L'*': case L'/': case L'%':
  case L'&': case L'|': case L'^': case L'<': case L'>':
  case L'=': case L'!': case L'~':
	rc = CHAR_OPR;
	break;
*/
  //jwz:add end

  case L'\0':
    rc = CHAR_END;
    break;
  }

  if ( rc == CHAR_OTHERS ){
    if (    (    character >= 'a'
              && character <= 'z' )
         || (    character >= 'A'
              && character <= 'Z' )
       )
    {
      rc = CHAR_ALPHA;
    }

    if (    rc == CHAR_OTHERS
         && character >= '0'
         && character <= '9' )
    {
      rc = CHAR_DIGIT;
    }
  }

  return rc;
}


int
CInterpreter::CheckTokenLiterals( void )
{
  int   rc = TOKEN_UNKNOWN;
  int   i;
  BOOL  tokenok;

  if ( Position >= Expression.GetLength() )
      return TOKEN_END;

   for ( i = 0; TokenArray[i].id != TOKEN_END; i++ ){

	if ( Expression.Mid( Position, _tcslen( TokenArray[i].string ) ).CompareNoCase( TokenArray[i].string ) == 0 ){
      tokenok = TRUE;

      if ( TokenArray[i].isAlpha == TRUE ){
        // Calculate the position after the found token string
        int pos = Position + _tcslen( TokenArray[i].string );

        // What follows?
        switch ( GetCharacterType( Expression[pos] ) ){
        case CHAR_ALPHA:
        case CHAR_DIGIT:
        case CHAR_UNDERSCORE:
          tokenok = FALSE;
          break;
        }
      }

      if ( tokenok == TRUE ){
        rc = TokenArray[i].id;
        Position += _tcslen( TokenArray[i].string );
      }

      // Found a fitting token, leave the loop
      // Because longer tokens are listed before short tokens, continuing is not required for those cases, too
      break;
    }
  }

  return rc;
}

void
CInterpreter::SkipSpaces( void ){
  while (    Position < Expression.GetLength()
          && (    GetCharacterType( Expression[Position] ) == CHAR_WHITESPACE
               || GetCharacterType( Expression[Position] ) == CHAR_CR
             )
        )
  {
    Position++;
  }
}




void
CInterpreter::NextToken( void )
{
  SkipSpaces();

  // If an error occurs, it's probably here, so let's save this position.
  // If necessary, it can be corrected later on.
  eErrorPos = Position;

  // Now let's check all token literals
  pToken = Token;
  Token = CheckTokenLiterals();

  // No token literal?
  if ( Token == TOKEN_UNKNOWN ){
    // So, what do we have here?
    switch ( GetCharacterType( Expression[Position] ) ){
    // Character or underline: Variable
    //case CHAR_ALPHA:
    //case CHAR_UNDERSCORE:
    //  Token = TOKEN_VARIABLE;
    //  break;

    // Digit or dot: A fixed number
    case CHAR_DOT:
    case CHAR_DIGIT:
      Token = TOKEN_NUMBER;
      break;

    // Quotes: A string
    case CHAR_STR_QUOTE:
      Token = TOKEN_STRING;
      break;

    // Quotes: A Char
    case CHAR_QUOTE:
      Token = TOKEN_CHAR;
      break;

    // \0: End of expression...
    case CHAR_END:
      Token = TOKEN_END;
      break;
	default:
		//MessageBox(NULL,L"Others",L"Split Next Token",MB_SETFOREGROUND);
		break;
    }
  }
}

//jwz:add for a char in a string
BOOL CInterpreter::In(TCHAR ch, CStr Arr){

	for (int i=0;i<Arr.GetLength();i++){
		if (Arr[i]==L'-' && i+1<Arr.GetLength() && Arr[i+1]==L'-')
			if (ch==L'-') return true; else i++;
		else if (Arr[i]==L'-'){
			if ((Arr[i-1] > Arr[i+1]) ? (ch < Arr[i-1] && ch > Arr[i+1]) : (ch > Arr[i-1] && ch < Arr[i+1])) return true;
		}else if (ch == Arr[i]) return true;
	}
	return false;
}
//jwz:add end

CValue
CInterpreter::Number( void )
{
  int                relpos;
  BOOL				 Hex = FALSE;
  BOOL               cont = TRUE;
  CValue             rc;

#ifdef _DEBUG
  int check = GetCharacterType( Expression[Position] );
  ASSERT( check == CHAR_DIGIT || check == CHAR_DOT );
#endif

  int state = 0; TCHAR ch,nch;
  //long ExprLen = Expression.GetLength();

  //jwz:modi for number get support 0xFFFF 2.0E+2 and so on
  for(relpos=Position;relpos<ExprLen;){
	ch = Expression[relpos];
	if (relpos+1<ExprLen) nch = Expression[relpos+1]; else nch = -1;

	if ( relpos == Position && !Hex && ch == L'0' && (nch == L'x' || nch == L'X')){
		Hex = true;
		relpos += 2;
	}else if (Hex && (ch>=L'0' && ch <=L'9' || ch>=L'a' && ch <=L'f' || ch >= L'A' && ch<=L'F')){//In(ch,L"0123456789abcdefABCDEF")
		relpos ++;
	}else{
		if (state == 0 && (ch == L'.' && nch >= L'0' && nch <= L'9' || ch >= L'0' && ch <=L'9')){//In(ch,L"0123456789.")
			if (ch == L'.') state = 1; else state = 2;
			relpos ++;
		}else if (state == 1 && (ch>=L'0' && ch <=L'9')){//In(ch,L"0123456789")
			state = 3;
			relpos ++;
		}else if (state == 2 && (ch==L'.' && nch >= L'0' && nch <= L'9' || ch == L'E' || ch==L'e' || ch>=L'0' && ch<=L'9')){ //In(ch,L"0123456789.Ee")
			if (ch == L'.') state = 1; else if (ch == L'E' || ch == L'e') state = 4;
			relpos ++;
		}else if (state == 3 && (ch == L'E' || ch==L'e' || ch>=L'0' && ch<=L'9')){//In(ch,L"0123456789Ee")
			if (ch == L'E' || ch == L'e') state = 4;
			relpos ++;
		}else if (state == 4 && (ch == L'+' || ch==L'-' || ch>=L'0' && ch<=L'9')){//In(ch,L"123456789+-"
			if (ch == L'+' || ch == L'-' ) state = 5; else state = 6;
			relpos ++;
		}else if (state == 5 && (ch>=L'0' && ch<=L'9')){//In(ch,L"123456789")
			state = 6;
			relpos ++;
		}else if (state == 6 && (ch>=L'0' && ch<=L'9')){//In(ch,L"0123456789")
			relpos ++;
		}else if (state != 0 && state != 3 && state != 6 && state != 2){
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Digital Number format error!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			exit(1);
		}else break;
	}
  }
	
  CValue tmpVal; CStr tVal;
  tVal = (LPCTSTR)Expression.Mid( Position, relpos );

  if (Hex){
	  rc = wcstol(tVal,NULL,16);
  }else if (state == 0 || state == 2){
	  rc = wcstol(tVal,NULL,10);
  }else{
	  rc = wcstod(tVal,NULL);
  }
  //jwz:modi end
  Position = relpos;               // Parse-Position weitersetzen

  NextToken();                        // Get next token

  return rc;
}

CValue
CInterpreter::String( void )
{
  CValue             rc;
  int                pos, startPos;

#ifdef _DEBUG
  // This function is only invoked if TOKEN_STRING is found,
  // so the next character should be a quote...
  int check = GetCharacterType( Expression[Position] );
  ASSERT( check == CHAR_STR_QUOTE );
#endif

  pos = startPos = Position+1; // skip opening quote

  //jwz:modi
  TCHAR ch=0,pch;
  while(pos<ExprLen){
	  pch = ch;
	  ch = Expression[pos];
	  if (ch == L'"' && pch != L'\\') break;
	  pos++;
  }
  //jwz:modi end

  if ( pos >= startPos && Expression[pos] == L'"')
  {
	 // replace special characters
     CStr str = Expression.Mid( startPos, pos-startPos );
	 str = SlashChar(str);
	 str.Replace(L"\\/",L"^NOT_PATH_SLASH^");
 	 str.Replace(L"/",L"\\");
	 str.Replace(L"^NOT_PATH_SLASH^",L"/");
	 rc = str;
  }
  else
  {
     Error        = 1;
     ErrorMessage = L"String not closed (missing quote)";
  }

  Position = pos + 1;

  
  if ( Error == 0 )
  {
	  NextToken();                         // Get next token
  }

  return rc;
}

//jwz:add for property
CValue
CInterpreter::Convert(CValue varVal){
  CValue rc = varVal;
  // jwz:add for a.toint type convert
  while ( Token == TOKEN_TOCHAR 
	  || Token == TOKEN_TOLONG 
	  || Token == TOKEN_TODOUBLE 
	  || Token == TOKEN_TOSTRING 
	  || Token == TOKEN_UCASE 
	  || Token == TOKEN_LCASE 
	  || Token == TOKEN_GETTYPE
	  || Token == TOKEN_GETSTRLEN
	  ){
	  
	  CStr tStr;
	  switch(Token){
	  case TOKEN_TOCHAR:
		  rc = (TCHAR)rc;
		  break;
	  case TOKEN_TOLONG:
		  rc = (long)rc;
		  break;
	  case TOKEN_TODOUBLE:
		  rc = (double)rc;
		  break;
	  case TOKEN_TOSTRING:
		  rc = (CStr)rc;
		  break;
	  case TOKEN_UCASE:
		  tStr = (CStr)rc;
		  tStr.MakeUpper();
		  rc = tStr;
		  break;
	  case TOKEN_GETSTRLEN:
	      if (rc.GetType() == VALUE_STRING){
			rc = (long)((CStr)rc).GetLength();
		  }
		  break;

	  case TOKEN_GETTYPE:
		  switch(rc.GetType()){
		  case VALUE_NULL:
			  rc = L"null";
			  break;
		  case VALUE_CHAR:
			  rc = L"char";
			  break;
		  case VALUE_LONG:
			  rc = L"long";
			  break;
		  case VALUE_DOUBLE:
			  rc = L"double";
			  break;
		  case VALUE_STRING:
			  rc = L"string";
			  break;
		  case VALUE_PTR:
			  rc = L"pointer";
			  break;
		  default:
			  rc = L"map";
			  break;
		  }
		  break;

	  default:
		  tStr = (CStr)rc;
		  tStr.MakeLower();
		  rc = tStr;
		  break;
	  }

	  SkipSpaces();

	  if ( Position < Expression.GetLength() && Expression[Position] == L'(' ){ //Skip ".Length( )" included "( )"
		  Position++;
		  SkipSpaces();
		  if ( Position < Expression.GetLength() && Expression[Position] == L')' )Position++;
	  }
	  
	  NextToken();
  }

  return rc;
}
//jwz:add end

CValue
CInterpreter::VariableValue( LPCTSTR start )
{
  CValue rc;

  SkipSpaces();
  
  int pos=0,GlobalPos = 0;
  CStr varName;
  if ( start != NULL )
	varName = start + Expression.Mid( Position );
  else
	varName = Expression.Mid( Position );

  //jwz:add for Global variable.
  if (Token == TOKEN_GLOBAL && pToken != TOKEN_UNKNOWN){ 
	 varName = L"::"+varName;
	 GlobalPos = 2;
  }
  //jwz:end

  CValue *varVal = GetVariable( varName, TRUE, &pos );
  if ( pos == -1 )
  {
     Error        = 2;
     ErrorMessage = VarError;
  }
  else
  {
	  Position += pos - GlobalPos;
	  if ( start != NULL ) Position -= wcslen( start );
  }

  if ( varVal != NULL )
  {
	  rc = *varVal;
	  NextToken();
  }
  
  return rc;
}


CValue
CInterpreter::Variable( void )
{
  CValue             rc;

  int check = GetCharacterType( Expression[Position] );
#ifdef _DEBUG
  ASSERT( check == CHAR_ALPHA || check == CHAR_UNDERSCORE || check == CHAR_DOLLOR_VAR);
#endif


  rc = VariableValue(NULL);
	
  if ( Error == 0 ){
    NextToken();
    if ( Token != TOKEN_MODULO ) // Closing "%"
	{
	  Error        = 2;
	  ErrorMessage = L"Variable not closed (missing %)";
	}
  }

  //Position = Position + 1;

  if ( Error == 0 )
  {
     NextToken();                         // Get next token
  }

  return rc;
}


CValue 
CInterpreter::Function( void )
{
  CValueArray params;
  CValue             rc;
  long                pos, startPos;
  pos = startPos = Position;
  int check = GetCharacterType( Expression[startPos] );
#ifdef _DEBUG
  ASSERT( check == CHAR_ALPHA || check == CHAR_UNDERSCORE || check == CHAR_DOLLOR_VAR );
#endif


  while ( check == CHAR_ALPHA || check == CHAR_DIGIT || check == CHAR_UNDERSCORE )
  {
	  pos++;
	  if ( pos >= Expression.GetLength() )
		  break;
	  check = GetCharacterType( Expression[pos] );
  }
  // jwz:add for Regular Expression Variable Skip Regex variable name characters
  if (Expression[pos]=='$'){
 	if (   Expression[pos+1] == L'&' 
		|| Expression[pos+1] == L'`' 
		|| Expression[pos+1] == L'\'' 
		|| Expression[pos+1] == L'+' 
		|| Expression[pos+1] == '_'
		)pos +=2;
    else{
		for (int i=1;i<=3;i++){
			if ( pos+i<Expression.GetLength() && Expression[pos+i] >='0' && Expression[pos+i]<='9')
				pos++;
			else 
				break;
		}
		pos++;
	}
  }
  // jwz:add end

  CStr funcName = Expression.Mid( startPos, pos-startPos );
  Position = pos;
  SkipSpaces();
  pos = Position;
  
  // Function!
  if ( pos < Expression.GetLength() && Expression[pos] == L'(' )
  {
	 funcName.MakeLower();
     BOOL funcFound = FALSE;
	 Position = pos+1;

	 NextToken();
	 while ( Token != TOKEN_BR_CLOSE && Error == 0 ){
		CValue param = Logical();
		if ( Error == 0 ) params.Add( param );

		if ( Error == 0 && Token != TOKEN_COMMA && Token != TOKEN_BR_CLOSE ){
			Error        = 4;
			ErrorMessage = L"Comma or closing parantheses expected";
		}

		if ( Error == 0 && Token == TOKEN_COMMA )
			NextToken();
	 }

	 if ( Error == 0 ){
		
		 functionPointer function = GetFunction( funcName );
		 //jwz:add for function call.
		 if (IsUserFunc(funcName)){
			 rc = EvalFunc(funcName, params);
		 }else //jwz:add end
			 if ( function != NULL ) {
			 rc = function(params, Error, ErrorMessage );

			 if ( Token != TOKEN_BR_CLOSE ){
			   // Klammer wurde nicht geschlossen
			   Error        = 4;
			   ErrorMessage = L"Missing closing bracket";
			   eErrorPos     = pos;  // Fehler-Position ist bei der 鰂fnenden Klammer
			 }
		 }else {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Unknown function, please check it!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			exit(1);
		 }
		 NextToken();
	 }
  }else{
	 funcName = Expression.Mid( startPos, pos-startPos );

	 //jwz:add for &varName as global variable
	 //jwz:add end
	 if ( Error == 0 ){
		 rc = VariableValue(funcName);
		//jwz:add for ++ --
		if (Token == TOKEN_D_PLUS){ // i++
			*_LastVariable = (long)*_LastVariable+1L;
			NextToken();
		}else if (Token == TOKEN_D_MINUS){
			*_LastVariable = (long)*_LastVariable-1L;
			NextToken();
		}
		//jwz:add end
	 }
  }

  return rc;
}

CValue
CInterpreter::Reference( void )
{
	CValue    rc;

	if ( Error != 0 )return rc;

	NextToken();                  // Skip "["
	rc = Logical();

    // Check for closing ]
    if ( Error == 0 ){
		if ( Token != TOKEN_REF_END ){
			// Missing ]
			Error        = 4;
			ErrorMessage = L"Missing closing bracket";
		}else{
			CStr varName = (CStr)rc;
			if ( varName.IsEmpty() ){
				Error        = 4;
				ErrorMessage = L"Reference is empty";
			}else{
				int pos=0;
				CValue *varVal = GetVariable( varName, FALSE, &pos );
				
				if ( pos == -1 ){
					Error        = 2;
					ErrorMessage = L"Error in referred variable: " + VarError;
				}else if ( pos < varName.GetLength() ){
					Error        = 2;
					ErrorMessage = L"Error in referred variable: Invalid characters";
				}else if ( varVal != NULL ){
					rc = *varVal;
				}else{
					rc.Clear();
				}
			}
			NextToken();              // skip "]"
		}
    }

	return rc;
}

CValue
CInterpreter::Chars( void ){
	CValue rc;
	if (Error != 0) return rc;
	
	if (Expression[Position] == L'\''){
		Position++;
		rc = (TCHAR)Expression[Position++];
	}

	if (Expression[Position++]!=L'\''){
		Error        = 1;
		ErrorMessage = L"Char not closed (missing Single quote)";
	}
  
	if ( Error == 0 ) NextToken();                         // Get next token

	return rc;
}

CValue
CInterpreter::Value( void )
{
  CValue rc;

  if ( Error != 0 ) return rc;

  switch ( Token ){
  case TOKEN_DOT:		// Decimal point (.123)
	Position--;			// include the dot
  case TOKEN_NUMBER:     // Any digit
    rc = Number();
    break;

  case TOKEN_CHAR:     // String
    rc = Chars();
    break;

  case TOKEN_STRING:     // String
    rc = String();
    break;

  case TOKEN_MODULO:   // Variable (with %..%)
    rc = Variable();
    break;

  case TOKEN_REF_START:  // Referred variable ([...])
    rc = Reference();
    break;

  case TOKEN_UNKNOWN:	// Text (Function/Variable)
  case TOKEN_GLOBAL:	//jwz: for Global variable
	rc = Function();
	break;

  default:
	Error        = 3; // Value expected
	ErrorMessage = L"Operator found where value expected";
  }

  if (Token==TOKEN_DOT){
	NextToken();
	rc = Convert(rc);
  }

  return rc;
}

CValue CInterpreter::IncDec(void){
	CValue rc; 
	if (Error !=0 ) return rc;

	switch(Token){
	case TOKEN_D_PLUS:
		NextToken();
		rc = Value();
		*_LastVariable = rc = (long)rc + 1L;
		break;
	case TOKEN_D_MINUS:
		NextToken();
		rc = Value();
		*_LastVariable = rc = (long)rc - 1L;
		break;
	default:
		rc = Value();
	}
	return rc;
}

CValue
CInterpreter::Brackets( void )
{
  int       saveErrorPos;
  CValue    rc;

  if ( Error != 0 ) return rc;
	
  switch ( Token ){
  case TOKEN_BR_OPEN:             // Klammer auf
    saveErrorPos = eErrorPos;      // Position der Klammer merken
    NextToken();                  // Klammer 黚ergehen
    rc = Logical();
    // N鋍hstes Token mu?")" sein.
	//Debug(ErrorMessage,Token);

    if ( Error == 0 ){
      if ( Token != TOKEN_BR_CLOSE ){
        // Klammer wurde nicht geschlossen
        Error        = 4;
        ErrorMessage = L"Missing closing bracket";
        eErrorPos     = saveErrorPos;  // Fehler-Position ist bei der 鰂fnenden Klammer
      }else
		  NextToken();
    }
    break;

  case TOKEN_BR_CLOSE:            // Klammer zu
    // fehlende 鰂fnende Klammer
    Error        = 5;
    ErrorMessage = L"Missing opening bracked";

  default:
    rc = IncDec();  // Ist's keine Klammer, muss es ein Wert sein
	//MessageBox(NULL,(CStr)rc,L"Brackets",MB_SETFOREGROUND);
  }
  return rc;
}


CValue
CInterpreter::Sign( void )
{
  int       saveErrorPos;
  CValue rc;

  if ( Error != 0 ) return rc;

  switch ( Token ){
  case TOKEN_MINUS:    // "-"
    NextToken();
    rc = Brackets();

    // Kein Fehler aufgetreten?
    if ( Error == 0 ){
	  if ( rc.GetType() == VALUE_LONG )
		  rc = 0 - (long)rc;
	  else if ( rc.GetType() == VALUE_DOUBLE )
		  rc = (double)0 - (double)rc;
	  else
		  rc = (long)0;
    }
    break;

  case TOKEN_PLUS:    // "+"
    NextToken();      // einfach ignorieren...
    rc = Brackets();
    break;

  default:
    rc = Brackets();
  }

  return rc;
}

CValue
CInterpreter::Not( void )
{
  CValue rc;
  int tk;

  if ( Error != 0 ) return rc;

  switch ( Token ){
  case TOKEN_NOT:    // "NOT"
  case TOKEN_LNOT:	//"~"
	tk=Token;
    NextToken();
    rc = Sign();
    // Kein Fehler aufgetreten?
    if ( Error == 0 ){
		if (tk!=TOKEN_LNOT){
		  if ( (long)rc == 0 )
			rc = (long)1;
		  else
			rc = (long)0;
		}else
		  rc=~(long)rc;
    }
    break;

  default:
    rc = Sign();
  }

  return rc;
}

CValue
CInterpreter::Power( void )
{
  BOOL      cont = TRUE;      
  CValue   rc;
  CValue   val1;
  CValue   val2;

  if ( Error != 0 ) return rc;
  
  rc = Not();

  // Der erste Wert von Operationen ist gleichzeitig der R點kgabewert:
  val1 = rc;

  while ( Error == 0 && cont == TRUE ){
    switch ( Token ){
    case TOKEN_POWER:
      NextToken();
      val2 = Not();
      if ( Error == 0 ){
	      if ( val1.IsDouble() || val2.IsDouble() ){
		    double dRc   = pow( (double)val1, (double)val2 );
		    rc = dRc;
		  }else{
		    long lRc   = (long)pow( (double)val1, (double)val2 );
		    rc = lRc;
		  }
          val1 = rc;
      }
      break;

    default:
      // No more power operations
      cont = FALSE;
    }
  }

  return rc;
}


CValue
CInterpreter::Mult( void )
{
  BOOL      cont = TRUE;      
  CValue   rc;
  CValue   val1;
  CValue   val2;
  long i;

  if ( Error != 0 ) return rc;

  rc = Power();

  // Der erste Wert von Operationen ist gleichzeitig der R點kgabewert:
  val1 = rc;

  while ( Error == 0 && cont == TRUE ){
    switch ( Token ){
    case TOKEN_MULT:
      NextToken();
      val2 = Power();
      if ( Error == 0 ){
	    if ( val1.GetType() == VALUE_STRING && val2.GetType() == VALUE_LONG ){
			rc=L"";
			for (i = 0; i< (long) val2; i++) rc=(CStr)rc+(CStr) val1;
		}else if(val1.GetType() == VALUE_LONG && val2.GetType() == VALUE_STRING){
			rc=L"";
			for (i = 0; i< (long) val1; i++) rc=(CStr)rc+(CStr) val2;
		}else if ( val1.IsDouble() || val2.IsDouble() ){
		  double dRc   = (double)val1 * (double)val2;
		  rc = dRc;
		}else{
		  long lRc   = (long)val1 * (long)val2;
		  rc = lRc;
		}

        // F黵 verkettete Operationen ("5 * 2 / 3") wird der erste Wert
        // auf das R點kgabe-Objekt gesetzt.
        val1 = rc;
      }
      break;

    case TOKEN_DIVIDE:
      NextToken();
      val2 = Power();
      if ( Error == 0 ){
		  //if (!val1.IsValidNumber()){
		  if (val1.GetType() == VALUE_STRING){
			  CStr tStr,tStr1;
			  tStr = (CStr) val1;
			  tStr1 = (CStr) val2;
			  tStr.Replace(tStr1,L"");
			  rc=tStr;
		  }else{
			  if ( (double)val2 == 0 ){
				Error        = 6;
				ErrorMessage = L"Division by zero (or invalid string?)";
			  }else if ( val1.IsDouble() || val2.IsDouble() ){
				double dRc   = (double)val1 / (double)val2;
				rc = dRc;
			  }else{
				long lRc   = (long)val1 / (long)val2;
				rc = lRc;
			  }
		  }
		  val1 = rc;
      }
      break;

    case TOKEN_MODULO:
      NextToken();
      val2 = Power();
      if ( Error == 0 ){
        if ( (long)val2 == 0 ){
          Error        = 6;
          ErrorMessage = L"Division by zero (or invalid string?)";
        }else{
          long lRc   = (long)val1 % (long)val2;
          rc = lRc;
          val1 = rc;
        }
      }
      break;

    default:
      // Keine Punkt-Operatoren mehr...
      cont = FALSE;
    }
  }

  return rc;
}


CValue
CInterpreter::Calculate( void )
{
  BOOL      cont = TRUE;
  int       opToken;
  CValue    rc;
  CValue    val1;
  CValue    val2;

  if ( Error != 0 ) return rc;

  rc = Mult();

  // Der erste Wert von Operationen ist gleichzeitig der R點kgabewert:
  val1 = rc;

  while ( cont == TRUE && Error == 0 ){
    switch ( Token ){
    case TOKEN_PLUS:
    //case TOKEN_STRCAT:
	case TOKEN_PATHCAT:
    case TOKEN_MINUS:
      opToken = Token;
      NextToken();
      val2 = Mult();

      if ( Error == 0 ){
  	    if (opToken == TOKEN_PATHCAT){
				CStr str1 = val1;
				CStr str2 = val2;
				str1.TrimRight( '\\' );
				str2.TrimLeft( '\\' );
				rc = str1 + L"\\" + str2;
        }else if ( opToken == TOKEN_PLUS ){
			//if (!val1.IsValidNumber() || !val2.IsValidNumber()){
			if (val1.GetType()==VALUE_STRING || val2.GetType() == VALUE_STRING){
				rc = (CStr) val1 + (CStr) val2;
			}else if ( val1.IsDouble() || val2.IsDouble() ){
				rc = (double)val1 + (double)val2;
			}else{
				rc = (long)val1 + (long)val2;
			}
        }else{
			//if (!val1.IsValidNumber() || !val2.IsValidNumber()){ //"Wolf&Lion SoftLion OK"-"Lion" -->"Wolf& SoftLion OK"
			if (val1.GetType()==VALUE_STRING || val2.GetType() == VALUE_STRING){
				CStr tStr,tStr1;
				tStr = (CStr) val1;
				tStr1 = (CStr) val2;
				long i=tStr.Find(tStr1);
				tStr = tStr.Left(i) + tStr.Mid(i+tStr1.GetLength());
				rc = tStr;
			}else if ( val1.IsDouble() || val2.IsDouble() ){
				rc = (double)val1 - (double)val2;
			}else{
				rc = (long)val1 - (long)val2;
			}
        }

        // F黵 verkettete Operationen ("5 * 2 / 3") wird der erste Wert
        // auf das R點kgabe-Objekt gesetzt.
        val1 = rc;
      }
      break;

    default:
      // Keine Strich-Operatoren mehr
      cont = FALSE;
    }
  }

  return rc;
}


CValue
CInterpreter::Shift( void )
{
	BOOL      cont = TRUE;      
	int       opToken;
	CValue    rc;
	CValue    val1;
	CValue    val2;

	if ( Error != 0 ) return rc;

	rc = Calculate();

	// First value of operations is the current return value
	val1 = rc;

	while ( cont == TRUE && Error == 0 ){
		switch ( Token ){
		case TOKEN_LST:
		case TOKEN_RST:
			opToken = Token;

			NextToken();

			val2 = Calculate();

			if ( opToken == TOKEN_RST )
				rc = (long)val1 >> (long)val2;
			else
				rc = (long)val1 << (long)val2;

			val1 = rc;
			break;

		default:
			// Keine String-Operatoren mehr
			cont = FALSE;
		}
	}

	return rc;
}



CValue
CInterpreter::Compare( void )
{
  int       compare;
  int       errPos;
  BOOL      result;
  CValue    rc;
  CValue    val1;
  CValue    val2;

  if ( Error != 0 ) return rc;

  rc = Shift();

  errPos = ErrorPos;

  val1 = rc;

  if ( Error == 0 ){
    switch ( Token ){
      case TOKEN_EQ:
      case TOKEN_LT:
      case TOKEN_LE:
      case TOKEN_GT:
      case TOKEN_GE:
      case TOKEN_NE:
#ifndef JWZ
      case TOKEN_EQ_STR:
      case TOKEN_LT_STR:
      case TOKEN_LE_STR:
      case TOKEN_GT_STR:
      case TOKEN_GE_STR:
      case TOKEN_NE_STR:
#endif
        compare = Token;
        NextToken();
        val2 = Shift();

        if ( Error == 0 )
        {
           result = Compare( compare, val1, val2 );
           rc = (long)result;
        }

        val1 = rc;
        break;

      default:
        // Kein Vergleich
        break;
    }
  }

  return rc;
}



CValue
CInterpreter::Xor( void )
{
  BOOL      cont = TRUE;
  CValue    rc;
  CValue    val1;
  CValue    val2;

  if ( Error != 0 ) return val1;

  rc = val1 = Compare();
  
  while ( cont == TRUE && Error == 0){
    if ( Token == TOKEN_XOR || Token == TOKEN_OR ||  Token == TOKEN_AND /*&& pToken == TOKEN_UNKNOWN */){
      int tk = Token;
      NextToken();
      val2 = Compare();

      if ( Error == 0 ){
        long lRc;
		switch (tk){
		case TOKEN_XOR:
			lRc = (long)val1 ^ (long)val2;
			break;
		case TOKEN_OR:
			lRc = (long)val1 | (long)val2;
			break;
		default:
			lRc = (long)val1 & (long)val2;
			break;
		}
        rc = lRc;
        val1 = rc;
      }
    }else{
      // Keine Oder-Operatoren mehr
      cont = FALSE;
    }
  }

  return rc;
}


CValue
CInterpreter::And( void )
{
  BOOL      cont = TRUE;
  CValue    rc;
  CValue    val1;
  CValue    val2;

  if ( Error != 0 ) return val1;

  rc = val1 = Xor();

  while ( cont == TRUE && Error == 0){
    if ( Token == TOKEN_LAND ){
      int tk = Token;
      NextToken();
      val2 = Xor();
      if ( Error == 0 ){
        long lRc;
        lRc = (long)val1 && (long)val2;
        rc = lRc;
        val1 = rc;
      }
    }else{
      // Keine Und-Operatoren mehr
      cont = FALSE;
    }
  }

  return rc;
}



CValue
CInterpreter::Logical( void )
{
  BOOL      cont = TRUE;
  CValue    rc;
  CValue    val1;
  CValue    val2;

  if ( Error != 0 ) return val1;

  rc = val1 = And();

  while ( cont == TRUE && Error == 0){
    if ( Token == TOKEN_LOR ){
      int tk = Token;
      NextToken();
      val2 = And();

      if ( Error == 0 ){
        long lRc;
        lRc = (long)val1 || (long)val2;
        rc = lRc;
        val1 = rc;
      }
    } else {
      // Keine Oder-Operatoren mehr
      cont = FALSE;
    }
  }

  return rc;
}


CValue
CInterpreter::Condition( void )
{
	BOOL      cont = TRUE;
	CValue    rc;
	CValue    val1;
	CValue    val2;

	if ( Error != 0 ) return val1;

	rc = Logical();

    if ( Error == 0 && Token == TOKEN_QUESTION ) {
		NextToken();
		val1 = Logical();
		if ( Error == 0 ) {
			if ( Token != TOKEN_COLON ) {
				 Error        = 8;
				 ErrorMessage = L"Missing ':' for '?' operator";
			} else {
				NextToken();
				val2 = Compare();

				if ( Error == 0 ) {
					if ( (long)rc == 0 )
						rc = val2;
					else
						rc = val1;
				}
			}
		}
    }

	return rc;
}


CValue
CInterpreter::EvaluateExpression( CStr Expr, BOOL fixedEnd )
{
	CValue rc = 0L;
	Token = 0;
	if (!Expr.IsEmpty()){

		Expression = Expr;
		ExprLen = Expression.GetLength();
		
		Position = 0;

		Error    = 0;

		rc = EvaluateExpr(fixedEnd);

	}
	return rc;
}

CValue
CInterpreter::EvaluateExpr( BOOL fixedEnd )
{
  CValue rc;

  Position   = 0;
  Error      = 0;

  // Erstes Token ermitteln
  NextToken();

  if ( Token == TOKEN_END || (!fixedEnd && Token == TOKEN_COMMA) ) // Leerer Ausdruck
  {
    rc.Clear();
  } else {
    rc = Condition();

    if ( Error == 0 && fixedEnd && Token != TOKEN_END ) {
        if ( Token == TOKEN_BR_CLOSE ) {
		  Error        = 5;
		  ErrorMessage = L"Missing opening bracked";
		} else {
          Error        = 7;
          ErrorMessage = L"Operator expected";
		}
    }
  }

  return rc;
}



int 
CInterpreter::GetError()
{
  return Error;
}



LPCTSTR
CInterpreter::GetErrorMessage( void )
{
  return ErrorMessage;
}



int 
CInterpreter::GetErrorPosition()
{
  return eErrorPos;
}

