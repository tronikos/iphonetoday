//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//


// ****************************************************************************
// FILE: iPhoneToday.cpp
// ABTRACT: Main implementation file for the today item
// ****************************************************************************
//

#include "stdafx.h"
#include "iPhoneToday.h"
#include "OptionDialog.h"  // CreatePropertySheet includes

// DEBUG
TCHAR accionActual[1024] = TEXT("");
DWORD tAnt = 0;

//global variables    
UINT                g_plugInHeight;
HINSTANCE           g_hInst;
HWND				g_hWndMenuBar = NULL; // menu bar handle
HWND                g_hWnd;
BOOL                g_bFirstDisplay = TRUE;
HFONT				hFont;
COLORREF			crText;

BOOL				g_bLoading = FALSE;


// Variables 
CListaPantalla *listaPantallas = NULL; // = new CListaPantalla();
CConfiguracion *configuracion = NULL; // = new CConfiguracion();
CEstado *estado = NULL; // = new CEstado();

int cont = 0;
POINTS posCursor;
POINTS posCursorInicial;
POINTS posImage = {0, 2};
POINTS posImageDestino = {0, 2};
HDC		hDCMem = NULL;
HBITMAP	hbmMem = NULL;
HBITMAP	hbmMemOld = NULL;
HBRUSH  hBrushFondo = NULL;
HBRUSH  hBrushNegro = NULL;

// Variables para detectar doble click
POINTS posUltimoClick = {0};
long timeUltimoClick = 0;

// Variables para mover iconos rapidamente
IDENT_ICONO moveIconoActivo = {0};
int moveTimeUltimaSeleccion = 0;

// Variable que almacena las ultimas rutas usadas
TCHAR lastPathImage[MAX_PATH];
TCHAR lastPathExec[MAX_PATH];

// Icono a editar
IDENT_ICONO iconoActual;

// int test = 0;

// forward function declarations
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);


static INT InitializeClasses();
LRESULT WndProcLoading (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doPaint (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseDown (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doMouseUp (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam);
BOOL LaunchApplication(LPCTSTR pCmdLine, LPCTSTR pParameters);
void pintaIconos(HDC *hDC, RECT *rcWindBounds);
void pintaIcono(HDC *hDC, CIcono *icono);
void pintaPantalla(HDC *hDC, CPantalla *pantalla, BOOL esBarraInferior = FALSE);
void iniciaIcono(CIcono *icono);
void setPosicionesIconos(CPantalla *pantalla, BOOL esBarraInferior);
void setPosiciones(BOOL inicializa, int offsetX, int offsetY);
void calculaPosicionObjetivo();
void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar = FALSE);
int hayNotificacion(int tipo);
BOOL editaIcono();
BOOL cargaFondoPantalla(HWND hwnd);
BOOL inicializaApp(HWND hwnd);
BOOL borraObjetosHDC();
BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld);
void drawNotification(HDC hDC, RECT *rect, CIcono *imagen, TCHAR *texto);
LRESULT CALLBACK editaIconoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void doDestroy(HWND hwnd);
void resizeWindow(HWND hwnd, BOOL fullScreen);
void autoConfigure();
LRESULT WINAPI		CustomItemOptionsDlgProc(HWND, UINT, WPARAM, LPARAM);

/*************************************************************************/
/* Entry point for the dll                                                 */
/*************************************************************************/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:      

		// WriteToLog(TEXT("DLL_PROCESS_ATTACH\r\n"));
		// init it
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
        
        // The DLL is being loaded for the first time by a given process.
        // Perform per-process initialization here.  If the initialization
        // is successful, return TRUE; if unsuccessful, return FALSE.
        g_hInst = (HINSTANCE)hModule;
        
        //initilize the application class, and set the global window handle
        UnregisterClass((LPCTSTR)LoadString(g_hInst, IDS_TODAY_STORAGE_APPNAME,0,0), g_hInst);
        InitializeClasses();
        g_hWnd = 0;
        
        break;
        
    case DLL_PROCESS_DETACH:

		// WriteToLog(TEXT("DLL_PROCESS_DETACH\r\n"));
        // The DLL is being unloaded by a given process.  Do any
        // per-process clean up here, such as undoing what was done in
        // DLL_PROCESS_ATTACH.    The return value is ignored.

		CoUninitialize();
       
        UnregisterClass((LPCTSTR)LoadString(g_hInst, IDS_TODAY_STORAGE_APPNAME,0,0), g_hInst);
        g_hInst = NULL;
        break;           
    }
    
    return TRUE;
}

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
	if (g_bLoading) {
		return WndProcLoading (hwnd, uimessage, wParam, lParam);
	}

    switch (uimessage)
    {          
        //check to see if a refresh is required
    case WM_TODAYCUSTOM_QUERYREFRESHCACHE: 
    {
            TODAYLISTITEM *ptliItem;
            // INT iItemHeight = 240;
            BOOL    bReturn = FALSE;
            
            // get the pointer to the item from the Today screen
            ptliItem = (TODAYLISTITEM*)wParam;

            if ((NULL == ptliItem) || (WaitForSingleObject(SHELL_API_READY_EVENT, 0) == WAIT_TIMEOUT))
            {
                return FALSE;
            }

			if (0 == ptliItem->cyp || configuracion->altoPantalla != ptliItem->cyp) 
			{
					// ptliItem->cyp = iItemHeight;
					ptliItem->cyp = configuracion->altoPantalla;
					bReturn = TRUE;
			}
			
			RECT            rcWindBounds; 
			GetClientRect( hwnd, &rcWindBounds); 
			InvalidateRect(hwnd, &rcWindBounds, FALSE);
			UpdateWindow(hwnd);
            return bReturn;            
    }        
        break;

    case WM_CREATE:         
		{
			doDestroy(g_hWnd);
			// inicializaApp(g_hWnd);
			
			
			#ifdef EXEC_MODE
				g_bLoading = TRUE;
			#else
				if (g_bFirstDisplay) {
					// g_bFirstDisplay = FALSE;
					inicializaApp(g_hWnd);
				} else {
					g_bLoading = TRUE;
				}
			#endif

			RECT            rcWindBounds; 
			GetClientRect( g_hWnd, &rcWindBounds); 
			InvalidateRect(g_hWnd, &rcWindBounds, FALSE);

			// Activamos el timer
			SetTimer(hwnd, TIMER_ACTUALIZA_NOTIF, 2000, NULL);
		}
        break;

	case WM_LBUTTONDOWN: 
		return doMouseDown (hwnd, uimessage, wParam, lParam);
	case WM_LBUTTONUP: 
		return doMouseUp (hwnd, uimessage, wParam, lParam);
	case WM_MOUSEMOVE:
		return doMove (hwnd, uimessage, wParam, lParam);
	case WM_ACTIVATE:
		return doActivate (hwnd, uimessage, wParam, lParam);
	case WM_TIMER:
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
					/**/
				}
				
				RECT            rcWindBounds2; 
				GetClientRect( hwnd, &rcWindBounds2); 
				InvalidateRect(hwnd, &rcWindBounds2, FALSE);
				UpdateWindow(hwnd);
				return FALSE;

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

			/*
			TCHAR cad[128];
			swprintf(cad, TEXT("%d, %d, %d, %d"), 
				estado->cuadroLanzando.left, estado->cuadroLanzando.right, 
				estado->cuadroLanzando.top, estado->cuadroLanzando.bottom);
			MessageBox(0, cad, cad, MB_OK);
			/**/

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

							configuracion->cargaImagenIcono(&hDCMem, icono);
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
		} else { // TIMER_RECUPERACION
			if (abs(posImage.x - estado->posObjetivo.x) < 2 && abs(posImage.y - estado->posObjetivo.y) < 2) {
				KillTimer(hwnd, wParam);	
				posImage = estado->posObjetivo;
				setPosiciones(true, 0, 0);
				estado->hayMovimiento = false;
			} else {
				// movx = abs(posImage.x - estado->posObjetivo.x)*0.30;
				// movy = (posImage.y - posObjetivo.y)*0.30;
				movx = configuracion->velMinima + (configuracion->velMaxima - configuracion->velMinima) * abs(posImage.x - estado->posObjetivo.x) / configuracion->anchoPantalla;
				movy = (posImage.y - estado->posObjetivo.y)*0.30;
				
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

				if (abs(posImage.x - estado->posObjetivo.x) < abs(int(movx))) {
					KillTimer(hwnd, wParam);	
					posImage = estado->posObjetivo;
					setPosiciones(true, 0, 0);
					estado->hayMovimiento = false;
				} else {
					// posImage.x -= short(movx);
					posImage.x += short(movx);
					posImage.y -= short(movy);
					// setPosiciones(false, int(-movx), int(-movy));
					setPosiciones(false, int(movx), int(-movy));
				}
			}	
		}

		RECT            rcWindBounds2; 
		GetClientRect( hwnd, &rcWindBounds2); 
		InvalidateRect(hwnd, &rcWindBounds2, FALSE);
		UpdateWindow(hwnd);

		return 0;
    case WM_PAINT: 
        return doPaint (hwnd, uimessage, wParam, lParam);

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
#ifdef EXEC_MODE
			// PostQuitMessage(0);
			// ShowWindow(hwnd, SW_SHOWNORMAL);
			// resizeWindow(hwnd, TRUE);
			// SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#endif
			return TRUE;
		} else {
			return DefWindowProc (hwnd, uimessage, wParam, lParam);
		}
	// Detect position and z-order changes
	case WM_WINDOWPOSCHANGED:
		{
		if (estado == NULL || configuracion == NULL) {
			return 0;
		}
		
		// Si el semaforo esta activo es porque se ha lanzado el evento antes
		if (estado->semaforoRotacion == 1 || configuracion->ignoreRotation > 0) {
			return 0;
		}
		estado->semaforoRotacion = 1;

		//Compare the height and width of the screen and act accordingly.
		RECT rc;
		#ifdef EXEC_MODE
			GetClientRect(hwnd, &rc);
		#else
			SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		#endif
		int theScreenWidth = rc.right - rc.left;
		int theScreenHeight = rc.bottom - rc.top;
		int estadoActual = 0;
		if (theScreenWidth > theScreenHeight) {
			estadoActual = 1;
		}
		if (estadoActual != estado->estadoPantalla) {

			estado->estadoPantalla = estadoActual;

			// Cargamos la configuracion calculada en funcion de los iconos
			int maxIconos = 0;
			for (int i = 0; i < (int)listaPantallas->numPantallas; i++) {
				maxIconos = max(maxIconos, (int)listaPantallas->listaPantalla[i]->numIconos);
			}
			configuracion->calculaConfiguracion(
				maxIconos, 
				(listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0),
				theScreenWidth, 
				theScreenHeight);

			// MoveWindow(hwnd, 0, 0, theScreenWidth, theScreenHeight, TRUE);

			configuracion->cargaFondo(&hDCMem);

			borraObjetosHDC();
			setPosiciones(true, 0, 0); 

			// Calculamos la posicion objetivo
			estado->posObjetivo.x = 0 - (estado->pantallaActiva * configuracion->anchoPantalla);
			estado->posObjetivo.y = 0;

			// Activamos el timer
			SetTimer(hwnd, TIMER_ROTATING, 100, NULL);
			SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
		}

		estado->semaforoRotacion = 0;
		}
		return DefWindowProc (hwnd, uimessage, wParam, lParam);
		// return 0;

    case WM_DESTROY:    
		// WriteToLog(TEXT("WM_DESTROY\r\n"));

		// KillTimer(hwnd, TIMER_RECUPERACION);
		// KillTimer(hwnd, TIMER_ACTUALIZA_NOTIF);

		doDestroy(hwnd);

