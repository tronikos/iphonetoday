/* 
 *  lpng.h, version 16-01-2009
 *
 *  Copyright (c) 2009, Alex Pankratov, ap-at-swapped-dot-cc
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or 
 *  sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *  
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _LOAD_PNG_H_
#define _LOAD_PNG_H_

/*
 *	To load PNG image from a disk file use:
 *
 *		LoadPng(L"c:\\sample.png", NULL, NULL, FALSE);
 *
 *	To load from a resource in process's own .exe use:
 *
 *		LoadPng(MAKEINTRESOURCE(IDR_SAMPLE), 
 *		        MAKEINTRESOURCE(1001),
 *		        NULL,
 *		        FALSE);
 *
 *	To load from a resource in some other .exe or .dll use:
 *
 *		module = LoadLibrary(L"c:\\sample.dll");
 *
 *		LoadPng(MAKEINTRESOURCE(IDR_SAMPLE), 
 *		        MAKEINTRESOURCE(1001),
 *		        module, 
 *		        FALSE);
 *
 *	where "1001" is a type of the resource. It is selected,
 *	when the .png image is imported into the resource file
 *	and it can be whatever.
 *
 *	The 'premultiplyAlpha' parameter needs to be TRUE if the
 *	bitmap is to be used with AlphaBlend() function or FALSE
 *	otherwise (e.g. if it will be added to an ImageList).
 */

HBITMAP LoadPng(const wchar_t * resName,
                const wchar_t * resType,
                HMODULE         resInst,
		BOOL   premultiplyAlpha);

#endif
