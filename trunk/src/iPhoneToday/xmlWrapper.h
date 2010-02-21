#pragma once

#include <windows.h>
#include <msxml.h>
#include "macros.h"

class XMLUtils
{
public:
	BOOL static GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, INT *value);
	BOOL static GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, UINT *value);
	BOOL static GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, LONG *value);
	BOOL static GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, COLORREF *value);
	BOOL static GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, LPTSTR pszRet, size_t bufflen);

	BOOL static GetTextElem(IXMLDOMNode *pNode, UINT *value);
};

IXMLDOMDocument *DomFromCOM();
VARIANT VariantString(BSTR str);
void AddWhiteSpaceToNode(IXMLDOMDocument* pDom, BSTR bstrWs, IXMLDOMNode *pNode);
void AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);