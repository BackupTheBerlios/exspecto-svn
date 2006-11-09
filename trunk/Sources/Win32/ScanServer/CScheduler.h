//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//

#ifndef CSCHEDULER_H_
#define CSCHEDULER_H_

#include "precomp.h"
#include "CAgentHandler.h"
#include "CStartTrigger.h"
#include "Container.hpp"
#include "AgentsLoadStrategy.h"

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

	//Контейнер, содержащий обьекты управления агентами, загружаемые стратегией AgentsLoadStrategy
	Container< CAgentHandler*, AgentsLoadStrategy > m_AgentsContainer;
	
	//Триггер, срабатывающий при необходимости начать сканирование
	std::auto_ptr< CStartTrigger > m_pTrigger;

};

#endif //CSCHEDULER_H_
