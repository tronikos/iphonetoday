//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// MAILSET.CPP
//
// Allows you to set properties on stores
//


#include "xmlWrapper.h"

// XML Filename
const TCHAR *kszXMLFileName = TEXT("mailset.xml");

// Node names used in the XML
const TCHAR *kszMailSetNode = TEXT("mailset");
const TCHAR *kszStoreNode = TEXT("store");

// Attribute names used in the XML
const TCHAR *kszDisplay = TEXT("display");
const TCHAR *kszDefault = TEXT("default");
const TCHAR *kszType = TEXT("type");
const TCHAR *kszPropTag = TEXT("proptag");
const TCHAR *kszPropName = TEXT("propname");


///////////////////////////////////////////////////////////////////////////////
//
// PathFindFileName
//
// Searches a path for a file name
//
// pszPath: IN parameter pointing to a null-terminated string that contains the path to search.
//
// Returns a pointer to the address of the string if successful, or a pointer to the beginning of the path otherwise.
//

LPTSTR PathFindFileName(LPCTSTR pszPath)
{
    UINT ichCurrent;
    const TCHAR *pch;

    ichCurrent = 0;
    pch = &pszPath[0];

    // Iterate through the entire string
    while (pszPath[ichCurrent])
    {
        // filename only if char after '\' is not end of path or another '\'
        if (pszPath[ichCurrent] == TEXT('\\') &&
            pszPath[ichCurrent + 1] &&
            pszPath[ichCurrent + 1] != TEXT('\\'))
        {
            pch = &pszPath[ichCurrent + 1];
        }
        ichCurrent++;
    }

    return (TCHAR*)pch;
}


///////////////////////////////////////////////////////////////////////////////
//
// GetXMLPathName
//
// Generate the XML PathName based on the current app directoy and a predefined XML filename
// 
// pszPath: IN parameter pointing to a buffer to hold the pathname of the xml file
// cMaxPath: IN parameter to the length of the buffer
//
// Returns BOOL
//

