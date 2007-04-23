#include "tempstorage.h"

CTempStorage::CTempStorage( const std::string& strFileName ):m_strFileName( strFileName )
{
}

CTempStorage::~CTempStorage(void)
{
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
			strTmp += " �� ������� ������� ���� ��� ������";
			throw TempStorageErr( strTmp );
		}
	}else
	{
		m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::out|std::ios::app );
		if( !m_sFile.is_open() )
		{
			std::string strTmp = m_strFileName;
			strTmp += " �� ������� ������� ���� ��� ������";
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

SmartPtr<BYTE> CTempStorage::GetBuf( unsigned long& ulCount )
{
	unsigned long ulSize = Size();
	if( ulCount >= ulSize || ulCount == 0 )
		ulCount = ulSize;
	Open( true );
	//TODO: �����, �������� ������, � �������� �� � ������� �������� delete, � �� delete[]
	SmartPtr<BYTE> pBuf = SmartPtr<BYTE>( new BYTE[ ulCount ] );
	m_sFile.read( (char*)pBuf.get(), ulCount );
	m_sFile.close();
	return pBuf;
}

void CTempStorage::Clear()
{
	if( FALSE == DeleteFile( m_strFileName.c_str() ) )
		Log::instance().Trace( 10, "CTempStorage::Clear: �� ������� ������� ���� %s", m_strFileName.c_str() );

/*
	m_sFile.open( m_strFileName.c_str(), std::ios::out| std::ios::trunc );
	m_sFile.close();
*/
}

bool CTempStorage::IsExists()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	bool bRes = m_sFile.is_open();
	m_sFile.close();
	return bRes;
}