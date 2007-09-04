//Группа методов для работы с иконкой в област System Tray 

#include "windows.h"

	//Adds an icon to the taskbar status area
	//hwnd - handle to the window to receive callback messages
	//uID - identifier of the icon
	//hicon - handle to the icon to add
	//lpszTip - ToolTip text
	//TRUE if successful, or FALSE otherwise
	BOOL TaskBarAddIcon( HWND hwnd, UINT uID, HICON hicon, LPCTSTR lpszTip,  UINT uCallbackMessage );

	//Adds an icon to the taskbar status area
	//hwnd - handle to the window that added the icon
	//uID - identifier of the icon
	//hicon - handle to the icon to add
	//TRUE if successful, or FALSE otherwise
	BOOL TaskBarModifyIcon( HWND hwnd, UINT uID, HICON hicon );

	//Adds an icon to the taskbar status area
	//hwnd - handle to the window that added the icon
	//uID - identifier of the icon to delete
	//TRUE if successful, or FALSE otherwise.
	BOOL TaskBarDeleteIcon( HWND hwnd, UINT uID );

	//Метод позволяет выбрать исходный 16x16 instance иконки, если та была
	//представлена ico-файлом сразу в нескольких кадрах разных размеров (32, 48, 16 и т.п.)
	//nResID - численный идентификатор ресурса, содержащего иконку
	//return хэндл на загруженную иконку.
	//полученный хэндл следует освобождать через ::DestroyIcon()
	//природа происхождения данного метода в следующей тонкости: если требуется загрузить
	//16х16 иконку из составного ресурса, применение LoadIcon() приведёт к тому, что будет загружен
	//instance кадра 32х32, после чего, в процессе использования будет осуществляться масштабирование.
	HICON LoadIcon16( HINSTANCE hExe, int nResID );
