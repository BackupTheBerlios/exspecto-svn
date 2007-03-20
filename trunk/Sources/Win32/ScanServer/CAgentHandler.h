//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
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

	//классы исключений, генерируемых CAgentHandler
	class HandlerErr: public std::runtime_error
	{
	public:
	
		HandlerErr( const std::string& strErr )throw():std::runtime_error( strErr ){};
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
	enumAgentResponse BeginScan( std::vector< std::string > vecAddresses )throw( HandlerErr, CSocket::SocketErr );
	
	//Остановить сканирование
	enumAgentResponse StopScan()throw( HandlerErr, CSocket::SocketErr );
	
	//Получить статус
	enumAgentResponse GetStatus( enumAgentState& Status )throw( HandlerErr, CSocket::SocketErr );
	
	//Получить данные последнего сканирования
	enumAgentResponse GetData()throw( HandlerErr, CSocket::SocketErr );
	
	//Открыто ли соединение с агентом
	bool IsOpened()const;
	
	void OnConnection( SmartPtr< CSocket > pSocket );
	
	void OnMessage( CPacket& Msg );
	
	bool IsScanFinished(){ return m_bFinished; };
		
protected:
	
	//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
	SmartPtr< BYTE, AllocMalloc<BYTE> > SendMessage( CPacket &Msg )throw( HandlerErr, CSocket::SocketErr );
	
private:
	
	//Адрес агента
	std::string m_strAddress;
	
	//Сокет для соединения с агентом
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
