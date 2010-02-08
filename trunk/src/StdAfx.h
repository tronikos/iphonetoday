// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

// NOTA: este valor no está convenientemente asociado con la versión del sistema operativo Windows CE de destino
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

#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 


// Archivos de encabezado de Windows:
#include <windows.h>

// Archivos de encabezado en tiempo de ejecución de C
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


//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__9E197510_42C8_42C3_B502_7BC64ABA1300__INCLUDED_)
#define AFX_STDAFX_H__9E197510_42C8_42C3_B502_7BC64ABA1300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <aygshell.h>
#include <todaycmn.h>
#include <DeviceResolutionAware.h>
#define SCALEX DRA::SCALEX
#define SCALEY DRA::SCALEY

#endif 

#include "resource.h"
#include "Utils.h"
#include "Log.h"
#include "GraphicFunctions.h"
#include "CReloadIcon.h"

#define CountOf(x) (sizeof(x)/sizeof(*x))

typedef struct
{
    int nPantallaActual;
	int nIconoActual;
} IDENT_ICONO;

#define MAX_PANTALLAS 32
#define MAX_ICONOS_PANTALLA 64

#define NUM_CONFIG_SCREENS 3

#define TIMER_RECUPERACION		10000
#define TIMER_ACTUALIZA_NOTIF	10001
#define TIMER_LANZANDO_APP		10002
#define TIMER_LOADING			10003
#define TIMER_ROTATING			10004

#define MENU_POPUP_ADD		100
#define MENU_POPUP_EDIT		101
#define MENU_POPUP_DELETE	102
#define MENU_POPUP_CANCEL	103
#define MENU_POPUP_MOVE		104
#define MENU_POPUP_MOVE_HERE 105
#define MENU_POPUP_OPTIONS	106
#define MENU_POPUP_EXIT		107

#define REGISTRY_MAXLENGTH          32

#define NOTIF_NORMAL		0
#define NOTIF_LLAMADAS		1
#define NOTIF_SMS			2
#define NOTIF_MMS			3
#define NOTIF_OTHER_EMAIL	4
#define NOTIF_SYNC_EMAIL	41
#define NOTIF_TOTAL_EMAIL	42
#define NOTIF_CITAS			5
#define NOTIF_TAREAS		6
#define NOTIF_CALENDAR		7
#define NOTIF_SMS_MMS		8
#define NOTIF_WIFI			9
#define NOTIF_BLUETOOTH		10
#define NOTIF_ALARM			11
#define NOTIF_CLOCK			12
#define NOTIF_CLOCK_ALARM	13

#define NOTIF_NORMAL_TXT	TEXT("Normal")
#define NOTIF_LLAMADAS_TXT	TEXT("Missed Calls")
#define NOTIF_SMS_TXT		TEXT("Unread SMS")
#define NOTIF_MMS_TXT		TEXT("Unread MMS")
#define NOTIF_OTHER_EMAIL_TXT			TEXT("Unread Others EMail")
#define NOTIF_SYNC_EMAIL_TXT	TEXT("Unread Sync EMail")
#define NOTIF_TOTAL_EMAIL_TXT			TEXT("Unread Total EMail")
#define NOTIF_CITAS_TXT		TEXT("Calendar & Appointments")
#define NOTIF_TAREAS_TXT	TEXT("Nexts Tasks")
#define NOTIF_CALENDAR_TXT	TEXT("Calendar")
#define NOTIF_SMS_MMS_TXT	TEXT("Unread SMS & MMS")
#define NOTIF_WIFI_TXT		TEXT("Wifi State")
#define NOTIF_BLUETOOTH_TXT	TEXT("Bluetooth State")
#define NOTIF_ALARM_TXT	TEXT("Alarm State")
#define NOTIF_CLOCK_TXT	TEXT("Clock")
#define NOTIF_CLOCK_ALARM_TXT	TEXT("Clock & Alarm State")
