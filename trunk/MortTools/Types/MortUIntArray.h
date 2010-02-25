// CUIntArray.h: interface for the CUIntArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMortUINTARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
#define AFX_CMortUINTARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMortUIntArray  
{
protected:
    UINT *data;
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation
    UINT dummy;

public:
	CMortUIntArray();
	virtual ~CMortUIntArray();

    void    RemoveAll();
    int     Add( UINT value );
    void    SetAt( int idx, UINT value );
    void    InsertAt( int idx, UINT value );
    void    RemoveAt( int idx );
    UINT    GetAt( int idx ) const;
    int     GetSize() const;
    int     GetUpperBound() const;
    BOOL    FindEntry( long value, int &pos ) const;
    UINT    &operator[](int idx);
};

#endif // !defined(AFX_CMortSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
