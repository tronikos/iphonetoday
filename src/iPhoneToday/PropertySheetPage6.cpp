//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage6.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog6(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 6);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN),				BM_SETCHECK, configuracion->fullscreen					? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR),		BM_SETCHECK, configuracion->neverShowTaskBar			? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE),		BM_SETCHECK, configuracion->noWindowTitle				? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT),				BM_SETCHECK, configuracion->showExit					? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_SETCHECK, configuracion->disableRightClick			? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_SETCHECK, configuracion->ignoreRotation				? BST_CHECKED : BST_UNCHECKED, 0);

			SetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER,	configuracion->notifyTimer,	TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_HEIGHTP,		configuracion->heightP,		TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_HEIGHTL,		configuracion->heightL,		TRUE);

#ifdef EXEC_MODE
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HEIGHTP), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HEIGHTL), FALSE);
#else
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT), FALSE);
#endif
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_NEVER_SHOW_TASKBAR:
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				int resp = MessageBox(hDlg,
					L"Are you really sure you never want to show the taskbar?\n"
					L"This option is meant for Pocket Navigation Devices.\n"
					L"The fullscreen option already hides the taskbar.",
					L"Are you sure?", MB_YESNO);
				if (resp == IDNO) {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}
			break;
		case IDC_EDIT_HEIGHTP:
		case IDC_EDIT_HEIGHTL:
			if (HIWORD(wParam) == EN_SETFOCUS) {
				static BOOL displayed_tip = FALSE;
				if (!displayed_tip) {
					displayed_tip = TRUE;
					MessageBox(hDlg, L"It is recommended to leave this 0 to automatically size the today item.", L"Tip", MB_OK);
				}
			}
			break;
		}
		return 0;
	}

	return DefOptionWindowProc(hDlg, 6, uMsg, wParam, lParam);
}

BOOL IsValidConfiguration6(HWND hDlg)
{
	int notifyTimer, heightP, heightL;

	notifyTimer = GetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER, NULL, TRUE);
	heightP = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTP, NULL, TRUE);
	heightL = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTL, NULL, TRUE);

	if (notifyTimer < 0) {
		MessageBox(hDlg, TEXT("Notify timer value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (heightP < 0 || heightP > 1000 || heightL < 0 || heightL > 1000) {
		MessageBox(hDlg, TEXT("Height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfiguration6(HWND hDlg)
{
	if (!IsValidConfiguration6(hDlg)) return FALSE;

	int notifyTimer, heightP, heightL;

	notifyTimer = GetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER, NULL, TRUE);
	heightP = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTP, NULL, TRUE);
	heightL = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTL, NULL, TRUE);

	configuracion->fullscreen			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN),			BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->neverShowTaskBar		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->noWindowTitle		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->showExit				= SendMessage(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT),			BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->disableRightClick	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->ignoreRotation		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_GETCHECK, 0, 0) == BST_CHECKED;

	configuracion->notifyTimer	= notifyTimer;
	configuracion->heightP		= heightP;
	configuracion->heightL		= heightL;

	return TRUE;
}
