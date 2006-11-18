//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CLog class
//Author: Bezborodov Dmitry
//Description: Класс, реализующий функции системы журналирования
//-------------------------------------------------------------------------------------//
#ifndef _CLOG_H
#define _CLOG_H
#include "windows.h"
#include <string>

/*
 * Класс Log является Singleton-ом( см. паттерн проектирования Singleton )
 * Гарантированно существует только один экземпляр этого класса, доступ к 
 * нему осуществляется через метод instance()
 * 
 * Пример.
 * 
 * в любом модуле
 * 
 * #include "CLog.h"
 * ...
 * Log::instance().Trace( 10, "LALALA" );
 * ...
 */
class Log
{
public:
	
	//Метод доступа к экземпляру класса Log
	//позволяет изменить имя модуля, из которого формируется имя файла
	//по умолчанию имя модуля выясняется с помощью системной функции
	//GetModuleFileName
	static Log& instance( const char* strModuleName = NULL )
	{
		//Если доступ осуществляется в первый раз - создать экземпляр
		if( NULL == Log::m_pInstance )
		//TODO: разобраться с удалением объекта, может воспользоваться atexit()
			Log::m_pInstance = new Log( strModuleName );
		return *Log::m_pInstance;
	}
	
	
	//Метод используется для записи форматированной записи в журнал
	//	iLevel - приоритет записи, все записи с приоритетом > установленного не записываются в журнал
	//	trace_text - строка, содержащая формат записи (аналогичный printf)
	//	... - аргументы форматирования
	void Trace(int iLevel, char* trace_text, ...);
	
	//Метод используется для записи форматированной записи в журнал, с добавлением
	//дампа участка памяти 
	//	iLevel - приоритет записи, все записи с приоритетом > установленного не записываются в журнал
	//	pbDumpData, iDataSize - указатель на уч-к памяти и его размер
	//	trace_text - строка, содержащая формат записи (аналогичный printf)
	//	... - аргументы форматирования
	void Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* about, ...);
	
	void SetLoglevel( int iLoglevel );
	
private:
	Log( const char* strModuleName = NULL );
	~Log();
	
	//имя файла лога
	std::string m_strFileName;
	
	//критическая секция на запись в файл
	CRITICAL_SECTION m_cs;
	
	static Log* m_pInstance;
	
	int m_iLogLevel;
};


#endif

