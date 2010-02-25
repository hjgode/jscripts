// Commands to handle data values (strings, numbers, etc.)

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_DIALOGS_H

BOOL    CmdClear( CInterpreter &interpreter, CStr &param );
//BOOL    CmdLocal( CInterpreter &interpreter, CStr &param );
//BOOL    CmdGlobal( CInterpreter &interpreter, CStr &param );
//BOOL	CmdReturn( CInterpreter &interpreter, CStr &param );

BOOL    CmdEval( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetRGB( CInterpreter &interpreter, CStr &param );
BOOL	CmdGetFilePath( CInterpreter &interpreter, CStr &param );
BOOL	CmdGetFileBase( CInterpreter &interpreter, CStr &param );
BOOL	CmdGetFileExt( CInterpreter &interpreter, CStr &param );
BOOL    CmdSplit( CInterpreter &interpreter, CStr &param );
BOOL	CmdGetPart( CInterpreter &interpreter, CStr &param );
BOOL	CmdSubStr( CInterpreter &interpreter, CStr &param );
BOOL	CmdFind( CInterpreter &interpreter, CStr &param );
BOOL	CmdReverseFind( CInterpreter &interpreter, CStr &param );
BOOL	CmdMakeUpper( CInterpreter &interpreter, CStr &param );
BOOL	CmdMakeLower( CInterpreter &interpreter, CStr &param );
#ifdef CELE
BOOL    CmdTGZX( CInterpreter &interpreter, CStr &param );
#endif
#endif