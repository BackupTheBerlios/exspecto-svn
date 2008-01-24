//------------------------------------------------------------------------------------//
//���� ���� �������� ������ ������� Exspecto 2006�.
//Module: Plugin load strategy
//Author: Parshin Dmitry
//Description: �����, ����������� ��������� �������� �������� � ����� Container
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
	Log::instance().Trace( 90, "PluginLoadStrategy: ��������� �������, ���� ��� ������: %s", strPluginPath.c_str() );
	//������� ��� dll � ����� � plugin-���
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
			Log::instance().Trace( 50, "PluginContainer: %s �� �������� �����������", It->c_str() );
			continue;
		}
		if( NULL == ( pScanFunc = ( ScanFunc )pLib->GetSymbol( "Scan" ) ) )
		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� Scan �� ���������� %s", It->c_str() );
			continue;
		}
 		if( NULL == ( pGetProtoName = ( GetProtoNameFunc )pLib->GetSymbol( "GetProtocolName" ) ) )
 		{
			Log::instance().Trace( 50, "PluginContainer: �� ������� �������� ����� ������� GetProtocolName �� ���������� %s", It->c_str() );
			continue;
 		}
		//��������� ������ m_mapLibraries ��� ���������� ���������� �������� dll
		m_vecLibraries.push_back( pLib );

		m_mapScanners[ pGetProtoName() ] = pScanFunc;
		Log::instance().Trace( 90, "PluginContainer: ��������� ���������� %s � �������� %s", It->c_str(), pGetProtoName() );
		iScannersCount++;
	}
	if( 0 == iScannersCount )
		throw PluginLoadErr( "PluginContainer:�� ������� �� ������ �������" );
	Log::instance().Trace( 90, "PluginContainer: ����� ��������� ��������: %d", iScannersCount );
}

PluginContainer::~PluginContainer()
{
	Log::instance().Trace( 90, "PluginContainer: �����������" );
}
