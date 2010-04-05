//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage0.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"
#include "ChooseFont.h"

LRESULT CALLBACK ScreenBackSettingsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

ConfigurationScreen ms, bb, tb;
ConfigurationScreen *cur_cs;
BOOL isStaticBar = FALSE;

void cs_enable(HWND hDlg, BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ICON_WIDTH),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ICONS_PER_ROW),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_TEXT_OFFSET),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_MINHSPACE),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_ADDVSPACE),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_LEFT),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_TOP),		bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_RIGHT),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM),	bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_TEXT),			bEnable);
	EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_BACK),			bEnable);
}

void cs_load(HWND hDlg, ConfigurationScreen *cs)
{
	if (cs != NULL) {
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ICON_WIDTH,		cs->iconWidthXML,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ICONS_PER_ROW,	cs->iconsPerRowXML,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_OFFSET,	cs->textOffset,		TRUE);

		SetDlgItemInt(hDlg, IDC_EDIT_CS_MINHSPACE,		cs->minHorizontalSpace,			TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_ADDVSPACE,		cs->additionalVerticalSpace,	TRUE);

		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_LEFT,	cs->offset.left,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_TOP,		cs->offset.top,		TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_RIGHT,	cs->offset.right,	TRUE);
		SetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM,	cs->offset.bottom,	TRUE);

		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_SHRINKTOFIT), BM_SETCHECK, cs->shrinkToFit ? BST_CHECKED : BST_UNCHECKED, 0);
	}
	cs_enable(hDlg, TRUE);
}

