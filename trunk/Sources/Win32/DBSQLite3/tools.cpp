#include "tools.h"

///////////////////////////////////////////////////////////////////////////////
//
/*char* Tools::GetTimeStr(time_t iTime, char* strRes)
{
	//Log::instance().Trace( 120, "CDBProvider::GetTimeStr [enter]" );
	try
	{
		struct tm *nt;
		time_t iTm;
		if (iTime == 0){
			time( &iTm );}
		else{ iTm = iTime;}
		nt = gmtime( &iTm );
		//Log::instance().Trace( 180, "%d :: ��������� ������ ���� [%li] � �����.", __LINE__, iTm );
		if(NULL==nt)
		{
			//Log::instance().Trace( 10,"[ERROR] CDBProvider::GetTimeStr: ������������ �������� ���������: %li", iTm );
			iTm=0;
			if( (nt = gmtime( &iTm )) == NULL )
			{
				//Log::instance().Trace( 5,"%d [ERROR] CDBProvider::GetTimeStr: ������ ������ ����������� �������", __LINE__ );
				throw;
			}
		}
		sprintf(strRes, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d\x0",nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday,
						nt->tm_hour, nt->tm_min, nt->tm_sec);
	}catch( std::exception& e )
	{
		//Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������� ����������: %s", e.what() );
		throw;
	}catch(...)
	{
		//Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������� ����������� ����������" );
		throw;
	}
	return strRes;
}*/
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// 
time_t Tools::ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime)
{
try
{
	if( lFTime == 0 && hFTime == 0) return 0; 
	time_t iTm;
	struct tm nt, *pnt;
	FILETIME ftWin;
	SYSTEMTIME stWin;
	ftWin.dwHighDateTime = hFTime;
	ftWin.dwLowDateTime = lFTime;
	
	if(!FileTimeToSystemTime(&ftWin, &stWin))
	{
		//Log::instance().Trace( 10,"[ERROR] %s: ������������ �������� ����������: %i, %i", __FUNCTION__, lFTime, hFTime );
		return 0;			
	}
	nt.tm_hour = stWin.wHour;
	nt.tm_mday = stWin.wDay;
	nt.tm_min = stWin.wMinute;
	nt.tm_mon = stWin.wMonth;
	nt.tm_sec = stWin.wSecond;
	nt.tm_year = stWin.wYear-1900;
	nt.tm_wday = stWin.wDayOfWeek;
	nt.tm_yday = 0;
	nt.tm_isdst = 0;
	if( (iTm = mktime(&nt)) <= 0 )
	{
		//Log::instance().Trace( 10,"[ERROR] %s: ������������ �������� ����������: %i, %i", __FUNCTION__, lFTime, hFTime );
		time( &iTm );
		pnt = gmtime( &iTm );
		iTm = mktime(pnt);
		return 0;
	}
	return iTm;
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

} 
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
time_t Tools::ConvertFileTimeToUTC(const fileDate& aDate)
{
	if( aDate.UTS == 0 )
	{
		return ConvertFileTimeToUTC(aDate.lFileTime, aDate.hFileTime);
	}	else return aDate.UTS;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Split(string & text, string & separators, list<string> & words)
// ��������� ��� ����� �� ������
// text        ������, ����������� �� �����
// separators  ������� �����������
// words       ������ ���� ������� �� ������
// varsep      ������ �������� ��������� �� ��������������� �������� ������������
void Tools::Split(string &text, string separators, list<string> &words)
{
	//Log::instance().Trace( 120, "%s [enter]", __FUNCTION__ );
try
{
	int n = text.length();
	int start, stop;
	string temp;
//	stop = 0;
//	start = 0;
	start = text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
//		start = stop;
//		stop = text.find_first_not_of(separators, start);
//    	if ((stop < 0) || (stop > n)) stop = n;
//    	if (&varsep != NULL)
//    	{
//    		if (stop == 0) varsep.push_back("#0#") // ����� ������ ������ ��� �������� �����������
//    		else varsep.push_back(text.substr(start, stop - start));
//    	}
//		start = stop;
		if (start >= n)return;
    	stop = text.find_first_of(separators, start);
    	if ((stop < 0) || (stop > n)) stop = n;
    	words.push_back(text.substr(start, stop - start));
    	start = text.find_first_not_of(separators, stop+1);
    }
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}
	//Log::instance().Trace( 120, "%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void Tools::FormatSQLQuery(const string& aText, string& aResult)
{
try
{
	for(int i = 0; i < (int)aText.length(); ++i)
	{
		switch (aText[i])
		{
//			case '\\':
//				aResult += "\\\\";
//				break;
			case '*':
				aResult += "%";
				break;
			case '?':
				aResult += "_";
				break;
			case '%':
				aResult += "%%";
				break;
			case '_':
				aResult += "\\_";
				break;
			case '\'':
				aResult += "\\'";
				break;
			default:
				aResult += aText[i];
		}
	}
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------