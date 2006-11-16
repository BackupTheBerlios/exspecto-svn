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

//Запись, содеражащая инцормацию о выделенной памяти
struct structLeakRec{
	//Размер выделенного блока
	int	iSize;
	//Имя функции, в которой выделен блок
	std::string	strFuncName;
	//Имя файла, в которой выделен блок
	std::string	strFileName;
	//Номер строки, в которой выделен блок
	int	iLine;
};

//Ассоциативный массив,в котором собержится информация о всех
//выделенных блоках памяти
std::map< void*, structLeakRec > mapMem;

//возвращаем макрос на место для корректной реализации
#define new	new

//Вспомогательная функция удаления неиспользуемых указателей из mapMem
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

//Этой функцией выводим все не удаленные участки памяти на момент вызова
void DumpMemLeaks()
{
	for( std::map< void*, structLeakRec >::iterator It = mapMem.begin(); It != mapMem.end(); It++ )
	{
		Log::instance().Trace( 0, "MemLeakDetector:Не очищенный участок: %p, размером: %d", It->first, It->second.iSize );
		Log::instance().Trace( 0, "MemLeakDetector:Выделен в %s:%d:%s()", It->second.strFileName.c_str(), It->second.iLine, It->second.strFuncName.c_str() );
	}		
}

#endif
