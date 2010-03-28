#include "CNotifications.h"

#include "RegistryUtils.h"

struct SN_RPV {
	HKEY hKey;
    LPCTSTR pszSubKey;
    LPCTSTR pszValueName;
};

// Order should match order of dw_notifications_enum
const SN_RPV SN_DW[] = {
	{
		SN_PHONEMISSEDCALLS_ROOT,
		SN_PHONEMISSEDCALLS_PATH,
		SN_PHONEMISSEDCALLS_VALUE
	},
	{
		SN_MESSAGINGSMSUNREAD_ROOT,
		SN_MESSAGINGSMSUNREAD_PATH,
		SN_MESSAGINGSMSUNREAD_VALUE
	},
	{
		SN_MESSAGINGMMSUNREAD_ROOT,
		SN_MESSAGINGMMSUNREAD_PATH,
		SN_MESSAGINGMMSUNREAD_VALUE
	},
	{
		SN_MESSAGINGOTHEREMAILUNREAD_ROOT,
		SN_MESSAGINGOTHEREMAILUNREAD_PATH,
		SN_MESSAGINGOTHEREMAILUNREAD_VALUE
	},
	{
		SN_MESSAGINGACTIVESYNCEMAILUNREAD_ROOT,
		SN_MESSAGINGACTIVESYNCEMAILUNREAD_PATH,
		SN_MESSAGINGACTIVESYNCEMAILUNREAD_VALUE
	},
	{
		HKEY_CURRENT_USER,
		TEXT("System\\State\\Appointments\\List"),
		TEXT("Count")
	},
	{
		SN_TASKSACTIVE_ROOT,
		SN_TASKSACTIVE_PATH,
		SN_TASKSACTIVE_VALUE
	},
	{
		SN_WIFISTATEPOWERON_ROOT,
		SN_WIFISTATEPOWERON_PATH,
		SN_WIFISTATEPOWERON_VALUE
	},
	{
		SN_BLUETOOTHSTATEPOWERON_ROOT,
		SN_BLUETOOTHSTATEPOWERON_PATH,
		SN_BLUETOOTHSTATEPOWERON_VALUE
	},
	{
		SN_CELLSYSTEMCONNECTED_ROOT,
		SN_CELLSYSTEMCONNECTED_PATH,
		SN_CELLSYSTEMCONNECTED_VALUE
	},
	{
		SN_PHONESIGNALSTRENGTH_ROOT,
		SN_PHONESIGNALSTRENGTH_PATH,
		SN_PHONESIGNALSTRENGTH_VALUE
	},
	{
		SN_POWERBATTERYSTATE_ROOT,
		SN_POWERBATTERYSTATE_PATH,
		SN_POWERBATTERYSTATE_VALUE
	},
	{
		HKEY_CURRENT_USER,
		TEXT("ControlPanel\\Volume"),
		TEXT("Volume")
	},
	{
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\Microsoft\\Clock\\0"),
		TEXT("AlarmFlags")
	},
	{
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\Microsoft\\Clock\\1"),
		TEXT("AlarmFlags")
	},
	{
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\Microsoft\\Clock\\2"),
		TEXT("AlarmFlags")
	},
	{
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\iPhoneToday"),
		TEXT("reloadIcon")
	},
	{
		HKEY_LOCAL_MACHINE,
		TEXT("Software\\iPhoneToday"),
		TEXT("reloadIcons")
	}
};

// Order should match order of sz_notifications_enum
const SN_RPV SN_SZ[] = {
	{
		SN_PHONEOPERATORNAME_ROOT,
		SN_PHONEOPERATORNAME_PATH,
		SN_PHONEOPERATORNAME_VALUE
	},
	{
		SN_PHONEPROFILE_ROOT,
		SN_PHONEPROFILE_PATH,
		SN_PHONEPROFILE_VALUE
	}
};

