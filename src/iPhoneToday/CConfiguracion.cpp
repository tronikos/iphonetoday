#include "CConfiguracion.h"

CConfiguracion::CConfiguracion(void)
{
	StringCchPrintf(rutaInstalacion, CountOf(rutaInstalacion), TEXT("\\iPhoneToday\\"));

	fondoPantalla = NULL;
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
}

// Estado = 0 = Normal; Estado = 1 = Landscape
// maxIconos = Maximo de iconos que hay en una pantalla
int CConfiguracion::calculaConfiguracion(int maxIconos, int hayBarraInferior, int width, int height)
{
	int estado = 0; // Posicion Normal

	// Dimensiones de la pantalla
	this->dimensionesPantalla.left = 0;
	this->dimensionesPantalla.right = width;
	this->dimensionesPantalla.top = 0;
	this->dimensionesPantalla.bottom = height;

	if (width > height) {
		estado = 1;
	}

	// Cargamos los parametros
	anchoPantalla = dimensionesPantalla.right - dimensionesPantalla.left;

#ifdef EXEC_MODE
	altoPantalla = dimensionesPantalla.bottom - dimensionesPantalla.top;
#else
	if ((estado == 1 && altoPantallaL == 0) || (estado == 0 && altoPantallaP == 0)) {
		altoPantalla = dimensionesPantalla.bottom - dimensionesPantalla.top - 2;
	} else if (estado == 1) {
		altoPantalla = altoPantallaL;
	} else {
		altoPantalla = altoPantallaP;
	}
#endif

	if (estado == 1 && maxIconos > (int)numeroIconosXML) {
		// Si hay barra inferior ponemos 2 filas de iconos
		// Sy NO hay barra inferior ponemos 3 filas de iconos
		int altoZonaIconos;
		int nFilas;
		if (hayBarraInferior) {
			nFilas = 2;
			altoZonaIconos = altoPantalla - distanciaIconosV;
		} else {
			nFilas = 3;
			altoZonaIconos = altoPantalla;
		}
		anchoIcono = UINT((altoZonaIconos / nFilas) / 1.55);


		maxIconos += maxIconos % nFilas;
		
		numeroIconos = (maxIconos + (maxIconos % nFilas)) / nFilas;
		numeroIconos = max(numeroIconos, numeroIconosXML);

		anchoIcono = min(anchoIcono, (UINT)(width * 0.94 / numeroIconos));
	} else {
		anchoIcono = anchoIconoXML;
		numeroIconos = numeroIconosXML;
	}
	
	UINT distanciaBordePantalla;
	
	// Factor usado para dejar espacio en los lados
	float factor;
	if (estado == 1) {
		factor = 0.96;
	} else {
		factor = 0.92;
	}

	distanciaIconosH = UINT(anchoPantalla * factor / numeroIconos);
	distanciaIconosV = UINT(anchoIcono * 1.45);

	distanciaBordePantalla = UINT((anchoPantalla / 2) - (((distanciaIconosH * numeroIconos) - (distanciaIconosH - anchoIcono)) / 2));
	
	posReferencia.x = distanciaBordePantalla;
	posReferencia.y = short(distanciaBordePantalla * 0.5);

	return estado;
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
    CLSID clsid;

	TCHAR fichIconos[256];
	TCHAR *nodeName;

	// Load the XML DOM
	hr = CLSIDFromProgID(TEXT("Msxml2.DOMDocument"), &clsid);
    CHR(hr);
    hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IXMLDOMDocument, (LPVOID *) &pDOM);
    CHR(hr);

	StringCchPrintf(fichIconos, CountOf(fichIconos), TEXT("%sicons.xml"), rutaInstalacion);

	// Load the xml file
    vt.vt = VT_BSTR;
    vt.bstrVal = SysAllocString(fichIconos);
    hr = pDOM->load(vt, &fSuccess);
	
    CHR(hr);
    CBR(fSuccess);

	// Find the root node
    hr = pDOM->selectSingleNode(TEXT("root"), &pRootNode);
    CHR(hr);

	// Walk the children of this node -- each top-level child will be a single store property
	hr = pRootNode->get_firstChild(&pNodeScreen);
    CHR(hr);

	if (listaPantallas->barraInferior == NULL) {
		listaPantallas->barraInferior = new CPantalla();
	}
	UINT nIcon = 0;
	UINT nScreen = 0;
    while (pNodeScreen)
    {
		pNodeScreen->get_nodeName(&nodeName);
		if (_tcscmp(nodeName, TEXT("bottomBar")) == 0) {
			pantalla = listaPantallas->barraInferior;
		} else {
			if (listaPantallas->listaPantalla[nScreen] == NULL) {
				pantalla = listaPantallas->creaPantalla();
			} else {
				pantalla = listaPantallas->listaPantalla[nScreen];
			}
			nScreen++;
		}

		hr = pNodeScreen->get_firstChild(&pNodeIcon);
        CHR(hr);

		nIcon = 0;
		while (pNodeIcon) 
		{
			// Load in any attributes on this node
            RELEASE_OBJ(pNodeMap)

            hr = pNodeIcon->get_attributes(&pNodeMap);
            CHR(hr);

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

			// Go to the next node
			hr = pNodeIcon->get_nextSibling(&pNodeIconSibling);
			CHR(hr);
	        
			pNodeIcon->Release();
			pNodeIcon = pNodeIconSibling;

		}
		while (nIcon < pantalla->numIconos) {
			pantalla->borraIcono(nIcon);
			// nIcon++;
		}

        // Go to the next node
		hr = pNodeScreen->get_nextSibling(&pNodeScreenSibling);
        CHR(hr);
        
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
	
	result = cargaXMLIconos(listaPantallas);

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

	
	for (int i = 0; i < nPantallas; i++) {
		pantalla = listaPantallas->listaPantalla[i];

		nIconos = pantalla->numIconos;
		for (int j = 0; j < nIconos; j++) {

			icono = pantalla->listaIconos[j];
			cargaImagenIcono(hDC, icono);
		}
	}

	// Cargamos los iconos de la barra inferior
	pantalla = listaPantallas->barraInferior;

	nIconos = pantalla->numIconos;
	for (int j = 0; j < nIconos; j++) {

		icono = pantalla->listaIconos[j];
		cargaImagenIcono(hDC, icono);
	}

	result = true;
	return result;
}

