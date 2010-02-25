// Commands that show a dialog

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_DIALOGS_H

BOOL	CmdMessage              ( CInterpreter &interpreter, CStr &params );
BOOL	CmdBigMessage           ( CInterpreter &interpreter, CStr &params );
BOOL	CmdInput                ( CInterpreter &interpreter, CStr &params );
BOOL	CmdSleepMessage			( CInterpreter &interpreter, CStr &params );
BOOL	CmdSetChoiceEntryFormat ( CInterpreter &interpreter, CStr &params );
BOOL	CmdSetMessageFont       ( CInterpreter &interpreter, CStr &params );

#endif