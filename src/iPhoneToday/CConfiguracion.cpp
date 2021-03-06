#include "stdafx.h"
#include "CConfiguracion.h"
#include "RegistryUtils.h"
#include "iPhoneToday.h"

// Maximum size of a wav to load it in memory
#define MAX_SND_MEMORY 200 * 1024

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
	backMainScreen = NULL;
	backBottomBar = NULL;
	backTopBar = NULL;
	mainScreenConfig = new CConfigurationScreen();
	bottomBarConfig = new CConfigurationScreen();
	topBarConfig = new CConfigurationScreen();
	altoPantallaMax = 0;

	this->defaultValues();
}

CConfiguracion::~CConfiguracion(void)
{
	delete pressedIcon;
	delete bubbleNotif;
	delete bubbleState;
	delete bubbleAlarm;
	delete fondoPantalla;
	delete backMainScreen;
	delete backBottomBar;
	delete backTopBar;
	delete mainScreenConfig;
	delete bottomBarConfig;
	delete topBarConfig;
	PlaySound(NULL, 0, 0);
	delete soundOnLaunchIcon_bytes;
	delete pressed_sound_bytes;
	delete change_screen_sound_bytes;
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

// pszDest = pszDir + pszSrc with "..\" properly handled.
void CConfiguracion::getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc, LPCTSTR pszDir)
{
	StringCchCopy(pszDest, cchDest, pszDir);
	if (pszDest[wcslen(pszDest) - 1] == L'\\') {
		pszDest[wcslen(pszDest) - 1] = 0;
	}
	LPCTSTR s = pszSrc;
	while (_wcsnicmp(s, L"..\\", 3) == 0) {
		s += 3;
		TCHAR *p = wcsrchr(pszDest, '\\');
		if (p) {
			*p = 0;
		}
	}
	StringCchCat(pszDest, cchDest, L"\\");
	StringCchCat(pszDest, cchDest, s);
}

void CConfiguracion::getAbsolutePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc)
{
	// Convert relative path to absolute
	if (pszSrc[0] == L'\\') {
		StringCchCopy(pszDest, cchDest, pszSrc);
	} else {
		// First check whether it is relative to the Icons directory
		getAbsolutePath(pszDest, cchDest, pszSrc, pathIconsDir);
		if (!FileExists(pszDest)) {
			// Second check whether it is relative to the icons.xml directory
			getAbsolutePath(pszDest, cchDest, pszSrc, pathIconsXMLDir);
			if (!FileExists(pszDest)) {
				// Third check whether it is relative to the executable's directory
				getAbsolutePath(pszDest, cchDest, pszSrc, pathExecutableDir);
			}
		}
	}
}

// Convert absolute path to relative
void CConfiguracion::getRelativePath(LPTSTR pszDest, size_t cchDest, LPCTSTR pszSrc)
{
	int offset = 0;
	if (pszSrc[0] == L'\\') {
		// First check whether it could be relative to the Icons directory
		if (wcslen(pszSrc) > wcslen(pathIconsDir) && wcsnicmp(pszSrc, pathIconsDir, wcslen(pathIconsDir)) == 0) {
			offset = wcslen(pathIconsDir);
		// Second check whether it could be relative to the icons.xml directory
		} else if (wcslen(pszSrc) > wcslen(pathIconsXMLDir) && wcsnicmp(pszSrc, pathIconsXMLDir, wcslen(pathIconsXMLDir)) == 0) {
			offset = wcslen(pathIconsXMLDir);
		// Third check whether it could be relative to the executable's directory
		} else if (wcslen(pszSrc) > wcslen(pathIconsXMLDir) && wcsnicmp(pszSrc, pathExecutableDir, wcslen(pathExecutableDir)) == 0) {
			offset = wcslen(pathExecutableDir);
		}
	}
	StringCchCopy(pszDest, cchDest, pszSrc + offset);
}

void CConfiguracion::calculaConfiguracion(CListaPantalla *listaPantallas, int width, int height)
{
	// maxIconos = Maximo de iconos que hay en una pantalla
	int maxIconos = 0;
	for (int i = 0; i < (int)listaPantallas->numPantallas; i++) {
		maxIconos = max(maxIconos, (int)listaPantallas->listaPantalla[i]->numIconos);
	}

	int numIconsInBottomBar	= listaPantallas->barraInferior == NULL ? 0 : listaPantallas->barraInferior->numIconos;
	int numIconsInTopBar = listaPantallas->topBar == NULL ? 0 : listaPantallas->topBar->numIconos;

	if (width > 0) {
		this->anchoPantalla = width;
	}

	if (height > 0) {
		this->altoPantalla = height;
	}

	bottomBarConfig->calculate(TRUE, numIconsInBottomBar, anchoPantalla, altoPantalla);
	topBarConfig->calculate(TRUE, numIconsInTopBar, anchoPantalla, altoPantalla);
	mainScreenConfig->calculate(FALSE, maxIconos, anchoPantalla, altoPantalla);

	this->altoPantallaMax = max(((maxIconos + mainScreenConfig->iconsPerRow - 1) / mainScreenConfig->iconsPerRow) * mainScreenConfig->distanceIconsV + mainScreenConfig->posReference.y * 2, altoPantalla);

	// calculate circles' configuration
	circlesDiameterMax = max(circlesDiameter, (UINT) (circlesDiameter + circlesDiameter * (circlesDiameterActivePerc / 100.0f)));
	if (listaPantallas->numPantallas > 1 && circlesDiameterMax > 0) {
		this->circlesDistAdjusted = this->circlesDistance;
		if (this->circlesDistAdjusted < 0) {
			CConfigurationScreen *cs_tmp = this->mainScreenConfig;
			if (this->circlesAlignTop) {
				if (numIconsInTopBar > 0 && this->topBarConfig->iconWidth > 0) {
					cs_tmp = this->topBarConfig;
				}
			} else {
				if (numIconsInBottomBar > 0 && this->bottomBarConfig->iconWidth > 0) {
					cs_tmp = this->bottomBarConfig;
				}
			}
			this->circlesBarRect.left = cs_tmp->posReference.x + (cs_tmp->iconWidth - circlesDiameterMax) / 2;
			this->circlesBarRect.left += (-(int) this->circlesDistAdjusted - 1) * cs_tmp->distanceIconsH;
			this->circlesDistAdjusted = cs_tmp->distanceIconsH - circlesDiameterMax;
		} else {
			this->circlesBarRect.left = int((this->anchoPantalla / 2) - ((listaPantallas->numPantallas - 1) * (circlesDiameterMax + this->circlesDistAdjusted) + circlesDiameterMax) / 2);
		}
		this->circlesBarRect.right = this->circlesBarRect.left + (listaPantallas->numPantallas - 1) * (circlesDiameterMax + this->circlesDistAdjusted) + circlesDiameterMax;
		if (this->circlesAlignTop) {
			this->circlesBarRect.top = this->circlesOffset;
			if (numIconsInTopBar > 0 && this->topBarConfig->iconWidth > 0) {
				//this->circlesBarRect.top += listaPantallas->topBar->altoPantalla;
				this->circlesBarRect.top += this->topBarConfig->distanceIconsV + this->topBarConfig->cs.offset.top + this->topBarConfig->cs.offset.bottom;
			}
		} else {
			this->circlesBarRect.top = int(this->altoPantalla) - circlesDiameterMax - this->circlesOffset;
			if (numIconsInBottomBar > 0 && this->bottomBarConfig->iconWidth > 0) {
				//this->circlesBarRect.top -= listaPantallas->barraInferior->altoPantalla;
				this->circlesBarRect.top -= this->bottomBarConfig->distanceIconsV + this->bottomBarConfig->cs.offset.top + this->bottomBarConfig->cs.offset.bottom;
			}
		}
		this->circlesBarRect.bottom = this->circlesBarRect.top + circlesDiameterMax;
	}
}

