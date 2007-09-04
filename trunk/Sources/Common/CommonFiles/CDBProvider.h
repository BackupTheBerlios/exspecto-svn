#ifndef CDBPROVIDER_H_
#define CDBPROVIDER_H_
/////////////////////////////////////////////////////////////////////////////
// ����� ���� ������                                                       //
//                                                                         //
// TableHostes  ������� ��������� �������� � ���� ����������               //
//   IDhost        INTEGER PRIMARY KEY ������ ����������                   //
//   IPNum         CHAR(15)       IP ����������                            //
//   HostName      TINYTEXT       ��� ����������                           //
//   DateRef       INT            ����� ����������� ���������� ����������  //
// ----------------------------------------------------------------------- //
// TableFiles   �������� ������ � �������� �� � ����������                 //
//   IDfile        INTEGER PRIMARY KEY ������ �����                        //
//   IDhost        INT            ������ ����������                        //
//   FileName      TINYTEXT       ��� �����                                //
//   FileSize      INT            ������ �����                             //
//   FileTimeCr    INT            ����� �������� �����                     //
// ----------------------------------------------------------------------- //
// TableWords   ������� ���� �������� ����                                 //
// { ���� � ������ ����������� �� ����� (������) � ��������� � �������.  } //
// { ��� ������� ��� ��������� ������ ���������.                         } //
//   IDword        INTEGER PRIMARY KEY ������ �����                        //
//   Word          TINYTEXT       "������" (�����)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   ������������ ���� ������ � ���� �������������� ����� //
//   IDword        INT            ������ �����                             //
//   IDfile        INT            ������ �����                             //
//   IsPath        BOOL           "������" - ����� ����?                   //
/////////////////////////////////////////////////////////////////////////////
 
//-----------------------------------------------------------------------------
#ifdef __BCPLUSPLUS__
  #define __FUNCTION__ __FUNC__
  #define _itoa itoa

  #include <stdio.h>
#endif

#include "constants.h"
#include "CLog.h"
#include <string>
#include <list>
#include <map>
#include <windows.h>
#include <time.h>

#include <malloc.h>
#include <stdlib.h>
 
#define RESULT_OK 0

using namespace std;

// ������� ����������� �������� ����� lFileTime � hFileTime � UTS ��� ���������� UTS, ���� �� �� ����� 0
// ��������!!! ���� ���� UTS �� ����� 0!!! �� � ���� ����� ������� �������� ����� ����
typedef struct FileDateTag 
{
	DWORD lFileTime;
	DWORD hFileTime;
	time_t UTS;
} fileDate;

typedef struct FileStrTag
{
	string FileName;
	__int64 FileSize;
	fileDate FDate;
} fileStr;
typedef list<fileStr> filesStr;

typedef struct RecordTag
{
	string HostName;
	string IPNum;
	filesStr Files;
} hostRec;
typedef list<hostRec> hostRecords;

class CDBProvider
{
public:
	virtual ~CDBProvider(){};

///////////////////////////////////////////////////////////////////////////////
// AddFiles(hostRecords &aRec)
// ��������� ������ ������ � ��
// aRec  ������ �������� ����� ������ � ������ ������ ��� ������� �����
// ��� ���������� ������ �� ��������� ���������� �� ������ �� ������������
// IsAutoIndex = false;	
	virtual void __stdcall AddFiles(hostRec &aRec)=0;

///////////////////////////////////////////////////////////////////////////////
// Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)
// ����� ����� � ��
// aText   ����� ��� ������
// aParams ������ ���������� ������
// Result  ������ �����������
// ���� �������� ��������� IndexFind = false �� ����� ���������� ��� ������ ��
// ������. ��� ���� ��������� ����� ������ � ������� �������.
// ���� �������� IsAutoIndex = false �� ����������� ����� � ������ ����, ������. 	
	virtual bool __stdcall Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)=0;

