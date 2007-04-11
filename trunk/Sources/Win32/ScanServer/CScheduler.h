//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_
#include "ServerSocket.h"
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "Container.hpp"
#include "AgentsLoadStrategy.h"
#include "SmartPtr.hpp"
#include "Event.hpp"
#include "ThreadsPool.h"



//Предварительное обьявление класса CStartTrigger
class CStartTrigger;

//Класс, описывающий интерфейс для функции обратного вызова OnStartScanEvent,
//вызываемой в CScheduler при срабатывании триггера CStartTrigger
class CStartScanEventInterface
{
public:

	virtual void OnStartScan() = 0;
};

class CScheduler: public CStartScanEventInterface 
{
public:
	CScheduler(void);
	virtual ~CScheduler(void);

	//Метод вызывается триггером при срабатывании
	virtual void OnStartScan();

private:
	CScheduler( const CScheduler& );
	CScheduler& operator=( const CScheduler& );

	//Контейнер, содержащий обьекты управления агентами, загружаемые стратегией AgentsLoadStrategy
	std::map< std::string, SmartPtr< CAgentHandler > > m_mapAgentsContainer;
	
	//Триггер, срабатывающий при необходимости начать сканирование
	std::auto_ptr< CStartTrigger > m_pTrigger;

	static unsigned _stdcall fnListenThreadProc(  void* pParameter );

	CEvent m_CloseEv;
	
	HANDLE m_hListenThread;
	
	CServerSocket m_EventSock;

};

#endif //CSCHEDULER_H_
