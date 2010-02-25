#include "mapmortstringtoptr.h"

CMapStrToPtr::CMapStrToPtr()
{
}

void CMapStrToPtr::SetAt( LPCTSTR key, void *value )
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

BOOL CMapStrToPtr::Lookup( LPCTSTR key, void *&value )
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

void CMapStrToPtr::RemoveKey( LPCTSTR key )
{
	int idx;
	BOOL exists = Keys.FindEntry( key, idx );
	
	if ( exists )
	{
		Keys.RemoveAt( idx );
		Values.RemoveAt( idx );
	}
}

void CMapStrToPtr::RemoveAll()
{
	Keys.RemoveAll();
	Values.RemoveAll();
}

int CMapStrToPtr::GetStartPosition()
{
    if ( Keys.GetSize() > 0 ) return 1;
                    else      return NULL;
}

void CMapStrToPtr::GetNextAssoc( int &pos, CStr &section, void *&values )
{
    section = Keys.GetAt( pos-1 );
    values  = Values.GetAt( pos-1 );
    pos++;
    if ( pos > Keys.GetSize() ) pos = NULL;
}

int CMapStrToPtr::GetCount()
{
	return Keys.GetSize();
}
