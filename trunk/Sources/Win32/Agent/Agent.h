//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CAgent class
//Author: Parshin Dmitry
//Description: �����, ����������� ������� ������
//-------------------------------------------------------------------------------------//
#pragma once
#include "windows.h"
#include "..\libNet\packet.h"
#include "CScanner.h"
#include "..\libCommon\Container.hpp"
#include "PluginLoadStrategy.h"
#include <map>

class CAgent
{
public:
	
	//�����������,strSchedulerAddress - ����� ������������
	CAgent( std::string strSchedulerAddress );
	
	~CAgent(void);

protected:
	
	//����������� ������ �� ������ ���������� �������� ���������
	CRITICAL_SECTION m_csCurState;

	//����������� ������ �� ���������� ������
	CRITICAL_SECTION m_csCommandExec;

	//���, ����������� ��������� ��������� ������
	enum enumStates{
		Idling = 0x1,
		Scanning = 0x2,
		SendingData = 0x3,
	};

	//������� ��������� ������
	enumStates m_CurState;

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
};
