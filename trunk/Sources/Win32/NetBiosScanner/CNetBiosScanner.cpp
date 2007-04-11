#include "windows.h"

#ifdef _MSC_VER
#define _WIN32_WINNT 0x0500
#endif

#include "lm.h"
#include "CNetBiosScanner.h"

CNetBiosScanner::CNetBiosScanner()
{
	Log::instance().Trace( 90, "Инициализация плагина NetBios" );
}

//Рекурсивная функция, перебирающая все вложенные папки/файлы
void CNetBiosScanner::EnumFiles( IN const char* strSharePath, OUT filesStr& FilesList, HANDLE hCancelEvent )
{
	//Если задание отменили - завершаем выполнение
	if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
		return;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFile;
	//TODO:разобраться с Stack overflow
    fileStr TmpStruct;
	if( INVALID_HANDLE_VALUE != ( hFile = ::FindFirstFile( strSharePath, &FindFileData ) ) )
	{
		//не выводим в результат записи вида . и ..
		if( ( 0 != strcmp( ".", FindFileData.cFileName ) ) )
		{
			TmpStruct.FileName.append( strSharePath, strSharePath + strlen( strSharePath ) - 3 );
			TmpStruct.FileName += FindFileData.cFileName;
			TmpStruct.FileSize = ( FindFileData.nFileSizeHigh * ((__int64)MAXDWORD+1) ) + FindFileData.nFileSizeLow;
			TmpStruct.FDate.hFileTime = FindFileData.ftLastWriteTime.dwHighDateTime;
			TmpStruct.FDate.lFileTime = FindFileData.ftLastWriteTime.dwLowDateTime;
			FilesList.push_back( TmpStruct );
		}
		while( 0 != ::FindNextFile( hFile, &FindFileData ) && WAIT_OBJECT_0 != WaitForSingleObject( hCancelEvent, 0 )) 
			//не выводим в результат записи вида . и ..
			if( 0 != strcmp( ".", FindFileData.cFileName ) && 0 != strcmp( "..", FindFileData.cFileName ) )
			{
				TmpStruct.FileName.clear();
				TmpStruct.FileName.append( strSharePath, strSharePath + strlen( strSharePath ) - 3  );
				TmpStruct.FileName += FindFileData.cFileName;
				TmpStruct.FileSize = ( FindFileData.nFileSizeHigh * ((__int64)MAXDWORD+1) ) + FindFileData.nFileSizeLow;
				TmpStruct.FDate.hFileTime = FindFileData.ftLastWriteTime.dwHighDateTime;
				TmpStruct.FDate.lFileTime = FindFileData.ftLastWriteTime.dwLowDateTime;
				Log::instance().Trace( 100, "CNetBiosScanner::EnumFiles: Добавляем ресурс: %s", TmpStruct.FileName.c_str() ); 
				FilesList.push_back( TmpStruct );
				TmpStruct.FileName += "\\";
				TmpStruct.FileName += "*.*";
				//Рекурсивно проводим поиск во вложенных папках
				EnumFiles( TmpStruct.FileName.c_str(), FilesList, hCancelEvent );
			}
		::FindClose( hFile );
	}else
		return;
}

//Сканировать адрес strAddress, результат сложить в ResList
void CNetBiosScanner::Scan( IN std::string strAddress, OUT filesStr& ResList, HANDLE hCancelEvent )
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
		if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
		{
			Log::instance().Trace( 10, "NetBios: Сканирование отменено" );
			break;
		}
		
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
				EnumFiles( strTmpSharePath, ResList, hCancelEvent );
				if( WAIT_OBJECT_0 == WaitForSingleObject( hCancelEvent, 0 ) )
					break; 
			}
		}
		::NetApiBufferFree( buf );
	}while( res == ERROR_MORE_DATA );
	Log::instance().Trace( 10, "NetBios: Завершение сканирования адреса %s", strAddress.c_str() );
}

