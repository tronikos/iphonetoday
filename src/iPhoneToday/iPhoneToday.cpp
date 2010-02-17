// iPhoneToday.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <initguid.h>
#include "iPhoneToday.h"
#include "CListaPantalla.h"
#include "CEstado.h"
#include "vibrate.h"
#include "OptionDialog.h"  // CreatePropertySheet includes

#define MAX_LOADSTRING 100

#define WM_USER_RELAUNCH (WM_USER + 0)
#define WM_USER_OPTIONS  (WM_USER + 1)
#define WM_USER_ADD      (WM_USER + 2)
#define WM_USER_GOTO     (WM_USER + 3)


// Global Variables:
HINSTANCE           g_hInst;
HWND				g_hWndMenuBar = NULL; // menu bar handle
HWND                g_hWnd;
TCHAR               g_szTitle[MAX_LOADSTRING];
IImagingFactory*    g_pImgFactory = NULL;
#ifdef EXEC_MODE
BOOL				g_bLoading = FALSE;
BOOL				g_bInitializing = FALSE;
#endif

// Variables
CListaPantalla *listaPantallas = NULL;
CConfiguracion *configuracion = NULL;
CEstado *estado = NULL;

BOOL posCursorInitialized = FALSE;
POINTS posCursor;
POINTS posImage = {0, 2};
HDC		hDCMem = NULL;
HBITMAP	hbmMem = NULL;
HBITMAP	hbmMemOld = NULL;
HBRUSH  hBrushFondo = NULL;
HBRUSH  hBrushWhite = NULL;

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
TCHAR lastPathExec[MAX_PATH];

// Icono a editar
IDENT_ICONO iconoActual;

// Forward declarations of functions included in this code module:
#ifdef EXEC_MODE
ATOM MyRegisterClass(HINSTANCE, LPTSTR);
BOOL InitInstance(HINSTANCE, int);
LRESULT WndProcLoading (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
#else
INT InitializeClasses();
BOOL cargaFondoPantalla(HWND hwnd);
#endif

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT doTimer (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doSize (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doPaint (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseDown (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseUp (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
BOOL LaunchApplication(LPCTSTR pCmdLine, LPCTSTR pParameters);
void pintaIconos(HDC *hDC, RECT *rcWindBounds);
void pintaIcono(HDC *hDC, CIcono *icono, BOOL esBarraInferior);
void pintaPantalla(HDC *hDC, CPantalla *pantalla, BOOL esBarraInferior = FALSE);
void setPosicionesIconos(CPantalla *pantalla, BOOL esBarraInferior);
void setPosiciones(BOOL inicializa, int offsetX, int offsetY);
void calculaPosicionObjetivo();
void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar = FALSE);
int hayNotificacion(int tipo);
BOOL inicializaApp(HWND hwnd);
BOOL borraObjetosHDC();
BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld);
void drawNotification(HDC hDC, RECT *rect, CIcono *imagen, TCHAR *texto);
LRESULT CALLBACK editaIconoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void doDestroy(HWND hwnd);
void resizeWindow(HWND hwnd, BOOL fullScreen);
void autoConfigure();
LRESULT WINAPI CustomItemOptionsDlgProc(HWND, UINT, WPARAM, LPARAM);
void RightClick(HWND hwnd, POINTS posCursor);
void calculateConfiguration(int width, int height);
void getWindowSize(HWND hwnd, int *windowWidth, int *windowHeight);

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

		if( g_pImgFactory != NULL ) g_pImgFactory->Release(); g_pImgFactory = NULL;
		CoUninitialize();

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
			// get the pointer to the item from the Today screen
			TODAYLISTITEM *ptliItem = (TODAYLISTITEM*)wParam;

			if ((NULL == ptliItem) || (WaitForSingleObject(SHELL_API_READY_EVENT, 0) == WAIT_TIMEOUT)) {
				return FALSE;
			}
			if (configuracion == NULL) {
				return FALSE;
			}
			if (0 == ptliItem->cyp || configuracion->altoPantalla != ptliItem->cyp) {
				ptliItem->cyp = configuracion->altoPantalla;
				return TRUE;
			}
			return FALSE;
		}
		break;
#endif

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

			RECT            rcWindBounds;
			GetClientRect( hwnd, &rcWindBounds);
			InvalidateRect(hwnd, &rcWindBounds, FALSE);
		}
		return 0;

	case WM_LBUTTONDOWN:
		return doMouseDown (hwnd, uimessage, wParam, lParam);
	case WM_LBUTTONUP:
		return doMouseUp (hwnd, uimessage, wParam, lParam);
	case WM_MOUSEMOVE:
		return doMove (hwnd, uimessage, wParam, lParam);
	case WM_ACTIVATE:
		return doActivate (hwnd, uimessage, wParam, lParam);
	case WM_TIMER:
		return doTimer (hwnd, uimessage, wParam, lParam);
	case WM_PAINT:
		return doPaint (hwnd, uimessage, wParam, lParam);
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
		return cargaFondoPantalla(hwnd);
#endif
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam) ;
}

