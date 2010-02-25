#include "stdafx.h"
#include "vibrate.h"
#include <nled.h>

extern "C" {
	BOOL NLedGetDeviceInfo(INT nID, PVOID pOutput);
	BOOL NLedSetDevice(INT nID, PVOID pOutput);
}


int GetLedCount()
{
	NLED_COUNT_INFO nci;
	int wCount = 0;
	if(NLedGetDeviceInfo(NLED_COUNT_INFO_ID, (PVOID) &nci))
		wCount = (int) nci.cLeds;
	return wCount;
} 

void SetLedStatus(int wLed, int wStatus)
{
	NLED_SETTINGS_INFO nsi;
	nsi.LedNum = (INT) wLed;
	nsi.OffOnBlink = (INT) wStatus;
	NLedSetDevice(NLED_SETTINGS_INFO_ID, &nsi);
}

void Vibrate()
{
	SetLedStatus( GetLedCount()-1, 1 );
}

void VibrateStop()
{
	SetLedStatus( GetLedCount()-1, 0 );
}