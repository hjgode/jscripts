// Functions regarding windows, controls, screen infos, ...

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_DIALOGS_H

BOOL	CmdShow( CInterpreter &interpreter, CStr &param );
BOOL	CmdMinimize( CInterpreter &interpreter, CStr &param );
BOOL	CmdClose( CInterpreter &interpreter, CStr &param );
	
BOOL	CmdWaitFor( CInterpreter &interpreter, CStr &param );
BOOL	CmdWaitForActive( CInterpreter &interpreter, CStr &param );

BOOL    CmdGetColorAt( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetWindowPos( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetWindowText( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetActiveWindow( CInterpreter &interpreter, CStr &element );

BOOL    CmdSendCommand( CInterpreter &interpreter, CStr &param );
BOOL    CmdSendMessage( CInterpreter &interpreter, CStr &param );
BOOL    CmdPostMessage( CInterpreter &interpreter, CStr &param );

#endif