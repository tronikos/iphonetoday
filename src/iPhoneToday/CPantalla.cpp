#include "CPantalla.h"

CPantalla::CPantalla(void)
{
	debeActualizar = TRUE;
	header[0] = '\0';
	hDC = NULL;
	imagen = NULL;
	imagenOld = NULL;
	pBits = NULL;
	mask_hDC = NULL;
	mask_imagen = NULL;
	mask_imagenOld = NULL;

	numIconos = 0;
	for (int i = 0; i < MAX_ICONOS_PANTALLA; i++) {
		listaIconos[i] = NULL;
	}
}

CPantalla::~CPantalla(void)
{
	for (int i = 0; i < MAX_ICONOS_PANTALLA; i++) {
		if (listaIconos[i] != NULL) {
			delete listaIconos[i];
		}
	}
	if (hDC != NULL) {
		if (hFontOld != NULL) {
			DeleteObject(SelectObject(hDC, hFontOld));
		}
		if (imagenOld != NULL) {
			SelectObject(hDC, imagenOld);
		}
		DeleteDC(hDC);
		hDC = NULL;
		if (imagen != NULL) {
			DeleteObject(imagen);
			imagen = NULL;
		}
	}
	if (mask_hDC != NULL) {
		if (mask_imagenOld != NULL) {
			SelectObject(mask_hDC, mask_imagenOld);
		}
		DeleteDC(mask_hDC);
		mask_hDC = NULL;
		if (mask_imagen != NULL) {
			DeleteObject(mask_imagen);
			mask_imagen = NULL;
		}
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
	listaIconos[numIconos - 1] = NULL;
	numIconos--;
	debeActualizar = TRUE;

	return TRUE;
}
