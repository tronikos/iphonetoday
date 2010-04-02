#include "stdafx.h"

typedef BOOL (STDAPICALLTYPE FAR fSipShowIM)(DWORD);
static fSipShowIM *pSipShowIM = NULL;

typedef BOOL (STDAPICALLTYPE FAR fSipGetInfo)(SIPINFO*);
static fSipGetInfo *pSipGetInfo = NULL;

typedef BOOL (STDAPICALLTYPE FAR fGetSystemPowerStatusEx)(PSYSTEM_POWER_STATUS_EX,BOOL);
static fGetSystemPowerStatusEx *pGetSystemPowerStatusEx = NULL;

typedef BOOL (STDAPICALLTYPE FAR fAlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);
static fAlphaBlend *pAlphaBlend = NULL;

BOOL InitCoredll()
{
	static HMODULE hCoredllLib = NULL;
	if (hCoredllLib != NULL)
		return TRUE;
	hCoredllLib = LoadLibrary(L"coredll.dll");
	if (hCoredllLib == NULL)
		return FALSE;
	pSipShowIM = (fSipShowIM*)GetProcAddress(hCoredllLib, L"SipShowIM");
	pSipGetInfo = (fSipGetInfo*)GetProcAddress(hCoredllLib, L"SipGetInfo");
	pGetSystemPowerStatusEx = (fGetSystemPowerStatusEx*)GetProcAddress(hCoredllLib, L"GetSystemPowerStatusEx");
	pAlphaBlend = (fAlphaBlend*)GetProcAddress(hCoredllLib, L"AlphaBlend");
	return TRUE;
}

BOOL WINAPI SipShowIM(DWORD dwFlag)
{
	if (InitCoredll() && pSipShowIM != NULL)
		return pSipShowIM(dwFlag);
	return FALSE;
}

BOOL WINAPI SipGetInfo(SIPINFO* pSipInfo)
{
	if (InitCoredll() && pSipGetInfo != NULL)
		return pSipGetInfo(pSipInfo);
	return FALSE;
}

BOOL WINAPI GetSystemPowerStatusEx(PSYSTEM_POWER_STATUS_EX pSystemPowerStatusEx, BOOL fUpdate)
{
	if (InitCoredll() && pGetSystemPowerStatusEx != NULL)
		return pGetSystemPowerStatusEx(pSystemPowerStatusEx, fUpdate);
	return FALSE;
}

