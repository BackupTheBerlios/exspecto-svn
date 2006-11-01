#include "precomp.h"
#include "CAgent.h"

int main(int argc, _TCHAR* argv[])
{
	try
	{
		CAgent ag( "127.0.0.1" );
		Sleep( 1000000 );
	}catch( std::exception& e)
	{
		Log::instance().Trace( 5, "Возникло исключение: %s", e.what() );
	}
	return 0;
}
