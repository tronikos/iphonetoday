//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage2.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[2] = hDlg;

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
				SetDlgItemText(hDlg, IDC_EDIT_BACK_WALLPAPER, configuracion->strFondoPantalla);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_BACK_STATIC), BM_SETCHECK, configuracion->fondoEstatico ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_BACK_TRANSPARENT), BM_SETCHECK, configuracion->fondoTransparente ? BST_CHECKED : BST_UNCHECKED, 0);
				SetDlgItemHex(hDlg, IDC_EDIT_BACK_COLOR, configuracion->fondoColor);
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_BACK_WALLPAPER:
				TCHAR str[MAX_PATH];
				TCHAR browseDir[MAX_PATH];
				GetDlgItemText(hDlg, IDC_EDIT_BACK_WALLPAPER, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BACK_WALLPAPER, str);
				}
				break;
			case IDC_BUTTON_BACK_COLOR:
				int rgbCurrent;
				COLORREF nextColor;
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BACK_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BACK_COLOR, nextColor);
				}
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration2(HWND hDlg)
{
	GetDlgItemText(hDlg, IDC_EDIT_BACK_WALLPAPER, configuracion->strFondoPantalla, CountOf(configuracion->strFondoPantalla));
	configuracion->fondoEstatico = SendMessage(GetDlgItem(hDlg, IDC_CHECK_BACK_STATIC), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->fondoTransparente = SendMessage(GetDlgItem(hDlg, IDC_CHECK_BACK_TRANSPARENT), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->fondoColor = GetDlgItemHex(hDlg, IDC_EDIT_BACK_COLOR, NULL);

	return TRUE;
}