LRESULT doTimer (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
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
					if (hayNotificacion(estado->iconoActivo->tipo) > 0 && _tcsclen(estado->iconoActivo->ejecutableAlt) > 0) {
						LaunchApplication(estado->iconoActivo->ejecutableAlt, estado->iconoActivo->parametrosAlt);
					} else if (_tcsclen(estado->iconoActivo->ejecutable) > 0) {
						LaunchApplication(estado->iconoActivo->ejecutable, estado->iconoActivo->parametros);
					}
				}
			}

			RECT            rcWindBounds2;
			GetClientRect( hwnd, &rcWindBounds2);
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
	} else if (wParam == TIMER_ACTUALIZA_NOTIF) {
		if (!estado->hayMovimiento) {
			// Actualizamos las notificaciones
			BOOL hayCambios = estado->actualizaNotificaciones();
			BOOL hayCambiosIconos = estado->checkReloadIcons();
			BOOL hayCambiosIcono = estado->checkReloadIcon();

			if (hayCambiosIcono) {
				CReloadIcon *reloadIcon = new CReloadIcon();
				int nIcon = 0;
				CPantalla *pantalla = NULL;
				CIcono *icono = NULL;
				while (estado->LoadRegistryIcon(nIcon, reloadIcon)) {
					if (reloadIcon->nScreen >= 0 &&
						reloadIcon->nScreen < listaPantallas->numPantallas) {

						pantalla = listaPantallas->listaPantalla[reloadIcon->nScreen];
					} else if (reloadIcon->nScreen == -1) {
						pantalla = listaPantallas->barraInferior;
					}
					if (pantalla != NULL && reloadIcon->nIcon >= 0 && reloadIcon->nIcon < pantalla->numIconos) {
						icono = pantalla->listaIconos[reloadIcon->nIcon];
					}

					if (icono != NULL) {
						if (_tcslen(reloadIcon->strName) > 0) {
							wcscpy(icono->nombre, reloadIcon->strName);
						}
						if (_tcslen(reloadIcon->strImage) > 0) {
							wcscpy(icono->rutaImagen, reloadIcon->strImage);
						}

						configuracion->cargaImagenIcono(&hDCMem, icono, (pantalla == listaPantallas->barraInferior));
						pantalla->debeActualizar = TRUE;
					}

					pantalla = NULL;
					icono = NULL;
					nIcon++;
				}

				delete reloadIcon;
				estado->clearReloadIcon();
			} else if (hayCambiosIconos) {
				// Cargamos la configuracion de iconos
				configuracion->cargaIconos(&hDCMem, listaPantallas);

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

				estado->clearReloadIcons();
			} else if (hayCambios) {
				// Marcamos aquellas pantallas que haya que actualizar
				CPantalla *pantalla;
				CIcono *icono;
				for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
					pantalla = listaPantallas->listaPantalla[i];
					if (pantalla != NULL) {
						for (UINT j = 0; j < pantalla->numIconos; j++) {
							icono = pantalla->listaIconos[j];
							if (icono->tipo != NOTIF_NORMAL) {
								pantalla->debeActualizar = TRUE;
								continue;
							}
						}
					}
				}

				pantalla = listaPantallas->barraInferior;
				if (pantalla != NULL) {
					for (UINT j = 0; j < pantalla->numIconos; j++) {
						icono = pantalla->listaIconos[j];
						if (icono->tipo != NOTIF_NORMAL) {
							pantalla->debeActualizar = TRUE;
							continue;
						}
					}
				}
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
	}

	RECT            rcWindBounds2;
	GetClientRect( hwnd, &rcWindBounds2);
	InvalidateRect(hwnd, &rcWindBounds2, FALSE);
	UpdateWindow(hwnd);

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
		if (configuracion != NULL) {
			int windowWidth;
			int windowHeight;
			getWindowSize(hwnd, &windowWidth, &windowHeight);
			if (windowWidth != configuracion->anchoPantalla || windowHeight != configuracion->altoPantalla) {
				calculateConfiguration(windowWidth, windowHeight);
				configuracion->cargaFondo(&hDCMem);
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

		GetClientRect( hwnd, &rcWindBounds);

		SetTextColor(hDC, RGB(0,0,0));
		TCHAR elementText[MAX_PATH];
		wcscpy(elementText, L"Loading...");
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
	PAINTSTRUCT     ps;
	RECT            rcWindBounds;
	HDC             hDC;

	GetClientRect(hwnd, &rcWindBounds);

	hDC = BeginPaint(hwnd, &ps);
	if (hDCMem == NULL) {
		hDCMem = CreateCompatibleDC(hDC);
		hbmMem = CreateCompatibleBitmap(hDC, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top);
		hbmMemOld = (HBITMAP)SelectObject(hDCMem, hbmMem);

		hBrushFondo = CreateSolidBrush(estado->colorFondo);
		hBrushWhite = CreateSolidBrush(RGB(255,255,255));
	}

	// draw the icon on the screen
	SetBkMode(hDCMem, TRANSPARENT);

	BOOL isTransparent = configuracion->fondoTransparente;
#ifdef EXEC_MODE
	isTransparent = FALSE;
#endif
	if ((isTransparent || configuracion->fondoEstatico) && configuracion->fondoPantalla != NULL && configuracion->fondoPantalla->hDC != NULL) {
		BitBlt(hDCMem, 0, 0, configuracion->fondoPantalla->anchoImagen, configuracion->fondoPantalla->altoImagen, configuracion->fondoPantalla->hDC, 0, 0, SRCCOPY);
	} else if (configuracion->fondoPantalla && configuracion->fondoPantalla->hDC) {
		int posMin = 0;
		int posMax = (listaPantallas->numPantallas - 1) * configuracion->anchoPantalla;
		double posX = -1 * listaPantallas->listaPantalla[0]->x;
		posX = max(posMin, min(posMax, posX));
		posX = posX / ((listaPantallas->numPantallas - 1) * configuracion->anchoPantalla);
		posX = posX * (configuracion->fondoPantalla->anchoImagen - configuracion->anchoPantalla);

		BitBlt(hDCMem, 0, 0, configuracion->anchoPantalla, configuracion->altoPantalla, configuracion->fondoPantalla->hDC, int(posX), 0, SRCCOPY);
	} else {
		FillRect(hDCMem, &rcWindBounds, hBrushFondo);
	}

	// Pintamos los iconos
	pintaIconos(&hDCMem, &rcWindBounds);


	if (estado->estadoCuadro == 1 || estado->estadoCuadro == 3) {
		FillRect(hDCMem, &estado->cuadroLanzando, hBrushWhite);
	} else if (estado->estadoCuadro == 2) {
		FillRect(hDCMem, &estado->cuadroLanzando, hBrushWhite);
		if (GetTickCount() - estado->timeUltimoLanzamiento >= 2000) {
			SetTimer(hwnd, TIMER_LANZANDO_APP, configuracion->refreshTime, NULL);
			estado->timeUltimoLanzamiento = GetTickCount();
			estado->estadoCuadro = 3;
		}
	}
	BitBlt(hDC, rcWindBounds.left, rcWindBounds.top, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top, hDCMem, rcWindBounds.left, rcWindBounds.top, SRCCOPY);

	EndPaint(hwnd, &ps);

	return 0;
}

LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	// Cogemos la posicion del raton
	POINTS posCursor2;
	posCursor2 = MAKEPOINTS(lParam);

	if (!posCursorInitialized) {
		posCursor = posCursor2;
		posCursorInitialized = TRUE;
	}

	// Comprobamos si se ha superado el umbral para considerar que es movimiento
	BOOL flag = estado->hayMovimiento || abs(posCursor.x - posCursor2.x) > int(configuracion->umbralMovimiento);
	if (configuracion->verticalScroll) {
		flag = flag || abs(posCursor.y - posCursor2.y) > int(configuracion->umbralMovimiento);
	}
	if (flag) {
		KillTimer(hwnd, TIMER_LONGTAP);
		estado->hayMovimiento = true;
		int movx = int((posCursor2.x - posCursor.x)*(1 + float(configuracion->factorMovimiento) / 10));
		int movy = 0;
		if (configuracion->verticalScroll) {
			movy = int((posCursor2.y - posCursor.y)*(1 + float(configuracion->factorMovimiento) / 10));
		}
		posImage.x += movx;
		posImage.y += movy;
		setPosiciones(false, movx, movy);
		posCursor = posCursor2;
	}

	// Actualizamos la imagen
	RECT            rcWindBounds;
	GetClientRect( hwnd, &rcWindBounds);
	InvalidateRect(hwnd, &rcWindBounds, FALSE);
	// UpdateWindow(hwnd);

	return 0;
}

void RightClick(HWND hwnd, POINTS posCursor)
{
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
	if (iconoActual.nIconoActual >= 0) {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_EDIT, TEXT("Edit Icon"));
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_DELETE, TEXT("Delete Icon"));
		if (moveIconoActivo.nIconoActual == -1 || timeActual - moveTimeUltimaSeleccion >= 12000) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_MOVE, TEXT("Move Icon"));
		}
		if (copyIconoActivo.nIconoActual == -1 || timeActual - copyTimeUltimaSeleccion >= 12000) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_COPY, TEXT("Copy Icon"));
		}
	} else {
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_OPTIONS, TEXT("Options"));
	}
	AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hmenu, MF_STRING, MENU_POPUP_CANCEL, TEXT("Cancel"));
