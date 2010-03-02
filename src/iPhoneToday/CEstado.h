#pragma once

#include "CReloadIcon.h"
#include "CIcono.h"

class CEstado
{
public:
	CEstado(void);
	~CEstado(void);

	BOOL actualizaNotificaciones();
	BOOL checkReloadIcons();
	BOOL clearReloadIcons();
	BOOL checkReloadIcon();
	BOOL clearReloadIcon();
	BOOL LoadRegistryIcon(int nIcon, CReloadIcon *reloadIcon);

	// Variables 
	POINTS posObjetivo; // Posicion Objetivo en una transicion
	UINT pantallaActiva;
	BOOL hayMovimiento;
	CIcono *iconoActivo;

	RECT cuadroLanzando;	// Posicion Objetivo en una transicion de lanzamiento
	UINT estadoCuadro;		// 0=Desactivado, 1=Lanzando, 2=Abierto
	LONG timeUltimoLanzamiento;
	BOOL debeCortarTimeOut;

	// Notificaciones

	// Number inside a bubble
	int numLlamadas;	// HKCU\System\State\Phone\Missed Call Count
	int numSMS;			// HKCU\System\State\Messages\sms\Unread\Count
	int numMMS;			// HKCU\System\State\Messages\MMS\Unread\Count
	int numOtherEmail;	// HKCU\System\State\Messages\OtherEmail\Unread\Count
	int numSyncEmail;	// HKCU\System\State\Messages\sync\Unread\Count
	int numCitas;		// HKCU\System\State\Appointments\List\Count
	int numTareas;		// HKCU\System\State\Tasks\Active

	// State on/off
	BOOL estadoWifi;	// HKLM\System\State\Hardware\WiFi
	int estadoBluetooth;// HKLM\System\State\Hardware\Bluetooth
	BOOL estadoAlarm;	// HKLM\Software\Microsoft\Clock\0\AlarmFlags HKLM\Software\Microsoft\Clock\1\AlarmFlags HKLM\Software\Microsoft\Clock\2\AlarmFlags
	BOOL estadoGPRS;	// HKLM\\System\State\Connections\Cellular\Count

	// Number inside icon
	int signalStrength;	// HKLM\System\State\Phone\Signal Strength
	SYSTEMTIME st;
	BOOL externalPowered;
	int batteryPercentage;
	int volumePercentage;
	int memoryLoad;
	int memoryFree;
	int memoryUsed;

	// Text under icon
	TCHAR operatorName[MAX_PATH];	// HKLM\System\State\Phone\Current Operator Name

	int reloadIcons;	// HKLM\Software\iPhoneToday\reloadIcons
	int reloadIcon;		// HKLM\Software\iPhoneToday\reloadIcon
};
