/////////////////////////////////////////////////////////////////////////////
// ����� ���� ������                                                       //
//                                                                         //
// TableHostes  ������� ��������� �������� � ���� ����������               //
//   IDhost        INTEGER PRIMARY KEY ������ ����������                   //
//   IPNum         CHAR(15)       IP ����������                            //
//   HostName      TINYTEXT       ��� ����������                           //
//   DateRef       DATETIME       ����� ����������� ���������� ����������  //
// ----------------------------------------------------------------------- //
// TableFiles   �������� ������ � �������� �� � ����������                 //
//   IDfile        INTEGER PRIMARY KEY ������ �����                        //
//   IDhost        INT            ������ ����������                        //
//   FileName      TINYTEXT       ��� �����                                //
//   FileSize      INT            ������ �����                             //
//   FileTimeCr    DATETIME       ����� �������� �����                     //
// ----------------------------------------------------------------------- //
// TableWords   ������� ���� �������� ����                                 //
// { ���� � ������ ����������� �� ����� (������) � ��������� � �������.  } //
// { ��� ������� ��� ��������� ������ ���������.                         } //
//   IDword        INTEGER PRIMARY KEY ������ �����                        //
//   Word          TINYTEXT       "������" (�����)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   ������������ ���� ������ � ���� �������������� ����� //
//   IDword        INT            ������ �����                             //
//   IDfile        INT            ������ �����                             //
//   IsPath        BOOL           "������" - ����� ����?                   //
/////////////////////////////////////////////////////////////////////////////
 
#include "CDBProvider.h"
const char sepFile[] = "\\/,;:<>|. &()[]{}-_"; 

/*CDBProvider::CDBProvider()
{
}*/

CDBSQLitProvider::CDBSQLitProvider(const char* szFile)//: CDBProvider()
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s ::: Open BD", __FUNCTION__ );
	db.open(szFile); // �������� ���� ���� ������
	
	// ������ ������� ��� �� ���������� //INTEGER PRIMARY KEY <-AUTO_INCREMENT
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableHostes" );
	if(!db.tableExists("TableHostes"))
		db.execDML("create table TableHostes(IDhost INTEGER PRIMARY KEY, IPNum char(15) NOT NULL, HostName tinytext, DateRef datetime);");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableFiles" );
	if(!db.tableExists("TableFiles"))
		db.execDML("create table TableFiles(IDfile INTEGER PRIMARY KEY, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr datetime);");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableWords" );
	if(!db.tableExists("TableWords"))
		db.execDML("create table TableWords(IDword INTEGER PRIMARY KEY, Word tinytext NOT NULL);");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableWordInFiles" );
	if(!db.tableExists("TableWordInFiles"))
	{
		db.execDML("create table TableWordInFiles(IDword INT NOT NULL, IDfile INT NOT NULL, IsPath BOOL NOT NULL DEFAULT false);");
		db.execDML("create index IDword ON TableWordInFiles (IDword);");
	}
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [exit]", __FUNCTION__ );
}

/*CDBProvider::~CDBProvider()
{
}*/

CDBSQLitProvider::~CDBSQLitProvider()
{
}

/*char* CDBProvider::GetTimeStr(time_t iTime, char* strRes)
{
	Log::instance().Trace( 120, "CDBProvider::GetTimeStr [enter]" );
	try
	{
		struct tm *nt;
		time_t iTm;
		if (iTime == 0){
			time( &iTm );}
		else{ iTm = iTime;}
		nt = gmtime( &iTm );
		Log::instance().Trace( 180, "%d :: ��������� ������ ���� [%li] � �����.", __LINE__, iTm );
		if(NULL==nt)
		{
			Log::instance().Trace( 10,"[ERROR] CDBProvider::GetTimeStr: ������������ �������� ���������: %li", iTm );
			iTm=0;
			if( (nt = gmtime( &iTm )) == NULL )
			{
				Log::instance().Trace( 5,"%d [ERROR] CDBProvider::GetTimeStr: ������ ������ ����������� �������", __LINE__ );
				throw;
			}
		}
		sprintf(strRes, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d\x0",nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday,
						nt->tm_hour, nt->tm_min, nt->tm_sec);
	}catch( std::exception& e )
	{
		Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������� ����������: %s", e.what() );
		throw;
	}catch(...)
	{
		Log::instance().Trace( 5,"CDBProvider::GetTimeStr: �������� ����������� ����������" );
		throw;
	}
	return strRes;
}*/
//-----------------------------------------------------------------------------
 
