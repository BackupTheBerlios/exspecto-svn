#include <string>
#include <windows.h>
#include <stdio.h>
//#include <dir.h>
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <memory.h>
//#include <string.h>
//#include "memleakdetector.h"
#include "CDBSQLite3.h"

/* #include <string>
#include <list>
#include <map>
#include <windows.h>
#include <time.h>

#include <malloc.h>
#include <stdlib.h>
//*/
#include "CppSQLite3.h"
#include "SQLiteWrapper.h"

using namespace std;

void ExtractFilePath(string & Dest, string Src)
{
	char tmp[400];
	GetFullPathName(Src.c_str(), 400, tmp, NULL);
	Dest = tmp;
	int iEnd = Dest.find_last_of("\\");
	Dest = Dest.substr(0, iEnd+1);
}

void FileInfo(string aPath, filesStr &FL)
{
    HANDLE hFindFile;
    WIN32_FIND_DATA hFindData;
    unsigned __int64 FSize = 0;
    bool IsFinished;
    string cPath;
		fileStr Cur;

    hFindFile = FindFirstFile(aPath.c_str(), &hFindData);
    IsFinished = hFindFile == INVALID_HANDLE_VALUE;
    while(!IsFinished)
    {
        if(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(hFindData.cFileName[0]!='.')
            {
                ExtractFilePath(cPath, aPath);
                cPath += hFindData.cFileName;
                cPath += "\\*.*";
                FileInfo(cPath, FL);
            }
        }
        else
        {
        	FSize = (((__int64)hFindData.nFileSizeHigh) * (MAXDWORD+1)) + ((__int64)hFindData.nFileSizeLow);
          ExtractFilePath(cPath, aPath);
          cPath += hFindData.cFileName;
					Cur.FileName=cPath;
					Cur.FileSize = FSize;
					Cur.FDate.lFileTime = hFindData.ftCreationTime.dwLowDateTime;
					Cur.FDate.hFileTime = hFindData.ftCreationTime.dwHighDateTime;
					Cur.FDate.UTS = 0;
					FL.push_back(Cur);
        }
        IsFinished = !FindNextFile(hFindFile, &hFindData);
    }
    if(hFindFile != INVALID_HANDLE_VALUE)
        FindClose(hFindFile);
}

void GenList(int aCount, filesStr &FL)
{
	char str[255];
	fileStr Cur;
	for( int i = 0; i < aCount; ++i)
	{
		sprintf(str, "D:\\Exspecto\\Test%d\\TestFile%d.tmp%d", i/10, i, i/100);
		Cur.FileName=str;
		Cur.FileSize = i*100;
		Cur.FDate.lFileTime = 0;
		Cur.FDate.hFileTime = 0;
		Cur.FDate.UTS = 1800000000+1000000/(i+1);
		FL.push_back(Cur);
	}
}

bool TimeProc(const string& fn, CDBProvider& db, hostRecords& aFL, map<string,int>& aTM)
{
	hostRecords::iterator idRec;
	DWORD dwStart;
	DWORD dwDur;
	char tmp[255];
	bool bFun = false;
	map<string,bool> aParams;

	if( fn == "Search" ){
		cout << "Input query: ";
		cin >> tmp;
		aFL.clear();
		dwStart = GetTickCount();
		bFun = db.Search(tmp, aParams, aFL);
	}else if( fn == "Add" ){
		dwStart = GetTickCount();
		db.AddFiles(aFL);
		bFun = true;
	}else if(fn == "Erase"){
		idRec = aFL.begin();
		dwStart = GetTickCount();
		db.EraseHost(idRec->HostName, idRec->IPNum, 0);
		bFun = true;
	}
	dwDur = GetTickCount() - dwStart;
	aTM[fn]=dwDur;
	return bFun;
}


