#include "stdafx.h"
#include "CConfiguracion.h"
#include "RegistryUtils.h"
#include "iPhoneToday.h"

CConfiguracion::CConfiguracion(void)
{
	// Initialize paths

	// Get the path of the executable
	TCHAR pathExecutable[MAX_PATH];
#ifdef EXEC_MODE
	GetModuleFileName(NULL, pathExecutable, CountOf(pathExecutable));
#else
	LoadTextSetting(HKEY_LOCAL_MACHINE, pathExecutable, L"Software\\Microsoft\\Today\\Items\\iPhoneToday", L"DLL", L"");
#endif

	StringCchCopy(pathExecutableDir, CountOf(pathExecutableDir), pathExecutable);
	TCHAR *p = wcsrchr(pathExecutableDir, '\\');
	if (p != NULL) *(p+1) = '\0';

	//p = L"\\iPhoneToday\\";
	//if (!FileOrDirExists(p, TRUE)) {
		p = pathExecutableDir;
	//}
	StringCchPrintf(pathSettingsXML, CountOf(pathSettingsXML),   L"%s%s", p, L"settings.xml");
	StringCchPrintf(pathIconsXML,    CountOf(pathIconsXML),      L"%s%s", p, L"icons.xml");
	StringCchPrintf(pathIconsDir,    CountOf(pathIconsDir),      L"%s%s", p, L"icons\\");

	StringCchCopy(pathIconsXMLDir, CountOf(pathIconsXMLDir), pathIconsXML);
	p = wcsrchr(pathIconsXMLDir, '\\');
	if (p != NULL) *(p+1) = '\0';

	fondoPantalla = NULL;
	mainScreenConfig = new CConfigurationScreen();
	bottomBarConfig = new CConfigurationScreen();
	topBarConfig = new CConfigurationScreen();
	altoPantallaMax = 0;

	this->defaultValues();
}

CConfiguracion::~CConfiguracion(void)
{
	if (bubbleNotif != NULL) {
		delete bubbleNotif;
	}
	if (bubbleState != NULL) {
		delete bubbleState;
	}
	if (bubbleAlarm != NULL) {
		delete bubbleAlarm;
	}
	if (fondoPantalla != NULL) {
		delete fondoPantalla;
	}
	if (mainScreenConfig != NULL) {
		delete mainScreenConfig;
	}
	if (bottomBarConfig != NULL) {
		delete bottomBarConfig;
	}
	if (topBarConfig != NULL) {
		delete topBarConfig;
	}
}

BOOL CConfiguracion::hasTimestampChanged()
{
	FILETIME newLastModifiedSettingsXML = FileModifyTime(pathSettingsXML);
	FILETIME newLastModifiedIconsXML = FileModifyTime(pathIconsXML);
	if (CompareFileTime(&newLastModifiedSettingsXML, &lastModifiedSettingsXML) != 0 ||
		CompareFileTime(&newLastModifiedIconsXML, &lastModifiedIconsXML) != 0) {
		return TRUE;
	}
	return FALSE;
}

void CConfiguracion::getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc)
{
	// Convert relative path to absolute
	if (pszSrc[0] == L'\\') {
		StringCchCopy(pszDest, cchDest, pszSrc);
	} else {
		// First check whether it is relative to the Icons directory
		StringCchPrintf(pszDest, cchDest, L"%s%s", pathIconsDir, pszSrc);
		if (!FileExists(pszDest)) {
			// Second check whether it is relative to the icons.xml directory
			StringCchPrintf(pszDest, cchDest, L"%s%s", pathIconsXMLDir, pszSrc);
			if (!FileExists(pszDest)) {
				// Third check whether it is relative to the executable's directory
				StringCchPrintf(pszDest, cchDest, L"%s%s", pathExecutableDir, pszSrc);
			}
		}
	}
}

// maxIconos = Maximo de iconos que hay en una pantalla
void CConfiguracion::calculaConfiguracion(int maxIconos, int numIconsInBottomBar, int numIconsInTopBar, int width, int height)
{
	if (width > 0) {
		anchoPantalla = width;
	}

	if (height > 0) {
		altoPantalla = height;
	}

	bottomBarConfig->calculate(TRUE, numIconsInBottomBar, anchoPantalla, altoPantalla);
	topBarConfig->calculate(TRUE, numIconsInTopBar, anchoPantalla, altoPantalla);
	mainScreenConfig->calculate(FALSE, maxIconos, anchoPantalla, altoPantalla);

	altoPantallaMax = max(((maxIconos + mainScreenConfig->iconsPerRow - 1) / mainScreenConfig->iconsPerRow) * mainScreenConfig->distanceIconsV + mainScreenConfig->posReference.y * 2, altoPantalla);
}

