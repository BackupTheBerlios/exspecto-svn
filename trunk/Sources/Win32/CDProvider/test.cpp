/*#include <string>
#include <windows.h>
#include <stdio.h>
//#include <dir.h>
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <memory.h>
//#include <string.h>
//#include "memleakdetector.h"
#include "CSQLite3.h"

typedef bool (__fastcall *TTimeCheck)(CDBProvider& db, filesStr& aFL, map<string,int>& aTM,...);

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
bool Time_Search(CDBProvider& db, filesStr& aFL, map<string,int>& aTM,...)
{
	return false;
}
bool Time_Add(CDBProvider& db, filesStr& aFL, map<string,int>& aTM,...)
{
	return false;
}
bool Time_Erase(CDBProvider& db, filesStr& aFL, map<string,int>& aTM,...)
{
	db.EraseHost("DrAlexandr", "192.168.1.2", 0);
	return true;
}
bool TimeProc(TTimeCheck fn, CDBProvider& db, filesStr& aFL, map<string,int>& aTM, ...)
{
	DWORD dwStart;
	DWORD dwDur;
	string fNm;
//	if(fn == Time_Search)
//		fNm = "Search";
//	else fNm = "Unknow";

	dwStart = GetTickCount();
//
	dwDur = GetTickCount() - dwStart;
	aTM[fNm]=dwDur;
}
int main_T()
{
	bool bTimeDbg = false;
	bool bResult;
	Log::instance().SetLoglevel( 200 );
	hostRecords res;
	map<string,bool> aParams;
	CDBProvider* db = NULL;
	filesStr FL;
	int resVal = 0;
	char tmp[255];
try
{
	db = new CDBSQLitProvider("c:\\test.db");
	FileInfo(".\\*.*", FL);
//	GenList(1000, FL);
	map<string, int> timeCode;

	if( bTimeDbg ){	
//	TimeProc(Time_Erase, *db, FL, timeCode);
	}else db->EraseHost("DrAlexandr", "192.168.1.2", 0);

	if( bTimeDbg ){
//	TimeProc(Time_Add, *db, FL, timeCode);
	}else db->AddFiles(&FL, "DrAlexandr", "192.168.1.2");

	cout << "Input query: ";
	cin >> tmp;
	
	if( bTimeDbg ){	
//	bResult = TimeProc(Time_Search, *db, FL, timeCode);
	}else bResult = db->Search(tmp, aParams, res); 
	
	if( bResult )
	{
		list<hostRec>::iterator idRes;
		list<fileStr>::iterator idF;
	  for (idRes=res.begin(); idRes != res.end(); ++idRes)
		{
			cout << (*idRes).HostName.c_str() << " | " << (*idRes).IPName << endl;
			for (idF = (*idRes).Files.begin(); idF != (*idRes).Files.end(); ++idF)
			{
				cout << (*idF).FileName << " \t" << (*idF).FileSize << " \t" << (*idF).FDate.UTS <<endl;
			}
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

	
	return resVal;
}
*/