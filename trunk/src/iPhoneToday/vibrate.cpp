#include <windows.h>
#include "vibrate.h"

#define NLED_COUNT_INFO_ID	0

struct NLED_COUNT_INFO
	{
	UINT	cLeds;				// @FIELD	Count of LED's
	};

#define NLED_SETTINGS_INFO_ID	2

struct NLED_SETTINGS_INFO
	{
	UINT	LedNum;					// @FIELD	LED number, 0 is first LED
	INT		OffOnBlink;				// @FIELD	0 == off, 1 == on, 2 == blink
	LONG	TotalCycleTime;			// @FIELD	total cycle time of a blink in microseconds
    LONG	OnTime;					// @FIELD	on time of a cycle in microseconds
    LONG	OffTime;				// @FIELD	off time of a cycle in microseconds
    INT		MetaCycleOn;			// @FIELD	number of on blink cycles
    INT		MetaCycleOff;			// @FIELD	number of off blink cycles
	};


typedef BOOL (STDAPICALLTYPE FAR fNLedGetDeviceInfo)(INT,VOID*); 
typedef BOOL (STDAPICALLTYPE FAR fNLedSetDevice)(INT,VOID*); 

static HMODULE hLib = NULL;
static fNLedGetDeviceInfo *pNLedGetDeviceInfo = NULL;
static fNLedSetDevice *pNLedSetDevice = NULL;

bool InitLed()
{
	if (hLib != NULL) return (pNLedGetDeviceInfo != NULL && pNLedSetDevice != NULL);
	hLib = LoadLibrary(L"coredll.dll");
	if (hLib == NULL) return false;

	pNLedGetDeviceInfo = (fNLedGetDeviceInfo*)GetProcAddress(hLib, L"NLedGetDeviceInfo");
	pNLedSetDevice = (fNLedSetDevice*)GetProcAddress(hLib, L"NLedSetDevice");

	return (pNLedGetDeviceInfo != NULL && pNLedSetDevice != NULL);
}

int GetLedCount()
{
	if (!InitLed()) return 0;
	NLED_COUNT_INFO nci;
	int wCount = 0;
	if(pNLedGetDeviceInfo(NLED_COUNT_INFO_ID, (PVOID) &nci))
		wCount = (int) nci.cLeds;
	return wCount;
} 

void SetLedStatus(int wLed, int wStatus)
{
	if (!InitLed()) return;
	NLED_SETTINGS_INFO nsi;
	nsi.LedNum = (INT) wLed;
	nsi.OffOnBlink = (INT) wStatus;
	pNLedSetDevice(NLED_SETTINGS_INFO_ID, &nsi);
}

void LedOn(int id)
{
	SetLedStatus(id, 1);
}

void LedOff(int id)
{
	SetLedStatus(id, 0);
}

void vibrate(int offMsec, int id)
{
	if (!InitLed()) return;
	LedOn(id);
	Sleep(offMsec);
	LedOff(id);
}

void vibrate(int offMsec)
{
	vibrate(offMsec, GetLedCount() - 1);
}
