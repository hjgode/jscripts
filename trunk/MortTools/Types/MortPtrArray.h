// CUIntArray.h: interface for the CUIntArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMortPTRARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
#define AFX_CMortPTRARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMortPtrArray  
{
protected:
    ULONG *data;
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation
    void *dummy;

public:
	CMortPtrArray();
	virtual ~CMortPtrArray();

    void    RemoveAll();
    int     Add( void* value );
    void    SetAt( int idx, void* value );
    void    InsertAt( int idx, void* value );
    void    RemoveAt( int idx );
    void*   GetAt( int idx ) const;
    int     GetSize() const;
    int     GetUpperBound() const;
    void    *&operator[](int idx);
};

#endif // !defined(AFX_CMortSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
