// Interpreter.h: The main interpreter
//

#if !defined(AFX_INTERPRETER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
#define AFX_INTERPRETER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef POCKETPC
#include "resource.h"		// main symbols
#endif

#ifdef DESKTOP
#include "vc6\resource.h"
#endif

#include "morttypes.h"

#define ERROR_OFF      0
#define ERROR_CRITICAL 1
#define ERROR_SYNTAX   2
#define ERROR_ERROR    3
#define ERROR_WARN     4
#define ERROR_DEBUG    5

#define CT_IF      1
#define CT_WHILE   2
#define CT_SWITCH  3
#define CT_CHOICE  4
#define CT_FOR     5
#define CT_FOREACH 6
#define CT_SUB     7 // Only for nesting test
#define CT_REPEAT  8

int GetCharacterType( TCHAR character );
//this for debug
void Debug(LPCTSTR msg);
void Debug(long msg);
void Debug(long msg,long title);
void Debug(LPCTSTR msg,long title);
void Debug(long msg,LPCTSTR title);
void Debug(LPCTSTR msg,LPCTSTR title);

CStr SlashChar(CStr str);
//void Trim(CStr &Func);
//CStr TrimOne(CStr line, LPCTSTR ch = L"\"");

HKEY    GetTopKey( CStr &topkey );
CStr	RegRead( HKEY topkey, LPCTSTR path, LPCTSTR value );
DWORD   RegReadDW( HKEY topkey, LPCTSTR path, LPCTSTR value );
BOOL	RegWrite( HKEY topkey, LPCTSTR path, LPCTSTR valName, LPCTSTR value );
BOOL	RegWriteDW( HKEY topkey, LPCTSTR path, LPCTSTR valName, DWORD value );

class CInterpreter
{
public:
	CInterpreter();
    ~CInterpreter();
	void	RunFile( LPCTSTR filename , bool isCmdRun);
	void    Parser( BOOL LocalVarsExist = FALSE, CStr Statement = L"" );
	CValue	EvalFunc(CStr FuncName, CValueArray &params);
	BOOL	Eval(CStr line);
	static BOOL	In(TCHAR ch, CStr Arr);
    int     Split( const CStr &source, TCHAR sep, CValueArray &dest, BOOL trim = TRUE, ULONG dontParse=0 /*CStrArray *variables = NULL*/ );

    CStr	ScriptFile;
	CStr	Content;
	
	CStrArray IncludedFiles;

	int		ErrorPos;

    BYTE		ErrorLevel;
    CStr		ErrorLine;
    HANDLE		Connection;

	BOOL	SetVariable( CStr &varName, CValue &value );
	BOOL	SetVariable( CStr &varName, LPCTSTR value );
	BOOL	ClearVariable( CStr &varName );

	CStr GetErrorLine();


protected:

	CMapStrToPtr     Subs;
	CMapStrToPtr     EndPosOfSubs;

    BOOL			 CondState;
    CMortUIntArray	 CondStack;
    
    CMortUIntArray	 RepeatCount;
    CMortUIntArray	 RepeatPos;
    
    CMortUIntArray	 WhilePos;
    CValueArray		 ChoiceStack;
    
	CMortUIntArray	 SubStack;

    CMortUIntArray	 SwitchPos;

    CMortUIntArray	 ForPos;
    CMortUIntArray	 ForVariable; // Pointer to CValue
    CMortUIntArray	 ForLimit;
    CMortUIntArray	 ForStep;

    //CUIntArray   ForEachStack;
    CStrArray		 ForEachVariables;
    CMortPtrArray    ForEachData;
    CMortUIntArray   ForEachType;
    CMortUIntArray   ForEachPos;
	
	CMapIntToInt	 EndRepeatPositions;
	
	CMapIntToInt     EndIfPositions;
	CMapIntToInt     EndForPositions;
	CMapIntToInt     EndForEachPositions;
	CMapIntToInt     EndWhilePositions;
	CMapIntToInt     EndSwitchPositions;

