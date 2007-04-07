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

#ifdef __BCPLUSPLUS__ 
  #pragma hdrstop
  #pragma package(smart_init)
  #define __FUNCTION__ __FUNC__
  #define _itoa itoa
#endif

#include "CDBSQLite3.h"
//---------------------------------------------------------------------------
const char sepFile[] = "\\/,;:<>|. &()[]{}-_";

static char* pParamTypes[] = {
	LOG_LEVEL, "int",
	DB_FILE,   "string"
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
CDBSQLitProvider::CDBSQLitProvider()//: CDBProvider()
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
	FAutoIndex = false;
	int iLogLevel;
	string dbFile;

try
{
	//������������� ������
	Settings::SetModule( "SQLite", pParamTypes, sizeof( pParamTypes )/sizeof( pParamTypes[0] ) );
	Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
	Log::instance().SetLoglevel( iLogLevel );	
	Settings::instance().GetParam( DB_FILE, dbFile );

	Log::instance().Trace( 100, "CDBSQLitProvider::%s ::: Open BD", __FUNCTION__ );
	db.open(dbFile.c_str()); // �������� ���� ���� ������

	// ������ ������� ��� �� ���������� //INTEGER PRIMARY KEY <-AUTO_INCREMENT
	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableHostes" );
	if(!db.tableExists("TableHostes"))
		db.execDML("create table TableHostes(IDhost INTEGER PRIMARY KEY, IPNum char(15) NOT NULL, HostName tinytext, DateRef int);");

	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableFiles" );
	if(!db.tableExists("TableFiles"))
		db.execDML("create table TableFiles(IDfile INTEGER PRIMARY KEY, IDhost int NOT NULL, FileName tinytext NOT NULL, FileSize int, FileTimeCr int);");

	Log::instance().Trace( 110, "CDBSQLitProvider::CDBSQLitProvider ::: If don't exist CREATE TABLE TableWords" );
	if(!db.tableExists("TableWords"))
	{
		db.execDML("create table TableWords(IDword INTEGER PRIMARY KEY, Word tinytext NOT NULL);");
		db.execDML("create index Word ON TableWords (Word);");
	}

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
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
CDBSQLitProvider::~CDBSQLitProvider()
{
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddFiles(pFilesStr aFileList, const char* aHostName, const char* aIPnum)
// ��������� ������ ������ � ��
// aFileList  ��������������� ������ �� ������ ��������� � ����������� �����
// aHostName  ��� ���������� �� ������ ��������� ������������� ������
// aIPnum     IP ������ ����������
void __stdcall CDBSQLitProvider::AddFiles(hostRecords &aRec)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
	int i,j; // ������ ID ������� ������
//	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
	filesStr::iterator Cur; // ��������� �� ������� ���� ������
	hostRecords::iterator Rec;
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
	CppSQLite3Query q;
	
// ������ ����������    
	db.execDML("begin transaction;");

	for( Rec=aRec.begin(); Rec!=aRec.end(); Rec++ )
	{
		Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
		i = -1;
  	bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", Rec->IPNum);
		q = db.execQuery(bufSQL); // ������

		Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
  	if(!q.eof())
  		i = q.getIntField("IDhost", -1); // ������� ����� ID ��������� �� ������������ ������
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
   		bufSQL.format("insert into TableHostes values(NULL, %Q, %Q, strftime('%%s','now'));", Rec->IPNum, Rec->HostName.c_str());
   		db.execDML(bufSQL); // �������� ����� ������ � ������� ������

			Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	  	bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", Rec->IPNum);
			q = db.execQuery(bufSQL); // ������

			Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
   		if(!q.eof())
   			i = q.getIntField("IDhost", -1); // ������� ����� ID
   		q.finalize();
   		if( i < 0 ) throw CPrvException("������ � ��������� ��. (������� ������)", __LINE__);
		}
    
		Log::instance().Trace( 180, "%d :: �������� ID ��������� ������", __LINE__ );
	  bufSQL.format("select MAX(IDfile) as Cnt from TableFiles;", i);
		q = db.execQuery(bufSQL); // ������
		Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
		j = -1;
   	if(!q.eof())
 			j = q.getIntField("Cnt", -1); // ������� ����� ID ��������� �� ������������ ������
   	q.finalize();
		
		CppSQLite3Statement stmt_File = db.compileStatement("insert into TableFiles values (?, ?, ?, ?, ?);");
 		for ( Cur = Rec->Files.begin(); Cur != Rec->Files.end(); Cur++ )
		{
			Log::instance().Trace( 185, "%d :: ��������� ����� ������ [%s]", __LINE__,  Cur->FileName.c_str());
	   	j++;

			stmt_File.bind(1,j);
    	stmt_File.bind(2, i);
    	stmt_File.bind(3, Cur->FileName.c_str());
    	stmt_File.bind(4, Cur->FileSize);
    	stmt_File.bind(5, (int)Tools::ConvertFileTimeToUTC(Cur->FDate));
    	stmt_File.execDML();
    	stmt_File.reset();

			if( IsAutoIndex() )
			{
				Log::instance().Trace( 185, "%d :: ��������� ����� ����� �� ������ [%s]", __LINE__,  Cur->FileName.c_str());
	   		AddWord(j, Cur->FileName);//*/
			}
  	}
  	stmt_File.finalize();
	}
// ���������� ����������
	db.execDML("commit transaction;");
	
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
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
void __fastcall CDBSQLitProvider::AddWord(int aID, const string& aPath)
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
		Tools::Split(tmp, sepFile, wrd); 
		AddWordInTable(aID, wrd, true);
	}
	wrd.clear();

	if( (n+1)<(int)aPath.size() ) //���� ����� ����� ���� ������� �� ��� ��� �����
	{
		tmp = aPath.substr(n+1,aPath.size()-(n+1));
		Tools::Split(tmp, sepFile, wrd); 
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
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::AddWord [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// ��������� ����� � ������� ����
// aID     ����� �����
// words   ������ ����
// IsPath  ����� ������ � ���� � �����
void __fastcall CDBSQLitProvider::AddWordInTable(int aID, list<string> &words, bool IsPath)
{
	Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
try
{
  CppSQLite3Buffer bufSQL; // ����� ��� ������������ �������
  CppSQLite3Query q;
  list<string>::iterator wptr;
  int j; 

	CppSQLite3Statement stmt_Word = db.compileStatement("insert into TableWords values (?, ?);");
	CppSQLite3Statement stmt_FileWord = db.compileStatement("insert into TableWordInFiles values (?, ?, ?);");
	for (wptr = words.begin(); wptr != words.end(); ++wptr)
 	{
 		j = -1; //������ ������ � �������
		bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		q = db.execQuery(bufSQL); // ������
 		if(!q.eof())
 			j = q.getIntField("IDword", -2);
 		q.finalize();
		if(j == -1)
	  {
			stmt_Word.bindNull(1);
    	stmt_Word.bind(2, (*wptr).c_str());
    	stmt_Word.execDML();
    	stmt_Word.reset();
	  	
			Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
		  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
			q = db.execQuery(bufSQL); // ������

			Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
	   	if(!q.eof())
   			j = q.getIntField("IDword", -2); // ������� ����� ID ��������� �� ������������ ������
		 	q.finalize();
   	}
   	if( j < 0 ) throw CPrvException("������ � ��! (������� ����)", __LINE__);
		stmt_FileWord.bind(1, j);
    stmt_FileWord.bind(2, aID);
    stmt_FileWord.bind(3, IsPath);
    stmt_FileWord.execDML();
    stmt_FileWord.reset();
	}
	stmt_Word.finalize();
	stmt_FileWord.finalize();
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Find(string aText, list<string> & aParams)
// ������� ���� ������� ��������� � ���� ������
// ������ ������������ ����� ������ ��������������� ������ ��������
// �������.
// ��� �������, �� ����������� ? � * �������� �������� ��������� ������ ��
// ������� ������������ �����.
bool __fastcall CDBSQLitProvider::Find(const string& aText, map<string,bool> &aParams, list<int> &Result)
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
	CExcerpts *idFiles = NULL;
	
	Tools::FormatSQLQuery(aText, temp);
	
	if( aParams["IndexFind"] )
	{
		Tools::Split(temp, sepFile, wrd);

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

  	idFiles = new CExcerpts();
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
	}
	
	Log::instance().Trace( 180, "%d :: ���� ����� �������������� ��������� %s", __LINE__, temp.c_str() );
	if( '%' == temp[0] )
		temp = temp.substr(1, temp.length()-1);
	if( '%' == temp[temp.length()-1] )
		temp = temp.substr(0, temp.length()-1);

	if( !aParams["IndexFind"] )
	{
	  int iTmp;
  	bufSQL.format("select IDfile from TableFiles where FileName LIKE '%%%s%%';", temp.c_str());
	  q = db.execQuery(bufSQL);
	  while(!q.eof())
  	{
    	iTmp = q.getIntField("IDfile", -1);
    	if(iTmp>=0)Result.push_back(iTmp);
	  	q.nextRow();
 		}
	}else if(!idFiles->IsEmpty()){
  	string tmpS;
  	idFiles->GetAsString(tmpS);
  	bufSQL.format("select IDfile from TableFiles where IDfile IN (%s) AND FileName LIKE '%%%s%%';", tmpS.c_str(), temp.c_str());
	  q = db.execQuery(bufSQL);
	  while(!q.eof())
  	{
			idFiles->Transact(q.getIntField("IDfile", -1), false);
	  	q.nextRow();
 		}
	  idFiles->TransactEnd();
	  idFiles->GetAsList(Result);
  }else Result.clear();
  q.finalize();
  if( NULL != idFiles )
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
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
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
bool __stdcall CDBSQLitProvider::Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
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
  bool bResult;
  bResult = Find(aText, aParams, res);
	if( bResult )
	{
	  id=res.begin();
	  if(id != res.end())
		{
	  	temp = _itoa((*id), ctmp, 10);
			id++;
		}
	  for (; id != res.end(); ++id)
	  {
			temp += ", ";
			temp += _itoa((*id), ctmp, 10);
	  }
//  	bufSQL.format("select th.IDHost as IDn, HostName, IPNum, FileName, FileSize, strftime('%%s',FileTimeCr) as FTUTS FROM TableFiles AS tf, TableHostes AS th WHERE tf.IDfile IN (%s) AND th.IDhost=tf.IDhost ORDER BY th.IDhost;", temp.c_str());
  	bufSQL.format("select th.IDHost as IDn, HostName, IPNum, FileName, FileSize, strftime('%%s',FileTimeCr) as FTUTS FROM TableFiles AS tf, TableHostes AS th WHERE tf.IDfile IN (%s) AND th.IDhost=tf.IDhost ORDER BY th.IDhost;", temp.c_str());
    q = db.execQuery(bufSQL); // ������
	  int curH = -1;
	  int iTmp;
	  while(!q.eof())
  	{
  		iTmp = q.getIntField("IDn", -2);
  		if ( curH != iTmp && iTmp != -2 )
  		{
  			if ( curH != -1 ) Result.push_back(hRec);
  			curH = iTmp;
	  		hRec.HostName = q.getStringField("HostName", "");
  			strcpy(hRec.IPNum, q.getStringField("IPNum", ""));
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
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// ���� � ������� ���������� ��������� time � �.�. ����������� ���������� �
void __stdcall CDBSQLitProvider::EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles)
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
  	Tools::FormatSQLQuery(aHostName, temp);
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
   	bufSQL.format("delete from TableWordInFiles where IDfile IN(select IDfile from TableFiles where IDhost = %d);", (*pIDhost));
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
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

	Log::instance().Trace( 100, "CDBSQLitProvider::%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// ���� � ������� ���������� ��������� time � �.�. ����������� ���������� �
char* __stdcall CDBSQLitProvider::GetNamePlugin()
{
  return "SQLite3";
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
time_t __stdcall CDBSQLitProvider::GetRefDateHost(const string& aHostName, const string& aIPnum)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
try
{
	time_t i; // ������ ID ������� ������
//	char strTmp[255]; // ��������������� ������ (������ ��� �������� ���� � ��������� �����)
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
  	Tools::FormatSQLQuery(aHostName, temp);
  	temp += "'";
  }

  if( &aIPnum != NULL && !aIPnum.empty() )
  {
  	if( temp.empty() )
  		temp = " WHERE ";
  	else temp += " AND ";
  	temp += "IPnum = '" + aIPnum + "'";
  }

	if( temp.empty() )return 0;
  
	bufSQL.format("select DateRef from TableHostes%s;", temp.c_str());

	Log::instance().Trace( 180, "%d :: ������ � ���� ������", __LINE__ );
	CppSQLite3Query q = db.execQuery(bufSQL); // ������

	Log::instance().Trace( 180, "%d :: ��������� �������", __LINE__ );
  if(!q.eof())
  	i = q.getIntField("IDhost", 0);
  q.finalize();

	Log::instance().Trace( 100, "%s [return %d]", __FUNCTION__, i );
  return i;
}catch( CPrvException& e )
{
	throw CPrvException(e.Message(), 0, __FUNCTION__);
}catch( std::exception& e )
{
	throw CPrvException(e, 0, __FUNCTION__);
}catch(CppSQLite3Exception&  e)
{
	throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
}catch(...)
{
	throw CPrvException("�������� ����������� ����������", 0, __FUNCTION__);
}

}
//-----------------------------------------------------------------------------
void __stdcall CDBSQLitProvider::SetAutoIndex(bool aVal)
{
	if( aVal != FAutoIndex )
	{
		FAutoIndex = aVal;
	}
}
//-----------------------------------------------------------------------------
bool __stdcall CDBSQLitProvider::IsAutoIndex()
{
	return FAutoIndex;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
CExcerpts::CExcerpts(int aCount)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	Init(aCount);
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
CExcerpts::CExcerpts()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	Init(1000);
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void CExcerpts::Init(int aCount)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
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
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
CExcerpts::~CExcerpts()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	free(Memory);
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
int CExcerpts::Find(int aVal)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	for(int i = 0; i < Length; i++)
		if(Memory[i]==aVal)return i;
	return -1;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void CExcerpts::Transact(int aVal, bool aisFirst)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
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
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void CExcerpts::TransactEnd()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	Length = Curr;
	Curr = 0;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void CExcerpts::GetAsList(list<int> & Res)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	for(int i = 0; i < Length; ++i)
		Res.push_back(Memory[i]);
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
void CExcerpts::GetAsString(string & Res)
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	char tmp[12];
	for(int i = 0; i < Length; ++i)
	{
		sprintf(tmp, "%d", Memory[i]);
		if( i != 0) Res += ", ";
		Res += tmp;
	}
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
bool CExcerpts::IsEmpty()
{
	Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
	return (Length <= 0);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

