//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage0.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[0] = hDlg;

			if (FindWindow(L"MS_SIPBUTTON", NULL) != NULL) {
				// Property sheet will destroy part of the soft key bar,
				// therefore we create an empty menu bar here
				// Only required on the first property sheet
				SHMENUBARINFO shmbi;
				shmbi.cbSize = sizeof(shmbi);
				shmbi.hwndParent = hDlg;
				shmbi.dwFlags = SHCMBF_EMPTYBAR;
				SHCreateMenuBar(&shmbi);
			}

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

				CConfigurationScreen *cs = configuracion->mainScreenConfig;

				SetDlgItemInt(hDlg, IDC_EDIT_MS_ICON_WIDTH,		cs->iconWidthXML,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_ICONS_PER_ROW,	cs->iconsPerRowXML,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_TEXT_HEIGHT,	cs->fontSize,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_TEXT_OFFSET,	cs->fontOffset,		TRUE);

				SetDlgItemHex(hDlg, IDC_EDIT_MS_TEXT_COLOR,		cs->fontColor);

				SendMessage(GetDlgItem(hDlg, IDC_CHECK_MS_TEXT_BOLD), BM_SETCHECK, cs->fontBold ? BST_CHECKED : BST_UNCHECKED, 0);

				SetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR1,	cs->backColor1);
				SetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR2,	cs->backColor2);

				SetDlgItemInt(hDlg, IDC_EDIT_MS_MINHSPACE,		cs->minHorizontalSpace,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_ADDVSPACE,		cs->additionalVerticalSpace,	TRUE);

				SetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_LEFT,	cs->offset.left,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_TOP,		cs->offset.top,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_RIGHT,	cs->offset.right,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_BOTTOM,	cs->offset.bottom,	TRUE);

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
			case IDC_BUTTON_MS_TEXT_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_MS_TEXT_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_MS_TEXT_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_MS_BACK_COLOR1:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR1, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR1, nextColor);
				}
				break;
			case IDC_BUTTON_MS_BACK_COLOR2:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR2, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR2, nextColor);
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

BOOL SaveConfiguration0(HWND hDlg)
{
	int iconWidth, iconsPerRow, textHeight, textOffset, textColor, textBold, backColor1, backColor2, minHSpace, addVSpace, offset_left, offset_top, offset_right, offset_bottom;

	CConfigurationScreen *cs = configuracion->mainScreenConfig;

	iconWidth	= GetDlgItemInt(hDlg, IDC_EDIT_MS_ICON_WIDTH,		NULL, TRUE);
	iconsPerRow	= GetDlgItemInt(hDlg, IDC_EDIT_MS_ICONS_PER_ROW,	NULL, TRUE);
	textHeight	= GetDlgItemInt(hDlg, IDC_EDIT_MS_TEXT_HEIGHT,		NULL, TRUE);
	textOffset	= GetDlgItemInt(hDlg, IDC_EDIT_MS_TEXT_OFFSET,		NULL, TRUE);

	textColor	= GetDlgItemHex(hDlg, IDC_EDIT_MS_TEXT_COLOR,		NULL);

	textBold = SendMessage(GetDlgItem(hDlg, IDC_CHECK_MS_TEXT_BOLD), BM_GETCHECK, 0, 0) == BST_CHECKED;

	backColor1	= GetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR1,		NULL);
	backColor2	= GetDlgItemHex(hDlg, IDC_EDIT_MS_BACK_COLOR2,		NULL);

	minHSpace	= GetDlgItemInt(hDlg, IDC_EDIT_MS_MINHSPACE,		NULL, TRUE);
	addVSpace	= GetDlgItemInt(hDlg, IDC_EDIT_MS_ADDVSPACE,		NULL, TRUE);

	offset_left		= GetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_LEFT,		NULL, TRUE);
	offset_top		= GetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_TOP,		NULL, TRUE);
	offset_right	= GetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_RIGHT,		NULL, TRUE);
	offset_bottom	= GetDlgItemInt(hDlg, IDC_EDIT_MS_OFFSET_BOTTOM,	NULL, TRUE);

	if (iconWidth < 0 || iconWidth > 256) {
		MessageBox(hDlg, TEXT("Icon width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (iconsPerRow < 0 || iconsPerRow > 32) {
		MessageBox(hDlg, TEXT("Icons per row value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (textHeight < 0 || textHeight > 100) {
		MessageBox(hDlg, TEXT("Text height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (textOffset < -256 || textOffset > 256) {
		MessageBox(hDlg, TEXT("Text offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (minHSpace < 0 || minHSpace > 256) {
		MessageBox(hDlg, TEXT("Minimum horizontal space value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (addVSpace < 0 || addVSpace > 256) {
		MessageBox(hDlg, TEXT("Additional vertical space value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (offset_left < 0 || offset_left > 256) {
		MessageBox(hDlg, TEXT("Offset left value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (offset_top < 0 || offset_top > 256) {
		MessageBox(hDlg, TEXT("Offset top value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (offset_right < 0 || offset_right > 256) {
		MessageBox(hDlg, TEXT("Offset right value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (offset_bottom < 0 || offset_bottom > 256) {
		MessageBox(hDlg, TEXT("Offset bottom value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	cs->iconWidthXML			= iconWidth;
	cs->iconsPerRowXML			= iconsPerRow;
	cs->fontSize				= textHeight;
	cs->fontOffset				= textOffset;
	cs->fontColor				= textColor;
	cs->fontBold				= textBold;
	cs->backColor1				= backColor1;
	cs->backColor2				= backColor2;
	cs->minHorizontalSpace		= minHSpace;
	cs->additionalVerticalSpace	= addVSpace;
	cs->offset.left				= offset_left;
	cs->offset.top				= offset_top;
	cs->offset.right			= offset_right;
	cs->offset.bottom			= offset_bottom;

	return TRUE;
}
