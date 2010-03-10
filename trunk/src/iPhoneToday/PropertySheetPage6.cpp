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
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT),				BM_SETCHECK, configuracion->showExit					? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_SETCHECK, configuracion->disableRightClick			? BST_CHECKED : BST_UNCHECKED, 0);
				SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_SETCHECK, configuracion->ignoreRotation				? BST_CHECKED : BST_UNCHECKED, 0);

				SetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER,	configuracion->notifyTimer,	TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_HEIGHTP,		configuracion->heightP,		TRUE);
				SetDlgItemInt(hDlg, IDC_EDIT_HEIGHTL,		configuracion->heightL,		TRUE);

#ifdef EXEC_MODE
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HEIGHTP), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_EDIT_HEIGHTL), FALSE);
#else
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT), FALSE);
#endif
			} else {
				MessageBox(hDlg, L"Empty Configuration!", 0, MB_OK);
			}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_NEVER_SHOW_TASKBAR:
			if (SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				int resp = MessageBox(hDlg,
					L"Are you really sure you never want to show the taskbar?\n"
					L"This option is meant for Pocket Navigation Devices.\n"
					L"The fullscreen option already hides the taskbar.",
					L"Are you sure?", MB_YESNO);
				if (resp == IDNO) {
					SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR), BM_SETCHECK, BST_UNCHECKED, 0);
				}
			}
			break;
		case IDC_EDIT_HEIGHTP:
		case IDC_EDIT_HEIGHTL:
			if (HIWORD(wParam) == EN_SETFOCUS) {
				static BOOL displayed_tip = FALSE;
				if (!displayed_tip) {
					displayed_tip = TRUE;
					MessageBox(hDlg, L"It is recommended to leave this 0 to automatically size the today item.", L"Tip", MB_OK);
				}
			}
			break;
		}
		return 0;
	case WM_CTLCOLORSTATIC:
		return (LRESULT)GetStockObject(WHITE_BRUSH);
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

BOOL SaveConfiguration6(HWND hDlg)
{
	int notifyTimer, heightP, heightL;

	notifyTimer = GetDlgItemInt(hDlg, IDC_EDIT_NOTIFY_TIMER, NULL, TRUE);
	heightP = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTP, NULL, TRUE);
	heightL = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHTL, NULL, TRUE);

	if (notifyTimer < 0 || notifyTimer > 10000) {
		MessageBox(hDlg, TEXT("Notify timer value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (heightP < 0 || heightP > 1000 || heightL < 0 || heightL > 1000) {
		MessageBox(hDlg, TEXT("Height value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	configuracion->fullscreen			= SendMessage(GetDlgItem(hDlg, IDC_CHECK_FULLSCREEN),			BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->neverShowTaskBar		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NEVER_SHOW_TASKBAR),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->noWindowTitle		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_NO_WINDOW_TITLE),		BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->showExit				= SendMessage(GetDlgItem(hDlg, IDC_CHECK_SHOW_EXIT),			BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->disableRightClick	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_DISABLE_RIGHT_CLICK),	BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->ignoreRotation		= SendMessage(GetDlgItem(hDlg, IDC_CHECK_IGNORE_ROTATION),		BM_GETCHECK, 0, 0) == BST_CHECKED;

	configuracion->notifyTimer	= notifyTimer;
	configuracion->heightP		= heightP;
	configuracion->heightL		= heightL;

	return TRUE;
}
