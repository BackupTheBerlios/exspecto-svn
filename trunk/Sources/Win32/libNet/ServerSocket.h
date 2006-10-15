//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CServerSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� ����� �������
//-------------------------------------------------------------------------------------//
#ifndef SERVERSOCKET_H_
#define SERVERSOCKET_H_

#include "precomp.h"
#include "Socket.h"

class CServerSocket: public CSocket
{
public:

	

	//���������, ����������� ����� ���������� � ����
	struct structAddr
	{
		std::string strAddr;
		std::string strName;
		int iPort;
	};

	//�����������
	//iType - ��� ������, ��������� �������� - SOCK_STREAM � SOCK_DGRAM
	//bBlocking - ����������� ���� �� ����������� �����
	CServerSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	//���������� �����������
	CServerSocket( CServerSocket& S );

	~CServerSocket(void);

	//������� "������������" ������ � ������, �� ��������� �� ���� �������� ����� � ����
	void Bind( int iPort = 0, std::string strAddr = "Any" )throw( SocketDNSErr, SocketErr );

	//������� ������ "�������������", iMaxConn - ������������ ���-�� ����������,
	//SOMAXCONN - ������������ ��������
	void Listen( int iMaxConn = SOMAXCONN )throw( SocketErr );

	//��� ������ accept, ����� ����������� ������ �� ��������� ������� � �������� 
	//���������� (�� �������� � �������� �� ���������� ������). ������� ���������� 
	//����� �����, ������� ����� �������������� ��� ����� � ���������������� ������� 
	//(������� ������ ��� ����, ��� �������� ����������). ����� ��������� ������ 
	//� ���������������� ������ accept ���������� � ��������� addr 
	//(��� ������, IP-�����, ����).
	CSocket* Accept( structAddr& addr )throw( SocketErr );
protected:
};

#endif
