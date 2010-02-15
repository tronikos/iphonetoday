#include "CConfiguracion.h"
#include "RegistryUtils.h"

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

	p = L"\\iPhoneToday\\";
	if (!FileOrDirExists(p, TRUE)) {
		p = pathExecutableDir;
	}
	StringCchPrintf(pathSettingsXML, CountOf(pathSettingsXML),   L"%s%s", p, L"settings.xml");
	StringCchPrintf(pathIconsXML,    CountOf(pathIconsXML),      L"%s%s", p, L"icons.xml");
	StringCchPrintf(pathIconsDir,    CountOf(pathIconsDir),      L"%s%s", p, L"icons\\");

	StringCchCopy(pathIconsXMLDir, CountOf(pathIconsXMLDir), pathIconsXML);
	p = wcsrchr(pathIconsXMLDir, '\\');
	if (p != NULL) *(p+1) = '\0';

	fondoPantalla = NULL;
	mainScreenConfig = new CConfigurationScreen();
	bottomBarConfig = new CConfigurationScreen();
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
void CConfiguracion::calculaConfiguracion(int maxIconos, int numIconsInBottomBar, int width, int height)
{
	if (width > 0) {
		anchoPantalla = width;
	}

	if (height > 0) {
		altoPantalla = height;
	}

	bottomBarConfig->calculate(TRUE, numIconsInBottomBar, anchoPantalla, altoPantalla);
	mainScreenConfig->calculate(FALSE, maxIconos, anchoPantalla, altoPantalla);

	altoPantallaMax = max(((maxIconos + mainScreenConfig->iconsPerRow - 1) / mainScreenConfig->iconsPerRow) * mainScreenConfig->distanceIconsV + mainScreenConfig->posReference.y * 2, altoPantalla);
}

