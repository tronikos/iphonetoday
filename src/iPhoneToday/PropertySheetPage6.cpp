//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage6.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

SpecialIconSettings batt, dow, dom, clck, vol;
SpecialIconSettings *cur_sis;

void enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_HEIGHT),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_WIDTH),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_WEIGHT),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_COLOR),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_SIS_COLOR),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_OFFSET_LEFT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_OFFSET_TOP),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_OFFSET_RIGHT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SIS_OFFSET_BOTTOM),	bEnable);
}

void load(HWND hDlg, SpecialIconSettings *sis)
{
	if (sis != NULL) {
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_HEIGHT, sis->height, TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_WIDTH,  sis->width,  TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_WEIGHT, sis->weight, TRUE);
		SetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR,  sis->color);

		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_LEFT,	sis->offset.left,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_TOP,	sis->offset.top,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_RIGHT,	sis->offset.right,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_BOTTOM,	sis->offset.bottom,	TRUE);
	}
	enable(hDlg, TRUE);
}

void save(HWND hDlg, SpecialIconSettings *sis)
{
	if (sis != NULL) {
		sis->height = GetDlgItemInt(hDlg, IDC_EDIT_SIS_HEIGHT, NULL, TRUE);
		sis->width  = GetDlgItemInt(hDlg, IDC_EDIT_SIS_WIDTH,  NULL, TRUE);
		sis->weight = GetDlgItemInt(hDlg, IDC_EDIT_SIS_WEIGHT, NULL, TRUE);
		sis->color  = GetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR,  NULL);

		sis->offset.left   = GetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_LEFT,   NULL, TRUE);
		sis->offset.top    = GetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_TOP,    NULL, TRUE);
		sis->offset.right  = GetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_RIGHT,  NULL, TRUE);
		sis->offset.bottom = GetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_BOTTOM, NULL, TRUE);
	}
}

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog6(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[6] = hDlg;

			SHINITDLGINFO shidi;

			// Create a Done button and size it.  
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			SHInitExtraControls();

			cur_sis = NULL;
			enable(hDlg, FALSE);

			if (configuracion == NULL) {
				configuracion = new CConfiguracion();
				configuracion->cargaXMLConfig();
			}
			if (configuracion != NULL) {
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Battery");
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Clock");
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Day of month");
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Day of week");
				SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Volume");

				memcpy(&batt, &configuracion->batt, sizeof(SpecialIconSettings));
				memcpy(&clck, &configuracion->clck, sizeof(SpecialIconSettings));
				memcpy(&dom,  &configuracion->dom,  sizeof(SpecialIconSettings));
				memcpy(&dow,  &configuracion->dow,  sizeof(SpecialIconSettings));
				memcpy(&vol,  &configuracion->vol,  sizeof(SpecialIconSettings));

				SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_SETCHECK, configuracion->clock12Format ? BST_CHECKED : BST_UNCHECKED, 0);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_HIDE);
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_SIS:
				save(hDlg, cur_sis);
				ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_HIDE);
				TCHAR str[MAX_PATH];
				GetDlgItemText(hDlg, IDC_COMBO_SIS, str, MAX_PATH);
				if (lstrcmpi(str, L"Battery") == 0) {
					cur_sis = &batt;
					load(hDlg, cur_sis);
				} else if (lstrcmpi(str, L"Clock") == 0) {
					cur_sis = &clck;
					load(hDlg, cur_sis);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_SHOW);
				} else if (lstrcmpi(str, L"Day of month") == 0) {
					cur_sis = &dom;
					load(hDlg, cur_sis);
				} else if (lstrcmpi(str, L"Day of week") == 0) {
					cur_sis = &dow;
					load(hDlg, cur_sis);
				} else if (lstrcmpi(str, L"Volume") == 0) {
					cur_sis = &vol;
					load(hDlg, cur_sis);
				} else {
					enable(hDlg, FALSE);
				}
				break;
			case IDC_BUTTON_SIS_COLOR:
				int rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR, NULL);
				COLORREF nextColor;
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR, nextColor);
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

BOOL check(HWND hDlg, SpecialIconSettings *sis)
{
	if (sis->height < 0 || sis->height > 100) {
		MessageBox(hDlg, TEXT("Special icon's height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->width < 0 || sis->width > 100) {
		MessageBox(hDlg, TEXT("Special icon's width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->weight < 0 || sis->weight > 1000) {
		MessageBox(hDlg, TEXT("Special icon's weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->offset.left < 0 || sis->offset.left > 100) {
		MessageBox(hDlg, TEXT("Special icon's left offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->offset.top < 0 || sis->offset.top > 100) {
		MessageBox(hDlg, TEXT("Special icon's top offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->offset.right < 0 || sis->offset.right > 100) {
		MessageBox(hDlg, TEXT("Special icon's right offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (sis->offset.bottom < 0 || sis->offset.bottom > 100) {
		MessageBox(hDlg, TEXT("Special icon's bottom offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL SaveConfiguration6(HWND hDlg)
{
	save(hDlg, cur_sis);

	if (!check(hDlg, &batt)) return FALSE;
	if (!check(hDlg, &clck)) return FALSE;
	if (!check(hDlg, &dom)) return FALSE;
	if (!check(hDlg, &dow)) return FALSE;
	if (!check(hDlg, &vol)) return FALSE;

	memcpy(&configuracion->batt, &batt, sizeof(SpecialIconSettings));
	memcpy(&configuracion->clck, &clck, sizeof(SpecialIconSettings));
	memcpy(&configuracion->dom,  &dom,  sizeof(SpecialIconSettings));
	memcpy(&configuracion->dow,  &dow,  sizeof(SpecialIconSettings));
	memcpy(&configuracion->vol,  &vol,  sizeof(SpecialIconSettings));

	configuracion->clock12Format = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
