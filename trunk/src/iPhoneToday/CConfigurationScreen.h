#pragma once

#include <windows.h>
#include "xmlWrapper.h"

typedef struct ConfigurationScreen {
	UINT iconWidthXML;
	UINT iconsPerRowXML;
	UINT minHorizontalSpace;
	UINT additionalVerticalSpace;
	UINT shrinkToFit;
	UINT fontSize;
	COLORREF fontColor;
	UINT fontBold;
	INT fontOffset;
	UINT fontShadow;
	BOOL fontRoundRect;
	RECT offset;
	COLORREF backColor1;
	COLORREF backColor2;
} ConfigurationScreen;

class CConfigurationScreen
{
public:
	CConfigurationScreen(void);
	~CConfigurationScreen(void);

	void defaultValues();
	BOOL loadXMLConfig(TiXmlElement *pRoot);
	BOOL saveXMLConfig(TiXmlElement *pRoot);

	void calculate(BOOL isStaticbar, int maxIcons, UINT screenWidth, UINT screenHeight);

	// Variables from XML
	ConfigurationScreen cs;

	// Calculated variables
	UINT iconWidth;
	UINT iconsPerRow;
	UINT distanceIconsH;
	UINT distanceIconsV;
	POINTS posReference;

private:
};
