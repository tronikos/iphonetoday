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
			case IDC_BUTTON_HEADER_TEXT:
				memcpy(&cfs, &hcfs, sizeof(ChooseFontSettings));
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGFONT), hDlg, (DLGPROC)ChooseFontProc);
				if (bChooseFontOK) {
					memcpy(&hcfs, &cfs, sizeof(ChooseFontSettings));
				}
				break;
			}
		}
		break;
	}

	return DefOptionWindowProc(hDlg, TAB_HEADER, uMsg, wParam, lParam);
}

BOOL IsValidConfigurationHeader(HWND hDlg)
{
	int headerOffset, circlesDiameter, circlesDistance, circlesOffset;

	headerOffset	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,	NULL, TRUE);
	circlesDiameter	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,NULL, TRUE);
	circlesDistance	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,NULL, TRUE);
	circlesOffset	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	NULL, TRUE);

	if (hcfs.height < 0 || hcfs.height > 128) {
		MessageBox(hDlg, TEXT("Header height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (hcfs.weight < 0 || hcfs.weight > 1000) {
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

BOOL SaveConfigurationHeader(HWND hDlg)
{
	if (!IsValidConfigurationHeader(hDlg)) return FALSE;

	int headerOffset, circlesDiameter, circlesDistance, circlesOffset;

	headerOffset	= GetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,	NULL, TRUE);
	circlesDiameter	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,NULL, TRUE);
	circlesDistance	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,NULL, TRUE);
	circlesOffset	= GetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	NULL, TRUE);

	wcscpy(configuracion->headerTextFacename, hcfs.facename);
	configuracion->headerTextColor = hcfs.color;
	configuracion->headerTextSize = hcfs.height;
	configuracion->headerTextWeight = hcfs.weight;
	configuracion->headerTextShadow = hcfs.shadow;
	configuracion->headerTextRoundRect = hcfs.roundrect;

	configuracion->headerOffset = headerOffset;
	configuracion->circlesDiameter = circlesDiameter;
	configuracion->circlesDistance = circlesDistance;
	configuracion->circlesOffset = circlesOffset;

	return TRUE;
}
