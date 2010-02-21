#include "stdafx.h"
#include "CIcono.h"
#include "iPhoneToday.h"

#ifdef TIMING
long loadImage_duration = 0;
long loadImage_IImagingFactory_CreateImageFromFile_duration = 0;
long loadImage_IImagingFactory_CreateBitmapFromImage_duration = 0;
#endif

HBITMAP LoadImageWithImgdecmp(HDC hDC, LPTSTR strFileName);

CIcono::CIcono(void)
{
	imagen = NULL;
	hDC = NULL;
	imagenOld = NULL;

	launchAnimation = 1;
}

CIcono::~CIcono(void)
{
	clearImageObjects();
}

void CIcono::loadImage(HDC *hDC, TCHAR *pathImage, int width, int height, int bitsPerPixel) {
	TIMER_START(loadImage_duration);
	// IImagingFactory* g_pImgFactory = NULL;
	HDC hdcResult;
	HBITMAP hbmResult;
	HBITMAP hbmResultOld;

	clearImageObjects();

	// initialize imaging API
	// CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// if (!SUCCEEDED(CoCreateInstance (CLSID_ImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IImagingFactory, (void **) & g_pImgFactory)))
	// {
	// 	return;
	// }

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
			hbmTemp = LoadImageWithImgdecmp(*hDC, pathImage);
		}
		BITMAP bm;
		GetObject(hbmTemp, sizeof(BITMAP), &bm);
		int widthTemp = bm.bmWidth;
		int heightTemp = bm.bmHeight;
		hbmTempOld = (HBITMAP)SelectObject(hdcTemp, hbmTemp);

		SetBkMode(hdcTemp, TRANSPARENT);

		// create a BITMAPINFO with minimal initilisation

		// for the CreateDIBSection
		BITMAPINFO RGB32BitsBITMAPINFO;
		ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
		RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		RGB32BitsBITMAPINFO.bmiHeader.biWidth=width;
		RGB32BitsBITMAPINFO.bmiHeader.biHeight=height;
		RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
		RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;

		// pointer used for direct Bitmap pixels access
		hdcResult = CreateCompatibleDC(*hDC);

		BYTE *ptPixels;
		hbmResult = CreateDIBSection(hdcResult,
			(BITMAPINFO *)&RGB32BitsBITMAPINFO,
			DIB_RGB_COLORS,
			(void **)&ptPixels,
			NULL, 0);

		hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);
		SetBkMode(hdcResult, TRANSPARENT);

		// Resize hdc
		StretchBlt(hdcResult, 0, 0, width, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);


		// get color of lower left corner pixel
		COLORREF llc = *(COLORREF*)ptPixels;
		if (llc != RGBA(0, 0, 0, 0)) {
			for (int i = 0; i < 4 * width * height; i += 4)
			{
				// set all the pixels that have llc color as transparent
				if (*(COLORREF*)(ptPixels + i) == llc) {
					*(COLORREF*)(ptPixels + i) = RGBA(0, 0, 0, 0);
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
	//IImage* m_pImageThumb = NULL;
	IBitmapImage *m_pBitmap = NULL;

	// load it
	TIMER_START(loadImage_IImagingFactory_CreateImageFromFile_duration);
	if( SUCCEEDED(g_pImgFactory->CreateImageFromFile(pathImage, &m_pImage )) )
	{
		TIMER_STOP(loadImage_IImagingFactory_CreateImageFromFile_duration);
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
		ImageInfo ii;
		RECT rect = {0, 0, width, height};
		// Get image width/height
		m_pImage->GetImageInfo(&ii);

		lockedBitmapData.Width = width;
		lockedBitmapData.Height = height;
		lockedBitmapData.PixelFormat = ii.PixelFormat;

		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_32BPP_ARGB, &lockedBitmapData);
		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_16BPP_RGB565, &lockedBitmapData);
		m_pBitmap->LockBits(&rect, ImageLockModeRead, bitsPerPixel, &lockedBitmapData);

		if (bitsPerPixel == PIXFMT_32BPP_ARGB) {
			for (int y=0; y<height; y++)
			{
				BYTE * pPixel = (BYTE *) lockedBitmapData.Scan0 + width * 4 * y;

				for (int x=0; x<width; x++)
				{
					if (pPixel[3] < 25 || (pPixel[0] > 0xFA && pPixel[1] < 0x06 && pPixel[2] > 0xFA)) {
						pPixel[0] = 0;
						pPixel[1] = 0;
						pPixel[2] = 0;
						pPixel[3] = 0;
					} else if (pPixel[0] < 0x0A && pPixel[1] < 0x0A && pPixel[2] < 0x0A) {
						pPixel[1] = 0x0A;
						pPixel[2] = 0x0A;
						pPixel[3] = 0x0A;
					}
					pPixel += 4;
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
		/*TCHAR cad[32];
		swprintf(cad, L"Bits hbmResult pixel: %d\r\n", bm.bmBitsPixel);
		WriteToLog(cad);*/

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
	//if( m_pImageThumb != NULL ) m_pImageThumb->Release();
	if( m_pImage != NULL ) m_pImage->Release();
	// if( m_pImgFactory != NULL ) m_pImgFactory->Release();
	if( m_pBitmap != NULL ) m_pBitmap->Release();
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
	BITMAPINFO RGB32BitsBITMAPINFO;
	ZeroMemory(&RGB32BitsBITMAPINFO,sizeof(BITMAPINFO));
	RGB32BitsBITMAPINFO.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	RGB32BitsBITMAPINFO.bmiHeader.biWidth=width;
	RGB32BitsBITMAPINFO.bmiHeader.biHeight=height;
	RGB32BitsBITMAPINFO.bmiHeader.biPlanes=1;
	RGB32BitsBITMAPINFO.bmiHeader.biBitCount=32;

	// pointer used for direct Bitmap pixels access
	HDC hdcResult = CreateCompatibleDC(*hDC);

	BYTE *ptPixels;
	HBITMAP hbmResult = CreateDIBSection(hdcResult,
		(BITMAPINFO *)&RGB32BitsBITMAPINFO,
		DIB_RGB_COLORS,
		(void **)&ptPixels,
		NULL, 0);
	HBITMAP hbmResultOld;

	hbmResultOld = (HBITMAP)SelectObject(hdcResult, hbmResult);
	SetBkMode(hdcResult, TRANSPARENT);

	// Resize hdc
	StretchBlt(hdcResult, 0, 0, width, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);


	// get color of lower left corner pixel's color
	COLORREF llc = *(COLORREF*)ptPixels;
	if (llc != RGBA(0, 0, 0, 0)) {
		for (int i = 0; i < 4 * width * height; i += 4)
		{
			// set all the pixels that have llc color as transparent
			if (*(COLORREF*)(ptPixels + i) == llc) {
				*(COLORREF*)(ptPixels + i) = RGBA(0, 0, 0, 0);
			}
		}
	}
	/*
	for (int i = 0; i < 4 * width * height; i += 4)
	{
		if (ptPixels[i+3] > 230 || (ptPixels[i] > 0xFA && ptPixels[i+1] < 0x06 && ptPixels[i+2] > 0xFA)) {
			// ptPixels[i] = 0xFF;
			// ptPixels[i+1] = 0x00;
			// ptPixels[i+2] = 0xFF;
			// ptPixels[i+3] = 0xFF;

			ptPixels[i] = 0x00;
			ptPixels[i+1] = 0x00;
			ptPixels[i+2] = 0x00;
			ptPixels[i+3] = 0x00;
		}
	}
	*/


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

HBITMAP LoadImageWithImgdecmp(HDC hDC, LPTSTR strFileName)
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
   dii.iBitDepth = GetDeviceCaps(hDC, BITSPIXEL);
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

