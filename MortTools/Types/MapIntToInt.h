#ifndef MORTINTTOINT_H
#define MORTINTTOINT_H

#include "morttypes.h"

class CMapIntToInt
{
protected:
	CMortUIntArray Keys;
	CMortUIntArray Values;

public:
		 CMapIntToInt();

	void SetAt( long key, long value );
	BOOL Lookup( long key, long &value );
	long GetAt( long key );
	void RemoveAll();
    int  GetStartPosition();
    void GetNextAssoc( int &pos, long &key, long &value );
	int  GetCount();
};

#endif