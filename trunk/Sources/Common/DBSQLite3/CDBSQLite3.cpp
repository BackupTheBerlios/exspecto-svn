/////////////////////////////////////////////////////////////////////////////
// ����������՜��ܜ��� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������                                                       //
//                                                                         //
// TableHostes  ������М��ќ��ۜ��؜������ ���ޜ��ߜ��؜��������Ҝ��М��՜��� ���ݜ��М��ٜ��Ԝ��՜��ݜ������ ���� ������՜������ ���ڜ��ޜ��ܜ��ߜ�����������՜�������               //
//   IDhost        INTEGER PRIMARY KEY �������ݜ��Ԝ��՜��ڜ��� ���ڜ��ޜ��ܜ��ߜ�����������՜�������                   //
//   IPNum         CHAR(15)       IP ���ڜ��ޜ��ܜ��ߜ�����������՜�������                            //
//   HostName      TINYTEXT       �������ܜ��� ���ڜ��ޜ��ܜ��ߜ�����������՜�������                           //
//   DateRef       INT            �����������՜��ܜ��� ���ߜ��ޜ�����ۜ��՜��Ԝ��ݜ��՜��ݜ��Ӝ��� ���ޜ��ќ��ݜ��ޜ��Ҝ��ۜ��՜��ݜ��؜��� ���؜��ݜ�����ޜ������ܜ��М�����؜���  //
// ----------------------------------------------------------------------- //
// TableFiles   ���ޜ��ߜ��؜�����М��ݜ��؜��� ������М��ٜ��ۜ��ޜ��� ���� ���ߜ������؜��Ҝ�����ל��ڜ��� ���؜��� ���� ���ڜ��ޜ��ܜ��ߜ�����������՜�������                 //
//   IDfile        INTEGER PRIMARY KEY �������ݜ��Ԝ��՜��ڜ��� ������М��ٜ��ۜ���                        //
//   IDhost        INT            �������ݜ��Ԝ��՜��ڜ��� ���ڜ��ޜ��ܜ��ߜ�����������՜�������                        //
//   FileName      TINYTEXT       �������ܜ��� ������М��ٜ��ۜ���                                //
//   FileSize      INT            �������М��ל��ܜ��՜��� ������М��ٜ��ۜ���                             //
//   FileTimeCr    INT            �����������՜��ܜ��� ������ޜ��ל��Ԝ��М��ݜ��؜��� ������М��ٜ��ۜ���                     //
// ----------------------------------------------------------------------- //
// TableWords   ������М��ќ��ۜ��؜������ ���Ҝ�����՜��� ���ݜ��М��ٜ��Ԝ��՜��ݜ������ ������ۜ��ޜ���                                 //
// { �������������� ���� ������М��ٜ��ۜ��М��� �������М��ל��ќ��؜��Ҝ��М������������ ���ݜ��� ������ۜ��ޜ��Ҝ��� (���؜��Ԝ��؜��ޜ��ܜ���) ���� ���ל��М��ݜ��ޜ��������������� ���� ������М��ќ��ۜ��؜������.  } //
// { ���͜������ ������Ԝ��՜��ۜ��М��ݜ��� ���Ԝ��ۜ��� ���������ڜ��ޜ������՜��ݜ��؜��� ���ߜ��ޜ��؜�����ڜ��� ���Ҝ���������М��֜��՜��ݜ��؜���.                         } //
//   IDword        INTEGER PRIMARY KEY �������ݜ��Ԝ��՜��ڜ��� ������ۜ��ޜ��Ҝ���                        //
//   Word          TINYTEXT       "�������Ԝ��؜��ޜ��ܜ���" (������ۜ��ޜ��Ҝ���)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   ������ޜ��ޜ�����Ҝ��՜�����������Ҝ��؜��� ������ۜ��ޜ��� ������М��ٜ��ۜ��М��� ���� ������ۜ��М��� ���ߜ������؜��ݜ��М��Ԝ��ۜ��՜��֜��ݜ��ޜ��������� ������ۜ��ޜ��Ҝ��� //
//   IDword        INT            �������ݜ��Ԝ��՜��ڜ��� ������ۜ��ޜ��Ҝ���                             //
//   IDfile        INT            �������ݜ��Ԝ��՜��ڜ��� ������М��ٜ��ۜ���                             //
//   IsPath        BOOL           "�������Ԝ��؜��ޜ��ܜ���" - ������М��������� ���ߜ���������?                   //
/////////////////////////////////////////////////////////////////////////////

