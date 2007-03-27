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
#include "CDBProvider.h"


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
					FL.push_back(Cur);
        }
        IsFinished = !FindNextFile(hFindFile, &hFindData);
    }
    if(hFindFile != INVALID_HANDLE_VALUE)
        FindClose(hFindFile);
}

int main()
{
	Log::instance().SetLoglevel( 200 );
	hostRecords res;
	map<string,bool> aParams;
	CDBProvider* db = NULL;
	filesStr FL;
	int resVal = 0;
try
{
	db = new CDBSQLitProvider("c:\\test.db");
	FileInfo(".\\*.*", FL);
/*	fileStr Cur;
	while(true)
	{
		//Cur.FileName = ;
		Cur.FileName="Test";
		Cur.FileName+=(FL.size()+48);
		Cur.FileName+=".tmp";
		Cur.FileSize = GetTickCount();
		Cur.FileTime = GetTickCount()*10;
//		i++;
//		if (i >= 10) break;
		FL.push_back(Cur);
		if (FL.size() >= 10) break;
	}*/
//	cout << typeid(CppSQLite3Exception);
//	CppSQLite3Exception* sqle = new CppSQLite3Exception(0, "Text");
//	std::exception e;
//	cout << typeid(sqle).name() << endl;
//	cout << typeid(e).name() << endl;
//	cout << typeid(res).name() << endl;
//	delete sqle;
	
	db->EraseHost("DrAlexandr", "192.168.1.2", 0); 
	db->AddFiles(&FL, "DrAlexandr", "192.168.1.2");
	char tmp[255];
	cout << "Input query: ";
	cin >> tmp;
	if(db->Search(tmp, aParams, res))
	{
		list<hostRec>::iterator idRes;
	  for (idRes=res.begin(); idRes != res.end(); ++idRes)
    	cout << (*idRes).HostName.c_str() << endl;
	}
}catch(...)
{
	resVal = -1;
}
	delete db;

/*    int i = 234;
    printf("%d, %0.6d, %.6d", i, i, i);*/
	
	return resVal;
}
