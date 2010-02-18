#include <windows.h>

#define WM_USER_RELAUNCH	(WM_USER + 0)
#define WM_USER_OPTIONS		(WM_USER + 1)
#define WM_USER_ADD			(WM_USER + 2)
#define WM_USER_GOTO		(WM_USER + 3)
#define WM_USER_GOTO_NEXT	(WM_USER + 4)
#define WM_USER_GOTO_PREV	(WM_USER + 5)
#define WM_USER_GOTO_UP		(WM_USER + 6)
#define WM_USER_GOTO_DOWN	(WM_USER + 7)

BOOL CommandLineArguements(HWND hwnd, LPCTSTR pCmdLine);