#ifdef EXEC_MODE
		PostQuitMessage(0);
#endif

        return 0;
        
    // this fills in the background with the today screen image
    case WM_ERASEBKGND:

		return cargaFondoPantalla(hwnd);
	}

    return DefWindowProc (hwnd, uimessage, wParam, lParam) ;
}

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

		inicializaApp(hwnd);
		g_bLoading = FALSE;
		UpdateWindow(hwnd);

		return 0;
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam);
}

LRESULT doPaint (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam) 
{
		PAINTSTRUCT     ps;
        RECT            rcWindBounds; 
        RECT            rcMyBounds;
		HDC             hDC;
        HFONT           hFontOld;
		LOGFONT			lf;
        HFONT			hSysFont;
      

		GetClientRect(hwnd, &rcWindBounds); 
        
        hDC = BeginPaint(hwnd, &ps);
		if (hDCMem == NULL) {
			hDCMem = CreateCompatibleDC(hDC);
		 	hbmMem = CreateCompatibleBitmap(hDC, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top);
			hbmMemOld = (HBITMAP)SelectObject(hDCMem, hbmMem);

			hBrushFondo = CreateSolidBrush(estado->colorFondo);
			hBrushNegro = CreateSolidBrush(RGB(255,255,255));
		}
		
        
        // create a custom rectangle relative to the client area
        rcMyBounds.left = 0;
        rcMyBounds.top = SCALEY(2);
        rcMyBounds.right = rcWindBounds.right - rcWindBounds.left;
        rcMyBounds.bottom = rcWindBounds.bottom - rcWindBounds.top;          
        
        // draw the icon on the screen
		SetBkMode(hDCMem, TRANSPARENT);

		
		if ((configuracion->fondoTransparente || configuracion->fondoEstatico) && configuracion->fondoPantalla != NULL && configuracion->fondoPantalla->hDC != NULL) {
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

		/*
		if (configuracion->fondoPantalla && configuracion->fondoPantalla->hDC) {
			// BitBlt(hDCMem, 0, 0, configuracion->fondoPantalla->anchoImagen, configuracion->fondoPantalla->altoImagen, configuracion->fondoPantalla->hDC, 0, 0, SRCCOPY);

			int posMin = 0;
			int posMax = (listaPantallas->numPantallas - 1) * configuracion->anchoPantalla;
			double posX = -1 * listaPantallas->listaPantalla[0]->x;
			posX = max(posMin, min(posMax, posX));
			posX = posX / ((listaPantallas->numPantallas - 1) * configuracion->anchoPantalla);
			posX = posX * (configuracion->fondoPantalla->anchoImagen - configuracion->anchoPantalla);

			BitBlt(hDCMem, 0, 0, configuracion->anchoPantalla, configuracion->altoPantalla, configuracion->fondoPantalla->hDC, int(posX), 0, SRCCOPY);
		} else if (configuracion->fondoTransparente) {
			BitBlt(hDCMem, 0, 0, estado->fondoPantalla->anchoImagen, estado->fondoPantalla->altoImagen, estado->fondoPantalla->hDC, 0, 0, SRCCOPY);
		} else {
			FillRect(hDCMem, &rcWindBounds, hBrushFondo);
		}*/

        hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
        GetObject(hSysFont, sizeof(LOGFONT), &lf);

		if (configuracion->fontBold) {
			lf.lfWeight = FW_BOLD;
		} else {
			lf.lfWeight = FW_NORMAL;
		}
		lf.lfHeight = -MulDiv(configuracion->fontSize, GetDeviceCaps(hDCMem, LOGPIXELSY), 72);
		lf.lfQuality = DEFAULT_QUALITY;

        // create the font
        hFont = CreateFontIndirect(&lf);
    
        // Select the system font into the device context
        hFontOld = (HFONT) SelectObject(hDCMem, hFont);

        // determine the theme's current text color
        //  this color will change when the user picks a new theme,
        //  so get it each time the display needs to be painted
		crText = RGB(255, 255, 255);

        // set that color
        SetTextColor(hDCMem, crText);
		
        // draw the storage item text
        rcMyBounds.left = rcMyBounds.left + SCALEX(28);
        //DrawText(hDC, TEXT("Storage:"), -1, &rcMyBounds, DT_LEFT);
        
        // draw the program item text
        rcMyBounds.top = SCALEY(0);
        // DrawText(hDC, TEXT("Program:"), -1, &rcMyBounds, DT_LEFT);
		cont++;
		

		// Pintamos los iconos
		pintaIconos(&hDCMem, &rcWindBounds);

        

		if (estado->estadoCuadro == 1 || estado->estadoCuadro == 3) {
			FillRect(hDCMem, &estado->cuadroLanzando, hBrushNegro);
		} else if (estado->estadoCuadro == 2) {
			FillRect(hDCMem, &estado->cuadroLanzando, hBrushNegro);
			if (GetTickCount() - estado->timeUltimoLanzamiento >= 2000) {
				SetTimer(hwnd, TIMER_LANZANDO_APP, configuracion->refreshTime, NULL);
				estado->timeUltimoLanzamiento = GetTickCount();
				estado->estadoCuadro = 3;
			}
		}
		BitBlt(hDC, rcWindBounds.left, rcWindBounds.top, rcWindBounds.right - rcWindBounds.left, rcWindBounds.bottom - rcWindBounds.top, hDCMem, rcWindBounds.left, rcWindBounds.top, SRCCOPY);

		// Select the previous font back into the device context
        DeleteObject(SelectObject(hDCMem, hFontOld));
		DeleteObject(hFont);

        EndPaint(hwnd, &ps);
		
        return 0;
}

LRESULT doMove (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam) 
{
	// Cogemos la posicion del raton
	POINTS posCursor2;
	posCursor2 = MAKEPOINTS(lParam);

	// Comprobamos si se ha superado el umbral para considerar que es movimiento
	if (estado->hayMovimiento || abs(posCursorInicial.x - posCursor2.x) > int(configuracion->umbralMovimiento)) {
		estado->hayMovimiento = true;
		int mov = int((posCursor2.x - posCursor.x)*(1 + float(configuracion->factorMovimiento * (1 + estado->estadoPantalla)) / 10));
		posImage.x += mov;
		setPosiciones(false, mov, 0);
		posCursor = posCursor2;
	}

	// Actualizamos la imagen
	RECT            rcWindBounds; 
	GetClientRect( hwnd, &rcWindBounds); 
	InvalidateRect(hwnd, &rcWindBounds, FALSE);
	// UpdateWindow(hwnd);

	return 0;
}

LRESULT doMouseDown (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	// Cacelamos el timer anterior en caso de haberlo
	KillTimer(hwnd, TIMER_RECUPERACION);
	posCursor = MAKEPOINTS(lParam);
	posCursorInicial = posCursor;



	// -----------------------------
	// -------- Menu PopUp ---------
	// -----------------------------

	// Initialize SHRGINFO structure
	SHRGINFO shrg;

	shrg.cbSize		= sizeof(shrg);
	shrg.hwndClient	= hwnd;
	shrg.ptDown.x	= LOWORD(lParam);
	shrg.ptDown.y	= HIWORD(lParam);
	shrg.dwFlags	= SHRG_RETURNCMD;

	// Create context menu
	if (SHRecognizeGesture(&shrg) == GN_CONTEXTMENU)
	{
		HMENU hmenu = CreatePopupMenu();
		
		if (hmenu==NULL)
			return 0;

		procesaPulsacion(hwnd, posCursor, FALSE, TRUE);

		long timeActual = GetTickCount();

		// Add menu
		AppendMenu(hmenu, MF_STRING, MENU_POPUP_ADD, TEXT("Add Icon"));
		if (moveIconoActivo.nIconoActual >= 0 && timeActual - moveTimeUltimaSeleccion < 12000) {
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_MOVE_HERE, TEXT("Move Here"));
		}
		if (iconoActual.nIconoActual >= 0) {
			if (moveIconoActivo.nIconoActual == -1 || timeActual - moveTimeUltimaSeleccion >= 12000) {
				AppendMenu(hmenu, MF_STRING, MENU_POPUP_MOVE, TEXT("Move Icon"));
			}
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_EDIT, TEXT("Edit Icon"));
			AppendMenu(hmenu, MF_STRING, MENU_POPUP_DELETE, TEXT("Delete Icon"));
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
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		//POINT pt={LOWORD(lParam), HIWORD(lParam)};
		ClientToScreen(hwnd, &pt);

		INT iMenuID = TrackPopupMenuEx(hmenu, TPM_RETURNCMD, pt.x, pt.y, hwnd, NULL);

		switch (iMenuID)
		{
			case MENU_POPUP_ADD:
				iconoActual.nPantallaActual = estado->pantallaActiva;
				resizeWindow(hwnd, false);
				editaIcono();
				break;
			case MENU_POPUP_EDIT:
				resizeWindow(hwnd, false);
				editaIcono();
				break;
			case MENU_POPUP_OPTIONS:
				resizeWindow(hwnd, false);
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOGDUMMY), NULL, (DLGPROC)CustomItemOptionsDlgProc);
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

	return 0;
}

