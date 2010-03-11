#pragma once

#include "stdafx.h"

// Utility functions being used by both iPhoneToday and iPhoneTodayCmdLine

WORD GetVolumePercentage();
void SetVolumePercentage(WORD vol);

void Rotate(int angle);
void ToggleKeyboard();
