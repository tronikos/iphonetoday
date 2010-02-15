#include "StdAfx.h"

typedef DWORD (STDAPICALLTYPE FAR fSHRecognizeGesture)(SHRGINFO*); 
typedef BOOL (STDAPICALLTYPE FAR fGetOpenFileNameEx)(LPOPENFILENAMEEX); 
typedef BOOL (STDAPICALLTYPE FAR fSHFullScreen)(HWND,DWORD);
typedef BOOL (STDAPICALLTYPE FAR fSHCreateMenuBar)(SHMENUBARINFO*);
typedef BOOL (STDAPICALLTYPE FAR fSHInitDialog)(PSHINITDLGINFO); 
typedef BOOL (STDAPICALLTYPE FAR fSHInitExtraControls)(); 

static fSHRecognizeGesture	*pSHRecognizeGesture = NULL;
static fGetOpenFileNameEx	*pGetOpenFileNameEx = NULL;
static fSHFullScreen		*pSHFullScreen = NULL;
static fSHCreateMenuBar		*pSHCreateMenuBar = NULL;
static fSHInitDialog		*pSHInitDialog = NULL;
static fSHInitExtraControls	*pSHInitExtraControls = NULL;

BOOL InitAygshell()
{
	static HMODULE hAygshellLib = NULL;
	if (hAygshellLib != NULL)
		return TRUE;
	hAygshellLib = LoadLibrary(L"aygshell.dll");
	if (hAygshellLib == NULL)
		return FALSE;
	pSHRecognizeGesture		= (fSHRecognizeGesture*)	GetProcAddress(hAygshellLib, L"SHRecognizeGesture");
	pGetOpenFileNameEx		= (fGetOpenFileNameEx*)		GetProcAddress(hAygshellLib, L"GetOpenFileNameEx");
	pSHFullScreen			= (fSHFullScreen*)			GetProcAddress(hAygshellLib, L"SHFullScreen");
	pSHCreateMenuBar		= (fSHCreateMenuBar*)		GetProcAddress(hAygshellLib, L"SHCreateMenuBar");
	pSHInitDialog			= (fSHInitDialog*)			GetProcAddress(hAygshellLib, L"SHInitDialog");
	pSHInitExtraControls	= (fSHInitExtraControls*)	GetProcAddress(hAygshellLib, L"SHInitExtraControls");
	return TRUE;
}

WINSHELLAPI DWORD SHRecognizeGesture(SHRGINFO *shrg)
{
	if (InitAygshell() && pSHRecognizeGesture != NULL)
		return pSHRecognizeGesture(shrg);
	return -1;
}

BOOL GetOpenFileNameEx(LPOPENFILENAMEEX lpofnex)
{
	if (InitAygshell() && pGetOpenFileNameEx != NULL)
		return pGetOpenFileNameEx(lpofnex);
	lpofnex->lStructSize = sizeof(OPENFILENAME);
	if (_tcslen(lpofnex->lpstrInitialDir) == 0) {
		lpofnex->lpstrInitialDir = L"\\";
	}
	return GetOpenFileName((LPOPENFILENAME)lpofnex);
}

BOOL SHFullScreen(HWND hwndRequester, DWORD dwState)
{
	if (InitAygshell() && pSHFullScreen != NULL)
		return pSHFullScreen(hwndRequester, dwState);
	return FALSE;
}

WINSHELLAPI BOOL SHCreateMenuBar(SHMENUBARINFO *pmbi)
{
	if (InitAygshell() && pSHCreateMenuBar != NULL) {
		BOOL res = pSHCreateMenuBar(pmbi);
		return res;
	}
	return FALSE;
}

BOOL SHInitDialog(PSHINITDLGINFO pshidi)
{
	if (InitAygshell() && pSHInitDialog != NULL)
		return pSHInitDialog(pshidi);
	return FALSE;
}

BOOL SHInitExtraControls(void)
{
	if (InitAygshell() && pSHInitExtraControls != NULL)
		return pSHInitExtraControls();
	return FALSE;
}
