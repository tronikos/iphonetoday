#pragma once

#include "stdafx.h"

class CReloadIcon
{
public:
	DWORD nScreen;
	DWORD nIcon;
	TCHAR strName[MAX_PATH];
	TCHAR strImage[MAX_PATH];

	CReloadIcon(void);
	~CReloadIcon(void);

	void ClearObject();
};
