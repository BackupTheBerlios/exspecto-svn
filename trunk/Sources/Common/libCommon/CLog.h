//-------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: CLog class
//Author: Bezborodov Dmitry
//Description: �����, ����������� ������� ������� ��������������
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
class CLog
{
public:
	CLog();
	~CLog();

	//��������� �������� ��� ������, �� �������� ����������� ��� �����
	//�� ��������� ��� ������ ���������� � ������� ��������� �������
	//GetModuleFileName, ����� ����� �������� ��� dll ����� ��� ������ dll
	//����������� � ��������� ����
	void SetModuleName( const std::string& strModuleName );

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
	CLog( const CLog& );
	CLog& operator=( const CLog& );

	//��� ����� ����
	std::string m_strFileName;

	//����������� ������ �� ������ � ����
	CMutex m_mutex;

	int m_iLogLevel;
};

typedef CSingleton< CLog > Log;

#endif

