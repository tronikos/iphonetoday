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
	textHeight = cs.textHeightXML;
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
		if (cs.shrinkToFit && maxIcons != 0) {
			iconsPerRow = maxIcons;
			iconWidth = min(cs.iconWidthXML, (w - maxIcons * cs.minHorizontalSpace) / iconsPerRow);
			if (cs.iconWidthXML > 0) {
				textHeight = UINT(0.5 + 1.0 * cs.textHeightXML * iconWidth / cs.iconWidthXML);
			}
		} else {
			iconsPerRow = min(iconsPerRow, UINT(maxIcons));
		}
	}

	if (iconWidth * iconsPerRow <= w) {
		posReference.x = SHORT((w - iconWidth * iconsPerRow) / (iconsPerRow + 1));
		distanceIconsH = iconWidth + posReference.x;
		posReference.x += SHORT(((w - iconWidth * iconsPerRow) % (iconsPerRow + 1)) / 2) + SHORT(cs.offset.left);
	} else {
		posReference.x = 0;
		distanceIconsH = screenWidth / iconsPerRow;
	}
	posReference.y = SHORT(cs.offset.top);
	distanceIconsV = UINT(iconWidth + textHeight + cs.textOffsetVertical + cs.additionalVerticalSpace);
}

void CConfigurationScreen::defaultValues()
{
	this->cs.iconWidthXML = 48;
	this->cs.iconsPerRowXML = 0;
	this->cs.minHorizontalSpace = 0;
	this->cs.additionalVerticalSpace = 0;
	this->cs.shrinkToFit = 0;

	this->cs.textFacename[0] = 0;
	this->cs.textHeightXML = 11;
	this->cs.textColor = RGB(255, 255, 255);
	this->cs.textWeight = 400;
	this->cs.textOffsetVertical = 0;
	this->cs.textOffsetHorizontal = 0;
	this->cs.textAlign = DT_CENTER;
	this->cs.textShadow = 1;
	this->cs.textRoundRect = 0;

	this->cs.offset.left = 0;
	this->cs.offset.top = 0;
	this->cs.offset.right = 0;
	this->cs.offset.bottom = 0;

	this->cs.backGradient = 0;
	this->cs.backColor1 = 0;
	this->cs.backColor2 = 0;

	this->cs.backWallpaper[0] = 0;
	this->cs.backWallpaperAlphaBlend = 0;
	this->cs.backWallpaperFitWidth = 1;
	this->cs.backWallpaperFitHeight = 1;
	this->cs.backWallpaperCenter = 1;
	this->cs.backWallpaperTile = 0;

	this->iconWidth = this->cs.iconWidthXML;
	this->iconsPerRow = this->cs.iconsPerRowXML;
	this->textHeight = this->cs.textHeightXML;
}

