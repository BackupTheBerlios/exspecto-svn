#include "TrayManagement.h"
#include <shellapi.h>

	// -----------------------------------------------------------------------------------------------
	BOOL TaskBarAddIcon( HWND hwnd, UINT uID, HICON hicon, LPCTSTR lpszTip, UINT uCallbackMessage )
	{ 
		BOOL res;
		NOTIFYICONDATA tnid;
	 
		tnid.cbSize = sizeof( NOTIFYICONDATA );
		tnid.hWnd = hwnd;
		tnid.uID = uID;
		tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnid.uCallbackMessage = uCallbackMessage;
		tnid.hIcon = hicon;
		if( lpszTip )
			lstrcpyn( tnid.szTip, lpszTip, sizeof( tnid.szTip ) );
		else
			tnid.szTip[0] = 0;
	 
		res = Shell_NotifyIcon( NIM_ADD, &tnid );
		if( hicon )
			DestroyIcon( hicon );
	 
		return res;
	}
	// -----------------------------------------------------------------------------------------------
	BOOL TaskBarModifyIcon( HWND hwnd, UINT uID, HICON hicon )
	{
		BOOL res;
		NOTIFYICONDATA tnid;

		tnid.cbSize = sizeof( NOTIFYICONDATA );
		tnid.hWnd = hwnd;
		tnid.uID = uID;
		tnid.uFlags = NIF_ICON;
		tnid.hIcon = hicon;

		res = Shell_NotifyIcon( NIM_MODIFY, &tnid );
		if( hicon )
			DestroyIcon( hicon );

		return res;
	}
	// -----------------------------------------------------------------------------------------------
	BOOL TaskBarDeleteIcon( HWND hwnd, UINT uID )
	{
		BOOL res;
		NOTIFYICONDATA tnid;

		tnid.cbSize = sizeof( NOTIFYICONDATA );
		tnid.hWnd = hwnd;
		tnid.uID = uID;

		res = Shell_NotifyIcon( NIM_DELETE, &tnid );
		return res;
	}
	// -----------------------------------------------------------------------------------------------
	HICON LoadIcon16( HINSTANCE hExe, int nResID )
	{
		// Данная функция взята из MSDN-справочника (VS.NET 7.0). Внутренняя ссылка на оригинльную статью:
		// ms-help://MS.VSCC/MS.MSDNVS/winui/icons_4fjn.htm

		HRSRC hResource;	// handle for FindResource
		HRSRC hMem;			// handle for LoadResource
		BYTE *lpResource;   // pointer to resource data
		int nID;            // ID of resource that best fits current screen

		// Find the icon directory whose identifier is nResID
		hResource = FindResource( hExe, MAKEINTRESOURCE( nResID ), RT_GROUP_ICON );
		// Load and lock the icon directory.
		hMem = (HRSRC)LoadResource( hExe, hResource );
		lpResource = (BYTE*)LockResource( hMem );
			
		// Get the identifier of the icon that is most appropriate for the video display.
		nID = LookupIconIdFromDirectoryEx( (PBYTE) lpResource, TRUE, 16, 16, LR_DEFAULTCOLOR );

		// Find the bits for the nID icon.
		hResource = FindResource( hExe, MAKEINTRESOURCE( nID ), MAKEINTRESOURCE( RT_ICON ) );

		// Load and lock the icon.
		hMem = (HRSRC)LoadResource( hExe, hResource );
		lpResource = (BYTE*)LockResource( hMem );

		// Create a handle to the icon.
		return CreateIconFromResourceEx( (PBYTE) lpResource,
			SizeofResource(hExe, hResource), TRUE, 0x00030000,
			16, 16, LR_DEFAULTCOLOR );
	}
	// -----------------------------------------------------------------------------------------------
