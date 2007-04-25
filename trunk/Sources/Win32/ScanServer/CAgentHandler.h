//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����, ��������������� �������������� � �������
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include "ClientSocket.h"
#include "commands.h"
#include "packet.h"
#include "SmartPtr.hpp"
#include "Event.hpp"
#include "CDBProvider.h"

class CConnectionHandler;

class CAgentHandler
{
public:

	//������ ����������, ������������ CAgentHandler
	class HandlerErr: public std::runtime_error
	{
	public:
	
		HandlerErr( const std::string& strErr ):std::runtime_error( strErr ){};
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
	enumAgentResponse BeginScan( std::vector< std::string > vecAddresses );
	
	//���������� ������������
	enumAgentResponse StopScan();
	
	//�������� ������
	enumAgentResponse GetStatus( enumAgentState& Status );
	
	//�������� ������ ���������� ������������
	enumAgentResponse GetData();
	
	//������� �� ���������� � �������
	bool IsOpened()const;
	
	void OnConnection( SmartPtr< CSocket > pSocket );
	
	void OnMessage( CPacket& Msg );
	
	CEvent& GetScanFinishedEvent(){ return m_ScanFinished; };
		
protected:
	
	//��������� ����� Msg ������ � �������� ����� � pbRespBuf, iRespSize - ��������� ������ ������
	void SendMessage( CPacket &Msg, std::vector< BYTE >& vecBuf );
	
private:
	CAgentHandler( const CAgentHandler& );
	CAgentHandler& operator=( const CAgentHandler& );


	//����� ������
	std::string m_strAddress;
	
	//����� ��� ���������� � �������
	CClientSocket m_Sock;
	
	SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
	CEvent m_ScanFinished;

	//�������� �����
	std::vector<BYTE> m_vecRecvBuf;

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
