#pragma once

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
	BOOL hayLanzamiento;
	COLORREF colorFondo;
	// CIcono *fondoPantalla;
	CIcono *iconoActivo;

	RECT cuadroLanzando; // Posicion Objetivo en una transicion de lanzamiento
	UINT estadoCuadro; // 0=Desactivado, 1=Lanzando, 2=Abierto
	LONG timeUltimoLanzamiento;
	BOOL debeCortarTimeOut;
	int estadoPantalla; // 0 = Normal; 1 = Landscape
	int semaforoRotacion; // 0 = Normal; 1 = Rotando

	// Notificaciones
	int numLlamadas;
	int numSMS;
	int numMMS;
	int numOtherEmail;
	int numSyncEmail;
	int numCitas;
	int numTareas;
	int estadoWifi;
	int estadoBluetooth;
	int estadoAlarm;

	int diaDelMes;
	int hora;

	int reloadIcons;
	int reloadIcon;
};
