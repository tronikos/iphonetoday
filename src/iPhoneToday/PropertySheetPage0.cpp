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

			// Property sheet will destroy part of the soft key bar,
			// therefore we create an empty menu bar here
			// Only required on the first property sheet
            SHMENUBARINFO shmbi;
            shmbi.cbSize = sizeof(shmbi);
            shmbi.hwndParent = hDlg;
            shmbi.dwFlags = SHCMBF_EMPTYBAR;
            SHCreateMenuBar(&shmbi);

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

				TCHAR str[MAX_PATH];
			
				// Configuramos el elemento Icon Width
				swprintf(str, L"%d", configuracion->anchoIconoXML);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_ICON_WIDTH), str);

				// Configuramos el elemento Icons for Row
				swprintf(str, L"%d", configuracion->numeroIconosXML);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_ICONS_ROW), str);

				// Configuramos el elemento Heigth Portrait
				swprintf(str, L"%d", configuracion->altoPantallaP);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_HEIGTH_P), str);

				// Configuramos el elemento Heigth Landscape
				swprintf(str, L"%d", configuracion->altoPantallaL);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_HEIGTH_L), str);

				// Configuramos el elemento Velocity
				swprintf(str, L"%d", configuracion->velMaxima);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_VELOCITY), str);

				// Configuramos el elemento Factor Movement
				swprintf(str, L"%d", configuracion->factorMovimiento);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_FACTOR_MOV), str);

				// Configuramos el elemento Font Size
				swprintf(str, L"%d", configuracion->fontSize);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_FONT_SIZE), str);

				// Configuramos el elemento Time of Vibration
				swprintf(str, L"%d", configuracion->vibrateOnLaunchIcon);
				SetWindowText(GetDlgItem(hDlg, IDC_EDIT_TIME_VIBRATION), str);

				// Configuramos los checks
				if (configuracion->ignoreRotation > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION), BM_SETCHECK, BST_UNCHECKED, 0);
				}

				if (configuracion->closeOnLaunchIcon > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOSEONLAUNCH), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOSEONLAUNCH), BM_SETCHECK, BST_UNCHECKED, 0);
				}

			} else {
				MessageBox(0, L"Configuracion vacio!", 0, MB_OK);
			}

			
        }
        return TRUE; 
	case WM_VSCROLL:
		{
			// Get the current scroll bar position
			SCROLLINFO si = {0};
			si.cbSize = sizeof (si);
			si.fMask = SIF_ALL;
			GetScrollInfo (hDlg, SB_VERT, &si);

			// Save the position for comparison later on
			int currentPos = si.nPos;

			// Adjust the scrollbar position based upon
			// the action the user took
			switch (LOWORD (wParam))
			{
				// user clicked the HOME keyboard key
				case SB_TOP:
					si.nPos = si.nMin;
					break;
				// user clicked the END keyboard key
				case SB_BOTTOM:
					si.nPos = si.nMax;
					break;

				// user clicked the top arrow
				case SB_LINEUP:
					si.nPos -= 1;
					break;

				// user clicked the bottom arrow
				case SB_LINEDOWN:
					si.nPos += 1;
					break;

				// user clicked the scroll bar shaft above the scroll box
				case SB_PAGEUP:
					si.nPos -= si.nPage;
					break;

				// user clicked the scroll bar shaft below the scroll box
				case SB_PAGEDOWN:
					si.nPos += si.nPage;
					break;

				// user dragged the scroll box
				case SB_THUMBTRACK:
					si.nPos = si.nTrackPos;
					break;
			}

			// Set the position and then retrieve it. Due to adjustments
			// by Windows it may not be the same as the value set.
			si.fMask = SIF_POS;
			SetScrollInfo (hDlg, SB_VERT, &si, TRUE);
			GetScrollInfo (hDlg, SB_VERT, &si);

			// If the position has changed
			if (si.nPos != currentPos)
			{ 
				// Scroll the window contents
				ScrollWindowEx(hDlg, 0, currentPos - si.nPos,
					NULL, NULL, NULL, NULL,
					SW_SCROLLCHILDREN | SW_INVALIDATE);
			}
		}
		break;
    }

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration0(HWND hDlg)
{
	// Recuperamos los valores introducidos
	TCHAR strIconWidth[MAX_PATH];
	TCHAR strIconsRow[MAX_PATH];
	TCHAR strScreenHeigthP[MAX_PATH];
	TCHAR strScreenHeigthL[MAX_PATH];
	TCHAR strVelocity[MAX_PATH];
	TCHAR strFactorMov[MAX_PATH];
	TCHAR strFontSize[MAX_PATH];
	TCHAR strVibrateOnLaunchIcon[MAX_PATH];


	int iconWidth;
	int iconsRow;
	int screenHeigthP;
	int screenHeigthL;
	int velocity;
	int factorMov;
	int fontSize;
	int ignoreRotation;
	int closeOnLaunchIcon;
	int vibrateOnLaunchIcon;

	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_ICON_WIDTH), strIconWidth, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_ICONS_ROW), strIconsRow, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_HEIGTH_P), strScreenHeigthP, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_HEIGTH_L), strScreenHeigthL, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_VELOCITY), strVelocity, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_FACTOR_MOV), strFactorMov, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_FONT_SIZE), strFontSize, MAX_PATH);
	GetWindowText(GetDlgItem(hDlg, IDC_EDIT_TIME_VIBRATION), strVibrateOnLaunchIcon, MAX_PATH);
	if (SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION), BM_GETCHECK, 0, 0) == BST_CHECKED) {
		ignoreRotation = 1;
	} else {
		ignoreRotation = 0;
	}
	if (SendMessage(GetDlgItem(hDlg, IDC_CHECK_CLOSEONLAUNCH), BM_GETCHECK, 0, 0) == BST_CHECKED) {
		closeOnLaunchIcon = 1;
	} else {
		closeOnLaunchIcon = 0;
	}

	iconWidth = _wtoi(strIconWidth);
	iconsRow = _wtoi(strIconsRow);
	screenHeigthP = _wtoi(strScreenHeigthP);
	screenHeigthL = _wtoi(strScreenHeigthL);
	velocity = _wtoi(strVelocity);
	factorMov = _wtoi(strFactorMov);
	fontSize = _wtoi(strFontSize);
	vibrateOnLaunchIcon = _wtoi(strVibrateOnLaunchIcon);

	// Comprobaciones
	if (iconWidth < 20 || iconWidth > 200) {
		MessageBox(hDlg, TEXT("Icon Width value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (iconsRow < 1 || iconsRow > 32) {
		MessageBox(hDlg, TEXT("Icons for Row value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (screenHeigthP <= 0) {
		MessageBox(hDlg, TEXT("Screen Heigth value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (screenHeigthL <= 0) {
		MessageBox(hDlg, TEXT("Screen Heigth value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (velocity < 10 || velocity > 1000) {
		MessageBox(hDlg, TEXT("Velocity value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (factorMov < 0 || factorMov > 40) {
		MessageBox(hDlg, TEXT("Factor of Movement value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	if (fontSize < 0 || fontSize > 20) {
		MessageBox(hDlg, TEXT("Font Size value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	if (vibrateOnLaunchIcon < 0 || vibrateOnLaunchIcon > 500) {
		MessageBox(hDlg, TEXT("Time of vibration value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	// Ponemos los nuevos valores
	configuracion->anchoIconoXML = iconWidth;
	configuracion->numeroIconosXML = iconsRow;
	configuracion->altoPantallaP = screenHeigthP;
	configuracion->altoPantallaL = screenHeigthL;
	configuracion->velMaxima = velocity;
	configuracion->factorMovimiento = factorMov;
	configuracion->fontSize = fontSize;
	configuracion->ignoreRotation = ignoreRotation;
	configuracion->closeOnLaunchIcon = closeOnLaunchIcon;
	configuracion->vibrateOnLaunchIcon = vibrateOnLaunchIcon;

	return TRUE;
}