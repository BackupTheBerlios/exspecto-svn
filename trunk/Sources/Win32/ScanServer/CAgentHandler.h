//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
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

	//классы исключений, генерируемых CAgentHandler
	class HandlerErr: public std::runtime_error
	{
	public:
	
		HandlerErr( const std::string& strErr ):std::runtime_error( strErr ){};
		virtual ~HandlerErr()throw(){};	
	}; 
	
	virtual ~CAgentHandler();
	
	//Создать обьект взаимодействия с агентом по адресу strAgentAddress 
	CAgentHandler( std::string strAgentAddress );
	
	//Открыть соединение с агентом
	void Open();
	
	//Закрыть соединение с агентом
	void Close();
	
	//Команды, передаваемые агенту:
	
	//Начать сканирование
	enumAgentResponse BeginScan( std::vector< std::string > vecAddresses );
	
	//Остановить сканирование
	enumAgentResponse StopScan();
	
	//Получить статус
	enumAgentResponse GetStatus( enumAgentState& Status );
	
	//Получить данные последнего сканирования
	enumAgentResponse GetData();
	
	//Открыто ли соединение с агентом
	bool IsOpened()const;
	
	void OnConnection( SmartPtr< CSocket > pSocket );
	
	void OnMessage( CPacket& Msg );
	
	CEvent& GetScanFinishedEvent(){ return m_ScanFinished; };
		
protected:
	
	//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
	void SendMessage( CPacket &Msg, std::vector< BYTE >& vecBuf );
	
private:
	CAgentHandler( const CAgentHandler& );
	CAgentHandler& operator=( const CAgentHandler& );


	//Адрес агента
	std::string m_strAddress;
	
	//Сокет для соединения с агентом
	CClientSocket m_Sock;
	
	SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
	CEvent m_ScanFinished;

	//Приемный буфер
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
