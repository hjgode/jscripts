// CMortUIntArray.cpp: implementation of the CMortUIntArray class.
//
//////////////////////////////////////////////////////////////////////

#include "morttypes.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMortUIntArray::CMortUIntArray()
{
    data = (UINT*)malloc( sizeof(UINT) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

CMortUIntArray::~CMortUIntArray()
{
    free( data );
}

void CMortUIntArray::RemoveAll()
{
    free( data );
    data = (UINT*)malloc( sizeof(UINT) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

int  CMortUIntArray::Add( UINT value )
{
    if ( nDataLength >= nAllocLength )
    {
        nAllocLength+=10;
        UINT *newData = (UINT*)malloc( sizeof(UINT) * nAllocLength );
        memcpy( newData, data, sizeof(UINT) * nDataLength );
        free( data );
        data = newData;
    }

    data[nDataLength] = value;
    nDataLength++;

    return nDataLength-1;
}

void    CMortUIntArray::SetAt( int idx, UINT value )
{
    if ( idx < nDataLength && idx != -1 )
    {
		data[idx] = value;
	}
	else
	{
		Add( value );
	}
}

void    CMortUIntArray::InsertAt( int idx, UINT value )
{
    if ( idx < nDataLength && idx != -1 )
    {
		if ( nDataLength >= nAllocLength )
		{
			nAllocLength+=10;
			UINT *newData = (UINT*)malloc( sizeof(UINT) * nAllocLength );
			memcpy( newData, data, sizeof(UINT) * nDataLength );
			free( data );
			data = newData;
		}

		// Copy entries from the insert index to a temporary buffer
		void *newData = malloc( sizeof(UINT) * (nDataLength-idx) );
		memcpy( newData, &(data[idx]), sizeof(UINT) * (nDataLength-idx) );
		// Copy entries from the temporary buffer to the new position (one up)
		memcpy( &(data[idx+1]), newData, sizeof(UINT) * (nDataLength-idx) );
		// Free temporary buffer
		free( newData );

		data[idx] = value;

		nDataLength++;
	}
	else
	{
		Add( value );
	}
}

void    CMortUIntArray::RemoveAt( int idx )
{
    if ( idx < nDataLength )
    {
        if ( idx < nDataLength - 1 )
        {
            memcpy( data+idx*sizeof(UINT), data+(idx+1)*sizeof(UINT), nDataLength-idx-1 );
        }
        nDataLength--;
    }
}

UINT CMortUIntArray::GetAt( int idx ) const
{
    if ( idx < nDataLength )
        return data[idx];
    else
        return 0;
}

int CMortUIntArray::GetSize() const
{
    return nDataLength;
}

int CMortUIntArray::GetUpperBound() const
{
    return nDataLength-1;
}

BOOL CMortUIntArray::FindEntry( long value, int &pos ) const
{
	int start = 0, end = nDataLength-1;
	int cmp;
	//BOOL found = FALSE;

	if ( nDataLength == 0 )
	{
		pos = -1;
		return FALSE;
	}
	for(pos=0;pos<nDataLength;pos++){
		cmp = value - data[pos];
		if (cmp==0) return TRUE;
	}
	return FALSE;
}

UINT &CMortUIntArray::operator[](int idx)
{
    if ( idx < nDataLength )
        return data[idx];
    else
    {
        dummy = 0;
        return dummy;
    }
}
