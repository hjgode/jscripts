#ifndef VARIABLES_H
#define VARIABLES_H

#include "mapmortstringtovalue.h"
#include "mapmortstringtoptr.h"

class CFileInfo
{
public:
    HANDLE      handle;
	BYTE       *data;
	ULONG		dataSize;
	int			encoding;
	BOOL		eof;

	CFileInfo( HANDLE h, int e )
	{
		handle = h;
		encoding = e;
		data = (BYTE*)malloc( 4096 );
		dataSize = 0;
		eof = FALSE;
	}

	~CFileInfo()
	{
		if ( handle != NULL ) CloseHandle( handle );
		if ( data != NULL ) free( data );
	}
};

extern CMapStrToValue  Variables;
extern CMapStrToPtr    FileHandles;
extern CMapStrToValue *LocalVariables;
extern CStr			  VarError;
extern BOOL isGlobal;
CValue *GetVariable( CStr &varName, BOOL create, int *position=NULL, BOOL local = FALSE );
CMapStrToValue *CreateAndInitLocalVariables();
BOOL	SetVariable( CStr &varName, CValue value );
BOOL	SetVariable( CStr &varName, LPCTSTR value );
BOOL	ClearVariable( CStr &varName );

#endif