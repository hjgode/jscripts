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

//TokenArray add/modi more operator.
CInterpreter::token CInterpreter::TokenArray[] =
            { 
#ifndef JWZ
				{ L"NOT", TOKEN_NOT,      TRUE  }//0
            ,	{ L"AND", TOKEN_AND,      TRUE  }//1
            ,	{ L"OR",  TOKEN_OR,       TRUE  }//4
            ,	{ L"XOR",	TOKEN_XOR,      TRUE  }//7
            ,	{ L"LE",  TOKEN_LE_STR,   TRUE  }//19
            ,	{ L"GE",  TOKEN_GE_STR,   TRUE  }//20
            ,	{ L"NE",  TOKEN_NE_STR,   TRUE  }//21
            ,	{ L"LT",  TOKEN_LT_STR,   TRUE  }//22
            ,	{ L"GT",  TOKEN_GT_STR,   TRUE  }//23
            ,	{ L"EQ",  TOKEN_EQ_STR,   TRUE  }//24
            ,	{ L"MOD", TOKEN_MODULO,   TRUE  }//32
            , 
#endif			
				{ L"&&",  TOKEN_LAND,     FALSE }//2 add
            ,	{ L"&",	TOKEN_AND,     FALSE }//3
            ,	{ L"||",  TOKEN_LOR,      FALSE }//5 add
            ,	{ L"|",	TOKEN_OR,      FALSE }//6
            ,	{ L"^",	TOKEN_XOR,     FALSE }//8  add
            ,	{ L"<=",  TOKEN_LE,       FALSE }//9
            ,	{ L">=",  TOKEN_GE,       FALSE }//10
            ,	{ L"<>",  TOKEN_NE,       FALSE }//11
            ,	{ L"!=",  TOKEN_NE,       FALSE }//12 add
            ,	{ L"<<",  TOKEN_LST,      FALSE }//13
            ,	{ L">>",  TOKEN_RST,      FALSE }//14
            ,	{ L"<",   TOKEN_LT,       FALSE }//15
            ,	{ L">",   TOKEN_GT,       FALSE }//16
            ,	{ L"==",  TOKEN_EQ,       FALSE }//17 add
            ,	{ L"=",   TOKEN_EQ,       FALSE }//18
            ,	{ L"++",  TOKEN_D_PLUS,   FALSE }//25 add
            ,	{ L"+",   TOKEN_PLUS,     FALSE }//26
            ,	{ L"--",  TOKEN_D_MINUS,  FALSE }//27 add
            ,	{ L"-",   TOKEN_MINUS,    FALSE }//28
            ,	{ L"**",  TOKEN_POWER,    FALSE }//29 add
            ,	{ L"*",   TOKEN_MULT,     FALSE }//30
            ,	{ L"/",   TOKEN_DIVIDE,   FALSE }//31
            ,	{ L"%",   TOKEN_MODULO,   FALSE }//MODULO 33 add
            ,	{ L"\\",  TOKEN_PATHCAT,  FALSE }//34
            ,	{ L"(",   TOKEN_BR_OPEN,  FALSE }//37
            ,	{ L")",   TOKEN_BR_CLOSE, FALSE }//38
            ,	{ L",",   TOKEN_COMMA,    FALSE }//39
            ,	{ L"[",   TOKEN_REF_START,FALSE }
            ,	{ L"]",   TOKEN_REF_END,  FALSE }
            ,	{ L"?",   TOKEN_QUESTION, FALSE }
            ,	{ L"!",   TOKEN_NOT,      FALSE }
            ,	{ L"~",	TOKEN_LNOT,     FALSE }// add
            ,	{ L":",   TOKEN_COLON,    FALSE }
            ,	{ L"::",  TOKEN_STRCAT,   FALSE }//27 add
            ,	{ L".TYPE()", TOKEN_GETTYPE,TRUE  }// add
            ,	{ L".UCASE",TOKEN_UCASE,  TRUE  }// add
            ,	{ L".LCASE",TOKEN_LCASE,  TRUE  }// add			
            ,	{ L".UPPER",TOKEN_UCASE,  TRUE  }// add
            ,	{ L".LOWER",TOKEN_LCASE,  TRUE  }// add			
            ,	{ L".TOCHAR",		TOKEN_TOCHAR,   TRUE  } //add
            ,	{ L".TOINT",		TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".TOLONG",		TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".TOFLOAT",	TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".TODOUBLE",	TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".TOSTRING",	TOKEN_TOSTRING, TRUE  } //add
            ,	{ L".TOSTR",		TOKEN_TOSTRING, TRUE  } //add
            ,	{ L".TOC",  TOKEN_TOCHAR,   TRUE  } //add
            ,	{ L".TOI",  TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".TOL",  TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".TOF",  TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".TOD",  TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".TOS",  TOKEN_TOSTRING, TRUE  } //add
            ,	{ L".C",  TOKEN_TOCHAR,   TRUE  } //add
            ,	{ L".I",  TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".L",  TOKEN_TOLONG,   TRUE  } //add
            ,	{ L".F",  TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".D",  TOKEN_TODOUBLE, TRUE  } //add
            ,	{ L".S",  TOKEN_TOSTRING, TRUE  } //add
			,	{ L".LENGTH()", TOKEN_GETSTRLEN, TRUE }
            ,	{ L".",   TOKEN_DOT,      FALSE } //36
            ,	{ L"",    TOKEN_END,      FALSE }  // "End of list" marker!
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

  switch ( character )
  {
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
  case L'+': case L'-': case L'*': case L'/': case L'%':
  case L'&': case L'|': case L'^': case L'<': case L'>':
  case L'=': case L'!': case L'~':
	rc = CHAR_OPR;
	break;
  //jwz:add end

  case L'\0':
    rc = CHAR_END;
    break;
  }

  if ( rc == CHAR_OTHERS )
  {
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

   for ( i = 0; TokenArray[i].id != TOKEN_END; i++ )
  {

	if ( Expression.Mid( Position, _tcslen( TokenArray[i].string ) ).CompareNoCase( TokenArray[i].string ) == 0 )
    {
      tokenok = TRUE;

      if ( TokenArray[i].isAlpha == TRUE )
      {
        // Calculate the position after the found token string
        int pos = Position + _tcslen( TokenArray[i].string );

        // What follows?
        switch ( GetCharacterType( Expression[pos] ) )
        {
        case CHAR_ALPHA:
        case CHAR_DIGIT:
        case CHAR_UNDERSCORE:
          tokenok = FALSE;
          break;
        }
      }

      if ( tokenok == TRUE )
      {
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
CInterpreter::SkipSpaces( void )
{
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
  if ( Token == TOKEN_UNKNOWN )
  {
    // So, what do we have here?
    switch ( GetCharacterType( Expression[Position] ) )
    {
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
		if (ch == Arr[i]) return true;
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
	  case TOKEN_GETTYPE:
		  Debug((CStr)rc,rc.GetType());
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
		case TOKEN_GETSTRLEN:
			if (rc.GetType() == VALUE_STRING){
				rc = (long)((CStr)rc).GetLength();
			}
			break;
	  default:
		  tStr = (CStr)rc;
		  tStr.MakeLower();
		  rc = tStr;
		  break;
	  }
	  NextToken();
  }

  return rc;
}

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

  //jwz:add
  if (Token == TOKEN_AND && pToken != TOKEN_UNKNOWN){ 
	 varName = L"&"+varName;
	 GlobalPos = 1;
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
	
  if ( Error == 0 )
  {
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
	 while ( Token != TOKEN_BR_CLOSE && Error == 0 )
	 {
		CValue param = Logical();
		if ( Error == 0 )
			params.Add( param );

		if ( Error == 0 && Token != TOKEN_COMMA && Token != TOKEN_BR_CLOSE )
		{
			Error        = 4;
			ErrorMessage = L"Comma or closing parantheses expected";
		}

		if ( Error == 0 && Token == TOKEN_COMMA )
			NextToken();
	 }

	 if ( Error == 0 )
	 {
		
		 functionPointer function = GetFunction( funcName );
		 //jwz:add for function call.
		 if (IsUserFunc(funcName)){
			 rc = EvalFunc(funcName, params);
		 }else //jwz:add end
			 if ( function != NULL ) {
			 rc = function(params, Error, ErrorMessage );

			 if ( Token != TOKEN_BR_CLOSE )
			 {
			   // Klammer wurde nicht geschlossen
			   Error        = 4;
			   ErrorMessage = L"Missing closing bracket";
			   eErrorPos     = pos;  // Fehler-Position ist bei der öffnenden Klammer
			 }
		 }else {
			if ( ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Unknown function, please check it!" + GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND  );
			exit(1);
		 }
		 NextToken();
	 }
  }
  else
  {
	 funcName = Expression.Mid( startPos, pos-startPos );

	 //jwz:add for &varName as global variable
	 //jwz:add end
	 if ( Error == 0 )
	 {
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

	if ( Error != 0 )
	{
		return rc;
	}

	NextToken();                  // Skip "["
	rc = Logical();

    // Check for closing ]
    if ( Error == 0 )
    {
		if ( Token != TOKEN_REF_END )
		{
			// Missing ]
			Error        = 4;
			ErrorMessage = L"Missing closing bracket";
		}
		else
		{
			CStr varName = (CStr)rc;
			if ( varName.IsEmpty() )
			{
				Error        = 4;
				ErrorMessage = L"Reference is empty";
			}
			else
			{
				int pos=0;
				CValue *varVal = GetVariable( varName, FALSE, &pos );
				
				if ( pos == -1 )
				{
					Error        = 2;
					ErrorMessage = L"Error in referred variable: " + VarError;
				}
				else if ( pos < varName.GetLength() )
				{
					Error        = 2;
					ErrorMessage = L"Error in referred variable: Invalid characters";
				}
				else if ( varVal != NULL )
				{
					rc = *varVal;
				}
				else
				{
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
	if (Error != 0){
		return rc;
	}
	
	if (Expression[Position] == L'\''){
		Position++;
		rc = (TCHAR)Expression[Position++];
	}

	if (Expression[Position++]!=L'\''){
		Error        = 1;
		ErrorMessage = L"Char not closed (missing Single quote)";
	}
  
	if ( Error == 0 )
	{
		NextToken();                         // Get next token
	}
	return rc;
}

CValue
CInterpreter::Value( void )
{
  CValue rc;

  if ( Error != 0 )
  {
    return rc;
  }

  switch ( Token )
  {
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
  case TOKEN_AND:
	rc = Function();
	break;

  default:
	Error        = 3; // Value expected
	ErrorMessage = L"Operator found where value expected";
  }

  rc = Convert(rc);

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

  if ( Error != 0 )
  {
    return rc;
  }
	
  switch ( Token )
  {
  case TOKEN_BR_OPEN:             // Klammer auf
    saveErrorPos = eErrorPos;      // Position der Klammer merken
    NextToken();                  // Klammer übergehen
    rc = Logical();
    // Nächstes Token muß ")" sein.
    if ( Error == 0 )
    {
      if ( Token != TOKEN_BR_CLOSE )
      {
        // Klammer wurde nicht geschlossen
        Error        = 4;
        ErrorMessage = L"Missing closing bracket";
        eErrorPos     = saveErrorPos;  // Fehler-Position ist bei der öffnenden Klammer
      }
      else
      {
        NextToken();              // ")" überspringen
		rc = Convert(rc);
      }
    }
    break;

  case TOKEN_BR_CLOSE:            // Klammer zu
    // fehlende öffnende Klammer
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
  CValue rc;

  if ( Error != 0 )
  {
    return rc;
  }

  switch ( Token )
  {
  case TOKEN_MINUS:    // "-"
    NextToken();
    rc = Brackets();

    // Kein Fehler aufgetreten?
    if ( Error == 0 )
    {
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

  if ( Error != 0 )
  {
    return rc;
  }

  switch ( Token )
  {
  case TOKEN_NOT:    // "NOT"
  case TOKEN_LNOT:	//"~"
	tk=Token;
    NextToken();
    rc = Sign();
    // Kein Fehler aufgetreten?
    if ( Error == 0 )
    {
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

  if ( Error != 0 )
  {
    return rc;
  }

  rc = Not();

  // Der erste Wert von Operationen ist gleichzeitig der Rückgabewert:
  val1 = rc;

  while ( Error == 0 && cont == TRUE )
  {
    switch ( Token )
    {
    case TOKEN_POWER:
      NextToken();
      val2 = Not();
      if ( Error == 0 )
      {
	      if ( val1.IsDouble() || val2.IsDouble() )
		  {
		    double dRc   = pow( (double)val1, (double)val2 );
		    rc = dRc;
		  }
		  else
		  {
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

  if ( Error != 0 )
  {
    return rc;
  }

  rc = Power();

  // Der erste Wert von Operationen ist gleichzeitig der Rückgabewert:
  val1 = rc;

  while ( Error == 0 && cont == TRUE )
  {
    switch ( Token )
    {
    case TOKEN_MULT:
      NextToken();
      val2 = Power();
      if ( Error == 0 )
      {
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

        // Für verkettete Operationen ("5 * 2 / 3") wird der erste Wert
        // auf das Rückgabe-Objekt gesetzt.
        val1 = rc;
      }
      break;

    case TOKEN_DIVIDE:
      NextToken();
      val2 = Power();
      if ( Error == 0 )
      {
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
      if ( Error == 0 )
      {
        if ( (long)val2 == 0 )
        {
          Error        = 6;
          ErrorMessage = L"Division by zero (or invalid string?)";
        }
        else
        {
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

  if ( Error != 0 )
  {
    return rc;
  }

  rc = Mult();

  // Der erste Wert von Operationen ist gleichzeitig der Rückgabewert:
  val1 = rc;

  while ( cont == TRUE && Error == 0 )
  {
    switch ( Token )
    {
    case TOKEN_PLUS:
    case TOKEN_STRCAT:
	case TOKEN_PATHCAT:
    case TOKEN_MINUS:
      opToken = Token;
      NextToken();
      val2 = Mult();

      if ( Error == 0 )
      {
  	    if (opToken == TOKEN_PATHCAT){
				CStr str1 = val1;
				CStr str2 = val2;
				str1.TrimRight( '\\' );
				str2.TrimLeft( '\\' );
				rc = str1 + L"\\" + str2;
        }
		else if (opToken == TOKEN_STRCAT)
		{
		    rc = (CStr) val1 + (CStr) val2;
        }
		else if ( opToken == TOKEN_PLUS )
        {
			//if (!val1.IsValidNumber() || !val2.IsValidNumber()){
			if (val1.GetType()==VALUE_STRING || val2.GetType() == VALUE_STRING){
				rc = (CStr) val1 + (CStr) val2;
			}else if ( val1.IsDouble() || val2.IsDouble() ){
				rc = (double)val1 + (double)val2;
			}else{
				rc = (long)val1 + (long)val2;
			}
        }
        else
        {
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
			}
			else
			{
				rc = (long)val1 - (long)val2;
			}
        }

        // Für verkettete Operationen ("5 * 2 / 3") wird der erste Wert
        // auf das Rückgabe-Objekt gesetzt.
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

	if ( Error != 0 )
	{
	    return rc;
	}
	
	rc = Calculate();

	// First value of operations is the current return value
	val1 = rc;

	while ( cont == TRUE && Error == 0 )
	{
		switch ( Token )
		{
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

  if ( Error != 0 )
  {
    return rc;
  }

  rc = Shift();

  errPos = ErrorPos;

  val1 = rc;

  if ( Error == 0 )
  {
    switch ( Token )
    {
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

  if ( Error != 0 )
  {
    return val1;
  }

  rc = val1 = Compare();

  while ( cont == TRUE && Error == 0 )
  {
    if ( Token == TOKEN_XOR || Token == TOKEN_OR ||  Token == TOKEN_AND && pToken == TOKEN_UNKNOWN )
    {
      int tk = Token;
      NextToken();
      val2 = Compare();

      if ( Error == 0 )
      {
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
    }
    else
    {
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

  if ( Error != 0 )
  {
    return val1;
  }

  rc = val1 = Xor();

  while ( cont == TRUE && Error == 0 )
  {
    if ( Token == TOKEN_LAND )
    {
      int tk = Token;
      NextToken();
      val2 = Xor();
      if ( Error == 0 )
      {
        long lRc;
        lRc = (long)val1 && (long)val2;
        rc = lRc;
        val1 = rc;
      }
    }
    else
    {
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

  if ( Error != 0 )
  {
    return val1;
  }

  rc = val1 = And();

  while ( cont == TRUE && Error == 0 )
  {
    if ( Token == TOKEN_LOR )
    {
      int tk = Token;
      NextToken();
      val2 = And();

      if ( Error == 0 )
      {
        long lRc;
        lRc = (long)val1 || (long)val2;
        rc = lRc;
        val1 = rc;
      }
    }
    else
    {
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

	if ( Error != 0 )
	{
		return val1;
	}

	rc = Logical();

    if ( Error == 0 && Token == TOKEN_QUESTION )
    {
		NextToken();
		val1 = Logical();
		if ( Error == 0 )
		{
			if ( Token != TOKEN_COLON )
			{
				 Error        = 8;
				 ErrorMessage = L"Missing ':' for '?' operator";
			}
			else
			{
				NextToken();
				val2 = Compare();

				if ( Error == 0 )
				{
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
  }
  else
  {
    rc = Condition();

    if ( Error == 0 && fixedEnd && Token != TOKEN_END )
    {
        if ( Token == TOKEN_BR_CLOSE )
		{
		  Error        = 5;
		  ErrorMessage = L"Missing opening bracked";
		}
        else
		{
          Error        = 7;
          ErrorMessage = L"Operator expected";
		}
    }
  }

  return rc;
}

