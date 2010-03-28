#include "CReloadIcon.h"
#include "RegistryUtils.h"

CReloadIcon::CReloadIcon(void)
{
	ClearObject();
}

CReloadIcon::~CReloadIcon(void)
{
}

void CReloadIcon::ClearObject(void)
{
	this->nScreen = 0;
	this->nIcon = -1;
	this->strName[0] = 0;
	this->strImage[0] = 0;
	this->strExec[0] = 0;
	this->strParameters[0] = 0;
}

BOOL CReloadIcon::LoadRegistryIcon(int i)
{
	TCHAR subKey[MAX_PATH];

	this->ClearObject();

	swprintf(subKey, L"Software\\iPhoneToday\\Icon%d", i);

	if (!LoadDwordSetting(HKEY_LOCAL_MACHINE, &this->nScreen, subKey, TEXT("nScreen"), 0))
		return FALSE;
	if (!LoadDwordSetting(HKEY_LOCAL_MACHINE, &this->nIcon, subKey, TEXT("nIcon"), 0))
		return FALSE;

	LoadTextSetting(HKEY_LOCAL_MACHINE, this->strName, subKey, TEXT("strName"), L"");
	LoadTextSetting(HKEY_LOCAL_MACHINE, this->strImage, subKey, TEXT("strImage"), L"");
	LoadTextSetting(HKEY_LOCAL_MACHINE, this->strExec, subKey, TEXT("strExec"), L"");
	LoadTextSetting(HKEY_LOCAL_MACHINE, this->strParameters, subKey, TEXT("strParameters"), L"");

	return TRUE;
}

void CReloadIcon::DeleteRegistryIcons()
{
	TCHAR subKey[MAX_PATH];
	int nIcon = 0;
	BOOL next = TRUE;
	while (next) {
		swprintf(subKey, L"Software\\iPhoneToday\\Icon%d", nIcon);
		next = DeleteKey(HKEY_LOCAL_MACHINE, subKey);
		nIcon++;
	}
}
