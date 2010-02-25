#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
#include "interpreter.h"

#ifndef SMARTPHONE
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#else
extern HINSTANCE g_hInst;
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "variables.h"
#include "inifile.h"
#include "Interpreter.h"
#include "CommandsWindows.h"


// defined in Interpreter.cpp
extern CMortPtrArray				SubResultStack;
extern CMortPtrArray				LocalVariablesStack;

BOOL CmdClear( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

	param.TrimLeft(); param.TrimRight();
	if ( param[0] == '(' && param[param.GetLength()-1] == ')' )
	{
		param = param.Mid( 1, param.GetLength() - 2 );
		param.TrimLeft(); param.TrimRight();
	}

	VarError.Empty();
    CStr varName;
	CValue *value;
	
	if ( param[0] == '%' )
	{
		int subPos;
		value = GetVariable( param.Mid(1), FALSE, &subPos );
		CStr rest = param.Mid(subPos+1);
		if ( rest.IsEmpty() || rest[0] != '%' )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable not closed (missing '%')" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
		if ( value == NULL )
			varName.Empty();
		else
		{
			varName = (LPCTSTR)*value;
			//varName.MakeUpper();
			value = GetVariable( varName, TRUE, &subPos );
		}
	}
	else if ( param[0] == '[' )
	{
		int subPos, dummy;
		CStr expression = param.Mid( 1 );

		CValue result = interpreter.EvaluateExpression( expression, false );

		subPos = interpreter.GetErrorPosition();
		if ( interpreter.GetError() != 0 )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Error in variable reference: " + VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}

		if ( expression[subPos] != ']' )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable reference not closed (missing ']')" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}

		varName = (CStr)result;
		if ( varName.IsEmpty() )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), L"Variable reference is empty" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
		value = GetVariable( varName, TRUE, &dummy );
	}
	else
	{
		int subPos;
		value = GetVariable( param, TRUE, &subPos );
		varName = param.Left(subPos);
		if ( VarError.IsEmpty() )
		{
			CStr rest = param.Mid(subPos); rest.TrimLeft();
			if ( !rest.IsEmpty() )
			{
				if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), L"Invalid variable name" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
				return FALSE;
			}
		}
	}

	if ( ! VarError.IsEmpty() )
	{
		if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
			MessageBox( GetMsgParent(), VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
		return FALSE;
	}

	if ( ! varName.IsEmpty() && value != NULL )
	{
		value->Clear();
	}

    return rc;
}
/*
BOOL CmdLocal( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

	CValueArray params;
    interpreter.Split( param, L',', params, 1, 0xFFFFFFFFL );

	if ( LocalVariables == NULL )
	{
		LocalVariables = CreateAndInitLocalVariables();
		LocalVariablesStack.SetAt( LocalVariablesStack.GetSize()-1, LocalVariables );
	}

	VarError.Empty();
	for ( int i=0; i<params.GetSize(); i++ )
	{
		CStr varName = params[i];
		varName.MakeUpper();
		// LocalVariables->SetAt( L"%GL_" + varName + "%", CValue() );

		CValue *value = GetVariable( params[i], TRUE, NULL, TRUE );
		if ( ! VarError.IsEmpty() )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

	if ( params.GetSize() == 0 )
	{
		LocalVariables->SetAt( L"%AUTOLOCAL%", CValue(1L) );
	}

    return rc;
}
*/
/*
BOOL CmdGlobal( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

	CValueArray params;
    interpreter.Split( param, L',', params, 1, 0xFFFFFFFFL );

	if ( LocalVariables == NULL )
	{
		LocalVariables = CreateAndInitLocalVariables();
		LocalVariablesStack.SetAt( LocalVariablesStack.GetSize()-1, LocalVariables );
	}
	LocalVariables->SetAt( L"%AUTOLOCAL%", CValue(1L) );

	VarError.Empty();
	for ( int i=0; i<params.GetSize(); i++ )
	{
		CStr varName = params[i];
		varName.MakeUpper();
		varName.TrimLeft(); varName.TrimRight();

		LocalVariables->SetAt( L"%GL_" + varName + L"%", CValue(1L) );
		// MessageBox( GetMsgParent(), L"%GL_" + varName + L"%", L"Global", MB_OK );

		CValue *value = GetVariable( params[i], TRUE, NULL, FALSE );
		if ( ! VarError.IsEmpty() )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}
	}

    return rc;
}
*/
/*BOOL CmdReturn( CInterpreter &interpreter, CStr &param )
{
    BOOL rc = TRUE;

	CValueArray params; CValue rc1;
	param.MakeUpper();
    interpreter.Split( param, L',', params );
	
	SubResultForReturn = new CValueArray();
	for(int i=0;i<params.GetSize();i++){
		rc1 = interpreter.EvaluateExpression(params[i]);
		SubResultForReturn->Add(rc1);
	}
	ReturnStack.Add(SubResultForReturn);
	char type; long state;
	interpreter.PopCondStack(type,state,false);

	if (type == CT_SUB){
		interpreter.PopCondStack(type,state);
		void * newPos;
		if (interpreter.EndPosOfSubs.Lookup(state,newPos)){
			interpreter.SetCurrentPos((long)newPos);
		}
	}
    return rc;
}*/