BOOL CConfiguracion::cargaImagenIcono(HDC *hDC, CIcono *icono)
{
	BOOL result;
	TCHAR rutaImgCompleta[MAX_PATH];

	icono->clearImageObjects();

	if (icono->rutaImagen[0] == TEXT('\\')) {
		StringCchCopy(rutaImgCompleta, CountOf(rutaImgCompleta), icono->rutaImagen);
	} else {
		StringCchPrintf(rutaImgCompleta, CountOf(rutaImgCompleta), TEXT("%sicons\\%s"), rutaInstalacion, icono->rutaImagen);
	}

	if (_tcslen(icono->rutaImagen) > 0) {
		icono->loadImage(hDC, rutaImgCompleta, anchoIcono, anchoIcono);
	} else if (_tcslen(icono->ejecutable) > 0) {
		icono->loadImageFromExec(hDC, icono->ejecutable, anchoIcono, anchoIcono);
	}

	result = true;
	return result;
}
BOOL CConfiguracion::cargaFondo(HDC *hDC)
{
	if (this->fondoTransparente == 0) {
		if (fondoPantalla != NULL) {
			delete fondoPantalla;
		}
		fondoPantalla = new CIcono();
		float factor = 1.75;
		if (this->fondoEstatico) {
			factor = 1.0;
		}
		fondoPantalla->loadImage(hDC, strFondoPantalla, UINT(anchoPantalla * factor), altoPantalla, PIXFMT_16BPP_RGB565);
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
	StringCchPrintf(rutaImgCompleta, CountOf(rutaImgCompleta), TEXT("%sskin\\bubble_notif.png"), rutaInstalacion);
	bubbleNotif->loadImage(hDC, rutaImgCompleta, UINT(anchoIcono * 0.80), UINT(anchoIcono * 0.80));

	bubbleState = new CIcono();
	StringCchPrintf(rutaImgCompleta, CountOf(rutaImgCompleta), TEXT("%sskin\\bubble_state.png"), rutaInstalacion);
	bubbleState->loadImage(hDC, rutaImgCompleta, anchoIcono, UINT(anchoIcono * 0.50));

	bubbleAlarm = new CIcono();
	StringCchPrintf(rutaImgCompleta, CountOf(rutaImgCompleta), TEXT("%sskin\\bubble_alarm.png"), rutaInstalacion);
	bubbleAlarm->loadImage(hDC, rutaImgCompleta, UINT(anchoIcono * 0.80), UINT(anchoIcono * 0.80));

	result = true;
	return result;
}

BOOL CConfiguracion::cargaXMLConfig()
{
	// Valores por defecto
	this->anchoIconoXML = 40;
	this->numeroIconosXML = 4;
	this->anchoIcono = this->anchoIconoXML;
	this->numeroIconos = this->numeroIconosXML;
	this->umbralMovimiento = 15;
	this->velMaxima = 140;
	this->velMinima = 20;
	this->refreshTime = 20;
	this->altoPantallaP = 0;
	this->altoPantallaL = 0;
	this->fontSize = 7;
	this->fontBold = 0;
	this->factorMovimiento = 4;
	this->fondoTransparente = 0;
	this->fondoEstatico = 0;
	this->ignoreRotation = 0;
	this->closeOnLaunchIcon = 0;
	this->vibrateOnLaunchIcon = 40;
	this->clockColor = RGB(230,230,230);
	this->clockWidth = 11;
	this->clockHeight = 22;
	this->clock12Format = 0;
	this->alreadyConfigured = 0;
	StringCchCopy(this->diasSemana[0], CountOf(this->diasSemana[0]), TEXT("Sun"));
	StringCchCopy(this->diasSemana[1], CountOf(this->diasSemana[1]), TEXT("Mon"));
	StringCchCopy(this->diasSemana[2], CountOf(this->diasSemana[2]), TEXT("Tue"));
	StringCchCopy(this->diasSemana[3], CountOf(this->diasSemana[3]), TEXT("Wed"));
	StringCchCopy(this->diasSemana[4], CountOf(this->diasSemana[4]), TEXT("Thu"));
	StringCchCopy(this->diasSemana[5], CountOf(this->diasSemana[5]), TEXT("Fri"));
	StringCchCopy(this->diasSemana[6], CountOf(this->diasSemana[6]), TEXT("Sat"));
	StringCchCopy(this->strFondoPantalla, CountOf(this->strFondoPantalla), TEXT(""));

	BOOL result = false;
	
	HRESULT hr = S_OK;
    IXMLDOMDocument *pDOM = NULL;
    IXMLDOMNode *pRootNode = NULL;
    IXMLDOMNode *pNode = NULL;
    IXMLDOMNode *pNodeSibling = NULL;

	TCHAR fichIconos[256];
	TCHAR *nameNode;
	
    VARIANT_BOOL fSuccess;
    VARIANT vt;
	CLSID clsid;

	// Load the XML DOM
	hr = CLSIDFromProgID(TEXT("Msxml2.DOMDocument"), &clsid);
    CHR(hr);
    hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (LPVOID *) &pDOM);
    CHR(hr);

	StringCchPrintf(fichIconos, CountOf(fichIconos), TEXT("%ssettings.xml"), rutaInstalacion);

	// Load the xml file
    vt.vt = VT_BSTR;
    vt.bstrVal = SysAllocString(fichIconos);
    hr = pDOM->load(vt, &fSuccess);
    CHR(hr);
    CBR(fSuccess);

	// Find the root node
    hr = pDOM->selectSingleNode(TEXT("root"), &pRootNode);
    CHR(hr);


	// Walk the children of this node -- each top-level child will be a single store property
	hr = pRootNode->get_firstChild(&pNode);
    CHR(hr);

    while (pNode)
    {
		pNode->get_baseName(&nameNode);
		// WriteToLog(TEXT("Bucle:\r\n"));

		if(lstrcmp(nameNode, TEXT("iconWidth")) == 0) {
			this->anchoIconoXML = ReadTextNodeNumber(pNode, this->anchoIconoXML);
		} else if(lstrcmp(nameNode, TEXT("numIcons")) == 0) {
			this->numeroIconosXML = ReadTextNodeNumber(pNode, this->numeroIconosXML);
		} else if(lstrcmp(nameNode, TEXT("moveThreshold")) == 0) {
			this->umbralMovimiento = ReadTextNodeNumber(pNode, this->umbralMovimiento);
		} else if(lstrcmp(nameNode, TEXT("maxVelocitiy")) == 0) {
			this->velMaxima = ReadTextNodeNumber(pNode, this->velMaxima);
		} else if(lstrcmp(nameNode, TEXT("minVelocitiy")) == 0) {
			this->velMinima = ReadTextNodeNumber(pNode, this->velMinima);
		} else if(lstrcmp(nameNode, TEXT("refreshTime")) == 0) {
			this->refreshTime = ReadTextNodeNumber(pNode, this->refreshTime);
		} else if(lstrcmp(nameNode, TEXT("screenHeigth")) == 0) {
			this->altoPantallaP = ReadTextNodeNumber(pNode, this->altoPantallaP);
		} else if(lstrcmp(nameNode, TEXT("screenHeigthL")) == 0) {
			this->altoPantallaL = ReadTextNodeNumber(pNode, this->altoPantallaL);
		} else if(lstrcmp(nameNode, TEXT("fontSize")) == 0) {
			this->fontSize = ReadTextNodeNumber(pNode, this->fontSize);
		} else if(lstrcmp(nameNode, TEXT("fontBold")) == 0) {
			this->fontBold = ReadTextNodeNumber(pNode, this->fontBold);
		} else if(lstrcmp(nameNode, TEXT("factorMov")) == 0) {
			this->factorMovimiento = ReadTextNodeNumber(pNode, this->factorMovimiento);
		} else if(lstrcmp(nameNode, TEXT("Sunday")) == 0) {
			ReadTextNode(this->diasSemana[0], pNode, this->diasSemana[0]);
		} else if(lstrcmp(nameNode, TEXT("Monday")) == 0) {
			ReadTextNode(this->diasSemana[1], pNode, this->diasSemana[1]);
		} else if(lstrcmp(nameNode, TEXT("Tuesday")) == 0) {
			ReadTextNode(this->diasSemana[2], pNode, this->diasSemana[2]);
		} else if(lstrcmp(nameNode, TEXT("Wednesday")) == 0) {
			ReadTextNode(this->diasSemana[3], pNode, this->diasSemana[3]);
		} else if(lstrcmp(nameNode, TEXT("Thursday")) == 0) {
			ReadTextNode(this->diasSemana[4], pNode, this->diasSemana[4]);
		} else if(lstrcmp(nameNode, TEXT("Friday")) == 0) {
			ReadTextNode(this->diasSemana[5], pNode, this->diasSemana[5]);
		} else if(lstrcmp(nameNode, TEXT("Saturday")) == 0) {
			ReadTextNode(this->diasSemana[6], pNode, this->diasSemana[6]);
		} else if(lstrcmp(nameNode, TEXT("backTransparent")) == 0) {
			this->fondoTransparente = ReadTextNodeNumber(pNode, this->fondoTransparente);
		} else if(lstrcmp(nameNode, TEXT("staticWallpaper")) == 0) {
			this->fondoEstatico = ReadTextNodeNumber(pNode, this->fondoEstatico);
		} else if(lstrcmp(nameNode, TEXT("wallpaper")) == 0) {
			ReadTextNode(this->strFondoPantalla, pNode, this->strFondoPantalla);
		} else if(lstrcmp(nameNode, TEXT("ignoreRotation")) == 0) {
			this->ignoreRotation = ReadTextNodeNumber(pNode, this->ignoreRotation);
		} else if(lstrcmp(nameNode, TEXT("closeOnLaunchIcon")) == 0) {
			this->closeOnLaunchIcon = ReadTextNodeNumber(pNode, this->closeOnLaunchIcon);
		} else if(lstrcmp(nameNode, TEXT("vibrateOnLaunchIcon")) == 0) {
			this->vibrateOnLaunchIcon = ReadTextNodeNumber(pNode, this->vibrateOnLaunchIcon);
		} else if(lstrcmp(nameNode, TEXT("clockColor")) == 0) {
			this->clockColor = ReadTextNodeNumber(pNode, this->clockColor);
		} else if(lstrcmp(nameNode, TEXT("clockWidth")) == 0) {
			this->clockWidth = ReadTextNodeNumber(pNode, this->clockWidth);
		} else if(lstrcmp(nameNode, TEXT("clockHeight")) == 0) {
			this->clockHeight = ReadTextNodeNumber(pNode, this->clockHeight);
		} else if(lstrcmp(nameNode, TEXT("clock12Format")) == 0) {
			this->clock12Format = ReadTextNodeNumber(pNode, this->clock12Format);
		} else if(lstrcmp(nameNode, TEXT("alreadyConfigured")) == 0) {
			this->alreadyConfigured = ReadTextNodeNumber(pNode, this->alreadyConfigured);
		}

		SysFreeString(nameNode);
        // Go to the next node
		hr = pNode->get_nextSibling(&pNodeSibling);
        CHR(hr);
        
        pNode->Release();
        pNode = pNodeSibling;
    }

	this->anchoIcono = this->anchoIconoXML;
	this->numeroIconos = this->numeroIconosXML;

	result = true;

Error:
	SysFreeString(nameNode);

    RELEASE_OBJ(pDOM)
    RELEASE_OBJ(pNode)
    RELEASE_OBJ(pNodeSibling)
	RELEASE_OBJ(pRootNode)

    VariantClear(&vt);
	
	return result;
}

