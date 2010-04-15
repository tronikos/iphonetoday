//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageAnimation.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogAnimation(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_ANIMATION);

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"None");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Expand rectangle from center");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Zoom out from center");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Zoom in to center");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Expand rectangle from clicked area");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Zoom out from clicked area");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_ADDSTRING, 0, (LPARAM)L"Zoom in to clicked area");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_SETCURSEL, configuracion->animationEffect, 0);

			SetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR,		configuracion->animationColor);
			SetDlgItemInt(hDlg, IDC_EDIT_ANIMATION_TIME,	configuracion->animationDuration, TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_ANIMATION_LAUNCHAPP), BM_SETCHECK, configuracion->launchAppAtBeginningOfAnimation ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ANIM_COLOR:
			int rgbCurrent;
			COLORREF nextColor;
			rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, NULL);
			if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
				SetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, nextColor);
			}
			break;
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_ANIMATION, uMsg, wParam, lParam);
}

BOOL IsValidConfigurationAnimation(HWND hDlg)
{
	int animationDuration = GetDlgItemInt(hDlg, IDC_EDIT_ANIMATION_TIME, NULL, TRUE);

	if (animationDuration < 0 || animationDuration > 10000) {
		MessageBox(hDlg, TEXT("Animation time value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfigurationAnimation(HWND hDlg)
{
	if (!IsValidConfigurationAnimation(hDlg)) return FALSE;

	configuracion->animationEffect	= SendMessage(GetDlgItem(hDlg, IDC_COMBO_ANIMATION), CB_GETCURSEL, 0, 0);
	configuracion->animationColor	= GetDlgItemHex(hDlg, IDC_EDIT_ANIM_COLOR, NULL);
	configuracion->animationDuration= GetDlgItemInt(hDlg, IDC_EDIT_ANIMATION_TIME, NULL, TRUE);
	configuracion->launchAppAtBeginningOfAnimation = SendMessage(GetDlgItem(hDlg, IDC_CHECK_ANIMATION_LAUNCHAPP), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
