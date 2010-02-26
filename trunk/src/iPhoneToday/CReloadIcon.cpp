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
	this->strName[0] = 0;
	this->strImage[0] = 0;
}
