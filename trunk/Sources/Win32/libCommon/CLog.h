//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CLog class
//Author: Bezborodov Dmitry
//Description: �����, ����������� ������� ������� ��������������
//-------------------------------------------------------------------------------------//
#ifndef _CLOG_H
#define _CLOG_H
#include "windows.h"
#include <string>

/*
 * ����� Log �������� Singleton-��( ��. ������� �������������� Singleton )
 * �������������� ���������� ������ ���� ��������� ����� ������, ������ � 
 * ���� �������������� ����� ����� instance()
 * 
 * ������.
 * 
 * � ����� ������
 * 
 * #include "CLog.h"
 * ...
 * Log::instance().Trace( 10, "LALALA" );
 * ...
 */
class Log
{
public:
	
	//����� ������� � ���������� ������ Log
	//��������� �������� ��� ������, �� �������� ����������� ��� �����
	//�� ��������� ��� ������ ���������� � ������� ��������� �������
	//GetModuleFileName
	static Log& instance( const char* strModuleName = NULL )
	{
		//���� ������ �������������� � ������ ��� - ������� ���������
		if( NULL == Log::m_pInstance )
		//TODO: ����������� � ��������� �������, ����� ��������������� atexit()
			Log::m_pInstance = new Log( strModuleName );
		return *Log::m_pInstance;
	}
	
	
	//����� ������������ ��� ������ ��������������� ������ � ������
	//	iLevel - ��������� ������, ��� ������ � ����������� > �������������� �� ������������ � ������
	//	trace_text - ������, ���������� ������ ������ (����������� printf)
	//	... - ��������� ��������������
	void Trace(int iLevel, char* trace_text, ...);
	
	//����� ������������ ��� ������ ��������������� ������ � ������, � �����������
	//����� ������� ������ 
	//	iLevel - ��������� ������, ��� ������ � ����������� > �������������� �� ������������ � ������
	//	pbDumpData, iDataSize - ��������� �� ��-� ������ � ��� ������
	//	trace_text - ������, ���������� ������ ������ (����������� printf)
	//	... - ��������� ��������������
	void Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* about, ...);
	
	void SetLoglevel( int iLoglevel );
	
private:
	Log( const char* strModuleName = NULL );
	~Log();
	
	//��� ����� ����
	std::string m_strFileName;
	
	//����������� ������ �� ������ � ����
	CRITICAL_SECTION m_cs;
	
	static Log* m_pInstance;
	
	int m_iLogLevel;
};


#endif

