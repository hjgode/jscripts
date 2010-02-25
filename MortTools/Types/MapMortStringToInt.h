#ifndef MORTSTRINGTOINT_H
#define MORTSTRINGTOINT_H

#include "morttypes.h"

class CMapStrToInt
{
protected:
	CStrArray Keys;
	CMortUIntArray	 Values;

public:
		 CMapStrToInt();

	void SetAt( LPCTSTR key, long value );
	BOOL Lookup( LPCTSTR key, long &value );
	long GetAt( LPCTSTR key );
	void RemoveAll();
    int  GetStartPosition();
    void GetNextAssoc( int &pos, CStr &key, long &value );
	int  GetCount();
};

#endif