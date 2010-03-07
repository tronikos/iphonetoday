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
			// Initialize handle to property sheet
			g_hDlg[8] = hDlg;

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
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHABLEND),   BM_SETCHECK, configuracion->alphaBlend     ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), BM_SETCHECK, configuracion->alphaOnBlack   ? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          BM_SETCHECK, configuracion->transparentBMP ? BST_CHECKED : BST_UNCHECKED, 0);
				SetDlgItemInt(hDlg, IDC_EDIT_TRANS_THRESHOLD, configuracion->alphaThreshold, TRUE);

				if (configuracion->alphaBlend) {
					EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_ALPHAONBLACK), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_EDIT_TRANS_THRESHOLD),     FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_CHECK_TRANS_BMP),          FALSE);
				}
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
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
	case WM_CTLCOLORSTATIC:
		return (LRESULT)GetStockObject(WHITE_BRUSH);
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
