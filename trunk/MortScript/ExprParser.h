#ifndef __EXPRPARSER_H__
#define __EXPRPARSER_H__

#include "morttypes.h"

class CExprParser
{
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
       , CHAR_QUOTE       // quotes
       , CHAR_WHITESPACE  // space/tab
       , CHAR_CR          // line break
       , CHAR_OTHERS      // everything else
       , CHAR_END         // "\0", ende of expression
       };

  // Tokens
  enum { TOKEN_NUMBER     // Number
       , TOKEN_STRING     // String (quoted)
       , TOKEN_MODULO1    // Variable(MODULO1)
	   , TOKEN_REF_START  // Start reference
	   , TOKEN_REF_END    // End reference
       , TOKEN_XOR        // XOR 
       , TOKEN_XOR1       // XOR1 ^ 
       , TOKEN_OR         // OR
       , TOKEN_OR1        // OR |
       , TOKEN_LOR        // logical OR
       , TOKEN_AND        // AND
       , TOKEN_AND1       // AND &
       , TOKEN_LAND       // logical AND
       , TOKEN_LT         // <
       , TOKEN_LE         // <=
       , TOKEN_GT         // >
	   , TOKEN_LST		  // <<
	   , TOKEN_RST		  // >>
       , TOKEN_GE         // >=
       , TOKEN_EQ         // =
       , TOKEN_EQ1        // ==
       , TOKEN_NE         // <>
       , TOKEN_NE1        // !=
       , TOKEN_LT_STR     // <
       , TOKEN_LE_STR     // <=
       , TOKEN_GT_STR     // >
       , TOKEN_GE_STR     // >=
       , TOKEN_EQ_STR     // =
       , TOKEN_NE_STR     // <>
       , TOKEN_PLUS       // +
       , TOKEN_MINUS      // -
       , TOKEN_MULT       // *
       , TOKEN_DIVIDE     // /
       , TOKEN_MODULO     // %
       , TOKEN_POWER      // ^
       , TOKEN_STRCAT     // &
       , TOKEN_PATHCAT    // "\"
       , TOKEN_NOT        // NOT
       , TOKEN_NOT1       // !
       , TOKEN_LNOT       // ~
       , TOKEN_BR_OPEN    // (
       , TOKEN_BR_CLOSE   // )
       , TOKEN_COMMA      // ,
	   , TOKEN_QUESTION   // ?
	   , TOKEN_COLON      // :
       , TOKEN_UNKNOWN    // something unknown
       , TOKEN_END        // "\0", end of expression
	   , TOKEN_DOT		  // .
       };

protected:
  CStr                 Expression;        // Entire text to parse
  int						  Position;          // Current parse position in the expression
  int						  Error;             // Current error code
  CStr                 ErrorMessage;      // Error message
  CStr                 Content;      // From Interpreter Content.
  int					      ErrorPos;          // Where was the error?
  int				          Token;             // Recently found token
//  static CMapStrToPtr  Functions;		 // Funktion name -> Pointer to the method 

protected:
  // Helpers
  int                     CheckTokenLiterals   ( void );
  void                    SkipSpaces           ( void );
  void                    NextToken            ( void );

  BOOL                    Compare              ( int      op
                                               , CValue  &par1
                                               , CValue  &par2
                                               );

  // Parse methods, corresponding to BNF
  CValue                  String               ( void );
  CValue                  Number               ( void );
  CValue				  VariableValue		   ( LPCTSTR start );
  CValue                  Variable             ( void );
  CValue                  Function             ( void );
  CValue                  Reference            ( void );
  CValue                  Value                ( void );
  CValue                  Brackets             ( void );
  CValue                  Sign                 ( void );
  CValue                  Not                  ( void );
  CValue                  Power                ( void );
  CValue                  Mult                 ( void );
  CValue                  Calculate            ( void );
  CValue                  AddString            ( void );
  CValue                  Compare              ( void );
  CValue                  Condition            ( void );
  CValue                  And                  ( void );
  CValue                  Logical              ( void );
  CValue                  Xor	               ( void );
  CValue                  Shift	               ( void );

public:
  // Constructor
                          CExprParser          ( LPCTSTR expression );
						  CExprParser		   ( LPCTSTR expression, CStr xContent);
                          ~CExprParser         ( void );

  // Parse the expression
  CValue                  EvaluateExpression   ( BOOL fixedEnd = TRUE );

  // Get error informations
  int                     GetError             ( void );
  LPCTSTR                 GetErrorMessage      ( void );
  int                     GetErrorPosition     ( void );

  // Helpers
  static int              GetCharacterType     ( TCHAR character );
};

#endif