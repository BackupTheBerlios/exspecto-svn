//������ ������� ��� ������ � ������� � ������ System Tray 

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

	//����� ��������� ������� �������� 16x16 instance ������, ���� �� ����
	//������������ ico-������ ����� � ���������� ������ ������ �������� (32, 48, 16 � �.�.)
	//nResID - ��������� ������������� �������, ����������� ������
	//return ����� �� ����������� ������.
	//���������� ����� ������� ����������� ����� ::DestroyIcon()
	//������� ������������� ������� ������ � ��������� ��������: ���� ��������� ���������
	//16�16 ������ �� ���������� �������, ���������� LoadIcon() ������� � ����, ��� ����� ��������
	//instance ����� 32�32, ����� ����, � �������� ������������� ����� �������������� ���������������.
	HICON LoadIcon16( HINSTANCE hExe, int nResID );