LRESULT doMouseUp (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam)
{
	POINTS point;
	point = MAKEPOINTS(lParam);
	bool doubleClick = false;

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

	// wsprintf(str, TEXT("%i, %i, %i, %i"), posReferencia.x, posImage.x, distanciaIconosH, anchoPantalla);
	// MessageBox(NULL, TEXT("HOLA"), TEXT("StorageCheck Today Item:"), MB_OK);

	if (estado->hayMovimiento) {
		doMove (hwnd, uimessage, wParam, lParam);

		// Calculamos la posicion objetivo
		calculaPosicionObjetivo();

		// Activamos el timer
		SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
	} else {
		procesaPulsacion(hwnd, point, doubleClick);
		// LaunchApplication(TEXT("\\Windows\\calendar.exe"));
	}

	return 0;
}

LRESULT doActivate (HWND hwnd, UINT uimessage, WPARAM wParam, LPARAM lParam) 
{
	if (wParam == WA_CLICKACTIVE || wParam == WA_ACTIVE)
	{
		resizeWindow(hwnd, true);

		// Hide SIP
		SipShowIM(SIPF_OFF);
		ShowWindow(FindWindow(L"MS_SIPBUTTON", NULL), SW_HIDE);
	}
	// The window is being deactivated... restore it to non-fullscreen
	else if (!::IsChild(hwnd, (HWND)lParam))
	{
		resizeWindow(hwnd, false);
		// SetWindowPos(hwnd, NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER);
	}

	return DefWindowProc (hwnd, uimessage, wParam, lParam);
}

/*************************************************************************/
/* Create and register our window class for the today item                 */
/*************************************************************************/
INT InitializeClasses()
{
    WNDCLASS         wc; 
    memset(&wc, 0, sizeof(wc));
    
    wc.style         = 0;                   
    wc.lpfnWndProc     = (WNDPROC) WndProc;
    wc.hInstance     = g_hInst;
    wc.hIcon         = 0;
    wc.hCursor         = 0;
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = (LPCTSTR)LoadString(g_hInst, IDS_TODAY_STORAGE_APPNAME,0,0);
    
    //register our window
    if(!RegisterClass(&wc))
    { 
        return 0 ;
    }
    return 1;
}

