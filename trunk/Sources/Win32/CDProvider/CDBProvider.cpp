/////////////////////////////////////////////////////////////////////////////
// ����� ���� ������                                                       //
//                                                                         //
// TableHostes  ������� ��������� �������� � ���� ����������               //
//   IDhost        INT            ������ ����������                        //
//   IPNum         CHAR(15)       IP ����������                            //
//   HostName      TINYTEXT       ��� ����������                           //
//   DateRef       DATETIME       ����� ����������� ���������� ����������  //
// ----------------------------------------------------------------------- //
// TableFiles   �������� ������ � �������� �� � ����������                 //
//   IDfile        INT            ������ �����                             //
//   IDhost        INT            ������ ����������                        //
//   FileName      TINYTEXT       ��� �����                                //
//   FileSize      INT            ������ �����                             //
//   FileTimeCr    DATETIME       ����� �������� �����                     //
// ----------------------------------------------------------------------- //
// TableWords   ������� ���� �������� ����                                 //
// { ���� � ������ ����������� �� ����� (������) � ��������� � �������.  } //
// { ��� ������� ��� ��������� ������ ���������.                         } //
//   IDword        INT            ������ �����                             //
//   Word          TINYTEXT       "������" (�����)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   ������������ ���� ������ � ���� �������������� ����� //
//   IDword        INT            ������ �����                             //
//   IDfile        INT            ������ �����                             //
//   IsPath        BOOL           "������" - ����� ����?                   //
/////////////////////////////////////////////////////////////////////////////

#include "CDBProvider.h"
const char sepFile[] = "\\/,;:<>|. &()[]{}-_";

CDBProvider::CDBProvider()
{
}

CDBSQLitProvider::CDBSQLitProvider(const char* szFile): CDBProvider()
{
	Log::instance().Trace( 100, "CDBSQLitProvider::CDBSQLitProvider ::: Open BD" );
	db.open(szFile); // �������� ���� ���� ������
	
	// ������ ������� ��� �� ����������
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableHostes" );
	if(!db.tableExists("TableHostes"))
		db.execDML("create table TableHostes(IDhost AUTO_INCREMENT INT NOT NULL, IPNum char(15) NOT NULL, HostName tinytext, DateRef datetime, PRIMARY KEY (IPNum));");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableFileaaaaaaaaaaaaaaaaaaaaaaaaaas" );
	if(!db.tableExists("TableFiles"))
		db.execDML("create table TableFiles(IDfile AUTO_INCREMENT INT NOT NULL, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr datetime, PRIMARY KEY (IDfile));");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableWords" );
	if(!db.tableExists("TableWords"))
		db.execDML("create table TableWords(IDword AUTO_INCREMENT INT NOT NULL, Word tinytext NOT NULL, PRIMARY KEY (Word));");
	
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableWordInFiles" );
	if(!db.tableExists("TableWordInFiles"))
	{
		db.execDML("create table TableWordInFiles(IDword INT NOT NULL, IDfile INT NOT NULL, IsPath BOOL NOT NULL DEFAULT false);");
		db.execDML("create index IDword ON TableWordInFiles (IDword);");
	}
}

CDBProvider::~CDBProvider()
{
}

CDBSQLitProvider::~CDBSQLitProvider()
{
}

char* CDBProvider::GetTimeStr(time_t iTime, char* strRes)
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
		sprintf(strRes, "%.4d.%.2d.%.2d %.2d:%.2d:%.2d\x0",nt->tm_year+1900, nt->tm_mon+1, nt->tm_mday,
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
}
//-----------------------------------------------------------------------------

