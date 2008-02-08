#ifndef SHAREDLIB_H_INCLUDED
#define SHAREDLIB_H_INCLUDED
#include <string>


class CSharedLib
{
public:

    CSharedLib( const std::string& strFileName ){ Load( strFileName ); }

    CSharedLib(){}

    ~CSharedLib(){ Close(); }

    bool Load( const std::string& strFileName );

    void* GetSymbol( const std::string& strSymbolName );

    bool Close();

private:

    CSharedLib( const CSharedLib& );
    CSharedLib& operator=( const CSharedLib& );

#ifdef WIN32

#else
    void* m_pLib;
#endif
};

#endif // SHAREDLIB_H_INCLUDED
