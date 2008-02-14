//------------------------------------------------------------------------------------//
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: 
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

  class HandlerErr: public std::runtime_error
	{
	public:
	
	HandlerErr( const std::string& strErr ):std::runtime_error( strErr ){};
	  virtual ~HandlerErr()throw(){};	
	}; 
	
  virtual ~CAgentHandler();
	
  CAgentHandler( std::string strAgentAddress );
	
  void Open();
	
  void Close();
	
	
  std::string BeginScan( std::vector< std::string > vecAddresses );

  std::string StopScan();
	
  std::string GetStatus( std::string& Status );
	
  std::string GetData();
	
  bool IsOpened()const;
	
  void OnConnection( SmartPtr< CSocket > pSocket );
	
  CEvent& GetScanFinishedEvent(){ return m_ScanFinished; };
		
 protected:

  void OnMessage( CInPacket& Msg );
	
  //enumAgentResponse SendMessage( COutPacket &Msg, CReceiver& Receiver );

  void SendMessage( COutPacket &Msg, CInPacket& Response );
	
 private:
  CAgentHandler( const CAgentHandler& );
  CAgentHandler& operator=( const CAgentHandler& );

  CMutex m_mtxExec;
	
  std::string m_strAddress;
	
  CClientSocket m_Sock;
	
  SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
  CEvent m_ScanFinished;

  std::vector<BYTE> m_vecRecvBuf;

};

#endif /*CAGENTHANDLER_H_*/
