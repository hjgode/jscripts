// Functions regarding windows, controls, screen infos, ...

#include "morttypes.h"
#include "Helpers.h"

#ifndef FUNCTIONS_WINDOWS_H

CValue        FctWndExists            ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctActiveWindow         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWndActive            ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWindowText           ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWndLeft              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWndRight             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWndTop               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctWndBottom            ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctColorAt              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctScreenToChars        ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctScreen               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSendMessage          ( CValueArray &params, int &error, CStr &errorMessage );
  
#endif