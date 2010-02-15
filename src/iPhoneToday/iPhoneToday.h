#if !defined(IPHONETODAY_H)
#define IPHONETODAY_H

#include "stdafx.h"
#include "CConfiguracion.h"

#define MAKEPOINTS(l)   (*((POINTS FAR *) & (l)))

//global variables    
extern HINSTANCE           g_hInst;
extern HWND                g_hWndMenuBar; // menu bar handle
extern HWND                g_hWnd;
extern IImagingFactory*    g_pImgFactory;

// Variables 
extern CConfiguracion *configuracion;

#endif // IPHONETODAY_H
