#pragma once

#include "stdafx.h"
#include "regext.h"
#include "snapi.h"

// Window Message for notifications of changes
#define WM_REGISTRY				WM_USER + 100


enum dw_notifications_enum {
	SN_PHONEMISSEDCALLS,				// HKCU\System\State\Phone\Missed Call Count
	SN_MESSAGINGVOICEMAILTOTALUNREAD,	// HKCU\System\State\Messages\VMail\Total\Unread\Count
	SN_MESSAGINGSMSUNREAD,				// HKCU\System\State\Messages\SMS\Unread\Count
	SN_MESSAGINGMMSUNREAD,				// HKCU\System\State\Messages\MMS\Unread\Count
	SN_MESSAGINGOTHEREMAILUNREAD,		// HKCU\System\State\Messages\OtherEmail\Unread\Count
	SN_MESSAGINGACTIVESYNCEMAILUNREAD,	// HKCU\System\State\Messages\Sync\Unread\Count
	SN_APPOINTMENTSLISTCOUINT,			// HKCU\System\State\Appointments\List\Count
	SN_TASKSACTIVE,						// HKCU\System\State\Tasks\Active
	SN_WIFISTATEPOWERON,				// HKLM\System\State\Hardware\WiFi
	SN_BLUETOOTHSTATEPOWERON,			// HKLM\System\State\Hardware\Bluetooth
	SN_CELLSYSTEMCONNECTED,				// HKLM\System\State\Phone\Cellular System Connected
	SN_PHONESIGNALSTRENGTH,				// HKLM\System\State\Phone\Signal Strength
	SN_POWERBATTERYSTATE,				// HKLM\System\State\Battery\Main
	SN_VOLUME,							// HKCU\ControlPanel\Volume\Volume
	SN_CLOCKALARMFLAGS0,				// HKLM\Software\Microsoft\Clock\0\AlarmFlags
	SN_CLOCKALARMFLAGS1,				// HKLM\Software\Microsoft\Clock\1\AlarmFlags
	SN_CLOCKALARMFLAGS2,				// HKLM\Software\Microsoft\Clock\2\AlarmFlags
	SN_IRDA,							// HKLM\Software\Microsoft\Obex\IsEnabled
	SN_RELOADICON,						// HKLM\Software\iPhoneToday\reloadIcon
	SN_RELOADICONS,						// HKLM\Software\iPhoneToday\reloadIcons
	MAXDWORDNOTIFICATION
};

enum sz_notifications_enum {
	SN_PHONEOPERATORNAME,				// HKLM\System\State\Phone\Current Operator Name
	SN_PHONEPROFILE,					// HKCU\ControlPanel\Profiles\ActiveProfile
	MAXSTRINGNOTIFICATION
};

enum ft_notifications_enum {
	SN_TIME,							// HKLM\System\State\DateTime\Time
	SN_DATE,							// HKLM\System\State\DateTime\Date
	MAXFILETIMENOTIFICATION
};

class CNotifications
{
public:
	CNotifications(HWND hWnd);
	~CNotifications();

	void ResetChanged();
	void InitRegistryNotifications(HWND hWnd);
	LRESULT Callback(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
	BOOL PollingUpdate();

	HREGNOTIFY dwHrNotify[MAXDWORDNOTIFICATION];
	DWORD dwNotifications[MAXDWORDNOTIFICATION];
	BOOL dwNotificationsChanged[MAXDWORDNOTIFICATION];

	HREGNOTIFY szHrNotify[MAXDWORDNOTIFICATION];
	TCHAR szNotifications[MAXDWORDNOTIFICATION][MAX_PATH];
	BOOL szNotificationsChanged[MAXDWORDNOTIFICATION];

	HREGNOTIFY ftHrNotify[MAXDWORDNOTIFICATION];
	BOOL ftNotificationsChanged[MAXDWORDNOTIFICATION];

	SYSTEMTIME st;

	MEMORYSTATUS memoryStatus;
	BOOL memory_changed;
};
