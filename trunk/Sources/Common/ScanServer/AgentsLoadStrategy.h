//------------------------------------------------------------------------------------//
//Module: Agents load strategy
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------//

#ifndef AGENTSLOADSTRATEGY_H_
#define AGENTSLOADSTRATEGY_H_

#include "CAgentHandler.h"

class AgentsLoadStrategy
{
public:
	AgentsLoadStrategy( std::vector< CAgentHandler* >& vecStorage );
	virtual ~AgentsLoadStrategy();
private:
	AgentsLoadStrategy( const AgentsLoadStrategy& );
	AgentsLoadStrategy& operator=( const AgentsLoadStrategy& );

	std::vector< CAgentHandler* > m_ReleseCont;
};

#endif /*AGENTSLOADSTRATEGY_H_*/