BOOL CConfiguracion::guardaXMLIconos(CListaPantalla *listaPantallas)
{
    IXMLDOMDocument *pXMLDom=NULL;
    IXMLDOMProcessingInstruction *pi=NULL;
    IXMLDOMComment *pc=NULL;
    IXMLDOMElement *pe=NULL;
    IXMLDOMElement *pRoot=NULL;
    IXMLDOMAttribute *pa=NULL;
    IXMLDOMAttribute *pa1=NULL;
    IXMLDOMCDATASection *pcd=NULL;
    IXMLDOMElement *peSub=NULL;
    IXMLDOMDocumentFragment *pdf=NULL;

    BSTR bstr = NULL;
    BSTR bstr1 = NULL;
    BSTR bstr_wsn = SysAllocString(L"\n");
    BSTR bstr_wsnt= SysAllocString(L"\n\t");
    BSTR bstr_wsntt=SysAllocString(L"\n\t\t");

    VARIANT var;
    HRESULT hr;

	TCHAR fichIconos[256];
	TCHAR fichIconosBak[256];
	StringCchPrintf(fichIconos, CountOf(fichIconos), TEXT("%sicons.xml"), rutaInstalacion);
	StringCchPrintf(fichIconosBak, CountOf(fichIconosBak), TEXT("%s.bak"), fichIconos);

	/*if(CopyFile(fichIconos, fichIconosBak, FALSE) == FALSE) {
		MessageBox(NULL, TEXT("Error creating backup icons file"), TEXT("Error!"), MB_OK);
		return false;
	}*/


    VariantInit(&var);
  
    pXMLDom = DomFromCOM();
    if (!pXMLDom) goto Error;

    // Create a processing instruction element.
    bstr = SysAllocString(L"xml");
    bstr1 = SysAllocString(L"version='1.0'");
    CHR(pXMLDom->createProcessingInstruction(bstr,bstr1, &pi));
    AppendChildToParent(pi, pXMLDom);
    pi->Release();
    pi=NULL;
    SysFreeString(bstr);
    bstr=NULL;
    SysFreeString(bstr1);
    bstr1=NULL;

    // Create a comment element.
    bstr = SysAllocString(L"iPhoneToday plugin for Windows Mobile");
    CHR(pXMLDom->createComment(bstr, &pc));
    AppendChildToParent(pc, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    pc->Release();
    pc=NULL;

    // Create the root element.
    bstr = SysAllocString(L"root");
    CHR(pXMLDom->createElement(bstr, &pRoot));
    SysFreeString(bstr);
    bstr = NULL;

    AppendChildToParent(pRoot, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;

	CPantalla *pantalla;
    // Insertamos los nodos hijos.
	for (UINT i = 0; i < listaPantallas->numPantallas; i++) {
		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);

		bstr = SysAllocString(L"screen");
		CHR(pXMLDom->createElement(bstr,&pe));
		SysFreeString(bstr);
		bstr=NULL;

		pantalla = listaPantallas->listaPantalla[i];
		for (UINT j = 0; j < pantalla->numIconos; j++) {
			creaNodoXMLIcono(pXMLDom, pe, pantalla->listaIconos[j]);
		}

		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

		AppendChildToParent(pe, pRoot);
		pe->Release();
		pe=NULL;
	}

	if (listaPantallas->barraInferior != NULL && listaPantallas->barraInferior->numIconos > 0) {
		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pRoot);

		bstr = SysAllocString(L"bottomBar");
		CHR(pXMLDom->createElement(bstr,&pe));
		SysFreeString(bstr);
		bstr=NULL;

		pantalla = listaPantallas->barraInferior;
		for (UINT j = 0; j < pantalla->numIconos; j++) {
			creaNodoXMLIcono(pXMLDom, pe, pantalla->listaIconos[j]);
		}

		// Add NEWLINE+TAB.
		AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, pe);

		AppendChildToParent(pe, pRoot);
		pe->Release();
		pe=NULL;
	}

	AddWhiteSpaceToNode(pXMLDom, bstr_wsn, pRoot);

    CHR(pXMLDom->get_xml(&bstr));

    VariantClear(&var);
    var = VariantString(fichIconos);
    CHR(pXMLDom->save(var));

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
	RELEASE_OBJ(peSub)
	RELEASE_OBJ(pi)
	RELEASE_OBJ(pa)
	RELEASE_OBJ(pa1)
	RELEASE_OBJ(pc)
	RELEASE_OBJ(pcd)
	RELEASE_OBJ(pdf)

    return 0;
}

