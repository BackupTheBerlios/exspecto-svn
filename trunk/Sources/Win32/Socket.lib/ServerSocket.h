//-------------------------------------------------------------------------------------
//Module: CClientSocket class
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� ����� �������
//-------------------------------------------------------------------------------------
#pragma once
#include "Socket.h"
#include <string>

class CServerSocket: public CSocket
{
public:
	CServerSocket( int iType, bool bBlocking );
	~CServerSocket(void);

	//������� "������������" ������ � ������, �� ��������� �� ���� �������� ����� � ����
	int Bind( int iPort = 0, std::string strAddr = "Any" );

protected:
};
