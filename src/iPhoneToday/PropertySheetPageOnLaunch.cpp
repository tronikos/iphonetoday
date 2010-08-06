//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageOnLaunch.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogOnLaunch(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_ONLAUNCH);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE),		BM_SETCHECK, configuracion->closeOnLaunchIcon			? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_MINIMIZE),	BM_SETCHECK, configuracion->minimizeOnLaunchIcon		? BST_CHECKED : BST_UNCHECKED, 0);

			SetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE,	configuracion->vibrateOnLaunchIcon, TRUE);
			SetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON,		configuracion->pressed_icon);

#ifndef EXEC_MODE
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_MINIMIZE), FALSE);
#endif
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			case IDC_BUTTON_PRESSED_ICON:
				GetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON, str);
				}
				break;
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_ONLAUNCH, uMsg, wParam, lParam);
}

/*BOOL IsValidConfigurationOnLaunch(HWND hDlg)
{
	int vibrateOnLaunchIcon;

	vibrateOnLaunchIcon = GetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE, NULL, TRUE);

	if (vibrateOnLaunchIcon < 0 || vibrateOnLaunchIcon > 500) {
		MessageBox(hDlg, TEXT("Vibrate on launch value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}*/

BOOL SaveConfigurationOnLaunch(HWND hDlg)
{
//	if (!IsValidConfigurationOnLaunch(hDlg)) return FALSE;

	int vibrateOnLaunchIcon;

	vibrateOnLaunchIcon = GetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE, NULL, TRUE);

	configuracion->vibrateOnLaunchIcon = vibrateOnLaunchIcon;

	configuracion->closeOnLaunchIcon			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->minimizeOnLaunchIcon			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_MINIMIZE),	BM_GETCHECK, 0, 0) == BST_CHECKED;

	GetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON,	configuracion->pressed_icon,		CountOf(configuracion->pressed_icon));

	return TRUE;
}
