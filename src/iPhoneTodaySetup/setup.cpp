#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "setup.h"

HINSTANCE g_hinstModule;


BOOL APIENTRY DllMain(
    HANDLE hModule, 
    DWORD  ul_reason_for_call, 
    LPVOID lpReserved
    )
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			g_hinstModule = (HINSTANCE) hModule;
			break;
	}
	return TRUE;
}

void RemoveRegistry() {
	HKEY myKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Today\\Items", 0, 0, &myKey) == ERROR_SUCCESS) {
		RegDeleteKey(myKey, L"iPhoneToday");
		RegCloseKey(myKey);
	}
}

void ToggleTodayPlugin(BOOL bEnable)
{
	HKEY myKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Today\\Items\\iPhoneToday", 0, 0, &myKey) == ERROR_SUCCESS) {
		DWORD dwValue = bEnable;
		RegSetValueEx(myKey, L"Enabled", 0, REG_DWORD, (BYTE *)&dwValue, sizeof(DWORD));
		RegCloseKey(myKey);

		// Refresh Today Screen
		::SendMessage(::GetDesktopWindow(), WM_WININICHANGE, 0xF2, 0);
	}
}

void DisableAllTodayPlugins()
{
	HKEY hKeyToday;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Today", 0, 0, &hKeyToday) == ERROR_SUCCESS) {
		DWORD dwValue = 0;
		RegSetValueEx(hKeyToday, L"Date", 0, REG_DWORD, (BYTE *)&dwValue, sizeof(DWORD));
		RegCloseKey(hKeyToday);

		HKEY hKeyItems;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Today\\Items", 0, 0, &hKeyItems) == ERROR_SUCCESS) {
			DWORD index = 0;
			TCHAR itemName[MAX_PATH];
			DWORD itemNameLen = sizeof(itemName) / sizeof(itemName[0]);
			while (RegEnumKeyEx(hKeyItems, index, itemName, &itemNameLen, 0, 0, 0, 0) == ERROR_SUCCESS) {
				TCHAR subkey[MAX_PATH];
				wsprintf(subkey, L"Software\\Microsoft\\Today\\Items\\%s", itemName);
				HKEY hKeyItem;
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, 0, &hKeyItem) == ERROR_SUCCESS) {
					DWORD dwValue;
					DWORD dwType;
					DWORD dwSize = sizeof(DWORD);
					if (RegQueryValueEx(hKeyItem, L"Enabled", 0, &dwType, (BYTE *)&dwValue, &dwSize) == ERROR_SUCCESS) {
						if (dwType == REG_DWORD && dwValue == 1) {
							dwValue = 0;
							RegSetValueEx(hKeyItem, L"Enabled", 0, REG_DWORD, (BYTE *)&dwValue, sizeof(DWORD));
						}
					}
					RegCloseKey(hKeyItem);
				}
				index++;
				itemNameLen = sizeof(itemName) / sizeof(itemName[0]);
			}
			RegCloseKey(hKeyItems);
		}

		// Refresh Today Screen
		::SendMessage(::GetDesktopWindow(), WM_WININICHANGE, 0xF2, 0);
	}
}

// **************************************************************************
// **************************************************************************
// Function Name: Install_Init
// 
// Purpose: processes the push message.
//
// Arguments:
//    IN HWND hwndParent  handle to the parent window
//    IN BOOL fFirstCall  indicates that this is the first time this function is being called
//    IN BOOL fPreviouslyInstalled  indicates that the current application is already installed
//    IN LPCTSTR pszInstallDir  name of the user-selected install directory of the application
//
// Return Values:
//    codeINSTALL_INIT
//    returns install status
//
// Description:  
//    The Install_Init function is called before installation begins.
//    User will be prompted to confirm installation.
// **************************************************************************
SETUP_API codeINSTALL_INIT Install_Init(
    HWND        hwndParent,
    BOOL        fFirstCall,     // is this the first time this function is being called?
    BOOL        fPreviouslyInstalled,
    LPCTSTR     pszInstallDir
    )
{
	// Not going to use fPreviouslyInstalled because it produce some unexpected behavior like program unsuccessfully installed

	if (fFirstCall) {  // Disable the plug-in once will do
		ToggleTodayPlugin(FALSE);
		RemoveRegistry();
	}

	return codeINSTALL_INIT_CONTINUE;
}



// **************************************************************************
// Function Name: Install_Exit
// 
// Purpose: processes the push message.
//
// Arguments:
//    IN HWND hwndParent  handle to the parent window
//    IN LPCTSTR pszInstallDir  name of the user-selected install directory of the application
//
// Return Values:
//    codeINSTALL_EXIT
//    returns install status
//
// Description:  
//    Register query client with the PushRouter as part of installation.
//    Only the first two parameters really count.
// **************************************************************************
SETUP_API codeINSTALL_EXIT Install_Exit(
    HWND    hwndParent,
    LPCTSTR pszInstallDir,      // final install directory
    WORD    cFailedDirs,
    WORD    cFailedFiles,
    WORD    cFailedRegKeys,
    WORD    cFailedRegVals,
    WORD    cFailedShortcuts
    )
{
	if (MessageBox(hwndParent, L"Would you like to enable today plugin?", L"Installation", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		DisableAllTodayPlugins();
		ToggleTodayPlugin(TRUE);
	}

/*
	//show message to user
	wsprintf(InstallInfo, _T("MyToday Plugin has been installed and is now available on your todayscreen."));
	MessageBox(hwndParent, InstallInfo, _T("Setup"), MB_OK | MB_ICONASTERISK);
*/
	return codeINSTALL_EXIT_DONE;
}



// **************************************************************************
// Function Name: Uninstall_Init
// 
// Purpose: processes the push message.
//
// Arguments:
//    IN HWND hwndParent  handle to the parent window
//    IN LPCTSTR pszInstallDir  name of the user-selected install directory of the application
//
// Return Values:
//    codeUNINSTALL_INIT
//    returns uninstall status
//
// Description:  
//    Query the device data using the query xml in the push message,
//    and send the query results back to the server.
// **************************************************************************
SETUP_API codeUNINSTALL_INIT Uninstall_Init(
    HWND        hwndParent,
    LPCTSTR     pszInstallDir
    )
{
	ToggleTodayPlugin(FALSE);
	RemoveRegistry();

	return codeUNINSTALL_INIT_CONTINUE;
}



// **************************************************************************
// Function Name: Uninstall_Exit
// 
// Purpose: processes the push message.
//
// Arguments:
//    IN HWND hwndParent  handle to the parent window
//
// Return Values:
//    codeUNINSTALL_EXIT
//    returns uninstall status
//
// Description:  
//    Query the device data using the query xml in the push message,
//    and send the query results back to the server.
// **************************************************************************
SETUP_API codeUNINSTALL_EXIT Uninstall_Exit(
    HWND    hwndParent
    )
{
/*
	TCHAR InstallInfo[255];

	wsprintf(InstallInfo, _T("MyToday Plugin has been removed."));
	MessageBox(hwndParent, InstallInfo, _T("Setup"), MB_OK | MB_ICONWARNING);
*/

    return codeUNINSTALL_EXIT_DONE;
}
