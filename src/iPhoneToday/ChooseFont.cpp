#include "ChooseFont.h"
#include "OptionDialog.h"

ChooseFontSettings cfs;
BOOL bChooseFontOK;

int CALLBACK EnumFontFamiliesProc(ENUMLOGFONT *lpelf, TEXTMETRIC *lpntm, int FontType, LPARAM lParam)
{
	SendMessage((HWND) lParam, CB_ADDSTRING, 0, (LPARAM) lpelf->elfFullName);
	return 1;
}

LRESULT CALLBACK ChooseFontProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CAPTIONOKBTN);
			if (FindWindow(L"MS_SIPBUTTON", NULL) == NULL) {
				SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CONTEXTHELP);
			}

			EnumFontFamilies(GetDC(NULL), NULL, (FONTENUMPROC)EnumFontFamiliesProc, (LPARAM)GetDlgItem(hDlg, IDC_COMBO_FONT_FACENAME));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"000 Don't care");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"100 Thin");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"200 Extra light");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"300 Light");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"400 Normal");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"500 Medium");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"600 Semi bold");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"700 Bold");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"800 Extra bold");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_FONT_WEIGHT), CB_ADDSTRING, 0, (LPARAM) L"900 Heavy");

			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_HEIGHT), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_EDIT_FONT_HEIGHT), 0);
			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_HEIGHT), UDM_SETRANGE, 0, MAKELPARAM(0, 100));

			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_SHADOW), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_EDIT_FONT_SHADOW), 0);
			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_SHADOW), UDM_SETRANGE, 0, MAKELPARAM(0, 5));

			SetDlgItemText(hDlg, IDC_COMBO_FONT_FACENAME, cfs.facename);
			SetDlgItemHex(hDlg, IDC_EDIT_FONT_COLOR, cfs.color);
			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_HEIGHT), UDM_SETPOS, 0, cfs.height);
			SetDlgItemInt(hDlg, IDC_COMBO_FONT_WEIGHT, cfs.weight, TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_SHADOW), UDM_SETPOS, 0, cfs.shadow);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_FONT_ROUNDRECT), BM_SETCHECK, cfs.roundrect ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				TCHAR tmp[4];
				bChooseFontOK = TRUE;
				GetDlgItemText(hDlg, IDC_COMBO_FONT_FACENAME, cfs.facename, LF_FACESIZE);
				cfs.color = GetDlgItemHex(hDlg, IDC_EDIT_FONT_COLOR, NULL);
				cfs.height = SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_HEIGHT), UDM_GETPOS, 0, 0);
				//cfs.weight = GetDlgItemInt(hDlg, IDC_COMBO_FONT_WEIGHT, NULL, TRUE);
				GetDlgItemText(hDlg, IDC_COMBO_FONT_WEIGHT, tmp, 4);
				cfs.weight = _wtoi(tmp);
				cfs.shadow = SendMessage(GetDlgItem(hDlg, IDC_SPIN_FONT_SHADOW), UDM_GETPOS, 0, 0);
				cfs.roundrect = SendMessage(GetDlgItem(hDlg, IDC_CHECK_FONT_ROUNDRECT), BM_GETCHECK, 0, 0) == BST_CHECKED;
				EndDialog(hDlg, LOWORD(wParam));
			}
			break;
		case IDCANCEL:
			bChooseFontOK = FALSE;
			EndDialog(hDlg, LOWORD(wParam));
			break;
		case IDC_BUTTON_FONT_COLOR:
			{
				int rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_FONT_COLOR, NULL);
				COLORREF nextColor;
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_FONT_COLOR, nextColor);
				}
			}
			break;
		}
		break;
	case WM_HELP:
		ToggleKeyboard();
		break;
	}
	return FALSE;
}