BOOL CmdEval( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Eval'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStr expression = params[1];

    CValue result = interpreter.EvaluateExpression(expression);

    if ( interpreter.GetError() != 0 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), CStr(interpreter.GetErrorMessage()) + L"\r\nIn expression " + (CStr)params.GetAt(1) + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    return interpreter.SetVariable( params[1], result );
}

BOOL CmdGetRGB( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;

    if ( interpreter.Split( param, L',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'RGB'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CValue value;
    value = (long)RGB((long)params[0], (long)params[1], (long)params[2]);
    return interpreter.SetVariable( params[3], value );
}

BOOL CmdGetFilePath( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 2 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetFileExt'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr path;
	int pos = ((CStr)params[0]).ReverseFind('\\');
	if ( pos == -1 )
		path.Empty();
	else
		path = ((CStr)params[0]).Left( pos );

    return interpreter.SetVariable( params[1], path );
}

BOOL CmdGetFileBase( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 2 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetFileExt'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr file;
	int pos1 = ((CStr)params[0]).ReverseFind('\\');
	int pos2 = ((CStr)params[0]).ReverseFind('.');
	if ( pos2 > pos1 )
		file = ((CStr)params[0]).Mid( pos1+1, pos2-pos1-1 );
	else
		file = ((CStr)params[0]).Mid( pos1+1 );

    return interpreter.SetVariable( params[1], file );
}

BOOL CmdGetFileExt( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 2 ) != 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetFileExt'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr ext;
	int pos = ((CStr)params[0]).ReverseFind('.');
	if ( pos == -1 || pos < ((CStr)params[0]).ReverseFind('\\') )
		ext.Empty();
	else
		ext = ((CStr)params[0]).Mid( pos );

    return interpreter.SetVariable( params[1], ext );
}

BOOL CmdSplit( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, TRUE, ~7 ) < 4 ) // variable, delimiter, trim?, variables
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Split'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStrArray elements;
    CIniFile::Split( params[0], params[1], elements, (long)params[2] );

    if ( params.GetSize() == 4 )
	{
		CStr varName = params[3];
		//CStr elem, cont;

		CValue *value;
		if ( varName.GetLength() > 0 && varName.GetAt(0) == '%' )
			value = GetVariable( varName, TRUE, NULL );
		else
			value = new CValue();

		CValue *mapValue = NULL;
		if ( value == NULL )
		{
			if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
				MessageBox( GetMsgParent(), VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
			return FALSE;
		}

		if( varName[0] == '%' && value->GetType() == VALUE_STRING )
			mapValue = GetVariable( *value, TRUE );
		else
			mapValue = value;

		CMapStrToValue *map = mapValue->GetMap();
		map->RemoveAll();
		CStr elem;
		for ( int i=0; i < elements.GetSize(); i++ )
		{
			elem.Format( L"%d", i+1 );
			map->SetAt( elem, elements[i] );
		}

		if ( varName.GetLength() == 0 || varName.GetAt(0) != '%' )
		{
			SetVariable( varName, *value );
			delete value;
		}
	}
	else
	{
		for ( int i=0; i+3<params.GetSize(); i++ )
		{
			CStr var = params[i+3];
			CValue *value = GetVariable( var, TRUE );
			if ( value == NULL )
			{
				if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
					MessageBox( GetMsgParent(), VarError + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
				return FALSE;
			}

			if ( i < elements.GetSize() )
			{
				*value = elements[i];
			}
			else
			{
				value->Clear();
			}
		}
	}

    return TRUE;
}

BOOL CmdGetPart( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1<<4 ) != 5 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'GetPart'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ((CStr)params[1]).GetLength() != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"Split delimiter must be one character" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    CStrArray elements;
    CIniFile::Split( params[0], params[1], elements, (long)params[2] );
    
	BOOL rc;
    if ( (long)params[3] > 0 && (long)params[3] < elements.GetSize() )
    {
        rc = interpreter.SetVariable( params[4], elements.GetAt( (long)params[3]-1 ) );
    }
    else
    {
        rc = ClearVariable( params[4] );
    }

    return rc;
}

