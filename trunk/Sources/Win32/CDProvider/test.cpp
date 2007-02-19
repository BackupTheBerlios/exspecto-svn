#include <string>
#include <windows.h>
#include <stdio.h>
#include <dir.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <memory.h>
//#include <string.h>
//#include "memleakdetector.h"
#include "CDBProvider.h"

char* ExtractFilePath(char* Dest, char* Src)
{
    GetFullPathName(Src, 400, Dest, NULL);
    return Dest;
}

void FileInfo(char* aPath, filesStr &FL)
{
    HANDLE hFindFile;
    WIN32_FIND_DATA hFindData;
    unsigned __int64 FSize = 0;
    bool IsFinished;
    char cPath[400];
	fileStr Cur;

    hFindFile = FindFirstFile(aPath, &hFindData);
    IsFinished = hFindFile == INVALID_HANDLE_VALUE;
    while(!IsFinished)
    {
        if(hFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(hFindData.cFileName[0]!='.')
            {
                ExtractFilePath(cPath, aPath);
                strcat(cPath, hFindData.cFileName);
                strcat(cPath, "\\*.*");
                FileInfo(cPath, FL);
            }
        }
        else
        {
            FSize = (hFindData.nFileSizeHigh * (MAXDWORD+1)) + hFindData.nFileSizeLow;
            ExtractFilePath(cPath, aPath);
            strcat(cPath, hFindData.cFileName);
			Cur.FileName=cPath;
			Cur.FileSize = FSize;
			Cur.FileTime = hFindData.ftCreationTime.dwLowDateTime;
			FL.push_back(Cur);
        }
        IsFinished = !FindNextFile(hFindFile, &hFindData);
    }
    if(hFindFile != INVALID_HANDLE_VALUE)
        FindClose(hFindFile);
}

int main()
{
	
	CDBProvider* db = new CDBProvider("c:\\test.db");
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
	delete db;

/*    int i = 234;
    printf("%d, %0.6d, %.6d", i, i, i);*/
	
	return 0;
}
