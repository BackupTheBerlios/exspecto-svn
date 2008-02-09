#ifndef CDBPROVIDER_H_
#define CDBPROVIDER_H_
/////////////////////////////////////////////////////////////////////////////
// Схема базы данных                                                       //
//                                                                         //
// TableHostes  таблица описывает найденые в сети компьютеры               //
//   IDhost        INTEGER PRIMARY KEY Индекс компьютера                   //
//   IPNum         CHAR(15)       IP компьютера                            //
//   HostName      TINYTEXT       Имя компьютера                           //
//   DateRef       INT            Время последненго обновления информации  //
// ----------------------------------------------------------------------- //
// TableFiles   описание файлов и привязка их к компьютеру                 //
//   IDfile        INTEGER PRIMARY KEY Индекс файла                        //
//   IDhost        INT            Индекс компьютера                        //
//   FileName      TINYTEXT       Имя файла                                //
//   FileSize      INT            Размер файла                             //
//   FileTimeCr    INT            Время создания файла                     //
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

#define RESULT_OK 0

using namespace std;

// функции преобразуют значения полей lFileTime и hFileTime в UTS или возвращают UTS, если он не равен 0
// ВНИМАНИЕ!!! если поле UTS не равно 0!!! то в базу будет внесено значение этого поля
typedef struct FileDateTag
{
	time_t UTS;
} fileDate;

typedef struct FileStrTag
{
	string FileName;
	long long FileSize;
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
// Добавляет список файлов в БД
// aRec  список содержит имена хостов и список файлов для каждого хоста
// при добавлении файлов по умолчанию индексация по словам не производится
// IsAutoIndex = false;
	virtual void AddFiles(hostRec &aRec)=0;

///////////////////////////////////////////////////////////////////////////////
// Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)
// Поиск фразы в БД
// aText   текст для поиска
// aParams список параметров поиска
// Result  список результатов
// если значение параметра IndexFind = false то поиск производит без поиска по
// словам. При этом введенная фраза ищется в строчке телеком.
// Если значение IsAutoIndex = false то производить поиск с учетом слов, нельзя.
	virtual bool Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)=0;

///////////////////////////////////////////////////////////////////////////////
// EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)
// Удаление записей из БД
// aHostName  Имя хоста
// aIPnum     IP номер
// aDate      удаляет записи старше указаной даты (дата обновления хоста меньше указаной)
// aOnlyFiles удаляет только файлы, записи из таблицы хостов не удаляются
// Можно передавать только по одному из параемтров, остальные поля занулять
	virtual void EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)=0;

///////////////////////////////////////////////////////////////////////////////
// GetRefDateHost(const string& aHostName, const string& aIPnum)
// Возвращает дату последненго обновления в формате time_t
// aHostName  Имя хоста
// aIPnum     IP номер
// Можно передавать один из параметров, но запись должна быть определена однозначно
// Если по заданным значениям найдено более одной записи, то возвращается время
// только первой записи
	virtual time_t GetRefDateHost(const string& aHostName, const string& aIPnum)=0;

///////////////////////////////////////////////////////////////////////////////
// GetNamePlugin()
// Возвращает имя класса
  virtual char* GetNamePlugin()=0;

///////////////////////////////////////////////////////////////////////////////
// RefreshDB()
// Оптимизация Базы данных, обновление списка слов и их индексов
// ПОКА НЕРАБОТАЕТ!!!
  virtual bool RefreshDB()=0;

///////////////////////////////////////////////////////////////////////////////
// GetProvError(string& mes)
// mes строковая переменя в которую возвращается текст ошибки
// функция возвращает код ошибки или RESULT_OK если ошибки нет
	virtual int GetProvError(string& mes)=0;

///////////////////////////////////////////////////////////////////////////////
// SetAutoIndex(bool aVal)
// aVal установить значения автоиндексирования
// Автоиндексирование указывает добавляются ли слова автоматически в таблицу слов
// при добавлении записи файла
//  virtual void __stdcall SetAutoIndex(bool aVal)=0;

///////////////////////////////////////////////////////////////////////////////
// IsAutoIndex()
// Возвращет значение автоиндексирования
//  virtual bool __stdcall IsAutoIndex()=0;

///////////////////////////////////////////////////////////////////////////////
// StartIndexing(map<string,bool> &aParams)
// aParams зарезервировано для передачи параметров
// Производит индексирования записей из таблицы файлов
// ВРЕМЕНО НЕ РАБОТАЕТ!!!!!!!!!
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
			iSize += 5; // на всякий случай :)
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
		int ErrorCode()
		{
			return codeError;
		}
		//-------------------------------------------------------------------------
};

#endif /*CDBPROVIDER_H_*/
