#pragma once

#include "stdafx.h"

/*
HKEY hKey: Carpeta a buscar
DWORD * szValue: Valor leido
const TCHAR * lpSubKey:	Ruta en el registro (El directorio)
const TCHAR * szKeyName: El registro en concreto (La entrada)
const DWORD * szDefault = NULL: Valor en caso de no existir?
*/
void SaveDwordSetting(HKEY hKEY, const TCHAR * lpSubKey, const DWORD *szValue,
                 const TCHAR * szKeyName);
void LoadDwordSetting(HKEY hKey, DWORD * szValue, const TCHAR * lpSubKey,
                 const TCHAR * szKeyName, const DWORD * szDefault);
void LoadTextSetting(HKEY hKEY, TCHAR * szValue, const TCHAR * lpSubKey,
                 const TCHAR * szKeyName, const TCHAR * szDefault);
BOOL DeleteKey(HKEY hKey, const TCHAR * lpSubKey);
