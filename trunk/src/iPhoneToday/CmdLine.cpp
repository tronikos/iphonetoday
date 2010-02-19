#include "CmdLine.h"
#include "stdafx.h"

BOOL CommandLineArguements(HWND hwnd, LPCTSTR pCmdLine)
{
	if (pCmdLine == NULL) {
		return FALSE;
	}

	if (wcsicmp(pCmdLine, L"reload") == 0) {
		SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, 0, NULL);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"relaunch") == 0) {
		PostMessage(hwnd, WM_USER_RELAUNCH, 0, 0);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"add") == 0) {
		PostMessage(hwnd, WM_USER_ADD, 0, 0);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"options") == 0) {
		PostMessage(hwnd, WM_USER_OPTIONS, 0, 0);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"close") == 0 || wcsicmp(pCmdLine, L"exit") == 0) {
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		return TRUE;
	} else if (_wcsnicmp(pCmdLine, L"goto:", wcslen(L"goto:")) == 0) {
		int l = wcslen(L"goto:");
		if (wcscmp(pCmdLine + l, L"next") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_NEXT, 0, 0);
		} else if (wcscmp(pCmdLine + l, L"previous") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_PREV, 0, 0);
		} else if (wcscmp(pCmdLine + l, L"up") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_UP, 0, 0);
		} else if (wcscmp(pCmdLine + l, L"down") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_DOWN, 0, 0);
		} else {
			PostMessage(hwnd, WM_USER_GOTO, _wtoi(pCmdLine + l), 0);
		}
		return TRUE;
	}

	return FALSE;
}
