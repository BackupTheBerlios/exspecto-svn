/////////////////////////////////////////////////////////////////////////////
// œô¨Áœô¨åœô¨Õœô¨Üœô¨Ð œô¨Ñœô¨Ðœô¨×œô¨ë œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å                                                       //
//                                                                         //
// TableHostes  œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Ð œô¨Þœô¨ßœô¨Øœô¨áœô¨ëœô¨Òœô¨Ðœô¨Õœô¨â œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ýœô¨ëœô¨Õ œô¨Ò œô¨áœô¨Õœô¨âœô¨Ø œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨ë               //
//   IDhost        INTEGER PRIMARY KEY œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð                   //
//   IPNum         CHAR(15)       IP œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð                            //
//   HostName      TINYTEXT       œô¨¸œô¨Üœô¨ï œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð                           //
//   DateRef       INT            œô¨²œô¨àœô¨Õœô¨Üœô¨ï œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õœô¨Ôœô¨Ýœô¨Õœô¨Ýœô¨Óœô¨Þ œô¨Þœô¨Ñœô¨Ýœô¨Þœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨ï œô¨Øœô¨Ýœô¨äœô¨Þœô¨àœô¨Üœô¨Ðœô¨æœô¨Øœô¨Ø  //
// ----------------------------------------------------------------------- //
// TableFiles   œô¨Þœô¨ßœô¨Øœô¨áœô¨Ðœô¨Ýœô¨Øœô¨Õ œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Þœô¨Ò œô¨Ø œô¨ßœô¨àœô¨Øœô¨Òœô¨ïœô¨×œô¨Úœô¨Ð œô¨Øœô¨å œô¨Ú œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨ã                 //
//   IDfile        INTEGER PRIMARY KEY œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð                        //
//   IDhost        INT            œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð                        //
//   FileName      TINYTEXT       œô¨¸œô¨Üœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð                                //
//   FileSize      INT            œô¨Àœô¨Ðœô¨×œô¨Üœô¨Õœô¨à œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð                             //
//   FileTimeCr    INT            œô¨²œô¨àœô¨Õœô¨Üœô¨ï œô¨áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð                     //
// ----------------------------------------------------------------------- //
// TableWords   œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Ð œô¨Òœô¨áœô¨Õœô¨å œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ýœô¨ëœô¨å œô¨áœô¨Ûœô¨Þœô¨Ò                                 //
// { œô¨¿œô¨ãœô¨âœô¨Ø œô¨Ú œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ðœô¨Ü œô¨àœô¨Ðœô¨×œô¨Ñœô¨Øœô¨Òœô¨Ðœô¨îœô¨âœô¨áœô¨ï œô¨Ýœô¨Ð œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð (œô¨Øœô¨Ôœô¨Øœô¨Þœô¨Üœô¨ë) œô¨Ø œô¨×œô¨Ðœô¨Ýœô¨Þœô¨áœô¨ïœô¨âœô¨áœô¨ï œô¨Ò œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ã.  } //
// { œô¨Íœô¨âœô¨Þ œô¨áœô¨Ôœô¨Õœô¨Ûœô¨Ðœô¨Ýœô¨Þ œô¨Ôœô¨Ûœô¨ï œô¨ãœô¨áœô¨Úœô¨Þœô¨àœô¨Õœô¨Ýœô¨Øœô¨ï œô¨ßœô¨Þœô¨Øœô¨áœô¨Úœô¨Ð œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Ù.                         } //
//   IDword        INTEGER PRIMARY KEY œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð                        //
//   Word          TINYTEXT       "œô¨¸œô¨Ôœô¨Øœô¨Þœô¨Üœô¨Ð" (œô¨áœô¨Ûœô¨Þœô¨Òœô¨Þ)                         //
// ----------------------------------------------------------------------- //
// TableWordInFiles   œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨Øœô¨Õ œô¨áœô¨Ûœô¨Þœô¨Ò œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ðœô¨Ü œô¨Ø œô¨äœô¨Ûœô¨Ðœô¨Ó œô¨ßœô¨àœô¨Øœô¨Ýœô¨Ðœô¨Ôœô¨Ûœô¨Õœô¨Öœô¨Ýœô¨Þœô¨áœô¨âœô¨Ø œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð //
//   IDword        INT            œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð                             //
//   IDfile        INT            œô¨¸œô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð                             //
//   IsPath        BOOL           "œô¨¸œô¨Ôœô¨Øœô¨Þœô¨Üœô¨Ð" - œô¨çœô¨Ðœô¨áœô¨âœô¨ì œô¨ßœô¨ãœô¨âœô¨Ø?                   //
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
		  //œô¨¸œô¨Ýœô¨Øœô¨æœô¨Øœô¨Ðœô¨Ûœô¨Øœô¨×œô¨Ðœô¨æœô¨Øœô¨ï œô¨Ûœô¨Þœô¨Óœô¨Õœô¨àœô¨Ð
		  Settings::instance().SetModule( "SQLite", pParamTypes, sizeof( pParamTypes )/sizeof( pParamTypes[0] ) );
		  Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
		  Log::instance().SetLoglevel( iLogLevel );	
		  Settings::instance().GetParam( DB_FILE, dbFile );
		  Settings::instance().GetParam( DB_AUTOINDEX_WORD, bAI );
		  FAutoIndex = bAI;

		  Log::instance().Trace( 100, "CDBSQLitProvider::%s ::: Open BD", __FUNCTION__ );
		  db.open(dbFile.c_str()); // œô¨¾œô¨âœô¨Úœô¨àœô¨ëœô¨Òœô¨Ðœô¨î œô¨äœô¨Ðœô¨Ùœô¨Û œô¨Ñœô¨Ðœô¨×œô¨ë œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å

		  // œô¨Áœô¨Þœô¨×œô¨Ôœô¨Ðœô¨î œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨ßœô¨àœô¨Ø œô¨Øœô¨å œô¨Þœô¨âœô¨áœô¨ãœô¨âœô¨áœô¨âœô¨Òœô¨Øœô¨Ø //INTEGER PRIMARY KEY <-AUTO_INCREMENT
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
		throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
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
// œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨â œô¨áœô¨ßœô¨Øœô¨áœô¨Þœô¨Ú œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Þœô¨Ò œô¨Ò œô¨±œô¨´
// aFileList  œô¨Þœô¨Ôœô¨Ýœô¨Þœô¨Ýœô¨Ðœô¨ßœô¨àœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨ëœô¨Ù œô¨áœô¨ßœô¨Øœô¨áœô¨Þœô¨Ú œô¨Ýœô¨Ð œô¨Þœô¨áœô¨Ýœô¨Þœô¨Òœô¨Õ œô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨ãœô¨àœô¨ë œô¨á œô¨ßœô¨Ðœô¨àœô¨Ðœô¨Üœô¨Õœô¨âœô¨àœô¨Ðœô¨Üœô¨Ø œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð
// aHostName  œô¨Øœô¨Üœô¨ï œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð œô¨Ýœô¨Ð œô¨Úœô¨Þœô¨âœô¨Þœô¨àœô¨Ü œô¨Ýœô¨Ðœô¨åœô¨Þœô¨Ôœô¨Øœô¨Ûœô¨áœô¨ï œô¨ßœô¨àœô¨Õœô¨Ôœô¨áœô¨âœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨ëœô¨Ù œô¨áœô¨ßœô¨Øœô¨áœô¨Þœô¨Ú
// aIPnum     IP œô¨Ðœô¨Ôœô¨àœô¨Õœô¨áœô¨á œô¨Úœô¨Þœô¨Üœô¨ßœô¨ìœô¨îœô¨âœô¨Õœô¨àœô¨Ð
void CDBSQLitProvider::AddFiles(hostRec &aRec)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  int i,j; // œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨â ID œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò
		  //	char strTmp[255]; // œô¨Òœô¨áœô¨ßœô¨Þœô¨Üœô¨Þœô¨Óœô¨Òœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ù œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò (œô¨Òœô¨Òœô¨Õœô¨Ôœô¨Õœô¨Ý œô¨Ôœô¨Ûœô¨ï œô¨åœô¨àœô¨Ðœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨Ôœô¨Ðœô¨âœô¨ë œô¨Ò œô¨áœô¨âœô¨àœô¨Þœô¨Úœô¨Þœô¨Òœô¨Þœô¨Ù œô¨äœô¨Þœô¨àœô¨Üœô¨Õ)
		  filesStr::iterator Cur; // œô¨ãœô¨Úœô¨Ðœô¨×œô¨Ðœô¨âœô¨Õœô¨Ûœô¨ì œô¨Ýœô¨Ð œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Øœô¨Ù œô¨äœô¨Ðœô¨Ùœô¨Û œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð
		  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
		  CppSQLite3Query q;
	
		  // œô¨½œô¨Ðœô¨çœô¨Ðœô¨Ûœô¨Þ œô¨âœô¨àœô¨Ðœô¨Ýœô¨×œô¨Ðœô¨Úœô¨æœô¨Øœô¨Ø    
		  db.execDML("begin transaction;");

		  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ú œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å", __LINE__ );
		  i = -1;
		  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aRec.IPNum.c_str());
		  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á

		  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
		  if(!q.eof())
  			i = q.getIntField("IDhost", -1); // œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨î œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à ID œô¨ãœô¨Úœô¨Ðœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Ýœô¨Ð œô¨áœô¨ãœô¨éœô¨Õœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì
		  q.finalize();

		  if(i >= 0) // œô¨Õœô¨áœô¨Ûœô¨Ø œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì œô¨áœô¨ãœô¨éœô¨Õœô¨áœô¨âœô¨Òœô¨ãœô¨Õœô¨â
			{
			  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì œô¨ãœô¨Öœô¨Õ œô¨áœô¨ãœô¨éœô¨Õœô¨áœô¨âœô¨Òœô¨ãœô¨Õœô¨â", __LINE__ );
			  bufSQL.format("update TableHostes set DateRef = strftime('%%s','now') where IDhost = %d;",i);
			  db.execDML(bufSQL); // œô¨Þœô¨Ñœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨î œô¨Ôœô¨Ðœô¨âœô¨ã/œô¨Òœô¨àœô¨Õœô¨Üœô¨ï œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õœô¨Ôœô¨Ýœô¨Õœô¨Óœô¨Þ œô¨Þœô¨Ñœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨Õœô¨Ýœô¨Øœô¨ï
			}
		  else // œô¨Øœô¨Ýœô¨Ðœô¨çœô¨Õ
			{
			  Log::instance().Trace( 180, "%d :: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨Ýœô¨Þœô¨Òœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì", __LINE__ );
			  bufSQL.format("insert into TableHostes values(NULL, %Q, %Q, strftime('%%s','now'));", aRec.IPNum.c_str(), aRec.HostName.c_str());
			  db.execDML(bufSQL); // œô¨Ôœô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨î œô¨Ýœô¨Þœô¨Òœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì œô¨Ò œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ã œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò

			  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ú œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å", __LINE__ );
			  bufSQL.format("select IDhost from TableHostes where IPNum = %Q;", aRec.IPNum.c_str());
			  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
	
			  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
			  if(!q.eof())
   				i = q.getIntField("IDhost", -1); // œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨î œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à ID
			  q.finalize();
			  if( i < 0 ) throw CPrvException("œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Ò œô¨áœô¨âœô¨àœô¨ãœô¨Úœô¨âœô¨ãœô¨àœô¨Õ œô¨±œô¨´. (œô¨Âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Ð œô¨Åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò)", __LINE__);
			}
    
		  Log::instance().Trace( 180, "%d :: œô¨¿œô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨âœô¨ì ID œô¨áœô¨Ûœô¨Õœô¨Ôœô¨ãœô¨îœô¨éœô¨Õœô¨Ù œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨Ø", __LINE__ );
		  bufSQL.format("select MAX(IDfile) as Cnt from TableFiles;", i);
		  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
		  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
		  j = -1;
		  if(!q.eof())
			j = q.getIntField("Cnt", -1); // œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨î œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à ID œô¨ãœô¨Úœô¨Ðœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Ýœô¨Ð œô¨áœô¨ãœô¨éœô¨Õœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì
		  q.finalize();

		  CppSQLite3Statement stmt_File = db.compileStatement("insert into TableFiles values (?, ?, ?, ?, ?);");
		  for ( Cur = aRec.Files.begin(); Cur != aRec.Files.end(); Cur++ )
			{
			  Log::instance().Trace( 185, "%d :: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨Ýœô¨Þœô¨Òœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì [%s]", __LINE__,  Cur->FileName.c_str());
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
				  Log::instance().Trace( 185, "%d :: œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨Ü œô¨Ýœô¨Þœô¨Òœô¨ëœô¨Õ œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð œô¨Øœô¨× œô¨áœô¨âœô¨àœô¨Þœô¨Úœô¨Ø [%s]", __LINE__,  Cur->FileName.c_str());
				  AddWord(j, Cur->FileName);//*/
				}
			}
		  stmt_File.finalize();

		  // œô¨·œô¨Ðœô¨Òœô¨Õœô¨àœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨âœô¨àœô¨Ðœô¨Ýœô¨×œô¨Ðœô¨Úœô¨æœô¨Øœô¨Ø
		  db.execDML("commit transaction;");
	
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__, e.ErrorCode());
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__, e.errorCode());
	  }catch(...){
		throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
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
// œô¨Àœô¨Ðœô¨×œô¨Ñœô¨Øœô¨Òœô¨Ðœô¨Õœô¨â œô¨Øœô¨Üœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð œô¨Ýœô¨Ð œô¨Øœô¨Ôœô¨Øœô¨Þœô¨Üœô¨ë œô¨Ø œô¨Ôœô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨â œô¨Ò œô¨áœô¨ßœô¨Øœô¨áœô¨Þœô¨Ú œô¨áœô¨Ûœô¨Þœô¨Ò œô¨Ø œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨Øœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ðœô¨Ü
// aID    œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð œô¨Ò œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Õ œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Þœô¨Ò
// aPath  œô¨ßœô¨Þœô¨Ûœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨Üœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð
void  CDBSQLitProvider::AddWord(int aID, const string& aPath)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  try
	{
	  list<string> wrd;
	  int n = (int)aPath.rfind("\\"); //œô¨Øœô¨éœô¨Õœô¨â œô¨ßœô¨Õœô¨àœô¨Òœô¨ëœô¨Ù œô¨àœô¨Ðœô¨×œô¨Ôœô¨Õœô¨Ûœô¨Øœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ù œô¨áœô¨Ûœô¨Õœô¨èœô¨ì œô¨áœô¨ßœô¨àœô¨Ðœô¨Òœô¨Þ
	  string tmp;
	  if( n>=0 ) // œô¨Õœô¨áœô¨Ûœô¨Ø œô¨áœô¨Ûœô¨Õœô¨èœô¨ì œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ý œô¨âœô¨Þ œô¨äœô¨àœô¨Ðœô¨×œô¨Ð œô¨áœô¨Ûœô¨Õœô¨Òœô¨Ð œô¨Þœô¨â œô¨áœô¨Ûœô¨Õœô¨èœô¨Ð œô¨íœô¨âœô¨Þ œô¨ßœô¨ãœô¨âœô¨ì
		{
		  tmp = aPath.substr(0,n);
		  Tools::Split(tmp, sepFile, wrd); 
		  AddWordInTable(aID, wrd, true);
		}
	  wrd.clear();

	  if( (n+1)<(int)aPath.size() ) //œô¨Õœô¨áœô¨Ûœô¨Ø œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õ œô¨áœô¨Ûœô¨Õœô¨èœô¨Ð œô¨Õœô¨áœô¨âœô¨ì œô¨áœô¨Øœô¨Üœô¨Òœô¨Þœô¨Ûœô¨ë œô¨âœô¨Þ œô¨íœô¨âœô¨Þ œô¨Øœô¨Üœô¨ï œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð
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
	  throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
	}

  Log::instance().Trace( 100, "CDBSQLitProvider::AddWord [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AddWordInTable(int aID, list<string> & words, bool IsPath)
// œô¨´œô¨Þœô¨Ñœô¨Ðœô¨Òœô¨Ûœô¨ïœô¨Õœô¨â œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð œô¨Ò œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ã œô¨áœô¨Ûœô¨Þœô¨Ò
// aID     œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Ð
// words   œô¨áœô¨ßœô¨Øœô¨áœô¨Þœô¨Ú œô¨áœô¨Ûœô¨Þœô¨Ò
// IsPath  œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ð œô¨Òœô¨åœô¨Þœô¨Ôœô¨ïœô¨â œô¨Ò œô¨ßœô¨ãœô¨âœô¨ì œô¨Ú œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨ã
void  CDBSQLitProvider::AddWordInTable(int aID, list<string> &words, bool IsPath)
{
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  try
	{
	  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
	  CppSQLite3Query q;
	  list<string>::iterator wptr;
	  int j; 

	  CppSQLite3Statement stmt_Word = db.compileStatement("insert into TableWords values (?, ?);");
	  CppSQLite3Statement stmt_FileWord = db.compileStatement("insert into TableWordInFiles values (?, ?, ?);");
	  for (wptr = words.begin(); wptr != words.end(); ++wptr)
		{
		  j = -1; //œô¨Øœô¨Ýœô¨Ôœô¨Õœô¨Úœô¨á œô¨Øœô¨Ôœô¨Øœô¨Þœô¨Üœô¨ë œô¨Ò œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Õ
		  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
		  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
		  if(!q.eof())
 			j = q.getIntField("IDword", -2);
		  q.finalize();
		  if(j == -1)
			{
			  stmt_Word.bindNull(1);
			  stmt_Word.bind(2, (*wptr).c_str());
			  stmt_Word.execDML();
			  stmt_Word.reset();
	  	
			  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ú œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å", __LINE__ );
			  bufSQL.format("select IDword from TableWords where Word = %Q;", (*wptr).c_str());
			  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á

			  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
			  if(!q.eof())
				j = q.getIntField("IDword", -2); // œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Ðœô¨î œô¨Ýœô¨Þœô¨Üœô¨Õœô¨à ID œô¨ãœô¨Úœô¨Ðœô¨×œô¨ãœô¨îœô¨éœô¨Øœô¨Ù œô¨Ýœô¨Ð œô¨áœô¨ãœô¨éœô¨Õœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨ãœô¨î œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨ì
			  q.finalize();
			}
		  if( j < 0 ) throw CPrvException("œô¨¾œô¨èœô¨Øœô¨Ñœô¨Úœô¨Ð œô¨Ò œô¨±œô¨´! (œô¨Âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨Ð œô¨Áœô¨Ûœô¨Þœô¨Ò)", __LINE__);
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
	  throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
	}

  Log::instance().Trace( 100, "CDBSQLitProvider::AddWordInTable [exit]" );
}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Find(string aText, list<string> & aParams)
// œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ï œô¨Øœô¨éœô¨Õœô¨â œô¨æœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨Þœô¨Õ œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ò œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å
// œô¨Âœô¨Þœô¨Õœô¨áœô¨âœô¨ì œô¨ßœô¨Õœô¨àœô¨Õœô¨Ôœô¨Ðœô¨Òœô¨Ðœô¨Õœô¨Üœô¨ëœô¨Ù œô¨âœô¨Õœô¨Úœô¨áœô¨â œô¨Ôœô¨Þœô¨Ûœô¨Öœô¨Õœô¨Ý œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨Þœô¨Òœô¨Ðœô¨âœô¨ì œô¨Þœô¨Ôœô¨Ýœô¨Þœô¨Üœô¨ã œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨ã
// œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð.
// œô¨²œô¨áœô¨Õ œô¨áœô¨Øœô¨Üœô¨Òœô¨Þœô¨Ûœô¨ë, œô¨×œô¨Ð œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õœô¨Ü ? œô¨Ø * œô¨ïœô¨Òœô¨Ûœô¨ïœô¨îœô¨âœô¨áœô¨ï œô¨Þœô¨Ñœô¨ëœô¨çœô¨Ýœô¨ëœô¨Üœô¨Ø œô¨áœô¨Øœô¨Üœô¨Òœô¨Þœô¨Ûœô¨Ðœô¨Üœô¨Ø œô¨áœô¨âœô¨àœô¨Þœô¨Úœô¨Ø œô¨ßœô¨Þ
// œô¨Úœô¨Þœô¨âœô¨Þœô¨àœô¨ëœô¨Ü œô¨ßœô¨àœô¨Þœô¨Øœô¨×œô¨Òœô¨Þœô¨Ôœô¨Øœô¨âœô¨áœô¨ï œô¨ßœô¨Þœô¨Øœô¨áœô¨Ú.
bool  CDBSQLitProvider::Find(const string& aText, map<string,bool> &aParams, list<int> &Result)
{
  try
	{
	  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
	  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
	  CppSQLite3Query q;
	  list<string> wrd; // œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò œô¨áœô¨Ûœô¨Þœô¨Ò œô¨Øœô¨× œô¨äœô¨àœô¨Ðœô¨×œô¨ë
	  //	bool CommASAnd=true; //œô¨áœô¨Þœô¨Þœô¨âœô¨Ýœô¨Þœô¨èœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨áœô¨Ûœô¨Þœô¨Òœô¨Ðœô¨Üœô¨Ø AND
	  //	list<string> varsep; //œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò œô¨àœô¨Ðœô¨×œô¨Ôœô¨Õœô¨Ûœô¨ïœô¨îœô¨éœô¨Øœô¨å œô¨áœô¨Õœô¨ßœô¨Ðœô¨àœô¨Ðœô¨âœô¨Þœô¨àœô¨Þœô¨Ò
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
			  Log::instance().Trace( 190, "%d :: œô¨¸œô¨éœô¨Õœô¨â œô¨áœô¨Ûœô¨Þœô¨Òœô¨Þ %s", __LINE__, (*wptr).c_str() );
			  bufSQL.format("SELECT IDword FROM TableWords WHERE Word LIKE %Q;", (*wptr).c_str());
			  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
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
			  Log::instance().Trace( 190, "%d :: œô¨¸œô¨éœô¨Õœô¨â œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨ë œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨Òœô¨ãœô¨îœô¨éœô¨Øœô¨Õ œô¨áœô¨Ûœô¨Þœô¨Òœô¨ã œô¨á ID=%d", __LINE__, (*idwPtr) );
			  bufSQL.format("select IDfile from TableWordInFiles where IDword = %d;", (*idwPtr));
			  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
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
	
	  Log::instance().Trace( 180, "%d :: œô¨¸œô¨éœô¨Õœô¨â œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨ë œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨Òœô¨ãœô¨îœô¨éœô¨Øœô¨Õ œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨î %s", __LINE__, temp.c_str() );
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
	  throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
	}

}
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Search(string aText, list<string> & aParams)
// œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ï œô¨àœô¨Ðœô¨×œô¨Ñœô¨Øœô¨Òœô¨Ðœô¨Õœô¨â œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ôœô¨Ûœô¨ï œô¨ßœô¨Þœô¨Øœô¨áœô¨Úœô¨Ð œô¨ßœô¨Þ œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Ü (œô¨ºœô¨Þœô¨âœô¨Þœô¨àœô¨ëœô¨Õ œô¨Øœô¨éœô¨ãœô¨âœô¨áœô¨ï œô¨Ò œô¨Ñœô¨Ðœô¨×œô¨Õ
// œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å) œô¨àœô¨Ðœô¨×œô¨Ôœô¨Õœô¨Ûœô¨Õœô¨Ýœô¨ëœô¨Üœô¨Ø œô¨áœô¨Øœô¨Üœô¨Òœô¨Þœô¨Ûœô¨Ðœô¨Üœô¨Ø:
// ,; - œô¨Ôœô¨Õœô¨Ûœô¨ïœô¨â œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ýœô¨Ð œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨ë œô¨Ø œô¨ãœô¨áœô¨âœô¨Ðœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨Øœô¨Òœô¨Ðœô¨Õœô¨â œô¨áœô¨Òœô¨ïœô¨×œô¨ì œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø œô¨¸œô¨»œô¨¸
// ^  - œô¨Ôœô¨Õœô¨Ûœô¨Øœô¨â œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨Ýœô¨Ð œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨ë œô¨Ø œô¨ãœô¨áœô¨âœô¨Ðœô¨Ýœô¨Ðœô¨Òœô¨Ûœô¨Øœô¨Òœô¨Ðœô¨Õœô¨â œô¨áœô¨Òœô¨ïœô¨×œô¨ì œô¨Üœô¨Õœô¨Öœô¨Ôœô¨ã œô¨Ýœô¨Øœô¨Üœô¨Ø œô¨¸
// œô¨Òœô¨ëœô¨àœô¨Ðœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ œô¨çœô¨Øœô¨âœô¨Ðœô¨Õœô¨âœô¨áœô¨ï œô¨áœô¨Ûœô¨Õœô¨Òœô¨Ð œô¨Ýœô¨Ðœô¨ßœô¨àœô¨Ðœô¨Òœô¨Þ œô¨Ø œô¨Ôœô¨Õœô¨Ùœô¨áœô¨âœô¨Òœô¨Øœô¨Õ œô¨Ýœô¨Ðœô¨Ô œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ýœô¨ëœô¨Üœô¨Ø œô¨íœô¨Ûœô¨Õœô¨Üœô¨Õœô¨Ýœô¨âœô¨Ðœô¨Üœô¨Ø
// œô¨Òœô¨ëœô¨ßœô¨Þœô¨Ûœô¨Ýœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨ßœô¨Þœô¨áœô¨Ûœô¨Õœô¨Ôœô¨Þœô¨Òœô¨Ðœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨Þ. (œô¨ßœô¨àœô¨Øœô¨Þœô¨àœô¨Õœô¨âœô¨Õœô¨â œô¨Þœô¨ßœô¨Õœô¨àœô¨Ðœô¨âœô¨Þœô¨àœô¨Þœô¨Ò œô¨¸ œô¨Ø œô¨¸œô¨»œô¨¸ œô¨Þœô¨Ôœô¨Øœô¨Ý)
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
		  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
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
			  q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á
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
		throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
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
// œô¨Ôœô¨Ðœô¨âœô¨Ð œô¨Ò œô¨äœô¨Þœô¨àœô¨Üœô¨Ðœô¨âœô¨Õ œô¨Òœô¨ëœô¨Ôœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Üœô¨ëœô¨å œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ïœô¨Üœô¨Ø time œô¨Ø œô¨â.œô¨ß. œô¨áœô¨âœô¨Ðœô¨Ýœô¨Ôœô¨Ðœô¨àœô¨âœô¨Ýœô¨Þœô¨Ù œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Ø œô¨Á
void  CDBSQLitProvider::EraseHost(const string& aHostName, const string& aIPnum, time_t aDate, bool aOnlyFiles)
{ 
  Log::instance().Trace( 100, "CDBSQLitProvider::%s [enter]", __FUNCTION__ );
  SetLastError(RESULT_OK, "");
  try
	{
	  try
		{
		  int i=-1; // œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨â ID œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò
		  char strTmp[255]; // œô¨Òœô¨áœô¨ßœô¨Þœô¨Üœô¨Þœô¨Óœô¨Òœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ù œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò (œô¨Òœô¨Òœô¨Õœô¨Ôœô¨Õœô¨Ý œô¨Ôœô¨Ûœô¨ï œô¨åœô¨àœô¨Ðœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨Ôœô¨Ðœô¨âœô¨ë œô¨Ò œô¨áœô¨âœô¨àœô¨Þœô¨Úœô¨Þœô¨Òœô¨Þœô¨Ù œô¨äœô¨Þœô¨àœô¨Üœô¨Õ)
		  //	filesStr::iterator Cur; // œô¨ãœô¨Úœô¨Ðœô¨×œô¨Ðœô¨âœô¨Õœô¨Ûœô¨ì œô¨Ýœô¨Ð œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Øœô¨Ù œô¨äœô¨Ðœô¨Ùœô¨Û œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð
		  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
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

		  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ú œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å", __LINE__ );
		  CppSQLite3Query q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á

		  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
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
			  db.execDML(bufSQL); // œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨ïœô¨î œô¨Òœô¨áœô¨Õ œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨Ø œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨Õœô¨Óœô¨Þ ID œô¨Øœô¨× œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë (œô¨äœô¨Ðœô¨Ùœô¨Û-œô¨áœô¨Ûœô¨Þœô¨Ò)

			  bufSQL.format("delete from TableFiles where IDhost = %d;", (*pIDhost));
			  db.execDML(bufSQL); // œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨ïœô¨î œô¨Òœô¨áœô¨Õ œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨Ø œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨Õœô¨Óœô¨Þ ID œô¨Øœô¨× œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨äœô¨Ðœô¨Ùœô¨Ûœô¨Þœô¨Ò

			  if( !aOnlyFiles )
				{
				  bufSQL.format("delete from TableHostes where IDhost = %d;",(*pIDhost));
				  db.execDML(bufSQL); // œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨ïœô¨î œô¨Òœô¨áœô¨Õ œô¨×œô¨Ðœô¨ßœô¨Øœô¨áœô¨Ø œô¨áœô¨Þœô¨Þœô¨âœô¨Òœô¨Õœô¨âœô¨áœô¨âœô¨Òœô¨ãœô¨îœô¨éœô¨Õœô¨Óœô¨Þ ID œô¨Øœô¨× œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò
				}
			}
		}catch( CPrvException& e ){
		throw CPrvException(e.Message(), 0, __FUNCTION__);
	  }catch( std::exception& e ){
		throw CPrvException(e, 0, __FUNCTION__);
	  }catch(CppSQLite3Exception&  e){
		throw CPrvException(e.errorMessage(), 0, __FUNCTION__);
	  }catch(...){
		throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
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
// œô¨Ôœô¨Ðœô¨âœô¨Ð œô¨Ò œô¨äœô¨Þœô¨àœô¨Üœô¨Ðœô¨âœô¨Õ œô¨Òœô¨ëœô¨Ôœô¨ëœô¨Òœô¨Ðœô¨Õœô¨Üœô¨ëœô¨å œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨ïœô¨Üœô¨Ø time œô¨Ø œô¨â.œô¨ß. œô¨áœô¨âœô¨Ðœô¨Ýœô¨Ôœô¨Ðœô¨àœô¨âœô¨Ýœô¨Þœô¨Ù œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Ø œô¨Á
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
		  time_t i; // œô¨åœô¨àœô¨Ðœô¨Ýœô¨Øœô¨â ID œô¨âœô¨Ðœô¨Ñœô¨Ûœô¨Øœô¨æœô¨ë œô¨åœô¨Þœô¨áœô¨âœô¨Þœô¨Ò
		  //	char strTmp[255]; // œô¨Òœô¨áœô¨ßœô¨Þœô¨Üœô¨Þœô¨Óœô¨Òœô¨âœô¨Õœô¨Ûœô¨ìœô¨Ýœô¨ëœô¨Ù œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò (œô¨Òœô¨Òœô¨Õœô¨Ôœô¨Õœô¨Ý œô¨Ôœô¨Ûœô¨ï œô¨åœô¨àœô¨Ðœô¨Ýœô¨Õœô¨Ýœô¨Øœô¨ï œô¨Ôœô¨Ðœô¨âœô¨ë œô¨Ò œô¨áœô¨âœô¨àœô¨Þœô¨Úœô¨Þœô¨Òœô¨Þœô¨Ù œô¨äœô¨Þœô¨àœô¨Üœô¨Õ)
		  //	filesStr::iterator Cur; // œô¨ãœô¨Úœô¨Ðœô¨×œô¨Ðœô¨âœô¨Õœô¨Ûœô¨ì œô¨Ýœô¨Ð œô¨âœô¨Õœô¨Úœô¨ãœô¨éœô¨Øœô¨Ù œô¨äœô¨Ðœô¨Ùœô¨Û œô¨áœô¨ßœô¨Øœô¨áœô¨Úœô¨Ð
		  CppSQLite3Buffer bufSQL; // œô¨Ñœô¨ãœô¨äœô¨Õœô¨à œô¨Ôœô¨Ûœô¨ï œô¨äœô¨Þœô¨àœô¨Üœô¨Øœô¨àœô¨Þœô¨Òœô¨Ðœô¨Ýœô¨Øœô¨ï œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð
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

		  Log::instance().Trace( 180, "%d :: œô¨·œô¨Ðœô¨ßœô¨àœô¨Þœô¨á œô¨Ú œô¨Ñœô¨Ðœô¨×œô¨Õ œô¨Ôœô¨Ðœô¨Ýœô¨Ýœô¨ëœô¨å", __LINE__ );
		  CppSQLite3Query q = db.execQuery(bufSQL); // œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨á

		  Log::instance().Trace( 180, "%d :: œô¨¾œô¨Ñœô¨àœô¨Ðœô¨Ñœô¨Þœô¨âœô¨Úœô¨Ð œô¨×œô¨Ðœô¨ßœô¨àœô¨Þœô¨áœô¨Ð", __LINE__ );
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
		throw CPrvException("œô¨²œô¨Þœô¨×œô¨Ýœô¨Øœô¨Úœô¨Ûœô¨Þ œô¨Ýœô¨Õœô¨Øœô¨×œô¨Òœô¨Õœô¨áœô¨âœô¨Ýœô¨Þœô¨Õ œô¨Øœô¨áœô¨Úœô¨Ûœô¨îœô¨çœô¨Õœô¨Ýœô¨Øœô¨Õ", 0, __FUNCTION__);
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

