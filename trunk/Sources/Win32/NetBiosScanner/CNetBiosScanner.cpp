#include "winsock2.h"
#include "windows.h"
#include "lm.h"
#include "CNetBiosScanner.h"


CNetBiosScanner::CNetBiosScanner()
{
	Log::instance().Trace( 90, "Инициализация плагина NetBios" );
}

//Рекурсивная функция, перебирающая все вложенные папки/файлы
void CNetBiosScanner::EnumFiles( IN const char* strSharePath, OUT std::vector< std::string >& vcFilesList, HANDLE hCancelEvent )
{
	//Если задание отменили - завершаем выполнение
	if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
		return;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile;
	//TODO:разобраться с Stack overflow
	std::string m_strBuf;
	if( INVALID_HANDLE_VALUE != ( hFile = ::FindFirstFile( strSharePath, &FindFileData ) ) )
	{
		//не выводим в результат записи вида . и ..
		if( ( 0 != strcmp( ".", FindFileData.cFileName ) ) )
		{
			m_strBuf.append( strSharePath, strSharePath[ strlen( strSharePath ) - 3 ] );
			m_strBuf += FindFileData.cFileName;
			vcFilesList.push_back( m_strBuf );
			m_strBuf.clear();
		}
		while( 0 != ::FindNextFile( hFile, &FindFileData ) && WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 )) 
			//не выводим в результат записи вида . и ..
			if( 0 != strcmp( ".", FindFileData.cFileName ) && 0 != strcmp( "..", FindFileData.cFileName ) )
			{
				m_strBuf.append( strSharePath, strSharePath + strlen( strSharePath ) - 3  );
				m_strBuf += FindFileData.cFileName;
				Log::instance().Trace( 100, "CNetBiosScanner::EnumFiles: Добавляем ресурс: %s", m_strBuf.c_str() ); 
				vcFilesList.push_back( m_strBuf );
				m_strBuf += "\\";
				m_strBuf += "*.*";
				//Рекурсивно проводим поиск во вложенных папках
				EnumFiles( m_strBuf.c_str(), vcFilesList, hCancelEvent );
				m_strBuf.clear();
			}
		::FindClose( hFile );
	}else
		return;
}

//Сканировать адрес strAddress, результат сложить в vcResList
void CNetBiosScanner::Scan( IN std::string strAddress, OUT std::vector< std::string >& vcResList, HANDLE hCancelEvent )
{
	BYTE* buf;
	DWORD p1,p2;
	WCHAR serv[1024];
	DWORD handle = 0;
	DWORD res;
	char strTmpShareName[255], strTmpSharePath[10240];
	Log::instance().Trace( 80, "NetBios: Начинаем сканирование адреса %s", strAddress.c_str() );
	//Переводим имя сервера в Unicode,т.к. WIN NET API понимает только Unicode
	::MultiByteToWideChar( CP_UTF8, 0, strAddress.c_str(), (int)strAddress.size() + 1, serv, 1024 );
	do
	{
		//Получаем список расшаренных ресурсов на serv
		res = ::NetShareEnum( serv, 0, &buf, MAX_PREFERRED_LENGTH, &p1, &p2, &handle );
		if( ERROR_SUCCESS != res && ERROR_MORE_DATA != res )
		{
			Log::instance().Trace( 10, "NetBios: Не удалось просканировать хост: %s, код ошибки: %d", strAddress.c_str(), res );
			return;
		}
		SHARE_INFO_0* inf =  (SHARE_INFO_0*)buf;
		for( unsigned int i = 0; i < p1; i++ )
		{
			int buflen = ::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, 0, 0, 0, 0);
			::WideCharToMultiByte( 1251, 0, (WCHAR*)( inf + i )->shi0_netname, -1, strTmpShareName, buflen, 0, 0 );
			//исключаем скрытые шары
			if( strTmpShareName[ buflen - 2 ] != '$' )
			{
				sprintf( strTmpSharePath, "\\\\%s\\%s\\*.*", strAddress.c_str(), strTmpShareName );
				//получаем список файлов
				EnumFiles( strTmpSharePath, vcResList, hCancelEvent );
				if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
					break; 
			}
		}
		::NetApiBufferFree( buf );
	}while( res == ERROR_MORE_DATA && WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 ));
}

