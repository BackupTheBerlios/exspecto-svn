#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "CLog.h"

CLog::CLog()
{
}

CLog::~CLog()
{
}

void CLog::Trace(int level, char* trace_text, ...)
{
	SYSTEMTIME st;
    GetSystemTime(&st);
    st.wHour = (st.wHour + 4) % 24; 
	
	char filename[30];  // на имя модуля добавить
	FILE *fp;
	fp = fopen("temp", "w+");
	fprintf(fp, "%d_%d_%d_%d_%d.log", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	rewind(fp);
	
	char ch = getc(fp);
	int i=0;
	while (ch != EOF) { filename[i++] = ch; ch = getc(fp); }
	
	fclose(fp);
	remove("temp");
	filename[i++] = '\0';
	
	fp = fopen( filename, "a+");

	fprintf(fp, "%d, ", level);
	va_list args;
	va_start(args, trace_text);
	
	vfprintf(fp, trace_text, args);
	putc('\n', fp);
						
	va_end(args);
	fclose(fp);		
}

void CLog::Dump(int iLevel, BYTE* pbDumpData, int iDataSize, char* strAbout, ... )
{
	SYSTEMTIME st;
    GetSystemTime(&st);
    st.wHour = (st.wHour + 4) % 24; 
	
	FILE *fp;
	fp = fopen("dump", "w+");
	
	char str[255];
	GetModuleFileName( NULL, str, sizeof(str) );
	
	int k = 0;
	char *pc;
	for (pc = str; *pc; pc++)
	{
		if ( *pc == '\\') k++; 
		str[pc - str] = *pc;
	}
	str[pc - str] = '\0'; 
	
	int i = 0;
	int j = 0;
	char ModuleName[255];
	for (pc = str; *pc; pc++)
	{ 
		if ( i == k) ModuleName[j++] = *pc;
		if ( *pc == '\\') i++;
	}
	ModuleName[j] = '\0';

    fprintf(fp, "%s\n", ModuleName);  
    fprintf(fp, "%d.%d.%d %d:%d:%d.%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
//    не учитывает часовой пояс!!!! Для нас часы +3!!!!????
    
	va_list args;
	va_start(args, strAbout);
	
	vfprintf(fp, strAbout, args);
	putc('\n', fp);
						
	va_end(args);

	BYTE *p;
	k = 1;
	for ( p = pbDumpData; p <= (pbDumpData + iDataSize); p++ )
	{
		if ( k == 16 ) { fprintf(fp, "%X\n", *p); k = 0; }
		else	if ( k == 8 ) fprintf(fp, "%X|", *p);
				else fprintf(fp, "%X ", *p);
		k++;
	}
	fprintf(fp, "\n");
	fclose(fp);		
}
