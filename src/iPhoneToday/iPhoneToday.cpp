// iPhoneToday.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <initguid.h>
#include "iPhoneToday.h"
#include "CListaPantalla.h"
#include "CEstado.h"
#include "CNotifications.h"
#include "vibrate.h"
#include "CmdLine.h"
#include "OptionDialog.h"  // CreatePropertySheet includes
#include "RegistryUtils.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE           g_hInst;
HWND				g_hWndMenuBar = NULL; // menu bar handle
HWND                g_hWnd;
TCHAR               g_szTitle[MAX_LOADSTRING];
IImagingFactory*    g_pImgFactory = NULL;
HINSTANCE           g_hImgdecmpDll = NULL;
#ifdef EXEC_MODE
BOOL				g_bLoading = FALSE;
BOOL				g_bInitializing = FALSE;
#endif

// Variables
CListaPantalla *listaPantallas = NULL;
CConfiguracion *configuracion = NULL;
CEstado *estado = NULL;
CNotifications *notifications = NULL;

BOOL posCursorInitialized = FALSE;
POINTS posCursor;
POINTS posImage = {0, 2};
HDC		hDCMem = NULL;
HBITMAP	hbmMem = NULL;
HBITMAP	hbmMemOld = NULL;

// Used only when configuration->alphablend == 2
HDC		hDCMem2 = NULL;
HBITMAP	hbmMem2 = NULL;
HBITMAP	hbmMemOld2 = NULL;
BITMAP bmBack = {0};

HBRUSH  hBrushFondo = NULL;
HBRUSH  hBrushTrans = NULL;
HBRUSH  hBrushAnimation = NULL;

BOOL pressed = FALSE;
void ResetPressed();

// Variables para detectar doble click
POINTS posUltimoClick = {0};
long timeUltimoClick = 0;

// Variables para mover iconos rapidamente
IDENT_ICONO moveIconoActivo = {0};
int moveTimeUltimaSeleccion = 0;

// Variables para copy iconos rapidamente
IDENT_ICONO copyIconoActivo = {0};
int copyTimeUltimaSeleccion = 0;

// Variable que almacena las ultimas rutas usadas
TCHAR lastPathImage[MAX_PATH];
TCHAR lastPathExec[MAX_PATH] = {0};
TCHAR lastPathSound[MAX_PATH];

// Icono a editar
IDENT_ICONO iconoActual;

// Forward declarations of functions included in this code module:
#ifdef EXEC_MODE
ATOM MyRegisterClass(HINSTANCE, LPTSTR);
BOOL InitInstance(HINSTANCE, int);
LRESULT WndProcLoading (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
#else
INT InitializeClasses();
BOOL doEraseBackground(HWND hwnd);
#endif

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT doTimer (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doSize (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doPaint (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseDown (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseUp (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
BOOL LaunchApplication(LPCTSTR pCmdLine, LPCTSTR pParameters);
void pintaIconos(HDC *hDC, RECT *rcWindBounds);
void pintaIcono(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type);
void pintaPantalla(HDC *hDC, CPantalla *pantalla, SCREEN_TYPE screen_type = MAINSCREEN);
void setPosicionesIconos(CPantalla *pantalla, SCREEN_TYPE screen_type);
void setPosiciones(BOOL inicializa, int offsetX, int offsetY);
void calculaPosicionObjetivo();
void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar = FALSE);
int hayNotificacion(int tipo);
BOOL inicializaApp(HWND hwnd);
BOOL borraObjetosHDC();
BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld);
LRESULT CALLBACK editaIconoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editHeaderDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void doDestroy(HWND hwnd);
void resizeWindow(HWND hwnd, BOOL fullScreen);
LRESULT WINAPI CustomItemOptionsDlgProc(HWND, UINT, WPARAM, LPARAM);
void RightClick(HWND hwnd, POINTS posCursor);
void calculateConfiguration(int width, int height);
void getWindowSize(HWND hwnd, int *windowWidth, int *windowHeight);
BOOL InvalidateListScreenIfNotificationsChanged(CListaPantalla *listaPantallas);
void InvalidateScreenIfNotificationsChanged(CPantalla *pantalla);
BOOL ProcessNotifications();
BOOL hasTopBar();
BOOL hasBottomBar();

#ifndef EXEC_MODE
/*************************************************************************/
/* Entry point for the dll                                                 */
/*************************************************************************/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// WriteToLog(TEXT("DLL_PROCESS_ATTACH\r\n"));
		// initialize imaging API only once
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (!SUCCEEDED(CoCreateInstance (CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) & g_pImgFactory)))
		{
			g_pImgFactory = NULL;
		}
		if (g_pImgFactory == NULL) {
			g_hImgdecmpDll = LoadLibrary(L"imgdecmp.dll");
		}

		// The DLL is being loaded for the first time by a given process.
		// Perform per-process initialization here.  If the initialization
		// is successful, return TRUE; if unsuccessful, return FALSE.
		g_hInst = (HINSTANCE)hModule;

		//initilize the application class, and set the global window handle
		UnregisterClass((LPCTSTR)LoadString(g_hInst, IDS_APPNAME, 0, 0), g_hInst);
		InitializeClasses();
		g_hWnd = 0;

		break;

	case DLL_PROCESS_DETACH:

		// WriteToLog(TEXT("DLL_PROCESS_DETACH\r\n"));
		// The DLL is being unloaded by a given process.  Do any
		// per-process clean up here, such as undoing what was done in
		// DLL_PROCESS_ATTACH.    The return value is ignored.

		if( g_pImgFactory != NULL ) {g_pImgFactory->Release(); g_pImgFactory = NULL;}
		CoUninitialize();
		if (g_hImgdecmpDll != NULL) {
			FreeLibrary(g_hImgdecmpDll);
		}

		UnregisterClass((LPCTSTR)LoadString(g_hInst, IDS_APPNAME, 0, 0), g_hInst);
		g_hInst = NULL;
		break;
	}

	return TRUE;
}
#endif

/*************************************************************************/
/* Handle any messages that may be needed for the plugin                 */
/* Handled messages:                                                     */
/*        WM_TODAYCUSTOM_QUERYREFRESHCACHE                                 */
/*        WM_CREATE                                                         */
/*        WM_LBUTTONUP                                                     */
/*        WM_PAINT                                                         */
/*        WM_DESTROY                                                         */
/*************************************************************************/
LRESULT CALLBACK WndProc (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
#ifdef DEBUG1
	{
		RECT rc;
		GetClientRect(hwnd, &rc);
		NKDbgPrintfW(L"WndProc(0x%x, %d, %d) (%d %d %d %d)\n", uimessage, wParam, lParam, rc.left, rc.top, rc.right, rc.bottom);
	}
#endif

#ifdef EXEC_MODE
	if (g_bLoading) {
		return WndProcLoading (hwnd, uimessage, wParam, lParam);
	}
#endif

	switch (uimessage)
	{
#ifndef EXEC_MODE
	//check to see if a refresh is required
	case WM_TODAYCUSTOM_QUERYREFRESHCACHE:
		{
			//NKDbgPrintfW(L"WM_TODAYCUSTOM_QUERYREFRESHCACHE %ld\n", GetTickCount());
			// get the pointer to the item from the Today screen
			TODAYLISTITEM *ptliItem = (TODAYLISTITEM*)wParam;

			if ((NULL == ptliItem) || (WaitForSingleObject(SHELL_API_READY_EVENT, 0) == WAIT_TIMEOUT)) {
				return FALSE;
			}
			if (configuracion == NULL || estado == NULL) {
				return FALSE;
			}
			if (!configuracion->updateWhenInactive && configuracion->notifyTimer > 0) {
				static DWORD start = 0;
				DWORD now = GetTickCount();
				if (now - start > (DWORD) configuracion->notifyTimer) {
					start = now;
					PostMessage(hwnd, WM_TIMER, TIMER_ACTUALIZA_NOTIF, 0);
				}
			}
			if (0 == ptliItem->cyp || configuracion->altoPantalla != ptliItem->cyp) {
				ptliItem->cyp = configuracion->altoPantalla;
				return TRUE;
			}
			return FALSE;
		}
		break;
#endif

	case WM_KEYDOWN:
		switch (wParam) {
			case VK_LEFT:
				PostMessage(hwnd, WM_USER_GOTO_PREV, 0, 0);
				break;
			case VK_RIGHT:
				PostMessage(hwnd, WM_USER_GOTO_NEXT, 0, 0);
				break;
			case VK_DOWN:
				PostMessage(hwnd, WM_USER_GOTO_DOWN, 0, 0);
				break;
			case VK_UP:
				PostMessage(hwnd, WM_USER_GOTO_UP, 0, 0);
				break;
			case VK_RETURN:
				PostMessage(hwnd, WM_USER_GOTO, 0, 0);
				break;
		}
		return 0;
	case WM_USER_RELAUNCH:
		if (configuracion!= NULL && configuracion->hasTimestampChanged()) {
			PostMessage(hwnd, WM_CREATE, 0, 0);
		}
		return 0;
	case WM_USER_ADD:
		if (estado != NULL) {
			iconoActual.nIconoActual = -1;
			iconoActual.nPantallaActual = estado->pantallaActiva;
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MENU_ICON), hwnd, (DLGPROC)editaIconoDlgProc);
		}
		return 0;
	case WM_USER_OPTIONS:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGDUMMY), hwnd, (DLGPROC)CustomItemOptionsDlgProc);
		return 0;
	case WM_USER_GOTO:
		if (estado != NULL && configuracion != NULL && listaPantallas != NULL) {
			estado->pantallaActiva = min(max((int) wParam, 0), (int)listaPantallas->numPantallas - 1);
			estado->posObjetivo.x = - (short) (configuracion->anchoPantalla * estado->pantallaActiva);
			estado->posObjetivo.y = 0;
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		}
		return 0;
	case WM_USER_GOTO_NEXT:
		if (estado != NULL && configuracion != NULL && listaPantallas != NULL) {
			estado->pantallaActiva = (estado->pantallaActiva + 1) % listaPantallas->numPantallas;
			estado->posObjetivo.x = - (short) (configuracion->anchoPantalla * estado->pantallaActiva);
			estado->posObjetivo.y = 0;
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		}
		return 0;
	case WM_USER_GOTO_PREV:
		if (estado != NULL && configuracion != NULL && listaPantallas != NULL) {
			estado->pantallaActiva = (listaPantallas->numPantallas + estado->pantallaActiva - 1) % listaPantallas->numPantallas;
			estado->posObjetivo.x = - (short) (configuracion->anchoPantalla * estado->pantallaActiva);
			estado->posObjetivo.y = 0;
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		}
		return 0;
	case WM_USER_GOTO_UP:
		if (estado != NULL && configuracion != NULL) {
			int h = configuracion->altoPantalla;
			if (hasBottomBar()) {
				h -= listaPantallas->barraInferior->altoPantalla;
			}
			if (hasTopBar()) {
				h -= listaPantallas->topBar->altoPantalla;
			}
			int newy = estado->posObjetivo.y + ((h + configuracion->mainScreenConfig->cs.offset.top) / configuracion->mainScreenConfig->distanceIconsV) * configuracion->mainScreenConfig->distanceIconsV;
			if (newy > 0) {
				newy = 0;
			}
			if (estado->posObjetivo.y != newy) {
				estado->posObjetivo.y = newy;
				SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
			}
		}
		return 0;
	case WM_USER_GOTO_DOWN:
		if (estado != NULL && configuracion != NULL && listaPantallas != NULL) {
			int h = configuracion->altoPantalla;
			if (hasBottomBar()) {
				h -= listaPantallas->barraInferior->altoPantalla;
			}
			if (hasTopBar()) {
				h -= listaPantallas->topBar->altoPantalla;
			}
			int newy = estado->posObjetivo.y -
				((h - configuracion->mainScreenConfig->cs.offset.top)
				/ configuracion->mainScreenConfig->distanceIconsV)
				* configuracion->mainScreenConfig->distanceIconsV;
			int hh = ((listaPantallas->listaPantalla[estado->pantallaActiva]->numIconos
				+ configuracion->mainScreenConfig->iconsPerRow - 1)
				/ configuracion->mainScreenConfig->iconsPerRow)
				* configuracion->mainScreenConfig->distanceIconsV;
			if (-newy < hh) {
				estado->posObjetivo.y = newy;
				SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
			}
		}
		return 0;
	case WM_CREATE:
		{
			doDestroy(g_hWnd);
			g_hWnd = hwnd;

#ifdef EXEC_MODE
			g_bLoading = TRUE;
			g_bInitializing = FALSE;
#else
			inicializaApp(hwnd);
#endif

			PostMessage(hwnd, WM_ERASEBKGND, 0, 0);

			RECT rcWindBounds;
			GetClientRect(hwnd, &rcWindBounds);
			InvalidateRect(hwnd, &rcWindBounds, FALSE);
		}
		return 0;

	case WM_LBUTTONDOWN:
		return doMouseDown (hwnd, uimessage, wParam, lParam);
	case WM_LBUTTONUP:
		return doMouseUp (hwnd, uimessage, wParam, lParam);
	case WM_MOUSEMOVE:
		return doMove (hwnd, uimessage, wParam, lParam);
#ifdef EXEC_MODE
	case WM_ACTIVATE:
		return doActivate (hwnd, uimessage, wParam, lParam);
#endif
	case WM_TIMER:
		return doTimer (hwnd, uimessage, wParam, lParam);
	case WM_PAINT:
		return doPaint (hwnd, uimessage, wParam, lParam);
#ifdef EXEC_MODE
	case WM_WININICHANGE:
		RECT rcw;
		if (!GetWindowRect(hwnd, &rcw)) return 0;
		if (configuracion == NULL) return 0;
		if (configuracion->fullscreen) {
			int cxScreen = GetSystemMetrics(SM_CXSCREEN);
			int cyScreen = GetSystemMetrics(SM_CYSCREEN);
			if (rcw.right - rcw.left != cxScreen || rcw.bottom - rcw.top != cyScreen) {
				SetWindowPos(hwnd, NULL, 0, 0, cxScreen, cyScreen, SWP_NOZORDER);
				resizeWindow(hwnd, true);
			}
		} else {
			RECT rcd;
			if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &rcd, 0)) return 0;
			if (rcd.left != rcw.left || rcd.top != rcw.top || rcd.right != rcw.right || rcd.bottom != rcw.bottom) {
				SetWindowPos(hwnd, NULL, rcd.left, rcd.top, rcd.right - rcd.left, rcd.bottom - rcd.top, SWP_NOZORDER);
				resizeWindow(hwnd, true);
			}
		}

		return 0;
#endif
	case WM_SIZE:
#ifdef DEBUG1
		NKDbgPrintfW(L"WM_SIZE (%d, cx = %d, cy = %d)\n", wParam, LOWORD(lParam), HIWORD(lParam));
#endif
		return doSize (hwnd, uimessage, wParam, lParam);
	case WM_DESTROY:
		doDestroy(hwnd);
#ifdef EXEC_MODE
		PostQuitMessage(0);
#endif
		return 0;
	// this fills in the background with the today screen image
	case WM_ERASEBKGND:
#ifdef EXEC_MODE
		return FALSE;
#else
		return doEraseBackground(hwnd);
#endif
	case WM_REGISTRY:
		if (notifications) {
			notifications->Callback(hwnd, uimessage, wParam, lParam);
			if (ProcessNotifications()) {
				RECT rc;
				GetClientRect(hwnd, &rc);
				InvalidateRect(hwnd, &rc, FALSE);
				UpdateWindow(hwnd);
			}
			notifications->ResetChanged();
		}
		return 0;
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam) ;
}

