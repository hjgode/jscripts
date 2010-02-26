// Functions to handle data values (strings, numbers, etc.)

#include "morttypes.h"
#include "helpers.h"

#ifndef FUNCTIONS_DATA_H

CValue        FctIsEmpty              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctVarType              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctEval                 ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctToUpper              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctToLower              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSubStr               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctLength               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctCharAt               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctUCValue              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctUCChar               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFind                 ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctReverseFind          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctReplace              ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRepeatString         ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctGet				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctGetNoCase			  ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctArray                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctMap                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSplit                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctPart                 ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctElementCount         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctMaxIndex             ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctFilePath             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileBase             ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFileExt              ( CValueArray &params, int &error, CStr &errorMessage );

CValue        FctFormat               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctNumberToHex          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctHexToNumber          ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctFloor                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRound                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctCeil                 ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRnd                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSin                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctCos                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctTan                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctArcSin               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctArcCos               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctArcTan               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctLog                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctLog10                ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctSqrt                 ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctCompareFloat         ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctMax                  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctMin                  ( CValueArray &params, int &error, CStr &errorMessage );

CValue		  FctFloatStoreFormat	  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctAsc				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctReverseStr			  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctGetRGB               ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctRed				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctGreen				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue        FctBlue				  ( CValueArray &params, int &error, CStr &errorMessage );
CValue		  FctIn					  ( CValueArray &params, int &error, CStr &errorMessage );
/*
#ifdef CELE
CValue		  FctRegx				  ( CValueArray &params, int &error, CStr &errorMessage );
#endif
*/
#endif