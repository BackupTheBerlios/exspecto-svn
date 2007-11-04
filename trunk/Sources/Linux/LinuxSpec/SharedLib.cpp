#include "SharedLib.h"
#include <dlfcn.h>

bool CSharedLib::Load( const std::string& strFileName )
{
    return ( NULL == ( m_pLib = dlopen( strFileName.c_str(), RTLD_LAZY ) ) )?false:true;
}

bool CSharedLib::Close()
{
    return ( 0 == dlclose( m_pLib ) )?true:false;
}

void* CSharedLib::GetSymbol( const std::string& strSymbolName )
{
    return dlsym( m_pLib, strSymbolName.c_str() );
}