BOOL GetXMLPathName(LPTSTR pszPath, UINT cMaxPath)
{
    TCHAR *pszFileName;
    BOOL fSuccess;
    HRESULT hr;
    DWORD cch;

    ZeroMemory(pszPath, cMaxPath);

    // Assume failure until all steps are done
    fSuccess = FALSE;
    
    // Get the pathname of the exe
    cch = GetModuleFileName(NULL, pszPath, cMaxPath);
    CBR(cch != 0);

    // Get the directory that the exe is running from
    // pszFileName should not point to pszPath, if so it means the filename of the exe cannot be found; pszPath is invalid
    pszFileName = PathFindFileName(pszPath);
    CBR(pszFileName != pszPath);
        
    // Generate the pathname of the XML file
    *pszFileName = NULL;
    hr = StringCchCat(pszPath, cMaxPath, kszXMLFileName);
    CHR(hr);

    // All steps successful
    fSuccess = TRUE;

Error:
    return fSuccess;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadNodeType
//
// This function will read in a type node and return the VALUETYPE corresponding to the node value
// 
// pNodeMap: IN parameter pointing to the node
// pszNodeName : IN parameter indicating the property to retrieve from the XML
//
// Returns VALUETYPE
//

VALUETYPE ReadNodeType(IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName)
{
    HRESULT hr;
    IXMLDOMNode *pAttribute = NULL;
    VARIANT vt;
    VALUETYPE vtRet = vtNone;

    // Get the appropriate attribute
    hr = pNodeMap->getNamedItem(pszNodeName, &pAttribute);
    CHR(hr);
    CBR(pAttribute != NULL);

    // Read the value from this attribute
    VariantInit(&vt);
    hr = pAttribute->get_nodeValue(&vt);
    CHR(hr);

    // The string should be 0 (string), 1 (integer), or 2 (boolean)
    // If it's not one of these, the return value will be vtNone
    if (!lstrcmp(vt.bstrVal, TEXT("0")))
    {
        vtRet = vtString;
    }
    else if (!lstrcmp(vt.bstrVal, TEXT("1")))
    {
        vtRet = vtInteger;
    }
    else if (!lstrcmp(vt.bstrVal, TEXT("2")))
    {
        vtRet = vtBoolean;
    }
    
Error:
    RELEASE_OBJ(pAttribute)

    VariantClear(&vt);

    return vtRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadNodeNumber
//
// This function will read in a type node and return the ULONG corresponding to the node value
// 
// pNodeMap: IN parameter pointing to the node
// pszNodeName : IN parameter indicating the property to retrieve from the XML
//
// Returns ULONG
//

ULONG ReadNodeNumber(IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName)
{
    HRESULT hr;
    IXMLDOMNode *pAttribute = NULL;
    VARIANT vt;
    ULONG ulRet = 0;

    // Get the appropriate attribute
    hr = pNodeMap->getNamedItem(pszNodeName, &pAttribute);
    CHR(hr);
    CBR(pAttribute != NULL);

    // Read the value of this attribute
    VariantInit(&vt);
    hr = pAttribute->get_nodeValue(&vt);
    CHR(hr);

    // Convert the string to a number
    ulRet = _ttoi(vt.bstrVal);

Error:
    RELEASE_OBJ(pAttribute)
    VariantClear(&vt);

    return ulRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// ReadNodeString
//
// This function will read in a type node and return the string corresponding to the node value
// 
// pNodeMap: IN parameter pointing to the node
// pszNodeName : IN parameter indicating the property to retrieve from the XML
//
// Returns LPTSTR -- the caller must free this string
//

BOOL ReadNodeString(LPTSTR pszRet, IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName, TCHAR *defaultValue)
{
	BOOL result = FALSE;
    HRESULT hr;
    IXMLDOMNode *pAttribute = NULL;
    VARIANT vt;
    size_t cch;
    

    // Get the appropriate attribute
    hr = pNodeMap->getNamedItem(pszNodeName, &pAttribute);
    CHR(hr);
    CBR(pAttribute != NULL);

    // Read the value of this attribute
    VariantInit(&vt);
    hr = pAttribute->get_nodeValue(&vt);
    CHR(hr);

    // Allocate memory and copy it
    hr = StringCchLength(vt.bstrVal, STRSAFE_MAX_CCH, &cch);
    CHR(hr);
    // pszRet = new TCHAR[++cch];
    // CPR(pszRet);
	StringCchCopy(pszRet, ++cch, vt.bstrVal);

	result = TRUE;
Error:
    RELEASE_OBJ(pAttribute)
	VariantClear(&vt);
	
	if (result) {
		return result;
	}

	hr = StringCchLength(defaultValue, STRSAFE_MAX_CCH, &cch);
    StringCchCopy(pszRet, ++cch, defaultValue);

    return result;
}

ULONG ReadTextNodeNumber(IXMLDOMNode *pNode, ULONG defaultValue)
{
	ULONG result = 0;
	TCHAR *value;
	size_t cch = 0;
	if (pNode) {
		pNode->get_text(&value);
		StringCchLength(value, STRSAFE_MAX_CCH, &cch);

		if (cch > 0) {
			result = _ttol(value);
		} else {
			result = defaultValue;
		}
	} else {
		result = defaultValue;
	}

	SysFreeString(value);
	value = NULL;

    return result;
}

BOOL ReadTextNode(LPTSTR pszRet, IXMLDOMNode *pNode, TCHAR *defaultValue)
{
	TCHAR *value;
	if (pNode) {
		pNode->get_text(&value);
		wcscpy(pszRet, value);
	} else {
		wcscpy(pszRet, defaultValue);
	}
	SysFreeString(value);
	value = NULL;

	return true;
}

// Helper function to create a DOM instance. 
IXMLDOMDocument *DomFromCOM()
{
    HRESULT hr;
    IXMLDOMDocument *pxmldoc = NULL;
	CLSID clsid;

	CHR(CLSIDFromProgID(TEXT("Msxml2.DOMDocument"), &clsid));
    CHR(CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (LPVOID *) &pxmldoc));

    CHR(pxmldoc->put_async(VARIANT_FALSE));
    CHR(pxmldoc->put_validateOnParse(VARIANT_FALSE));
    CHR(pxmldoc->put_resolveExternals(VARIANT_FALSE));
    CHR(pxmldoc->put_preserveWhiteSpace(VARIANT_TRUE));

    return pxmldoc;
Error:
    if (pxmldoc)
    {
        pxmldoc->Release();
    }
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


// Helper function to display xml parse error:
void ReportParseError(IXMLDOMDocument *pDom, char *desc) {
    IXMLDOMParseError *pXMLErr=NULL;
    BSTR bstrReason = NULL;
    HRESULT hr;
    CHR(pDom->get_parseError(&pXMLErr));
    CHR(pXMLErr->get_reason(&bstrReason));

Error:
    if (pXMLErr) pXMLErr->Release();
    if (bstrReason) SysFreeString(bstrReason);
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
    if (pws) pws->Release();
    pws=NULL;
    if (pBuf) pBuf->Release();
    pBuf=NULL;
}

// Helper function to append a child to a parent node:
void AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent)
{
    HRESULT hr;
    IXMLDOMNode *pNode=NULL;
    CHR(pParent->appendChild(pChild, &pNode));
Error:
    if (pNode) pNode->Release();
    pNode=NULL;
}

