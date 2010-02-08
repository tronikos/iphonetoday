#pragma once

#include "stdafx.h"

class CIcono
{
public:
	FLOAT x;
	FLOAT y;
	TCHAR nombre[MAX_PATH];
	TCHAR rutaImagen[MAX_PATH];
	TCHAR ejecutable[MAX_PATH];
	TCHAR parametros[MAX_PATH];
	TCHAR ejecutableAlt[MAX_PATH];
	TCHAR parametrosAlt[MAX_PATH];
	UINT tipo;
	UINT launchAnimation;

	HBITMAP imagen;
	HBITMAP imagenOld;
	HDC hDC;

	UINT anchoImagen;
	UINT altoImagen;

	CIcono(void);
	~CIcono(void);

	void loadImage(HDC *hDC, TCHAR *pathImage, int witdh = 0, int heigth = 0, int bitsPerPixel = PIXFMT_32BPP_ARGB);
	void loadImageFromExec(HDC *hDC, TCHAR *pathExec, int witdh = 0, int heigth = 0);
	void clearImageObjects();
};
