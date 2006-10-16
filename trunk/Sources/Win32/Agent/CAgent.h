//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "precomp.h"
#include "..\libNet\packet.h"
#include "..\libCommon\Container.hpp"
#include "CScanner.h"
#include "PluginLoadStrategy.h"

class CAgent
{
public:
	
	//�����������,strSchedulerAddress - ����� ������������
	CAgent( std::string strSchedulerAddress );
	
	~CAgent(void);

private:
	
	//����������� ������ �� ������ ���������� �������� ���������
	CRITICAL_SECTION m_csCurState;

	//����������� ������ �� ���������� ������
	CRITICAL_SECTION m_csCommandExec;

	//������� ��������� ������
	enumAgentState m_CurState;

	//����� ������������
	std::string m_strSchedulerAddress;

	//���������, ����������� ���������,������������ ������� fnProcessThreadProc
	struct ProcessParam{
		CAgent*	pThis;
		BYTE*	pbBuf;
		int		iCount;
		CSocket*	client_sock;
	};

	//����� ��������� �������� ���������
	static DWORD WINAPI fnProcessThreadProc( LPVOID pParameter );

	//����� �������� �������� ����������
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

	//��������� ��������
	Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//��� �������� ��� ����������� � ����������� ��������
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
	std::map< int, enumAgentResponse(CAgent::*)( CPacket&, CSocket* pSchedSocket )> m_mapHandlers;
	
	//����������� ������
	enumAgentResponse GetStatus( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse StartScan( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse GetData( CPacket& Msg, CSocket* pSchedSocket );
	
	enumAgentResponse StopScan( CPacket& Msg, CSocket* pSchedSocket );
};

#endif
