#include <FileSearch.h>
#include <stdexcept>

FileSearch::~FileSearch()
{
    ::globfree( &m_GlobBuf );
}

std::vector<std::string> FileSearch::GetFiles( const std::string& strMask )
{
    std::vector<std::string> vecRes;
    if( 0 != glob( strMask.c_str(), 0, NULL, &m_GlobBuf ) )
        for( unsigned int i = 0; i < m_GlobBuf.gl_pathc; i++ )
            vecRes.push_back( m_GlobBuf.gl_pathv[i] );
    else
        throw std::runtime_error( "FileSearch:GetFiles: glob error" );
    return vecRes;
}
