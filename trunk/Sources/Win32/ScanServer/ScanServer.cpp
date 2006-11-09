#include "memleakdetector.h"
#include "precomp.h"
#include "CScheduler.h"
#include "CLog.h"
#include "conio.h"


int main(int argc, _TCHAR* argv[])
{
	try
	{
		CScheduler shed;
	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}
	Dump();
	getch();
	return 0;
}
