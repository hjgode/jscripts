// Value.h: interface for the CValue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VALUE_H__15CB419D_731B_4B66_ABE1_67B8EFA95662__INCLUDED_)
#define AFX_VALUE_H__15CB419D_731B_4B66_ABE1_67B8EFA95662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VALUE_NULL   0	// unused
#define VALUE_LONG   1	// integer value
#define VALUE_DOUBLE 2	// float value
#define VALUE_STRING 3	// string value
#define VALUE_MAP    4	// Map value ("arrays")
#define VALUE_PTR    5	// internal pointer (e.g. internal CArrays)

#define VALUE_CHAR   6	// char value

#include "mortstring.h"

class CMapStrToValue;

class CValue  
{
protected:
	BYTE type;
	union
	{
		//TCHAR   c;
		long	l;
		double	d;
		const void	*p;
	} value;

	BOOL referredPtr;
	CStr string;

public:
	CValue();
	CValue( const CValue &other );
	CValue( TCHAR );
	CValue( long );
	CValue( double );
	CValue( LPCTSTR );
	~CValue();

	void	Clear();
	BOOL	IsNull() const;
	BYTE	GetType() const;
	BOOL	IsDouble() const;
	BOOL	IsValidNumber() const;

	TCHAR	operator=	( const TCHAR   cValue );
	long	operator=	( const long   lValue );
	double	operator=	( const double dValue );
	LPCTSTR	operator=	( const TCHAR* sValue );
	CValue&	operator=	( const CValue& value );
	//const void*	operator=	( const void*  pValue );

	void	SetPtr( void *ptr, LPCTSTR desc = NULL );
	void	CopyFrom( const CValue& value );

	CMapStrToValue* GetMap();

    operator TCHAR() const;
    operator long()	const;
    operator double() const;
    operator CStr&();
    operator LPCTSTR();
    operator const void*() const;
};

#endif // !defined(AFX_VALUE_H__15CB419D_731B_4B66_ABE1_67B8EFA95662__INCLUDED_)