time_t CDBProvider::ConvertFileTimeToUTC(DWORD lFTime, DWORD hFTime)
{
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
void CDBSQLitProvider::AddFiles(pFilesStr aFileList, string aHostName, string aIPnum)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::AddFiles [enter]" );
	int i=-1; // ������ ID ������� ������
	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
	filesStr::iterator Cur; // ��������� �� ������� ���� ������
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  bufSQL.format("select * from TableHostes where IPNum = %Q;", aIPnum.c_str());

	Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	CppSQLite3Query q = db.execQuery(bufSQL); // ������

	Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
    if(!q.eof())
    	i = q.getIntField("IDhost", -1); // ������� ����� ID ��������� �� ������������ ������
    									 // � ������� IP ����������� � aIPnum
    q.finalize();

// ������ ����������    
//	db.execDML("begin transaction;");

    if(i >= 0) // ���� ������ ����������
    {
			Log::instance().Trace( 180, "%d :: ������ ��� ����������", __LINE__ );
    	bufSQL.format("delete from TableWordInFiles where IDfile = (select IDfile from TableFiles where IDhost = %d);", i);
   		db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� (����-����)
	    
    	bufSQL.format("delete from TableFiles where IDhost = %d;", i);
    	db.execDML(bufSQL); // ������ ��� ������ ���������������� ID �� ������� ������

    	bufSQL.format("update TableHostes set DateRef = %Q where IDhost = %d;",GetTimeStr(0, strTmp),i);
    	db.execDML(bufSQL); // �������� ����/����� ���������� ����������
    }
    else // �����
    {
			Log::instance().Trace( 180, "%d :: ��������� ����� ������", __LINE__ );
    	if (0 != db.execScalar("select count(0) from TableHostes;"))
    		i = db.execScalar("select MAX(IDhost) from TableHostes;");
    	i++;  // ��������� ��������� ����� ID ������� ������
    	bufSQL.format("insert into TableHostes values(%d, %Q, %Q, %Q);", i, aIPnum.c_str(), aHostName.c_str(), GetTimeStr(0, strTmp));
    	db.execDML(bufSQL); // �������� ����� ������ � ������� ������
    }
    
    int j = -1;
		Log::instance().Trace( 180, "%d :: ����� ����� ������", __LINE__ );
   	if (0 != db.execScalar("select count(0) from TableFiles;"))
   		j = db.execScalar("select MAX(IDfile) from TableFiles;");
   	j++; // ��������� ��������� ����� ID ������� ������
   	
 	for ( Cur = (*aFileList).begin(); Cur != (*aFileList).end(); Cur++ )
	{
//   		printf("insert into TableFiles values(%d, %d, %s, %d, %s);", j, i, (*Cur).FileName.c_str(),(*Cur).FileSize, GetTimeStr((*Cur).FileTime, strTmp));
			Log::instance().Trace( 190, "%d :: �������� ����� ����", __LINE__);
			GetTimeStr(ConvertFileTimeToUTC((*Cur).lFileTime, (*Cur).hFileTime), strTmp);
			Log::instance().Trace( 185, "%d :: ��������� ����� ������ [%s]", __LINE__,  (*Cur).FileName.c_str());
   		bufSQL.format("insert into TableFiles values(%d, %d, %Q, %d, %Q);", j, i, (*Cur).FileName.c_str(), (*Cur).FileSize, strTmp);
			Log::instance().Trace( 190, "%d :: ��������� � ��", __LINE__);
   		db.execDML(bufSQL);
			Log::instance().Trace( 185, "%d :: ��������� ����� ����� �� ������ [%s]", __LINE__,  (*Cur).FileName.c_str());
   		AddWord(j,(*Cur).FileName);
   		j++;
  }
// ���������� ����������
//	db.execDML("commit transaction;");
	Log::instance().Trace( 100, "CDBSQLitProvider::AddFiles [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWord(int aID, string aPath)
// ��������� ��� ����� �� ������ � ��������� � ������ ���� � ������������ ����� ������
// aID    ����� ����� � ������� ������
// aPath  ������ ��� �����
void CDBSQLitProvider::AddWord(int aID, string aPath)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::AddWord [enter]" );
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
void CDBProvider::Split(string & text, string separators, list<string> & words)
{
	Log::instance().Trace( 120, "CDBProvider::Splite [enter]" );
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
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// ��������� ����� � ������� ����
// aID     ����� �����
// words   ������ ����
// IsPath  ����� ������ � ���� � �����
void CDBSQLitProvider::AddWordInTable(int aID, list<string> & words, bool IsPath)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [enter]" );
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
 			i = q.getIntField("IDword", -1);
 		q.finalize();
		if(i < 0)
	  {
   		if (0 != db.execScalar("select count(0) from TableWords;"))
   			i = db.execScalar("select MAX(IDword) from TableWords;");
   		i++;  // ��������� ��������� �����
   		bufSQL.format("insert into TableWords values(%d, %Q);", i, (*wptr).c_str());
   		db.execDML(bufSQL); // �������� ����� ������ � �������
   	}
		bufSQL.format("insert into TableWordInFiles values(%d, %d, %d);", i, aID, IsPath);
		db.execDML(bufSQL); //�������� ����� ����������� ������ �����
	}
	Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [exit]" );
}
//-----------------------------------------------------------------------------

void CDBProvider::FormatSQLQuery(string aText, string & aResult)
{
	aResult="";
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
				aResult += "\\%";
				break;
			case '_':
				aResult += "\\_";
				break;
			default:
				aResult += aText[i];
		}
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
bool CDBSQLitProvider::Find(string aText, map<string,bool> & aParams, list<int> & Result)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::Find [enter]" );
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
	list<string> wrd; // ������ ���� �� �����
//	bool CommASAnd=true; //����������� ����� ������� AND
//	list<string> varsep; //������ ����������� �����������
	list<string>::iterator wptr;//, prev, next;//, sptr;
	list<int> IDWord;
	list<int> IDFile;
	list<int>::iterator idwPtr, idfPtr;
	string temp;
	int idT;
	FormatSQLQuery(aText, temp);
	Split(aText, sepFile, wrd);

  for (wptr=wrd.begin(); wptr != wrd.end(); ++wptr)
  {
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
  for (idwPtr=IDWord.begin(); idwPtr != IDWord.end(); ++idwPtr)
  {
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
  if(idFiles->IsEmpty())
  {
  	bufSQL.format("select IDfile from TableFiles where FileName LIKE %%%s%%;", temp.c_str());
  }else{
  	bufSQL.format("select IDfile from TableFiles where IDfile IN (%s) AND FileName LIKE \%%s\%;", tmpS.c_str(), temp.c_str());
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
  return (Result.size()>0);
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
bool CDBSQLitProvider::Search(string aText, map<string,bool> & aParams, list<int> & Result)
{
		return Find(aText, aParams, Result);
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
