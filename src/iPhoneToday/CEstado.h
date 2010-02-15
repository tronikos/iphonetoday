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

	BOOL actualizaDateTime();

	// Variables 
	POINTS posObjetivo; // Posicion Objetivo en una transicion
	UINT pantallaActiva;
	BOOL hayMovimiento;
	COLORREF colorFondo;
	CIcono *iconoActivo;

	RECT cuadroLanzando;	// Posicion Objetivo en una transicion de lanzamiento
	UINT estadoCuadro;		// 0=Desactivado, 1=Lanzando, 2=Abierto
	LONG timeUltimoLanzamiento;
	BOOL debeCortarTimeOut;

	// Notificaciones
	int numLlamadas;	// HKCU\System\State\Phone\Missed Call Count
	int numSMS;			// HKCU\System\State\Messages\sms\Unread\Count
	int numMMS;			// HKCU\System\State\Messages\MMS\Unread\Count
	int numOtherEmail;	// HKCU\System\State\Messages\OtherEmail\Unread\Count
	int numSyncEmail;	// HKCU\System\State\Messages\sync\Unread\Count
	int numCitas;		// HKCU\System\State\Appointments\List\Count
	int numTareas;		// HKCU\System\State\Tasks\Active
	int estadoWifi;		// HKLM\System\State\Hardware\Wifi\WiFi
	int estadoBluetooth;// HKLM\System\State\Hardware\Wifi\Bluetooth
	int estadoAlarm;	// HKLM\Software\Microsoft\Clock\0\AlarmFlags HKLM\Software\Microsoft\Clock\1\AlarmFlags HKLM\Software\Microsoft\Clock\2\AlarmFlags

	int diaDelMes;		// day of the month
	int hora;			// hours+mins

	int reloadIcons;	// HKLM\Software\iPhoneToday\reloadIcons
	int reloadIcon;		// HKLM\Software\iPhoneToday\reloadIcon
};