void CConfigurationScreen::Scale(double scale)
{
	this->cs.iconWidthXML = UINT(scale * this->cs.iconWidthXML);
	this->cs.minHorizontalSpace = UINT(scale * this->cs.minHorizontalSpace);
	this->cs.additionalVerticalSpace = UINT(scale * this->cs.additionalVerticalSpace);

	this->cs.textHeightXML = UINT(scale * this->cs.textHeightXML);
	this->cs.textOffsetVertical = INT(scale * this->cs.textOffsetVertical);
	this->cs.textOffsetHorizontal = INT(scale * this->cs.textOffsetHorizontal);

	this->cs.offset.left = LONG(scale * this->cs.offset.left);
	this->cs.offset.top = LONG(scale * this->cs.offset.top);
	this->cs.offset.right = LONG(scale * this->cs.offset.right);
	this->cs.offset.bottom = LONG(scale * this->cs.offset.bottom);

	this->iconWidth = this->cs.iconWidthXML;
	this->iconsPerRow = this->cs.iconsPerRowXML;
	this->textHeight = this->cs.textHeightXML;
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
		} else if(_stricmp(nameNode, "ShrinkToFit") == 0) {
			XMLUtils::GetTextElem(pElem, &this->cs.shrinkToFit);
		} else if(_stricmp(nameNode, "Offset") == 0) {
			XMLUtils::GetAttr(pElem, "left",   &this->cs.offset.left);
			XMLUtils::GetAttr(pElem, "top",    &this->cs.offset.top);
			XMLUtils::GetAttr(pElem, "right",  &this->cs.offset.right);
			XMLUtils::GetAttr(pElem, "bottom", &this->cs.offset.bottom);
		} else if(_stricmp(nameNode, "Font") == 0) {
			XMLUtils::GetAttr(pElem, "facename",  this->cs.textFacename, CountOf(this->cs.textFacename));
			XMLUtils::GetAttr(pElem, "size",      &this->cs.textHeightXML);
			XMLUtils::GetAttr(pElem, "color",     &this->cs.textColor);
			XMLUtils::GetAttr(pElem, "weight",    &this->cs.textWeight);
			XMLUtils::GetAttr(pElem, "offset",    &this->cs.textOffsetVertical);
			XMLUtils::GetAttr(pElem, "offsetH",   &this->cs.textOffsetHorizontal);
			XMLUtils::GetAttr(pElem, "align",     &this->cs.textAlign);
			XMLUtils::GetAttr(pElem, "shadow",    &this->cs.textShadow);
			XMLUtils::GetAttr(pElem, "roundrect", &this->cs.textRoundRect);
		} else if(_stricmp(nameNode, "Background") == 0) {
			XMLUtils::GetAttr(pElem, "gradient",   &this->cs.backGradient);
			XMLUtils::GetAttr(pElem, "color1",     &this->cs.backColor1);
			XMLUtils::GetAttr(pElem, "color2",     &this->cs.backColor2);
			XMLUtils::GetAttr(pElem, "image",      this->cs.backWallpaper, CountOf(this->cs.backWallpaper));
			XMLUtils::GetAttr(pElem, "alphablend", &this->cs.backWallpaperAlphaBlend);
			XMLUtils::GetAttr(pElem, "fitwidth",   &this->cs.backWallpaperFitWidth);
			XMLUtils::GetAttr(pElem, "fitheight",  &this->cs.backWallpaperFitHeight);
			XMLUtils::GetAttr(pElem, "center",     &this->cs.backWallpaperCenter);
			XMLUtils::GetAttr(pElem, "tile",       &this->cs.backWallpaperTile);
		}
	}

	this->iconWidth = this->cs.iconWidthXML;
	this->iconsPerRow = this->cs.iconsPerRowXML;
	this->textHeight = this->cs.textHeightXML;

	return TRUE;
}

BOOL CConfigurationScreen::saveXMLConfig(TiXmlElement *pRoot, BOOL isStaticbar)
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

	if (isStaticbar) {
		pElem = new TiXmlElement("ShrinkToFit");
		XMLUtils::SetTextElem(pElem, this->cs.shrinkToFit);
		pRoot->LinkEndChild(pElem);
	}

	pElem = new TiXmlElement("Offset");
	XMLUtils::SetAttr(pElem, "left",   this->cs.offset.left);
	XMLUtils::SetAttr(pElem, "top",    this->cs.offset.top);
	XMLUtils::SetAttr(pElem, "right",  this->cs.offset.right);
	XMLUtils::SetAttr(pElem, "bottom", this->cs.offset.bottom);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Font");
	XMLUtils::SetAttr(pElem, "facename",  this->cs.textFacename, CountOf(this->cs.textFacename));
	XMLUtils::SetAttr(pElem, "size",      this->cs.textHeightXML);
	XMLUtils::SetAttr(pElem, "color",     this->cs.textColor);
	XMLUtils::SetAttr(pElem, "weight",    this->cs.textWeight);
	XMLUtils::SetAttr(pElem, "offset",    this->cs.textOffsetVertical);
	XMLUtils::SetAttr(pElem, "offsetH",   this->cs.textOffsetHorizontal);
	XMLUtils::SetAttr(pElem, "align",     this->cs.textAlign);
	XMLUtils::SetAttr(pElem, "shadow",    this->cs.textShadow);
	XMLUtils::SetAttr(pElem, "roundrect", this->cs.textRoundRect);
	pRoot->LinkEndChild(pElem);

	pElem = new TiXmlElement("Background");
	XMLUtils::SetAttr(pElem, "gradient",   this->cs.backGradient);
	XMLUtils::SetAttr(pElem, "color1",     this->cs.backColor1);
	XMLUtils::SetAttr(pElem, "color2",     this->cs.backColor2);
	XMLUtils::SetAttr(pElem, "image",      this->cs.backWallpaper, CountOf(this->cs.backWallpaper));
	XMLUtils::SetAttr(pElem, "alphablend", this->cs.backWallpaperAlphaBlend);
	XMLUtils::SetAttr(pElem, "fitwidth",   this->cs.backWallpaperFitWidth);
	XMLUtils::SetAttr(pElem, "fitheight",  this->cs.backWallpaperFitHeight);
	XMLUtils::SetAttr(pElem, "center",     this->cs.backWallpaperCenter);
	XMLUtils::SetAttr(pElem, "tile",       this->cs.backWallpaperTile);
	pRoot->LinkEndChild(pElem);

	return TRUE;
}
