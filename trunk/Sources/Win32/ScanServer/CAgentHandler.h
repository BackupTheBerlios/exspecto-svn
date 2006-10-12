//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����, ��������������� �������������� � �������
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include <vector>
#include <string>
#include "commands.h"
#include "..\libNet\packet.h"
#include "..\libNet\ClientSocket.h"

#define PORT 5000

class CAgentHandler
{
public:

	//��� ������������ ������� ������ �� �������
	enum enumCommandResult{
		RES_OK = 0x00,			//�� ������
		RES_NOT_OPENED,			//������ ����������� ������ �����,���� ����� �� �������
		RES_NO_RESPONSE,		//������ �� ������ �� ������� �� �����������
		RES_AGENT_ERR,			//����� �� ���� ��������� �������
		RES_INCORRECT,			//�������� �����
		RES_SOCK_ERR			//����� ������ ������
	};

	virtual ~CAgentHandler();
	
	CAgentHandler( std::string strAgentAddress );
	
	bool Open();
	
	bool Close();
	
	enumCommandResult BeginScan( std::vector< std::string > vecAddresses );
	
	enumCommandResult StopScan();
	
	enumCommandResult GetStatus( enumAgentStatus& Status );
	
	enumCommandResult GetData();
	
	bool IsOpened();
	

protected:
	
	enumCommandResult SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize );

private:
	
	std::string m_strAddress;
	
	CClientSocket m_Sock;
	
	bool m_bOpened;
	
};

#endif /*CAGENTHANDLER_H_*/