LRESULT doTimer (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	BOOL shouldInvalidateRect = FALSE;
	double movx;
	double movy;
	if (wParam == TIMER_LANZANDO_APP) {

		if (estado->debeCortarTimeOut == TRUE) {
			estado->debeCortarTimeOut = FALSE;

			KillTimer(hwnd, TIMER_LANZANDO_APP);
			if (estado->estadoCuadro == 3) {
				estado->estadoCuadro = 0;
			} else {
				estado->estadoCuadro = 2;

				estado->timeUltimoLanzamiento = GetTickCount();

				if (estado->iconoActivo) {
					BOOL bWorked = FALSE;
					if (hayNotificacion(estado->iconoActivo->tipo) > 0 && _tcsclen(estado->iconoActivo->ejecutableAlt) > 0) {
						bWorked = LaunchApplication(estado->iconoActivo->ejecutableAlt, estado->iconoActivo->parametrosAlt);
					} else if (_tcsclen(estado->iconoActivo->ejecutable) > 0) {
						bWorked = LaunchApplication(estado->iconoActivo->ejecutable, estado->iconoActivo->parametros);
					}
					if (!bWorked) {
						estado->timeUltimoLanzamiento = 0;
					}
				}
			}

			RECT rcWindBounds2;
			GetClientRect(hwnd, &rcWindBounds2);
			InvalidateRect(hwnd, &rcWindBounds2, FALSE);
			UpdateWindow(hwnd);
			return 0;
		}

		// estado->estadoCuadro = 1;
		long despX = configuracion->anchoPantalla / 2;
		long despY = configuracion->altoPantalla / 2;
		long timeInicial = GetTickCount() - estado->timeUltimoLanzamiento;
		float porcent = 1;
		long timeLanzamiento = 200;

		long time = timeLanzamiento - max(0, timeLanzamiento - timeInicial);
		if (time > 0) {
			porcent = (((float) time) / timeLanzamiento);
		}

		despX = (long) (((float) despX) * porcent + 1);
		despY = (long) (((float) despY) * porcent + 1);

		if (estado->estadoCuadro == 3) {
			estado->cuadroLanzando.left = despX;
			estado->cuadroLanzando.right = configuracion->anchoPantalla - despX;
			estado->cuadroLanzando.top = despY;
			estado->cuadroLanzando.bottom = configuracion->altoPantalla - despY;
		} else {
			estado->estadoCuadro = 1;
			estado->cuadroLanzando.left = configuracion->anchoPantalla / 2 - despX;
			estado->cuadroLanzando.right = configuracion->anchoPantalla / 2 + despX;
			estado->cuadroLanzando.top = configuracion->altoPantalla / 2 - despY;
			estado->cuadroLanzando.bottom = configuracion->altoPantalla / 2 + despY;
		}

		if (timeInicial > timeLanzamiento) {
			estado->debeCortarTimeOut = TRUE;
		}

		shouldInvalidateRect = TRUE;

	} else if (wParam == TIMER_ACTUALIZA_NOTIF) {
		//NKDbgPrintfW(L"TIMER_ACTUALIZA_NOTIF %ld\n", GetTickCount());
		if (!estado->hayMovimiento) {
			if (notifications->PollingUpdate()) {
				shouldInvalidateRect = ProcessNotifications();
			}
		}
	} else if (wParam == TIMER_LONGTAP) {
		KillTimer(hwnd, TIMER_LONGTAP);
		RightClick(hwnd, posCursor);
	} else { // TIMER_RECUPERACION
		if (abs(posImage.x - estado->posObjetivo.x) < 2 && abs(posImage.y - estado->posObjetivo.y) < 2) {
			KillTimer(hwnd, wParam);
			posImage = estado->posObjetivo;
			setPosiciones(true, 0, 0);
			estado->hayMovimiento = false;
		} else {
			// movx = abs(posImage.x - estado->posObjetivo.x)*0.30;
			// movy = (posImage.y - estado->posObjetivo.y)*0.30;
			movx = configuracion->velMinima + (configuracion->velMaxima - configuracion->velMinima) * abs(posImage.x - estado->posObjetivo.x) / configuracion->anchoPantalla;
			movy = configuracion->velMinima + (configuracion->velMaxima - configuracion->velMinima) * abs(posImage.y - estado->posObjetivo.y) / configuracion->altoPantalla;

			/*
			if (movx > 0) {
				movx += MIN_VELOCITY;
				/*if (movx > posImage.x - posObjetivo.x) {
					movx = posImage.x - posObjetivo.x;
				}*/
			/*} else if (movx < 0) {
				movx -= MIN_VELOCITY;
				/*if (movx < posImage.x - posObjetivo.x) {
					movx = posImage.x - posObjetivo.x;
				}*/
			/*}*/

			// movx = max(movx, configuracion->velMinima);
			movx++;
			if (posImage.x >= estado->posObjetivo.x) {
				movx = -movx;
			}

			movy++;
			if (posImage.y >= estado->posObjetivo.y) {
				movy = -movy;
			}

			if (abs(posImage.x - estado->posObjetivo.x) < abs(int(movx))) {
				posImage.x = estado->posObjetivo.x;
				movx = 0;
			} else {
				posImage.x += short(movx);
			}

			if (abs(posImage.y - estado->posObjetivo.y) < abs(int(movy))) {
				posImage.y = estado->posObjetivo.y;
				movy = 0;
			} else {
				posImage.y += short(movy);
			}

			if (posImage.x == estado->posObjetivo.x && posImage.y == estado->posObjetivo.y) {
				KillTimer(hwnd, wParam);
				setPosiciones(true, 0, 0);
				estado->hayMovimiento = false;
			} else {
				setPosiciones(false, int(movx), int(movy));
			}
		}
		shouldInvalidateRect = TRUE;
	}

	if (shouldInvalidateRect) {
		RECT rcWindBounds2;
		GetClientRect(hwnd, &rcWindBounds2);
		InvalidateRect(hwnd, &rcWindBounds2, FALSE);
		UpdateWindow(hwnd);
	}

	return 0;
}

LRESULT doSize (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	if (wParam == SIZE_MINIMIZED) {
#ifdef EXEC_MODE
		// PostQuitMessage(0);
		// ShowWindow(hwnd, SW_SHOWNORMAL);
		// resizeWindow(hwnd, TRUE);
		// SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#endif
	} else {
		if (configuracion != NULL && !configuracion->ignoreRotation) {
			int windowWidth;
			int windowHeight;
			getWindowSize(hwnd, &windowWidth, &windowHeight);
			if (windowWidth != configuracion->anchoPantalla || windowHeight != configuracion->altoPantalla) {
				calculateConfiguration(windowWidth, windowHeight);
				configuracion->loadBackgrounds(&hDCMem);
			}
			borraObjetosHDC();
			setPosiciones(true, 0, 0);

			// Calculamos la posicion objetivo
			estado->posObjetivo.x = 0 - (estado->pantallaActiva * configuracion->anchoPantalla);
			estado->posObjetivo.y = 0;

			// Activamos el timer
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		}
	}
	return 0;
}

#ifdef EXEC_MODE
LRESULT WndProcLoading (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	switch (uimessage)
	{
		case WM_PAINT:

		PAINTSTRUCT     ps;
		RECT            rcWindBounds;
		HDC             hDC;

		hDC = BeginPaint(hwnd, &ps);

		SetBkMode(hDC, TRANSPARENT);

		GetClientRect(hwnd, &rcWindBounds);

		HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, &rcWindBounds, hBrush);
		DeleteObject(hBrush);

		SetTextColor(hDC, RGB(255,255,255));
		TCHAR elementText[MAX_PATH];
		StringCchCopy(elementText, CountOf(elementText), L"Loading...");
		DrawText(hDC, elementText, _tcslen( elementText ), &rcWindBounds,
			DT_SINGLELINE | DT_VCENTER | DT_CENTER );

		EndPaint(hwnd, &ps);

		if (!g_bInitializing) {
			g_bInitializing = TRUE;
			// long duration = -(long)GetTickCount();
			inicializaApp(hwnd);
			// duration += GetTickCount();
			// NKDbgPrintfW(L" *** %d \t to inicializaApp.\n", duration);
			g_bLoading = FALSE;
			RECT rcWindBounds;
			GetClientRect(hwnd, &rcWindBounds);
			InvalidateRect(hwnd, &rcWindBounds, FALSE);
			UpdateWindow(hwnd);
		}

		return 0;
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam);
}
#endif

LRESULT doPaint (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	//NKDbgPrintfW(L"WM_PAINT\n");
	PAINTSTRUCT     ps;
	RECT            rcWindBounds;
	HDC             hDC;

	GetClientRect(hwnd, &rcWindBounds);

	hDC = BeginPaint(hwnd, &ps);

	if (configuracion->alphaBlend == 2 && hDCMem2 == NULL) {
		hDCMem2 = CreateCompatibleDC(hDC);
		hbmMem2 = CreateCompatibleBitmap(hDC, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top);
		hbmMemOld2 = (HBITMAP)SelectObject(hDCMem2, hbmMem2);
	}
	if (hDCMem == NULL) {
		hDCMem = CreateCompatibleDC(hDC);
		if (configuracion->alphaBlend == 2) {
			BITMAPINFO bmBackInfo;
			memset(&bmBackInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
			bmBackInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmBackInfo.bmiHeader.biWidth = rcWindBounds.right - rcWindBounds.left;
			bmBackInfo.bmiHeader.biHeight = rcWindBounds.bottom - rcWindBounds.top;
			bmBackInfo.bmiHeader.biPlanes = 1;
			bmBackInfo.bmiHeader.biBitCount = 32;

			bmBack.bmWidth = bmBackInfo.bmiHeader.biWidth;
			bmBack.bmHeight = bmBackInfo.bmiHeader.biHeight;
			bmBack.bmPlanes = bmBackInfo.bmiHeader.biPlanes;
			bmBack.bmBitsPixel = bmBackInfo.bmiHeader.biBitCount;

			hbmMem = CreateDIBSection(hDC, &bmBackInfo, DIB_RGB_COLORS, &bmBack.bmBits, 0, 0);
		} else {
			hbmMem = CreateCompatibleBitmap(hDC, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top);
		}
		hbmMemOld = (HBITMAP)SelectObject(hDCMem, hbmMem);

		SetBkMode(hDCMem, TRANSPARENT);

		hBrushFondo = CreateSolidBrush(configuracion->fondoColor);
		hBrushTrans = CreateSolidBrush(RGB(0, 0, 0));
		hBrushAnimation = CreateSolidBrush(configuracion->colorOfAnimationOnLaunchIcon);
	}

	if (pressed && estado->estadoCuadro == 0 && configuracion->pressedIcon->hDC != NULL
		&& ps.rcPaint.right - ps.rcPaint.left == ps.rcPaint.bottom - ps.rcPaint.top
		&& (ps.rcPaint.right - ps.rcPaint.left == configuracion->mainScreenConfig->iconWidth
			|| ps.rcPaint.right - ps.rcPaint.left == configuracion->bottomBarConfig->iconWidth
			|| ps.rcPaint.right - ps.rcPaint.left == configuracion->topBarConfig->iconWidth)) {
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(hDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
				configuracion->pressedIcon->hDC, 0, 0, configuracion->pressedIcon->anchoImagen, configuracion->pressedIcon->altoImagen, bf);
		EndPaint(hwnd, &ps);
		return 0;
	}

#ifdef EXEC_MODE
	BOOL isTransparent = FALSE;
#else
	BOOL isTransparent = configuracion->fondoTransparente;
#endif
	if (configuracion->fondoPantalla == NULL || configuracion->fondoPantalla->hDC == NULL) {
		FillRect(hDCMem, &rcWindBounds, hBrushFondo);
	} else {
		if (isTransparent) {
			BitBlt(hDCMem, 0, 0, configuracion->fondoPantalla->anchoImagen, configuracion->fondoPantalla->altoImagen,
				configuracion->fondoPantalla->hDC, 0, 0, SRCCOPY);
		} else {
			if (configuracion->fondoPantalla->anchoImagen < configuracion->anchoPantalla ||
				configuracion->fondoPantalla->altoImagen < configuracion->altoPantalla) {
					FillRect(hDCMem, &rcWindBounds, hBrushFondo);
			}
			int destX = 0;
			int destY = 0;
			int srcX = 0;
			int srcY = 0;
			if (configuracion->fondoEstatico) {
				if (configuracion->fondoCenter) {
					destX = ((int) configuracion->anchoPantalla - (int) configuracion->fondoPantalla->anchoImagen) / 2;
					destY = ((int) configuracion->altoPantalla - (int) configuracion->fondoPantalla->altoImagen) / 2;
				}
			} else {
				int posXMin = 0;
				int posXMax = (listaPantallas->numPantallas - 1) * configuracion->anchoPantalla;
				double posX = -1 * posImage.x;
				posX = max(posXMin, min(posXMax, posX));
				posX = posX / posXMax;
				posX = max(posX * ((int)configuracion->fondoPantalla->anchoImagen - (int)configuracion->anchoPantalla), 0);

				int posYMin = 0;
				int posYMax = configuracion->altoPantallaMax - configuracion->altoPantalla;
				double posY = -1 * posImage.y;
				posY = max(posYMin, min(posYMax, posY));
				posY = posY / posYMax;
				posY = max(posY * ((int)configuracion->fondoPantalla->altoImagen - (int)configuracion->altoPantalla), 0);

				if (configuracion->fondoCenter) {
					destX = max(0, ((int) configuracion->anchoPantalla - (int) configuracion->fondoPantalla->anchoImagen) / 2);
					destY = max(0, ((int) configuracion->altoPantalla - (int) configuracion->fondoPantalla->altoImagen) / 2);
				}

				srcX = int(posX);
				srcY = int(posY);
			}
			BitBlt(hDCMem, destX, destY, configuracion->fondoPantalla->anchoImagen, configuracion->fondoPantalla->altoImagen,
				configuracion->fondoPantalla->hDC, srcX, srcY, SRCCOPY);
		}
	}

	// Pintamos los iconos
	pintaIconos(&hDCMem, &rcWindBounds);


	if (estado->estadoCuadro == 1 || estado->estadoCuadro == 3) {
		FillRect(hDCMem, &estado->cuadroLanzando, hBrushAnimation);
	} else if (estado->estadoCuadro == 2) {
		FillRect(hDCMem, &estado->cuadroLanzando, hBrushAnimation);
		if (GetTickCount() - estado->timeUltimoLanzamiento >= 2000) {
			SetTimer(hwnd, TIMER_LANZANDO_APP, configuracion->refreshTime, NULL);
			estado->timeUltimoLanzamiento = GetTickCount();
			estado->estadoCuadro = 3;
		}
	}
	if (configuracion->alphaBlend == 2) {
		// to avoid screen tearing first copy the DIB section of hDCMem to the device compatible bitmap of hDCMem2 and then copy it to the window's device context hDC
		BitBlt(hDCMem2, rcWindBounds.left, rcWindBounds.top, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top, hDCMem, rcWindBounds.left, rcWindBounds.top, SRCCOPY);
		BitBlt(hDC, rcWindBounds.left, rcWindBounds.top, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top, hDCMem2, rcWindBounds.left, rcWindBounds.top, SRCCOPY);
	} else {
		BitBlt(hDC, rcWindBounds.left, rcWindBounds.top, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top, hDCMem, rcWindBounds.left, rcWindBounds.top, SRCCOPY);
	}

	EndPaint(hwnd, &ps);

	return 0;
}

LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	// Cogemos la posicion del raton
	POINTS posCursor2;
	posCursor2 = MAKEPOINTS(lParam);
	//NKDbgPrintfW(L"WM_MOUSEMOVE at (%d,%d)\n", posCursor2.x, posCursor2.y);
	static BOOL movementInitiatedByVertical = FALSE;

	ResetPressed();

	if (!posCursorInitialized) {
		posCursor = posCursor2;
		posCursorInitialized = TRUE;
	}

	// Comprobamos si se ha superado el umbral para considerar que es movimiento
	BOOL flag = estado->hayMovimiento;
	if (configuracion->verticalScroll) {
		if (!flag) {
			movementInitiatedByVertical = abs(posCursor.y - posCursor2.y) > abs(posCursor.x - posCursor2.x) && abs(posCursor.y - posCursor2.y) > int(configuracion->moveThreshold);
		}
		flag = flag || movementInitiatedByVertical;
	}
	flag = flag || abs(posCursor.x - posCursor2.x) > int(configuracion->moveThreshold);
	flag = flag || abs(posCursor.y - posCursor2.y) > int(configuracion->moveThreshold);
	if (flag) {
		KillTimer(hwnd, TIMER_LONGTAP);
		estado->hayMovimiento = true;
		int movx = 0;
		int movy = 0;
		if (configuracion->verticalScroll && movementInitiatedByVertical) {
			movy = int((posCursor2.y - posCursor.y)*(1 + float(configuracion->factorMovimiento) / 10));
		}
		if (!movementInitiatedByVertical || configuracion->freestyleScroll) {
			movx = int((posCursor2.x - posCursor.x)*(1 + float(configuracion->factorMovimiento) / 10));
		}

		if (movx != 0 || movy != 0) {

			if (movx != 0 && (configuracion->ooss_left.stop || configuracion->ooss_right.stop)) {
				int tmp = (int) (configuracion->anchoPantalla * configuracion->ooss_left.stopAt / 100);
				if (configuracion->ooss_left.stop && posImage.x + movx > tmp) {
					movx = tmp - posImage.x;
				} else {
					tmp = -(int) ((listaPantallas->numPantallas - 1) * configuracion->anchoPantalla + configuracion->anchoPantalla * configuracion->ooss_right.stopAt / 100);
					if (configuracion->ooss_right.stop && posImage.x + movx < tmp) {
						movx = tmp - posImage.x;
					}
				}
			}
			if (movy != 0 && (configuracion->ooss_up.stop || configuracion->ooss_down.stop)) {
				int h = configuracion->altoPantalla;
				if (hasBottomBar()) {
					h -= listaPantallas->barraInferior->altoPantalla;
				}
				if (hasTopBar()) {
					h -= listaPantallas->topBar->altoPantalla;
				}
				int tmp = h * configuracion->ooss_up.stopAt / 100;
				if (configuracion->ooss_up.stop && posImage.y + movy > tmp) {
					movy = tmp - posImage.y;
				} else {
					int hh;
					if (configuracion->freestyleScroll) {
						hh = configuracion->altoPantallaMax;
					} else {
						hh = ((listaPantallas->listaPantalla[estado->pantallaActiva]->numIconos
							+ configuracion->mainScreenConfig->iconsPerRow - 1)
							/ configuracion->mainScreenConfig->iconsPerRow)
							* configuracion->mainScreenConfig->distanceIconsV;
					}
					int tmp = -(hh - h + h * (int) configuracion->ooss_down.stopAt / 100);
					if (configuracion->ooss_down.stop && posImage.y + movy < tmp) {
						movy = tmp - posImage.y;
					}
				}
			}

			posCursor = posCursor2;

			if (movx != 0 || movy != 0) {
				posImage.x += movx;
				posImage.y += movy;
				setPosiciones(false, movx, movy);

				// Actualizamos la imagen
				RECT rcWindBounds;
				GetClientRect(hwnd, &rcWindBounds);
				InvalidateRect(hwnd, &rcWindBounds, FALSE);
				// UpdateWindow(hwnd);
			}
		}
	}

	return 0;
}

