#pragma once

#include "stdafx.h"

typedef struct ChooseFontSettings {
	TCHAR facename[LF_FACESIZE];
	COLORREF color;
	UINT height;
	UINT weight;
	UINT shadow;
	BOOL roundrect;
} ChooseFontSettings;

extern ChooseFontSettings cfs;
extern BOOL bChooseFontOK;

int CALLBACK EnumFontFamiliesProc(ENUMLOGFONT *lpelf, TEXTMETRIC *lpntm, int FontType, LPARAM lParam);
LRESULT CALLBACK ChooseFontProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
