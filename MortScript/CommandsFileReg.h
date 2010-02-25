// Commands to handle files and registry entries

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_DIALOGS_H

#ifdef DESKTOP
BOOL    CmdChDir( CInterpreter &interpreter, CStr &param );
#endif
BOOL    CmdMkDir( CInterpreter &interpreter, CStr &param );
BOOL    CmdRmDir( CInterpreter &interpreter, CStr &param );
BOOL	CmdDeleteIntern( CInterpreter &interpreter, CStr &param, BOOL recursive = FALSE );
BOOL	CmdDelete( CInterpreter &interpreter, CStr &param );
BOOL	CmdDelTree( CInterpreter &interpreter, CStr &param );
BOOL    CmdSetProxy( CInterpreter &interpreter, CStr &param );
BOOL    CmdDownload( CInterpreter &interpreter, CStr &param );
BOOL	CmdCopy( CInterpreter &interpreter, CStr &param );
BOOL	CmdRename( CInterpreter &interpreter, CStr &param );
BOOL	CmdRecursiveCopyMove( CInterpreter &interpreter, CStr &path, CStr &filter, CStr &target, BOOL failIfExists, BOOL rec, BOOL move );
BOOL	CmdXCopy( CInterpreter &interpreter, CStr &param );
BOOL	CmdMove( CInterpreter &interpreter, CStr &param );
BOOL	CmdCreateShortcut( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetVersion( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetMortScriptVersion( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetFileAttribs( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetFileAttribute( CInterpreter &interpreter, CStr &param );

BOOL	CmdZipFile( CInterpreter &interpreter, CStr &param );
BOOL	CmdZipFiles( CInterpreter &interpreter, CStr &param );
BOOL	CmdUnzipFile( CInterpreter &interpreter, CStr &param );
BOOL	CmdUnzipAll( CInterpreter &interpreter, CStr &param );
BOOL	CmdUnzipPath( CInterpreter &interpreter, CStr &param );

BOOL	CmdRegWriteString( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegWriteDWord( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegWriteBinary( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegWriteMultiString( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegReadString( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegReadDWord( CInterpreter &interpreter, CStr &param );
BOOL	CmdRegReadBinary( CInterpreter &interpreter, CStr &param );
BOOL    CmdRegDelete( CInterpreter &interpreter, CStr &param );
BOOL    CmdRegDeleteKey( CInterpreter &interpreter, CStr &param );
void    CmdRegDeleteSubKey( HKEY key, LPCTSTR subkeyName, BOOL values, BOOL recursive );

BOOL	CmdIniRead( CInterpreter &interpreter, CStr &param );
BOOL	CmdIniWrite( CInterpreter &interpreter, CStr &param );
BOOL    CmdReadFile( CInterpreter &interpreter, CStr &param );
BOOL    CmdWriteFile( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetComInfo( CInterpreter &interpreter, CStr &param );

#endif