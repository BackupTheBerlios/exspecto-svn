#include "tempstorage.h"

CTempStorage::CTempStorage( const std::string& strHostName, const std::string& strHostAddr, const std::string& strProtoName )
:m_strHostName( strHostName )
,m_strHostAddr( strHostAddr )
,m_bOpenedForWrite( false )
{
	CreateDirectory( "temp", NULL );
	m_strFileName = "temp\\" + strProtoName + strHostAddr;
}

CTempStorage::~CTempStorage(void)
{
	Clear();
	RemoveDirectory( "temp" );
}

void CTempStorage::PutRecord( const fileStr& File )
{
	Open( false );

	m_sFile << File.FileSize << " " << File.FileName << " " << File.FDate.UTS << " ";
    m_sFile.close();
}

int CTempStorage::GetRecords( hostRec& Host, int iRecordsCount )
{
	if( IsExists() )
	{
		Open( true );
		Host.IPNum = m_strHostAddr;
		Host.HostName = m_strHostName;
		Host.Files.clear();
		fileStr TmpFile;
		int i = 0;
		m_sFile.seekg( m_ReadPos );
		while( m_sFile >> TmpFile.FileSize >> TmpFile.FileName >> TmpFile.FDate.UTS && ++i < iRecordsCount )
			Host.Files.push_back( TmpFile );
		m_ReadPos = m_sFile.tellg();
		m_sFile.close();
		return i?--i:0;
	}else
		return 0;
}

CTempStorage& CTempStorage::operator<<( CTempStorage& st )
{
	if( st.IsExists() )
	{
		st.Open( true );
		Open( false );
		char ch;
		while( st.m_sFile.get( ch ) ) m_sFile.put( ch );
		m_ReadPos = m_sFile.tellg();
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
		m_bOpenedForWrite = true;
	}
}

void CTempStorage::Clear()
{
	//���� ���� ���������� - ������� ���
	if( m_bOpenedForWrite && !DeleteFile( m_strFileName.c_str() ) )
		Log::instance().Trace( 100, "CTempStorage::Clear: �� ������� ������� ���� %s, LastError = %d", m_strFileName.c_str(), GetLastError() );
	m_bOpenedForWrite = false;
}

bool CTempStorage::IsExists()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	bool bRes = m_sFile.is_open();
	m_sFile.close();
	return bRes;
}

unsigned int CTempStorage::GetSize()
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

bool CTempStorage::IsEof()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	if( !m_sFile.is_open() )
		return true;
	m_sFile.seekg( 0, std::ios_base::end );
	bool bRes = ( m_ReadPos >= m_sFile.tellg() )?true:false;
	m_sFile.close();
	return bRes;  
}