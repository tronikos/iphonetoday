#include "Utils2.h"
#include "RegistryUtils.h"
#include <Pm.h>

WORD ConvertVolumeToPercentage(DWORD vol)
{
	double ret;
	WORD leftvol = LOWORD(vol);
	WORD rightvol = HIWORD(vol);

	if (rightvol == 0) {
		ret = leftvol;
	} else {
		ret = (leftvol + rightvol) / 2.0;
	}

	return WORD(ret * 100.0 / 0xFFFF + 0.5);
}

WORD GetVolumePercentage()
{
	DWORD vol;
	waveOutGetVolume(0, &vol);
	return ConvertVolumeToPercentage(vol);
}

void SetVolumePercentage(WORD vol)
{
	DWORD v = DWORD(min(max(vol, 0), 100) * 0xFFFF / 100.0 + 0.5);
	v = v | v << 16;
	waveOutSetVolume(0, v);
	SaveDwordSetting(HKEY_CURRENT_USER, TEXT("ControlPanel\\Volume"), &v, TEXT("Volume"));
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

void ToggleKeyboard(int bShow)
{
	SIPINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(SIPINFO);
	if (SipGetInfo(&si)) {
		if (bShow == -1) {
			SipShowIM((si.fdwFlags|SIPF_ON) == si.fdwFlags ? SIPF_OFF : SIPF_ON);
		} else {
			SipShowIM(bShow ? SIPF_ON : SIPF_OFF);
		}
	} else {
		HMODULE hCoredllLib = LoadLibrary(L"coredl2.dll");
		if (hCoredllLib != NULL) {
			fSipGetInfo *pSipGetInfo = (fSipGetInfo*) GetProcAddress(hCoredllLib, L"SipGetInfo");
			fSipShowIM *pSipShowIM = (fSipShowIM*) GetProcAddress(hCoredllLib, L"SipShowIM");
			if (pSipGetInfo && pSipShowIM) {
				memset(&si, 0, sizeof(si));
				si.cbSize = sizeof(SIPINFO);
				if (pSipGetInfo(&si)) {
					if (bShow == -1) {
						pSipShowIM((si.fdwFlags|SIPF_ON) == si.fdwFlags ? SIPF_OFF : SIPF_ON);
					} else {
						pSipShowIM(bShow ? SIPF_ON : SIPF_OFF);
					}
				}
			}
			FreeLibrary(hCoredllLib);
		}
	}
}

typedef int (STDAPICALLTYPE FAR fBthGetMode)(DWORD*);
typedef int (STDAPICALLTYPE FAR fBthSetMode)(DWORD);

enum BTH_RADIO_MODE
{
    BTH_POWER_OFF,
    BTH_CONNECTABLE,
    BTH_DISCOVERABLE
};

void ToggleBluetooth(int bEnable)
{
	HMODULE hLib = LoadLibrary(L"bthutil.dll");
	if (hLib != NULL) {
		fBthGetMode *pBthGetMode = (fBthGetMode*) GetProcAddress(hLib, L"BthGetMode");
		fBthSetMode *pBthSetMode = (fBthSetMode*) GetProcAddress(hLib, L"BthSetMode");
		if (pBthGetMode && pBthSetMode) {
			DWORD mode = 0;
			if (pBthGetMode(&mode) == ERROR_SUCCESS) {
				if (bEnable == -1) {
					pBthSetMode((mode == BTH_POWER_OFF) ? BTH_CONNECTABLE : BTH_POWER_OFF);
				} else {
					pBthSetMode(bEnable ? BTH_CONNECTABLE : BTH_POWER_OFF);
				}
			}
		}
		FreeLibrary(hLib);
	}
}

void ToggleWLAN(int bEnable)
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\Power\\State", 0, KEY_READ, &hKey) == ERROR_SUCCESS ) {
		BOOL found = FALSE;
		TCHAR achValue[MAX_PATH];
		DWORD cchValue = MAX_PATH;
		int i = 0;
		while (RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
			if (wcsncmp(achValue, L"{98C5250D-C29A-4985-AE5F-AFE5367E5006}", 38) == 0) {
				found = TRUE;
				break;
			}
			cchValue = MAX_PATH;
			i++;
		}
		RegCloseKey(hKey);
		if (found) {
			CEDEVICE_POWER_STATE state;
			if (GetDevicePower(achValue, POWER_NAME, &state) == ERROR_SUCCESS) {
				if (bEnable == -1) {
					SetDevicePower(achValue, POWER_NAME, state != D4 ? D4 : D1);
				} else {
					SetDevicePower(achValue, POWER_NAME, bEnable ? D1 : D4);
				}
			}
		}
	}
}
