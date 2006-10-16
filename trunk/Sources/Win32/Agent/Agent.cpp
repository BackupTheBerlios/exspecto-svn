#include "precomp.h"
#include "CAgent.h"

int main(int argc, _TCHAR* argv[])
{
	try
	{
		CAgent ag( "127.0.0.1" );
		Sleep( 1000000 );
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
	return 0;
}
