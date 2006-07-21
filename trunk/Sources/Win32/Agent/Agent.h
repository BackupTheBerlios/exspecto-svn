#pragma once

class CAgent
{
public:
	CAgent(void);
	~CAgent(void);

	void StateMachine();
protected:

	enum enumStates{
		Idling = 0x1,
		Scanning = 0x2,
		SendingData = 0x3,
		SendingStatus = 0x4
	};

	enumStates m_CurState;
	void Parse( BYTE* pBuf, int iSize );
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

};
