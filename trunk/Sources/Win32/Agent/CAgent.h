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
	CAgent();
	
	~CAgent(void);

private:

	//��������� ������� ����������� �� ��������� ��������( GET_STATUS, GET_DATA ... )
	//����� ���������� �������� ����� ������ ���� ������, � ������ ����������� ������ ��������� �������
	//������� ��� ����������, ���������� ��������� �������
	enumCommands m_LastCommand;
	
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
		std::auto_ptr< CSocket > client_sock;
		HANDLE	hCancelEvent;
	};

	//������� ������ ���������� �������
	HANDLE m_hCancelEvent;
	
	//����� ��������� �������� ���������
	static DWORD WINAPI fnProcessThreadProc( LPVOID pParameter );

	//����� �������� �������� ����������
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

	//��������� ��������
	Container< CScanner*, PluginLoadStrategy > m_PluginContainer;
	
	//��� �������� ��� ����������� � ����������� ��������
	typedef Container< CScanner*, PluginLoadStrategy >::iterator PluginIterator;
	
	std::map< int, void(CAgent::*)( CPacket&, CSocket* pSchedSocket, HANDLE hCancelEvent )> m_mapHandlers;
	
	//����������� ������
	void GetStatus( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void StartScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void GetData( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
	
	void StopScan( CPacket& Msg, CSocket* pSchedSocket, HANDLE hCancelEvent );
};

#endif