#ifdef __BCPLUSPLUS__ 
#pragma hdrstop
#pragma package(smart_init)
#define __FUNCTION__ __FUNC__
#define _itoa itoa
#endif

#include "CDBSQLite3.h"
#include <sstream>
//---------------------------------------------------------------------------
const char sepFile[] = "\\/,;:<>|. &()[]{}-_";

static char* pParamTypes[] = {
  LOG_LEVEL, "int",
  DB_FILE,   "string",
  DB_AUTOINDEX_WORD, "int"
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
CDBSQLitProvider::CDBSQLitProvider()//: CDBProvider()
{
  Log::instance().SetModuleName( "SQLite" );
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
	
  FAutoIndex = false;
  int iLogLevel, bAI;
  string dbFile;

  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  //�������ݜ��؜�����؜��М��ۜ��؜��ל��М�����؜��� ���ۜ��ޜ��Ӝ��՜�������
		  Settings::instance().SetModule( "SQLite", pParamTypes, sizeof( pParamTypes )/sizeof( pParamTypes[0] ) );
		  Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
		  Log::instance().SetLoglevel( iLogLevel );	
		  Settings::instance().GetParam( DB_FILE, dbFile );
		  Settings::instance().GetParam( DB_AUTOINDEX_WORD, bAI );
		  FAutoIndex = bAI;

		  Log::instance().Trace( 100, "CDBSQLitProvider::%s ::: Open BD", __FUNCTION__ );
		  db.open(dbFile.c_str()); // ����������ڜ���������Ҝ��М��� ������М��ٜ��� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������

		  // �������ޜ��ל��Ԝ��М��� ������М��ќ��ۜ��؜������ ���ߜ������� ���؜��� ���ޜ��������������������Ҝ��؜��� //INTEGER PRIMARY KEY <-AUTO_INCREMENT
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
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__);
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
	  }catch(...){
		throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	  }
	}catch( CPrvException& e ){
	int errc = e.ErrorCode();
	if( errc == RESULT_OK ) errc = 1;
	SetLastError(errc, e.Message());
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
// �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ������ߜ��؜�����ޜ��� ������М��ٜ��ۜ��ޜ��� ���� ��������
// aFileList  ���ޜ��Ԝ��ݜ��ޜ��ݜ��М��ߜ������М��Ҝ��ۜ��՜��ݜ������ ������ߜ��؜�����ޜ��� ���ݜ��� ���ޜ�����ݜ��ޜ��Ҝ��� ����������������ڜ������������� ���� ���ߜ��М������М��ܜ��՜���������М��ܜ��� ������М��ٜ��ۜ���
// aHostName  ���؜��ܜ��� ���ڜ��ޜ��ܜ��ߜ�����������՜������� ���ݜ��� ���ڜ��ޜ�����ޜ������� ���ݜ��М�����ޜ��Ԝ��؜��ۜ������ ���ߜ������՜��Ԝ��������М��Ҝ��ۜ��՜��ݜ������ ������ߜ��؜�����ޜ���
// aIPnum     IP ���М��Ԝ������՜������ ���ڜ��ޜ��ܜ��ߜ�����������՜�������
void CDBSQLitProvider::AddFiles(hostRec &aRec)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  int i,j; // ����������М��ݜ��؜��� ID ������М��ќ��ۜ��؜������ ������ޜ��������ޜ���
		  //	char strTmp[255]; // ���Ҝ�����ߜ��ޜ��ܜ��ޜ��Ӝ��Ҝ�����՜��ۜ�����ݜ������ ���ܜ��М��������؜��� (���Ҝ��Ҝ��՜��Ԝ��՜��� ���Ԝ��ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���Ԝ��М������ ���� �������������ޜ��ڜ��ޜ��Ҝ��ޜ��� ������ޜ������ܜ���)
		  filesStr::iterator Cur; // ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ��� ������՜��ڜ��������؜��� ������М��ٜ��� ������ߜ��؜�����ڜ���
		  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
		  CppSQLite3Query q;
	
		  // �������М�����М��ۜ��� ����������М��ݜ��ל��М��ڜ�����؜���    
		  db.execDML("begin transaction;");

		  Log::instance().Trace( 180, "%d :: �������М��ߜ������ޜ��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������", __LINE__ );
		  i = -1;
		  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aRec.IPNum.c_str());
		  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���

		  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
		  if(!q.eof())
  			i = q.getIntField("IDhost", -1); // ���ߜ��ޜ��ۜ��������М��� ���ݜ��ޜ��ܜ��՜��� ID ������ڜ��М��ל�����������؜��� ���ݜ��� ������������՜��������Ҝ��������������� ���ל��М��ߜ��؜������
		  q.finalize();

		  if(i >= 0) // ���՜�����ۜ��� ���ל��М��ߜ��؜������ ������������՜��������Ҝ�����՜���
			{
			  Log::instance().Trace( 180, "%d :: �������М��ߜ��؜������ ������֜��� ������������՜��������Ҝ�����՜���", __LINE__ );
			  bufSQL.format("update TableHostes set DateRef = strftime('%%s','now') where IDhost = %d;",i);
			  db.execDML(bufSQL); // ���ޜ��ќ��ݜ��М��Ҝ��ۜ������ ���Ԝ��М������/���Ҝ������՜��ܜ��� ���ߜ��ޜ�����ۜ��՜��Ԝ��ݜ��՜��Ӝ��� ���ޜ��ќ��ݜ��М��Ҝ��ۜ��՜��ݜ��؜���
			}
		  else // ���؜��ݜ��М������
			{
			  Log::instance().Trace( 180, "%d :: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ݜ��ޜ��Ҝ������ ���ל��М��ߜ��؜������", __LINE__ );
			  bufSQL.format("insert into TableHostes values(NULL, %Q, %Q, strftime('%%s','now'));", aRec.IPNum.c_str(), aRec.HostName.c_str());
			  db.execDML(bufSQL); // ���Ԝ��ޜ��ќ��М��Ҝ��ۜ������ ���ݜ��ޜ��Ҝ������ ���ל��М��ߜ��؜������ ���� ������М��ќ��ۜ��؜������ ������ޜ��������ޜ���

			  Log::instance().Trace( 180, "%d :: �������М��ߜ������ޜ��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������", __LINE__ );
			  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aRec.IPNum.c_str());
			  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
	
			  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
			  if(!q.eof())
   				i = q.getIntField("IDhost", -1); // ���ߜ��ޜ��ۜ��������М��� ���ݜ��ޜ��ܜ��՜��� ID
			  q.finalize();
			  if( i < 0 ) throw CPrvException("����������؜��ќ��ڜ��� ���� ����������������ڜ������������� ��������. (�����М��ќ��ۜ��؜������ ���Ŝ��ޜ��������ޜ���)", __LINE__);
			}
    
		  Log::instance().Trace( 180, "%d :: �������ޜ��ۜ��������؜������ ID ������ۜ��՜��Ԝ�����������՜��� ���ל��М��ߜ��؜������", __LINE__ );
		  bufSQL.format("select MAX(IDfile) as Cnt from TableFiles;", i);
		  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
		  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
		  j = -1;
		  if(!q.eof())
			j = q.getIntField("Cnt", -1); // ���ߜ��ޜ��ۜ��������М��� ���ݜ��ޜ��ܜ��՜��� ID ������ڜ��М��ל�����������؜��� ���ݜ��� ������������՜��������Ҝ��������������� ���ל��М��ߜ��؜������
		  q.finalize();

		  CppSQLite3Statement stmt_File = db.compileStatement("insert into TableFiles values (?, ?, ?, ?, ?);");
		  for ( Cur = aRec.Files.begin(); Cur != aRec.Files.end(); Cur++ )
			{
			  Log::instance().Trace( 185, "%d :: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ݜ��ޜ��Ҝ������ ���ל��М��ߜ��؜������ [%s]", __LINE__,  Cur->FileName.c_str());
			  j++;

			  stmt_File.bind(1,j);
			  stmt_File.bind(2, i);
			  stmt_File.bind(3, Cur->FileName.c_str());
			  stmt_File.bind(4, Cur->FileSize);
			  stmt_File.bind(5, (int)Cur->FDate.UTS);
			  stmt_File.execDML();
			  stmt_File.reset();

			  if( IsAutoIndex() )
				{
				  Log::instance().Trace( 185, "%d :: �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ���ݜ��ޜ��Ҝ������ ������ۜ��ޜ��Ҝ��� ���؜��� �������������ޜ��ڜ��� [%s]", __LINE__,  Cur->FileName.c_str());
				  AddWord(j, Cur->FileName);//*/
				}
			}
		  stmt_File.finalize();

		  // �������М��Ҝ��՜���������՜��ݜ��؜��� ����������М��ݜ��ל��М��ڜ�����؜���
		  db.execDML("commit transaction;");
	
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__, e.ErrorCode());
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__, e.errorCode());
	  }catch(...){
		throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	  }
	}catch( CPrvException& e ){
	int errc = e.ErrorCode();
	if( errc == RESULT_OK ) errc = 1;
	SetLastError(errc, e.Message());
  }

  Log::instance().Trace( 100, "CDBSQLitProvider::AddFiles [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWord(int aID, string aPath)
// �������М��ל��ќ��؜��Ҝ��М��՜��� ���؜��ܜ��� ������М��ٜ��ۜ��� ���ݜ��� ���؜��Ԝ��؜��ޜ��ܜ��� ���� ���Ԝ��ޜ��ќ��М��Ҝ��ۜ�����՜��� ���� ������ߜ��؜�����ޜ��� ������ۜ��ޜ��� ���� ������ޜ��ޜ�����Ҝ��՜�����������Ҝ��؜��� ������М��ٜ��ۜ��� ������ۜ��ޜ��Ҝ��М���
// aID    ���ݜ��ޜ��ܜ��՜��� ������М��ٜ��ۜ��� ���� ������М��ќ��ۜ��؜������ ������М��ٜ��ۜ��ޜ���
// aPath  ���ߜ��ޜ��ۜ��ݜ��ޜ��� ���؜��ܜ��� ������М��ٜ��ۜ���
void  CDBSQLitProvider::AddWord(int aID, const string& aPath)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  try
	{
	  list<string> wrd;
	  int n = (int)aPath.rfind("\\"); //���؜�����՜��� ���ߜ��՜������Ҝ������ �������М��ל��Ԝ��՜��ۜ��؜�����՜��ۜ�����ݜ������ ������ۜ��՜������ ������ߜ������М��Ҝ���
	  string tmp;
	  if( n>=0 ) // ���՜�����ۜ��� ������ۜ��՜������ ���ݜ��М��ٜ��Ԝ��՜��� ������� ����������М��ל��� ������ۜ��՜��Ҝ��� ���ޜ��� ������ۜ��՜������ ���������� ���ߜ���������
		{
		  tmp = aPath.substr(0,n);
		  Tools::Split(tmp, sepFile, wrd); 
		  AddWordInTable(aID, wrd, true);
		}
	  wrd.clear();

	  if( (n+1)<(int)aPath.size() ) //���՜�����ۜ��� ���ߜ��ޜ�����ۜ��� ������ۜ��՜������ ���՜��������� ������؜��ܜ��Ҝ��ޜ��ۜ��� ������� ���������� ���؜��ܜ��� ������М��ٜ��ۜ���
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
	  throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	}

  Log::instance().Trace( 100, "CDBSQLitProvider::AddWord [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// �������ޜ��ќ��М��Ҝ��ۜ�����՜��� ������ۜ��ޜ��Ҝ��� ���� ������М��ќ��ۜ��؜������ ������ۜ��ޜ���
// aID     ���ݜ��ޜ��ܜ��՜��� ������М��ٜ��ۜ���
// words   ������ߜ��؜�����ޜ��� ������ۜ��ޜ���
// IsPath  ������ۜ��ޜ��Ҝ��� ���Ҝ�����ޜ��Ԝ������ ���� ���ߜ��������� ���� ������М��ٜ��ۜ���
void  CDBSQLitProvider::AddWordInTable(int aID, list<string> &words, bool IsPath)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  try
	{
	  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
	  CppSQLite3Query q;
	  list<string>::iterator wptr;
	  int j; 

	  CppSQLite3Statement stmt_Word = db.compileStatement("insert into TableWords values (?, ?);");
	  CppSQLite3Statement stmt_FileWord = db.compileStatement("insert into TableWordInFiles values (?, ?, ?);");
	  for (wptr = words.begin(); wptr != words.end(); ++wptr)
		{
		  j = -1; //���؜��ݜ��Ԝ��՜��ڜ��� ���؜��Ԝ��؜��ޜ��ܜ��� ���� ������М��ќ��ۜ��؜������
		  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
		  if(!q.eof())
 			j = q.getIntField("IDword", -2);
		  q.finalize();
		  if(j == -1)
			{
			  stmt_Word.bindNull(1);
			  stmt_Word.bind(2, (*wptr).c_str());
			  stmt_Word.execDML();
			  stmt_Word.reset();
	  	
			  Log::instance().Trace( 180, "%d :: �������М��ߜ������ޜ��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������", __LINE__ );
			  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
			  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���

			  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
			  if(!q.eof())
				j = q.getIntField("IDword", -2); // ���ߜ��ޜ��ۜ��������М��� ���ݜ��ޜ��ܜ��՜��� ID ������ڜ��М��ל�����������؜��� ���ݜ��� ������������՜��������Ҝ��������������� ���ל��М��ߜ��؜������
			  q.finalize();
			}
		  if( j < 0 ) throw CPrvException("����������؜��ќ��ڜ��� ���� ��������! (�����М��ќ��ۜ��؜������ �������ۜ��ޜ���)", __LINE__);
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
	  throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	}

  Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Find(string aText, list<string> & aParams)
// ���������ݜ��ڜ�����؜��� ���؜�����՜��� ������՜��ۜ�����ݜ��ޜ��� ���Ҝ���������М��֜��՜��ݜ��؜��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������
// �����ޜ��՜��������� ���ߜ��՜������՜��Ԝ��М��Ҝ��М��՜��ܜ������ ������՜��ڜ������ ���Ԝ��ޜ��ۜ��֜��՜��� ������ޜ��ޜ�����Ҝ��՜�����������Ҝ��ޜ��Ҝ��М������ ���ޜ��Ԝ��ݜ��ޜ��ܜ��� ������ۜ��՜��ܜ��՜��ݜ������
// ���ל��М��ߜ������ޜ������.
// ����������� ������؜��ܜ��Ҝ��ޜ��ۜ���, ���ל��� ���؜�����ڜ��ۜ��������՜��ݜ��؜��՜��� ? ���� * ������Ҝ��ۜ��������������� ���ޜ��ќ��������ݜ�����ܜ��� ������؜��ܜ��Ҝ��ޜ��ۜ��М��ܜ��� �������������ޜ��ڜ��� ���ߜ���
// ���ڜ��ޜ�����ޜ���������� ���ߜ������ޜ��؜��ל��Ҝ��ޜ��Ԝ��؜��������� ���ߜ��ޜ��؜������.
bool  CDBSQLitProvider::Find(const string& aText, map<string,bool> &aParams, list<int> &Result)
{
  try
	{
	  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
	  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
	  CppSQLite3Query q;
	  list<string> wrd; // ���ܜ��М��������؜��� ������ۜ��ޜ��� ���؜��� ����������М��ל���
	  //	bool CommASAnd=true; //������ޜ��ޜ�����ݜ��ޜ�����՜��ݜ��؜��� ���ܜ��՜��֜��Ԝ��� ������ۜ��ޜ��Ҝ��М��ܜ��� AND
	  //	list<string> varsep; //���ܜ��М��������؜��� �������М��ל��Ԝ��՜��ۜ�����������؜��� ������՜��ߜ��М������М�����ޜ������ޜ���
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
			  Log::instance().Trace( 190, "%d :: ����������՜��� ������ۜ��ޜ��Ҝ��� %s", __LINE__, (*wptr).c_str() );
			  bufSQL.format("SELECT IDword FROM TableWords WHERE Word LIKE %Q;", (*wptr).c_str());
			  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
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
			  Log::instance().Trace( 190, "%d :: ����������՜��� ������М��ٜ��ۜ��� ������ޜ��ޜ�����Ҝ��՜��������Ҝ�����������؜��� ������ۜ��ޜ��Ҝ��� ���� ID=%d", __LINE__, (*idwPtr) );
			  bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
			  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
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
	
	  Log::instance().Trace( 180, "%d :: ����������՜��� ������М��ٜ��ۜ��� ������ޜ��ޜ�����Ҝ��՜��������Ҝ�����������؜��� ���Ҝ���������М��֜��՜��ݜ��؜��� %s", __LINE__, temp.c_str() );
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
	  throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	}

}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Search(string aText, list<string> & aParams)
// ���������ݜ��ڜ�����؜��� �������М��ל��ќ��؜��Ҝ��М��՜��� ���Ҝ���������М��֜��՜��ݜ��؜��� ���Ԝ��ۜ��� ���ߜ��ޜ��؜�����ڜ��� ���ߜ��� ������ۜ��՜��ܜ��՜��ݜ�����М��� (�������ޜ�����ޜ���������� ���؜��������������� ���� ���ќ��М��ל���
// ���Ԝ��М��ݜ��ݜ������) �������М��ל��Ԝ��՜��ۜ��՜��ݜ�����ܜ��� ������؜��ܜ��Ҝ��ޜ��ۜ��М��ܜ���:
// ,; - ���Ԝ��՜��ۜ������ ���Ҝ���������М��֜��՜��ݜ��؜��� ���ݜ��� ������ۜ��՜��ܜ��՜��ݜ������ ���� ������������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��՜��� ������Ҝ�����ל��� ���ܜ��՜��֜��Ԝ��� ������ۜ��՜��ܜ��՜��ݜ�����М��ܜ��� ������������
// ^  - ���Ԝ��՜��ۜ��؜��� ���Ҝ���������М��֜��՜��ݜ��؜��� ���ݜ��� ������ۜ��՜��ܜ��՜��ݜ������ ���� ������������М��ݜ��М��Ҝ��ۜ��؜��Ҝ��М��՜��� ������Ҝ�����ל��� ���ܜ��՜��֜��Ԝ��� ���ݜ��؜��ܜ��� ����
// ���Ҝ���������М��֜��՜��ݜ��؜��� ������؜�����М��՜��������� ������ۜ��՜��Ҝ��� ���ݜ��М��ߜ������М��Ҝ��� ���� ���Ԝ��՜��ٜ��������Ҝ��؜��� ���ݜ��М��� ���ݜ��М��ٜ��Ԝ��՜��ݜ�����ܜ��� ������ۜ��՜��ܜ��՜��ݜ�����М��ܜ���
// ���Ҝ�����ߜ��ޜ��ۜ��ݜ�����՜��������� ���ߜ��ޜ�����ۜ��՜��Ԝ��ޜ��Ҝ��М�����՜��ۜ�����ݜ���. (���ߜ������؜��ޜ������՜�����՜��� ���ޜ��ߜ��՜������М�����ޜ������ޜ��� ���� ���� ������������ ���ޜ��Ԝ��؜���)
bool  CDBSQLitProvider::Search(const string& aText, map<string,bool> &aParams, hostRecords &Result)
{
  Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  list<int> res;
		  list<int>::iterator id;
		  list<hostRec>::iterator ihRec;
		  hostRec hRec;
		  fileStr fRec;
		  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
		  CppSQLite3Query q;
		  string temp = "";
		  string tHName;
		  string tIPName;
		  bool bResult;
		  std::stringstream ss;
		  bResult = Find(aText, aParams, res);
		  if( bResult )
			{
			  id=res.begin();
			  if(id != res.end())
				{
				  ss << *id;
				  id++;
				}
			  for (; id != res.end(); ++id)
				  ss << ", " << *id;
			  //  	bufSQL.format("select th.IDHost as IDn, HostName, IPNum, FileName, FileSize, strftime('%%s',FileTimeCr) as FTUTS FROM TableFiles AS tf, TableHostes AS th WHERE tf.IDfile IN (%s) AND th.IDhost=tf.IDhost ORDER BY th.IDhost;", temp.c_str());
			  bufSQL.format("select th.IDHost as IDn, HostName, IPNum, FileName, FileSize, FileTimeCr as FTUTS FROM TableFiles AS tf, TableHostes AS th WHERE tf.IDfile IN (%s) AND th.IDhost=tf.IDhost ORDER BY th.IDhost;", ss.str().c_str());
			  q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���
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
					  hRec.IPNum = q.getStringField("IPNum", "");
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
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__);
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
	  }catch(...){
		throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	  }
	}catch( CPrvException& e ){
	int errc = e.ErrorCode();
	if( errc == RESULT_OK ) errc = 1;
	SetLastError(errc, e.Message());
  }
  return false;
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// ���Ԝ��М������ ���� ������ޜ������ܜ��М������ ���Ҝ�����Ԝ�����Ҝ��М��՜��ܜ������ ���������ݜ��ڜ�����؜�����ܜ��� time ���� ����.����. ���������М��ݜ��Ԝ��М���������ݜ��ޜ��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��� ����
void  CDBSQLitProvider::EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles)
{ 
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  int i=-1; // ����������М��ݜ��؜��� ID ������М��ќ��ۜ��؜������ ������ޜ��������ޜ���
		  char strTmp[255]; // ���Ҝ�����ߜ��ޜ��ܜ��ޜ��Ӝ��Ҝ�����՜��ۜ�����ݜ������ ���ܜ��М��������؜��� (���Ҝ��Ҝ��՜��Ԝ��՜��� ���Ԝ��ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���Ԝ��М������ ���� �������������ޜ��ڜ��ޜ��Ҝ��ޜ��� ������ޜ������ܜ���)
		  //	filesStr::iterator Cur; // ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ��� ������՜��ڜ��������؜��� ������М��ٜ��� ������ߜ��؜�����ڜ���
		  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
		  string temp;
		  std::stringstream ssTmp;
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
			  ssTmp << aDate;
			  temp += ssTmp.str();
			  temp += "'";
			}

		  bufSQL.format("select * from TableHostes%s;", temp.c_str());

		  Log::instance().Trace( 180, "%d :: �������М��ߜ������ޜ��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������", __LINE__ );
		  CppSQLite3Query q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���

		  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
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
			  db.execDML(bufSQL); // ������Ԝ��М��ۜ������ ���Ҝ������ ���ל��М��ߜ��؜������ ������ޜ��ޜ�����Ҝ��՜�����������Ҝ�����������՜��Ӝ��� ID ���؜��� ������М��ќ��ۜ��؜������ (������М��ٜ���-������ۜ��ޜ���)

			  bufSQL.format("delete from TableFiles where IDhost = %d;", (*pIDhost));
			  db.execDML(bufSQL); // ������Ԝ��М��ۜ������ ���Ҝ������ ���ל��М��ߜ��؜������ ������ޜ��ޜ�����Ҝ��՜�����������Ҝ�����������՜��Ӝ��� ID ���؜��� ������М��ќ��ۜ��؜������ ������М��ٜ��ۜ��ޜ���

			  if( !aOnlyFiles )
				{
				  bufSQL.format("delete from TableHostes where IDhost = %d;",(*pIDhost));
				  db.execDML(bufSQL); // ������Ԝ��М��ۜ������ ���Ҝ������ ���ל��М��ߜ��؜������ ������ޜ��ޜ�����Ҝ��՜�����������Ҝ�����������՜��Ӝ��� ID ���؜��� ������М��ќ��ۜ��؜������ ������ޜ��������ޜ���
				}
			}
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__);
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
	  }catch(...){
		throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	  }
	}catch( CPrvException& e ){
	int errc = e.ErrorCode();
	if( errc == RESULT_OK ) errc = 1;
	SetLastError(errc, e.Message());
  }

  Log::instance().Trace( 100, "CDBSQLitProvider::%s [exit]", __FUNCTION__ );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// ���Ԝ��М������ ���� ������ޜ������ܜ��М������ ���Ҝ�����Ԝ�����Ҝ��М��՜��ܜ������ ���������ݜ��ڜ�����؜�����ܜ��� time ���� ����.����. ���������М��ݜ��Ԝ��М���������ݜ��ޜ��� ���ќ��؜��ќ��ۜ��؜��ޜ�����՜��ڜ��� ����
