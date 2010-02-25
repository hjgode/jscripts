// CStrArray.h: interface for the CStrArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CStrARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
#define AFX_CStrARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mortstring.h"

class CStrArray  
{
protected:
    LPTSTR *data;
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation
	CStr tmpString;

public:
	CStrArray();
	virtual ~CStrArray();

    void    RemoveAll();
    int     Add( LPCTSTR string );
	void    Append( const CStrArray &otherArray );
    void    SetAt( int idx, LPCTSTR string );
    void    InsertAt( int idx, LPCTSTR string );
    void    RemoveAt( int idx );
    CStr	&GetAt( int idx );
    int     GetSize() const;
    BOOL    FindEntry( LPCTSTR string, int &pos ) const;
    CStr &operator[](int idx);
};

#endif // !defined(AFX_CStrARRAY_H__ABD52C57_0A79_4B95_A49E_1C3EF894010A__INCLUDED_)
