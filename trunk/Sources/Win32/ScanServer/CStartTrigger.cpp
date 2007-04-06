//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CStartTrigger interface and CCron, CTimer, CCalendar implementations
//Author: Parshin Dmitry
//Description: �����, ����������� ������� �������� ��� ������������ CScheduler
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
	Log::instance().Trace( 90, "CTimer: ��������" );
	//TODO:��������� �������� ������� �� ����������
	Settings::instance().GetParam( TIMER_VALUE, m_iTimerValue );
	Log::instance().Trace( 90, "CTimer: ������ �������: %d �", m_iTimerValue );
	m_hCancelEvent = CreateEvent( 0, 0, 0, NULL ); 
}

CTimer::~CTimer()
{
	Log::instance().Trace( 90, "CTimer: �����������" );
	//������������� ��������� ����� ����� ������������
	Stop();
	
	CloseHandle( m_hCancelEvent );
	CloseHandle( m_hThread );
}

//��������� ������	
void CTimer::Start()
{
	//������������� ������
	Stop();
	Log::instance().Trace( 90, "CTimer: �����" );
	//��������� ��������� ����� �������
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, &fnTimerProc, this, 0, NULL );
}

//���������� � �������� ������	
void CTimer::Stop()
{
	Log::instance().Trace( 90, "CTimer: ����" );
	//���� ����� �������
	if( INVALID_HANDLE_VALUE != m_hThread )
	{
		//�� ������������� ���
		SetEvent( m_hCancelEvent );
		//���� ����������� ���������� ������
		::WaitForSingleObject( m_hThread, 30000 );
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

//����� �������
unsigned __stdcall CTimer::fnTimerProc( void* pParam )
{
	CTimer* pThis = static_cast<CTimer*>( pParam );
	//TODO: ���� try catch ������ �� �����
	try{
		pThis->m_pCallBack->OnStartScan();
		for(;;)
		{
			//������� ���� ������(������� �������) ���� ������ ��������, ������� ������ ������ �������
			if( WAIT_OBJECT_0 == ::WaitForSingleObject( pThis->m_hCancelEvent, pThis->m_iTimerValue*1000 ) )
				break;
			Log::instance().Trace( 95, "CTimer: ������� �������" );
			//�������� ���������� � ������������ � ���� ��� ���������
			pThis->m_pCallBack->OnStartScan();
			Log::instance().Trace( 10,"2" );
		}
	}catch( std::exception& e )
	{
		//TODO:
		Log::instance().Trace( 0, "CTimer::fnTimerProc: �������� ����������: %s" , e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 10,"CTimer::fnTimerProc: �������� ����������� ����������" );
	}
	Log::instance().Trace( 10,"CTimer::fnTimerProc: ��������" );
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
