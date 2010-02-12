#include <windows.h>
#include <nled.h>

// from the platform builder <Pwinuser.h>
extern "C" {
	BOOL WINAPI NLedGetDeviceInfo( UINT nInfoId, void *pOutput );
	BOOL WINAPI NLedSetDevice( UINT nDeviceId, void *pInput );
};

void LedOn(int id);
void LedOff(int id);
void vibrate(int offMsec, int id = 1);
int WINAPI WinMain2( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow);