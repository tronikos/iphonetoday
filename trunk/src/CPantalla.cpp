#include "CPantalla.h"

CPantalla::CPantalla(void)
{
	debeActualizar = TRUE;
	imagen = NULL;
	hDC = NULL;

	numIconos = 0;
	for (int i = 0; i < MAX_ICONOS_PANTALLA; i++) {
		listaIconos[i] = NULL;
	}

	fondoPantalla = NULL;
}

CPantalla::~CPantalla(void)
{
	for (int i = 0; i < MAX_ICONOS_PANTALLA; i++) {
		if (listaIconos[i] != NULL) {
			delete listaIconos[i];
		}
	}
	if(imagen != NULL && imagenOld != NULL && hDC != NULL) {
		SelectObject(hDC, imagenOld);
		DeleteDC(hDC);
		DeleteObject(imagen);
		imagen = NULL;
		hDC = NULL;
	}
	if (hFontOld != NULL) {
		DeleteObject(hFontOld);
	}
	if (imagenMascara != NULL) {
		DeleteObject(imagenMascara);
	}
	if (hDCMascara != NULL) {
		DeleteDC(hDCMascara);
	}
	if (fondoPantalla != NULL){
		delete fondoPantalla;
	}
}

CIcono *CPantalla::creaIcono(int posIcon)
{
	CIcono *newIcon = new CIcono();
	if (posIcon >= int(numIconos)) {
		posIcon = -1;
	}

	if (posIcon >= 0) {
		for (int i = numIconos; i > posIcon; i--) {
			listaIconos[i] = listaIconos[i - 1];
		}
		listaIconos[posIcon] = newIcon;
	} else {
		listaIconos[numIconos] = newIcon;
	}
	numIconos++;
	debeActualizar = TRUE;

	return newIcon;
}

BOOL CPantalla::borraIcono(int posIcon)
{
	if (listaIconos == NULL || posIcon >= int(numIconos)) {
		return FALSE;
	}

	delete listaIconos[posIcon];
	for (int i = posIcon; i < int(numIconos - 1); i++) {
		listaIconos[i] = listaIconos[i + 1];
	}
	// delete listaIconos[numIconos - 1];
	listaIconos[numIconos - 1] = NULL;
	numIconos--;
	debeActualizar = TRUE;

	return TRUE;
}