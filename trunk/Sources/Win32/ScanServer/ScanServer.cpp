#include "precomp.h"
#include "CScheduler.h"

int main(int argc, _TCHAR* argv[])
{
	try
	{
		CScheduler shed;
		Sleep(20000);
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
	return 0;
}
