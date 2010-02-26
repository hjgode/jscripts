#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#else
#include "stdafx.h"
#endif

#include <string.h>
//#include "Tlhelp32.h"
//#include "resource.h"
#include "inifile.h"
#include "interpreter.h"
BOOL CALLBACK SearchWindowText( HWND hwnd, LPARAM lParam );
time_t SystemTimeToUnixTime( const SYSTEMTIME &st );
void   UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst);
time_t FileTimeToUnixTime( const FILETIME &ft );

#ifndef SMARTPHONE
#include "mortscriptapp.h"
extern CMortScriptApp theApp;
#else
extern HINSTANCE g_hInst;
#endif

#ifndef PNA
extern CStr Proxy;
#include "wininet.h"
#endif

#if !defined( PNA ) && !defined( DESKTOP )
#include "ras.h"
#endif

#ifdef DESKTOP
#include "vc6\stdafx.h"
#include "vc6\resource.h"
#include <math.h>
#endif

#include "ValueArray.h"
#include "FunctionsData.h"
#include "Interpreter.h"


CValue  FctIsEmpty( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'IsEmpty'";
        error = 9;
        return CValue();
    }

    CValue rc;
    if ( params[0].IsNull() )
        rc = 1L;
    else
        rc = 0L;

    return rc;
}

CValue  FctVarType( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'VarType'";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = (long)params[0].GetType();
	if ( (long)rc == VALUE_PTR )
	{
		// Insert special pointer variable checks (HWND, file handles, etc.) here
	}

    return rc;
}

CValue  FctEval( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Eval'";
        error = 9;
        return CValue();
    }

    CValue rc;
    CInterpreter parser;

    rc = parser.EvaluateExpression( (CStr) params[0]);
    if ( parser.GetError() != 0 )
    {
        error = parser.GetError();
        errorMessage = parser.GetErrorMessage();
    }

    return rc;
}

CValue  FctToUpper( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ToUpper'";
        error = 9;
        return CValue();
    }

    CStr string = (LPCTSTR)params[0];
    string.MakeUpper();
    CValue rc; rc = string;

    return rc;
}

CValue  FctToLower( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ToLower'";
        error = 9;
        return CValue();
    }

    CStr string = (LPCTSTR)params[0];
    string.MakeLower();
    CValue rc; rc = string;

    return rc;
}

CValue  FctSubStr( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'SubStr'";
        error = 9;
        return CValue();
    }

    CStr rc;
	int start = (long)params[1];
	if (params.GetSize()==2){
		if (start>0) 
			rc = ((CStr)params[0]).Left(start );
		else {
			if ( start < 0 ) start = wcslen( (LPCTSTR)params[0] ) + start;
			if ( start < 0 ) start = 0;
			rc = ((CStr)params[0]).Mid( start );
		}
	}else if (params.GetSize() >= 3){
		if ( start > 0 ) start--;
		if ( start < 0 ) start = wcslen( (LPCTSTR)params[0] ) + start;
		if ( start < 0 ) start = 0;
		if ( (long)params[2] > 0 )
		{
			int len   = (long)params[2];
			rc = ((CStr)params[0]).Mid( start, len );
		}else{
			rc = ((CStr)params[0]).Mid( start );
		}
	}
    CValue val;
    val = rc;

    return val;
}


CValue  FctLength( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Length'";
        error = 9;
        return CValue();
    }

    return (long)wcslen( (LPCTSTR)params[0] );

}

CValue  FctCharAt( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'CharAt'";
        error = 9;
        return CValue();
    }

    CStr rc;
    int start = (long)params[1];
    if ( start > 0 ) start--;
    if ( start < 0 ) start = wcslen( (LPCTSTR)params[0] ) + start;
    if ( start < 0 ) start = 0;
    rc = ((CStr)params[0]).Mid( start, 1 );

    return CValue(rc);
}

CValue  FctUCValue( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'UCValue'";
        error = 9;
        return CValue();
    }

    CValue rc;
	CStr string = params[0];
	if ( string.GetLength() > 0 )
		rc = (long)string.GetAt( 0 );

    return rc;
}

CValue  FctUCChar( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'UCChar'";
        error = 9;
        return CValue();
    }

    CValue rc;
	if ( (long)params[0] != 0 )
	{
		TCHAR string[2];
		string[0] = (unsigned short)(long)params[0];
		string[1] = '\0';
		rc = string;
	}

    return rc;
}

