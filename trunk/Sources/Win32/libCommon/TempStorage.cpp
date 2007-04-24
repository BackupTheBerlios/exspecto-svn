#include "tempstorage.h"

CTempStorage::CTempStorage( const std::string& strFileName )
{
	CreateDirectory( "temp", NULL );
	m_strFileName = "temp\\" + strFileName;
}

CTempStorage::~CTempStorage(void)
{
	Clear();
	RemoveDirectory( "temp" );
}

CTempStorage& CTempStorage::operator<<( CTempStorage& st )
{
	if( st.IsExists() )
	{
		st.Open( true );
		Open( false );
		char ch;
		while( st.m_sFile.get( ch ) ) m_sFile.put( ch );
		st.m_sFile.close();
		st.Clear();
		m_sFile.close();
	}
	return *this;
}

void CTempStorage::Open( bool bRead )
{
	if( bRead )
	{
		m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
		if( !m_sFile.is_open() )
		{
			std::string strTmp = m_strFileName;
			strTmp += " не удалось открыть файл для чтения";
			throw TempStorageErr( strTmp );
		}
	}else
	{
		m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::out|std::ios::app );
		if( !m_sFile.is_open() )
		{
			std::string strTmp = m_strFileName;
			strTmp += " не удалось открыть файл для записи";
			throw TempStorageErr( strTmp );
		}
	}
}

long CTempStorage::Size()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	if( !m_sFile.is_open() )
		return 0;
	else
	m_sFile.seekp( 0, std::ios_base::end );
	long lSize = m_sFile.tellp();
	m_sFile.close();
	return lSize; 
}

SmartPtr<BYTE, AllocNewArray<BYTE> > CTempStorage::GetBuf( unsigned long& ulCount )
{
	unsigned long ulSize = Size();
	if( ulCount >= ulSize || ulCount == 0 )
		ulCount = ulSize;
	Open( true );
	SmartPtr<BYTE, AllocNewArray<BYTE> > pBuf = SmartPtr<BYTE, AllocNewArray<BYTE> >( new BYTE[ ulCount ] );
	m_sFile.read( (char*)pBuf.get(), ulCount );
	m_sFile.close();
	return pBuf;
}

void CTempStorage::Clear()
{
	if( FALSE == DeleteFile( m_strFileName.c_str() ) )
		Log::instance().Trace( 100, "CTempStorage::Clear: не удалось удалить файл %s", m_strFileName.c_str() );
}

bool CTempStorage::IsExists()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	bool bRes = m_sFile.is_open();
	m_sFile.close();
	return bRes;
}