BOOL CConfiguracion::loadXMLIcons(CListaPantalla *listaPantallas)
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
				XMLUtils::GetAttr(pElemIcon, "typeParams",		icono->typeParams,		CountOf(icono->typeParams));
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

BOOL CConfiguracion::loadXMLIcons2(CListaPantalla *listaPantallas)
{
	// long duration = -(long)GetTickCount();
	BOOL result = loadXMLIcons(listaPantallas);
	// duration += GetTickCount();
	// NKDbgPrintfW(L" *** %d \t to loadXMLIcons.\n", duration);

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
		return FALSE;
	}

	return TRUE;
}

void CConfiguracion::loadIconsImages(HDC *hDC, CListaPantalla *listaPantallas)
{
	TIMER_RESET(loadImage_duration);
	TIMER_RESET(loadImage_load_duration);
	TIMER_RESET(loadImage_resize_duration);
	TIMER_RESET(loadImage_fix_duration);

	int nPantallas = listaPantallas->numPantallas;
	int nIconos;
	CPantalla *pantalla;
	CIcono *icono;

	for (int i = 0; i < nPantallas; i++) {
		pantalla = listaPantallas->listaPantalla[i];
		nIconos = pantalla->numIconos;
		for (int j = 0; j < nIconos; j++) {
			icono = pantalla->listaIconos[j];
			loadIconImage(hDC, icono, MAINSCREEN);
		}
	}

	// Cargamos los iconos de la barra inferior
	pantalla = listaPantallas->barraInferior;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {
		icono = pantalla->listaIconos[j];
		loadIconImage(hDC, icono, BOTTOMBAR);
	}

	// Load icons for the top bar
	pantalla = listaPantallas->topBar;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {
		icono = pantalla->listaIconos[j];
		loadIconImage(hDC, icono, TOPBAR);
	}

#ifdef TIMING
	NKDbgPrintfW(L" *** %d msec\t loadImage.\n", loadImage_duration);	
	NKDbgPrintfW(L" *** %d msec\t loadImage_load_duration.\n", loadImage_load_duration);
	NKDbgPrintfW(L" *** %d msec\t loadImage_resize_duration.\n", loadImage_resize_duration);
	NKDbgPrintfW(L" *** %d msec\t loadImage_fix_duration.\n", loadImage_fix_duration);
#endif
}

void CConfiguracion::loadIconImage(HDC *hDC, CIcono *icono, SCREEN_TYPE screen_type)
{
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
}

void CConfiguracion::loadBackground(HDC *hDC)
{
	delete fondoPantalla;
	fondoPantalla = new CIcono();

#ifndef EXEC_MODE
	BOOL isTransparent = this->fondoTransparente;
	if (!isTransparent) {
#endif
		TCHAR rutaImgCompleta[MAX_PATH];
		getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), strFondoPantalla);
		fondoPantalla->loadImage(hDC, rutaImgCompleta, this->fondoFitWidth ? this->anchoPantalla : 0, this->fondoFitHeight ? this->altoPantalla : 0, PIXFMT_16BPP_RGB565, this->fondoFactor);
		if (fondoPantalla->hDC == NULL) {
			delete fondoPantalla;
			fondoPantalla = NULL;
		}
#ifndef EXEC_MODE
	} else {
		HDC hdc = GetDC(NULL);
		fondoPantalla->hDC = CreateCompatibleDC(hdc);
		fondoPantalla->imagen = CreateCompatibleBitmap(hdc, anchoPantalla, altoPantalla);
		fondoPantalla->imagenOld = (HBITMAP)SelectObject(fondoPantalla->hDC, fondoPantalla->imagen);
		fondoPantalla->anchoImagen = anchoPantalla;
		fondoPantalla->altoImagen = altoPantalla;
		ReleaseDC(NULL, hdc);
	}
#endif
}

void CConfiguracion::loadBackgrounds(HDC *hDC)
{
	TCHAR fullPath[MAX_PATH];

	if (this->backMainScreen != NULL) {
		delete backMainScreen;
		backMainScreen = NULL;
	}
	if (wcslen(this->mainScreenConfig->cs.backWallpaper) > 0) {
		backMainScreen = new CIcono();
		getAbsolutePath(fullPath, CountOf(fullPath), this->mainScreenConfig->cs.backWallpaper);
		backMainScreen->loadImage(hDC, fullPath,
			this->mainScreenConfig->cs.backWallpaperFitWidth ? this->anchoPantalla : 0,
			this->mainScreenConfig->cs.backWallpaperFitHeight ? this->altoPantallaMax : 0,
			PIXFMT_16BPP_RGB565,
			1, TRUE);
		if (backMainScreen->hDC == NULL) {
			delete backMainScreen;
			backMainScreen = NULL;
		}
	}

	if (this->backBottomBar != NULL) {
		delete backBottomBar;
		backBottomBar = NULL;
	}
	if (wcslen(this->bottomBarConfig->cs.backWallpaper) > 0) {
		backBottomBar = new CIcono();
		getAbsolutePath(fullPath, CountOf(fullPath), this->bottomBarConfig->cs.backWallpaper);
		int height = this->bottomBarConfig->distanceIconsV + this->bottomBarConfig->cs.offset.top + this->bottomBarConfig->cs.offset.bottom;
		const TCHAR *ext = wcsrchr(fullPath, '.');
		BOOL isPNG = lstrcmpi(ext, L".png") == 0;
		backBottomBar->loadImage(hDC, fullPath,
			this->bottomBarConfig->cs.backWallpaperFitWidth ? this->anchoPantalla : 0,
			this->bottomBarConfig->cs.backWallpaperFitHeight ? height : 0,
			this->bottomBarConfig->cs.backWallpaperAlphaBlend && isPNG ? PIXFMT_32BPP_ARGB : PIXFMT_16BPP_RGB565,
			1, TRUE);
		if (backBottomBar->hDC == NULL) {
			delete backBottomBar;
			backBottomBar = NULL;
		}
	}

	if (this->backTopBar != NULL) {
		delete backTopBar;
		backTopBar = NULL;
	}
	if (wcslen(this->topBarConfig->cs.backWallpaper) > 0) {
		backTopBar = new CIcono();
		getAbsolutePath(fullPath, CountOf(fullPath), this->topBarConfig->cs.backWallpaper);
		int height = this->topBarConfig->distanceIconsV + this->topBarConfig->cs.offset.top + this->topBarConfig->cs.offset.bottom;
		const TCHAR *ext = wcsrchr(fullPath, '.');
		BOOL isPNG = lstrcmpi(ext, L".png") == 0;
		backTopBar->loadImage(hDC, fullPath,
			this->topBarConfig->cs.backWallpaperFitWidth ? this->anchoPantalla : 0,
			this->topBarConfig->cs.backWallpaperFitHeight ? height : 0,
			this->topBarConfig->cs.backWallpaperAlphaBlend && isPNG ? PIXFMT_32BPP_ARGB : PIXFMT_16BPP_RGB565,
			1, TRUE);
		if (backTopBar->hDC == NULL) {
			delete backTopBar;
			backTopBar = NULL;
		}
	}

	// do not load background if mainscreen pages already have a background (gradient or image)
	if ((backMainScreen && backMainScreen->hDC) || mainScreenConfig->cs.backGradient) {
		delete fondoPantalla;
		fondoPantalla = NULL;
	} else {
		loadBackground(hDC);
	}
}

