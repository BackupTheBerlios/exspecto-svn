//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agents load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки Агентов в класс Container
//-------------------------------------------------------------------------------------//
#include "AgentsLoadStrategy.h"

AgentsLoadStrategy::AgentsLoadStrategy( std::vector< CAgentHandler* >& vecStorage )
{
	//TODO:реализовать загрузку адресов агентов из параметров
	vecStorage.push_back( new CAgentHandler( "127.0.0.1" ) );
	m_ReleseCont = vecStorage;
	
	
}

AgentsLoadStrategy::~AgentsLoadStrategy()
{
	for( std::vector< CAgentHandler* >::iterator It = m_ReleseCont.begin(); It != m_ReleseCont.end(); It++ )
		delete *It; 
}
