#include "windows.h"

typedef void (*StoreDataFunc)( const char* strAddress, const char* strProtocolName, const char* strFileName, long long FileSize, int lFileTime, int hFileTime );

typedef bool (*ScanFunc)( const char* strAddress, StoreDataFunc pStoreFunc, HANDLE hCancelEvent );

typedef const char* (*GetProtoNameFunc)();