#ifdef EXEC_MODE
	AppendMenu(hmenu, MF_STRING, MENU_POPUP_EXIT, TEXT("Exit"));
#endif

	POINT pt;

	// Get coordinates of tap
	pt.x = posCursor.x;
	pt.y = posCursor.y;

	ClientToScreen(hwnd, &pt);

	INT iMenuID = TrackPopupMenuEx(hmenu, TPM_RETURNCMD, pt.x, pt.y, hwnd, NULL);

	switch (iMenuID)
	{
		case MENU_POPUP_ADD:
			iconoActual.nIconoActual = -1;
			iconoActual.nPantallaActual = estado->pantallaActiva;
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
			configuracion->guardaXMLIconos(listaPantallas);
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
				configuracion->cargaImagenIcono(&hDCMem, destIcon, (iconoActual.nPantallaActual == -1));

				configuracion->guardaXMLIconos(listaPantallas);
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
					configuracion->guardaXMLIconos(listaPantallas);
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
	posCursorInitialized = TRUE;

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

	DWORD res = SHRecognizeGesture(&shrg);
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
	bool doubleClick = false;

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
		doMove (hwnd, uimessage, wParam, lParam);

		// Calculamos la posicion objetivo
		calculaPosicionObjetivo();

		// Activamos el timer
		SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
	} else {
		procesaPulsacion(hwnd, point, doubleClick);
	}

	return 0;
}

LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE)
	{
		resizeWindow(hwnd, true);
	}
	// The window is being deactivated... restore it to non-fullscreen
	else if (!::IsChild(hwnd, (HWND)lParam))
	{
		resizeWindow(hwnd, false);
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam);
}

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

void pintaIconos(HDC *hDC, RECT *rcWindBounds) {
	UINT i = 0;
	UINT j = 0;

	CPantalla *pantalla = NULL;
	CIcono *icono = NULL;

	while (i < listaPantallas->numPantallas) {
		pantalla = listaPantallas->listaPantalla[i];

		pintaPantalla(hDC, pantalla);
		i++;
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

		if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0) {
			posCirculos.top = int(configuracion->altoPantalla - listaPantallas->barraInferior->altoPantalla - anchoCirculo);
		} else {
			posCirculos.top = int(configuracion->altoPantalla - anchoCirculo - distanciaCirculo);
		}
		posCirculos.bottom = posCirculos.top + anchoCirculo;

		int colorCircle; // Un valor de 0 a 255
		int distanciaMaxima = (int)(((float)listaPantallas->listaPantalla[0]->anchoPantalla) * 1.4f);

		for (int i = 0; i < nCirculos; i++) {
			pantalla = listaPantallas->listaPantalla[i];
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

	// Pintamos la barra inferior de botones
	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0) {
		pintaPantalla(hDC, listaPantallas->barraInferior, TRUE);
	}
}

