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

				CConfigurationScreen *cs = configuracion->bottomBarConfig;

				SetDlgItemInt(hDlg, IDC_EDIT_BB_ICON_WIDTH,		cs->iconWidthXML,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_ICONS_PER_ROW,	cs->iconsPerRowXML,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_TEXT_HEIGHT,	cs->fontSize,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_TEXT_OFFSET,	cs->fontOffset,		TRUE);

				SetDlgItemHex(hDlg, IDC_EDIT_BB_TEXT_COLOR,		cs->fontColor);

				SendMessage(GetDlgItem(hDlg, IDC_CHECK_BB_TEXT_BOLD), BM_SETCHECK, cs->fontBold ? BST_CHECKED : BST_UNCHECKED, 0);

				SetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR1,	cs->backColor1);
				SetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR2,	cs->backColor2);

				SetDlgItemInt(hDlg, IDC_EDIT_BB_MINVSPACE,		cs->minVerticalSpace,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_ADDHSPACE,		cs->additionalHorizontalSpace,	TRUE);

				SetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_LEFT,	cs->offset.left,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_TOP,		cs->offset.top,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_RIGHT,	cs->offset.right,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_BOTTOM,	cs->offset.bottom,	TRUE);

			} else {
				MessageBox(0, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			int rgbCurrent;
			COLORREF nextColor;
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_BB_TEXT_COLOR:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BB_TEXT_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BB_TEXT_COLOR, nextColor);
				}
				break;
			case IDC_BUTTON_BB_BACK_COLOR1:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR1, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR1, nextColor);
				}
				break;
			case IDC_BUTTON_BB_BACK_COLOR2:
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR2, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR2, nextColor);
				}
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration1(HWND hDlg)
{
	int iconWidth, iconsPerRow, textHeight, textOffset, textColor, textBold, backColor1, backColor2, minVSpace, addHSpace, offset_left, offset_top, offset_right, offset_bottom;

	CConfigurationScreen *cs = configuracion->bottomBarConfig;

	iconWidth	= GetDlgItemInt(hDlg, IDC_EDIT_BB_ICON_WIDTH,		NULL, TRUE);
	iconsPerRow	= GetDlgItemInt(hDlg, IDC_EDIT_BB_ICONS_PER_ROW,	NULL, TRUE);
	textHeight	= GetDlgItemInt(hDlg, IDC_EDIT_BB_TEXT_HEIGHT,		NULL, TRUE);
	textOffset	= GetDlgItemInt(hDlg, IDC_EDIT_BB_TEXT_OFFSET,		NULL, TRUE);

	textColor	= GetDlgItemHex(hDlg, IDC_EDIT_BB_TEXT_COLOR,		NULL);

	textBold = SendMessage(GetDlgItem(hDlg, IDC_CHECK_BB_TEXT_BOLD), BM_GETCHECK, 0, 0) == BST_CHECKED;

	backColor1	= GetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR1,		NULL);
	backColor2	= GetDlgItemHex(hDlg, IDC_EDIT_BB_BACK_COLOR2,		NULL);

	minVSpace	= GetDlgItemInt(hDlg, IDC_EDIT_BB_MINVSPACE,		NULL, TRUE);
	addHSpace	= GetDlgItemInt(hDlg, IDC_EDIT_BB_ADDHSPACE,		NULL, TRUE);

	offset_left		= GetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_LEFT,		NULL, TRUE);
	offset_top		= GetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_TOP,		NULL, TRUE);
	offset_right	= GetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_RIGHT,		NULL, TRUE);
	offset_bottom	= GetDlgItemInt(hDlg, IDC_EDIT_BB_OFFSET_BOTTOM,	NULL, TRUE);

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
	if (minVSpace < 0 || minVSpace > 256) {
		MessageBox(hDlg, TEXT("Minimum vertical space value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (addHSpace < 0 || addHSpace > 256) {
		MessageBox(hDlg, TEXT("Additional horizontal space value is not valid!"), TEXT("Error"), MB_OK);
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

	cs->iconWidthXML				= iconWidth;
	cs->iconsPerRowXML				= iconsPerRow;
	cs->fontSize					= textHeight;
	cs->fontOffset					= textOffset;
	cs->fontColor					= textColor;
	cs->fontBold					= textBold;
	cs->backColor1					= backColor1;
	cs->backColor2					= backColor2;
	cs->minVerticalSpace			= minVSpace;
	cs->additionalHorizontalSpace	= addHSpace;
	cs->offset.left					= offset_left;
	cs->offset.top					= offset_top;
	cs->offset.right				= offset_right;
	cs->offset.bottom				= offset_bottom;

	return TRUE;
}
