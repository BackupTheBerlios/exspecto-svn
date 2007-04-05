#ifndef CDBPROVIDER_H_
#define CDBPROVIDER_H_
/////////////////////////////////////////////////////////////////////////////
// Схема базы данных                                                       //
//                                                                         //
// TableHostes  таблица описывает найденые в сети компьютеры               //
//   IDhost        INTEGER PRIMARY KEY Индекс компьютера                   //
//   IPNum         CHAR(15)       IP компьютера                            //
//   HostName      TINYTEXT       Имя компьютера                           //
//   DateRef       DATETIME       Время последненго обновления информации  //
// ----------------------------------------------------------------------- //
// TableFiles   описание файлов и привязка их к компьютеру                 //
//   IDfile        INTEGER PRIMARY KEY Индекс файла                        //
//   IDhost        INT            Индекс компьютера                        //
//   FileName      TINYTEXT       Имя файла                                //
//   FileSize      INT            Размер файла                             //
//   FileTimeCr    DATETIME       Время создания файла                     //
// ----------------------------------------------------------------------- //
// TableWords   таблица всех найденых слов                                 //
// { Пути к файлам разбиваются на слова (идиомы) и заносятся в таблицу.  } //
// { Это сделано для ускорения поиска выражений.                         } //
//   IDword        INTEGER PRIMARY KEY Индекс слова                        //
//   Word          TINYTEXT       "Идиома" (слово)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   соответствие слов файлам и флаг принадлежности слова //
//   IDword        INT            Индекс слова                             //
//   IDfile        INT            Индекс файла                             //
//   IsPath        BOOL           "Идиома" - часть пути?                   //
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
 

using namespace std;

// функции преобразуют значения полей lFileTime и hFileTime в UTS или возвращают UTS, если он не равен 0
// ВНИМАНИЕ!!! если поле UTS не равно 0!!! то в базу будет внесено значение этого поля
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
	char IPNum[16];
	filesStr Files;
} hostRec;
typedef list<hostRec> hostRecords;

class CDBProvider
{
public:
	virtual ~CDBProvider(){};
	virtual void __stdcall AddFiles(hostRecords &aRec)=0;
	virtual bool __stdcall Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)=0;
	virtual void __stdcall EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)=0;
	virtual time_t __stdcall GetRefDateHost(const string& aHostName, const string& aIPnum)=0;
  virtual char* __stdcall GetNamePlugin()=0;
  virtual void __stdcall SetAutoIndex(bool aVal)=0;
  virtual bool __stdcall IsAutoIndex()=0;
};

class CPrvException
{
	char *strError;
	public:
		CPrvException(const char* aText, int aLine=0, const char* aFunct=NULL)
		{
			int iSize = strlen(aText)+1;
			int i=0;
			if( aLine != 0 ) iSize += 15;
			if( aFunct != NULL ) iSize += strlen(aFunct)+3;
			iSize += 5; // на всякий случай :)
			strError = new char[iSize];
			if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
			if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
			i += sprintf(strError + i, "%s ", aText);
			Log::instance().Trace( 5,"%s", strError );
		}
		//-------------------------------------------------------------------------
		CPrvException(std::exception& e, int aLine=0, const char* aFunct=NULL)
		{
			const char* tmp = e.what(); 
			int iSize = strlen(tmp)+1;
			int i=0;
			if( aLine != 0 ) iSize += 15;
			if( aFunct != NULL ) iSize += strlen(aFunct)+3;
			iSize += 5; // на всякий случай :)
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
};

#endif /*CDBPROVIDER_H_*/
