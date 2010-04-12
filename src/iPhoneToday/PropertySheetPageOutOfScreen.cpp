//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageOutOfScreen.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

OutOfScreenSettings ooss_left, ooss_right, ooss_up, ooss_down;
OutOfScreenSettings *cur_ooss;

void ooss_enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_STOP),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDITOUTOFSCREEN_STOPAT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_OUTOFSCREEN_EXEC),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_WRAP),	bEnable);
	if (bEnable) {
		BOOL checked = SendMessage(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_STOP), BM_GETCHECK, 0, 0) == BST_CHECKED;
		EnableWindow(GetDlgItem(hDlg, IDC_EDITOUTOFSCREEN_STOPAT),	checked);
		EnableWindow(GetDlgItem(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC),	!checked);
		EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_OUTOFSCREEN_EXEC),	!checked);
		EnableWindow(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_WRAP),	!checked);
	}
}

void ooss_load(HWND hDlg, OutOfScreenSettings *ooss)
{
	if (ooss != NULL) {
		SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC, ooss->exec);
		SetDlgItemInt(hDlg, IDC_EDITOUTOFSCREEN_STOPAT, ooss->stopAt, TRUE);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_STOP), BM_SETCHECK, ooss->stop ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_WRAP), BM_SETCHECK, ooss->wrap ? BST_CHECKED : BST_UNCHECKED, 0);

		ooss_enable(hDlg, TRUE);
	}
}

void ooss_save(HWND hDlg, OutOfScreenSettings *ooss)
{
	if (ooss != NULL) {
		GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC, ooss->exec, CountOf(ooss->exec));
		ooss->stopAt = GetDlgItemInt(hDlg, IDC_EDITOUTOFSCREEN_STOPAT, NULL, TRUE);
		ooss->stop = SendMessage(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_STOP), BM_GETCHECK, 0, 0) == BST_CHECKED;
		ooss->wrap = SendMessage(GetDlgItem(hDlg, IDC_CHECK_OUTOFSCREEN_WRAP), BM_GETCHECK, 0, 0) == BST_CHECKED;
	}
}

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogOutOfScreen(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_OUTOFSCREEN);

			cur_ooss = NULL;
			ooss_enable(hDlg, FALSE);

			memcpy(&ooss_left,  &configuracion->ooss_left,  sizeof(OutOfScreenSettings));
			memcpy(&ooss_right, &configuracion->ooss_right, sizeof(OutOfScreenSettings));
			memcpy(&ooss_up,    &configuracion->ooss_up,    sizeof(OutOfScreenSettings));
			memcpy(&ooss_down,  &configuracion->ooss_down,  sizeof(OutOfScreenSettings));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_OUTOFSCREEN), CB_ADDSTRING, 0, (LPARAM)L"Left");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_OUTOFSCREEN), CB_ADDSTRING, 0, (LPARAM)L"Right");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_OUTOFSCREEN), CB_ADDSTRING, 0, (LPARAM)L"Up");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_OUTOFSCREEN), CB_ADDSTRING, 0, (LPARAM)L"Down");
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO_OUTOFSCREEN:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				ooss_save(hDlg, cur_ooss);
				TCHAR str[MAX_PATH];
				GetDlgItemText(hDlg, IDC_COMBO_OUTOFSCREEN, str, MAX_PATH);
				if (lstrcmpi(str, L"Left") == 0) {
					cur_ooss = &ooss_left;
					ooss_load(hDlg, cur_ooss);
				} else if (lstrcmpi(str, L"Right") == 0) {
					cur_ooss = &ooss_right;
					ooss_load(hDlg, cur_ooss);
				} else if (lstrcmpi(str, L"Up") == 0) {
					cur_ooss = &ooss_up;
					ooss_load(hDlg, cur_ooss);
				} else if (lstrcmpi(str, L"Down") == 0) {
					cur_ooss = &ooss_down;
					ooss_load(hDlg, cur_ooss);
				} else {
					ooss_enable(hDlg, FALSE);
				}
			}
			break;
		case IDC_BUTTON_OUTOFSCREEN_EXEC:
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC, str, MAX_PATH);
			configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
			getPathFromFile(fullPath, browseDir);
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, fullPath, browseDir)) {
				configuracion->getRelativePath(str, MAX_PATH, fullPath);
				SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_EXEC, str);
			}
			break;
		case IDC_CHECK_OUTOFSCREEN_STOP:
			ooss_enable(hDlg, TRUE);
			break;
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_OUTOFSCREEN, uMsg, wParam, lParam);
}

BOOL ooss_check(HWND hDlg, OutOfScreenSettings *ooss)
{
	if (ooss->stopAt < 0 || ooss->stopAt > 100) {
		MessageBox(hDlg, TEXT("Out of screen stop at value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL IsValidConfigurationOutOfScreen(HWND hDlg)
{
	ooss_save(hDlg, cur_ooss);

	if (!ooss_check(hDlg, &ooss_left))  return FALSE;
	if (!ooss_check(hDlg, &ooss_right)) return FALSE;
	if (!ooss_check(hDlg, &ooss_up))    return FALSE;
	if (!ooss_check(hDlg, &ooss_down))  return FALSE;

	return TRUE;
}

BOOL SaveConfigurationOutOfScreen(HWND hDlg)
{
	ooss_save(hDlg, cur_ooss);

	if (!IsValidConfigurationOutOfScreen(hDlg)) return FALSE;

	memcpy(&configuracion->ooss_left,  &ooss_left,  sizeof(OutOfScreenSettings));
	memcpy(&configuracion->ooss_right, &ooss_right, sizeof(OutOfScreenSettings));
	memcpy(&configuracion->ooss_up,    &ooss_up,    sizeof(OutOfScreenSettings));
	memcpy(&configuracion->ooss_down,  &ooss_down,  sizeof(OutOfScreenSettings));

	return TRUE;
}
