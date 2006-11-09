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

//Кол-во выделений памяти
int i;
//Указатели на выделенную память
void* pointers[10240];
//Имена функций, в которых выделялась память
char strFuncs[10240][255];
//Имена файлов, в которых выделялась память
char strFiles[10240][255];
//Номера строк,в которых выделялась память
int Lines[10240];

//возвращаем макрос на место для корректной реализации
#define new	new

void erase( void** pArray, int iSize, void* pSrc )
{
	//Удаленные указатели обнуляем
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

void DumpMemLeaks()
{
	//TODO:Пока выводим на экран
	for( int k = 0; k < i; k++ )
	{
		if( pointers[k] )
		{
			printf( "%p\n", pointers[k] );
			printf( "%s\n", strFiles[k] );
			printf( "%d\n", Lines[k] );
			printf( "%s\n\n", strFuncs[k] );
		} 
	}
}

#endif
