// Functions that show a dialog

#include "morttypes.h"
#include "Helpers.h"

#ifndef FUNCTIONS_DIALOGS_H

CValue        FctQuestion             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctChoice               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctInput                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSelectDirectory      ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSelectFile           ( CValueArray &params, int &error, CStr &errorMessage );

#endif