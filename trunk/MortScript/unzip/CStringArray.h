// CStringArray.h: interface for the CStringArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
#define AFX_CSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringArray  
{
protected:
    LPTSTR *data;
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation

public:
	CStringArray();
	virtual ~CStringArray();

    void    RemoveAll();
    int     Add( LPCTSTR string );
    void    SetAt( int idx, LPCTSTR string );
    void    Insert( int idx, LPCTSTR string );
    void    RemoveAt( int idx );
    CString GetAt( int idx ) const;
    int     GetSize() const;
    BOOL    FindEntry( LPCTSTR string, int &pos ) const;
    CString operator[](int idx) const;
};

#endif // !defined(AFX_CSTRINGARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
