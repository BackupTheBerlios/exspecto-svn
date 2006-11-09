#ifndef MEMLEAKDETECTOR_H_
#define MEMLEAKDETECTOR_H_

#include <new>
#include <iostream>

static int i = 0;
static void* pointers[10240];
static char strFuncs[10240][255];
static char strFiles[10240][255];
static int Lines[10240];

#ifndef NDEBUG

void erase( void** pArray, int iSize, void* pSrc );

void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc );

void operator delete( void* address )throw();

void operator delete( void *address , size_t bytes );

void Dump();

#define DEBUG_NEW new(__FILE__, __LINE__, __FUNCTION__ )
#define new DEBUG_NEW

      
#endif
#endif /*MEMLEAKDETECTOR_H_*/
