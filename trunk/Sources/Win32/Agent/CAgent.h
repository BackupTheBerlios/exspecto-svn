//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#ifndef AGENT_H_
#define AGENT_H_

#include "precomp.h"
#include "packet.h"
#include "Container.hpp"
#include "CScanner.h"
#include "PluginLoadStrategy.h"
#include <set>


class CAgent
{
public:
	
	//�����������
	CAgent();
	
	~CAgent(void);

private:

	std::vector< std::string > m_vecData;
	
	//����������� ������ �� ������ ���������� �������� ���������
	CRITICAL_SECTION m_csCurState;

	//����������� ������ �� ���������� ������
	CRITICAL_SECTION m_csCommandExec;

	//����������� ������ �� ������ � m_vecCloseHandles
	CRITICAL_SECTION m_csCloseHandles;
	
	//����������� ������ �� ������ � m_setProcessThreads
	CRITICAL_SECTION m_csProcessThreads;

	//������� ��������� ������
	enumAgentState m_CurState;

	//����� ������������
	std::string m_strSchedulerAddress;

	//���������, ����������� ���������,������������ ������� fnProcessThreadProc
	struct ProcessParam{
		CAgent*	pThis;
		BYTE*	pbBuf;
		int		iCount;
		CSocket* client_sock;
		HANDLE	hCancelEvent;
	};

	//������� ������ ���������� �������
	HANDLE m_hCancelEvent;
	
	//���������� ������ �������� ����������
	HANDLE m_hListenThread;
		
	//��������� ������� ���������� ������� ��������� ������
	std::set< HANDLE > m_setProcessThreads;
	
	//������ ������������ ��� �������� 
	std::vector< HANDLE > m_vecCloseHandles;
	
	//����� ��������� �������� ���������
	static unsigned _stdcall fnProcessThreadProc( void* pParameter );

	//����� �������� �������� ����������
	static unsigned _stdcall fnListenThreadProc(  void* pParameter );

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