BOOL CConfiguracion::cargaXMLIconos(CListaPantalla *listaPantallas)
{
	BOOL result = false;
	CPantalla *pantalla = NULL;
	CIcono *icono = NULL;

	HRESULT hr = S_OK;
    IXMLDOMDocument *pDOM = NULL;
    IXMLDOMNode *pRootNode = NULL;
    IXMLDOMNode *pNodeScreen = NULL;
    IXMLDOMNode *pNodeIcon = NULL;
	IXMLDOMNode *pNodeScreenSibling = NULL;
    IXMLDOMNode *pNodeIconSibling = NULL;
	
    IXMLDOMNamedNodeMap *pNodeMap = NULL;
    VARIANT_BOOL fSuccess;
    VARIANT vt;

	TCHAR *nameNode;

	// Load the XML DOM
	pDOM = DomFromCOM();
	if (!pDOM) goto Error;

	// Load the xml file
    vt.vt = VT_BSTR;
    vt.bstrVal = SysAllocString(pathIconsXML);
    CHR(pDOM->load(vt, &fSuccess));
    CBR(fSuccess);

	lastModifiedIconsXML = FileModifyTime(pathIconsXML);

	// Find the root node
    CHR(pDOM->selectSingleNode(TEXT("root"), &pRootNode));

	// Walk the children of this node -- each top-level child will be a single store property
	CHR(pRootNode->get_firstChild(&pNodeScreen));

	if (listaPantallas->barraInferior == NULL) {
		listaPantallas->barraInferior = new CPantalla();
	}
	UINT nIcon = 0;
	UINT nScreen = 0;
    while (pNodeScreen)
    {
		pNodeScreen->get_baseName(&nameNode);

		BOOL isScreen = FALSE;
		if (lstrcmpi(nameNode, TEXT("BottomBar")) == 0) {
			pantalla = listaPantallas->barraInferior;
			isScreen = TRUE;
		} else if (lstrcmpi(nameNode, TEXT("screen")) == 0) {
			if (listaPantallas->listaPantalla[nScreen] == NULL) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				pantalla = listaPantallas->listaPantalla[nScreen];
			}
			nScreen++;
			isScreen = TRUE;
		}
		if (isScreen) {
			CHR(pNodeScreen->get_attributes(&pNodeMap));
			ReadNodeString(pantalla->header, pNodeMap, TEXT("header"));
			RELEASE_OBJ(pNodeMap);

			CHR(pNodeScreen->get_firstChild(&pNodeIcon));

			nIcon = 0;
			while (pNodeIcon) 
			{
				pNodeIcon->get_baseName(&nameNode);

				if (lstrcmpi(nameNode, TEXT("icon")) == 0) {
					// Load in any attributes on this node
					CHR(pNodeIcon->get_attributes(&pNodeMap));

					// Init Icon creation
					if (pantalla->listaIconos[nIcon] == NULL) {
						icono = pantalla->creaIcono();
					} else {
						icono = pantalla->listaIconos[nIcon];

						wcscpy(icono->nombre, L"");
						wcscpy(icono->rutaImagen, L"");
						wcscpy(icono->ejecutable, L"");
						wcscpy(icono->parametros, L"");
						wcscpy(icono->ejecutableAlt, L"");
						wcscpy(icono->parametrosAlt, L"");
						icono->tipo = NOTIF_NORMAL;
						icono->launchAnimation = 1;
					}
					nIcon++;

					ReadNodeString(icono->nombre, pNodeMap, TEXT("name"));
					ReadNodeString(icono->rutaImagen, pNodeMap, TEXT("image"));
					ReadNodeString(icono->ejecutable, pNodeMap, TEXT("exec"));
					ReadNodeString(icono->parametros, pNodeMap, TEXT("parameters"));
					ReadNodeString(icono->ejecutableAlt, pNodeMap, TEXT("execAlt"));
					ReadNodeString(icono->parametrosAlt, pNodeMap, TEXT("parametersAlt"));
					icono->tipo = ReadNodeNumber(pNodeMap, TEXT("type"));
					icono->launchAnimation = ReadNodeNumber(pNodeMap, TEXT("animation"));

					// WriteToLog(icono->ejecutable);

					// End Icon creation
				}

				// Go to the next node
				CHR(pNodeIcon->get_nextSibling(&pNodeIconSibling));

				pNodeIcon->Release();
				pNodeIcon = pNodeIconSibling;

				RELEASE_OBJ(pNodeMap);
			}
			while (nIcon < pantalla->numIconos) {
				pantalla->borraIcono(nIcon);
				// nIcon++;
			}
		}

        // Go to the next node
		CHR(pNodeScreen->get_nextSibling(&pNodeScreenSibling));
        
        pNodeScreen->Release();
        pNodeScreen = pNodeScreenSibling;
    }
	while (nScreen < listaPantallas->numPantallas) {
		listaPantallas->numPantallas--;
		delete listaPantallas->listaPantalla[listaPantallas->numPantallas];
	}

	result = true;

