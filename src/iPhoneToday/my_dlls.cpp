#include "stdafx.h"

typedef BOOL (STDAPICALLTYPE FAR fSipShowIM)(DWORD);
static fSipShowIM *pSipShowIM = NULL;

BOOL InitCoredll()
{
	static HMODULE hCoredllLib = NULL;
	if (hCoredllLib != NULL)
		return TRUE;
	hCoredllLib = LoadLibrary(L"coredll.dll");
	if (hCoredllLib == NULL)
		return FALSE;
	pSipShowIM = (fSipShowIM*)GetProcAddress(hCoredllLib, L"SipShowIM");
	return TRUE;
}

BOOL WINAPI SipShowIM(DWORD dwFlag)
{
	if (InitCoredll() && pSipShowIM != NULL)
		return pSipShowIM(dwFlag);
	return FALSE;
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
