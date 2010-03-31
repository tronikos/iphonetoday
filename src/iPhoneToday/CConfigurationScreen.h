#pragma once

#include <windows.h>
#include "xmlWrapper.h"

typedef struct ConfigurationScreen {
	UINT iconWidthXML;
	UINT iconsPerRowXML;
	UINT minHorizontalSpace;
	UINT additionalVerticalSpace;
	UINT shrinkToFit;
	TCHAR textFacename[LF_FACESIZE];
	UINT textHeightXML;
	COLORREF textColor;
	UINT textBold;
	INT textOffset;
	UINT textShadow;
	BOOL textRoundRect;
	RECT offset;
	COLORREF backColor1;
	COLORREF backColor2;
	TCHAR backWallpaper[MAX_PATH];
} ConfigurationScreen;

class CConfigurationScreen
{
public:
	CConfigurationScreen(void);
	~CConfigurationScreen(void);

	void defaultValues();
	void Scale(double scale);
	BOOL loadXMLConfig(TiXmlElement *pRoot);
	BOOL saveXMLConfig(TiXmlElement *pRoot);

	void calculate(BOOL isStaticbar, int maxIcons, UINT screenWidth, UINT screenHeight);

	// Variables from XML
	ConfigurationScreen cs;

	// Calculated variables
	UINT iconWidth;
	UINT iconsPerRow;
	UINT textHeight;
	UINT distanceIconsH;
	UINT distanceIconsV;
	POINTS posReference;

private:
};
