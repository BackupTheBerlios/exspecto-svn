//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ���������� ����� �������
//-------------------------------------------------------------------------------------//
#ifndef CCLIENTSOCKET_H_
#define CCLIENTSOCKET_H_

#include "Socket.h"

class CClientSocket: public CSocket
{
public:

	//�����������
	//iType - ��� ������, ��������� �������� - SOCK_STREAM � SOCK_DGRAM
	//bBlocking - ����������� ���� �� ����������� �����
	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	~CClientSocket( void );


	//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
	//								 iPort - ����, � �������� ���������� ������������
	void Connect( std::string strAddr, int iPort );

private:
	CClientSocket( const CClientSocket& );
	CClientSocket& operator=( const CClientSocket& );
};

#endif
