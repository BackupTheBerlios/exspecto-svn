//------------------------------------------------------------------------------------//
//�����������������������������Ҝ�������������Μ�������������� ��������������Ԝ����������������������������ɜ�������������� ��������������ߜ��������������������������˜�������������ߜ�������������Ŝ�������������Ҝ�������������ќ�������������� ��������������ל����������������������������ќ�������������Ҝ�������������ܜ�������������� ��������������Ϝ�������������М�������������Μ�������������Ŝ�������������ʜ�������������Ҝ�������������� Exspecto 2006���������������.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: �����������������������������˜����������������������������ќ��������������, ��������������Ȝ�������������͜�������������ʜ����������������������������Ϝ�������������ќ�������������Ӝ�������������˜�������������Ȝ�������������М�������������Ӝ�������������ޜ�������������ٜ�������������Ȝ�������������� ���������������������������ǜ����������������������������Ȝ�������������̜�������������Μ�������������Ĝ�������������Ŝ�������������ɜ�������������ќ�������������Ҝ��������������������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ��������������
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include "ClientSocket.h"
#include "Packet.h"
#include "SmartPtr.hpp"
#include "Event.h"
#include "CDBProvider.h"
#include <set>
#include <Thread.h>


enum enumAgentResponse{
  RESP_OK = 0x00,
  RESP_INPUT_DATA_ERR = 0x01,
  RESP_PROC_ERR = 0x02
};

class CConnectionHandler;
class CReceiver;

class CAgentHandler
{
 public:

  class CConnectionHandler
  {
  public:

  CConnectionHandler( CAgentHandler* pAgentHandler )
	:m_pAgentHandler( pAgentHandler )
	  ,m_ListenThread( SmartPtr<CThreadTask>( new CListenThreadTask( this ) ), false )
	  {};

	virtual ~CConnectionHandler();

	void Listen( SmartPtr<CSocket> pSocket );

  private:

	class CListenThreadTask: public CThreadTask
	{
	public:

	CListenThreadTask( CConnectionHandler* pConnectionHandler ):
	  m_pConnectionHandler( pConnectionHandler )
	  {}
		  
	  virtual void Execute( CEvent& CancelEv );

	private:
		  
	  CConnectionHandler* m_pConnectionHandler;
	};

	CThread m_ListenThread;
	
	CEvent m_CloseEv;

	CAgentHandler* m_pAgentHandler;

	SmartPtr< CSocket > m_pSocket;
  };

  //��������������ʜ�������������˜����������������������������ќ�������������ќ�������������� ��������������Ȝ�������������ќ�������������ʜ�������������˜�������������ޜ�������������ל�������������Ŝ�������������͜�������������Ȝ��������������, ��������������Ü�������������Ŝ�������������͜�������������Ŝ�������������М�������������Ȝ�������������М�������������Ӝ�������������Ŝ�������������̜�������������ۜ�������������� CAgentHandler
  class HandlerErr: public std::runtime_error
	{
	public:
	
	HandlerErr( const std::string& strErr ):std::runtime_error( strErr ){};
	  virtual ~HandlerErr()throw(){};	
	}; 
	
  virtual ~CAgentHandler();
	
  //�����������������������������Μ�������������ǜ�������������Ĝ����������������������������Ҝ�������������� ��������������Μ����������������������������ܜ�������������Ŝ�������������ʜ�������������� ���������������������������ǜ����������������������������Ȝ�������������̜�������������Μ�������������Ĝ�������������Ŝ�������������ɜ�������������ќ�������������Ҝ��������������������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ�������������� ��������������Ϝ�������������� �����������������������������Ĝ�������������М�������������Ŝ�������������ќ�������������� strAgentAddress 
  CAgentHandler( std::string strAgentAddress );
	
  //�����������������������������Ҝ�������������ʜ�������������М�������������ۜ�������������Ҝ�������������� ��������������ќ�������������Μ�������������Ŝ�������������Ĝ�������������Ȝ�������������͜�������������Ŝ�������������͜�������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ��������������
  void Open();
	
  //������������������������������������������ʜ�������������М�������������ۜ�������������Ҝ�������������� ��������������ќ�������������Μ�������������Ŝ�������������Ĝ�������������Ȝ�������������͜�������������Ŝ�������������͜�������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ��������������
  void Close();
	
  //�����������������������������Μ�������������̜����������������������������͜�������������Ĝ��������������, ��������������Ϝ�������������Ŝ�������������М�������������Ŝ�������������Ĝ��������������������������������������������������������Ŝ�������������̜�������������ۜ�������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ��������������:
	
