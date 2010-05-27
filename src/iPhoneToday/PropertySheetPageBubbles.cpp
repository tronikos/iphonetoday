//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageBubbles.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"
#include "ChooseFont.h"

BubbleSettings bnotif, balarm, bstate;
BubbleSettings *cur_bs;

void bs_show(HWND hDlg, BOOL bShow)
{
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_FONT),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HEIGHT),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_WIDTH),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_WEIGHT),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_COLOR),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_OFFSETS),bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_L),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_T),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_R),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_B),		bShow ? SW_SHOW : SW_HIDE);

	ShowWindow(GetDlgItem(hDlg, IDC_COMBO_BUBBLE_SIS_FACENAME),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_HEIGHT),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_WIDTH),			bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_WEIGHT),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR),			bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_BUTTON_BUBBLE_SIS_COLOR),		bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_LEFT),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_TOP),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_RIGHT),	bShow ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_BOTTOM),	bShow ? SW_SHOW : SW_HIDE);

	ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HINT),	!bShow ? SW_SHOW : SW_HIDE);
}

void bs_enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_IMAGE),				bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_BUBBLE_IMAGE),				bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_X),					bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_Y),					bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_W),					bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_H),					bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_COMBO_BUBBLE_SIS_FACENAME),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_HEIGHT),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_WIDTH),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_WEIGHT),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_BUBBLE_SIS_COLOR),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_LEFT),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_TOP),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_RIGHT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_BOTTOM),	bEnable);
}

void bs_load(HWND hDlg, BubbleSettings *bs)
{
	if (bs != NULL) {
		SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_IMAGE,			bs->image);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_X,				bs->x,			TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_Y,				bs->y,			TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_W,				bs->width,		TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_H,				bs->height,		TRUE);

		SetDlgItemText(hDlg, IDC_COMBO_BUBBLE_SIS_FACENAME, bs->sis.facename);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_HEIGHT,		bs->sis.height, TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_WIDTH,		bs->sis.width,  TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_WEIGHT,		bs->sis.weight, TRUE);
		SetDlgItemHex(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR,		bs->sis.color);

		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_LEFT,	bs->sis.offset.left,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_TOP,		bs->sis.offset.top,		TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_RIGHT,	bs->sis.offset.right,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_BOTTOM,	bs->sis.offset.bottom,	TRUE);
	}
	bs_enable(hDlg, TRUE);
}

