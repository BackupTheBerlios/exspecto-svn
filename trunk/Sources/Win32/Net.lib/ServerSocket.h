//-------------------------------------------------------------------------------------
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� ����� �������
//-------------------------------------------------------------------------------------
#pragma once
#include "Socket.h"
#include <string>

struct structAddr
{
	std::string strAddr;
	std::string strName;
	int iPort;
};

class CServerSocket: public CSocket
{
public:
	CServerSocket( int iType = AF_INET, bool bBlocking = false );
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
