#include "Utils2.h"

WORD GetVolumePercentage()
{
	double ret;
	DWORD vol;
	waveOutGetVolume(0, &vol);

	WORD leftvol = LOWORD(vol);
	WORD rightvol = HIWORD(vol);

	if (rightvol == 0) {
		ret = leftvol;
	} else {
		ret = (leftvol + rightvol) / 2.0;
	}

	return WORD(ret * 100.0 / 0xFFFF + 0.5);
}

void SetVolumePercentage(WORD vol)
{
	DWORD v = DWORD(min(max(vol, 0), 100) * 0xFFFF / 100.0 + 0.5);
	waveOutSetVolume(0, v | v << 16);
}

void Rotate(int angle)
{
	DEVMODE mode;
	mode.dmSize = sizeof(DEVMODE);
	mode.dmFields = DM_DISPLAYORIENTATION;
	ChangeDisplaySettingsEx(NULL, &mode, 0, CDS_TEST, NULL);
	switch (angle) {
		case 90:
			if (mode.dmDisplayOrientation == DMDO_90) {
				mode.dmDisplayOrientation = DMDO_0;
			} else {
				mode.dmDisplayOrientation = DMDO_90;
			}
			break;
		case 270:
			if (mode.dmDisplayOrientation == DMDO_270) {
				mode.dmDisplayOrientation = DMDO_0;
			} else {
				mode.dmDisplayOrientation = DMDO_270;
			}
			break;
		case 180:
			if (mode.dmDisplayOrientation == DMDO_180) {
				mode.dmDisplayOrientation = DMDO_0;
			} else {
				mode.dmDisplayOrientation = DMDO_180;
			}
			break;
		default:
			mode.dmDisplayOrientation = DMDO_0;
			break;
	}
	ChangeDisplaySettingsEx(NULL, &mode, NULL, 0, NULL);
}

typedef BOOL (STDAPICALLTYPE FAR fSipGetInfo)(SIPINFO*);
typedef BOOL (STDAPICALLTYPE FAR fSipShowIM)(DWORD);

void ToggleKeyboard()
{
	SIPINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(SIPINFO);
	if (SipGetInfo(&si)) {
		SipShowIM((si.fdwFlags|SIPF_ON) == si.fdwFlags ? SIPF_OFF : SIPF_ON);
	} else {
		HMODULE hCoredllLib = LoadLibrary(L"coredl2.dll");
		if (hCoredllLib != NULL) {
			fSipGetInfo *pSipGetInfo = (fSipGetInfo*) GetProcAddress(hCoredllLib, L"SipGetInfo");
			fSipShowIM *pSipShowIM = (fSipShowIM*) GetProcAddress(hCoredllLib, L"SipShowIM");
			if (pSipGetInfo && pSipShowIM) {
				memset(&si, 0, sizeof(si));
				si.cbSize = sizeof(SIPINFO);
				if (pSipGetInfo(&si)) {
					pSipShowIM((si.fdwFlags|SIPF_ON) == si.fdwFlags ? SIPF_OFF : SIPF_ON);
				}
			}
			FreeLibrary(hCoredllLib);
		}
	}
}
