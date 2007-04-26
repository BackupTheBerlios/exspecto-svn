#include "windows.h"
#include "constants.h"
#include "PluginInterface.h"

#define MOD_NAME "NetBios"

//Описание типов параметров
static char* pNetBiosParamTypes[] = {
	LOG_LEVEL,	"int"
};

#define DllExport __declspec( dllexport )

extern "C" 
{
	DllExport bool Scan( IN const char* strAddress, IN StoreDataFunc pStoreFunc,IN HANDLE hCancelEvent );
	DllExport const char* GetProtocolName();
}
DllExport const char* GetProtocolName();
DllExport bool Scan( IN const char* strAddress, IN StoreDataFunc pStoreFunc, IN HANDLE hCancelEvent );
