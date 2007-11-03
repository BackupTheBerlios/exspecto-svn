#ifndef SHAREDLIB_H_INCLUDED
#define SHAREDLIB_H_INCLUDED
#include <string>


class CSharedLib
{
public:

    ~CSharedLib();

    bool Load( const std::string& strFileName );

    void* GetSymbol( const std::string& strSymbolName );

private:

#ifdef WIN32

#else
    void* m_pLib;
#endif
};

#endif // SHAREDLIB_H_INCLUDED
