#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "CLog.h"

char FileName[40];
FILE *fp;
SYSTEMTIME st;

CLog::CLog()
{
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
	bool flag = false;
	
	for (pc = str; *pc; pc++)
	{ 
		if ( *pc == '.') flag = false;
		if ( i == k)
			if (flag) FileName[j++] = *pc;
		if ( *pc == '\\') i++;
	}
	FileName[j] = '\0';	
	
    GetSystemTime(&st);
    st.wHour = (st.wHour + 4) % 24; 
		
	fp = fopen("temp", "w+");
	fprintf(fp, "%s_%d%d%d_%d%d.log", FileName, st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute);
	rewind(fp);
	
	char ch = getc(fp);
	i = 0;
	while (ch != EOF) { FileName[i++] = ch; ch = getc(fp); }
	FileName[i] = '\0';
	
	fclose(fp);
	remove("temp");
	
}

CLog::~CLog()
{
}

void CLog::Trace(int level, char* trace_text, ...)
{
	
	fp = fopen( FileName, "a+");
	fprintf(fp, "%d.%d.%d %d:%d:%d.%d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	
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
	
	fp = fopen( FileName, "a+");
	fprintf(fp, "%d.%d.%d %d:%d:%d.%d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    
	va_list args;
	va_start(args, strAbout);
	
	vfprintf(fp, strAbout, args);
	putc('\n', fp);
						
	va_end(args);

	BYTE *p;
	BYTE k = 1;
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
