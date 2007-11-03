#include "SharedLib.h"

CSharedLib::~CSharedLib()
{
}

bool CSharedLib::Load( const std::string& strFileName )
{
    return false;
}

void* CSharedLib::GetSymbol( const std::string& strSymbolName )
{
    return NULL;
}

