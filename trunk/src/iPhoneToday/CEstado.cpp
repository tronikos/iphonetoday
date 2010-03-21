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
	estadoWifi = FALSE;
	estadoBluetooth = 0;
	estadoGPRS = FALSE;
	estadoAlarm = FALSE;

	signalStrength = 0;
	externalPowered = FALSE;
	batteryPercentage = 0;
	volumePercentage = 0;
	memoryLoad = 0;
	memoryFree = 0;
	memoryUsed = 0;

	reloadIcons = 0;
	reloadIcon = 0;

	operatorName[0] = 0;
}

CEstado::~CEstado(void)
{
}

BOOL CEstado::actualizaNotificaciones() {

	BOOL hayCambios = FALSE;
	DWORD valorRegistro = 0;
	int newValue;
	this->changedBubble = FALSE;
	this->changedState = FALSE;
	this->changedSignal = FALSE;
	this->changedTime = FALSE;
	this->changedBattery = FALSE;
	this->changedVolume = FALSE;
	this->changedMemory = FALSE;

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Phone"),
		TEXT("Missed Call Count"), 
		0);

	if (numLlamadas != valorRegistro) {
		numLlamadas = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\sms\\Unread"),
		TEXT("Count"), 
		0);

	if (numSMS != valorRegistro) {
		numSMS = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\MMS\\Unread"),
		TEXT("Count"), 
		0);

	if (numMMS != valorRegistro) {
		numMMS = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\OtherEmail\\Unread"),
		TEXT("Count"), 
		0);

	if (numOtherEmail != valorRegistro) {
		numOtherEmail = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Messages\\sync\\Unread"),
		TEXT("Count"), 
		0);

	if (numSyncEmail != valorRegistro) {
		numSyncEmail = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Appointments\\List"),
		TEXT("Count"), 
		0);

	if (numCitas != valorRegistro) {
		numCitas = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_CURRENT_USER, &valorRegistro, 
		TEXT("System\\State\\Tasks"),
		TEXT("Active"), 
		0);

	if (numTareas != valorRegistro) {
		numTareas = valorRegistro;
		hayCambios = TRUE;
		changedBubble = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Hardware"),
		TEXT("Wifi"), 
		0);

	if (valorRegistro == 5 || valorRegistro == 1) {
		valorRegistro = 0;
	}

	if (estadoWifi != (valorRegistro > 0)) {
		estadoWifi = (valorRegistro > 0);
		hayCambios = TRUE;
		changedState = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Hardware"),
		TEXT("Bluetooth"), 
		0);

	if (valorRegistro == 8) {
		valorRegistro = 0;
	}

	if (estadoBluetooth != valorRegistro) {
		estadoBluetooth = valorRegistro;
		hayCambios = TRUE;
		changedState = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Connections\\Cellular"),
		TEXT("Count"), 
		0);

	if (estadoGPRS != (valorRegistro > 0)) {
		estadoGPRS = (valorRegistro > 0);
		hayCambios = TRUE;
		changedState = TRUE;
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

	if (estadoAlarm != (valorRegistro > 0)) {
		estadoAlarm = (valorRegistro > 0);
		hayCambios = TRUE;
		changedState = TRUE;
	}

	LoadDwordSetting(HKEY_LOCAL_MACHINE, &valorRegistro, 
		TEXT("System\\State\\Phone"),
		TEXT("Signal Strength"), 
		0);

	if (signalStrength != valorRegistro) {
		signalStrength = valorRegistro;
		hayCambios = TRUE;
		changedSignal = TRUE;
	}

	LoadTextSetting(HKEY_LOCAL_MACHINE, operatorName,
		TEXT("System\\State\\Phone"),
		TEXT("Current Operator Name"), 
		TEXT(""));

	SYSTEMTIME st_new;
	GetLocalTime(&st_new);

	if (st.wMinute != st_new.wMinute ||
		st.wHour != st_new.wHour ||
		st.wDay != st_new.wDay) {
			memcpy(&st, &st_new, sizeof(SYSTEMTIME));
			hayCambios = TRUE;
			changedTime = TRUE;
	}

	newValue = ExternalPowered();
	if (externalPowered != newValue) {
		externalPowered = newValue;
		hayCambios = TRUE;
		changedBattery = TRUE;
	}

	newValue = BatteryPercentage();
	if (batteryPercentage != newValue) {
		batteryPercentage = newValue;
		hayCambios = TRUE;
		changedBattery = TRUE;
	}

	newValue = GetVolumePercentage();
	if (volumePercentage != newValue) {
		volumePercentage = newValue;
		hayCambios = TRUE;
		changedVolume = TRUE;
	}

	newValue = MemoryLoad();
	if (memoryLoad != newValue) {
		memoryLoad = newValue;
		hayCambios = TRUE;
		changedMemory = TRUE;
	}

	newValue = MemoryFree();
	if (abs(memoryFree - newValue) > 100 * 1024) {
		memoryFree = newValue;
		hayCambios = TRUE;
		changedMemory = TRUE;
	}

	newValue = MemoryUsed();
	if (abs(memoryUsed - newValue) > 100 * 1024) {
		memoryUsed = newValue;
		hayCambios = TRUE;
		changedMemory = TRUE;
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

	if (reloadIcons != valorRegistro) {
		reloadIcons = valorRegistro;
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

	if (reloadIcon != valorRegistro) {
		reloadIcon = valorRegistro;
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
