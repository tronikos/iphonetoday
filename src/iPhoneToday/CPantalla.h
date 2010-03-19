#pragma once

#include "stdafx.h"
#include "CIcono.h"

class CPantalla
{
public:
	CIcono *listaIconos[MAX_ICONOS_PANTALLA];
	UINT numIconos;
	BOOL debeActualizar;

	FLOAT x;
	FLOAT y;
	TCHAR header[MAX_PATH];
	UINT anchoPantalla;	// screen width
	UINT altoPantalla;	// screen height

	HDC hDC;
	HBITMAP imagen;
	HBITMAP imagenOld;
	BYTE *pBits;

	HDC mask_hDC;
	HBITMAP mask_imagen;
	HBITMAP mask_imagenOld;

	HFONT hFontOld;

	CPantalla(void);
	~CPantalla(void);

	// create icon
	CIcono *creaIcono(int posIcon = -1);
	// delete icon
	BOOL borraIcono(int posIcon);
};
