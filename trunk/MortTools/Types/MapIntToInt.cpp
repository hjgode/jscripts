// CPtrArray.cpp: implementation of the CPtrArray class.
//
//////////////////////////////////////////////////////////////////////

#include "mapinttoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapIntToInt::CMapIntToInt()
{
}

void CMapIntToInt::SetAt( long key, long value )
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

BOOL CMapIntToInt::Lookup( long key, long &value )
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

void CMapIntToInt::RemoveAll()
{
	Keys.RemoveAll();
	Values.RemoveAll();
}

int CMapIntToInt::GetStartPosition()
{
    if ( Keys.GetSize() > 0 ) return 1;
                    else      return NULL;
}

void CMapIntToInt::GetNextAssoc( int &pos, long &key, long &value )
{
    key    = Keys.GetAt( pos-1 );
    value  = Values.GetAt( pos-1 );
    pos++;
    if ( pos > Keys.GetSize() ) pos = NULL;
}

int CMapIntToInt::GetCount()
{
	return Keys.GetSize();
}
