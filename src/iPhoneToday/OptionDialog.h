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
// OptionDialogs.h : Header file for the option dialogs and property sheet pages
//

#pragma once


//////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////

extern HWND			g_hDlg[NUM_CONFIG_SCREENS];



//////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

// Option dialog functions
UINT	CALLBACK	PropSheetPageProc(HWND, UINT, LPPROPSHEETPAGE);  // Option dialog PropSheetPageProc callback function

INT					PropSheetCallback(HWND, UINT, LPARAM);  // PropertySheet callback function

BOOL				CreatePropertySheet(HWND);  // Function to initialize and create the property sheet for the option dialog

BOOL				SaveConfiguration();
BOOL				SaveConfiguration0(HWND);
BOOL				SaveConfiguration1(HWND);
BOOL				SaveConfiguration2(HWND);
BOOL				SaveConfiguration3(HWND);
BOOL				SaveConfiguration4(HWND);
BOOL				SaveConfiguration5(HWND);
BOOL				SaveConfiguration6(HWND);

// Option dialog box procedure functions
LRESULT	CALLBACK	OptionDialog0(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog1(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog2(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog3(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog4(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog5(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog6(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog7(HWND, UINT, WPARAM, LPARAM);

UINT GetDlgItemHex(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated);
BOOL SetDlgItemHex(HWND hDlg, int nIDDlgItem, UINT uValue);