Error:
    RELEASE_OBJ(pDOM)
    RELEASE_OBJ(pNodeScreen)
    RELEASE_OBJ(pNodeIcon)
	RELEASE_OBJ(pNodeScreenSibling)
	RELEASE_OBJ(pNodeIconSibling)
    RELEASE_OBJ(pNodeMap)

    VariantClear(&vt);
	return result;
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
		MessageBox(0, TEXT("Bad icons.xml, check for errors please. (Remember: No Special Characters and tag well formed)"), TEXT("Error"), MB_OK);
		if (listaPantallas->numPantallas == 0) {
			listaPantallas->creaPantalla();
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
			cargaImagenIcono(hDC, icono, FALSE);
		}
	}

	// Cargamos los iconos de la barra inferior
	pantalla = listaPantallas->barraInferior;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {

		icono = pantalla->listaIconos[j];
		cargaImagenIcono(hDC, icono, TRUE);
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

BOOL CConfiguracion::cargaImagenIcono(HDC *hDC, CIcono *icono, BOOL esBarraInferior)
{
	BOOL result;
	TCHAR rutaImgCompleta[MAX_PATH];
	UINT width;
	if (esBarraInferior) {
		width = bottomBarConfig->iconWidth;
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

	this->circlesDiameter = 15;
	this->circlesDistance = 7;

	this->headerFontSize = 0;
	this->headerFontWeight = 900;
	this->headerFontColor = RGB(255, 255, 255);
	this->headerOffset = 0;

	this->fondoTransparente = 1;
	this->fondoColor = RGB(0, 0, 0);
	this->fondoEstatico = 0;
	StringCchCopy(this->strFondoPantalla, CountOf(this->strFondoPantalla), L"");

	this->umbralMovimiento = 15;
	this->velMaxima = 140;
	this->velMinima = 20;
	this->refreshTime = 20;
	this->factorMovimiento = 4;
	this->verticalScroll = 0;

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
	BOOL result = false;
	HRESULT hr = S_OK;
	IXMLDOMDocument *pDOM = NULL;
	IXMLDOMNode *pRootNode = NULL;
	IXMLDOMNode *pNode = NULL;
	IXMLDOMNode *pNodeSibling = NULL;
	IXMLDOMNamedNodeMap *pNodeMap = NULL;
	TCHAR *nameNode = NULL;
    VARIANT_BOOL fSuccess;
    VARIANT vt;

	// Load the XML DOM
	pDOM = DomFromCOM();
	if (!pDOM) goto Error;

	// Load the xml file
    vt.vt = VT_BSTR;
    vt.bstrVal = SysAllocString(pathSettingsXML);
    CHR(pDOM->load(vt, &fSuccess));
    CBR(fSuccess);

	lastModifiedSettingsXML = FileModifyTime(pathSettingsXML);

	// Find the root node
    CHR(pDOM->selectSingleNode(TEXT("root"), &pRootNode));


	// Walk the children of this node -- each top-level child will be a single store property
	CHR(pRootNode->get_firstChild(&pNode));

    while (pNode)
    {
		pNode->get_baseName(&nameNode);
		// WriteToLog(TEXT("Bucle:\r\n"));

		if(lstrcmpi(nameNode, TEXT("Circles")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->circlesDiameter = ReadNodeNumber(pNodeMap, TEXT("diameter"), this->circlesDiameter);
			this->circlesDistance = ReadNodeNumber(pNodeMap, TEXT("distance"), this->circlesDistance);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Header")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->headerFontSize   = ReadNodeNumber(pNodeMap, TEXT("size"),   this->headerFontSize);
			this->headerFontColor  = ReadNodeNumber(pNodeMap, TEXT("color"),  this->headerFontColor);
			this->headerFontWeight = ReadNodeNumber(pNodeMap, TEXT("weight"), this->headerFontWeight);
			this->headerOffset     = ReadNodeNumber(pNodeMap, TEXT("offset"), this->headerOffset);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Movement")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->umbralMovimiento = ReadNodeNumber(pNodeMap, TEXT("MoveThreshold"),  this->umbralMovimiento);
			this->velMaxima        = ReadNodeNumber(pNodeMap, TEXT("MaxVelocity"),    this->velMaxima);
			this->velMinima        = ReadNodeNumber(pNodeMap, TEXT("MinVelocity"),    this->velMinima);
			this->refreshTime      = ReadNodeNumber(pNodeMap, TEXT("RefreshTime"),    this->refreshTime);
			this->factorMovimiento = ReadNodeNumber(pNodeMap, TEXT("FactorMov"),      this->factorMovimiento);
			this->verticalScroll   = ReadNodeNumber(pNodeMap, TEXT("VerticalScroll"), this->verticalScroll);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("DayOfWeek")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->dowColor  = ReadNodeNumber(pNodeMap,    TEXT("color"),     this->dowColor);
			this->dowWidth  = ReadNodeNumber(pNodeMap,    TEXT("width"),     this->dowWidth);
			this->dowHeight = ReadNodeNumber(pNodeMap,    TEXT("height"),    this->dowHeight);
			this->dowWeight = ReadNodeNumber(pNodeMap,    TEXT("weight"),    this->dowWeight);
			ReadNodeString(this->diasSemana[0], pNodeMap, TEXT("Sunday"),    this->diasSemana[0]);
			ReadNodeString(this->diasSemana[1], pNodeMap, TEXT("Monday"),    this->diasSemana[1]);
			ReadNodeString(this->diasSemana[2], pNodeMap, TEXT("Tuesday"),   this->diasSemana[2]);
			ReadNodeString(this->diasSemana[3], pNodeMap, TEXT("Wednesday"), this->diasSemana[3]);
			ReadNodeString(this->diasSemana[4], pNodeMap, TEXT("Thursday"),  this->diasSemana[4]);
			ReadNodeString(this->diasSemana[5], pNodeMap, TEXT("Friday"),    this->diasSemana[5]);
			ReadNodeString(this->diasSemana[6], pNodeMap, TEXT("Saturday"),  this->diasSemana[6]);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("DayOfMonth")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->domColor  = ReadNodeNumber(pNodeMap, TEXT("color"),  this->domColor);
			this->domWidth  = ReadNodeNumber(pNodeMap, TEXT("width"),  this->domWidth);
			this->domHeight = ReadNodeNumber(pNodeMap, TEXT("height"), this->domHeight);
			this->domWeight = ReadNodeNumber(pNodeMap, TEXT("weight"), this->domWeight);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Clock")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->clockColor    = ReadNodeNumber(pNodeMap, TEXT("color"),    this->clockColor);
			this->clockWidth    = ReadNodeNumber(pNodeMap, TEXT("width"),    this->clockWidth);
			this->clockHeight   = ReadNodeNumber(pNodeMap, TEXT("height"),   this->clockHeight);
			this->clockWeight   = ReadNodeNumber(pNodeMap, TEXT("weight"),   this->clockWeight);
			this->clock12Format = ReadNodeNumber(pNodeMap, TEXT("format12"), this->clock12Format);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Bubbles")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			ReadNodeString(this->bubble_notif, pNodeMap, TEXT("notif"), this->bubble_notif);
			ReadNodeString(this->bubble_state, pNodeMap, TEXT("state"), this->bubble_state);
			ReadNodeString(this->bubble_alarm, pNodeMap, TEXT("alarm"), this->bubble_alarm);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("OnLaunchIcon")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->closeOnLaunchIcon          = ReadNodeNumber(pNodeMap, TEXT("close"),   this->closeOnLaunchIcon);
			this->vibrateOnLaunchIcon        = ReadNodeNumber(pNodeMap, TEXT("vibrate"), this->vibrateOnLaunchIcon);
			this->allowAnimationOnLaunchIcon = ReadNodeNumber(pNodeMap, TEXT("animate"), this->allowAnimationOnLaunchIcon);
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("Background")) == 0) {
			CHR(pNode->get_attributes(&pNodeMap));
			this->fondoTransparente = ReadNodeNumber(pNodeMap, TEXT("transparent"), this->fondoTransparente);
			this->fondoColor        = ReadNodeNumber(pNodeMap, TEXT("color"),       this->fondoColor);
			this->fondoEstatico     = ReadNodeNumber(pNodeMap, TEXT("static"),      this->fondoEstatico);
			ReadNodeString(this->strFondoPantalla,   pNodeMap, TEXT("wallpaper"));
			RELEASE_OBJ(pNodeMap);
		} else if(lstrcmpi(nameNode, TEXT("NotifyTimer")) == 0) {
			this->notifyTimer = ReadTextNodeNumber(pNode, this->notifyTimer);
		} else if(lstrcmpi(nameNode, TEXT("IgnoreRotation")) == 0) {
			this->ignoreRotation = ReadTextNodeNumber(pNode, this->ignoreRotation);
		} else if(lstrcmpi(nameNode, TEXT("DisableRightClick")) == 0) {
			this->disableRightClick = ReadTextNodeNumber(pNode, this->disableRightClick);
		} else if(lstrcmpi(nameNode, TEXT("Fullscreen")) == 0) {
			this->fullscreen = ReadTextNodeNumber(pNode, this->fullscreen);
		} else if(lstrcmpi(nameNode, TEXT("NeverShowTaskBar")) == 0) {
			this->neverShowTaskBar = ReadTextNodeNumber(pNode, this->neverShowTaskBar);
		} else if(lstrcmpi(nameNode, TEXT("NoWindowTitle")) == 0) {
			this->noWindowTitle = ReadTextNodeNumber(pNode, this->noWindowTitle);
		} else if(lstrcmpi(nameNode, TEXT("AlreadyConfigured")) == 0) {
			this->alreadyConfigured = ReadTextNodeNumber(pNode, this->alreadyConfigured);
		} else if(lstrcmpi(nameNode, TEXT("MainScreen")) == 0) {
			mainScreenConfig->loadXMLConfig(pNode);
		} else if(lstrcmpi(nameNode, TEXT("BottomBar")) == 0) {
			bottomBarConfig->loadXMLConfig(pNode);
		}

		SysFreeString(nameNode);
        // Go to the next node
		CHR(pNode->get_nextSibling(&pNodeSibling));
        
        pNode->Release();
        pNode = pNodeSibling;
    }

	result = true;

Error:
	SysFreeString(nameNode);

    RELEASE_OBJ(pDOM)
    RELEASE_OBJ(pNode)
    RELEASE_OBJ(pNodeSibling)
	RELEASE_OBJ(pRootNode)
	RELEASE_OBJ(pNodeMap);

    VariantClear(&vt);
	
	return result;
}

BOOL CConfiguracion::guardaXMLIconos(CListaPantalla *listaPantallas)
{
    IXMLDOMDocument *pXMLDom = NULL;
    IXMLDOMProcessingInstruction *pi = NULL;
    IXMLDOMComment *pc = NULL;
    IXMLDOMElement *pe = NULL;
    IXMLDOMElement *pRoot = NULL;

	BSTR bstr = NULL;
	BSTR bstr1 = NULL;
	BSTR bstr_wsn = SysAllocString(L"\n");
	BSTR bstr_wsnt= SysAllocString(L"\n\t");
	BSTR bstr_wsntt=SysAllocString(L"\n\t\t");

	VARIANT var;
	HRESULT hr;

	VariantInit(&var);
  
	pXMLDom = DomFromCOM();
	if (!pXMLDom) goto Error;

	// Create a processing instruction element.
	bstr = SysAllocString(L"xml");
	bstr1 = SysAllocString(L"version='1.0'");
	CHR(pXMLDom->createProcessingInstruction(bstr,bstr1, &pi));
	AppendChildToParent(pi, pXMLDom);
	RELEASE_OBJ(pi);
	SysFreeString(bstr); bstr = NULL;
	SysFreeString(bstr1); bstr1 = NULL;

	// Create a comment element.
	bstr = SysAllocString(L"iPhoneToday for Windows Mobile");
	CHR(pXMLDom->createComment(bstr, &pc));
	AppendChildToParent(pc, pXMLDom);
	SysFreeString(bstr); bstr = NULL;
	RELEASE_OBJ(pc);

	// Create the root element.
	bstr = SysAllocString(L"root");
	CHR(pXMLDom->createElement(bstr, &pRoot));
	SysFreeString(bstr); bstr = NULL;

	AppendChildToParent(pRoot, pXMLDom);
	SysFreeString(bstr); bstr = NULL;

	CPantalla *pantalla;
	// Insertamos los nodos hijos.
	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);

		bstr = SysAllocString(L"screen");
		CHR(pXMLDom->createElement(bstr, &pe));
		SysFreeString(bstr); bstr = NULL;

		pantalla = listaPantallas->listaPantalla[i];
		createAttributeXML(pXMLDom, pe, L"header", pantalla->header);
		for (UINT j = 0; j < pantalla->numIconos; j++) {
			creaNodoXMLIcono(pXMLDom, pe, pantalla->listaIconos[j]);
		}

		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

		AppendChildToParent(pe, pRoot);
		RELEASE_OBJ(pe);
	}

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);

		bstr = SysAllocString(L"BottomBar");
		CHR(pXMLDom->createElement(bstr, &pe));
		SysFreeString(bstr); bstr = NULL;

		pantalla = listaPantallas->barraInferior;
		for (UINT j = 0; j < pantalla->numIconos; j++) {
			creaNodoXMLIcono(pXMLDom, pe, pantalla->listaIconos[j]);
		}

		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

		AppendChildToParent(pe, pRoot);
		RELEASE_OBJ(pe);
	}

	AddWhiteSpaceToNode(pXMLDom, bstr_wsn, pRoot);

    CHR(pXMLDom->get_xml(&bstr));

    VariantClear(&var);
    var = VariantString(pathIconsXML);
    CHR(pXMLDom->save(var));

	lastModifiedIconsXML = FileModifyTime(pathIconsXML);