// Order should match order of ft_notifications_enum
const SN_RPV SN_FT[] = {
	{
		SN_TIME_ROOT,
		SN_TIME_PATH,
		SN_TIME_VALUE
	},
	{
		SN_DATE_ROOT,
		SN_DATE_PATH,
		SN_DATE_VALUE
	}
};


CNotifications::CNotifications(HWND hWnd)
{
	for (int i = 0; i < MAXDWORDNOTIFICATION; i++) {
		dwHrNotify[i] = NULL;
		dwNotifications[i] = 0;
		dwNotificationsChanged[i] = FALSE;
	}
	for (int i = 0; i < MAXSTRINGNOTIFICATION; i++) {
		szHrNotify[i] = NULL;
		szNotifications[i][0] = 0;
		szNotificationsChanged[i] = FALSE;
	}
	for (int i = 0; i < MAXFILETIMENOTIFICATION; i++) {
		ftHrNotify[i] = NULL;
		ftNotificationsChanged[i] = FALSE;
	}

	this->PollingUpdate();
	this->InitRegistryNotifications(hWnd);
}

CNotifications::~CNotifications()
{
	for (int i = 0; i < MAXDWORDNOTIFICATION; i++) {
		if (dwHrNotify[i]) {
			RegistryCloseNotification(dwHrNotify[i]);
			dwHrNotify[i] = NULL;
		}
	}
	for (int i = 0; i < MAXSTRINGNOTIFICATION; i++) {
		if (szHrNotify[i]) {
			RegistryCloseNotification(szHrNotify[i]);
			szHrNotify[i] = NULL;
		}
	}
	for (int i = 0; i < MAXFILETIMENOTIFICATION; i++) {
		if (ftHrNotify[i]) {
			RegistryCloseNotification(ftHrNotify[i]);
			ftHrNotify[i] = NULL;
		}
	}
}

void CNotifications::ResetChanged()
{
	for (int i = 0; i < MAXDWORDNOTIFICATION; i++) {
		dwNotificationsChanged[i] = FALSE;
	}
	for (int i = 0; i < MAXSTRINGNOTIFICATION; i++) {
		szNotificationsChanged[i] = FALSE;
	}
	for (int i = 0; i < MAXFILETIMENOTIFICATION; i++) {
		ftNotificationsChanged[i] = FALSE;
	}
}

void CNotifications::InitRegistryNotifications(HWND hWnd)
{
	HRESULT hr;

	for (int i = 0; i < MAXDWORDNOTIFICATION; i++) {
		hr = RegistryNotifyWindow(
			SN_DW[i].hKey, SN_DW[i].pszSubKey, SN_DW[i].pszValueName,
			hWnd, WM_REGISTRY, i,
			NULL, &dwHrNotify[i]);
	}
	for (int i = 0; i < MAXSTRINGNOTIFICATION; i++) {
		hr = RegistryNotifyWindow(
			SN_SZ[i].hKey, SN_SZ[i].pszSubKey, SN_SZ[i].pszValueName,
			hWnd, WM_REGISTRY, MAXDWORDNOTIFICATION + i,
			NULL, &szHrNotify[i]);
	}
	for (int i = 0; i < MAXFILETIMENOTIFICATION; i++) {
		hr = RegistryNotifyWindow(
			SN_FT[i].hKey, SN_FT[i].pszSubKey, SN_FT[i].pszValueName,
			hWnd, WM_REGISTRY, MAXDWORDNOTIFICATION + MAXSTRINGNOTIFICATION + i,
			NULL, &ftHrNotify[i]);
	}
}

