#include "Utils.h"

/*
// ANSI to Unicode
BSTR ansiToUnicode(char *ansistr) {
	int lenA = strlen(ansistr);
	int lenW;
	BSTR unicodestr;

	lenW = ::MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, 0, 0);
	if (lenW > 0)
	{
		// Check whether conversion was successful
		unicodestr = ::SysAllocStringLen(0, lenW);
		::MultiByteToWideChar(CP_ACP, 0, ansistr, lenA, unicodestr, lenW);
	} else {
		// handle the error
	}

	return unicodestr;
}

// Unicode to ANSI
char* unicodeToAnsi (BSTR lpwszStrIn)
{
  LPSTR pszOut = NULL;
  if (lpwszStrIn != NULL)
  {
	int nInputStrLen = wcslen (lpwszStrIn);

	// Double NULL Termination
	int nOutputStrLen = WideCharToMultiByte (CP_UTF8, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
	pszOut = new char [nOutputStrLen];

	if (pszOut)
	{
	  memset (pszOut, 0x00, nOutputStrLen);
	  WideCharToMultiByte(CP_UTF8, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
	}
  }
  return pszOut;
}
*/

BOOL openFileBrowse(HWND hwnd, DWORD flags, TCHAR *strResult, TCHAR *strInitialDir) {
	TCHAR szFile[MAX_PATH];
	szFile[0] = '\0';

	OPENFILENAMEEX ofnex  = {0};
	ofnex.lStructSize     = sizeof(ofnex);
	ofnex.hwndOwner       = hwnd;
	ofnex.lpstrFile       = szFile;
	ofnex.nMaxFile        = sizeof(szFile)/sizeof(szFile[0]);
	ofnex.lpstrFilter     = TEXT("All Files (*.*)\0*.*\0");
	ofnex.lpstrTitle      = TEXT("Files");
	ofnex.ExFlags         = flags;
	ofnex.lpstrInitialDir = strInitialDir;

	// Display the Open dialog box. 
	if (GetOpenFileNameEx(&ofnex)==TRUE)
	{
		if (ofnex.lpstrFile[0] != _T('\0'))
		{
			// return ofnex.lpstrFile;
			swprintf(strResult, TEXT("%s"), ofnex.lpstrFile);
			return TRUE;
		}
	}
	return FALSE;
}

/*
TCHAR *getStringNotNull(TCHAR *strIn) {
	if (strIn) {
		return strIn;
	}
	return TEXT("");
}
*/

BOOL getPathFromFile(TCHAR *strIn, TCHAR *strOut) {
	// Extract Path for save lastPath
	TCHAR path[MAX_PATH];
	int len = min(_tcsclen(strIn), MAX_PATH);
	int i = 0;
	for (i = len - 1; i > 0; i--) {
		if(strIn[i] == '\\'){
			break;
		}
	}
	if (i > 0) {
		for (int j = 0; j <= i; j++) {
			path[j] = strIn[j];
		}
		path[i] = '\0';
		wcscpy(strOut, path);
		return TRUE;
	}
	return FALSE;
}

BOOL FileOrDirExists(TCHAR *file, BOOL dir)
{
	BOOL rc = 0;

	DWORD attribs = GetFileAttributes(file);
	if (attribs != -1) {
		if ( (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			if (dir) rc = 1;
		} else {
			if (!dir) rc = 1;
		}
	}

	return rc;
}

BOOL FileExists(TCHAR *file)
{
	return FileOrDirExists(file, FALSE);
}

FILETIME FileModifyTime(TCHAR *file)
{
	FILETIME filetime = {0};
	HANDLE hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		GetFileTime(hFile, NULL, NULL, &filetime);
		CloseHandle(hFile);
	}

	return filetime;
}

BOOL ExternalPowered()
{
	SYSTEM_POWER_STATUS_EX pwrStatus;
	if (GetSystemPowerStatusEx(&pwrStatus,TRUE)) {
		if (pwrStatus.ACLineStatus == 1
		|| pwrStatus.BatteryFlag == 8 // charging
		|| pwrStatus.BatteryFlag == 128 // no battery
		|| pwrStatus.BatteryLifePercent > 100) {
			return TRUE;
		}
	}
	return FALSE;
}

int BatteryPercentage()
{
	int b = -1;
	SYSTEM_POWER_STATUS_EX pwrStatus;
	if (GetSystemPowerStatusEx(&pwrStatus, TRUE)) {
		b = pwrStatus.BatteryLifePercent;
	}
	return b;
}

int MemoryLoad()
{
	MEMORYSTATUS mems;
	mems.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mems);
	return mems.dwMemoryLoad;
}

DWORD MemoryFree()
{
	MEMORYSTATUS mems;
	mems.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mems);
	return mems.dwAvailPhys;
}

DWORD MemoryUsed()
{
	MEMORYSTATUS mems;
	mems.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&mems);
	return mems.dwTotalPhys - mems.dwAvailPhys;
}

BOOL isPND()
{
	static int i = -1;
	if (i == -1) {
		TCHAR buf[MAX_PATH];
		SystemParametersInfo(SPI_GETPLATFORMTYPE, MAX_PATH, buf, 0);
		if (wcsicmp(buf, L"PocketPC") != 0 && wcsicmp(buf, L"SmartPhone") != 0) {
			i = 1;
		} else {
			i = 0;
		}
	}
	return (i == 1);
}

BOOL nativelySupportsAlphaBlend()
{
	static int i = -1;
	if (i == -1) {
		i = 0;
		HMODULE hCoredllLib = LoadLibrary(L"coredll.dll");
		if (hCoredllLib != NULL) {
			if (GetProcAddress(hCoredllLib, L"AlphaBlend") != NULL) {
				i = 1;
			}
			FreeLibrary(hCoredllLib);
		}
	}
	return (i == 1);
}
