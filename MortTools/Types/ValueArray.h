// CStringArray.h: interface for the CStringArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CVALUEARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
#define AFX_CVALUEARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "value.h"

class CValueArray  
{
protected:
    CValue **data;
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation
	CValue empty;

public:
	CValueArray();
	virtual ~CValueArray();

    void    RemoveAll();
    int     Add( const CValue &value );
	void    Append( const CValueArray &otherArray );
    void    SetAt( int idx, const CValue &value );
    void    InsertAt( int idx, const CValue &value );
    void    RemoveAt( int idx );
    CValue& GetAt( int idx );
    int     GetSize() const;
    CValue& operator[](int idx);
};

#endif // !defined(AFX_CSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