void pintaIconos(HDC *hDC, RECT *rcWindBounds) {
	UINT i = 0;
	UINT j = 0;
	
	// Prepares structure for AlphaBlending  
    BLENDFUNCTION bf;  
    bf.BlendOp = AC_SRC_OVER;  
    bf.BlendFlags = 0;  
    bf.SourceConstantAlpha = 60;  
    bf.AlphaFormat = 0;  

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
	int anchoCirculo = int(configuracion->anchoIcono * 0.15);
	int distanciaCirculo = int(anchoCirculo * 0.50);
	int nCirculos = listaPantallas->numPantallas;
	int xReferencia;
	

	if (nCirculos > 1) {
		xReferencia = int((configuracion->anchoPantalla / 2) - ((nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo) / 2);

		if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
			posCirculos.top = int(configuracion->altoPantalla - configuracion->distanciaIconosV - anchoCirculo);
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
	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
		pintaPantalla(hDC, listaPantallas->barraInferior, TRUE);
	}
}

void pintaIcono(HDC *hDC, CIcono *icono) {
	// Iniciamos el proceso de pintado
	HDC hdcIconos;
	RECT posTexto;
	hdcIconos = CreateCompatibleDC(*hDC);

	TransparentBlt(*hDC, int(icono->x), int(icono->y), configuracion->anchoIcono, configuracion->anchoIcono, 
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

				// Pintamos el dia de la semana
				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + configuracion->anchoIcono);
				posTexto.top = int(icono->y);
				posTexto.bottom = int(icono->y + (configuracion->anchoIcono * 0.28));

				DrawText(*hDC, configuracion->diasSemana[st.wDayOfWeek], -1, &posTexto, DT_CENTER | DT_VCENTER);

			
				// Pintamos el dia del mes
				TCHAR diaDelMes[16];
				LOGFONT lf;
				HFONT hFont;
				HFONT hFontOld;
				COLORREF colorOld;
				HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
				GetObject(hSysFont, sizeof(LOGFONT), &lf);

				lf.lfWeight = FW_EXTRABOLD;
				// lf.lfItalic = FALSE;
				// Calculate the font size, making sure to round the result to the nearest integer
				// lf.lfHeight = (long) -(configuracion->anchoIcono * 0.60);
				
				lf.lfHeight = -MulDiv(18, GetDeviceCaps(*hDC, LOGPIXELSY), 72);
				lf.lfQuality = DEFAULT_QUALITY;

				// create the font
				hFont = CreateFontIndirect(&lf);
    
				// Select the system font into the device context
				hFontOld = (HFONT) SelectObject(*hDC, hFont);

				// set that color
				colorOld = SetTextColor(*hDC, RGB(30,30,30));
		
				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + configuracion->anchoIcono);
				posTexto.top = int(icono->y + (configuracion->anchoIcono * 0.25));
				posTexto.bottom = int(icono->y + configuracion->anchoIcono);

				wsprintf(diaDelMes, TEXT("%i"), st.wDay);

				DrawText(*hDC, diaDelMes, -1, &posTexto, DT_CENTER | DT_VCENTER);
        
				// Select the previous font back into the device context
				DeleteObject(SelectObject(*hDC, hFontOld));
				SetTextColor(*hDC, colorOld);
		        DeleteObject(hFont);

				}
				break;
			case NOTIF_CLOCK_ALARM:
				numNotif = estado->estadoAlarm;
			case NOTIF_CLOCK:
				{

				SYSTEMTIME st;
				GetLocalTime(&st);
			
				// Pintamos el dia del mes
				TCHAR strTime[8];
				LOGFONT lf;
				HFONT hFont;
				HFONT hFontOld;
				COLORREF colorOld;
				HFONT hSysFont = (HFONT) GetStockObject(SYSTEM_FONT);
				GetObject(hSysFont, sizeof(LOGFONT), &lf);

				lf.lfWeight = FW_HEAVY;
				
				lf.lfWidth = MulDiv(configuracion->clockWidth, GetDeviceCaps(*hDC, LOGPIXELSX), 144);
				lf.lfHeight = -MulDiv(configuracion->clockHeight, GetDeviceCaps(*hDC, LOGPIXELSY), 72);
				lf.lfQuality = DEFAULT_QUALITY;

				// create the font
				hFont = CreateFontIndirect(&lf);
    
				// Select the system font into the device context
				hFontOld = (HFONT) SelectObject(*hDC, hFont);

				// set that color
				// colorOld = SetTextColor(*hDC, RGB(30,30,30));
				colorOld = SetTextColor(*hDC, configuracion->clockColor);
		
				posTexto.left = int(icono->x);
				posTexto.right = int(icono->x + configuracion->anchoIcono);
				posTexto.top = int(icono->y);
				posTexto.bottom = int(icono->y + configuracion->anchoIcono);

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
		        DeleteObject(hFont);

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
					posTexto.left = int(icono->x + (configuracion->anchoIcono * 0.44));
					posTexto.top = int(icono->y - (configuracion->anchoIcono * 0.28));
					posTexto.right = int(posTexto.left + configuracion->anchoIcono * 0.80);
					posTexto.bottom = int(posTexto.top + configuracion->anchoIcono * 0.80);

					break;
				case NOTIF_CLOCK_ALARM:
					wcscpy(notif, TEXT(""));
					
					/* Bubble with same size that icon and text in top right*/
					posTexto.left = int(icono->x + (configuracion->anchoIcono * 0.44));
					posTexto.top = int(icono->y - (configuracion->anchoIcono * 0.36));
					posTexto.right = int(posTexto.left + configuracion->anchoIcono * 0.80);
					posTexto.bottom = int(posTexto.top + configuracion->anchoIcono * 0.80);

					break;
				case NOTIF_ALARM:
					wcscpy(notif, TEXT(""));
					
					/* Bubble with same size that icon and text in top right*/
					posTexto.left = int(icono->x + (configuracion->anchoIcono * 0.44));
					posTexto.top = int(icono->y - (configuracion->anchoIcono * 0.28));
					posTexto.right = int(posTexto.left + configuracion->anchoIcono * 0.80);
					posTexto.bottom = int(posTexto.top + configuracion->anchoIcono * 0.80);

					break;
				case NOTIF_WIFI:
					wcscpy(notif, TEXT("On"));

					/* Centrado grande */
					posTexto.left = int(icono->x);
					posTexto.top = int(icono->y + (configuracion->anchoIcono * 0.50));
					posTexto.right = int(icono->x + (configuracion->anchoIcono));
					posTexto.bottom = int(icono->y + (configuracion->anchoIcono));

					break;
				case NOTIF_BLUETOOTH:
					if (numNotif == 2) {
						wcscpy(notif, TEXT("Disc"));
					} else {
						wcscpy(notif, TEXT("On"));
					}

					/* Centrado grande */
					/*
					posTexto.left = int(icono->x + (configuracion->anchoIcono * 0.15));
					posTexto.top = int(icono->y + (configuracion->anchoIcono * 0.60));
					posTexto.right = int(icono->x + (configuracion->anchoIcono * 0.85));
					posTexto.bottom = int(icono->y + (configuracion->anchoIcono * 0.95));
					*/
					posTexto.left = int(icono->x);
					posTexto.top = int(icono->y + (configuracion->anchoIcono * 0.50));
					posTexto.right = int(icono->x + (configuracion->anchoIcono));
					posTexto.bottom = int(icono->y + (configuracion->anchoIcono));

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
						// drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(200, 0, 0), notif);	
					}
					break;

				case NOTIF_CLOCK_ALARM:
				case NOTIF_ALARM:
					// Pintamos la notificacion
					if (configuracion->bubbleAlarm->hDC) {
						drawNotification(*hDC, &posTexto, configuracion->bubbleAlarm, notif);
					} else {
						// drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(200, 0, 0), notif);	
					}
					break;

				case NOTIF_WIFI:
				case NOTIF_BLUETOOTH:
					// Pintamos la notificacion
					if (configuracion->bubbleState->hDC) {
						drawNotification(*hDC, &posTexto, configuracion->bubbleState, notif);
					} else {
						// drawEllipse(*hDC, posTexto.left, posTexto.top, posTexto.right, posTexto.bottom, RGB(0, 200, 0), notif);	
					}
					break;
			}
		}

	}

	// Pintamos el nombre del icono
	posTexto.top = int(icono->y + (configuracion->anchoIcono * 1.02));
	posTexto.bottom = int(icono->y + (configuracion->anchoIcono * 1.40));
	posTexto.left = int(icono->x - (configuracion->distanciaIconosH * 0.5));
	posTexto.right = int(icono->x + configuracion->anchoIcono + (configuracion->distanciaIconosH * 0.5));

	DrawText(*hDC, icono->nombre, -1, &posTexto, DT_CENTER | DT_TOP);

	DeleteDC(hdcIconos);
}

