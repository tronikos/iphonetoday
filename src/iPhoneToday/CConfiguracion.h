#pragma once

#include "CConfigurationScreen.h"
#include "CListaPantalla.h"
#include "GraphicFunctions.h"
#include "xmlWrapper.h"

typedef struct SpecialIconSettings {
	INT height;
	INT width;
	INT weight;
	COLORREF color;
	RECT offset;
} SpecialIconSettings;

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
	float fondoFactor;
	TCHAR strFondoPantalla[MAX_PATH];

	// Movement
	UINT umbralMovimiento;
	UINT velMaxima;
	UINT velMinima;
	UINT refreshTime;
	UINT factorMovimiento;
	UINT verticalScroll;
	UINT freestyleScroll;

	// Special icons
	SpecialIconSettings dow;	// DayOfWeek
	TCHAR diasSemana[7][16];
	SpecialIconSettings dom;	// DayOfMonth
	SpecialIconSettings clck;	// Clock
	UINT clock12Format;
	SpecialIconSettings batt;	// Battery
	SpecialIconSettings vol;	// Volume
	SpecialIconSettings meml;	// Memory load
	SpecialIconSettings memf;	// Memory free
	SpecialIconSettings memu;	// Memory used
	SpecialIconSettings sign;	// Signal strength

	// Bubbles
	TCHAR bubble_notif[MAX_PATH];
	TCHAR bubble_state[MAX_PATH];
	TCHAR bubble_alarm[MAX_PATH];

	// OnLaunch
	UINT closeOnLaunchIcon;
	UINT vibrateOnLaunchIcon;
	UINT allowAnimationOnLaunchIcon;
	COLORREF colorOfAnimationOnLaunchIcon;
	UINT allowSoundOnLaunchIcon;
	TCHAR soundOnLaunchIcon[MAX_PATH];

	// General
	UINT notifyTimer;
	UINT ignoreRotation;
	UINT disableRightClick;
	UINT fullscreen;
	UINT neverShowTaskBar;
	UINT noWindowTitle;

	// Today item height (Portrait|Landscape)
	UINT heightP;
	UINT heightL;

	// Out of screen
	TCHAR outOfScreenLeft[MAX_PATH];
	TCHAR outOfScreenRight[MAX_PATH];
	TCHAR outOfScreenTop[MAX_PATH];
	TCHAR outOfScreenBottom[MAX_PATH];

	// Transparency
	BOOL alphaBlend;
	BOOL alphaOnBlack;
	UINT alphaThreshold;
	BOOL transparentBMP;

	UINT alreadyConfigured;
};
