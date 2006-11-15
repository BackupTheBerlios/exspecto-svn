#include "precomp.h"
#include "CAgent.h"
#include "conio.h"

int main(int argc, _TCHAR* argv[])
{
	try
	{
		CAgent* ag = new CAgent( "127.0.0.1" );
		getch();
		delete ag;
		DumpMemLeaks();
		return 0;
	}catch( std::exception& e)
	{
		Log::instance().Trace( 5, "Возникло исключение: %s", e.what() );
	}
}
