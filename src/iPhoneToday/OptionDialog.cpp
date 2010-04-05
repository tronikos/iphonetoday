//////////////////////////////////////////////////////////////////////////////
// OptionDialog.cpp : Defines the option dialog procedures of the application.
//


#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"


// Handles to the dialog pages
HWND		g_hDlg[NUM_CONFIG_SCREENS];

HWND g_hwndKB = NULL;
BOOL doNotAskToSaveOptions = FALSE;
int saveOptionsAnswer = -1;
int initializedDialogs = 0;
int appliedDialogs = 0;


void InitOptionsDialog(HWND hDlg, INT iDlg)
{
	// Initialize handle to property sheet
	g_hDlg[iDlg] = hDlg;
	initializedDialogs++;

	if (iDlg == 0) {
		if (FindWindow(L"MS_SIPBUTTON", NULL) != NULL) {
			// Property sheet will destroy part of the soft key bar,
			// therefore we create an empty menu bar here
			// Only required on the first property sheet
			SHMENUBARINFO shmbi;
			shmbi.cbSize = sizeof(shmbi);
			shmbi.hwndParent = hDlg;
			shmbi.dwFlags = SHCMBF_EMPTYBAR;
			SHCreateMenuBar(&shmbi);
		} else {
//			SetWindowLong(GetParent(hDlg), GWL_EXSTYLE, GetWindowLong(GetParent(hDlg), GWL_EXSTYLE) | WS_EX_CONTEXTHELP);
			g_hwndKB = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_KB_BUTTON), GetParent(hDlg), (DLGPROC) KBButtonDlgProc);
		}
	}

	SHINITDLGINFO shidi;
	shidi.dwMask = SHIDIM_FLAGS;
	shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
	shidi.hDlg = hDlg;
	SHInitDialog(&shidi);

	if (configuracion == NULL) {
		configuracion = new CConfiguracion();
		configuracion->loadXMLConfig();
	}
}

void PaintOptionsDialog(HWND hDlg, INT iDlg)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rcDlg;
	HBRUSH hBrush;

	hdc = BeginPaint(hDlg, &ps);
	GetClientRect(hDlg, &rcDlg);
	hBrush = CreateSolidBrush(GetSysColor(COLOR_MENU));
	FillRect(hdc, &rcDlg, hBrush);
	DeleteObject(hBrush);
	EndPaint(hDlg, &ps);
}

BOOL IsValidConfiguration(HWND hDlg, INT iDlg)
{
	BOOL isValid = TRUE;
	switch (iDlg) {
		case 0:
			isValid = IsValidConfiguration0(hDlg);
			break;
		case 1:
			isValid = IsValidConfiguration1(hDlg);
			break;
		case 2:
			isValid = IsValidConfiguration2(hDlg);
			break;
		case 3:
			isValid = IsValidConfiguration3(hDlg);
			break;
		case 4:
			isValid = IsValidConfiguration4(hDlg);
			break;
		case 5:
			isValid = IsValidConfiguration5(hDlg);
			break;
		case 6:
			isValid = IsValidConfiguration6(hDlg);
			break;
		case 7:
			isValid = IsValidConfiguration7(hDlg);
			break;
		case 8:
			isValid = IsValidConfiguration8(hDlg);
			break;
		case 9:
			isValid = IsValidConfiguration9(hDlg);
			break;
	}
	return isValid;
}

LRESULT DefOptionWindowProc(HWND hDlg, INT iDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
			if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE) {
				EnableWindow(g_hwndKB, TRUE);
			} else if (!::IsChild(hDlg, (HWND)lParam)) {
				EnableWindow(g_hwndKB, FALSE);
			}
			break;
		case WM_PAINT:
			PositionKBButton(g_hwndKB, GetParent(hDlg));
			PaintOptionsDialog(hDlg, iDlg);
			return 0;
		case WM_NOTIFY:
			switch (((LPNMHDR) lParam)->code)
			{
//				case PSN_HELP:
//					ToggleKeyboard();
//					return 0;
				case PSN_QUERYCANCEL:
					if (!doNotAskToSaveOptions && MessageBox(hDlg, TEXT("Close without saving?"), TEXT("Exit"), MB_YESNO) == IDNO) {
						SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
						return TRUE;
					}
					return FALSE;
				case PSN_APPLY:
					appliedDialogs++;
					if (saveOptionsAnswer == -1) {
						int resp = MessageBox(hDlg, TEXT("Save Changes?"), TEXT("Exit"), MB_YESNOCANCEL);
						if (resp == IDCANCEL) {
							SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
							appliedDialogs = 0;
							return TRUE;
						}
						saveOptionsAnswer = (resp == IDYES) ? 1 : 0;
					}
					if (saveOptionsAnswer == 1) {
						if (!IsValidConfiguration(hDlg, iDlg)) {
							SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID);
							saveOptionsAnswer = -1;
							appliedDialogs = 0;
							return TRUE;
						}
						if (appliedDialogs == initializedDialogs) {
							if (SaveConfiguration()) {
								PostMessage(g_hWnd, WM_CREATE, 0, 0);
							}
						}
					}
					return FALSE;
			}
			break;
	}

	return DefWindowProc(hDlg, uMsg, wParam, lParam);
}