BOOL CConfiguracion::creaNodoXMLIcono(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, CIcono *icono)
{
	IXMLDOMElement *pe = NULL;
    IXMLDOMAttribute *pa = NULL;
	IXMLDOMAttribute *pa1 = NULL;

	BSTR bstr = NULL;
	VARIANT var;
	HRESULT hr;
	BOOL result = FALSE;
	TCHAR tcharInt[MAX_PATH];

	BSTR bstr_wsntt = SysAllocString(L"\n\t\t");

	// Create a <node1> to hold text content.
    bstr = SysAllocString(L"icon");
    CHR(pXMLDom->createElement(bstr, &pe));
    SysFreeString(bstr);
    bstr=NULL;

	// Insertamos un atributo
    bstr = SysAllocString(L"name");
	var = VariantString(icono->nombre);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
    bstr = SysAllocString(L"image");
	var = VariantString(icono->rutaImagen);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
    bstr = SysAllocString(L"exec");
	var = VariantString(icono->ejecutable);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
    bstr = SysAllocString(L"parameters");
	var = VariantString(icono->parametros);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
    bstr = SysAllocString(L"execAlt");
	var = VariantString(icono->ejecutableAlt);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
    bstr = SysAllocString(L"parametersAlt");
	var = VariantString(icono->parametrosAlt);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);

	// Insertamos un atributo
	swprintf(tcharInt, L"%d", icono->tipo);
    bstr = SysAllocString(L"type");
	var = VariantString(tcharInt);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);
	bstr=NULL;

	// Insertamos un atributo
	swprintf(tcharInt, L"%d", icono->launchAnimation);
    bstr = SysAllocString(L"animation");
	var = VariantString(tcharInt);
    CHR(pXMLDom->createAttribute(bstr, &pa));
    CHR(pa->put_value(var));
    CHR(pe->setAttributeNode(pa, &pa1));
    AppendChildToParent(pe, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    if (pa1) {
        pa1->Release();
        pa1=NULL;
    }
    pa->Release();
    pa=NULL;
    VariantClear(&var);
	bstr=NULL;

	// Insertamos NEWLINE+TAB+TAB
    AddWhiteSpaceToNode(pXMLDom, bstr_wsntt, parent);

    // Insertamos el nodo al parent.
    AppendChildToParent(pe, parent);
    pe->Release();
    pe=NULL;

	result = TRUE;
Error:

	RELEASE_OBJ(pe);
	RELEASE_OBJ(pa);
	RELEASE_OBJ(pa1);

	SysFreeString(bstr);
	SysFreeString(bstr_wsntt);

	if(&var) VariantClear(&var);

	return result;
}

