//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Agents load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� ������� � ����� Container
//� ������ ������ �� ������������,���������� �� �� �����
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "AgentsLoadStrategy.h"

AgentsLoadStrategy::AgentsLoadStrategy( std::vector< CAgentHandler* >& vecStorage )
{
	Log::instance().Trace( 90, "AgentsLoadStrategy: �������� �������" );
	//TODO:����������� �������� ������� ������� �� ����������
	vecStorage.push_back( new CAgentHandler( "127.0.0.1" ) );
	m_ReleseCont = vecStorage;
}

AgentsLoadStrategy::~AgentsLoadStrategy()
{
	Log::instance().Trace( 90, "AgentsLoadStrategy: �����������" );
	//������� ��������� ��� ��������������� ������� ���������� ��������
	for( std::vector< CAgentHandler* >::iterator It = m_ReleseCont.begin(); It != m_ReleseCont.end(); It++ )
		delete *It; 
}
