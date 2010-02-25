// ParseCmdLine.cpp: implementation of the CParseCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#ifdef SMARTPHONE
#include <windows.h>
#include "smartphone/mortafx.h"
#include "smartphone/MortRunnerSP.h"
#else
#include "stdafx.h"
#include "MortScriptApp.h"
#endif

#include "ParseCmdLine.h"

extern CMapStrToValue  Variables;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseCmdLine::CParseCmdLine()
{
	WaitForFile = 5;
	RegOnly = FALSE;
}

CParseCmdLine::~CParseCmdLine()
{

}

void CParseCmdLine::ParseCmdLine( LPCTSTR cmdLine )
{
	bool inQuotes = false;
	bool inSpQuotes = false;
	bool inOption = false;
	bool inValue  = false;
	bool withFlag = false;

	CStr option;
	CStr value;

	//MessageBox( NULL, cmdLine, L"cmdLine", MB_SETFOREGROUND );

	int len = wcslen( cmdLine );

	for ( int i = 0; i < len; i++ )
	{
		// skip spaces
		if ( !inQuotes && !inSpQuotes && !inOption && !inValue && ( cmdLine[i] == ' ' || cmdLine[i] == '\t' ) )
		{
			//MessageBox( NULL, L"Skip space", L"Debug", MB_SETFOREGROUND );
			continue;
		}

		if ( !inSpQuotes && cmdLine[i] == '\"' )
		{
			inQuotes = !inQuotes;
			continue;
		}

		if ( cmdLine[i] == '`' )
		{
			inSpQuotes = true;
			continue;
		}

		if ( inSpQuotes && (BYTE)cmdLine[i] == (BYTE)'?' )
		{
			inSpQuotes = false;
			continue;
		}

		if ( inOption && cmdLine[i] == '=' )
		{
			inOption = false;
			inValue = true;
			continue;
		}

		if ( ( inOption || inValue ) && !inQuotes && !inSpQuotes && ( cmdLine[i] == ' ' || cmdLine[i] == '\t' ) )
		{
			//MessageBox( NULL, value, L"opt: "+option, MB_SETFOREGROUND );
			ParseParam( option, value, withFlag, inValue );

			inOption = false;
			inValue  = false;
			withFlag = false;

			option.Empty();
			value.Empty();
			continue;
		}

		if ( !inOption && !inValue )
		{
			inOption = true;
			if ( cmdLine[i] == '/' || cmdLine[i] == '-' )
			{
				withFlag = true;
				//MessageBox( NULL, L"With flag", L"Debug", MB_SETFOREGROUND );
				continue;
			}
		}

		if ( inOption )
		{
			option += cmdLine[i];
			//MessageBox( NULL, option, L"option", MB_SETFOREGROUND );
		}

		if ( inValue )
		{
			value += cmdLine[i];
			//MessageBox( NULL, value, L"value", MB_SETFOREGROUND );
		}
	}

	if ( inOption || inValue )
	{
		//MessageBox( NULL, value, L"fin. opt: "+option, MB_SETFOREGROUND );
		ParseParam( option, value, withFlag, inValue );
	}
}

void CParseCmdLine::ParseParam( CStr &option, CStr &value, BOOL withFlag, BOOL withValue )
{
	if ( !withValue && !option.IsEmpty() )
		Filename = option;
	else
	{
		if ( withFlag )
		{
			if ( option.CompareNoCase( L"register" ) == 0 )
				RegOnly = TRUE;

			if ( option.CompareNoCase( L"wait" ) == 0 && withValue )
				WaitForFile = _wtol( value );
		}
		else
		{
			option.MakeUpper();
			//MessageBox( NULL, value, L"variable: '"+option+L"'", MB_SETFOREGROUND );
			Variables.SetAt( option, value );
		}
	}
}