void RightClick(HWND hwnd, POINTS posCursor)
{
	ResetPressed();

	// Create context menu
	HMENU hmenu = CreatePopupMenu();

	if (hmenu == NULL)
		return;

	procesaPulsacion(hwnd, posCursor, FALSE, TRUE);

	long timeActual = GetTickCount();

	// Add menu
	AppendMenu(hmenu, MF_STRING, MENU_POPUP_ADD, TEXT("Add Icon"));
	if (moveIconoActivo.nIconoActual >= 0 && timeActual - moveTimeUltimaSeleccion < 12000) {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_MOVE_HERE, TEXT("Move Here"));
	}
	if (copyIconoActivo.nIconoActual >= 0 && timeActual - copyTimeUltimaSeleccion < 12000) {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_COPY_HERE, TEXT("Copy Here"));
	}
	AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	if (iconoActual.nIconoActual >= 0) {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_EDIT, TEXT("Edit Icon"));
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_DELETE, TEXT("Delete Icon"));
		//if (moveIconoActivo.nIconoActual == -1 || timeActual - moveTimeUltimaSeleccion >= 12000) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_MOVE, TEXT("Move Icon"));
		//}
		//if (copyIconoActivo.nIconoActual == -1 || timeActual - copyTimeUltimaSeleccion >= 12000) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_COPY, TEXT("Copy Icon"));
		//}
	} else {
		if (configuracion->headerTextSize > 0) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_EDIT_HEADER, TEXT("Edit header"));
		}
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_OPTIONS, TEXT("Options"));
	}
	AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hmenu, MF_STRING, MENU_POPUP_CANCEL, TEXT("Cancel"));
#ifdef EXEC_MODE
	if (configuracion->showExit) {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_EXIT, TEXT("Exit"));
	}
#endif

	POINT pt;

	// Get coordinates of tap
	pt.x = posCursor.x;
	pt.y = posCursor.y;

	ClientToScreen(hwnd, &pt);

	INT iMenuID = TrackPopupMenuEx(hmenu, TPM_RETURNCMD, pt.x, pt.y, hwnd, NULL);

	switch (iMenuID)
	{
		case MENU_POPUP_EDIT_HEADER:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MENU_HEADER), hwnd, (DLGPROC)editHeaderDlgProc);
			break;
		case MENU_POPUP_ADD:
			iconoActual.nIconoActual = -1;
			//iconoActual.nPantallaActual = estado->pantallaActiva;
		case MENU_POPUP_EDIT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MENU_ICON), hwnd, (DLGPROC)editaIconoDlgProc);
			break;
		case MENU_POPUP_OPTIONS:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGDUMMY), hwnd, (DLGPROC)CustomItemOptionsDlgProc);
			break;
		case MENU_POPUP_MOVE:
			moveTimeUltimaSeleccion = timeActual;
			moveIconoActivo.nPantallaActual = iconoActual.nPantallaActual;
			moveIconoActivo.nIconoActual = iconoActual.nIconoActual;
			break;
		case MENU_POPUP_MOVE_HERE:
			if (iconoActual.nIconoActual == -1) {
				iconoActual.nIconoActual = MAX_ICONOS_PANTALLA;
			}
			listaPantallas->mueveIcono(
				moveIconoActivo.nPantallaActual,
				moveIconoActivo.nIconoActual,
				iconoActual.nPantallaActual,
				iconoActual.nIconoActual);
			configuracion->saveXMLIcons(listaPantallas);
			moveIconoActivo.nIconoActual = -1;
			calculateConfiguration(0, 0);
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
			break;
		case MENU_POPUP_COPY:
			copyTimeUltimaSeleccion = timeActual;
			copyIconoActivo.nPantallaActual = iconoActual.nPantallaActual;
			copyIconoActivo.nIconoActual = iconoActual.nIconoActual;
			break;
		case MENU_POPUP_COPY_HERE:
			{
				if (iconoActual.nIconoActual == -1) {
					iconoActual.nIconoActual = MAX_ICONOS_PANTALLA;
				}
				CIcono *destIcon = new CIcono();
				CIcono *srcIcon = listaPantallas->copyIcono(
					copyIconoActivo.nPantallaActual,
					copyIconoActivo.nIconoActual,
					iconoActual.nPantallaActual,
					iconoActual.nIconoActual,
					destIcon);

				StringCchCopy(destIcon->nombre, CountOf(destIcon->nombre), srcIcon->nombre);
				StringCchCopy(destIcon->rutaImagen, CountOf(destIcon->rutaImagen), srcIcon->rutaImagen);
				StringCchCopy(destIcon->ejecutable, CountOf(destIcon->ejecutable), srcIcon->ejecutable);
				StringCchCopy(destIcon->parametros, CountOf(destIcon->parametros), srcIcon->parametros);
				StringCchCopy(destIcon->ejecutableAlt, CountOf(destIcon->ejecutableAlt), srcIcon->ejecutableAlt);
				StringCchCopy(destIcon->parametrosAlt, CountOf(destIcon->parametrosAlt), srcIcon->parametrosAlt);
				destIcon->tipo = srcIcon->tipo;
				destIcon->launchAnimation = srcIcon->launchAnimation;
				SCREEN_TYPE st = MAINSCREEN;
				if (iconoActual.nPantallaActual == -1) {
					st = BOTTOMBAR;
				} else if (iconoActual.nPantallaActual == -2) {
					st = TOPBAR;
				}
				configuracion->loadIconImage(&hDCMem, destIcon, st);

				configuracion->saveXMLIcons(listaPantallas);
				copyIconoActivo.nIconoActual = -1;
				calculateConfiguration(0, 0);
				SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
			}
			break;
		case MENU_POPUP_DELETE:
			int mbResult;
			mbResult =  MessageBox(hwnd, TEXT("Are you sure?"), TEXT("Delete icon"), MB_YESNO);
			if (mbResult == IDYES) {
				if(listaPantallas->borraIcono(iconoActual.nPantallaActual, iconoActual.nIconoActual) == FALSE) {
					MessageBox(hwnd, TEXT("Error deleting icon"), TEXT("Error!"), MB_OK);
				} else {
					setPosiciones(true, 0, 0);
					configuracion->saveXMLIcons(listaPantallas);
					calculateConfiguration(0, 0);
					SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
				}
			}
			break;
		case MENU_POPUP_EXIT:
			PostQuitMessage(0);
			break;
		default:
			break;
	}

	DestroyMenu(hmenu);
}

LRESULT doMouseDown (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	// Cacelamos el timer anterior en caso de haberlo
	KillTimer(hwnd, TIMER_RECUPERACION);
	posCursor = MAKEPOINTS(lParam);
	NKDbgPrintfW(L"WM_LBUTTONDOWN at (%d,%d)\n", posCursor.x, posCursor.y);
	posCursorInitialized = TRUE;

	if (configuracion->pressedIcon->hDC != NULL || _tcsclen(configuracion->pressed_sound) > 0) {
		procesaPulsacion(hwnd, posCursor, FALSE, TRUE);
		if (iconoActual.nIconoActual >= 0) {
			if (lstrcmpi(notifications->szNotifications[SN_PHONEPROFILE], L"Silent") != 0 && _tcsclen(configuracion->pressed_sound) > 0) {
				TCHAR fullPath[MAX_PATH];
				configuracion->getAbsolutePath(fullPath, CountOf(fullPath), configuracion->pressed_sound);
				PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
			}
			if (configuracion->pressedIcon->hDC != NULL) {
				CIcono *icono;
				int x, y, width;
				if (iconoActual.nPantallaActual == -2) {
					icono = listaPantallas->topBar->listaIconos[iconoActual.nIconoActual];
					x = int(icono->x);
					y = int(icono->y);
					width = configuracion->topBarConfig->iconWidth;
				} else if (iconoActual.nPantallaActual == -1) {
					icono = listaPantallas->barraInferior->listaIconos[iconoActual.nIconoActual];
					x = int(icono->x);
					y = int(icono->y) + configuracion->altoPantalla - listaPantallas->barraInferior->altoPantalla;
					width = configuracion->bottomBarConfig->iconWidth;
				} else {
					icono = listaPantallas->listaPantalla[iconoActual.nPantallaActual]->listaIconos[iconoActual.nIconoActual];
					x = int(icono->x);
					int offset = estado->posObjetivo.y;
					if (hasTopBar()) {
						offset += listaPantallas->topBar->altoPantalla;
					}
					y = int(icono->y) + offset;
					width = configuracion->mainScreenConfig->iconWidth;
				}
				pressed = TRUE;
				RECT rc;
				rc.left = x;
				rc.top = y;
				rc.right = rc.left + width;
				rc.bottom = rc.top + width;
				InvalidateRect(hwnd, &rc, FALSE);
				UpdateWindow(hwnd);
			}
		}
	}

	if (configuracion->disableRightClick) {
		return 0;
	}

	// -----------------------------
	// -------- Menu PopUp ---------
	// -----------------------------

	// Initialize SHRGINFO structure
	SHRGINFO shrg;

	shrg.cbSize		= sizeof(shrg);
	shrg.hwndClient	= hwnd;
	shrg.ptDown.x	= LOWORD(lParam);
	shrg.ptDown.y	= HIWORD(lParam);
	shrg.dwFlags	= SHRG_LONGDELAY | SHRG_RETURNCMD;

	DWORD res = -1;
	if (!configuracion->disableRightClickDots) {
		res = SHRecognizeGesture(&shrg);
	}
	if (res == GN_CONTEXTMENU) {
		RightClick(hwnd, posCursor);
		// After the right click menu is shown on WM5 devices the WM_LBUTTONDOWN is not captured.
		// Instead it goes directly to the WM_MOUSEMOVE event hence the doMove will have to initialize the posCursor.
		posCursorInitialized = FALSE;
	} else if (res == -1) {
		SetTimer(hwnd, TIMER_LONGTAP, 1500, NULL);
	}

	return 0;
}

LRESULT doMouseUp (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	POINTS point;
	point = MAKEPOINTS(lParam);
	//NKDbgPrintfW(L"WM_LBUTTONUP at (%d,%d)\n", point.x, point.y);
	bool doubleClick = false;

	ResetPressed();

	KillTimer(hwnd, TIMER_LONGTAP);

	// Detect double click
	if (timeUltimoClick > 0) {
		timeUltimoClick = GetTickCount() - timeUltimoClick;
		if (timeUltimoClick < 400 &&
			abs(point.x - posUltimoClick.x) < 15 &&
			abs(point.y - posUltimoClick.y) < 15) {
			// DOUBLE CLICK!
			doubleClick = true;
			timeUltimoClick = 0;
		} else {
			timeUltimoClick = GetTickCount();
			posUltimoClick = point;
		}
	} else {
		timeUltimoClick = GetTickCount();
		posUltimoClick = point;
	}

	if (estado->hayMovimiento) {
		// Calculamos la posicion objetivo
		calculaPosicionObjetivo();

		// Activamos el timer
		SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
	} else {
		procesaPulsacion(hwnd, point, doubleClick);
	}

	return 0;
}

#ifdef EXEC_MODE
LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE)
	{
		resizeWindow(hwnd, true);
		if (estado->estadoCuadro == 2) {
			estado->timeUltimoLanzamiento = 0;
		}
		PostMessage(hwnd, WM_TIMER, TIMER_ACTUALIZA_NOTIF, 0);
		if (!configuracion->updateWhenInactive && configuracion->notifyTimer > 0) {
			SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, configuracion->notifyTimer, NULL);
		}
	}
	// The window is being deactivated... restore it to non-fullscreen
	else if (!::IsChild(hwnd, (HWND)lParam))
	{
		if (!configuracion->updateWhenInactive) {
			KillTimer(hwnd, TIMER_ACTUALIZA_NOTIF);
		}
		resizeWindow(hwnd, false);
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam);
}
#endif

#ifndef EXEC_MODE
/*************************************************************************/
/* Create and register our window class for the today item                 */
/*************************************************************************/
INT InitializeClasses()
{
	WNDCLASS         wc;
	memset(&wc, 0, sizeof(wc));

	wc.style         = 0;
	wc.lpfnWndProc   = (WNDPROC) WndProc;
	wc.hInstance     = g_hInst;
	wc.hIcon         = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = (LPCTSTR)LoadString(g_hInst, IDS_APPNAME, 0, 0);

	//register our window
	if(!RegisterClass(&wc))
	{
		return 0 ;
	}
	return 1;
}
#endif

void pintaIconos(HDC *hDC, RECT *rcWindBounds)
{
	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		CPantalla *pantalla = listaPantallas->listaPantalla[i];
		pintaPantalla(hDC, pantalla, MAINSCREEN);
	}

	// Pintamos la barra inferior de botones
	BOOL bHasBottomBar = hasBottomBar();
	if (bHasBottomBar) {
		pintaPantalla(hDC, listaPantallas->barraInferior, BOTTOMBAR);
	}

	if (hasTopBar()) {
		pintaPantalla(hDC, listaPantallas->topBar, TOPBAR);
	}

	// Pintamos los circulos para indicar pantalla activa
	RECT posCirculos;
	COLORREF color;
	int anchoCirculo = configuracion->circlesDiameter;
	int distanciaCirculo = configuracion->circlesDistance;
	int nCirculos = listaPantallas->numPantallas;
	int xReferencia;

	if (nCirculos > 1) {
		xReferencia = int((configuracion->anchoPantalla / 2) - ((nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo) / 2);

		posCirculos.top = int(configuracion->altoPantalla) - anchoCirculo - configuracion->circlesOffset;
		if (bHasBottomBar) {
			posCirculos.top -= listaPantallas->barraInferior->altoPantalla;
		}
		posCirculos.bottom = posCirculos.top + anchoCirculo;

		int colorCircle; // Un valor de 0 a 255
		int distanciaMaxima = (int)(((float)listaPantallas->listaPantalla[0]->anchoPantalla) * 1.4f);

		for (int i = 0; i < nCirculos; i++) {
			CPantalla *pantalla = listaPantallas->listaPantalla[i];
			colorCircle = min(distanciaMaxima, abs((int) (pantalla->x)));
			colorCircle = (int) (220.0f - (220.0f / distanciaMaxima) * colorCircle);

			color = RGB(colorCircle,colorCircle,colorCircle);
			/*if (i == estado->pantallaActiva) {
				color = RGB(220,220,220);
			} else {
				color = RGB(0,0,0);
			}*/

			posCirculos.left = xReferencia + i * (anchoCirculo + distanciaCirculo);
			posCirculos.right = posCirculos.left + anchoCirculo;

			drawEllipse(*hDC, posCirculos.left, posCirculos.top, posCirculos.right, posCirculos.bottom, color, NULL);
		}
	}

}

void DrawBubbleText(HDC hDC, CIcono *bubble, int numNotif, CIcono *icon, int iconWidth, BubbleSettings *bs)
{
	TCHAR str[4];
	if (numNotif >= 100) {
		StringCchCopy(str, CountOf(str), TEXT("+"));
	} else if (numNotif < 0) {
		StringCchCopy(str, CountOf(str), TEXT("*"));
	} else {
		StringCchPrintf(str, CountOf(str), TEXT("%i"), numNotif);
	}

	RECT rc;
	rc.left = int(icon->x + bs->x / 100.0 * iconWidth);
	rc.top = int(icon->y + bs->y / 100.0 * iconWidth);
	rc.right = int(rc.left + bs->width / 100.0 * iconWidth);
	rc.bottom = int(rc.top + bs->height / 100.0 * iconWidth);

	LOGFONT lf;
	GetObject((HFONT) GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
	lf.lfWeight = bs->sis.weight;
	lf.lfWidth = LONG(bs->sis.width / 100.0 * iconWidth);
	lf.lfHeight = LONG(bs->sis.height / 100.0 * iconWidth);
	lf.lfQuality = configuracion->textQualityInIcons;
	if (wcslen(bs->sis.facename) > 0) {
		wcsncpy(lf.lfFaceName, bs->sis.facename, CountOf(lf.lfFaceName));
	}

	HFONT hFont = CreateFontIndirect(&lf);
	HFONT hFontOld = (HFONT) SelectObject(hDC, hFont);
	COLORREF colorOld = SetTextColor(hDC, bs->sis.color);

	if (bubble->hDC) {
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			bubble->hDC, 0, 0, bubble->anchoImagen, bubble->altoImagen, bf);
		if (numNotif > 0) {
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;
			rc.left += LONG(bs->sis.offset.left / 100.0 * w);
			rc.right -= LONG(bs->sis.offset.right / 100.0 * w);
			rc.top += LONG(bs->sis.offset.top / 100.0 * w);
			rc.bottom -= LONG(bs->sis.offset.bottom / 100.0 * w);
			DrawText(hDC, str, -1, &rc, DT_CENTER | DT_VCENTER);
		}
	} else {
		drawEllipse(hDC, rc.left, rc.top, rc.right, rc.bottom, RGB(200, 0, 0), str);
	}

	DeleteObject(SelectObject(hDC, hFontOld));
	SetTextColor(hDC, colorOld);
}

