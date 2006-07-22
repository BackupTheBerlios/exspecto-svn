#pragma once
#include "windows.h"
#include "..\net.lib\packet.h"

class CAgent
{
public:
	CAgent(void);
	~CAgent(void);

	void Process( BYTE* pBuf, int iSize );

protected:

	enum enumStates{
		Idling = 0x1,
		Scanning = 0x2,
		SendingData = 0x3,
		SendingStatus = 0x4
	};

	enumStates m_CurState;
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );
};
