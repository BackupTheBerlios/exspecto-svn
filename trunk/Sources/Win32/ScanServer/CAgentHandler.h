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
#include <set>

class CConnectionHandler;
class CReceiver;

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
	enumAgentResponse SendMessage( CPacket &Msg, CReceiver& Receiver );
	
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

//Абстрактный интерфейс к приемнику данных во время получения от агента
class CReceiver
{
public:

	virtual ~CReceiver(){}

	typedef std::vector<BYTE> buf_t;

	virtual buf_t::iterator AddData( buf_t::iterator begin, buf_t::iterator end ) = 0;
};

//Получает данные в заданный вектор
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

//Разбирает данные и кладет их в БД
class CDbReceiver: public CReceiver
{
public:

	CDbReceiver():m_ulDataSize(0){}
	virtual ~CDbReceiver(){}

	virtual buf_t::iterator AddData( buf_t::iterator begin, buf_t::iterator end );

private:

	unsigned long m_ulDataSize;

	//карта хостов, которые уже были очещены
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