// Process WM_REGISTY message for window
LRESULT CNotifications::Callback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	if (lParam < MAXDWORDNOTIFICATION) {
		i = lParam;
		if (lParam == SN_CLOCKALARMFLAGS0 || lParam == SN_CLOCKALARMFLAGS1 || lParam == SN_CLOCKALARMFLAGS2) {
			dwNotifications[i] = 0;
			LoadDwordSetting(SN_DW[i].hKey, &dwNotifications[i], SN_DW[i].pszSubKey, SN_DW[i].pszValueName, 0);
		} else {
			dwNotifications[i] = (DWORD) wParam;
		}
		dwNotificationsChanged[i] = TRUE;
	} else if (lParam - MAXDWORDNOTIFICATION < MAXSTRINGNOTIFICATION) {
		i = lParam - MAXDWORDNOTIFICATION;
		LoadTextSetting(SN_SZ[i].hKey, szNotifications[i], SN_SZ[i].pszSubKey, SN_SZ[i].pszValueName, L"");
		szNotificationsChanged[i] = TRUE;
	} else if (lParam - MAXDWORDNOTIFICATION - MAXSTRINGNOTIFICATION < MAXFILETIMENOTIFICATION) {
		i = lParam - MAXDWORDNOTIFICATION - MAXSTRINGNOTIFICATION;
		GetLocalTime(&st);
		ftNotificationsChanged[i] = TRUE;
	}
	return 0;
}

// Update all notifications except those that RegistryNotifyWindow succeeded.
// Return TRUE if at least one has changed.
BOOL CNotifications::PollingUpdate()
{
	BOOL changed = FALSE;

	for (int i = 0; i < MAXDWORDNOTIFICATION; i++) {
		if (!dwHrNotify[i]) {
			DWORD value = 0;
			if (i == SN_POWERBATTERYSTATE) {
				SYSTEM_POWER_STATUS_EX pwrStatus;
				if (GetSystemPowerStatusEx(&pwrStatus, TRUE)) {
					value = pwrStatus.BatteryLifePercent << 16;
					value |= pwrStatus.BatteryFlag;
				} else {
					value = BATTERY_PERCENTAGE_UNKNOWN << 16;
				}
			} else if (i == SN_VOLUME) {
				waveOutGetVolume(0, &value);
			} else {
				LoadDwordSetting(SN_DW[i].hKey, &value, SN_DW[i].pszSubKey, SN_DW[i].pszValueName, 0);
			}
			if (dwNotifications[i] != value) {
				dwNotifications[i] = value;
				dwNotificationsChanged[i] = TRUE;
				changed = TRUE;
			} else {
				dwNotificationsChanged[i] = FALSE;
			}
		}
	}

	for (int i = 0; i < MAXSTRINGNOTIFICATION; i++) {
		TCHAR value[MAX_PATH];
		if (!szHrNotify[i]) {
			LoadTextSetting(SN_SZ[i].hKey, value, SN_SZ[i].pszSubKey, SN_SZ[i].pszValueName, L"");
			if (lstrcmp(szNotifications[i], value) != 0) {
				wcscpy(szNotifications[i], value);
				szNotificationsChanged[i] = TRUE;
				changed = TRUE;
			} else {
				szNotificationsChanged[i] = FALSE;
			}
		}
	}

	if (!ftHrNotify[SN_TIME] || !ftHrNotify[SN_DATE]) {
		SYSTEMTIME st_new;
		GetLocalTime(&st_new);
		if (st.wMinute != st_new.wMinute || st.wHour != st_new.wHour) {
			ftNotificationsChanged[SN_TIME] = TRUE;
			changed = TRUE;
		} else {
			ftNotificationsChanged[SN_TIME] = FALSE;
		}
		if (st.wDay != st_new.wDay || st.wDayOfWeek != st_new.wDayOfWeek) {
			ftNotificationsChanged[SN_DATE] = TRUE;
			changed = TRUE;
		} else {
			ftNotificationsChanged[SN_DATE] = FALSE;
		}
		memcpy(&st, &st_new, sizeof(SYSTEMTIME));
	}

	MEMORYSTATUS mems;
	mems.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mems);
	if (mems.dwMemoryLoad != memoryStatus.dwMemoryLoad) {
		memory_changed = TRUE;
		changed = TRUE;
	} else {
		memory_changed = FALSE;
	}
	memcpy(&memoryStatus, &mems, sizeof(SYSTEMTIME));

	return changed;
}
