//---------------------------------------------------------------------------

#include <windows.h>
#include "CDBSQLite3.h"

void *thisPlugin = NULL; 

#ifdef __BCPLUSPLUS__ 
	#pragma argsused
#endif
/*
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
  return 1;
}
*/
//---------------------------------------------------------------------------


extern "C"void* RegisterPlugin()
{
  thisPlugin = new CDBSQLitProvider();
  return thisPlugin;
}

