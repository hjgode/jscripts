#ifndef MORTSTRINGTOSTRING_H
#define MORTSTRINGTOSTRING_H

#include "mortString.h"
#include "mortStringArray.h"

class CMapStrToString
{
protected:
	CStrArray Keys;
	CStrArray Values;

public:
		 CMapStrToString();

	void SetAt( LPCTSTR key, LPCTSTR value );
	BOOL Lookup( LPCTSTR key, CStr &value );
	void RemoveKey( LPCTSTR key );
	void RemoveAll();
    int  GetStartPosition();
    void GetNextAssoc( int &pos, CStr &section, CStr &value );
};

#endif