CValue  FctArray( CValueArray &params, int &error, CStr &errorMessage )
{
	CValue res;
	CMapStrToValue *map = res.GetMap();

	CStr index;
	for ( int i=0; i<params.GetSize(); i++ )
	{
		index.Format( L"%d", i+1 );
		map->SetAt( index, params[i] );
		// CValue *value;
		// map->Lookup( index, value );
		// MessageBox( NULL, (LPCTSTR)*value, index, MB_SETFOREGROUND );
	}

	return res;
}

CValue  FctMap( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() % 2 != 0 )
    {
        errorMessage = InvalidParameterCount + L"'Map'";
        error = 9;
        return CValue();
    }

	CValue res;
	CMapStrToValue *map = res.GetMap();

	CStr index;
	for ( int i=0; i<params.GetSize(); i+=2 )
	{
		index = (CStr)params[i];
		index.MakeUpper();
		map->SetAt( (LPCTSTR)index, params[i+1] );
	}

	return res;
}


CValue  FctSplit( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'Split'";
        error = 9;
        return CValue();
    }

    CStrArray elements;
    int parts = CIniFile::Split( params[0], params[1], elements, (params.GetSize() >= 3) ? (long)params[2] : 1 );

	CValue res;
	CMapStrToValue *map = res.GetMap();

	CStr index;
	for ( int i=0; i<elements.GetSize(); i++ )
	{
		index.Format( L"%d", i+1 );
		map->SetAt( index, elements[i] );
	}

	return res;
}

CValue  FctPart( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 3 && params.GetSize() != 4 )
    {
        errorMessage = InvalidParameterCount + L"'GetPart'";
        error = 9;
        return CValue();
    }

    CStrArray elements;
    int parts = CIniFile::Split( params[0], params[1], elements, (params.GetSize() >= 4) ? (long)params[3] : 1 );

    int idx = (long)params[2];
    if ( idx == 0 || abs( idx ) > parts )
        return CValue();

    CValue part;
    if ( idx > 0 )
        part = elements.GetAt( idx-1 );
    else
        part = elements.GetAt( parts+idx );
    
    return part;
}

// Get(Source,Begin,End,Prev,Include)
CValue  FctGet( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 3 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'Get'";
        error = 9;
        return CValue();
    }

	CStr tStr;
	CValue Result,tValue;
	tStr = (CStr)params[0];
	if (params.GetSize()==3)
		Result = tStr.Get((CStr)params[1],(CStr)params[2]);
	else if (params.GetSize()==4)
	{
		tValue = params[3];
		if (tValue.IsValidNumber())
			Result = tStr.Get((CStr)params[1],(CStr)params[2],(long)tValue);
		else
			Result = tStr.Get((CStr)params[1],(CStr)params[2],(CStr)tValue);
	}
	else if (params.GetSize()==5)
	{
		tValue = params[4];
		if (!tValue.IsValidNumber())
		{
			errorMessage = L"Parameter 4 of 'Get' must be 0 or 1.";
			error = 10;
			return CValue();
		}
		Result = tStr.Get((CStr)params[1],(CStr)params[2],(CStr)params[3],(long)tValue);
	}
  
    return Result;
}

// Get(Source,Begin,End,Prev,Include)
CValue  FctGetNoCase( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() < 3 || params.GetSize() > 5 )
    {
        errorMessage = InvalidParameterCount + L"'Get'";
        error = 9;
        return CValue();
    }

	CStr tStr;
	CValue Result,tValue;
	tStr = (CStr)params[0];
	if (params.GetSize()==3)
		Result = tStr.GetNoCase((CStr)params[1],(CStr)params[2]);
	else if (params.GetSize()==4)
	{
		tValue = params[3];
		if (tValue.IsValidNumber())
			Result = tStr.GetNoCase((CStr)params[1],(CStr)params[2],(long)tValue);
		else
			Result = tStr.GetNoCase((CStr)params[1],(CStr)params[2],(CStr)tValue);
	}
	else if (params.GetSize()==5)
	{
		tValue = params[4];
		if (!tValue.IsValidNumber())
		{
			errorMessage = L"Parameter 4 of 'Get' must be 0 or 1.";
			error = 10;
			return CValue();
		}
		Result = tStr.GetNoCase((CStr)params[1],(CStr)params[2],(CStr)params[3],(long)tValue);
	}
  
    return Result;
}

CValue  FctElementCount( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ElementCount'";
        error = 9;
        return CValue();
    }

	if ( params[0].GetType() != VALUE_MAP )
	{
        errorMessage = L"'ElementCount': Parameter is no array";
        error = 9;
        return CValue();
	}

	CMapStrToValue *map = params[0].GetMap();
	
    CValue size;
	size = (long)map->GetSize();

    return size;
}