BOOL CConfiguracion::cargaXMLIconos(CListaPantalla *listaPantallas)
{
	if (listaPantallas == NULL) {
		return FALSE;
	}

	TiXmlDocument doc;
	FILE *f = _wfopen(pathIconsXML, L"rb");
	if (!f) return FALSE;
	bool loaded = doc.LoadFile(f, TIXML_ENCODING_UTF8);
	fclose(f);
	if (!loaded) return FALSE;
	TiXmlNode* pRoot = doc.FirstChild("root");
	if (!pRoot) return FALSE;

	lastModifiedIconsXML = FileModifyTime(pathIconsXML);

	if (listaPantallas->barraInferior == NULL) {
		listaPantallas->barraInferior = new CPantalla();
	}
	if (listaPantallas->topBar == NULL) {
		listaPantallas->topBar = new CPantalla();
	}

	// for each screen (child of root)
	UINT nScreen = 0;
	for (TiXmlElement *pElemScreen = pRoot->FirstChildElement(); pElemScreen; pElemScreen = pElemScreen->NextSiblingElement()) {
		const char *nameNode = pElemScreen->Value();
		CPantalla *pantalla = NULL;
		if (_stricmp(nameNode, "BottomBar") == 0) {
			pantalla = listaPantallas->barraInferior;
		} else if (_stricmp(nameNode, "TopBar") == 0) {
			pantalla = listaPantallas->topBar;
		} else if (_stricmp(nameNode, "screen") == 0) {
			if (listaPantallas->listaPantalla[nScreen] == NULL) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				pantalla = listaPantallas->listaPantalla[nScreen];
			}
			nScreen++;
		} else {
			continue;
		}

		XMLUtils::GetAttr(pElemScreen, "header", pantalla->header, CountOf(pantalla->header));

		// for each icon (child of screen)
		UINT nIcon = 0;
		for (TiXmlElement* pElemIcon = pElemScreen->FirstChildElement(); pElemIcon; pElemIcon = pElemIcon->NextSiblingElement()) {
			if (_stricmp(pElemIcon->Value(), "icon") == 0) {
				CIcono *icono;

				if (pantalla->listaIconos[nIcon] == NULL) {
					icono = pantalla->creaIcono();
				} else {
					icono = pantalla->listaIconos[nIcon];
					icono->defaultValues();
				}
				nIcon++;

				XMLUtils::GetAttr(pElemIcon, "name",			icono->nombre,			CountOf(icono->nombre));
				XMLUtils::GetAttr(pElemIcon, "image",			icono->rutaImagen,		CountOf(icono->rutaImagen));
				XMLUtils::GetAttr(pElemIcon, "sound",			icono->sound,			CountOf(icono->sound));
				XMLUtils::GetAttr(pElemIcon, "exec",			icono->ejecutable,		CountOf(icono->ejecutable));
				XMLUtils::GetAttr(pElemIcon, "parameters",		icono->parametros,		CountOf(icono->parametros));
				XMLUtils::GetAttr(pElemIcon, "execAlt",			icono->ejecutableAlt,	CountOf(icono->ejecutableAlt));
				XMLUtils::GetAttr(pElemIcon, "parametersAlt",	icono->parametrosAlt,	CountOf(icono->parametrosAlt));
				XMLUtils::GetAttr(pElemIcon, "type",			&icono->tipo);
				XMLUtils::GetAttr(pElemIcon, "animation",		&icono->launchAnimation);
			}
		}
		while (nIcon < pantalla->numIconos) {
			pantalla->borraIcono(nIcon);
		}
	}

	while (nScreen < listaPantallas->numPantallas) {
		listaPantallas->numPantallas--;
		delete listaPantallas->listaPantalla[listaPantallas->numPantallas];
	}

	return TRUE;
}

