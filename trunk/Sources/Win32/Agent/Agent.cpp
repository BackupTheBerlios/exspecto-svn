#include "precomp.h"
#include "CAgent.h"
#include "conio.h"

//Описание типов параметров
static char* pAgentParamTypes[] = {
	SCHEDULER_ADDRESS, "string",
	LOG_LEVEL,	"int"
};

int main(int argc, _TCHAR* argv[])
{
	try
	{
		int iLogLevel;
		Settings::SetModule( "Agent", pAgentParamTypes, sizeof( pAgentParamTypes )/sizeof( pAgentParamTypes[0] ) );
		Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
		Log::instance().SetLoglevel( iLogLevel );
		CAgent* ag = new CAgent();
		getch();
		delete ag;
		DumpMemLeaks();
	}catch( std::exception& e)
	{
		Log::instance().Trace( 5, "Возникло исключение: %s", e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 5, "Возникло исключение !!!" );
	}
	return 0;
	
}
