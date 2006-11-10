#include "precomp.h"
#include "CScheduler.h"
#include "CLog.h"
#include "conio.h"


int main(int argc, _TCHAR* argv[])
{
	try
	{
		CScheduler* shed = new CScheduler;
		getch();
		delete shed;
		DumpMemLeaks();
		getch();
		return 0;
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
}
