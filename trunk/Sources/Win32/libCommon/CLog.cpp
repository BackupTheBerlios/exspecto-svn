#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "CLog.h"

CLog::CLog()
{
}

CLog::~CLog()
{
}

void CLog::Trace(int level, char* trace_text, ...)
{
	FILE *fp;
	fp = fopen("..\\log.txt", "a");

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
	FILE *fp;
	fp = fopen("..\\dump.txt", "a");

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
