/*******************************************************************
This file is part of iContact.

iContact is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

iContact is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with iContact.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************/

#include "GraphicFunctions.h"
#include <commdlg.h>

/*
#if _WIN32_WCE < 0x500
typedef struct _BLENDFUNCTION
{
    BYTE   BlendOp;
    BYTE   BlendFlags;
    BYTE   SourceConstantAlpha;
    BYTE   AlphaFormat;
}BLENDFUNCTION,*PBLENDFUNCTION;

#define AC_SRC_OVER                 0x00
#define AC_SRC_ALPHA                0x01      // premultiplied alpha
#define AC_SRC_ALPHA_NONPREMULT     0x02      // non-premultiplied alpha
#endif

typedef BOOL (STDAPICALLTYPE FAR fAlphaBlend)(HDC,int,int,int,int,HDC,int,int,int,int,BLENDFUNCTION);

void BltAlpha(HDC hdcDest, int nXOriginDest, int nYOriginDest,
              int nWidthDest, int nHeightDest,
              HDC hdcSrc, int nXOriginSrc, int nYoriginSrc,
              int nWidthSrc, int nHeightSrc,
              BYTE alpha) {
	static HMODULE hLib = NULL;
	static fAlphaBlend *pAlphaBlend = NULL;

	if (hLib == NULL) {
		hLib = LoadLibrary(L"coredll.dll");
		pAlphaBlend = (fAlphaBlend*)GetProcAddress(hLib, L"AlphaBlend");
	}
	if (pAlphaBlend == NULL) {
		TransparentBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYoriginSrc, nWidthSrc, nHeightSrc, RGBA(0, 0, 0, 0));
		//BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, 0, SRCCOPY);
	} else {
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = alpha;
		bf.AlphaFormat = AC_SRC_ALPHA;
		pAlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, 
			hdcSrc, nXOriginSrc, nYoriginSrc, nWidthSrc, nHeightSrc, bf);
	}
}

void BltAlpha(HDC hdcDest, int nLeft, int nTop, int nWidth, int nHeight, 
			  HDC hdcSrc, BYTE alpha) {

    BltAlpha(hdcDest, nLeft, nTop, nWidth, nHeight, 
		hdcSrc, 0, 0, nWidth, nHeight, alpha);
}

HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
    HDC hdcMem, hdcMem2;
    HBITMAP hbmMask;
    BITMAP bm;

    // Create monochrome (1 bit) mask bitmap.  
    GetObject(hbmColour, sizeof(BITMAP), &bm);

    hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

    // Get some HDCs that are compatible with the display driver

    hdcMem = CreateCompatibleDC(0);
    hdcMem2 = CreateCompatibleDC(0);

    // SelectBitmap(hdcMem, hbmColour);
    // SelectBitmap(hdcMem2, hbmMask);
	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

    // Set the background colour of the colour image to the colour
    // you want to be transparent.
    SetBkColor(hdcMem, crTransparent);

    // Copy the bits from the colour image to the B+W mask... everything
    // with the background colour ends up white while everythig else ends up
    // black...Just what we wanted.
    BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Take our new mask and use it to turn the transparent colour in our
    // original colour image to black so the transparency effect will
    // work right.
    BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);


    // Clean up.
    DeleteDC(hdcMem);
    DeleteDC(hdcMem2);

    return hbmMask;
}
*/

void drawCircle(HDC hDC, float centerX, float centerY, float diameter, COLORREF color, COLORREF colorOuter) {
	float x = centerX - diameter / 2.0f;
	float y = centerY - diameter / 2.0f;
	drawEllipse(hDC, (int) (x + 0.5f), (int) (y + 0.5f), (int) (x + diameter + 0.5f), (int) (y + diameter + 0.5f), color, NULL, colorOuter);
}

// Draw an ellipse
void drawEllipse(HDC hDC, int x, int y, int x2, int y2, COLORREF color, TCHAR *texto, COLORREF colorOuter) {
	HPEN hEllipsePen;
	hEllipsePen = CreatePen(PS_SOLID, 1, colorOuter);
	// hEllipsePen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);
	HPEN hPenOld = (HPEN)SelectObject(hDC, hEllipsePen);
	HBRUSH hBrushOld = (HBRUSH)SelectObject(hDC, hBrush);

	Ellipse(hDC, x, y, x2, y2);

	if (texto != NULL) {
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = x2;
		rect.bottom = y2;

		DrawText(hDC, texto, -1, &rect, DT_CENTER | DT_VCENTER);
	}

	SelectObject(hDC, hBrushOld);
	SelectObject(hDC, hPenOld);
	DeleteObject(hEllipsePen);
	DeleteObject(hBrush);
}

/*
HBITMAP HBITMAPFromImage (IImage *pImage) {

	HRESULT hr;
	HBITMAP hbmResult = NULL;
	ImageInfo ii;
	HDC hDC = NULL;
	HBITMAP hbmNew = NULL;
	void * pv;
	BITMAPINFO bmi = { 0 };
	HBITMAP hbmOld = NULL;
	RECT rc = { 0 };

	CBR(pImage != NULL);

	// Get image width/height
	hr = pImage->GetImageInfo(&ii);
	CHR(hr);

	// Create HDC
	hDC = CreateCompatibleDC(NULL);
	CBR(hDC != NULL);

	// Create DIB section
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ii.Width;
	bmi.bmiHeader.biHeight = ii.Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 16; //(SHORT) max(16, GetDeviceCaps(hDC, BITSPIXEL));
	bmi.bmiHeader.biCompression = BI_RGB;

	hbmNew = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pv, NULL, 0);
	CBR(hbmNew != NULL);

	// Select DIB into DC
	hbmOld = (HBITMAP)SelectObject(hDC, hbmNew);

	rc.right = ii.Width;
	rc.bottom = ii.Height;

	// Clear the bitmap using the background color
	SetBkMode(hDC, TRANSPARENT);

	// Draw into DC/DIB
	hr = pImage->Draw(hDC, &rc, NULL);
	CHR(hr);

	hbmResult = hbmNew;
	hbmNew = NULL;

Error:
	if (hbmNew)
	{
		DeleteObject(hbmNew); 
	}

	if (hDC)
	{
		if (hbmOld)
		{
			SelectObject(hDC, hbmOld);
		}

		DeleteDC(hDC);
	}

	return hbmResult;
}
*/