BOOL CConfiguracion::cargaIconos(HDC *hDC, CListaPantalla *listaPantallas)
{
	BOOL result = false;

	TIMER_RESET(loadImage_duration);
	TIMER_RESET(loadImage_IImagingFactory_CreateImageFromFile_duration);
	TIMER_RESET(loadImage_IImagingFactory_CreateBitmapFromImage_duration);

	// long duration = -(long)GetTickCount();
	result = cargaXMLIconos(listaPantallas);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to cargaXMLIconos.\n", duration);

	if (result == false) {
#if EXEC_MODE
		MessageBox(g_hWnd, TEXT("Bad icons.xml, check for errors please. (Remember: No Special Characters and tag well formed)"), TEXT("Error"), MB_OK);
#endif
		if (listaPantallas->numPantallas == 0) {
			listaPantallas->creaPantalla();
		}
		if (listaPantallas->barraInferior == NULL) {
			listaPantallas->barraInferior = new CPantalla();
		}
		if (listaPantallas->topBar == NULL) {
			listaPantallas->topBar = new CPantalla();
		}
		return result;
	}

	int nPantallas = listaPantallas->numPantallas;
	int nIconos;
	CPantalla *pantalla;
	CIcono *icono;

	// duration = -(long)GetTickCount();

	for (int i = 0; i < nPantallas; i++) {
		pantalla = listaPantallas->listaPantalla[i];

		nIconos = pantalla->numIconos;
		for (int j = 0; j < nIconos; j++) {

			icono = pantalla->listaIconos[j];
			cargaImagenIcono(hDC, icono, MAINSCREEN);
		}
	}

	// Cargamos los iconos de la barra inferior
	pantalla = listaPantallas->barraInferior;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {

		icono = pantalla->listaIconos[j];
		cargaImagenIcono(hDC, icono, BOTTOMBAR);
	}

	// Load icons for the top bar
	pantalla = listaPantallas->topBar;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {

		icono = pantalla->listaIconos[j];
		cargaImagenIcono(hDC, icono, TOPBAR);
	}

	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to cargaImagenIcono.\n", duration);

#ifdef TIMING
	NKDbgPrintfW(L" *** %d msec\t loadImage.\n", loadImage_duration);	
	NKDbgPrintfW(L" *** %d msec\t loadImage_IImagingFactory_CreateImageFromFile_duration.\n", loadImage_IImagingFactory_CreateImageFromFile_duration);
	NKDbgPrintfW(L" *** %d msec\t loadImage_IImagingFactory_CreateBitmapFromImage_duration.\n", loadImage_IImagingFactory_CreateBitmapFromImage_duration);
#endif

	result = true;
	return result;
}

BOOL CConfiguracion::cargaImagenIcono(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type)
{
	BOOL result;
	TCHAR rutaImgCompleta[MAX_PATH];
	UINT width;
	if (screen_type == BOTTOMBAR) {
		width = bottomBarConfig->iconWidth;
	} else if (screen_type == TOPBAR) {
		width = topBarConfig->iconWidth;
	} else {
		width = mainScreenConfig->iconWidth;
	}

	if (icono->rutaImagen != NULL && _tcslen(icono->rutaImagen) > 0) {
		getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), icono->rutaImagen);
		icono->loadImage(hDC, rutaImgCompleta, width, width);
	} else if (icono->ejecutable != NULL && _tcslen(icono->ejecutable) > 0) {
		getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), icono->ejecutable);
		icono->loadImageFromExec(hDC, rutaImgCompleta, width, width);
	}

	result = true;
	return result;
}
BOOL CConfiguracion::cargaFondo(HDC *hDC)
{
	BOOL isTransparent = this->fondoTransparente;
#ifdef EXEC_MODE
	isTransparent = FALSE;
#endif
	if (!isTransparent) {
		if (fondoPantalla != NULL) {
			delete fondoPantalla;
		}
		fondoPantalla = new CIcono();
		float factor = 1.75;
		if (this->fondoEstatico) {
			factor = 1.0;
		}

		TCHAR rutaImgCompleta[MAX_PATH];
		getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), strFondoPantalla);
		fondoPantalla->loadImage(hDC, rutaImgCompleta, UINT(anchoPantalla * factor), altoPantalla, PIXFMT_16BPP_RGB565);
	} else {
		if (fondoPantalla != NULL) {
			delete fondoPantalla;
			fondoPantalla = NULL;
		}
	}
	return TRUE;
}

