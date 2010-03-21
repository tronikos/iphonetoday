#include "stdafx.h"
#include "CIcono.h"
#include "iPhoneToday.h"
#include "lpng.h"

#ifdef TIMING
long loadImage_duration = 0;
long loadImage_load_duration = 0;
long loadImage_resize_duration = 0;
long loadImage_fix_duration = 0;
#endif

IBitmapImage* LoadImageWithImagingApi(BitmapData *lockedBitmapData, LPTSTR strFileName, int width, int height, int bitsPerPixel, float factor);
HBITMAP LoadImageWithImgdecmp(LPTSTR strFileName);

CIcono::CIcono(void)
{
	imagen = NULL;
	hDC = NULL;
	imagenOld = NULL;

	defaultValues();
}

CIcono::~CIcono(void)
{
	clearImageObjects();
}

void CIcono::defaultValues()
{
	nombre[0] = 0;
	rutaImagen[0] = 0;
	sound[0] = 0;
	ejecutable[0] = 0;
	parametros[0] = 0;
	ejecutableAlt[0] = 0;
	parametrosAlt[0] = 0;
	tipo = NOTIF_NORMAL;
	launchAnimation = 1;
}

void CIcono::loadImage(HDC *hDC, TCHAR *pathImage, int width, int height, int bitsPerPixel, float factor, BOOL alwaysPremultiply) {
	clearImageObjects();

	if (pathImage == NULL || !FileExists(pathImage)) {
		return;
	}

	TIMER_START(loadImage_duration);

	HDC hdcResult = NULL;
	HBITMAP hbmResult = NULL;
	HBITMAP hbmResultOld = NULL;
	BYTE *pBits = NULL;

	const TCHAR *ext = wcsrchr(pathImage, '.');
	BOOL isPNG = lstrcmpi(ext, L".png") == 0;
	BOOL isBMP = lstrcmpi(ext, L".bmp") == 0;

	// loaded bitmap by the Imaging API (already resized)
	IBitmapImage *pBitmap = NULL;
	BitmapData lockedBitmapData;

	// loaded bitmap by the other methods (needs to be resized)
	HBITMAP hbmTemp = NULL;

	BOOL byImgdecmp = FALSE;
	BOOL loaded = FALSE;

	TIMER_START(loadImage_load_duration);

	if (!loaded && isBMP) {
		hbmTemp = SHLoadDIBitmap(pathImage);
		loaded = (hbmTemp != NULL);
	}
	if (!loaded && g_pImgFactory != NULL ) {
		pBitmap = LoadImageWithImagingApi(&lockedBitmapData, pathImage, width, height, bitsPerPixel, factor);
		if (pBitmap) {
			pBits = (BYTE *) lockedBitmapData.Scan0;
			width = lockedBitmapData.Width;
			height = lockedBitmapData.Height;
		}
		loaded = (pBitmap != NULL);
	}
	if (!loaded && isPNG) {
		hbmTemp = LoadPng(pathImage, 0, 0, FALSE);
		loaded = (hbmTemp != NULL);
	}
	if (!loaded && g_hImgdecmpDll != NULL) {
		hbmTemp = LoadImageWithImgdecmp(pathImage);
		if (hbmTemp) byImgdecmp = TRUE;
		loaded = (hbmTemp != NULL);
	}

	TIMER_STOP(loadImage_load_duration);

	if (!loaded) {
		return;
	}

	// Create result hdc
	hdcResult = CreateCompatibleDC(*hDC);

	TIMER_START(loadImage_resize_duration);

	if (hbmTemp) {
		HDC hdcTemp = CreateCompatibleDC(GetDC(NULL));
		HBITMAP hbmTempOld = (HBITMAP) SelectObject(hdcTemp, hbmTemp);

		BITMAP bm;
		GetObject(hbmTemp, sizeof(BITMAP), &bm);
		int widthTemp = bm.bmWidth;
		int heightTemp = bm.bmHeight;

		// get desired width and height
		if (width == 0 && height == 0) {
			width = int(widthTemp * factor);
			height = int(heightTemp * factor);
		} else {
			if (width == 0) {
				width = widthTemp * height / heightTemp / 2 * 2;
			}
			if (height == 0) {
				height = heightTemp * width / widthTemp / 2 * 2;
			}
		}

		// Create result dib bitmap
		BITMAPINFO bmInfo;
		ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = width;
		bmInfo.bmiHeader.biHeight = height;
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = GetPixelFormatSize(bitsPerPixel);
		hbmResult = CreateDIBSection(hdcResult, (BITMAPINFO *)&bmInfo, DIB_RGB_COLORS, (void **)&pBits, NULL, 0);
		hbmResultOld = (HBITMAP) SelectObject(hdcResult, hbmResult);

		// Resize the temp hdc and copy it to the result hdc
		//SetStretchBltMode(hdcResult, BILINEAR);
		StretchBlt(hdcResult, 0, 0, width, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);

		// Clean up temp dc
		SelectObject(hdcTemp, hbmTempOld);
		DeleteObject(hbmTemp);
		DeleteDC(hdcTemp);
	}

	TIMER_STOP(loadImage_resize_duration);

	TIMER_START(loadImage_fix_duration);

	if (isBMP && GetPixelFormatSize(bitsPerPixel) == 32 && pBits != NULL) {
		BOOL blackAsTransparent = TRUE;
		if (configuracion != NULL) {
			if (!configuracion->alphaBlend) {
				blackAsTransparent = configuracion->transparentBMP;
			}
		}
		if (!blackAsTransparent) {
			BYTE *p = pBits;
			for (int i = 0; i < width * height; i++) {
				// Replace black colored pixels with semi-black pixels
				if (p[0] < 0x0A && p[1] < 0x0A && p[2] < 0x0A) {
					p[0] = 0x0A;
					p[1] = 0x0A;
					p[2] = 0x0A;
				}
				p += 4;
			}
		}
	}

	// Since imgdecmp doesn't support transparency use the color of the lower left corner pixel as transparent
	if (byImgdecmp && isPNG && GetPixelFormatSize(bitsPerPixel) == 32 && pBits != NULL) {
		BYTE *p = pBits;
		// get color of lower left corner pixel
		COLORREF llc = *(COLORREF*)p;
		for (int i = 0; i < width * height; i++) {
			// set all the pixels that have llc color as transparent
			COLORREF *c = (COLORREF*)p;
			if (*c == llc) {
				*c = RGBA(0, 0, 0, 0);
			// Replace black colored pixels with semi-black pixels
			} else if (p[0] < 0x0A && p[1] < 0x0A && p[2] < 0x0A) {
				*c = RGBA(0x0A, 0x0A, 0x0A, 0xFF);
			//} else {
			//	p[3] = 0xFF;
			}
			p += 4;
		}
	}

	if (!byImgdecmp && isPNG && GetPixelFormatSize(bitsPerPixel) == 32 && pBits != NULL) {
		BOOL alphaBlend = FALSE;
		BYTE alphaThreshold = 25;
		BOOL premultiply = FALSE;
		BOOL fixBlack = TRUE;
		if (configuracion != NULL) {
			alphaBlend = configuracion->alphaBlend;
			alphaThreshold = configuracion->alphaThreshold;
			premultiply = configuracion->alphaOnBlack;
		}
		// if the alphaBlend option is set,
		// then the pixels will be pre-multiplied later once they have been copied to the page's hdc
		// and before they are alpha blended to the window's hdc
		if (alphaBlend) {
			alphaThreshold = 0;
			premultiply = FALSE;
			fixBlack = FALSE;
		}
		// if it is a pressed icon then it has to be pre-multiplied now
		if (alwaysPremultiply) {
			alphaThreshold = 0;
			premultiply = TRUE;
			fixBlack = FALSE;
		}

		BYTE *p = (BYTE *) pBits;
		for (int i = 0; i < width * height; i++) {
			BYTE A = p[3];
			if (premultiply) {
				p[0] = (BYTE)((p[0] * A) >> 8);
				p[1] = (BYTE)((p[1] * A) >> 8);
				p[2] = (BYTE)((p[2] * A) >> 8);
			}
			// Replace semi-transparent pixels (alpha value <= alphaThreshold) with black
			if (A <= alphaThreshold) {
				p[0] = 0;
				p[1] = 0;
				p[2] = 0;
				p[3] = 0;
			// Replace black colored pixels with semi-black pixels
			} else if (fixBlack && p[0] < 0x0A && p[1] < 0x0A && p[2] < 0x0A) {
				p[0] = 0x0A;
				p[1] = 0x0A;
				p[2] = 0x0A;
			}
			p += 4;
		}
	}

	// if the bitmap was loaded by the Imaging API create the result dib bitmap
	if (pBitmap) {
		hbmResult = CreateBitmap(lockedBitmapData.Width, lockedBitmapData.Height, 1,
			GetPixelFormatSize(lockedBitmapData.PixelFormat), lockedBitmapData.Scan0);
		pBitmap->UnlockBits(&lockedBitmapData);
		pBitmap->Release();
		hbmResultOld = (HBITMAP) SelectObject(hdcResult, hbmResult);
	}

	// if alphablend is disabled and this icon is not a pressed icon
	// then in order to save memory convert the 32bit image to whatever the screen supports (usually 16bit)
	if (!configuracion->alphaBlend && !alwaysPremultiply && GetPixelFormatSize(bitsPerPixel) == 32) {
		HDC hdc = CreateCompatibleDC(hdcResult);
		HBITMAP hbmp = CreateCompatibleBitmap(*hDC, width, height);
		HBITMAP hbmpOld = (HBITMAP) SelectObject(hdc, hbmp);
		BitBlt(hdc, 0, 0, width, height, hdcResult, 0, 0, SRCCOPY);
		SelectObject(hdcResult, hbmResultOld);
		DeleteObject(hbmResult);
		DeleteDC(hdcResult);
		hdcResult = hdc;
		hbmResult = hbmp;
		hbmResultOld = hbmpOld;
	}

	TIMER_STOP(loadImage_fix_duration);

	this->hDC = hdcResult;
	this->imagen = hbmResult;
	this->imagenOld = hbmResultOld;
	this->anchoImagen = width;
	this->altoImagen = height;

	TIMER_STOP(loadImage_duration);

	return;
}