void pintaPantalla(HDC *hDC, CPantalla *pantalla, BOOL esBarraInferior) {
	// Si debemos recalcular la pantalla
	if (pantalla->debeActualizar) {
		pantalla->debeActualizar = FALSE;
		if (pantalla->hDC == NULL) {
			pantalla->hDC = CreateCompatibleDC(*hDC);
			pantalla->imagen = CreateCompatibleBitmap(*hDC, pantalla->anchoPantalla, pantalla->altoPantalla);

			pantalla->imagenOld = (HBITMAP)SelectObject(pantalla->hDC, pantalla->imagen);
			pantalla->hFontOld = (HFONT)SelectObject(pantalla->hDC, hFont);
			SetTextColor(pantalla->hDC, crText);
		}
		SetBkMode(pantalla->hDC, TRANSPARENT);
		// SetBkColor(pantalla->hDC, RGB(0,0,0));

		RECT rc = {0};
		rc.right = pantalla->anchoPantalla;
		rc.bottom = pantalla->altoPantalla;

		FillRect(pantalla->hDC, &rc, hBrushFondo);

		if (esBarraInferior) {
			// DrawGradientGDI(pantalla->hDC,rc,RGB(50,50,50), RGB(150,150,150), 0xAAAA);
		}

		setPosicionesIconos(pantalla, esBarraInferior);

		if (pantalla->fondoPantalla) {
			BitBlt(pantalla->hDC, 0, 0, pantalla->anchoPantalla, pantalla->altoPantalla, 
				pantalla->fondoPantalla->hDC, 0, 0, SRCCOPY);
		}
		
		CIcono *icono = NULL;
		UINT j = 0;
		while (j < pantalla->numIconos) {
			icono = pantalla->listaIconos[j];

			pintaIcono(&pantalla->hDC, icono);

			j++;
		}
		// pantalla->crearMascara();
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

		if (configuracion->fondoTransparente || configuracion->fondoPantalla != NULL) {
			TransparentBlt(*hDC, int(pantalla->x), int(pantalla->y), pantalla->anchoPantalla, pantalla->altoPantalla, 
				pantalla->hDC, 0, 0, pantalla->anchoPantalla, pantalla->altoPantalla, estado->colorFondo);
		} else {
			BitBlt(*hDC, int(pantalla->x), int(pantalla->y), pantalla->anchoPantalla, pantalla->altoPantalla, 
				pantalla->hDC, 0, 0, SRCCOPY);
		}
		/*BitBlt(*hDC, int(pantalla->x), int(pantalla->y), pantalla->anchoPantalla, pantalla->altoPantalla, 
				pantalla->hDC, 0, 0, SRCCOPY);*/

	}
}

void setPosicionesIconos(CPantalla *pantalla, BOOL esBarraInferior) {
	UINT i = 0;
	CIcono *icono = NULL;
	// int offsetX = 0;
	int distanciaIconos = configuracion->distanciaIconosH;
	int posReferenciaX = configuracion->posReferencia.x;

	if (esBarraInferior && pantalla->numIconos > 0) {
		int espacioLibre = pantalla->anchoPantalla - pantalla->numIconos * configuracion->anchoIcono;
		distanciaIconos = espacioLibre / (pantalla->numIconos + 1);
		posReferenciaX = distanciaIconos;
		
		distanciaIconos += configuracion->anchoIcono;

		/*
		offsetX = (pantalla->anchoPantalla - (((pantalla->numIconos - 1) * configuracion->distanciaIconosH) + configuracion->anchoIcono)) / 2;
		offsetX -= configuracion->posReferencia.x;
		offsetX = max(offsetX, 0);*/
	}

	i = 0;
	while (i < pantalla->numIconos) {

		icono = pantalla->listaIconos[i];

		icono->x = posReferenciaX + float(int(i % configuracion->numeroIconos)*distanciaIconos);
		icono->y = configuracion->posReferencia.y + float(int((i / configuracion->numeroIconos))*configuracion->distanciaIconosV);
	
		i++;
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

		/*if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
			pantalla->altoPantalla = configuracion->altoPantalla - configuracion->distanciaIconosV;
			pantalla->anchoPantalla = configuracion->anchoPantalla;
		} else {
			pantalla->altoPantalla = configuracion->altoPantalla;
			pantalla->anchoPantalla = configuracion->anchoPantalla;
		}*/
		pantalla->altoPantalla = configuracion->altoPantalla;
		pantalla->anchoPantalla = configuracion->anchoPantalla;

		i++;
	}

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
		listaPantallas->barraInferior->x = 0;
		listaPantallas->barraInferior->y = float(configuracion->altoPantalla - configuracion->distanciaIconosV);
		listaPantallas->barraInferior->altoPantalla = configuracion->distanciaIconosV;
		listaPantallas->barraInferior->anchoPantalla = configuracion->anchoPantalla;
	}
}

void calculaPosicionObjetivo() {
	CPantalla *pantalla = NULL;
	CIcono *icono = NULL;
	UINT distanciaMinima = 0xFFFF;
	UINT distAux;
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
	estado->posObjetivo.y = 0;
}

/*************************************************************************/
/* Initialize the DLL by creating a new window                             */
/*************************************************************************/
HWND InitializeCustomItem(TODAYLISTITEM *ptli, HWND hwndParent) 
{
	// WriteToLog(TEXT("InitializeCustomItem\r\n"));	
    LPCTSTR appName = (LPCTSTR)LoadString(g_hInst,IDS_TODAY_STORAGE_APPNAME,0,0);

	//create a new window
	g_hWnd = CreateWindow(appName,appName, WS_VISIBLE | WS_CHILD, 
		CW_USEDEFAULT,CW_USEDEFAULT,GetSystemMetrics(SM_CXSCREEN), 0, hwndParent, NULL, g_hInst, 0);
	
	// attach our winproc to the newly created window
	SetWindowLong(g_hWnd, GWL_WNDPROC, (LONG) WndProc);

	//display the window
	ShowWindow (g_hWnd, SW_SHOWNORMAL);
	UpdateWindow (g_hWnd) ;
    
    return g_hWnd;
}

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
	// Launch de application
	BOOL bWorked;
	SHELLEXECUTEINFO sei;
	
	memset(&sei,0,sizeof(sei));
	sei.cbSize=sizeof(sei);
	
	sei.lpFile=pCmdLine;
	sei.nShow=SW_SHOWNORMAL;
	sei.nShow=SW_SHOWNORMAL;
	sei.lpParameters=pParametros;

	// TCHAR str[256];
	// wsprintf(str, TEXT("%i, %i - %i, %i"), posCursor.x, posCursor.y, int(icono->x), int(icono->y));
	// MessageBox(NULL, pCmdLine, TEXT("Probando..."), MB_OK);
	
	bWorked = ShellExecuteEx(&sei);
	