	//jwz:add func GetParam(L"abc(a,b,c,d,func(e,f,g,h),i)",'(',')') will return L"a,b,c,d,func(e,f,g,h),i"
	CStr	GetParam(CStr &Func,TCHAR beg='(',TCHAR end=')',BOOL IncludeSep=false);
	BOOL	IsUserFunc(CStr &line);	//jwz:add for judge a function is user define or not.
	CStr	GetParamDefName(CStr &line,CStr &DefName); //jwz:add Get param and name, like ABC(a,b,c) => (a,b,c) and DefName is ABC.
	CStr	FetchLine();	//jwz:add for FetchLine from Source File's Content.
	CStr	GetLine();
	CValue * GetVariable( CStr &var, BOOL create, int *position = NULL, BOOL local = FALSE );
	BOOL	IsSingleIf(CStr &line, CStr & ReminderStr);	//jwz:add for single If statement.
	BOOL	IsSingleElse(CStr &line, CStr & ReminderStr); //jwz:add for single else statement.
	int		FindStrNotInQuote(CStr &line, CStr sch, CStr NotFollowStr=NULL,bool isAssign=false); //jwz:add for Find a string, it's not in the string.
	int		FindStrNotInQuote(CStr &line, CStr sch,bool isAssign);
	void	Comment(CStr &line);//jwz:add for Comment // # ; /* ... */
	BOOL	Break();		//jwz:add for Break statement.
	BOOL	Continue();		//jwz:add for Continue statement.

	BOOL	SetConst(CStr &param);	//jwz:add for SetConst

	long	PreviousPos; //Previous line pos
	long	CurrentPos;  //current postion for next fetchline.
	long	ContentLength; //source code line, include("a.mscr") is already inserted.

	//===================================

	// Remote control
	BOOL	SendSpecial( CStr &params );
    BOOL	SendKey( CStr &window, USHORT ch, BOOL shift=FALSE, BOOL ctrl=FALSE, BOOL alt=FALSE );
	BOOL	SendChar( CStr &window, USHORT ch );
    BOOL    SendSpecialKey( CStr &cmd, CStr &param );
    BOOL    SendCtrlKey( CStr &param );
    BOOL    SendKeys( CStr &param );
	BOOL    SendCommand( CStr &cmd, CStr &param, DWORD message );

    DWORD   MouseEvent( BYTE button, BOOL release );
    BOOL	MouseClick( CStr &param, BOOL dbl=FALSE, BYTE button=0 );
	BOOL	MouseDown( CStr &param, BYTE button=0 );
	BOOL	MouseUp( CStr &param, BYTE button=0 );
	
	// System

	// Variables
	BYTE 	Assignment( CStr &line , int AssignPos = -1);						// var = expr
    BOOL    Set( CStr &element );							// Set() command
	BOOL	SetVarToExpr( CValue *variable, CStr &expr, BOOL remAllowed );	// helper for both
    BOOL    SetErrorLevel( CStr &param );

    // Conditions and control structures
    BOOL    CheckCondition( CStr &param, BOOL &condRes );
    BOOL    CheckWndExists( CStr &element, BOOL &condRes );
    BOOL    CheckWndActive( CStr &element, BOOL &condRes );
    BOOL    CheckProcExists( CStr &element, BOOL &condRes );
    BOOL    CheckFileExists( CStr &element, BOOL dir, BOOL &condRes );
    BOOL    CheckQuestion( CStr &element, BOOL &condRes );
    BOOL    CheckScreen( CStr &element, BOOL &condRes );
    BOOL    CheckRegKeyExists( CStr &element, BOOL &condRes );
    BOOL    CheckRegKeyEqualsString( CStr &element, BOOL &condRes );
    BOOL    CheckRegKeyEqualsDWord( CStr &element, BOOL &condRes );
    BOOL    CheckEquals( CStr &element, BOOL &condRes );
    BOOL    CheckExpression( CStr &element, BOOL &condRes );

