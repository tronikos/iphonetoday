//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage6.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog6(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			// Initialize handle to property sheet
			g_hDlg[6] = hDlg;

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
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN),				BM_SETCHECK, configuracion->fullscreen					? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR),		BM_SETCHECK, configuracion->neverShowTaskBar			? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE),		BM_SETCHECK, configuracion->noWindowTitle				? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_SETCHECK, configuracion->disableRightClick			? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_SETCHECK, configuracion->ignoreRotation				? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE),			BM_SETCHECK, configuracion->closeOnLaunchIcon			? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_ANIMATE),		BM_SETCHECK, configuracion->allowAnimationOnLaunchIcon	? BST_CHECKED : BST_UNCHECKED, 0);

				SetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE,	configuracion->vibrateOnLaunchIcon,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER,		configuracion->notifyTimer,			TRUE);

#ifndef EXEC_MODE
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE), FALSE);
#endif
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_CTLCOLORSTATIC:
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration6(HWND hDlg)
{
	int vibrateOnLaunchIcon, notifyTimer;

	vibrateOnLaunchIcon = GetDlgItemInt(hDlg, IDC_EDIT_ONLAUNCH_VIBRATE, NULL, TRUE);
	notifyTimer = GetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER, NULL, TRUE);

	if (vibrateOnLaunchIcon < 0 || vibrateOnLaunchIcon > 500) {
		MessageBox(hDlg, TEXT("Vibrate on launch value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (notifyTimer < 0 || notifyTimer > 10000) {
		MessageBox(hDlg, TEXT("Notify timer value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	configuracion->vibrateOnLaunchIcon = vibrateOnLaunchIcon;
	configuracion->notifyTimer = notifyTimer;

	configuracion->fullscreen					= SendMessage(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN),			BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->neverShowTaskBar				= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->noWindowTitle				= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->disableRightClick			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->ignoreRotation				= SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->closeOnLaunchIcon			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_CLOSE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->allowAnimationOnLaunchIcon	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_ONLAUNCH_ANIMATE),		BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
