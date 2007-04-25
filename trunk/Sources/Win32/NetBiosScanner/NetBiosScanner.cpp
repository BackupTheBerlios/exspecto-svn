#include "NetBiosScanner.h"
#include "MemLeakDetector.h"
#include "SettingsContainer.h"
#ifdef _MSC_VER
#define _WIN32_WINNT 0x0500
#endif
#include "lm.h"

DllExport const char* GetProtocolName()
{
	static const char* str = MOD_NAME;
	return str;
}

//������� IP-����� �� ����
std::string RemoveIp( const std::string& ResPath )
{
	std::string::size_type sSlashPos;
	if( std::string::npos != ( sSlashPos = ResPath.find_first_of( '\\', 3 ) ) )
	{
		std::string strRes( ResPath.begin() + sSlashPos, ResPath.end() );
		return strRes;
	}
	return ResPath;
}

//����������� �������, ������������ ��� ��������� �����/�����
void EnumFiles( IN const char* strAddress, IN const char* strSharePath, IN StoreDataFunc pStoreFunc, HANDLE hCancelEvent )
{
	//���� ������� �������� - ��������� ����������
	if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
		return;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile;
	std::string strFileName;
	if( INVALID_HANDLE_VALUE != ( hFile = ::FindFirstFile( strSharePath, &FindFileData ) ) )
	{
		//�� ������� � ��������� ������ ���� . � ..
		if( ( 0 != strcmp( ".", FindFileData.cFileName ) ) )
		{
			strFileName.append( strSharePath, strSharePath + strlen( strSharePath ) - 3 );
			strFileName += FindFileData.cFileName;
			pStoreFunc(	 strAddress
						, RemoveIp( strFileName ).c_str(), ( FindFileData.nFileSizeHigh * ((__int64)MAXDWORD+1) ) + FindFileData.nFileSizeLow
						, FindFileData.ftLastWriteTime.dwLowDateTime
						, FindFileData.ftLastWriteTime.dwHighDateTime );
		}
		while( 0 != ::FindNextFile( hFile, &FindFileData ) && WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 )) 
			//�� ������� � ��������� ������ ���� . � ..
			if( 0 != strcmp( ".", FindFileData.cFileName ) && 0 != strcmp( "..", FindFileData.cFileName ) )
			{
				strFileName.clear();
				strFileName.append( strSharePath, strSharePath + strlen( strSharePath ) - 3  );
				strFileName += FindFileData.cFileName;
				RemoveIp( strFileName );
				pStoreFunc( strAddress
							, RemoveIp( strFileName ).c_str(), ( FindFileData.nFileSizeHigh * ((__int64)MAXDWORD+1) ) + FindFileData.nFileSizeLow
							, FindFileData.ftLastWriteTime.dwLowDateTime
							, FindFileData.ftLastWriteTime.dwHighDateTime );

				Log::instance().Trace( 100, "CNetBiosScanner::EnumFiles: ��������� ������: %s", strFileName.c_str() ); 
				strFileName += "\\";
				strFileName += "*.*";
				//���������� �������� ����� �� ��������� ������
				EnumFiles( strAddress, strFileName.c_str(), pStoreFunc, hCancelEvent );
			}
		::FindClose( hFile );
	}else
		return;
}

DllExport bool Scan( IN const char* strAddress, IN StoreDataFunc pStoreFunc, IN HANDLE hCancelEvent )
{
	try{
		BYTE* buf;
		DWORD p1,p2;
		WCHAR serv[1024];
		DWORD handle = 0;
		DWORD res;
		char strTmpShareName[255], strTmpSharePath[10240];
		Log::instance().Trace( 80, "NetBios: �������� ������������ ������ %s", strAddress );
		//��������� ��� ������� � Unicode,�.�. WIN NET API �������� ������ Unicode
		::MultiByteToWideChar( CP_UTF8, 0, strAddress, (int)strlen( strAddress ) + 1, serv, 1024 );
		do
		{
			if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
			{
				Log::instance().Trace( 10, "NetBios: ������������ ��������" );
				break;
			}
			
			//�������� ������ ����������� �������� �� serv
			res = ::NetShareEnum( serv, 0, &buf, MAX_PREFERRED_LENGTH, &p1, &p2, &handle );
			if( ERROR_SUCCESS != res && ERROR_MORE_DATA != res )
			{
				Log::instance().Trace( 10, "NetBios: �� ������� �������������� ����: %s, ��� ������: %d", strAddress, res );
				return true;
			}
			SHARE_INFO_0* inf =  (SHARE_INFO_0*)buf;
			for( unsigned int i = 0; i < p1; i++ )
			{
				int buflen = ::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, 0, 0, 0, 0);
				::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, strTmpShareName, buflen, 0, 0 );
				//��������� ������� ����
				if( strTmpShareName[ buflen - 2 ] != '$' )
				{
					sprintf( strTmpSharePath, "\\\\%s\\%s\\*.*", strAddress, strTmpShareName );
					//�������� ������ ������
					EnumFiles( strAddress, strTmpSharePath, pStoreFunc, hCancelEvent );
					if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
						break; 
				}
			}
			::NetApiBufferFree( buf );
		}while( res == ERROR_MORE_DATA );
		Log::instance().Trace( 10, "NetBios: ���������� ������������ ������ %s", strAddress );
		return true;
	}catch( std::exception& e )
	{
		Log::instance().Trace( 0, "NetBios::Scan: ������ �� ����� ������������ ������ %s: %s", strAddress, e.what() );
	}catch( ... )
	{
		Log::instance().Trace( 0, "NetBios::Scan: ����������� ������ �� ����� ������������ ������ %s", strAddress );
	}
	return false;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( DLL_PROCESS_ATTACH == ul_reason_for_call )
	{
		try{
			//�������������� ��������������� ������
			int iLogLevel;	
			Log::instance().SetModuleName( MOD_NAME );
			Settings::instance().SetModule( MOD_NAME, pNetBiosParamTypes, sizeof( pNetBiosParamTypes )/sizeof( pNetBiosParamTypes[0] ) );
			Settings::instance().GetParam( LOG_LEVEL, iLogLevel );
			Log::instance().SetLoglevel( iLogLevel );
		}catch( std::exception& e)
		{
			Log::instance().Trace( 0, "DllMain: ������: %s", e.what() );
			return FALSE;
		}catch( ... )
		{
			Log::instance().Trace( 0, "DllMain: ����������� �����" );
			return FALSE;
		}
	}
	if( DLL_PROCESS_DETACH == ul_reason_for_call )
	{
		DumpMemLeaks();
	}
    return TRUE;
}

