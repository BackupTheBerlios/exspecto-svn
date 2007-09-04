//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agent handler class
//Author: Parshin Dmitry
//Description: Класс, инкапсулирующий взаимодействие с агентом
//-------------------------------------------------------------------------------------//
#ifndef CAGENTHANDLER_H_
#define CAGENTHANDLER_H_

#include "ClientSocket.h"
#include "packet.h"
#include "SmartPtr.hpp"
#include "Event.hpp"
#include "CDBProvider.h"
#include <set>


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
	std::string BeginScan( std::vector< std::string > vecAddresses );
	
	//Остановить сканирование
	std::string StopScan();
	
	//Получить статус
	std::string GetStatus( std::string& Status );
	
	//Получить данные последнего сканирования
	std::string GetData();
	
	//Открыто ли соединение с агентом
	bool IsOpened()const;
	
	void OnConnection( SmartPtr< CSocket > pSocket );
	
	CEvent& GetScanFinishedEvent(){ return m_ScanFinished; };
		
protected:

	void OnMessage( CInPacket& Msg );
	
	//Отправить пакет Msg агенту и получить ответ в pbRespBuf, iRespSize - ожидаемый размер ответа
	//enumAgentResponse SendMessage( COutPacket &Msg, CReceiver& Receiver );

	void SendMessage( COutPacket &Msg, CInPacket& Response );
	
private:
	CAgentHandler( const CAgentHandler& );
	CAgentHandler& operator=( const CAgentHandler& );

	//Критическая секция на посылку команд
	CCriticalSection m_csExec;
	
	//Адрес агента
	std::string m_strAddress;
	
	//Сокет для соединения с агентом
	CClientSocket m_Sock;
	
	SmartPtr< CConnectionHandler > m_pConnectionHandler;
	
	CEvent m_ScanFinished;

	//Приемный буфер
	std::vector<BYTE> m_vecRecvBuf;

};

#endif /*CAGENTHANDLER_H_*/
