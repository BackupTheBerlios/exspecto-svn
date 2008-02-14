#include <unistd.h>
#include <CScheduler.h>

int main( int argc, char** argv )
{
  daemon( 1, 0 );
  try
	{
	  if( 0 != daemon( 1, 0 ) )
		std::cout << "Init error" << std::endl;
	  CScheduler Server;
	  if( !Server.IsStarted() )
        return 1;

	  while(1)
		Sleep(1);
	}
  catch( std::exception& e )
	{
	  std::cout << "Exception catched: %s" << e.what() << std::endl;
	}
  catch( ... )
	{
	  std::cout << "Unknown exception catched" << std::endl;
	}


  return 0;
}