time_t CDBProvider::ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime)
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
		Log::instance().Trace( 10,"[ERROR] CDBProvider::ConvertFileTimeToUTC: ������������ �������� ����������: %i, %i", lFTime, hFTime );
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
		Log::instance().Trace( 10,"[ERROR] CDBProvider::ConvertFileTimeToUTC: ������������ �������� ����������: %i, %i", lFTime, hFTime );
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
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

} 
//-----------------------------------------------------------------------------

time_t CDBProvider::ConvertFileTimeToUTC(const fileDate& aDate)
{
	if( aDate.UTS == 0 )
	{
		return ConvertFileTimeToUTC(aDate.lFileTime, aDate.hFileTime);
	}	else return aDate.UTS;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddFile(fileStr* fs, const char* aHostName, const char* aIPnum)
// �������� �������, � ���������� �������������� �� �����
/*void CDBProvider::AddFile(fileStr* fs, string aHostName, string aIPnum)
{
	int i=-1;
	char strTmp[255];
    CppSQLite3Buffer bufSQL;
    printf("%s\n",bufSQL.format("select * from TableHostes where IPNum = %Q;", aIPnum));
	CppSQLite3Query q = db.execQuery(bufSQL);
    if(!q.eof())
    	i = q.getIntField("IDhost", -1);
    q.finalize();
    
    if(i >= 0)
    {
    	printf("%s\n",bufSQL.format("delete from TableFiles where IDhost = %d;", i));
    	db.execDML(bufSQL);

    	printf("%s\n",bufSQL.format("update TableHostes set DateRef = %Q where IDhost = %d;",GetTimeStr(0, strTmp),i));
    	db.execDML(bufSQL);
    }
    else
    {
    	if (0 != db.execScalar("select count(0) from TableHostes;"))
    		i = db.execScalar("select MAX(IDhost) from TableHostes;");
    	i++;
    	printf("%s\n",bufSQL.format("insert into TableHostes values(%d, %Q, %Q, %Q);", i, aIPnum, aHostName, GetTimeStr(0, strTmp)));
    	db.execDML(bufSQL);
    }
    
    int j = -1;
   	if (0 != db.execScalar("select count(0) from TableFiles;"))
   		j = db.execScalar("select MAX(IDfile) from TableFiles;");
   	j++;
   	printf("%s\n",bufSQL.format("insert into TableFiles values(%d, %d, %Q, %d, %Q);", j, i, fs->FileName, fs->FileSize, GetTimeStr(fs->FileTime, strTmp)));
   	db.execDML(bufSQL);
}*/
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum)
// ��������� ������ ������ � ��
// aFileList  ��������������� ������ �� ������ ��������� � ����������� �����
// aHostName  ��� ���������� �� ������ ��������� ������������� ������
// aIPnum     IP ������ ����������
void CDBSQLitProvider::AddFiles(pFilesStr aFileList, const string& aHostName, const string& aIPnum)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
	int i=-1; // ������ ID ������� ������
//	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
	filesStr::iterator Cur; // ��������� �� ������� ���� ������
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
	CppSQLite3Query q;
	
