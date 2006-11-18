#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <windows.h>
#include "SettingsContainer.h"
#include "CLog.h"

//Инициализация статической переменной
Log* Log::m_pInstance = NULL;

Log::Log( const char* strModuleName )
{
	::InitializeCriticalSection( &m_cs );
	char str[255];
	
	if( NULL == strModuleName )
	{
		//Получаем имя файла текущего процесса и составляем ищ него имя файла журнала
		GetModuleFileName( NULL, str, sizeof(str) );
	
		m_strFileName = str;
		
		int iPointPos = m_strFileName.find_first_of( '.' );
		int iSlashPos = m_strFileName.find_last_of( '\\' ) + 1;
		m_strFileName = m_strFileName.substr( iSlashPos, iPointPos - iSlashPos );
	}else
		m_strFileName = strModuleName;	

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
	if( iLevel > m_iLogLevel ) return;
	
	SYSTEMTIME st;
	FILE* fp;

   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");

	::EnterCriticalSection( &m_cs );
	fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d.%03d:%d	", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, iLevel );
	
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
	if( iLevel > m_iLogLevel ) return;
	
	SYSTEMTIME st;
	FILE* fp;

   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");
	
	::EnterCriticalSection( &m_cs );
	fprintf(fp, "%02d.%02d.%02d %02d:%02d:%02d.%03d:%d	", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, iLevel );
    
	va_list args;
	va_start(args, strAbout);
	
	vfprintf(fp, strAbout, args);
	putc('\n', fp);
						
	va_end(args);

	//Запись дампа
	char str[17];
	BYTE *p;
	int k = 1;
	fprintf(fp, "\t\t\t\t");
	for ( p = pbDumpData; p < (pbDumpData + iDataSize); p++ )
	{
		if ( *p < 32 || *p == 127 ) str[k-1] = '.';
		else	str[k-1] = *p;
		if ( k == 16 ) { str[k] = '\0'; fprintf(fp, "%02X\t| %s\n\t\t\t\t", *p, str); k = 0; }
		else	if ( k == 8 ) fprintf(fp, "%02X|", *p);
				else fprintf(fp, "%02X ", *p);
		k++;
	}
	
	k--;
	
	if (k)
	{
		int i;
		for ( i = 0; i <= (16-k)*3/8; i++ ) fprintf(fp, "\t");
		str[k] = '\0';
		fprintf(fp, "| %s\n", str);
	}
	else putc('\n', fp);
	
	::LeaveCriticalSection( &m_cs );
		
	fclose(fp);	
}

void Log::SetLoglevel( int iLoglevel )
{
	m_iLogLevel = iLoglevel;
}
