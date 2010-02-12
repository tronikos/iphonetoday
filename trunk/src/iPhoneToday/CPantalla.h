#pragma once

#include "CIcono.h"

class CPantalla
{
public:
	CIcono *listaIconos[MAX_ICONOS_PANTALLA];
	UINT numIconos;
	BOOL debeActualizar;

	FLOAT x;
	FLOAT y;
	UINT anchoPantalla;
	UINT altoPantalla;
	HBITMAP imagen;
	HDC hDC;

	HBITMAP imagenOld;
	HFONT hFontOld;


	HBITMAP imagenMascara;
	HDC hDCMascara;
	CIcono *fondoPantalla;

	CPantalla(void);
	~CPantalla(void);

	CIcono *creaIcono(int posIcon = -1);
	BOOL borraIcono(int posIcon);
};
