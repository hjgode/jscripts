// Commands to handle data values (strings, numbers, etc.)

#include "morttypes.h"
#include "Helpers.h"
#include "interpreter.h"

#ifndef COMMANDS_SYSTEM_H

#ifndef DESKTOP
#ifndef PNA
BOOL    CmdConnect( CInterpreter &interpreter, CStr &param );
BOOL    CmdDisconnect( CInterpreter &interpreter, CStr &param );
BOOL	CmdCloseConnection( CInterpreter &interpreter, CStr &param );
#endif

BOOL	CmdRotate( CInterpreter &interpreter, CStr &param );
BOOL    CmdRedrawToday( CInterpreter &interpreter, CStr &param );
BOOL	CmdPowerOff( CInterpreter &interpreter, CStr &param );
BOOL	CmdIdleTimerReset( CInterpreter &interpreter, CStr &param );
BOOL	CmdReset( CInterpreter &interpreter, CStr &param );
#endif

BOOL	CmdSetVolume( CInterpreter &interpreter, CStr &param );
BOOL	CmdVibrate( CInterpreter &interpreter, CStr &param );
BOOL	CmdPlaySnd( CInterpreter &interpreter, CStr &param );

#ifdef POCKETPC
BOOL	CmdSetBacklight( CInterpreter &interpreter, CStr &param );
BOOL	CmdToggleDisplay( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetInput( CInterpreter &interpreter, CStr &param );
BOOL	CmdShowInput( CInterpreter &interpreter, CStr &param );
BOOL	CmdHideInput( CInterpreter &interpreter, CStr &param );
BOOL	CmdHardReset( CInterpreter &interpreter, CStr &param );
#endif
#ifdef SMARTPHONE
BOOL	CmdSmartphoneBackLight(void);
#endif
BOOL	CmdSleep( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetTime( CInterpreter &interpreter, CStr &param );
BOOL    CmdSetTime( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetActiveProcess( CInterpreter &interpreter, CStr &param );
BOOL    CmdGetSystemPath( CInterpreter &interpreter, CStr &param );
BOOL	CmdGetMortScriptType( CInterpreter &interpreter, CStr &param );

BOOL	CmdGetClipText( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetClipText( CInterpreter &interpreter, CStr &param );

BOOL	CmdShowWaitCursor( CInterpreter &interpreter, CStr &param );
BOOL	CmdHideWaitCursor( CInterpreter &interpreter, CStr &param );

BOOL	CmdKill( CInterpreter &interpreter, CStr &param );
BOOL	CmdKillScript( CInterpreter &interpreter, CStr &param );

BOOL	RunApp( CInterpreter &interpreter, CStr &param, BOOL wait = FALSE );
BOOL	CmdRun( CInterpreter &interpreter, CStr &param );
BOOL	CmdRunWait( CInterpreter &interpreter, CStr &param );
#ifndef DESKTOP
BOOL    CmdRunOnPowerOn( CInterpreter &interpreter, CStr &param );
BOOL    CmdRunAt( CInterpreter &interpreter, CStr &param );
BOOL    CmdRemoveNotifications( CInterpreter &interpreter, CStr &param );
#endif
#ifdef POCKETPC
BOOL	CmdNew( CInterpreter &interpreter, CStr &param );
#endif
#ifdef SMARTPHONE
BOOL	CmdSetBTState( CInterpreter &interpreter, CStr &param );
BOOL	CmdSetRadioMode( CInterpreter &interpreter, CStr &param );
#endif
#endif