void CConfiguracion::loadImages(HDC *hDC)
{
	TCHAR rutaImgCompleta[MAX_PATH];

	// Background
	loadBackgrounds(hDC);

	// Pressed icon
	pressedIcon = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), pressed_icon);
	pressedIcon->loadImage(hDC, rutaImgCompleta, mainScreenConfig->iconWidth, mainScreenConfig->iconWidth, PIXFMT_32BPP_ARGB, 1, TRUE);

	// Bubbles
	bubbleNotif = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_notif.image);
	bubbleNotif->loadImage(hDC, rutaImgCompleta, UINT(mainScreenConfig->iconWidth * bubble_notif.width / 100.0), UINT(mainScreenConfig->iconWidth * bubble_notif.height / 100.0), PIXFMT_32BPP_ARGB, 1, TRUE);

	bubbleState = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_state.image);
	bubbleState->loadImage(hDC, rutaImgCompleta, UINT(mainScreenConfig->iconWidth * bubble_state.width / 100.0), UINT(mainScreenConfig->iconWidth * bubble_state.height / 100.0), PIXFMT_32BPP_ARGB, 1, TRUE);

	bubbleAlarm = new CIcono();
	getAbsolutePath(rutaImgCompleta, CountOf(rutaImgCompleta), bubble_alarm.image);
	bubbleAlarm->loadImage(hDC, rutaImgCompleta, UINT(mainScreenConfig->iconWidth * bubble_alarm.width / 100.0), UINT(mainScreenConfig->iconWidth * bubble_alarm.height / 100.0), PIXFMT_32BPP_ARGB, 1, TRUE);
}

void CConfiguracion::loadSounds()
{
	if (!this->soundsEnabled) {
		return;
	}

	TCHAR fullPath[MAX_PATH];

	this->getAbsolutePath(fullPath, CountOf(fullPath), this->soundOnLaunchIcon);
	this->soundOnLaunchIcon_bytes = LoadFileData(fullPath, MAX_SND_MEMORY);

	this->getAbsolutePath(fullPath, CountOf(fullPath), this->pressed_sound);
	this->pressed_sound_bytes = LoadFileData(fullPath, MAX_SND_MEMORY);

	this->getAbsolutePath(fullPath, CountOf(fullPath), this->change_screen_sound);
	this->change_screen_sound_bytes = LoadFileData(fullPath, MAX_SND_MEMORY);
}

