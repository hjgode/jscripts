#ifndef MORTSTRINGTOVALUE_H
#define MORTSTRINGTOVALUE_H

#include "mortstring.h"
#include "mortstringarray.h"

#include "Value.h"
#include "ValueArray.h"

class CMapStrToValue
{
protected:
	CStrArray Keys;
	CValueArray  Values;

public:
			 CMapStrToValue();

	void     SetAt( LPCTSTR key, const CValue &value );
	void     SetAt( LPCTSTR key, LPCTSTR value );
	BOOL     Lookup( LPCTSTR key, CValue *&value );
	CValue	*GetAt( LPCTSTR key );
	void	 RemoveAll();
    int      GetStartPosition();
    void	 GetNextAssoc( int &pos, CStr &section, CValue &value );
	int		 GetSize();
	CMapStrToValue *Clone();
};
#endif