Error:
    if (bstr) SysFreeString(bstr);
    if (bstr1) SysFreeString(bstr1);
	if (bstr_wsn) SysFreeString(bstr_wsn);
	if (bstr_wsnt) SysFreeString(bstr_wsnt);
	if (bstr_wsntt) SysFreeString(bstr_wsntt);

    if (&var) VariantClear(&var);

	RELEASE_OBJ(pXMLDom)
	RELEASE_OBJ(pRoot)
	RELEASE_OBJ(pe)
	RELEASE_OBJ(pi)
	RELEASE_OBJ(pc)

    return 0;
}

BOOL CConfiguracion::creaNodoXMLIcono(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, CIcono *icono)
{
	IXMLDOMElement *pe = NULL;

	BSTR bstr = NULL;
	HRESULT hr;
	BOOL result = FALSE;

	BSTR bstr_wsntt = SysAllocString(L"\n\t\t");

	// Create a <node1> to hold text content.
    bstr = SysAllocString(L"icon");
    CHR(pXMLDom->createElement(bstr, &pe));
    SysFreeString(bstr); bstr = NULL;

	createAttributeXML(pXMLDom, pe, L"name", icono->nombre);
	createAttributeXML(pXMLDom, pe, L"image", icono->rutaImagen);
	createAttributeXML(pXMLDom, pe, L"exec", icono->ejecutable);
	createAttributeXML(pXMLDom, pe, L"parameters", icono->parametros);
	createAttributeXML(pXMLDom, pe, L"execAlt", icono->ejecutableAlt);
	createAttributeXML(pXMLDom, pe, L"parametersAlt", icono->parametrosAlt);
	createAttributeXML(pXMLDom, pe, L"type", icono->tipo);
	createAttributeXML(pXMLDom, pe, L"animation", icono->launchAnimation);

	// Insertamos NEWLINE+TAB+TAB
    AddWhiteSpaceToNode(pXMLDom, bstr_wsntt, parent);

    // Insertamos el nodo al parent.
    AppendChildToParent(pe, parent);
    RELEASE_OBJ(pe);

	result = TRUE;
Error:

	RELEASE_OBJ(pe);

	SysFreeString(bstr);
	SysFreeString(bstr_wsntt);

	return result;
}