#ifdef EXEC_MODE
	if (bWorked && configuracion->closeOnLaunchIcon == 1) {
		PostQuitMessage(0);
	}
#endif

	return bWorked;
}

void procesaPulsacion(HWND hwnd, POINTS posCursor, BOOL doubleClick, BOOL noLanzar) {

	BOOL backFirstPage = FALSE;
	if (doubleClick) {
		int nCirculos = listaPantallas->numPantallas;
		int anchoCirculo = int(configuracion->anchoIcono * 0.15);
		int distanciaCirculo = int(anchoCirculo * 0.50);
		int xLeft, xRight, yTop, yBottom;

		// Calculamos el cuadrado recubridor de la barra de circulos activos
		xLeft = int((configuracion->anchoPantalla / 2) - ((nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo) / 2);
		xRight = xLeft + (nCirculos - 1) * (anchoCirculo + distanciaCirculo) + anchoCirculo;

		if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
			yTop = int(configuracion->altoPantalla - configuracion->distanciaIconosV - anchoCirculo);
		} else {
			yTop = int(configuracion->altoPantalla - anchoCirculo - distanciaCirculo);
		}
		yBottom = yTop + anchoCirculo;

		// Expandimos ligerammente dicho cuadro para facilitar el doble click
		xLeft -= int(anchoCirculo * 1.80);
		xRight += int(anchoCirculo * 1.50);
		yTop -= int(anchoCirculo * 1.40);
		yBottom += int(anchoCirculo);

		if (posCursor.x >= xLeft && posCursor.x <= xRight &&
			posCursor.y >= yTop && posCursor.y <= yBottom) {

			backFirstPage = TRUE;
		}
	}

	if (backFirstPage) {
		estado->pantallaActiva = 0;
		estado->posObjetivo.x = 0;
		estado->posObjetivo.y = 0;

		// Activamos el timer
		SetTimer(hwnd, TIMER_RECUPERACION, configuracion->refreshTime, NULL);
	} else {
		CPantalla *pantalla = listaPantallas->listaPantalla[estado->pantallaActiva];
		CIcono *icono = NULL;
		BOOL enc = false;
		int i = 0;
		int nIconos = pantalla->numIconos;

		iconoActual.nPantallaActual = estado->pantallaActiva;
		iconoActual.nIconoActual = -1;

		while (i < nIconos && !enc) {
			icono = pantalla->listaIconos[i];
			if (posCursor.x >= pantalla->x + icono->x && posCursor.x <= pantalla->x + icono->x + configuracion->anchoIcono && 
				posCursor.y >= pantalla->y + icono->y && posCursor.y <= pantalla->y + icono->y + configuracion->anchoIcono) {
					enc = true;
					iconoActual.nIconoActual = i;
			}

			i++;
		}

		if (!enc && listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
			i = 0;
			pantalla = listaPantallas->barraInferior;
			nIconos = pantalla->numIconos;
			while (i < nIconos && !enc) {
				icono = pantalla->listaIconos[i];
				if (posCursor.x >= pantalla->x + icono->x && posCursor.x <= pantalla->x + icono->x + configuracion->anchoIcono && 
					posCursor.y >= pantalla->y + icono->y && posCursor.y <= pantalla->y + icono->y + configuracion->anchoIcono) {
						enc = true;
						iconoActual.nPantallaActual = -1;
						iconoActual.nIconoActual = i;
				}

				i++;
			}

			if (!enc && posCursor.x >= pantalla->x && posCursor.x <= pantalla->x + pantalla->anchoPantalla
				&& posCursor.y >= pantalla->y && posCursor.y <= pantalla->y + pantalla->altoPantalla) {
					enc = true;
					iconoActual.nPantallaActual = -1;
			}
		}
		
		if (!noLanzar && enc && iconoActual.nIconoActual >= 0) {
			// Vibration
			if (configuracion->vibrateOnLaunchIcon > 0) {
				vibrate(configuracion->vibrateOnLaunchIcon);
			}

			// Activamos el timer
			if (icono->launchAnimation > 0) {
				SetTimer(hwnd, TIMER_LANZANDO_APP, configuracion->refreshTime, NULL);
				estado->timeUltimoLanzamiento = GetTickCount();

				estado->iconoActivo = icono;
			} else {
				// MessageBox(0, icono->ejecutable, 0, MB_OK);
				if (hayNotificacion(icono->tipo) > 0 && _tcsclen(icono->ejecutableAlt) > 0) {
					LaunchApplication(icono->ejecutableAlt, icono->parametrosAlt);
				} else if (_tcsclen(icono->ejecutable) > 0) {
					LaunchApplication(icono->ejecutable, icono->parametros);
				}
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

BOOL editaIcono() {
	
	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MENU_ICON), NULL, (DLGPROC)editaIconoDlgProc);

	return true;
}

LRESULT CALLBACK editaIconoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
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

				SetWindowText(GetDlgItem(hDlg, IDC_MICON_NAME), icono->nombre);
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_IMAGE), icono->rutaImagen);
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_EXEC), icono->ejecutable);
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_PARAMETERS), icono->parametros);
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_EXECALT), icono->ejecutableAlt);
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_PARAMETERSALT), icono->parametrosAlt);

				if (icono->launchAnimation > 0) {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, BST_CHECKED, 0);
				} else {
					SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_SETCHECK, BST_UNCHECKED, 0);
				}

				if (icono->tipo == NOTIF_NORMAL) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_NORMAL_TXT);
				} else if (icono->tipo == NOTIF_LLAMADAS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_LLAMADAS_TXT);
				} else if (icono->tipo == NOTIF_SMS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_SMS_TXT);
				} else if (icono->tipo == NOTIF_MMS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_MMS_TXT);
				} else if (icono->tipo == NOTIF_OTHER_EMAIL) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_OTHER_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_SYNC_EMAIL) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_SYNC_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_TOTAL_EMAIL) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_TOTAL_EMAIL_TXT);
				} else if (icono->tipo == NOTIF_CITAS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_CITAS_TXT);
				} else if (icono->tipo == NOTIF_CALENDAR) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_CALENDAR_TXT);
				} else if (icono->tipo == NOTIF_TAREAS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_TAREAS_TXT);
				} else if (icono->tipo == NOTIF_SMS_MMS) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_SMS_MMS_TXT);
				} else if (icono->tipo == NOTIF_WIFI) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_WIFI_TXT);
				} else if (icono->tipo == NOTIF_BLUETOOTH) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_BLUETOOTH_TXT);
				} else if (icono->tipo == NOTIF_ALARM) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_ALARM_TXT);
				} else if (icono->tipo == NOTIF_CLOCK) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_CLOCK_TXT);
				} else if (icono->tipo == NOTIF_CLOCK_ALARM) {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_CLOCK_ALARM_TXT);
				} else {
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_NORMAL_TXT);
				}
			} else {
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), NOTIF_NORMAL_TXT);
			}
        }
        return TRUE; 
        
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) 
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
			TCHAR szTemp[MAX_PATH];
			int nScreen, nIcon, nType;
			TCHAR strName[MAX_PATH];
			TCHAR strImage[MAX_PATH];
			TCHAR strType[MAX_PATH];
			TCHAR strExec[MAX_PATH];
			TCHAR strParameters[MAX_PATH];
			TCHAR strExecAlt[MAX_PATH];
			TCHAR strParametersAlt[MAX_PATH];
			UINT launchAnimation = 0;

			GetWindowText(GetDlgItem(hDlg, IDC_MICON_SCREEN), szTemp, 4);
			nScreen = _wtoi(szTemp);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_ICON), szTemp, 4);
			nIcon = _wtoi(szTemp);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_NAME), strName, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_IMAGE), strImage, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_TYPE), strType, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_EXEC), strExec, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_PARAMETERS), strParameters, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_EXECALT), strExecAlt, MAX_PATH);
			GetWindowText(GetDlgItem(hDlg, IDC_MICON_PARAMETERSALT), strParametersAlt, MAX_PATH);
			if (SendMessage(GetDlgItem(hDlg, IDC_MICON_LAUNCHANIMATION), BM_GETCHECK, 0, 0) == BST_CHECKED) {
				launchAnimation = 1;
			}
			

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
			if (lstrcmp(strType, NOTIF_NORMAL_TXT) == 0) {
				nType = NOTIF_NORMAL;
			} else if (lstrcmp(strType, NOTIF_LLAMADAS_TXT) == 0) {
				nType = NOTIF_LLAMADAS;
			} else if (lstrcmp(strType, NOTIF_SMS_TXT) == 0) {
				nType = NOTIF_SMS;
			} else if (lstrcmp(strType, NOTIF_MMS_TXT) == 0) {
				nType = NOTIF_MMS;
			} else if (lstrcmp(strType, NOTIF_OTHER_EMAIL_TXT) == 0) {
				nType = NOTIF_OTHER_EMAIL;
			} else if (lstrcmp(strType, NOTIF_SYNC_EMAIL_TXT) == 0) {
				nType = NOTIF_SYNC_EMAIL;
			} else if (lstrcmp(strType, NOTIF_TOTAL_EMAIL_TXT) == 0) {
				nType = NOTIF_TOTAL_EMAIL;
			} else if (lstrcmp(strType, NOTIF_CITAS_TXT) == 0) {
				nType = NOTIF_CITAS;
			} else if (lstrcmp(strType, NOTIF_CALENDAR_TXT) == 0) {
				nType = NOTIF_CALENDAR;
			} else if (lstrcmp(strType, NOTIF_TAREAS_TXT) == 0) {
				nType = NOTIF_TAREAS;
			} else if (lstrcmp(strType, NOTIF_SMS_MMS_TXT) == 0) {
				nType = NOTIF_SMS_MMS;
			} else if (lstrcmp(strType, NOTIF_WIFI_TXT) == 0) {
				nType = NOTIF_WIFI;
			} else if (lstrcmp(strType, NOTIF_BLUETOOTH_TXT) == 0) {
				nType = NOTIF_BLUETOOTH;
			} else if (lstrcmp(strType, NOTIF_ALARM_TXT) == 0) {
				nType = NOTIF_ALARM;
			} else if (lstrcmp(strType, NOTIF_CLOCK_TXT) == 0) {
				nType = NOTIF_CLOCK;
			} else if (lstrcmp(strType, NOTIF_CLOCK_ALARM_TXT) == 0) {
				nType = NOTIF_CLOCK_ALARM;
			} else {
				MessageBox(hDlg, TEXT("Type not valid!"), TEXT("Error"), MB_OK);
				return FALSE;
			}

			// All Ok, Icon Creation
			CPantalla *pantalla = NULL;
			CIcono *icono = NULL;
			// BITMAP bm;
			TCHAR rutaImgCompleta[MAX_PATH];

			if (newScreen) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				if (nScreen == -1) {
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

			if (CountOf(icono->rutaImagen) > 0) {
				if (icono->rutaImagen[0] == TEXT('\\')) {
					StringCchCopy(rutaImgCompleta, CountOf(rutaImgCompleta), icono->rutaImagen);
				} else {
					StringCchPrintf(rutaImgCompleta, CountOf(rutaImgCompleta), TEXT("%sicons\\%s"), configuracion->rutaInstalacion, icono->rutaImagen);
				}

				icono->clearImageObjects();
				// icono->loadImage(rutaImgCompleta, configuracion->anchoIcono, configuracion->anchoIcono);
				if (_tcslen(icono->rutaImagen) > 0) {
					icono->loadImage(&hDCMem, rutaImgCompleta, configuracion->anchoIcono, configuracion->anchoIcono);
				} else if (_tcslen(icono->ejecutable) > 0) {
					icono->loadImageFromExec(&hDCMem, icono->ejecutable, configuracion->anchoIcono, configuracion->anchoIcono);
				}

				/*
				GetObject(icono->imagen, sizeof(BITMAP), &bm);
				icono->anchoImagen = bm.bmWidth;
				icono->altoImagen = bm.bmHeight;*/
			}

			setPosiciones(true, 0, 0);

			configuracion->guardaXMLIconos(listaPantallas);

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

				SetWindowText(GetDlgItem(hDlg, IDC_MICON_IMAGE), pathFile);
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
					SetWindowText(GetDlgItem(hDlg, IDC_MICON_NAME), cbFileInfo.szDisplayName);
				}
				SetWindowText(GetDlgItem(hDlg, IDC_MICON_EXEC), pathFile);
			}
		} else if (LOWORD(wParam) == IDC_MICON_EXECALT_B) {
			TCHAR pathFile[MAX_PATH];
			if (openFileBrowse(hDlg, OFN_EXFLAG_DETAILSVIEW, pathFile, lastPathExec)) {

				// Extract Path for save lastPath
				getPathFromFile(pathFile, lastPathExec);

				SetWindowText(GetDlgItem(hDlg, IDC_MICON_EXECALT), pathFile);
			}
		}
        break;
	/*
	case WM_VSCROLL:
		{
			// Get the current scroll bar position
			SCROLLINFO si = {0};
			si.cbSize = sizeof (si);
			si.fMask = SIF_ALL;
			GetScrollInfo (hDlg, SB_VERT, &si);

			// Save the position for comparison later on
			int currentPos = si.nPos;

			// Adjust the scrollbar position based upon
			// the action the user took
			switch (LOWORD (wParam))
			{
				// user clicked the HOME keyboard key
				case SB_TOP:
					si.nPos = si.nMin;
					break;
				// user clicked the END keyboard key
				case SB_BOTTOM:
					si.nPos = si.nMax;
					break;

				// user clicked the top arrow
				case SB_LINEUP:
					si.nPos -= 1;
					break;

				// user clicked the bottom arrow
				case SB_LINEDOWN:
					si.nPos += 1;
					break;

				// user clicked the scroll bar shaft above the scroll box
				case SB_PAGEUP:
					si.nPos -= si.nPage;
					break;

				// user clicked the scroll bar shaft below the scroll box
				case SB_PAGEDOWN:
					si.nPos += si.nPage;
					break;

				// user dragged the scroll box
				case SB_THUMBTRACK:
					si.nPos = si.nTrackPos;
					break;
			}

			// Set the position and then retrieve it. Due to adjustments
			// by Windows it may not be the same as the value set.
			si.fMask = SIF_POS;
			SetScrollInfo (hDlg, SB_VERT, &si, TRUE);
			GetScrollInfo (hDlg, SB_VERT, &si);

			// If the position has changed
			if (si.nPos != currentPos)
			{ 
				// Scroll the window contents
				ScrollWindowEx(hDlg, 0, currentPos - si.nPos,
					NULL, NULL, NULL, NULL,
					SW_SCROLLCHILDREN | SW_INVALIDATE);
			}
		}
		break;
	*/
    case WM_DESTROY:
        break;

    }

    return FALSE;
}

