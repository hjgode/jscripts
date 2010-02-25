#include "mapmortstringtostring.h"

CMapStrToString::CMapStrToString()
{
}

void CMapStrToString::SetAt( LPCTSTR key, LPCTSTR value )
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

BOOL CMapStrToString::Lookup( LPCTSTR key, CStr &value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		value = Values.GetAt( idx );
	}
	else
	{
		value.Empty();
	}

	return exists;
}

void CMapStrToString::RemoveKey( LPCTSTR key )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		Keys.RemoveAt( idx );
		Values.RemoveAt( idx );
	}
}

void CMapStrToString::RemoveAll()
{
	Keys.RemoveAll();
	Values.RemoveAll();
}

int CMapStrToString::GetStartPosition()
{
    if ( Keys.GetSize() > 0 ) return 1;
                    else      return NULL;
}

void CMapStrToString::GetNextAssoc( int &pos, CStr &section, CStr &value )
{
    section = Keys.GetAt( pos-1 );
    value   = Values.GetAt( pos-1 );
    pos++;
    if ( pos > Keys.GetSize() ) pos = NULL;
}