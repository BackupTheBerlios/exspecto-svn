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

typedef struct FileStrTag
{
	string FileName;
	int FileSize;
	DWORD lFileTime;
	DWORD hFileTime;
} fileStr;

typedef list<fileStr> filesStr;

/*typedef struct FilesStrTag
{
	fileStr fl;
	struct FilesStrTag *pNext;
}filesStr;*/

typedef filesStr* pFilesStr;

class CDBProvider
{
public:
//	CDBProvider(const char* szFile);
	CDBProvider();
	virtual ~CDBProvider();
//	virtual void AddFile(fileStr* fs, string aHostName, string aIPnum){}
	virtual void AddFiles(pFilesStr aFileList, string aHostName, string aIPnum){}
	virtual bool Find(string aText, map<string,bool> & aParams, list<int> & Result){return false;}
	virtual bool Search(string aText, map<string,bool> & aParams, list<int> & Result){return false;}
	virtual void AddWord(int aID, string aPath){}
	virtual void AddWordInTable(int aID, list<string> & words, bool IsPath){}

	void Split(string & text, string separators, list<string> & words);
	char* GetTimeStr(time_t iTime, char* strRes);
	void FormatSQLQuery(string aText, string & aResult);
	time_t ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime); 
private:
	CppSQLite3DB db;
//	char* aFileName;
};

class CDBSQLitProvider: public CDBProvider
{
public:
	CDBSQLitProvider(const char* szFile);
	virtual ~CDBSQLitProvider();
//	void AddFile(fileStr* fs, string aHostName, string aIPnum);
	void AddFiles(pFilesStr aFileList, string aHostName, string aIPnum);
	bool Find(string aText, map<string,bool> & aParams, list<int> & Result);
	bool Search(string aText, map<string,bool> & aParams, list<int> & Result);
	void AddWord(int aID, string aPath);
	void AddWordInTable(int aID, list<string> & words, bool IsPath);
	
//	void Split(string & text, string separators, list<string> & words);
//	char* GetTimeStr(time_t iTime, char* strRes);
//	void FormatSQLQuery(string aText, string & aResult);
private:
	CppSQLite3DB db;
};

class CExcerpts
{
	public:
		CExcerpts(int aCount);
		CExcerpts();
		~CExcerpts();
		void Transact(int aVal, bool aisFirst);
		void TransactEnd();
		void GetAsList(list<int> & Res);
		void GetAsString(string & Res);
	private:
		int Allocation;
		int Curr;
		int Length;
		int* Memory;
		int Find(int aVal);
};
#endif /*CDBPROVIDER_H_*/
