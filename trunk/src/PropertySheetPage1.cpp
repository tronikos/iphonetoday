//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage1.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog1(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
    {
    case WM_INITDIALOG:
        {
			// Initialize handle to property sheet
			g_hDlg[1] = hDlg;

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

				// TCHAR str[MAX_PATH];
			
				// Configuramos el elemento Wallpaper
				SetWindowText(GetDlgItem(hDlg, IDC_P1_EDIT_WALLPAPER), configuracion->strFondoPantalla);

				// Configuramos los checks
				if (configuracion->fondoTransparente > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_TRANSPARENT), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_TRANSPARENT), BM_SETCHECK, BST_UNCHECKED, 0);
				}

				if (configuracion->fondoEstatico > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_STATIC), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_STATIC), BM_SETCHECK, BST_UNCHECKED, 0);
				}

			} else {
				MessageBox(0, L"Configuracion vacio!", L"Error", MB_OK);
			}

			
        }
        return TRUE; 
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_P1_EDIT_WALLPAPER_B) {
				TCHAR pathFile[MAX_PATH];
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, pathFile, L"")) {
					SetWindowText(GetDlgItem(hDlg, IDC_P1_EDIT_WALLPAPER), pathFile);
				}
			}
		}
		break;
    }

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration1(HWND hDlg)
{
	// Recuperamos los valores introducidos
	TCHAR strWallpaper[MAX_PATH];

	int transparentBackground;
	int staticWallpaper;

	GetWindowText(GetDlgItem(hDlg, IDC_P1_EDIT_WALLPAPER), strWallpaper, MAX_PATH);
	if (SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_TRANSPARENT), BM_GETCHECK, 0, 0) == BST_CHECKED) {
		transparentBackground = 1;
	} else {
		transparentBackground = 0;
	}
	if (SendMessage(GetDlgItem(hDlg, IDC_P1_CHECK_STATIC), BM_GETCHECK, 0, 0) == BST_CHECKED) {
		staticWallpaper = 1;
	} else {
		staticWallpaper = 0;
	}

	// Ponemos los nuevos valores
	StringCchCopy(configuracion->strFondoPantalla, CountOf(configuracion->strFondoPantalla), strWallpaper);
	configuracion->fondoTransparente = transparentBackground;
	configuracion->fondoEstatico = staticWallpaper;

	return TRUE;
}