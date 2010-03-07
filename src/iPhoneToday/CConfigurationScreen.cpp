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

void CConfigurationScreen::calculate(BOOL isStaticbar, int maxIcons, UINT screenWidth, UINT screenHeight)
{
	UINT w = screenWidth - cs.offset.left - cs.offset.right;

	iconWidth = cs.iconWidthXML;
	if (cs.iconsPerRowXML == 0) {
		if (iconWidth == 0) {
			iconsPerRow = maxIcons;
		} else {
			iconsPerRow = w / (iconWidth + cs.minHorizontalSpace);
		}
	} else {
		iconsPerRow = cs.iconsPerRowXML;
	}
	if (isStaticbar) {
		iconsPerRow = min(iconsPerRow, UINT(maxIcons));
	}

	if (iconWidth * iconsPerRow <= w) {
		posReference.x = SHORT((w - iconWidth * iconsPerRow) / (iconsPerRow + 1));
		distanceIconsH = iconWidth + posReference.x;
		posReference.x += SHORT(cs.offset.left);
	} else {
		posReference.x = 0;
		distanceIconsH = screenWidth / iconsPerRow;
	}
	posReference.y = SHORT(cs.offset.top);
	distanceIconsV = UINT(iconWidth + cs.fontSize + cs.fontOffset + cs.additionalVerticalSpace);
}

void CConfigurationScreen::defaultValues()
{
	this->cs.iconWidthXML = 48;
	this->cs.iconsPerRowXML = 0;
	this->cs.minHorizontalSpace = 0;
	this->cs.additionalVerticalSpace = 0;

	this->cs.fontSize = 12;
	this->cs.fontColor = RGB(255, 255, 255);
	this->cs.fontBold = 0;
	this->cs.fontOffset = 0;
	this->cs.fontShadow = 0;

	this->cs.offset.left = 0;
	this->cs.offset.top = 0;
	this->cs.offset.right = 0;
	this->cs.offset.bottom = 0;

	this->cs.backColor1 = 0;
	this->cs.backColor2 = 0;

	this->iconWidth = this->cs.iconWidthXML;
	this->iconsPerRow = this->cs.iconsPerRowXML;
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
			XMLUtils::GetTextElem(pElem, &this->cs.iconWidthXML);
		} else if(_stricmp(nameNode, "IconsPerRow") == 0) {
			XMLUtils::GetTextElem(pElem, &this->cs.iconsPerRowXML);
		} else if(_stricmp(nameNode, "MinHorizontalSpace") == 0) {
			XMLUtils::GetTextElem(pElem, &this->cs.minHorizontalSpace);
		} else if(_stricmp(nameNode, "AdditionalVerticalSpace") == 0) {
			XMLUtils::GetTextElem(pElem, &this->cs.additionalVerticalSpace);
		} else if(_stricmp(nameNode, "Offset") == 0) {
			XMLUtils::GetAttr(pElem, "left",   &this->cs.offset.left);
			XMLUtils::GetAttr(pElem, "top",    &this->cs.offset.top);
			XMLUtils::GetAttr(pElem, "right",  &this->cs.offset.right);
			XMLUtils::GetAttr(pElem, "bottom", &this->cs.offset.bottom);
		} else if(_stricmp(nameNode, "Font") == 0) {
			XMLUtils::GetAttr(pElem, "size",   &this->cs.fontSize);
			XMLUtils::GetAttr(pElem, "color",  &this->cs.fontColor);
			XMLUtils::GetAttr(pElem, "bold",   &this->cs.fontBold);
			XMLUtils::GetAttr(pElem, "offset", &this->cs.fontOffset);
			XMLUtils::GetAttr(pElem, "shadow", &this->cs.fontShadow);
		} else if(_stricmp(nameNode, "Background") == 0) {
			XMLUtils::GetAttr(pElem, "color1",  &this->cs.backColor1);
			XMLUtils::GetAttr(pElem, "color2",  &this->cs.backColor2);
		}
	}

	this->iconWidth = this->cs.iconWidthXML;
	this->iconsPerRow = this->cs.iconsPerRowXML;

	return TRUE;
}

BOOL CConfigurationScreen::saveXMLConfig(TiXmlElement *pRoot)
{
	TiXmlElement *pElem ;

	pElem = new TiXmlElement("IconWidth");
	XMLUtils::SetTextElem(pElem, this->cs.iconWidthXML);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("IconsPerRow");
	XMLUtils::SetTextElem(pElem, this->cs.iconsPerRowXML);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("MinHorizontalSpace");
	XMLUtils::SetTextElem(pElem, this->cs.minHorizontalSpace);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("AdditionalVerticalSpace");
	XMLUtils::SetTextElem(pElem, this->cs.additionalVerticalSpace);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Offset");
	XMLUtils::SetAttr(pElem, "left",   this->cs.offset.left);
	XMLUtils::SetAttr(pElem, "top",    this->cs.offset.top);
	XMLUtils::SetAttr(pElem, "right",  this->cs.offset.right);
	XMLUtils::SetAttr(pElem, "bottom", this->cs.offset.bottom);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Font");
	XMLUtils::SetAttr(pElem, "size",   this->cs.fontSize);
	XMLUtils::SetAttr(pElem, "color",  this->cs.fontColor);
	XMLUtils::SetAttr(pElem, "bold",   this->cs.fontBold);
	XMLUtils::SetAttr(pElem, "offset", this->cs.fontOffset);
	XMLUtils::SetAttr(pElem, "shadow", this->cs.fontShadow);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Background");
	XMLUtils::SetAttr(pElem, "color1", this->cs.backColor1);
	XMLUtils::SetAttr(pElem, "color2", this->cs.backColor2);
	pRoot->LinkEndChild(pElem);

	return TRUE;
}
