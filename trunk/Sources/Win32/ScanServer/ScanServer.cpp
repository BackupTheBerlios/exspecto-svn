#include "precomp.h"
#include "CScheduler.h"
#include "CLog.h"
#include "conio.h"

//Описание типов параметров
static char* pServerParamTypes[] = {
	TIMER_VALUE, "int",
	LOG_LEVEL,	"int"
};

int main(int argc, _TCHAR* argv[])
{
	
	try
	{
		int iLogLevel;
		Settings::SetModule( "ScanServer", pServerParamTypes, sizeof( pServerParamTypes )/sizeof( pServerParamTypes[0] ) );
		Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
		Log::instance().SetLoglevel( iLogLevel );
		CScheduler* shed = new CScheduler;
		getch();
		delete shed;
		DumpMemLeaks();
		return 0;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0,"Возникло исключение: %s", e.what() );
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
}
