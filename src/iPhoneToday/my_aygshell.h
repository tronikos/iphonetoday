typedef enum tagOFN_EXFLAG
{
    OFN_EXFLAG_DETAILSVIEW          = 0x00000001,
    OFN_EXFLAG_THUMBNAILVIEW        = 0x00000002,
    OFN_EXFLAG_LOCKDIRECTORY        = 0x00000100,
    OFN_EXFLAG_NOFILECREATE         = 0x00000200,
    OFN_EXFLAG_HIDEDRMPROTECTED     = 0x00010000,     //If this flag is set and the DRM engine is installed - the PicturePicker will not show ANY DRM content
    OFN_EXFLAG_HIDEDRMFORWARDLOCKED = 0x00020000     //If this flag is set and the DRM engine is installed - the PicturePicker will not show ANY DRM FORWARD LOCK content
} OFN_EXFLAG;

typedef struct tagOPENFILENAMEEX
{
    // Fields which map to OPENFILENAME
   DWORD        lStructSize;
   HWND         hwndOwner;
   HINSTANCE    hInstance;
   LPCTSTR      lpstrFilter;
   LPTSTR       lpstrCustomFilter;
   DWORD        nMaxCustFilter;
   DWORD        nFilterIndex;
   LPTSTR       lpstrFile;
   DWORD        nMaxFile;
   LPTSTR       lpstrFileTitle;
   DWORD        nMaxFileTitle;
   LPCTSTR      lpstrInitialDir;
   LPCTSTR      lpstrTitle;
   DWORD        Flags;
   WORD         nFileOffset;
   WORD         nFileExtension;
   LPCTSTR      lpstrDefExt;
   LPARAM       lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCTSTR      lpTemplateName;

   // Extended fields
   DWORD       dwSortOrder;
   DWORD       ExFlags;
}OPENFILENAMEEX, *LPOPENFILENAMEEX ;

BOOL GetOpenFileNameEx(LPOPENFILENAMEEX lpofnex);
