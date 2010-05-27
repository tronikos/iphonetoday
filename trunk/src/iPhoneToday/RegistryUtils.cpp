#include "RegistryUtils.h"

BOOL SaveDwordSetting(HKEY hKEY, const TCHAR * lpSubKey, const DWORD *szValue,
					const TCHAR * szKeyName) {

	HKEY  hkey = 0;
	DWORD dwDisposition = 0;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = 0;

	// create (or open) the specified registry key
	LONG result = RegCreateKeyEx(hKEY, lpSubKey,
		0, NULL, 0, 0, NULL, &hkey, &dwDisposition);

	if (result != ERROR_SUCCESS)
		return FALSE;

	dwSize = sizeof(DWORD);
	result = RegSetValueEx(hkey, szKeyName, NULL, dwType, (PBYTE)szValue, dwSize);

	RegCloseKey(hkey);

	return (result == ERROR_SUCCESS);
}

BOOL LoadDwordSetting(HKEY hKEY, DWORD * szValue, const TCHAR * lpSubKey,
					const TCHAR * szKeyName, const DWORD dwDefault) {

	HKEY  hkey = 0;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	
	LONG result = RegOpenKeyEx(hKEY, lpSubKey, 0, 0, &hkey);

	// could not open key
	if (result != ERROR_SUCCESS) {
		*szValue = dwDefault;
		return FALSE;
	}

	// load the value
	result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
		(PBYTE)szValue, &dwSize);
	if (result != ERROR_SUCCESS) {
		*szValue = dwDefault;
	}

	RegCloseKey(hkey);

	return (result == ERROR_SUCCESS);
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
BOOL LoadTextSetting(HKEY hKEY, TCHAR * szValue, const TCHAR * lpSubKey,
					const TCHAR * szKeyName, const TCHAR * szDefault) {

	HKEY  hkey = 0;
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
		return FALSE;
	}

	// load the value
	ZeroMemory(szValue, dwSize);
	result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
		(LPBYTE)szValue, &dwSize);
	if (result != ERROR_SUCCESS) {
		if (szDefault) {
			wcscpy(szValue, szDefault);
		} else {
			wcscpy(szValue, L"");
		}
	}

	RegCloseKey(hkey);

	return (result == ERROR_SUCCESS);
}

BOOL LoadDateTimeSetting(HKEY hKEY, FILETIME * ftValue, const TCHAR * lpSubKey,
					const TCHAR * szKeyName) {

	HKEY  hkey = 0;
	DWORD dwType;
	DWORD dwSize = sizeof(FILETIME);

	LONG result = RegOpenKeyEx(hKEY, lpSubKey, 0, 0, &hkey);

	// could not open key
	if (result != ERROR_SUCCESS) {
		return FALSE;
	}

	// load the value
	ZeroMemory(ftValue, dwSize);
	result = RegQueryValueEx(hkey, szKeyName, NULL, &dwType,
		(LPBYTE)ftValue, &dwSize);

	RegCloseKey(hkey);

	return (result == ERROR_SUCCESS);
}

BOOL DeleteKey(HKEY hKey, const TCHAR * lpSubKey)
{
	LONG result = RegDeleteKey(hKey, lpSubKey);
	// could not open key
	if (result != ERROR_SUCCESS) {
		return FALSE;
	} else {
		return TRUE;
	}
}

BOOL RegValueExists(const HKEY root, const LPCTSTR path, const LPCTSTR value)
{
	HKEY hkey = 0;	
	LONG result = RegOpenKeyEx(root, path, 0, 0, &hkey);

	if (result != ERROR_SUCCESS) {
		return FALSE;
	}

	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof(DWORD);
	result = RegQueryValueEx(hkey, value, NULL, &dwType, NULL, &dwSize);
	RegCloseKey(hkey);

	return (result == ERROR_SUCCESS || result == ERROR_MORE_DATA);
}
