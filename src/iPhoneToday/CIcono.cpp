#include "CIcono.h"
#include "iPhoneToday.h"

#ifdef TIMING
long loadImage_duration = 0;
long loadImage_IImagingFactory_CreateImageFromFile_duration = 0;
long loadImage_IImagingFactory_CreateBitmapFromImage_duration = 0;
#endif

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

void CIcono::loadImage(HDC *hDC, TCHAR *pathImage, int witdh, int height, int bitsPerPixel) {
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

	// If imaging API is not available or it is a .bmp use SHLoadDIBitmap that only supports .bmp images and is faster.
	if (g_pImgFactory == NULL || lstrcmpi(wcsrchr(pathImage, '.'), L".bmp") == 0) {

		HDC hdcTemp;
		HBITMAP hbmTemp;
		HBITMAP hbmTempOld;

		HDC hdcTempNULL = GetDC(NULL);
		hdcTemp = CreateCompatibleDC(hdcTempNULL);
		hbmTemp = SHLoadDIBitmap(pathImage);
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
		RGB32BitsBITMAPINFO.bmiHeader.biWidth=witdh;
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
		StretchBlt(hdcResult, 0, 0, witdh, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);



		DeleteObject(SelectObject(hdcTemp, hbmTempOld));
		DeleteDC(hdcTemp);
		ReleaseDC(NULL, hdcTempNULL);

		this->hDC = hdcResult;
		this->imagen = hbmResult;
		this->imagenOld = hbmResultOld;
		this->anchoImagen = witdh;
		this->altoImagen = height;

		TIMER_STOP(loadImage_duration);
		return;
	}

	IImage* m_pImage = NULL;
	IImage* m_pImageThumb = NULL;
	IBitmapImage *m_pBitmap = NULL;

	// load it
	TIMER_START(loadImage_IImagingFactory_CreateImageFromFile_duration);
	if( SUCCEEDED(g_pImgFactory->CreateImageFromFile(pathImage, &m_pImage )) )
	{
		TIMER_STOP(loadImage_IImagingFactory_CreateImageFromFile_duration);
		HRESULT hRet;
		TIMER_START(loadImage_IImagingFactory_CreateBitmapFromImage_duration);
		// hRet = m_pImgFactory->CreateBitmapFromImage(m_pImage, witdh, height, PIXFMT_16BPP_RGB565, InterpolationHintDefault, &m_pBitmap);
		// hRet = m_pImgFactory->CreateBitmapFromImage(m_pImage, witdh, height, PIXFMT_16BPP_ARGB1555, InterpolationHintDefault, &m_pBitmap);
		hRet = g_pImgFactory->CreateBitmapFromImage(m_pImage, witdh, height, bitsPerPixel, InterpolationHintDefault, &m_pBitmap);
		TIMER_STOP(loadImage_IImagingFactory_CreateBitmapFromImage_duration);
		if (hRet != S_OK) {
			return;
		}

		BitmapData  lockedBitmapData;
		ImageInfo ii;
		RECT rect = {0, 0, witdh, height};
		// Get image width/height
		m_pImage->GetImageInfo(&ii);

		lockedBitmapData.Width = witdh;
		lockedBitmapData.Height = height;
		lockedBitmapData.PixelFormat = ii.PixelFormat;

		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_32BPP_ARGB, &lockedBitmapData);
		// m_pBitmap->LockBits(&rect, ImageLockModeRead, PIXFMT_16BPP_RGB565, &lockedBitmapData);
		m_pBitmap->LockBits(&rect, ImageLockModeRead, bitsPerPixel, &lockedBitmapData);

		if (bitsPerPixel == PIXFMT_32BPP_ARGB) {
			for (int y=0; y<height; y++)
			{
				BYTE * pPixel = (BYTE *) lockedBitmapData.Scan0 + witdh * 4 * y;

				for (int x=0; x<witdh; x++)
				{
					if (y == 34 && x == 18) {
						x = x;
					}
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
		this->anchoImagen = witdh;
		this->altoImagen = height;
	} else {
		//err(( TEXT("CreateImageFromFile failed!") ));
		return;
	}

	// release
	if( m_pImageThumb != NULL ) m_pImageThumb->Release(); 
	if( m_pImage != NULL ) m_pImage->Release(); 
	// if( m_pImgFactory != NULL ) m_pImgFactory->Release();
	if( m_pBitmap != NULL ) m_pBitmap->Release();
	// CoUninitialize();

	TIMER_STOP(loadImage_duration);

	return;
}

void CIcono::loadImageFromExec(HDC *hDC, TCHAR *pathExec, int witdh, int height) {

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
	RGB32BitsBITMAPINFO.bmiHeader.biWidth=witdh;
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
	StretchBlt(hdcResult, 0, 0, witdh, height, hdcTemp, 0, 0, widthTemp, heightTemp, SRCCOPY);

	// TCHAR cad[32];
	// WriteToLog(TEXT("CARGANDO...\n\r"));
	for (int i = 0; i < witdh * height; i += 4)
	{
		// swprintf(cad, TEXT("%x.%x.%x.%x-"), ptPixels[i],ptPixels[i+1],ptPixels[i+2],ptPixels[i+3]);
		// WriteToLog(cad);
		
		// ptPixels[i]=cNewColor;

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
		// swprintf(cad, TEXT("%x.%x.%x.%x\r\n"), ptPixels[i],ptPixels[i+1],ptPixels[i+2],ptPixels[i+3]);
		// WriteToLog(cad);
	}
	// WriteToLog(TEXT("FIN CARGA...\n\r"));


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
	this->anchoImagen = witdh;
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
