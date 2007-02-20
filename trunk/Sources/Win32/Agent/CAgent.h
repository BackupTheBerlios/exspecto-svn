//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "precomp.h"
#include "packet.h"
#include "Container.hpp"
#include "CScanner.h"
#include "PluginLoadStrategy.h"
#include <set>

#include <map>
#include <vector>
#include <deque>
#include <string>
#include "SmartPtr.hpp"
#include "CriticalSection.hpp"
#include "Event.hpp" 

class CConnectionHandler;

class CAgent
{
public:
	
	//Конструктор
	CAgent();
	
	~CAgent(void);

private:

	//Адрес планировщика
	std::string m_strSchedulerAddress;

	//Событие закрытия
	CEvent m_CloseEvent;
	
	//Дескриптор потока ожидания соединений
	HANDLE m_hListenThread;
		
	//Поток ожидания входящих соединений
	static unsigned _stdcall fnListenThreadProc(  void* pParameter );
	
	std::vector< SmartPtr< CConnectionHandler > > m_vecConnections;
};

class CTask
{
public:
	
	CTask( SmartPtr< CSocket > pSocket ):m_pSocket( pSocket )
	{};
	
	virtual ~CTask()
	{}
	
	virtual bool Immidiate() = 0;
	
	virtual void Execute() = 0;
	
	void Cancel()
	{
		m_CancelEv.Set();
	};
	
protected:

	SmartPtr< CSocket > m_pSocket;
	
	static enumAgentState m_CurState;
	
	static CCriticalSection m_csCurState; 	
	
	static std::vector< std::string > m_vecData;
	
	static CEvent m_CancelEv;
	
};

class CMessageParser
{
public:

	CMessageParser( SmartPtr< CSocket > pSocket):m_pSocket( pSocket ){};

	SmartPtr< CTask > TaskFactory( BYTE bCommandId, CPacket& Msg );
	
	std::vector< SmartPtr< CTask > > Parse( CPacket& Message );
	
private:

	SmartPtr< CSocket > m_pSocket;
};

class CTaskHandler
{
public:

	CTaskHandler();
	~CTaskHandler();
	
	void AddTask( SmartPtr< CTask > pTask );

private:

	static unsigned _stdcall fnProcessThread( void* );
	
	HANDLE m_hProcessThread, m_hCloseEv, m_hCancelOpEv;
	
	std::deque< SmartPtr< CTask > > m_deqTasks;
	
	CRITICAL_SECTION m_csTasks, m_csCurState;
};

class CConnectionHandler
{
public:

	CConnectionHandler( SmartPtr< CSocket > pSocket ):m_pSocket( pSocket )
													 ,m_MessageParser( m_pSocket )
	{};

	void Listen();

private:
	
	SmartPtr< CSocket > m_pSocket;
	
	CTaskHandler m_TaskHandler;
	
	CMessageParser m_MessageParser;
};

class CGetStatus: public CTask
{
public:
	
	CGetStatus( SmartPtr< CSocket > pSocket ):CTask( pSocket ){};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
};

class CStopScan: public CTask
{
public:
	
	CStopScan( SmartPtr< CSocket > pSocket ):CTask( pSocket ){};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
};

class CGetData: public CTask
{
public:
	
	CGetData( SmartPtr< CSocket > pSocket ):CTask( pSocket ){};
	
	virtual bool Immidiate();
	
	virtual void Execute(){};
};

class CStartScan: public CTask
{
public:
	
	CStartScan( SmartPtr< CSocket > pSocket, std::vector< std::string > vecAddresses ):CTask( pSocket )
																					  ,m_vecAddresses( vecAddresses )
	{};
	
	virtual bool Immidiate();
	
	virtual void Execute();
	
private:

	std::vector< std::string > m_vecAddresses;
	
	//Контейнер плагинов
	Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//Тип итератор для манипуляций с контейнером плагинов
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
};
#endif
