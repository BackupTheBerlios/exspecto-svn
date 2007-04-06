//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: Класс, реализующий функции триггера для планировщика CScheduler
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CStartTrigger.h"
#include "process.h"

//------------------------------------------------------------------------------------------------------
//-----------------------------------CStartTrigger Implementation---------------------------------------
//------------------------------------------------------------------------------------------------------
CStartTrigger::CStartTrigger( CStartScanEventInterface* pCallBack ):m_pCallBack( pCallBack )
{
}

CStartTrigger::~CStartTrigger()
{
}

//------------------------------------------------------------------------------------------------------
//-----------------------------------CTimer Implementation----------------------------------------------
//------------------------------------------------------------------------------------------------------
CTimer::CTimer( CStartScanEventInterface* pCallBack ):CStartTrigger( pCallBack )
													  ,m_hThread( INVALID_HANDLE_VALUE )
{
	Log::instance().Trace( 90, "CTimer: создание" );
	//TODO:доставать значение таймера из параметров
	Settings::instance().GetParam( TIMER_VALUE, m_iTimerValue );
	Log::instance().Trace( 90, "CTimer: период таймера: %d с", m_iTimerValue );
	m_hCancelEvent = CreateEvent( 0, 0, 0, NULL ); 
}

CTimer::~CTimer()
{
	Log::instance().Trace( 90, "CTimer: уничтожение" );
	//Останавливаем служебный поток перед уничтожением
	Stop();
	
	CloseHandle( m_hCancelEvent );
	CloseHandle( m_hThread );
}

//Запустить таймер	
void CTimer::Start()
{
	//Останавливаем таймер
	Stop();
	Log::instance().Trace( 90, "CTimer: старт" );
	//Запускаем служебный поток таймера
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &fnTimerProc, this, 0, NULL );
}

//Остановить и сбросить таймер	
void CTimer::Stop()
{
	Log::instance().Trace( 90, "CTimer: стоп" );
	//Если поток запущен
	if( INVALID_HANDLE_VALUE != m_hThread )
	{
		//то останавливаем его
		SetEvent( m_hCancelEvent );
		//ждем корректного завершения потока
		::WaitForSingleObject( m_hThread, 30000 );
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

//Поток таймера
unsigned __stdcall CTimer::fnTimerProc( void* pParam )
{
	CTimer* pThis = static_cast<CTimer*>( pParam );
	//TODO: Этот try catch ничего не ловит
	try{
		pThis->m_pCallBack->OnStartScan();
		for(;;)
		{
			//Ожидаем либо отмены(останов таймера) либо выхода таймаута, который задает период таймера
			if( WAIT_OBJECT_0 == ::WaitForSingleObject( pThis->m_hCancelEvent, pThis->m_iTimerValue*1000 ) )
				break;
			Log::instance().Trace( 95, "CTimer: событие таймера" );
			//Вызываем обработчик в планировщике и ждем его отработки
			pThis->m_pCallBack->OnStartScan();
			Log::instance().Trace( 10,"2" );
		}
	}catch( std::exception& e )
	{
		//TODO:
		Log::instance().Trace( 0, "CTimer::fnTimerProc: Возникло исключение: %s" , e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10,"CTimer::fnTimerProc: Возникло неизвестное исключение" );
	}
	Log::instance().Trace( 10,"CTimer::fnTimerProc: Закрытие" );
	return 0;
}

//------------------------------------------------------------------------------------------------------
//-----------------------------------CCron Implementation-----------------------------------------------
//------------------------------------------------------------------------------------------------------
CCron::CCron( CStartScanEventInterface* pCallBack ):CStartTrigger( pCallBack )
{
}

CCron::~CCron()
{
}
	
void CCron::Start()
{
}
	
void CCron::Stop()
{
}

//------------------------------------------------------------------------------------------------------
//----------------------------------CCalendar Implementation--------------------------------------------
//------------------------------------------------------------------------------------------------------
CCalendar::CCalendar( CStartScanEventInterface* pCallBack ):CStartTrigger( pCallBack )
{
}

CCalendar::~CCalendar()
{
}
	
void CCalendar::Start()
{
}
	
void CCalendar::Stop()
{
}