BOOL CmdSubStr( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1<<3 ) != 4 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'SubStr'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	BOOL rc = TRUE;
	if ( (long)params[2] > 0 )
	{
        int start = (long)params[1] - 1;
        int len   = (long)params[2];
		rc = interpreter.SetVariable( params[3], ((CStr)params[0]).Mid( start, len ) );
    }
    else
    {
		rc = interpreter.SetVariable( params[3], ((CStr)params[0]).Mid( (long)params[1] - 1 ) );
    }

    return rc;
}

BOOL CmdFind( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1<<2 ) != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'Find'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ((CStr)params[1]).GetLength() < 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"String to find must be at least one character" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CValue pos;
	pos = (long)( ((CStr)params[0]).Find( (LPCTSTR)params[1] ) + 1 );

	return interpreter.SetVariable( params[2], pos );
}

BOOL CmdReverseFind( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1<<2 ) != 3 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'ReverseFind'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

    if ( ((CStr)params[1]).GetLength() != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), L"String to find must be one character" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CValue pos;
	pos = (long)( ((CStr)params[0]).ReverseFind( ((CStr)params[1]).GetAt(0) ) + 1 );

	return interpreter.SetVariable( params[2], pos );
}

BOOL CmdMakeUpper( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'MakeUpper'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr str;
	CValue *val = GetVariable( params[0], FALSE );
	if ( val != NULL )
		str = (LPCTSTR)*val;
	str.MakeUpper();

	return interpreter.SetVariable( params[0], str );
}

BOOL CmdMakeLower( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
    if ( interpreter.Split( param, L',', params, 1, 1 ) != 1 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'MakeLower'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }

	CStr str;
	CValue *val = GetVariable( params[0], FALSE );
	if ( val != NULL )
		str = (LPCTSTR)*val;
	str.MakeLower();

	return interpreter.SetVariable( params[0], str );
}

#ifdef CELE
#include "../zlib/tgzx.cpp"

BOOL CmdTGZX( CInterpreter &interpreter, CStr &param )
{
    CValueArray params;
	//Debug(param);
    if ( interpreter.Split( param, L',', params ) > 2 )
    {
        if ( interpreter.ErrorLevel >= ERROR_SYNTAX )
            MessageBox( GetMsgParent(), InvalidParameterCount + L"'TGZX'" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
        return FALSE;
    }
	
	CStr tStr = params[0];
	tStr.TrimRight(L"\"");
	tStr.TrimLeft(L"\"");
	CStr OutDir = L"/";	//Default tgz output directory is "root"
	if (params.GetSize()==2){
		OutDir = (CStr)params[1];
		OutDir.TrimRight(L"\"");
		OutDir.TrimLeft(L"\"");
	}

	long hRES = TGZX((PTSTR)((LPCTSTR)tStr),OutDir); //extract the tar gzip files

	if (hRES>0){ //if extract files
		return TRUE;
	}else{	//no file extracted or return a negative number the error occurs.
        MessageBox( GetMsgParent(), L"'TGZX' extracting Get an error, Extract failure!" + interpreter.GetErrorLine(), L"Error", MB_OK|MB_ICONERROR|MB_SETFOREGROUND );
		return FALSE;
	}
}
#endif