BOOL AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
				HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
				BLENDFUNCTION blendFunction)
{
	BOOL ret = FALSE;
	if (hdcSrc == NULL || hdcDest == NULL)
		return FALSE;
	if (InitCoredll() && pAlphaBlend != NULL)
		ret = pAlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
							hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc,
							blendFunction);
	if (ret)
		return TRUE;

	BITMAPINFO bmInfoDest;
	memset(&bmInfoDest.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmInfoDest.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoDest.bmiHeader.biWidth = nWidthDest;
	bmInfoDest.bmiHeader.biHeight = nHeightDest;
	bmInfoDest.bmiHeader.biPlanes = 1;
	bmInfoDest.bmiHeader.biBitCount = 32;
	BYTE *pDest;
	HDC tmp_hdcDest = CreateCompatibleDC(hdcDest);
	HBITMAP tmp_bmpDest = CreateDIBSection(hdcDest, &bmInfoDest, DIB_RGB_COLORS, (void**)&pDest, 0, 0);
	HGDIOBJ tmp_objDest = SelectObject(tmp_hdcDest, tmp_bmpDest);
	BitBlt(tmp_hdcDest, 0, 0, nWidthDest, nHeightDest, hdcDest, nXOriginDest, nYOriginDest, SRCCOPY);

	BITMAPINFO bmInfoSrc;
	memset(&bmInfoSrc.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
	bmInfoSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//bmInfoSrc.bmiHeader.biWidth = nWidthSrc;
	//bmInfoSrc.bmiHeader.biHeight = nHeightSrc;
	bmInfoSrc.bmiHeader.biWidth = nWidthDest;
	bmInfoSrc.bmiHeader.biHeight = nHeightDest;
	bmInfoSrc.bmiHeader.biPlanes = 1;
	bmInfoSrc.bmiHeader.biBitCount = 32;
	BYTE *pSrc;
	HDC tmp_hdcSrc = CreateCompatibleDC(hdcSrc);
	HBITMAP tmp_bmpSrc = CreateDIBSection(hdcSrc, &bmInfoSrc, DIB_RGB_COLORS, (void**)&pSrc, 0, 0);
	HGDIOBJ tmp_objSrc = SelectObject(tmp_hdcSrc, tmp_bmpSrc);
	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc) {
		BitBlt(tmp_hdcSrc, 0, 0, nWidthSrc, nHeightSrc,
			hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	} else {
		StretchBlt(tmp_hdcSrc, 0, 0, nWidthDest, nHeightDest,
			hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	}

	BYTE *pD = pDest;
	BYTE *pS = pSrc;
	for (int i = 0; i < nWidthDest * nHeightDest; i++) {
		BYTE a = pS[3];
		if (a != 0) {
			if (a == 0xFF) {
				pD[0] = pS[0];
				pD[1] = pS[1];
				pD[2] = pS[2];
			} else {
				BYTE na = ~a;
				if (blendFunction.AlphaFormat == AC_SRC_ALPHA_NONPREMULT) {
					pD[0] = ((na * pD[0]) >> 8) + (BYTE)((pS[0] * a) >> 8);
					pD[1] = ((na * pD[1]) >> 8) + (BYTE)((pS[1] * a) >> 8);
					pD[2] = ((na * pD[2]) >> 8) + (BYTE)((pS[2] * a) >> 8);
				} else {
					pD[0] = ((na * pD[0]) >> 8) + pS[0];
					pD[1] = ((na * pD[1]) >> 8) + pS[1];
					pD[2] = ((na * pD[2]) >> 8) + pS[2];
				}
			}
		}
		pD += 4;
		pS += 4;
	}

	SetDIBitsToDevice(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 0, 0, 0, nHeightDest, pDest, &bmInfoDest, 0);
	DeleteObject(SelectObject(tmp_hdcDest, tmp_objDest));
	DeleteDC(tmp_hdcDest);

	DeleteObject(SelectObject(tmp_hdcSrc, tmp_objSrc));
	DeleteDC(tmp_hdcSrc);

	return TRUE;
}

typedef BOOL (STDAPICALLTYPE FAR fPropertySheet)(LPCPROPSHEETHEADERW); 
static fPropertySheet *pPropertySheet = NULL;


BOOL InitCommctrl()
{
	static HMODULE hCommctrLib = NULL;
	if (hCommctrLib != NULL)
		return TRUE;
	hCommctrLib = LoadLibrary(L"commctrl.dll");
	if (hCommctrLib == NULL)
		return FALSE;
	pPropertySheet = (fPropertySheet*)GetProcAddress(hCommctrLib, L"PropertySheetW");
	return TRUE;
}

WINCOMMCTRLAPI int WINAPI PropertySheet(LPCPROPSHEETHEADERW lppsph)
{
	if (InitCommctrl() && pPropertySheet != NULL)
		return pPropertySheet(lppsph);
	return -1;
}



typedef BOOL (STDAPICALLTYPE FAR fChooseColor)(LPCHOOSECOLOR); 
static fChooseColor *pChooseColor = NULL;

BOOL InitCommdlg()
{
	static HMODULE hCommdlgLib = NULL;
	if (hCommdlgLib != NULL)
		return TRUE;
	hCommdlgLib = LoadLibrary(L"commdlg.dll");
	if (hCommdlgLib == NULL)
		return FALSE;
	pChooseColor = (fChooseColor*)GetProcAddress(hCommdlgLib, L"ChooseColor");
	return TRUE;
}

BOOL ChooseColor(LPCHOOSECOLOR lpcc)
{
	if (InitCommdlg() && pChooseColor != NULL)
		return pChooseColor(lpcc);
	return FALSE;
}

/*
int MulDiv(int a, int b, int c)
{
	__int64 ret = ((__int64) a * (__int64) b) / c;

	if(ret > INT_MAX || ret < INT_MIN)
		return -1;
	return (int) ret;
}
*/


typedef HRESULT (STDAPICALLTYPE FAR fCoInitializeEx)(LPVOID,DWORD);
static fCoInitializeEx *pCoInitializeEx = NULL;

typedef void (STDAPICALLTYPE FAR fCoUninitialize)();
static fCoUninitialize *pCoUninitialize = NULL;

typedef HRESULT (STDAPICALLTYPE FAR fCoCreateInstance)(REFCLSID,LPUNKNOWN,DWORD,REFIID,LPVOID*);
static fCoCreateInstance *pCoCreateInstance = NULL;

BOOL InitOle32()
{
	static HMODULE hOl32Lib = NULL;
	if (hOl32Lib != NULL)
		return TRUE;
	hOl32Lib = LoadLibrary(L"ole32.dll");
	if (hOl32Lib == NULL)
		return FALSE;
	pCoInitializeEx = (fCoInitializeEx*)GetProcAddress(hOl32Lib, L"CoInitializeEx");
	pCoUninitialize = (fCoUninitialize*)GetProcAddress(hOl32Lib, L"CoUninitialize");
	pCoCreateInstance = (fCoCreateInstance*)GetProcAddress(hOl32Lib, L"CoCreateInstance");
	return TRUE;
}

HRESULT CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit)
{
	if (InitOle32() && pCoInitializeEx != NULL)
		return pCoInitializeEx(pvReserved, dwCoInit);
	return S_FALSE;
}

void CoUninitialize()
{
	if (InitOle32() && pCoUninitialize != NULL)
		pCoUninitialize();
}

STDAPI CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
	if (InitOle32() && pCoCreateInstance != NULL)
		return pCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
	return S_FALSE;
}
