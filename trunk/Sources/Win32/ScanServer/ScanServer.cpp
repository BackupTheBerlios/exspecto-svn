#include "precomp.h"
#include "CScheduler.h"
#include "TrayManagement.h"

#define IDI_SCHEDULER 200
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szMenuName[MAX_LOADSTRING];			// the main window class name
//Сообщение поступающее от иконки в трее
#define TRAY_ICON_MSG	WM_USER+1
//Идентификатор команды нажатия на пункт Exit в меню иконки
#define IDM_EXIT	1

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

CScheduler* pScheduler;

//Обработчик сообщений от иконки в трее
void OnTrayIcon( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	// Идентификатор иконки
		switch( wParam ) {
			case 0:
			{
				// Тип нотификации
				switch( lParam ) {
					
					// Поднимаем popup-меню, при помощи которого можно управлять работой сервиса
					case WM_RBUTTONDOWN:
					{
						HMENU hMenu = CreatePopupMenu();
						if( hMenu )
						{
							AppendMenu( hMenu, MF_STRING | MF_ENABLED, IDM_EXIT, "Exit" );

							POINT pt;
							::GetCursorPos( &pt );
							if( IDM_EXIT == TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL ) )
								DestroyWindow(hWnd);							
						}
						DestroyMenu( hMenu );
					} break;
				}
			} break;
			default:
			{
			} break;
		}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;

	// Initialize global strings
	strcpy( szTitle, "Exspecto ScanServer" );
	strcpy( szWindowClass, "ExspectoScanServerWindowsClass" );
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
  // Если окно создано успешно - поднимаем иконку в область системного трэя
   // и регистрируем callback-сообщения на себя
	TaskBarAddIcon(
		   hWnd,
		   0,
		   LoadIcon(hInst, MAKEINTRESOURCE( IDI_SCHEDULER )),
		   szTitle,
		   TRAY_ICON_MSG
		   );
		   
	pScheduler = new CScheduler();
		   
   	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case TRAY_ICON_MSG:
			OnTrayIcon( hWnd, wParam, lParam );
			break;
	case WM_DESTROY:
		delete pScheduler;
		DumpMemLeaks();
		// Разрушаем иконку, помещённую в системный трэй
		TaskBarDeleteIcon( hWnd, 0 );
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

