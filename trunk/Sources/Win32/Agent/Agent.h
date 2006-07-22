#pragma once
#include "windows.h"
#include "..\net.lib\packet.h"

class CAgent
{
public:
	CAgent( std::string strSchedulerAddress );
	~CAgent(void);

	void Process( BYTE* pBuf, int iSize, CSocket* sock );

protected:

	//Критическая секция на запись переменной текущего состояния
	CRITICAL_SECTION m_csCurState;

	enum enumStates{
		Idling = 0x1,
		Scanning = 0x2,
		SendingData = 0x3,
	};

	enumStates m_CurState;

	std::string m_strSchedulerAddress;

	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );
};
