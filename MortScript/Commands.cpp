#include "Helpers.h"

#include "CommandsDialogs.h"
#include "CommandsStatus.h"
#include "CommandsFileReg.h"
#include "CommandsWindows.h"
#include "CommandsData.h"
#include "CommandsSystem.h"


//------------------------------------------------------------
//  All commands
//------------------------------------------------------------
// Alphabetially sorted! Necessary to save memory (no use to have it unsorted in the programs
// code and sorted on the heap) and speed (sorting, binary search)
function CommandsArray[] =
	{
		{ L"bigmessage",			CmdBigMessage }
#ifdef DESKTOP
	,	{ L"chdir",					CmdChDir }
#endif
	,	{ L"clear",					CmdClear }
	,	{ L"close",					CmdClose }
#if ( !defined( DESKTOP ) && !defined( PNA ) )
	,	{ L"closeconnection",		CmdCloseConnection }
	,	{ L"connect",				CmdConnect }
#endif
	,	{ L"copy",					CmdCopy }
	,	{ L"createshortcut",		CmdCreateShortcut }
	,	{ L"delete",				CmdDelete }
	,	{ L"deltree",				CmdDelTree }
#if ( !defined( DESKTOP ) && !defined( PNA ) )
	,	{ L"disconnect",			CmdDisconnect }
#endif
#ifndef SMARTPHONE
	,	{ L"download",				CmdDownload }
#endif
	,	{ L"eval",					CmdEval }
	,	{ L"find",					CmdFind }
	,	{ L"getactiveprocess",		CmdGetActiveProcess }
	,	{ L"getactivewindow",		CmdGetActiveWindow }
	,	{ L"getcliptext",			CmdGetClipText }
	,	{ L"getcolorat",			CmdGetColorAt }
	,	{ L"getfilebase",			CmdGetFileBase }
	,	{ L"getfileext",			CmdGetFileExt }
	,	{ L"getfilepath",			CmdGetFilePath }
	,	{ L"getmortscripttype",		CmdGetMortScriptType }
	,	{ L"getmortscriptversion",	CmdGetMortScriptVersion }
	,	{ L"getpart",				CmdGetPart }
	,	{ L"getrgb",				CmdGetRGB }
	,	{ L"getsystempath",			CmdGetSystemPath }
	,	{ L"gettime",				CmdGetTime }
	,	{ L"getversion",			CmdGetVersion }
	,	{ L"getwindowpos",			CmdGetWindowPos }
	,	{ L"getwindowtext",			CmdGetWindowText }
	//,	{ L"global",				CmdGlobal }
#ifdef POCKETPC
	,	{ L"hardreset",				CmdHardReset }
	,	{ L"hideinput",				CmdHideInput }
#endif
	,	{ L"hidewaitcursor",		CmdHideWaitCursor }
#ifndef DESKTOP
	,	{ L"idletimerreset",		CmdIdleTimerReset }
#endif
	,	{ L"iniread",				CmdIniRead }
	,	{ L"iniwrite",				CmdIniWrite }
	,	{ L"input",					CmdInput }
	,	{ L"kill",					CmdKill }
	,	{ L"killscript",			CmdKillScript }
//	,	{ L"local",					CmdLocal }
	,	{ L"makelower",				CmdMakeLower }
	,	{ L"makeupper",				CmdMakeUpper }
	,	{ L"message",				CmdMessage }
	,	{ L"minimize",				CmdMinimize }
	,	{ L"mkdir",					CmdMkDir }
	,	{ L"move",					CmdMove }
	,	{ L"msgbox",				CmdMessage }
	,	{ L"msgboxx",				CmdBigMessage }
#ifdef POCKETPC
	,	{ L"new",					CmdNew }
#endif
	,	{ L"playsound",				CmdPlaySnd }
	,	{ L"postmessage",			CmdPostMessage }
#ifndef DESKTOP
	,	{ L"poweroff",				CmdPowerOff }
#endif
	,	{ L"readfile",				CmdReadFile }
#ifndef DESKTOP
	,	{ L"redrawtoday",			CmdRedrawToday }
#endif
	,	{ L"regdelete",				CmdRegDelete }
	,	{ L"regdeletekey",			CmdRegDeleteKey }
	,	{ L"regreadbinary",			CmdRegReadBinary }
	,	{ L"regreaddword",			CmdRegReadDWord }
	,	{ L"regreadstring",			CmdRegReadString }
	,	{ L"regwritebinary",		CmdRegWriteBinary }
	,	{ L"regwritedword",			CmdRegWriteDWord }
	,	{ L"regwritemultistring",	CmdRegWriteMultiString }
	,	{ L"regwritestring",		CmdRegWriteString }
#ifndef DESKTOP
	,	{ L"removenotifications",	CmdRemoveNotifications }
#endif
	,	{ L"rename",				CmdRename }
#ifndef DESKTOP
	,	{ L"reset",					CmdReset }
#endif
//	,	{ L"return",				CmdReturn }
	,	{ L"reversefind",			CmdReverseFind }
	,	{ L"rmdir",					CmdRmDir }
#ifndef DESKTOP
	,	{ L"rotate",				CmdRotate }
#endif
	,	{ L"run",					CmdRun }
#ifndef DESKTOP
	,	{ L"runat",					CmdRunAt }
	,	{ L"runonpoweron",			CmdRunOnPowerOn }
#endif
	,	{ L"runwait",				CmdRunWait }
	,	{ L"sendcommand",			CmdSendCommand }
	,	{ L"sendmessage",			CmdSendMessage }
#ifdef POCKETPC
	,	{ L"setbacklight",			CmdSetBacklight }
#endif

//#ifdef SMARTPHONE
//	,	{ L"setbacklight",			CmdSmartphoneBackLight	}
//#endif
#ifdef SMARTPHONE
	,	{ L"setbtstate",			CmdSetBTState }
#endif
	,   { L"setchoiceentryformat",  CmdSetChoiceEntryFormat }
	,	{ L"setcliptext",			CmdSetClipText }
	,	{ L"setcominfo",			CmdSetComInfo }
	,	{ L"setfileattribs",		CmdSetFileAttribs }
	,	{ L"setfileattribute",		CmdSetFileAttribute }
#ifdef POCKETPC
	,	{ L"setinput",				CmdSetInput }
#endif
	,	{ L"setmessagefont",		CmdSetMessageFont }
	,	{ L"setproxy",				CmdSetProxy }
#ifdef SMARTPHONE
	,	{ L"setradiomode",			CmdSetRadioMode }
#endif
	,	{ L"settime",				CmdSetTime }
	,	{ L"setvolume",				CmdSetVolume }
	,	{ L"show",					CmdShow }
#ifdef POCKETPC
	,	{ L"showinput",				CmdShowInput }
#endif
	,	{ L"showwaitcursor",		CmdShowWaitCursor }
	,	{ L"sleep",					CmdSleep }
	,	{ L"sleepmessage",			CmdSleepMessage }
	,	{ L"split",					CmdSplit }
	,	{ L"statusclear",			CmdStatusClear }
	,	{ L"statushistorysize",		CmdStatusHistorySize }
	,	{ L"statusinfo",			CmdStatusInfo }
	,	{ L"statuslistentryformat",	CmdStatusListEntryFormat }
	,	{ L"statusmessage",			CmdStatusMessage }
	,	{ L"statusmessageappend",	CmdStatusMessageAppend }
	,	{ L"statusremovelastmessage",	CmdStatusRemoveLastMessage }
	,	{ L"statusshow",			CmdStatusShow }
	,	{ L"statustype",			CmdStatusType }
	,	{ L"substr",				CmdSubStr }
#ifdef CELE
	,	{ L"tgzx",					CmdTGZX }
#endif
#ifdef POCKETPC
	,	{ L"toggledisplay",			CmdToggleDisplay }
#endif

	,	{ L"unzipall",				CmdUnzipAll }
	,	{ L"unzipfile",				CmdUnzipFile }
	,	{ L"unzippath",				CmdUnzipPath }

	,	{ L"vibrate",				CmdVibrate }
	,	{ L"waitfor",				CmdWaitFor }
	,	{ L"waitforactive",			CmdWaitForActive }
	,	{ L"writefile",				CmdWriteFile }
	,	{ L"writestatushistory",	CmdWriteStatusHistory }
	,	{ L"xcopy",					CmdXCopy }

	,	{ L"zipfile",				CmdZipFile }
	,	{ L"zipfiles",				CmdZipFiles }

	,	{ NULL, NULL } // End
	};

short CommandsArraySize = 0;
