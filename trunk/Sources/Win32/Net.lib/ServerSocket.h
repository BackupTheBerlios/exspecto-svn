//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� ����� �������
//-------------------------------------------------------------------------------------//
#pragma once
#include "Socket.h"
#include <string>

//���������, ����������� ����� ���������� � ����
struct structAddr
{
	std::string strAddr;
	std::string strName;
	int iPort;
};

class CServerSocket: public CSocket
{
public:
	//�����������
	//iType - ��� ������, ��������� �������� - SOCK_STREAM � SOCK_DGRAM
	//bBlocking - ����������� ���� �� ����������� �����
	CServerSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//���������� �����������
	CServerSocket( CServerSocket& S );

	~CServerSocket(void);

	//������� "������������" ������ � ������, �� ��������� �� ���� �������� ����� � ����
	int Bind( int iPort = 0, std::string strAddr = "Any" );

	//������� ������ "�������������", iMaxConn - ������������ ���-�� ����������,
	//SOMAXCONN - ������������ ��������
	int Listen( int iMaxConn = SOMAXCONN );

	//��� ������ accept, ����� ����������� ������ �� ��������� ������� � �������� 
	//���������� (�� �������� � �������� �� ���������� ������). ������� ���������� 
	//����� �����, ������� ����� �������������� ��� ����� � ���������������� ������� 
	//(������� ������ ��� ����, ��� �������� ����������). ����� ��������� ������ 
	//� ���������������� ������ accept ���������� � ��������� addr 
	//(��� ������, IP-�����, ����).
	CSocket* Accept( structAddr& addr );
protected:
};