BOOL cargaFondoPantalla(HWND hwnd) {	

	if (!configuracion || !configuracion->fondoTransparente) {
		return FALSE;
	}
	
	if (configuracion->fondoPantalla == NULL) {
		configuracion->fondoPantalla = new CIcono();
	}

	
	RECT rc;
	GetClientRect(hwnd, &rc);
	if (configuracion->fondoPantalla->hDC == NULL) {		
		HDC hdc = GetDC(hwnd);
		configuracion->fondoPantalla->hDC = CreateCompatibleDC(hdc);
		configuracion->fondoPantalla->imagen = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
		configuracion->fondoPantalla->imagenOld = (HBITMAP)SelectObject(configuracion->fondoPantalla->hDC, configuracion->fondoPantalla->imagen);

		ReleaseDC(hwnd, hdc);
	}

	TODAYDRAWWATERMARKINFO	dwi;
	dwi.hdc = configuracion->fondoPantalla->hDC;
	dwi.hwnd = hwnd;
	dwi.rc = rc;
	SendMessage(GetParent(hwnd), TODAYM_DRAWWATERMARK, 0, (LPARAM)&dwi);

	BITMAP bm;
	GetObject(configuracion->fondoPantalla->imagen, sizeof(BITMAP), &bm);
	configuracion->fondoPantalla->anchoImagen = bm.bmWidth;
	configuracion->fondoPantalla->altoImagen = bm.bmHeight;
	
	return TRUE;
}

