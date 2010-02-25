// Value.cpp: implementation of the CValue class.
//
//////////////////////////////////////////////////////////////////////

#include "morttypes.h"

#define _ASSERTE(x)
#ifdef DESKTOP
#include "tchar.h"
#include "stddef.h"
#endif

#include <atlconv.h>
#include <math.h>


/*
#ifdef DESKTOP
#include "..\..\vc6\mortrunner\stdafx.h"
#endif

#ifdef POCKETPC
#include "stdafx.h"
#endif

#ifdef SMARTPHONE
#include <windows.h>
#include <atldef.h>
#include "smartphone\mortafx.h"
#endif

#include "Value.h"
#include "morttypes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValue::CValue()
{
	type = VALUE_NULL;
}

CValue::CValue( const CValue &other )
{
	type = other.type;
	switch( type )
	{
	case VALUE_NULL:
		break;

	case VALUE_CHAR:
	case VALUE_LONG:
		value.l = other.value.l;
		break;

	case VALUE_DOUBLE:
		value.d = other.value.d;
		break;

	case VALUE_STRING:
		string = other.string;
		value.l = string[0];
		break;

	case VALUE_MAP:
		referredPtr = FALSE;
		value.p = ((CMapStrToValue*)other.value.p)->Clone();
		break;

	case VALUE_PTR:
		value.p = other.value.p;
		string = other.string;
		break;
	}
}


CValue::CValue( TCHAR c )
{
	type = VALUE_CHAR;
	value.l = c;
}


CValue::CValue( long l )
{
	type = VALUE_LONG;
	value.l = l;
}

CValue::CValue( double d )
{
	type = VALUE_DOUBLE;
	value.d = d;
}

CValue::CValue( LPCTSTR s )
{
	type = VALUE_STRING;
	string = s;
}

CValue::~CValue()
{
	Clear();
}

void CValue::Clear()
{
	if ( type == VALUE_STRING )
		string = L"";

	if ( type == VALUE_MAP && !referredPtr )
		delete (CMapStrToValue*)value.p;

	type = VALUE_NULL;
}

BOOL CValue::IsNull() const
{
	return ( type == VALUE_NULL );
}

BYTE CValue::GetType() const
{
	return type;
}

BOOL CValue::IsDouble() const
{
	BOOL isDouble = FALSE;

	switch( type )
	{
	case VALUE_DOUBLE:
		isDouble = TRUE;
		break;

	case VALUE_STRING:
		isDouble = ( string.Find('.') != -1 );
		break;
	}

	return isDouble;
}

BOOL CValue::IsValidNumber() const
{
	BOOL isNumber = FALSE;

	switch( type )
	{
	case VALUE_CHAR:
	case VALUE_LONG:
	case VALUE_DOUBLE:
		isNumber = TRUE;
		break;

	case VALUE_STRING:
		isNumber = TRUE;
		if ( string.GetLength() == 0 )
			isNumber = FALSE;
		else
		{
			for ( int i=0; i<string.GetLength(); i++ )
			{
				if (  ( string.GetAt(i) < '0' || string.GetAt(i) > '9' )
					&& string.GetAt(i) != '.' )
				{
					isNumber = FALSE;
					break;
				}
			}
		}
		break;
	}

	return isNumber;
}

CMapStrToValue* CValue::GetMap()
{
	if ( type != VALUE_MAP )
	{
		Clear();
		type = VALUE_MAP;
		value.p = new CMapStrToValue();
		referredPtr = FALSE;
	}
	return (CMapStrToValue*)value.p;
}


TCHAR	CValue::operator=	( const TCHAR   cValue )
{
	Clear();
	type = VALUE_CHAR;
	value.l = cValue;
	return cValue;
}

long	CValue::operator=	( const long   lValue )
{
	Clear();
	type = VALUE_LONG;
	value.l = lValue;
	return lValue;
}

double	CValue::operator=	( const double dValue )
{
	Clear();
	type = VALUE_DOUBLE;
	value.d = dValue;
	return dValue;
}

LPCTSTR	CValue::operator=	( const TCHAR* sValue )
{
	Clear();
	type = VALUE_STRING;
	string = sValue;
	value.l = string[0];
	return (LPCTSTR)string;
}

CValue&	CValue::operator=	( const CValue& other )
{
	Clear();
	type = other.type;
	switch( type )
	{
	case VALUE_NULL:
		break;

	case VALUE_CHAR:
	case VALUE_LONG:
		value.l = other.value.l;
		break;

	case VALUE_DOUBLE:
		value.d = other.value.d;
		break;

	case VALUE_STRING:
		string = other.string;
		value.l = string[0];
		break;

	case VALUE_MAP:
		referredPtr = FALSE;
		value.p = ((CMapStrToValue*)other.value.p)->Clone();
		break;

	case VALUE_PTR:
		value.p = other.value.p;
		string = other.string;
		break;
	}
	return *this;
}

void CValue::SetPtr( void *ptr, LPCTSTR desc )
{
	type = VALUE_PTR;
	value.p = ptr;
	if ( desc != NULL )
		string = desc;
	else
		string = L"[Pointer]";
}

void CValue::CopyFrom( const CValue& other )
{
	Clear();
	type = other.type;
	switch( type )
	{
	case VALUE_NULL:
		break;

	case VALUE_CHAR:
		value.l = other.value.l;
		break;

	case VALUE_LONG:
		value.l = other.value.l;
		break;

	case VALUE_DOUBLE:
		value.d = other.value.d;
		break;

	case VALUE_STRING:
		string = other.string;
		value.l = string[0];
		break;

	case VALUE_MAP:
		referredPtr = FALSE;
		value.p = ((CMapStrToValue*)other.value.p)->Clone();
		break;

	case VALUE_PTR:
		value.p = other.value.p;
		string = other.string;
		break;
	}
}

/*
const void*	CValue::operator=	( const void*  pValue )
{
	Clear();
	type = VALUE_PTR;
	value.p = pValue;
	return pValue;
}
*/