void CIcono::loadImageFromExec(HDC *hDC, TCHAR *pathExec, int width, int height) {
	clearImageObjects();

	if (pathExec == NULL || !FileExists(pathExec)) {
		return;
	}

	// get icon from exe
	SHFILEINFO cbFileInfo;
	SHGetFileInfo(pathExec, 0, &cbFileInfo, sizeof(cbFileInfo), SHGFI_ICON | SHGFI_LARGEICON);
	HICON icon = cbFileInfo.hIcon;
	if (icon == NULL) {
		return;
	}

	// create result hdc
	HDC hdcResult = CreateCompatibleDC(*hDC);
	HBITMAP hbmResult = CreateCompatibleBitmap(*hDC, width, height);
	HBITMAP hbmResultOld = (HBITMAP) SelectObject(hdcResult, hbmResult);

	// fill with black
	RECT rc = {0, 0, width, height};
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdcResult, &rc, hBrush);
	DeleteObject(hBrush);

	// draw the icon
	SetBkMode(hdcResult, TRANSPARENT);
	DrawIconEx(hdcResult, 0, 0, icon, width, height, 0, NULL, DI_NORMAL);
	DestroyIcon(icon);

	this->hDC = hdcResult;
	this->imagen = hbmResult;
	this->imagenOld = hbmResultOld;
	this->anchoImagen = width;
	this->altoImagen = height;
}

