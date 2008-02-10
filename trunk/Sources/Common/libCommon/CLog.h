//-------------------------------------------------------------------------------------//
//Module: CLog class
//Author: Bezborodov Dmitry
//Description:
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

class CLog
{
public:
	CLog();
	~CLog();

	void SetModuleName( const std::string& strModuleName );

	void Trace(int iLevel, char* trace_text, ...);

	void Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* about, ...);

	void SetLoglevel( int iLoglevel );

private:
	CLog( const CLog& );
	CLog& operator=( const CLog& );

	std::string m_strFileName;

	CMutex m_mutex;

	int m_iLogLevel;
};

typedef CSingleton< CLog > Log;

#endif

