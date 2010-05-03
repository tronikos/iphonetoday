#pragma once

#include "stdafx.h"

class CReloadIcon
{
public:
	DWORD nScreen;
	DWORD nIcon;
	TCHAR strName[MAX_PATH];
	TCHAR strImage[MAX_PATH];
	TCHAR strExec[MAX_PATH];
	TCHAR strParameters[MAX_PATH];

	CReloadIcon(void);
	~CReloadIcon(void);

	void ClearObject();
	BOOL LoadRegistryIcon(LPCTSTR szSoftwareSubKey, int i);
	void DeleteRegistryIcons(LPCTSTR szSoftwareSubKey);
};
