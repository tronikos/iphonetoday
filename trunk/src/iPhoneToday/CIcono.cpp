#include "stdafx.h"
#include "CIcono.h"
#include "iPhoneToday.h"

#ifdef TIMING
long loadImage_duration = 0;
long loadImage_IImagingFactory_CreateImageFromFile_duration = 0;
long loadImage_IImagingFactory_CreateBitmapFromImage_duration = 0;
#endif

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

	if (!FileExists(pathImage)) {
		return;
	}

	TIMER_START(loadImage_duration);
	// IImagingFactory* g_pImgFactory = NULL;
	HDC hdcResult;
	HBITMAP hbmResult;
	HBITMAP hbmResultOld;


	// initialize imaging API
	// CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// if (!SUCCEEDED(CoCreateInstance (CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) & g_pImgFactory)))
	// {
	// 	return;
	// }

	BOOL isPNG = lstrcmpi(wcsrchr(pathImage, '.'), L".png") == 0;
	BOOL isBMP = lstrcmpi(wcsrchr(pathImage, '.'), L".bmp") == 0;
	// If imaging API is not available or it is a .bmp use SHLoadDIBitmap that only supports .bmp images and is faster.
	if (g_pImgFactory == NULL || isBMP) {

		HDC hdcTemp;
		HBITMAP hbmTemp;
		HBITMAP hbmTempOld;

		HDC hdcTempNULL = GetDC(NULL);
		hdcTemp = CreateCompatibleDC(hdcTempNULL);
		if (g_hImgdecmpDll == NULL || isBMP) {
			hbmTemp = SHLoadDIBitmap(pathImage);
		} else {
			hbmTemp = LoadImageWithImgdecmp(pathImage);
		}
		BITMAP bm;
		GetObject(hbmTemp, sizeof(BITMAP), &bm);
		int widthTemp = bm.bmWidth;
		int heightTemp = bm.bmHeight;
		hbmTempOld = (HBITMAP)SelectObject(hdcTemp, hbmTemp);

		if (factor != 0) {
			width = int(widthTemp * factor);
			height = int(heightTemp * factor);
		}

		SetBkMode(hdcTemp, TRANSPARENT);

		// create a BITMAPINFO with minimal initilisation

		// for the CreateDIBSection
		BITMAPINFO bmInfo;
		ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
		bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmInfo.bmiHeader.biWidth = width;
		bmInfo.bmiHeader.biHeight = height;
		bmInfo.bmiHeader.biPlanes = 1;
		bmInfo.bmiHeader.biBitCount = GetPixelFormatSize(bitsPerPixel);

		// pointer used for direct Bitmap pixels access
		hdcResult = CreateCompatibleDC(*hDC);

		BYTE *ptPixels;
		hbmResult = CreateDIBSection(hdcResult,
			(BITMAPINFO *)&bmInfo,
			DIB_RGB_COLORS,
			(void **)&ptPixels,
			NULL, 0);

		hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);
		SetBkMode(hdcResult, TRANSPARENT);

		// Resize hdc
		StretchBlt(hdcResult, 0, 0, width, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);


		if (isPNG && GetPixelFormatSize(bitsPerPixel) == 32) {
			BYTE *p = ptPixels;
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

		if (isBMP && GetPixelFormatSize(bitsPerPixel) == 32) {
			BOOL blackAsTransparent = TRUE;
			if (configuracion != NULL) {
				if (!configuracion->alphaBlend) {
					blackAsTransparent = configuracion->transparentBMP;
				}
			}
			if (!blackAsTransparent) {
				BYTE *p = ptPixels;
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

		DeleteObject(SelectObject(hdcTemp, hbmTempOld));
		DeleteDC(hdcTemp);
		ReleaseDC(NULL, hdcTempNULL);

		this->hDC = hdcResult;
		this->imagen = hbmResult;
		this->imagenOld = hbmResultOld;
		this->anchoImagen = width;
		this->altoImagen = height;

		TIMER_STOP(loadImage_duration);
		return;
	}

	IImage* m_pImage = NULL;
	IBitmapImage *m_pBitmap = NULL;

	// load it
	TIMER_START(loadImage_IImagingFactory_CreateImageFromFile_duration);
	if( SUCCEEDED(g_pImgFactory->CreateImageFromFile(pathImage, &m_pImage )) )
	{
		TIMER_STOP(loadImage_IImagingFactory_CreateImageFromFile_duration);

		// Get image width/height
		ImageInfo ii;
		m_pImage->GetImageInfo(&ii);

		if (factor != 0) {
			width = int(ii.Width * factor);
			height = int(ii.Height * factor);
		}

		HRESULT hRet;
		TIMER_START(loadImage_IImagingFactory_CreateBitmapFromImage_duration);
		// hRet = m_pImgFactory->CreateBitmapFromImage(m_pImage, width, height, PIXFMT_16BPP_RGB565, InterpolationHintDefault, &m_pBitmap);
		// hRet = m_pImgFactory->CreateBitmapFromImage(m_pImage, width, height, PIXFMT_16BPP_ARGB1555, InterpolationHintDefault, &m_pBitmap);
		hRet = g_pImgFactory->CreateBitmapFromImage(m_pImage, width, height, bitsPerPixel, InterpolationHintDefault, &m_pBitmap);
		TIMER_STOP(loadImage_IImagingFactory_CreateBitmapFromImage_duration);
		if (hRet != S_OK) {
			return;
		}

		BitmapData  lockedBitmapData;
		RECT rect = {0, 0, width, height};

		lockedBitmapData.Width = width;
		lockedBitmapData.Height = height;
		lockedBitmapData.PixelFormat = ii.PixelFormat;

		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_32BPP_ARGB, &lockedBitmapData);
		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_16BPP_RGB565, &lockedBitmapData);
		m_pBitmap->LockBits(&rect, ImageLockModeRead, bitsPerPixel, &lockedBitmapData);

		if (isPNG && GetPixelFormatSize(bitsPerPixel) == 32) {
			BOOL alphaBlend = FALSE;
			if (configuracion != NULL) {
				alphaBlend = configuracion->alphaBlend;
			}

			// if the alphaBlend option is set,
			// the pixels will be pre-multiplied later once they have been copied to the page's hdc
			// and before they are alpha blended to the window's hdc
			if (!alphaBlend || alwaysPremultiply) {
				BYTE alphaThreshold = 25;
				BOOL premultiply = FALSE;
				if (configuracion != NULL) {
					alphaThreshold = configuracion->alphaThreshold;
					premultiply = configuracion->alphaOnBlack;
				}
				if (alwaysPremultiply) {
					alphaThreshold = 1;
					premultiply = TRUE;
				}

				BYTE *p = (BYTE *) lockedBitmapData.Scan0;
				for (int i = 0; i < width * height; i++) {
					BYTE A = p[3];
					if (premultiply) {
						p[0] = (BYTE)((p[0] * A) >> 8);
						p[1] = (BYTE)((p[1] * A) >> 8);
						p[2] = (BYTE)((p[2] * A) >> 8);
					}
					// Replace semi-transparent pixels (alpha value < alphaThreshold) with black
					if (A < alphaThreshold) {
						p[0] = 0;
						p[1] = 0;
						p[2] = 0;
						p[3] = 0;
					// Replace black colored pixels with semi-black pixels
					} else if (p[0] < 0x0A && p[1] < 0x0A && p[2] < 0x0A) {
						p[0] = 0x0A;
						p[1] = 0x0A;
						p[2] = 0x0A;
					}
					p += 4;
				}
			}
		}

		// Create HDC
		hdcResult = CreateCompatibleDC(*hDC);

		// Create HBITMAP
		hbmResult = CreateBitmap(lockedBitmapData.Width, lockedBitmapData.Height, 1,
			GetPixelFormatSize(lockedBitmapData.PixelFormat), lockedBitmapData.Scan0);

		m_pBitmap->UnlockBits(&lockedBitmapData);

		hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);

		// BITMAP bm;
		// GetObject(hbmResult, sizeof(BITMAP), &bm);
		// NKDbgPrintfW(L"Bits hbmResult pixel: %d\n", bm.bmBitsPixel);

		this->hDC = hdcResult;
		this->imagen = hbmResult;
		this->imagenOld = hbmResultOld;
		this->anchoImagen = width;
		this->altoImagen = height;
	} else {
		//err(( TEXT("CreateImageFromFile failed!") ));
		return;
	}

	// release
	if( m_pImage != NULL ) m_pImage->Release();
	if( m_pBitmap != NULL ) m_pBitmap->Release();
	// if( m_pImgFactory != NULL ) m_pImgFactory->Release();
	// CoUninitialize();

	TIMER_STOP(loadImage_duration);

	return;
}

