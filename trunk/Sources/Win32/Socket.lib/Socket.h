//-------------------------------------------------------------------------------------
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � �������� (����� ��� ������� � ������� �����)
//-------------------------------------------------------------------------------------
#pragma once
#include "winsock2.h"
#include <string>

class CSocket 
{
public:
	//�����������, iType - ��� ������,����� ���� SOCK_STREAM/SOCK_DGRAM
	//			   bBlocking - ��� �������, �� ��������� - �����������
	CSocket( int iType, bool bBlocking = true );

	CSocket( SOCKET s, bool bBlocking );

	virtual ~CSocket(void);

	//�������, ������������ ��� ��������� ������
	int GetLastError(void);

	//������� �������� ������
    int Close( void );

	//������� ������� ������,���������� SOCKET_ERROR ���� ���-�� ������������ ����
	int Send( void* pBuffer, int iSize );

	//������� �����,���������� SOCKET_ERROR ���� ���-�� ������������ ����
	int Receive( void* pBuffer, int iBufSize );

	//�������, ��������������� ��� �������(true - �����������,false - ������������� )
	void SetBlocking( bool bIsBlocking );

	//��� ������������� ������������� �������, ������ ���������� true,���� � �������� �����
	//��������� ������ � ����� ����������� �������� Receive
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForRead( int iTimeout = -1 );

	//��� ������������� ������������� �������, ������ ���������� true,���� ����� ����� �
	//������
	//Timeout - ����� �������� (���),���� -1,����������� ��������
	bool IsReadyForWrite( int iTimeout = -1 );

protected:
	//����,����������� �� ��� ������������ ������� (�����������/�������������)
	bool m_bBlocking;

	//����������, ���������� ��� ��������� ������
	int m_iLastError;

	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;

	//��� ������
	int m_iType;
};
