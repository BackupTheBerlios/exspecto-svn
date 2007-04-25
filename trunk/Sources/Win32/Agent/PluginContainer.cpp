//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "PluginContainer.h"

PluginContainer::PluginContainer()
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HINSTANCE hLib;
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	ScanFunc pScanFunc;
	GetProtoNameFunc pGetProtoName;
	strPluginPath += "\\*.dll";
	Log::instance().Trace( 90, "PluginLoadStrategy: Загружаем плагины, путь для поиска: %s", strPluginPath.c_str() ); 
	//Находим все dll в папке с plugin-ами
	if( INVALID_HANDLE_VALUE == ( hFindFile = ::FindFirstFile( strPluginPath.c_str(), &FindData ) ) )
		throw PluginLoadErr( "PluginContainer:Не найдено ни одного плагина" );
	do
	{ 
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += FindData.cFileName;
		if( NULL == ( hLib = ::LoadLibraryA( strPluginFileName.c_str() ) ) )
		{
			Log::instance().Trace( 50, "%d", GetLastError() );
			Log::instance().Trace( 50, "PluginContainer: %s не является библиотекой", FindData.cFileName );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )::GetProcAddress( hLib, "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: не удалось получить адрес функции Scan из библиотеки %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )::GetProcAddress( hLib, "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: не удалось получить адрес функции GetProtocolName из библиотеки %s", FindData.cFileName ); 
			::FreeLibrary( hLib );
			continue;
 		} 
		//Заполняем массив m_mapLibraries для дальнейшей корректной выгрузки dll
		m_vecLibraries.push_back( hLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: Загружаем библиотеку %s с плагином %s", FindData.cFileName, pGetProtoName() );
		iScannersCount++;
	}while( ::FindNextFile( hFindFile, &FindData ) );
	FindClose( hFindFile );
	if( 0 == iScannersCount )
		throw PluginLoadErr( "Не найдено ни одного плагина" );
	Log::instance().Trace( 90, "PluginContainer: всего загружено плагинов: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: Уничтожение" );
	for( std::vector< HMODULE >::iterator It = m_vecLibraries.begin(); It != m_vecLibraries.end(); It++ )
		::FreeLibrary( *It );
}