void DrawGradientGDI(HDC tdc, const RECT& iRect, COLORREF StartRGB, COLORREF EndRGB, COLOR16 alpha) {
           
  unsigned int Shift = 8;
  TRIVERTEX        vert[2] ;
  GRADIENT_RECT    gRect;
  vert [0] .x      = iRect.left;
  vert [0] .y      = iRect.top;
  vert [0] .Red    = GetRValue(StartRGB) << Shift;
  vert [0] .Green  = GetGValue(StartRGB) << Shift;
  vert [0] .Blue   = GetBValue(StartRGB) << Shift;
  vert [0] .Alpha  = alpha;
  vert [1] .x      = iRect.right;
  vert [1] .y      = iRect.bottom; 
  vert [1] .Red    = GetRValue(EndRGB) << Shift;
  vert [1] .Green  = GetGValue(EndRGB) << Shift;
  vert [1] .Blue   = GetBValue(EndRGB) << Shift;
  vert [1] .Alpha  = alpha;
  gRect.UpperLeft  = 0;
  gRect.LowerRight = 1;
  GradientFill(tdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

BOOL ColorSelector(HWND hwndOwner, COLORREF rgbCurrent, COLORREF *nextColor) {

	BOOL result = FALSE;

	CHOOSECOLOR cc;                 // common dialog box structure 
	COLORREF acrCustClr[16]; // array of custom colors 

	// Initialize CHOOSECOLOR 
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwndOwner;
	cc.lpCustColors = (LPDWORD) acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_ANYCOLOR | CC_RGBINIT;
	 
	if (ChooseColor(&cc)==TRUE) 
	{
		*nextColor = cc.rgbResult;
		result = TRUE;
	} else {
		*nextColor = rgbCurrent;
	}
	free(&cc);

	return result;
}

BOOL AlphaBlend2(BITMAP *bmDst, int xDstOrg, int yDstOrg, BITMAP *bmSrc, int xSrcOrg, int ySrcOrg, int cx, int cy)
{
	if (bmDst->bmBits == NULL || bmSrc->bmBits == NULL || bmDst->bmBitsPixel != 32 || bmSrc->bmBitsPixel != 32) {
		return FALSE;
	}

	BYTE *pD;
	BYTE *pS;
	cx = min(cx, bmDst->bmWidth - xDstOrg);
	cx = min(cx, bmSrc->bmWidth - xSrcOrg);
	cy = min(cy, bmDst->bmHeight - yDstOrg);
	cy = min(cy, bmSrc->bmHeight - ySrcOrg);

	yDstOrg = bmDst->bmHeight - yDstOrg - cy;
	ySrcOrg = bmSrc->bmHeight - ySrcOrg - cy;

	for (int y = 0; y < cy; y++) {
		pD = (BYTE *) bmDst->bmBits + 4 * ((y + yDstOrg) * bmDst->bmWidth + xDstOrg);
		pS = (BYTE *) bmSrc->bmBits + 4 * ((y + ySrcOrg) * bmSrc->bmWidth + xSrcOrg);
		for (int x = 0; x < cx; x++) {
			BYTE a = pS[3];
			if (a != 0) {
				if (a == 0xFF) {
					pD[0] = pS[0];
					pD[1] = pS[1];
					pD[2] = pS[2];
				} else {
					BYTE na = ~a;
					pD[0] = ((na * pD[0]) >> 8) + pS[0];
					pD[1] = ((na * pD[1]) >> 8) + pS[1];
					pD[2] = ((na * pD[2]) >> 8) + pS[2];
				}
			}
			pD += 4;
			pS += 4;
		}
	}

	return TRUE;
}

int DrawText2(HDC hDC, LPCTSTR lpString, int nCount, LPRECT lpRect, UINT uFormat,
			 BOOL bRoundRect, UINT uTextShadow)
{
	if (bRoundRect) {
		RECT rc = {0};
		DrawText(hDC, lpString, nCount, &rc, uFormat | DT_CALCRECT);
		int tmp = (lpRect->right - lpRect->left - rc.right) / 2;
		HBRUSH hBrush = CreateSolidBrush(RGB(10, 10, 10));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(hDC, hBrush);
		RoundRect(hDC, lpRect->left + tmp - rc.bottom / 2, lpRect->top - 1, lpRect->right - tmp + rc.bottom / 2, lpRect->bottom + 1, rc.bottom - 2, rc.bottom - 2);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	} else if (uTextShadow > 0) {
		COLORREF colorOld = SetTextColor(hDC, RGB(10, 10, 10));
		RECT posTexto2;
		posTexto2.left = lpRect->left + uTextShadow;
		posTexto2.top = lpRect->top + uTextShadow;
		posTexto2.right = lpRect->right + uTextShadow;
		posTexto2.bottom = lpRect->bottom + uTextShadow;
		DrawText(hDC, lpString, nCount, &posTexto2, uFormat);
		SetTextColor(hDC, colorOld);
	}
	return DrawText(hDC, lpString, nCount, lpRect, uFormat);
}