CValue  FctMaxIndex( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'MaxIndex'";
        error = 9;
        return CValue();
    }

	if ( params[0].GetType() != VALUE_MAP )
	{
        errorMessage = L"'MaxIndex': Parameter is no array";
        error = 9;
        return CValue();
	}

	CMapStrToValue *map = params[0].GetMap();

	int max = 0;
	CStr idx;
	CValue *dummy;
	while ( TRUE )
	{
		idx.Format( L"%d", max+1 );
		if ( map->Lookup( idx, dummy ) && ! dummy->IsNull() )
		{
			max++;
		}
		else
			break;
	}

    CValue size;
	size = (long)max;

    return size;
}

CValue  FctFind( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'Find'";
        error = 9;
        return CValue();
    }

    if ( ((CStr)params[1]).GetLength() < 1 )
    {
        errorMessage = L"String to find must be at least one character";
        error = 9;
        return CValue();
    }

	int start = 0;
	if ( params.GetSize() == 3 )
	{
		start = (int)(long)params[2]-1;
	}

    CValue rc;
    rc = (long) ( ((CStr)params[0]).Find((LPCTSTR)params[1], start )+1 );

    return rc;
}

CValue  FctReverseFind( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'ReverseFind'";
        error = 9;
        return CValue();
    }

    if ( ((CStr)params[1]).GetLength() < 1 )
    {
        errorMessage = L"String to find must be at least one character";
        error = 9;
        return CValue();
    }

    CValue rc;
    rc = (long) ( ((CStr)params[0]).ReverseFind( ((CStr)params[1]).GetAt(0) )+1 );

    return rc;
}

CValue  FctReplace( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'Replace'";
        error = 9;
        return CValue();
    }

    if ( ((CStr)params[1]).GetLength() < 1 )
    {
        errorMessage = L"String to replace must be at least one character";
        error = 9;
        return CValue();
    }

    CValue rc;
	CStr string = (CStr)params[0];
    string.Replace( (LPCTSTR)params[1], (LPCTSTR)params[2] );
	rc = string;

    return rc;
}

CValue  FctRepeatString( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'RepeatString'";
        error = 9;
        return CValue();
    }

    if ( ((CStr)params[0]).GetLength() < 1 )
    {
        errorMessage = L"String to repeat must be at least one character";
        error = 9;
        return CValue();
    }

    CValue rc;
	CStr string;
	for ( int i=0; i<(long)params[1]; i++ )
		string += (CStr)params[0];

	rc = string;

    return rc;
}


CValue  FctFilePath( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FilePath'";
        error = 9;
        return CValue();
    }

    CValue rc;
    int pos = ((CStr)params[0]).ReverseFind('\\');
    if ( pos != -1 )
        rc = ((CStr)params[0]).Left( pos );

    return rc;
}

CValue  FctFileBase( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FilePath'";
        error = 9;
        return CValue();
    }

    CValue rc;
    int pos1 = ((CStr)params[0]).ReverseFind('\\');
    int pos2 = ((CStr)params[0]).ReverseFind('.');
    if ( pos2 > pos1 )
        rc = ((CStr)params[0]).Mid( pos1+1, pos2-pos1-1 );
    else
        rc = ((CStr)params[0]).Mid( pos1+1 );

    return rc;
}

CValue  FctFileExt( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'FilePath'";
        error = 9;
        return CValue();
    }

    CValue rc;
    int pos = ((CStr)params[0]).ReverseFind('.');
    if ( pos != -1 && pos >= ((CStr)params[0]).ReverseFind('\\') )
        rc = ((CStr)params[0]).Mid( pos );

    return rc;
}

CValue  FctFormat( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 2 && params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'Format'";
        error = 9;
        return CValue();
    }

    if ( params.GetSize() == 2 )
    {
        CStr format, output;
		if ( (long)params[1] == 0 )
	        format = L"%0.0f";
		else
			format.Format( L"%%0.0%df", (long)params[1] );
        output.Format( format, (double)params[0] );
        rc = output;
	}
    else
	{
        CStr format, output;
		if ( (long)params[1] == 0 )
	        format.Format( L"%%0%d.0f", (long)params[2] );
		else
			format.Format( L"%%0%d.0%df", (long)params[2]+(long)params[1]+1, (long)params[1] );
        output.Format( format, (double)params[0] );
        rc = output;
    }

    return rc;
}