void DrawState(HDC hDC, CIcono *bubble, CIcono *icon, int iconWidth, BubbleSettings *bs)
{
	RECT rc;
	rc.left = int(icon->x + bs->x / 100.0 * iconWidth);
	rc.top = int(icon->y + bs->y / 100.0 * iconWidth);
	rc.right = int(rc.left + bs->width / 100.0 * iconWidth);
	rc.bottom = int(rc.top + bs->height / 100.0 * iconWidth);

	if (bubble->hDC) {
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			bubble->hDC, 0, 0, bubble->anchoImagen, bubble->altoImagen, bf);
	} else {
		drawEllipse(hDC, rc.left, rc.top, rc.right, rc.bottom, RGB(0, 200, 0), L"");
	}
}

void DrawSpecialIconText(HDC hDC, TCHAR *str, CIcono *icon, int iconWidth, SpecialIconSettings *sis)
{
	RECT posText;
	LOGFONT lf;
	HFONT hFont;
	HFONT hFontOld;
	COLORREF colorOld;
	HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);

	GetObject(hSysFont, sizeof(LOGFONT), &lf);

	lf.lfWeight = sis->weight;
	lf.lfWidth = LONG(sis->width / 100.0 * iconWidth);
	lf.lfHeight = LONG(sis->height / 100.0 * iconWidth);
	lf.lfQuality = configuracion->textQualityInIcons;
	if (wcslen(sis->facename) > 0) {
		wcsncpy(lf.lfFaceName, sis->facename, CountOf(lf.lfFaceName));
	}

	hFont = CreateFontIndirect(&lf);

#if 0
	// Fix from http://support.microsoft.com/kb/306198 doesn't work...
	if (configuracion->alphaBlend) {
		HDC h = GetDC(g_hWnd);
		hFontOld = (HFONT) SelectObject(h, hFont);
		SelectObject(h, hFontOld);
		ReleaseDC(g_hWnd, h);
	}
#endif

	hFontOld = (HFONT) SelectObject(hDC, hFont);

	colorOld = SetTextColor(hDC, sis->color);

	posText.left = int(icon->x + sis->offset.left / 100.0 * iconWidth);
	posText.top = int(icon->y + sis->offset.top / 100.0 * iconWidth);
	posText.right = int(icon->x + iconWidth - sis->offset.right / 100.0 * iconWidth);
	posText.bottom = int(icon->y + iconWidth - sis->offset.bottom / 100.0 * iconWidth);

	DrawText(hDC, str, -1, &posText, DT_CENTER | DT_VCENTER);

	DeleteObject(SelectObject(hDC, hFontOld));
	SetTextColor(hDC, colorOld);
}

void pintaIcono(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type)
{
	// Iniciamos el proceso de pintado
	RECT posTexto;
	CConfigurationScreen *cs;
	if (screen_type == BOTTOMBAR) {
		cs = configuracion->bottomBarConfig;
	} else if (screen_type == TOPBAR) {
		cs = configuracion->topBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}
	UINT width = cs->iconWidth;
	TCHAR str[16];

	if (icono->hDC && icono->imagen) {
		if (cs->cs.backGradient) {
			if (configuracion->alphaBlend) {
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 255;
				if (cs->cs.backWallpaperAlphaBlend) {
					bf.AlphaFormat = AC_SRC_ALPHA;
				} else {
					bf.AlphaFormat = AC_SRC_ALPHA_NONPREMULT;
				}
				AlphaBlend(*hDC, int(icono->x), int(icono->y), width, width,
					icono->hDC, 0, 0, icono->anchoImagen, icono->altoImagen, bf);
			} else {
				TransparentBlt(*hDC, int(icono->x), int(icono->y), width, width,
					icono->hDC, 0, 0, icono->anchoImagen, icono->altoImagen, RGB(0, 0, 0));
			}
		} else {
			if (icono->anchoImagen == width && icono->altoImagen == width) {
				BitBlt(*hDC, int(icono->x), int(icono->y), width, width,
					icono->hDC, 0, 0, SRCCOPY);
			} else {
				StretchBlt(*hDC, int(icono->x), int(icono->y), width, width,
					icono->hDC, 0, 0, icono->anchoImagen, icono->altoImagen, SRCCOPY);
			}
		}
	}

	// Notifications
	if (icono->tipo != NOTIF_NORMAL) {
		int numNotif = 0;
		switch(icono->tipo) {
			case NOTIF_MISSEDCALLS:
				numNotif = notifications->dwNotifications[SN_PHONEMISSEDCALLS];
				break;
			case NOTIF_VMAIL:
				numNotif = notifications->dwNotifications[SN_MESSAGINGVOICEMAILTOTALUNREAD];
				break;
			case NOTIF_SMS:
				numNotif = notifications->dwNotifications[SN_MESSAGINGSMSUNREAD];
				break;
			case NOTIF_MMS:
				numNotif = notifications->dwNotifications[SN_MESSAGINGMMSUNREAD];
				break;
			case NOTIF_OTHER_EMAIL:
				numNotif = notifications->dwNotifications[SN_MESSAGINGOTHEREMAILUNREAD];
				break;
			case NOTIF_SYNC_EMAIL:
				numNotif = notifications->dwNotifications[SN_MESSAGINGACTIVESYNCEMAILUNREAD];
				break;
			case NOTIF_TOTAL_EMAIL:
				numNotif = notifications->dwNotifications[SN_MESSAGINGOTHEREMAILUNREAD] + notifications->dwNotifications[SN_MESSAGINGACTIVESYNCEMAILUNREAD];
				break;
			case NOTIF_APPOINTS:
				numNotif = notifications->dwNotifications[SN_APPOINTMENTSLISTCOUINT];
			case NOTIF_CALENDAR:
				if (!configuracion->dowUseLocale || !GetDateFormat(LOCALE_USER_DEFAULT, 0, &notifications->st, L"ddd", str, CountOf(str))) {
					StringCchCopy(str, CountOf(str), configuracion->diasSemana[notifications->st.wDayOfWeek]);
				}
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->dow);
				StringCchPrintf(str, CountOf(str), TEXT("%i"), notifications->st.wDay);
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->dom);
				break;
			case NOTIF_ALARM:
				if ((notifications->dwNotifications[SN_CLOCKALARMFLAGS0] + notifications->dwNotifications[SN_CLOCKALARMFLAGS1] + notifications->dwNotifications[SN_CLOCKALARMFLAGS2]) > 0) {
					DrawBubbleText(*hDC, configuracion->bubbleAlarm, -1, icono, width, &configuracion->bubble_alarm);
				}
				break;
			case NOTIF_CLOCK_ALARM:
				if ((notifications->dwNotifications[SN_CLOCKALARMFLAGS0] + notifications->dwNotifications[SN_CLOCKALARMFLAGS1] + notifications->dwNotifications[SN_CLOCKALARMFLAGS2]) > 0) {
					DrawBubbleText(*hDC, configuracion->bubbleAlarm, -1, icono, width, &configuracion->bubble_alarm);
				}
			case NOTIF_CLOCK:
				if (configuracion->clock12Format) {
					StringCchPrintf(str, CountOf(str), TEXT("%d:%02d"), (notifications->st.wHour == 0 ? 12 : (notifications->st.wHour > 12 ? (notifications->st.wHour - 12) : notifications->st.wHour)), notifications->st.wMinute);
				} else {
					StringCchPrintf(str, CountOf(str), TEXT("%02d:%02d"), notifications->st.wHour, notifications->st.wMinute);
				}
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->clck);
				break;
			case NOTIF_BATTERY:
				{
					WORD batteryFlag = LOWORD(notifications->dwNotifications[SN_POWERBATTERYSTATE]);
					BOOL charging = batteryFlag & BATTERY_FLAG_CHARGING;
					// BATTERY_FLAG_HIGH, BATTERY_FLAG_LOW, BATTERY_FLAG_CRITICAL, BATTERY_FLAG_CHARGING
					int batteryLifePercent = HIWORD(notifications->dwNotifications[SN_POWERBATTERYSTATE]);
					if (batteryLifePercent == BATTERY_PERCENTAGE_UNKNOWN) {
						StringCchCopy(str, CountOf(str), L"NA");
					} else {
						if (configuracion->battShowAC) {
							if (charging) {
								StringCchCopy(str, CountOf(str), L"AC");
							} else {
								StringCchPrintf(str, CountOf(str), L"%d", batteryLifePercent);
							}
						} else {
							StringCchPrintf(str, CountOf(str), L"%s%d", charging ? "." : "", batteryLifePercent);
						}
					}
					DrawSpecialIconText(*hDC, str, icono, width, &configuracion->batt);
				}
				break;
			case NOTIF_VOLUME:
				StringCchPrintf(str, CountOf(str), L"%d", ConvertVolumeToPercentage(notifications->dwNotifications[SN_VOLUME]));
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->vol);
				break;
			case NOTIF_MEMORYLOAD:
				StringCchPrintf(str, CountOf(str), L"%d", notifications->memoryStatus.dwMemoryLoad);
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->meml);
				break;
			case NOTIF_MEMORYFREE:
				StringCchPrintf(str, CountOf(str), L"%.1f", notifications->memoryStatus.dwAvailPhys / 1024.0 / 1024.0);
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->memf);
				break;
			case NOTIF_MEMORYUSED:
				StringCchPrintf(str, CountOf(str), L"%.1f", (notifications->memoryStatus.dwTotalPhys - notifications->memoryStatus.dwAvailPhys) / 1024.0 / 1024.0);
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->memu);
				break;
			case NOTIF_MC_SIG_OPER:
				numNotif = notifications->dwNotifications[SN_PHONEMISSEDCALLS];
			case NOTIF_SIGNAL:
			case NOTIF_SIGNAL_OPER:
				if (wcslen(notifications->szNotifications[SN_PHONEOPERATORNAME]) == 0) {
					StringCchCopy(str, CountOf(str), L"NA");
				} else {
					StringCchPrintf(str, CountOf(str), L"%d", notifications->dwNotifications[SN_PHONESIGNALSTRENGTH]);
				}
				DrawSpecialIconText(*hDC, str, icono, width, &configuracion->sign);
				break;
			case NOTIF_TASKS:
				numNotif = notifications->dwNotifications[SN_TASKSACTIVE];
				break;
			case NOTIF_SMS_MMS:
				numNotif = notifications->dwNotifications[SN_MESSAGINGSMSUNREAD] + notifications->dwNotifications[SN_MESSAGINGMMSUNREAD];
				break;
			case NOTIF_WIFI:
				if (notifications->dwNotifications[SN_WIFISTATEPOWERON] & SN_WIFISTATEPOWERON_BITMASK) {
					DrawState(*hDC, configuracion->bubbleState, icono, width, &configuracion->bubble_state);
				}
				break;
			case NOTIF_BLUETOOTH:
				if (notifications->dwNotifications[SN_BLUETOOTHSTATEPOWERON] & SN_BLUETOOTHSTATEPOWERON_BITMASK) {
					DrawState(*hDC, configuracion->bubbleState, icono, width, &configuracion->bubble_state);
				}
				break;
			case NOTIF_IRDA:
				if (notifications->dwNotifications[SN_IRDA] > 0) {
					DrawState(*hDC, configuracion->bubbleState, icono, width, &configuracion->bubble_state);
				}
				break;
			case NOTIF_CELLNETWORK:
				if (notifications->dwNotifications[SN_CELLSYSTEMCONNECTED] > 0) {
					DrawState(*hDC, configuracion->bubbleState, icono, width, &configuracion->bubble_state);
				}
				break;
			default:
				numNotif = 0;
		}
		if (numNotif > 0) {
			DrawBubbleText(*hDC, configuracion->bubbleNotif, numNotif, icono, width, &configuracion->bubble_notif);
		}

	}

	// Pintamos el nombre del icono
	posTexto.top = int(icono->y + width + cs->cs.textOffset);
	posTexto.bottom = posTexto.top + cs->textHeight;
	posTexto.left = int(icono->x - (cs->distanceIconsH * 0.5) + 0.5);
	posTexto.right = int(icono->x + width + (cs->distanceIconsH * 0.5) + 0.5);

	if (cs->textHeight > 0) {
		TCHAR *p = icono->nombre;
		if (icono->tipo == NOTIF_OPERATOR || icono->tipo == NOTIF_SIGNAL_OPER || icono->tipo == NOTIF_MC_SIG_OPER) {
			p = notifications->szNotifications[SN_PHONEOPERATORNAME];
		} else if (icono->tipo == NOTIF_PROFILE) {
			p = notifications->szNotifications[SN_PHONEPROFILE];
		}
		if (p && p[0]) {
			DrawText2(*hDC, p, -1, &posTexto, DT_CENTER | DT_TOP, cs->cs.textRoundRect, cs->cs.textShadow);
		}
	}
}

