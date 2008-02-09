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
	void  AddFiles(hostRec &aRec);
	bool  Search(const string& aText, map<string,bool> &aParams, hostRecords &Result);
	void  EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles=false);
	time_t GetRefDateHost(const string& aHostName, const string& aIPnum);
	char*  GetNamePlugin();
	bool  RefreshDB();
//  void __stdcall StartIndexing(map<string,bool> &aParams);
	int  GetProvError(string& mes);
private:
	CppSQLite3DB db;
	bool FAutoIndex;
	bool FIndexed;
	string FErrMsg;
	int FErrCode;
	bool  Find(const string& aText, map<string,bool> &aParams, list<int> &Result);
	void  AddWord(int aID, const string& aPath);
	void  AddWordInTable(int aID, list<string> &words, bool IsPath);
	void  SetAutoIndex(bool aVal);
	bool  IsAutoIndex();
	void  SetLastError(int aCode, const string& aMes);
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
