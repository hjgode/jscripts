// Commands for status messages

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_STATUS_H

BOOL	CmdStatusInfo           ( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusType           ( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusHistorySize    ( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusMessage		( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusRemoveLastMessage( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusMessageAppend  (CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusClear          ( CInterpreter &interpreter, CStr &params );
BOOL	CmdStatusShow           ( CInterpreter &interpreter, CStr &params );
BOOL    CmdStatusListEntryFormat( CInterpreter &interpreter, CStr &params );
BOOL	CmdWriteStatusHistory   ( CInterpreter &interpreter, CStr &params );

#endif