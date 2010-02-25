#include "variables.h"
#include "Interpreter.h"

CStr			VarError;
CMapStrToValue  Variables;
CMapStrToPtr    FileHandles;
CMapStrToValue *LocalVariables = NULL;
BOOL isGlobal = FALSE;

CValue *GetVariable( CStr &var, BOOL create, int *position, BOOL local )
{
	int     pos, startPos;
	bool	oldStyle = false, reference = false, isPtr = false;
	CStr entryName;
	CMapStrToValue *map = NULL;
	CValue  *value;
	CValue *dummy;

	pos = 0;
	if ( !local )
	{
		if ( LocalVariables != NULL && LocalVariables->Lookup( L"%AUTOLOCAL%", dummy ) )
			local = TRUE;
	}

	// Skip spaces
	while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;
	
	if ( pos < var.GetLength() && var[pos] == L'%' )
	{
		oldStyle = true;
		pos++;
		while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;
	}
	
	if ( create && pos < var.GetLength() && var[pos] == L'[' )
	{
		reference = true;
		pos++;
		while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;
	}

	startPos = pos;
	// Skip all valid variable name characters
	while (    pos < var.GetLength()
		    && (   GetCharacterType( var[pos] ) == CInterpreter::CHAR_ALPHA
			    || GetCharacterType( var[pos] ) == CInterpreter::CHAR_DIGIT
				|| GetCharacterType( var[pos] ) == CInterpreter::CHAR_UNDERSCORE
				|| (var[pos] == L':' && (pos == startPos || pos == startPos + 1 ))
			)
		  ) pos++;
	// Skip Regex variable name characters
	if ((var[pos]==L'$') && (pos+1<var.GetLength())){
		if (   var[pos+1] == L'&' 
			|| var[pos+1] == L'`' 
			|| var[pos+1] == L'\'' 
			|| var[pos+1] == L'+' 
			|| var[pos+1] == L'_'
			)pos +=2;
		else{
			int i=0;
			for (i=1;i<=3;i++){
				if ( pos+i<var.GetLength() && var[pos+i] >=L'0' && var[pos+i]<=L'9') { /* do nothing */ }else break;
			}
			pos+=i;
		}
	}
	
	// Skip spaces
	while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;

	//jwz:add for Global Variable
	if (var[startPos]==L':' && var[startPos+1]==L':'){
		local = FALSE;
		startPos +=2;
	}

	// No array index?
	if ( pos == -1 || pos >= var.GetLength() || var[pos] != L'[' )
	{
		entryName = var;

		if ( pos != -1 && pos <= var.GetLength() )
			entryName = var.Mid( startPos, pos-startPos );


		entryName.TrimLeft();
		entryName.TrimRight();

		if ( position != NULL ) 
			*position = pos;

		if ( entryName.IsEmpty())
		{
			VarError = L"Empty variable name not allowed";
			if ( position != NULL ) *position = -1;
			return NULL;
		}

		entryName.MakeUpper();

		if ( local && LocalVariables != NULL && LocalVariables->Lookup( L"%GL_" + entryName + L"%", dummy ) ) //predefined GLOBAL declare variable
		{
			local = FALSE;
		}

		if ( local && LocalVariables != NULL && LocalVariables->Lookup( entryName, value ) ) {}
		else if ( local == FALSE && Variables.Lookup( entryName, value ) ) {}
		else
		{
			if ( create )
			{
				// MessageBox( NULL, L"%GL_" + entryName + L"%", L"Debug", MB_OK );
				if ( LocalVariables != NULL && local )
				{
					LocalVariables->SetAt( entryName, CValue() );
					LocalVariables->Lookup( entryName, value );
				}
				else
				{
					Variables.SetAt( entryName, CValue() );
					Variables.Lookup( entryName, value );
				}
			}
			else
				value = NULL;
		}
	}
	else
	{
		entryName = var.Mid( startPos, pos-startPos );
		entryName.TrimLeft(); entryName.TrimRight();
		entryName.MakeUpper();

		if ( entryName.IsEmpty() )
		{
			VarError = L"'[' without array name";
			if ( position != NULL ) *position = -1;
			value = NULL;
		}
		else
		{
			if ( local && LocalVariables != NULL && LocalVariables->Lookup( L"%GL_" + entryName + L"%", dummy ) )
			{
				local = FALSE;
			}

			if ( local && LocalVariables != NULL && LocalVariables->Lookup( entryName, value ) ) {}
			else if ( local == FALSE && Variables.Lookup( entryName, value ) ) {}
			else
			{
				if ( create )
				{
					if ( LocalVariables != NULL && local )
					{
						LocalVariables->SetAt( entryName, CValue() );
						LocalVariables->Lookup( entryName, value );
					}
					else
					{
						Variables.SetAt( entryName, CValue() );
						Variables.Lookup( entryName, value );
					}
				}
				else
					value = NULL;
			}

			if ( value != NULL )
				map = value->GetMap();

			while ( pos != -1 && pos < var.GetLength() && var[pos] == L'[' )
			{
				startPos = pos+1;
				CStr arrIdxExpr = var.Mid( startPos );
				CInterpreter parser;

				entryName = (LPCTSTR)parser.EvaluateExpression( arrIdxExpr, FALSE );

				if ( parser.GetError() != 0 )
				{
					VarError = parser.GetErrorMessage();
					if ( position != NULL ) *position = -1;
					value = NULL;
					break;
				}

				if ( entryName.IsEmpty() != 0 )
				{
					VarError = L"Array index is empty";
					if ( position != NULL ) *position = -1;
					value = NULL;
					break;
				}

				pos = startPos + parser.GetErrorPosition();
				if ( var[pos] != L']' )
				{
					VarError = L"Missing ']'";
					if ( position != NULL ) *position = -1;
					value = NULL;
					break;
				}

				if ( position != NULL ) *position = pos+1;
				while ( pos < var.GetLength() && parser.GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;

				entryName.MakeUpper();
				BOOL isArray = FALSE;
				if ( pos <= var.GetLength() && var[pos+1] == L'[' )
				{
					isArray = TRUE;
					pos++;
				}

				if ( value != NULL )
				{
					if ( map != NULL )
					{
						if ( map->Lookup( entryName, value ) == FALSE )
						{
							if ( create )
							{
								map->SetAt( entryName, CValue() );
								map->Lookup( entryName, value );
								if ( isArray )
									map = value->GetMap();
							}
							else
								value = NULL;
						}
						else
							if ( isArray ) map = value->GetMap();
					}
				}
			}
		}
	}

	if ( create && reference && value != NULL )
	{
		while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;
		if ( pos < var.GetLength() && var[pos] == L']' )
		{
			pos++;
			CStr referredVar = (CStr)*value;
			value = GetVariable( referredVar, create );
		}
		else
		{
			VarError = L"Missing ']'";
			pos = -1;
			if ( position != NULL ) *position = -1;
			value = NULL;
		}
	}

	if ( oldStyle && value != NULL )
	{
		while ( pos < var.GetLength() && GetCharacterType( var[pos] ) == CInterpreter::CHAR_WHITESPACE ) pos++;
		if ( pos < var.GetLength() && var[pos] == L'%' )
		{
			pos++;
			//CStr referredVar = (LPCTSTR)value;
			//value = GetVariable( referredVar, create );
		}
		else
		{
			VarError = L"Missing '%'";
			pos = -1;
			if ( position != NULL ) *position = -1;
			value = NULL;
		}
	}
	return value;
}

CMapStrToValue *CreateAndInitLocalVariables()
{
	CMapStrToValue *local = new CMapStrToValue();

	local->SetAt( L"%GL_TRUE%",   CValue(1L) );
	local->SetAt( L"%GL_FALSE%",  CValue(1L) );
	local->SetAt( L"%GL_ON%",     CValue(1L) );
	local->SetAt( L"%GL_OFF%",    CValue(1L) );
	local->SetAt( L"%GL_YES%",    CValue(1L) );
	local->SetAt( L"%GL_NO%",     CValue(1L) );
	local->SetAt( L"%GL_CANCEL%", CValue(1L) );
	local->SetAt( L"%GL_PI%",	  CValue(1L) );
	local->SetAt( L"%GL_SQRT2%",  CValue(1L) );
	local->SetAt( L"%GL_PHI%",	  CValue(1L) );
	local->SetAt( L"%GL_EULER%",  CValue(1L) );
	local->SetAt( L"%GL_HKCU%",   CValue(1L) );
	local->SetAt( L"%GL_HKLM%",   CValue(1L) );
	local->SetAt( L"%GL_HKCR%",   CValue(1L) );
	local->SetAt( L"%GL_HKUS%",   CValue(1L) );
	local->SetAt( L"%GL_BYTES%",  CValue(1L) );
	local->SetAt( L"%GL_KB%",     CValue(1L) );
	local->SetAt( L"%GL_MB%",     CValue(1L) );
	local->SetAt( L"%GL_GB%",     CValue(1L) );
	local->SetAt( L"%GL_ST_HIDDEN%",  CValue(1L) );
	local->SetAt( L"%GL_ST_LIST%",    CValue(1L) );
	local->SetAt( L"%GL_ST_MESSAGE%", CValue(1L) );
	local->SetAt( L"%GL_DC_FILES%",   CValue(1L) );
	local->SetAt( L"%GL_DC_DIRS%",    CValue(1L) );
	local->SetAt( L"%GL_DC_ALL%",     CValue(1L) );
	local->SetAt( L"%GL_VAR_EMPTY",  CValue(1L) );
	local->SetAt( L"%GL_VAR_INT",    CValue(1L) );
	local->SetAt( L"%GL_VAR_FLOAT",  CValue(1L) );
	local->SetAt( L"%GL_VAR_STRING", CValue(1L) );
	local->SetAt( L"%GL_VAR_ARRAY",  CValue(1L) );
	local->SetAt( L"%GL_VAR_FILE",   CValue(1L) );
	local->SetAt( L"%GL_VAR_WINDOW", CValue(1L) );
	local->SetAt( L"%AUTOLOCAL%", CValue(1L) );

	return local;
}

BOOL SetVariable( CStr &varName, CValue value )
{
	VarError.Empty();
	int		pos;
	CValue *val;
	if ( varName.GetLength() > 0 && varName.GetAt(0) == L'[' )
	{
		CStr expression = varName.Mid( 1 );
		CInterpreter parser;
		CValue result = parser.EvaluateExpression( expression, FALSE );

		int subPos = parser.GetErrorPosition();
		if ( parser.GetError() != 0 )
		{
			VarError = CStr( L"Error in variable reference: " ) + parser.GetErrorMessage();
			return FALSE;
		}

		if ( expression[subPos] != L']' )
		{
			VarError = L"Variable reference not closed (missing ']')";
			return FALSE;
		}

		val = GetVariable( (CStr)result, TRUE, &pos );
	}
	else{
		val = GetVariable( varName, TRUE, &pos );
	}

	val->CopyFrom( value );
	return (pos != -1);
}

BOOL SetVariable( CStr &varName, LPCTSTR value )
{
	return SetVariable( varName, CValue(value) );
}

BOOL ClearVariable( CStr &varName )
{
	VarError.Empty();
	int pos;
	CValue *val = GetVariable( varName, TRUE, &pos );
	if ( val != NULL )
		val->Clear();
	return (pos != -1);
}
