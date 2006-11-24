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

class CAgent
{
public:
	
	//Конструктор,strSchedulerAddress - адрес планировщика
	CAgent();
	
	~CAgent(void);

private:

	std::vector< std::string > m_vecData;
	
	//Критическая секция на запись переменной текущего состояния
	CRITICAL_SECTION m_csCurState;

	//Критическая секция на выполнение команд
	CRITICAL_SECTION m_csCommandExec;

	//Критическая секция на доступ к m_vecCloseHandles
	CRITICAL_SECTION m_csCloseHandles;

	//Текущее состояние агента
	enumAgentState m_CurState;

	//Адрес планировщика
	std::string m_strSchedulerAddress;

	//Структура, описывающая параметры,передаваемые функции fnProcessThreadProc
	struct ProcessParam{
		CAgent*	pThis;
		BYTE*	pbBuf;
		int		iCount;
		std::auto_ptr< CSocket > client_sock;
		HANDLE	hCancelEvent;
	};

	//Событие отмены выполнения команды
	HANDLE m_hCancelEvent;
	
	//Множество хэндлов запущенных потоков обработки команд
	std::set< HANDLE > m_setProcessThreads;
	
	//Вектор дескрипторов для закрытия 
	std::vector< HANDLE > m_vecCloseHandles;
	
	//Поток обработки входящих сообщений
	static DWORD WINAPI fnProcessThreadProc( LPVOID pParameter );

	//Поток ожидания входящих соединений
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

	//Контейнер плагинов
	Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//Тип итератор для манипуляций с контейнером плагинов
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
	std::map< int, void(CAgent::*)( CPacket&, CSocket* pSchedSocket, HANDLE hCancelEvent )> m_mapHandlers;
	
	//Обработчики команд
	void GetStatus( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void StartScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void GetData( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void StopScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
};

#endif