void pintaIcono(HDC *hDC, CIcono *icono, BOOL esBarraInferior) {
	// Iniciamos el proceso de pintado
	HDC hdcIconos;
	RECT posTexto;
	hdcIconos = CreateCompatibleDC(*hDC);
	CConfigurationScreen *cs;
	if (esBarraInferior) {
		cs = configuracion->bottomBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}
	UINT width = cs->iconWidth;

	TransparentBlt(*hDC, int(icono->x), int(icono->y), width, width,
		icono->hDC, 0, 0, icono->anchoImagen, icono->altoImagen, RGBA(0, 0, 0, 0));

	// Notificaciones
	if (icono->tipo > 0) {
		int numNotif = 0;
		switch(icono->tipo) {
			case NOTIF_LLAMADAS:
				numNotif = estado->numLlamadas;
				break;
			case NOTIF_SMS:
				numNotif = estado->numSMS;
				break;
			case NOTIF_MMS:
				numNotif = estado->numMMS;
				break;
			case NOTIF_OTHER_EMAIL:
				numNotif = estado->numOtherEmail;
				break;
			case NOTIF_SYNC_EMAIL:
				numNotif = estado->numSyncEmail;
				break;
			case NOTIF_TOTAL_EMAIL:
				numNotif = estado->numOtherEmail + estado->numSyncEmail;
				break;
			case NOTIF_CITAS:
				numNotif = estado->numCitas;
			case NOTIF_CALENDAR:
				{

				SYSTEMTIME st;
				GetLocalTime(&st);
				TCHAR dayOfMonth[16];
				wsprintf(dayOfMonth, TEXT("%i"), st.wDay);

				LOGFONT lf;
				HFONT hFont;
				HFONT hFontOld;
				COLORREF colorOld;
				HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
				GetObject(hSysFont, sizeof(LOGFONT), &lf);

				// Print day of week
				lf.lfWeight = configuracion->dowWeight;
				lf.lfWidth = LONG(configuracion->dowWidth / 100.0 * cs->iconWidth);
				lf.lfHeight = LONG(configuracion->dowHeight / 100.0 * cs->iconWidth);
				lf.lfQuality = DEFAULT_QUALITY;

				hFont = CreateFontIndirect(&lf);
				hFontOld = (HFONT) SelectObject(*hDC, hFont);
				colorOld = SetTextColor(*hDC, configuracion->dowColor);

				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + width);
				posTexto.top = int(icono->y);
				posTexto.bottom = int(icono->y + (width * 0.28));

				DrawText(*hDC, configuracion->diasSemana[st.wDayOfWeek], -1, &posTexto, DT_CENTER | DT_VCENTER);

				DeleteObject(SelectObject(*hDC, hFontOld));
				SetTextColor(*hDC, colorOld);

				// Print day of month
				lf.lfWeight = configuracion->domWeight;
				lf.lfWidth = LONG(configuracion->domWidth / 100.0 * cs->iconWidth);
				lf.lfHeight = LONG(configuracion->domHeight / 100.0 * cs->iconWidth);
				lf.lfQuality = DEFAULT_QUALITY;

				hFont = CreateFontIndirect(&lf);
				hFontOld = (HFONT) SelectObject(*hDC, hFont);
				colorOld = SetTextColor(*hDC, configuracion->domColor);

				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + width);
				posTexto.top = int(icono->y + (width * 0.25));
				posTexto.bottom = int(icono->y + width);

				DrawText(*hDC, dayOfMonth, -1, &posTexto, DT_CENTER | DT_VCENTER);

				DeleteObject(SelectObject(*hDC, hFontOld));
				SetTextColor(*hDC, colorOld);

				}
				break;
			case NOTIF_CLOCK_ALARM:
				numNotif = estado->estadoAlarm;
			case NOTIF_CLOCK:
				{

				SYSTEMTIME st;
				GetLocalTime(&st);

				TCHAR strTime[8];
				LOGFONT lf;
				HFONT hFont;
				HFONT hFontOld;
				COLORREF colorOld;
				HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
				GetObject(hSysFont, sizeof(LOGFONT), &lf);

				lf.lfWeight = configuracion->clockWeight;
				lf.lfWidth = LONG(configuracion->clockWidth / 100.0 * cs->iconWidth);
				lf.lfHeight = LONG(configuracion->clockHeight / 100.0 * cs->iconWidth);
				lf.lfQuality = DEFAULT_QUALITY;

				// create the font
				hFont = CreateFontIndirect(&lf);

				// Select the system font into the device context
				hFontOld = (HFONT) SelectObject(*hDC, hFont);

				// set that color
				colorOld = SetTextColor(*hDC, configuracion->clockColor);

				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + width);
				posTexto.top = int(icono->y);
				posTexto.bottom = int(icono->y + width);

				TCHAR strHour[4];
				TCHAR strMinute[4];
				int hour = st.wHour;
				if (configuracion->clock12Format) {
					if (hour == 0) {
						hour = 12;
					} else if (hour > 12) {
						hour -= 12;
					}
				}

				if (hour < 10 && !configuracion->clock12Format) {
					swprintf(strHour, L"0%i", hour);
				} else {
					swprintf(strHour, L"%i", hour);
				}
				if (st.wMinute < 10) {
					swprintf(strMinute, L"0%i", st.wMinute);
				} else {
					swprintf(strMinute, L"%i", st.wMinute);
				}
				wsprintf(strTime, TEXT("%s:%s"), strHour, strMinute);

				DrawText(*hDC, strTime, -1, &posTexto, DT_CENTER | DT_VCENTER);

				// Select the previous font back into the device context
				DeleteObject(SelectObject(*hDC, hFontOld));
				SetTextColor(*hDC, colorOld);

				}
				break;
			case NOTIF_TAREAS:
				numNotif = estado->numTareas;
				break;
			case NOTIF_SMS_MMS:
				numNotif = estado->numSMS + estado->numMMS;
				break;
			case NOTIF_WIFI:
				numNotif = estado->estadoWifi;
				break;
			case NOTIF_BLUETOOTH:
				numNotif = estado->estadoBluetooth;
				break;
			case NOTIF_ALARM:
				numNotif = estado->estadoAlarm;
				break;
			default:
				numNotif = 0;
		}
		if (numNotif > 0) {
			TCHAR notif[16];

			switch(icono->tipo) {
				case NOTIF_LLAMADAS:
				case NOTIF_SMS:
				case NOTIF_MMS:
				case NOTIF_OTHER_EMAIL:
				case NOTIF_SYNC_EMAIL:
				case NOTIF_TOTAL_EMAIL:
				case NOTIF_CITAS:
				case NOTIF_CALENDAR:
				case NOTIF_TAREAS:
				case NOTIF_SMS_MMS:
					if (numNotif >= 100) {
						wcscpy(notif, TEXT("+"));
					} else {
						swprintf(notif, TEXT("%i"), numNotif);
					}
					/* Bubble with same size that icon and text in top right*/
					posTexto.left = int(icono->x + (width * 0.44));
					posTexto.top = int(icono->y - (width * 0.28));
					posTexto.right = int(posTexto.left + width * 0.80);
					posTexto.bottom = int(posTexto.top + width * 0.80);

					break;
				case NOTIF_CLOCK_ALARM:
					wcscpy(notif, TEXT(""));

					/* Bubble with same size that icon and text in top right*/
					posTexto.left = int(icono->x + (width * 0.44));
					posTexto.top = int(icono->y - (width * 0.36));
					posTexto.right = int(posTexto.left + width * 0.80);
					posTexto.bottom = int(posTexto.top + width * 0.80);

					break;
				case NOTIF_ALARM:
					wcscpy(notif, TEXT(""));

					/* Bubble with same size that icon and text in top right*/
					posTexto.left = int(icono->x + (width * 0.44));
					posTexto.top = int(icono->y - (width * 0.28));
					posTexto.right = int(posTexto.left + width * 0.80);
					posTexto.bottom = int(posTexto.top + width * 0.80);

					break;
				case NOTIF_WIFI:
					wcscpy(notif, TEXT("On"));

					/* Centrado grande */
					posTexto.left = int(icono->x);
					posTexto.top = int(icono->y + (width * 0.50));
					posTexto.right = int(icono->x + (width));
					posTexto.bottom = int(icono->y + (width));

					break;
				case NOTIF_BLUETOOTH:
					if (numNotif == 2) {
						wcscpy(notif, TEXT("Disc"));
					} else {
						wcscpy(notif, TEXT("On"));
					}

					/* Centrado grande */
					/*
					posTexto.left = int(icono->x + (width * 0.15));
					posTexto.top = int(icono->y + (width * 0.60));
					posTexto.right = int(icono->x + (width * 0.85));
					posTexto.bottom = int(icono->y + (width * 0.95));
					*/
					posTexto.left = int(icono->x);
					posTexto.top = int(icono->y + (width * 0.50));
					posTexto.right = int(icono->x + (width));
					posTexto.bottom = int(icono->y + (width));

					break;
			}

			switch(icono->tipo) {
				case NOTIF_LLAMADAS:
				case NOTIF_SMS:
				case NOTIF_MMS:
				case NOTIF_OTHER_EMAIL:
				case NOTIF_SYNC_EMAIL:
				case NOTIF_TOTAL_EMAIL:
				case NOTIF_CITAS:
				case NOTIF_CALENDAR:
				case NOTIF_TAREAS:
				case NOTIF_SMS_MMS:
					// Pintamos la notificacion
					if (configuracion->bubbleNotif->hDC) {
						drawNotification(*hDC, &posTexto, configuracion->bubbleNotif, notif);
					} else {
						drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(200, 0, 0), notif);
					}
					break;

				case NOTIF_CLOCK_ALARM:
				case NOTIF_ALARM:
					// Pintamos la notificacion
					if (configuracion->bubbleAlarm->hDC) {
						drawNotification(*hDC, &posTexto, configuracion->bubbleAlarm, notif);
					} else {
						drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(200, 0, 0), notif);
					}
					break;

				case NOTIF_WIFI:
				case NOTIF_BLUETOOTH:
					// Pintamos la notificacion
					if (configuracion->bubbleState->hDC) {
						drawNotification(*hDC, &posTexto, configuracion->bubbleState, notif);
					} else {
						drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(0, 200, 0), notif);
					}
					break;
			}
		}

	}

	// Pintamos el nombre del icono
	posTexto.top = int(icono->y + width + cs->fontOffset);
	posTexto.bottom = posTexto.top + cs->fontSize;
	posTexto.left = int(icono->x - (cs->distanceIconsH * 0.5));
	posTexto.right = int(icono->x + width + (cs->distanceIconsH * 0.5));

	if (cs->fontSize > 0) {
		DrawText(*hDC, icono->nombre, -1, &posTexto, DT_CENTER | DT_TOP);
	}

	DeleteDC(hdcIconos);
}

