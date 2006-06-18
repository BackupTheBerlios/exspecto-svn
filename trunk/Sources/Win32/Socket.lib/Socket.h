#pragma once
#include "winsock2.h"
#include <string>

class CSocket
{
public:
	CSocket( int iType );
	virtual ~CSocket(void);

	//�������, ������������ ��� ��������� ������
	int GetLastError(void);

	//���������� � strAddr:iPort,��� sAddr - ��� ���������� �����,���� ��� IP-������
	//								 iPort - ����, � �������� ���������� ������������
	int Connect( std::string strAddr, int iPort );

	//������� ��������: �������� �� strName ip - �������
	bool IsAddr(std::string strName);

protected:
	
	int m_iLastError;
	//���������� ������, ������������ ����������� �� ���� ��������
	SOCKET m_Socket;
};