    void    PushCondStack( char type, long state );
    void    PopCondStack( char &type, long &state, BOOL remove=TRUE );

    BOOL    If( CStr &param );
    BOOL    ElseIf( CStr &param );
    BOOL    Else();
    BOOL    EndIf();
    
    BOOL    Switch( CStr &param );
    BOOL    Choice( CStr &param );
    BOOL    ChoiceDefault( CStr &param );
    BOOL    Case( CStr &param );
	BOOL	Default();
    BOOL    EndChoice();
    
    BOOL    While( CStr &param);
    BOOL    EndWhile( CStr &param);

    BOOL    Repeat( CStr &param);
    BOOL    EndRepeat( CStr &param);

    BOOL    For( CStr &param);
    BOOL    Next( CStr &param);

    BOOL    ForEach( CStr &param);
    BOOL    EndForEach( CStr &param);

    //BOOL    CallScript( CStr &param );
    //BOOL    Call( CStr &param);
    //BOOL    CallScriptFunction( CStr &param );
    //BOOL    CallFunction( CStr &param);
    BOOL    EndSub();

	BOOL	End(CStr &param);
	BOOL	Def(CStr &param);
	BOOL	Return( CStr &Expr );
//=============================================================
//=============================================================
//=============================================================
//=============================================================
//=============================================================
// below is from ExprParse.h
public:
  typedef struct _token
      { TCHAR *string;   // String, that identifies a token
        int   id;        // ID of the tokens
        BOOL  isAlpha;   // alphanumerical token? (i.e., no characters must follow. E.g. "x||y" compared to "xory")
      } token;

  static token TokenArray[];

public:
  // Zeichenklassen
  enum { CHAR_DIGIT       // digit
       , CHAR_ALPHA       // character (a-z,A-Z)
       , CHAR_DOT         // dot
       , CHAR_UNDERSCORE  // underscore
	   , CHAR_DOLLOR_VAR  // Only for RegEx Variable Use
       , CHAR_BACKSLASH   // backslash
	   , CHAR_STR_QUOTE	  // string
       , CHAR_QUOTE       // quotes
       , CHAR_WHITESPACE  // space/tab
       , CHAR_CR          // line break
       , CHAR_OTHERS      // everything else
	   , CHAR_OPR		  // Operator (+-*/...)
       , CHAR_END         // "\0", ende of expression
       };

