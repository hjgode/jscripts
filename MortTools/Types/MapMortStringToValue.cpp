
#include "mortstring.h"
#include "mapmortstringtovalue.h"

CMapStrToValue::CMapStrToValue()
{
}

void CMapStrToValue::SetAt( LPCTSTR key, const CValue &value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		Values.SetAt( idx, value );
	}
	else
	{
		Keys.InsertAt( idx, key );
		Values.InsertAt( idx, value );
	}
}

void CMapStrToValue::SetAt( LPCTSTR key, LPCTSTR value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	CValue val(value);
	if ( exists )
	{
		Values.SetAt( idx, val );
	}
	else
	{
		Keys.InsertAt( idx, key );
		Values.InsertAt( idx, val );
	}
}

BOOL CMapStrToValue::Lookup( LPCTSTR key, CValue *&value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		value = &Values.GetAt( idx );
	}
	else
	{
		value = NULL;
	}

	return exists;
}

void CMapStrToValue::RemoveAll()
{
	Keys.RemoveAll();
	Values.RemoveAll();
}

int CMapStrToValue::GetStartPosition()
{
    if ( Keys.GetSize() > 0 ) return 1;
                    else      return NULL;
}

void CMapStrToValue::GetNextAssoc( int &pos, CStr &key, CValue &value )
{
    key    = Keys.GetAt( pos-1 );
    value  = Values.GetAt( pos-1 );
    pos++;
    if ( pos > Keys.GetSize() ) pos = NULL;
}

int CMapStrToValue::GetSize()
{
	return Keys.GetSize();
}

CMapStrToValue *CMapStrToValue::Clone()
{
	CMapStrToValue *newMap = new CMapStrToValue();
	CValue newValue;
	for ( int i=0; i<Keys.GetSize(); i++ )
	{
		newMap->Keys.Add( Keys.GetAt(i) );
		newValue.CopyFrom( Values.GetAt(i) );
		newMap->Values.Add( newValue );
	}
	return newMap;
}