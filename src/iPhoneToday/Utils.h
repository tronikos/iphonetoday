#pragma once

#include "stdafx.h"

/*
// ANSI to Unicode
BSTR ansiToUnicode(char *ansistr);

// Unicode to ANSI
char *unicodeToAnsi(BSTR unicodestr);
*/

// Abrir ficheros
// LPWSTR openFileBrowse(HWND hwnd, DWORD flags = OFN_EXFLAG_DETAILSVIEW);
BOOL openFileBrowse(HWND hwnd, DWORD flags, TCHAR *strResult, TCHAR *strInitialDir);

//TCHAR *getStringNotNull(TCHAR *strIn);

BOOL getPathFromFile(TCHAR *strIn, TCHAR *strOut);

BOOL FileOrDirExists(TCHAR *file, BOOL dir);
BOOL FileExists(TCHAR *file);
FILETIME FileModifyTime(TCHAR *file);

//BOOL ExternalPowered();
//int BatteryPercentage();
//int MemoryLoad();
//DWORD MemoryFree();
//DWORD MemoryUsed();

BOOL isPND();
BOOL isPhone();
BOOL nativelySupportsAlphaBlend();

void AddRemoveFonts(TCHAR *location, BOOL add);

// Loads the file into memory only if its size is less than max_size.
// Allocates memory that has to be manually deallocated.
BYTE* LoadFileData(TCHAR *file, UINT max_size);

void PopulateOtherEmailAccounts(HWND combobox);
int GetUnreadOtherEmailAccount(TCHAR *account_name);