//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage10.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"
#include "VersionNo.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog10(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[10] = hDlg;

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

			SendMessage(GetDlgItem(hDlg, IDC_STATICAPPVER), WM_SETTEXT, 0, (LPARAM)LSTRPRODUCTVER);
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDC_STATICURL:
				SHELLEXECUTEINFO sei;
				memset(&sei, 0, sizeof(sei));
				sei.cbSize = sizeof(sei);
				sei.lpFile = L"http://iphonetoday.googlecode.com/";
				sei.nShow = SW_SHOWNORMAL;
				sei.nShow = SW_SHOWNORMAL;
				ShellExecuteEx(&sei);
				break;
			case IDC_BUTTON_RESET_SETTINGS:
				if (MessageBox(hDlg, L"Are you sure you want to reset all settings to defaults?", L"Reset settings?", MB_YESNO) == IDYES) {
					if (configuracion != NULL) {
						configuracion->defaultValues();
						//configuracion->autoConfigure();
						configuracion->guardaXMLConfig();
						PostMessage(g_hWnd, WM_CREATE, 0, 0);
						doNotAskToSaveOptions = TRUE;
						SendMessage(GetParent(hDlg), WM_CLOSE, 0, 0);
					}
				}
				break;
		}
		break;

	case WM_CTLCOLORSTATIC:
		if ((HWND)lParam == GetDlgItem (hDlg, IDC_STATICURL)) {
			// Modify the font color of the URL string
			SetTextColor ((HDC)wParam, GetSysColor(COLOR_MENUTEXT));
		}
		// If an application processes this message,
		// the return value is a handle to a brush that
		// the system uses to paint the background of
		// the static control.
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}
