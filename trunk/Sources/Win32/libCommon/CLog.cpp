#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <windows.h>
#include "CLog.h"

//TODO: заменить после реализации хранилища параметров
int log_level = 100;

//Инициализация статической переменной
Log* Log::m_pInstance = NULL;

Log::Log()
{
	MessageBox( NULL, "asdas","",MB_OK);
	::InitializeCriticalSection( &m_cs );
	char str[255];
	
	//Получаем имя файла текущего процесса и составляем ищ него имя файла журнала
	GetModuleFileName( NULL, str, sizeof(str) );

	m_strFileName = str;
	
	int iPointPos = m_strFileName.find_first_of( '.' );
	int iSlashPos = m_strFileName.find_last_of( '\\' ) + 1;
	m_strFileName = m_strFileName.substr( iSlashPos, iPointPos - iSlashPos );
	

	SYSTEMTIME st;
   	GetLocalTime(&st);
	sprintf( str, "_%02d%02d%04d_%02d%02d%02d.log", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond );
	m_strFileName += str;
}

Log::~Log()
{
	::DeleteCriticalSection( &m_cs );
}

void Log::Trace(int iLevel, char* trace_text, ...)
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > log_level ) return;
	
	SYSTEMTIME st;
	FILE* fp;

   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");

	::EnterCriticalSection( &m_cs );
	fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d.%03d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	
	fprintf(fp, "%d ", iLevel);
	va_list args;
	va_start(args, trace_text);
	
	vfprintf(fp, trace_text, args);
	putc('\n', fp);
	::LeaveCriticalSection( &m_cs );
							
	va_end(args);
	fclose(fp);
}

void Log::Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* strAbout, ... )
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > log_level ) return;
	
	SYSTEMTIME st;
	FILE* fp;

   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");
	
	::EnterCriticalSection( &m_cs );
	fprintf(fp, "%02d.%02d.%02d %02d:%02d:%02d.%03d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
	va_list args;
	va_start(args, strAbout);
	
	vfprintf(fp, strAbout, args);
	putc('\n', fp);
						
	va_end(args);

	//Запись дампа
	BYTE *p;
	BYTE k = 1;
	for ( p = pbDumpData; p < (pbDumpData + iDataSize); ++p )
	{
		if ( k == 16 ) { fprintf(fp, "%X\n", *p); k = 0; }
		else	if ( k == 8 ) fprintf(fp, "%X|", *p);
				else fprintf(fp, "%X ", *p);
		k++;
	}
	fprintf(fp, "\n");
	::LeaveCriticalSection( &m_cs );	
	fclose(fp);	
}
