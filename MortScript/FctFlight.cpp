
#include <windows.h>
#include <tapi.h>
#include <extapi.h>
#include <aygshell.h>
#include <tsp.h>
#include "resource.h"

// some definitons
#define ARRAY_LENGTH(x)         (sizeof(x)/sizeof((x)[0]))
#define TAPI_API_LOW_VERSION    0x00020000
#define TAPI_API_HIGH_VERSION   0x00020000
#define EXT_API_LOW_VERSION     0x00010000
#define EXT_API_HIGH_VERSION    0x00010000
#define HUGE_BUFFER             4096



// ***************************************************************************
// Function Name: GetTSPLineDeviceID
//
// Purpose: To get a TSP Line Device ID
//
// Arguments:
//  hLineApp = the HLINEAPP returned by lineInitializeEx
//  dwNumberDevices = also returned by lineInitializeEx
//  dwAPIVersionLow/High = min version of TAPI that we need
//  psTSPLineName = "Cellular Line"
//
// Return Values: Current Device ID
//
// Description:
//  This function returns the device ID of a named TAPI TSP.  The Device ID is 
//  used in the call to lineOpen

DWORD GetTSPLineDeviceID(const HLINEAPP hLineApp, 
                         const DWORD dwNumberDevices, 
                         const DWORD dwAPIVersionLow, 
                         const DWORD dwAPIVersionHigh, 
                         const TCHAR* const psTSPLineName)
{
    DWORD dwReturn = 0xffffffff;
    for(DWORD dwCurrentDevID = 0 ; dwCurrentDevID < dwNumberDevices ; dwCurrentDevID++)
    {
        DWORD dwAPIVersion;
        LINEEXTENSIONID LineExtensionID;
        if(0 == lineNegotiateAPIVersion(hLineApp, dwCurrentDevID, 
                                        dwAPIVersionLow, dwAPIVersionHigh, 
                                        &dwAPIVersion, &LineExtensionID)) 
        {
            LINEDEVCAPS LineDevCaps;
            LineDevCaps.dwTotalSize = sizeof(LineDevCaps);
            if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                                   dwAPIVersion, 0, &LineDevCaps)) 
            {
                BYTE* pLineDevCapsBytes = new BYTE[LineDevCaps.dwNeededSize];
                if(0 != pLineDevCapsBytes) 
                {
                    LINEDEVCAPS* pLineDevCaps = (LINEDEVCAPS*)pLineDevCapsBytes;
                    pLineDevCaps->dwTotalSize = LineDevCaps.dwNeededSize;
                    if(0 == lineGetDevCaps(hLineApp, dwCurrentDevID, 
                                           dwAPIVersion, 0, pLineDevCaps)) 
                    {
                        if(0 == _tcscmp((TCHAR*)((BYTE*)pLineDevCaps+pLineDevCaps->dwLineNameOffset), 
                                        psTSPLineName)) 
                        {
                            dwReturn = dwCurrentDevID;
                        }
                    }
                    delete[]  pLineDevCapsBytes;
                }
            }
        }
    }
    return dwReturn;
}


// ***************************************************************************
// Function Name: GetExTAPIInfo
//
// Purpose: To get and display general ExTAPI information.
//
// Arguments: None
//
// Return Values: 
//  TRUE if all ExTAPI operations completed successfully, FALSE otherwise
//
// Description:
//  This function initializes ExTAPI and then calls lineGetGeneralInfo.
//  It then takes the info that lGGI returned and displays it in a MessageBox

DWORD Flight(int State)
{
    DWORD dwNumDevs;
    DWORD dwAPIVersion = TAPI_API_HIGH_VERSION;
    LINEINITIALIZEEXPARAMS liep;
    HLINEAPP hLineApp = 0;
    HLINE hLine = 0;

    DWORD bRetVal = 0;
    DWORD dwTAPILineDeviceID;
    const DWORD dwMediaMode = LINEMEDIAMODE_DATAMODEM | LINEMEDIAMODE_INTERACTIVEVOICE;

    // set the line init params
    liep.dwTotalSize = sizeof(liep);
    liep.dwOptions = LINEINITIALIZEEXOPTION_USEEVENT;
    
    if (lineInitializeEx(&hLineApp, 0, 0, NULL,/*gszFriendlyAppName,*/ 
                         &dwNumDevs, &dwAPIVersion, &liep)) 
    {
        goto cleanup;
    }

    // get the device ID
    dwTAPILineDeviceID = GetTSPLineDeviceID(hLineApp, dwNumDevs, 
                                                  TAPI_API_LOW_VERSION, 
                                                  TAPI_API_HIGH_VERSION, 
                                                  CELLTSP_LINENAME_STRING);

    // error getting the line device ID?
    if (0xffffffff == dwTAPILineDeviceID) 
    {
        goto cleanup;
    }

    // now try and open the line
    if(lineOpen(hLineApp, dwTAPILineDeviceID, 
                &hLine, dwAPIVersion, 0, 0, 
                LINECALLPRIVILEGE_OWNER, dwMediaMode, 0)) 
    {
        goto cleanup;
    }


	DWORD dwState,RadioSupport;
	lineGetEquipmentState(hLine,&dwState,&RadioSupport);
	if (dwState==LINEEQUIPSTATE_FULL && (State == 1 || State == 2)){
		lineSetEquipmentState(hLine,LINEEQUIPSTATE_MINIMUM);
	}else if (dwState!=LINEEQUIPSTATE_FULL && (State == 0 || State == 2)){
		lineSetEquipmentState(hLine,LINEEQUIPSTATE_FULL);
		lineRegister(hLine,LINEREGMODE_AUTOMATIC,NULL,NULL);
	}else if (State == 3)
		return dwState;


cleanup:
    if (hLine) lineClose(hLine);
    if (hLineApp) lineShutdown(hLineApp);

    return bRetVal;
}
