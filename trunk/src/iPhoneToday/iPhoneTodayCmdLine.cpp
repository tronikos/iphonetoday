#include <windows.h>
#include "CmdLine.h"

// Find a window under parent with the specified title.
HWND FindChildWindow(HWND parent, TCHAR *title) {
	if (parent == NULL || title == NULL) {
		return NULL;
	}
	TCHAR tmp[100];
	if (GetWindowText(parent, tmp, 100)) {
		if (wcsicmp(tmp, title) == 0) {
			return parent;
		}
	}
	HWND child = GetWindow(parent, GW_CHILD);
	while (child != NULL) {
		HWND h = FindChildWindow(child, title);
		if (h) {
			return h;
		}
		child = GetWindow(child, GW_HWNDNEXT);
	}
	return NULL;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (_tcsclen(lpCmdLine) > 0) {
		if (wcsncmp(lpCmdLine, L"--", 2) == 0) {
			// Find iPhoneToday window that should be somewhere under the Desktop window...
			HWND hwnd = FindChildWindow(GetDesktopWindow(), L"iPhoneToday");
			CommandLineArguements(hwnd, lpCmdLine + 2);
		}
	}

	return 0;
}
