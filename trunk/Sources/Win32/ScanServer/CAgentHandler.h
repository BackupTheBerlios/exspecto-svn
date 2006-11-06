//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����, ��������������� �������������� � �������
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include "commands.h"
#include "packet.h"
#include "ClientSocket.h"

#define PORT 5000

class CAgentHandler
{
public:

	//������ ����������, ������������ CAgentHandler
	class HandlerErr: public std::runtime_error
	{
	public:
	
		HandlerErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
		virtual ~HandlerErr()throw(){};	
	}; 
	
	virtual ~CAgentHandler();
	
	//������� ������ �������������� � ������� �� ������ strAgentAddress 
	CAgentHandler( std::string strAgentAddress );
	
	//������� ���������� � �������
	void Open();
	
	//������� ���������� � �������
	void Close();
	
	//�������, ������������ ������:
	
	//������ ������������
	enumAgentResponse BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr );
	
	//���������� ������������
	enumAgentResponse StopScan()throw( HandlerErr, CSocket::SocketErr );
	
	//�������� ������
	enumAgentResponse GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr );
	
	//�������� ������ ���������� ������������
	enumAgentResponse GetData()throw( HandlerErr, CSocket::SocketErr );
	
	//������� �� ���������� � �������
	bool IsOpened()const;
	

protected:
	
	//��������� ����� Msg ������ � �������� ����� � pbRespBuf, iRespSize - ��������� ������ ������
	void SendMessage( CPacket &Msg, BYTE* pbRespBuf, int iRespSize )throw( HandlerErr, CSocket::SocketErr );

private:
	
	//����� ������
	std::string m_strAddress;
	
	//����� ��� ���������� � �������
	CClientSocket m_Sock;
	
	//���� �������� ����������
	bool m_bOpened;
	
};

#endif /*CAGENTHANDLER_H_*/
