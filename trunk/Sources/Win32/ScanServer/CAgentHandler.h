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
#include "SmartPtr.hpp"

#define PORT 4000

class CConnectionHandler;

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
	
	void OnConnection( SmartPtr< CSocket > pSocket );
	
	void OnMessage( CPacket& Msg );
	
	bool IsScanFinished(){ return m_bFinished; };
		
protected:
	
	//��������� ����� Msg ������ � �������� ����� � pbRespBuf, iRespSize - ��������� ������ ������
	SmartPtr< BYTE, AllocMalloc<BYTE> > SendMessage( CPacket &Msg )throw( HandlerErr, CSocket::SocketErr );
	
private:
	
	//����� ������
	std::string m_strAddress;
	
	//����� ��� ���������� � �������
	CClientSocket m_Sock;
	
	SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
	bool m_bFinished;
	
};

class CConnectionHandler
{
public:
	
	typedef void(CConnectionHandler::*fnOnMessageProc)(CPacket&);
	
	CConnectionHandler( CAgentHandler* pAgentHandler ):m_pAgentHandler( pAgentHandler )
	{
		m_hCloseEv = CreateEvent( 0,0,0,0 );
	};
	
	virtual ~CConnectionHandler();
	
	void Listen( SmartPtr<CSocket> pSocket );
	
private:
	
	static unsigned __stdcall fnListenThread( void* );
	
	HANDLE m_hListenThread, m_hCloseEv;
	
	CAgentHandler* m_pAgentHandler;
	
	SmartPtr< CSocket > m_pSocket;
};

#endif /*CAGENTHANDLER_H_*/