void TestDB()
{
/*	
	ofstream out;
	out.open( "log.txt", ios_base::out | ios_base::trunc );

	DWORD tick_start,tick_end,tick_el_start,tick_el_end;
	bool res = true;

	SQLiteWrapper base;
	SQLiteStatement* st;

	if( base.Open( "first.db" ) )
		out<<"Opened..."<<endl;
	else
		out<<"Error during opening"<<endl;
	
	
	base.DirectStatement( "create table log (tracelevel INTEGER,text1 VARCHAR(255),text2 VARCHAR(255),text3 VARCHAR(2048))");

	st = base.Statement( "insert into log values(?,?,?,?)");

	char str[2047] = "t3sdfsdkjfhsdkjfhskdjfhkjsdhfkjsdhfksdhfkjshdfkjhsdfkjhsdkfjhdsjkfhsdkfjhsdkjfhksdjhfksjdhfksdjhfksdjhfksjdhfksjdhfkjsdhfkjsdhfkjhsdkjfhsdkjfhskdjfhksdjhfksjdfhskdjfhksjdhfksjdhfksj";
	char tmp[255];

	out<<"Creating table:"<<endl;
	tick_start = ::GetTickCount();

	base.Begin();
	for( int i = 0; i < 1000000; i++ )
	{
		if( 0 == i%100000 )
            cout<<i<<endl;
		res = true;
		st->Bind(0,i);
		st->Bind(1,"t1fsdjgkahdsfjgkl");
		st->Bind(2,"tsdgfkdsjglksdgjhdsg2");
		itoa(i,tmp,10);
		memcpy(str+20,tmp,strlen(tmp));
		st->Bind(3,str);
		res = st->Execute();
		if( !res )
			out<<"Error: insert "<<i<<" record\n";
	}
	base.Commit();

	tick_end = ::GetTickCount();
	out<<"Time: "<<(tick_end - tick_start)<<" ms"<<endl;
	delete st;
	
	out<<"Selecting..."<<endl;
	tick_start = ::GetTickCount();
	st = base.Statement( "select text2 from log where text3 like '%skd453%'");

	while( st->NextRow() )
	{
//		out<<st->ValueInt(0)<<"|"<<st->ValueString(1)<<"|"<<st->ValueString(2)<<"|"<<st->ValueString(3)<<endl;
	}

	tick_end = ::GetTickCount();
	out<<"Time: "<< (tick_end - tick_start)<<endl;

	SQLiteWrapper::ResultTable tab;
	base.SelectStmt( "select text2 from log where text2 like '%23533qw%'", tab );

	delete st;
	out.close();
	return 0;
//*/

	DWORD tick_start,tick_end;

	bool res = true;
	
	SQLiteWrapper base;
	SQLiteStatement* st;
	if( base.Open( "C:\\test2.db" ) )
	{
		cout << "Opened..." << endl;
	}
	else
	{
		cout << "Error during opening" << endl;
	}

	base.DirectStatement("create table TempKey(IDfile INTEGER PRIMARY KEY, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr datetime);");

	st = base.Statement( "insert into TempKey values(?,?,?,?,?)");

	char str[60] = "FileName";
	char tmp[20];

	cout<<"Creating table:"<<endl;
	tick_start = ::GetTickCount();

	base.Begin();
	for( int i = 0; i < 1000000; i++ )
	{
		if( 0 == i%100000 )
             cout<<i<< " | " << (::GetTickCount()-tick_start) << " ms" << endl;
 		res = true;
		st->Bind(0,i);
		st->Bind(1,0);
		itoa(i,tmp,10);
		memcpy(str+8,tmp,strlen(tmp));
		st->Bind(2,str);
		st->Bind(3,1000000+i);
		st->Bind(4,"2007-04-03 22:00:00");
		res = st->Execute();
		if( !res )
			cout<<"Error: insert "<<i<<" record\n"<<endl;
	}
	base.Commit();

	tick_end = ::GetTickCount();
	cout<<"Time key for SQLiteWrapper: "<<(tick_end - tick_start)<<" ms"<<endl;
	delete st;



	CppSQLite3DB db;
  CppSQLite3Buffer bufSQL; // буфер дл€ формировани€ запроса
	db.open("C:\\test1.db"); // ќткрываю файл базы данных

	cout<<"Creating table:"<<endl;
	if(!db.tableExists("TempKey"))
		db.execDML("create table TempKey(IDfile INTEGER PRIMARY KEY, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr datetime);");
//	if(!db.tableExists("Temp1"))
//		db.execDML("create table Temp1(IDfile INTEGER, IDhost int, FileName tinytext, FileSize int, FileTimeCr varchar(25);");

  db.execDML("begin transaction;");
	cout<<"Insert table:"<<endl;
	tick_start = ::GetTickCount();

	for( int i = 0; i < 1000000; i++ )
	{
		if( 0 == i%100000 )
            cout<<i<< " | " << (::GetTickCount()-tick_start) << " ms" << endl;

		itoa(i,tmp,10);
		memcpy(str+8,tmp,strlen(tmp));

   	bufSQL.format("insert into TempKey values(NULL, %d, %Q, %d, strftime('%%s','now'));", 0, str, 1000000+i);
   	db.execDML(bufSQL); // добавл€ю новую запись в таблицу хостов
	}

	tick_end = ::GetTickCount();
	cout<<"Time key for CppSQLite3DB: "<<(tick_end - tick_start)<<" ms"<<endl;

  db.execDML("commit transaction;");


  db.execDML("begin transaction;");
  CppSQLite3Statement stmt = db.compileStatement("insert into TempKey values (?, ?, ?, ?, ?);");
	
	cout<<"Insert table:"<<endl;
	tick_start = ::GetTickCount();

	for( int i = 0; i < 1000000; i++ )
	{
		if( 0 == i%100000 )
            cout<<i<< " | " << (::GetTickCount()-tick_start) << " ms" << endl;

    sprintf(str, "FileName%06d", i);

	  stmt.bindNull(1);
    stmt.bind(2, 0);
    stmt.bind(3, str);
    stmt.bind(4, 1000000+i);
    stmt.bind(5, "2007-04-03 22:00:00");
    stmt.execDML();
    stmt.reset();
  }

	tick_end = ::GetTickCount();
	cout<<"Time key for CppSQLite3DB.Bind: "<<(tick_end - tick_start)<<" ms"<<endl;

  db.execDML("commit transaction;");

/*
	cout<<"Insert table:"<<endl;
	for( int i = 0; i < 1000000; i++ )
	{
		if( 0 == i%100000 )
            cout<<i<<endl;

		itoa(i,tmp,10);
		memcpy(str+8,tmp,strlen(tmp));

   	bufSQL.format("insert into Temp1 values(%d, %d, %Q, %d, '2007-04-03 22:00:00');", i, 0, str, 1000000+i);
   	db.execDML(bufSQL); // добавл€ю новую запись в таблицу хостов
	}

	tick_end = ::GetTickCount();
	cout<<"Time for CppSQLite3DB: "<<(tick_end - tick_start)<<" ms"<<endl;
//*/	
}