/*************************************************************************/
/* Option dialog PropSheetPageProc callback function                     */
/*************************************************************************/
UINT CALLBACK PropSheetPageProc(HWND hwnd,UINT uMsg,LPPROPSHEETPAGE ppsp)
{
	// This callback is called when each property page gets created and when it gets released
	switch(uMsg)
	{
		case PSPCB_CREATE:
			//Return any non zero value to indicate success
			return 1;

		case PSPCB_RELEASE:  // Every property page will call here when it gets released
			return 0;

		default:
			break;
	}
	//return for default case above
	return (UINT)-1;
}


/*************************************************************************/
/* PropertySheet callback function when property page is being created   */
/*************************************************************************/
INT PropSheetCallback(HWND hwndDlg, UINT message, LPARAM lParam)
{
	switch (message)
	{
		case PSCB_GETVERSION:
			// This is necessary so that dialog tabs get drawn in the flat Pocket PC style
			return COMCTL32_VERSION;
		default:
			break;
    }
    return 0;
}



/*************************************************************************/
/* Initialize and create the property sheet for the option dialog        */
/*************************************************************************/
BOOL CreatePropertySheet(HWND hwnd)
{
	// Initialize handles to the dialog pages
	for (int i = 0; i < NUM_CONFIG_SCREENS; i++)
		g_hDlg[i] = NULL;

	doNotAskToSaveOptions = FALSE;
	saveOptionsAnswer = -1;
	initializedDialogs = 0;
	appliedDialogs = 0;



    PROPSHEETPAGE	psp[NUM_CONFIG_SCREENS];
    PROPSHEETHEADER	psh;

    // Fill in default values in property page structures
    for (int i = 0; i < NUM_CONFIG_SCREENS; i++)
	{
		psp[i].dwSize = sizeof(psp[i]);
		psp[i].dwFlags = PSP_DEFAULT | PSP_USETITLE| PSP_USECALLBACK;
		psp[i].hInstance = g_hInst;
		psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE0 + i);  // Make sure the Resource Value is in sequence for all the dialog boxes
		psp[i].pszTitle = (LPCTSTR)LoadString(g_hInst, (IDS_TAB0 + i), NULL, 0);  // Make sure the value of the tab titles in string table is in sequence too
		psp[i].lParam = (LPARAM)i;
		psp[i].pfnCallback = PropSheetPageProc;
	}


    // Set the dialog box procedures for each page
	psp[0].pfnDlgProc = (DLGPROC)&OptionDialog0;
	psp[1].pfnDlgProc = (DLGPROC)&OptionDialog1;
	psp[2].pfnDlgProc = (DLGPROC)&OptionDialog2;
	psp[3].pfnDlgProc = (DLGPROC)&OptionDialog3;
	psp[4].pfnDlgProc = (DLGPROC)&OptionDialog4;
	psp[5].pfnDlgProc = (DLGPROC)&OptionDialog5;
	psp[6].pfnDlgProc = (DLGPROC)&OptionDialog6;
	psp[7].pfnDlgProc = (DLGPROC)&OptionDialog7;
	psp[8].pfnDlgProc = (DLGPROC)&OptionDialog8;
	psp[9].pfnDlgProc = (DLGPROC)&OptionDialog9;
	psp[10].pfnDlgProc = (DLGPROC)&OptionDialog10;


    //
    // See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/win_ce/htm/pwc_propertysheets.asp
    // for more information about PSH_MAXIMIZE and handling PSCB_GETVERSION in the
    // PropertySheet callback.
    //

	// Initialize property sheet header
    psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_DEFAULT | PSH_PROPSHEETPAGE | PSH_MAXIMIZE | PSH_NOAPPLYNOW | PSH_USECALLBACK;
    psh.hwndParent = hwnd;
    psh.hInstance = g_hInst;
    psh.pszCaption = L"Options";
    psh.nPages = NUM_CONFIG_SCREENS;
    psh.nStartPage = 0;
    psh.ppsp = &psp[0];
    psh.pfnCallback = PropSheetCallback;


	// Create and display property sheet
    PropertySheet(&psh);


	return 0;
}

BOOL SaveConfiguration()
{
	BOOL result = TRUE;

	if (g_hDlg[0])
		result &= SaveConfiguration0(g_hDlg[0]);
	if (g_hDlg[1])
		result &= SaveConfiguration1(g_hDlg[1]);
	if (g_hDlg[2])
		result &= SaveConfiguration2(g_hDlg[2]);
	if (g_hDlg[3])
		result &= SaveConfiguration3(g_hDlg[3]);
	if (g_hDlg[4])
		result &= SaveConfiguration4(g_hDlg[4]);
	if (g_hDlg[5])
		result &= SaveConfiguration5(g_hDlg[5]);
	if (g_hDlg[6])
		result &= SaveConfiguration6(g_hDlg[6]);
	if (g_hDlg[7])
		result &= SaveConfiguration7(g_hDlg[7]);
	if (g_hDlg[8])
		result &= SaveConfiguration8(g_hDlg[8]);
	if (g_hDlg[9])
		result &= SaveConfiguration9(g_hDlg[9]);

	if (result) {
		configuracion->saveXMLConfig();
	}
	return result;
}

