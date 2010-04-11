// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// NOTE - this value is not strongly correlated to the Windows CE OS version being targeted
#define WINVER _WIN32_WCE

#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

#include <windows.h>
#include <commctrl.h>

#ifdef SHELL_AYGSHELL
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#endif // SHELL_AYGSHELL


// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_	
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
#endif

#include <altcecrt.h>

#ifndef EXEC_MODE
#include <todaycmn.h>
#endif

#include <commdlg.h>

#if _WIN32_WCE < 0x500
#include "my_aygshell.h"
#include "my_dlls.h"
#endif

#include "resource.h"
#include "Utils.h"
#include "Utils2.h"
#include "Log.h"
#include "GraphicFunctions.h"
#include "CReloadIcon.h"

#define CountOf(x) (sizeof(x)/sizeof(*x))

typedef struct
{
    int nPantallaActual;
	int nIconoActual;
} IDENT_ICONO;

typedef enum
{
    MAINSCREEN  = 0,
    BOTTOMBAR,
	TOPBAR
} SCREEN_TYPE;

#define MAX_PANTALLAS			32
#define MAX_ICONOS_PANTALLA		64

#define NUM_CONFIG_SCREENS		11

#define TIMER_RECUPERACION		10000
#define TIMER_ACTUALIZA_NOTIF	10001
#define TIMER_LANZANDO_APP		10002
#define TIMER_LOADING			10003
#define TIMER_LONGTAP			10004

#define MENU_POPUP_ADD			100
#define MENU_POPUP_EDIT			101
#define MENU_POPUP_DELETE		102
#define MENU_POPUP_CANCEL		103
#define MENU_POPUP_MOVE			104
#define MENU_POPUP_MOVE_HERE	105
#define MENU_POPUP_COPY			106
#define MENU_POPUP_COPY_HERE	107
#define MENU_POPUP_OPTIONS		108
#define MENU_POPUP_EXIT			109
#define MENU_POPUP_EDIT_HEADER	110

#define NOTIF_NORMAL		0
#define NOTIF_MISSEDCALLS	1
#define NOTIF_SMS			2
#define NOTIF_MMS			3
#define NOTIF_OTHER_EMAIL	4
#define NOTIF_SYNC_EMAIL	41
#define NOTIF_TOTAL_EMAIL	42
#define NOTIF_APPOINTS		5
#define NOTIF_TASKS			6
#define NOTIF_CALENDAR		7
#define NOTIF_SMS_MMS		8
#define NOTIF_WIFI			9
#define NOTIF_BLUETOOTH		10
#define NOTIF_ALARM			11
#define NOTIF_CLOCK			12
#define NOTIF_CLOCK_ALARM	13
#define NOTIF_BATTERY		14
#define NOTIF_VOLUME		15
#define NOTIF_MEMORYLOAD	16
#define NOTIF_MEMORYFREE	17
#define NOTIF_MEMORYUSED	18
#define NOTIF_CELLNETWORK	19
#define NOTIF_SIGNAL		20
#define NOTIF_OPERATOR		21
#define NOTIF_SIGNAL_OPER	22
#define NOTIF_MC_SIG_OPER	23
#define NOTIF_PROFILE		24
#define NOTIF_VMAIL			25
#define NOTIF_IRDA			26
#define NOTIF_CRADLE		27

#define NOTIF_NORMAL_TXT		TEXT("Normal")
#define NOTIF_MISSEDCALLS_TXT	TEXT("Missed Calls")
#define NOTIF_SMS_TXT			TEXT("Unread SMS")
#define NOTIF_MMS_TXT			TEXT("Unread MMS")
#define NOTIF_OTHER_EMAIL_TXT	TEXT("Unread Others EMail")
#define NOTIF_SYNC_EMAIL_TXT	TEXT("Unread Sync EMail")
#define NOTIF_TOTAL_EMAIL_TXT	TEXT("Unread Total EMail")
#define NOTIF_APPOINTS_TXT		TEXT("Calendar & Appointments")
#define NOTIF_TASKS_TXT			TEXT("Next Tasks")
#define NOTIF_CALENDAR_TXT		TEXT("Calendar")
#define NOTIF_SMS_MMS_TXT		TEXT("Unread SMS & MMS")
#define NOTIF_WIFI_TXT			TEXT("Wifi State")
#define NOTIF_BLUETOOTH_TXT		TEXT("Bluetooth State")
#define NOTIF_ALARM_TXT			TEXT("Alarm State")
#define NOTIF_CLOCK_TXT			TEXT("Clock")
#define NOTIF_CLOCK_ALARM_TXT	TEXT("Clock & Alarm State")
#define NOTIF_BATTERY_TXT		TEXT("Battery")
#define NOTIF_VOLUME_TXT		TEXT("Volume")
#define NOTIF_MEMORYLOAD_TXT	TEXT("Memory Load")
#define NOTIF_MEMORYFREE_TXT	TEXT("Memory Free")
#define NOTIF_MEMORYUSED_TXT	TEXT("Memory Used")
#define NOTIF_CELLNETWORK_TXT	TEXT("Cell Network State")
#define NOTIF_SIGNAL_TXT		TEXT("Signal Strength")
#define NOTIF_OPERATOR_TXT		TEXT("Operator Name")
#define NOTIF_SIGNAL_OPER_TXT	TEXT("Signal Strength & Operator Name")
#define NOTIF_MC_SIG_OPER_TXT	TEXT("Missed Calls & Signal Strength & Operator Name")
#define NOTIF_PROFILE_TXT		TEXT("Phone Profile")
#define NOTIF_VMAIL_TXT			TEXT("Unplayed Voice Mails")
#define NOTIF_IRDA_TXT			TEXT("Irda State")
#define NOTIF_CRADLE_TXT		TEXT("Cradle State")
