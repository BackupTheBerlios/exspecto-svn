//---------------------------------------------------------------------------

#ifndef CDBSQLite3H
#define CDBSQLite3H
//---------------------------------------------------------------------------
#include "CDBProvider.h"
#include "CppSQLite3.h"

using namespace std;

__declspec(dllexport) class CDBSQLitProvider: public CDBProvider
{
public:
	CDBSQLitProvider(const char* szFile);
	virtual ~CDBSQLitProvider();
	void __stdcall AddFiles(pFilesStr aFileList, const string& aHostName, const string& aIPnum);
	bool __stdcall Find(const string& aText, map<string,bool> &aParams, list<int> &Result);
	bool __stdcall Search(const string& aText, map<string,bool> &aParams, hostRecords &Result);
	void __stdcall AddWord(int aID, const string& aPath);
	void __stdcall AddWordInTable(int aID, list<string> &words, bool IsPath);
	void __stdcall EraseHost(const string& aHostName, const string& aIPnum, const fileDate& aDate, bool aOnlyFiles=false);
	void __stdcall EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false);
  char* __stdcall GetNamePlugin();

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

#endif
