//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Memory leak detection routines
//Author: Parshin Dmitry
//Description: Операции, необходимые для поиска утечек памяти, реализация
//TODO:В данной версии для хранения информации об утечках используются глобальные
//	   переменные-массивы большого размера.Необходимо переделать.
//-------------------------------------------------------------------------------------//
#ifndef NDEBUG

#include "MemLeakDetector.h"
#include "windows.h"

//Кол-во выделений памяти
static int i = 0;
static int Sizes[10240];
//Указатели на выделенную память
static void* pointers[10240];

//Имена функций, в которых выделялась память
static char strFuncs[10240][255];
//Имена файлов, в которых выделялась память
static char strFiles[10240][255];
//Номера строк,в которых выделялась память
static int Lines[10240];

//возвращаем макрос на место для корректной реализации
#define new	new

void erase_ptr( void** pArray, int iSize, void* pSrc )
{
	//Удаленные указатели обнуляем
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
			Log::instance().Trace( 0, "MemLeakDetector:Не очищенный участок: %p, размером: %d", pointers[k], Sizes[k] );
			Log::instance().Trace( 0, "MemLeakDetector:Выделен в %s:%d:%s()", strFiles[k], Lines[k], strFuncs[k] );
		} 
	}
}

#endif
