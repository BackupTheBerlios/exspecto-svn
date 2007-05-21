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
#include <set>

class CConnectionHandler;
class CReceiver;

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
	enumAgentResponse SendMessage( CPacket &Msg, CReceiver& Receiver );
	
private:
	CAgentHandler( const CAgentHandler& );
	CAgentHandler& operator=( const CAgentHandler& );

	//����������� ������ �� ������� ������
	CCriticalSection m_csExec;
	
	//����� ������
	std::string m_strAddress;
	
	//����� ��� ���������� � �������
	CClientSocket m_Sock;
	
	SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
	CEvent m_ScanFinished;

	//�������� �����
	std::vector<BYTE> m_vecRecvBuf;

};

//����������� ��������� � ��������� ������ �� ����� ��������� �� ������
class CReceiver
{
public:

	virtual ~CReceiver(){}

	typedef std::vector<BYTE> buf_t;

	virtual buf_t::iterator AddData( buf_t::iterator begin, buf_t::iterator end ) = 0;
};

//�������� ������ � �������� ������
class CBufReceiver: public CReceiver
{
public:

	virtual ~CBufReceiver(){}

	CBufReceiver( buf_t& buf ):m_buf( buf ){};

	virtual buf_t::iterator AddData( buf_t::iterator begin, buf_t::iterator end )
	{
		m_buf.insert( m_buf.end(), begin, end );
		return end;
	}

private:

	buf_t& m_buf;
};

//��������� ������ � ������ �� � ��
class CDbReceiver: public CReceiver
{
public:

	CDbReceiver():m_ulDataSize(0){}
	virtual ~CDbReceiver(){}

	virtual buf_t::iterator AddData( buf_t::iterator begin, buf_t::iterator end );

private:

	unsigned long m_ulDataSize;

	//����� ������, ������� ��� ���� �������
	std::set< std::string > m_mapErased;

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