void CConfiguracion::defaultValues()
{
	this->mainScreenConfig->defaultValues();
	this->bottomBarConfig->defaultValues();
	this->topBarConfig->defaultValues();

	this->mainScreenConfig->cs.offset.top = 5;
	this->mainScreenConfig->cs.minHorizontalSpace = 5;
	this->bottomBarConfig->cs.offset.top = 5;
	//this->bottomBarConfig->cs.minHorizontalSpace = 5;

	this->circlesDiameter = 7;
	this->circlesDiameterActivePerc = 30;
	this->circlesDistance = 3;
	this->circlesOffset = 3;
	this->circlesAlignTop = 0;
	this->circlesColorActive = RGB(220, 220, 220);
	this->circlesColorInactive = RGB(0, 0, 0);
	this->circlesColorOuter = RGB(255, 255, 255);
	this->circlesSingleTap = 1;
	this->circlesDoubleTap = 1;

	this->headerTextFacename[0] = 0;
	this->headerTextSize = 0;
	this->headerTextColor = RGB(255, 255, 255);
	this->headerTextWeight = 900;
	this->headerOffset = 0;
	this->headerTextShadow = 1;
	this->headerTextRoundRect = 0;

	this->fondoTransparente = 1;
	this->fondoColor = RGB(0, 0, 0);
	this->fondoEstatico = 1;
	this->fondoFitWidth = 1;
	this->fondoFitHeight = 1;
	this->fondoCenter = 1;
	this->fondoTile = 0;
	this->fondoFactor = 1;
	this->strFondoPantalla[0] = 0;

	this->moveThreshold = 7;
	this->velMaxima = 140;
	this->velMinima = 20;
	this->refreshTime = 20;
	this->factorMovimiento = 4;
	this->verticalScroll = 0;
	this->freestyleScroll = 0;

	this->dowUseLocale = 1;
	StringCchCopy(this->diasSemana[0], CountOf(this->diasSemana[0]), TEXT("Sun"));
	StringCchCopy(this->diasSemana[1], CountOf(this->diasSemana[1]), TEXT("Mon"));
	StringCchCopy(this->diasSemana[2], CountOf(this->diasSemana[2]), TEXT("Tue"));
	StringCchCopy(this->diasSemana[3], CountOf(this->diasSemana[3]), TEXT("Wed"));
	StringCchCopy(this->diasSemana[4], CountOf(this->diasSemana[4]), TEXT("Thu"));
	StringCchCopy(this->diasSemana[5], CountOf(this->diasSemana[5]), TEXT("Fri"));
	StringCchCopy(this->diasSemana[6], CountOf(this->diasSemana[6]), TEXT("Sat"));

	this->dow.facename[0] = 0;
	this->dow.color = RGB(255,255,255);
	this->dow.width = 18;
	this->dow.height = 40;
	this->dow.weight = 400;
	this->dow.offset.left = 0;
	this->dow.offset.top = 0;
	this->dow.offset.right = 0;
	this->dow.offset.bottom = 72;

	this->dom.facename[0] = 0;
	this->dom.color = RGB(30,30,30);
	this->dom.width = 30;
	this->dom.height = 80;
	this->dom.weight = 800;
	this->dom.offset.left = 0;
	this->dom.offset.top = 25;
	this->dom.offset.right = 0;
	this->dom.offset.bottom = 0;

	this->clck.facename[0] = 0;
	this->clck.color = RGB(230,230,230);
	this->clck.width = 13;
	this->clck.height = 60;
	this->clck.weight = 900;
	this->clck.offset.left = 0;
	this->clck.offset.top = 0;
	this->clck.offset.right = 0;
	this->clck.offset.bottom = 0;

	this->clckShowAMPM = 0;
	this->clock12Format = 0;

	this->alrm.facename[0] = 0;
	this->alrm.color = RGB(230,230,230);
	this->alrm.width = 10;
	this->alrm.height = 28;
	this->alrm.weight = 400;
	this->alrm.offset.left = 0;
	this->alrm.offset.top = 70;
	this->alrm.offset.right = 0;
	this->alrm.offset.bottom = 0;

	this->alrmShowAMPM = 1;

	this->batt.facename[0] = 0;
	this->batt.color = RGB(230,230,230);
	this->batt.width = 0;
	this->batt.height = 35;
	this->batt.weight = 400;
	this->batt.offset.left = 0;
	this->batt.offset.top = 7;
	this->batt.offset.right = 0;
	this->batt.offset.bottom = 70;

	this->battShowAC = 0;
	this->battShowPercentage = 1;
	wcscpy(this->battChargingSymbol, L".");

	this->vol.facename[0] = 0;
	this->vol.color = RGB(230,230,230);
	this->vol.width = 0;
	this->vol.height = 35;
	this->vol.weight = 400;
	this->vol.offset.left = 0;
	this->vol.offset.top = 7;
	this->vol.offset.right = 0;
	this->vol.offset.bottom = 70;

	this->volShowPercentage = 1;

	this->meml.facename[0] = 0;
	this->meml.color = RGB(230,230,230);
	this->meml.width = 0;
	this->meml.height = 35;
	this->meml.weight = 400;
	this->meml.offset.left = 0;
	this->meml.offset.top = 7;
	this->meml.offset.right = 0;
	this->meml.offset.bottom = 70;

	this->memlShowPercentage = 1;

	this->memf.facename[0] = 0;
	this->memf.color = RGB(230,230,230);
	this->memf.width = 0;
	this->memf.height = 35;
	this->memf.weight = 400;
	this->memf.offset.left = 0;
	this->memf.offset.top = 7;
	this->memf.offset.right = 0;
	this->memf.offset.bottom = 70;

	this->memfShowMB = 0;
	this->memOSUsedKB = 0;

	this->memu.facename[0] = 0;
	this->memu.color = RGB(230,230,230);
	this->memu.width = 0;
	this->memu.height = 35;
	this->memu.weight = 400;
	this->memu.offset.left = 0;
	this->memu.offset.top = 7;
	this->memu.offset.right = 0;
	this->memu.offset.bottom = 70;

	this->memuShowMB = 0;

	this->psig.facename[0] = 0;
	this->psig.color = RGB(230,230,230);
	this->psig.width = 0;
	this->psig.height = 35;
	this->psig.weight = 400;
	this->psig.offset.left = 0;
	this->psig.offset.top = 7;
	this->psig.offset.right = 0;
	this->psig.offset.bottom = 70;

	this->psigShowPercentage = 1;

	this->wsig.facename[0] = 0;
	this->wsig.color = RGB(230,230,230);
	this->wsig.width = 0;
	this->wsig.height = 28;
	this->wsig.weight = 400;
	this->wsig.offset.left = 0;
	this->wsig.offset.top = 7;
	this->wsig.offset.right = 0;
	this->wsig.offset.bottom = 70;

	this->wsigShowdBm = 1;

	this->animationEffect = 5;
	this->animationColor = RGB(0,0,0);
	this->animationDuration = 300;
	this->animationDelay = 500;
	this->launchAppAtBeginningOfAnimation = 0;

	this->closeOnLaunchIcon = 0;
	this->minimizeOnLaunchIcon = 0;
	this->vibrateOnLaunchIcon = 0;
	this->soundOnLaunchIcon[0] = 0;
	this->soundOnLaunchIcon_bytes = NULL;
	this->runTool[0] = 0;

	StringCchCopy(this->pressed_icon, CountOf(this->pressed_icon), TEXT("Pressed\\RoundedPressed.png"));
	this->pressed_sound[0] = 0;
	this->pressed_sound_bytes = NULL;

	this->change_screen_sound[0] = 0;
	this->change_screen_sound_bytes = NULL;

	this->notifyTimer = 2000;
	this->updateWhenInactive = 0;
	this->ignoreRotation = 0;
	this->ignoreMinimize = 0;
	this->disableRightClick = 0;
	this->disableRightClickDots = 0;
	this->fullscreen = 0;
	this->neverShowTaskBar = 0;
	this->noWindowTitle = 0;
	this->showExit = 1;
	this->textQuality = 0;
	this->textQualityInIcons = 0;
	this->autoShowKeyboardOnTextboxFocus = 0;
	this->soundsEnabled = 1;
	this->heightP = 0;
	this->heightL = 0;

	this->ooss_left.stop = 0;
	this->ooss_left.wrap = 1;
	this->ooss_left.stopAt = 40;
	this->ooss_left.exec[0] = 0;

	this->ooss_right.stop = 0;
	this->ooss_right.wrap = 1;
	this->ooss_right.stopAt = 40;
	this->ooss_right.exec[0] = 0;

	this->ooss_up.stop = 0;
	this->ooss_up.wrap = 0;
	this->ooss_up.stopAt = 40;
	this->ooss_up.exec[0] = 0;

	this->ooss_down.stop = 0;
	this->ooss_down.wrap = 0;
	this->ooss_down.stopAt = 40;
	this->ooss_down.exec[0] = 0;

	this->alphaBlend = 0;
	this->alphaOnBlack = 0;
	this->alphaThreshold = 25;
	this->transparentBMP = 1;
	this->useMask = 0;

	this->lastConfiguredAtWidth = 240;
	this->lastConfiguredAtHeight = 320;

	memset(&this->bubble_notif, 0, sizeof(BubbleSettings));
	this->bubble_notif.image[0] = 0;
	this->bubble_notif.x = 55;
	this->bubble_notif.y = -10;
	this->bubble_notif.width = 63;
	this->bubble_notif.height = 63;
	this->bubble_notif.sis.facename[0] = 0;
	this->bubble_notif.sis.color = RGB(255, 255, 255);
	this->bubble_notif.sis.width = 0;
	this->bubble_notif.sis.height = 25;
	this->bubble_notif.sis.weight = 700;
	this->bubble_notif.sis.offset.left = 0;
	this->bubble_notif.sis.offset.top = 0;
	this->bubble_notif.sis.offset.right = 0;
	this->bubble_notif.sis.offset.bottom = 0;

	memset(&this->bubble_alarm, 0, sizeof(BubbleSettings));
	this->bubble_alarm.image[0] = 0;
	this->bubble_alarm.x = 55;
	this->bubble_alarm.y = -10;
	this->bubble_alarm.width = 63;
	this->bubble_alarm.height = 63;
	this->bubble_alarm.sis.facename[0] = 0;
	this->bubble_alarm.sis.color = RGB(255, 255, 255);
	this->bubble_alarm.sis.width = 0;
	this->bubble_alarm.sis.height = 25;
	this->bubble_alarm.sis.weight = 700;
	this->bubble_alarm.sis.offset.left = 0;
	this->bubble_alarm.sis.offset.top = 0;
	this->bubble_alarm.sis.offset.right = 0;
	this->bubble_alarm.sis.offset.bottom = 0;

	memset(&this->bubble_state, 0, sizeof(BubbleSettings));
	this->bubble_state.image[0] = 0;
	this->bubble_state.x = 70;
	this->bubble_state.y = 70;
	this->bubble_state.width = 30;
	this->bubble_state.height = 30;

	if (isPND()) {
		this->disableRightClickDots = 1;
		this->fullscreen = 1;
	} else {
		StringCchCopy(this->bubble_notif.image, CountOf(this->bubble_notif.image), TEXT("Bubbles\\Notif.png"));
		StringCchCopy(this->bubble_state.image, CountOf(this->bubble_notif.image), TEXT("Bubbles\\State.png"));
		StringCchCopy(this->bubble_alarm.image, CountOf(this->bubble_notif.image), TEXT("Bubbles\\Alarm.png"));
	}

	if (isPhone()) {
		this->vibrateOnLaunchIcon = 40;
	}

	this->AutoScale();
}

BOOL CConfiguracion::AutoScale()
{
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	if (width > height) {
		int tmp = height;
		height = width;
		width = tmp;
	}

	if (width == this->lastConfiguredAtWidth) {
		return FALSE;
	}

	if (this->lastConfiguredAtWidth == 0) {
		this->lastConfiguredAtWidth = width;
		this->lastConfiguredAtHeight = height;
		return FALSE;
	}

	double scale = 1.0 * width / this->lastConfiguredAtWidth;

	this->mainScreenConfig->Scale(scale);
	this->bottomBarConfig->Scale(scale);
	this->topBarConfig->Scale(scale);

	this->circlesDiameter = UINT(scale * this->circlesDiameter);
	if (this->circlesDistance > 0) {
		this->circlesDistance = UINT(scale * this->circlesDistance);
	}
	this->circlesOffset = INT(scale * this->circlesOffset);

	this->headerTextSize = UINT(scale * this->headerTextSize);
	this->headerOffset = UINT(scale * this->headerOffset);

	this->moveThreshold = UINT(scale * this->moveThreshold);

	this->lastConfiguredAtWidth = width;
	this->lastConfiguredAtHeight = height;

	return TRUE;
}

