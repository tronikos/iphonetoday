#pragma once

#include "CPantalla.h"

class CListaPantalla
{
public:
	CPantalla *listaPantalla[MAX_PANTALLAS];
	UINT numPantallas;

	CPantalla *barraInferior;

	CListaPantalla(void);
	~CListaPantalla(void);

	CPantalla *creaPantalla();
	CIcono *mueveIcono(int nPantallaActual, int nIconoActual, int nScreen, int nIcon);
	BOOL borraIcono(int posScreen, int posIcon);
};

