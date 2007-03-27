#ifndef CDBPROVIDER_H_
#define CDBPROVIDER_H_
//#include "..\CommonFiles\CppSQLite3.h"
#include "CppSQLite3.h"
#include "constants.h"
#include "CLog.h"
#include <string>
#include <list>
#include <map>
#include <windows.h>
//#include <sys\timeb.h>
#include <time.h>

//#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
 

using namespace std;

// функции преобразуют значения полей lFileTime и hFileTime в UTS или возвращают UTS, если он не равен 0
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
//	DWORD lFileTime;
//	DWORD hFileTime;
} fileStr;

typedef list<fileStr> filesStr;
typedef filesStr* pFilesStr;

typedef struct RecordTag
{
	string HostName;
	char IPName[16];
	filesStr Files;
} hostRec;
typedef list<hostRec> hostRecords;

/*typedef struct FilesStrTag
{
	fileStr fl;
	struct FilesStrTag *pNext;
}filesStr;*/


class CDBProvider
{
public:
//	CDBProvider(const char* szFile);
//	CDBProvider();
	virtual ~CDBProvider(){};
//	virtual void AddFile(fileStr* fs, string aHostName, string aIPnum){}
	virtual void AddFiles(pFilesStr aFileList, const string& aHostName, const string& aIPnum)=0;
	virtual bool Find(const string& aText, map<string,bool> &aParams, list<int> &Result)=0;
	virtual bool Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)=0;
	virtual void AddWord(int aID, const string& aPath)=0;
	virtual void AddWordInTable(int aID, list<string> &words, bool IsPath)=0;
	virtual void EraseHost(const string& aHostName, const string& aIPnum, const fileDate& aDate, bool aOnlyFiles=false)=0;
	virtual void EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false)=0; 

	void Split(string &text, string separators, list<string> &words);
//	char* GetTimeStr(time_t iTime, char* strRes);
	void FormatSQLQuery(const string& aText, string& aResult);
	time_t ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime); 
	time_t ConvertFileTimeToUTC(const fileDate& aDate); 
private:
//	CppSQLite3DB db;
//	char* aFileName;
};

class CDBSQLitProvider: public CDBProvider
{
public:
	CDBSQLitProvider(const char* szFile);
	virtual ~CDBSQLitProvider();
//	void AddFile(fileStr* fs, string aHostName, string aIPnum);
	void AddFiles(pFilesStr aFileList, const string& aHostName, const string& aIPnum);
	bool Find(const string& aText, map<string,bool> &aParams, list<int> &Result);
	bool Search(const string& aText, map<string,bool> &aParams, hostRecords &Result);
	void AddWord(int aID, const string& aPath);
	void AddWordInTable(int aID, list<string> &words, bool IsPath);
	void EraseHost(const string& aHostName, const string& aIPnum, const fileDate& aDate, bool aOnlyFiles=false);
	void EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false);
	
private:
	CppSQLite3DB db;
};

class CExcerpts
{
	public:
		CExcerpts(int aCount);
		CExcerpts();
		void Init(int aCount);
		virtual ~CExcerpts();
		void Transact(int aVal, bool aisFirst);
		void TransactEnd();
		void GetAsList(list<int> &Res);
		void GetAsString(string &Res);
		bool IsEmpty();
	private:
		int Allocation;
		int Curr;
		int Length;
		int* Memory;
		int Find(int aVal);
};

class CPrvException
{
	char *strError;
	public:
		CPrvException(const char* aText, int aLine=0, const char* aFunct=NULL);
		CPrvException(CppSQLite3Exception&  e, int aLine=0, const char* aFunct=NULL);
		CPrvException(std::exception& e, int aLine=0, const char* aFunct=NULL);
		virtual ~CPrvException();
		const char* Message();
};
#endif /*CDBPROVIDER_H_*/
