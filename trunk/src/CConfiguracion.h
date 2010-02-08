#pragma once

#include "CListaPantalla.h"
#include "GraphicFunctions.h"
#include "xmlWrapper.h"

class CConfiguracion
{
public:
	CConfiguracion(void);
	~CConfiguracion(void);

	int calculaConfiguracion(int maxIconos, int hayBarraInferior, int width, int height);
	BOOL cargaIconos(HDC *hDC, CListaPantalla *listaPantallas);
	BOOL cargaImagenIcono(HDC *hDC, CIcono *icono);
	BOOL cargaImagenes(HDC *hDC);
	BOOL cargaFondo(HDC *hDC);

	BOOL cargaXMLIconos(CListaPantalla *listaPantallas);
	BOOL cargaXMLConfig();
	BOOL guardaXMLConfig();
	BOOL guardaXMLIconos(CListaPantalla *listaPantallas);
	BOOL guardaXMLIconos2(CListaPantalla *listaPantallas);

	BOOL creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], int content);
	BOOL creaNodoXMLConfig(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, TCHAR name[MAX_PATH], TCHAR content[MAX_PATH]);
	BOOL creaNodoXMLIcono(IXMLDOMDocument *pXMLDom, IXMLDOMElement *parent, CIcono *icono);

	TCHAR rutaInstalacion[MAX_PATH];

	// Variables que cambian al poner la PDA en horizontal
	UINT numeroIconos;
	UINT numeroIconosXML;
	UINT altoPantalla;
	UINT altoPantallaP;
	UINT altoPantallaL;


	// Variables 
	RECT dimensionesPantalla;
	UINT anchoPantalla;
	UINT anchoIcono;
	UINT anchoIconoXML;
	UINT distanciaIconosH;
	UINT distanciaIconosV;
	UINT fondoTransparente;
	UINT ignoreRotation;
	UINT closeOnLaunchIcon;
	UINT vibrateOnLaunchIcon;
	UINT fondoEstatico; // Si es cierto el fondo no tendra animacion
	UINT alreadyConfigured;

	// Clock
	COLORREF clockColor;
	UINT clockWidth;
	UINT clockHeight;
	UINT clock12Format;

	POINTS posReferencia; // Posicion 1º icono
	// RECT cuadroVirtualLanzando; // Posicion Objetivo en una transicion de lanzamiento
	UINT umbralMovimiento;
	UINT fontSize;
	UINT fontBold;
	UINT velMaxima;
	UINT velMinima;
	UINT refreshTime;
	UINT factorMovimiento;

	TCHAR diasSemana[7][16];

	// Burbujas
	CIcono *bubbleNotif;
	CIcono *bubbleState;
	CIcono *bubbleAlarm;

	// Fondo de pantalla
	TCHAR strFondoPantalla[MAX_PATH];
	CIcono *fondoPantalla;

};
