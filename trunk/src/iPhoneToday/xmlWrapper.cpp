#include "xmlWrapper.h"

BOOL XMLUtils::GetAttr(TiXmlElement *pElem, const char *pszAttrName, INT *value)
{
	int v;
	if (pElem == NULL || value == NULL || TIXML_SUCCESS != pElem->QueryIntAttribute(pszAttrName, &v)) {
		return FALSE;
	}
	*value = v;
	return TRUE;
}

BOOL XMLUtils::GetAttr(TiXmlElement *pElem, const char *pszAttrName, UINT *value)
{
	return XMLUtils::GetAttr(pElem, pszAttrName, (INT *) value);
}

BOOL XMLUtils::GetAttr(TiXmlElement *pElem, const char *pszAttrName, LONG *value)
{
	int v;
	if (!XMLUtils::GetAttr(pElem, pszAttrName, &v)) {
		return FALSE;
	}
	*value = v;
	return TRUE;
}

BOOL XMLUtils::GetAttr(TiXmlElement *pElem, const char *pszAttrName, COLORREF *value)
{
	return XMLUtils::GetAttr(pElem, pszAttrName, (INT *) value);
}

BOOL XMLUtils::GetAttr(TiXmlElement *pElem, const char *pszAttrName, LPTSTR pszRet, size_t bufflen)
{
	if (pElem == NULL || pszRet == NULL) return FALSE;
	const char *value = pElem->Attribute(pszAttrName);
	if (value == NULL) {
		return FALSE;
	}
	MultiByteToWideChar(CP_UTF8, 0, value, -1, pszRet, bufflen);
	return TRUE;
}

void XMLUtils::SetAttr(TiXmlElement *pElem, const char *pszAttrName, UINT value)
{
	if (pElem == NULL) return;
	pElem->SetAttribute(pszAttrName, (int) value);
}

void XMLUtils::SetAttr(TiXmlElement *pElem, const char *pszAttrName, LPTSTR psz, size_t bufflen)
{
	if (pElem == NULL) return;
	char *buffer = new char[bufflen];
	WideCharToMultiByte(CP_UTF8, 0, psz, bufflen, buffer, bufflen, NULL, NULL);
	pElem->SetAttribute(pszAttrName, buffer);
	delete buffer;
}

BOOL XMLUtils::GetTextElem(TiXmlElement *pElem, UINT *value)
{
	if (pElem == NULL || value == NULL) return FALSE;
	const char *text = pElem->GetText();
	if (text == NULL) {
		return FALSE;
	}
	*value = atoi(text);
	return TRUE;
}

void XMLUtils::SetTextElem(TiXmlElement *pElem, INT value)
{
	if (pElem == NULL) return;
	char buffer[MAX_PATH];
	sprintf(buffer, "%d", value);
	TiXmlText *pText = new TiXmlText(buffer);
	pElem->LinkEndChild(pText);
}