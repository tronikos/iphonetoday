//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage2.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

COLORREF tempNextColor;

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

				TCHAR str[MAX_PATH];

				swprintf(str, L"%d", configuracion->clockWidth);
				SetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_WIDTH), str);

				swprintf(str, L"%d", configuracion->clockHeight);
				SetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_HEIGHT), str);

				tempNextColor = configuracion->clockColor;
				swprintf(str, L"#%X", tempNextColor);
				SetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_COLOR), str);

				// Configuramos los checks
				if (configuracion->clock12Format > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_P2_CHECK_12FORMAT), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_P2_CHECK_12FORMAT), BM_SETCHECK, BST_UNCHECKED, 0);
				}

			} else {
				MessageBox(0, L"Configuracion vacio!", L"Error", MB_OK);
			}

			
        }
        return TRUE; 
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDC_P2_EDIT_FONT_COLOR_B) {
				TCHAR pathFile[MAX_PATH];
				COLORREF nextColor;
				if (ColorSelector(tempNextColor, &nextColor)) {
					tempNextColor = nextColor;
					swprintf(pathFile, L"#%X", tempNextColor);
					SetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_COLOR), pathFile);
				}
			}
		}
		break;
    }

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration2(HWND hDlg)
{
	// Recuperamos los valores introducidos
	TCHAR strWidth[MAX_PATH];
	TCHAR strHeight[MAX_PATH];

	int clockWidth;
	int clockHeight;

	int clock12Format;

	GetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_WIDTH), strWidth, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_P2_EDIT_FONT_HEIGHT), strHeight, MAX_PATH);
	if (SendMessage(GetDlgItem(hDlg, IDC_P2_CHECK_12FORMAT), BM_GETCHECK, 0, 0) == BST_CHECKED) {
		clock12Format = 1;
	} else {
		clock12Format = 0;
	}

	clockWidth = _wtoi(strWidth);
	clockHeight = _wtoi(strHeight);

	// Comprobaciones
	if (clockWidth < 1 || clockWidth > 80) {
		MessageBox(hDlg, TEXT("Clock Width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (clockHeight < 1 || clockHeight > 80) {
		MessageBox(hDlg, TEXT("Clock Height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	// Ponemos los nuevos valores
	configuracion->clockWidth = clockWidth;
	configuracion->clockHeight = clockHeight;
	configuracion->clockColor = tempNextColor;
	configuracion->clock12Format = clock12Format;

	return TRUE;
}