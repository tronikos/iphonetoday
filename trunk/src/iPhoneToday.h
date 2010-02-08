#if !defined(IPHONETODAY_H)
#define IPHONETODAY_H


#include "stdafx.h"
#include "CListaPantalla.h"
#include "CConfiguracion.h"
#include "CEstado.h"
#include "vibrate.h"

#define MAX_LOADSTRING 100

#define INVALID_HEIGHT            0xFFFFFFFF

// flags bits for options - we use negative logic to make the default (0)
// show both
#define FLAGS_HIDE_STORAGE    0x1
#define FLAGS_HIDE_PROGRAM    0x2

#define MAKEPOINTS(l)   (*((POINTS FAR *) & (l)))

//global variables    
extern UINT                g_plugInHeight;
extern HINSTANCE           g_hInst;
extern HWND				g_hWndMenuBar; // menu bar handle
extern HWND                g_hWnd;
extern BOOL                g_bFirstDisplay;
extern HFONT				hFont;
extern COLORREF			crText;


// Variables 
extern CListaPantalla *listaPantallas; // = new CListaPantalla();
extern CConfiguracion *configuracion; // = new CConfiguracion();
extern CEstado *estado; // = new CEstado();

#endif // IPHONETODAY_H