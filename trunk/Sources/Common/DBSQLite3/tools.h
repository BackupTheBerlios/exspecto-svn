#ifndef TOOLS_H_
#define TOOLS_H_

#include "CDBProvider.h"

namespace Tools{
	void Split(string &text, string separators, list<string> &words);
//	char* GetTimeStr(time_t iTime, char* strRes);
	void FormatSQLQuery(const string& aText, string& aResult);
	//	time_t ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime); 
	//	time_t ConvertFileTimeToUTC(const fileDate& aDate); 
}

#endif /*TOOLS_H_*/
