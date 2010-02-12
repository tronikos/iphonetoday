#include "vibrate.h"

void LedOn(int id)
{
	NLED_SETTINGS_INFO settings;
	settings.LedNum= id;
	settings.OffOnBlink= 1;
	NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
}
void LedOff(int id)
{
	NLED_SETTINGS_INFO settings;
	settings.LedNum= id;
	settings.OffOnBlink= 0;
	NLedSetDevice(NLED_SETTINGS_INFO_ID, &settings);
}
void vibrate(int offMsec, int id)
{
	LedOn(id);
	Sleep(offMsec);
	LedOff(id);
}

int WINAPI WinMain2( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	for (int i=0 ; i<10 ; i++)
	{
		LedOn(1);
		Sleep(400);
		LedOff(1);
		Sleep(200);
	}

	return 0;
}