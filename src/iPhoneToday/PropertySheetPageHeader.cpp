//////////////////////////////////////////////////////////////////////////////
// PropertySheetPageHeader.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"
#include "ChooseFont.h"

ChooseFontSettings hcfs;

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialogHeader(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, TAB_HEADER);

			wcscpy(hcfs.facename, configuracion->headerTextFacename);
			hcfs.color = configuracion->headerTextColor;
			hcfs.height = configuracion->headerTextSize;
			hcfs.weight = configuracion->headerTextWeight;
			hcfs.shadow = configuracion->headerTextShadow;
			hcfs.roundrect = configuracion->headerTextRoundRect;

			SetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,				configuracion->headerOffset,				TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,			configuracion->circlesDiameter,				TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER_ACTIVE,	configuracion->circlesDiameterActivePerc,	TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,			configuracion->circlesDistance,				TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,			configuracion->circlesOffset,				TRUE);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_ALIGN_TOP), BM_SETCHECK, configuracion->circlesAlignTop? BST_CHECKED : BST_UNCHECKED, 0);

			SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_ACTIVE,	configuracion->circlesColorActive);
			SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_INACTIVE,configuracion->circlesColorInactive);
			SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_OUTER,	configuracion->circlesColorOuter);

			SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_SINGLE_TAP), BM_SETCHECK, configuracion->circlesSingleTap ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_DOUBLE_TAP), BM_SETCHECK, configuracion->circlesDoubleTap ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		return TRUE;
	case WM_COMMAND:
		int rgbCurrent;
		COLORREF nextColor;
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_HEADER_TEXT:
			memcpy(&cfs, &hcfs, sizeof(ChooseFontSettings));
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGFONT), hDlg, (DLGPROC)ChooseFontProc);
			if (bChooseFontOK) {
				memcpy(&hcfs, &cfs, sizeof(ChooseFontSettings));
			}
			break;
		case IDC_BUTTON_CIRCLES_COLOR_ACTIVE:
			rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_ACTIVE, NULL);
			if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
				SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_ACTIVE, nextColor);
			}
			break;
		case IDC_BUTTON_CIRCLES_COLOR_INACTIVE:
			rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_INACTIVE, NULL);
			if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
				SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_INACTIVE, nextColor);
			}
			break;
		case IDC_BUTTON_CIRCLES_COLOR_OUTER:
			rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_OUTER, NULL);
			if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
				SetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_OUTER, nextColor);
			}
			break;
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_HEADER, uMsg, wParam, lParam);
}

/*BOOL IsValidConfigurationHeader(HWND hDlg)
{
	int headerOffset, circlesDiameter, circlesDiameterActive, circlesDistance, circlesOffset;

	headerOffset			= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,			NULL, TRUE);
	circlesDiameter			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,		NULL, TRUE);
	circlesDiameterActive	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER_ACTIVE,	NULL, TRUE);
	circlesDistance			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,		NULL, TRUE);
	circlesOffset			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,			NULL, TRUE);

	if (hcfs.height < 0 || hcfs.height > 128) {
		MessageBox(hDlg, TEXT("Header height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (hcfs.weight < 0 || hcfs.weight > 1000) {
		MessageBox(hDlg, TEXT("Header weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (headerOffset < -128 || headerOffset > 128) {
		MessageBox(hDlg, TEXT("Header offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (circlesDiameter < 0 || circlesDiameter > 128) {
		MessageBox(hDlg, TEXT("Circles diameter value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (circlesDiameterActive < 0 || circlesDiameterActive > 1000) {
		MessageBox(hDlg, TEXT("Active circles diameter value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (circlesDistance < 0 || circlesDistance > 128) {
		MessageBox(hDlg, TEXT("Circles distance value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (circlesOffset < -800 || circlesDistance > 800) {
		MessageBox(hDlg, TEXT("Circles offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}*/

BOOL SaveConfigurationHeader(HWND hDlg)
{
//	if (!IsValidConfigurationHeader(hDlg)) return FALSE;

	int headerOffset, circlesDiameter, circlesDiameterActive, circlesDistance, circlesOffset;

	headerOffset			= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,			NULL, TRUE);
	circlesDiameter			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,		NULL, TRUE);
	circlesDiameterActive	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER_ACTIVE,	NULL, TRUE);
	circlesDistance			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,		NULL, TRUE);
	circlesOffset			= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,			NULL, TRUE);

	wcscpy(configuracion->headerTextFacename, hcfs.facename);
	configuracion->headerTextColor = hcfs.color;
	configuracion->headerTextSize = hcfs.height;
	configuracion->headerTextWeight = hcfs.weight;
	configuracion->headerTextShadow = hcfs.shadow;
	configuracion->headerTextRoundRect = hcfs.roundrect;

	configuracion->headerOffset = headerOffset;
	configuracion->circlesDiameter = circlesDiameter;
	configuracion->circlesDiameterActivePerc = circlesDiameterActive;
	configuracion->circlesDistance = circlesDistance;
	configuracion->circlesOffset = circlesOffset;

	configuracion->circlesAlignTop = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_ALIGN_TOP), BM_GETCHECK, 0, 0) == BST_CHECKED;

	configuracion->circlesColorActive	= GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_ACTIVE, NULL);
	configuracion->circlesColorInactive	= GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_INACTIVE, NULL);
	configuracion->circlesColorOuter	= GetDlgItemHex(hDlg, IDC_EDIT_CIRCLES_COLOR_OUTER, NULL);

	configuracion->circlesSingleTap = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_SINGLE_TAP), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->circlesDoubleTap = SendMessage(GetDlgItem(hDlg, IDC_CHECK_CIRCLES_DOUBLE_TAP), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
