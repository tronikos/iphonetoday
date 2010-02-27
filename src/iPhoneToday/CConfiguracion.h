#pragma once

#include "CConfigurationScreen.h"
#include "CListaPantalla.h"
#include "GraphicFunctions.h"
#include "xmlWrapper.h"

class CConfiguracion
{
public:
	CConfiguracion(void);
	~CConfiguracion(void);

	void calculaConfiguracion(int maxIconos, int numIconsInBottomBar, int numIconsInTopBar, int width, int height);
	BOOL cargaIconos(HDC *hDC, CListaPantalla *listaPantallas);
	BOOL cargaImagenIcono(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type);
	BOOL cargaImagenes(HDC *hDC);
	BOOL cargaFondo(HDC *hDC);

	BOOL cargaXMLIconos(CListaPantalla *listaPantallas);
	void defaultValues();
	BOOL cargaXMLConfig();
	BOOL guardaXMLConfig();
	BOOL guardaXMLIconos(CListaPantalla *listaPantallas);

	BOOL saveXMLScreenIcons(TiXmlElement *pElemScreen, CPantalla *pantalla);

	void getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc);
	BOOL hasTimestampChanged();

	TCHAR pathExecutableDir[MAX_PATH]; // Path of executable's directory
	TCHAR pathSettingsXML[MAX_PATH];   // Path of settings.xml
	TCHAR pathIconsXML[MAX_PATH];      // Path of icons.xml
	TCHAR pathIconsXMLDir[MAX_PATH];   // Path of the directory where icons.xml resides
	TCHAR pathIconsDir[MAX_PATH];      // Path of the icons directory

	FILETIME lastModifiedSettingsXML;
	FILETIME lastModifiedIconsXML;

	UINT altoPantalla;
	UINT altoPantallaMax;
	UINT anchoPantalla;

	CIcono *bubbleNotif;
	CIcono *bubbleState;
	CIcono *bubbleAlarm;

	CIcono *fondoPantalla;


	// Variables from XML

	// Screens
	CConfigurationScreen *mainScreenConfig;
	CConfigurationScreen *bottomBarConfig;
	CConfigurationScreen *topBarConfig;

	// Circles
	UINT circlesDiameter;
	UINT circlesDistance;
	INT circlesOffset;

	// Header
	UINT headerFontSize;
	UINT headerFontWeight;
	UINT headerFontColor;
	UINT headerOffset;

	// Background
	UINT fondoTransparente;
	COLORREF fondoColor;
	UINT fondoEstatico;
	TCHAR strFondoPantalla[MAX_PATH];

	// Movement
	UINT umbralMovimiento;
	UINT velMaxima;
	UINT velMinima;
	UINT refreshTime;
	UINT factorMovimiento;
	UINT verticalScroll;
	UINT freestyleScroll;

	// DayOfWeek
	COLORREF dowColor;
	UINT dowWidth;
	UINT dowHeight;
	UINT dowWeight;
	TCHAR diasSemana[7][16];

	// DayOfMonth
	COLORREF domColor;
	UINT domWidth;
	UINT domHeight;
	UINT domWeight;

	// Clock
	COLORREF clockColor;
	UINT clockWidth;
	UINT clockHeight;
	UINT clockWeight;
	UINT clock12Format;

	// Battery
	COLORREF battColor;
	UINT battWidth;
	UINT battHeight;
	UINT battWeight;

	// Bubbles
	TCHAR bubble_notif[MAX_PATH];
	TCHAR bubble_state[MAX_PATH];
	TCHAR bubble_alarm[MAX_PATH];

	// OnLaunch
	UINT closeOnLaunchIcon;
	UINT vibrateOnLaunchIcon;
	UINT allowAnimationOnLaunchIcon;

	UINT notifyTimer;
	UINT ignoreRotation;
	UINT disableRightClick;
	UINT fullscreen;
	UINT neverShowTaskBar;
	UINT noWindowTitle;
	UINT alreadyConfigured;

};
