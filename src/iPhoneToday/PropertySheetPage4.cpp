//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage4.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog4(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 4);

			SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF,		configuracion->bubble_notif);
			SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE,		configuracion->bubble_state);
			SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM,		configuracion->bubble_alarm);
		}
		return TRUE;
	case WM_COMMAND:
		{
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_BUBBLE_NOTIF:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, str);
				}
				break;
			case IDC_BUTTON_BUBBLE_STATE:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, str);
				}
				break;
			case IDC_BUTTON_BUBBLE_ALARM:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, str);
				}
				break;
			}
		}
		return 0;
	case WM_PAINT:
		PaintOptionsDialog(hDlg, 4);
		return 0;
	case WM_NOTIFY:
		if (((LPNMHDR) lParam)->code == PSN_HELP) {
			ToggleKeyboard();
			return 0;
		}
		break;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration4(HWND hDlg)
{
	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, configuracion->bubble_notif, CountOf(configuracion->bubble_notif));
	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, configuracion->bubble_state, CountOf(configuracion->bubble_state));
	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, configuracion->bubble_alarm, CountOf(configuracion->bubble_alarm));

	return TRUE;
}