void pintaPantalla(HDC *hDC, CPantalla *pantalla, BOOL esBarraInferior) {
	CConfigurationScreen *cs;
	if (esBarraInferior) {
		cs = configuracion->bottomBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}

	// Si debemos recalcular la pantalla
	if (pantalla->debeActualizar) {
		pantalla->debeActualizar = FALSE;
		if (pantalla->hDC == NULL) {
			pantalla->hDC = CreateCompatibleDC(*hDC);
			pantalla->imagen = CreateCompatibleBitmap(*hDC, pantalla->anchoPantalla, pantalla->altoPantalla);
			pantalla->imagenOld = (HBITMAP)SelectObject(pantalla->hDC, pantalla->imagen);

			HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
			LOGFONT lf;
			GetObject(hSysFont, sizeof(LOGFONT), &lf);

			if (cs->fontBold) {
				lf.lfWeight = FW_BOLD;
			} else {
				lf.lfWeight = FW_NORMAL;
			}
			lf.lfHeight = cs->fontSize;
			lf.lfQuality = DEFAULT_QUALITY;

			HFONT hFont = CreateFontIndirect(&lf);

			pantalla->hFontOld = (HFONT)SelectObject(pantalla->hDC, hFont);

			SetTextColor(pantalla->hDC, cs->fontColor);
		}
		SetBkMode(pantalla->hDC, TRANSPARENT);
		// SetBkColor(pantalla->hDC, RGB(0,0,0));

		RECT rc = {0};
		rc.right = pantalla->anchoPantalla;
		rc.bottom = pantalla->altoPantalla;

		FillRect(pantalla->hDC, &rc, hBrushFondo);

		BOOL isTransparent = configuracion->fondoTransparente;
#ifdef EXEC_MODE
		isTransparent = FALSE;
#endif
		if (_tcsclen(configuracion->strFondoPantalla) == 0 && !isTransparent) {
			DrawGradientGDI(pantalla->hDC, rc, cs->backColor1,  cs->backColor2,  0xAAAA);
		}

		if (configuracion->headerFontSize > 0 && _tcslen(pantalla->header) > 0) {
			LOGFONT lf;
			HFONT hFont;
			HFONT hFontOld;
			COLORREF colorOld;
			HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
			GetObject(hSysFont, sizeof(LOGFONT), &lf);

			lf.lfWeight = configuracion->headerFontWeight;
			lf.lfHeight = configuracion->headerFontSize;
			lf.lfQuality = DEFAULT_QUALITY;

			hFont = CreateFontIndirect(&lf);
			hFontOld = (HFONT) SelectObject(pantalla->hDC, hFont);
			colorOld = SetTextColor(pantalla->hDC, configuracion->headerFontColor);

			RECT posTexto;
			posTexto.left = 0;
			posTexto.right = pantalla->anchoPantalla;
			posTexto.top = configuracion->headerOffset;
			posTexto.bottom = configuracion->headerFontSize + configuracion->headerOffset;
			DrawText(pantalla->hDC, pantalla->header, -1, &posTexto, DT_CENTER | DT_VCENTER);

			DeleteObject(SelectObject(pantalla->hDC, hFontOld));
			SetTextColor(pantalla->hDC, colorOld);
		}

		setPosicionesIconos(pantalla, esBarraInferior);

		CIcono *icono = NULL;
		UINT j = 0;
		while (j < pantalla->numIconos) {
			icono = pantalla->listaIconos[j];

			pintaIcono(&pantalla->hDC, icono, esBarraInferior);

			j++;
		}
	}

	// Pintamos la pantalla
	if (pantalla->x + configuracion->anchoPantalla >= 0
		&& pantalla->x <= configuracion->anchoPantalla) {

		int posX = int(pantalla->x);
		int posY = int(pantalla->y);
		int ancho = pantalla->anchoPantalla;
		int alto = pantalla->altoPantalla;

		if (posX < 0) {
			ancho = pantalla->anchoPantalla + posX;
			posX = 0;
		} else {
			ancho = pantalla->anchoPantalla - posX;
		}

		BOOL isTransparent = configuracion->fondoTransparente;
#ifdef EXEC_MODE
		isTransparent = FALSE;
#endif
		if (isTransparent || configuracion->fondoPantalla != NULL) {
			TransparentBlt(*hDC, int(pantalla->x), int(pantalla->y), pantalla->anchoPantalla, pantalla->altoPantalla,
				pantalla->hDC, 0, 0, pantalla->anchoPantalla, pantalla->altoPantalla, estado->colorFondo);
		} else {
			BitBlt(*hDC, int(pantalla->x), int(pantalla->y), pantalla->anchoPantalla, pantalla->altoPantalla,
				pantalla->hDC, 0, 0, SRCCOPY);
		}
	}
}

void setPosicionesIconos(CPantalla *pantalla, BOOL esBarraInferior) {
	CConfigurationScreen *cs;
	if (esBarraInferior) {
		cs = configuracion->bottomBarConfig;
	} else {
		cs = configuracion->mainScreenConfig;
	}

	for (UINT i = 0; i < pantalla->numIconos; i++) {
		CIcono *icono = pantalla->listaIconos[i];

		icono->x = cs->posReference.x + float(int(i % cs->iconsPerRow) * cs->distanceIconsH);
		icono->y = cs->posReference.y + float(int((i / cs->iconsPerRow)) * cs->distanceIconsV);
	}
}

void setPosiciones(BOOL inicializa, int offsetX, int offsetY) {
	UINT i = 0;
	UINT j = 0;

	CPantalla *pantalla = NULL;
	while (i < listaPantallas->numPantallas) {
		pantalla = listaPantallas->listaPantalla[i];

		if (inicializa) {
			pantalla->x = estado->posObjetivo.x + float(i * configuracion->anchoPantalla);
			pantalla->y = estado->posObjetivo.y;
		} else {
			pantalla->x += offsetX;
			pantalla->y += offsetY;
		}

		pantalla->altoPantalla = configuracion->altoPantallaMax;
		pantalla->anchoPantalla = configuracion->anchoPantalla;

		i++;
	}

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0) {
		listaPantallas->barraInferior->altoPantalla = configuracion->bottomBarConfig->distanceIconsV + configuracion->bottomBarConfig->offset.top + configuracion->bottomBarConfig->offset.bottom;
		listaPantallas->barraInferior->anchoPantalla = configuracion->anchoPantalla;
		listaPantallas->barraInferior->x = 0;
		listaPantallas->barraInferior->y = float(configuracion->altoPantalla - listaPantallas->barraInferior->altoPantalla);
	}
}

void calculaPosicionObjetivo() {
	CPantalla *pantalla = NULL;
	CIcono *icono = NULL;
	UINT distanciaMinima = 0xFFFF;
	UINT distAux;
	UINT pantallaActivaOld = estado->pantallaActiva;
	estado->pantallaActiva = 0;
	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		pantalla = listaPantallas->listaPantalla[i];
		if (pantalla != NULL && pantalla->numIconos > 0) {
			distAux = abs(int(pantalla->x));
			if (distAux < distanciaMinima) {
				distanciaMinima = distAux;
				estado->pantallaActiva = i;
			}
		}
	}

	if (distanciaMinima >= configuracion->anchoPantalla * 4 / 5) {
		if (estado->pantallaActiva == 0) {
			estado->pantallaActiva = listaPantallas->numPantallas - 1;
		} else {
			estado->pantallaActiva = 0;
		}
	}

	estado->posObjetivo.x = 0 - (estado->pantallaActiva * configuracion->anchoPantalla);
	if (posImage.y >= 0 || pantallaActivaOld != estado->pantallaActiva) {
		estado->posObjetivo.y = 0;
	} else {
		INT h = configuracion->altoPantalla;
		if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0) {
			h -= listaPantallas->barraInferior->altoPantalla;
		}
		INT nrows_scroll = (-posImage.y + configuracion->mainScreenConfig->distanceIconsV / 2) / configuracion->mainScreenConfig->distanceIconsV;
		INT nrows_screen = ((listaPantallas->listaPantalla[estado->pantallaActiva]->numIconos + configuracion->mainScreenConfig->iconsPerRow - 1) / configuracion->mainScreenConfig->iconsPerRow);
		INT nrows_fit = h / configuracion->mainScreenConfig->distanceIconsV;
		if (nrows_scroll > nrows_screen - nrows_fit) {
			nrows_scroll = nrows_screen - nrows_fit;
			if (nrows_scroll < 0) {
				nrows_scroll = 0;
			}
		}
		estado->posObjetivo.y = - nrows_scroll * (int) configuracion->mainScreenConfig->distanceIconsV;
	}
}