void SpecialIconSettingsLoad(TiXmlElement *pElem, SpecialIconSettings *sis)
{
	XMLUtils::GetAttr(pElem, "facename", sis->facename, CountOf(sis->facename));
	XMLUtils::GetAttr(pElem, "color",    &sis->color);
	XMLUtils::GetAttr(pElem, "width",    &sis->width);
	XMLUtils::GetAttr(pElem, "height",   &sis->height);
	XMLUtils::GetAttr(pElem, "weight",   &sis->weight);
	XMLUtils::GetAttr(pElem, "left",     &sis->offset.left);
	XMLUtils::GetAttr(pElem, "top",      &sis->offset.top);
	XMLUtils::GetAttr(pElem, "right",    &sis->offset.right);
	XMLUtils::GetAttr(pElem, "bottom",   &sis->offset.bottom);
}

void SpecialIconSettingsSave(TiXmlElement *pElem, SpecialIconSettings *sis)
{
	XMLUtils::SetAttr(pElem, "facename", sis->facename, CountOf(sis->facename));
	XMLUtils::SetAttr(pElem, "color",    sis->color);
	XMLUtils::SetAttr(pElem, "width",    sis->width);
	XMLUtils::SetAttr(pElem, "height",   sis->height);
	XMLUtils::SetAttr(pElem, "weight",   sis->weight);
	XMLUtils::SetAttr(pElem, "left",     sis->offset.left);
	XMLUtils::SetAttr(pElem, "top",      sis->offset.top);
	XMLUtils::SetAttr(pElem, "right",    sis->offset.right);
	XMLUtils::SetAttr(pElem, "bottom",   sis->offset.bottom);
}

void BubbleSettingsLoad(TiXmlElement *pElem, BubbleSettings *bs)
{
	XMLUtils::GetAttr(pElem, "image",  bs->image, CountOf(bs->image));
	XMLUtils::GetAttr(pElem, "x",      &bs->x);
	XMLUtils::GetAttr(pElem, "y",      &bs->y);
	XMLUtils::GetAttr(pElem, "width",  &bs->width);
	XMLUtils::GetAttr(pElem, "height", &bs->height);
	for (TiXmlElement *pElem2 = pElem->FirstChildElement(); pElem2; pElem2 = pElem2->NextSiblingElement()) {
		const char *nameNode = pElem2->Value();
		if(_stricmp(nameNode, "Text") == 0) {
			SpecialIconSettingsLoad(pElem2, &bs->sis);
		}
	}
}

void BubbleSettingsSave(TiXmlElement *pElem, BubbleSettings *bs, BOOL hasText)
{
	XMLUtils::SetAttr(pElem, "image",  bs->image, CountOf(bs->image));
	XMLUtils::SetAttr(pElem, "x",      bs->x);
	XMLUtils::SetAttr(pElem, "y",      bs->y);
	XMLUtils::SetAttr(pElem, "width",  bs->width);
	XMLUtils::SetAttr(pElem, "height", bs->height);
	if (hasText) {
		TiXmlElement *pElem2 = new TiXmlElement("Text");
		SpecialIconSettingsSave(pElem2, &bs->sis);
		pElem->LinkEndChild(pElem2);
	}
}

void OutOfScreenSettingsLoad(TiXmlElement *pElem, OutOfScreenSettings *ooss)
{
	XMLUtils::GetAttr(pElem, "stop",   &ooss->stop);
	XMLUtils::GetAttr(pElem, "stopAt", &ooss->stopAt);
	XMLUtils::GetAttr(pElem, "wrap",   &ooss->wrap);
	XMLUtils::GetAttr(pElem, "exec",   ooss->exec, CountOf(ooss->exec));
}

void OutOfScreenSettingsSave(TiXmlElement *pElem, OutOfScreenSettings *ooss)
{
	XMLUtils::SetAttr(pElem, "stop",   ooss->stop);
	XMLUtils::SetAttr(pElem, "stopAt", ooss->stopAt);
	XMLUtils::SetAttr(pElem, "wrap",   ooss->wrap);
	XMLUtils::SetAttr(pElem, "exec",   ooss->exec, CountOf(ooss->exec));
}