BOOL CConfiguracion::guardaXMLConfig()
{
	IXMLDOMDocument *pXMLDom = NULL;
	IXMLDOMProcessingInstruction *pi = NULL;
	IXMLDOMComment *pc = NULL;
	IXMLDOMElement *pe = NULL;
	IXMLDOMElement *pRoot = NULL;

	BSTR bstr = NULL;
	BSTR bstr1 = NULL;
	BSTR bstr_wsn = SysAllocString(L"\n");
	BSTR bstr_wsnt= SysAllocString(L"\n\t");

	VARIANT var;
	HRESULT hr;

	VariantInit(&var);

	pXMLDom = DomFromCOM();
	if (!pXMLDom) goto Error;

	// Create a processing instruction element.
	bstr = SysAllocString(L"xml");
	bstr1 = SysAllocString(L"version='1.0'");
	CHR(pXMLDom->createProcessingInstruction(bstr, bstr1, &pi));
	SysFreeString(bstr); bstr = NULL;
	SysFreeString(bstr1); bstr1 = NULL;
	AppendChildToParent(pi, pXMLDom);

	// Create a comment element.
	bstr = SysAllocString(L"iPhoneToday for Windows Mobile");
	CHR(pXMLDom->createComment(bstr, &pc));
	SysFreeString(bstr); bstr = NULL;
	AppendChildToParent(pc, pXMLDom);

	// Create the root element.
	bstr = SysAllocString(L"root");
	CHR(pXMLDom->createElement(bstr, &pRoot));
	SysFreeString(bstr); bstr = NULL;
	AppendChildToParent(pRoot, pXMLDom);

	// Insertamos los nodos hijos.

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"MainScreen");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	mainScreenConfig->saveXMLConfig(pXMLDom, pe);
	AppendChildToParent(pe, pRoot);
	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"BottomBar");
	CHR(pXMLDom->createElement(bstr, &pe));
	bottomBarConfig->saveXMLConfig(pXMLDom, pe);
	AppendChildToParent(pe, pRoot);
	SysFreeString(bstr); bstr = NULL;
	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Circles");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"diameter", this->circlesDiameter);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"distance", this->circlesDistance);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Header");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"size",   this->headerFontSize);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color",  this->headerFontColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"weight", this->headerFontWeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"offset", this->headerOffset);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Background");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"transparent", this->fondoTransparente);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color",       this->fondoColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"static",      this->fondoEstatico);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"wallpaper",   this->strFondoPantalla);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Movement");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"MoveThreshold",  this->umbralMovimiento);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"MaxVelocity",    this->velMaxima);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"MinVelocity",    this->velMinima);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"RefreshTime",    this->refreshTime);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"FactorMov",      this->factorMovimiento);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"VerticalScroll", this->verticalScroll);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"DayOfWeek");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color",     this->dowColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"width",     this->dowWidth);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"height",    this->dowHeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"weight",    this->dowWeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Sunday",    this->diasSemana[0]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Monday",    this->diasSemana[1]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Tuesday",   this->diasSemana[2]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Wednesday", this->diasSemana[3]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Thursday",  this->diasSemana[4]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Friday",    this->diasSemana[5]);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"Saturday",  this->diasSemana[6]);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"DayOfMonth");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color",  this->domColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"width",  this->domWidth);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"height", this->domHeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"weight", this->domWeight);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Clock");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"color",    this->clockColor);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"width",    this->clockWidth);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"height",   this->clockHeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"weight",   this->clockWeight);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"format12", this->clock12Format);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"Bubbles");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"notif", this->bubble_notif);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"state", this->bubble_state);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"alarm", this->bubble_alarm);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);
	bstr = SysAllocString(L"OnLaunchIcon");
	CHR(pXMLDom->createElement(bstr, &pe));
	SysFreeString(bstr); bstr = NULL;
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"close",   this->closeOnLaunchIcon);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"vibrate", this->vibrateOnLaunchIcon);
	CConfiguracion::createAttributeXML(pXMLDom, pe, L"animate", this->allowAnimationOnLaunchIcon);
	AppendChildToParent(pe, pRoot);
	RELEASE_OBJ(pe);

	creaNodoXMLConfig(pXMLDom, pRoot, L"NotifyTimer",       this->notifyTimer);
	creaNodoXMLConfig(pXMLDom, pRoot, L"IgnoreRotation",    this->ignoreRotation);
	creaNodoXMLConfig(pXMLDom, pRoot, L"DisableRightClick", this->disableRightClick);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Fullscreen",        this->fullscreen);
	creaNodoXMLConfig(pXMLDom, pRoot, L"NeverShowTaskBar",  this->neverShowTaskBar);
	creaNodoXMLConfig(pXMLDom, pRoot, L"NoWindowTitle",     this->noWindowTitle);
	creaNodoXMLConfig(pXMLDom, pRoot, L"AlreadyConfigured", this->alreadyConfigured);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsn, pRoot);

	CHR(pXMLDom->get_xml(&bstr));

	VariantClear(&var);
	var = VariantString(pathSettingsXML);
	CHR(pXMLDom->save(var));

	lastModifiedSettingsXML = FileModifyTime(pathSettingsXML);