void bs_save(HWND hDlg, BubbleSettings *bs)
{
	if (bs != NULL) {
		GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_IMAGE, bs->image, CountOf(bs->image));
		bs->x      = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_X, NULL, TRUE);
		bs->y      = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_Y, NULL, TRUE);
		bs->width  = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_W, NULL, TRUE);
		bs->height = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_H, NULL, TRUE);

		GetDlgItemText(hDlg, IDC_COMBO_BUBBLE_SIS_FACENAME, bs->sis.facename, CountOf(bs->sis.facename));
		bs->sis.height = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_HEIGHT, NULL, TRUE);
		bs->sis.width  = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_WIDTH,  NULL, TRUE);
		bs->sis.weight = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_WEIGHT, NULL, TRUE);
		bs->sis.color  = GetDlgItemHex(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR,  NULL);

		bs->sis.offset.left   = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_LEFT,   NULL, TRUE);
		bs->sis.offset.top    = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_TOP,    NULL, TRUE);
		bs->sis.offset.right  = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_RIGHT,  NULL, TRUE);
		bs->sis.offset.bottom = GetDlgItemInt(hDlg, IDC_EDIT_BUBBLE_SIS_OFFSET_BOTTOM, NULL, TRUE);
	}
}

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogBubbles(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_BUBBLES);

			EnumFontFamilies(GetDC(NULL), NULL, (FONTENUMPROC)EnumFontFamiliesProc, (LPARAM)GetDlgItem(hDlg, IDC_COMBO_BUBBLE_SIS_FACENAME));

			cur_bs = NULL;
			bs_enable(hDlg, FALSE);
			bs_show(hDlg, FALSE);

			memcpy(&balarm, &configuracion->bubble_alarm, sizeof(BubbleSettings));
			memcpy(&bnotif, &configuracion->bubble_notif, sizeof(BubbleSettings));
			memcpy(&bstate, &configuracion->bubble_state, sizeof(BubbleSettings));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_BUBBLE), CB_ADDSTRING, 0, (LPARAM)L"Alarm");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_BUBBLE), CB_ADDSTRING, 0, (LPARAM)L"Notif");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_BUBBLE), CB_ADDSTRING, 0, (LPARAM)L"State");

			ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HINT), SW_SHOW);
		}
		return TRUE;
	case WM_COMMAND:
		{
			int rgbCurrent;
			COLORREF nextColor;
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_BUBBLE:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					bs_save(hDlg, cur_bs);
					TCHAR str[MAX_PATH];
					GetDlgItemText(hDlg, IDC_COMBO_BUBBLE, str, MAX_PATH);
					if (lstrcmpi(str, L"Alarm") == 0) {
						cur_bs = &balarm;
						bs_load(hDlg, cur_bs);
						bs_show(hDlg, FALSE);
					} else if (lstrcmpi(str, L"Notif") == 0) {
						cur_bs = &bnotif;
						bs_load(hDlg, cur_bs);
						bs_show(hDlg, TRUE);
					} else if (lstrcmpi(str, L"State") == 0) {
						cur_bs = &bstate;
						bs_load(hDlg, cur_bs);
						bs_show(hDlg, FALSE);
					} else {
						bs_enable(hDlg, FALSE);
						bs_show(hDlg, FALSE);
					}
				}
				break;
			case IDC_BUTTON_BUBBLE_SIS_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BUBBLE_SIS_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_BUBBLE_IMAGE:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_IMAGE, str, MAX_PATH);
				configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
				getPathFromFile(fullPath, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, fullPath, browseDir)) {
					configuracion->getRelativePath(str, MAX_PATH, fullPath);
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_IMAGE, str);
				}
				break;
			}
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_BUBBLES, uMsg, wParam, lParam);
}

/*BOOL bs_check(HWND hDlg, BubbleSettings *bs)
{
	if (bs->sis.height < 0 || bs->sis.height > 100) {
		MessageBox(hDlg, TEXT("Bubble's text height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.width < 0 || bs->sis.width > 100) {
		MessageBox(hDlg, TEXT("Bubble's text width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.weight < 0 || bs->sis.weight > 1000) {
		MessageBox(hDlg, TEXT("Bubble's text weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.offset.left < 0 || bs->sis.offset.left > 100) {
		MessageBox(hDlg, TEXT("Bubble's text left offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.offset.top < 0 || bs->sis.offset.top > 100) {
		MessageBox(hDlg, TEXT("Bubble's text top offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.offset.right < 0 || bs->sis.offset.right > 100) {
		MessageBox(hDlg, TEXT("Bubble's text right offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (bs->sis.offset.bottom < 0 || bs->sis.offset.bottom > 100) {
		MessageBox(hDlg, TEXT("Bubble's text bottom offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL IsValidConfigurationBubbles(HWND hDlg)
{
	bs_save(hDlg, cur_bs);

	if (!bs_check(hDlg, &balarm)) return FALSE;
	if (!bs_check(hDlg, &bnotif)) return FALSE;
	if (!bs_check(hDlg, &bstate)) return FALSE;

	return TRUE;
}*/

BOOL SaveConfigurationBubbles(HWND hDlg)
{
	bs_save(hDlg, cur_bs);

//	if (!IsValidConfigurationBubbles(hDlg)) return FALSE;

	memcpy(&configuracion->bubble_alarm, &balarm, sizeof(BubbleSettings));
	memcpy(&configuracion->bubble_notif, &bnotif, sizeof(BubbleSettings));
	memcpy(&configuracion->bubble_state, &bstate, sizeof(BubbleSettings));

	return TRUE;
}
