//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: Класс, реализующий функции триггера для планировщика CScheduler
//-------------------------------------------------------------------------------------//
#ifndef CSTARTTRIGGER_H_
#define CSTARTTRIGGER_H_

#include "precomp.h"
#include "CScheduler.h"


//Предварительное обьявление класса CStartScanEventInterface
class CStartScanEventInterface;

//Класс, описывающий интерфейс триггера для планировщика CScheduler
class CStartTrigger
{
public:
	//Триггер создается с указателем на интерфейс обратного вызова,
	//которому будет передаваться событие
	CStartTrigger( CStartScanEventInterface* pCallBack );
	virtual ~CStartTrigger();
	
	//Начало работы триггера
	virtual void Start() = 0;
	
	//Останов и сброс триггера
	virtual void Stop() = 0;
	
protected:
	
	CStartScanEventInterface* m_pCallBack;
	
};

//Класс,реализуюший триггер, срабатывающий периодически по таймеру
//Значение периода таймера берется из установок
class CTimer: public CStartTrigger
{
public:

	CTimer( CStartScanEventInterface* pCallBack );
	~CTimer();
	
	//Запустить таймер
	void Start();
	
	//Остановить и сбросить таймер
	void Stop();

private:
	
	//Дескриптор потока таймера
	HANDLE m_hThread;
	
	//Дескриптор события, останавливающего поток таймера
	HANDLE m_hCancelEvent;
	
	//Значение периода таймера
	int m_iTimerValue;
	
	//Поток таймера
	static unsigned __stdcall fnTimerProc( void* pParam );
 
};

//Класс,реализуюший триггер, срабатывающий по событию от внешней программы 
class CCron: public CStartTrigger
{
public:

	CCron( CStartScanEventInterface* pCallBack );
	~CCron();
	
	void Start();
	
	void Stop();
};

//Класс,реализуюший триггер, срабатывающий по расписанию
class CCalendar: public CStartTrigger
{
public:

	CCalendar( CStartScanEventInterface* pCallBack );
	~CCalendar();
	
	void Start();
	
	void Stop();
};

#endif /*CSTARTTRIGGER_H_*/
