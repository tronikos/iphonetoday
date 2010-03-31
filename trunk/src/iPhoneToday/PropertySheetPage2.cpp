//////////////////////////////////////////////////////////////////////////////
// PropertySheetPage2.cpp : Defines the property sheet page procedures.
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"

/*************************************************************************/
/* General options dialog box procedure function                 */
/*************************************************************************/
LRESULT CALLBACK OptionDialog2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			InitOptionsDialog(hDlg, 2);

			SetDlgItemInt(hDlg, IDC_EDIT_MOVE_THRESHOLD,	configuracion->moveThreshold,		TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_MOVE_FACTOR,		configuracion->factorMovimiento,	TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_MIN_VELOCITY,		configuracion->velMinima,			TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_MAX_VELOCITY,		configuracion->velMaxima,			TRUE);
			SetDlgItemInt(hDlg, IDC_EDIT_REFRESH_TIME,		configuracion->refreshTime,			TRUE);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_VERTICAL_SCROLL), BM_SETCHECK, configuracion->verticalScroll ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDC_CHECK_FREESTYLE_SCROLL), BM_SETCHECK, configuracion->freestyleScroll ? BST_CHECKED : BST_UNCHECKED, 0);

			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_FREESTYLE_SCROLL), configuracion->verticalScroll);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_VERTICAL_SCROLL:
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_FREESTYLE_SCROLL), SendMessage(GetDlgItem(hDlg, IDC_CHECK_VERTICAL_SCROLL), BM_GETCHECK, 0, 0) == BST_CHECKED);
			break;
		}
		return 0;
	}

	return DefOptionWindowProc(hDlg, 2, uMsg, wParam, lParam);
}

BOOL IsValidConfiguration2(HWND hDlg)
{
	int moveThreshold, moveFactor, minVelocity, maxVelocity, refreshTime;

	moveThreshold	= GetDlgItemInt(hDlg, IDC_EDIT_MOVE_THRESHOLD,	NULL, TRUE);
	moveFactor		= GetDlgItemInt(hDlg, IDC_EDIT_MOVE_FACTOR,		NULL, TRUE);
	minVelocity		= GetDlgItemInt(hDlg, IDC_EDIT_MIN_VELOCITY,	NULL, TRUE);
	maxVelocity		= GetDlgItemInt(hDlg, IDC_EDIT_MAX_VELOCITY,	NULL, TRUE);
	refreshTime		= GetDlgItemInt(hDlg, IDC_EDIT_REFRESH_TIME,	NULL, TRUE);

	if (moveThreshold < 0 || moveThreshold > 256) {
		MessageBox(hDlg, TEXT("Movement threshold value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (moveFactor < 0 || moveFactor > 256) {
		MessageBox(hDlg, TEXT("Movement factor value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (minVelocity < 0 || minVelocity > 256) {
		MessageBox(hDlg, TEXT("Minimum velocity value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (maxVelocity < 0 || maxVelocity > 256) {
		MessageBox(hDlg, TEXT("Maximum velocity value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}
	if (refreshTime < 0 || refreshTime > 1000) {
		MessageBox(hDlg, TEXT("Refresh time value is not valid!"), TEXT("Error"), MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfiguration2(HWND hDlg)
{
	if (!IsValidConfiguration2(hDlg)) return FALSE;

	int moveThreshold, moveFactor, minVelocity, maxVelocity, refreshTime;

	moveThreshold	= GetDlgItemInt(hDlg, IDC_EDIT_MOVE_THRESHOLD,	NULL, TRUE);
	moveFactor		= GetDlgItemInt(hDlg, IDC_EDIT_MOVE_FACTOR,		NULL, TRUE);
	minVelocity		= GetDlgItemInt(hDlg, IDC_EDIT_MIN_VELOCITY,	NULL, TRUE);
	maxVelocity		= GetDlgItemInt(hDlg, IDC_EDIT_MAX_VELOCITY,	NULL, TRUE);
	refreshTime		= GetDlgItemInt(hDlg, IDC_EDIT_REFRESH_TIME,	NULL, TRUE);

	configuracion->moveThreshold	= moveThreshold;
	configuracion->factorMovimiento	= moveFactor;
	configuracion->velMinima		= minVelocity;
	configuracion->velMaxima		= maxVelocity;
	configuracion->refreshTime		= refreshTime;

	configuracion->verticalScroll	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_VERTICAL_SCROLL), BM_GETCHECK, 0, 0) == BST_CHECKED;
	configuracion->freestyleScroll	= SendMessage(GetDlgItem(hDlg, IDC_CHECK_FREESTYLE_SCROLL), BM_GETCHECK, 0, 0) == BST_CHECKED;

	return TRUE;
}
