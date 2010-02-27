#pragma once

#include <windows.h>
#include "tinyxml.h"

class XMLUtils
{
public:
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, INT *value);
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, UINT *value);
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, LONG *value);
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, COLORREF *value);
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, float *value);
	BOOL static GetAttr(TiXmlElement *pElem, const char *pszAttrName, LPTSTR pszRet, size_t bufflen);

	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, INT value);
	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, UINT value);
	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, LONG value);
	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, COLORREF value);
	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, float value);
	void static SetAttr(TiXmlElement *pElem, const char *pszAttrName, LPTSTR psz, size_t bufflen);

	BOOL static GetTextElem(TiXmlElement *pElem, UINT *value);

	void static SetTextElem(TiXmlElement *pElem, INT value);
};
