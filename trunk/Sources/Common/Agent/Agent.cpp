#include "precomp.h"
#include "CAgent.h"
#include "windows.h"
#include "TrayManagement.h"


#define IDI_AGENT 200
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szMenuName[MAX_LOADSTRING];			// the main window class name
//��������� ����������� �� ������ � ����
#define TRAY_ICON_MSG	WM_USER+1
//������������� ������� ������� �� ����� Exit � ���� ������
#define IDM_EXIT	1

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

CAgent* pAgent;

//���������� ��������� �� ������ � ����
void OnTrayIcon( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	// ������������� ������
		switch( wParam ) {
			case 0:
			{
				// ��� �����������
				switch( lParam ) {
					
					// ��������� popup-����, ��� ������ �������� ����� ��������� ������� �������
					case WM_RBUTTONDOWN:
					{
						HMENU hMenu = CreatePopupMenu();
						if( hMenu )
						{
							AppendMenu( hMenu, MF_STRING | MF_ENABLED, IDM_EXIT, "Exit" );
							SetForegroundWindow( hWnd );

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
	strcpy( szTitle, "Exspecto Agent" );
	strcpy( szWindowClass, "ExspectoAgentWindowsClass" );
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

	hWnd = CreateWindow(szWindowClass, szTitle, 0,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	// ���� ���� ������� ������� - ��������� ������ � ������� ���������� ����
   // � ������������ callback-��������� �� ����
	TaskBarAddIcon(
		   hWnd,
		   0,
		   LoadIcon(hInst, MAKEINTRESOURCE( IDI_AGENT )),
		   szTitle,
		   TRAY_ICON_MSG
		   );

	try{
		pAgent = new CAgent();
		if( !pAgent->IsStarted() )
			return FALSE;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "InitInstance: �������� ���������� ��� �������������: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "InitInstance: �������� ����������� ���������� ��� �������������" );
	}
		   

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
		delete pAgent;
		DumpMemLeaks();
		// ��������� ������, ���������� � ��������� ����
		TaskBarDeleteIcon( hWnd, 0 );
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

