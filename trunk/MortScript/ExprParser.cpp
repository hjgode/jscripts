#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#include "morttypes.h"
#endif

#include <string.h>

#include "variables.h"
#include "exprparser.h"
#include "helpers.h"
#include "math.h"

CInterpreter::token CInterpreter::TokenArray[] =
            { { L"NOT", TOKEN_NOT,      TRUE  }
            , { L"AND", TOKEN_AND,      TRUE  }
            , { L"&&",  TOKEN_LAND,     FALSE }
            , { L"&",	TOKEN_AND1,     FALSE }
            , { L"OR",  TOKEN_OR,       TRUE  }
            , { L"||",  TOKEN_LOR,      FALSE }
            , { L"|",	TOKEN_OR1,      FALSE }
            , { L"XOR",	TOKEN_XOR,      TRUE  }
            , { L"^",	TOKEN_XOR1,     FALSE }
            , { L"<=",  TOKEN_LE,       FALSE }
            , { L">=",  TOKEN_GE,       FALSE }
            , { L"<>",  TOKEN_NE,       FALSE }
            , { L"!=",  TOKEN_NE1,      FALSE }
            , { L"<<",  TOKEN_LST,      FALSE }
            , { L">>",  TOKEN_RST,      FALSE }
            , { L"<",   TOKEN_LT,       FALSE }
            , { L">",   TOKEN_GT,       FALSE }
            , { L"==",  TOKEN_EQ1,      FALSE }
            , { L"=",   TOKEN_EQ,       FALSE }
            , { L"LE",  TOKEN_LE_STR,   TRUE  }
            , { L"GE",  TOKEN_GE_STR,   TRUE  }
            , { L"NE",  TOKEN_NE_STR,   TRUE  }
            , { L"LT",  TOKEN_LT_STR,   TRUE  }
            , { L"GT",  TOKEN_GT_STR,   TRUE  }
            , { L"EQ",  TOKEN_EQ_STR,   TRUE  }
            , { L"+",   TOKEN_PLUS,     FALSE }
            , { L"-",   TOKEN_MINUS,    FALSE }
            , { L"**",  TOKEN_POWER,    FALSE }
            , { L"*",   TOKEN_MULT,     FALSE }
            , { L"/",   TOKEN_DIVIDE,   FALSE }
            , { L"MOD", TOKEN_MODULO,   TRUE  }
            , { L"%",   TOKEN_MODULO1,  FALSE } //MODULO
            , { L"\\",  TOKEN_PATHCAT,  FALSE }
            , { L"..",  TOKEN_STRCAT,   FALSE }
            , { L".",   TOKEN_DOT,      FALSE }
            , { L"(",   TOKEN_BR_OPEN,  FALSE }
            , { L")",   TOKEN_BR_CLOSE, FALSE }
            , { L",",   TOKEN_COMMA,    FALSE }
            , { L"[",   TOKEN_REF_START,FALSE }
            , { L"]",   TOKEN_REF_END,  FALSE }
            , { L"?",   TOKEN_QUESTION, FALSE }
            , { L"!",   TOKEN_NOT1,     FALSE }
            , { L"~",	TOKEN_LNOT,     FALSE }
            , { L":",   TOKEN_COLON,    FALSE }
            , { L"",    TOKEN_END,      FALSE }  // "End of list" marker!
            };


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

	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) < 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'lt' instead.";
	}
	else
	    rc = ( (double)par1 < (double)par2 );
    break;

  case TOKEN_LE:
	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) <= 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'le' instead.";
	}
	else
	    rc = ( (double)par1 <= (double)par2 );
    break;

  case TOKEN_GT:
	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) > 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'gt' instead.";
	}
	else
	    rc = ( (double)par1 > (double)par2 );
    break;

  case TOKEN_GE:
	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) >= 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'ge' instead.";
	}
	else
	    rc = ( (double)par1 >= (double)par2 );
    break;

  case TOKEN_EQ:
  case TOKEN_EQ1:
	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) == 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'eq' instead.";
	}
	else
		rc = ( (double)par1 == (double)par2 );
    break;

  case TOKEN_NE:
  case TOKEN_NE1:
	if ( !par1.IsValidNumber() || !par2.IsValidNumber() )
	{
		rc = ( ((CStr)par1).Compare( (CStr)par2 ) != 0 );
		//break;
		//Error        = 3;
		//ErrorMessage = L"Comparing two string values with numerical operator. Use 'ne' instead.";
	}else
	    rc = ( (double)par1 != (double)par2 );
    break;

  case TOKEN_LT_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) < 0 );
    break;

  case TOKEN_LE_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) <= 0 );
    break;

  case TOKEN_GT_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) > 0 );
    break;

  case TOKEN_GE_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) >= 0 );
    break;

  case TOKEN_EQ_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) == 0 );
    break;

  case TOKEN_NE_STR:
    rc = ( ((CStr)par1).Compare( (CStr)par2 ) != 0 );
    break;

  default:
    rc = 0;
    ASSERT( FALSE );
  }

  return rc;
}



