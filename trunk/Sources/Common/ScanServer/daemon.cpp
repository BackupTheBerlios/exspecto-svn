#include <unistd.h>
#include <CScheduler.h>

int main( int argc, char** argv )
{
    CScheduler Server;
    if( !Server.IsStarted() )
        return 1;
    daemon( 1, 0 );
    return 0;
}
