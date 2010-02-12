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

	// Load all SheetPages
	psp[0].dwSize = sizeof(psp[0]);
    psp[0].dwFlags = PSP_DEFAULT | PSP_USETITLE| PSP_USECALLBACK;
    psp[0].hInstance = g_hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE0); 
	psp[0].pszTitle = (LPCTSTR)LoadString(g_hInst, (IDS_TAB0), NULL, 0);
    psp[0].lParam = (LPARAM)0;
	psp[0].pfnCallback = PropSheetPageProc;
	psp[0].pfnDlgProc = (DLGPROC)&OptionDialog0;

	psp[1].dwSize = sizeof(psp[1]);
    psp[1].dwFlags = PSP_DEFAULT | PSP_USETITLE| PSP_USECALLBACK;
    psp[1].hInstance = g_hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE1); 
	psp[1].pszTitle = (LPCTSTR)LoadString(g_hInst, (IDS_TAB1), NULL, 0);
    psp[1].lParam = (LPARAM)1;
	psp[1].pfnCallback = PropSheetPageProc;
	psp[1].pfnDlgProc = (DLGPROC)&OptionDialog1;

	psp[2].dwSize = sizeof(psp[2]);
    psp[2].dwFlags = PSP_DEFAULT | PSP_USETITLE| PSP_USECALLBACK;
    psp[2].hInstance = g_hInst;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_DIALOGPAGE2); 
	psp[2].pszTitle = (LPCTSTR)LoadString(g_hInst, (IDS_TAB2), NULL, 0);
    psp[2].lParam = (LPARAM)2;
	psp[2].pfnCallback = PropSheetPageProc;
	psp[2].pfnDlgProc = (DLGPROC)&OptionDialog2;


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
    psh.pszCaption = NULL;
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
	// General (first) dialog
	if (g_hDlg[0])
		result &= SaveConfiguration0(g_hDlg[0]);

	// Date (second) dialog
	if (g_hDlg[1])
		result &= SaveConfiguration1(g_hDlg[1]);

	// Date (third) dialog
	if (g_hDlg[2])
		result &= SaveConfiguration2(g_hDlg[2]);

	if (result) {
		configuracion->guardaXMLConfig();
	}
	return result;
}