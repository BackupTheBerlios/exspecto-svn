//------------------------------------------------------------------------------------//
//Этот файл является частью проекта Exspecto 2006г.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: Класс, описывающий стратегию загрузки плагинов в класс Container
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "PluginContainer.h"
#include "FileSearch.h"

PluginContainer::PluginContainer()
{
	int iScannersCount = 0;
	std::string strPluginPath = PLUGIN_PATH;
	std::string strPluginFileName;
	ScanFunc pScanFunc;
	GetProtoNameFunc pGetProtoName;
	strPluginPath += "\\*.dll";
	Log::instance().Trace( 90, "PluginLoadStrategy: Загружаем плагины, путь для поиска: %s", strPluginPath.c_str() );
	//Находим все dll в папке с plugin-ами
	FileSearch fs;
	std::vector<std::string> vecPluginFiles = fs.GetFiles( strPluginPath );
	for( std::vector<std::string>::iterator It = vecPluginFiles.begin(); It != vecPluginFiles.end(); It++ )
	{
		strPluginFileName = PLUGIN_PATH;
		strPluginFileName += "\\";
		strPluginFileName += *It;
		SmartPtr<CSharedLib> pLib = SmartPtr<CSharedLib>( new CSharedLib() );
		if( !pLib->Load( strPluginFileName ) )
		{
			Log::instance().Trace( 50, "PluginContainer: %s не является библиотекой", It->c_str() );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: не удалось получить адрес функции Scan из библиотеки %s", It->c_str() );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: не удалось получить адрес функции GetProtocolName из библиотеки %s", It->c_str() );
			continue;
 		}
		//Заполняем массив m_mapLibraries для дальнейшей корректной выгрузки dll
		m_vecLibraries.push_back( pLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: Загружаем библиотеку %s с плагином %s", It->c_str(), pGetProtoName() );
		iScannersCount++;
	}
	if( 0 == iScannersCount )
		throw PluginLoadErr( "PluginContainer:Не найдено ни одного плагина" );
	Log::instance().Trace( 90, "PluginContainer: всего загружено плагинов: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: Уничтожение" );
}
