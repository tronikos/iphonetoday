//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage0.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

ConfigurationScreen ms, bb, tb;
ConfigurationScreen *cur_cs;

void cs_enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ICON_WIDTH),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ICONS_PER_ROW),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_TEXT_HEIGHT),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_TEXT_OFFSET),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_TEXT_COLOR),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_TEXT_COLOR),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_CHECK_CS_TEXT_BOLD),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_BACK_COLOR1),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_BACK_COLOR1),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_BACK_COLOR2),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_BACK_COLOR2),	bEnable);	
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_MINHSPACE),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ADDVSPACE),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_LEFT),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_TOP),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_RIGHT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM),	bEnable);

}

void cs_load(HWND hDlg, ConfigurationScreen *cs)
{
	if (cs != NULL) {
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ICON_WIDTH,		cs->iconWidthXML,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ICONS_PER_ROW,	cs->iconsPerRowXML,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_HEIGHT,	cs->textHeightXML,		TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_OFFSET,	cs->textOffset,		TRUE);

		SetDlgItemHex(hDlg, IDC_EDIT_CS_TEXT_COLOR,		cs->textColor);

		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_TEXT_BOLD), BM_SETCHECK, cs->textBold ? BST_CHECKED : BST_UNCHECKED, 0);

		SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1,	cs->backColor1);
		SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2,	cs->backColor2);

		SetDlgItemInt(hDlg, IDC_EDIT_CS_MINHSPACE,		cs->minHorizontalSpace,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ADDVSPACE,		cs->additionalVerticalSpace,	TRUE);

		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_LEFT,	cs->offset.left,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_TOP,		cs->offset.top,		TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_RIGHT,	cs->offset.right,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM,	cs->offset.bottom,	TRUE);
	}
	cs_enable(hDlg, TRUE);
}

void cs_save(HWND hDlg, ConfigurationScreen *cs)
{
	if (cs != NULL) {
		cs->iconWidthXML	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ICON_WIDTH,		NULL, TRUE);
		cs->iconsPerRowXML	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ICONS_PER_ROW,	NULL, TRUE);
		cs->textHeightXML	= GetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_HEIGHT,		NULL, TRUE);
		cs->textOffset		= GetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_OFFSET,		NULL, TRUE);

		cs->textColor		= GetDlgItemHex(hDlg, IDC_EDIT_CS_TEXT_COLOR,		NULL);

		cs->textBold		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_TEXT_BOLD), BM_GETCHECK, 0, 0) == BST_CHECKED;

		cs->backColor1		= GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1,		NULL);
		cs->backColor2		= GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2,		NULL);

		cs->minHorizontalSpace		= GetDlgItemInt(hDlg, IDC_EDIT_CS_MINHSPACE,		NULL, TRUE);
		cs->additionalVerticalSpace	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ADDVSPACE,		NULL, TRUE);

		cs->offset.left		= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_LEFT,		NULL, TRUE);
		cs->offset.top		= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_TOP,		NULL, TRUE);
		cs->offset.right	= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_RIGHT,		NULL, TRUE);
		cs->offset.bottom	= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM,	NULL, TRUE);
	}
}

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 0);

			cur_cs = NULL;
			cs_enable(hDlg, FALSE);

			memcpy(&ms, &configuracion->mainScreenConfig->cs, sizeof(ConfigurationScreen));
			memcpy(&bb, &configuracion->bottomBarConfig->cs,  sizeof(ConfigurationScreen));
			memcpy(&tb, &configuracion->topBarConfig->cs,     sizeof(ConfigurationScreen));

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_CS), CB_ADDSTRING, 0, (LPARAM)L"Mainscreen");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_CS), CB_ADDSTRING, 0, (LPARAM)L"Bottombar");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO_CS), CB_ADDSTRING, 0, (LPARAM)L"Topbar");

			SendMessage(GetDlgItem(hDlg, IDC_COMBO_CS), CB_SETCURSEL, 0, 0);
			cur_cs = &ms;
			cs_load(hDlg, cur_cs);
		}
		return TRUE;
	case WM_COMMAND:
		{
			int rgbCurrent;
			COLORREF nextColor;
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_CS:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					cs_save(hDlg, cur_cs);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CLOCK_FORMAT12), SW_HIDE);
					TCHAR str[MAX_PATH];
					GetDlgItemText(hDlg, IDC_COMBO_CS, str, MAX_PATH);
					if (lstrcmpi(str, L"Mainscreen") == 0) {
						cur_cs = &ms;
						cs_load(hDlg, cur_cs);
					} else if (lstrcmpi(str, L"Bottombar") == 0) {
						cur_cs = &bb;
						cs_load(hDlg, cur_cs);
					} else if (lstrcmpi(str, L"Topbar") == 0) {
						cur_cs = &tb;
						cs_load(hDlg, cur_cs);
					} else {
						cs_enable(hDlg, FALSE);
					}
				}
				break;
			case IDC_BUTTON_CS_TEXT_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CS_TEXT_COLOR, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_CS_TEXT_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_CS_BACK_COLOR1:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1, nextColor);
				}
				break;
			case IDC_BUTTON_CS_BACK_COLOR2:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2, nextColor);
				}
				break;
			case IDC_EDIT_CS_ICONS_PER_ROW:
				if (HIWORD(wParam) == EN_SETFOCUS) {
					static BOOL displayed_tip = FALSE;
					if (!displayed_tip) {
						displayed_tip = TRUE;
						MessageBox(hDlg, L"It is recommended to leave this 0. In that case:\nicons_per_row = screen_width / (icon_width + minimum_horizontal_space).", L"Tip", MB_OK);
					}
				}
			}
		}
		return 0;
	}

	return DefOptionWindowProc(hDlg, 0, uMsg, wParam, lParam);
}