int main()
{
	bool bTimeDbg = true;
	bool bResult;
	Log::instance().SetLoglevel( 20 );
	hostRecords rec;
	hostRecords::iterator idRec;
	hostRec CurRec;
	map<string,bool> aParams;
  aParams["IndexFind"]= false; // ќтключение поиска по таблице индексированых слов
	CDBProvider* db = NULL;
	filesStr FL;
	int resVal = 0;
	char tmp[255];
	HINSTANCE hLib = NULL;
//	typedef void* __declspec(dllimport) ProvType();
	typedef void* (*ProvType)();
	ProvType ProvFn;
try
{
		if( NULL == ( hLib = ::LoadLibraryA( "DBSQLite3" ) ) )
		{
			Log::instance().Trace( 10, "%d", GetLastError() );
			return -1;
		}
		if( NULL == ( ProvFn = (ProvType)::GetProcAddress( hLib, "RegisterPlugin" ) ) )
		{
			Log::instance().Trace( 10, "не удалось получить адрес функции RegisterPlugin из библиотеки DBSQLite" ); 
			::FreeLibrary( hLib );
			return -1;
		}
	db = (CDBProvider*)ProvFn();
//	db = new CDBSQLitProvider();
//	FileInfo(".\\*.*", FL);
	GenList(1000, FL); 
	CurRec.HostName = "DrAlexandr";
	strcpy (CurRec.IPNum, "192.168.1.2");
	CurRec.Files = FL;
	rec.push_back(CurRec);

	map<string, int> timeCode;
	typedef map<string, int>::const_iterator pTC;

	if( bTimeDbg ){	
		TimeProc("Erase", *db, rec, timeCode);
	}else db->EraseHost("DrAlexandr", "192.168.1.2", 0);

	cout << "Press Enter to cintinue...";
	cin >> tmp;

	if( bTimeDbg ){
		TimeProc("Add", *db, rec, timeCode);
	}else db->AddFiles(rec);

	rec.clear();
	if( bTimeDbg ){	
		bResult = TimeProc("Search", *db, rec, timeCode);
	}else{
		cout << "Input query: ";
		cin >> tmp;
		bResult = db->Search(tmp, aParams, rec);
	} 
	
	if( bResult )
	{
		list<fileStr>::iterator idF;
	  for (idRec=rec.begin(); idRec != rec.end(); ++idRec)
		{
			cout << (*idRec).HostName.c_str() << " | " << (*idRec).IPNum << endl;
			for (idF = (*idRec).Files.begin(); idF != (*idRec).Files.end(); ++idF)
			{
				cout << (*idF).FileName << " \t" << (*idF).FileSize << " \t" << (*idF).FDate.UTS <<endl;
			}
		}
	}

	if( bTimeDbg ){
		for(pTC p = timeCode.begin(); p != timeCode.end(); ++p)
		{
			cout << p->first << ": " << p->second << "ms" << endl;
		}	
	}
}catch(CPrvException& e)
{
	resVal = -1;
	cout << e.Message();
}catch( std::exception& e )
{
	resVal = -1;
	cout << e.what();
}catch(...)
{
	resVal = -1;
	cout << "Unknown exeption!";
}
	delete db;
	::FreeLibrary( hLib );

	
	return resVal;//*/
}
