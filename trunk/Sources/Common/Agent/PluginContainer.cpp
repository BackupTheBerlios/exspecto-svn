//------------------------------------------------------------------------------------//
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description:
//-------------------------------------------------------------------------------------//
#include "precomp.h"
#include "PluginContainer.h"
#include "FileSearch.h"

PluginContainer::PluginContainer()
{
  int iScannersCount = 0;
  std::string strPluginPath = "./";
  strPluginPath += PLUGIN_PATH;
  ScanFunc pScanFunc;
  GetProtoNameFunc pGetProtoName;
  strPluginPath += "/*.so";
  Log::instance().Trace( 90, "%s:Plugin find mask %s", __FUNCTION__, strPluginPath.c_str() );
  FileSearch fs;
  std::vector<std::string> vecPluginFiles = fs.GetFiles( strPluginPath );
  for( std::vector<std::string>::iterator It = vecPluginFiles.begin(); It != vecPluginFiles.end(); It++ )
	{
	  SmartPtr<CSharedLib> pLib = SmartPtr<CSharedLib>( new CSharedLib() );
	  if( !pLib->Load( *It ) )
		{
		  Log::instance().Trace( 50, "%s: Cannot load library %s, is this a shared library?", __FUNCTION__, It->c_str() );
		  continue;
		}
	  if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
		  Log::instance().Trace( 50, "%s: Cannot get function Scan from %s, is this exspecto plugin?", __FUNCTION__, It->c_str() );
		  continue;
		}
	  if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
		  Log::instance().Trace( 50, "%s: Cannot get function GetProtocolName from %s, is this exspecto plugin?", __FUNCTION__, It->c_str() );
		  continue;
 		}
	  m_vecLibraries.push_back( pLib );

	  m_mapScanners[ pGetProtoName() ] = pScanFunc;
	  Log::instance().Trace( 90, "%s: Plugin loaded from %s, protocol = %s", __FUNCTION__, It->c_str(), pGetProtoName() );
	  iScannersCount++;
	}
  if( 0 == iScannersCount )
	throw PluginLoadErr( "PluginContainer: no plugins found" );
  Log::instance().Trace( 90, "%s: Found %d plugins", __FUNCTION__, iScannersCount );
}

PluginContainer::~PluginContainer()
{
  Log::instance().Trace( 90, "%s", __FUNCTION__ );
}