// ������ ����������    
//	db.execDML("begin transaction;");

	Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aIPnum.c_str());
	q = db.execQuery(bufSQL); // ������

	Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
  if(!q.eof())
  	i = q.getIntField("IDhost", -1); // ������� ����� ID ��������� �� ������������ ������
		                                 // � ������� IP ����������� � aIPnum
	q.finalize();

  if(i >= 0) // ���� ������ ����������
  {
		Log::instance().Trace( 180, "%d :: ������ ��� ����������", __LINE__ );
   	bufSQL.format("update TableHostes set DateRef = select strftime('%%s','now') where IDhost = %d;",i);
   	db.execDML(bufSQL); // �������� ����/����� ���������� ����������
  }
	else // �����
	{
		Log::instance().Trace( 180, "%d :: ��������� ����� ������", __LINE__ );
   	bufSQL.format("insert into TableHostes values(NULL, %Q, %Q, strftime('%%s','now'));", aIPnum.c_str(), aHostName.c_str());
   	db.execDML(bufSQL); // �������� ����� ������ � ������� ������

		Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aIPnum.c_str());
		q = db.execQuery(bufSQL); // ������

		Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
   	if(!q.eof())
   		i = q.getIntField("IDhost", -1); // ������� ����� ID ��������� �� ������������ ������
			                                 // � ������� IP ����������� � aIPnum
   	q.finalize();
   	if( i < 0 ) throw CPrvException("������ � ��������� ��. (������� ������)", __LINE__);
	}
    
  int j, ftm;
   	
 	for ( Cur = (*aFileList).begin(); Cur != (*aFileList).end(); Cur++ )
	{
		Log::instance().Trace( 185, "%d :: ��������� ����� ������ [%s]", __LINE__,  (*Cur).FileName.c_str());
		ftm = ConvertFileTimeToUTC((*Cur).FDate); 
		bufSQL.format("insert into TableFiles values(NULL, %d, %Q, %d, %d);", i, (*Cur).FileName.c_str(), (*Cur).FileSize, ftm);
		Log::instance().Trace( 190, "%d :: ��������� � ��", __LINE__);
		db.execDML(bufSQL);

		Log::instance().Trace( 185, "%d :: ��������� ����� ����� �� ������ [%s]", __LINE__,  (*Cur).FileName.c_str());
		Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	  bufSQL.format("select IDfile from TableFiles where IDhost = %d AND FileName= %Q;", i, (*Cur).FileName.c_str());
		q = db.execQuery(bufSQL); // ������

		Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
		j = -1;
   	if(!q.eof())
   	{
   		j = q.getIntField("IDfile", -1); // ������� ����� ID ��������� �� ������������ ������
			                                 // � ������� IP ����������� � aIPnum
			q.nextRow();
   		if(!q.eof()) j = -2;
   	}
   	q.finalize();
   	if( j == -2 ) throw CPrvException("������ � ������� ������! ������ �� ������ �������������!", __LINE__);
   	if( j < 0 )throw CPrvException("������ � ��������� ��. (������� ������)", __LINE__);
   	AddWord(j,(*Cur).FileName);
  }
