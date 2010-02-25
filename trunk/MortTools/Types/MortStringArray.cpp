// CStrArray.cpp: implementation of the CStrArray class.
//
//////////////////////////////////////////////////////////////////////

#include "mortstringarray.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrArray::CStrArray()
{
    data = (LPTSTR*)malloc( sizeof(LPCTSTR) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

CStrArray::~CStrArray()
{
    RemoveAll();
    free( data );
}

void CStrArray::RemoveAll()
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

int  CStrArray::Add( LPCTSTR string )
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

void    CStrArray::Append( const CStrArray &otherArray )
{
	for ( int i=0; i < otherArray.nDataLength; i++ )
	{
		Add( otherArray.data[i] );
	}
}

void    CStrArray::SetAt( int idx, LPCTSTR string )
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

void    CStrArray::InsertAt( int idx, LPCTSTR string )
{
    if ( idx < nDataLength && idx != -1 )
    {
		if ( nDataLength >= nAllocLength )
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

		nDataLength++;
	}
	else
	{
		Add( string );
	}
}

void    CStrArray::RemoveAt( int idx )
{
    if ( idx < nDataLength )
    {
        free( data[idx] );
        if ( idx < nDataLength - 1 )
        {
            memcpy( data+idx, data+(idx+1), (nDataLength-idx-1) * sizeof(LPTSTR) );
        }
        nDataLength--;
    }
}

CStr &CStrArray::GetAt( int idx )
{
    if ( idx < nDataLength )
		tmpString = data[idx];
    else
        tmpString.Empty();
	return tmpString;
}

int CStrArray::GetSize() const
{
    return nDataLength;
}

BOOL CStrArray::FindEntry( LPCTSTR string, int &pos ) const
{
	int start = 0, end = nDataLength-1;
	int cmp;
	BOOL found = FALSE;

	if ( nDataLength == 0 ){
		pos = -1;
		return FALSE;
	}

	while ( end-start > 5 && start >= 0 && end < nDataLength && found == FALSE ){
		pos = int( (end-start)/2 )+start;
		cmp = wcscmp( string, data[pos] );
		if ( cmp == 0 )
			found = TRUE;
		else{
			if ( cmp < 0 )
				end = pos-1;
			else
				start = pos+1;
		}
	}

	if ( !found ){
		if ( start < 0 ) start = 0;
		if ( end >= nDataLength ) end = nDataLength-1;
		pos = start;
		while ( pos < end && wcscmp( string, data[pos] ) > 0 ){
			pos++;
		}
		if ( wcscmp( string, data[pos] ) == 0 ){
			found = TRUE;
		}else{
			if ( pos == end ){
				if ( pos < nDataLength && wcscmp( string, data[pos] ) > 0 )
					pos++;
				if ( pos > nDataLength ) pos = -1;
			}else{
				while ( pos > 0 && wcscmp( string, data[pos-1] ) < 0 ){
					pos--;
				}
			}
		}
	}
	return found;
}

CStr &CStrArray::operator[](int idx)
{
    return GetAt(idx);
}