BOOL CConfiguracion::loadXMLConfig()
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
			XMLUtils::GetAttr(pElem, "diameter",      &this->circlesDiameter);
			XMLUtils::GetAttr(pElem, "diameterActivePerc", &this->circlesDiameterActivePerc);
			XMLUtils::GetAttr(pElem, "distance",      &this->circlesDistance);
			XMLUtils::GetAttr(pElem, "offset",        &this->circlesOffset);
			XMLUtils::GetAttr(pElem, "alignTop",      &this->circlesAlignTop);
			XMLUtils::GetAttr(pElem, "colorActive",   &this->circlesColorActive);
			XMLUtils::GetAttr(pElem, "colorInactive", &this->circlesColorInactive);
			XMLUtils::GetAttr(pElem, "colorOuter",    &this->circlesColorOuter);
			XMLUtils::GetAttr(pElem, "singletap",     &this->circlesSingleTap);
			XMLUtils::GetAttr(pElem, "doubletap",     &this->circlesDoubleTap);
		} else if(_stricmp(nameNode, "Header") == 0) {
			XMLUtils::GetAttr(pElem, "facename",  this->headerTextFacename, CountOf(this->headerTextFacename));
			XMLUtils::GetAttr(pElem, "size",      &this->headerTextSize);
			XMLUtils::GetAttr(pElem, "color",     &this->headerTextColor);
			XMLUtils::GetAttr(pElem, "weight",    &this->headerTextWeight);
			XMLUtils::GetAttr(pElem, "offset",    &this->headerOffset);
			XMLUtils::GetAttr(pElem, "shadow",    &this->headerTextShadow);
			XMLUtils::GetAttr(pElem, "roundrect", &this->headerTextRoundRect);
		} else if(_stricmp(nameNode, "Movement") == 0) {
			XMLUtils::GetAttr(pElem, "MoveThreshold",   &this->moveThreshold);
			XMLUtils::GetAttr(pElem, "MaxVelocity",     &this->velMaxima);
			XMLUtils::GetAttr(pElem, "MinVelocity",     &this->velMinima);
			XMLUtils::GetAttr(pElem, "RefreshTime",     &this->refreshTime);
			XMLUtils::GetAttr(pElem, "FactorMov",       &this->factorMovimiento);
			XMLUtils::GetAttr(pElem, "VerticalScroll",  &this->verticalScroll);
			XMLUtils::GetAttr(pElem, "FreestyleScroll", &this->freestyleScroll);
		} else if(_stricmp(nameNode, "DayOfWeek") == 0) {
			SpecialIconSettingsLoad(pElem, &this->dow);
			XMLUtils::GetAttr(pElem, "UseLocale", &dowUseLocale);
			XMLUtils::GetAttr(pElem, "Sunday",    this->diasSemana[0], CountOf(this->diasSemana[0]));
			XMLUtils::GetAttr(pElem, "Monday",    this->diasSemana[1], CountOf(this->diasSemana[1]));
			XMLUtils::GetAttr(pElem, "Tuesday",   this->diasSemana[2], CountOf(this->diasSemana[2]));
			XMLUtils::GetAttr(pElem, "Wednesday", this->diasSemana[3], CountOf(this->diasSemana[3]));
			XMLUtils::GetAttr(pElem, "Thursday",  this->diasSemana[4], CountOf(this->diasSemana[4]));
			XMLUtils::GetAttr(pElem, "Friday",    this->diasSemana[5], CountOf(this->diasSemana[5]));
			XMLUtils::GetAttr(pElem, "Saturday",  this->diasSemana[6], CountOf(this->diasSemana[6]));
		} else if(_stricmp(nameNode, "DayOfMonth") == 0) {
			SpecialIconSettingsLoad(pElem, &this->dom);
		} else if(_stricmp(nameNode, "Clock") == 0) {
			SpecialIconSettingsLoad(pElem, &this->clck);
			XMLUtils::GetAttr(pElem, "ShowAMPM", &this->clckShowAMPM);
			XMLUtils::GetAttr(pElem, "format12", &this->clock12Format);
		} else if(_stricmp(nameNode, "Alarm") == 0) {
			SpecialIconSettingsLoad(pElem, &this->alrm);
			XMLUtils::GetAttr(pElem, "ShowAMPM", &this->alrmShowAMPM);
		} else if(_stricmp(nameNode, "Battery") == 0) {
			SpecialIconSettingsLoad(pElem, &this->batt);
			XMLUtils::GetAttr(pElem, "AC", &this->battShowAC);
			XMLUtils::GetAttr(pElem, "ShowPercentage", &this->battShowPercentage);
			XMLUtils::GetAttr(pElem, "ChargingSymbol", this->battChargingSymbol, CountOf(this->battChargingSymbol));
		} else if(_stricmp(nameNode, "Volume") == 0) {
			SpecialIconSettingsLoad(pElem, &this->vol);
			XMLUtils::GetAttr(pElem, "ShowPercentage", &this->volShowPercentage);
		} else if(_stricmp(nameNode, "MemoryLoad") == 0) {
			SpecialIconSettingsLoad(pElem, &this->meml);
			XMLUtils::GetAttr(pElem, "ShowPercentage", &this->memlShowPercentage);
		} else if(_stricmp(nameNode, "MemoryFree") == 0) {
			SpecialIconSettingsLoad(pElem, &this->memf);
			XMLUtils::GetAttr(pElem, "ShowMB", &this->memfShowMB);
			XMLUtils::GetAttr(pElem, "OSUsedKB", &this->memOSUsedKB);
		} else if(_stricmp(nameNode, "MemoryUsed") == 0) {
			SpecialIconSettingsLoad(pElem, &this->memu);
			XMLUtils::GetAttr(pElem, "ShowMB", &this->memuShowMB);
		} else if(_stricmp(nameNode, "PhoneSignalStrength") == 0) {
			SpecialIconSettingsLoad(pElem, &this->psig);
			XMLUtils::GetAttr(pElem, "ShowPercentage", &this->psigShowPercentage);
		} else if(_stricmp(nameNode, "WifiSignalStrength") == 0) {
			SpecialIconSettingsLoad(pElem, &this->wsig);
			XMLUtils::GetAttr(pElem, "ShowdBm", &this->wsigShowdBm);
		} else if(_stricmp(nameNode, "BubbleNotif") == 0) {
			BubbleSettingsLoad(pElem, &this->bubble_notif);
		} else if(_stricmp(nameNode, "BubbleAlarm") == 0) {
			BubbleSettingsLoad(pElem, &this->bubble_alarm);
		} else if(_stricmp(nameNode, "BubbleState") == 0) {
			BubbleSettingsLoad(pElem, &this->bubble_state);
		} else if(_stricmp(nameNode, "Animation") == 0) {
			XMLUtils::GetAttr(pElem, "effect",   &this->animationEffect);
			XMLUtils::GetAttr(pElem, "color",    &this->animationColor);
			XMLUtils::GetAttr(pElem, "duration", &this->animationDuration);
			XMLUtils::GetAttr(pElem, "delay",    &this->animationDelay);
			XMLUtils::GetAttr(pElem, "LaunchAppAtBeginningOfAnimation", &this->launchAppAtBeginningOfAnimation);
		} else if(_stricmp(nameNode, "OnLaunchIcon") == 0) {
			XMLUtils::GetAttr(pElem, "close",    &this->closeOnLaunchIcon);
			XMLUtils::GetAttr(pElem, "minimize", &this->minimizeOnLaunchIcon);
			XMLUtils::GetAttr(pElem, "vibrate",  &this->vibrateOnLaunchIcon);
			if (!XMLUtils::GetAttr(pElem, "wav", this->soundOnLaunchIcon, CountOf(this->soundOnLaunchIcon))) {
				XMLUtils::GetAttr(pElem, "sound", this->soundOnLaunchIcon, CountOf(this->soundOnLaunchIcon));
			}
			XMLUtils::GetAttr(pElem, "RunTool", this->runTool, CountOf(this->runTool));
		} else if(_stricmp(nameNode, "OnPressIcon") == 0) {
			XMLUtils::GetAttr(pElem, "icon",  this->pressed_icon,  CountOf(this->pressed_icon));
			XMLUtils::GetAttr(pElem, "sound", this->pressed_sound, CountOf(this->pressed_sound));
		} else if(_stricmp(nameNode, "OnChangeScreen") == 0) {
			XMLUtils::GetAttr(pElem, "sound", this->change_screen_sound, CountOf(this->change_screen_sound));
		} else if(_stricmp(nameNode, "Background") == 0) {
			XMLUtils::GetAttr(pElem, "transparent", &this->fondoTransparente);
			XMLUtils::GetAttr(pElem, "color",       &this->fondoColor);
			XMLUtils::GetAttr(pElem, "static",      &this->fondoEstatico);
			XMLUtils::GetAttr(pElem, "fitwidth",    &this->fondoFitWidth);
			XMLUtils::GetAttr(pElem, "fitheight",   &this->fondoFitHeight);
			XMLUtils::GetAttr(pElem, "center",      &this->fondoCenter);
			XMLUtils::GetAttr(pElem, "tile",        &this->fondoTile);
			XMLUtils::GetAttr(pElem, "factor",      &this->fondoFactor);
			XMLUtils::GetAttr(pElem, "wallpaper",   this->strFondoPantalla, CountOf(this->strFondoPantalla));
		} else if(_stricmp(nameNode, "NotifyTimer") == 0) {
			XMLUtils::GetAttr(pElem, "UpdateWhenInactive", &this->updateWhenInactive);
			XMLUtils::GetTextElem(pElem, &this->notifyTimer);
		} else if(_stricmp(nameNode, "IgnoreRotation") == 0) {
			XMLUtils::GetTextElem(pElem, &this->ignoreRotation);
		} else if(_stricmp(nameNode, "IgnoreMinimize") == 0) {
			XMLUtils::GetTextElem(pElem, &this->ignoreMinimize);
		} else if(_stricmp(nameNode, "DisableRightClick") == 0) {
			XMLUtils::GetTextElem(pElem, &this->disableRightClick);
		} else if(_stricmp(nameNode, "DisableRightClickDots") == 0) {
			XMLUtils::GetTextElem(pElem, &this->disableRightClickDots);
		} else if(_stricmp(nameNode, "Fullscreen") == 0) {
			XMLUtils::GetTextElem(pElem, &this->fullscreen);
		} else if(_stricmp(nameNode, "NeverShowTaskBar") == 0) {
			XMLUtils::GetTextElem(pElem, &this->neverShowTaskBar);
		} else if(_stricmp(nameNode, "NoWindowTitle") == 0) {
			XMLUtils::GetTextElem(pElem, &this->noWindowTitle);
		} else if(_stricmp(nameNode, "ShowExit") == 0) {
			XMLUtils::GetTextElem(pElem, &this->showExit);
		} else if(_stricmp(nameNode, "TextQuality") == 0) {
			XMLUtils::GetTextElem(pElem, &this->textQuality);
		} else if(_stricmp(nameNode, "TextQualityInIcons") == 0) {
			XMLUtils::GetTextElem(pElem, &this->textQualityInIcons);
		} else if(_stricmp(nameNode, "AutoShowKeyboardOnTextboxFocus") == 0) {
			XMLUtils::GetTextElem(pElem, &this->autoShowKeyboardOnTextboxFocus);
		} else if(_stricmp(nameNode, "Sounds") == 0) {
			XMLUtils::GetTextElem(pElem, &this->soundsEnabled);
		} else if(_stricmp(nameNode, "OutOfScreenLeft") == 0) {
			OutOfScreenSettingsLoad(pElem, &this->ooss_left);
		} else if(_stricmp(nameNode, "OutOfScreenRight") == 0) {
			OutOfScreenSettingsLoad(pElem, &this->ooss_right);
		} else if(_stricmp(nameNode, "OutOfScreenUp") == 0) {
			OutOfScreenSettingsLoad(pElem, &this->ooss_up);
		} else if(_stricmp(nameNode, "OutOfScreenDown") == 0) {
			OutOfScreenSettingsLoad(pElem, &this->ooss_down);
		} else if(_stricmp(nameNode, "Transparency") == 0) {
			XMLUtils::GetAttr(pElem, "alphablend",     &this->alphaBlend);
			XMLUtils::GetAttr(pElem, "alphaonblack",   &this->alphaOnBlack);
			XMLUtils::GetAttr(pElem, "threshold",      &this->alphaThreshold);
			XMLUtils::GetAttr(pElem, "transparentbmp", &this->transparentBMP);
			XMLUtils::GetAttr(pElem, "mask",           &this->useMask);
		} else if(_stricmp(nameNode, "TodayItemHeight") == 0) {
			XMLUtils::GetAttr(pElem, "portrait",  &this->heightP);
			XMLUtils::GetAttr(pElem, "landscape", &this->heightL);
		} else if(_stricmp(nameNode, "LastConfiguredAt") == 0) {
			XMLUtils::GetAttr(pElem, "width",  &this->lastConfiguredAtWidth);
			XMLUtils::GetAttr(pElem, "height", &this->lastConfiguredAtHeight);
		} else if(_stricmp(nameNode, "MainScreen") == 0) {
			mainScreenConfig->loadXMLConfig(pElem);
		} else if(_stricmp(nameNode, "BottomBar") == 0) {
			bottomBarConfig->loadXMLConfig(pElem);
		} else if(_stricmp(nameNode, "TopBar") == 0) {
			topBarConfig->loadXMLConfig(pElem);
		}
    }

	this->AutoScale();

	return TRUE;
}

