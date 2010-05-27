//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageSpecialIcons.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"
#include "ChooseFont.h"

SpecialIconSettings batt, dow, dom, clck, alrm, vol, meml, memf, memu, psig, wsig;
SpecialIconSettings *cur_sis;

void sis_enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_COMBO_SIS_FACENAME),		bEnable);
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

void sis_load(HWND hDlg, SpecialIconSettings *sis)
{
	if (sis != NULL) {
		SetDlgItemText(hDlg, IDC_COMBO_SIS_FACENAME, sis->facename);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_HEIGHT, sis->height, TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_WIDTH,  sis->width,  TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_WEIGHT, sis->weight, TRUE);
		SetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR,  sis->color);

		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_LEFT,	sis->offset.left,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_TOP,	sis->offset.top,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_RIGHT,	sis->offset.right,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_SIS_OFFSET_BOTTOM,	sis->offset.bottom,	TRUE);
	}
	sis_enable(hDlg, TRUE);
}

void sis_save(HWND hDlg, SpecialIconSettings *sis)
{
	if (sis != NULL) {
		GetDlgItemText(hDlg, IDC_COMBO_SIS_FACENAME, sis->facename, CountOf(sis->facename));
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
LRESULT CALLBACK OptionDialogSpecialIcons(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_SPECIALICONS);

			EnumFontFamilies(GetDC(NULL), NULL, (FONTENUMPROC)EnumFontFamiliesProc, (LPARAM)GetDlgItem(hDlg, IDC_COMBO_SIS_FACENAME));

			cur_sis = NULL;
			sis_enable(hDlg, FALSE);

			memcpy(&batt, &configuracion->batt, sizeof(SpecialIconSettings));
			memcpy(&clck, &configuracion->clck, sizeof(SpecialIconSettings));
			memcpy(&alrm, &configuracion->alrm, sizeof(SpecialIconSettings));
			memcpy(&dom,  &configuracion->dom,  sizeof(SpecialIconSettings));
			memcpy(&dow,  &configuracion->dow,  sizeof(SpecialIconSettings));
			memcpy(&vol,  &configuracion->vol,  sizeof(SpecialIconSettings));
			memcpy(&meml, &configuracion->meml, sizeof(SpecialIconSettings));
			memcpy(&memf, &configuracion->memf, sizeof(SpecialIconSettings));
			memcpy(&memu, &configuracion->memu, sizeof(SpecialIconSettings));
			memcpy(&psig, &configuracion->psig, sizeof(SpecialIconSettings));
			memcpy(&wsig, &configuracion->wsig, sizeof(SpecialIconSettings));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Battery");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Clock");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Alarm");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Day of month");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Day of week");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Volume");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Memory load");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Memory free");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Memory used");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Phone signal strength");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_SIS), CB_ADDSTRING, 0, (LPARAM)L"Wifi signal strength");

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_SETCHECK, configuracion->clock12Format ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_DOW_USE_LOCALE), BM_SETCHECK, configuracion->dowUseLocale ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_BATT_ALWAYS_SHOW_PERC), BM_SETCHECK, !configuracion->battShowAC ? BST_CHECKED : BST_UNCHECKED, 0);

			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_DOW_USE_LOCALE), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_BATT_ALWAYS_SHOW_PERC), SW_HIDE);
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_SIS:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					sis_save(hDlg, cur_sis);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_DOW_USE_LOCALE), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_BATT_ALWAYS_SHOW_PERC), SW_HIDE);
					TCHAR str[MAX_PATH];
					GetDlgItemText(hDlg, IDC_COMBO_SIS, str, MAX_PATH);
					if (lstrcmpi(str, L"Battery") == 0) {
						cur_sis = &batt;
						sis_load(hDlg, cur_sis);
						ShowWindow(GetDlgItem(hDlg, IDC_CHECK_BATT_ALWAYS_SHOW_PERC), SW_SHOW);
					} else if (lstrcmpi(str, L"Clock") == 0) {
						cur_sis = &clck;
						sis_load(hDlg, cur_sis);
						ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_SHOW);
					} else if (lstrcmpi(str, L"Alarm") == 0) {
						cur_sis = &alrm;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Day of month") == 0) {
						cur_sis = &dom;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Day of week") == 0) {
						cur_sis = &dow;
						sis_load(hDlg, cur_sis);
						ShowWindow(GetDlgItem(hDlg, IDC_CHECK_DOW_USE_LOCALE), SW_SHOW);
					} else if (lstrcmpi(str, L"Volume") == 0) {
						cur_sis = &vol;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Memory load") == 0) {
						cur_sis = &meml;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Memory free") == 0) {
						cur_sis = &memf;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Memory used") == 0) {
						cur_sis = &memu;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Phone signal strength") == 0) {
						cur_sis = &psig;
						sis_load(hDlg, cur_sis);
					} else if (lstrcmpi(str, L"Wifi signal strength") == 0) {
						cur_sis = &wsig;
						sis_load(hDlg, cur_sis);
					} else {
						sis_enable(hDlg, FALSE);
					}
				}
				break;
			case IDC_BUTTON_SIS_COLOR:
				int rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR, NULL);
				COLORREF nextColor;
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_SIS_COLOR, nextColor);
				}
				break;
			}
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_SPECIALICONS, uMsg, wParam, lParam);
}

