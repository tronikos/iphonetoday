//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage5.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog5(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[5] = hDlg;

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
				SetDlgItemInt(hDlg, IDC_EDIT_BATT_HEIGHT,	configuracion->battHeight,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BATT_WIDTH,	configuracion->battWidth,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BATT_WEIGHT,	configuracion->battWeight,	TRUE);
				SetDlgItemHex(hDlg, IDC_EDIT_BATT_COLOR,	configuracion->battColor);

				SetDlgItemInt(hDlg, IDC_EDIT_DOW_HEIGHT,	configuracion->dowHeight,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_DOW_WIDTH,		configuracion->dowWidth,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_DOW_WEIGHT,	configuracion->dowWeight,	TRUE);
				SetDlgItemHex(hDlg, IDC_EDIT_DOW_COLOR,		configuracion->dowColor);

				SetDlgItemInt(hDlg, IDC_EDIT_DOM_HEIGHT,	configuracion->domHeight,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_DOM_WIDTH,		configuracion->domWidth,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_DOM_WEIGHT,	configuracion->domWeight,	TRUE);
				SetDlgItemHex(hDlg, IDC_EDIT_DOM_COLOR,		configuracion->domColor);

				SetDlgItemInt(hDlg, IDC_EDIT_CLOCK_HEIGHT,	configuracion->clockHeight,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_CLOCK_WIDTH,	configuracion->clockWidth,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_CLOCK_WEIGHT,	configuracion->clockWeight,	TRUE);
				SetDlgItemHex(hDlg, IDC_EDIT_CLOCK_COLOR,	configuracion->clockColor);

				SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_SETCHECK, configuracion->clock12Format ? BST_CHECKED : BST_UNCHECKED, 0);
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			int rgbCurrent;
			COLORREF nextColor;
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_BATT_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BATT_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BATT_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_DOW_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_DOW_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_DOW_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_DOM_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_DOM_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_DOM_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_CLOCK_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CLOCK_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_CLOCK_COLOR, nextColor);
				}
				break;
			}
		}
		return 0;
	case WM_CTLCOLORSTATIC:
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration5(HWND hDlg)
{
	int battHeight, battWidth, battWeight, battColor;
	int dowHeight, dowWidth, dowWeight, dowColor;
	int domHeight, domWidth, domWeight, domColor;
	int clockHeight, clockWidth, clockWeight, clockColor;

	battHeight	= GetDlgItemInt(hDlg, IDC_EDIT_BATT_HEIGHT,	NULL, TRUE);
	battWidth	= GetDlgItemInt(hDlg, IDC_EDIT_BATT_WIDTH,	NULL, TRUE);
	battWeight	= GetDlgItemInt(hDlg, IDC_EDIT_BATT_WEIGHT,	NULL, TRUE);
	battColor	= GetDlgItemHex(hDlg, IDC_EDIT_BATT_COLOR,	NULL);

	dowHeight	= GetDlgItemInt(hDlg, IDC_EDIT_DOW_HEIGHT,	NULL, TRUE);
	dowWidth	= GetDlgItemInt(hDlg, IDC_EDIT_DOW_WIDTH,	NULL, TRUE);
	dowWeight	= GetDlgItemInt(hDlg, IDC_EDIT_DOW_WEIGHT,	NULL, TRUE);
	dowColor	= GetDlgItemHex(hDlg, IDC_EDIT_DOW_COLOR,	NULL);

	domHeight	= GetDlgItemInt(hDlg, IDC_EDIT_DOM_HEIGHT,	NULL, TRUE);
	domWidth	= GetDlgItemInt(hDlg, IDC_EDIT_DOM_WIDTH,	NULL, TRUE);
	domWeight	= GetDlgItemInt(hDlg, IDC_EDIT_DOM_WEIGHT,	NULL, TRUE);
	domColor	= GetDlgItemHex(hDlg, IDC_EDIT_DOM_COLOR,	NULL);

	clockHeight	= GetDlgItemInt(hDlg, IDC_EDIT_CLOCK_HEIGHT,NULL, TRUE);
	clockWidth	= GetDlgItemInt(hDlg, IDC_EDIT_CLOCK_WIDTH,	NULL, TRUE);
	clockWeight	= GetDlgItemInt(hDlg, IDC_EDIT_CLOCK_WEIGHT,NULL, TRUE);
	clockColor = GetDlgItemHex(hDlg, IDC_EDIT_CLOCK_COLOR,	NULL);

	if (battHeight < 0 || battHeight > 100) {
		MessageBox(hDlg, TEXT("Battery height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (battWidth < 0 || battWidth > 100) {
		MessageBox(hDlg, TEXT("Battery width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (battWeight < 0 || battWeight > 1000) {
		MessageBox(hDlg, TEXT("Battery weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (dowHeight < 0 || dowHeight > 100) {
		MessageBox(hDlg, TEXT("Day of week height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (dowWidth < 0 || dowWidth > 100) {
		MessageBox(hDlg, TEXT("Day of week width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (dowWeight < 0 || dowWeight > 1000) {
		MessageBox(hDlg, TEXT("Day of week weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (domHeight < 0 || domHeight > 100) {
		MessageBox(hDlg, TEXT("Day of month height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (domWidth < 0 || domWidth > 100) {
		MessageBox(hDlg, TEXT("Day of month width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (domWeight < 0 || domWeight > 1000) {
		MessageBox(hDlg, TEXT("Day of month weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (clockHeight < 0 || clockHeight > 100) {
		MessageBox(hDlg, TEXT("Clock height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (clockWidth < 0 || clockWidth > 100) {
		MessageBox(hDlg, TEXT("Clock width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (clockWeight < 0 || clockWeight > 1000) {
		MessageBox(hDlg, TEXT("Clock weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	configuracion->battHeight = battHeight;
	configuracion->battWidth  = battWidth;
	configuracion->battWeight = battWeight;
	configuracion->battColor  = battColor;

	configuracion->dowHeight = dowHeight;
	configuracion->dowWidth  = dowWidth;
	configuracion->dowWeight = dowWeight;
	configuracion->dowColor  = dowColor;

	configuracion->domHeight = domHeight;
	configuracion->domWidth  = domWidth;
	configuracion->domWeight = domWeight;
	configuracion->domColor  = domColor;

	configuracion->clockHeight = clockHeight;
	configuracion->clockWidth  = clockWidth;
	configuracion->clockWeight = clockWeight;
	configuracion->clockColor  = clockColor;


	configuracion->clock12Format = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
