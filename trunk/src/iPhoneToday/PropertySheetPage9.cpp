//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage9.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog9(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 9);

			SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_LEFT,   configuracion->outOfScreenLeft);
			SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_RIGHT,  configuracion->outOfScreenRight);
			SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_TOP,    configuracion->outOfScreenTop);
			SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_BOTTOM, configuracion->outOfScreenBottom);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			TCHAR str[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			case IDC_BUTTON_OUTOFSCREEN_LEFT:
				GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_LEFT, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_LEFT, str);
				}
				break;
			case IDC_BUTTON_OUTOFSCREEN_RIGHT:
				GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_RIGHT, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_RIGHT, str);
				}
				break;
			case IDC_BUTTON_OUTOFSCREEN_TOP:
				GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_TOP, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_TOP, str);
				}
				break;
			case IDC_BUTTON_OUTOFSCREEN_BOTTOM:
				GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_BOTTOM, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_BOTTOM, str);
				}
				break;
		}
		return 0;
	case WM_PAINT:
		PaintOptionsDialog(hDlg, 9);
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

BOOL SaveConfiguration9(HWND hDlg)
{
	GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_LEFT,   configuracion->outOfScreenLeft,   CountOf(configuracion->outOfScreenLeft));
	GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_RIGHT,  configuracion->outOfScreenRight,  CountOf(configuracion->outOfScreenRight));
	GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_TOP,    configuracion->outOfScreenTop,    CountOf(configuracion->outOfScreenTop));
	GetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_BOTTOM, configuracion->outOfScreenBottom, CountOf(configuracion->outOfScreenBottom));

	return TRUE;
}
