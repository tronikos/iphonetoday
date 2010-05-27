//////////////////////////////////////////////////////////////////////////////
// OptionDialog.cpp : Defines the option dialog procedures of the application.
//


#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"


// Handles to the dialog pages
HWND		g_hDlg[NUM_TABS];

HWND g_hwndKB = NULL;
BOOL doNotAskToSaveOptions = FALSE;
int saveOptionsAnswer = -1;
int initializedDialogs = 0;
int appliedDialogs = 0;
int appliedDialogsMask = 0;

int lasttab = 0;

void InitOptionsDialog(HWND hDlg, INT iDlg)
{
	// Initialize handle to property sheet
	g_hDlg[iDlg] = hDlg;
	initializedDialogs++;
	//WriteToLog(L"Initializing dialog #%d\n", iDlg);

	if (iDlg == lasttab) {
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

/*BOOL IsValidConfiguration(HWND hDlg, INT iDlg)
{
	BOOL isValid = TRUE;
	switch (iDlg) {
		case TAB_SCREEN:
			isValid = IsValidConfigurationScreen(hDlg);
			break;
		case TAB_WALLPAPER:
			isValid = IsValidConfigurationWallpaper(hDlg);
			break;
		case TAB_MOVEMENT:
			isValid = IsValidConfigurationMovement(hDlg);
			break;
		case TAB_HEADER:
			isValid = IsValidConfigurationHeader(hDlg);
			break;
		case TAB_BUBBLES:
			isValid = IsValidConfigurationBubbles(hDlg);
			break;
		case TAB_SPECIALICONS:
			isValid = IsValidConfigurationSpecialIcons(hDlg);
			break;
		case TAB_GENERAL:
			isValid = IsValidConfigurationGeneral(hDlg);
			break;
		case TAB_ONLAUNCH:
			isValid = IsValidConfigurationOnLaunch(hDlg);
			break;
		case TAB_TRANSPARENCY:
			isValid = IsValidConfigurationTransparency(hDlg);
			break;
		case TAB_OUTOFSCREEN:
			isValid = IsValidConfigurationOutOfScreen(hDlg);
			break;
		case TAB_ANIMATION:
			isValid = IsValidConfigurationAnimation(hDlg);
			break;
	}
	return isValid;
}*/

LRESULT DefOptionWindowProc(HWND hDlg, INT iDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL focus = FALSE;
	static BOOL allowfocus = TRUE;

	switch (uMsg)
	{
		case WM_CTLCOLOREDIT:
			if (focus) {
				//static DWORD start = -1;
				//if (start == -1) {
				//	start = GetTickCount();
				//}
				PostMessage((HWND) lParam, EM_SETSEL, 0, -1);
				//if (GetTickCount() - start > 1000) {
				//	start = -1;
					focus = FALSE;
				//}
			}
			return 0;
		case WM_COMMAND:
			if (HIWORD(wParam) == EN_SETFOCUS && allowfocus) {
				ToggleKeyboard(TRUE);
				focus = TRUE;
				PostMessage((HWND) lParam, EM_SETSEL, 0, -1);
			} else if (HIWORD(wParam) == EN_KILLFOCUS) {
				ToggleKeyboard(FALSE);
				focus = FALSE;
			}
			return 0;
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
				case PSN_SETACTIVE:
					lasttab = iDlg;
					allowfocus = TRUE;
					return 0;
				case PSN_QUERYCANCEL:
					allowfocus = FALSE;
					if (!doNotAskToSaveOptions && MessageBox(hDlg, TEXT("Close without saving?"), TEXT("Exit"), MB_YESNO) == IDNO) {
						allowfocus = TRUE;
						SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
						return TRUE;
					}
					return FALSE;
				case PSN_APPLY:
					//WriteToLog(L"Applying dialog #%d\n", iDlg);
					if ((appliedDialogsMask & (1 << iDlg)) > 0) {
						//WriteToLog(L"Skipped applying dialog #%d\n", iDlg);
						return FALSE;
					}
					appliedDialogsMask |= (1 << iDlg);
					appliedDialogs++;
					if (saveOptionsAnswer == -1) {
						saveOptionsAnswer = -2;
						allowfocus = FALSE;
						int resp = MessageBox(hDlg, TEXT("Save Changes?"), TEXT("Exit"), MB_YESNOCANCEL);
						//WriteToLog(L"#%d: User response to \"Save Changes?\": %d\n", iDlg, resp);
						if (resp == IDCANCEL) {
							saveOptionsAnswer = -1;
							allowfocus = TRUE;
							SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
							appliedDialogs = 0;
							appliedDialogsMask = 0;
							return TRUE;
						}
						saveOptionsAnswer = (resp == IDNO) ? 0 : 1;
					}
					if (saveOptionsAnswer == 1) {
						/*//WriteToLog(L"Calling IsValidConfiguration(%d)\n", iDlg);
						if (!IsValidConfiguration(hDlg, iDlg)) {
							//WriteToLog(L"IsValidConfiguration(%d) returned FALSE\n", iDlg);
							SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_INVALID);
							saveOptionsAnswer = -1;
							appliedDialogs = 0;
							appliedDialogsMask = 0;
							return TRUE;
						}*/
						if (appliedDialogs == initializedDialogs) {
							//WriteToLog(L"#%d: Calling SaveConfiguration()\n", iDlg);
							if (SaveConfiguration()) {
								//WriteToLog(L"#%d: Options saved. Restarting iPT.\n", iDlg);
								PostMessage(g_hWnd, WM_CREATE, 0, 0);
							} else {
								//WriteToLog(L"SaveConfiguration() returned FALSE\n");
							}
						}
					}
					return FALSE;
			}
			break;
	}

	return 0;
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
	for (int i = 0; i < NUM_TABS; i++)
		g_hDlg[i] = NULL;

	doNotAskToSaveOptions = FALSE;
	saveOptionsAnswer = -1;
	initializedDialogs = 0;
	appliedDialogs = 0;
	appliedDialogsMask = 0;



    PROPSHEETPAGE	psp[NUM_TABS];
    PROPSHEETHEADER	psh;

    // Fill in default values in property page structures
    for (int i = 0; i < NUM_TABS; i++) {
		psp[i].dwSize = sizeof(psp[i]);
		psp[i].dwFlags = PSP_DEFAULT | PSP_USECALLBACK;
		psp[i].hInstance = g_hInst;
		psp[i].lParam = (LPARAM)i;
		psp[i].pfnCallback = PropSheetPageProc;
	}


    // Set the dialog box procedures for each page

	psp[TAB_SCREEN].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_SCREEN);
	psp[TAB_SCREEN].pfnDlgProc = (DLGPROC) &OptionDialogScreen;

	psp[TAB_WALLPAPER].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_WALLPAPER);
	psp[TAB_WALLPAPER].pfnDlgProc = (DLGPROC) &OptionDialogWallpaper;

	psp[TAB_MOVEMENT].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_MOVEMENT);
	psp[TAB_MOVEMENT].pfnDlgProc = (DLGPROC) &OptionDialogMovement;

	psp[TAB_HEADER].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_HEADER);
	psp[TAB_HEADER].pfnDlgProc = (DLGPROC) &OptionDialogHeader;

	psp[TAB_BUBBLES].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_BUBBLES);
	psp[TAB_BUBBLES].pfnDlgProc = (DLGPROC) &OptionDialogBubbles;

	psp[TAB_SPECIALICONS].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_SPECIALICONS);
	psp[TAB_SPECIALICONS].pfnDlgProc = (DLGPROC) &OptionDialogSpecialIcons;

	psp[TAB_GENERAL].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_GENERAL);
	psp[TAB_GENERAL].pfnDlgProc = (DLGPROC) &OptionDialogGeneral;

	psp[TAB_ONLAUNCH].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_ONLAUNCH);
	psp[TAB_ONLAUNCH].pfnDlgProc = (DLGPROC) &OptionDialogOnLaunch;

	psp[TAB_TRANSPARENCY].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_TRANSPARENCY);
	psp[TAB_TRANSPARENCY].pfnDlgProc = (DLGPROC) &OptionDialogTransparency;

	psp[TAB_OUTOFSCREEN].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_OUTOFSCREEN);
	psp[TAB_OUTOFSCREEN].pfnDlgProc = (DLGPROC) &OptionDialogOutOfScreen;

	psp[TAB_ABOUT].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_ABOUT);
	psp[TAB_ABOUT].pfnDlgProc = (DLGPROC) &OptionDialogAbout;

	psp[TAB_ANIMATION].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE_ANIMATION);
	psp[TAB_ANIMATION].pfnDlgProc = (DLGPROC) &OptionDialogAnimation;


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
    psh.nPages = NUM_TABS;
    psh.nStartPage = lasttab;
    psh.ppsp = &psp[0];
    psh.pfnCallback = PropSheetCallback;


	// Create and display property sheet
	//WriteToLog(L"Calling PropertySheet()\n");
    PropertySheet(&psh);


	return 0;
}

