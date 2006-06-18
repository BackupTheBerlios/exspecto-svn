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

	//������� ������� ������
	int Send( void* pBuffer, int iSize );

	//�������, ��������������� ��� �������(true - �����������,false - ������������� )
	void SetBlocking( bool bIsBlocking );

protected:
	//����,����������� �� ��� ������������ ������� (�����������/�������������)
	bool m_bBlocking;

	//����������, ���������� ��� ��������� ������
	int m_iLastError;

	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;
};