#ifndef EXEC_MODE
/*************************************************************************/
/* Initialize the DLL by creating a new window                             */
/*************************************************************************/
HWND InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent)
{
#ifdef DEBUG1
	// Inifinite loop to attach the debugger to mstli.exe (Pocket PC) or shell32.exe (Windows Mobile)
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
					// Currently included in:
					// PropertySheetPage0.cpp
					// PropertySheetPage3.cpp
					// PropertySheetPage4.cpp


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

	TCHAR fullPath[MAX_PATH];
	configuracion->getAbsolutePath(fullPath, CountOf(fullPath), pCmdLine);

	// Launch de application
	BOOL bWorked;
	SHELLEXECUTEINFO sei;

	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);

	sei.lpFile = fullPath;
	sei.nShow = SW_SHOWNORMAL;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = pParametros;

	bWorked = ShellExecuteEx(&sei);

#ifdef EXEC_MODE
	if (bWorked && configuracion->closeOnLaunchIcon == 1) {
		PostQuitMessage(0);
	}
#endif

	return bWorked;
}

void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar) {

	int nCirculos = listaPantallas->numPantallas;
	int anchoCirculo = configuracion->circlesDiameter;
	int distanciaCirculo = configuracion->circlesDistance;
	int xLeft, xRight, yTop, yBottom;

	// Calculamos el cuadrado recubridor de la barra de circulos activos
	xLeft = int((configuracion->anchoPantalla / 2) - ((nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo) / 2);
	xRight = xLeft + (nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo;

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0) {
		yTop = int(configuracion->altoPantalla - listaPantallas->barraInferior->altoPantalla - anchoCirculo);
	} else {
		yTop = int(configuracion->altoPantalla - anchoCirculo - distanciaCirculo);
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
	BOOL enc = false;
	UINT i;

	iconoActual.nPantallaActual = estado->pantallaActiva;
	iconoActual.nIconoActual = -1;

	BOOL isClickOnBottomBar = listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0 && configuracion->bottomBarConfig->iconWidth > 0
		&& posCursor.x >= listaPantallas->barraInferior->x && posCursor.x <= (listaPantallas->barraInferior->x + listaPantallas->barraInferior->anchoPantalla)
		&& posCursor.y >= listaPantallas->barraInferior->y && posCursor.y <= (listaPantallas->barraInferior->y + listaPantallas->barraInferior->altoPantalla);

	if (!enc && isClickOnBottomBar) {
		pantalla = listaPantallas->barraInferior;
		i = 0;
		while (i < pantalla->numIconos && !enc) {
			icono = pantalla->listaIconos[i];
			if (posCursor.x >= pantalla->x + icono->x && posCursor.x <= pantalla->x + icono->x + configuracion->bottomBarConfig->iconWidth &&
				posCursor.y >= pantalla->y + icono->y && posCursor.y <= pantalla->y + icono->y + configuracion->bottomBarConfig->iconWidth) {
					enc = true;
					iconoActual.nPantallaActual = -1;
					iconoActual.nIconoActual = i;
			}

			i++;
		}

		if (noLanzar) {
			enc = true;
			iconoActual.nPantallaActual = -1;
		}
	}

	pantalla = listaPantallas->listaPantalla[estado->pantallaActiva];
	i = 0;
	while (!enc && i < pantalla->numIconos) {
		icono = pantalla->listaIconos[i];
		if (posCursor.x >= pantalla->x + icono->x && posCursor.x <= pantalla->x + icono->x + configuracion->mainScreenConfig->iconWidth &&
			posCursor.y >= pantalla->y + icono->y && posCursor.y <= pantalla->y + icono->y + configuracion->mainScreenConfig->iconWidth) {
				enc = true;
				iconoActual.nIconoActual = i;
		}

		i++;
	}

	if (!enc && isClickOnBottomBar) {
		enc = true;
		iconoActual.nPantallaActual = -1;
	}

	if (!noLanzar && enc && iconoActual.nIconoActual >= 0) {
		// Vibration
		if (configuracion->vibrateOnLaunchIcon > 0) {
			vibrate(configuracion->vibrateOnLaunchIcon);
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

int hayNotificacion(int tipo) {
	int numNotif = 0;
	switch(tipo) {
		case NOTIF_LLAMADAS:
			numNotif = estado->numLlamadas;
			break;
		case NOTIF_SMS:
			numNotif = estado->numSMS;
			break;
		case NOTIF_MMS:
			numNotif = estado->numMMS;
			break;
		case NOTIF_OTHER_EMAIL:
			numNotif = estado->numOtherEmail;
			break;
		case NOTIF_SYNC_EMAIL:
			numNotif = estado->numSyncEmail;
			break;
		case NOTIF_TOTAL_EMAIL:
			numNotif = estado->numOtherEmail + estado->numSyncEmail;
			break;
		case NOTIF_CITAS:
			numNotif = estado->numCitas;
			break;
		case NOTIF_TAREAS:
			numNotif = estado->numTareas;
			break;
		case NOTIF_SMS_MMS:
			numNotif = estado->numSMS + estado->numMMS;
			break;
		case NOTIF_WIFI:
			numNotif = estado->estadoWifi;
			break;
		case NOTIF_BLUETOOTH:
			numNotif = estado->estadoBluetooth;
			break;
		case NOTIF_CLOCK_ALARM:
		case NOTIF_ALARM:
			numNotif = estado->estadoAlarm;
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

			// Create a Done button and size it.
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLG | SHIDIF_WANTSCROLLBAR;
			shidi.hDlg = hDlg;
			SHInitDialog(&shidi);

			SHInitExtraControls();

			// Configuramos el elemento Screen
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_MICON_SCREEN), 0);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETRANGE, 0, MAKELPARAM(-1, MAX_PANTALLAS - 1));
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_SCREEN), UDM_SETPOS, 0, iconoActual.nPantallaActual);

			// Configuramos el elemento Icon
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETBUDDY, (WPARAM) GetDlgItem(hDlg, IDC_MICON_ICON), 0);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETRANGE, 0, MAKELPARAM(0, MAX_ICONOS_PANTALLA - 1));
			SendMessage(GetDlgItem(hDlg, IDC_MICON_SPIN_ICON), UDM_SETPOS, 0, max(iconoActual.nIconoActual, 0));

			// Configuramos el elemento Type
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_NORMAL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_LLAMADAS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_MMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_OTHER_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SYNC_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_TOTAL_EMAIL_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CITAS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CALENDAR_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_TAREAS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_SMS_MMS_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_WIFI_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_BLUETOOTH_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_ALARM_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CLOCK_TXT);
			SendMessage(GetDlgItem(hDlg, IDC_MICON_TYPE), CB_ADDSTRING, 0, (LPARAM)NOTIF_CLOCK_ALARM_TXT);

			// Configuramos los checks
			SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, BST_CHECKED, 0);

			// Si es editar ponemos los valores
			if (iconoActual.nIconoActual >= 0) {
				CIcono *icono = NULL;
				if (iconoActual.nPantallaActual == -1) {
					icono = listaPantallas->barraInferior->listaIconos[iconoActual.nIconoActual];
				} else {
					icono = listaPantallas->listaPantalla[iconoActual.nPantallaActual]->listaIconos[iconoActual.nIconoActual];
				}

				SetDlgItemText(hDlg, IDC_MICON_NAME, icono->nombre);
				SetDlgItemText(hDlg, IDC_MICON_IMAGE, icono->rutaImagen);
				SetDlgItemText(hDlg, IDC_MICON_EXEC, icono->ejecutable);
				SetDlgItemText(hDlg, IDC_MICON_PARAMETERS, icono->parametros);
				SetDlgItemText(hDlg, IDC_MICON_EXECALT, icono->ejecutableAlt);
				SetDlgItemText(hDlg, IDC_MICON_PARAMETERSALT, icono->parametrosAlt);

				SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, icono->launchAnimation ? BST_CHECKED : BST_UNCHECKED, 0);

				if (icono->tipo == NOTIF_NORMAL) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_NORMAL_TXT);
				} else if (icono->tipo == NOTIF_LLAMADAS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_LLAMADAS_TXT);
				} else if (icono->tipo == NOTIF_SMS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_SMS_TXT);
				} else if (icono->tipo == NOTIF_MMS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_MMS_TXT);
				} else if (icono->tipo == NOTIF_OTHER_EMAIL) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_OTHER_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_SYNC_EMAIL) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_SYNC_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_TOTAL_EMAIL) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_TOTAL_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_CITAS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_CITAS_TXT);
				} else if (icono->tipo == NOTIF_CALENDAR) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_CALENDAR_TXT);
				} else if (icono->tipo == NOTIF_TAREAS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_TAREAS_TXT);
				} else if (icono->tipo == NOTIF_SMS_MMS) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_SMS_MMS_TXT);
				} else if (icono->tipo == NOTIF_WIFI) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_WIFI_TXT);
				} else if (icono->tipo == NOTIF_BLUETOOTH) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_BLUETOOTH_TXT);
				} else if (icono->tipo == NOTIF_ALARM) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_ALARM_TXT);
				} else if (icono->tipo == NOTIF_CLOCK) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_CLOCK_TXT);
				} else if (icono->tipo == NOTIF_CLOCK_ALARM) {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_CLOCK_ALARM_TXT);
				} else {
					SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_NORMAL_TXT);
				}
			} else {
				SetDlgItemText(hDlg, IDC_MICON_TYPE, NOTIF_NORMAL_TXT);
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
			} else if (lstrcmpi(strType, NOTIF_LLAMADAS_TXT) == 0) {
				nType = NOTIF_LLAMADAS;
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
			} else if (lstrcmpi(strType, NOTIF_CITAS_TXT) == 0) {
				nType = NOTIF_CITAS;
			} else if (lstrcmpi(strType, NOTIF_CALENDAR_TXT) == 0) {
				nType = NOTIF_CALENDAR;
			} else if (lstrcmpi(strType, NOTIF_TAREAS_TXT) == 0) {
				nType = NOTIF_TAREAS;
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
			} else {
				MessageBox(hDlg, TEXT("Type not valid!"), TEXT("Error"), MB_OK);
				return FALSE;
			}

			// All Ok, Icon Creation
			CPantalla *pantalla = NULL;
			CIcono *icono = NULL;
			// BITMAP bm;

			if (newScreen) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				if (nScreen == -1) {
					if (listaPantallas->barraInferior == NULL) {
						listaPantallas->barraInferior = new CPantalla();
					}
					pantalla = listaPantallas->barraInferior;
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
			StringCchCopy(icono->ejecutable, CountOf(icono->ejecutable), strExec);
			StringCchCopy(icono->parametros, CountOf(icono->parametros), strParameters);
			StringCchCopy(icono->ejecutableAlt, CountOf(icono->ejecutableAlt), strExecAlt);
			StringCchCopy(icono->parametrosAlt, CountOf(icono->parametrosAlt), strParametersAlt);
			icono->tipo = nType;
			icono->launchAnimation = launchAnimation;
			configuracion->cargaImagenIcono(&hDCMem, icono, (pantalla == listaPantallas->barraInferior));

			setPosiciones(true, 0, 0);

			configuracion->guardaXMLIconos(listaPantallas);
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
		} else if (LOWORD(wParam) == IDC_MICON_EXEC_B) {
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
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, pathFile, lastPathExec)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathExec);

				SetDlgItemText(hDlg, IDC_MICON_EXECALT, pathFile);
			}
		}
		break;
	case WM_DESTROY:
		break;
	}
	return FALSE;
}

