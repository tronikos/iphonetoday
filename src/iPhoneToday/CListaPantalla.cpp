#include "stdafx.h"
#include "CListaPantalla.h"

CListaPantalla::CListaPantalla(void)
{
	numPantallas = 0;
	for (int i = 0; i < MAX_PANTALLAS; i++) {
		listaPantalla[i] = NULL;
	}

	barraInferior = NULL;
}

CListaPantalla::~CListaPantalla(void)
{
	// WriteToLog(TEXT("CListaPantalla 1!\r\n"));
	for (int i = 0; i < MAX_PANTALLAS; i++) {
		if (listaPantalla[i] != NULL) {
			delete listaPantalla[i];
		}
	}
	if (barraInferior != NULL) {
		delete barraInferior;
	}
	// WriteToLog(TEXT("CListaPantalla 2!\r\n"));
}

CPantalla *CListaPantalla::creaPantalla()
{
	listaPantalla[numPantallas] = new CPantalla();
	numPantallas++;

	return listaPantalla[numPantallas - 1];
}

CIcono *CListaPantalla::mueveIcono(int nPantallaActual, int nIconoActual, int nScreen, int nIcon)
{
	
	CIcono *icon = NULL;
	CPantalla *pantalla = NULL;

	if (nPantallaActual == -1) {
		pantalla = barraInferior;
	} else {
		pantalla = listaPantalla[nPantallaActual];
	}
	icon = pantalla->listaIconos[nIconoActual];

	for (int i = nIconoActual; i < int(pantalla->numIconos - 1); i++) {
		pantalla->listaIconos[i] = pantalla->listaIconos[i + 1];
	}
	pantalla->listaIconos[pantalla->numIconos - 1] = NULL;
	pantalla->numIconos--;
	pantalla->debeActualizar = TRUE;

	if (nScreen >= int(numPantallas)) {
		nScreen = numPantallas - 1;
	}

	if (nScreen == -1) {
		pantalla = barraInferior;
	} else {
		pantalla = listaPantalla[nScreen];
	}

	if (nIcon >= int(pantalla->numIconos)) {
		nIcon = pantalla->numIconos;
	}

	for (int i = pantalla->numIconos; i > nIcon; i--) {
		pantalla->listaIconos[i] = pantalla->listaIconos[i - 1];
	}
	pantalla->listaIconos[nIcon] = icon;
	pantalla->numIconos++;
	pantalla->debeActualizar = TRUE;

	if (nPantallaActual != -1) {
		if (listaPantalla[nPantallaActual]->numIconos == 0 && numPantallas > 1) {
			delete listaPantalla[nPantallaActual];
			for (int i = nPantallaActual; i < int(numPantallas - 1); i++) {
				listaPantalla[i] = listaPantalla[i + 1];
			}
			
			listaPantalla[numPantallas - 1] = NULL;
			numPantallas--;
		}
	}
	
	return icon;
}

CIcono *CListaPantalla::copyIcono(int nPantallaActual, int nIconoActual, int nScreen, int nIcon, CIcono *destIcon)
{
	
	CIcono *icon = NULL;
	CPantalla *pantalla = NULL;

	if (nPantallaActual == -1) {
		pantalla = barraInferior;
	} else {
		pantalla = listaPantalla[nPantallaActual];
	}
	icon = pantalla->listaIconos[nIconoActual];

	if (nScreen >= int(numPantallas)) {
		nScreen = numPantallas - 1;
	}

	if (nScreen == -1) {
		pantalla = barraInferior;
	} else {
		pantalla = listaPantalla[nScreen];
	}

	if (nIcon >= int(pantalla->numIconos)) {
		nIcon = pantalla->numIconos;
	}

	for (int i = pantalla->numIconos; i > nIcon; i--) {
		pantalla->listaIconos[i] = pantalla->listaIconos[i - 1];
	}
	pantalla->listaIconos[nIcon] = destIcon;
	pantalla->numIconos++;
	pantalla->debeActualizar = TRUE;

	if (nPantallaActual != -1) {
		if (listaPantalla[nPantallaActual]->numIconos == 0 && numPantallas > 1) {
			delete listaPantalla[nPantallaActual];
			for (int i = nPantallaActual; i < int(numPantallas - 1); i++) {
				listaPantalla[i] = listaPantalla[i + 1];
			}
			
			listaPantalla[numPantallas - 1] = NULL;
			numPantallas--;
		}
	}
	
	return icon;
}

BOOL CListaPantalla::borraIcono(int posScreen, int posIcon)
{
	if (listaPantalla == NULL || posScreen >= int(numPantallas)) {
		return FALSE;
	}

	if (posScreen == -1) {
		barraInferior->borraIcono(posIcon);
	} else {
		listaPantalla[posScreen]->borraIcono(posIcon);

		if (listaPantalla[posScreen]->numIconos == 0 && numPantallas > 1) {
			delete listaPantalla[posScreen];
			for (int i = posScreen; i < int(numPantallas - 1); i++) {
				listaPantalla[i] = listaPantalla[i + 1];
			}
			listaPantalla[numPantallas - 1] = NULL;
			numPantallas--;
		}
	}

	return TRUE;
}
