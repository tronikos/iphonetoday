#include "stdafx.h"
#include "CmdLine.h"
#include <Pm.h>

BOOL CommandLineArguements(HWND hwnd, LPCTSTR pCmdLine)
{
	if (pCmdLine == NULL) {
		return FALSE;
	}

	if (wcsicmp(pCmdLine, L"reload") == 0) {
		PostMessage(hwnd, WM_TIMER, TIMER_ACTUALIZA_NOTIF, 0);
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
#ifdef EXEC_MODE
	} else if (wcsicmp(pCmdLine, L"close") == 0 || wcsicmp(pCmdLine, L"exit") == 0) {
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"minimize") == 0) {
		ShowWindow(hwnd, SW_MINIMIZE);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"taskbar") == 0) {
		HWND hWndTaskbar = FindWindow(L"HHTaskBar", NULL);
		if (hWndTaskbar != NULL) {
			ShowWindow(hWndTaskbar, IsWindowVisible(hWndTaskbar) ? SW_HIDE : SW_SHOW);
		}
		return TRUE;
#endif
	} else if (_wcsnicmp(pCmdLine, L"goto:", wcslen(L"goto:")) == 0) {
		int l = wcslen(L"goto:");
		if (wcsicmp(pCmdLine + l, L"next") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_NEXT, 0, 0);
		} else if (wcsicmp(pCmdLine + l, L"previous") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_PREV, 0, 0);
		} else if (wcsicmp(pCmdLine + l, L"up") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_UP, 0, 0);
		} else if (wcsicmp(pCmdLine + l, L"down") == 0) {
			PostMessage(hwnd, WM_USER_GOTO_DOWN, 0, 0);
		} else {
			PostMessage(hwnd, WM_USER_GOTO, _wtoi(pCmdLine + l), 0);
		}
		return TRUE;
	} else if (_wcsnicmp(pCmdLine, L"volume:", wcslen(L"volume:")) == 0) {
		int l = wcslen(L"volume:");
		WORD vol;
		if (*(pCmdLine + l) == '+') {
			vol = GetVolumePercentage() + (WORD) _wtoi(pCmdLine + l + 1);
		} else if (*(pCmdLine + l) == '-') {
			vol = GetVolumePercentage() - (WORD) _wtoi(pCmdLine + l + 1);
		} else {
			vol = (WORD) _wtoi(pCmdLine + l);
		}
		SetVolumePercentage(vol);
		PostMessage(hwnd, WM_TIMER, TIMER_ACTUALIZA_NOTIF, 0);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"rotate") == 0) {
		Rotate(90);
		return TRUE;
	} else if (_wcsnicmp(pCmdLine, L"rotate:", wcslen(L"rotate:")) == 0) {
		int l = wcslen(L"rotate:");
		Rotate(_wtoi(pCmdLine + l));
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"reset") == 0 || wcsicmp(pCmdLine, L"restart") == 0) {
		SetSystemPowerState(NULL, POWER_STATE_RESET, POWER_FORCE);
		return TRUE;
	} else if (wcsicmp(pCmdLine, L"off") == 0 || wcsicmp(pCmdLine, L"standby") == 0 || wcsicmp(pCmdLine, L"suspend") == 0) {
		SetSystemPowerState(NULL, POWER_STATE_SUSPEND, POWER_FORCE);
		return TRUE;
	}

	return FALSE;
}