void CIcono::clearImageObjects()
{
	if(imagen != NULL && imagenOld != NULL && hDC != NULL) {
		SelectObject(hDC, imagenOld);
		DeleteDC(hDC);
		DeleteObject(imagen);
		imagen = NULL;
		hDC = NULL;
	}
}

///////////////////////////////////////////////////////////

IBitmapImage* LoadImageWithImagingApi(BitmapData *lockedBitmapData, LPTSTR strFileName, int width, int height, int bitsPerPixel, float factor)
{
	IBitmapImage *pBitmap = NULL;

	//IImagingFactory *g_pImgFactory = NULL;
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//if (SUCCEEDED(CoCreateInstance(CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) &g_pImgFactory))) {
		IImage *pImage = NULL;
		ImageInfo ii;
		if (SUCCEEDED(g_pImgFactory->CreateImageFromFile(strFileName, &pImage))
			&& SUCCEEDED(pImage->GetImageInfo(&ii)))
		{
			int widthTemp = ii.Width;
			int heightTemp = ii.Height;

			// get desired width and height
			if (width == 0 && height == 0) {
				width = int(widthTemp * factor);
				height = int(heightTemp * factor);
			} else {
				if (width == 0) {
					width = widthTemp * height / heightTemp / 2 * 2;
				}
				if (height == 0) {
					height = heightTemp * width / widthTemp / 2 * 2;
				}
			}
			if (SUCCEEDED(g_pImgFactory->CreateBitmapFromImage(pImage, width, height, bitsPerPixel, InterpolationHintAveraging, &pBitmap))) {
				RECT rect = {0, 0, width, height};
				lockedBitmapData->Width = width;
				lockedBitmapData->Height = height;
				lockedBitmapData->PixelFormat = ii.PixelFormat;
				pBitmap->LockBits(&rect, ImageLockModeRead, bitsPerPixel, lockedBitmapData);
				//hResult = CreateBitmap(lockedBitmapData->Width, lockedBitmapData->Height, 1,
				//	GetPixelFormatSize(lockedBitmapData->PixelFormat), lockedBitmapData->Scan0);
				//pBitmap->UnlockBits(lockedBitmapData);
			}
			//if (pBitmap) pBitmap->Release();
		}
		if (pImage) pImage->Release();
	//}
	//if (g_pImgFactory) g_pImgFactory->Release();
	//CoUninitialize();

	return pBitmap;
}

