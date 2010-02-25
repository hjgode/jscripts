#include "Helpers.h"

#include "FunctionsData.h"
#include "FunctionsDialogs.h"
#include "FunctionsFileReg.h"
#include "FunctionsWindows.h"
#include "FunctionsSystem.h"

//------------------------------------------------------------
//  All functions
//------------------------------------------------------------
// Alphabetially sorted! Necessary to save memory (no use to have it unsorted in the programs
// code and sorted on the heap) and speed (sorting, binary search)
function FunctionsArray[] =
	{   { L"activeprocess",			FctActiveProcess }
	,   { L"activewindow",			FctActiveWindow }
	,   { L"actwnd",				FctActiveWindow }
	,   { L"arccos",				FctArcCos }
	,   { L"arcsin",				FctArcSin }
	,   { L"arctan",				FctArcTan }
	,   { L"array",					FctArray }
	,   { L"asc",					FctAsc }
	
	,   { L"backupbatterypercentage", FctBackupBatteryPercentage }
	,   { L"batterylevel",			FctBatteryPercentage }
	,   { L"batterypercentage",		FctBatteryPercentage }
	,   { L"blue",					FctBlue }
	,   { L"ceil",					FctCeil }
	,   { L"char",					FctUCChar }
	,   { L"charat",				FctCharAt }
	,   { L"choice",				FctChoice }
	,   { L"chr",					FctUCChar }
	,   { L"cliptext",				FctClipText }
	,   { L"colorat",				FctColorAt }
	,   { L"comparefloat",			FctCompareFloat }
#ifndef PNA
#ifndef DESKTOP
	,   { L"connected",				FctConnected }
#endif
#endif
	,   { L"cos",					FctCos }
	,   { L"currentcursor",			FctCurrentCursor }
	,   { L"dircontents",			FctDirContents }
	,   { L"direxists",				FctDirExists }
	,   { L"elementcount",			FctElementCount }
	,   { L"eval",					FctEval }
	,   { L"externalpowered",		FctExternalPowered }
	,   { L"fileattribute",			FctFileAttribute }
	,   { L"filebase",				FctFileBase }
	,   { L"filecreatetime",		FctFileCreateTime }
	,   { L"fileexists",			FctFileExists }
	,   { L"fileext",				FctFileExt }
	,   { L"filemodifytime",		FctFileModifyTime }
	,   { L"filepath",				FctFilePath }
	,   { L"filesize",				FctFileSize }
	,   { L"fileversion",			FctFileVersion }
	,   { L"find",					FctFind }
	,	{ L"floatstoreformat",		FctFloatStoreFormat }
	,   { L"floor",					FctFloor }
	,   { L"format",				FctFormat }
	,   { L"formattime",			FctFormatTime }
	,   { L"freediskspace",			FctFreeDiskSpace }
	,   { L"freememory",			FctFreeMemory }
#ifdef SMARTPHONE
	,	{ L"getbtstate",			FctGetBTState }
	,	{ L"getradiomode",			FctGetRadioMode }
#endif
	,	{ L"getwindowfromproc",		FctGetWindowFromProc }
	,   { L"green",					FctGreen }
	,   { L"hextonumber",			FctHexToNumber }
	,   { L"in",					FctIn }
	,   { L"iniread",			    FctIniRead }
	,   { L"input",					FctInput }
#ifndef PNA
	,   { L"internetconnected",		FctInternetConnected }
#endif
	,	{ L"isempty",				FctIsEmpty }
	,   { L"length",				FctLength }
	,   { L"log",					FctLog }
	,   { L"log10",					FctLog10 }
	,   { L"map",					FctMap }
	,   { L"max",				    FctMax }
	,   { L"maxindex",				FctMaxIndex }
	,   { L"min",				    FctMin }
	,   { L"mortscripttype",		FctMortScriptType }
	,   { L"mortscriptversion",		FctMortScriptVersion }
	,   { L"numbertohex",			FctNumberToHex }
	,   { L"part",					FctPart }
	,   { L"procexists",			FctProcExists }
	,   { L"question",				FctQuestion }
	,   { L"rand",					FctRnd }
	,   { L"random",				FctRnd }
	,   { L"readfile",			    FctReadFile }
	,   { L"readline",			    FctReadLine }
	,   { L"red",					FctRed }
	,	{ L"regex",					FctRegEx }
	,	{ L"regexreplace",			FctRegExReplace }
	,   { L"regkeyexists",			FctRegKeyExists }
	,   { L"regread",			    FctRegRead }
	,   { L"regvalueexists",		FctRegValueExists }
	,   { L"repeatstring",			FctRepeatString }
	,   { L"replace",				FctReplace }
	,   { L"reversefind",			FctReverseFind }
	,   { L"reversestr",			FctReverseStr }
	,   { L"rgb",					FctGetRGB }
	,   { L"rnd",					FctRnd }
	,   { L"round",					FctRound }
	,   { L"runwait",				FctRunWait }
	,   { L"screen",				FctScreen }
	,   { L"screenheight",			FctScreenHeight }
	,   { L"screentochars",			FctScreenToChars }
	,   { L"screenwidth",			FctScreenWidth }
	,   { L"scriptprocexists",		FctScriptProcExists }
	,   { L"selectdirectory",		FctSelectDirectory }
	,   { L"selectfile",			FctSelectFile }
	,   { L"sendmessage",			FctSendMessage }
	,   { L"sin",					FctSin }
	,   { L"split",					FctSplit }
	,   { L"sqrt",					FctSqrt }
	,   { L"substr",				FctSubStr }
	,   { L"supportsprochandling",  FctSupportsProcHandling }
	,   { L"systempath",			FctSystemPath }
	,   { L"systemversion",			FctSystemVersion }
	,   { L"tan",					FctTan }
	,   { L"timestamp",				FctTimeStamp }
	,   { L"timestamputc",			FctTimeStampUTC }
	,   { L"timezonebias",			FctTimeZoneBias }
	,   { L"timezonedst",			FctTimeZoneDST }
	,   { L"timezonename",			FctTimeZoneName }
	,   { L"tolower",				FctToLower }
	,   { L"totaldiskspace",		FctTotalDiskSpace }
	,   { L"totalmemory",			FctTotalMemory }
	,   { L"toupper",				FctToUpper }
	,   { L"ucchar",				FctUCChar }
	,   { L"ucvalue",				FctUCValue }
	,   { L"vartype",				FctVarType }
	,   { L"volume",				FctVolume }
	,   { L"windowactive",			FctWndActive }
	,   { L"windowexists",			FctWndExists }
	,   { L"windowprocess",			FctWindowProcess }
	,   { L"windowtext",			FctWindowText }
	,   { L"wndactive",				FctWndActive }
	,   { L"wndbottom",				FctWndBottom }
	,   { L"wndexists",				FctWndExists }
	,   { L"wndleft",				FctWndLeft }
	,   { L"wndright",				FctWndRight }
	,   { L"wndtop",				FctWndTop }
	,	{ NULL, NULL } // End
	};

short FunctionsArraySize = 0;