BOOL CConfiguracion::saveXMLIcons(CListaPantalla *listaPantallas)
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

	FILE *f = _wfopen(pathIconsXML, L"w");
	doc.SetUseMicrosoftBOM(true);
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

		if (icon->nombre[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "name", icon->nombre, CountOf(icon->nombre));
		}
		if (icon->rutaImagen[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "image", icon->rutaImagen, CountOf(icon->rutaImagen));
		}
		if (icon->sound[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "sound", icon->sound, CountOf(icon->sound));
		}
		if (icon->ejecutable[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "exec", icon->ejecutable, CountOf(icon->ejecutable));
		}
		if (icon->parametros[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "parameters", icon->parametros, CountOf(icon->parametros));
		}
		if (icon->ejecutableAlt[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "execAlt", icon->ejecutableAlt, CountOf(icon->ejecutableAlt));
		}
		if (icon->parametrosAlt[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "parametersAlt", icon->parametrosAlt, CountOf(icon->parametrosAlt));
		}
		if (icon->tipo != 0) {
			XMLUtils::SetAttr(pElemIcon, "type", icon->tipo);
		}
		if (icon->typeParams[0] != 0) {
			XMLUtils::SetAttr(pElemIcon, "typeParams", icon->typeParams, CountOf(icon->typeParams));
		}
		XMLUtils::SetAttr(pElemIcon, "animation", icon->launchAnimation);

		pElemScreen->LinkEndChild(pElemIcon);
	}

	return TRUE;
}