void pintaPantalla(HDC *hDC, CPantalla *pantalla, SCREEN_TYPE screen_type)
{
	CConfigurationScreen *cs;
	if (screen_type == BOTTOMBAR) {
		cs = configuracion->bottomBarConfig;
	} else if (screen_type == TOPBAR) {
		cs = configuracion->topBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}

	// Si debemos recalcular la pantalla
	if (pantalla->debeActualizar) {
		pantalla->debeActualizar = FALSE;

		RECT rc = {0};
		rc.right = pantalla->anchoPantalla;
		rc.bottom = pantalla->altoPantalla;

		if (pantalla->hDC == NULL) {
			pantalla->hDC = CreateCompatibleDC(*hDC);
			if (configuracion->alphaBlend) {
				BITMAPINFO bmInfo;
				memset(&bmInfo.bmiHeader, 0, sizeof(BITMAPINFOHEADER));
				bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmInfo.bmiHeader.biWidth = pantalla->anchoPantalla;
				bmInfo.bmiHeader.biHeight = pantalla->altoPantalla;
				bmInfo.bmiHeader.biPlanes = 1;
				bmInfo.bmiHeader.biBitCount = 32;
				pantalla->imagen = CreateDIBSection(*hDC, &bmInfo, DIB_RGB_COLORS, (void**)&pantalla->pBits, 0, 0);
			} else {
				pantalla->imagen = CreateCompatibleBitmap(*hDC, pantalla->anchoPantalla, pantalla->altoPantalla);
			}
			pantalla->imagenOld = (HBITMAP)SelectObject(pantalla->hDC, pantalla->imagen);

			HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
			LOGFONT lf;
			GetObject(hSysFont, sizeof(LOGFONT), &lf);
			lf.lfWeight = cs->cs.textWeight;
			lf.lfHeight = cs->textHeight;
			lf.lfQuality = configuracion->textQuality;
			if (wcslen(cs->cs.textFacename) > 0) {
				wcsncpy(lf.lfFaceName, cs->cs.textFacename, CountOf(lf.lfFaceName));
			}

			HFONT hFont = CreateFontIndirect(&lf);

			pantalla->hFontOld = (HFONT)SelectObject(pantalla->hDC, hFont);

			SetTextColor(pantalla->hDC, cs->cs.textColor);
			FillRect(pantalla->hDC, &rc, hBrushTrans);
		}
		SetBkMode(pantalla->hDC, TRANSPARENT);

		if (cs->cs.backGradient) {
			DrawGradientGDI(pantalla->hDC, rc, cs->cs.backColor1,  cs->cs.backColor2,  0xAAAA);
		} else {
			FillRect(pantalla->hDC, &rc, hBrushTrans);
		}

		if (configuracion->headerTextSize > 0 && _tcslen(pantalla->header) > 0) {
			LOGFONT lf;
			HFONT hFont;
			HFONT hFontOld;
			COLORREF colorOld;
			HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
			GetObject(hSysFont, sizeof(LOGFONT), &lf);

			lf.lfWeight = configuracion->headerTextWeight;
			lf.lfHeight = configuracion->headerTextSize;
			lf.lfQuality = configuracion->textQuality;
			if (wcslen(configuracion->headerTextFacename) > 0) {
				wcsncpy(lf.lfFaceName, configuracion->headerTextFacename, CountOf(lf.lfFaceName));
			}

			hFont = CreateFontIndirect(&lf);
			hFontOld = (HFONT) SelectObject(pantalla->hDC, hFont);
			colorOld = SetTextColor(pantalla->hDC, configuracion->headerTextColor);

			RECT posTexto;
			posTexto.left = 0;
			posTexto.right = pantalla->anchoPantalla;
			posTexto.top = configuracion->headerOffset;
			posTexto.bottom = configuracion->headerTextSize + configuracion->headerOffset;

			DrawText2(pantalla->hDC, pantalla->header, -1, &posTexto, DT_CENTER | DT_VCENTER, configuracion->headerTextRoundRect, configuracion->headerTextShadow);

			DeleteObject(SelectObject(pantalla->hDC, hFontOld));
			SetTextColor(pantalla->hDC, colorOld);
		}

		setPosicionesIconos(pantalla, screen_type);

		for (UINT j = 0; j < pantalla->numIconos; j++) {
			CIcono *icono = pantalla->listaIconos[j];
			pintaIcono(&pantalla->hDC, icono, screen_type);
		}

		if (configuracion->alphaBlend && (!cs->cs.backGradient || (cs->cs.backGradient && cs->cs.backWallpaperAlphaBlend))) {
			if (pantalla->pBits) {
				BYTE *p = pantalla->pBits;
				for (UINT i = 0; i < pantalla->anchoPantalla * pantalla->altoPantalla; i++) {
					BYTE A = p[3];
					if (A == 0 && (p[0] != 0 || p[1] != 0 || p[2] != 0)) {
						p[3] = 0xFF;
					} else {
						p[0] = (BYTE)((p[0] * A) >> 8);
						p[1] = (BYTE)((p[1] * A) >> 8);
						p[2] = (BYTE)((p[2] * A) >> 8);
					}
					p += 4;
				}
			}
		} else if (configuracion->useMask) {
			if (pantalla->mask_hDC == NULL) {
				pantalla->mask_hDC = CreateCompatibleDC(*hDC);
				pantalla->mask_imagen = CreateBitmap(pantalla->anchoPantalla, pantalla->altoPantalla, 1, 1, NULL);
				pantalla->mask_imagenOld = (HBITMAP) SelectObject(pantalla->mask_hDC, pantalla->mask_imagen);
			}
			SetBkColor(pantalla->hDC, RGB(0, 0, 0));
			BitBlt(pantalla->mask_hDC, 0, 0, pantalla->anchoPantalla, pantalla->altoPantalla, pantalla->hDC, 0, 0, SRCCOPY);
		}
	}

	// Pintamos la pantalla
	if (pantalla->x + configuracion->anchoPantalla >= 0 && pantalla->x < configuracion->anchoPantalla) {
		int posX = int(pantalla->x);
		int posY = int(pantalla->y);
		int xDestOrg = max(posX, 0);
		int yDestOrg = max(posY, 0);
		int xSrcOrg = abs(min(posX, 0));
		int ySrcOrg = abs(min(posY, 0));
		int cx = configuracion->anchoPantalla - abs(posX);
		int cy;
		if (screen_type != MAINSCREEN) {
			cy = pantalla->altoPantalla;
		} else {
			if (hasTopBar()) {
				yDestOrg = max(posY, int(listaPantallas->topBar->altoPantalla));
				ySrcOrg = abs(min(posY - int(listaPantallas->topBar->altoPantalla), 0));
			}
			cy = configuracion->altoPantalla - yDestOrg;
			if (hasBottomBar()) {
				cy -= listaPantallas->barraInferior->altoPantalla;
			}
		}

		// Print background of static bars
		CIcono *back = NULL;
		if (screen_type == BOTTOMBAR) {
			back = configuracion->backBottomBar;
		} else if (screen_type == TOPBAR) {
			back = configuracion->backTopBar;
		}
		if (back && back->hDC) {
			int bcx = min(cx, (int) back->anchoImagen);
			int bcy = min(cy, (int) back->altoImagen);
			int bxDestOrg = xDestOrg;
			int byDestOrg = yDestOrg;
			int bxSrcOrg = xSrcOrg;
			int bySrcOrg = ySrcOrg;
			if (cs->cs.backWallpaperCenter) {
				if ((int) back->anchoImagen < cx) {
					bxDestOrg += (cx - back->anchoImagen) / 2;
				} else if ((int) back->anchoImagen > cx) {
					bxSrcOrg += (back->anchoImagen - cx) / 2;
				}
				if ((int) back->altoImagen < cy) {
					byDestOrg += (cy - back->altoImagen) / 2;
				} else if ((int) back->altoImagen > cy) {
					bySrcOrg += (back->altoImagen - cy) / 2;
				}
			}
			BOOL ab = FALSE;
			if (configuracion->alphaBlend && cs->cs.backWallpaperAlphaBlend) {
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				ab = AlphaBlend(*hDC, bxDestOrg, byDestOrg, bcx, bcy, back->hDC, bxSrcOrg, bySrcOrg, bcx, bcy, bf);
			}
			if (!ab) {
				BitBlt(*hDC, bxDestOrg, byDestOrg, bcx, bcy, back->hDC, bxSrcOrg, bySrcOrg, SRCCOPY);
			}
		}

		BOOL ab = FALSE;
		if (configuracion->alphaBlend && (!cs->cs.backGradient || (cs->cs.backGradient && cs->cs.backWallpaperAlphaBlend))) {
			if (configuracion->alphaBlend == 2 && bmBack.bmBits) {
				BITMAP bmScreen = {0};
				bmScreen.bmWidth = pantalla->anchoPantalla;
				bmScreen.bmHeight = pantalla->altoPantalla;
				bmScreen.bmBits = pantalla->pBits;
				bmScreen.bmBitsPixel = 32;
				ab = AlphaBlend2(&bmBack, xDestOrg, yDestOrg, &bmScreen, xSrcOrg, ySrcOrg, cx, cy);
			} else {
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				ab = AlphaBlend(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->hDC, xSrcOrg, ySrcOrg, cx, cy, bf);
			}
		}
		if (!ab) {
			if (cs->cs.backGradient) {
				BitBlt(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->hDC, xSrcOrg, ySrcOrg, SRCCOPY);
			} else {
				if (configuracion->useMask && pantalla->mask_hDC) {
					BitBlt(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->hDC, xSrcOrg, ySrcOrg, SRCINVERT);
					BitBlt(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->mask_hDC, xSrcOrg, ySrcOrg, SRCAND);
					BitBlt(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->hDC, xSrcOrg, ySrcOrg, SRCINVERT);
				} else {
					TransparentBlt(*hDC, xDestOrg, yDestOrg, cx, cy, pantalla->hDC, xSrcOrg, ySrcOrg, cx, cy, RGB(0, 0, 0));
				}
			}
		}
	}
}

void setPosicionesIconos(CPantalla *pantalla, SCREEN_TYPE screen_type)
{
	CConfigurationScreen *cs;
	if (screen_type == BOTTOMBAR) {
		cs = configuracion->bottomBarConfig;
	} else if (screen_type == TOPBAR) {
		cs = configuracion->topBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}

	for (UINT i = 0; i < pantalla->numIconos; i++) {
		CIcono *icono = pantalla->listaIconos[i];

		icono->x = cs->posReference.x + float(int(i % cs->iconsPerRow) * cs->distanceIconsH);
		icono->y = cs->posReference.y + float(int((i / cs->iconsPerRow)) * cs->distanceIconsV);
	}
}

void setPosiciones(BOOL inicializa, int offsetX, int offsetY)
{
	if (hasBottomBar()) {
		listaPantallas->barraInferior->altoPantalla = configuracion->bottomBarConfig->distanceIconsV + configuracion->bottomBarConfig->cs.offset.top + configuracion->bottomBarConfig->cs.offset.bottom;
		listaPantallas->barraInferior->anchoPantalla = configuracion->anchoPantalla;
		listaPantallas->barraInferior->x = 0;
		listaPantallas->barraInferior->y = float(configuracion->altoPantalla - listaPantallas->barraInferior->altoPantalla);
	}

	if (hasTopBar()) {
		listaPantallas->topBar->altoPantalla = configuracion->topBarConfig->distanceIconsV + configuracion->topBarConfig->cs.offset.top + configuracion->topBarConfig->cs.offset.bottom;
		listaPantallas->topBar->anchoPantalla = configuracion->anchoPantalla;
		listaPantallas->topBar->x = 0;
		listaPantallas->topBar->y = 0;
	}

	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		CPantalla *pantalla = listaPantallas->listaPantalla[i];

		if (inicializa) {
			pantalla->x = estado->posObjetivo.x + float(i * configuracion->anchoPantalla);
			pantalla->y = estado->posObjetivo.y;
			if (hasTopBar()) {
				pantalla->y += listaPantallas->topBar->altoPantalla;
			}
		} else {
			pantalla->x += offsetX;
			pantalla->y += offsetY;
		}

		pantalla->altoPantalla = configuracion->altoPantallaMax;
		pantalla->anchoPantalla = configuracion->anchoPantalla;
	}
}

void calculaPosicionObjetivo()
{
	//NKDbgPrintfW(L"posImage = %d %d\n", posImage.x, posImage.y);
	UINT pantallaActivaOld = estado->pantallaActiva;
	estado->pantallaActiva = posImage.x > 0 ? 0 : min(listaPantallas->numPantallas - 1, (-posImage.x + configuracion->anchoPantalla / 2) / configuracion->anchoPantalla);

	if (posImage.x >= (int) configuracion->anchoPantalla * 4 / 5) {
		if (_tcsclen(configuracion->ooss_left.exec) > 0) {
			LaunchApplication(configuracion->ooss_left.exec, L"");
		} else if (configuracion->ooss_left.wrap) {
			estado->pantallaActiva = listaPantallas->numPantallas - 1;
		}
	} else if (-posImage.x - (int) (listaPantallas->numPantallas - 1) * (int) configuracion->anchoPantalla >= (int) configuracion->anchoPantalla * 4 / 5) {
		if (_tcsclen(configuracion->ooss_right.exec) > 0) {
			LaunchApplication(configuracion->ooss_right.exec, L"");
		} else if (configuracion->ooss_right.wrap) {
			estado->pantallaActiva = 0;
		}
	}

	estado->posObjetivo.x = 0 - (estado->pantallaActiva * configuracion->anchoPantalla);

	int h = configuracion->altoPantalla;
	if (hasBottomBar()) {
		h -= listaPantallas->barraInferior->altoPantalla;
	}
	if (hasTopBar()) {
		h -= listaPantallas->topBar->altoPantalla;
	}
	int nrows_screen = ((listaPantallas->listaPantalla[estado->pantallaActiva]->numIconos + configuracion->mainScreenConfig->iconsPerRow - 1) / configuracion->mainScreenConfig->iconsPerRow);

	if (posImage.y >= 0 || pantallaActivaOld != estado->pantallaActiva) {
		estado->posObjetivo.y = 0;
	} else {
		int nrows_scroll = (-posImage.y + configuracion->mainScreenConfig->distanceIconsV / 2) / configuracion->mainScreenConfig->distanceIconsV;
		int nrows_fit = (h - configuracion->mainScreenConfig->cs.offset.top) / configuracion->mainScreenConfig->distanceIconsV;
		if (nrows_scroll > nrows_screen - nrows_fit) {
			nrows_scroll = nrows_screen - nrows_fit;
			if (nrows_scroll < 0) {
				nrows_scroll = 0;
			}
		}
		estado->posObjetivo.y = - nrows_scroll * (int) configuracion->mainScreenConfig->distanceIconsV;
		if (-posImage.y >= nrows_screen * (int) configuracion->mainScreenConfig->distanceIconsV) {
			if (_tcsclen(configuracion->ooss_down.exec) > 0) {
				LaunchApplication(configuracion->ooss_down.exec, L"");
			} else if (configuracion->ooss_down.wrap) {
				estado->posObjetivo.y = 0;
			}
		}
	}

	if (posImage.y >= h) {
		if (_tcsclen(configuracion->ooss_up.exec) > 0) {
			LaunchApplication(configuracion->ooss_up.exec, L"");
		} else if (configuracion->ooss_up.wrap) {
			estado->posObjetivo.y = -(nrows_screen * (int) configuracion->mainScreenConfig->distanceIconsV - h);
		}
	}
}

#ifndef EXEC_MODE
/*************************************************************************/
/* Initialize the DLL by creating a new window                             */
/*************************************************************************/
HWND InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent)
{
#ifdef DEBUG1
	// Infinite loop to attach the debugger to mstli.exe (Pocket PC) or shell32.exe (Windows Mobile)
	// Set a breakpoint and change value of skip to TRUE
	static BOOL skip = FALSE;
	//while (!skip) {
	for (int s = 0; s < 30; s++) {
		if (skip) break;
		Sleep(1000);
	}
#endif
	// WriteToLog(TEXT("InitializeCustomItem\r\n"));
	LPCTSTR appName = (LPCTSTR)LoadString(g_hInst, IDS_APPNAME, 0, 0);

	//create a new window
	g_hWnd = CreateWindow(appName, appName, WS_VISIBLE | WS_CHILD,
		CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), 0, hwndParent, NULL, g_hInst, 0);

	// attach our winproc to the newly created window
	SetWindowLong(g_hWnd, GWL_WNDPROC, (LONG) WndProc);

	//display the window
	ShowWindow (g_hWnd, SW_SHOWNORMAL);
	UpdateWindow (g_hWnd) ;

	return g_hWnd;
}
#endif

/*************************************************************************/
/* Message Handler for the options dialog                                */
/*************************************************************************/
LRESULT WINAPI CustomItemOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_INITDIALOG:
			{
				// This export function can only fire up a standard dialog
				// when the Options button is pressed. We will treat this
				// as a dummy dialog and create another modal property sheet
				// via the export function to use with the application.
				//
				// Remember to set Resource Value of the dummy dialog to 500
				// or else clicking on Options button will do nothing.

				// Close the dummy dialog immediately since we don't need it
				// Only start the property sheet if the dummy dialog is closed properly
				if (EndDialog(hDlg, TRUE))
				{
					// Initialize parameters for the edit controls
					// This is used for auto SIP show/hide when any edit control is focused/unfocused
					// See http://msdn2.microsoft.com/en-us/library/ms912025.aspx for more information.
					SHInitExtraControls();
					// In addition, we must include this line at the end of each affected dialog in the .rc file:
					// CONTROL         "",-1,"SIPPREF",NOT WS_VISIBLE,-10,-10,6,6


					// Create property sheet for option dialogs
					CreatePropertySheet(hDlg);
				}


				return TRUE;

			}

			break;


		// Though redundant, we make sure the dummy dialog is closed properly just in case it failed to do so during initialization
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;


		default:
			break;

	}  // End message switch

	return 0;  // Return FALSE indicates the message is NOT processed.

}  // End CustomItemOptionsDlgProc

BOOL LaunchApplication(LPCTSTR pCmdLine, LPCTSTR pParametros)
{
	if (pCmdLine == NULL) {
		return FALSE;
	}

	if (wcsncmp(pCmdLine, L"--", 2) == 0) {
		if (CommandLineArguements(g_hWnd, pCmdLine + 2)) {
			return FALSE;
		}
	}

	TCHAR fullPath[MAX_PATH];
	configuracion->getAbsolutePath(fullPath, CountOf(fullPath), pCmdLine);

	// Launch de application
	BOOL bWorked;
	SHELLEXECUTEINFO sei;

	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);

	if (FileExists(fullPath)) {
		sei.lpFile = fullPath;
	} else {
		sei.lpFile = pCmdLine;
	}
	sei.nShow = SW_SHOWNORMAL;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = pParametros;

	bWorked = ShellExecuteEx(&sei);

#ifdef EXEC_MODE
	if (bWorked && configuracion->closeOnLaunchIcon) {
		PostQuitMessage(0);
	}
	if (bWorked && configuracion->minimizeOnLaunchIcon) {
		ShowWindow(g_hWnd, SW_MINIMIZE);
	}
#endif

	return bWorked;
}

