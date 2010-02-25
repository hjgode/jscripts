// Functions for system informations

#include "morttypes.h"
#include "helpers.h"

#ifndef FUNCTIONS_DATA_H

CValue        FctExternalPowered      ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctBackupBatteryPercentage    ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctBatteryPercentage    ( CValueArray &params, int &error, CStr &errorMessage );
  
CValue        FctSupportsProcHandling ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctActiveProcess        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWindowProcess        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctProcExists           ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctGetWindowFromProc	  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctScriptProcExists     ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctClipText             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctVolume               ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctTimeStamp            ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTimeStampUTC         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTimeZoneBias         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTimeZoneName         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTimeZoneDST          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFormatTime           ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSystemVersion        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctMortScriptType       ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctScreenWidth          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctScreenHeight         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctCurrentCursor        ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctConnected            ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctInternetConnected    ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFreeMemory           ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTotalMemory          ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctRunWait              ( CValueArray &params, int &error, CStr &errorMessage );

CValue		  FctRegEx				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctRegExReplace		  ( CValueArray &params, int &error, CStr &errorMessage );

#ifdef SMARTPHONE
CValue		  FctGetBTState			  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctGetRadioMode		  ( CValueArray &params, int &error, CStr &errorMessage );
#endif

#endif