CValue  FctNumberToHex( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'NumberToHex'";
        error = 9;
        return CValue();
    }

	CStr out;
	out.Format( L"%x", (long)params[0] );
	if ( out.GetLength() % 2 == 1 ) // Fill up to full bytes
		out = L"0" + out;
	rc = out;

    return rc;
}

CValue FctHexToNumber( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'HexToNumber'";
        error = 9;
        return CValue();
    }

	DWORD data;
    if ( swscanf( (LPCTSTR)params[0], L"%X", &data ) != 1 )
    {
        errorMessage = L"Invalid value '" + (CStr)params[0] + L"' for 'HexToNumber'";
        error = 9;
        return CValue();
	}

	rc = (long)data;

    return rc;
}


CValue  FctFloor( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'Floor'";
        error = 9;
        return CValue();
    }

    switch ( params[0].GetType() )
    {
    case VALUE_LONG:
        rc = params[0];
        break;

    case VALUE_STRING:
    case VALUE_DOUBLE:
        rc = (long)floor( (double)params[0] );
        break;

    default:
        rc = 0L;
    }

    return rc;
}

CValue  FctRound( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'Round'";
        error = 9;
        return CValue();
    }

    rc = (long)params[0];

    return rc;
}

CValue  FctCeil( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'Ceil'";
        error = 9;
        return CValue();
    }

    switch ( params[0].GetType() )
    {
    case VALUE_LONG:
        rc = params[0];
        break;

    case VALUE_STRING:
    case VALUE_DOUBLE:
        rc = (long)ceil( (double)params[0] );
        break;

    default:
        rc = 0L;
    }

    return rc;
}

//jwz::add
CValue FctFloatStoreFormat(CValueArray &params, int &error, CStr &errorMessage){
	CValue rc;

    if ( params.GetSize() > 1 )
    {
        errorMessage = InvalidParameterCount + L"'FloatToHex'";
        error = 9;
        return CValue();
    }

	TCHAR t[9];
	float f = (float)(DOUBLE)params[0];
	char * c = (char *)&f;
	TCHAR s,i;
	for (i=0;i<4;i++){
		s = *(c+i)&0x0F;
		if (s>=10) s=s-10+L'A'; else s+=L'0';
		t[7-(i*2+0)] = s;
		s = *(c+i)>>4 & 0x0F;
		if (s>=10) s=s-10+L'A'; else s+=L'0';
		t[7-(i*2+1)] = s;
	}
	t[8]=L'\0';
	rc = t;
	return rc;
}

CValue FctAsc(CValueArray &params, int &error, CStr &errorMessage){
	CValue rc;

    if ( params.GetSize() > 2 )
    {
        errorMessage = InvalidParameterCount + L"'ASC'";
        error = 9;
        return CValue();
    }
	CStr tStr = SlashChar((CStr)params[0]), Str;
	
	int Len = tStr.GetLength();
	TCHAR SepCh=L'';
	if (params.GetSize()==2 ) {
		Str = (CStr)params[1];
		if (Str.GetLength()>=1)
			SepCh = Str[0];
		else
			SepCh = L'';
	}

	Str.Empty();
	TCHAR c,sc;
	for(int i=0;i<Len;i++){
		c = tStr[i];

		sc = (c>>4) & 0x000F;
		if (sc >= 10) sc = sc - 10 + L'A'; else sc += L'0';
		Str += sc;

		sc = c & 0x000F;
		if (sc >= 10) sc = sc - 10 + L'A'; else sc += L'0';
		Str += sc;
		if (SepCh != L'' && i != Len-1) Str+=SepCh;
	}

	rc = (CStr)Str;

	return rc;
}

CValue FctReverseStr(CValueArray &params, int &error, CStr &errorMessage){
	CValue rc;

    if ( params.GetSize() > 2 )
    {
        errorMessage = InvalidParameterCount + L"'REVERSE'";
        error = 9;
        return CValue();
    }
	CStr tStr = (CStr)params[0],ResultStr = L"";
	int Hex = 1;
	if (params.GetSize()==2 && (long)params[1]!=0) Hex = (long)params[1]; // Reverse(STRINGS,16);//"7E32EF"->"EF327E" otherwise "FE23E7"
	for (int i=0;i<tStr.GetLength();i+=Hex){
		ResultStr = tStr.Mid(i,Hex) + ResultStr;
	}
	rc = ResultStr;
	return rc;
}
//jwz::add end
CValue  FctRnd( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() > 1 )
    {
        errorMessage = InvalidParameterCount + L"'Rnd'";
        error = 9;
        return CValue();
    }

	long max = 0;
    switch ( params[0].GetType() )
    {
    case VALUE_LONG:
        max = (long)params[0];
        break;

    case VALUE_STRING:
    case VALUE_DOUBLE:
        max = (long)floor( (double)params[0] );
        break;

    default:
        max = 0L;
    }

	if ( max > 0 )
		rc = (long)( rand() % max );
	else
		rc = (double)( rand()/(float(RAND_MAX)+1) );

    return rc;
}


