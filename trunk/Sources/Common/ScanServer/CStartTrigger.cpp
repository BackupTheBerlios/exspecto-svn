//-------------------------------------------------------------------------------------//
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: 
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "CStartTrigger.h"

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
													 ,m_Thread( SmartPtr<CThreadTask>( new CTimerThreadTask( this ) ), false )
{
	Log::instance().Trace( 90, "CTimer: " );
	Settings::instance().GetParam( TIMER_VALUE, m_iTimerValue );
	Log::instance().Trace( 90, "CTimer: : %d", m_iTimerValue );
}

CTimer::~CTimer()
{
	Log::instance().Trace( 90, "CTimer: " );
	Stop();
}

void CTimer::Start()
{
	Stop();
	Log::instance().Trace( 90, "CTimer: " );
	m_Thread.Start();
}

void CTimer::Stop()
{
	Log::instance().Trace( 90, "CTimer: " );
	if( m_Thread.IsWorking() )
	  m_CancelEv.Set();
}

void CTimer::CTimerThreadTask::Execute( CEvent& CancelEv )
{
	//TODO: try catch gcc
	try{
		m_pTimer->m_pCallBack->OnStartScan();
		for(;;)
		{
		  if( m_pTimer->m_CancelEv.Wait( m_pTimer->m_iTimerValue*1000 ) )
			break;
		  Log::instance().Trace( 95, "CTimer: " );
		  m_pTimer->m_pCallBack->OnStartScan();
		}
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "CTimer::fnTimerProc: : %s" , e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10,"CTimer::fnTimerProc: " );
	}
	Log::instance().Trace( 10,"CTimer::fnTimerProc: " );
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
