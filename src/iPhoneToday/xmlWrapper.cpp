#include "xmlWrapper.h"

BOOL XMLUtils::GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, INT *value)
{

	if (pNode == NULL || value == NULL) return FALSE;

	BOOL result = FALSE;
	HRESULT hr;
	IXMLDOMNode *pAttribute = NULL;
	IXMLDOMNamedNodeMap *pNodeMap = NULL;
	VARIANT vt;

	// Get the appropriate attribute
	CHR(pNode->get_attributes(&pNodeMap));
	CHR(pNodeMap->getNamedItem(pszAttrName, &pAttribute));
	CBR(pAttribute != NULL);

	// Read the value of this attribute
	VariantInit(&vt);
	hr = pAttribute->get_nodeValue(&vt);
	CHR(hr);

	// Convert the string to a number
	*value = _ttoi(vt.bstrVal);
	result = TRUE;

Error:
	RELEASE_OBJ(pNodeMap);
	RELEASE_OBJ(pAttribute)
	VariantClear(&vt);
	return result;
}

BOOL XMLUtils::GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, UINT *value)
{
	return XMLUtils::GetAttr(pNode, pszAttrName, (INT *) value);
}

BOOL XMLUtils::GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, LONG *value)
{
	int v;
	if (!XMLUtils::GetAttr(pNode, pszAttrName, &v)) {
		return FALSE;
	}
	*value = v;
	return TRUE;
}

BOOL XMLUtils::GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, COLORREF *value)
{
	return XMLUtils::GetAttr(pNode, pszAttrName, (INT *) value);
}

BOOL XMLUtils::GetAttr(IXMLDOMNode *pNode, TCHAR *pszAttrName, LPTSTR pszRet, size_t bufflen)
{
	if (pNode == NULL || pszRet == NULL) return FALSE;

	BOOL result = FALSE;
	HRESULT hr;
	IXMLDOMNode *pAttribute = NULL;
	IXMLDOMNamedNodeMap *pNodeMap = NULL;
	VARIANT vt;

	// Get the appropriate attribute
	CHR(pNode->get_attributes(&pNodeMap));
	CHR(pNodeMap->getNamedItem(pszAttrName, &pAttribute));
	CBR(pAttribute != NULL);

	// Read the value of this attribute
	VariantInit(&vt);
	hr = pAttribute->get_nodeValue(&vt);
	CHR(hr);

	// Allocate memory and copy it
	//CHR(StringCchLength(vt.bstrVal, STRSAFE_MAX_CCH, &cch));
	// pszRet = new TCHAR[++cch];
	// CPR(pszRet);
	StringCchCopy(pszRet, bufflen, vt.bstrVal);

	result = TRUE;
Error:
	RELEASE_OBJ(pAttribute)
	VariantClear(&vt);
	
	return result;
}

BOOL XMLUtils::GetTextElem(IXMLDOMNode *pNode, UINT *value)
{
	if (pNode == NULL || value == NULL) return FALSE;

	TCHAR *v;
	if (SUCCEEDED(pNode->get_text(&v))) {
		*value = _ttol(v);
		SysFreeString(v);
	}

	return TRUE;
}






// Helper function to create a DOM instance. 
IXMLDOMDocument *DomFromCOM()
{
	HRESULT hr;
	IXMLDOMDocument *pxmldoc = NULL;
	CLSID clsid;

	hr = CLSIDFromProgID(TEXT("Msxml2.DOMDocument"), &clsid);
	if (FAILED(hr))
		CHR(CLSIDFromProgID(TEXT("Microsoft.XMLDOM"), &clsid));
	CHR(CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (LPVOID *) &pxmldoc));

	CHR(pxmldoc->put_async(VARIANT_FALSE));
	CHR(pxmldoc->put_validateOnParse(VARIANT_FALSE));
	CHR(pxmldoc->put_resolveExternals(VARIANT_FALSE));
	CHR(pxmldoc->put_preserveWhiteSpace(VARIANT_TRUE));

	return pxmldoc;
Error:
	RELEASE_OBJ(pxmldoc);
	return NULL;
}

VARIANT VariantString(BSTR str)
{
	VARIANT var;
	VariantInit(&var);
	if (str != NULL) {
		V_BSTR(&var) = SysAllocString(str);
	} else {
		V_BSTR(&var) = SysAllocString(L"");
	}
	V_VT(&var) = VT_BSTR;
	return var;
}


// Helper function to append a whitespace text node to a 
// specified element:
void AddWhiteSpaceToNode(IXMLDOMDocument* pDom, BSTR bstrWs, IXMLDOMNode *pNode)
{
	HRESULT hr;
	IXMLDOMText *pws=NULL;
	IXMLDOMNode *pBuf=NULL;
	CHR(pDom->createTextNode(bstrWs,&pws));
	CHR(pNode->appendChild(pws,&pBuf));
Error:
	RELEASE_OBJ(pws);
	RELEASE_OBJ(pBuf);
}

// Helper function to append a child to a parent node:
void AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
	HRESULT hr;
	IXMLDOMNode *pNode=NULL;
	CHR(pParent->appendChild(pChild, &pNode));
Error:
	RELEASE_OBJ(pNode);
}