///////////////////////////////////////////////////////////

static DWORD CALLBACK GetImageData(LPSTR szBuffer, DWORD dwBufferMax, LPARAM lParam)
{
   FILE* f = (FILE*)lParam;
   size_t c = fread(szBuffer, 1, dwBufferMax, f);
   return c;
}

#ifndef DecompressImageInfo

class IImageRender;
typedef void (CALLBACK *PROGRESSFUNC)(IImageRender *pRender, BOOL bComplete, LPARAM lParam);
typedef DWORD (CALLBACK *GETDATAFUNC)(LPSTR szBuffer, DWORD dwBufferMax, LPARAM lParam);

typedef struct tagDecompressImageInfo {
   DWORD dwSize;
   LPBYTE pbBuffer;
   DWORD dwBufferMax;
   DWORD dwBufferCurrent;
   HBITMAP* phBM;
   IImageRender **ppImageRender;
   int iBitDepth;
   LPARAM lParam;
   HDC hdc;
   int iScale;
   int iMaxWidth;
   int iMaxHeight;
   GETDATAFUNC pfnGetData;
   PROGRESSFUNC pfnImageProgress;
   COLORREF crTransparentOverride;
} DecompressImageInfo;

#endif

HBITMAP LoadImageWithImgdecmp(LPTSTR strFileName)
{
   FILE* f = _tfopen(strFileName, TEXT("rb"));
   if (! f) {
      return 0;
   }

   DecompressImageInfo dii;

   HBITMAP hBitmap = 0;

   const int nBuffSize = 4096;
   BYTE buff[nBuffSize];

   dii.dwSize = sizeof(DecompressImageInfo);
   dii.pbBuffer = buff;
   dii.dwBufferMax = nBuffSize;
   dii.dwBufferCurrent = 0;
   dii.phBM = &hBitmap;
   dii.ppImageRender = NULL;
   dii.iBitDepth = 16;
   dii.lParam = LPARAM(f);
   dii.hdc = 0;
   dii.iScale = 100;
   dii.iMaxWidth = 1000;
   dii.iMaxHeight = 1000;
   dii.pfnGetData = GetImageData;
   dii.pfnImageProgress = 0;
   dii.crTransparentOverride = (UINT) -1;

   //HINSTANCE g_hImgdecmpDll = LoadLibrary(TEXT("imgdecmp.dll"));
   if (!g_hImgdecmpDll) {
      fclose(f);
      return 0;
   }
   typedef HRESULT (*DecompressImageIndirect_t)(DecompressImageInfo *pParams);
   DecompressImageIndirect_t proc = (DecompressImageIndirect_t)GetProcAddress(g_hImgdecmpDll, TEXT("DecompressImageIndirect"));
   if (! proc) {
      //FreeLibrary(g_hImgdecmpDll);
      fclose(f);
      return 0;
   }
   HRESULT hr = proc(&dii);

   fclose(f);
   //FreeLibrary(g_hImgdecmpDll);

   if (FAILED(hr)) {
      return 0;
   } else {
      return hBitmap;
   }
}

