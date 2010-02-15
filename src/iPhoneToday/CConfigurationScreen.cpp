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
		iconsPerRow = w / (iconWidth + minVerticalSpace);
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
	distanceIconsV = UINT(iconWidth + fontSize + fontOffset + additionalHorizontalSpace);
}

void CConfigurationScreen::defaultValues()
{
	this->iconWidthXML = 48;
	this->iconsPerRowXML = 0;
	this->minVerticalSpace = 0;
	this->additionalHorizontalSpace = 0;

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

BOOL CConfigurationScreen::loadXMLConfig(IXMLDOMNode *pRootNode)
{
	BOOL result = false;
	HRESULT hr = S_OK;
	IXMLDOMNode *pNode = NULL;
	IXMLDOMNode *pNodeSibling = NULL;
	IXMLDOMNamedNodeMap *pNodeMap = NULL;
	TCHAR *nameNode = NULL;

	CHR(pRootNode->get_firstChild(&pNode));

	while (pNode) {
		pNode->get_baseName(&nameNode);

		if(lstrcmpi(nameNode, TEXT("IconWidth")) == 0) {
			this->iconWidthXML = ReadTextNodeNumber(pNode, this->iconWidthXML);
		} else if(lstrcmpi(nameNode, TEXT("IconsPerRow")) == 0) {
			this->iconsPerRowXML = ReadTextNodeNumber(pNode, this->iconsPerRowXML);
		} else if(lstrcmpi(nameNode, TEXT("MinVerticalSpace")) == 0) {
			this->minVerticalSpace = ReadTextNodeNumber(pNode, this->minVerticalSpace);
		} else if(lstrcmpi(nameNode, TEXT("AdditionalHorizontalSpace")) == 0) {
			this->additionalHorizontalSpace = ReadTextNodeNumber(pNode, this->additionalHorizontalSpace);
		} else if(lstrcmpi(nameNode, TEXT("Offset")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->offset.left   = ReadNodeNumber(pNodeMap, TEXT("left"),   this->offset.left);
			this->offset.top    = ReadNodeNumber(pNodeMap, TEXT("top"),    this->offset.top);
			this->offset.right  = ReadNodeNumber(pNodeMap, TEXT("right"),  this->offset.right);
			this->offset.bottom = ReadNodeNumber(pNodeMap, TEXT("bottom"), this->offset.bottom);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Font")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->fontSize   = ReadNodeNumber(pNodeMap, TEXT("size"),   this->fontSize);
			this->fontColor  = ReadNodeNumber(pNodeMap, TEXT("color"),  this->fontColor);
			this->fontBold   = ReadNodeNumber(pNodeMap, TEXT("bold"),   this->fontBold);
			this->fontOffset = ReadNodeNumber(pNodeMap, TEXT("offset"), this->fontOffset);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Background")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->backColor1 = ReadNodeNumber(pNodeMap, TEXT("color1"),  this->backColor1);
			this->backColor2 = ReadNodeNumber(pNodeMap, TEXT("color2"),  this->backColor2);
			RELEASE_OBJ(pNodeMap);
		}

		SysFreeString(nameNode);

		CHR(pNode->get_nextSibling(&pNodeSibling));

		pNode->Release();
		pNode = pNodeSibling;
	}

	this->iconWidth = this->iconWidthXML;
	this->iconsPerRow = this->iconsPerRowXML;

	result = true;

Error:
	SysFreeString(nameNode);
	RELEASE_OBJ(pNode)
	RELEASE_OBJ(pNodeSibling)
	RELEASE_OBJ(pNodeMap);

	return result;
}

BOOL CConfigurationScreen::saveXMLConfig(IXMLDOMDocument* pXMLDom, IXMLDOMElement *pRoot)
{
	IXMLDOMElement *pe = NULL;

	BSTR bstr = NULL;
	HRESULT hr;
	BOOL result = FALSE;

	BSTR bstr_wsntt = SysAllocString(L"\n\t\t");

	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"IconWidth", this->iconWidthXML, 2);
	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"IconsPerRow", this->iconsPerRowXML, 2);
	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"MinVerticalSpace", this->minVerticalSpace, 2);
	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"AdditionalHorizontalSpace", this->additionalHorizontalSpace, 2);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsntt, pRoot);
	bstr = SysAllocString(L"Offset");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"left", this->offset.left);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"top", this->offset.top);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"right", this->offset.right);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"bottom", this->offset.bottom);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsntt, pRoot);
	bstr = SysAllocString(L"Font");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"size", this->fontSize);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color", this->fontColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"bold", this->fontBold);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"offset", this->fontOffset);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsntt, pRoot);
	bstr = SysAllocString(L"Background");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color1", this->backColor1);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color2", this->backColor2);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	result = TRUE;
Error:
	RELEASE_OBJ(pe);
	SysFreeString(bstr);
	SysFreeString(bstr_wsntt);
	return result;
}
