//-------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: CLog class
//Author: Bezborodov Dmitry
//Description: Класс, реализующий функции системы журналирования
//-------------------------------------------------------------------------------------//
#ifndef _CLOG_H
#define _CLOG_H
#include <string>
#include "Singleton.hpp"
#ifdef WIN32
#include "windows.h"
#else
#define BYTE unsigned char
#endif

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
class CLog
{
public:
	CLog();
	~CLog();

	//позволяет изменить имя модуля, из которого формируется имя файла
	//по умолчанию имя модуля выясняется с помощью системной функции
	//GetModuleFileName, имеет смысл вызывать для dll чтобы лог модуля dll
	//записывался в отдельный файл
	void SetModuleName( const std::string& strModuleName );

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
	CLog( const CLog& );
	CLog& operator=( const CLog& );

	//имя файла лога
	std::string m_strFileName;

	//критическая секция на запись в файл
	CMutex m_mutex;

	int m_iLogLevel;
};

typedef CSingleton< CLog > Log;

#endif

