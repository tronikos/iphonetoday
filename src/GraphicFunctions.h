#pragma once

#include "stdafx.h"
#include <commdlg.h>
#include <imaging.h>

void BltAlpha(HDC, int, int, int, int, HDC, BYTE);
void BltAlpha(HDC, int, int, int, int, HDC, int, int, int, int, BYTE alpha);

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
void drawEllipse(HDC hDC, int x, int y, int x2, int y2, COLORREF color, TCHAR *texto);

HBITMAP HBITMAPFromImage (IImage *pImage);
void DrawGradientGDI(HDC, const RECT&, COLORREF, COLORREF, COLOR16 alpha = 0x0000);

BOOL ColorSelector(COLORREF rgbCurrent, COLORREF *nextColor);