//-------------------------------------------------------------------------------------
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ���������� ����� �������
//-------------------------------------------------------------------------------------
#pragma once
#include "Socket.h"
#include <string>

class CClientSocket: public CSocket
{
public:
	CClientSocket( int iType = SOCK_STREAM, bool bBlocking = true );

	~CClientSocket( void );


	//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
	//								 iPort - ����, � �������� ���������� ������������
	int Connect( std::string strAddr, int iPort );
};