/*BOOL sis_check(HWND hDlg, SpecialIconSettings *sis)
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

BOOL IsValidConfigurationSpecialIcons(HWND hDlg)
{
	sis_save(hDlg, cur_sis);

	if (!sis_check(hDlg, &batt)) return FALSE;
	if (!sis_check(hDlg, &clck)) return FALSE;
	if (!sis_check(hDlg, &alrm)) return FALSE;
	if (!sis_check(hDlg, &dom))  return FALSE;
	if (!sis_check(hDlg, &dow))  return FALSE;
	if (!sis_check(hDlg, &vol))  return FALSE;
	if (!sis_check(hDlg, &meml)) return FALSE;
	if (!sis_check(hDlg, &memf)) return FALSE;
	if (!sis_check(hDlg, &memu)) return FALSE;
	if (!sis_check(hDlg, &psig)) return FALSE;
	if (!sis_check(hDlg, &wsig)) return FALSE;

	return TRUE;
}*/

BOOL SaveConfigurationSpecialIcons(HWND hDlg)
{
	sis_save(hDlg, cur_sis);

//	if (!IsValidConfigurationSpecialIcons(hDlg)) return FALSE;

	memcpy(&configuracion->batt, &batt, sizeof(SpecialIconSettings));
	memcpy(&configuracion->clck, &clck, sizeof(SpecialIconSettings));
	memcpy(&configuracion->alrm, &alrm, sizeof(SpecialIconSettings));
	memcpy(&configuracion->dom,  &dom,  sizeof(SpecialIconSettings));
	memcpy(&configuracion->dow,  &dow,  sizeof(SpecialIconSettings));
	memcpy(&configuracion->vol,  &vol,  sizeof(SpecialIconSettings));
	memcpy(&configuracion->meml, &meml, sizeof(SpecialIconSettings));
	memcpy(&configuracion->memf, &memf, sizeof(SpecialIconSettings));
	memcpy(&configuracion->memu, &memu, sizeof(SpecialIconSettings));
	memcpy(&configuracion->psig, &psig, sizeof(SpecialIconSettings));
	memcpy(&configuracion->wsig, &wsig, sizeof(SpecialIconSettings));

	configuracion->clock12Format = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->dowUseLocale  = SendMessage(GetDlgItem(hDlg, IDC_CHECK_DOW_USE_LOCALE), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->battShowAC    = !SendMessage(GetDlgItem(hDlg, IDC_CHECK_BATT_ALWAYS_SHOW_PERC), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
