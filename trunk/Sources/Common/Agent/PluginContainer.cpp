//------------------------------------------------------------------------------------//
//œô¨Íœô¨âœô¨Þœô¨â œô¨äœô¨Ðœô¨Ùœô¨Û œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨çœô¨Ðœô¨áœô¨âœô¨ìœô¨î œô¨ßœô¨àœô¨Þœô¨Õœô¨Úœô¨âœô¨Ð Exspecto 2006œô¨Ó.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: œô¨ºœô¨Ûœô¨Ðœô¨áœô¨á, œô¨Þœô¨ßœô¨Øœô¨áœô¨ëœô¨Òœô¨Ðœô¨îœô¨éœô¨Øœô¨Ù œô¨áœô¨âœô¨àœô¨Ðœô¨âœô¨Õœô¨Óœô¨Øœô¨î œô¨×œô¨Ðœô¨Óœô¨àœô¨ãœô¨×œô¨Úœô¨Ø œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ò œô¨Ò œô¨Úœô¨Ûœô¨Ðœô¨áœô¨á Container
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
	Log::instance().Trace( 90, "PluginLoadStrategy: œô¨·œô¨Ðœô¨Óœô¨àœô¨ãœô¨Öœô¨Ðœô¨Õœô¨Ü œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨ë, œô¨ßœô¨ãœô¨âœô¨ì œô¨Ôœô¨Ûœô¨ï œô¨ßœô¨Þœô¨Øœô¨áœô¨Úœô¨Ð: %s", strPluginPath.c_str() );
	//œô¨½œô¨Ðœô¨åœô¨Þœô¨Ôœô¨Øœô¨Ü œô¨Òœô¨áœô¨Õ dll œô¨Ò œô¨ßœô¨Ðœô¨ßœô¨Úœô¨Õ œô¨á plugin-œô¨Ðœô¨Üœô¨Ø
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
			Log::instance().Trace( 50, "PluginContainer: %s œô¨Ýœô¨Õ œô¨ïœô¨Òœô¨Ûœô¨ïœô¨Õœô¨âœô¨áœô¨ï œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Þœô¨Ù", It->c_str() );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: œô¨Ýœô¨Õ œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨âœô¨ì œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø Scan œô¨Øœô¨× œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Ø %s", It->c_str() );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: œô¨Ýœô¨Õ œô¨ãœô¨Ôœô¨Ðœô¨Ûœô¨Þœô¨áœô¨ì œô¨ßœô¨Þœô¨Ûœô¨ãœô¨çœô¨Øœô¨âœô¨ì œô¨Ðœô¨Ôœô¨àœô¨Õœô¨á œô¨äœô¨ãœô¨Ýœô¨Úœô¨æœô¨Øœô¨Ø GetProtocolName œô¨Øœô¨× œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨Ø %s", It->c_str() );
			continue;
 		}
		//œô¨·œô¨Ðœô¨ßœô¨Þœô¨Ûœô¨Ýœô¨ïœô¨Õœô¨Ü œô¨Üœô¨Ðœô¨áœô¨áœô¨Øœô¨Ò m_mapLibraries œô¨Ôœô¨Ûœô¨ï œô¨Ôœô¨Ðœô¨Ûœô¨ìœô¨Ýœô¨Õœô¨Ùœô¨èœô¨Õœô¨Ù œô¨Úœô¨Þœô¨àœô¨àœô¨Õœô¨Úœô¨âœô¨Ýœô¨Þœô¨Ù œô¨Òœô¨ëœô¨Óœô¨àœô¨ãœô¨×œô¨Úœô¨Ø dll
		m_vecLibraries.push_back( pLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: œô¨·œô¨Ðœô¨Óœô¨àœô¨ãœô¨Öœô¨Ðœô¨Õœô¨Ü œô¨Ñœô¨Øœô¨Ñœô¨Ûœô¨Øœô¨Þœô¨âœô¨Õœô¨Úœô¨ã %s œô¨á œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ü %s", It->c_str(), pGetProtoName() );
		iScannersCount++;
	}
	if( 0 == iScannersCount )
		throw PluginLoadErr( "PluginContainer:œô¨½œô¨Õ œô¨Ýœô¨Ðœô¨Ùœô¨Ôœô¨Õœô¨Ýœô¨Þ œô¨Ýœô¨Ø œô¨Þœô¨Ôœô¨Ýœô¨Þœô¨Óœô¨Þ œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Ð" );
	Log::instance().Trace( 90, "PluginContainer: œô¨Òœô¨áœô¨Õœô¨Óœô¨Þ œô¨×œô¨Ðœô¨Óœô¨àœô¨ãœô¨Öœô¨Õœô¨Ýœô¨Þ œô¨ßœô¨Ûœô¨Ðœô¨Óœô¨Øœô¨Ýœô¨Þœô¨Ò: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: œô¨Ãœô¨Ýœô¨Øœô¨çœô¨âœô¨Þœô¨Öœô¨Õœô¨Ýœô¨Øœô¨Õ" );
}