void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar)
{
	BOOL bHasBottomBar = hasBottomBar();
	BOOL bHasTopBar = hasTopBar();

	int nCirculos = listaPantallas->numPantallas;
	int anchoCirculo = configuracion->circlesDiameter;
	int distanciaCirculo = configuracion->circlesDistance;
	int xLeft, xRight, yTop, yBottom;

	// Calculamos el cuadrado recubridor de la barra de circulos activos
	xLeft = int((configuracion->anchoPantalla / 2) - ((nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo) / 2);
	xRight = xLeft + (nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo;

	yTop = int(configuracion->altoPantalla) - anchoCirculo - configuracion->circlesOffset;
	if (bHasBottomBar) {
		yTop -= listaPantallas->barraInferior->altoPantalla;
	}
	yBottom = yTop + anchoCirculo;

	// Expandimos ligerammente dicho cuadro para facilitar el doble click
	// xLeft -= int(anchoCirculo * 1.80);
	// xRight += int(anchoCirculo * 1.50);
	// yTop -= int(anchoCirculo * 1.40);
	// yBottom += int(anchoCirculo);
	//yTop    -= anchoCirculo;
	//yBottom += anchoCirculo;
	if (posCursor.x >= xLeft && posCursor.x <= xRight && posCursor.y >= yTop && posCursor.y <= yBottom) {
		if (doubleClick) {
			estado->pantallaActiva = 0;
		} else {
			estado->pantallaActiva = (posCursor.x - xLeft) / (anchoCirculo + distanciaCirculo);
		}
		estado->posObjetivo.x = - (short) (configuracion->anchoPantalla * estado->pantallaActiva);
		estado->posObjetivo.y = 0;

		// Activamos el timer
		SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		return;
	}

	CPantalla *pantalla;
	CIcono *icono = NULL;
	int iconWidth;

	BOOL isClickOnBottomBar = bHasBottomBar
		&& posCursor.x >= listaPantallas->barraInferior->x && posCursor.x <= (listaPantallas->barraInferior->x + listaPantallas->barraInferior->anchoPantalla)
		&& posCursor.y >= listaPantallas->barraInferior->y && posCursor.y <= (listaPantallas->barraInferior->y + listaPantallas->barraInferior->altoPantalla);
	BOOL isClickOnTopBar = bHasTopBar
		&& posCursor.x >= listaPantallas->topBar->x && posCursor.x <= (listaPantallas->topBar->x + listaPantallas->topBar->anchoPantalla)
		&& posCursor.y >= listaPantallas->topBar->y && posCursor.y <= (listaPantallas->topBar->y + listaPantallas->topBar->altoPantalla);

	if (isClickOnBottomBar) {
		pantalla = listaPantallas->barraInferior;
		iconWidth = configuracion->bottomBarConfig->iconWidth;
		iconoActual.nPantallaActual = -1;
	} else if (isClickOnTopBar) {
		pantalla = listaPantallas->topBar;
		iconWidth = configuracion->topBarConfig->iconWidth;
		iconoActual.nPantallaActual = -2;
	} else {
		pantalla = listaPantallas->listaPantalla[estado->pantallaActiva];
		iconWidth = configuracion->mainScreenConfig->iconWidth;
		iconoActual.nPantallaActual = estado->pantallaActiva;
	}
	iconoActual.nIconoActual = -1;

	for (UINT i = 0; i < pantalla->numIconos; i++) {
		icono = pantalla->listaIconos[i];
		if (posCursor.x >= pantalla->x + icono->x && posCursor.x <= pantalla->x + icono->x + iconWidth &&
			posCursor.y >= pantalla->y + icono->y && posCursor.y <= pantalla->y + icono->y + iconWidth) {
				iconoActual.nIconoActual = i;
				break;
		}
	}

	if (!noLanzar && iconoActual.nIconoActual >= 0) {

		if (lstrcmpi(notifications->szNotifications[SN_PHONEPROFILE], L"Silent") != 0) {
			if (configuracion->allowSoundOnLaunchIcon) {
				TCHAR fullPath[MAX_PATH];
				if (_tcsclen(icono->sound) > 0) {
					configuracion->getAbsolutePath(fullPath, CountOf(fullPath), icono->sound);
					PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				} else if (_tcsclen(configuracion->soundOnLaunchIcon) > 0) {
					configuracion->getAbsolutePath(fullPath, CountOf(fullPath), configuracion->soundOnLaunchIcon);
					PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
				}
			}

			// Vibration
			if (configuracion->vibrateOnLaunchIcon > 0) {
				vibrate(configuracion->vibrateOnLaunchIcon);
			}
		}

		// Activamos el timer
		if (configuracion->allowAnimationOnLaunchIcon && icono->launchAnimation > 0) {
			SetTimer(hwnd, TIMER_LANZANDO_APP, configuracion->refreshTime, NULL);
			estado->timeUltimoLanzamiento = GetTickCount();

			estado->iconoActivo = icono;
		} else {
			if (hayNotificacion(icono->tipo) > 0 && _tcsclen(icono->ejecutableAlt) > 0) {
				LaunchApplication(icono->ejecutableAlt, icono->parametrosAlt);
			} else if (_tcsclen(icono->ejecutable) > 0) {
				LaunchApplication(icono->ejecutable, icono->parametros);
			}
		}
	}
}

int hayNotificacion(int tipo)
{
	int numNotif = 0;
	switch(tipo) {
		case NOTIF_MISSEDCALLS:
			numNotif = notifications->dwNotifications[SN_PHONEMISSEDCALLS];
			break;
		case NOTIF_VMAIL:
			numNotif = notifications->dwNotifications[SN_MESSAGINGVOICEMAILTOTALUNREAD];
			break;
		case NOTIF_SMS:
			numNotif = notifications->dwNotifications[SN_MESSAGINGSMSUNREAD];
			break;
		case NOTIF_MMS:
			numNotif = notifications->dwNotifications[SN_MESSAGINGMMSUNREAD];
			break;
		case NOTIF_OTHER_EMAIL:
			numNotif = notifications->dwNotifications[SN_MESSAGINGOTHEREMAILUNREAD];
			break;
		case NOTIF_SYNC_EMAIL:
			numNotif = notifications->dwNotifications[SN_MESSAGINGACTIVESYNCEMAILUNREAD];
			break;
		case NOTIF_TOTAL_EMAIL:
			numNotif = notifications->dwNotifications[SN_MESSAGINGOTHEREMAILUNREAD] + notifications->dwNotifications[SN_MESSAGINGACTIVESYNCEMAILUNREAD];
			break;
		case NOTIF_APPOINTS:
			numNotif = notifications->dwNotifications[SN_APPOINTMENTSLISTCOUINT];
			break;
		case NOTIF_TASKS:
			numNotif = notifications->dwNotifications[SN_TASKSACTIVE];
			break;
		case NOTIF_SMS_MMS:
			numNotif = notifications->dwNotifications[SN_MESSAGINGSMSUNREAD] + notifications->dwNotifications[SN_MESSAGINGMMSUNREAD];
			break;
		case NOTIF_WIFI:
			numNotif = notifications->dwNotifications[SN_WIFISTATEPOWERON] & SN_WIFISTATEPOWERON_BITMASK;
			break;
		case NOTIF_BLUETOOTH:
			numNotif = notifications->dwNotifications[SN_BLUETOOTHSTATEPOWERON] & SN_BLUETOOTHSTATEPOWERON_BITMASK;
			break;
		case NOTIF_IRDA:
			numNotif = notifications->dwNotifications[SN_IRDA] > 0;
			break;
		case NOTIF_CELLNETWORK:
			numNotif = notifications->dwNotifications[SN_CELLSYSTEMCONNECTED] > 0;
			break;
		case NOTIF_CLOCK_ALARM:
		case NOTIF_ALARM:
			numNotif = (notifications->dwNotifications[SN_CLOCKALARMFLAGS0] + notifications->dwNotifications[SN_CLOCKALARMFLAGS1] + notifications->dwNotifications[SN_CLOCKALARMFLAGS2]) > 0;
			break;
		default:
			numNotif = 0;
	}

	return numNotif;
}

LRESULT CALLBACK editaIconoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			if (FindWindow(L"MS_SIPBUTTON", NULL) != NULL) {
				SHMENUBARINFO mbi;

				memset(&mbi, 0, sizeof(SHMENUBARINFO));  // Reset mbi to 0.
				mbi.cbSize = sizeof(SHMENUBARINFO);
				mbi.dwFlags = SHCMBF_EMPTYBAR;
				mbi.hwndParent = hDlg;  // Soft key bar's owner.
				mbi.nToolBarId = NULL;  // Soft key bar resource.
				mbi.hInstRes = NULL;  // HINST in which resource is located.

				g_hWndMenuBar = mbi.hwndMB;

				if (g_hWndMenuBar) {
					CommandBar_Destroy(g_hWndMenuBar);
				}
				// Create the Soft key bar.
				if (!SHCreateMenuBar(&mbi))
				{
					g_hWndMenuBar = NULL;
				}
				else
				{
					g_hWndMenuBar = mbi.hwndMB;
				}
			}

			SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			if (FindWindow(L"MS_SIPBUTTON", NULL) == NULL) {
				SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CONTEXTHELP | WS_EX_CAPTIONOKBTN);
			}

			// Configuramos el elemento Screen
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_MICON_SCREEN), 0);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETRANGE, 0, MAKELPARAM(MAX_PANTALLAS - 1, -2));
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETPOS, 0, iconoActual.nPantallaActual);

			// Configuramos el elemento Icon
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_MICON_ICON), 0);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETRANGE, 0, MAKELPARAM(MAX_ICONOS_PANTALLA - 1, 0));
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETPOS, 0, max(iconoActual.nIconoActual, 0));

			// Configuramos el elemento Type
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_NORMAL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MISSEDCALLS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_OTHER_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SYNC_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_TOTAL_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_APPOINTS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CALENDAR_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_TASKS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SMS_MMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_WIFI_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_BLUETOOTH_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_ALARM_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CLOCK_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CLOCK_ALARM_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_BATTERY_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_VOLUME_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MEMORYLOAD_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MEMORYFREE_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MEMORYUSED_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CELLNETWORK_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SIGNAL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_OPERATOR_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SIGNAL_OPER_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MC_SIG_OPER_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_PROFILE_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_VMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_IRDA_TXT);

			// Configuramos los checks
			SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, BST_CHECKED, 0);

			// Si es editar ponemos los valores
			if (iconoActual.nIconoActual >= 0) {
				CIcono *icono = NULL;
				if (iconoActual.nPantallaActual == -1) {
					icono = listaPantallas->barraInferior->listaIconos[iconoActual.nIconoActual];
				} else if (iconoActual.nPantallaActual == -2) {
					icono = listaPantallas->topBar->listaIconos[iconoActual.nIconoActual];
				} else {
					icono = listaPantallas->listaPantalla[iconoActual.nPantallaActual]->listaIconos[iconoActual.nIconoActual];
				}

				SetDlgItemText(hDlg, IDC_MICON_NAME, icono->nombre);
				SetDlgItemText(hDlg, IDC_MICON_IMAGE, icono->rutaImagen);
				SetDlgItemText(hDlg, IDC_MICON_SOUND, icono->sound);
				SetDlgItemText(hDlg, IDC_MICON_EXEC, icono->ejecutable);
				SetDlgItemText(hDlg, IDC_MICON_PARAMETERS, icono->parametros);
				SetDlgItemText(hDlg, IDC_MICON_EXECALT, icono->ejecutableAlt);
				SetDlgItemText(hDlg, IDC_MICON_PARAMETERSALT, icono->parametrosAlt);

				SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, icono->launchAnimation ? BST_CHECKED : BST_UNCHECKED, 0);

				if (icono->tipo == NOTIF_NORMAL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 0, 0);
				} else if (icono->tipo == NOTIF_MISSEDCALLS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 1, 0);
				} else if (icono->tipo == NOTIF_SMS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 2, 0);
				} else if (icono->tipo == NOTIF_MMS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 3, 0);
				} else if (icono->tipo == NOTIF_OTHER_EMAIL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 4, 0);
				} else if (icono->tipo == NOTIF_SYNC_EMAIL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 5, 0);
				} else if (icono->tipo == NOTIF_TOTAL_EMAIL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 6, 0);
				} else if (icono->tipo == NOTIF_APPOINTS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 7, 0);
				} else if (icono->tipo == NOTIF_CALENDAR) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 8, 0);
				} else if (icono->tipo == NOTIF_TASKS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 9, 0);
				} else if (icono->tipo == NOTIF_SMS_MMS) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 10, 0);
				} else if (icono->tipo == NOTIF_WIFI) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 11, 0);
				} else if (icono->tipo == NOTIF_BLUETOOTH) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 12, 0);
				} else if (icono->tipo == NOTIF_ALARM) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 13, 0);
				} else if (icono->tipo == NOTIF_CLOCK) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 14, 0);
				} else if (icono->tipo == NOTIF_CLOCK_ALARM) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 15, 0);
				} else if (icono->tipo == NOTIF_BATTERY) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 16, 0);
				} else if (icono->tipo == NOTIF_VOLUME) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 17, 0);
				} else if (icono->tipo == NOTIF_MEMORYLOAD) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 18, 0);
				} else if (icono->tipo == NOTIF_MEMORYFREE) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 19, 0);
				} else if (icono->tipo == NOTIF_MEMORYUSED) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 20, 0);
				} else if (icono->tipo == NOTIF_CELLNETWORK) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 21, 0);
				} else if (icono->tipo == NOTIF_SIGNAL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 22, 0);
				} else if (icono->tipo == NOTIF_OPERATOR) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 23, 0);
				} else if (icono->tipo == NOTIF_SIGNAL_OPER) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 24, 0);
				} else if (icono->tipo == NOTIF_MC_SIG_OPER) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 25, 0);
				} else if (icono->tipo == NOTIF_PROFILE) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 26, 0);
				} else if (icono->tipo == NOTIF_VMAIL) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 27, 0);
				} else if (icono->tipo == NOTIF_IRDA) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 28, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 0, 0);
				}
			} else {
				SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_SETCURSEL, 0, 0);
			}
		}
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			// Comprobamos si quiere guardar o solo salir
			int resp = MessageBox(hDlg, TEXT("Save Changes?"), TEXT("Exit"), MB_YESNOCANCEL);
			if (resp == IDNO) {
				if (g_hWndMenuBar) {
					CommandBar_Destroy(g_hWndMenuBar);
				}
				EndDialog(hDlg, LOWORD(wParam));
				return FALSE;
			} else if (resp == IDCANCEL) {
				return FALSE;
			}


			// Recuperamos los valores introducidos
			int nScreen, nIcon, nType;
			TCHAR strName[MAX_PATH];
			TCHAR strImage[MAX_PATH];
			TCHAR strSound[MAX_PATH];
			TCHAR strType[MAX_PATH];
			TCHAR strExec[MAX_PATH];
			TCHAR strParameters[MAX_PATH];
			TCHAR strExecAlt[MAX_PATH];
			TCHAR strParametersAlt[MAX_PATH];
			UINT launchAnimation = 0;

			nScreen = GetDlgItemInt(hDlg, IDC_MICON_SCREEN, NULL, TRUE);
			nIcon = GetDlgItemInt(hDlg, IDC_MICON_ICON, NULL, TRUE);
			GetDlgItemText(hDlg, IDC_MICON_NAME, strName, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_IMAGE, strImage, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_SOUND, strSound, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_TYPE, strType, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_EXEC, strExec, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_PARAMETERS, strParameters, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_EXECALT, strExecAlt, MAX_PATH);
			GetDlgItemText(hDlg, IDC_MICON_PARAMETERSALT, strParametersAlt, MAX_PATH);
			launchAnimation = SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_GETCHECK, 0, 0) == BST_CHECKED;


			// Comprobaciones

			// Posicion
			BOOL newScreen = false;
			if (nScreen >= MAX_PANTALLAS && listaPantallas->numPantallas == MAX_PANTALLAS) {
				MessageBox(hDlg, TEXT("Screen Number not valid!"), TEXT("Error"), MB_OKCANCEL);
				return FALSE;
			} else if (nScreen >= int(listaPantallas->numPantallas)) {
				nScreen = listaPantallas->numPantallas;
				nIcon = 0;
				newScreen = true;
			} else {
				CPantalla *pantalla = NULL;
				if (nScreen == -1) {
					if (listaPantallas->barraInferior == NULL) {
						listaPantallas->barraInferior = new CPantalla();
					}
					pantalla = listaPantallas->barraInferior;
				} else if (nScreen == -2) {
					if (listaPantallas->topBar == NULL) {
						listaPantallas->topBar = new CPantalla();
					}
					pantalla = listaPantallas->topBar;
				} else {
					pantalla = listaPantallas->listaPantalla[nScreen];
				}
				if (nIcon >= MAX_ICONOS_PANTALLA && pantalla->numIconos == MAX_ICONOS_PANTALLA) {
					MessageBox(hDlg, TEXT("Icon Number not valid!"), TEXT("Error"), MB_OK);
					return FALSE;
				} else if (nIcon > int(pantalla->numIconos)) {
					nIcon = pantalla->numIconos;
				}
			}

			// Type Icon
			if (lstrcmpi(strType, NOTIF_NORMAL_TXT) == 0) {
				nType = NOTIF_NORMAL;
			} else if (lstrcmpi(strType, NOTIF_MISSEDCALLS_TXT) == 0) {
				nType = NOTIF_MISSEDCALLS;
			} else if (lstrcmpi(strType, NOTIF_SMS_TXT) == 0) {
				nType = NOTIF_SMS;
			} else if (lstrcmpi(strType, NOTIF_MMS_TXT) == 0) {
				nType = NOTIF_MMS;
			} else if (lstrcmpi(strType, NOTIF_OTHER_EMAIL_TXT) == 0) {
				nType = NOTIF_OTHER_EMAIL;
			} else if (lstrcmpi(strType, NOTIF_SYNC_EMAIL_TXT) == 0) {
				nType = NOTIF_SYNC_EMAIL;
			} else if (lstrcmpi(strType, NOTIF_TOTAL_EMAIL_TXT) == 0) {
				nType = NOTIF_TOTAL_EMAIL;
			} else if (lstrcmpi(strType, NOTIF_APPOINTS_TXT) == 0) {
				nType = NOTIF_APPOINTS;
			} else if (lstrcmpi(strType, NOTIF_CALENDAR_TXT) == 0) {
				nType = NOTIF_CALENDAR;
			} else if (lstrcmpi(strType, NOTIF_TASKS_TXT) == 0) {
				nType = NOTIF_TASKS;
			} else if (lstrcmpi(strType, NOTIF_SMS_MMS_TXT) == 0) {
				nType = NOTIF_SMS_MMS;
			} else if (lstrcmpi(strType, NOTIF_WIFI_TXT) == 0) {
				nType = NOTIF_WIFI;
			} else if (lstrcmpi(strType, NOTIF_BLUETOOTH_TXT) == 0) {
				nType = NOTIF_BLUETOOTH;
			} else if (lstrcmpi(strType, NOTIF_ALARM_TXT) == 0) {
				nType = NOTIF_ALARM;
			} else if (lstrcmpi(strType, NOTIF_CLOCK_TXT) == 0) {
				nType = NOTIF_CLOCK;
			} else if (lstrcmpi(strType, NOTIF_CLOCK_ALARM_TXT) == 0) {
				nType = NOTIF_CLOCK_ALARM;
			} else if (lstrcmpi(strType, NOTIF_BATTERY_TXT) == 0) {
				nType = NOTIF_BATTERY;
			} else if (lstrcmpi(strType, NOTIF_VOLUME_TXT) == 0) {
				nType = NOTIF_VOLUME;
			} else if (lstrcmpi(strType, NOTIF_MEMORYLOAD_TXT) == 0) {
				nType = NOTIF_MEMORYLOAD;
			} else if (lstrcmpi(strType, NOTIF_MEMORYFREE_TXT) == 0) {
				nType = NOTIF_MEMORYFREE;
			} else if (lstrcmpi(strType, NOTIF_MEMORYUSED_TXT) == 0) {
				nType = NOTIF_MEMORYUSED;
			} else if (lstrcmpi(strType, NOTIF_CELLNETWORK_TXT) == 0) {
				nType = NOTIF_CELLNETWORK;
			} else if (lstrcmpi(strType, NOTIF_SIGNAL_TXT) == 0) {
				nType = NOTIF_SIGNAL;
			} else if (lstrcmpi(strType, NOTIF_OPERATOR_TXT) == 0) {
				nType = NOTIF_OPERATOR;
			} else if (lstrcmpi(strType, NOTIF_SIGNAL_OPER_TXT) == 0) {
				nType = NOTIF_SIGNAL_OPER;
			} else if (lstrcmpi(strType, NOTIF_MC_SIG_OPER_TXT) == 0) {
				nType = NOTIF_MC_SIG_OPER;
			} else if (lstrcmpi(strType, NOTIF_PROFILE_TXT) == 0) {
				nType = NOTIF_PROFILE;
			} else if (lstrcmpi(strType, NOTIF_VMAIL_TXT) == 0) {
				nType = NOTIF_VMAIL;
			} else if (lstrcmpi(strType, NOTIF_IRDA_TXT) == 0) {
				nType = NOTIF_IRDA;
			} else {
				MessageBox(hDlg, TEXT("Type not valid!"), TEXT("Error"), MB_OK);
				return FALSE;
			}

			// All Ok, Icon Creation
			CPantalla *pantalla = NULL;
			CIcono *icono = NULL;

			if (newScreen) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				if (nScreen == -1) {
					if (listaPantallas->barraInferior == NULL) {
						listaPantallas->barraInferior = new CPantalla();
					}
					pantalla = listaPantallas->barraInferior;
				} else if (nScreen == -2) {
					if (listaPantallas->topBar == NULL) {
						listaPantallas->topBar = new CPantalla();
					}
					pantalla = listaPantallas->topBar;
				} else {
					pantalla = listaPantallas->listaPantalla[nScreen];
				}
			}
			if (iconoActual.nIconoActual >= 0) {
				icono = listaPantallas->mueveIcono(iconoActual.nPantallaActual, iconoActual.nIconoActual, nScreen, nIcon);
			} else {
				icono = pantalla->creaIcono(nIcon);
			}

			StringCchCopy(icono->nombre, CountOf(icono->nombre), strName);
			StringCchCopy(icono->rutaImagen, CountOf(icono->rutaImagen), strImage);
			StringCchCopy(icono->sound, CountOf(icono->sound), strSound);
			StringCchCopy(icono->ejecutable, CountOf(icono->ejecutable), strExec);
			StringCchCopy(icono->parametros, CountOf(icono->parametros), strParameters);
			StringCchCopy(icono->ejecutableAlt, CountOf(icono->ejecutableAlt), strExecAlt);
			StringCchCopy(icono->parametrosAlt, CountOf(icono->parametrosAlt), strParametersAlt);
			icono->tipo = nType;
			icono->launchAnimation = launchAnimation;
			SCREEN_TYPE st = MAINSCREEN;
			if (pantalla == listaPantallas->barraInferior) {
				st = BOTTOMBAR;
			} else if (pantalla == listaPantallas->topBar) {
				st = TOPBAR;
			}
			configuracion->loadIconImage(&hDCMem, icono, st);

			setPosiciones(true, 0, 0);

			configuracion->saveXMLIcons(listaPantallas);
			calculateConfiguration(0, 0);

			if (g_hWndMenuBar) {
				CommandBar_Destroy(g_hWndMenuBar);
			}

			EndDialog(hDlg, LOWORD(wParam));
			return FALSE;
		} else if (HIWORD(wParam) == EN_CHANGE) {

		} else if (LOWORD(wParam) == IDC_MICON_IMAGE_B) {
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_THUMBNAILVIEW, pathFile, lastPathImage)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathImage);

				SetDlgItemText(hDlg, IDC_MICON_IMAGE, pathFile);
			}
		} else if (LOWORD(wParam) == IDC_MICON_SOUND_B) {
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, pathFile, lastPathSound)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathSound);

				SetDlgItemText(hDlg, IDC_MICON_SOUND, pathFile);
			}
		} else if (LOWORD(wParam) == IDC_BUTTON_WAV_PLAY) {
			TCHAR pathFile[MAX_PATH];
			TCHAR fullPath[MAX_PATH];
			GetDlgItemText(hDlg, IDC_MICON_SOUND, pathFile, MAX_PATH);
			configuracion->getAbsolutePath(fullPath, CountOf(fullPath), pathFile);
			PlaySound(fullPath, 0, SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
		} else if (LOWORD(wParam) == IDC_MICON_EXEC_B) {
			if (lastPathExec[0] == 0) {
				if (!SHGetSpecialFolderPath(hDlg, lastPathExec, CSIDL_PROGRAMS, FALSE)) {
					StringCchCopy(lastPathExec, CountOf(lastPathExec), L"\\");
				}
			}
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, pathFile, lastPathExec)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathExec);

				// Extract Name from path
				int resp = MessageBox(hDlg, TEXT("Set Icon Name?"), TEXT("Exit"), MB_YESNO);
				if (resp == IDYES) {
					SHFILEINFO cbFileInfo;
					SHGetFileInfo(pathFile, 0, &cbFileInfo, sizeof(cbFileInfo), SHGFI_DISPLAYNAME);
					SetDlgItemText(hDlg, IDC_MICON_NAME, cbFileInfo.szDisplayName);
				}
				SetDlgItemText(hDlg, IDC_MICON_EXEC, pathFile);
			}
		} else if (LOWORD(wParam) == IDC_MICON_EXECALT_B) {
			if (lastPathExec[0] == 0) {
				if (!SHGetSpecialFolderPath(hDlg, lastPathExec, CSIDL_PROGRAMS, FALSE)) {
					StringCchCopy(lastPathExec, CountOf(lastPathExec), L"\\");
				}
			}
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, pathFile, lastPathExec)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathExec);

				SetDlgItemText(hDlg, IDC_MICON_EXECALT, pathFile);
			}
		}
		break;
	case WM_HELP:
		ToggleKeyboard();
		break;
	case WM_DESTROY:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK editHeaderDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			if (FindWindow(L"MS_SIPBUTTON", NULL) != NULL) {
				SHMENUBARINFO mbi;

				memset(&mbi, 0, sizeof(SHMENUBARINFO));  // Reset mbi to 0.
				mbi.cbSize = sizeof(SHMENUBARINFO);
				mbi.dwFlags = SHCMBF_EMPTYBAR;
				mbi.hwndParent = hDlg;  // Soft key bar's owner.
				mbi.nToolBarId = NULL;  // Soft key bar resource.
				mbi.hInstRes = NULL;  // HINST in which resource is located.

				g_hWndMenuBar = mbi.hwndMB;

				if (g_hWndMenuBar) {
					CommandBar_Destroy(g_hWndMenuBar);
				}
				// Create the Soft key bar.
				if (!SHCreateMenuBar(&mbi))
				{
					g_hWndMenuBar = NULL;
				}
				else
				{
					g_hWndMenuBar = mbi.hwndMB;
				}
			}

			SHINITDLGINFO shidi;
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			if (FindWindow(L"MS_SIPBUTTON", NULL) == NULL) {
				SetWindowLong(hDlg, GWL_EXSTYLE, GetWindowLong(hDlg, GWL_EXSTYLE) | WS_EX_CONTEXTHELP | WS_EX_CAPTIONOKBTN);
			}

			SetDlgItemText(hDlg, IDC_EDIT_HEADER, listaPantallas->listaPantalla[estado->pantallaActiva]->header);
		}
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			int resp = MessageBox(hDlg, TEXT("Save Changes?"), TEXT("Exit"), MB_YESNOCANCEL);
			if (resp == IDNO) {
				if (g_hWndMenuBar) {
					CommandBar_Destroy(g_hWndMenuBar);
				}
				EndDialog(hDlg, LOWORD(wParam));
				return FALSE;
			} else if (resp == IDCANCEL) {
				return FALSE;
			}

			CPantalla *pantalla = listaPantallas->listaPantalla[estado->pantallaActiva];

			TCHAR header[MAX_PATH];
			GetDlgItemText(hDlg, IDC_EDIT_HEADER, header, MAX_PATH);
			if (wcscmp(pantalla->header, header) != 0) {
				StringCchCopy(pantalla->header, CountOf(pantalla->header), header);
				configuracion->saveXMLIcons(listaPantallas);
				pantalla->debeActualizar = TRUE;
			}

			if (g_hWndMenuBar) {
				CommandBar_Destroy(g_hWndMenuBar);
			}

			EndDialog(hDlg, LOWORD(wParam));
			return FALSE;
		}
		break;
	case WM_HELP:
		ToggleKeyboard();
		break;
	case WM_DESTROY:
		break;
	}
	return FALSE;
}

