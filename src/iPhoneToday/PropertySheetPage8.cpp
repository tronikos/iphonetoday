//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage8.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog8(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 8);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHABLEND),   BM_SETCHECK, configuracion->alphaBlend     ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), BM_SETCHECK, configuracion->alphaOnBlack   ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          BM_SETCHECK, configuracion->transparentBMP ? BST_CHECKED : BST_UNCHECKED, 0);
			SetDlgItemInt(hDlg, IDC_EDIT_TRANS_THRESHOLD, configuracion->alphaThreshold, TRUE);

			if (configuracion->alphaBlend) {
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_TRANS_THRESHOLD),     FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          FALSE);
			}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_TRANS_ALPHABLEND:
			BOOL checked = SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHABLEND), BM_GETCHECK, 0, 0) == BST_CHECKED;
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), !checked);
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT_TRANS_THRESHOLD),     !checked);
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          !checked);
			break;
		}
		return 0;
	case WM_PAINT:
		PaintOptionsDialog(hDlg, 8);
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

BOOL SaveConfiguration8(HWND hDlg)
{
	int alphaThreshold;

	alphaThreshold = GetDlgItemInt(hDlg, IDC_EDIT_TRANS_THRESHOLD, NULL, TRUE);

	if (alphaThreshold < 0 || alphaThreshold > 255) {
		MessageBox(hDlg, TEXT("Alpha threshold value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	configuracion->alphaThreshold = alphaThreshold;

	configuracion->alphaOnBlack   = SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->alphaBlend     = SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHABLEND),   BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->transparentBMP = SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
