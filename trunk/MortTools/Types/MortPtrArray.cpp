// CMortPtrArray.cpp: implementation of the CMortPtrArray class.
//
//////////////////////////////////////////////////////////////////////

#include "windows.h"
#include "string.h"
#include "mortafx.h"
#include "morttypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMortPtrArray::CMortPtrArray()
{
    data = (ULONG*)malloc( sizeof(ULONG) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

CMortPtrArray::~CMortPtrArray()
{
    free( data );
}

void CMortPtrArray::RemoveAll()
{
    free( data );
    data = (ULONG*)malloc( sizeof(ULONG) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

int  CMortPtrArray::Add( void* value )
{
    if ( nDataLength >= nAllocLength )
    {
        nAllocLength+=10;
        ULONG *newData = (ULONG*)malloc( sizeof(ULONG) * nAllocLength );
        memcpy( newData, data, sizeof(ULONG) * nDataLength );
        free( data );
        data = newData;
    }

    data[nDataLength] = (ULONG)value;
    nDataLength++;

    return nDataLength-1;
}

void    CMortPtrArray::SetAt( int idx, void *value )
{
    if ( idx < nDataLength && idx != -1 )
    {
		data[idx] = (ULONG)value;
	}
	else
	{
		Add( value );
	}
}

void    CMortPtrArray::InsertAt( int idx, void *value )
{
    if ( idx < nDataLength && idx != -1 )
    {
		if ( nDataLength >= nAllocLength )
		{
			nAllocLength+=10;
			ULONG *newData = (ULONG*)malloc( sizeof(ULONG) * nAllocLength );
			memcpy( newData, data, sizeof(ULONG) * nDataLength );
			free( data );
			data = newData;
		}

		// Copy entries from the insert index to a temporary buffer
		void *newData = malloc( sizeof(ULONG) * (nDataLength-idx) );
		memcpy( newData, &(data[idx]), sizeof(ULONG) * (nDataLength-idx) );
		// Copy entries from the temporary buffer to the new position (one up)
		memcpy( &(data[idx+1]), newData, sizeof(ULONG) * (nDataLength-idx) );
		// Free temporary buffer
		free( newData );

		data[idx] = (ULONG)value;

		nDataLength++;
	}
	else
	{
		Add( value );
	}
}

void    CMortPtrArray::RemoveAt( int idx )
{
    if ( idx < nDataLength )
    {
        if ( idx < nDataLength - 1 )
        {
            memcpy( data+idx*sizeof(ULONG), data+(idx+1)*sizeof(ULONG), nDataLength-idx-1 );
        }
        nDataLength--;
    }
}

void* CMortPtrArray::GetAt( int idx ) const
{
    if ( idx < nDataLength )
        return (void*)data[idx];
    else
        return 0;
}

int CMortPtrArray::GetSize() const
{
    return nDataLength;
}

int CMortPtrArray::GetUpperBound() const
{
    return nDataLength-1;
}

void *&CMortPtrArray::operator[](int idx)
{
    if ( idx < nDataLength )
        return (void*&)data[idx];
    else
    {
        dummy = 0;
        return (void*&)dummy;
    }
}
