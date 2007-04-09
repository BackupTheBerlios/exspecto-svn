#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include "SettingsContainer.h"
#include "CLog.h"
#include <windows.h>

CLog::CLog():m_iLogLevel(100)
{
	char str[255];
	//Получаем имя файла текущего процесса и составляем ищ него имя файла журнала
	GetModuleFileName( NULL, str, sizeof(str) );

	m_strFileName = str;

	int iPointPos = (int)m_strFileName.find_first_of( '.' );
	int iSlashPos = (int)m_strFileName.find_last_of( '\\' ) + 1;
	m_strFileName = m_strFileName.substr( iSlashPos, iPointPos - iSlashPos );
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf( str, "_%02d%02d%04d_%02d%02d%02d.log", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond );
	m_strFileName += str;
}

CLog::~CLog()
{
}

void CLog::SetModuleName( const std::string& strModuleName )
{
	char str[255];
	
	m_strFileName = strModuleName;	

	SYSTEMTIME st;
   	GetLocalTime(&st);
	sprintf( str, "_%02d%02d%04d_%02d%02d%02d.log", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond );
	m_strFileName += str;
}

void CLog::Trace(int iLevel, char* trace_text, ...)
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > m_iLogLevel ) return;

	SYSTEMTIME st;
	FILE* fp;

	m_cs.Enter();	

   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");

	va_list args;
	fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d.%03d:%d	", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, iLevel );
	
	va_start(args, trace_text);
	
	vfprintf(fp, trace_text, args);
	putc('\n', fp);
	putc('\n', fp);

	va_end(args);
	fclose(fp);
	m_cs.Leave();
}

void CLog::Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* strAbout, ... )
{
	//Если приоритет записи больше чем установленный - не выполняем никаких действий
	if( iLevel > m_iLogLevel ) return;
	
	SYSTEMTIME st;
	FILE* fp;

	m_cs.Enter();
   	GetLocalTime(&st);
	fp = fopen( m_strFileName.c_str(), "a+");
	
	fprintf(fp, "%02d.%02d.%02d %02d:%02d:%02d.%03d:%d	", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, iLevel );
    
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
		int i;
		for ( i = 0; i <= (16-k)*3/8; i++ ) fprintf(fp, "\t");
		str[k] = '\0';
		fprintf(fp, "| %s\n", str);
	}
	else putc('\n', fp);
	
	m_cs.Leave();
		
	fclose(fp);	
}

void CLog::SetLoglevel( int iLoglevel )
{
	m_iLogLevel = iLoglevel;
}