#ifndef EXEC_MODE
BOOL cargaFondoPantalla(HWND hwnd) {

	if (!configuracion) {
		return FALSE;
	}

	BOOL isTransparent = configuracion->fondoTransparente;
#ifdef EXEC_MODE
	isTransparent = FALSE;
#endif
	if (!isTransparent) {
		return FALSE;
	}

	if (configuracion->fondoPantalla == NULL) {
		configuracion->fondoPantalla = new CIcono();
	}


	RECT rc;
	// GetClientRect(hwnd, &rc);
	rc.left = 0;
	rc.top = 0;
	rc.right = configuracion->anchoPantalla;
	rc.bottom = configuracion->altoPantalla;
	if (configuracion->fondoPantalla->hDC == NULL) {
		HDC hdc = GetDC(hwnd);
		configuracion->fondoPantalla->hDC = CreateCompatibleDC(hdc);
		configuracion->fondoPantalla->imagen = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		configuracion->fondoPantalla->imagenOld = (HBITMAP)SelectObject(configuracion->fondoPantalla->hDC, configuracion->fondoPantalla->imagen);

		// BITMAP bm;
		// GetObject(configuracion->fondoPantalla->imagen, sizeof(BITMAP), &bm);
		// configuracion->fondoPantalla->anchoImagen = bm.bmWidth;
		// configuracion->fondoPantalla->altoImagen = bm.bmHeight;
		configuracion->fondoPantalla->anchoImagen = rc.right - rc.left;
		configuracion->fondoPantalla->altoImagen = rc.bottom - rc.top;

		ReleaseDC(hwnd, hdc);
	}

	TODAYDRAWWATERMARKINFO	dwi;
	dwi.hdc = configuracion->fondoPantalla->hDC;
	dwi.hwnd = hwnd;
	dwi.rc = rc;
	SendMessage(GetParent(hwnd), TODAYM_DRAWWATERMARK, 0, (LPARAM)&dwi);


	return TRUE;
}
#endif