///////////////////////////////////////////////////////////////////////////////
// EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)
// �������� ������� �� ��
// aHostName  ��� �����
// aIPnum     IP �����
// aDate      ������� ������ ������ �������� ���� (���� ���������� ����� ������ ��������)
// aOnlyFiles ������� ������ �����, ������ �� ������� ������ �� ���������
// ����� ���������� ������ �� ������ �� ����������, ��������� ���� ��������
	virtual void __stdcall EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)=0;

///////////////////////////////////////////////////////////////////////////////
// GetRefDateHost(const string& aHostName, const string& aIPnum)
// ���������� ���� ����������� ���������� � ������� time_t
// aHostName  ��� �����
// aIPnum     IP �����
// ����� ���������� ���� �� ����������, �� ������ ������ ���� ���������� ����������
// ���� �� �������� ��������� ������� ����� ����� ������, �� ������������ �����
// ������ ������ ������
	virtual time_t __stdcall GetRefDateHost(const string& aHostName, const string& aIPnum)=0;

///////////////////////////////////////////////////////////////////////////////
// GetNamePlugin()
// ���������� ��� ������
  virtual char* __stdcall GetNamePlugin()=0;

///////////////////////////////////////////////////////////////////////////////
// RefreshDB()
// ����������� ���� ������, ���������� ������ ���� � �� ��������
// ���� ����������!!!
  virtual bool __stdcall RefreshDB()=0;

///////////////////////////////////////////////////////////////////////////////
// GetProvError(string& mes)
// mes ��������� �������� � ������� ������������ ����� ������
// ������� ���������� ��� ������ ��� RESULT_OK ���� ������ ���
	virtual int __stdcall GetProvError(string& mes)=0;

///////////////////////////////////////////////////////////////////////////////
// SetAutoIndex(bool aVal)
// aVal ���������� �������� ������������������
// ������������������ ��������� ����������� �� ����� ������������� � ������� ����
// ��� ���������� ������ �����
//  virtual void __stdcall SetAutoIndex(bool aVal)=0;

///////////////////////////////////////////////////////////////////////////////
// IsAutoIndex()
// ��������� �������� ������������������
//  virtual bool __stdcall IsAutoIndex()=0;
  
///////////////////////////////////////////////////////////////////////////////
// StartIndexing(map<string,bool> &aParams)
// aParams ��������������� ��� �������� ����������
// ���������� �������������� ������� �� ������� ������
// ������� �� ��������!!!!!!!!!
//  virtual void __stdcall StartIndexing(map<string,bool> &aParams)=0;
};

class CPrvException
{
	char *strError;
	int codeError;
	public:
		CPrvException(const char* aText, int aLine=0, const char* aFunct=NULL, const int aErrCode=-1)
		{
			codeError = aErrCode;
			int iSize = (int)strlen(aText)+1;
			int i=0;
			if( aLine != 0 ) iSize += 15;
			if( aFunct != NULL ) iSize += (int)strlen(aFunct)+3;
			iSize += 5; // �� ������ ������ :)
			strError = new char[iSize];
			if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
			if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
			i += sprintf(strError + i, "%s ", aText);
			Log::instance().Trace( 5,"%s", strError );
		}
		//-------------------------------------------------------------------------
		CPrvException(std::exception& e, int aLine=0, const char* aFunct=NULL, const int aErrCode=-1)
		{
			codeError = aErrCode;
			const char* tmp = e.what();
			int iSize = (int)strlen(tmp)+1;
			int i=0;
			if( aLine != 0 ) iSize += 15;
			if( aFunct != NULL ) iSize += (int)strlen(aFunct)+3;
			iSize += 5; // �� ������ ������ :)
			strError = new char[iSize];
			if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
			if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
			i += sprintf(strError + i, "%s ", tmp);
			Log::instance().Trace( 5,"%s", strError );
		}
		//-------------------------------------------------------------------------
		virtual ~CPrvException()
		{
			if( strError )
			{
				delete[] strError;
				strError = NULL;
			}
		}
		//-------------------------------------------------------------------------
		const char* Message()
		{
			return strError;
		}
		//-------------------------------------------------------------------------
		int ErrorCode()
		{
			return codeError;
		}
		//-------------------------------------------------------------------------
};

#endif /*CDBPROVIDER_H_*/
