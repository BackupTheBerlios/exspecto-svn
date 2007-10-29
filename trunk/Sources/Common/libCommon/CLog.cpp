#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "CLog.h"
#include "SettingsContainer.h"
#ifdef WIN32
#include <windows.h>
#endif
#include "Os_Spec.h"
#include "time.h"
#include <sstream>

CLog::CLog():m_iLogLevel(100)
{
	//Получаем имя файла текущего процесса и составляем из него имя файла журнала
	m_strFileName = get_basepath();

	int iPointPos = (int)m_strFileName.find_first_of( '.' );
	int iSlashPos = (int)m_strFileName.find_last_of( '\\' ) + 1;
	m_strFileName = m_strFileName.substr( iSlashPos, iPointPos - iSlashPos );
	time_t tTime;
	time( &tTime );
	struct tm* lt = localtime( &tTime );
	std::stringstream ss;
	ss << lt->tm_mday << lt->tm_mon << lt->tm_year << "_" << lt->tm_hour << lt->tm_min << lt->tm_sec << ".log";
	m_strFileName += ss.str();
}

CLog::~CLog()
{
}

void CLog::SetModuleName( const std::string& strModuleName )
{
	m_strFileName = strModuleName;

	time_t tTime;
	time( &tTime );
	struct tm* lt = localtime( &tTime );
	std::stringstream ss;
	ss << lt->tm_mday << lt->tm_mon << lt->tm_year << "_" << lt->tm_hour << lt->tm_min << lt->tm_sec << ".log";
	m_strFileName += ss.str();
}

void CLog::Trace(int iLevel, char* trace_text, ...)
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > m_iLogLevel ) return;

	FILE* fp;

	m_mutex.Lock();

	fp = fopen( m_strFileName.c_str(), "a+");

	time_t tTime;
	time( &tTime );
	struct tm* lt = localtime( &tTime );
	fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d-%d	", lt->tm_mday, lt->tm_mon, lt->tm_year, lt->tm_hour, lt->tm_min, lt->tm_sec, iLevel );
	va_list args;
	va_start(args, trace_text);

	vfprintf(fp, trace_text, args);
	putc('\n', fp);
	putc('\n', fp);

	va_end(args);
	fclose(fp);
	m_mutex.Unlock();
}

void CLog::Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* strAbout, ... )
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > m_iLogLevel ) return;

	FILE* fp;

	m_mutex.Lock();
	fp = fopen( m_strFileName.c_str(), "a+");

	time_t tTime;
	time( &tTime );
	struct tm* lt = localtime( &tTime );
	fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d-%d	", lt->tm_mday, lt->tm_mon, lt->tm_year, lt->tm_hour, lt->tm_min, lt->tm_sec, iLevel );

	va_list args;
	va_start(args, strAbout);

	vfprintf(fp, strAbout, args);
	putc('\n', fp);
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
		int i=0;
		while ((k*3+i*8) < 48)	{ i++; fprintf(fp, "\t");}
		str[k] = '\0';
		fprintf(fp, "| %s\n", str);
	}
	else putc('\n', fp);

	m_mutex.Unlock();

	fclose(fp);
}

void CLog::SetLoglevel( int iLoglevel )
{
	m_iLogLevel = iLoglevel;
}
