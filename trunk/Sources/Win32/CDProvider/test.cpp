#include <string>
#include <windows.h>
#include <stdio.h>
#include <dir.h>
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
        	FSize = (hFindData.nFileSizeHigh * (MAXDWORD+1)) + hFindData.nFileSizeLow;
          ExtractFilePath(cPath, aPath);
          cPath += hFindData.cFileName;
					Cur.FileName=cPath;
					Cur.FileSize = FSize;
					Cur.lFileTime = hFindData.ftCreationTime.dwLowDateTime;
					Cur.hFileTime = hFindData.ftCreationTime.dwHighDateTime;
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
	list<int> res;
	map<string,bool> aParams;
	CDBProvider* db = new CDBSQLitProvider("c:\\test.db");
	filesStr FL;
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
	
	db->AddFiles(&FL, "DrAlexandr", "192.168.1.2");
	char tmp[255];
	cout << "Input query: ";
	cin >> tmp;
	if(db->Search(tmp, aParams, res))
	{
		list<int>::iterator idRes;
	  for (idRes=res.begin(); idRes != res.end(); ++idRes)
    	cout << (*idRes) << endl;
	}
	delete db;

/*    int i = 234;
    printf("%d, %0.6d, %.6d", i, i, i);*/
	
	return 0;
}
