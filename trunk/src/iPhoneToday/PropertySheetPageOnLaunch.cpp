//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage7.cpp : Defines the property sheet page procedures.
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
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_ANIMATE),	BM_SETCHECK, configuracion->allowAnimationOnLaunchIcon	? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_SOUND),		BM_SETCHECK, configuracion->allowSoundOnLaunchIcon		? BST_CHECKED : BST_UNCHECKED, 0);

			SetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE,	configuracion->vibrateOnLaunchIcon, TRUE);
			SetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR,		configuracion->colorOfAnimationOnLaunchIcon);
			SetDlgItemText(hDlg, IDC_EDIT_WAV,				configuracion->soundOnLaunchIcon);
			SetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON,		configuracion->pressed_icon);
			SetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND,	configuracion->pressed_sound);

#ifndef EXEC_MODE
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE), FALSE);
#endif
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			case IDC_BUTTON_WAV:
				GetDlgItemText(hDlg, IDC_EDIT_WAV, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_WAV, str);
				}
				break;
			case IDC_BUTTON_WAV_PLAY:
				GetDlgItemText(hDlg, IDC_EDIT_WAV, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				break;
			case IDC_BUTTON_ANIM_COLOR:
				int rgbCurrent;
				COLORREF nextColor;
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_PRESSED_ICON:
				GetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON, str);
				}
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
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_ONLAUNCH, uMsg, wParam, lParam);
}

BOOL IsValidConfigurationOnLaunch(HWND hDlg)
{
	int vibrateOnLaunchIcon;

	vibrateOnLaunchIcon = GetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE, NULL, TRUE);

	if (vibrateOnLaunchIcon < 0 || vibrateOnLaunchIcon > 500) {
		MessageBox(hDlg, TEXT("Vibrate on launch value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfigurationOnLaunch(HWND hDlg)
{
	if (!IsValidConfigurationOnLaunch(hDlg)) return FALSE;

	int vibrateOnLaunchIcon;

	vibrateOnLaunchIcon = GetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE, NULL, TRUE);

	configuracion->vibrateOnLaunchIcon = vibrateOnLaunchIcon;

	configuracion->closeOnLaunchIcon			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->allowAnimationOnLaunchIcon	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_ANIMATE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->allowSoundOnLaunchIcon		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_SOUND),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->colorOfAnimationOnLaunchIcon	= GetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, NULL);

	GetDlgItemText(hDlg, IDC_EDIT_WAV,			configuracion->soundOnLaunchIcon,	CountOf(configuracion->soundOnLaunchIcon));
	GetDlgItemText(hDlg, IDC_EDIT_PRESSED_ICON,	configuracion->pressed_icon,		CountOf(configuracion->pressed_icon));
	GetDlgItemText(hDlg, IDC_EDIT_PRESSED_SOUND,configuracion->pressed_sound,		CountOf(configuracion->pressed_sound));

	return TRUE;
}
