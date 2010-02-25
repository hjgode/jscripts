// Functions to handle files and registry entries

#include "morttypes.h"
#include "Helpers.h"

#ifndef FUNCTIONS_FILE_REG_H

CValue        FctFileExists           ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctDirExists            ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctMortScriptVersion    ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileVersion          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileAttribute        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileCreateTime       ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileModifyTime       ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileSize             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctDirContents          ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctSystemPath           ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFreeDiskSpace        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTotalDiskSpace       ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctReadFile             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctReadLine             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctIniRead              ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctRegRead              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRegKeyExists         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRegValueExists       ( CValueArray &params, int &error, CStr &errorMessage );

#endif