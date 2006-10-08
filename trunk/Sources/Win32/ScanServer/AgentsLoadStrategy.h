//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Agents load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки Агентов в класс Container
//-------------------------------------------------------------------------------------//

#ifndef AGENTSLOADSTRATEGY_H_
#define AGENTSLOADSTRATEGY_H_

#include "CAgentHandler.h"

class AgentsLoadStrategy
{
public:
	AgentsLoadStrategy( std::vector< CAgentHandler* >& vecStorage );
	virtual ~AgentsLoadStrategy();
};

#endif /*AGENTSLOADSTRATEGY_H_*/
