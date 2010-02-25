#include "stdafx.h"
#include "CConfigurationScreen.h"
#include "CConfiguracion.h"

CConfigurationScreen::CConfigurationScreen(void)
{
	this->defaultValues();
}

CConfigurationScreen::~CConfigurationScreen(void)
{
}

void CConfigurationScreen::calculate(BOOL isBottombar, int maxIcons, UINT screenWidth, UINT screenHeight)
{
	UINT w = screenWidth - offset.left - offset.right;

	iconWidth = iconWidthXML;
	if (iconsPerRowXML == 0) {
		if (iconWidth == 0) {
			iconsPerRow = maxIcons;
		} else {
			iconsPerRow = w / (iconWidth + minHorizontalSpace);
		}
	} else {
		iconsPerRow = iconsPerRowXML;
	}
	if (isBottombar) {
		iconsPerRow = min(iconsPerRow, UINT(maxIcons));
	}

	if (iconWidth * iconsPerRow <= w) {
		posReference.x = SHORT((w - iconWidth * iconsPerRow) / (iconsPerRow + 1));
		distanceIconsH = iconWidth + posReference.x;
		posReference.x += SHORT(offset.left);
	} else {
		posReference.x = 0;
		distanceIconsH = screenWidth / iconsPerRow;
	}
	posReference.y = SHORT(offset.top);
	distanceIconsV = UINT(iconWidth + fontSize + fontOffset + additionalVerticalSpace);
}

void CConfigurationScreen::defaultValues()
{
	this->iconWidthXML = 48;
	this->iconsPerRowXML = 0;
	this->minHorizontalSpace = 0;
	this->additionalVerticalSpace = 0;

	this->fontSize = 12;
	this->fontColor = RGB(255, 255, 255);
	this->fontBold = 0;
	this->fontOffset = 0;

	this->offset.left = 0;
	this->offset.top = 0;
	this->offset.right = 0;
	this->offset.bottom = 0;

	this->backColor1 = 0;
	this->backColor2 = 0;

	this->iconWidth = this->iconWidthXML;
	this->iconsPerRow = this->iconsPerRowXML;
}

BOOL CConfigurationScreen::loadXMLConfig(TiXmlElement *pRoot)
{
	if (pRoot == NULL) {
		return FALSE;
	}

	// for each child of root
	for (TiXmlElement *pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement()) {
		const char *nameNode = pElem->Value();

		if(_stricmp(nameNode, "IconWidth") == 0) {
			XMLUtils::GetTextElem(pElem, &this->iconWidthXML);
		} else if(_stricmp(nameNode, "IconsPerRow") == 0) {
			XMLUtils::GetTextElem(pElem, &this->iconsPerRowXML);
		} else if(_stricmp(nameNode, "MinHorizontalSpace") == 0) {
			XMLUtils::GetTextElem(pElem, &this->minHorizontalSpace);
		} else if(_stricmp(nameNode, "AdditionalVerticalSpace") == 0) {
			XMLUtils::GetTextElem(pElem, &this->additionalVerticalSpace);
		} else if(_stricmp(nameNode, "Offset") == 0) {
			XMLUtils::GetAttr(pElem, "left",   &this->offset.left);
			XMLUtils::GetAttr(pElem, "top",    &this->offset.top);
			XMLUtils::GetAttr(pElem, "right",  &this->offset.right);
			XMLUtils::GetAttr(pElem, "bottom", &this->offset.bottom);
		} else if(_stricmp(nameNode, "Font") == 0) {
			XMLUtils::GetAttr(pElem, "size",   &this->fontSize);
			XMLUtils::GetAttr(pElem, "color",  &this->fontColor);
			XMLUtils::GetAttr(pElem, "bold",   &this->fontBold);
			XMLUtils::GetAttr(pElem, "offset", &this->fontOffset);
		} else if(_stricmp(nameNode, "Background") == 0) {
			XMLUtils::GetAttr(pElem, "color1",  &this->backColor1);
			XMLUtils::GetAttr(pElem, "color2",  &this->backColor2);
		}
	}

	this->iconWidth = this->iconWidthXML;
	this->iconsPerRow = this->iconsPerRowXML;

	return TRUE;
}

BOOL CConfigurationScreen::saveXMLConfig(TiXmlElement *pRoot)
{
	TiXmlElement *pElem ;

	pElem = new TiXmlElement("IconWidth");
	XMLUtils::SetTextElem(pElem, this->iconWidthXML);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("IconsPerRow");
	XMLUtils::SetTextElem(pElem, this->iconsPerRowXML);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("MinHorizontalSpace");
	XMLUtils::SetTextElem(pElem, this->minHorizontalSpace);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("AdditionalVerticalSpace");
	XMLUtils::SetTextElem(pElem, this->additionalVerticalSpace);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Offset");
	XMLUtils::SetAttr(pElem, "left",   this->offset.left);
	XMLUtils::SetAttr(pElem, "top",    this->offset.top);
	XMLUtils::SetAttr(pElem, "right",  this->offset.right);
	XMLUtils::SetAttr(pElem, "bottom", this->offset.bottom);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Font");
	XMLUtils::SetAttr(pElem, "size",   this->fontSize);
	XMLUtils::SetAttr(pElem, "color",  this->fontColor);
	XMLUtils::SetAttr(pElem, "bold",   this->fontBold);
	XMLUtils::SetAttr(pElem, "offset", this->fontOffset);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Background");
	XMLUtils::SetAttr(pElem, "color1", this->backColor1);
	XMLUtils::SetAttr(pElem, "color2", this->backColor2);
	pRoot->LinkEndChild(pElem);

	return TRUE;
}
