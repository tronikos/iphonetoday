#pragma once

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


#include <windows.h>
#include "mapiutil.h"
#include "macros.h"
#include "msxml.h"

#include "Log.h"


/*
// Different types that we recognize from the XML:
//   0 = String
//   1 = Integer
//   2 = Boolean
typedef enum
{
    vtNone,
    vtInteger,
    vtBoolean,
    vtString
} VALUETYPE;

// Structure to hold information about a store property
//   pwszPropertyName - The name of the property (e.g. PR_CE_SIGNATURE)
//   ulPropTag - The numeric value of this property
//   pwszDisplayName - A friendly description of this property
//   pwszDefault - The default value to use if this property isn't in the store
//   vt - The type of property
//   pStore - A pointer to the store being modified or queried
typedef struct tagSTOREINFO
{
    TCHAR *pwszPropertyName;
    ULONG ulPropTag;
    TCHAR *pwszDisplayName;
    TCHAR *pwszDefault;
    VALUETYPE vt;
    IMsgStore *pStore;
} STOREINFO;


LPTSTR PathFindFileName(LPCTSTR pszPath);
BOOL GetXMLPathName(LPTSTR pszPath, UINT cMaxPath);
VALUETYPE ReadNodeType(IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName);
*/
ULONG ReadNodeNumber(IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName, ULONG defaultValue = 0);
BOOL ReadNodeString(LPTSTR pszRet, IXMLDOMNamedNodeMap *pNodeMap, TCHAR *pszNodeName, TCHAR *defaultValue = TEXT(""));
ULONG ReadTextNodeNumber(IXMLDOMNode *pNode, ULONG defaultValue);
BOOL ReadTextNode(LPTSTR pszRet, IXMLDOMNode *pNode, TCHAR *defaultValue = TEXT(""));

IXMLDOMDocument *DomFromCOM();
VARIANT VariantString(BSTR str);
void AddWhiteSpaceToNode(IXMLDOMDocument* pDom, BSTR bstrWs, IXMLDOMNode *pNode);
void AppendChildToParent(IXMLDOMNode *pChild, IXMLDOMNode *pParent);