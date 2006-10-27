#include "winsock2.h"
#include "windows.h"
#include "lm.h"
#include "CNetBiosScanner.h"


CNetBiosScanner::CNetBiosScanner()
{
	strcpy( m_strProtoName, "NetBios" );
}

//����������� �������, ������������ ��� ��������� �����/�����
void CNetBiosScanner::EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile;
	//TODO:����������� � Stack overflow
	std::string m_strBuf;
	if( INVALID_HANDLE_VALUE != ( hFile = ::FindFirstFile( strSharePath, &FindFileData ) ) )
	{
		//�� ������� � ��������� ������ ���� . � ..
		if( ( 0 != strcmp( ".", FindFileData.cFileName ) ) )
		{
			m_strBuf.append( strSharePath, strSharePath[ strlen( strSharePath ) - 3 ] );
			m_strBuf += FindFileData.cFileName;
			vcFilesList.push_back( m_strBuf );
			m_strBuf.clear();
		}
		while( 0 != ::FindNextFile( hFile, &FindFileData ) ) 
			//�� ������� � ��������� ������ ���� . � ..
			if( 0 != strcmp( ".", FindFileData.cFileName ) && 0 != strcmp( "..", FindFileData.cFileName ) )
			{
				m_strBuf.append( strSharePath, strSharePath + strlen( strSharePath ) - 3  );
				m_strBuf += FindFileData.cFileName;
				m_strBuf += "\\";
				m_strBuf += "*.*";
				vcFilesList.push_back( m_strBuf );
				//���������� �������� ����� �� ��������� ������
				EnumFiles( m_strBuf.c_str(), vcFilesList );
				m_strBuf.clear();
			}
			::FindClose( hFile );
	}else
		return;
}

//����������� ����� strAddress, ��������� ������� � vcResList
void CNetBiosScanner::Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList )
{
	BYTE* buf;
	DWORD p1,p2;
	WCHAR serv[1024];
	DWORD handle = 0;
	DWORD res;
	char strTmpShareName[255], strTmpSharePath[10240];
	//��������� ��� ������� � Unicode,�.�. WIN NET API �������� ������ Unicode
	::MultiByteToWideChar( CP_UTF8, 0, strAddress.c_str(), (int)strAddress.size() + 1, serv, 1024 );
	do
	{
		//�������� ������ ����������� �������� �� serv
		res = ::NetShareEnum( serv, 0, &buf, MAX_PREFERRED_LENGTH, &p1, &p2, &handle );
		SHARE_INFO_0* inf =  (SHARE_INFO_0*)buf;
		for( unsigned int i = 0; i < p1; i++ )
		{
			int buflen = ::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, 0, 0, 0, 0);
			::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, strTmpShareName, buflen, 0, 0 );
			//��������� ������� ����
			if( strTmpShareName[ buflen - 2 ] != '$' )
			{
				sprintf( strTmpSharePath, "\\\\%s\\%s\\*.*", strAddress.c_str(), strTmpShareName );
				//�������� ������ ������
				EnumFiles( strTmpSharePath, vcResList ); 
			}
		}
		::NetApiBufferFree( buf );
	}while( res == ERROR_MORE_DATA );
};