  // Tokens
  enum { TOKEN_NUMBER     // 0Number
	   , TOKEN_CHAR		  // 1Char
       , TOKEN_STRING     // 2String (quoted)
	   , TOKEN_REF_START  // 3Start reference
	   , TOKEN_REF_END    // 4End reference
       , TOKEN_XOR        // 5XOR ^
       , TOKEN_OR         // 6OR |
       , TOKEN_LOR        // 7logical OR
       , TOKEN_AND        // 8AND &
       , TOKEN_LAND       // 9logical AND
       , TOKEN_LT         // 0<
       , TOKEN_LE         // 1<=
       , TOKEN_GT         // 2>
	   , TOKEN_LST		  // 3<<
	   , TOKEN_RST		  // 4>>
       , TOKEN_GE         // 5>=
       , TOKEN_EQ         // 6==
       , TOKEN_NE         // 7<> !=
#ifndef JWZ
       , TOKEN_LT_STR     // 8<
       , TOKEN_LE_STR     // 9<=
       , TOKEN_GT_STR     // 0>
       , TOKEN_GE_STR     // 1>=
       , TOKEN_EQ_STR     // 2=
       , TOKEN_NE_STR     // 3<>
#endif
       , TOKEN_D_PLUS     // 4++
       , TOKEN_PLUS       // 5+
       , TOKEN_D_MINUS    // 6--
       , TOKEN_MINUS      // 7-
       , TOKEN_MULT       // 8*
       , TOKEN_DIVIDE     // 9/
       , TOKEN_MODULO     // 0MOD %
       , TOKEN_POWER      // 1^
       , TOKEN_PATHCAT    // 2"\"
       , TOKEN_NOT        // 3NOT !
       , TOKEN_LNOT       // 4~
       , TOKEN_BR_OPEN    // 5(
       , TOKEN_BR_CLOSE   // 6)
       , TOKEN_COMMA      // 7,
	   , TOKEN_QUESTION   // 8?
	   , TOKEN_GLOBAL	  // 9:: Global
	   , TOKEN_COLON      // 0:
       , TOKEN_UNKNOWN    // 1something unknown
       , TOKEN_END        // 2"\0", end of expression
	   , TOKEN_DOT		  // 3.
	   , TOKEN_VARIABLE	  // 4
	   , TOKEN_TOCHAR	  // 5.c
	   , TOKEN_TOLONG	  // 6.L
	   , TOKEN_TODOUBLE	  // 7.D
	   , TOKEN_TOSTRING   // 8.S
	   , TOKEN_UCASE      // 9.UCASE
	   , TOKEN_LCASE      // 0.LCASE
	   , TOKEN_GETTYPE    // 1GetType()
	   , TOKEN_GETSTRLEN  // 2Length()
     };

protected:
  CStr			Expression;        // Entire text to parse
  int			ExprLen;		   // Expression Length/jwz
  int			Position;          // Current parse position in the expression
  int			Error;             // Current error code

  int			eErrorPos;		   // Save Error Position.jwz
  
  CStr			ErrorMessage;      // Error message
  int			Token;             // Recently found token
  int			pToken;            // previous token.jwz
  
  CValue * _Statement_Result;	   // Last statement result;jwz
  CValue * _LastVariable;		   // Last Variable;jwz

  //  static CMapStrToPtr  Functions;		 // Funktion name -> Pointer to the method 

public:
	long				GetParamSize(CStr line = L"", long begin = 0);
	CValue              EvaluateExpr( BOOL fixedEnd = TRUE );
	CValue				EvaluateExpression( CStr Expr, BOOL fixedEnd = TRUE );

	// Get error informations
	int                  GetError             ( void );
	LPCTSTR              GetErrorMessage      ( void );
	int                  GetErrorPosition     ( void );
		
		// Helpers
	int                  GetCharacterType     ( TCHAR character );

protected:
  // Helpers
  int                     CheckTokenLiterals   ( void );
  BOOL					  Expect			   ( int ExpectToken );
  void                    SkipSpaces           ( void );
  void                    NextToken            ( void );

  BOOL					  Compare              ( int      op
                                               , CValue  &par1
                                               , CValue  &par2
                                               );

  // Parse methods, corresponding to BNF
  CValue                  String               ( void );
  CValue                  Number               ( void );
  CValue				  VariableValue 	   ( LPCTSTR start );
  CValue                  Variable             ( void );
  CValue                  Function             ( void );
  CValue                  Reference            ( void );
  CValue				  Chars				   ( void );
  CValue                  Value                ( void );
  CValue                  IncDec               ( void );
  CValue                  Brackets             ( void );
  CValue                  Sign                 ( void );
  CValue                  Not                  ( void );
  CValue                  Power                ( void );
  CValue                  Mult                 ( void );
  CValue                  Calculate            ( void );
  CValue                  Compare              ( void );
  CValue                  Condition            ( void );
  CValue                  And                  ( void );
  CValue                  Logical              ( void );
  CValue                  Xor	               ( void );
  CValue                  Shift	               ( void );
  CValue				  Convert			   (CValue varVal);
//=============================================================
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERPRETER_H__06BDEA0C_49F8_41CF_A12A_8BCEB95E53E8__INCLUDED_)