int
CInterpreter::GetCharacterType( TCHAR character )
{
  int rc = CHAR_OTHERS;

  //Debug(Expression,L"Expression Parser");

  switch ( character )
  {
  case ' ':
  case '\t':
    rc = CHAR_WHITESPACE;
    break;

  case '\r':
  case '\n':
    rc = CHAR_CR;
    break;

  case '_':
    rc = CHAR_UNDERSCORE;
    break;

  case '$':
    rc = CHAR_DOLLOR_VAR;
    break;

  case '.':
    rc = CHAR_DOT;
    break;

  case '\\':
    rc = CHAR_BACKSLASH;
    break;

  case '\"':
  case '\'':
    rc = CHAR_QUOTE;
    break;

  case '\0':
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
  //Debug(Expression,L"Expression Parser");

  if ( Position >= Expression.GetLength() )
      return TOKEN_END;

   for ( i = 0; TokenArray[i].id != TOKEN_END; i++ )
  {
	
	//MessageBox(NULL,TokenArray[i].string,L"Token",MB_SETFOREGROUND);
	//Debug(Expression.Mid( Position, _tcslen( TokenArray[i].string ) ),TokenArray[i].string);

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
	    //CStr msg;
		//msg.Format( L"---Token '%d'", (int)rc );

		//MessageBox(NULL,TokenArray[i].string+msg,L"Token",MB_SETFOREGROUND);
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
  //Debug(Expression,L"Expression Parser");

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
  //Debug(Expression,L"Expression Parser");

  // If an error occurs, it's probably here, so let's save this position.
  // If necessary, it can be corrected later on.
  ErrorPos = Position;

  // Now let's check all token literals
  Token = CheckTokenLiterals();

  //MessageBox(NULL,TokenArray[Token].string,L"Split E",MB_SETFOREGROUND);

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
    case CHAR_QUOTE:
      Token = TOKEN_STRING;
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



CValue
CInterpreter::Number( void )
{
  int                relpos;
  BOOL               dotfound = FALSE;
  BOOL               cont = TRUE;
  CValue             rc;
  //Debug(Expression,L"Expression Parser");

#ifdef _DEBUG
  int check = GetCharacterType( Expression[Position] );
  ASSERT( check == CHAR_DIGIT || check == CHAR_DOT );
#endif

  for ( relpos = 0; cont == TRUE; relpos++ )
  {
    if ( Position+relpos == Expression.GetLength() )
    {
      relpos++;
      break;
    }

    switch ( GetCharacterType( Expression[ Position + relpos ] ) )
    {
    case CHAR_DIGIT:
      // digits are fine, let's continue...
      break;

    case CHAR_DOT:
      if ( dotfound == FALSE )
      {
		// Remember there's been a dot
        dotfound = TRUE;
      }
      else
      {
        // Oh, wait, a number with two decimals???
        // However, the error should be shown for the second dot, so we just stop here.
		// The second dot then will be tried to be parsed as next token, i.e. an operator...
        cont = FALSE; // Exit loop
      }
      break;

    default:  // Anything else (probably an operator): end of number
      cont = FALSE; // Exit loop
    }
  }

  CValue tmpVal;
  tmpVal = (LPCTSTR)Expression.Mid( Position, relpos-1 );
  if ( dotfound )
	  rc = (double)tmpVal;
  else
	  rc = (long)tmpVal;
  
  Position += relpos-1;               // Parse-Position weitersetzen

  NextToken();                        // Get next token

  return rc;
}



CValue
CInterpreter::String( void )
{
  CValue             rc;
  int                pos, startPos;
  //Debug(Expression,L"Expression Parser");

#ifdef _DEBUG
  // This function is only invoked if TOKEN_STRING is found,
  // so the next character should be a quote...
  int check = GetCharacterType( Expression[Position] );
  ASSERT( check == CHAR_QUOTE );
#endif

  TCHAR quote = Expression[Position];

  pos = startPos = Position+1; // skip opening quote
  
  pos = Expression.Find( quote, pos );

  while ( pos != -1 && pos < Expression.GetLength()-2 )
  {
    int nextQuote = Expression.Find( quote, pos+1 );
    if ( nextQuote != pos+1 )
        break;
    else
        pos = Expression.Find( quote, pos+2 );
  }

  if ( pos != -1 )
  {
	 // replace special characters
     CStr str = Expression.Mid( startPos, pos-startPos );
	 str = SlashChar (str);
	 /*if ( quote == '\"' )
	     str.Replace( L"\"\"", L"\"" );
	 if ( quote == '\'' )
		 str.Replace( L"\'\'", L"\'" );
	 
	 //MessageBox(NULL,str,L"String",MB_SETFOREGROUND);
	 str.Replace(L"\\r",L"\r");
	 str.Replace(L"\\n",L"\n");
	 str.Replace(L"\\t",L"\t");
	 str.Replace(L"\\\"",L"\"");
	 str.Replace( L"^CR^", L"\r" );
     str.Replace( L"^LF^", L"\n" );
     str.Replace( L"^NL^", L"\r\n" );
     str.Replace( L"^TAB^", L"\t" );
	 str.Replace( L"^QUOT^", L"\"" );*/

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
CInterpreter::VariableValue( LPCTSTR start )
{
  CValue rc;
  //Debug(Expression,L"Expression Parser");

  SkipSpaces();
  
  int pos=0;
  CStr varName;
  if ( start != NULL )
	varName = start + Expression.Mid( Position );
  else
	varName = Expression.Mid( Position );
  //Debug(Expression,varName);
  CValue *varVal = ::GetVariable( varName, FALSE, &pos );
  if ( pos == -1 )
  {
     Error        = 2;
     ErrorMessage = VarError;
  }
  else
  {
	  Position += pos;
	  if ( start != NULL ) Position -= wcslen( start );
  }

  if ( varVal != NULL )
  {
	 rc = *varVal;
  }
  
  return rc;
}


CValue
CInterpreter::Variable( void )
{
  CValue             rc;
  //Debug(Expression,L"Expression Parser");

  int check = GetCharacterType( Expression[Position] );
#ifdef _DEBUG
  ASSERT( check == CHAR_ALPHA || check == CHAR_UNDERSCORE || check == CHAR_DOLLOR_VAR);
#endif


  rc = VariableValue(NULL);
  //Debug(Expression,L"Expression Parser");
	
  if ( Error == 0 )
  {
    NextToken();
    if ( Token != TOKEN_MODULO1 ) // Closing "%"
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
  CValue             rc;
  long                pos, startPos;
  //Debug(Expression,L"Expression Parser");

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
  // Skip Regex variable name characters
  if (Expression[pos]=='$'){
 	if (   Expression[pos+1] == '&' 
		|| Expression[pos+1] == '`' 
		|| Expression[pos+1] == '\'' 
		|| Expression[pos+1] == '+' 
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

  CStr funcName = Expression.Mid( startPos, pos-startPos );
  Position = pos;
  SkipSpaces();
  pos = Position;

  // pos = startPos;
  // pos = Expression.Find( '(', pos );

  // Function!
  if ( pos < Expression.GetLength() && Expression[pos] == '(' )
  {
	 funcName.MakeLower();
     BOOL funcFound = FALSE;
	 Position = pos+1;

	 NextToken();
	 CValueArray params;
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
		
		 //Debug(funcName,L"Function()");

		 functionPointer function = GetFunction( funcName );
		 if ( function != NULL )
		 {
			 rc = function(params, Error, ErrorMessage );

			 if ( Token != TOKEN_BR_CLOSE )
			 {
			   // Klammer wurde nicht geschlossen
			   Error        = 4;
			   ErrorMessage = L"Missing closing bracket";
			   ErrorPos     = pos;  // Fehler-Position ist bei der öffnenden Klammer
			 }
		 }else{
			Error        = 3; // Wert erwartet
			ErrorMessage = L"Unknown function " + funcName;
		 }
	 }
  }
  else
  {
	 funcName = Expression.Mid( startPos, pos-startPos );
	 //Debug(funcName);
	 rc = VariableValue(funcName);
  }

  // Position = pos + 1;

  if ( Error == 0 )
  {
    NextToken();                         // Get next token
  }
//MessageBox(NULL,(CStr)rc,L"Value2",MB_SETFOREGROUND);
  return rc;
}

CValue
CInterpreter::Reference( void )
{
	CValue    rc;
  //Debug(Expression,L"Expression Parser");

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
				CValue *varVal = ::GetVariable( varName, FALSE, &pos );
				
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
CInterpreter::Value( void )
{
  CValue rc;

  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return rc;
  }
	
  switch ( Token )
  {
  case TOKEN_DOT:		// Decimal point (.123)
	Position--;			// include the dot
  case TOKEN_NUMBER:     // Any digit
	  //MessageBox(NULL,L"Dot or Number",L"Value",MB_SETFOREGROUND);
    rc = Number();
    break;

  case TOKEN_STRING:     // String
	//MessageBox(NULL,L"String",L"Value",MB_SETFOREGROUND);
    rc = String();
    break;

  case TOKEN_MODULO1:   // Variable (with %..%)
	  //MessageBox(NULL,L"Variable",L"Value",MB_SETFOREGROUND);
    rc = Variable();
	
    break;

  case TOKEN_REF_START:  // Referred variable ([...])
	  //MessageBox(NULL,L"Referance",L"Value",MB_SETFOREGROUND);
    rc = Reference();
    break;

  case TOKEN_UNKNOWN:	// Text (Function/Variable)
	  //MessageBox(NULL,L"Function",L"Value",MB_SETFOREGROUND);
	rc = Function();
	break;

  default:
	  //MessageBox(NULL,L"Default",L"Value",MB_SETFOREGROUND);
	Error        = 3; // Value expected
	ErrorMessage = L"Operator found where value expected";
  }

  return rc;
}



CValue
CInterpreter::Brackets( void )
{
  int       saveErrorPos;
  CValue    rc;
  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return rc;
  }
	
  switch ( Token )
  {
  case TOKEN_BR_OPEN:             // Klammer auf
    saveErrorPos = ErrorPos;      // Position der Klammer merken
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
        ErrorPos     = saveErrorPos;  // Fehler-Position ist bei der öffnenden Klammer
      }
      else
      {
        NextToken();              // ")" überspringen
      }
    }
    break;

  case TOKEN_BR_CLOSE:            // Klammer zu
    // fehlende öffnende Klammer
    Error        = 5;
    ErrorMessage = L"Missing opening bracked";

  default:
    rc = Value();  // Ist's keine Klammer, muss es ein Wert sein
	//MessageBox(NULL,(CStr)rc,L"Brackets",MB_SETFOREGROUND);
  }

  return rc;
}


CValue
CInterpreter::Sign( void )
{
  CValue rc;
  //Debug(Expression,L"Expression Parser");

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
  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return rc;
  }

  switch ( Token )
  {
  case TOKEN_NOT:    // "NOT"
  case TOKEN_NOT1:   // "!"
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
  //Debug(Expression,L"Expression Parser");

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
  //Debug(Expression,L"Expression Parser");

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
		if (!val1.IsValidNumber() && val2.IsValidNumber()){
			rc=L"";
			for (i = 0; i< (long) val2; i++) rc=(CStr)rc+(CStr) val1;
		}else if(val1.IsValidNumber() && !val2.IsValidNumber()){
			rc=L"";
			for (i = 0; i< (long) val1; i++) rc=(CStr)rc+(CStr) val2;
		}else if ( val1.IsDouble() || val2.IsDouble() ){
		  double dRc   = (double)val1 * (double)val2;
		  rc = dRc;
		}
		else
		{
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
		  if (!val1.IsValidNumber()){
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
    case TOKEN_MODULO1:
	//case TOKEN_VARIABLE:
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
  //Debug(Expression,L"Expression Parser");

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
    case TOKEN_MINUS:
      opToken = Token;
      NextToken();
      val2 = Mult();

      if ( Error == 0 )
      {
        if ( opToken == TOKEN_PLUS )
        {
			if (!val1.IsValidNumber() || !val2.IsValidNumber()){
				rc = (CStr) val1 + (CStr) val2;
			}else if ( val1.IsDouble() || val2.IsDouble() ){
				rc = (double)val1 + (double)val2;
			}else{
				rc = (long)val1 + (long)val2;
			}
        }
        else
        {
			if (!val1.IsValidNumber() || !val2.IsValidNumber()){ //"Wolf&Lion SoftLion OK"-"Lion" -->"Wolf& SoftLion OK"
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
    //Debug(Expression,L"Expression Parser");

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
CInterpreter::AddString( void )
{
	BOOL      cont = TRUE;      
	int       opToken;
	CValue    rc;
	CValue    val1;
	CValue    val2;
  //Debug(Expression,L"Expression Parser");

	if ( Error != 0 )
	{
	    return rc;
	}
	
	rc = Shift();

	// First value of operations is the current return value
	val1 = rc;

	while ( cont == TRUE && Error == 0 )
	{
		switch ( Token )
		{
		case TOKEN_STRCAT:
		case TOKEN_DOT:
		case TOKEN_PATHCAT:
			opToken = Token;

			NextToken();

			val2 = Shift();

			if ( opToken == TOKEN_PATHCAT )
			{
				CStr str1 = val1;
				CStr str2 = val2;
				str1.TrimRight( '\\' );
				str2.TrimLeft( '\\' );
				rc = str1 + L"\\" + str2;
			}
			else
			{
				rc = (CStr)val1 + (CStr)val2;
			}

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
CInterpreter::Xor( void )
{
  BOOL      cont = TRUE;
  CValue    rc;
  CValue    val1;
  CValue    val2;
  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return val1;
  }

  rc = val1 = AddString();

  while ( cont == TRUE && Error == 0 )
  {
    if ( Token == TOKEN_XOR || Token == TOKEN_XOR1 || Token == TOKEN_OR || Token == TOKEN_OR1 ||  Token == TOKEN_AND  || Token == TOKEN_AND1 )
    {
      int tk = Token;
      NextToken();
      val2 = AddString();

      if ( Error == 0 )
      {
        long lRc;
		switch (tk){
		case TOKEN_XOR:
		case TOKEN_XOR1:
			lRc = (long)val1 ^ (long)val2;
			break;
		case TOKEN_OR:
		case TOKEN_OR1:
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
CInterpreter::Compare( void )
{
  int       compare;
  int       errPos;
  BOOL      result;
  CValue    rc;
  CValue    val1;
  CValue    val2;
  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return rc;
  }

  rc = Xor();

  errPos = ErrorPos;

  val1 = rc;

  if ( Error == 0 )
  {
    switch ( Token )
    {
      case TOKEN_EQ:
      case TOKEN_EQ1:
      case TOKEN_LT:
      case TOKEN_LE:
      case TOKEN_GT:
      case TOKEN_GE:
      case TOKEN_NE:
      case TOKEN_NE1:
      case TOKEN_EQ_STR:
      case TOKEN_LT_STR:
      case TOKEN_LE_STR:
      case TOKEN_GT_STR:
      case TOKEN_GE_STR:
      case TOKEN_NE_STR:
        compare = Token;
        NextToken();
        val2 = Xor();

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
CInterpreter::Condition( void )
{
	BOOL      cont = TRUE;
	CValue    rc;
	CValue    val1;
	CValue    val2;
  //Debug(Expression,L"Expression Parser");

	if ( Error != 0 )
	{
		return val1;
	}

	rc = Compare();

    if ( Error == 0 && Token == TOKEN_QUESTION )
    {
		NextToken();
		val1 = Compare();
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
CInterpreter::And( void )
{
  BOOL      cont = TRUE;
  CValue    rc;
  CValue    val1;
  CValue    val2;
  //Debug(Expression,L"Expression Parser");

  if ( Error != 0 )
  {
    return val1;
  }

  rc = val1 = Condition();

  while ( cont == TRUE && Error == 0 )
  {
    if ( Token == TOKEN_LAND )
    {
      int tk = Token;
      NextToken();
      val2 = Condition();
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
  //Debug(Expression,L"Expression Parser");

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



CInterpreter::CExprParser( LPCTSTR expression )
{
  ASSERT( expression != NULL );

  Expression = expression;

  Position = 0;
  Error    = 0;
}

CInterpreter::CExprParser( LPCTSTR expression ,CStr xContent)
{
  ASSERT( expression != NULL );

  Expression = expression;

  Content = xContent;

  //DERROR( Content );

  Position = 0;
  Error    = 0;
}

CInterpreter::~CExprParser( void )
{
}



CValue
CInterpreter::EvaluateExpression( BOOL fixedEnd )
{
  CValue rc;

  Position   = 0;
  Error      = 0;
  //Debug(Expression,L"Expression Parser");

  // Erstes Token ermitteln
  NextToken();

  if ( Token == TOKEN_END || (!fixedEnd && Token == TOKEN_COMMA) ) // Leerer Ausdruck
  {
    rc.Clear();
  }
  else
  {
    rc = Logical();//Xor();

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
  return ErrorPos;
}