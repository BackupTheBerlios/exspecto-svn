//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ���������� ����� �������
//-------------------------------------------------------------------------------------//
#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_

#include "precomp.h"
#include "Socket.h"

class CClientSocket: public CSocket
{
public:

	//�����������
	//iType - ��� ������, ��������� �������� - SOCK_STREAM � SOCK_DGRAM
	//bBlocking - ����������� ���� �� ����������� �����
	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true )throw( SocketErr );

	~CClientSocket( void )throw( SocketErr );


	//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
	//								 iPort - ����, � �������� ���������� ������������
	void Connect( std::string strAddr, int iPort )throw( SocketDNSErr, CSocket::SocketErr );
};

#endif