UINT GetDlgItemHex(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated)
{
	TCHAR str[MAX_PATH];
	UINT result = 0;
	if (GetDlgItemText(hDlg, nIDDlgItem, str, MAX_PATH) > 0) {
		swscanf(str, L"#%X", &result);
		if (lpTranslated != NULL) {
			*lpTranslated = TRUE;
		}
	} else {
		if (lpTranslated != NULL) {
			*lpTranslated = FALSE;
		}
	}
	return result;
}

BOOL SetDlgItemHex(HWND hDlg, int nIDDlgItem, UINT uValue)
{
	TCHAR str[MAX_PATH];
	swprintf(str, L"#%X", uValue);
	return SetDlgItemText(hDlg, nIDDlgItem, str);
}

float GetDlgItemFloat(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated)
{
	TCHAR str[MAX_PATH];
	float result = 0;
	if (GetDlgItemText(hDlg, nIDDlgItem, str, MAX_PATH) > 0) {
		swscanf(str, L"%f", &result);
		if (lpTranslated != NULL) {
			*lpTranslated = TRUE;
		}
	} else {
		if (lpTranslated != NULL) {
			*lpTranslated = FALSE;
		}
	}
	return result;
}

BOOL SetDlgItemFloat(HWND hDlg, int nIDDlgItem, float fValue)
{
	TCHAR str[MAX_PATH];
	swprintf(str, L"%.3f", fValue);
	return SetDlgItemText(hDlg, nIDDlgItem, str);
}


////////////////////////////////////////////////////////////////////////////////
#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_THICKFRAME)))

#define HAS_THICKFRAME(style,exStyle) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define HAS_THINFRAME(style) \
    (((style) & WS_BORDER) || !((style) & (WS_CHILD | WS_POPUP)))


// Place a (KB) button on the left of any possible (?)(OK)(X) buttons.
BOOL PositionKBButton(HWND hwndKB, HWND hwnd)
{
	if (hwndKB == NULL || hwnd == NULL) {
		return FALSE;
	}

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	// The (KB) button can only be placed on windows with a title bar
	if ((style|WS_CAPTION) != style) {
		SetWindowPos(hwndKB, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE);
		return FALSE;
	}

	int i = 0;
	if ((style|WS_SYSMENU) == style) i++;	// has a close (X) button
	if ((exStyle|WS_EX_CAPTIONOKBTN) == exStyle) i++;	// has an OK button
	if ((exStyle|WS_EX_CONTEXTHELP) == exStyle) i++;	// has a Help (?) button

	int x = 0;
	int y = 0;

	if (HAS_THICKFRAME(style, exStyle)) {
//		x = GetSystemMetrics(SM_CXFRAME);
//		y = GetSystemMetrics(SM_CYFRAME);
	//} else if (HAS_DLGFRAME(style, exStyle)) {
	} else if (HAS_DLGFRAME(style, exStyle) && (exStyle|WS_EX_NODRAG) != exStyle) {
		x = GetSystemMetrics(SM_CXDLGFRAME);
		y = GetSystemMetrics(SM_CYDLGFRAME);
	} else if (HAS_THINFRAME(style)) {
		x = GetSystemMetrics(SM_CXBORDER);
		y = GetSystemMetrics(SM_CYBORDER);
	}

	if ((style & WS_CHILD)) {
		if (exStyle & WS_EX_CLIENTEDGE) {
			x += GetSystemMetrics(SM_CXEDGE);
			y += GetSystemMetrics(SM_CYEDGE);
		}
		if (exStyle & WS_EX_STATICEDGE) {
			x += GetSystemMetrics(SM_CXBORDER);
			y += GetSystemMetrics(SM_CYBORDER);
		}
	}

	RECT rc;
	GetWindowRect(hwnd, &rc);

	int w = GetSystemMetrics(SM_CYCAPTION) - 2;
	int h = w;
	x = rc.right - i * (w + 4) - w - x - 2;	// 4 is the space between buttons, 2 is the space between the right most button and the border
	y += rc.top + 1;

	SetWindowPos(hwndKB, HWND_TOP, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
	MoveWindow(GetDlgItem(hwndKB, IDC_KB_BUTTON), 0, 0, w, h, TRUE);

	return TRUE;
}

LRESULT CALLBACK KBButtonDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_KB_BUTTON:
			ToggleKeyboard();
			break;
		}
		break;
	case WM_DESTROY:
		ToggleKeyboard(FALSE);
	}
	return 0;
}
