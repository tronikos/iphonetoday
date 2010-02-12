#include "Log.h"

HANDLE	g_hLogFile;
TCHAR	g_szLogPath[MAX_PATH];


// This function could write ASCII log files instead of unicode, but internationalization
// was important for this sample
void WriteToLog(LPTSTR tszLine)
{
	DWORD dwWritten;

	TCHAR*            ptszSystemDate = (TCHAR*)LocalAlloc(LPTR, MAX_PATH);

	wsprintf(g_szLogPath, TEXT("\\log.txt"));
	g_hLogFile = CreateFile(g_szLogPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, 
                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
    SetFilePointer(g_hLogFile, 0, NULL, FILE_END);
    
	WriteFile(g_hLogFile, tszLine, _tcslen(tszLine) * sizeof(*tszLine), &dwWritten, NULL);
            
    CloseHandle(g_hLogFile);
    LocalFree(ptszSystemDate);
}