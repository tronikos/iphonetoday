#include "stdafx.h"
#include "CEstado.h"
#include "RegistryUtils.h"

CEstado::CEstado(void)
{
	hayMovimiento = false;

	pantallaActiva = 0;

	numLlamadas = 0;
	numSMS = 0;
	numMMS = 0;
	numOtherEmail = 0;
	numSyncEmail = 0;
	numCitas = 0;
	numTareas = 0;
	estadoWifi = 0;
	estadoBluetooth = 0;
	estadoAlarm = 0;

	externalPowered = 0;
	batteryPercentage = 0;
	volumePercentage = 0;

	reloadIcons = 0;
	reloadIcon = 0;
}

CEstado::~CEstado(void)
{
}

BOOL CEstado::actualizaNotificaciones() {

	BOOL hayCambios = FALSE;
	int temp;

	DWORD valorRegistro = 0;
	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Phone"),
		TEXT("Missed Call Count"), 
		0);

	if (numLlamadas != int(valorRegistro)) {
		numLlamadas = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\sms\\Unread"),
		TEXT("Count"), 
		0);

	if (numSMS != int(valorRegistro)) {
		numSMS = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\MMS\\Unread"),
		TEXT("Count"), 
		0);

	if (numMMS != int(valorRegistro)) {
		numMMS = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\OtherEmail\\Unread"),
		TEXT("Count"), 
		0);

	if (numOtherEmail != int(valorRegistro)) {
		numOtherEmail = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\sync\\Unread"),
		TEXT("Count"), 
		0);

	if (numSyncEmail != int(valorRegistro)) {
		numSyncEmail = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Appointments\\List"),
		TEXT("Count"), 
		0);

	if (numCitas != int(valorRegistro)) {
		numCitas = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Tasks"),
		TEXT("Active"), 
		0);

	if (numTareas != int(valorRegistro)) {
		numTareas = int(valorRegistro);
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Hardware"),
		TEXT("Wifi"), 
		-1);
	temp = int(valorRegistro);
	if (temp == 5 || temp == 1 || temp == -1) {
		temp = 0;
	} else {
		temp = 1;
	}

	if (estadoWifi != temp) {
		estadoWifi = temp;
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Hardware"),
		TEXT("Bluetooth"), 
		-1);
	temp = int(valorRegistro);
	if (temp == 8 || temp == -1 || temp == 0) {
		temp = 0;
	} else if (temp == 11) {
		temp = 2;
	} else {
		temp = 1;
	}

	if (estadoBluetooth != temp) {
		estadoBluetooth = temp;
		hayCambios = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("Software\\Microsoft\\Clock\\0"),
		TEXT("AlarmFlags"), 
		0);

	if (valorRegistro == 0) {
		LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("Software\\Microsoft\\Clock\\1"),
		TEXT("AlarmFlags"), 
		0);
	}

	if (valorRegistro == 0) {
		LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("Software\\Microsoft\\Clock\\2"),
		TEXT("AlarmFlags"), 
		0);
	}

	if (estadoAlarm != int(valorRegistro)) {
		estadoAlarm = int(valorRegistro);
		hayCambios = TRUE;
	}

	SYSTEMTIME st_new;
	GetLocalTime(&st_new);

	if (st.wMinute != st_new.wMinute ||
		st.wHour != st_new.wHour ||
		st.wDay != st_new.wDay) {
			memcpy(&st, &st_new, sizeof(SYSTEMTIME));
			hayCambios = TRUE;
	}

	int newValue = ExternalPowered();
	if (externalPowered != newValue) {
		externalPowered = newValue;
		hayCambios = TRUE;
	}

	newValue = BatteryPercentage();
	if (batteryPercentage != newValue) {
		batteryPercentage = newValue;
		hayCambios = TRUE;
	}

	newValue = GetVolumePercentage();
	if (volumePercentage != newValue) {
		volumePercentage = newValue;
		hayCambios = TRUE;
	}

	return hayCambios;
}

BOOL CEstado::checkReloadIcons() {

	BOOL hayCambios = FALSE;
	DWORD valorRegistro = 0;

	// Comprobamos si hay que actualizar los iconos
	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("Software\\iPhoneToday"),
		TEXT("reloadIcons"), 
		0);

	if (reloadIcons != int(valorRegistro)) {
		reloadIcons = int(valorRegistro);
		hayCambios = TRUE;
	}

	return hayCambios;
}

BOOL CEstado::clearReloadIcons() {

	BOOL result = FALSE;
	DWORD valorRegistro = 0;

	// Comprobamos si hay que actualizar los iconos
	SaveDwordSetting(HKEY_LOCAL_MACHINE, TEXT("Software\\iPhoneToday"),
		&valorRegistro, TEXT("reloadIcons"));

	result = TRUE;
	return result;
}

BOOL CEstado::checkReloadIcon() {

	BOOL hayCambios = FALSE;
	DWORD valorRegistro = 0;

	// Comprobamos si hay que actualizar los iconos
	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("Software\\iPhoneToday"),
		TEXT("reloadIcon"), 
		0);

	if (reloadIcon != int(valorRegistro)) {
		reloadIcon = int(valorRegistro);
		hayCambios = TRUE;
	}

	return hayCambios;
}

BOOL CEstado::clearReloadIcon() {

	BOOL result = FALSE;
	DWORD valorRegistro = 0;
	TCHAR subKey[MAX_PATH];

	// Comprobamos si hay que actualizar los iconos
	SaveDwordSetting(HKEY_LOCAL_MACHINE, TEXT("Software\\iPhoneToday"),
		&valorRegistro, TEXT("reloadIcon"));

	int nIcon = 0;
	BOOL next = true;
	while (next) {
		swprintf(subKey, L"Software\\iPhoneToday\\Icon%d", nIcon);
		next = DeleteKey(HKEY_LOCAL_MACHINE, subKey);
		nIcon++;
	}

	result = TRUE;
	return result;
}

BOOL CEstado::LoadRegistryIcon(int nIcon, CReloadIcon *reloadIcon)
{
	TCHAR subKey[MAX_PATH];

	reloadIcon->ClearObject();

	swprintf(subKey, L"Software\\iPhoneToday\\Icon%d", nIcon);

	if (!LoadDwordSetting(HKEY_LOCAL_MACHINE, &reloadIcon->nScreen, subKey, TEXT("nScreen"), 0))
		return FALSE;
	if (!LoadDwordSetting(HKEY_LOCAL_MACHINE, &reloadIcon->nIcon, subKey, TEXT("nIcon"), 0))
		return FALSE;
	LoadTextSetting(HKEY_LOCAL_MACHINE, reloadIcon->strName, subKey, TEXT("strName"), L"");
	LoadTextSetting(HKEY_LOCAL_MACHINE, reloadIcon->strImage, subKey, TEXT("strImage"), L"");

	return TRUE;
}
