//////////////////////////////////////////////////////////////////////////////
// OptionDialogs.h : Header file for the option dialogs and property sheet pages
//

#pragma once

#define NUM_TABS			12

#define TAB_SCREEN			0
#define TAB_WALLPAPER		1
#define TAB_MOVEMENT		2
#define TAB_GENERAL			3
#define TAB_ONLAUNCH		4
#define TAB_ANIMATION		5
#define TAB_TRANSPARENCY	6
#define TAB_OUTOFSCREEN		7
#define TAB_BUBBLES			8
#define TAB_SPECIALICONS	9
#define TAB_HEADER			10
#define TAB_ABOUT			11

//////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////

extern HWND			g_hDlg[NUM_TABS];
extern int lasttab;
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
BOOL				SaveConfigurationScreen(HWND);
BOOL				SaveConfigurationWallpaper(HWND);
BOOL				SaveConfigurationMovement(HWND);
BOOL				SaveConfigurationHeader(HWND);
BOOL				SaveConfigurationBubbles(HWND);
BOOL				SaveConfigurationSpecialIcons(HWND);
BOOL				SaveConfigurationGeneral(HWND);
BOOL				SaveConfigurationOnLaunch(HWND);
BOOL				SaveConfigurationTransparency(HWND);
BOOL				SaveConfigurationOutOfScreen(HWND);
BOOL				SaveConfigurationAnimation(HWND);

/*BOOL				IsValidConfiguration(HWND, INT);
BOOL				IsValidConfigurationScreen(HWND);
BOOL				IsValidConfigurationWallpaper(HWND);
BOOL				IsValidConfigurationMovement(HWND);
BOOL				IsValidConfigurationHeader(HWND);
BOOL				IsValidConfigurationBubbles(HWND);
BOOL				IsValidConfigurationSpecialIcons(HWND);
BOOL				IsValidConfigurationGeneral(HWND);
BOOL				IsValidConfigurationOnLaunch(HWND);
BOOL				IsValidConfigurationTransparency(HWND);
BOOL				IsValidConfigurationOutOfScreen(HWND);
BOOL				IsValidConfigurationAnimation(HWND);*/

// Option dialog box procedure functions
LRESULT	CALLBACK	OptionDialogScreen(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogWallpaper(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogMovement(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogHeader(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogBubbles(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogSpecialIcons(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogGeneral(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogOnLaunch(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogTransparency(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogOutOfScreen(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogAbout(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK	OptionDialogAnimation(HWND, UINT, WPARAM, LPARAM);

UINT GetDlgItemHex(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated);
BOOL SetDlgItemHex(HWND hDlg, int nIDDlgItem, UINT uValue);

float GetDlgItemFloat(HWND hDlg, int nIDDlgItem, BOOL* lpTranslated);
BOOL SetDlgItemFloat(HWND hDlg, int nIDDlgItem, float fValue);

BOOL PositionKBButton(HWND hwndKB, HWND hwnd);
LRESULT CALLBACK KBButtonDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
