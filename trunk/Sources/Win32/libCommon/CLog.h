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
#include "CriticalSection.hpp"


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
		static Log log( strModuleName );
		return log;
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
	CCriticalSection m_cs;
	
	static Log* m_pInstance;
	
	int m_iLogLevel;
};


#endif