Error:
	if (bstr) SysFreeString(bstr);
	if (bstr1) SysFreeString(bstr1);
	if (bstr_wsn) SysFreeString(bstr);
	if (bstr_wsnt) SysFreeString(bstr_wsnt);
	if (&var) VariantClear(&var);

	RELEASE_OBJ(pXMLDom);
	RELEASE_OBJ(pRoot);
	RELEASE_OBJ(pe);
	RELEASE_OBJ(pi);
	RELEASE_OBJ(pc);

	return 0;
}

BOOL CConfiguracion::creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], int content, int tabs)
{
	TCHAR str[MAX_PATH];
	swprintf(str, L"%d", content);
	return creaNodoXMLConfig(pXMLDom, parent, name, str, tabs);
}
BOOL CConfiguracion::creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], TCHAR content[MAX_PATH], int tabs)
{
	IXMLDOMElement *pe = NULL;

	BSTR bstr = NULL;
	HRESULT hr;
	BOOL result = FALSE;

	// Add NEWLINE+TABS.
	TCHAR tmp[256];
	tmp[0] = '\n';
	int i = 0;
	for (i = 1; i <= tabs; i++) {
		tmp[i] = '\t';
	}
	tmp[i] = 0;
	BSTR bstr_wsnt = SysAllocString(tmp);
	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, parent);
	SysFreeString(bstr_wsnt); bstr_wsnt = NULL;

	// Creamos el nodo.
    bstr = SysAllocString(name);
    CHR(pXMLDom->createElement(bstr, &pe));
    SysFreeString(bstr); bstr = NULL;

	bstr=SysAllocString(content);
    CHR(pe->put_text(bstr));
    SysFreeString(bstr); bstr = NULL;

    // Insertamos el nodo al parent.
    AppendChildToParent(pe, parent);
    RELEASE_OBJ(pe);

	result = TRUE;
Error:
	return result;
}

BOOL CConfiguracion::createAttributeXML(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], int content)
{
	TCHAR str[MAX_PATH];
	swprintf(str, L"%d", content);
	return createAttributeXML(pXMLDom, parent, name, str);
}
BOOL CConfiguracion::createAttributeXML(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], TCHAR content[MAX_PATH])
{
	IXMLDOMAttribute *pa = NULL;
	IXMLDOMAttribute *pa1 = NULL;

	BSTR bstr = NULL;
	VARIANT var;
	HRESULT hr;
	BOOL result = FALSE;

	bstr = SysAllocString(name);
	var = VariantString(content);
	CHR(pXMLDom->createAttribute(bstr, &pa));
	CHR(pa->put_value(var));
	CHR(parent->setAttributeNode(pa, &pa1));

	result = TRUE;
Error:
	SysFreeString(bstr);;
	RELEASE_OBJ(pa1);
	RELEASE_OBJ(pa);
	if(&var) VariantClear(&var);

	return result;
}