#ifndef EXEC_MODE
BOOL doEraseBackground(HWND hwnd)
{
	if (!configuracion) {
		return FALSE;
	}

#ifdef EXEC_MODE
	BOOL isTransparent = FALSE;
#else
	BOOL isTransparent = configuracion->fondoTransparente;
#endif
	if (!isTransparent) {
		return FALSE;
	}

	RECT rc;
	// GetClientRect(hwnd, &rc);
	rc.left = 0;
	rc.top = 0;
	rc.right = configuracion->anchoPantalla;
	rc.bottom = configuracion->altoPantalla;

	TODAYDRAWWATERMARKINFO	dwi;
	dwi.hdc = configuracion->fondoPantalla->hDC;
	dwi.hwnd = hwnd;
	dwi.rc = rc;
	SendMessage(GetParent(hwnd), TODAYM_DRAWWATERMARK, 0, (LPARAM)&dwi);

	return TRUE;
}
#endif

BOOL inicializaApp(HWND hwnd)
{
	configuracion = new CConfiguracion();
	estado = new CEstado();
	notifications = new CNotifications(hwnd);

	// Debe ser inicializado en modo normal
	estado->estadoCuadro = 0;

	// Cargamos los parametros de configuracion
	// long duration = -(long)GetTickCount();
	configuracion->loadXMLConfig();
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to loadXMLConfig.\n", duration);

#ifdef EXEC_MODE
	if (configuracion->fullscreen) {
		SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
	} else {
		RECT rc;
		if (SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0)) {
			SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}
	}
	if (configuracion->noWindowTitle) {
		SetWindowText(hwnd, L"");
	} else {
		SetWindowText(hwnd, g_szTitle);
	}
#endif
	int windowWidth;
	int windowHeight;
	getWindowSize(hwnd, &windowWidth, &windowHeight);

	resizeWindow(hwnd, true);

	estado->posObjetivo.x = 0;
	estado->posObjetivo.y = 0;
	posImage = estado->posObjetivo;

	estado->estadoCuadro = 0;
	estado->cuadroLanzando.left = configuracion->anchoPantalla / 2;
	estado->cuadroLanzando.right = estado->cuadroLanzando.left;
	estado->cuadroLanzando.top = configuracion->altoPantalla / 2;
	estado->cuadroLanzando.bottom = estado->cuadroLanzando.top;

	// Cargamos la configuracion de iconos
	listaPantallas = new CListaPantalla();
	configuracion->loadXMLIcons2(listaPantallas);

	calculateConfiguration(windowWidth, windowHeight);

	HDC hdc = GetDC(hwnd);

	// duration = -(long)GetTickCount();
	configuracion->loadIconsImages(&hdc, listaPantallas);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to loadIconsImages.\n", duration);

	// duration = -(long)GetTickCount();
	configuracion->loadImages(&hdc);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to loadImages.\n", duration);

	ReleaseDC(hwnd, hdc);

	TCHAR szFontsPath[MAX_PATH];
	configuracion->getAbsolutePath(szFontsPath, CountOf(szFontsPath), L"fonts");
	AddRemoveFonts(szFontsPath, TRUE);

	// Establecemos la ruta por defecto para buscar programas
	//if (!SHGetSpecialFolderPath(hwnd, lastPathExec, CSIDL_PROGRAMS, FALSE)) {
	//	StringCchCopy(lastPathExec, CountOf(lastPathExec), L"\\");
	//}

	StringCchCopy(lastPathImage, CountOf(lastPathImage), configuracion->pathIconsDir);
	StringCchCopy(lastPathSound, CountOf(lastPathSound), configuracion->pathExecutableDir);

	setPosiciones(true, 0, 0);

#ifdef EXEC_MODE
	if (configuracion->notifyTimer > 0) {
		SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, configuracion->notifyTimer, NULL);
	}
#else
	if (configuracion->updateWhenInactive && configuracion->notifyTimer > 0) {
		SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, configuracion->notifyTimer, NULL);
	}
#endif

	return TRUE;
}

BOOL borraObjetosHDC()
{
	borraHDC_HBITMPAP(&hDCMem, &hbmMem, &hbmMemOld);
	borraHDC_HBITMPAP(&hDCMem2, &hbmMem2, &hbmMemOld2);

	if (listaPantallas != NULL) {
		if (listaPantallas->barraInferior != NULL) {
			borraHDC_HBITMPAP(
				&listaPantallas->barraInferior->hDC,
				&listaPantallas->barraInferior->imagen,
				&listaPantallas->barraInferior->imagenOld);
			borraHDC_HBITMPAP(
				&listaPantallas->barraInferior->mask_hDC,
				&listaPantallas->barraInferior->mask_imagen,
				&listaPantallas->barraInferior->mask_imagenOld);
			listaPantallas->barraInferior->debeActualizar = TRUE;
		}

		if (listaPantallas->topBar != NULL) {
			borraHDC_HBITMPAP(
				&listaPantallas->topBar->hDC,
				&listaPantallas->topBar->imagen,
				&listaPantallas->topBar->imagenOld);
			borraHDC_HBITMPAP(
				&listaPantallas->topBar->mask_hDC,
				&listaPantallas->topBar->mask_imagen,
				&listaPantallas->topBar->mask_imagenOld);
			listaPantallas->topBar->debeActualizar = TRUE;
		}

		for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
			borraHDC_HBITMPAP(
				&listaPantallas->listaPantalla[i]->hDC,
				&listaPantallas->listaPantalla[i]->imagen,
				&listaPantallas->listaPantalla[i]->imagenOld);
			borraHDC_HBITMPAP(
				&listaPantallas->listaPantalla[i]->mask_hDC,
				&listaPantallas->listaPantalla[i]->mask_imagen,
				&listaPantallas->listaPantalla[i]->mask_imagenOld);
			listaPantallas->listaPantalla[i]->debeActualizar = TRUE;
		}
	}

	if(hBrushFondo) {
		DeleteObject(hBrushFondo);
		hBrushFondo = NULL;
	}

	if(hBrushTrans) {
		DeleteObject(hBrushTrans);
		hBrushTrans = NULL;
	}

	if(hBrushAnimation) {
		DeleteObject(hBrushAnimation);
		hBrushAnimation = NULL;
	}

	return true;
}

BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld)
{
	if(*hbm && *hdc) {
		SelectObject(*hdc, *hbmOld);
		DeleteDC(*hdc);
		DeleteObject(*hbm);
		*hdc = NULL;
		*hbm = NULL;
	}

	return true;
}

