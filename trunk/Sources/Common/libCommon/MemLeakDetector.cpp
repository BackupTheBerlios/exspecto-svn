œóòß//-------------------------------------------------------------------------------------//
//Module: Memory leak detection routines
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------//
#ifndef NDEBUG
/*
#include "MemLeakDetector.h"
#include "windows.h"

struct structLeakRec{
	int	iSize;
	std::string	strFuncName;
	std::string	strFileName;
	int	iLine;
};

std::map< void*, structLeakRec > mapMem;

#define new	new

void erase( void* pSrc )
{
	if( mapMem.find( pSrc ) != mapMem.end() )
		mapMem.erase( pSrc );
}

void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc )
{
	void* p = malloc( size );
	structLeakRec Rec;
	Rec.iSize = (int)size;
	Rec.strFuncName = strFuncName;
	Rec.strFileName = strFile;
	Rec.iLine = iLine;
	mapMem[p] = Rec;
	return p;
} 

void* operator new[]( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc )
{
	void* p = malloc( size );
	structLeakRec Rec;
	Rec.iSize = (int)size;
	Rec.strFuncName = strFuncName;
	Rec.strFileName = strFile;
	Rec.iLine = iLine;
	mapMem[p] = Rec;
	return p;	
}

void operator delete( void* address )throw()
{
	erase( address );
	free( address );
} 

void operator delete( void *address , size_t bytes)
{
	erase( address );
	free( address );
}

void operator delete[]( void* address )throw()
{
	erase( address );
	free( address );
}

void operator delete[]( void *address , size_t bytes )
{
	erase( address );
	free( address );
}

void DumpMemLeaks()
{
	for( std::map< void*, structLeakRec >::iterator It = mapMem.begin(); It != mapMem.end(); It++ )
	{
		Log::instance().Trace( 0, "MemLeakDetector: : %p, : %d", It->first, It->second.iSize );
		Log::instance().Trace( 0, "MemLeakDetector:  %s:%d:%s()", It->second.strFileName.c_str(), It->second.iLine, It->second.strFuncName.c_str() );
	}		
}
*/
void DumpMemLeaks(){};
#endif
