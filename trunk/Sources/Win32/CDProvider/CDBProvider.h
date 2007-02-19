#ifndef CDBPROVIDER_H_
#define CDBPROVIDER_H_
//#include "..\CommonFiles\CppSQLite3.h"
#include "CppSQLite3.h"
#include "constants.h"
#include "CLog.h"
#include <string>
#include <list>
#include <windows.h>
//#include <sys\timeb.h>
#include <time.h>

using namespace std;

typedef struct FileStrTag
{
	string FileName;
	int FileSize;
	int FileTime;
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
	CDBProvider(const char* szFile);
	virtual ~CDBProvider();
	void AddFile(fileStr* fs, const char* aHostName, const char* aIPnum);
	void AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum);
private:
	CppSQLite3DB db;
	char* aFileName;
	char* GetTimeStr(time_t iTime, char* strRes);
	void AddWord(int aID, string aPath);
	void Split(string & text, string & separators, list<string> & words);
	void AddWordInTable(int aID, list<string> & words, bool IsPath);
	
};

#endif /*CDBPROVIDER_H_*/
