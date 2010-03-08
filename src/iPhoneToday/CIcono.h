#pragma once

#include <windows.h>
#include <imaging.h>

//#define TIMING

#ifdef TIMING

#define TIMER_START(t) (t -= GetTickCount())
#define TIMER_STOP(t) (t += GetTickCount())
#define TIMER_RESET(t) (t = 0)

extern long loadImage_duration;
extern long loadImage_IImagingFactory_CreateImageFromFile_duration;
extern long loadImage_IImagingFactory_CreateBitmapFromImage_duration;

#else

#define TIMER_START(t)
#define TIMER_STOP(t)
#define TIMER_RESET(t)

#endif

class CIcono
{
public:
	FLOAT x;
	FLOAT y;
	TCHAR nombre[MAX_PATH];			// name
	TCHAR rutaImagen[MAX_PATH];		// image
	TCHAR sound[MAX_PATH];			// sound
	TCHAR ejecutable[MAX_PATH];		// exec
	TCHAR parametros[MAX_PATH];		// parameters
	TCHAR ejecutableAlt[MAX_PATH];	// execAlt
	TCHAR parametrosAlt[MAX_PATH];	// parametersAlt
	UINT tipo;						// type
	UINT launchAnimation;			// animation

	HBITMAP imagen;
	HBITMAP imagenOld;
	HDC hDC;

	UINT anchoImagen;				// width
	UINT altoImagen;				// height

	CIcono(void);
	~CIcono(void);

	void defaultValues();
	void loadImage(HDC *hDC, TCHAR *pathImage, int width = 0, int height = 0, int bitsPerPixel = PIXFMT_32BPP_ARGB, float factor = 0, BOOL alwaysPremultiply = FALSE);
	void loadImageFromExec(HDC *hDC, TCHAR *pathExec, int width = 0, int height = 0);
	void clearImageObjects();
};
