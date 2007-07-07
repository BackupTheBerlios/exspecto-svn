#include "tempstorage.h"

CTempStorage::CTempStorage( const std::string& strHostName, const std::string& strHostAddr, const std::string& strProtoName )
:m_bOpenedForWrite( false )
,m_strHostAddr( strHostAddr )
,m_strHostName( strHostName )
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
/*	m_sFile << "<" << FILE_TAG << ">";
		m_sFile << "<" << FILE_PATH << ">" + File.FileName + "</" + FILE_PATH + ">";
		m_sFile << "<" << FILE_DATE << ">";
		m_sFile.write( (const char*)&File.FDate.UTS, sizeof( time_t ) );
		m_sFile << "</" << FILE_DATE << ">";
		m_sFile << "<" << FILE_SIZE << ">" << File.FileSize << "</" << FILE_SIZE << ">";
	m_sFile << "</" << FILE_TAG << ">\r\n";
	*/
	m_sFile << File.FileSize << File.FileName << File.FDate.UTS;
    m_sFile.close();
}

void CTempStorage::GetRecords( hostRec& Host, int iRecordsCount )
{
	Host.IPNum = m_strHostAddr;
	Host.HostName = m_strHostName;
	Host.Files.clear();
	fileStr TmpFile;
	int i = 0;
	while( m_sFile >> TmpFile.FileSize >> TmpFile.FileName >> TmpFile.FDate.UTS && ++i < iRecordsCount );
	m_sFile.close();
}

std::string CTempStorage::GetPacket( int iRecordsCount )
{
	std::string strTmp = "<?xml version=\"1.0\" encoding=\"windows-1251\"?>\r\n<packet>\r\n</packet>\r\n";
	strTmp += "<";strTmp += HOST_TAG;strTmp += " ";strTmp += HOST_ADDR;strTmp += "=" + m_strHostAddr;
	strTmp += " ";strTmp += HOST_NAME;strTmp += " ";strTmp += m_strHostName + ">";
    Open( true );
	int i = 0;
	while( m_sFile >> strTmp && ++i < iRecordsCount );
	m_sFile.close();
	strTmp += "</";strTmp += HOST_TAG;strTmp += ">";
	return strTmp;
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
		m_bOpenedForWrite = true;
	}
}

void CTempStorage::Clear()
{
	//Если файл создавался - удаляем его
	if( m_bOpenedForWrite && !DeleteFile( m_strFileName.c_str() ) )
		Log::instance().Trace( 100, "CTempStorage::Clear: не удалось удалить файл %s, LastError = %d", m_strFileName.c_str(), GetLastError() );
	m_bOpenedForWrite = false;
}

bool CTempStorage::IsExists()
{
	m_sFile.open( m_strFileName.c_str(), std::ios::binary|std::ios::in );
	bool bRes = m_sFile.is_open();
	m_sFile.close();
	return bRes;
}
