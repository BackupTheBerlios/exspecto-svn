//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции Агента
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "precomp.h"
#include "..\libNet\packet.h"
#include "..\libCommon\Container.hpp"
#include "CScanner.h"
#include "PluginLoadStrategy.h"

class CAgent
{
public:
	
	//Конструктор,strSchedulerAddress - адрес планировщика
	CAgent( std::string strSchedulerAddress );
	
	~CAgent(void);

private:
	
	//Критическая секция на запись переменной текущего состояния
	CRITICAL_SECTION m_csCurState;

	//Критическая секция на выполнение команд
	CRITICAL_SECTION m_csCommandExec;

	//Текущее состояние агента
	enumAgentState m_CurState;

	//Адрес планировщика
	std::string m_strSchedulerAddress;

	//Структура, описывающая параметры,передаваемые функции fnProcessThreadProc
	struct ProcessParam{
		CAgent*	pThis;
		BYTE*	pbBuf;
		int		iCount;
		CSocket*	client_sock;
	};

	//Поток обработки входящих сообщений
	static DWORD WINAPI fnProcessThreadProc( LPVOID pParameter );

	//Поток ожидания входящих соединений
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

	//Контейнер плагинов
	Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//Тип итератор для манипуляций с контейнером плагинов
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
	std::map< int, enumAgentResponse(CAgent::*)( CPacket&, CSocket* pSchedSocket )> m_mapHandlers;
	
	//Обработчики команд
	enumAgentResponse GetStatus( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse StartScan( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse GetData( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse StopScan( CPacket& Msg, CSocket* pSchedSocket );
};

#endif
