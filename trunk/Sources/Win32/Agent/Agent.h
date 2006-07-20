#pragma once

class CAgent
{
public:
	CAgent(void);
	~CAgent(void);

protected:

	void StateMachine();
	void Parse( BYTE* pBuf, int iSize );
	static DWORD WINAPI fnListenThreadProc(  LPVOID pParameter );

};
