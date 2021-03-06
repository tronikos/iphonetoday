#pragma once

#include "CConfigurationScreen.h"
#include "CListaPantalla.h"
#include "GraphicFunctions.h"
#include "xmlWrapper.h"

typedef struct SpecialIconSettings {
	TCHAR facename[LF_FACESIZE];
	INT height;
	INT width;
	INT weight;
	COLORREF color;
	RECT offset;
} SpecialIconSettings;

typedef struct BubbleSettings {
	TCHAR image[MAX_PATH];
	INT x;
	INT y;
	INT width;
	INT height;
	SpecialIconSettings sis;
} BubbleSettings;

typedef struct OutOfScreenSettings {
	BOOL stop;
	UINT stopAt;
	BOOL wrap;
	TCHAR exec[MAX_PATH];
} OutOfScreenSettings;

class CConfiguracion
{
public:
	CConfiguracion(void);
	~CConfiguracion(void);

	void calculaConfiguracion(CListaPantalla *listaPantallas, int width, int height);
	BOOL loadXMLIcons2(CListaPantalla *listaPantallas);
	void loadIconsImages(HDC *hDC, CListaPantalla *listaPantallas);
	void loadIconImage(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type);
	void loadImages(HDC *hDC);
	void loadBackground(HDC *hDC);
	void loadBackgrounds(HDC *hDC);
	void loadSounds();

	BOOL loadXMLIcons(CListaPantalla *listaPantallas);
	void defaultValues();
	BOOL AutoScale();
	BOOL loadXMLConfig();
	BOOL saveXMLConfig();
	BOOL saveXMLIcons(CListaPantalla *listaPantallas);

	BOOL saveXMLScreenIcons(TiXmlElement *pElemScreen, CPantalla *pantalla);

	void getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc, LPCTSTR pszDir);
	void getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc);
	void getRelativePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc);
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

	CIcono *pressedIcon;

	CIcono *bubbleNotif;
	CIcono *bubbleState;
	CIcono *bubbleAlarm;

	CIcono *fondoPantalla;
	CIcono *backMainScreen;
	CIcono *backBottomBar;
	CIcono *backTopBar;

	RECT circlesBarRect;
	int circlesDistAdjusted;

	// Variables from XML

	// Screens
	CConfigurationScreen *mainScreenConfig;
	CConfigurationScreen *bottomBarConfig;
	CConfigurationScreen *topBarConfig;

	// Circles
	UINT circlesDiameter;
	INT circlesDiameterActivePerc;
	UINT circlesDiameterMax;
	INT circlesDistance;
	INT circlesOffset;
	BOOL circlesAlignTop;
	COLORREF circlesColorActive;
	COLORREF circlesColorInactive;
	COLORREF circlesColorOuter;
	BOOL circlesSingleTap;
	BOOL circlesDoubleTap;

	// Header
	TCHAR headerTextFacename[LF_FACESIZE];
	UINT headerTextSize;
	UINT headerTextColor;
	UINT headerTextWeight;
	UINT headerOffset;
	UINT headerTextShadow;
	BOOL headerTextRoundRect;

	// Background
	UINT fondoTransparente;
	COLORREF fondoColor;
	BOOL fondoEstatico;
	float fondoFactor;
	BOOL fondoFitWidth;
	BOOL fondoFitHeight;
	BOOL fondoCenter;
	BOOL fondoTile;
	TCHAR strFondoPantalla[MAX_PATH];

	// Movement
	UINT moveThreshold;
	UINT velMaxima;
	UINT velMinima;
	UINT refreshTime;
	UINT factorMovimiento;
	UINT verticalScroll;
	UINT freestyleScroll;

	// Special icons
	SpecialIconSettings dow;	// DayOfWeek
	BOOL dowUseLocale;
	TCHAR diasSemana[7][16];
	SpecialIconSettings dom;	// DayOfMonth
	SpecialIconSettings clck;	// Clock
	BOOL clckShowAMPM;
	BOOL clock12Format;
	SpecialIconSettings alrm;	// Alarm
	BOOL alrmShowAMPM;
	SpecialIconSettings batt;	// Battery
	BOOL battShowAC;
	BOOL battShowPercentage;
	TCHAR battChargingSymbol[2];
	SpecialIconSettings vol;	// Volume
	BOOL volShowPercentage;
	SpecialIconSettings meml;	// Memory load
	BOOL memlShowPercentage;
	SpecialIconSettings memf;	// Memory free
	BOOL memfShowMB;
	UINT memOSUsedKB;
	SpecialIconSettings memu;	// Memory used
	BOOL memuShowMB;
	SpecialIconSettings psig;	// Signal strength
	BOOL psigShowPercentage;
	SpecialIconSettings wsig;	// Wifi signal strength
	BOOL wsigShowdBm;

	// Bubbles
	BubbleSettings bubble_notif;
	BubbleSettings bubble_state;
	BubbleSettings bubble_alarm;

	// Animation
	UINT animationEffect;
	COLORREF animationColor;
	UINT animationDuration;
	UINT animationDelay;
	BOOL launchAppAtBeginningOfAnimation;

	// OnLaunchIcon
	BOOL closeOnLaunchIcon;
	BOOL minimizeOnLaunchIcon;
	UINT vibrateOnLaunchIcon;
	TCHAR soundOnLaunchIcon[MAX_PATH];
	BYTE* soundOnLaunchIcon_bytes;
	TCHAR runTool[MAX_PATH];

	// OnPressIcon
	TCHAR pressed_icon[MAX_PATH];
	TCHAR pressed_sound[MAX_PATH];
	BYTE* pressed_sound_bytes;

	// OnChangeScreen
	TCHAR change_screen_sound[MAX_PATH];
	BYTE* change_screen_sound_bytes;

	// General
	UINT notifyTimer;
	BOOL updateWhenInactive;
	UINT ignoreRotation;
	UINT ignoreMinimize;
	UINT disableRightClick;
	UINT disableRightClickDots;
	UINT fullscreen;
	UINT neverShowTaskBar;
	UINT noWindowTitle;
	UINT showExit;
	UINT textQuality;
	UINT textQualityInIcons;
	UINT autoShowKeyboardOnTextboxFocus;
	UINT soundsEnabled;

	// Today item height (Portrait|Landscape)
	UINT heightP;
	UINT heightL;

	// Out of screen
	OutOfScreenSettings ooss_left;
	OutOfScreenSettings ooss_right;
	OutOfScreenSettings ooss_up;
	OutOfScreenSettings ooss_down;

	// Transparency
	UINT alphaBlend;
	BOOL alphaOnBlack;
	UINT alphaThreshold;
	BOOL transparentBMP;
	BOOL useMask;

	UINT lastConfiguredAtWidth;
	UINT lastConfiguredAtHeight;
};