char*  CDBSQLitProvider::GetNamePlugin()
{
  return "SQLite3";
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
//
time_t  CDBSQLitProvider::GetRefDateHost(const string& aHostName, const string& aIPnum)
{
  Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );

  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  time_t i; // ����������М��ݜ��؜��� ID ������М��ќ��ۜ��؜������ ������ޜ��������ޜ���
		  //	char strTmp[255]; // ���Ҝ�����ߜ��ޜ��ܜ��ޜ��Ӝ��Ҝ�����՜��ۜ�����ݜ������ ���ܜ��М��������؜��� (���Ҝ��Ҝ��՜��Ԝ��՜��� ���Ԝ��ۜ��� ����������М��ݜ��՜��ݜ��؜��� ���Ԝ��М������ ���� �������������ޜ��ڜ��ޜ��Ҝ��ޜ��� ������ޜ������ܜ���)
		  //	filesStr::iterator Cur; // ������ڜ��М��ל��М�����՜��ۜ��� ���ݜ��� ������՜��ڜ��������؜��� ������М��ٜ��� ������ߜ��؜�����ڜ���
		  CppSQLite3Buffer bufSQL; // ���ќ��������՜��� ���Ԝ��ۜ��� ������ޜ������ܜ��؜������ޜ��Ҝ��М��ݜ��؜��� ���ל��М��ߜ������ޜ������
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

		  Log::instance().Trace( 180, "%d :: �������М��ߜ������ޜ��� ���� ���ќ��М��ל��� ���Ԝ��М��ݜ��ݜ������", __LINE__ );
		  CppSQLite3Query q = db.execQuery(bufSQL); // ���ל��М��ߜ������ޜ���

		  Log::instance().Trace( 180, "%d :: �������ќ������М��ќ��ޜ�����ڜ��� ���ל��М��ߜ������ޜ������", __LINE__ );
		  if(!q.eof())
			i = q.getIntField("DateRef", 0);
		  q.finalize();

		  Log::instance().Trace( 100, "%s [return %d]", __FUNCTION__, i );
		  return i;
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__);
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
	  }catch(...){
		throw CPrvException("�������ޜ��ל��ݜ��؜��ڜ��ۜ��� ���ݜ��՜��؜��ל��Ҝ��՜��������ݜ��ޜ��� ���؜�����ڜ��ۜ��������՜��ݜ��؜���", 0, __FUNCTION__);
	  }
	}catch( CPrvException& e ){
	int errc = e.ErrorCode();
	if( errc == RESULT_OK ) errc = 1;
	SetLastError(errc, e.Message());
  }
  return 0;
}
//-----------------------------------------------------------------------------

bool  CDBSQLitProvider::RefreshDB()
{
  return false;
}
//-----------------------------------------------------------------------------

int  CDBSQLitProvider::GetProvError(string& mes)
{
  Log::instance().Trace( 100, "%s [enter]", __FUNCTION__ );
  if(&mes != NULL)
	mes = FErrMsg.c_str();
  return FErrCode;
}
//-----------------------------------------------------------------------------

void  CDBSQLitProvider::SetLastError(int aCode, const string& aMes)
{
  Log::instance().Trace( 150, "%s [enter]", __FUNCTION__ );
  if(&aMes != NULL)
	FErrMsg = aMes;
  FErrCode = aCode;
}
//-----------------------------------------------------------------------------

void  CDBSQLitProvider::SetAutoIndex(bool aVal)
{
  if( aVal != FAutoIndex )
	{
	  FAutoIndex = aVal;
	}
}
//-----------------------------------------------------------------------------
bool  CDBSQLitProvider::IsAutoIndex()
{
  return FAutoIndex;
}
/*/-----------------------------------------------------------------------------
  void  CDBSQLitProvider::StartIndexing(map<string,bool> &aParams)
  {
  }*/
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