BOOL CConfiguracion::cargaImagenes(HDC *hDC)
{
	BOOL result = false;
	TCHAR rutaImgCompleta[MAX_PATH];

	// Background
	cargaFondo(hDC);

	// Bubbles
	bubbleNotif = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_notif);
	bubbleNotif->loadImage(hDC, rutaImgCompleta, UINT(mainScreenConfig->iconWidth * 0.80), UINT(mainScreenConfig->iconWidth * 0.80));

	bubbleState = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_state);
	bubbleState->loadImage(hDC, rutaImgCompleta, mainScreenConfig->iconWidth, UINT(mainScreenConfig->iconWidth * 0.50));

	bubbleAlarm = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_alarm);
	bubbleAlarm->loadImage(hDC, rutaImgCompleta, UINT(mainScreenConfig->iconWidth * 0.80), UINT(mainScreenConfig->iconWidth * 0.80));

	result = true;
	return result;
}

void CConfiguracion::defaultValues()
{
	this->mainScreenConfig->defaultValues();
	this->bottomBarConfig->defaultValues();
	this->topBarConfig->defaultValues();

	this->circlesDiameter = 15;
	this->circlesDistance = 7;
	this->circlesOffset = 7;

	this->headerFontSize = 0;
	this->headerFontWeight = 900;
	this->headerFontColor = RGB(255, 255, 255);
	this->headerOffset = 0;

	this->fondoTransparente = 1;
	this->fondoColor = RGB(0, 0, 0);
	this->fondoEstatico = 1;
	StringCchCopy(this->strFondoPantalla, CountOf(this->strFondoPantalla), L"");

	this->umbralMovimiento = 15;
	this->velMaxima = 140;
	this->velMinima = 20;
	this->refreshTime = 20;
	this->factorMovimiento = 4;
	this->verticalScroll = 0;
	this->freestyleScroll = 0;

	StringCchCopy(this->diasSemana[0], CountOf(this->diasSemana[0]), TEXT("Sun"));
	StringCchCopy(this->diasSemana[1], CountOf(this->diasSemana[1]), TEXT("Mon"));
	StringCchCopy(this->diasSemana[2], CountOf(this->diasSemana[2]), TEXT("Tue"));
	StringCchCopy(this->diasSemana[3], CountOf(this->diasSemana[3]), TEXT("Wed"));
	StringCchCopy(this->diasSemana[4], CountOf(this->diasSemana[4]), TEXT("Thu"));
	StringCchCopy(this->diasSemana[5], CountOf(this->diasSemana[5]), TEXT("Fri"));
	StringCchCopy(this->diasSemana[6], CountOf(this->diasSemana[6]), TEXT("Sat"));
	StringCchCopy(this->strFondoPantalla, CountOf(this->strFondoPantalla), TEXT(""));

	this->dowColor = RGB(255,255,255);
	this->dowWidth = 18;
	this->dowHeight = 40;
	this->dowWeight = 400;

	this->domColor = RGB(30,30,30);
	this->domWidth = 30;
	this->domHeight = 80;
	this->domWeight = 800;

	this->clockColor = RGB(230,230,230);
	this->clockWidth = 13;
	this->clockHeight = 60;
	this->clockWeight = 900;
	this->clock12Format = 0;

	this->battColor = RGB(230,230,230);
	this->battWidth = 25;
	this->battHeight = 70;
	this->battWeight = 900;

	StringCchCopy(this->bubble_notif, CountOf(this->bubble_notif), TEXT("bubble_notif.png"));
	StringCchCopy(this->bubble_state, CountOf(this->bubble_notif), TEXT("bubble_state.png"));
	StringCchCopy(this->bubble_alarm, CountOf(this->bubble_notif), TEXT("bubble_alarm.png"));

	this->closeOnLaunchIcon = 0;
	this->vibrateOnLaunchIcon = 40;
	this->allowAnimationOnLaunchIcon = 1;

	this->notifyTimer = 2000;
	this->ignoreRotation = 0;
	this->disableRightClick = 0;
	this->fullscreen = 0;
	this->neverShowTaskBar = 0;
	this->noWindowTitle = 0;
	this->alreadyConfigured = 0;
}

