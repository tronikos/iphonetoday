#include "CReloadIcon.h"

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
	wcscpy(this->strName, L"");
	wcscpy(this->strImage, L"");
}