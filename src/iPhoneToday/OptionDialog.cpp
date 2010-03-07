//////////////////////////////////////////////////////////////////////////////
// Pocket Digital Clock
// Copyright 2008 ProjectION
// Written by Chia Chee Kit "Ionized"
// http://project.ionized.googlepages.com/pdc
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// OptionDialog.cpp : Defines the option dialog procedures of the application.
//


#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"


// Handles to the dialog pages
HWND		g_hDlg[NUM_CONFIG_SCREENS];  

// Variable to track how many times PropSheetPageProc is called when closing the property sheet
static INT	s_iReleaseCall;


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
			break;

		case PSPCB_RELEASE:  // Every property page will call here when it gets released
			{
				// Increase variable for every call
				s_iReleaseCall++;

				// Only save the registry values when the last property sheet is released
				if (s_iReleaseCall >= NUM_CONFIG_SCREENS)
				{
					// Comprobamos si quiere guardar o solo salir
					int resp = MessageBox(hwnd, TEXT("Save Changes?"), TEXT("Exit"), MB_YESNO);
					if (resp == IDNO) {
						if (g_hWndMenuBar) {
							CommandBar_Destroy(g_hWndMenuBar);
						}
						// EndDialog(hwnd, LOWORD(wParam));
						return FALSE;
					}

					// Update the settings
					SaveConfiguration();
					PostMessage(g_hWnd, WM_CREATE, 0, 0);
				}

				return 0;
			}
			break;

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
	// This is necessary so that dialog tabs get drawn in the flat Pocket PC style
	switch (message)
	{
		case PSCB_GETVERSION:
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

	// Initialize iPageCall used for updating the registry only once when property sheet is dismissed
	s_iReleaseCall = 0;



	
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
		configuracion->guardaXMLConfig();
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