BOOL CConfiguracion::guardaXMLConfig()
{
    IXMLDOMDocument *pXMLDom=NULL;
    IXMLDOMProcessingInstruction *pi=NULL;
    IXMLDOMComment *pc=NULL;
    IXMLDOMElement *pe=NULL;
    IXMLDOMElement *pRoot=NULL;
    IXMLDOMAttribute *pa=NULL;
    IXMLDOMAttribute *pa1=NULL;
    IXMLDOMCDATASection *pcd=NULL;
    IXMLDOMElement *peSub=NULL;
    IXMLDOMDocumentFragment *pdf=NULL;

    BSTR bstr = NULL;
    BSTR bstr1 = NULL;
    BSTR bstr_wsn = SysAllocString(L"\n");
    BSTR bstr_wsnt= SysAllocString(L"\n\t");
    BSTR bstr_wsntt=SysAllocString(L"\n\t\t");

    VARIANT var;
    HRESULT hr;

	TCHAR fichIconos[256];
	TCHAR fichIconosBak[256];
	StringCchPrintf(fichIconos, CountOf(fichIconos), TEXT("%ssettings.xml"), rutaInstalacion);
	StringCchPrintf(fichIconosBak, CountOf(fichIconosBak), TEXT("%s.bak"), fichIconos);
	
	// DeleteFile(fichIconosBak);

	/*if(CopyFile(fichIconos, fichIconosBak, FALSE) == FALSE) {
		MessageBox(NULL, TEXT("Error creating backup icons file"), TEXT("Error!"), MB_OK);
		return false;
	}*/


    VariantInit(&var);
  
    pXMLDom = DomFromCOM();
    if (!pXMLDom) goto Error;

    // Create a processing instruction element.
    bstr = SysAllocString(L"xml");
    bstr1 = SysAllocString(L"version='1.0'");
    CHR(pXMLDom->createProcessingInstruction(bstr,bstr1, &pi));
    AppendChildToParent(pi, pXMLDom);
    pi->Release();
    pi=NULL;
    SysFreeString(bstr);
    bstr=NULL;
    SysFreeString(bstr1);
    bstr1=NULL;

    // Create a comment element.
    bstr = SysAllocString(L"iPhoneToday plugin for Windows Mobile");
    CHR(pXMLDom->createComment(bstr, &pc));
    AppendChildToParent(pc, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;
    pc->Release();
    pc=NULL;

    // Create the root element.
    bstr = SysAllocString(L"root");
    CHR(pXMLDom->createElement(bstr, &pRoot));
    SysFreeString(bstr);
    bstr = NULL;

    AppendChildToParent(pRoot, pXMLDom);
    SysFreeString(bstr);
    bstr=NULL;

    // Insertamos los nodos hijos.
	creaNodoXMLConfig(pXMLDom, pRoot, L"iconWidth", this->anchoIconoXML);
	creaNodoXMLConfig(pXMLDom, pRoot, L"numIcons", this->numeroIconosXML);
	creaNodoXMLConfig(pXMLDom, pRoot, L"moveThreshold", this->umbralMovimiento);
	creaNodoXMLConfig(pXMLDom, pRoot, L"maxVelocitiy", this->velMaxima);
	creaNodoXMLConfig(pXMLDom, pRoot, L"minVelocitiy", this->velMinima);
	creaNodoXMLConfig(pXMLDom, pRoot, L"refreshTime", this->refreshTime);
	creaNodoXMLConfig(pXMLDom, pRoot, L"screenHeigth", this->altoPantallaP);
	creaNodoXMLConfig(pXMLDom, pRoot, L"screenHeigthL", this->altoPantallaL);
	creaNodoXMLConfig(pXMLDom, pRoot, L"factorMov", this->factorMovimiento);
	creaNodoXMLConfig(pXMLDom, pRoot, L"fontSize", this->fontSize);
	creaNodoXMLConfig(pXMLDom, pRoot, L"fontBold", this->fontBold);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Sunday", this->diasSemana[0]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Monday", this->diasSemana[1]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Tuesday", this->diasSemana[2]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Wednesday", this->diasSemana[3]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Thursday", this->diasSemana[4]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Friday", this->diasSemana[5]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"Saturday", this->diasSemana[6]);
	creaNodoXMLConfig(pXMLDom, pRoot, L"wallpaper", this->strFondoPantalla);
	creaNodoXMLConfig(pXMLDom, pRoot, L"backTransparent", this->fondoTransparente);
	creaNodoXMLConfig(pXMLDom, pRoot, L"staticWallpaper", this->fondoEstatico);
	creaNodoXMLConfig(pXMLDom, pRoot, L"ignoreRotation", this->ignoreRotation);
	creaNodoXMLConfig(pXMLDom, pRoot, L"closeOnLaunchIcon", this->closeOnLaunchIcon);
	creaNodoXMLConfig(pXMLDom, pRoot, L"vibrateOnLaunchIcon", this->vibrateOnLaunchIcon);
	creaNodoXMLConfig(pXMLDom, pRoot, L"clockColor", this->clockColor);
	creaNodoXMLConfig(pXMLDom, pRoot, L"clockWidth", this->clockWidth);
	creaNodoXMLConfig(pXMLDom, pRoot, L"clockHeight", this->clockHeight);
	creaNodoXMLConfig(pXMLDom, pRoot, L"clock12Format", this->clock12Format);
	creaNodoXMLConfig(pXMLDom, pRoot, L"alreadyConfigured", this->alreadyConfigured);

	AddWhiteSpaceToNode(pXMLDom, bstr_wsn, pRoot);

    CHR(pXMLDom->get_xml(&bstr));

    VariantClear(&var);
    var = VariantString(fichIconos);
    CHR(pXMLDom->save(var));

Error:
    if (bstr) SysFreeString(bstr);
    if (bstr1) SysFreeString(bstr1);
    if (&var) VariantClear(&var);

    if (pXMLDom) pXMLDom->Release();
    if (pRoot) pRoot->Release();
    if (pe) pe->Release();
    if (peSub) peSub->Release();
    if (pi) pi->Release();
    if (pa) pa->Release();
    if (pa1) pa1->Release();
    if (pc) pc->Release();
    if (pcd) pcd->Release();
    if (pdf) pdf->Release();

    return 0;
}

BOOL CConfiguracion::creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], int content)
{
	TCHAR str[MAX_PATH];
	swprintf(str, L"%d", content);
	return creaNodoXMLConfig(pXMLDom, parent, name, str);
}
BOOL CConfiguracion::creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], TCHAR content[MAX_PATH])
{
	IXMLDOMElement *pe = NULL;
    IXMLDOMAttribute *pa = NULL;
	IXMLDOMAttribute *pa1 = NULL;

	BSTR bstr = NULL;
	HRESULT hr;
	BOOL result = FALSE;

	BSTR bstr_wsnt = SysAllocString(L"\n\t");

	// Add NEWLINE+TAB.
	AddWhiteSpaceToNode(pXMLDom, bstr_wsnt, parent);

	// Creamos el nodo.
    bstr = SysAllocString(name);
    CHR(pXMLDom->createElement(bstr, &pe));
    SysFreeString(bstr);
    bstr=NULL;

	bstr=SysAllocString(content);
    CHR(pe->put_text(bstr));
    SysFreeString(bstr);
    bstr=NULL;

    // Insertamos el nodo al parent.
    AppendChildToParent(pe, parent);
    pe->Release();
    pe=NULL;

	result = TRUE;
Error:
	return result;
}