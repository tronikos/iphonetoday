
#include "RegistryUtils.h"

void SaveDwordSetting(HKEY hKEY, const TCHAR * lpSubKey, const DWORD *szValue,
                 const TCHAR * szKeyName) {

    HKEY  hkey = 0;
    DWORD dwDisposition = 0;
    DWORD dwType = REG_DWORD;
    DWORD dwSize = 0;

    // create (or open) the specified registry key
    LONG result = RegCreateKeyEx(hKEY, lpSubKey, 
        0, NULL, 0, 0, NULL, &hkey, &dwDisposition);

    if (result != ERROR_SUCCESS)
        return;

    dwSize = sizeof(DWORD);
    result = RegSetValueEx(hkey, szKeyName, NULL, dwType, (PBYTE)szValue, dwSize);

    if (hkey != NULL)
        RegCloseKey(hkey);
}

void LoadDwordSetting(HKEY hKEY, DWORD * szValue, const TCHAR * lpSubKey,
                 const TCHAR * szKeyName, const DWORD * szDefault) {

    HKEY  hkey = 0;
    DWORD dwDisposition = 0;
	DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);
	
	if (szDefault) {
		*szValue = *szDefault;
	} else {
		*szValue = 0;
	}

    LONG result = RegOpenKeyEx(hKEY, lpSubKey, 0, 0, &hkey);

    // could not open key
    if (result != ERROR_SUCCESS) {
		return;
    }

    // load the value
    result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
        (PBYTE)szValue, &dwSize);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hkey);
		if (szDefault) {
			*szValue = *szDefault;
		} else {
			*szValue = 0;
		}
        return;
    }

    RegCloseKey(hkey);
}

/*
void LoadDwordSetting(HKEY hKEY, DWORD * szValue, const TCHAR * lpSubKey,
                 const TCHAR * szKeyName, const DWORD * szDefault) {

    HKEY  hkey = 0;
    DWORD dwDisposition = 0;
	DWORD dwType = REG_DWORD;
    DWORD dwSize = sizeof(DWORD);
	
	if (szDefault) {
		*szValue = *szDefault;
	} else {
		*szValue = 0;
	}

    LONG result = RegOpenKeyEx(hKEY, lpSubKey, 0, 0, &hkey);

    // could not open key
    if (result != ERROR_SUCCESS) {

        // no default value exists
        if (!szDefault)
            return;

        // try to create the key instead
        result = RegCreateKeyEx(hKEY, lpSubKey, 
            0, NULL, 0, 0, NULL, &hkey, &dwDisposition);

        // also couldn't create key, just use default
        if (result != ERROR_SUCCESS) {
            // StringCchCopy(szValue, cchValue, szDefault);
			*szValue = *szDefault;
            return;
        }
    
        // opened key (also creating it)
        if (dwDisposition == REG_CREATED_NEW_KEY) {
            RegCloseKey(hkey);
            SaveDwordSetting(hKEY, lpSubKey, szDefault, szKeyName);
            // StringCchCopy(szValue, cchValue, szDefault);
			*szValue = *szDefault;
            return;
        }

    }

    // load the value
    result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
        (PBYTE)szValue, &dwSize);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hkey);
        if (szDefault) {
            SaveDwordSetting(hKEY, lpSubKey, szDefault, szKeyName);
            // StringCchCopy(szValue, cchValue, szDefault);
			*szValue = *szDefault;
        }
        return;
    }

    RegCloseKey(hkey);
}
*/
void LoadTextSetting(HKEY hKEY, TCHAR * szValue, const TCHAR * lpSubKey,
                 const TCHAR * szKeyName, const TCHAR * szDefault) {

    HKEY  hkey = 0;
    DWORD dwDisposition = 0;
	DWORD dwType;
    DWORD dwSize = MAX_PATH;
	
	if (szDefault) {
		wcscpy(szValue, szDefault);
	} else {
		wcscpy(szValue, L"");
	}

    LONG result = RegOpenKeyEx(hKEY, lpSubKey, 0, 0, &hkey);

    // could not open key
    if (result != ERROR_SUCCESS) {
		return;
    }

    // load the value
	ZeroMemory(szValue, dwSize);
    result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
        (LPBYTE)szValue, &dwSize);
    if (result != ERROR_SUCCESS) {
        RegCloseKey(hkey);
		if (szDefault) {
			wcscpy(szValue, szDefault);
		} else {
			wcscpy(szValue, L"");
		}
        return;
    }

    RegCloseKey(hkey);
}

BOOL DeleteKey(HKEY hKey, const TCHAR * lpSubKey)
{
	LONG result = RegDeleteKey(hKey, lpSubKey);
    // could not open key
    if (result != ERROR_SUCCESS) {
		return false;
	} else {
		return true;
	}
}