BOOL SaveConfiguration()
{
	BOOL result = TRUE;

	for (int i = 0; i < NUM_TABS; i++) {
		HWND hDlg = g_hDlg[i];
		if (hDlg) {
			switch(i) {
				case TAB_SCREEN:
					result &= SaveConfigurationScreen(hDlg);
					break;
				case TAB_WALLPAPER:
					result &= SaveConfigurationWallpaper(hDlg);
					break;
				case TAB_MOVEMENT:
					result &= SaveConfigurationMovement(hDlg);
					break;
				case TAB_HEADER:
					result &= SaveConfigurationHeader(hDlg);
					break;
				case TAB_BUBBLES:
					result &= SaveConfigurationBubbles(hDlg);
					break;
				case TAB_SPECIALICONS:
					result &= SaveConfigurationSpecialIcons(hDlg);
					break;
				case TAB_GENERAL:
					result &= SaveConfigurationGeneral(hDlg);
					break;
				case TAB_ONLAUNCH:
					result &= SaveConfigurationOnLaunch(hDlg);
					break;
				case TAB_TRANSPARENCY:
					result &= SaveConfigurationTransparency(hDlg);
					break;
				case TAB_OUTOFSCREEN:
					result &= SaveConfigurationOutOfScreen(hDlg);
					break;
				case TAB_ANIMATION:
					result &= SaveConfigurationAnimation(hDlg);
					break;
			}
		}
	}

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