CValue  FctGetRGB( CValueArray &params, int &error, CStr &errorMessage )
{
    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'RGB'";
        error = 9;
        return CValue();
    }

    CValue rc;

    rc = (long)RGB( (long)params[0], (long)params[1], (long)params[2] );

    return rc;
}

CValue  FctSin( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Sin'";
        error = 9;
        return CValue();
    }

	rc = sin( (double)params[0] );

    return rc;
}

CValue  FctCos( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Cos'";
        error = 9;
        return CValue();
    }

	rc = cos( (double)params[0] );

    return rc;
}

CValue  FctTan( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Tan'";
        error = 9;
        return CValue();
    }

	rc = tan( (double)params[0] );

    return rc;
}

CValue  FctArcSin( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ArcSin'";
        error = 9;
        return CValue();
    }

	rc = asin( (double)params[0] );

    return rc;
}

CValue  FctArcCos( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ArcCos'";
        error = 9;
        return CValue();
    }

	rc = acos( (double)params[0] );

    return rc;
}

CValue  FctArcTan( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'ArcTan'";
        error = 9;
        return CValue();
    }

	rc = atan( (double)params[0] );

    return rc;
}

CValue  FctLog( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Log'";
        error = 9;
        return CValue();
    }

	rc = log( (double)params[0] );

    return rc;
}

CValue  FctLog10( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Log10'";
        error = 9;
        return CValue();
    }

	rc = log10( (double)params[0] );

    return rc;
}

CValue  FctSqrt( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Sqrt'";
        error = 9;
        return CValue();
    }

	rc = sqrt( (double)params[0] );

    return rc;
}

CValue  FctCompareFloat( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 3 )
    {
        errorMessage = InvalidParameterCount + L"'CompareFloat'";
        error = 9;
        return CValue();
    }

	long lVal1 = (long)( (double)params[0] * pow( 10, (long)params[2] ) + .5 );
	long lVal2 = (long)( (double)params[1] * pow( 10, (long)params[2] ) + .5 );

	if ( lVal1 < lVal2 ) rc = -1L;
	else if ( lVal1 > lVal2 ) rc = 1L;
	else rc = 0L;

    return rc;
}

CValue  FctMax( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() < 1 )
    {
        errorMessage = InvalidParameterCount + L"'Max'";
        error = 9;
        return CValue();
    }

	rc = params[0];
	for ( int i=1; i<params.GetSize(); i++ )
	{
		if ( (double)params[i] > (double)rc )
			rc = params[i];
	}

	return rc;
}

CValue  FctMin( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() < 1 )
    {
        errorMessage = InvalidParameterCount + L"'Min'";
        error = 9;
        return CValue();
    }

	rc = params[0];
	for ( int i=1; i<params.GetSize(); i++ )
	{
		if ( (double)params[i] < (double)rc )
			rc = params[i];
	}

	return rc;
}

CValue  FctRed( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Red'";
        error = 9;
        return CValue();
    }

	rc = (long)GetRValue( (long)params[0] );

	return rc;
}

CValue  FctGreen( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Green'";
        error = 9;
        return CValue();
    }

	rc = (long)GetGValue( (long)params[0] );

	return rc;
}

CValue  FctBlue( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 1 )
    {
        errorMessage = InvalidParameterCount + L"'Blue'";
        error = 9;
        return CValue();
    }

	rc = (long)GetBValue( (long)params[0] );

	return rc;
}

//In(Ch,Str) true is 1 otherwise is 0
CValue  FctIn( CValueArray &params, int &error, CStr &errorMessage )
{
    CValue rc;

    if ( params.GetSize() != 2 )
    {
        errorMessage = InvalidParameterCount + L"'In'";
        error = 9;
        return CValue();
    }


	CStr tch = params[1];
	TCHAR ch = tch[0];
    //CInterpreter parser;
	rc = (long)CInterpreter::In( ch, (CStr)params[0] );

	return rc;
}
