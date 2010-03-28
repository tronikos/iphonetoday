#include "StdAfx.h"

#include "regext.h"

typedef DWORD (STDAPICALLTYPE FAR fSHRecognizeGesture)(SHRGINFO*); 
typedef BOOL (STDAPICALLTYPE FAR fGetOpenFileNameEx)(LPOPENFILENAMEEX); 
typedef BOOL (STDAPICALLTYPE FAR fSHFullScreen)(HWND,DWORD);
typedef BOOL (STDAPICALLTYPE FAR fSHCreateMenuBar)(SHMENUBARINFO*);
typedef BOOL (STDAPICALLTYPE FAR fSHInitDialog)(PSHINITDLGINFO); 
typedef BOOL (STDAPICALLTYPE FAR fSHInitExtraControls)(); 
typedef HRESULT (STDAPICALLTYPE FAR fRegistryNotifyWindow)(HKEY,LPCTSTR,LPCTSTR,HWND,UINT,DWORD,NOTIFICATIONCONDITION*,HREGNOTIFY*);
typedef HRESULT (STDAPICALLTYPE FAR fRegistryCloseNotification)(HREGNOTIFY);

static fSHRecognizeGesture			*pSHRecognizeGesture = NULL;
static fGetOpenFileNameEx			*pGetOpenFileNameEx = NULL;
static fSHFullScreen				*pSHFullScreen = NULL;
static fSHCreateMenuBar				*pSHCreateMenuBar = NULL;
static fSHInitDialog				*pSHInitDialog = NULL;
static fSHInitExtraControls			*pSHInitExtraControls = NULL;
static fRegistryNotifyWindow		*pRegistryNotifyWindow = NULL;
static fRegistryCloseNotification	*pRegistryCloseNotification = NULL;

BOOL InitAygshell()
{
	static HMODULE hAygshellLib = NULL;
	if (hAygshellLib != NULL)
		return TRUE;
	hAygshellLib = LoadLibrary(L"aygshell.dll");
	if (hAygshellLib == NULL)
		return FALSE;
	pSHRecognizeGesture			= (fSHRecognizeGesture*)		GetProcAddress(hAygshellLib, L"SHRecognizeGesture");
	pGetOpenFileNameEx			= (fGetOpenFileNameEx*)			GetProcAddress(hAygshellLib, L"GetOpenFileNameEx");
	pSHFullScreen				= (fSHFullScreen*)				GetProcAddress(hAygshellLib, L"SHFullScreen");
	pSHCreateMenuBar			= (fSHCreateMenuBar*)			GetProcAddress(hAygshellLib, L"SHCreateMenuBar");
	pSHInitDialog				= (fSHInitDialog*)				GetProcAddress(hAygshellLib, L"SHInitDialog");
	pSHInitExtraControls		= (fSHInitExtraControls*)		GetProcAddress(hAygshellLib, L"SHInitExtraControls");
	pRegistryNotifyWindow		= (fRegistryNotifyWindow*)		GetProcAddress(hAygshellLib, L"RegistryNotifyWindow");
	pRegistryCloseNotification	= (fRegistryCloseNotification*)	GetProcAddress(hAygshellLib, L"RegistryCloseNotification");
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

HRESULT WINAPI RegistryNotifyWindow(HKEY hKey,
                                    LPCTSTR pszSubKey,
                                    LPCTSTR pszValueName,
                                    HWND hWnd,
                                    UINT msg,
                                    DWORD dwUserData,
                                    NOTIFICATIONCONDITION * pCondition,
                                    HREGNOTIFY * phNotify)
{
	if (InitAygshell() && pRegistryNotifyWindow != NULL)
		return pRegistryNotifyWindow(hKey, pszSubKey, pszValueName, hWnd, msg, dwUserData, pCondition, phNotify);
	return S_FALSE;
}

HRESULT WINAPI RegistryCloseNotification(HREGNOTIFY hNotify)
{
	if (InitAygshell() && pRegistryCloseNotification != NULL)
		return pRegistryCloseNotification(hNotify);
	return S_FALSE;
}
