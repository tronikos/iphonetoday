#include <windows.h>
#include <winioctl.h>
#include <NtDDNdis.h>
#include <nuiouser.h>

#include "Wifi.h"

int GetWifiSignalStrength(TCHAR *ptcDeviceName)
{
	int signal = 0;

	if (ptcDeviceName == NULL || ptcDeviceName[0] == 0) {
		return signal;
	}

	// Attach to NDISUIO
	HANDLE hNdisUio = CreateFile(NDISUIO_DEVICE_NAME,
								0,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
								INVALID_HANDLE_VALUE );

	if (hNdisUio == INVALID_HANDLE_VALUE) return signal;

	// Get Signal strength
	UCHAR QueryBuffer[sizeof(NDISUIO_QUERY_OID) + sizeof(DWORD)];
	ZeroMemory(QueryBuffer, sizeof(QueryBuffer));
	PNDISUIO_QUERY_OID pQueryOid = (PNDISUIO_QUERY_OID)&QueryBuffer[0];
	pQueryOid->ptcDeviceName = ptcDeviceName;
	pQueryOid->Oid = OID_802_11_RSSI;
	DWORD dwBytesReturned = 0;
	if (DeviceIoControl(hNdisUio,
						IOCTL_NDISUIO_QUERY_OID_VALUE,
						(LPVOID) pQueryOid,
						sizeof(NDISUIO_QUERY_OID) + sizeof(DWORD),
						(LPVOID) pQueryOid,
						sizeof(NDISUIO_QUERY_OID) + sizeof(DWORD),
						&dwBytesReturned,
						NULL)) {
		signal = *(DWORD *)&pQueryOid->Data;
	}

	CloseHandle(hNdisUio);

	return signal;
}

LPWSTR GetWirelessDeviceName()
{
	static TCHAR szWirelessDeviceName[100] = L"";
	static int flag = -1; // -1: re-check, 0: no wireless adapter, 1: wireless adapter found
	if (flag == -1) {
		HANDLE hFile = CreateFile(L"NDS0:",
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								INVALID_HANDLE_VALUE);
		if (hFile != INVALID_HANDLE_VALUE) {
			BYTE Buffer[2048];
			DWORD dwRet = sizeof(Buffer);
			if (DeviceIoControl(hFile,
								IOCTL_NDIS_GET_ADAPTER_NAMES,
								NULL, 0,
								Buffer, sizeof(Buffer),
								&dwRet,
								NULL)) {
				int count_unknown = 0;
				for (LPWSTR pszStr = (LPWSTR) Buffer; *pszStr; pszStr += wcslen(pszStr) + 1) {
					//NKDbgPrintfW(L"Adapter: %s\n", pszStr);
					// skip adapter names that are known not to be wireless (skip infrared, gprs, ActiveSync etc.)
					if (wcsicmp(pszStr, L"ASYNCMAC1") == 0 ||
						wcsicmp(pszStr, L"IRSIR1") == 0 ||
						wcsicmp(pszStr, L"L2TP1") == 0 ||
						wcsicmp(pszStr, L"PPTP1") == 0 ||
						wcsicmp(pszStr, L"RNDISFN1") == 0 ||
						wcsicmp(pszStr, L"WWAN1") == 0 ||
						wcsicmp(pszStr, L"XSC1_IRDA1") == 0) {
							continue;
					}
					if (GetWifiSignalStrength(pszStr) != 0) {
						// if we can get a signal strength, it's a wireless card
						// (a better method might be to call WZCQueryInterface of wzcsapi.dll)
						flag = 1;
						wcscpy(szWirelessDeviceName, pszStr);
						break;
					} else {
						// however, due to errors this strength might be zero,
						// so we can't be sure that it's a non wireless card...
						count_unknown++;
					}
				}
				if (flag == -1 && count_unknown == 0) {
					flag = 0;
				}
			}
		}
	}

	return szWirelessDeviceName;
}

int GetWifiSignalStrength()
{
	return GetWifiSignalStrength(GetWirelessDeviceName());
}

int GetWifiSignalStrengthLevel(int nSignalStrength)
{
	// No signal
	if (nSignalStrength == 0 || nSignalStrength < -90)
		return 0;
	// Very low
	else if (nSignalStrength < -81)
		return 1;
	// Low
	else if (nSignalStrength < -71)
		return 2;
	// Good
	else if (nSignalStrength < -67)
		return 3;
	// Very good
	else if (nSignalStrength < -57)
		return 4;
	// Excellent
	else
		return 5;
}