void cs_save(HWND hDlg, ConfigurationScreen *cs)
{
	if (cs != NULL) {
		cs->iconWidthXML	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ICON_WIDTH,		NULL, TRUE);
		cs->iconsPerRowXML	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ICONS_PER_ROW,	NULL, TRUE);
		cs->textOffset		= GetDlgItemInt(hDlg, IDC_EDIT_CS_TEXT_OFFSET,		NULL, TRUE);

		cs->minHorizontalSpace		= GetDlgItemInt(hDlg, IDC_EDIT_CS_MINHSPACE,	NULL, TRUE);
		cs->additionalVerticalSpace	= GetDlgItemInt(hDlg, IDC_EDIT_CS_ADDVSPACE,	NULL, TRUE);

		cs->offset.left		= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_LEFT,		NULL, TRUE);
		cs->offset.top		= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_TOP,		NULL, TRUE);
		cs->offset.right	= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_RIGHT,		NULL, TRUE);
		cs->offset.bottom	= GetDlgItemInt(hDlg, IDC_EDIT_CS_OFFSET_BOTTOM,	NULL, TRUE);

		cs->shrinkToFit		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_SHRINKTOFIT), BM_GETCHECK, 0, 0) == BST_CHECKED;
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

			isStaticBar = FALSE;
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
			switch (LOWORD(wParam))
			{
			case IDC_COMBO_CS:
				if (HIWORD(wParam) == CBN_SELCHANGE) {
					cs_save(hDlg, cur_cs);
					ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_SHRINKTOFIT), SW_HIDE);
					isStaticBar = FALSE;
					TCHAR str[MAX_PATH];
					GetDlgItemText(hDlg, IDC_COMBO_CS, str, MAX_PATH);
					if (lstrcmpi(str, L"Mainscreen") == 0) {
						cur_cs = &ms;
						cs_load(hDlg, cur_cs);
					} else if (lstrcmpi(str, L"Bottombar") == 0) {
						cur_cs = &bb;
						cs_load(hDlg, cur_cs);
						ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_SHRINKTOFIT), SW_SHOW);
						isStaticBar = TRUE;
					} else if (lstrcmpi(str, L"Topbar") == 0) {
						cur_cs = &tb;
						cs_load(hDlg, cur_cs);
						ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_SHRINKTOFIT), SW_SHOW);
						isStaticBar = TRUE;
					} else {
						cs_enable(hDlg, FALSE);
					}
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
				break;
			case IDC_BUTTON_CS_TEXT:
				wcscpy(cfs.facename, cur_cs->textFacename);
				cfs.color = cur_cs->textColor;
				cfs.height = cur_cs->textHeightXML;
				cfs.weight = cur_cs->textWeight;
				cfs.shadow = cur_cs->textShadow;
				cfs.roundrect = cur_cs->textRoundRect;
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGFONT), hDlg, (DLGPROC)ChooseFontProc);
				if (bChooseFontOK) {
					wcscpy(cur_cs->textFacename, cfs.facename);
					cur_cs->textColor = cfs.color;
					cur_cs->textHeightXML = cfs.height;
					cur_cs->textWeight = cfs.weight;
					cur_cs->textShadow = cfs.shadow;
					cur_cs->textRoundRect = cfs.roundrect;
				}
				break;
			case IDC_BUTTON_CS_BACK:
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGBACK), hDlg, (DLGPROC)ScreenBackSettingsProc);
				break;
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
	if (cs->textWeight < 0 || cs->textWeight > 1000) {
		MessageBox(hDlg, TEXT("Text weight value is not valid!"), TEXT("Error"), MB_OK);
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

LRESULT CALLBACK ScreenBackSettingsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndKB = NULL;

	int rgbCurrent;
	COLORREF nextColor;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CAPTIONOKBTN);
		if (FindWindow(L"MS_SIPBUTTON", NULL) == NULL) {
//			SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CONTEXTHELP);
			hwndKB = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_KB_BUTTON), hDlg, (DLGPROC) KBButtonDlgProc);
		}

		if (!isStaticBar) {
			ShowWindow(GetDlgItem(hDlg, IDC_STATIC_CS_WALLPAPER),		SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_EDIT_CS_BACK_WALLPAPER),	SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_BUTTON_CS_BACK_WALLPAPER),	SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_ALPHABLEND),	SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_CENTER),		SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_WIDTH),	SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_HEIGHT),	SW_HIDE);
		}

		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_GRADIENT), BM_SETCHECK, cur_cs->backGradient ? BST_CHECKED : BST_UNCHECKED, 0);
		SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1, cur_cs->backColor1);
		SetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2, cur_cs->backColor2);
		SetDlgItemText(hDlg, IDC_EDIT_CS_BACK_WALLPAPER, cur_cs->backWallpaper);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_ALPHABLEND), BM_SETCHECK, cur_cs->backWallpaperAlphaBlend ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_CENTER), BM_SETCHECK, cur_cs->backWallpaperCenter ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_WIDTH), BM_SETCHECK, cur_cs->backWallpaperFitWidth ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_HEIGHT), BM_SETCHECK, cur_cs->backWallpaperFitHeight ? BST_CHECKED : BST_UNCHECKED, 0);
		return TRUE;
	case WM_MOVE:
		PositionKBButton(hwndKB, hDlg);
		break;
	case WM_ACTIVATE:
		if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE) {
			EnableWindow(hwndKB, TRUE);
		} else if (!::IsChild(hDlg, (HWND)lParam)) {
			EnableWindow(hwndKB, FALSE);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			cur_cs->backGradient = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_GRADIENT), BM_GETCHECK, 0, 0) == BST_CHECKED;
			cur_cs->backColor1 = GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR1, NULL);
			cur_cs->backColor2 = GetDlgItemHex(hDlg, IDC_EDIT_CS_BACK_COLOR2, NULL);
			GetDlgItemText(hDlg, IDC_EDIT_CS_BACK_WALLPAPER, cur_cs->backWallpaper, CountOf(cur_cs->backWallpaper));
			cur_cs->backWallpaperAlphaBlend = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_ALPHABLEND), BM_GETCHECK, 0, 0) == BST_CHECKED;
			cur_cs->backWallpaperCenter = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_CENTER), BM_GETCHECK, 0, 0) == BST_CHECKED;
			cur_cs->backWallpaperFitWidth = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_WIDTH), BM_GETCHECK, 0, 0) == BST_CHECKED;
			cur_cs->backWallpaperFitHeight = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CS_BACK_FIT_HEIGHT), BM_GETCHECK, 0, 0) == BST_CHECKED;
			EndDialog(hDlg, LOWORD(wParam));
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
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
		case IDC_BUTTON_CS_BACK_WALLPAPER:
			TCHAR str[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			GetDlgItemText(hDlg, IDC_EDIT_CS_BACK_WALLPAPER, str, MAX_PATH);
			configuracion->getAbsolutePath(fullPath, MAX_PATH, str);
			getPathFromFile(fullPath, browseDir);
			if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
				SetDlgItemText(hDlg, IDC_EDIT_CS_BACK_WALLPAPER, str);
			}
			break;
		}
		break;
//	case WM_HELP:
//		ToggleKeyboard();
//		break;
	}
	return FALSE;
}
