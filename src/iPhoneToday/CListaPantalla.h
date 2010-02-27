#pragma once

#include "CPantalla.h"

class CListaPantalla
{
public:
	CPantalla *listaPantalla[MAX_PANTALLAS];
	UINT numPantallas;

	CPantalla *barraInferior;
	CPantalla *topBar;

	CListaPantalla(void);
	~CListaPantalla(void);

	// create screen
	CPantalla *creaPantalla();
	// move icon
	CIcono *mueveIcono(int nPantallaActual, int nIconoActual, int nScreen, int nIcon);
	// copy icon
	CIcono *copyIcono(int nPantallaActual, int nIconoActual, int nScreen, int nIcon, CIcono *destIcon);
	// delete icon
	BOOL borraIcono(int posScreen, int posIcon);
};
