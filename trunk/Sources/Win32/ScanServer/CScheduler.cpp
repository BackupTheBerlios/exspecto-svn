//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CScheduler class
//Author: Parshin Dmitry
//Description: Класс, реализующий функции планировщика
//-------------------------------------------------------------------------------------//
#include <iostream>
#include <tchar.h>
#include "CScheduler.h"
#include "windows.h"

CScheduler::CScheduler(void)
{
	Log::instance().Trace( 90, "CScheduler: создание, стартуем таймер" );
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
		enumAgentState bStatus;
		(*It)->Open();
		if( (*It)->IsOpened() )
		{
			(*It)->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: Статус агента: %d", bStatus );
		}
		(*It)->Open();
		if( (*It)->IsOpened() )
		{
			Log::instance().Trace( 10, "CScheduler: Агент вернул: %d", (*It)->BeginScan( vecAdr ) );
		}
		(*It)->Open();
		if( (*It)->IsOpened() )
		{
			(*It)->GetStatus( bStatus );
			Log::instance().Trace( 10, "CScheduler: Статус агента: %d", bStatus );
		}
	}
}
