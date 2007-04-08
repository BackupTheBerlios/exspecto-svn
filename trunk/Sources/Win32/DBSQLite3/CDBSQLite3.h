//---------------------------------------------------------------------------

#ifndef CDBSQLite3H
#define CDBSQLite3H
//---------------------------------------------------------------------------
#include "CDBProvider.h"
#include "CppSQLite3.h"
#include "tools.h"
#include "SettingsContainer.h"

using namespace std;

class CDBSQLitProvider: public CDBProvider
{
public:
	CDBSQLitProvider();
	virtual ~CDBSQLitProvider();
	void __stdcall AddFiles(hostRecords &aRec);
	bool __stdcall Search(const string& aText, map<string,bool> &aParams, hostRecords &Result);
	void __stdcall EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false);
	time_t __stdcall GetRefDateHost(const string& aHostName, const string& aIPnum);
  char* __stdcall GetNamePlugin();
  void __stdcall SetAutoIndex(bool aVal);
  bool __stdcall IsAutoIndex();
  void __stdcall StartIndexing(map<string,bool> &aParams);

private:
	CppSQLite3DB db;
	bool FAutoIndex;
	bool __fastcall Find(const string& aText, map<string,bool> &aParams, list<int> &Result);
	void __fastcall AddWord(int aID, const string& aPath);
	void __fastcall AddWordInTable(int aID, list<string> &words, bool IsPath);
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
