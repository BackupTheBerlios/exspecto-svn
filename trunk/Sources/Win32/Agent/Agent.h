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
#include <map>

class CAgent
{
public:
	
	//�����������,strSchedulerAddress - ����� ������������
	CAgent( std::string strSchedulerAddress );
	
	~CAgent(void);

protected:

	//��������� m_vecScanners � m_mapLibraries,���������� ���-�� ��������� plugin-�� ������������
	int FillScannersList();
	//������� m_vecScanners � m_mapLibraries
	void ClearScannersList();
	
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
	
	//���� �������, ������������ � dll � plugin-��
	typedef CScanner* (*fnGetScannerFunc)();
	typedef void (*fnReleaseScannerFunc)();
	
	//������ ������� �������� ������� ������������ (����������� �� ��������)
	std::vector< CScanner* > m_vecScanners;
	
	typedef std::map< HINSTANCE , fnReleaseScannerFunc > LibrariesMapType;
	//������ ������������ �������� ��������� ��� ����������� �� ��������
	LibrariesMapType m_mapLibraries;
};
