//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage3.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog3(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 3);

			SetDlgItemInt(hDlg, IDC_EDIT_HEADER_HEIGHT,		configuracion->headerFontSize,		TRUE);
			SetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR,		configuracion->headerFontColor);
			SetDlgItemInt(hDlg, IDC_EDIT_HEADER_WEIGHT,		configuracion->headerFontWeight,	TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,		configuracion->headerOffset,		TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,	configuracion->circlesDiameter,		TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,	configuracion->circlesDistance,		TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	configuracion->circlesOffset,		TRUE);
		}
		return TRUE;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_HEADER_COLOR:
				int rgbCurrent;
				COLORREF nextColor;
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR, NULL);
				if (ColorSelector(hDlg, rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR, nextColor);
				}
				break;
			}
		}
		return 0;
	}

	return DefOptionWindowProc(hDlg, 3, uMsg, wParam, lParam);
}

BOOL IsValidConfiguration3(HWND hDlg)
{
	int headerHeight, headerColor, headerWeight, headerOffset, circlesDiameter, circlesDistance, circlesOffset;

	headerHeight	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_HEIGHT,	NULL, TRUE);
	headerColor		= GetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR,	NULL);
	headerWeight	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_WEIGHT,	NULL, TRUE);
	headerOffset	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,	NULL, TRUE);
	circlesDiameter	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,NULL, TRUE);
	circlesDistance	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,NULL, TRUE);
	circlesOffset	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	NULL, TRUE);

	if (headerHeight < 0 || headerHeight > 128) {
		MessageBox(hDlg, TEXT("Header height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (headerWeight < 0 || headerWeight > 1000) {
		MessageBox(hDlg, TEXT("Header weight value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (headerOffset < 0 || headerOffset > 128) {
		MessageBox(hDlg, TEXT("Header offset value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (circlesDiameter < 0 || circlesDiameter > 128) {
		MessageBox(hDlg, TEXT("Circles diameter value is not valid!"), TEXT("Error"), MB_OK);
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
}

BOOL SaveConfiguration3(HWND hDlg)
{
	if (!IsValidConfiguration3(hDlg)) return FALSE;

	int headerHeight, headerColor, headerWeight, headerOffset, circlesDiameter, circlesDistance, circlesOffset;

	headerHeight	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_HEIGHT,	NULL, TRUE);
	headerColor		= GetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR,	NULL);
	headerWeight	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_WEIGHT,	NULL, TRUE);
	headerOffset	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,	NULL, TRUE);
	circlesDiameter	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,NULL, TRUE);
	circlesDistance	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,NULL, TRUE);
	circlesOffset	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	NULL, TRUE);

	configuracion->headerFontSize = headerHeight;
	configuracion->headerFontColor = headerColor;
	configuracion->headerFontWeight = headerWeight;
	configuracion->headerOffset = headerOffset;
	configuracion->circlesDiameter = circlesDiameter;
	configuracion->circlesDistance = circlesDistance;
	configuracion->circlesOffset = circlesOffset;

	return TRUE;
}