BOOL CConfiguracion::saveXMLConfig()
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
	mainScreenConfig->saveXMLConfig(pElem, FALSE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("BottomBar");
	bottomBarConfig->saveXMLConfig(pElem, TRUE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("TopBar");
	topBarConfig->saveXMLConfig(pElem, TRUE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Circles");
	XMLUtils::SetAttr(pElem, "diameter",      this->circlesDiameter);
	XMLUtils::SetAttr(pElem, "diameterActivePerc", this->circlesDiameterActivePerc);
	XMLUtils::SetAttr(pElem, "distance",      this->circlesDistance);
	XMLUtils::SetAttr(pElem, "offset",        this->circlesOffset);
	XMLUtils::SetAttr(pElem, "alignTop",      this->circlesAlignTop);
	XMLUtils::SetAttr(pElem, "colorActive",   this->circlesColorActive);
	XMLUtils::SetAttr(pElem, "colorInactive", this->circlesColorInactive);
	XMLUtils::SetAttr(pElem, "colorOuter",    this->circlesColorOuter);
	XMLUtils::SetAttr(pElem, "singletap",     this->circlesSingleTap);
	XMLUtils::SetAttr(pElem, "doubletap",     this->circlesDoubleTap);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Header");
	XMLUtils::SetAttr(pElem, "facename",  this->headerTextFacename, CountOf(this->headerTextFacename));
	XMLUtils::SetAttr(pElem, "size",      this->headerTextSize);
	XMLUtils::SetAttr(pElem, "color",     this->headerTextColor);
	XMLUtils::SetAttr(pElem, "weight",    this->headerTextWeight);
	XMLUtils::SetAttr(pElem, "offset",    this->headerOffset);
	XMLUtils::SetAttr(pElem, "shadow",    this->headerTextShadow);
	XMLUtils::SetAttr(pElem, "roundrect", this->headerTextRoundRect);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Background");
	XMLUtils::SetAttr(pElem, "transparent", this->fondoTransparente);
	XMLUtils::SetAttr(pElem, "color",       this->fondoColor);
	XMLUtils::SetAttr(pElem, "static",      this->fondoEstatico);
	XMLUtils::SetAttr(pElem, "fitwidth",    this->fondoFitWidth);
	XMLUtils::SetAttr(pElem, "fitheight",   this->fondoFitHeight);
	XMLUtils::SetAttr(pElem, "center",      this->fondoCenter);
	XMLUtils::SetAttr(pElem, "tile",        this->fondoTile);
	XMLUtils::SetAttr(pElem, "factor",      this->fondoFactor);
	XMLUtils::SetAttr(pElem, "wallpaper",   this->strFondoPantalla, CountOf(this->strFondoPantalla));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Movement");
	XMLUtils::SetAttr(pElem, "MoveThreshold",   this->moveThreshold);
	XMLUtils::SetAttr(pElem, "MaxVelocity",     this->velMaxima);
	XMLUtils::SetAttr(pElem, "MinVelocity",     this->velMinima);
	XMLUtils::SetAttr(pElem, "RefreshTime",     this->refreshTime);
	XMLUtils::SetAttr(pElem, "FactorMov",       this->factorMovimiento);
	XMLUtils::SetAttr(pElem, "VerticalScroll",  this->verticalScroll);
	XMLUtils::SetAttr(pElem, "FreestyleScroll", this->freestyleScroll);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DayOfWeek");
	SpecialIconSettingsSave(pElem, &this->dow);
	XMLUtils::SetAttr(pElem, "UseLocale", dowUseLocale);
	XMLUtils::SetAttr(pElem, "Sunday",    this->diasSemana[0], CountOf(this->diasSemana[0]));
	XMLUtils::SetAttr(pElem, "Monday",    this->diasSemana[1], CountOf(this->diasSemana[1]));
	XMLUtils::SetAttr(pElem, "Tuesday",   this->diasSemana[2], CountOf(this->diasSemana[2]));
	XMLUtils::SetAttr(pElem, "Wednesday", this->diasSemana[3], CountOf(this->diasSemana[3]));
	XMLUtils::SetAttr(pElem, "Thursday",  this->diasSemana[4], CountOf(this->diasSemana[4]));
	XMLUtils::SetAttr(pElem, "Friday",    this->diasSemana[5], CountOf(this->diasSemana[5]));
	XMLUtils::SetAttr(pElem, "Saturday",  this->diasSemana[6], CountOf(this->diasSemana[6]));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DayOfMonth");
	SpecialIconSettingsSave(pElem, &this->dom);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Clock");
	SpecialIconSettingsSave(pElem, &this->clck);
	XMLUtils::SetAttr(pElem, "ShowAMPM", this->clckShowAMPM);
	XMLUtils::SetAttr(pElem, "format12", this->clock12Format);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Alarm");
	SpecialIconSettingsSave(pElem, &this->alrm);
	XMLUtils::SetAttr(pElem, "ShowAMPM", this->alrmShowAMPM);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Battery");
	SpecialIconSettingsSave(pElem, &this->batt);
	XMLUtils::SetAttr(pElem, "AC", this->battShowAC);
	XMLUtils::SetAttr(pElem, "ShowPercentage", this->battShowPercentage);
	XMLUtils::SetAttr(pElem, "ChargingSymbol", this->battChargingSymbol, CountOf(this->battChargingSymbol));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Volume");
	SpecialIconSettingsSave(pElem, &this->vol);
	XMLUtils::SetAttr(pElem, "ShowPercentage", this->volShowPercentage);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("MemoryLoad");
	SpecialIconSettingsSave(pElem, &this->meml);
	XMLUtils::SetAttr(pElem, "ShowPercentage", this->memlShowPercentage);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("MemoryFree");
	SpecialIconSettingsSave(pElem, &this->memf);
	XMLUtils::SetAttr(pElem, "ShowMB", this->memfShowMB);
	XMLUtils::SetAttr(pElem, "OSUsedKB", this->memOSUsedKB);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("MemoryUsed");
	SpecialIconSettingsSave(pElem, &this->memu);
	XMLUtils::SetAttr(pElem, "ShowMB", this->memuShowMB);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("PhoneSignalStrength");
	SpecialIconSettingsSave(pElem, &this->psig);
	XMLUtils::SetAttr(pElem, "ShowPercentage", this->psigShowPercentage);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("WifiSignalStrength");
	SpecialIconSettingsSave(pElem, &this->wsig);
	XMLUtils::SetAttr(pElem, "ShowdBm", this->wsigShowdBm);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("BubbleNotif");
	BubbleSettingsSave(pElem, &this->bubble_notif, TRUE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("BubbleAlarm");
	BubbleSettingsSave(pElem, &this->bubble_alarm, FALSE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("BubbleState");
	BubbleSettingsSave(pElem, &this->bubble_state, FALSE);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Animation");
	XMLUtils::SetAttr(pElem, "effect",   this->animationEffect);
	XMLUtils::SetAttr(pElem, "color",    this->animationColor);
	XMLUtils::SetAttr(pElem, "duration", this->animationDuration);
	XMLUtils::SetAttr(pElem, "delay",    this->animationDelay);
	XMLUtils::SetAttr(pElem, "LaunchAppAtBeginningOfAnimation", this->launchAppAtBeginningOfAnimation);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OnLaunchIcon");
	XMLUtils::SetAttr(pElem, "close",    this->closeOnLaunchIcon);
	XMLUtils::SetAttr(pElem, "minimize", this->minimizeOnLaunchIcon);
	XMLUtils::SetAttr(pElem, "vibrate",  this->vibrateOnLaunchIcon);
	XMLUtils::SetAttr(pElem, "sound",    this->soundOnLaunchIcon, CountOf(this->soundOnLaunchIcon));
	XMLUtils::SetAttr(pElem, "RunTool",  this->runTool, CountOf(this->runTool));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OnPressIcon");
	XMLUtils::SetAttr(pElem, "icon",  this->pressed_icon,  CountOf(this->pressed_icon));
	XMLUtils::SetAttr(pElem, "sound", this->pressed_sound, CountOf(this->pressed_sound));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OnChangeScreen");
	XMLUtils::SetAttr(pElem, "sound", this->change_screen_sound, CountOf(this->change_screen_sound));
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("NotifyTimer");
	XMLUtils::SetAttr(pElem, "UpdateWhenInactive", this->updateWhenInactive);
	XMLUtils::SetTextElem(pElem, this->notifyTimer);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("IgnoreRotation");
	XMLUtils::SetTextElem(pElem, this->ignoreRotation);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("IgnoreMinimize");
	XMLUtils::SetTextElem(pElem, this->ignoreMinimize);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DisableRightClick");
	XMLUtils::SetTextElem(pElem, this->disableRightClick);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("DisableRightClickDots");
	XMLUtils::SetTextElem(pElem, this->disableRightClickDots);
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

	pElem = new TiXmlElement("ShowExit");
	XMLUtils::SetTextElem(pElem, this->showExit);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("TextQuality");
	XMLUtils::SetTextElem(pElem, this->textQuality);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("TextQualityInIcons");
	XMLUtils::SetTextElem(pElem, this->textQualityInIcons);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("AutoShowKeyboardOnTextboxFocus");
	XMLUtils::SetTextElem(pElem, this->autoShowKeyboardOnTextboxFocus);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Sounds");
	XMLUtils::SetTextElem(pElem, this->soundsEnabled);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OutOfScreenLeft");
	OutOfScreenSettingsSave(pElem, &this->ooss_left);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OutOfScreenRight");
	OutOfScreenSettingsSave(pElem, &this->ooss_right);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OutOfScreenUp");
	OutOfScreenSettingsSave(pElem, &this->ooss_up);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("OutOfScreenDown");
	OutOfScreenSettingsSave(pElem, &this->ooss_down);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("Transparency");
	XMLUtils::SetAttr(pElem, "alphablend",     this->alphaBlend);
	XMLUtils::SetAttr(pElem, "alphaonblack",   this->alphaOnBlack);
	XMLUtils::SetAttr(pElem, "threshold",      this->alphaThreshold);
	XMLUtils::SetAttr(pElem, "transparentbmp", this->transparentBMP);
	XMLUtils::SetAttr(pElem, "mask",           this->useMask);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("TodayItemHeight");
	XMLUtils::SetAttr(pElem, "portrait",  this->heightP);
	XMLUtils::SetAttr(pElem, "landscape", this->heightL);
	root->LinkEndChild(pElem);

	pElem = new TiXmlElement("LastConfiguredAt");
	XMLUtils::SetAttr(pElem, "width",  this->lastConfiguredAtWidth);
	XMLUtils::SetAttr(pElem, "height", this->lastConfiguredAtHeight);
	root->LinkEndChild(pElem);

	FILE *f = _wfopen(pathSettingsXML, L"w");
	doc.SetUseMicrosoftBOM(true);
	doc.SaveFile(f);
	fclose(f);

	lastModifiedSettingsXML = FileModifyTime(pathSettingsXML);

	return 0;
}