CValue::operator TCHAR()	const
{
	TCHAR r = 0;

	switch( type )
	{
	case VALUE_NULL:
		r = 0;
		break;

	case VALUE_CHAR:
	case VALUE_LONG:
		r = (TCHAR)value.l;
		break;

	case VALUE_DOUBLE:
		r = (TCHAR)floor( value.d + .5 );
		break;

	case VALUE_STRING:
		r = (TCHAR)string[0];
		break;

	case VALUE_MAP:
		r = 0;
		break;

	case VALUE_PTR:
		r = (TCHAR)value.p;
		break;
	}

	return r;
}

CValue::operator long()	const
{
	long r = 0;

	switch( type )
	{
	case VALUE_NULL:
		r = 0;
		break;

	case VALUE_CHAR:
		r = value.l;
		break;

	case VALUE_LONG:
		r = value.l;
		break;

	case VALUE_DOUBLE:
		r = (long)floor( value.d + .5 );
		break;

	case VALUE_STRING:
		r = _wtol( (LPCTSTR)string );
		break;

	case VALUE_MAP:
		r = 0;
		break;

	case VALUE_PTR:
		r = (long)value.p;
		break;
	}

	return r;
}

CValue::operator double() const
{
	USES_CONVERSION;

	double r = 0;

	switch( type )
	{
	case VALUE_NULL:
		r = 0;
		break;

	case VALUE_CHAR:
	case VALUE_LONG:
		r = (double)value.l;
		break;

	case VALUE_DOUBLE:
		r = value.d;
		break;

	case VALUE_STRING:
		r = atof( T2CA( (LPCTSTR)string ) );
		break;

	case VALUE_MAP:
	case VALUE_PTR:
		r = 0;
		break;
	}

	return r;
}

CValue::operator CStr&()
{
	switch( type )
	{

	case VALUE_CHAR:
		string.Format( L"%c", value.l );
		break;

	case VALUE_LONG:
		string.Format( L"%d", value.l );
		break;

	case VALUE_DOUBLE:
		string.Format( L"%f", value.d );
		string.TrimRight(L"0");
		if (string.Right(1).Compare(L".")==0) string += L'0';
		break;

	case VALUE_STRING:
	case VALUE_PTR:
		break;

	case VALUE_MAP:
		string = L"[Array]";
		break;

	default:
		string = L"";
	}

	return string;
}

CValue::operator LPCTSTR()
{
	switch( type )
	{

	case VALUE_CHAR:
		string.Format( L"%c", value.l );
		break;

	case VALUE_LONG:
		string.Format( L"%d", value.l );
		break;

	case VALUE_DOUBLE:
		string.Format( L"%f", value.d );
		string.TrimRight(L"0");
		if (string.Right(1).Compare(L".")==0) string += L'0';
		break;

	case VALUE_STRING:
	case VALUE_PTR:
		break;

	case VALUE_MAP:
		string = L"[Array]";
		break;

	default:
		string = L"";
	}

	return (LPCTSTR)string;
}

CValue::operator const void*() const
{
	if ( type == VALUE_PTR )
		return value.p;
	else
		return NULL;
}
