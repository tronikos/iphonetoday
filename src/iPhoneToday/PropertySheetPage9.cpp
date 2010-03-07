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
			// Initialize handle to property sheet
			g_hDlg[9] = hDlg;

			SHINITDLGINFO shidi;

			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			SHInitExtraControls();

			if (configuracion == NULL) {
				configuracion = new CConfiguracion();
				configuracion->cargaXMLConfig();
			}
			if (configuracion != NULL) {
				SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_LEFT,   configuracion->outOfScreenLeft);
				SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_RIGHT,  configuracion->outOfScreenRight);
				SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_TOP,    configuracion->outOfScreenTop);
				SetDlgItemText(hDlg, IDC_EDIT_OUTOFSCREEN_BOTTOM, configuracion->outOfScreenBottom);
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
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
	case WM_CTLCOLORSTATIC:
		return (LRESULT)GetStockObject(WHITE_BRUSH);
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
