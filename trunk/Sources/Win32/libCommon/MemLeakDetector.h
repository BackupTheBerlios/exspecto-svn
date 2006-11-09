//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Memory leak detection routines
//Author: Parshin Dmitry
//Description: Операции, необходимые для поиска утечек памяти
//-------------------------------------------------------------------------------------//
#ifndef MEMLEAKDETECTOR_H_
#define MEMLEAKDETECTOR_H_

//Модуль будет использован только в отладочной версии программы
#ifndef NDEBUG

#include <new>
#include <iostream>



//Перегружаем оператор new в глобальном пространстве имен, дополняем его параметрами
//strFile - имя файла
//iLine - номер строки
//strFuncName - имя функции 
void* operator new( size_t size, const char* strFile, int iLine, const char* strFuncName )throw( std::bad_alloc );

//Перегружаем две версии оператора delete
void operator delete( void* address )throw();
void operator delete( void *address , size_t bytes );

//Вспомогательная функция, для удаления неиспользуемых указателей( к которым применили delete )
void erase( void** pArray, int iSize, void* pSrc );

//Этой функцией выводим все не удаленные участки памяти на момент вызова
void DumpMemLeaks();

//обьявляем макрос new с дополнительными параметрами, чтобы пользовательский код не изменялся
#define new new(__FILE__, __LINE__, __FUNCTION__ )

      
#endif
#endif /*MEMLEAKDETECTOR_H_*/
