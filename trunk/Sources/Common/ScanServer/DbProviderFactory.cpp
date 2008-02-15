#include "precomp.h"
#include "DbProviderFactory.h"
#include <FileSearch.h>


CDbProviderFactory::CDbProviderFactory(void):m_pDbInstance( NULL )
{
  static char cBuf[10240];
  Log::instance().Trace( 10, "CDbProviderFactory::CDbProviderFactory: " );
  std::string strLibName;
  Settings::instance().GetParam( DB_PROV_NAME, strLibName );
  if( NULL == getcwd( cBuf, sizeof( cBuf ) ) )
  	throw std::runtime_error( "error getting current directory" );
  strLibName.insert( strLibName.begin(), '/' );
  strLibName.insert( strLibName.begin(), cBuf, cBuf + strlen(cBuf) );
  m_pLib = SmartPtr<CSharedLib>( new CSharedLib() );
  if( !m_pLib->Load( strLibName.c_str() ) )
	throw std::runtime_error( ": " + strLibName );

  typedef void*(*RP)(void);
  RP pFunc;
  if( NULL == ( pFunc = ( (RP)m_pLib->GetSymbol( "RegisterPlugin" ) ) ) )
	throw std::runtime_error( ": " +strLibName );
  if( NULL == ( m_pDbInstance = (CDBProvider*)pFunc() ) )
	throw std::runtime_error( ": " +strLibName );
}

CDBProvider* CDbProviderFactory::GetProviderInstance()
{
	return m_pDbInstance;
}

