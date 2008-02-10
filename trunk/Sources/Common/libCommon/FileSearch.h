#ifndef FILESEARCH_H_INCLUDED
#define FILESEARCH_H_INCLUDED

#include <vector>
#include <string>
#ifdef WIN32
#else
#include <glob.h>
#endif

class FileSearch
{
public:

    ~FileSearch();

    std::vector<std::string> GetFiles( const std::string& strMask );

private:

#ifdef WIN32
#else
    glob_t m_GlobBuf;
#endif

};

#endif // FILESEARCH_H_INCLUDED
