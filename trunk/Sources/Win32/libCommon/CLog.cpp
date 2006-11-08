#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <windows.h>
#include "CLog.h"

//TODO: �������� ����� ���������� ��������� ����������
int log_level = 100;

//������������� ����������� ����������
Log* Log::m_pInstance = NULL;

Log::Log( const char* strModuleName )
{
	::InitializeCriticalSection( &m_cs );
	char str[255];
	
	if( NULL == strModuleName )
	{
		//�������� ��� ����� �������� �������� � ���������� �� ���� ��� ����� �������
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
	//���� ��������� ������ ������ ��� ������������� - �� ��������� ������� ��������
	if( iLevel > log_level ) return;
	
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
	//���� ��������� ������ ������ ��� ������������� - �� ��������� ������� ��������
	if( iLevel > log_level ) return;
	
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

	//������ �����
	char str[17];
	BYTE *p;
	int k = 1;
	for ( p = pbDumpData; p < (pbDumpData + iDataSize); p++ )
	{
		str[k-1] = *p;
		if ( k == 16 ) { str[k] = '\0'; fprintf(fp, "%X\t| %s\n", *p, str); k = 0; }
		else	if ( k == 8 ) fprintf(fp, "%X|", *p);
				else fprintf(fp, "%X ", *p);
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