BOOL CConfiguracion::cargaXMLConfig()
{
	TiXmlDocument doc;
	FILE *f = _wfopen(pathSettingsXML, L"rb");
	if (!f) return FALSE;
	bool loaded = doc.LoadFile(f, TIXML_ENCODING_UTF8);
	fclose(f);
	if (!loaded) return FALSE;
	TiXmlNode* pRoot = doc.FirstChild("root");
	if (!pRoot) return FALSE;

	lastModifiedSettingsXML = FileModifyTime(pathSettingsXML);

	// for each child of root
	for (TiXmlElement *pElem = pRoot->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement()) {
		const char *nameNode = pElem->Value();

		if(_stricmp(nameNode, "Circles") == 0) {
			XMLUtils::GetAttr(pElem, "diameter", &this->circlesDiameter);
			XMLUtils::GetAttr(pElem, "distance", &this->circlesDistance);
			XMLUtils::GetAttr(pElem, "offset", &this->circlesOffset);
		} else if(_stricmp(nameNode, "Header") == 0) {
			XMLUtils::GetAttr(pElem, "size",   &this->headerFontSize);
			XMLUtils::GetAttr(pElem, "color",  &this->headerFontColor);
			XMLUtils::GetAttr(pElem, "weight", &this->headerFontWeight);
			XMLUtils::GetAttr(pElem, "offset", &this->headerOffset);
		} else if(_stricmp(nameNode, "Movement") == 0) {
			XMLUtils::GetAttr(pElem, "MoveThreshold",  &this->umbralMovimiento);
			XMLUtils::GetAttr(pElem, "MaxVelocity",    &this->velMaxima);
			XMLUtils::GetAttr(pElem, "MinVelocity",    &this->velMinima);
			XMLUtils::GetAttr(pElem, "RefreshTime",    &this->refreshTime);
			XMLUtils::GetAttr(pElem, "FactorMov",      &this->factorMovimiento);
			XMLUtils::GetAttr(pElem, "VerticalScroll", &this->verticalScroll);
			XMLUtils::GetAttr(pElem, "FreestyleScroll", &this->freestyleScroll);
		} else if(_stricmp(nameNode, "DayOfWeek") == 0) {
			XMLUtils::GetAttr(pElem, "color",     &this->dowColor);
			XMLUtils::GetAttr(pElem, "width",     &this->dowWidth);
			XMLUtils::GetAttr(pElem, "height",    &this->dowHeight);
			XMLUtils::GetAttr(pElem, "weight",    &this->dowWeight);
			XMLUtils::GetAttr(pElem, "Sunday",    this->diasSemana[0], CountOf(this->diasSemana[0]));
			XMLUtils::GetAttr(pElem, "Monday",    this->diasSemana[1], CountOf(this->diasSemana[1]));
			XMLUtils::GetAttr(pElem, "Tuesday",   this->diasSemana[2], CountOf(this->diasSemana[2]));
			XMLUtils::GetAttr(pElem, "Wednesday", this->diasSemana[3], CountOf(this->diasSemana[3]));
			XMLUtils::GetAttr(pElem, "Thursday",  this->diasSemana[4], CountOf(this->diasSemana[4]));
			XMLUtils::GetAttr(pElem, "Friday",    this->diasSemana[5], CountOf(this->diasSemana[5]));
			XMLUtils::GetAttr(pElem, "Saturday",  this->diasSemana[6], CountOf(this->diasSemana[6]));
		} else if(_stricmp(nameNode, "DayOfMonth") == 0) {
			XMLUtils::GetAttr(pElem, "color",  &this->domColor);
			XMLUtils::GetAttr(pElem, "width",  &this->domWidth);
			XMLUtils::GetAttr(pElem, "height", &this->domHeight);
			XMLUtils::GetAttr(pElem, "weight", &this->domWeight);
		} else if(_stricmp(nameNode, "Clock") == 0) {
			XMLUtils::GetAttr(pElem, "color",    &this->clockColor);
			XMLUtils::GetAttr(pElem, "width",    &this->clockWidth);
			XMLUtils::GetAttr(pElem, "height",   &this->clockHeight);
			XMLUtils::GetAttr(pElem, "weight",   &this->clockWeight);
			XMLUtils::GetAttr(pElem, "format12", &this->clock12Format);
		} else if(_stricmp(nameNode, "Battery") == 0) {
			XMLUtils::GetAttr(pElem, "color",    &this->battColor);
			XMLUtils::GetAttr(pElem, "width",    &this->battWidth);
			XMLUtils::GetAttr(pElem, "height",   &this->battHeight);
			XMLUtils::GetAttr(pElem, "weight",   &this->battWeight);
		} else if(_stricmp(nameNode, "Bubbles") == 0) {
			XMLUtils::GetAttr(pElem, "notif", this->bubble_notif, CountOf(this->bubble_notif));
			XMLUtils::GetAttr(pElem, "state", this->bubble_state, CountOf(this->bubble_state));
			XMLUtils::GetAttr(pElem, "alarm", this->bubble_alarm, CountOf(this->bubble_alarm));
		} else if(_stricmp(nameNode, "OnLaunchIcon") == 0) {
			XMLUtils::GetAttr(pElem, "close",   &this->closeOnLaunchIcon);
			XMLUtils::GetAttr(pElem, "vibrate", &this->vibrateOnLaunchIcon);
			XMLUtils::GetAttr(pElem, "animate", &this->allowAnimationOnLaunchIcon);
		} else if(_stricmp(nameNode, "Background") == 0) {
			XMLUtils::GetAttr(pElem, "transparent", &this->fondoTransparente);
			XMLUtils::GetAttr(pElem, "color",       &this->fondoColor);
			XMLUtils::GetAttr(pElem, "static",      &this->fondoEstatico);
			XMLUtils::GetAttr(pElem, "wallpaper",   this->strFondoPantalla, CountOf(this->strFondoPantalla));
		} else if(_stricmp(nameNode, "NotifyTimer") == 0) {
			XMLUtils::GetTextElem(pElem, &this->notifyTimer);
		} else if(_stricmp(nameNode, "IgnoreRotation") == 0) {
			XMLUtils::GetTextElem(pElem, &this->ignoreRotation);
		} else if(_stricmp(nameNode, "DisableRightClick") == 0) {
			XMLUtils::GetTextElem(pElem, &this->disableRightClick);
		} else if(_stricmp(nameNode, "Fullscreen") == 0) {
			XMLUtils::GetTextElem(pElem, &this->fullscreen);
		} else if(_stricmp(nameNode, "NeverShowTaskBar") == 0) {
			XMLUtils::GetTextElem(pElem, &this->neverShowTaskBar);
		} else if(_stricmp(nameNode, "NoWindowTitle") == 0) {
			XMLUtils::GetTextElem(pElem, &this->noWindowTitle);
		} else if(_stricmp(nameNode, "AlreadyConfigured") == 0) {
			XMLUtils::GetTextElem(pElem, &this->alreadyConfigured);
		} else if(_stricmp(nameNode, "MainScreen") == 0) {
			mainScreenConfig->loadXMLConfig(pElem);
		} else if(_stricmp(nameNode, "BottomBar") == 0) {
			bottomBarConfig->loadXMLConfig(pElem);
		} else if(_stricmp(nameNode, "TopBar") == 0) {
			topBarConfig->loadXMLConfig(pElem);
		}
    }

	return TRUE;
}

