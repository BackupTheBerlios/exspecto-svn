#include "windows.h"

typedef void (*StoreDataFunc)( const char* strAddress, const char* strFileName, __int64 FileSize, DWORD lFileTime, DWORD hFileTime );

typedef bool (*ScanFunc)( const char* strAddress, StoreDataFunc pStoreFunc, HANDLE hCancelEvent );

typedef const char* (*GetProtoNameFunc)();
