//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Memory leak detection routines
//Author: Parshin Dmitry
//Description: ��������, ����������� ��� ������ ������ ������, ����������
//TODO:� ������ ������ ��� �������� ���������� �� ������� ������������ ����������
//	   ����������-������� �������� �������.���������� ����������.
//-------------------------------------------------------------------------------------//
#ifndef NDEBUG

#include "MemLeakDetector.h"
#include "windows.h"

//���-�� ��������� ������
static int i = 0;
static int Sizes[10240];
//��������� �� ���������� ������
static void* pointers[10240];

//����� �������, � ������� ���������� ������
static char strFuncs[10240][255];
//����� ������, � ������� ���������� ������
static char strFiles[10240][255];
//������ �����,� ������� ���������� ������
static int Lines[10240];

//���������� ������ �� ����� ��� ���������� ����������
#define new	new

void erase_ptr( void** pArray, int iSize, void* pSrc )
{
	//��������� ��������� ��������
	for( int j = 0; j < iSize; j++ )
	{
		if( pArray[ j ] == pSrc )
			pArray[ j ] = 0;
	}
}

void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc )
{
	void* p = malloc( size );
	Sizes[ i ] = (int)size;
	strcpy( strFuncs[ i ], strFuncName );
	strcpy( strFiles[ i ], strFile );
	Lines[ i ] = iLine;
	pointers[ i++ ] = p;
	return p;
} 

void* operator new[]( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc )
{
	void* p = malloc( size );
	Sizes[ i ] = (int)size;
	strcpy( strFuncs[ i ], strFuncName );
	strcpy( strFiles[ i ], strFile );
	Lines[ i ] = iLine;
	pointers[ i++ ] = p;
	return p;	
}

void operator delete( void* address )throw()
{
	erase_ptr( pointers, i, address );
	free( address );
} 

void operator delete( void *address , size_t bytes)
{
	erase_ptr( pointers, i, address );
	free( address );
}

void operator delete[]( void* address )throw()
{
	erase_ptr( pointers, i, address );
	free( address );
}

void operator delete[]( void *address , size_t bytes )
{
	erase_ptr( pointers, i, address );
	free( address );
}

void DumpMemLeaks()
{
	for( int k = 0; k < i; k++ )
	{
		if( pointers[k] )
		{
			Log::instance().Trace( 0, "MemLeakDetector:�� ��������� �������: %p, ��������: %d", pointers[k], Sizes[k] );
			Log::instance().Trace( 0, "MemLeakDetector:������� � %s:%d:%s()", strFiles[k], Lines[k], strFuncs[k] );
		} 
	}
}

#endif
