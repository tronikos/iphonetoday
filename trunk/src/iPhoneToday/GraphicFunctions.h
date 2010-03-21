#pragma once

#include <windows.h>

//void BltAlpha(HDC, int, int, int, int, HDC, BYTE);
//void BltAlpha(HDC, int, int, int, int, HDC, int, int, int, int, BYTE alpha);

//HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
void drawEllipse(HDC hDC, int x, int y, int x2, int y2, COLORREF color, TCHAR *texto);

//HBITMAP HBITMAPFromImage (IImage *pImage);
void DrawGradientGDI(HDC, const RECT&, COLORREF, COLORREF, COLOR16 alpha = 0x0000);

BOOL ColorSelector(HWND hwndOwner, COLORREF rgbCurrent, COLORREF *nextColor);

// AlphaBlends bitmap bmSrc to bmDst. Bitmaps are required to be DIB sections.
BOOL AlphaBlend2(BITMAP *bmDst, int xDstOrg, int yDstOrg, BITMAP *bmSrc, int xSrcOrg, int ySrcOrg, int cx, int cy);

int DrawText2(HDC hDC, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat, BOOL bRoundRect = FALSE, UINT uTextShadow = 0);
