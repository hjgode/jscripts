// CValueArray.cpp: implementation of the CValueArray class.
//
//////////////////////////////////////////////////////////////////////

#include "value.h"
#include "valuearray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueArray::CValueArray()
{
    data = (CValue**)malloc( sizeof(CValue*) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

CValueArray::~CValueArray()
{
    RemoveAll();
    free( data );
}

void CValueArray::RemoveAll()
{
    for ( int i=0; i<nDataLength; i++ )
    {
        delete data[i];
    }
    free( data );
    data = (CValue**)malloc( sizeof(CValue*) * 10 );
    nDataLength  = 0;
    nAllocLength = 10;
}

int  CValueArray::Add( const CValue &value )
{
    if ( nDataLength >= nAllocLength )
    {
        nAllocLength+=10;
        CValue **newData = (CValue**)malloc( sizeof(CValue*) * nAllocLength );
        memcpy( newData, data, sizeof(CValue*) * nDataLength );
        free( data );
        data = newData;
    }

	CValue *newValue = new CValue();
	newValue->CopyFrom( value );
    data[nDataLength] = newValue;
    nDataLength++;

    return nDataLength-1;
}

void    CValueArray::Append( const CValueArray &otherArray )
{
	for ( int i=0; i < otherArray.nDataLength; i++ )
	{
		Add( *otherArray.data[i] );
	}
}

void    CValueArray::SetAt( int idx, const CValue &value )
{
    if ( idx < nDataLength && idx != -1 )
    {
        delete data[idx];
		CValue *newValue = new CValue();
		newValue->CopyFrom( value );
		data[idx] = newValue;
	}
	else
	{
		Add( value );
	}
}

void    CValueArray::InsertAt( int idx, const CValue &value )
{
    if ( idx < nDataLength && idx != -1 )
    {
		if ( nDataLength >= nAllocLength )
		{
			nAllocLength+=10;
			CValue **newData = (CValue**)malloc( sizeof(CValue*) * nAllocLength );
			memcpy( newData, data, sizeof(CValue*) * nDataLength );
			free( data );
			data = newData;
		}

		// Copy entries from the insert index to a temporary buffer
		void *newData = malloc( sizeof(CValue*) * (nDataLength-idx) );
		memcpy( newData, &(data[idx]), sizeof(CValue*) * (nDataLength-idx) );
		// Copy entries from the temporary buffer to the new position (one up)
		memcpy( &(data[idx+1]), newData, sizeof(CValue*) * (nDataLength-idx) );
		// Free temporary buffer
		free( newData );

		CValue *newValue = new CValue();
		newValue->CopyFrom( value );
		data[idx] = newValue;

		nDataLength++;
	}
	else
	{
		Add( value );
	}
}

void    CValueArray::RemoveAt( int idx )
{
    if ( idx < nDataLength )
    {
        delete data[idx];
        if ( idx < nDataLength - 1 )
        {
            memcpy( data+idx, data+(idx+1), (nDataLength-idx-1) * sizeof(CValue*) );
        }
        nDataLength--;
    }
}

CValue& CValueArray::GetAt( int idx )
{
    if ( idx < nDataLength )
        return *(data[idx]);
    else
        return empty;
}

int CValueArray::GetSize() const
{
    return nDataLength;
}

CValue& CValueArray::operator[](int idx)
{
    return GetAt(idx);
}
