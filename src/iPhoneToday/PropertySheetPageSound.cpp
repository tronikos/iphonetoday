//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageSound.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

void EnableAllDlgItemsSound(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_LAUNCH_SOUND),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_LAUNCH_SOUND),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_LAUNCH_SOUND_PLAY),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_PRESSED_SOUND),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_PRESSED_SOUND),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_PRESSED_SOUND_PLAY),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_FLIP_SOUND),				bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_FLIP_SOUND),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_FLIP_SOUND_PLAY),		bEnable);
}

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogSound(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_SOUND);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ENABLE_SOUNDS), BM_SETCHECK, configuracion->soundsEnabled ? BST_CHECKED : BST_UNCHECKED, 0);

			SetDlgItemText(hDlg, IDC_EDIT_LAUNCH_SOUND,		configuracion->soundOnLaunchIcon);
			SetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND,	configuracion->pressed_sound);
			SetDlgItemText(hDlg, IDC_EDIT_FLIP_SOUND,		configuracion->change_screen_sound);

			EnableAllDlgItemsSound(hDlg, configuracion->soundsEnabled);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			case IDC_CHECK_ENABLE_SOUNDS:
				EnableAllDlgItemsSound(hDlg, SendMessage(GetDlgItem(hDlg, IDC_CHECK_ENABLE_SOUNDS), BM_GETCHECK, 0, 0) == BST_CHECKED);
				break;
			case IDC_BUTTON_LAUNCH_SOUND:
				GetDlgItemText(hDlg, IDC_EDIT_LAUNCH_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_LAUNCH_SOUND, str);
				}
				break;
			case IDC_BUTTON_LAUNCH_SOUND_PLAY:
				GetDlgItemText(hDlg, IDC_EDIT_LAUNCH_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				break;
			case IDC_BUTTON_PRESSED_SOUND:
				GetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND, str);
				}
				break;
			case IDC_BUTTON_PRESSED_SOUND_PLAY:
				GetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				break;
			case IDC_BUTTON_FLIP_SOUND:
				GetDlgItemText(hDlg, IDC_EDIT_FLIP_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_FLIP_SOUND, str);
				}
				break;
			case IDC_BUTTON_FLIP_SOUND_PLAY:
				GetDlgItemText(hDlg, IDC_EDIT_FLIP_SOUND, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				break;
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_SOUND, uMsg, wParam, lParam);
}

/*BOOL IsValidConfigurationSound(HWND hDlg)
{
	return TRUE;
}*/

BOOL SaveConfigurationSound(HWND hDlg)
{
//	if (!IsValidConfigurationSound(hDlg)) return FALSE;

	configuracion->soundsEnabled = SendMessage(GetDlgItem(hDlg, IDC_CHECK_ENABLE_SOUNDS), BM_GETCHECK, 0, 0) == BST_CHECKED;

	GetDlgItemText(hDlg, IDC_EDIT_LAUNCH_SOUND,	configuracion->soundOnLaunchIcon,	CountOf(configuracion->soundOnLaunchIcon));
	GetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND,configuracion->pressed_sound,		CountOf(configuracion->pressed_sound));
	GetDlgItemText(hDlg, IDC_EDIT_FLIP_SOUND,	configuracion->change_screen_sound,	CountOf(configuracion->change_screen_sound));

	return TRUE;
}
