//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
//-------------------------------------------------------------------------------------//
#include "PluginLoadStrategy.h"

PluginLoadStrategy::PluginLoadStrategy( std::vector< CScanner* >& vecStorage )throw( PluginLoadErr )
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	CScanner* pScanner;
	fnGetScannerFunc fnGetScanner;
	fnReleaseScannerFunc fnReleaseScanner;
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	
	strPluginPath += "\\*.dll";
	log.Trace( 90, "PluginLoadStrategy: Загружаем плагины, путь для поиска: %s", strPluginPath.c_str() ); 
	//Находим все dll в папке с plugin-ами
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
		throw PluginLoadErr( "Не найдено ни одного плагина" );
	do
	{ 
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += FindData.cFileName;
		if( NULL == ( hLib = ::LoadLibraryA( strPluginFileName.c_str() ) ) )
		{
			log.Trace( 50, "%d", GetLastError() );
			log.Trace( 50, "PluginLoadStrategy: %s не является библиотекой", FindData.cFileName );
			continue;
		}
		if( NULL == ( fnGetScanner = ( fnGetScannerFunc )::GetProcAddress( hLib, "GetScanner" ) ) )
		{
			log.Trace( 50, "PluginLoadStrategy: не удалось получить адрес функции GetScanner из библиотеки %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
		}
 		if( NULL == ( fnReleaseScanner = ( fnReleaseScannerFunc )::GetProcAddress( hLib, "ReleaseScanner" ) ) )
 		{
			log.Trace( 50, "PluginLoadStrategy: не удалось получить адрес функции ReleaseScanner из библиотеки %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
 		} 
		if( NULL == ( pScanner = fnGetScanner() ) )
		{
			log.Trace( 50, "PluginLoadStrategy: Ошибка при выполнении GetScanner из библиотеки %s", FindData.cFileName );			
			::FreeLibrary( hLib );
			continue;
		}
		//Заполняем массив m_mapLibraries для дальнейшей корректной выгрузки dll и удаления
		//соответствующего объекта CScanner
		m_mapLibraries[ hLib ] = fnReleaseScanner;

		vecStorage.push_back( pScanner );
		log.Trace( 90, "PluginLoadStrategy: Загружаем библиотеку %s с плагином %s", FindData.cFileName, pScanner->GetProtocolName() );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
	log.Trace( 90, "PluginLoadStrategy: всего загружено %d плагинов", iScannersCount );
}

PluginLoadStrategy::~PluginLoadStrategy()
{
	for( LibrariesMapType::iterator It = m_mapLibraries.begin(); It != m_mapLibraries.end(); It++ )
	{
		It->second();
		::FreeLibrary( It->first );
	}
}