BOOL CConfiguracion::guardaXMLIconos(CListaPantalla *listaPantallas)
{
	if (listaPantallas == NULL) {
		return FALSE;
	}

	TiXmlDocument doc;

	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	doc.LinkEndChild(decl);

	TiXmlComment *comment = new TiXmlComment("iPhoneToday for Windows Mobile");
	doc.LinkEndChild(comment);

	TiXmlElement *root = new TiXmlElement("root");
	doc.LinkEndChild(root);

	CPantalla *pantalla;
	TiXmlElement *pElemScreen;

	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		pantalla = listaPantallas->listaPantalla[i];

		pElemScreen = new TiXmlElement("screen");
		XMLUtils::SetAttr(pElemScreen, "header", pantalla->header, CountOf(pantalla->header));
		saveXMLScreenIcons(pElemScreen, pantalla);
		root->LinkEndChild(pElemScreen);
	}

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
		pantalla = listaPantallas->barraInferior;

		pElemScreen = new TiXmlElement("BottomBar");
		saveXMLScreenIcons(pElemScreen, pantalla);
		root->LinkEndChild(pElemScreen);
	}

	if (listaPantallas->topBar != NULL && listaPantallas->topBar->numIconos > 0) {
		pantalla = listaPantallas->topBar;

		pElemScreen = new TiXmlElement("TopBar");
		saveXMLScreenIcons(pElemScreen, pantalla);
		root->LinkEndChild(pElemScreen);
	}

	FILE *f = _wfopen(pathIconsXML, L"wb");
	doc.SaveFile(f);
	fclose(f);

	lastModifiedIconsXML = FileModifyTime(pathIconsXML);

	return 0;
}

