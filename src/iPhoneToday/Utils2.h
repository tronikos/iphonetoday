#pragma once

#include "stdafx.h"

// Utility functions being used by both iPhoneToday and iPhoneTodayCmdLine

WORD ConvertVolumeToPercentage(DWORD vol);
WORD GetVolumePercentage();
void SetVolumePercentage(WORD vol);

void Rotate(int angle);
void ToggleKeyboard(int bShow = -1);

void ToggleBluetooth(int bEnable = -1);
void ToggleWLAN(int bEnable = -1);
