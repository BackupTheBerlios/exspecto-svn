#ifndef NDEBUG

#include "MemLeakDetector.h" 

int i;
void* pointers[10240];
char strFuncs[10240][255];
char strFiles[10240][255];
int Lines[10240];

#define new	new

void erase( void** pArray, int iSize, void* pSrc )
{
	for( int j = 0; j < iSize; j++ )
	{
		if( pArray[ j ] = pSrc )
			pArray[ j ] = 0;
	}
}

void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc )
{
	void* p = malloc( size );
	strcpy( strFuncs[ i ], strFuncName );
	strcpy( strFiles[ i ], strFile );
	Lines[ i ] = iLine;
	pointers[ i++ ] = p;
	return p;
} 

void operator delete( void* address )throw()
{
	erase( pointers, i, address );
	free( address );
} 

void operator delete( void *address , size_t bytes)
{
	erase( pointers, i, address );
	free( address );
}

void Dump()
{
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
}

#define DEBUG_NEW new(__FILE__, __LINE__, __FUNCTION__ )
#define new DEBUG_NEW

#endif