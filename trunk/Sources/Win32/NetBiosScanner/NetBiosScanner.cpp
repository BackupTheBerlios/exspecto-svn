#include "windows.h"
#include "CNetBiosScanner.h"

CScanner* m_pScanner;

extern "C" 
{
	CScanner* GetScanner();
	void ReleaseScanner();
}

CScanner* GetScanner()
{
	if( NULL != m_pScanner )
		return NULL;
	
	m_pScanner = new CNetBiosScanner();
	
	return m_pScanner;
}

void ReleaseScanner()
{
	if( NULL == m_pScanner )
		return;
	
	delete m_pScanner;
	m_pScanner = NULL;
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
