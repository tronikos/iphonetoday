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
		iconsPerRow = w / (iconWidth + minHorizontalSpace);
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

BOOL CConfigurationScreen::loadXMLConfig(IXMLDOMNode *pRoot)
{
	if (pRoot == NULL) {
		return FALSE;
	}

	BOOL result = false;
	HRESULT hr = S_OK;
	IXMLDOMNode *pNode = NULL;
	IXMLDOMNode *pNodeSibling = NULL;
	IXMLDOMNamedNodeMap *pNodeMap = NULL;
	TCHAR *nameNode = NULL;

	CHR(pRoot->get_firstChild(&pNode));

	while (pNode) {
		pNode->get_baseName(&nameNode);

		if(lstrcmpi(nameNode, TEXT("IconWidth")) == 0) {
			XMLUtils::GetTextElem(pNode, &this->iconWidthXML);
		} else if(lstrcmpi(nameNode, TEXT("IconsPerRow")) == 0) {
			XMLUtils::GetTextElem(pNode, &this->iconsPerRowXML);
		} else if(lstrcmpi(nameNode, TEXT("MinHorizontalSpace")) == 0) {
			XMLUtils::GetTextElem(pNode, &this->minHorizontalSpace);
		} else if(lstrcmpi(nameNode, TEXT("AdditionalVerticalSpace")) == 0) {
			XMLUtils::GetTextElem(pNode, &this->additionalVerticalSpace);
		} else if(lstrcmpi(nameNode, TEXT("Offset")) == 0) {
			XMLUtils::GetAttr(pNode, TEXT("left"),   &this->offset.left);
			XMLUtils::GetAttr(pNode, TEXT("top"),    &this->offset.top);
			XMLUtils::GetAttr(pNode, TEXT("right"),  &this->offset.right);
			XMLUtils::GetAttr(pNode, TEXT("bottom"), &this->offset.bottom);
		} else if(lstrcmpi(nameNode, TEXT("Font")) == 0) {
			XMLUtils::GetAttr(pNode, TEXT("size"),   &this->fontSize);
			XMLUtils::GetAttr(pNode, TEXT("color"),  &this->fontColor);
			XMLUtils::GetAttr(pNode, TEXT("bold"),   &this->fontBold);
			XMLUtils::GetAttr(pNode, TEXT("offset"), &this->fontOffset);
		} else if(lstrcmpi(nameNode, TEXT("Background")) == 0) {
			XMLUtils::GetAttr(pNode, TEXT("color1"),  &this->backColor1);
			XMLUtils::GetAttr(pNode, TEXT("color2"),  &this->backColor2);
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
	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"MinHorizontalSpace", this->minHorizontalSpace, 2);
	CConfiguracion::creaNodoXMLConfig(pXMLDom, pRoot, L"AdditionalVerticalSpace", this->additionalVerticalSpace, 2);

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
