//////////////////////////////////////////////////////////////////////////////
// OptionDialogs.h : Header file for the option dialogs and property sheet pages
//

#pragma once


//////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////

extern HWND			g_hDlg[NUM_CONFIG_SCREENS];

extern BOOL doNotAskToSaveOptions;

//////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

// Option dialog functions
void				InitOptionsDialog(HWND, INT);
void				PaintOptionsDialog(HWND, INT);
LRESULT				DefOptionWindowProc(HWND hDlg, INT iDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
BOOL				SaveConfiguration7(HWND);
BOOL				SaveConfiguration8(HWND);
BOOL				SaveConfiguration9(HWND);

BOOL IsValidConfiguration(HWND, INT);
BOOL IsValidConfiguration0(HWND);
BOOL IsValidConfiguration1(HWND);
BOOL IsValidConfiguration2(HWND);
BOOL IsValidConfiguration3(HWND);
BOOL IsValidConfiguration4(HWND);
BOOL IsValidConfiguration5(HWND);
BOOL IsValidConfiguration6(HWND);
BOOL IsValidConfiguration7(HWND);
BOOL IsValidConfiguration8(HWND);
BOOL IsValidConfiguration9(HWND);

// Option dialog box procedure functions
LRESULT	CALLBACK	OptionDialog0(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog1(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog2(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog3(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog4(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog5(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog6(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog7(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog8(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog9(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialog10(HWND, UINT, WPARAM, LPARAM);

UINT GetDlgItemHex(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated);
BOOL SetDlgItemHex(HWND hDlg, int nIDDlgItem, UINT uValue);

float GetDlgItemFloat(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated);
BOOL SetDlgItemFloat(HWND hDlg, int nIDDlgItem, float fValue);

BOOL PositionKBButton(HWND hwndKB, HWND hwnd);
LRESULT CALLBACK KBButtonDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