BOOL cs_check(HWND hDlg, ConfigurationScreen *cs)
{
	if (cs->iconWidthXML < 0 || cs->iconWidthXML > 256) {
		MessageBox(hDlg, TEXT("Icon width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->iconsPerRowXML < 0 || cs->iconsPerRowXML > 32) {
		MessageBox(hDlg, TEXT("Icons per row value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->textHeightXML < 0 || cs->textHeightXML > 100) {
		MessageBox(hDlg, TEXT("Text height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->textOffset < -256 || cs->textOffset > 256) {
		MessageBox(hDlg, TEXT("Text offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->minHorizontalSpace < 0 || cs->minHorizontalSpace > 256) {
		MessageBox(hDlg, TEXT("Minimum horizontal space value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->additionalVerticalSpace < 0 || cs->additionalVerticalSpace > 256) {
		MessageBox(hDlg, TEXT("Additional vertical space value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->offset.left < 0 || cs->offset.left > 256) {
		MessageBox(hDlg, TEXT("Offset left value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->offset.top < 0 || cs->offset.top > 256) {
		MessageBox(hDlg, TEXT("Offset top value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->offset.right < 0 || cs->offset.right > 256) {
		MessageBox(hDlg, TEXT("Offset right value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (cs->offset.bottom < 0 || cs->offset.bottom > 256) {
		MessageBox(hDlg, TEXT("Offset bottom value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}

BOOL IsValidConfiguration0(HWND hDlg)
{
	cs_save(hDlg, cur_cs);

	if (!cs_check(hDlg, &ms)) return FALSE;
	if (!cs_check(hDlg, &bb)) return FALSE;
	if (!cs_check(hDlg, &tb)) return FALSE;

	return TRUE;
}

BOOL SaveConfiguration0(HWND hDlg)
{
	cs_save(hDlg, cur_cs);

	if (!IsValidConfiguration0(hDlg)) return FALSE;

	memcpy(&configuracion->mainScreenConfig->cs, &ms, sizeof(CConfigurationScreen));
	memcpy(&configuracion->bottomBarConfig->cs,  &bb, sizeof(CConfigurationScreen));
	memcpy(&configuracion->topBarConfig->cs,     &tb, sizeof(CConfigurationScreen));

	return TRUE;
}
