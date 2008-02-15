#include "SharedLib.h"
#include <dlfcn.h>

bool CSharedLib::Load( const std::string& strFileName )
{
  std::string strTmp;
  bool bRes = ( NULL == ( m_pLib = dlopen( strFileName.c_str(), RTLD_LAZY ) ) )?false:true;
  if( !bRes )
	{
	  char cTmp[10240];
	  strcpy( cTmp, dlerror() );
	  strTmp = cTmp;
	}
  return bRes;
}

bool CSharedLib::Close()
{
    return ( 0 == dlclose( m_pLib ) )?true:false;
}

void* CSharedLib::GetSymbol( const std::string& strSymbolName )
{
    return dlsym( m_pLib, strSymbolName.c_str() );
}