BOOL CConfiguracion::saveXMLScreenIcons(TiXmlElement *pElemScreen, CPantalla *pantalla)
{
	if (pantalla == NULL) {
		return FALSE;
	}

	for (UINT j = 0; j < pantalla->numIconos; j++) {
		CIcono *icon = pantalla->listaIconos[j];
		if (icon == NULL) continue;

		TiXmlElement *pElemIcon = new TiXmlElement("icon");

		XMLUtils::SetAttr(pElemIcon, "name",			icon->nombre,			CountOf(icon->nombre));
		XMLUtils::SetAttr(pElemIcon, "image",			icon->rutaImagen,		CountOf(icon->rutaImagen));
		XMLUtils::SetAttr(pElemIcon, "sound",			icon->sound,			CountOf(icon->sound));
		XMLUtils::SetAttr(pElemIcon, "exec",			icon->ejecutable,		CountOf(icon->ejecutable));
		XMLUtils::SetAttr(pElemIcon, "parameters",		icon->parametros,		CountOf(icon->parametros));
		XMLUtils::SetAttr(pElemIcon, "execAlt",			icon->ejecutableAlt,	CountOf(icon->ejecutableAlt));
		XMLUtils::SetAttr(pElemIcon, "parametersAlt",	icon->parametrosAlt,	CountOf(icon->parametrosAlt));
		XMLUtils::SetAttr(pElemIcon, "type",			icon->tipo);
		XMLUtils::SetAttr(pElemIcon, "animation",		icon->launchAnimation);

		pElemScreen->LinkEndChild(pElemIcon);
	}

	return TRUE;
}

