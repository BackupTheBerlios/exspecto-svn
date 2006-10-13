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
{
	//TODO:доставать значение таймера из параметров
	m_ulTimerValue = 5; //5 сек
	m_hCancelEvent = CreateEvent( 0, 0, 0, NULL ); 
}

CTimer::~CTimer()
{
	Stop();
	CloseHandle( m_hCancelEvent );
	CloseHandle( m_hThread );
}
	
void CTimer::Start()
{
	if( WAIT_TIMEOUT == ::WaitForSingleObject( m_hThread, 0 ) )
		Stop();
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &fnTimerProc, this, 0, NULL );
}
	
void CTimer::Stop()
{
	SetEvent( m_hCancelEvent );
	::WaitForSingleObject( m_hThread, INFINITE );
}

unsigned __stdcall CTimer::fnTimerProc( void* pParam )
{
	CTimer* pThis = static_cast<CTimer*>( pParam );
	for(;;)
	{
		if( WAIT_OBJECT_0 == ::WaitForSingleObject( pThis->m_hCancelEvent, pThis->m_ulTimerValue*1000 ) )
			break;
		pThis->m_pCallBack->OnStartScan();
	}
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
