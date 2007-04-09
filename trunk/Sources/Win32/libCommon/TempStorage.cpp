#include "tempstorage.h"

CTempStorage::CTempStorage( const std::string& strFileName )
{
	m_file.open( strFileName.c_str(), std::ios::binary|std::ios::in|std::ios::out );
}

CTempStorage::~CTempStorage(void)
{
}

CTempStorage& CTempStorage::operator<<( CTempStorage& st )
{
	char ch;
	while( st.m_file.get( ch ) ) m_file.put( ch );
	return *this;
}