BOOL inicializaApp(HWND hwnd) {

	configuracion = new CConfiguracion();
	estado = new CEstado();

	HDC hdc = GetDC(hwnd);

	// Debe ser inicializado en modo normal
	estado->estadoCuadro = 0;

	// Cargamos los parametros de configuracion
	// long duration = -(long)GetTickCount();
	configuracion->cargaXMLConfig();
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to cargaXMLConfig.\n", duration);

#ifdef EXEC_MODE
	if (configuracion->fullscreen) {
		SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
	} else {
		RECT rc;
		if (GetWindowRect(GetDesktopWindow(), &rc)) {
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

	// Auto-Configure if is neccesary
	autoConfigure();

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
	// duration = -(long)GetTickCount();
	configuracion->cargaIconos(&hdc, listaPantallas);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to cargaIconos.\n", duration);

	calculateConfiguration(windowWidth, windowHeight);

	configuracion->cargaImagenes(&hdc);

	BOOL isTransparent = configuracion->fondoTransparente;
#ifdef EXEC_MODE
	isTransparent = FALSE;
#endif
	if (isTransparent || _tcsclen(configuracion->strFondoPantalla) > 0) {
		// estado->colorFondo = RGB(255, 0, 255);
		estado->colorFondo = RGBA(0, 0, 0, 0);
	} else {
		estado->colorFondo = configuracion->fondoColor;
	}

	// Establecemos la ruta por defecto para buscar programas
	if (!SHGetSpecialFolderPath(0, lastPathExec, CSIDL_PROGRAMS, FALSE)) {
		wcscpy(lastPathExec, L"\\");
	}

	StringCchCopy(lastPathImage, CountOf(lastPathImage), configuracion->pathIconsDir);

	ReleaseDC(hwnd, hdc);

	setPosiciones(true, 0, 0);

	if (configuracion->notifyTimer > 0) {
		SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, configuracion->notifyTimer, NULL);
	}

	return TRUE;
}

BOOL borraObjetosHDC() {
	borraHDC_HBITMPAP(&hDCMem, &hbmMem, &hbmMemOld);

	borraHDC_HBITMPAP(
		&listaPantallas->barraInferior->hDC,
		&listaPantallas->barraInferior->imagen,
		&listaPantallas->barraInferior->imagenOld);
	listaPantallas->barraInferior->debeActualizar = TRUE;

	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		borraHDC_HBITMPAP(
			&listaPantallas->listaPantalla[i]->hDC,
			&listaPantallas->listaPantalla[i]->imagen,
			&listaPantallas->listaPantalla[i]->imagenOld);
		listaPantallas->listaPantalla[i]->debeActualizar = TRUE;
	}

	if(hBrushFondo) {
		DeleteObject(hBrushFondo);
		hBrushFondo = NULL;
	}

	if(hBrushWhite) {
		DeleteObject(hBrushWhite);
		hBrushWhite = NULL;
	}

	return true;
}

BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld) {
	if(*hbm && *hdc) {
		SelectObject(*hdc, *hbmOld);
		DeleteDC(*hdc);
		DeleteObject(*hbm);
		*hdc = NULL;
		*hbm = NULL;
	}

	return true;
}

// Draw a ellipse
void drawNotification(HDC hDC, RECT *rect, CIcono *imagen, TCHAR *texto) {
	TransparentBlt(hDC, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top,
						imagen->hDC, 0, 0, imagen->anchoImagen, imagen->altoImagen, RGBA(0, 0, 0, 0));

	if (texto != NULL) {
		DrawText(hDC, texto, -1, rect, DT_CENTER | DT_VCENTER);
	}
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
			if (lstrcmpi(lpCmdLine, L"close") == 0) {
				PostMessage(g_hWnd, WM_DESTROY, 0, 0);
			} else if (lstrcmpi(lpCmdLine, L"options") == 0) {
				PostMessage(g_hWnd, WM_USER_OPTIONS, 0, 0);
			} else if (lstrcmpi(lpCmdLine, L"add") == 0) {
				PostMessage(g_hWnd, WM_USER_ADD, 0, 0);
			} else {
				PostMessage(g_hWnd, WM_USER_GOTO, _wtoi(lpCmdLine), 0);
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

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// duration = -(long)GetTickCount();
	doDestroy(g_hWnd);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to destroy.\n", duration);

	// duration = -(long)GetTickCount();
	if( g_pImgFactory != NULL ) g_pImgFactory->Release(); g_pImgFactory = NULL;
	CoUninitialize();
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to uninitialize the imaging API.\n", duration);

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

	if (listaPantallas != NULL) {
		delete listaPantallas;
	}
	if (estado != NULL) {
		delete estado;
	}
	if (configuracion != NULL) {
		delete configuracion;
	}

	listaPantallas = NULL;
	estado = NULL;
	configuracion = NULL;

	if(hbmMem != NULL && hDCMem != NULL) {
		SelectObject(hDCMem, hbmMemOld);
		DeleteDC(hDCMem);
		DeleteObject(hbmMem);
		hDCMem = NULL;
		hbmMem = NULL;
	}

	if(hBrushFondo != NULL) {
		DeleteObject(hBrushFondo);
	}

	if(hBrushWhite != NULL) {
		DeleteObject(hBrushWhite);
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

void autoConfigure()
{
	if (configuracion == NULL || configuracion->alreadyConfigured > 0) {
		return;
	}

	// Check if user wants autoconfigure
	//int resp = MessageBox(NULL, TEXT("Auto configure icon and font size?"), TEXT("First Run!"), MB_YESNO);
	//if (resp == IDYES) {
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		int tmp;
		if (width > height) {
			tmp = height;
			height = width;
			width = tmp;
		}

		int iconWidth = int(float(width) * 0.1875);

		configuracion->mainScreenConfig->iconWidthXML = iconWidth;
		configuracion->mainScreenConfig->fontSize = iconWidth / 4;

		configuracion->bottomBarConfig->iconWidthXML = iconWidth;
		configuracion->bottomBarConfig->fontSize = iconWidth / 4;

		configuracion->mainScreenConfig->minVerticalSpace = 5;
		configuracion->mainScreenConfig->offset.top = 5;

		configuracion->circlesDiameter = iconWidth / 6;
		configuracion->circlesDistance = configuracion->circlesDiameter / 2;
	//}

	configuracion->alreadyConfigured = 1;
	configuracion->guardaXMLConfig();
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
	RECT rwp;
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	if (GetWindowRect(GetParent(hwnd), &rwp)) {
		if (rwp.bottom <= cyScreen) {
			*windowHeight = rwp.bottom - rw.top;
		} else {
			*windowHeight = cyScreen - rw.top;
		}
	} else {
		*windowHeight = cyScreen - rw.top;
	}
	*windowHeight = *windowHeight - GetSystemMetrics(SM_CYBORDER);
	*windowWidth = GetSystemMetrics(SM_CXSCREEN);

#endif
#ifdef DEBUG1
	NKDbgPrintfW(L"getWindowSize(%d, %d)\n", *windowWidth, *windowHeight);
#endif
}
