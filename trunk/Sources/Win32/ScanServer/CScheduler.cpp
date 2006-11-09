//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������������
//-------------------------------------------------------------------------------------//
#include "memleakdetector.h"
#include <iostream>
#include <tchar.h>
#include "CScheduler.h"
#include "windows.h"

CScheduler::CScheduler(void)
{
	Log::instance().Trace( 90, "CScheduler: ��������, �������� ������" );
	m_pTrigger = std::auto_ptr< CTimer>( new CTimer( this ) );
	m_pTrigger->Start();
}

CScheduler::~CScheduler(void){}

void CScheduler::OnStartScan()
{
	std::vector< std::string > vecAdr;
	vecAdr.push_back( "127.0.0.1" );
	for( Container< CAgentHandler*, AgentsLoadStrategy >::iterator It = m_AgentsContainer.begin(); It != m_AgentsContainer.end(); It++ )
	{
		(*It)->Open();
		if( (*It)->IsOpened() )
			Log::instance().Trace( 10, "CScheduler: ����� ������: %d", (*It)->BeginScan( vecAdr ) );
	}
}
