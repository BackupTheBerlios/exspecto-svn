//-------------------------------------------------------------------------------------
//Module: CSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� �������������� � ��������
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
	virtual ~CSocket(void);

	//�������, ������������ ��� ��������� ������
	int GetLastError(void);

	//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
	//								 iPort - ����, � �������� ���������� ������������
	int Connect( std::string strAddr, int iPort );

	//������� ��������: �������� �� strName ip - �������
	bool IsAddr(std::string strName);

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
};
