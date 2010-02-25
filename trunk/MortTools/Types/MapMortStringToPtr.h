#ifndef MORTSTRINGTOPTR_H
#define MORTSTRINGTOPTR_H

#include "morttypes.h"

class CMapStrToPtr
{
protected:
	CStrArray Keys;
	CMortPtrArray    Values;

public:
		 CMapStrToPtr();

	void SetAt( LPCTSTR key, void *value );
	BOOL Lookup( LPCTSTR key, void *&value );
	void RemoveAll();
	void RemoveKey( LPCTSTR key );
    int  GetStartPosition();
    void GetNextAssoc( int &pos, CStr &section, void *&values );
	int GetCount();
};

#endif