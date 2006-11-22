#include "windows.h"
#include "MemLeakDetector.h"
#include "CNetBiosScanner.h"
#include "SettingsContainer.h"

//Описание типов параметров
static char* pNetBiosParamTypes[] = {
	LOG_LEVEL,	"int"
};

CScanner* m_pScanner;

extern "C" 
{
	CScanner* GetScanner();
	void ReleaseScanner();
}

CScanner* GetScanner()
{
	//Инициализируем вспомогательные службы
	int iLogLevel;	
	Log::instance(MOD_NAME);
	Settings::SetModule( "NetBiosPlugin", pNetBiosParamTypes, sizeof( pNetBiosParamTypes )/sizeof( pNetBiosParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );
		
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
	DumpMemLeaks();
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( DLL_PROCESS_DETACH == ul_reason_for_call )
		DumpMemLeaks();
	
    return TRUE;
}

