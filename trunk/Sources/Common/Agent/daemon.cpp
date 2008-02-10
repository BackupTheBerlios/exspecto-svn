#include <unistd.h>
#include <CAgent.h>

int main( int argc, char** argv )
{
    CAgent agent;
    if( !agent.IsStarted() )
        return 1;
    daemon( 1, 0 );
    return 0;
}