// ���������� ����������
//	db.execDML("commit transaction;");
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::AddFiles [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWord(int aID, string aPath)
// ��������� ��� ����� �� ������ � ��������� � ������ ���� � ������������ ����� ������
// aID    ����� ����� � ������� ������
// aPath  ������ ��� �����
void CDBSQLitProvider::AddWord(int aID, const string& aPath)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
	list<string> wrd;
	int n = aPath.rfind("\\"); //���� ������ �������������� ����� ������
	string tmp;
	if( n>=0 ) // ���� ����� ������ �� ����� ����� �� ����� ��� ����
	{
		tmp = aPath.substr(0,n);
		Split(tmp, sepFile, wrd); 
		AddWordInTable(aID, wrd, true);
	}
	wrd.clear();

	if( (n+1)<(int)aPath.size() ) //���� ����� ����� ���� ������� �� ��� ��� �����
	{
		tmp = aPath.substr(n+1,aPath.size()-(n+1));
		Split(tmp, sepFile, wrd); 
		AddWordInTable(aID, wrd, false);
	}
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::AddWord [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Split(string & text, string & separators, list<string> & words)
// ��������� ��� ����� �� ������
// text        ������, ����������� �� �����
// separators  ������� �����������
// words       ������ ���� ������� �� ������
// varsep      ������ �������� ��������� �� ��������������� �������� ������������
void CDBProvider::Split(string &text, string separators, list<string> &words)
{
	Log::instance().Trace( 120, "CDBProvider::%s [enter]", __FUNCTION__ );
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
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}
	Log::instance().Trace( 120, "CDBProvider::%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// ��������� ����� � ������� ����
// aID     ����� �����
// words   ������ ����
// IsPath  ����� ������ � ���� � �����
void CDBSQLitProvider::AddWordInTable(int aID, list<string> &words, bool IsPath)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
  list<string>::iterator wptr;
  int i; 
	for (wptr = words.begin(); wptr != words.end(); ++wptr)
 	{
 		i = -1; //������ ������ � �������
		bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		q = db.execQuery(bufSQL); // ������
 		if(!q.eof())
 			i = q.getIntField("IDword", -2);
 		q.finalize();
		if(i == -1)
	  {
   		bufSQL.format("insert into TableWords values(NULL, %Q);", (*wptr).c_str());
   		db.execDML(bufSQL); // �������� ����� ������ � �������

			Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
		  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
			q = db.execQuery(bufSQL); // ������

			Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
	   	if(!q.eof())
   			i = q.getIntField("IDword", -2); // ������� ����� ID ��������� �� ������������ ������
		 	q.finalize();
   	}
   	if( i < 0 ) throw CPrvException("������ � ��! (������� ����)", __LINE__);
		bufSQL.format("insert into TableWordInFiles values(%d, %d, %d);", i, aID, IsPath);
		db.execDML(bufSQL); //�������� ����� ����������� ������ �����
	}
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [exit]" );
}
//-----------------------------------------------------------------------------

void CDBProvider::FormatSQLQuery(const string& aText, string& aResult)
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
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Find(string aText, list<string> & aParams)
// ������� ���� ������� ��������� � ���� ������
// ������ ������������ ����� ������ ��������������� ������ ��������
// �������. 
// ��� �������, �� ����������� ? � * �������� �������� ��������� ������ ��
// ������� ������������ �����.
bool CDBSQLitProvider::Find(const string& aText, map<string,bool> &aParams, list<int> &Result)
{
try
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
	list<string> wrd; // ������ ���� �� �����
//	bool CommASAnd=true; //����������� ����� ������� AND
//	list<string> varsep; //������ ����������� �����������
	list<string>::iterator wptr;//, prev, next;//, sptr;
	list<int> IDWord;
	list<int> IDFile;
	list<int>::iterator idwPtr, idfPtr;
	string temp="";
	int idT;
	FormatSQLQuery(aText, temp);
	Split(temp, sepFile, wrd);

  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
		Log::instance().Trace( 190, "%d :: ���� ����� %s", __LINE__, (*wptr).c_str() );
    bufSQL.format("SELECT IDword FROM TableWords WHERE Word LIKE %Q;", (*wptr).c_str());
	  q = db.execQuery(bufSQL); // ������
	  while(!q.eof())
	  {
	 	  if( (idT = q.getIntField("IDword", -1)) != -1) 
		    IDWord.push_back(idT);
		  q.nextRow();
	  }
	  q.finalize();
	}
  
  idwPtr=IDWord.begin();
  CExcerpts *idFiles = new CExcerpts();
  bool isFirst = true; 
  for (; idwPtr != IDWord.end(); ++idwPtr)
  {
		Log::instance().Trace( 190, "%d :: ���� ����� �������������� ����� � ID=%d", __LINE__, (*idwPtr) );
    bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
	  q = db.execQuery(bufSQL); // ������
	  while(!q.eof())
	  {
			idFiles->Transact(q.getIntField("IDfile", -1), isFirst);
		  q.nextRow();
	 	}
	  q.finalize();
	  
	  if( !isFirst )
	  {
	    idFiles->TransactEnd();
	  }else{
	  	isFirst = false;
	  }
  }
  string tmpS;
  idFiles->GetAsString(tmpS);

	Log::instance().Trace( 180, "%d :: ���� ����� �������������� ��������� %s", __LINE__, temp.c_str() );
  if(idFiles->IsEmpty())
  {
  	bufSQL.format("select IDfile from TableFiles where FileName LIKE '%%%s%%';", temp.c_str());
  }else{
  	bufSQL.format("select IDfile from TableFiles where IDfile IN (%s) AND FileName LIKE '%%%s%%';", tmpS.c_str(), temp.c_str());
  }
  q = db.execQuery(bufSQL); // ������
  while(!q.eof())
  {
		idFiles->Transact(q.getIntField("IDfile", -1), false);
	  q.nextRow();
 	}
  q.finalize();
  idFiles->TransactEnd();
  idFiles->GetAsList(Result);
  delete idFiles;

	Log::instance().Trace( 100, "CDBSQLitProvider::%s [return <%d>]", __FUNCTION__, Result.size()>0 );
  return (Result.size()>0);
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Search(string aText, list<string> & aParams)
// ������� ��������� ��������� ��� ������ �� ��������� (������� ������ � ����
// ������) ����������� ���������:
// ,; - ����� ��������� �� �������� � ������������� ����� ����� ���������� ���
// ^  - ����� ��������� �� �������� � ������������� ����� ����� ���� �
// ��������� �������� ����� ������� � �������� ��� ��������� ����������
// ����������� ���������������. (��������� ���������� � � ��� ����)
bool CDBSQLitProvider::Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)
{
try
{
	list<int> res;
	list<int>::iterator id;
	list<hostRec>::iterator ihRec;
	hostRec hRec;
	fileStr fRec;
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
  string temp = "";
  string tHName;
  string tIPName;
  char ctmp[12];
	if( Find(aText, aParams, res) )
	{
	  id=res.begin();
	  if(id != res.end())
	  	temp = _itoa((*id), ctmp, 10);
	  for (; id != res.end(); ++id)
	  {
			temp += ", ";
			temp += _itoa((*id), ctmp, 10);
	  }
  	bufSQL.format("select th.IDHost, HostName, IPNum, FileName, FileSize, strftime('%%s',FileTimeCr) as FTUTS FROM TableFiles AS tf, TableHostes AS th WHERE tf.IDfile IN (%s) AND th.IDhost=tf.IDhost ORDER BY th.IDhost;", temp.c_str());
	  q = db.execQuery(bufSQL); // ������
	  int curH = -1;
	  int iTmp;
	  while(!q.eof())
  	{
  		iTmp = q.getIntField("IDfile", -2);
  		if ( curH != iTmp && iTmp != -2 )
  		{
  			if ( curH != -1 ) Result.push_back(hRec);
  			curH = iTmp;
	  		hRec.HostName = q.getStringField("HostName", "");
  			strcpy(hRec.IPName, q.getStringField("IPName", ""));
  			hRec.Files.clear();
  		}
  		fRec.FileName = q.getStringField("FileName", "");
  		fRec.FileSize =	q.getIntField("FileSize", 0);
			fRec.FDate.UTS = q.getIntField("FTUTS", 0);
			hRec.Files.push_back(fRec);
	  	q.nextRow();
 		}
		if ( curH != -1 ) Result.push_back(hRec);
  	q.finalize();
		return true;
	}else return false;
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------

// ���� � ������� � ������� �������� ��� ������
void CDBSQLitProvider::EraseHost(const string& aHostName, const string& aIPnum, const fileDate& aFileDate, bool aOnlyFiles)
{
		EraseHost(aHostName, aIPnum, ConvertFileTimeToUTC(aFileDate), aOnlyFiles);
}
//-----------------------------------------------------------------------------

// ���� � ������� ���������� ��������� time � �.�. ����������� ���������� �
void CDBSQLitProvider::EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles)
{ 
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
	int i=-1; // ������ ID ������� ������
	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
//	filesStr::iterator Cur; // ��������� �� ������� ���� ������
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  string temp;
  list<int>liIDhost;
  list<int>::iterator pIDhost;
  if( &aHostName != NULL && !aHostName.empty() )
  {
  	if( temp.empty() )
  		temp = " WHERE ";
  	else temp += " AND ";
  	temp += "HostName = '"; 
  	FormatSQLQuery(aHostName, temp);
  	temp += "'";
  }

  if( &aIPnum != NULL && !aIPnum.empty() )
  {
  	if( temp.empty() )
  		temp = " WHERE ";
  	else temp += " AND ";
  	temp += "IPnum = '" + aIPnum + "'";
  }
  
  if( aDate != 0)
  {
  	if( temp.empty() )
  		temp = " WHERE ";
  	else temp += " AND ";
  	temp += "DateRef < '";
  	temp += _itoa(aDate,strTmp,10);
  	temp += "'";
  }

	bufSQL.format("select * from TableHostes%s;", temp.c_str());

	Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	CppSQLite3Query q = db.execQuery(bufSQL); // ������

	Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
  while(!q.eof())
  {
  	if( (i = q.getIntField("IDhost", -1)) != -1 )
  		liIDhost.push_back(i);
	  q.nextRow();
  }
  q.finalize();
  	
	for( pIDhost = liIDhost.begin(); pIDhost != liIDhost.end(); ++pIDhost )
	{
   	bufSQL.format("delete from TableWordInFiles where IDfile = (select IDfile from TableFiles where IDhost = %d);", (*pIDhost));
  	db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� (����-����)

   	bufSQL.format("delete from TableFiles where IDhost = %d;", (*pIDhost));
   	db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� ������

   	if( !aOnlyFiles )
   	{
   		bufSQL.format("delete from TableHostes where IDhost = %d;",(*pIDhost));
   		db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� ������
   	}
  }
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

CExcerpts::CExcerpts(int aCount)
{
	Allocation = aCount;
	Memory = NULL;
	try
	{
		Memory = (int *)calloc(Allocation , sizeof( int ) );
		Length=0;
		Curr=0;
	}catch(...){
		Allocation = 0;
		free(Memory);
		Memory = NULL;
		throw;
	} 
}

CExcerpts::CExcerpts()
{
	CExcerpts(1000);
}

CExcerpts::~CExcerpts()
{
	free(Memory);
}

int CExcerpts::Find(int aVal)
{
	for(int i = 0; i < Length; i++)
		if(Memory[Length]==aVal)return i;
	return -1;
}

void CExcerpts::Transact(int aVal, bool aisFirst)
{
	int el = Find(aVal);
	int tmp;
	if(aisFirst)
	{
		if(el == -1)
		{
			if(Allocation <= Length)
			{
				try
				{
					Allocation += 500;
					Memory = (int *)realloc(Memory, Allocation*sizeof(int));
				}catch(...){
					Allocation = 0;
					free(Memory);
					Memory = NULL;
					throw;
				} 
			}
			Memory[Length]=aVal;
			Length++;
		}
	}else{
		if( el >=0 )
		{
			tmp = Memory[Curr];
			Memory[Curr]=Memory[el];
			Memory[el]=tmp;
			Curr++;
		}
	}
}

void CExcerpts::TransactEnd()
{
	Length = Curr;
	Curr = 0;
}

void CExcerpts::GetAsList(list<int> & Res)
{
	for(int i = 0; i < Length; ++i)
		Res.push_back(Memory[i]);
}

void CExcerpts::GetAsString(string & Res)
{
	char tmp[12];
	for(int i = 0; i < Length; ++i)
	{
		sprintf(tmp, "%d", Memory[i]);
		if( i != 0) Res += ", ";
		Res += tmp;
	}
}

bool CExcerpts::IsEmpty()
{
	return (Length <= 0);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CPrvException::CPrvException(const char* aText, int aLine, const char* aFunct)
{
	int iSize = sizeof(aText)+1;
	int i=0;
	if( aLine != 0 ) iSize += 13;
	if( aFunct != NULL ) iSize += sizeof(aFunct)+2;
	strError = new char[iSize];
	if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
	if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
	i += sprintf(strError + i, "%s ", aText);
	Log::instance().Trace( 5,"%s", strError );
}
CPrvException::CPrvException(CppSQLite3Exception&  e, int aLine, const char* aFunct)
{
	const char* tmp = e.errorMessage(); 
	int iSize = sizeof(tmp)+1;
	int i=0;
	if( aLine != 0 ) iSize += 13;
	if( aFunct != NULL ) iSize += sizeof(aFunct)+2;
	strError = new char[iSize];
	if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
	if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
	i += sprintf(strError + i, "%s ", tmp);
	Log::instance().Trace( 5,"%s", strError );
}
CPrvException::CPrvException(std::exception& e, int aLine, const char* aFunct)
{
	const char* tmp = e.what(); 
	int iSize = sizeof(tmp)+1;
	int i=0;
	if( aLine != 0 ) iSize += 13;
	if( aFunct != NULL ) iSize += sizeof(aFunct)+2;
	strError = new char[iSize];
	if( aFunct != NULL ) i += sprintf(strError + i, "%s->", aFunct);
	if( aLine != 0 ) i += sprintf(strError + i, "[%d] ", aLine);
	i += sprintf(strError + i, "%s ", tmp);
	Log::instance().Trace( 5,"%s", strError );
}
CPrvException::~CPrvException()
{
	delete[] strError;
}
const char* CPrvException::Message()
{
	return strError;
}
