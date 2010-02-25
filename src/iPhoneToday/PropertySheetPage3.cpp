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
			// Initialize handle to property sheet
			g_hDlg[3] = hDlg;

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
				SetDlgItemInt(hDlg, IDC_EDIT_HEADER_HEIGHT,		configuracion->headerFontSize,		TRUE);
				SetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR,		configuracion->headerFontColor);
				SetDlgItemInt(hDlg, IDC_EDIT_HEADER_WEIGHT,		configuracion->headerFontWeight,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_HEADER_OFFSET,		configuracion->headerOffset,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DIAMETER,	configuracion->circlesDiameter,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_DISTANCE,	configuracion->circlesDistance,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_CIRCLES_OFFSET,	configuracion->circlesOffset,		TRUE);
				SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF,		configuracion->bubble_notif);
				SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE,		configuracion->bubble_state);
				SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM,		configuracion->bubble_alarm);
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		{
			TCHAR str[MAX_PATH];
			TCHAR browseDir[MAX_PATH];
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_BUBBLE_NOTIF:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, str);
				}
				break;
			case IDC_BUTTON_BUBBLE_STATE:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, str);
				}
				break;
			case IDC_BUTTON_BUBBLE_ALARM:
				GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, str, MAX_PATH);
				getPathFromFile(str, browseDir);
				if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, str, browseDir)) {
					SetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, str);
				}
				break;
			case IDC_BUTTON_HEADER_COLOR:
				int rgbCurrent;
				COLORREF nextColor;
				rgbCurrent = GetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR, NULL);
				if (ColorSelector(rgbCurrent, &nextColor)) {
					SetDlgItemHex(hDlg, IDC_EDIT_HEADER_COLOR, nextColor);
				}
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration3(HWND hDlg)
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

	configuracion->headerFontSize = headerHeight;
	configuracion->headerFontColor = headerColor;
	configuracion->headerFontWeight = headerWeight;
	configuracion->headerOffset = headerOffset;
	configuracion->circlesDiameter = circlesDiameter;
	configuracion->circlesDistance = circlesDistance;
	configuracion->circlesOffset = circlesOffset;

	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_NOTIF, configuracion->bubble_notif, CountOf(configuracion->bubble_notif));
	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_STATE, configuracion->bubble_state, CountOf(configuracion->bubble_state));
	GetDlgItemText(hDlg, IDC_EDIT_BUBBLE_ALARM, configuracion->bubble_alarm, CountOf(configuracion->bubble_alarm));

	return TRUE;
}