void CIcono::loadImageFromExec(HDC *hDC, TCHAR *pathExec, int width, int height) {

	clearImageObjects();

	if (pathExec == NULL) {
		return;
	}

	SHFILEINFO cbFileInfo;
	SHGetFileInfo(pathExec, 0, &cbFileInfo, sizeof(cbFileInfo), SHGFI_ICON);





	HICON icon = cbFileInfo.hIcon;

	// ExtractIconEx(path, 0, &icon, NULL, 1);
	if (icon == NULL) {
		return;
	}

	HDC hdcTemp;
	HBITMAP hbmTemp;
	HBITMAP hbmTempOld;
	HBRUSH hBrush;

	// Create HDC
	int widthTemp = GetSystemMetrics(SM_CXICON);
	int heightTemp = GetSystemMetrics(SM_CYICON);
	HDC hdcTempNULL = GetDC(NULL);
	hdcTemp = CreateCompatibleDC(hdcTempNULL);
	hbmTemp = CreateCompatibleBitmap(hdcTempNULL, widthTemp, heightTemp);
	hbmTempOld = (HBITMAP)SelectObject(hdcTemp, hbmTemp);

	SetBkMode(hdcTemp, TRANSPARENT);

	RECT rc = {0, 0, widthTemp, heightTemp};
	hBrush = CreateSolidBrush(RGBA(0, 0, 0, 0));
	FillRect(hdcTemp, &rc, hBrush);

	DrawIcon(hdcTemp, 0, 0, icon);











	// create a BITMAPINFO with minimal initilisation

	// for the CreateDIBSection
	BITMAPINFO bmInfo;
	ZeroMemory(&bmInfo, sizeof(BITMAPINFO));
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = width;
	bmInfo.bmiHeader.biHeight = height;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;

	// pointer used for direct Bitmap pixels access
	HDC hdcResult = CreateCompatibleDC(*hDC);

	BYTE *ptPixels;
	HBITMAP hbmResult = CreateDIBSection(hdcResult,
		(BITMAPINFO *)&bmInfo,
		DIB_RGB_COLORS,
		(void **)&ptPixels,
		NULL, 0);
	HBITMAP hbmResultOld;

	hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);
	SetBkMode(hdcResult, TRANSPARENT);

	// Resize hdc
	StretchBlt(hdcResult, 0, 0, width, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);


	if (icon != NULL)  {
		DestroyIcon(icon);
	}

	DeleteObject(SelectObject(hdcTemp, hbmTempOld));
	DeleteDC(hdcTemp);
	DeleteObject(hBrush);
	ReleaseDC(NULL, hdcTempNULL);

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