#ifdef EXEC_MODE
int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	if (_tcsclen(lpCmdLine) > 0) {
		TCHAR szWindowClass[MAX_LOADSTRING];
		LoadString(hInstance, IDS_APPNAME, szWindowClass, MAX_LOADSTRING);
		g_hWnd = FindWindow(szWindowClass, NULL);
		if (g_hWnd) {
			if (wcsncmp(lpCmdLine, L"--", 2) == 0) {
				CommandLineArguements(g_hWnd, lpCmdLine + 2);
			}
		}
		return 0;
	}

	MSG msg;

	// long duration = -(long)GetTickCount();

	// initialize imaging API only once
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED(CoCreateInstance (CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) & g_pImgFactory)))
	{
		g_pImgFactory = NULL;
	}
	if (g_pImgFactory == NULL) {
		g_hImgdecmpDll = LoadLibrary(L"imgdecmp.dll");
	}

	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to initialize the imaging API.\n", duration);
	// duration = -(long)GetTickCount();

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to InitInstance.\n", duration);

	//HACCEL hAccelTable;
	//hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}

	// duration = -(long)GetTickCount();
	doDestroy(g_hWnd);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to destroy.\n", duration);

	// duration = -(long)GetTickCount();
	if( g_pImgFactory != NULL ) {g_pImgFactory->Release(); g_pImgFactory = NULL;}
	CoUninitialize();
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to uninitialize the imaging API.\n", duration);
	if (g_hImgdecmpDll != NULL) {
		FreeLibrary(g_hImgdecmpDll);
	}

	return (int) msg.wParam;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// HWND hWnd;
	// TCHAR szTitle[MAX_LOADSTRING];		// title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name

	g_hInst = hInstance; // Store instance handle in our global variable

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	// SHInitExtraControls should be called once during your application's initialization to initialize any
	// of the device specific controls such as CAPEDIT and SIPPREF.
	SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

	LoadString(hInstance, IDS_APPNAME, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APPNAME, szWindowClass, MAX_LOADSTRING);

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	//If it is already running, then focus on the window, and exit
	g_hWnd = FindWindow(szWindowClass, NULL);
	if (g_hWnd)
	{
		// set focus to foremost child window
		// The "| 0x00000001" is used to bring any owned windows to the foreground and
		// activate them.
		SetForegroundWindow((HWND)((ULONG) g_hWnd | 0x00000001));
		PostMessage(g_hWnd, WM_USER_RELAUNCH, 0, 0);
		return 0;
	}
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

	if (!MyRegisterClass(hInstance, szWindowClass))
	{
		return FALSE;
	}

	// Create main window.
	g_hWnd = CreateWindowEx (WS_EX_NODRAG,      // Ex Style
		szWindowClass,       // Window class
		g_szTitle,           // Window title
		WS_SYSMENU,          // Style flags
		CW_USEDEFAULT,       // x position
		CW_USEDEFAULT,       // y position
		CW_USEDEFAULT,       // Initial width
		CW_USEDEFAULT,       // Initial height
		NULL,                // Parent
		NULL,                // Menu, must be null
		hInstance,           // Application instance
		NULL);               // Pointer to create
	// parameters
	// Return fail code if window not created.
	if (!IsWindow (g_hWnd)) return 0;

	SetForegroundWindow(g_hWnd);

	resizeWindow(g_hWnd, true);

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	// wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IPHONELAUNCHER));
	wc.hIcon         = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}
#endif

void doDestroy(HWND hwnd) {
	if (hwnd != NULL) {
		resizeWindow(hwnd, false);
		KillTimer(hwnd, TIMER_RECUPERACION);
		KillTimer(hwnd, TIMER_ACTUALIZA_NOTIF);
		KillTimer(hwnd, TIMER_LANZANDO_APP);
		KillTimer(hwnd, TIMER_LONGTAP);
		g_hWnd = NULL;
	}

	borraObjetosHDC();

	if (listaPantallas != NULL) {
		delete listaPantallas;
		listaPantallas = NULL;
	}
	if (estado != NULL) {
		delete estado;
		estado = NULL;
	}
	if (notifications != NULL) {
		delete notifications;
		notifications = NULL;
	}
	if (configuracion != NULL) {
		TCHAR szFontsPath[MAX_PATH];
		configuracion->getAbsolutePath(szFontsPath, CountOf(szFontsPath), L"fonts");
		AddRemoveFonts(szFontsPath, FALSE);

		delete configuracion;
		configuracion = NULL;
	}
}

void resizeWindow(HWND hwnd, BOOL fullScreen)
{
	static HWND hWndSip = FindWindow(L"MS_SIPBUTTON", NULL);

#ifdef EXEC_MODE
	static HWND hWndTaskbar = FindWindow(L"HHTaskBar", NULL);
	DWORD dwState;
	if (fullScreen) {
		dwState = SHFS_HIDESIPBUTTON;
		if (configuracion != NULL && configuracion->fullscreen) {
			dwState |= SHFS_HIDETASKBAR;
		}
	} else {
		dwState = SHFS_HIDESIPBUTTON;
		if (configuracion != NULL && !configuracion->neverShowTaskBar) {
			dwState |= SHFS_SHOWTASKBAR;
		}
	}
	SHFullScreen(hwnd, dwState);

	if (configuracion != NULL && hWndTaskbar != NULL) {
		if (configuracion->fullscreen) {
			ShowWindow(hWndTaskbar, fullScreen || configuracion->neverShowTaskBar ? SW_HIDE : SW_SHOW);
		} else {
			ShowWindow(hWndTaskbar, configuracion->neverShowTaskBar ? SW_HIDE : SW_SHOW);
		}
	}
#endif

	if (fullScreen) {
		// Hide SIP keyboard
		SipShowIM(SIPF_OFF);
		// Hide SIP button
		if (hWndSip != NULL) {
			ShowWindow(hWndSip, SW_HIDE);
		}

		ShowWindow(hwnd, SW_SHOWNORMAL);
	}
}

void calculateConfiguration(int width, int height)
{
	// NKDbgPrintfW(L"calculateConfiguration(%d, %d)\n", width, height);
	// Cargamos la configuracion calculada en funcion de los iconos
	int maxIconos = 0;
	for (int i = 0; i < (int)listaPantallas->numPantallas; i++) {
		maxIconos = max(maxIconos, (int)listaPantallas->listaPantalla[i]->numIconos);
	}
	UINT altoPantallaMax_old = configuracion->altoPantallaMax;
	configuracion->calculaConfiguracion(
		maxIconos,
		listaPantallas->barraInferior == NULL ? 0 : listaPantallas->barraInferior->numIconos,
		listaPantallas->topBar == NULL ? 0 : listaPantallas->topBar->numIconos,
		width,
		height);
	if (altoPantallaMax_old != 0 && configuracion->altoPantallaMax > altoPantallaMax_old) {
		borraObjetosHDC();
		setPosiciones(false, 0, 0);
	}
}

void getWindowSize(HWND hwnd, int *windowWidth, int *windowHeight)
{
#ifdef EXEC_MODE
	RECT rc;
	GetClientRect(hwnd, &rc);
	*windowWidth = rc.right - rc.left;
	*windowHeight = rc.bottom - rc.top;
#else
	RECT rw;
	GetWindowRect(hwnd, &rw);
	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	int h = 0;
	if (cyScreen > cxScreen) {
		h = configuracion->heightP;
	} else {
		h = configuracion->heightL;
	}
	if (h == 0) {
		RECT rwp;
		if (GetWindowRect(GetParent(hwnd), &rwp) && rwp.bottom > rw.top && rwp.bottom <= cyScreen) {
			*windowHeight = rwp.bottom - rw.top;
		} else {
			*windowHeight = cyScreen - rw.top;
		}
		*windowHeight = *windowHeight - GetSystemMetrics(SM_CYBORDER);
	} else {
		*windowHeight = h;
	}
	*windowWidth = cxScreen;

#endif
#ifdef DEBUG1
	NKDbgPrintfW(L"getWindowSize(%d, %d)\n", *windowWidth, *windowHeight);
#endif
}

void ResetPressed()
{
	if (pressed) {
		pressed = FALSE;
		RECT rcWindBounds;
		GetClientRect(g_hWnd, &rcWindBounds);
		InvalidateRect(g_hWnd, &rcWindBounds, FALSE);
		UpdateWindow(g_hWnd);
	}
}


BOOL InvalidateListScreenIfNotificationsChanged(CListaPantalla *listaPantallas)
{
	BOOL changed = FALSE;

	if (listaPantallas == NULL) {
		return FALSE;
	}

	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		InvalidateScreenIfNotificationsChanged(listaPantallas->listaPantalla[i]);
		if (listaPantallas->listaPantalla[i]->debeActualizar) {
			changed = TRUE;
		}
	}

	InvalidateScreenIfNotificationsChanged(listaPantallas->barraInferior);
	if (listaPantallas->barraInferior->debeActualizar) {
		changed = TRUE;
	}
	InvalidateScreenIfNotificationsChanged(listaPantallas->topBar);
	if (listaPantallas->topBar->debeActualizar) {
		changed = TRUE;
	}
	return changed;
}

// "Invalidate" screen (mark that has to be updated) if it contains special icons that their values have changed.
void InvalidateScreenIfNotificationsChanged(CPantalla *pantalla)
{
	if (pantalla == NULL) {
		return;
	}

	for (UINT j = 0; j < pantalla->numIconos; j++) {
		CIcono *icono = pantalla->listaIconos[j];
		switch(icono->tipo) {
			case NOTIF_NORMAL:
				break;
			case NOTIF_MISSEDCALLS:
				if (notifications->dwNotificationsChanged[SN_PHONEMISSEDCALLS]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_VMAIL:
				if (notifications->szNotificationsChanged[SN_MESSAGINGVOICEMAILTOTALUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_SMS:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGSMSUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_MMS:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGMMSUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_SMS_MMS:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGSMSUNREAD] || notifications->dwNotificationsChanged[SN_MESSAGINGMMSUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_OTHER_EMAIL:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGOTHEREMAILUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_SYNC_EMAIL:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGACTIVESYNCEMAILUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_TOTAL_EMAIL:
				if (notifications->dwNotificationsChanged[SN_MESSAGINGOTHEREMAILUNREAD] || notifications->dwNotificationsChanged[SN_MESSAGINGACTIVESYNCEMAILUNREAD]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_APPOINTS:
				if (notifications->dwNotificationsChanged[SN_APPOINTMENTSLISTCOUINT]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_TASKS:
				if (notifications->dwNotificationsChanged[SN_TASKSACTIVE]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_WIFI:
				if (notifications->dwNotificationsChanged[SN_WIFISTATEPOWERON]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_BLUETOOTH:
				if (notifications->dwNotificationsChanged[SN_BLUETOOTHSTATEPOWERON]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_IRDA:
				if (notifications->szNotificationsChanged[SN_IRDA]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_CELLNETWORK:
				if (notifications->dwNotificationsChanged[SN_CELLSYSTEMCONNECTED]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_ALARM:
				if (notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS0] || notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS1] || notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS2]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_CALENDAR:
				if (notifications->ftNotificationsChanged[SN_DATE]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_CLOCK:
				if (notifications->ftNotificationsChanged[SN_TIME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_CLOCK_ALARM:
				if (notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS0] || notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS1] || notifications->dwNotificationsChanged[SN_CLOCKALARMFLAGS2]
				|| notifications->ftNotificationsChanged[SN_TIME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_BATTERY:
				if (notifications->dwNotificationsChanged[SN_POWERBATTERYSTATE]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_VOLUME:
				if (notifications->dwNotificationsChanged[SN_VOLUME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_MEMORYLOAD:
			case NOTIF_MEMORYFREE:
			case NOTIF_MEMORYUSED:
				if (notifications->memory_changed) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_SIGNAL:
				if (notifications->dwNotificationsChanged[SN_PHONESIGNALSTRENGTH]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_OPERATOR:
				if (notifications->szNotificationsChanged[SN_PHONEOPERATORNAME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_SIGNAL_OPER:
				if (notifications->dwNotificationsChanged[SN_PHONESIGNALSTRENGTH] || notifications->szNotificationsChanged[SN_PHONEOPERATORNAME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_MC_SIG_OPER:
				if (notifications->dwNotificationsChanged[NOTIF_MISSEDCALLS] || notifications->dwNotificationsChanged[SN_PHONESIGNALSTRENGTH] || notifications->szNotificationsChanged[SN_PHONEOPERATORNAME]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
			case NOTIF_PROFILE:
				if (notifications->szNotificationsChanged[SN_PHONEPROFILE]) {
					pantalla->debeActualizar = TRUE;
					return;
				}
				break;
		}
	}
}

// Process notifications. Return true if screens needs to be redrawn.
BOOL ProcessNotifications()
{
	BOOL shouldInvalidateRect = FALSE;

	if (notifications->dwNotificationsChanged[SN_RELOADICON] && notifications->dwNotifications[SN_RELOADICON]) {
		CReloadIcon *reloadIcon = new CReloadIcon();
		CPantalla *pantalla = NULL;
		CIcono *icono = NULL;
		for (int nIcon = 0; reloadIcon->LoadRegistryIcon(nIcon); nIcon++) {
			if (reloadIcon->nScreen >= 0 && reloadIcon->nScreen < listaPantallas->numPantallas) {
				pantalla = listaPantallas->listaPantalla[reloadIcon->nScreen];
			} else if (reloadIcon->nScreen == -1) {
				pantalla = listaPantallas->barraInferior;
			} else if (reloadIcon->nScreen == -2) {
				pantalla = listaPantallas->topBar;
			} else {
				continue;
			}
			if (pantalla != NULL && reloadIcon->nIcon >= 0 && reloadIcon->nIcon < pantalla->numIconos) {
				icono = pantalla->listaIconos[reloadIcon->nIcon];
			} else {
				continue;
			}
			if (icono != NULL) {
				if (_tcslen(reloadIcon->strName) > 0) {
					StringCchCopy(icono->nombre, CountOf(icono->nombre), reloadIcon->strName);
				}
				if (_tcslen(reloadIcon->strImage) > 0) {
					StringCchCopy(icono->rutaImagen, CountOf(icono->rutaImagen), reloadIcon->strImage);
				}
				if (_tcslen(reloadIcon->strExec) > 0) {
					StringCchCopy(icono->nombre, CountOf(icono->ejecutable), reloadIcon->strExec);
					StringCchCopy(icono->nombre, CountOf(icono->parametros), reloadIcon->strParameters);
				}
				SCREEN_TYPE st = MAINSCREEN;
				if (pantalla == listaPantallas->barraInferior) {
					st = BOTTOMBAR;
				} else if (pantalla == listaPantallas->topBar) {
					st = TOPBAR;
				}
				configuracion->loadIconImage(&hDCMem, icono, st);
				pantalla->debeActualizar = TRUE;
				shouldInvalidateRect = TRUE;
			}
		}

		if (notifications->dwNotifications[SN_RELOADICON] == 2) {
			configuracion->saveXMLIcons(listaPantallas);
		}

		reloadIcon->DeleteRegistryIcons();
		delete reloadIcon;

		DWORD value = 0;
		SaveDwordSetting(HKEY_LOCAL_MACHINE, TEXT("Software\\iPhoneToday"),
			&value, TEXT("reloadIcon"));

		notifications->dwNotifications[SN_RELOADICON] = 0;
	}

	if (notifications->dwNotificationsChanged[SN_RELOADICONS] && notifications->dwNotifications[SN_RELOADICONS]) {
		// Cargamos la configuracion de iconos
		configuracion->loadXMLIcons2(listaPantallas);
		configuracion->loadIconsImages(&hDCMem, listaPantallas);

		// Marcamos aquellas pantallas que haya que actualizar
		CPantalla *pantalla;
		for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
			pantalla = listaPantallas->listaPantalla[i];
			if (pantalla != NULL) {
				pantalla->debeActualizar = TRUE;
			}
		}

		pantalla = listaPantallas->barraInferior;
		if (pantalla != NULL) {
			pantalla->debeActualizar = TRUE;
		}

		pantalla = listaPantallas->topBar;
		if (pantalla != NULL) {
			pantalla->debeActualizar = TRUE;
		}

		// Comprobamos si hay que actualizar los iconos
		DWORD value = 0;
		SaveDwordSetting(HKEY_LOCAL_MACHINE, TEXT("Software\\iPhoneToday"),
			&value, TEXT("reloadIcons"));

		notifications->dwNotifications[SN_RELOADICONS] = 0;
		shouldInvalidateRect = TRUE;
	}

	if (InvalidateListScreenIfNotificationsChanged(listaPantallas)) {
		shouldInvalidateRect = TRUE;
	}

	return shouldInvalidateRect;
}

BOOL hasTopBar()
{
	return listaPantallas->topBar != NULL && listaPantallas->topBar->numIconos > 0 && configuracion->topBarConfig->iconWidth > 0;
}

BOOL hasBottomBar()
{
	return listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0;
}
