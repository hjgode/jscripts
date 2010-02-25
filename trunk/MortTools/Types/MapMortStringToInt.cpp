// CPtrArray.cpp: implementation of the CPtrArray class.
//
//////////////////////////////////////////////////////////////////////

#include "mapmortstringtoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapStrToInt::CMapStrToInt()
{
}

void CMapStrToInt::SetAt( LPCTSTR key, long value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		Values.SetAt( idx, value );
	}
	else
	{
		if ( idx == -1 )
		{
			Keys.Add( key );
			Values.Add( value );
		}
		else
		{
			Keys.InsertAt( idx, key );
			Values.InsertAt( idx, value );
		}
	}
}

BOOL CMapStrToInt::Lookup( LPCTSTR key, long &value )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		value = Values.GetAt( idx );
	}
	else
	{
		value = 0;
	}

	return exists;
}

void CMapStrToInt::RemoveAll()
{
	Keys.RemoveAll();
	Values.RemoveAll();
}

int CMapStrToInt::GetStartPosition()
{
    if ( Keys.GetSize() > 0 ) return 1;
                    else      return NULL;
}

void CMapStrToInt::GetNextAssoc( int &pos, CStr &section, long &values )
{
    section = Keys.GetAt( pos-1 );
    values  = Values.GetAt( pos-1 );
    pos++;
    if ( pos > Keys.GetSize() ) pos = NULL;
}

int CMapStrToInt::GetCount()
{
	return Keys.GetSize();
}
