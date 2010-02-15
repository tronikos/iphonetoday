#pragma once

#include <windows.h>
#include "xmlWrapper.h"

class CConfigurationScreen
{
public:
	CConfigurationScreen(void);
	~CConfigurationScreen(void);

	void defaultValues();
	BOOL loadXMLConfig(IXMLDOMNode *pRootNode);
	BOOL saveXMLConfig(IXMLDOMDocument* pXMLDom, IXMLDOMElement *pRoot);

	void calculate(BOOL isBottombar, int maxIcons, UINT screenWidth, UINT screenHeight);

	// Variables from XML
	UINT iconWidthXML;
	UINT iconsPerRowXML;
	UINT minVerticalSpace;
	UINT additionalHorizontalSpace;
	UINT fontSize;
	COLORREF fontColor;
	UINT fontBold;
	INT fontOffset;
	RECT offset;
	COLORREF backColor1;
	COLORREF backColor2;

	// Calculated variables
	UINT iconWidth;
	UINT iconsPerRow;
	UINT distanceIconsH;
	UINT distanceIconsV;
	POINTS posReference;

private:
};
