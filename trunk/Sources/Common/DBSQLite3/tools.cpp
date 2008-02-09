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
		//Log::instance().Trace( 180, "%d :: ���ߜ��ޜ��ܜ��՜�����՜��ݜ��؜��� ���������М��ܜ��ߜ��� ���Ԝ��М������ [%li] ���� ���ќ��������՜���.", __LINE__, iTm );
		if(NULL==nt)
		{
			//Log::instance().Trace( 10,"[ERROR] CDBProvider::GetTimeStr: �������՜��ڜ��ޜ����������՜��ڜ�����ݜ��ޜ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���М������Ӝ�����ܜ��՜��ݜ������: %li", iTm );
			iTm=0;
			if( (nt = gmtime( &iTm )) == NULL )
			{
				//Log::instance().Trace( 5,"%d [ERROR] CDBProvider::GetTimeStr: ����������؜��ќ��ڜ��� ���Ҝ�����ל��ޜ��Ҝ��� ���������М��ݜ��Ԝ��М���������ݜ��ޜ��� ���������ݜ��ڜ�����؜���", __LINE__ );
				throw;
			}
		}
		sprintf(strRes, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d\x0",nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday,
						nt->tm_hour, nt->tm_min, nt->tm_sec);
	}catch( std::exception& e )
	{
		//Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���: %s", e.what() );
		throw;
	}catch(...)
	{
		//Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���" );
		throw;
	}
	return strRes;
}*/
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// 
 /*
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
		//Log::instance().Trace( 10,"[ERROR] %s: �������՜��ڜ��ޜ����������՜��ڜ�����ݜ��ޜ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���М������Ӝ�����ܜ��՜��ݜ�����ޜ���: %i, %i", __FUNCTION__, lFTime, hFTime );
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
		//Log::instance().Trace( 10,"[ERROR] %s: �������՜��ڜ��ޜ����������՜��ڜ�����ݜ��ޜ��� ���ל��ݜ��М�����՜��ݜ��؜��� ���М������Ӝ�����ܜ��՜��ݜ�����ޜ���: %i, %i", __FUNCTION__, lFTime, hFTime );
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
	throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
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
*/
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Split(string & text, string & separators, list<string> & words)
// �������М��ל��ќ��؜��Ҝ��М��՜��� ���؜��ܜ��� ������М��ٜ��ۜ��� ���ݜ��� ���؜��Ԝ��؜��ޜ��ܜ���
// text        �������������ޜ��ڜ���, �������М��ל��ќ��؜��Ҝ��М��՜��ܜ��М��� ���ݜ��� ������ۜ��ޜ��Ҝ���
// separators  ������؜��ܜ��Ҝ��ޜ��ۜ��� �������М��ל��Ԝ��՜��ۜ��؜�����՜��ۜ���
// words       ������ߜ��؜�����ޜ��� ������ۜ��ޜ��� ���؜��ל��������������� ���؜��� �������������ޜ��ڜ���
// varsep      ������ߜ��؜�����ޜ��� ������؜��ܜ��Ҝ��ޜ��ۜ��ޜ��� ������ޜ��������ޜ��������М��� ���؜��� ���ߜ��ޜ�����ۜ��՜��Ԝ��ޜ��Ҝ��М�����՜��ۜ�����ݜ��� ���Ҝ�����ޜ��Ԝ��������؜��� �������М��ל��Ԝ��՜��ۜ��؜�����՜�����՜���
void Tools::Split(string &text, string separators, list<string> &words)
{
	//Log::instance().Trace( 120, "%s [enter]", __FUNCTION__ );
try
{
	int n = (int)text.length();
	int start, stop;
	string temp;
//	stop = 0;
//	start = 0;
	start = (int)text.find_first_not_of(separators);
	while ((start >= 0) && (start < n))
	{
//		start = stop;
//		stop = text.find_first_not_of(separators, start);
//    	if ((stop < 0) || (stop > n)) stop = n;
//    	if (&varsep != NULL)
//    	{
//    		if (stop == 0) varsep.push_back("#0#") // �������՜������՜��� ���ߜ��՜������Ҝ������ ������ۜ��ޜ��Ҝ��ޜ��� ���ݜ��՜��� ������؜��ܜ��Ҝ��ޜ��ۜ��ޜ��� ������՜��ߜ��М������М�����ޜ������ޜ���
//    		else varsep.push_back(text.substr(start, stop - start));
//    	}
//		start = stop;
		if (start >= n)return;
    	stop = (int)text.find_first_of(separators, start);
    	if ((stop < 0) || (stop > n)) stop = n;
    	words.push_back(text.substr(start, stop - start));
    	start = (int)text.find_first_not_of(separators, stop+1);
    }
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
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
	throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