  //��������������������������������������������ל����������������������������Ҝ�������������� ��������������ќ�������������ʜ����������������������������͜�������������Ȝ�������������М�������������Μ�����������������������������������������͜�������������Ȝ��������������
  std::string BeginScan( std::vector< std::string > vecAddresses );
	
  //�����������������������������ќ�������������Ҝ����������������������������͜�������������Μ��������������������������Ȝ�������������Ҝ�������������� ��������������ќ�������������ʜ����������������������������͜�������������Ȝ�������������М�������������Μ�����������������������������������������͜�������������Ȝ��������������
  std::string StopScan();
	
  //�����������������������������Μ�������������˜�������������Ӝ�������������ל�������������Ȝ�������������Ҝ�������������� ��������������ќ�������������Ҝ����������������������������Ҝ�������������Ӝ��������������
  std::string GetStatus( std::string& Status );
	
  //�����������������������������Μ�������������˜�������������Ӝ�������������ל�������������Ȝ�������������Ҝ�������������� ��������������Ĝ����������������������������͜�������������͜�������������ۜ�������������� ��������������Ϝ�������������Μ�������������ќ�������������˜�������������Ŝ�������������Ĝ�������������͜�������������Ŝ�������������Ü�������������� ��������������ќ�������������ʜ����������������������������͜�������������Ȝ�������������М�������������Μ�����������������������������������������͜�������������Ȝ��������������
  std::string GetData();
	
  //�����������������������������Ҝ�������������ʜ�������������М�������������ۜ�������������Ҝ�������������� ��������������˜�������������� ��������������ќ�������������Μ�������������Ŝ�������������Ĝ�������������Ȝ�������������͜�������������Ŝ�������������͜�������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ��������������
  bool IsOpened()const;
	
  void OnConnection( SmartPtr< CSocket > pSocket );
	
  CEvent& GetScanFinishedEvent(){ return m_ScanFinished; };
		
 protected:

  void OnMessage( CInPacket& Msg );
	
  //�����������������������������Ҝ�������������Ϝ�������������М�����������������������������������������Ȝ�������������Ҝ�������������� ��������������Ϝ����������������������������ʜ�������������Ŝ�������������� Msg �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������� ��������������� ��������������Ϝ�������������Μ�������������˜�������������Ӝ�������������ל�������������Ȝ�������������Ҝ�������������� ��������������Μ�������������Ҝ��������������������������Ŝ�������������� ��������������� pbRespBuf, iRespSize - ��������������Μ�������������Ɯ�������������Ȝ�������������Ĝ����������������������������Ŝ�������������̜�������������ۜ�������������� ��������������М����������������������������ǜ�������������̜�������������Ŝ�������������� ��������������Μ�������������Ҝ��������������������������Ŝ�������������Ҝ��������������
  //enumAgentResponse SendMessage( COutPacket &Msg, CReceiver& Receiver );

  void SendMessage( COutPacket &Msg, CInPacket& Response );
	
 private:
  CAgentHandler( const CAgentHandler& );
  CAgentHandler& operator=( const CAgentHandler& );

  //�����������������������������М�������������Ȝ�������������Ҝ�������������Ȝ�������������ל�������������Ŝ�������������ќ�������������ʜ����������������������������� ��������������ќ�������������Ŝ�������������ʜ�������������֜�������������Ȝ�������������� ��������������͜�������������� ��������������Ϝ�������������Μ�������������ќ�������������ۜ�������������˜�������������ʜ�������������� ��������������ʜ�������������Μ�������������̜����������������������������͜��������������
  CMutex m_mtxExec;
	
  //���������������������������Ĝ�������������М�������������Ŝ�������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ��������������
  std::string m_strAddress;
	
  //�����������������������������Μ�������������ʜ�������������Ŝ�������������� ��������������Ĝ�������������˜�������������� ��������������ќ�������������Μ�������������Ŝ�������������Ĝ�������������Ȝ�������������͜�������������Ŝ�������������͜�������������Ȝ�������������� ��������������� �����������������������������Ü�������������Ŝ�������������͜�������������Ҝ�������������Μ��������������
  CClientSocket m_Sock;
	
  SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
  CEvent m_ScanFinished;

  //�����������������������������М�������������Ȝ�������������Ŝ�������������̜�������������͜�������������ۜ�������������� �����������������������������Ӝ�������������Ԝ�������������Ŝ��������������
  std::vector<BYTE> m_vecRecvBuf;

};

#endif /*CAGENTHANDLER_H_*/
