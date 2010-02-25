// CStringArray.cpp: implementation of the CStringArray class.
//
//////////////////////////////////////////////////////////////////////

#include "windows.h"
#include "string.h"
#include "mortafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringArray::CStringArray()
{
    data = (LPTSTR*)malloc( sizeof(LPCTSTR) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

CStringArray::~CStringArray()
{
    RemoveAll();
    free( data );
}

void CStringArray::RemoveAll()
{
    for ( int i=0; i<nDataLength; i++ )
    {
        free( data[i] );
    }
    free( data );
    data = (LPTSTR*)malloc( sizeof(LPCTSTR) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

int  CStringArray::Add( LPCTSTR string )
{
    if ( nDataLength >= nAllocLength )
    {
        nAllocLength+=10;
        TCHAR **newData = (LPTSTR*)malloc( sizeof(LPTSTR) * nAllocLength );
        memcpy( newData, data, sizeof(LPTSTR) * nDataLength );
        free( data );
        data = newData;
    }

    LPTSTR buffer = (LPTSTR)malloc( sizeof(TCHAR) * (wcslen(string)+1) );
    wcscpy( buffer, string );
    data[nDataLength] = buffer;
    nDataLength++;

    return nDataLength-1;
}

void    CStringArray::SetAt( int idx, LPCTSTR string )
{
    if ( idx < nDataLength && idx != -1 )
    {
        free( data[idx] );
		LPTSTR buffer = (LPTSTR)malloc( sizeof(TCHAR) * (wcslen(string)+1) );
		wcscpy( buffer, string );
		data[idx] = buffer;
	}
	else
	{
		Add( string );
	}
}

void    CStringArray::Insert( int idx, LPCTSTR string )
{
    if ( idx < nDataLength && idx != -1 )
    {
		if ( nDataLength > nAllocLength )
		{
			nAllocLength+=10;
			TCHAR **newData = (LPTSTR*)malloc( sizeof(LPTSTR) * nAllocLength );
			memcpy( newData, data, sizeof(LPTSTR) * nDataLength );
			free( data );
			data = newData;
		}

		// Copy entries from the insert index to a temporary buffer
		void *newData = malloc( sizeof(LPTSTR) * (nDataLength-idx) );
		memcpy( newData, &(data[idx]), sizeof(LPTSTR) * (nDataLength-idx) );
		// Copy entries from the temporary buffer to the new position (one up)
		memcpy( &(data[idx+1]), newData, sizeof(LPTSTR) * (nDataLength-idx) );
		// Free temporary buffer
		free( newData );

		LPTSTR buffer = (LPTSTR)malloc( sizeof(TCHAR) * (wcslen(string)+1) );
		wcscpy( buffer, string );
		data[idx] = buffer;
	}
	else
	{
		Add( string );
	}
}

void    CStringArray::RemoveAt( int idx )
{
    if ( idx < nDataLength )
    {
        free( data[idx] );
        if ( idx < nDataLength - 1 )
        {
            memcpy( data+idx*sizeof(LPTSTR), data+(idx+1)*sizeof(LPTSTR), nDataLength-idx-1 );
        }
        nDataLength--;
    }
}

CString CStringArray::GetAt( int idx ) const
{
    if ( idx < nDataLength )
        return CString(data[idx]);
    else
        return CString();
}

int CStringArray::GetSize() const
{
    return nDataLength;
}

BOOL CStringArray::FindEntry( LPCTSTR string, int &pos ) const
{
	int start = 0, end = nDataLength-1;
	int cmp;
	BOOL found = FALSE;

	if ( nDataLength == 0 )
	{
		pos = -1;
		return FALSE;
	}

	while ( end-start > 5 && start >= 0 && end < nDataLength && found == FALSE )
	{
		pos = int( (end-start)/2 )+start;
		cmp = wcscmp( string, data[pos] );
		if ( cmp == 0 )
			found = TRUE;
		else
		{
			if ( cmp < 0 )
				end = pos-1;
			else
				start = pos+1;
		}
	}

	if ( !found )
	{
		if ( start < 0 ) start = 0;
		if ( end >= nDataLength ) end = nDataLength-1;
		pos = start;
		while ( wcscmp( string, data[pos] ) > 0 && pos < end )
		{
			pos++;
		}
		if ( wcscmp( string, data[pos] ) == 0 )
		{
			found = TRUE;
		}
		else
		{
			if ( pos == end ) pos = -1;
			else
			{
				while ( pos > 0 && wcscmp( string, data[pos-1] ) < 0 )
				{
					pos--;
				}
			}
		}
	}
	return found;
}

CString CStringArray::operator[](int idx) const
{
    return GetAt(idx);
}
