#include <unistd.h>
#include <CAgent.h>

int main( int argc, char** argv )
{
  try
	{
	  CAgent agent;
	  if( !agent.IsStarted() )
        return 1;
	  if( 0 != daemon( 1, 0 ) )
		std::cout << "Init error" << std::endl;
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
