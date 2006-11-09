#include "memleakdetector.h"
#include "precomp.h"
#include "CScheduler.h"
#include "CLog.h"
#include "conio.h"

int main(int argc, _TCHAR* argv[])
{
//	try
	//{
	CScheduler* shed = new CScheduler();
	volatile int* j = new int();
	Sleep(2000);
/*	}catch(...)
	{
		MessageBox( NULL, "Some error occured", "Error:", MB_OK );
	}*/
	delete shed;
	for( int k = 0; k < i; k++ )
	{
		if( pointers[k] )
		{
			printf( "%p\n", pointers[k] );
			printf( "%s\n", strFiles[k] );
			printf( "%d\n", Lines[k] );
			printf( "%s\n", strFuncs[k] );
		} 
	}
	volatile char ch = getch();
	return 0;
}