BOOL CConfiguracion::guardaXMLConfig()
{
	TiXmlDocument doc;

	TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	doc.LinkEndChild(decl);

	TiXmlComment *comment = new TiXmlComment("iPhoneToday for Windows Mobile");
	doc.LinkEndChild(comment);

	TiXmlElement *root = new TiXmlElement("root");
	doc.LinkEndChild(root);

	TiXmlElement *pElem;

	pElem = new TiXmlElement("MainScreen");
	mainScreenConfig->saveXMLConfig(pElem);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("BottomBar");
	bottomBarConfig->saveXMLConfig(pElem);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("TopBar");
	topBarConfig->saveXMLConfig(pElem);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Circles");
	XMLUtils::SetAttr(pElem, "diameter", this->circlesDiameter);
	XMLUtils::SetAttr(pElem, "distance", this->circlesDistance);
	XMLUtils::SetAttr(pElem, "offset",   this->circlesOffset);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Header");
	XMLUtils::SetAttr(pElem, "size",   this->headerFontSize);
	XMLUtils::SetAttr(pElem, "color",  this->headerFontColor);
	XMLUtils::SetAttr(pElem, "weight", this->headerFontWeight);
	XMLUtils::SetAttr(pElem, "offset", this->headerOffset);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Background");
	XMLUtils::SetAttr(pElem, "transparent", this->fondoTransparente);
	XMLUtils::SetAttr(pElem, "color",       this->fondoColor);
	XMLUtils::SetAttr(pElem, "static",      this->fondoEstatico);
	XMLUtils::SetAttr(pElem, "wallpaper",   this->strFondoPantalla, CountOf(this->strFondoPantalla));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Movement");
	XMLUtils::SetAttr(pElem, "MoveThreshold",  this->umbralMovimiento);
	XMLUtils::SetAttr(pElem, "MaxVelocity",    this->velMaxima);
	XMLUtils::SetAttr(pElem, "MinVelocity",    this->velMinima);
	XMLUtils::SetAttr(pElem, "RefreshTime",    this->refreshTime);
	XMLUtils::SetAttr(pElem, "FactorMov",      this->factorMovimiento);
	XMLUtils::SetAttr(pElem, "VerticalScroll", this->verticalScroll);
	XMLUtils::SetAttr(pElem, "FreestyleScroll", this->freestyleScroll);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DayOfWeek");
	XMLUtils::SetAttr(pElem, "color",     this->dowColor);
	XMLUtils::SetAttr(pElem, "width",     this->dowWidth);
	XMLUtils::SetAttr(pElem, "height",    this->dowHeight);
	XMLUtils::SetAttr(pElem, "weight",    this->dowWeight);
	XMLUtils::SetAttr(pElem, "Sunday",    this->diasSemana[0], CountOf(this->diasSemana[0]));
	XMLUtils::SetAttr(pElem, "Monday",    this->diasSemana[1], CountOf(this->diasSemana[1]));
	XMLUtils::SetAttr(pElem, "Tuesday",   this->diasSemana[2], CountOf(this->diasSemana[2]));
	XMLUtils::SetAttr(pElem, "Wednesday", this->diasSemana[3], CountOf(this->diasSemana[3]));
	XMLUtils::SetAttr(pElem, "Thursday",  this->diasSemana[4], CountOf(this->diasSemana[4]));
	XMLUtils::SetAttr(pElem, "Friday",    this->diasSemana[5], CountOf(this->diasSemana[5]));
	XMLUtils::SetAttr(pElem, "Saturday",  this->diasSemana[6], CountOf(this->diasSemana[6]));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DayOfMonth");
	XMLUtils::SetAttr(pElem, "color",  this->domColor);
	XMLUtils::SetAttr(pElem, "width",  this->domWidth);
	XMLUtils::SetAttr(pElem, "height", this->domHeight);
	XMLUtils::SetAttr(pElem, "weight", this->domWeight);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Clock");
	XMLUtils::SetAttr(pElem, "color",    this->clockColor);
	XMLUtils::SetAttr(pElem, "width",    this->clockWidth);
	XMLUtils::SetAttr(pElem, "height",   this->clockHeight);
	XMLUtils::SetAttr(pElem, "weight",   this->clockWeight);
	XMLUtils::SetAttr(pElem, "format12", this->clock12Format);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Battery");
	XMLUtils::SetAttr(pElem, "color",    this->battColor);
	XMLUtils::SetAttr(pElem, "width",    this->battWidth);
	XMLUtils::SetAttr(pElem, "height",   this->battHeight);
	XMLUtils::SetAttr(pElem, "weight",   this->battWeight);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Bubbles");
	XMLUtils::SetAttr(pElem, "notif", this->bubble_notif, CountOf(this->bubble_notif));
	XMLUtils::SetAttr(pElem, "state", this->bubble_state, CountOf(this->bubble_state));
	XMLUtils::SetAttr(pElem, "alarm", this->bubble_alarm, CountOf(this->bubble_alarm));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OnLaunchIcon");
	XMLUtils::SetAttr(pElem, "close",   this->closeOnLaunchIcon);
	XMLUtils::SetAttr(pElem, "vibrate", this->vibrateOnLaunchIcon);
	XMLUtils::SetAttr(pElem, "animate", this->allowAnimationOnLaunchIcon);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("NotifyTimer");
	XMLUtils::SetTextElem(pElem, this->notifyTimer);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("IgnoreRotation");
	XMLUtils::SetTextElem(pElem, this->ignoreRotation);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DisableRightClick");
	XMLUtils::SetTextElem(pElem, this->disableRightClick);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Fullscreen");
	XMLUtils::SetTextElem(pElem, this->fullscreen);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("NeverShowTaskBar");
	XMLUtils::SetTextElem(pElem, this->neverShowTaskBar);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("NoWindowTitle");
	XMLUtils::SetTextElem(pElem, this->noWindowTitle);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("AlreadyConfigured");
	XMLUtils::SetTextElem(pElem, this->alreadyConfigured);
	root->LinkEndChild(pElem);

	FILE *f = _wfopen(pathSettingsXML, L"wb");
	doc.SaveFile(f);
	fclose(f);

	lastModifiedSettingsXML = FileModifyTime(pathSettingsXML);

	return 0;
}