BOOL inicializaApp(HWND hwnd) {

	configuracion = new CConfiguracion();
	estado = new CEstado();

	HDC hdc = GetDC(hwnd);

#ifdef EXEC_MODE
	GetClientRect( hwnd, &configuracion->dimensionesPantalla);
#else
	// Resize the main window to the size of the screen.
    SetRect(&configuracion->dimensionesPantalla, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

	resizeWindow(hwnd, true);

	// Debe ser inicializado en modo normal
	estado->estadoCuadro = 0;

	// Cargamos los parametros de configuracion
	configuracion->cargaXMLConfig();
	
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
	configuracion->cargaIconos(&hdc, listaPantallas);

	// Cargamos la configuracion calculada en funcion de los iconos
	int maxIconos = 0;
	for (int i = 0; i < (int)listaPantallas->numPantallas; i++) {
		maxIconos = max(maxIconos, (int)listaPantallas->listaPantalla[i]->numIconos);
	}
	configuracion->calculaConfiguracion(
		maxIconos,
		(listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0),
		configuracion->dimensionesPantalla.right,
		configuracion->dimensionesPantalla.bottom);

	configuracion->cargaImagenes(&hdc);

	if (configuracion->fondoTransparente || _tcsclen(configuracion->strFondoPantalla) > 0) {
		// estado->colorFondo = RGB(255, 0, 255);
		estado->colorFondo = RGBA(0, 0, 0, 0);
	} else {
		estado->colorFondo = RGB(0, 0, 0);
	}

	// Establecemos la ruta por defecto para buscar programas
	if (!SHGetSpecialFolderPath(0, lastPathExec, CSIDL_PROGRAMS, FALSE)) {
		wcscpy(lastPathExec, L"");
	}

	ReleaseDC(hwnd, hdc);

	setPosiciones(true, 0, 0); 

	// Get all inputs hardware
	// AllKeys(TRUE);

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

	/* 
	if (estado->fondoPantalla && estado->fondoPantalla->hDC) {
		borraHDC_HBITMPAP(
			&estado->fondoPantalla->hDC,
			&estado->fondoPantalla->imagen,
			&estado->fondoPantalla->imagenOld);
	} */

	if(hBrushFondo) {
		DeleteObject(hBrushFondo);
	}

	if(hBrushNegro) {
		DeleteObject(hBrushNegro);
	}

	return true;
}

BOOL borraHDC_HBITMPAP(HDC *hdc, HBITMAP *hbm, HBITMAP *hbmOld) {
	/*if(*hbitmap) {
		DeleteObject(*hbitmap);
		*hbitmap = NULL;
	}
	if(*hdc) {
		DeleteDC(*hdc);
		*hdc = NULL;
	}*/

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


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// init it
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Realizar la inicialización de la aplicación:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATORS));

	// Bucle principal de mensajes:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	doDestroy(NULL);

	CoUninitialize();

	return (int) msg.wParam;
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: guardar el identificador de instancia y crear la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // HWND hWnd;
    TCHAR szTitle[MAX_LOADSTRING];		// texto de la barra de título
    TCHAR szWindowClass[MAX_LOADSTRING];	// nombre de clase de la ventana principal

    g_hInst = hInstance; // Almacenar identificador de instancia en una variable global

    // SHInitExtraControls se debe llamar una vez durante la inicialización de la aplicación para inicializar cualquiera
    // de los controles específicos del dispositivo como por ejemplo CAPEDIT y SIPPREF.
    SHInitExtraControls();

	LoadString(hInstance, IDS_APPNAME, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDS_APPNAME, szWindowClass, MAX_LOADSTRING);

    //Si ya se está ejecutando, establezca el foco en la ventana y cierre
    g_hWnd = FindWindow(szWindowClass, szTitle);	
    if (g_hWnd) 
    {
        // Establecer el foco en la ventana secundaria en primer lugar
        // "| 0x00000001" se utiliza para traer las ventanas en propiedad a primer plano y
        // activarlas.
        SetForegroundWindow((HWND)((ULONG) g_hWnd | 0x00000001));
        return 0;
    } 

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

	// Create main window.
	g_hWnd = CreateWindowEx (WS_EX_NODRAG,      // Ex Style
		szWindowClass,           // Window class
		szTitle,           // Window title
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

	SHFullScreen(g_hWnd, SHFS_HIDESIPBUTTON);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    return TRUE;
}

//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: registrar la clase de ventana.
//
//  COMENTARIOS:
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
	wc.hIcon       = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

void doDestroy(HWND hwnd) {
	if (hwnd != NULL) {
		KillTimer(hwnd, TIMER_RECUPERACION);
		KillTimer(hwnd, TIMER_ACTUALIZA_NOTIF);
		KillTimer(hwnd, TIMER_LANZANDO_APP);
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

	if(hBrushNegro != NULL) {
		DeleteObject(hBrushNegro);
	}
}

void resizeWindow(HWND hwnd, BOOL fullScreen) 
{
#ifdef EXEC_MODE
	DWORD dwState;
	// RECT rc;

	if (fullScreen) {
		dwState = SHFS_HIDESIPBUTTON;
	} else {
		dwState = (SHFS_SHOWTASKBAR | SHFS_SHOWSTARTICON | SHFS_HIDESIPBUTTON);
	}
	  
	SHFullScreen(hwnd, dwState);

	// Resize the main window to the size of the screen.
	// SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	// SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER);

	if (fullScreen) {
		ShowWindow(hwnd, SW_SHOWNORMAL);
	}
#endif
}

void autoConfigure()
{
	if (configuracion == NULL || configuracion->alreadyConfigured > 0) {
		return;
	}

	// Check if user want autoconfigure
	int resp = MessageBox(NULL, TEXT("Auto configure?"), TEXT("First Run!"), MB_YESNO);
	if (resp == IDYES) {
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);
		int heightBars = 26 * 2 + 1;

		// Width icon
		configuracion->anchoIconoXML = int(float(width) * 0.1875);
		configuracion->anchoIcono = configuracion->anchoIconoXML;

		// QVGA, WQVGA
		if (width < 300) {
			configuracion->altoPantallaP = height - heightBars;
			configuracion->altoPantallaL = width - heightBars;

			configuracion->altoPantalla = configuracion->altoPantallaP;
		} else if (width < 400) { // SQUARE QVGA
			heightBars = 35 * 2 + 1;
			configuracion->altoPantallaP = height - heightBars;
			configuracion->altoPantallaL = width - heightBars;

			configuracion->altoPantalla = configuracion->altoPantallaP;
		} else { // VGA, WVGA
			configuracion->altoPantallaP = height - heightBars * 2;
			configuracion->altoPantallaL = width - heightBars * 2;

			configuracion->altoPantalla = configuracion->altoPantallaP;
		}
	}

	configuracion->alreadyConfigured = 1;
	configuracion->guardaXMLConfig();
}